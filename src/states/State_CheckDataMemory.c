#include "Flashboot_States.h"
#include "Flashboot_Types.h"
#include <Helper.h>

void State_CheckDataMemory(void *arg) {
  Flashboot_Context *this = (Flashboot_Context *)arg;

  this->Flashboot_State = State_CheckProgramMemory;

  /*Look for a valid application MBR*/
  if (Flashboot_SearchForMBR(this, this->dataMem, &this->mbr) ==
      Flashboot_eOK) {
    uint32_t crc =
        Flashboot_CalcCRC(this, this->mbr.loadAddress, this->mbr.size);

    if (crc == this->mbr.crc &&
        Flashboot_CopyToMemory(this, this->progMem, &this->mbr) ==
            Flashboot_eOK) {
      Flashboot_MemoryErase(this, this->mbr.mbrAddress, sizeof(this->mbr));
      this->Flashboot_State = State_CheckProgramMemory;
    }
  }
}
