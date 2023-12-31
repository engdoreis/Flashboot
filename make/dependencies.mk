# Copyright Douglas Santos Reis.
# Licensed under the Apache License, Version 2.0, see LICENSE for details.
# SPDX-License-Identifier: Apache-2.0

INCDIRS                 += dependencies/$(CRYPTO_ENGINE)/include

LIB_DIR                 += dependencies/$(CRYPTO_ENGINE)/lib/$(CPU)

DEPENDENCIES_NAME       ?= $(CRYPTO_ENGINE)

DOXYGEN                 := $(DOXYGEN_DIR)doxygen

dependenciesRequisites:
