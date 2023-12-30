#*********************************************************************************************
# @file     
# @project  
# @platform 
# @device   
# @author   Eng. Douglas Reis
# @version  0.1
# @date     03-Mar-2020
# @brief    Configure the source to be use during compilation of the Project    
#
# @history
#
# When         Who               What
# -----------  ----------------  -------------------------------------------------------------
# 03-Mar-2020  Eng. Douglas Reis     - Initial Version. 
# ********************************************************************************************


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