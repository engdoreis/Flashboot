
#include "Flashboot_Protocol.h"
#include "Flashboot_States.h"
#include "Flashboot_Types.h"
#include "config.h"
#include <Helper.h>
#include <string.h>

void Flashboot_Test(Flashboot_Context *this);

Flashboot_eResult Flashboot_Initialize(Flashboot_Context *this, void *arg,
                                       const Flashboot_Interface *interface,
                                       Flashboot_Memory *progMem,
                                       uint8_t *buffer, uint32_t size) {
  Flashboot_eResult res = Flashboot_eOK;

  Flashboot_ASSERT(
      (this == NULL || interface == NULL || progMem == NULL || buffer == NULL),
      Flashboot_eNullPointerError);

  Flashboot_ASSERT(size < progMem->sectorSize, Flashboot_eBufferOverflowError);

  memset(this, 0, sizeof(Flashboot_Context));

  this->arg = arg;
  this->interface = (Flashboot_Interface *)interface;

  this->dataMem = NULL;
  this->backupMem = NULL;
  this->update.memory = this->progMem = progMem;
  this->workBuffer = buffer;
  this->wbs = size;

  this->update.size = this->update.startAddress = this->update.currentAddress =
      0;

  this->Flashboot_State = State_CheckProgramMemory;

  /*Set the timeout for command mode without receive any valid command*/
  Flashboot_SetTimeout(this, CONFIG_BOOT_TIMEOUT);

  // Flashboot_Test(this);

  return res;
}

Flashboot_eResult Flashboot_SetDataMemory(Flashboot_Context *this,
                                          Flashboot_Memory *dataMemory) {
  Flashboot_eResult res = Flashboot_eOK;
  Flashboot_ASSERT(this == NULL || dataMemory == NULL,
                   Flashboot_eNullPointerError);

  this->dataMem = dataMemory;
  this->Flashboot_State = State_CheckDataMemory;
  return res;
}

Flashboot_eResult Flashboot_SetBackupMemory(Flashboot_Context *this,
                                            bool successBoot,
                                            Flashboot_Memory *backupMemory) {
  Flashboot_eResult res = Flashboot_eOK;
  Flashboot_ASSERT(this == NULL || backupMemory == NULL,
                   Flashboot_eNullPointerError);

  this->backupMem = backupMemory;
  this->createBackup = successBoot;
  return res;
}

Flashboot_eResult Flashboot_Execute(Flashboot_Context *this) {
  Flashboot_eResult res = Flashboot_eOK;

  Flashboot_ASSERT(this == NULL, Flashboot_eNullPointerError);

  /*check if the next state is not null*/
  if (this->Flashboot_State) {
    /*execute next state*/
    this->Flashboot_State(this);
    res = this->result;
    if (this->result != Flashboot_eOK && this->interface->SetError) {
      this->interface->SetError(this->arg, this->result);
    }
  }
  return res;
}

Flashboot_eResult Flashboot_ProcessProtocol(Flashboot_Context *this,
                                            const uint8_t *data,
                                            uint32_t size) {
  Flashboot_eResult res = Flashboot_eOK;

  Flashboot_ASSERT((this == NULL || data == NULL), Flashboot_eNullPointerError);

  this->frame = (Flashboot_Frame *)data;
  this->frameSize = size;

  /*Define the next state to be processed based on the command arrived.*/
  switch (this->frame->id) {
  case Flashboot_GetVersion:
    this->Flashboot_State = State_GetVersion;
    break;
  case Flashboot_BootApplication:
    this->Flashboot_State = State_CheckProgramMemory;
    this->commandMode = false;
    break;
  case Flashboot_UpdateStart:
    this->Flashboot_State = State_UpdateStart;
    break;
  case Flashboot_UpdateTransfer:
    this->Flashboot_State = State_UpdateTransfer;
    break;
  case Flashboot_UpdateFinish:
    this->Flashboot_State = State_UpdateFinish;
    break;
  default:
    res = Flashboot_eProtocolError;
    break;
  }

  Flashboot_SetTimeout(this, CONFIG_BOOT_TIMEOUT);
  return res;
}

Flashboot_eResult Flashboot_SetCommandMode(Flashboot_Context *this,
                                           bool enable) {
  Flashboot_eResult res = Flashboot_eOK;
  Flashboot_ASSERT((this == NULL), Flashboot_eNullPointerError);

  this->commandMode = enable;
  return res;
}

Flashboot_eResult Flashboot_Boot(Flashboot_Context *this) {
  Flashboot_ASSERT((this == NULL), Flashboot_eNullPointerError);
  this->result = Flashboot_eOK;
  this->Flashboot_State = State_CheckProgramMemory;
  this->commandMode = false;
  return this->result;
}

Flashboot_eResult Flashboot_StartLoad(Flashboot_Context *this,
                                      const uint32_t selfUpdate,
                                      const uint64_t address, uint32_t size) {
  Flashboot_ASSERT((this == NULL), Flashboot_eNullPointerError);

  Flashboot_Memory *target = this->progMem;

  if (selfUpdate) {
    Flashboot_ASSERT((this->dataMem == NULL), Flashboot_eNullPointerError);
    target = this->dataMem;
  }

  this->result = Flashboot_FlashStart(this, target, address, size);
  return this->result;
}

Flashboot_eResult Flashboot_WriteLoad(Flashboot_Context *this,
                                      const uint8_t *data, uint32_t size) {
  Flashboot_ASSERT((this == NULL || data == NULL), Flashboot_eNullPointerError);

  this->result = Flashboot_FlashWrite(this, data, size);

  return this->result;
}

Flashboot_eResult Flashboot_FinishLoad(Flashboot_Context *this,
                                       const uint32_t crc, uint32_t version) {
  Flashboot_ASSERT((this == NULL), Flashboot_eNullPointerError);

  this->result = Flashboot_FlashFinish(this, crc, version);

  return this->result;
}

void Flashboot_Test(Flashboot_Context *this) {
  uint8_t *address = (uint8_t *)0x08006400;
  static uint8_t oldData[1024];
  static uint8_t newData[1024];
  volatile uint32_t crc = 0;

  for (int i = 0; i < sizeof(oldData); i++) {
    oldData[i] = 0x55;
    newData[i] = 'a' + (i % 23);
  }

  // crc = Flashboot_CalcCRC(this, newData, 3);
  (void)crc;

  this->interface->ProtectMem(this->arg, false);

  Flashboot_MemoryErase(this, (uint64_t)address, 1024);
  Flashboot_MemoryWrite(this, (uint64_t)address, oldData, 1024);
  Flashboot_MemoryErase(this, (uint64_t)address + 2, 508);
  Flashboot_MemoryWrite(this, (uint64_t)address + 2, newData, 508);

  Flashboot_MemoryErase(this, (uint64_t)address, 1024);
  Flashboot_MemoryWrite(this, (uint64_t)address, oldData, 1024);
  Flashboot_MemoryErase(this, (uint64_t)address + 2, 512);
  Flashboot_MemoryWrite(this, (uint64_t)address + 2, newData, 512);

  Flashboot_MemoryErase(this, (uint64_t)address, 1024);
  Flashboot_MemoryWrite(this, (uint64_t)address, oldData, 1024);
  Flashboot_MemoryErase(this, (uint64_t)address + 510, 516);
  Flashboot_MemoryWrite(this, (uint64_t)address + 510, newData, 516);

  this->interface->ProtectMem(this->arg, true);
}
