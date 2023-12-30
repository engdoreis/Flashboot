#include "Flashboot_States.h"
#include "Flashboot_Types.h"
#include <Helper.h>

void State_UpdateStart(void *arg) {
  Flashboot_Context *this = (Flashboot_Context *)arg;

  this->result = Flashboot_FlashStart(
      this, this->progMem,
      FLASHBOOT_ARRAY_TO_U32(this->frame->payload.updateStart.startAddress),
      FLASHBOOT_ARRAY_TO_U32(this->frame->payload.updateStart.size));

  Flashboot_Frame *frame = (Flashboot_Frame *)this->workBuffer;

  frame->id = Flashboot_Result;
  frame->payload.Result.result = this->result;
  uint32_t size = sizeof(frame->id) + sizeof(frame->payload.Result);

  /*Send response to the host*/
  this->interface->SendData(this->arg, (uint8_t *)frame, size);

  /*Return to Idle state to wait the next command*/
  this->Flashboot_State = State_Idle;
}
