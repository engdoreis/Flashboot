#include "Flashboot_States.h"
#include "Flashboot_Types.h"
#include <Helper.h>

void State_BackupFW(void *arg) {
  Flashboot_Context *this = (Flashboot_Context *)arg;

  this->Flashboot_State = State_Boot;

  if (this->backupMem && this->createBackup) {
    Flashboot_MBR mbr;
    /*Look for a valid application MBR*/
    if (Flashboot_SearchForMBR(this, this->backupMem, &mbr) != Flashboot_eOK ||
        mbr.crc != this->mbr.crc) {
      Flashboot_CopyToMemory(this, this->backupMem, &this->mbr);
    }
  }
}
