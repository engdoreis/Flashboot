#*********************************************************************************************
# @file     
# @project  
# @platform 
# @device   
# @author   Douglas Reis
# @version  0.1
# @date     06-Feb-2019
# @brief    Configure the source to be use during compilation of the Project    
#
# @history
#
# When         Who               What
# -----------  ----------------  -------------------------------------------------------------
# 06-Feb-2019  Douglas Reis     - Initial Version. 
# ********************************************************************************************


CROSS_TOOLCHAIN        ?= $(subst \,/,$(TOOLCHAIN_HOME))bin/arm-linux-androideabi-

CC                     := $(wildcard $(CROSS_TOOLCHAIN)gcc*)
AR                     := $(CROSS_TOOLCHAIN)ar
OBJCPY                 := $(CROSS_TOOLCHAIN)objcpy
OBJDUMP                := $(CROSS_TOOLCHAIN)objdump
STRIP                  := $(CROSS_TOOLCHAIN)strip
NM                     := $(CROSS_TOOLCHAIN)nm
RM                     := rm -rf

CFLAGS                 := -std=c99 -fPIE -fPIC -I$(subst \,/,$(TOOLCHAIN_HOME))../opt/include


cpuRequisites:
ifeq (, $(CC))
	$(error Compiler not found "$(CC)", set variable TOOLCHAIN_HOME or put it on the path)
endif	