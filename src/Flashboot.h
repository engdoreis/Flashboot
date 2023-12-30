#ifndef __FLASHBOOT_H_
#define __FLASHBOOT_H_

#include "Flashboot_Types.h"

/*!
 * @brief Initialize the loader library.
 * @param[in] this           Context struct that must be allocated by the
 * application.
 * @param[in] arg            Void pointer to a object that may be passed the
 * interface API.
 * @param[in] interface      Struct with the platform based API.
 * @param[in] programMemory  Memory region used to execute the application.
 * @param[in] buffer         Work buffer allocated by the application needed by
 * the library.
 * @param[in] size           Work buffer size.
 * @return See ::Flashboot_Result
 */
Flashboot_eResult Flashboot_Initialize(Flashboot_Context *this, void *arg,
                                       const Flashboot_Interface *interface,
                                       Flashboot_Memory *programMemory,
                                       uint8_t *buffer, uint32_t size);

/*!
 * @brief Enable the use of a memory region where can contain a firmware to be
 * loaded into the program memory region.
 * @details Calling this function during the initialization will make the
 * Flashboot search of a valid binary in the data memory, once the binary is
 * found it will be loaded into the program memory.
 *
 * @param[in] this           Context struct that must be allocated by the
 * application.
 * @param[in] programMemory  Memory region where can contain a binary to be
 * loaded.
 *
 * @return See ::Flashboot_Result
 */
Flashboot_eResult Flashboot_SetDataMemory(Flashboot_Context *this,
                                          Flashboot_Memory *dataMemory);

/*!
 * @brief Enable the use of a memory region where can contain the firmware
 * backup.
 * @details Calling this function during the initialization will make the
 * Flashboot search of a valid binary in the backup memory either there's no
 * firmware loaded or the loaded firmware is corrupted. When a valid firmware is
 * found, it will be loaded into the program memory.
 *
 * @param[in] this          Context struct that must be allocated by the
 * application.
 * @param[in] successBoot   Indicate if the last boot was successfully. If so
 * the the backMemory can be used to create a backup of the current FW. If not
 * the backupMemory will be searched for a backup FW.
 * @param[in] backupMemory  Memory region where can contain a binary to be
 * loaded.
 *
 * @return See ::Flashboot_Result
 */
Flashboot_eResult Flashboot_SetBackupMemory(Flashboot_Context *this,
                                            bool successBoot,
                                            Flashboot_Memory *backupMemory);

/*!
 * @brief Execute the Flashboot in a cooperative way.
 * @param[in] this    Struct initialized by ::Flashboot_Initialize function.
 * @return            See ::Flashboot_Result
 */
Flashboot_eResult Flashboot_Execute(Flashboot_Context *this);

/*!
 * @brief Shall be called when a command arrives and is ready to be processed.
 * @param[in] this     Struct initialized by ::Flashboot_Initialize function.
 * @param[in] data     Buffer with the command.
 * @param[in] size     Data size.
 * @return See ::Flashboot_Result
 */
Flashboot_eResult Flashboot_ProcessProtocol(Flashboot_Context *this,
                                            const uint8_t *data, uint32_t size);

/*!
 * @brief Start a update process.
 * @param[in] this     Struct initialized by ::Flashboot_Initialize function.
 * @param[in] address  The start address to load the FW.
 * @param[in] size     FW size.
 * @return See ::Flashboot_Result
 */
Flashboot_eResult Flashboot_StartLoad(Flashboot_Context *this, bool selfUpdate,
                                      const uint64_t address, uint32_t size);

/*!
 * @brief Write a slice of the update process.
 * @param[in] this     Struct initialized by ::Flashboot_Initialize function.
 * @param[in] data     Slice of FW.
 * @param[in] size     Slice size.
 * @return See ::Flashboot_Result
 */
Flashboot_eResult Flashboot_WriteLoad(Flashboot_Context *this,
                                      const uint8_t *data, uint32_t size);

/*!
 * @brief Finish update process started by Flashboot_UpdateStart command.
 * @param[in] this     Struct initialized by ::Flashboot_Initialize function.
 * @param[in] crc      CRC FW.
 * @param[in] version  FW version.
 * @return See ::Flashboot_Result
 */
Flashboot_eResult Flashboot_FinishLoad(Flashboot_Context *this,
                                       const uint32_t crc, uint32_t version);

/*!
 * @brief Find the entry point in the program memory and boot.
 * @param[in] this     Struct initialized by ::Flashboot_Initialize function.
 * @return See ::Flashboot_Result
 */
Flashboot_eResult Flashboot_Boot(Flashboot_Context *this);

/*!
 * @brief Set the execution mode. By default the Flashboot will try find a
 * valid application and boot it.
 * @param[in] this      Struct initialized by ::Flashboot_Initialize function.
 * @param[in] enable    True to enable command mode instead of try to boot a
 * valid application.
 * @return See ::Flashboot_Result
 */
Flashboot_eResult Flashboot_SetCommandMode(Flashboot_Context *this,
                                           bool enable);

#endif //__FLASHBOOT_H_
