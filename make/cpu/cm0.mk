# Copyright Douglas Santos Reis.
# Licensed under the Apache License, Version 2.0, see LICENSE for details.
# SPDX-License-Identifier: Apache-2.0

CROSS_TOOLCHAIN        ?= $(subst \,/,$(TOOLCHAIN_HOME))/bin/arm-none-eabi-

CC                     = $(CROSS_TOOLCHAIN)gcc
AR                     = $(CROSS_TOOLCHAIN)ar
OBJCPY                 = $(CROSS_TOOLCHAIN)objcopy
OBJDUMP                = $(CROSS_TOOLCHAIN)objdump
STRIP                  = $(CROSS_TOOLCHAIN)strip
SIZE                   = $(CROSS_TOOLCHAIN)size
NM                     = $(CROSS_TOOLCHAIN)nm
RM                     = rm -rf

C_FLAGS                := -std=c99 -fmessage-length=0 -mcpu=cortex-m0 -mthumb -mfloat-abi=soft -fno-common -fdata-sections -ffunction-sections

LL_FLAGS               := -mcpu=cortex-m0 -mthumb -mfloat-abi=soft -specs=nosys.specs -specs=nano.specs -Wl,--gc-sections -lm

cpuRequisites:
#ifeq (, $(shell which $(CC)))
#	$(error Compiler not found "$(CC)", set variable TOOLCHAIN_HOME or put it on the path)
#endif	