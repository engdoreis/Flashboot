#include "Flashboot_States.h"
#include "Flashboot_Types.h"
#include <Helper.h>

void State_UpdateTransfer(void *arg) {
  Flashboot_Context *this = (Flashboot_Context *)arg;

  /*Calculate the packet size*/
  uint32_t size = this->frameSize - sizeof(this->frame->id) -
                  sizeof(this->frame->payload.updateTransfer.packetID);

  Flashboot_FlashWrite(this, this->frame->payload.updateTransfer.packet, size);

  Flashboot_Frame *frame = (Flashboot_Frame *)this->workBuffer;

  frame->id = Flashboot_Result;
  frame->payload.Result.result = this->result;
  size = sizeof(frame->id) + sizeof(frame->payload.Result);

  /*Send response to the host*/
  this->interface->SendData(this->arg, (uint8_t *)frame, size);

  /*Return to Idle state to wait the next command*/
  this->Flashboot_State = State_Idle;
}
