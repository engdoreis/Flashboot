
// Copyright Douglas Santos Reis.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#ifndef __STATES_H_
#define __STATES_H_

/*!
 * Check for new commands and timeout
 * @param arg  Library context struct
 */
void State_Idle(void *arg);

/*!
 * Respond the version to the host
 * @param arg  Library context struct
 */
void State_GetVersion(void *arg);

/*!
 * Perform the final verifications and call boot callback
 * @param arg  Library context struct
 */
void State_Boot(void *arg);

/*!
 * Process the Update start command from the host
 * @param arg  Library context struct
 */
void State_UpdateStart(void *arg);

/*!
 * Process Update Finish command
 * @param arg  Library context struct
 */
void State_UpdateTransfer(void *arg);

/*!
 * Process Update finish command
 * @param arg  Library context struct
 */
void State_UpdateFinish(void *arg);

/*!
 * Check if the application CRC is valid
 * @param arg  Library context struct
 */
void State_CheckCRC(void *arg);

/*!
 * Look for a valid application header in the memory
 * @param arg  Library context struct
 */
void State_CheckProgramMemory(void *arg);

void State_CheckDataMemory(void *arg);

void State_CheckBackupMemory(void *arg);

void State_BackupFW(void *arg);

#endif //__STATES_H_
