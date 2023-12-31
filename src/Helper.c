// Copyright Douglas Santos Reis.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#include "Helper.h"
#include "Flashboot_Types.h"
#include "config.h"
#include <string.h>

Flashboot_eResult Flashboot_MemoryWrite(Flashboot_Context *this,
                                        uint64_t address, uint8_t *data,
                                        uint32_t size) {
  /*Check if address if aligned*/
  if (address % this->update.memory->pageSize ||
      size % this->update.memory->pageSize) {
    return Flashboot_eAlignmentError;
  }

  while (size >= this->update.memory->pageSize) {
    /*write page*/
    this->interface->WriteMemPage(this->arg, address, data);

    /*Calculate next page and the remaining size*/
    size -= this->update.memory->pageSize;
    address += this->update.memory->pageSize;
    data += this->update.memory->pageSize;
  }
  return Flashboot_eOK;
}

/*!
 * Check if the sector is erased already
 * @param this        Struct initialized by ::Flashboot_Initialize function.
 * @param address    Start address to be checked
 *
 * @return
 */
static bool IsSectorErased(Flashboot_Context *this, uint64_t address) {
  /*Define the last sector address based on the memory information*/

  this->interface->ReadMem(this->arg, address, this->workBuffer,
                           this->update.memory->sectorSize);
  uint32_t *word = (uint32_t *)this->workBuffer;
  /*If is not erased, then return false*/
  for (int i = 0; i < this->update.memory->sectorSize; i += sizeof(*word)) {
    if (*word != 0xFFFFFFFF) {
      return false;
    }
    word++;
  }

  return true;
}

Flashboot_eResult Flashboot_MemoryErase(Flashboot_Context *this,
                                        uint64_t address, uint32_t size) {
  /* Check if the address is the in beginning of a sector.
   * If not, the useful content of the sector shall be back up
   */
  if (address % this->update.memory->sectorSize) {
    /*+-----------------------------------------------------+
          |xxxxxxxxxxxxfffffffffffffffffffffffffffxxxxxxxxxxxxxx|
          +-----------------------------------------------------+
          ^ <-Offset-->^<-------erase size----->^<--tail size-->|
          |            |                        |               ^
          |            |                        |               |
          |            |                        +--tail         +-- next sector
          |			 +--address
          +--previous sector
           */
    uint32_t previousSector =
        address - (address % this->update.memory->sectorSize);
    uint32_t nextSector = previousSector + this->update.memory->sectorSize;
    uint64_t offset = address - previousSector;
    int32_t tailSize = this->update.memory->sectorSize - offset - size;
    tailSize = tailSize < 0 ? 0 : tailSize;
    uint32_t toEraseOnSector =
        (size > (nextSector - address)) ? (nextSector - address) : size;

    /*Back-up useful content from the Sector*/
    this->interface->ReadMem(this->arg, previousSector, this->workBuffer,
                             this->update.memory->sectorSize);

    /*Erase sector*/
    this->interface->EraseMemSector(this->arg, previousSector);

    /*Restore the beginning of the Sector*/
    Flashboot_MemoryWrite(this, previousSector, this->workBuffer, offset);

    /*Restore the end of the Sector if there's something to restore*/
    if (tailSize > 0)
      Flashboot_MemoryWrite(this, nextSector - tailSize,
                            this->workBuffer + offset + toEraseOnSector,
                            tailSize);

    /*calc remaining size*/
    size -= toEraseOnSector;

    // calc address of remaining memory to erase
    address += toEraseOnSector;
  }

  /*Erase the complete sectors*/
  while (size >= this->update.memory->sectorSize) {
    if (!IsSectorErased(this, address)) {
      this->interface->EraseMemSector(this->arg, address);
    }
    /*next address*/
    address += this->update.memory->sectorSize;
    size -= this->update.memory->sectorSize;
  };

  // Check if there's remaining space to erase that doesn't fit in a sector.
  if (size > 0) {
    /*Back-up useful content in the end of the Sector*/
    this->interface->ReadMem(this->arg, address + size, this->workBuffer,
                             this->update.memory->sectorSize - size);

    /*Erase the whole sector*/
    this->interface->EraseMemSector(this->arg, address);

    /*Restore useful content to the end of the Sector*/
    Flashboot_MemoryWrite(this, address + size, this->workBuffer,
                          this->update.memory->sectorSize - size);
  }

  return Flashboot_eOK;
}

Flashboot_eResult Flashboot_SetTimeout(Flashboot_Context *this,
                                       uint32_t timeout) {
  this->timeout = this->interface->GetTick(this->arg) + timeout;
  return Flashboot_eOK;
}

Flashboot_eResult Flashboot_CheckTimeout(Flashboot_Context *this) {
  if (this->timeout < this->interface->GetTick(this->arg)) {
    return Flashboot_eTimeoutError;
  }
  return Flashboot_eOK;
}

