# Copyright Douglas Santos Reis.
# Licensed under the Apache License, Version 2.0, see LICENSE for details.
# SPDX-License-Identifier: Apache-2.0

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
