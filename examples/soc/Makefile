# ***************************************************************************************
#  Copyright (c) 2024 Emin
#  axi-diff is licensed under Mulan PSL v2.
#  You can use this software according to the terms and conditions of the Mulan PSL v2.
#  You may obtain a copy of Mulan PSL v2 at:
#           http://license.coscl.org.cn/MulanPSL2
#  THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
#  KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
#  NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
#  See the Mulan PSL v2 for more details.
# **************************************************************************************

EXAMPLE_DIR ?= $(shell pwd)/examples
BUILD_DIR := $(shell pwd)/build

include ../../Makefile

CXX_FILES := $(shell find $(shell pwd)/src/test -name "*.cpp")

TOPNAME ?= axiDiffSoCFull

VERILOG_FILES := $(shell find $(shell pwd) -name "*.sv")

examples_verilog:
	mill -i axidiff.runMain soc.Top

examples_sim: examples_verilog
	mkdir -p $(BUILD_DIR)
	@echo $(CFLAGS)
	$(VERILATOR) $(FLAGS) $(INCS) -CFLAGS "$(CFLAGS)" -LDFLAGS $(LDFLAGS) $(CXX_FILES) $(VERILOG_FILES)

examples_run: examples_sim
	$(BUILD_DIR)/V$(TOPNAME)
