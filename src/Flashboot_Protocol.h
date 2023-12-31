// Copyright Douglas Santos Reis.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#ifndef __Flashboot_H_
#define __Flashboot_H_

#include "stdbool.h"
#include "stdint.h"

#define PACKET_SIZE 256

/*!
 * The Flashboot_CmdId enumeration define the commands available for the LD
 * protocol.
 */
typedef enum __attribute__((__packed__)) {
  Flashboot_GetVersion, /*!< Request the application and boot loader versions*/
  Flashboot_BootApplication, /*!< Request to the Boot Loader to start the boot
                          process*/
  Flashboot_UpdateStart,     /*!< Start a update process */
  Flashboot_UpdateTransfer, /*!< Transfer a binary to the Boot Loader as part of
                         the update process*/
  Flashboot_UpdateFinish,   /*!< Finish update process started by
                         Flashboot_UpdateStart   command*/
  Flashboot_Result, /*!< Reply from the boot loader to the host application with
                 the execution status of a command*/
} Flashboot_CmdId;

#pragma pack(push, 1)
/*!
 * The Flashboot_Frame defines how the Flashboot protocol will format the data.
 */
typedef struct {
  Flashboot_CmdId id; /*!< Command ID, this defines which struct of the
                       payload union shall be used.*/
  union {
    struct {
      uint8_t boot[4]; /*!< Flashboot version .*/
      uint8_t app[4];  /*!< User application version.*/
    } version;

    struct {
      uint8_t startAddress[8]; /*!< User application start address.*/
      uint8_t size[4];         /*!< Total size of the user application.*/
    } updateStart;

    struct {
      uint8_t packetID[2];         /*!< Packet Id.*/
      uint8_t packet[PACKET_SIZE]; /*!< Payload with a part of the user
                                      application.*/
    } updateTransfer;

    struct {
      uint8_t crc[4];     /*!< CRC of the user application.*/
      uint8_t version[4]; /*!< User application version.*/
    } updateFinish;

    struct {
      uint8_t result; /*!< Command execution status result.*/
    } Result;
  } payload;
} Flashboot_Frame;
#pragma pack(pop)

/*!
 * Convert a 4 bytes array to a Int32
 */
#define FLASHBOOT_ARRAY_TO_U32(_array)                                         \
  (uint32_t)((_array[0] << 24) | (_array[1] << 16) | (_array[2] << 8) |        \
             (_array[3] & 0xFF))

/*!
 * Convert Int32 to a 4 bytes array
 */
#define FLASHBOOT_U32_TO_ARRAY(_num, _array)                                   \
  _array[0] = (_num >> 24) & 0xFF;                                             \
  _array[1] = (_num >> 16) & 0xFF;                                             \
  _array[2] = (_num >> 8) & 0xFF;                                              \
  _array[3] = (_num >> 0) & 0xFF

/*!
 * Convert a 8 bytes array to a Int64
 */
#define FLASHBOOT_ARRAY_TO_U64(_array)                                         \
  (uint32_t)((_array[0] << 56) | (_array[1] << 48) | (_array[2] << 40) |       \
             (_array[3] << 32)(_array[4] << 24) | (_array[5] << 16) |          \
             (_array[6] << 8) | (_array[7] << 0))

/*!
 * Convert Int64 to a 8 bytes array
 */
#define FLASHBOOT_U64_TO_ARRAY(_num, _array)                                   \
  _array[0] = (_num >> 56) & 0xFF;                                             \
  _array[1] = (_num >> 48) & 0xFF;                                             \
  _array[2] = (_num >> 40) & 0xFF;                                             \
  _array[3] = (_num >> 32) & 0xFF;                                             \
  _array[4] = (_num >> 24) & 0xFF;                                             \
  _array[5] = (_num >> 16) & 0xFF;                                             \
  _array[6] = (_num >> 8) & 0xFF;                                              \
  _array[7] = (_num >> 0) & 0xFF

/*!
 * Convert a 2 bytes array to a Int16
 */
#define FLASHBOOT_ARRAY_TO_U16(_array)                                         \
  (uint16_t)((_array[0] << 8) | (_array[1] & 0xFF))

/*!
 * Convert Int16 to a 2 bytes array
 */
#define FLASHBOOT_U16_TO_ARRAY(_num, _array)                                   \
  _array[0] = (_num >> 8) & 0xFF;                                              \
  _array[1] = (_num >> 0) & 0xFF

#endif //__Flashboot_H_
