#ifndef CRC16_H
#define CRC16_H

#include <stdint.h>

#define CRC16_INIT_VALUE 0;

uint16_t crc16(uint8_t next_data, uint16_t crc);

#endif // CRC16_H

