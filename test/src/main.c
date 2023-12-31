// Copyright Douglas Santos Reis.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#include "config.h"
#include <Flashboot.h>
#include <Flashboot_Types.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  Flashboot_Context loader; /*!< Boot loader library object*/
  uint8_t workbuffer[1024]; /*!< Work buffer for general use.*/
} BootApp_Context;

BootApp_Context appContext;

static const Flashboot_Interface interface = {
    .WriteMemPage = FlashWrite,
    .ReadMem = FlashRead,
    .EraseMemSector = FlashErase,
    .ProtectMem = FlashProtect,
    .CheckData = CheckData,
    .SendData = SendData,
    .CalcCRC = CRCCalc,
    .Boot = Boot,
    .GetTick = GetTick,
    .SetError = SetError,
};

#define TIMEOUT 2000

typedef struct {
  bool running;
  uint32_t timeout;
  int status;
  FILE *inputFile;
  FILE *outputFile;
} ClassTest;

typedef int (*Function)(ClassTest *, void *);
typedef struct {
  char *name;
  Function function;
} TestCase;

int ProgramMemoryLoad(ClassTest *test, void *arg);
int DataMemoryLoad(ClassTest *test, void *arg);
int BackupMemoryLoad(ClassTest *test, void *arg);

TestCase TestMap[] = {
    {"ProgramMemoryLoad", (Function)ProgramMemoryLoad},
    {"DataMemoryLoad", (Function)DataMemoryLoad},
    {"BackupMemoryLoad", (Function)BackupMemoryLoad},
};

int ProgramMemoryLoad(ClassTest *test, void *arg) {
  BootApp_Context *obj = &appContext;

  Flashboot_Memory progMemory = {
      .startAddress = 0x08004000, // Complete memory size
      .size = 20 * 1024,          // Complete memory size
      .pageSize = PAGE_SIZE,      // Minimum writable block
      .sectorSize = SECTOR_SIZE,  // Minimum erasable block
  };

  test_init(test->inputFile, test->outputFile, &test->running,
            progMemory.startAddress, progMemory.size, 0, 0, 0, 0);

  /*Init loader library*/
  Flashboot_eResult ret =
      Flashboot_Initialize(&obj->loader, obj, &interface, &progMemory,
                           obj->workbuffer, sizeof(obj->workbuffer));

  if (ret != Flashboot_eOK) {
    return 0;
  }
  test->running = true;

  char hexData[128];
  uint8_t binData[128 + 1];
  uint32_t size, crc, version;
  uint64_t addr = 0;
  int32_t binSize;

  size_t res =
      fscanf(test->inputFile, "addr=%lx\nsize=%u\ncrc=%x\nversion=%x\n", &addr,
             (uint32_t *)&binSize, &crc, &version);
  assert(res > 0);
  ret = Flashboot_StartLoad(&obj->loader, false, addr, binSize);
  SetError(0, ret);

  while (test->running && ret == Flashboot_eOK && binSize > 0) {
    ret = Flashboot_Execute(&obj->loader);
    SetError(0, ret);
    hexData[0] = 0;
    if (fscanf(test->inputFile, "%s\r\n", hexData) &&
        (size = strlen(hexData)) > 0) {
      Hex2Bin(binData, (uint8_t *)hexData, size);
      ret = Flashboot_WriteLoad(&obj->loader, binData, size / 2);
      SetError(0, ret);
      binSize -= size / 2;
    } else
      break;
  }

  if (test->running && ret == Flashboot_eOK) {
    ret = Flashboot_FinishLoad(&obj->loader, crc, version);
    SetError(0, ret);
    if (ret == Flashboot_eOK) {
      ret = Flashboot_Boot(&obj->loader);
      SetError(0, ret);

      while (test->running && ret == Flashboot_eOK) {
        ret = Flashboot_Execute(&obj->loader);
        SetError(0, ret);
      }
    }
  }

  test_finish();
  return 0;
}

