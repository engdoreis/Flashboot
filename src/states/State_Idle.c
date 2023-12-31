// Copyright Douglas Santos Reis.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#include "Flashboot_States.h"
#include "Flashboot_Types.h"
#include "Helper.h"
#include "config.h"

void State_Idle(void *arg) {
  Flashboot_Context *this = (Flashboot_Context *)arg;

  /*Check If there's a command to process*/
  this->interface->CheckData(this->arg);

  /*Check timeout*/
  if (Flashboot_CheckTimeout(this) == Flashboot_eTimeoutError) {
    /*Start application boot process*/
    this->commandMode = false;
    this->Flashboot_State = State_CheckCRC;

    /*Reset timeout in case the boot fails somehow*/
    Flashboot_SetTimeout(this, CONFIG_BOOT_TIMEOUT);
  }
}
