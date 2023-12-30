/*
 * Memory.h
 *
 *  Created on: Mar 13, 2020
 *      Author: Douglas Reis
 */

#ifndef FLASHBOOT_HELPER_H_
#define FLASHBOOT_HELPER_H_

#include "Flashboot_Types.h"
#include <string.h>

/*!
 * Write a stream of data the memory taking in consideration it limitations
 * @param this        Struct initialized by ::Flashboot_Initialize function.
 * @param address    Start address to write
 * @param data       Data to be written
 * @param size       Data size
 * @return
 */
Flashboot_eResult Flashboot_MemoryWrite(Flashboot_Context *this,
                                        uint64_t address, uint8_t *data,
                                        uint32_t size);

/*!
 * Erase a space of the memory taking in consideration it limitations
 * @param this        Struct initialized by ::Flashboot_Initialize function.
 * @param address    Start address to be erased
 * @param size       Erase size
 * @return
 */
Flashboot_eResult Flashboot_MemoryErase(Flashboot_Context *this,
                                        uint64_t address, uint32_t size);

/*!
 * Set a timeout
 * @param this        Struct initialized by ::Flashboot_Initialize function.
 * @param timeout    Timeout
 * @return
 */
Flashboot_eResult Flashboot_SetTimeout(Flashboot_Context *this,
                                       uint32_t timeout);

/*!
 * Check if the timeout set by ::Flashboot_SetTimeout was reach.
 * @param this        Struct initialized by ::Flashboot_Initialize function.
 * @return
 */
Flashboot_eResult Flashboot_CheckTimeout(Flashboot_Context *this);

Flashboot_eResult Flashboot_SearchForMBR(Flashboot_Context *this,
                                         Flashboot_Memory *mem,
                                         Flashboot_MBR *mbr);

Flashboot_eResult Flashboot_CopyToMemory(Flashboot_Context *this,
                                         Flashboot_Memory *mem,
                                         Flashboot_MBR *mbr);

Flashboot_eResult Flashboot_FlashStart(Flashboot_Context *this,
                                       Flashboot_Memory *target,
                                       const uint64_t bootAddress,
                                       uint32_t size);
Flashboot_eResult Flashboot_FlashWrite(Flashboot_Context *this,
                                       const uint8_t *data, uint32_t size);
Flashboot_eResult Flashboot_FlashFinish(Flashboot_Context *this,
                                        const uint32_t crc, uint32_t version);

uint32_t Flashboot_CalcCRC(Flashboot_Context *this, uint64_t address,
                           uint32_t size);

#endif /* FLASHBOOT_HELPER_H_ */
