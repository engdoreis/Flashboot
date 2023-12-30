#include "Flashboot_States.h"
#include "Flashboot_Types.h"

void State_GetVersion(void *arg) {
  Flashboot_Context *this = (Flashboot_Context *)arg;
  uint32_t size = 0;
  Flashboot_Frame frame;

  /*Convert version number to byte array*/
  FLASHBOOT_U32_TO_ARRAY(0, frame.payload.version.boot);
  FLASHBOOT_U32_TO_ARRAY(this->mbr.version, frame.payload.version.app);
  size = sizeof(frame.id) + sizeof(frame.payload.version);

  /*Send response*/
  this->interface->SendData(this->arg, (uint8_t *)&frame, size);

  /*Return to Idle state*/
  this->Flashboot_State = State_Idle;
}
