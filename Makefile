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

AXI_DIFF_HOME ?= $(shell pwd)
DRAMSIM3_HOME ?= $(AXI_DIFF_HOME)/DRAMsim3
BUILD_DIR ?= $(shell pwd)/build
WITH_DRAMSIM3 ?= 1
RUN_EXAMPLES ?= 1

# co-simulation with DRAMsim3
ifeq ($(WITH_DRAMSIM3),1)
EMU_CXXFLAGS += -I$(DRAMSIM3_HOME)/src
EMU_CXXFLAGS += -DWITH_DRAMSIM3 -DDRAMSIM3_CONFIG=\\\"$(DRAMSIM3_HOME)/configs/XiangShan.ini\\\" -DDRAMSIM3_OUTDIR=\\\"$(BUILD_DIR)\\\"
EMU_LDFLAGS  += $(DRAMSIM3_HOME)/build/libdramsim3.a
endif

LIBS += $(EMU_LDFLAGS)

LDFLAGS += $(LIBS) $(EMU_LDFLAGS)
CXX_FILES += $(AXI_DIFF_HOME)/src/test/axi.cpp

INCS += -I/usr/include -I$(AXI_DIFF_HOME)/src/test -I$(BUILD_DIR)
CFLAGS += -g -std=c++17 $(INCS) $(EMU_CXXFLAGS)

VERILATOR ?= verilator
BUILD_DIR ?= build

FLAGS += --cc --exe --build --timescale-override 1ns/1ns --sv -j `nproc` --top $(TOPNAME) --Mdir $(BUILD_DIR)

init:
	git submodule update --init --recursive

dramsim: init
	cd $(DRAMSIM3_HOME) && mkdir -p build && cd $(DRAMSIM3_HOME)/build && cmake -D COSIM=1 $(DRAMSIM3_HOME) && make -j`nproc`

sim:
	mkdir -p $(BUILD_DIR)
	@echo $(CFLAGS)
	$(VERILATOR) $(FLAGS) $(INCS) -CFLAGS "$(CFLAGS)" -LDFLAGS $(LDFLAGS) $(CXX_FILES) $(VERILOG_FILES) 

run:
	$(BUILD_DIR)/V$(TOPNAME)

gdb:
	gdb $(BUILD_DIR)/V$(TOPNAME)

clean:
	rm -rf $(BUILD_DIR)