int DataMemoryLoad(ClassTest *test, void *arg) {
  BootApp_Context *obj = &appContext;

  Flashboot_Memory progMemory = {
      .startAddress = 0x08004000, // Complete memory size
      .size = 20 * 1024,          // Complete memory size
      .pageSize = PAGE_SIZE,      // Minimum writable block
      .sectorSize = SECTOR_SIZE,  // Minimum erasable block
  };

  Flashboot_Memory dataMemory = {
      .startAddress = 0x10000000, // Complete memory size
      .size = 20 * 1024,          // Complete memory size
      .pageSize = PAGE_SIZE,      // Minimum writable block
      .sectorSize = SECTOR_SIZE,  // Minimum erasable block
  };

  test_init(test->inputFile, test->outputFile, &test->running,
            progMemory.startAddress, progMemory.size, dataMemory.startAddress,
            dataMemory.size, 0, 0);

  /*Init loader library*/
  Flashboot_eResult ret =
      Flashboot_Initialize(&obj->loader, obj, &interface, &progMemory,
                           obj->workbuffer, sizeof(obj->workbuffer));

  if (ret != Flashboot_eOK) {
    return 0;
  }

  Flashboot_SetDataMemory(&obj->loader, &dataMemory);

  test->running = true;

  char hexData[128];
  uint8_t binData[128 + 1];
  uint32_t size, crc, version;
  int32_t binSize;
  uint64_t addr = 0;

  size_t res =
      fscanf(test->inputFile, "addr=%lx\nsize=%u\ncrc=%x\nversion=%x\n", &addr,
             (uint32_t *)&binSize, &crc, &version);
  assert(res == 4);
  ret = Flashboot_StartLoad(&obj->loader, true, addr, binSize);
  SetError(0, ret);

  while (test->running && ret == Flashboot_eOK && binSize > 0) {
    ret = Flashboot_Execute(&obj->loader);
    SetError(0, ret);
    hexData[0] = 0;
    if (fscanf(test->inputFile, "%s\r\n", hexData) &&
        (size = strlen(hexData)) > 0) {
      Hex2Bin(binData, (uint8_t *)hexData, size);
      ret = Flashboot_WriteLoad(&obj->loader, binData, size / 2);
      SetError(0, ret);
      binSize -= size / 2;
    } else
      break;
  }

  if (test->running && ret == Flashboot_eOK) {
    ret = Flashboot_FinishLoad(&obj->loader, crc, version);
    SetError(0, ret);
    if (ret == Flashboot_eOK) {
      Flashboot_SetDataMemory(&obj->loader, &dataMemory);
      SetError(0, ret);

      while (test->running && ret == Flashboot_eOK) {
        ret = Flashboot_Execute(&obj->loader);
        SetError(0, ret);
      }
    }
  }

  test_finish();
  return 0;
}

