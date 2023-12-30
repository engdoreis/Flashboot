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

help:
	# @$(eval CPU_LIST=$(basename $(notdir $(shell find make/cpu -type f -name *.mk))))
	# @$(eval PROFILE_LIST=$(basename $(notdir $(shell find make/profile -type f -name *.mk))))
	@$(eval CPU_LIST=$(basename $(notdir $(call  rfwildcard,make/cpu,*.mk))))
	@$(eval PROFILE_LIST=$(basename $(notdir $(call rfwildcard,make/profile,*.mk))))
	@printf "\nSet the following variables:\n"
	@printf "TOOLCHAIN_HOME:\n"
	@printf "EMV_REPOSITORY:\n"
	@printf "MIDDLEWARE_REPOSITORY:\n"
	@printf "\nChoose one of the following recipe:\n"
	@printf "static    : Build static library\n"
	@printf "binary    : Build dynamic library\n"
	@printf "clean     : Erase build files\n"
	@printf "help      : Show this message\n"
	@printf "\nSet the following variables during make call:\n"
	@printf "CPU       : [ $(CPU_LIST) ]\n"
	@printf "PROFILE   : [ $(PROFILE_LIST) ]\n"
	@printf "\nExample   : make static CPU=$(firstword $(CPU_LIST)) PROFILE=CPU=$(firstword $(PROFILE_LIST))\n"
	@printf "\n"
