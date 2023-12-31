# Copyright Douglas Santos Reis.
# Licensed under the Apache License, Version 2.0, see LICENSE for details.
# SPDX-License-Identifier: Apache-2.0

CROSS_TOOLCHAIN        ?= $(subst \,/,$(TOOLCHAIN_HOME))arm-brcm-linux-gnueabi-

CC                     = $(CROSS_TOOLCHAIN)gcc
AR                     = $(CROSS_TOOLCHAIN)ar
OBJCPY                 = $(CROSS_TOOLCHAIN)objcpy
OBJDUMP                = $(CROSS_TOOLCHAIN)objdump
STRIP                  = $(CROSS_TOOLCHAIN)strip
NM                     = $(CROSS_TOOLCHAIN)nm
RM                     = rm -rf

CFLAGS                 := -std=c99 -fmessage-length=0 -fPIC


cpuRequisites:
#ifeq (, $(shell which $(CC)))
#	$(error Compiler not found "$(CC)", set variable TOOLCHAIN_HOME or put it on the path)
#endif	