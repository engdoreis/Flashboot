// Copyright Douglas Santos Reis.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#include "Flashboot_States.h"
#include "Flashboot_Types.h"
#include <Helper.h>

void State_Boot(void *arg) {
  Flashboot_Context *this = (Flashboot_Context *)arg;
  uint64_t address = 0;

  this->result = Flashboot_eGeneralError;
  this->Flashboot_State = State_Idle;

  /* if an application header is available, if not return to Idle state*/
  if (this->mbr.magicKey == MBR_MAGIC_KEY) {
    /*Calculate the application fist address*/
    address = (uint64_t)this->mbr.loadAddress;
    this->result = Flashboot_eOK;
  }

  Flashboot_Frame *frame = (Flashboot_Frame *)this->workBuffer;

  frame->payload.Result.result = this->result;

  if (this->result == Flashboot_eOK) {
    /*Enable flash projection*/
    this->interface->ProtectMem(this->arg, true);
    this->interface->Boot(this->arg, address);
  }
}
