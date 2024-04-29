/***************************************************************************************
* Copyright (c) 2024 Emin
* axi-diff is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
* KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
* NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include "verilated.h"
#include <stdio.h>
#include <assert.h>
#include <VDummy___024root.h>
#include <VDummy.h>
#include "axi.h"

VDummy *dut;

bool flag = false;
int counter = 0;

void step() {
	dut->clock = 0;
	dut->eval();
#ifdef WITH_DRAMSIM3
  single_cycle_falling();
#endif
	dut->clock = 1;
	dut->eval();
#ifdef WITH_DRAMSIM3
	single_cycle_rising();
#endif
}

void rst_step() {
	dut->clock = 0;
	dut->eval();
	dut->clock = 1;
	dut->eval();
}
void reset(int n) { 
	dut->reset = 1; 
	while (n --) { rst_step(); } 
	dut->reset = 0; 
}

int main(int argc, char *argv[]) {
	dut = new VDummy;
#ifdef WITH_DRAMSIM3
	init_dramsim3();
#endif
	reset(6);
	int i = 0;
	while (i < 100) {
		step();
		i++;
	}
	printf("Simulation done\n");
	return 0;
}
