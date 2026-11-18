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

#define ATA_READ_SECTORS 0x20

#define ATA_TIMEOUT 1000000

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

int ata_read_sector(uint8_t drive, uint32_t lba, void *buffer) {
    if (drive > 1 || lba > 0x0FFFFFFF || buffer == 0) return -1;
    if (ata_wait_not_busy() != 0) return -1;

    outb(ATA_DRIVE_SELECT, 0xE0 | (drive << 4) | ((lba >> 24) & 0x0F));

    ata_delay();
    if (ata_wait_not_busy() != 0) return -1;

    outb(ATA_SECTOR_COUNT, 1);
    outb(ATA_LBA_LOW,  (uint8_t)lba);
    outb(ATA_LBA_MID,  (uint8_t)(lba >> 8));
    outb(ATA_LBA_HIGH, (uint8_t)(lba >> 16));
    outb(ATA_COMMAND, ATA_READ_SECTORS);

    ata_delay();
    if (ata_wait_drq() != 0) return -1;

    // Transfer one sector (512 bytes) from data port to buffer
    uint16_t *data = (uint16_t *)buffer;
    for (int i = 0; i < 256; i++) {
        data[i] = inw(ATA_DATA);
    }

    ata_delay();
    return 0;
}