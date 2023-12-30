
#ifndef __CRC_H__
#define __CRC_H__

#include <stdbool.h>
#include <stdint.h>
//------------------------------------------------------------------------------------------------//

unsigned int crc32(const void *buf, int len, unsigned int init);

uint8_t bLRC(const void *data, uint32_t len);

#endif // # __CRC_H__ #//

//------------------------------------------------------------------------------------------------//
