# Copyright Douglas Santos Reis.
# Licensed under the Apache License, Version 2.0, see LICENSE for details.
# SPDX-License-Identifier: Apache-2.0

# Make does not offer a recursive wildcard function, so here's one for files:
rfwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rfwildcard,$d/,$2))

# Make does not offer a recursive wildcard function, so here's one for directories:
rdwildcard=$(sort $(dir $(call rfwildcard,$1,$2)))
