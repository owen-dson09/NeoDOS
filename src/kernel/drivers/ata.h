#ifndef ATA_H
#define ATA_H

#include "util.h"

int ata_read_sector(uint8_t drive, uint32_t lba, void *buffer);

#endif