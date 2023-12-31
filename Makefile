# Copyright Douglas Santos Reis.
# Licensed under the Apache License, Version 2.0, see LICENSE for details.
# SPDX-License-Identifier: Apache-2.0

MKFILE_DIR      := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
DIR_NAME        := $(notdir $(patsubst %/,%,$(MKFILE_DIR)))

CPU             ?= x86
PROFILE         ?= release

include  $(MKFILE_DIR)/make/functions.mk
-include $(MKFILE_DIR)/make/help.mk
include  $(MKFILE_DIR)/make/config.mk
include  $(MKFILE_DIR)/make/cpu/$(CPU).mk
include  $(MKFILE_DIR)/make/profile/$(PROFILE).mk
include  $(MKFILE_DIR)/make/dependencies.mk

SOURCE_HOME               ?= src
                          
TARGET_NAME               ?= $(DIR_NAME)
BUILD_DIR                 ?= build/$(CPU)/$(PROFILE)

DEPENDENCIES_LIB_OBJ      := $(wildcard $(LIB_DIR)/lib*.a)
EXTRACTED_OBJ_DIR         := $(BUILD_DIR)/$(DEPENDENCIES_NAME)

INCDIRS                   += $(call rdwildcard,$(SOURCE_HOME)/,*) src
                         
SRC                       := $(call rfwildcard,$(SOURCE_HOME)/,*.c)
                         
OBJ                       := $(addprefix $(BUILD_DIR)/, $(SRC))
OBJ                       := $(OBJ:.c=.o)
                         
INCFLAGS                  := $(addprefix -I, $(INCDIRS)) 
                         
CFLAGS                    += -Wall -Werror -fdata-sections -ffunction-sections

.PHONY: clean static help deploy doc

static: prerequisites log $(BUILD_DIR)/lib$(TARGET_NAME).a

# $@ expands to the left side of the semicolon
# $^ expands to the right side of the semicolon

$(BUILD_DIR)/lib$(TARGET_NAME).a: $(OBJ)
	@mkdir -p $(dir $@)	
	@echo "Generating $@..."
	@$(AR) -rcs $@ $^
	@printf "Done! \n"

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@echo "Compiling $@..."
	@$(CC) $(CFLAGS) -c $< -o $@ $(INCFLAGS)

#Create distribution packet
install: $(BUILD_DIR)/lib$(TARGET_NAME).a
	@mkdir -p $(dir $<)$(TARGET_NAME)
	@mkdir -p $(dir $<)$(TARGET_NAME)	
	@mkdir -p $(dir $<)$(TARGET_NAME)/lib
	@cp $< $(dir $<)/$(TARGET_NAME)/lib
	@cp -rv src/*.h $(dir $<)$(TARGET_NAME) 

test: testApp
	python3 test/AutoTest.py --elf=$(BUILD_DIR)/$^.exe

testApp: test/src/main.c test/src/porting.c test/src/crc.c $(BUILD_DIR)/lib$(TARGET_NAME).a
	@echo "Generating $(BUILD_DIR)/$@.exe..."
	@$(CC) $(CFLAGS) $^ -o $(BUILD_DIR)/$@.exe $(INCFLAGS)
doc:
	@printf "\nBuilding documentation...\n"
	@mkdir -p doc
	@$(DOXYGEN) Doxyfile

clean:
	$(RM) $(BUILD_DIR)/

debug: testApp
	gdb -ex "layout src" -ex "b main" $(BUILD_DIR)/$^.exe

format-fix:
	@find ./ -name *.c -o -name *.h -o -name *.cpp | xargs clang-format -i
	@black test/*.py

format-check:
	@find ./ -name *.c -o -name *.h -o -name *.cpp | xargs clang-format -n
	@black --check test/*.py
	
log:
	@printf "\nBuilding $(TARGET_NAME) whith the folowing setup:\n"
	@printf "CPU      : $(CPU)\n"
	@printf "PROFILE  : $(PROFILE)\n"
	@printf "CC       : $(CC)\n"
	@printf "CFLAGS   : $(CFLAGS)\n"
	@printf "INCFLAGS : $(INCFLAGS)\n\n"

prerequisites: dependenciesRequisites cpuRequisites profileRequisites
