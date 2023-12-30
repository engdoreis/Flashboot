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

INCDIRS                 += dependencies/$(CRYPTO_ENGINE)/include

LIB_DIR                 += dependencies/$(CRYPTO_ENGINE)/lib/$(CPU)

DEPENDENCIES_NAME       ?= $(CRYPTO_ENGINE)

DOXYGEN                 := $(DOXYGEN_DIR)doxygen

dependenciesRequisites:
