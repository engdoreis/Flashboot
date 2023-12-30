#include "Flashboot_States.h"
#include "Flashboot_Types.h"
#include <Helper.h>

void State_CheckProgramMemory(void *arg) {
  Flashboot_Context *this = (Flashboot_Context *)arg;

  this->Flashboot_State = State_CheckBackupMemory;

  /*Look for a valid application MBR*/
  if (Flashboot_SearchForMBR(this, this->progMem, &this->mbr) ==
      Flashboot_eOK) {
    /*Application found, if command mode is not enabled, then continue the boot
     * process*/
    this->Flashboot_State = this->commandMode ? State_Idle : State_CheckCRC;
  }
}
