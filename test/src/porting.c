// Copyright Douglas Santos Reis.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#include "config.h"
#include "crc.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>
#include <unistd.h>

#include <Flashboot.h>
#include <Flashboot_Types.h>

struct memory {
  uint64_t addr;
  uint32_t size;
  uint8_t *buf;
};
struct test {
  FILE *input;
  FILE *output;
  bool *running;
  uint32_t idleCount;
  struct memory programMem;
  struct memory bakMem;
  struct memory dataMem;
} test;

static void *GetMemAddr(uint64_t address);
static void dumpMemory(FILE *f, struct memory *mem);

void test_init(FILE *input, FILE *output, bool *running,
               uint64_t progMemAddress, uint32_t progMemSize,
               uint64_t dataMemAddress, uint32_t dataMemSize,
               uint64_t bakMemAddress, uint32_t bakMemSize) {
  test.input = input;
  test.output = output;
  test.running = running;

  test.programMem.addr = progMemAddress;
  test.programMem.size = progMemSize;
  test.programMem.buf = malloc(progMemSize);
  memset(test.programMem.buf, 0, test.programMem.size);

  test.bakMem.addr = bakMemAddress;
  test.bakMem.size = bakMemSize;
  test.bakMem.buf = malloc(bakMemSize);
  memset(test.bakMem.buf, 0, test.bakMem.size);

  test.dataMem.addr = dataMemAddress;
  test.dataMem.size = dataMemSize;
  test.dataMem.buf = malloc(dataMemSize);
  memset(test.dataMem.buf, 0, test.dataMem.size);

  test.idleCount = 0;
}

void test_finish() {
  fputs("Program memory\n", test.output);
  dumpMemory(test.output, &test.programMem);

  fputs("bakup memory\n", test.output);
  dumpMemory(test.output, &test.bakMem);

  fputs("Data memory\n", test.output);
  dumpMemory(test.output, &test.dataMem);
}
/*!
 * This function check if there's data in the protocol to be processed
 * @param arg Void pointer to the argument passed during the initialization.
 */
void CheckData(void *arg) {
  //	BootApp_Context *obj = (BootApp_Context *)arg;
  if (test.idleCount++ > 2)
    *test.running = false;
}

/*!
 * Called by Boot library to send data to the host
 * @param arg   Void pointer to the argument passed during the initialization.
 * @param data  Data to be send
 * @param size  Data size
 * @return
 */
uint32_t SendData(void *arg, uint8_t *data, uint32_t size) {
  //	BootApp_Context *obj = (BootApp_Context *)arg;

  return size;
}

/*!
 * Called by boot library to write a page to memory.
 * @param arg       Void pointer to the argument passed during the
 * initialization.
 * @param address   Memory address.
 * @param data      data to be written.
 * @return
 */
uint32_t FlashWrite(void *arg, uint64_t address, uint8_t *data) {
  // BootApp_Context *obj = (BootApp_Context *)arg;

  test.idleCount = 0;
  uint8_t *memory = GetMemAddr(address);
  if (memory) {
    memcpy(memory, data, PAGE_SIZE);
    return PAGE_SIZE;
  }

  return 0;
}

/*!
 * Called by boot library to read stream of data from memory.
 * @param arg       Void pointer to the argument passed during the
 * initialization.
 * @param address   Memory address.
 * @param data      data to be written.
 * @param size      data size.
 * @return
 */
uint32_t FlashRead(void *arg, uint64_t address, uint8_t *data, uint32_t size) {
  uint8_t *memory = GetMemAddr(address);
  if (memory) {
    memcpy(data, (uint8_t *)memory, size);
  }
  test.idleCount = 0;

  return size;
}

/*!
 * Called by boot library to erase a block in memory.
 * @param arg       Void pointer to the argument passed during the
 * initialization.
 * @param address   Memory address.
 * @return
 */
uint32_t FlashErase(void *arg, uint64_t address) {
  //	BootApp_Context *obj = (BootApp_Context *)arg;

  uint8_t *memory = GetMemAddr(address);
  if (memory) {
    memset(memory, 0xFF, SECTOR_SIZE);
  }

  test.idleCount = 0;
  return SECTOR_SIZE;
}

/*!
 * Called by boot library to enable/disable the memory protection.
 * @param arg       Void pointer to the argument passed during the
 * initialization.
 * @param lock      true to enable and false to disable the protection.
 * @return
 */
uint32_t FlashProtect(void *arg, bool lock) {
  //	BootApp_Context *obj = (BootApp_Context *)arg;
  if (lock) {
  } else {
  }
  test.idleCount = 0;
  fprintf(test.output, "FlashProtect=%u\n", lock);
  return 0;
}