Flashboot_eResult Flashboot_SearchForMBR(Flashboot_Context *this,
                                         Flashboot_Memory *mem,
                                         Flashboot_MBR *mbr) {
  /*Look for a valid application MBR*/
  uint64_t address = mem->startAddress;
  while (address < (mem->startAddress + mem->size - MBR_ALINGMENT)) {
    this->interface->ReadMem(this->arg, address, (uint8_t *)mbr,
                             sizeof(Flashboot_MBR));

    if (mbr->magicKey == MBR_MAGIC_KEY && // Is MBR found?
        address == mbr->mbrAddress &&
        ((mbr->mbrAddress < mbr->loadAddress) ||
         (mbr->mbrAddress >= (mbr->loadAddress + mbr->size))) &&
        mbr->loadAddress >= mem->startAddress && // Is start address valid?
        (mbr->loadAddress + mbr->size) <
            (mem->startAddress + mem->size)) // Is end address valid?
    {
      return Flashboot_eOK;
    }
    address += MBR_ALINGMENT;
  }
  return Flashboot_eGeneralError;
}

Flashboot_eResult Flashboot_CopyToMemory(Flashboot_Context *this,
                                         Flashboot_Memory *target,
                                         Flashboot_MBR *mbr) {
  uint64_t address = this->mbr.loadAddress, size = 0;

  Flashboot_FlashStart(this, target, mbr->bootAddress, mbr->size);

  /*Copy to program memory*/
  while (this->result == Flashboot_eOK &&
         address < (this->mbr.loadAddress + this->mbr.size)) {
    size = this->interface->ReadMem(this->arg, address,
                                    (uint8_t *)this->workBuffer, this->wbs);
    Flashboot_FlashWrite(this, this->workBuffer, size);
    address += size;
  }

  if (this->result == Flashboot_eOK) {
    Flashboot_FlashFinish(this, this->mbr.crc, this->mbr.version);
  }

  return this->result;
}

Flashboot_eResult Flashboot_FlashStart(Flashboot_Context *this,
                                       Flashboot_Memory *target,
                                       const uint64_t bootAddress,
                                       uint32_t size) {
  this->result = Flashboot_eAddressOutOfBoundsError;

  /*Check if address if aligned*/
  Flashboot_ASSERT((bootAddress % this->progMem->pageSize) ||
                       (size % this->progMem->pageSize),
                   Flashboot_eAlignmentError);

  /*Check if address and size are valid and erase the memory if positive*/
  if ((bootAddress >= this->progMem->startAddress) &&
      (size <= this->progMem->size)) {
    this->update.memory = target;
    this->update.size = size;
    this->update.bootAddress = bootAddress;
    this->update.startAddress = target->startAddress;

    if (this->progMem->startAddress == target->startAddress) {
      this->update.startAddress = bootAddress;
    }

    this->update.currentAddress = this->update.startAddress;

    /*Disable flash projection*/
    this->interface->ProtectMem(this->arg, false);

    /*Erase the whole application flash area*/
    this->result = Flashboot_MemoryErase(
        this, this->update.memory->startAddress, this->update.memory->size);

    if (this->result != Flashboot_eOK) {
      this->interface->ProtectMem(this->arg, true);
      this->update.size = this->update.startAddress =
          this->update.currentAddress = 0;
    }
  }

  return this->result;
}

Flashboot_eResult Flashboot_FlashWrite(Flashboot_Context *this,
                                       const uint8_t *data,
                                       uint32_t size) { // TODO: init result
  if (this->update.currentAddress >= this->update.memory->startAddress &&
      this->update.currentAddress + size <
          this->update.memory->startAddress + this->update.memory->size) {
    /*Write packet in the memory*/
    this->result = Flashboot_MemoryWrite(this, this->update.currentAddress,
                                         (uint8_t *)data, size);

    /*If ok, calculate the next address*/
    if (this->result == Flashboot_eOK) {
      this->update.currentAddress += size;
    }
  }

  return this->result;
}

Flashboot_eResult Flashboot_FlashFinish(Flashboot_Context *this,
                                        const uint32_t crc, uint32_t version) {
  /*Calculate the CRC of the received binary*/
  uint32_t _crc =
      Flashboot_CalcCRC(this, this->update.startAddress, this->update.size);

  /*If CRC mismatch erase the memory and return error to the host */
  if (_crc != crc) {
    this->result = Flashboot_eCRCError;
    Flashboot_MemoryErase(this, this->update.startAddress, this->update.size);
  } else {
    /*Generate MBR*/
    Flashboot_MBR mbr = {
        .magicKey = MBR_MAGIC_KEY,
        .version = version,
        .loadAddress = this->update.startAddress,
        .bootAddress = this->update.bootAddress,
        .size = this->update.size,
        .crc = crc,
    };

    uint64_t endAddress = this->update.startAddress + this->update.size;
    mbr.mbrAddress =
        ((endAddress % MBR_ALINGMENT) == 0)
            ? endAddress
            : endAddress + MBR_ALINGMENT - (endAddress % MBR_ALINGMENT);
    this->result = Flashboot_MemoryWrite(this, mbr.mbrAddress, (uint8_t *)&mbr,
                                         sizeof(mbr));
  }

  /*Enable memory protection*/
  this->interface->ProtectMem(this->arg, true);
  this->update.size = this->update.startAddress = this->update.currentAddress =
      0;

  return this->result;
}

uint32_t Flashboot_CalcCRC(Flashboot_Context *this, uint64_t address,
                           uint32_t size) {
  uint32_t crc = 0;

  do {
    uint32_t min = size > this->update.memory->sectorSize
                       ? this->update.memory->sectorSize
                       : size;

    this->interface->ReadMem(this->arg, address, this->workBuffer, min);
    crc = this->interface->CalcCRC(this->arg, crc, this->workBuffer, min);
    size -= min;
    address += min;
  } while (size);

  return crc;
}
