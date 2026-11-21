/* Basic ATA PIO Disk Driver */

#include "ata.h"
#include "util.h"

#define ATA_DATA         0x1F0
#define ATA_SECTOR_COUNT 0x1F2
#define ATA_LBA_LOW      0x1F3
#define ATA_LBA_MID      0x1F4
#define ATA_LBA_HIGH     0x1F5
#define ATA_DRIVE_SELECT 0x1F6
#define ATA_STATUS       0x1F7
#define ATA_COMMAND      0x1F7
#define ATA_ALT_STATUS   0x3F6

#define ATA_BSY 0x80
#define ATA_DRQ 0x08
#define ATA_DF  0x20
#define ATA_ERR 0x01

#define ATA_READ_SECTORS  0x20
#define ATA_WRITE_SECTORS 0x30
#define ATA_IDENTIFY      0xEC
#define ATA_CACHE_FLUSH   0xE7

#define ATA_TIMEOUT 1000000

// Drive 0: Master, Drive 1: Slave
static ata_device_t ata_devices[2];
static uint16_t identify_buffer[256];

//// Helper functions ////
    static void ata_delay(void) {
        for (int i = 0; i < 4; i++) {
            (void)inb(ATA_ALT_STATUS);
        }
    }

    // Wait until BSY clears
    static int ata_wait_not_busy(void) {
        uint32_t timeout = ATA_TIMEOUT;
        while (timeout--) {
            uint8_t status = inb(ATA_STATUS);

            if (status == 0xFF || status == 0x00) return -1; // Drive not present
            if (!(status & ATA_BSY)) return 0;
        }
        return -1;
    }

    // Wait until DRQ sets
    static int ata_wait_drq(void) {
        uint32_t timeout = ATA_TIMEOUT;
        while (timeout--) {
            uint8_t status = inb(ATA_STATUS);

            if (status == 0xFF || status == 0x00) return -1; // Drive not present
            if (!(status & ATA_BSY)) {
                if (status & ATA_ERR) return -1; // Error occurred
                if (status & ATA_DF) return -1; // Device fault
                if (status & ATA_DRQ) return 0;
            }
        }
        return -1;
    }

    static int ata_wait_complete(void) {
        if (ata_wait_not_busy() != 0) return -1;
        uint8_t status = inb(ATA_STATUS);
        if (status & (ATA_ERR | ATA_DF)) return -1;
        return 0;
    }

    static int ata_select_drive(uint8_t drive, uint8_t lba_high) {
        if (drive > 1) return -1;
        if (ata_wait_not_busy() != 0) return -1;
        outb(ATA_DRIVE_SELECT, 0xE0 | (drive << 4) | (lba_high & 0x0F));
        ata_delay();
        return ata_wait_not_busy();
    }

    static void ata_setup_lba(uint32_t lba, uint8_t count) {
        outb(ATA_SECTOR_COUNT, count);
        outb(ATA_LBA_LOW,  (uint8_t)lba);
        outb(ATA_LBA_MID,  (uint8_t)(lba >> 8));
        outb(ATA_LBA_HIGH, (uint8_t)(lba >> 16));
    }

    static void ata_send_command(uint8_t command) {
        outb(ATA_COMMAND, command);
        ata_delay();
    }

    static void ata_read_data(void *buffer) {
        uint16_t *data = (uint16_t *)buffer;

        for (int i = 0; i < 256; i++) {
            data[i] = inw(ATA_DATA);
        }
    }

    static void ata_write_data(const void *buffer) {
        const uint16_t *data = (const uint16_t *)buffer;

        for (int i = 0; i < 256; i++) {
            outw(ATA_DATA, data[i]);
        }
    }

    static int ata_flush_cache(void) {
        ata_send_command(ATA_CACHE_FLUSH);
        return ata_wait_complete();
    }

    static int ata_validate_access(uint8_t drive, uint32_t lba, const void *buffer) {
        if (drive >= 2 || buffer == 0) return -1;

        const ata_device_t *device = ata_get_device(drive);

        if (device == 0) return -1;
        if (!device->lba28) return -1;
        if (lba > 0x0FFFFFFF) return -1;
        if (lba >= device->sector_count) return -1;
        return 0;
    }
//// END Helper functions ////

void ata_init(void) {
    for (uint8_t drive = 0; drive < 2; drive++) {
        ata_device_t *device = &ata_devices[drive];

        device->present = 0;
        device->drive = drive;
        device->lba28 = 0;
        device->lba48 = 0;
        device->sector_count = 0;
        device->model[0] = '\0';

        if (ata_identify(drive, identify_buffer) != 0) continue;

        device->present = 1;
        device->lba28 = (identify_buffer[49] & (1 << 9)) != 0; // word 49, bit 9 = LBA addressing supported
        device->lba48 = (identify_buffer[83] & (1 << 10)) != 0; // word 83, bit 10 = 48-bit addressing supported

        device->sector_count = ata_get_sector_count(identify_buffer);

        ata_get_model(identify_buffer, device->model);
    }
}

const ata_device_t *ata_get_device(uint8_t drive) {
    if (drive >= 2) return 0;
    if (!ata_devices[drive].present) return 0;
    return &ata_devices[drive];
}

int ata_read_sector(uint8_t drive, uint32_t lba, void *buffer) {
    if (ata_validate_access(drive, lba, buffer) != 0) return -1;
    if (ata_select_drive(drive, lba >> 24) != 0) return -1;

    ata_setup_lba(lba, 1);
    ata_send_command(ATA_READ_SECTORS);

    if (ata_wait_drq() != 0) return -1;

    ata_read_data(buffer);
    ata_delay();

    return 0;
}

int ata_write_sector(uint8_t drive, uint32_t lba, const void *buffer) {
    if (ata_validate_access(drive, lba, buffer) != 0) return -1;
    if (ata_select_drive(drive, lba >> 24) != 0) return -1;

    ata_setup_lba(lba, 1);
    ata_send_command(ATA_WRITE_SECTORS);

    if (ata_wait_drq() != 0) return -1;

    ata_write_data(buffer);
    ata_delay();

    if (ata_wait_complete() != 0) return -1;

    return ata_flush_cache();
}


int ata_identify(uint8_t drive, void *buffer) {
    if (drive > 1 || buffer == 0) return -1;
    if (ata_select_drive(drive, 0) != 0) return -1;

    ata_setup_lba(0, 0);
    ata_send_command(ATA_IDENTIFY);

    uint8_t status = inb(ATA_STATUS);
    if (status == 0x00 || status == 0xFF) return -1;
    if (ata_wait_not_busy() != 0) return -1;
    if (inb(ATA_LBA_MID) != 0 || inb(ATA_LBA_HIGH) != 0) return -1;
    if (ata_wait_drq() != 0) return -1;

    ata_read_data(buffer);
    ata_delay();

    return 0;
}

uint32_t ata_get_sector_count(const uint16_t *data) {
    return (uint32_t)data[60] | ((uint32_t)data[61] << 16);
}

void ata_get_model(const uint16_t *data, char *model) {
    for (int i = 0; i < 20; i++) {
        model[i * 2] = (char)(data[27 + i] >> 8);
        model[i * 2 + 1] = (char)(data[27 + i] & 0xFF);
    }

    model[40] = '\0';
}