/*!
 * Called by bootloader library to calculate a CRC32.
 * @param arg       Void pointer to the argument passed during the
 * initialization..
 * @param data      Data input for the calculation.
 * @param buf_size  Data size.
 * @return
 */
uint32_t CRCCalc(void *arg, uint32_t initValue, void *data, uint32_t size) {
  //	BootApp_Context *obj = (BootApp_Context *)arg;
  test.idleCount = 0;
  //   uint8_t *memory = GetMemAddr((uint64_t)data);
  uint32_t crc = crc32(data, size, initValue);

  fprintf(test.output, "CRC=0x%X\n", crc);
  return crc;
}

uint32_t GetTick(void *arg) {
  struct timespec ts;

  clock_gettime(CLOCK_MONOTONIC, &ts);
  return ts.tv_sec * 1000 + ts.tv_nsec / (1000 * 1000);
}

void SetError(void *arg, Flashboot_eResult error) {
  char *errorTable[] = {
      "Flashboot_eGeneralError           ",
      "Flashboot_eCRCError               ",
      "Flashboot_eNullPointerError       ",
      "Flashboot_eBufferOverflowError    ",
      "Flashboot_eBadInputError          ",
      "Flashboot_eAlignmentError         ",
      "Flashboot_eProtocolError          ",
      "Flashboot_eAddressOutOfBoundsError",
      "Flashboot_eTimeoutError           ",
      "Flashboot_eOverflowError          ",
      "Flashboot_eOK                     ",
  };

  if (error != 0) {
    fprintf(test.output, "Error=%s\n",
            errorTable[abs(Flashboot_eGeneralError) + error]);
  }
}
/*!
 * Called by bootloader library to perform the boot process.
 * @param arg       Void pointer to the argument passed during the
 * initialization.
 * @param address   Application start address.
 * @return
 */
uint32_t Boot(void *arg, uint64_t address) {

  fprintf(test.output, "Boot=%p\n", (void *)address);

  *test.running = false;
  return 0;
}

static void *GetMemAddr(uint64_t address) {
  if (address >= test.programMem.addr &&
      address < (test.programMem.addr + test.programMem.size)) {
    return &test.programMem.buf[address - test.programMem.addr];
  }

  if (address >= test.dataMem.addr &&
      address < (test.dataMem.addr + test.dataMem.size)) {
    return &test.dataMem.buf[address - test.dataMem.addr];
  }

  if (address >= test.bakMem.addr &&
      address < (test.bakMem.addr + test.bakMem.size)) {
    return &test.bakMem.buf[address - test.bakMem.addr];
  }

  return NULL;
}

static void dumpMemory(FILE *f, struct memory *mem) {
  enum { kHexSize = 64 };
  uint64_t currentAddress = mem->addr;
  uint8_t *pData = mem->buf;
  uint8_t hexData[kHexSize + 1] = {0};
  while (pData < (mem->buf + mem->size)) {
    Bin2Hex(hexData, pData, kHexSize);
    fprintf(test.output, "%lx %s\n", currentAddress, hexData);
    pData += kHexSize / 2;
    currentAddress += kHexSize / 2;
  }
}

void Bin2Hex(uint8_t *abHex, const uint8_t *abBin, uint32_t uiHexLen) {
  for (int i = uiHexLen - 1; i >= 0; i--) {
    if ((i & 0x01) == 0x00) {                // Check if nibble is even
      abHex[i] = (abBin[i / 2] & 0xF0) >> 4; // Asc gets odd nibble
    } else {
      abHex[i] = abBin[i / 2] & 0x0F; // Asc gets nibble even
    }
    abHex[i] += abHex[i] > 0x09
                    ? 0x37
                    : 0x30; // If character adds 0x37, otherwise adds 0x30
  }
}

void Hex2Bin(uint8_t *abBin, const uint8_t *abHex, uint32_t uiHexLen) {
  for (int i = 0; i < uiHexLen; i++) {
    if ((i & 0x01) == 0x00) { // Check if nibble is even
      abBin[i / 2] &= 0x0F;   // Clear odd nibble bits
      abBin[i / 2] += (abHex[i] > '9' ? abHex[i] - 0x37 : abHex[i] - 0x30) << 4;
    } else {
      abBin[i / 2] &= 0xF0; // Clear even nibble bits
      abBin[i / 2] += (abHex[i] > '9' ? abHex[i] - 0x37 : abHex[i] - 0x30);
    }
  }
}
