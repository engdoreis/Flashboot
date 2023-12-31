// Copyright Douglas Santos Reis.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#ifndef TEST_SRC_CONFIG_H_
#define TEST_SRC_CONFIG_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PAGE_SIZE 2
#define SECTOR_SIZE 512

void Bin2Hex(uint8_t *abHex, const uint8_t *abBin, uint32_t uiHexLen);
void Hex2Bin(uint8_t *abBin, const uint8_t *abHex, uint32_t uiHexLen);

uint32_t FlashWrite(void *arg, uint64_t address, uint8_t *data);
uint32_t FlashRead(void *arg, uint64_t address, uint8_t *data, uint32_t size);
uint32_t FlashErase(void *arg, uint64_t address);
uint32_t FlashProtect(void *arg, bool lock);
uint32_t CRCCalc(void *arg, uint32_t initValue, void *data, uint32_t size);
uint32_t GetTick(void *);
void SetError(void *arg, int error);

uint32_t Boot(void *arg, uint64_t address);
void CheckData(void *arg);
uint32_t SendData(void *arg, uint8_t *data, uint32_t size);
void test_init(FILE *input, FILE *output, bool *running,
               uint64_t progMemAddress, uint32_t progMemSize,
               uint64_t dataMemAddress, uint32_t dataMemSize,
               uint64_t bakMemAddress, uint32_t bakMemSize);

void test_finish();

#endif /* TEST_SRC_CONFIG_H_ */