int BackupMemoryLoad(ClassTest *test, void *arg) {
  BootApp_Context *obj = &appContext;

  Flashboot_Memory progMemory = {
      .startAddress = 0x08004000, // Complete memory size
      .size = 20 * 1024,          // Complete memory size
      .pageSize = PAGE_SIZE,      // Minimum writable block
      .sectorSize = SECTOR_SIZE,  // Minimum erasable block
  };

  Flashboot_Memory backupMemory = {
      .startAddress = 0x10000000, // Complete memory size
      .size = 20 * 1024,          // Complete memory size
      .pageSize = PAGE_SIZE,      // Minimum writable block
      .sectorSize = SECTOR_SIZE,  // Minimum erasable block
  };

  test_init(test->inputFile, test->outputFile, &test->running,
            progMemory.startAddress, progMemory.size, 0, 0,
            backupMemory.startAddress, backupMemory.size);

  /*Init loader library*/
  Flashboot_eResult ret =
      Flashboot_Initialize(&obj->loader, obj, &interface, &progMemory,
                           obj->workbuffer, sizeof(obj->workbuffer));

  if (ret != Flashboot_eOK) {
    return 0;
  }

  test->running = true;

  char hexData[128];
  uint8_t binData[128 + 1];
  uint32_t size, crc, version;
  int32_t binSize, remaningSize;
  uint64_t addr = 0;

  size_t res =
      fscanf(test->inputFile, "addr=%lx\nsize=%u\ncrc=%x\nversion=%x\n", &addr,
             (uint32_t *)&binSize, &crc, &version);
  assert(res == 4);
  ret = Flashboot_StartLoad(&obj->loader, false, addr, binSize);
  SetError(0, ret);

  remaningSize = binSize;
  while (test->running && ret == Flashboot_eOK && remaningSize > 0) {
    ret = Flashboot_Execute(&obj->loader);
    SetError(0, ret);
    hexData[0] = 0;
    if (fscanf(test->inputFile, "%s\r\n", hexData) &&
        (size = strlen(hexData)) > 0) {
      Hex2Bin(binData, (uint8_t *)hexData, size);
      ret = Flashboot_WriteLoad(&obj->loader, binData, size / 2);
      SetError(0, ret);
      remaningSize -= size / 2;
    } else
      break;
  }

  // Create backup
  if (test->running && ret == Flashboot_eOK) {
    ret = Flashboot_FinishLoad(&obj->loader, crc, version);
    SetError(0, ret);
    if (ret == Flashboot_eOK) {
      Flashboot_Initialize(&obj->loader, obj, &interface, &progMemory,
                           obj->workbuffer, sizeof(obj->workbuffer));
      ret = Flashboot_SetBackupMemory(&obj->loader, true, &backupMemory);
      SetError(0, ret);

      while (test->running && ret == Flashboot_eOK) {
        ret = Flashboot_Execute(&obj->loader);
        SetError(0, ret);
      }
    }
  }

  // erase crc
  FlashErase(NULL, addr + binSize + 4);

  test->running = true;
  // recovery
  if (test->running && ret == Flashboot_eOK) {
    Flashboot_Initialize(&obj->loader, obj, &interface, &progMemory,
                         obj->workbuffer, sizeof(obj->workbuffer));
    ret = Flashboot_SetBackupMemory(&obj->loader, true, &backupMemory);
    SetError(0, ret);

    while (test->running && ret == Flashboot_eOK) {
      ret = Flashboot_Execute(&obj->loader);
      SetError(0, ret);
    }
  }

  test_finish();
  return 0;
}
TestCase *GetTestCase(char *name) {
  for (int i = 0; i < sizeof(TestMap) / sizeof(TestMap[0]); i++) {
    if (strcmp(TestMap[i].name, name) == 0) {
      return &TestMap[i];
    }
  }
  return NULL;
}

int Test(ClassTest *client, char *testName, char *inputFile, char *outputFile) {
  uint8_t testBuffer[512];
  FILE *input, *output;

  TestCase *tc;

  memset(testBuffer, 0xFF, sizeof(testBuffer));
  printf("Test: %s\n ", testName);
  tc = GetTestCase(testName);
  if (tc == NULL) {
    printf("Test %s not found\n ", testName);
    return 2;
  }

  printf("\n Opening: %s\n ", inputFile);
  input = fopen(inputFile, "r");
  if (input) {
    printf("\n Opening: %s\n ", outputFile);
    output = fopen(outputFile, "w");
    if (output) {
      client->inputFile = input;
      client->outputFile = output;

      client->status = tc->function(client, testBuffer);

      fclose(output);
    } else {
      printf("Failed to openfile\n");
    }
    fclose(input);
  } else {
    printf("Failed to openfile\n");
  }
  return 0;
}

int main(int argc, char **argv) {
  ClassTest clientTest;
  int i = 0;
  while (argc > 3) {
    Test(&clientTest, argv[i + 1], argv[i + 2], argv[i + 3]);
    argc -= 3;
    i += 3;
  }
  return 0;
}

void my_assert(const char *file, const uint32_t line) { exit(1); }
