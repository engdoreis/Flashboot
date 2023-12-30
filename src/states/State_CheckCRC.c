#include "Flashboot_States.h"
#include "Flashboot_Types.h"
#include <Helper.h>
#include <stddef.h>

void State_CheckCRC(void *arg) {
  Flashboot_Context *this = (Flashboot_Context *)arg;
  uint64_t address = 0;

  this->Flashboot_State = State_Idle;

  /* if an application header is available, if not return to Idle state*/
  if (this->mbr.magicKey == MBR_MAGIC_KEY) {
    /*Calculate the application fist address*/
    address = (uint64_t)this->mbr.loadAddress;

    /*Calculate the application CRC*/
    uint32_t crc = Flashboot_CalcCRC(this, address, this->mbr.size);

    /* Check if CRC is valid, if not return to Idle state*/
    if (this->mbr.crc == crc) {
      /*Perform backup before boot*/
      this->Flashboot_State = State_BackupFW;
    }
  }
}
