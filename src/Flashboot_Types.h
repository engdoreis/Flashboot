#ifndef __INTERFACE_H_
#define __INTERFACE_H_

#include <Flashboot_Protocol.h>
#include <stdbool.h>
#include <stdint.h>

typedef enum {
  Flashboot_eGeneralError = -10,
  Flashboot_eCRCError = -9,
  Flashboot_eNullPointerError = -8,
  Flashboot_eBufferOverflowError = -7,
  Flashboot_eBadInputError = -6,
  Flashboot_eAlignmentError = -5,
  Flashboot_eProtocolError = -4,
  Flashboot_eAddressOutOfBoundsError = -3,
  Flashboot_eTimeoutError = -2,
  Flashboot_eOverflowError = -1,
  Flashboot_eOK = 0,
} Flashboot_eResult;

/*!
 * The MBR is used to locate and hold important information about the loaded
 * binaries to the bootloader. The main purpose for the MBR is to receive the
 * CRC during the Update and hold it for integrity verification of the binary
 * before each boot.
 */
#define MBR_MAGIC_KEY 0xFECABABE
#define MBR_ALINGMENT 256
typedef struct {
  uint32_t magicKey; /*!< Four bytes number used to identify the struct in the
                        memory. See ::MBR_MAGIC_KEY*/
  uint32_t version;  /*!< Application version filled after received from host*/
  uint64_t loadAddress; /*!< Storage start address */
  uint32_t
      bootAddress; /*!< Address in program memory where it shall be loaded*/
  uint32_t size;   /*!< Application last address*/
  uint32_t crc; /*!< Application CRC calculated after the flash process and used
                   every boot to verify integrity*/
  uint64_t mbrAddress; /*!< Application start address */
  uint32_t rfu[1];     /*!< Reserved for future use*/
} Flashboot_MBR;

/*!
 * The Interface struct is used for the Boot Loader to hold the callbacks that
 * implement platform dependent functions, thus the Boot Loader remains platform
 * independent and can be ported easily to the other platforms.
 */
typedef struct {
  /*!
   * Function shall write a page of memory.
   * @param arg       Void pointer to the argument passed during the
   * initialization.
   * @param address   Address of the page.
   * @param data      data to be written.
   * @return          The amount of bytes written.
   */
  uint32_t (*WriteMemPage)(void *arg, uint64_t address, uint8_t *data);

  /*!
   * Function shall read the memory.
   * @param arg       Void pointer to the argument passed during the
   * initialization.
   * @param address   Start read address.
   * @param data      Buffer to receive the data.
   * @param size      Size to be read.
   * @return          The amount of bytes read.
   */
  uint32_t (*ReadMem)(void *arg, uint64_t address, uint8_t *data,
                      uint32_t size);

  /*!
   * Function shall erase sector of memory.
   * @param arg       Void pointer to the argument passed during the
   * initialization.
   * @param address   Address of the sector.
   * @return          1 if the sector was erased, 0 if not.
   */
  uint32_t (*EraseMemSector)(void *arg, uint64_t address);

  /*!
   * Function shall enable/disable the memory protection.
   * @param arg       Void pointer to the argument passed during the
   * initialization.
   * @param lock      True to enable protection and False to disable.
   * @return          n/a.
   */
  uint32_t (*ProtectMem)(void *arg, bool lock);

  /*!
   * Function shall verify if a command arrived, if so the function
   * ::Flashboot_ProcessProtocol shall be called.
   * @param arg       Void pointer to the argument passed during the
   * initialization.
   */
  void (*CheckData)(void *arg);

  /*!
   * Function shall send a packet to the host.
   * @param arg       Void pointer to the argument passed during the
   * initialization.
   * @param data      Data to be send.
   * @param size      Data size.
   * @return          The amount of bytes sent.
   */
  uint32_t (*SendData)(void *arg, uint8_t *data, uint32_t size);

  /*!
   * Function shall calculate the CRC32.
   * @param arg       Void pointer to the argument passed during the
   * initialization.
   * @param initValue Init value.
   * @param data      Data input of the calculation.
   * @param size      Data size.
   * @return          The CRC value.
   */
  uint32_t (*CalcCRC)(void *arg, uint32_t initValue, void *data, uint32_t size);

  /*!
   * Function shall perform the specific CPU boot process.
   * @param arg       Void pointer to the argument passed during the
   * initialization.
   * @param address   Binary boot address
   * @return          0 if boot fail.
   */
  uint32_t (*Boot)(void *arg, uint64_t address);

  /*!
   * Function shall return the current system tick value in milliseconds.
   * @param arg       Void pointer to the argument passed during the
   * initialization.
   * @return          The current system tick value in milliseconds.
   */
  uint32_t (*GetTick)(void *arg);

  /*!
   * Function called when an error occur.
   * @param arg       Void pointer to the argument passed during the
   * initialization.
   * @param error     Error value See ::Flashboot_Result.
   */
  void (*SetError)(void *arg, Flashboot_eResult error);
} Flashboot_Interface;

/*! The Flashboot_MemoryConfig struct is used to hold the memory configuration.
 * As it is a platform dependent information it helps the Boot Loader to deal
 * with different memories types
 */
typedef struct {
  uint64_t startAddress; /*!< Application memory area start address */
  uint32_t size;         /*!< Application memory area size          */
  uint32_t pageSize;     /*!< Minimum writable size                 */
  uint32_t sectorSize;   /*!< Minimum erasable size                 */
} Flashboot_Memory;

/*!
 * The Flashboot_Context struct defines the Boot Loader class with its
 * attributes and callbacks.
 */
typedef struct {
  void *arg; /*!< Used to hold an argument that will be send during the
                ::Flashboot_Interface calls.*/
  Flashboot_Interface *interface; /*!< See::Flashboot_Interface. */
  Flashboot_eResult
      result; /*!< Used to hold the current status. See ::Flashboot_Result.*/
  bool commandMode;       /*!< Used to flag if the loader is in command mode.*/
  bool createBackup;      /*!< Check if backup is required.*/
  Flashboot_Frame *frame; /*!< Pointer to a received protocol frame. */
  uint32_t frameSize;     /*!< Received protocol frame size. */
  Flashboot_Memory
      *progMem; /*!< Point to the program memory which is initialized by
            ::Flashboot_Initialize function. */
  Flashboot_Memory
      *dataMem; /*!< Point to the data memory which is initialized by
            ::Flashboot_Initialize function. */
  Flashboot_Memory
      *backupMem;                     /*!< Point to the backup memory which is
                                     initialized      by ::Flashboot_Initialize function. */
  uint8_t *workBuffer;                /*!< Buffer for general use. */
  uint32_t wbs;                       /*!< Work buffer size. */
  uint32_t timeout;                   /*!< Hold the command mode timeout. */
  Flashboot_MBR mbr;                  /*!< Copy of the MBR. */
  void (*Flashboot_State)(void *arg); /*!< Hold the current state function. */

  /*hold a update process information*/
  struct {
    uint64_t bootAddress;  /*!< Hold the load address of the current update to
                              calculate the CRC in the end of the process. */
    uint64_t startAddress; /*!< Hold the start address of the current update to
                              calculate the CRC in the end of the process. */
    uint64_t currentAddress; /*!< Point the current written address. */
    uint32_t size; /*!< Hold the current update size to calculate the CRC in the
                      end of the process. */
    Flashboot_Memory *memory;
  } update;
} Flashboot_Context;

#define Flashboot_ASSERT(_x, _y)                                               \
  if (_x) {                                                                    \
    return _y;                                                                 \
  }

#endif //__INTERFACE_H_
