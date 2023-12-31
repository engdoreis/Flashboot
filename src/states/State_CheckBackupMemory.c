// Copyright Douglas Santos Reis.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#include "Flashboot_States.h"
#include "Flashboot_Types.h"
#include <Helper.h>

void State_CheckBackupMemory(void *arg) {
  Flashboot_Context *this = (Flashboot_Context *)arg;

  this->Flashboot_State = State_Idle;

  if (this->backupMem) {
    /*Look for a valid application MBR*/
    if (Flashboot_SearchForMBR(this, this->backupMem, &this->mbr) ==
        Flashboot_eOK) {
      if (Flashboot_CopyToMemory(this, this->progMem, &this->mbr) ==
          Flashboot_eOK) {
        this->Flashboot_State = State_CheckProgramMemory;
      }
    }
  }
}
