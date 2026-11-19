#ifndef ATA_H
#define ATA_H

#include "util.h"

typedef struct {
    uint8_t present;
    uint8_t drive;
    uint8_t lba28;
    uint8_t lba48;

    uint32_t sector_count;
    char model[41];
} ata_device_t;

void ata_init(void);

const ata_device_t *ata_get_device(uint8_t drive);
int ata_read_sector(uint8_t drive, uint32_t lba, void *buffer);
int ata_write_sector(uint8_t drive, uint32_t lba, const void *buffer);
int ata_identify(uint8_t drive, void *buffer);
uint32_t ata_get_sector_count(const uint16_t *data);
void ata_get_model(const uint16_t *data, char *model);

#endif