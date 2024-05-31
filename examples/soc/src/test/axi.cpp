/***************************************************************************************
* Copyright (c) 2020-2021 Institute of Computing Technology, Chinese Academy of Sciences
* Copyright (c) 2020-2021 Peng Cheng Laboratory
*
* XiangShan is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include "axi.h"

#include <iostream>
#include <assert.h>

// This shoulde be you own dut
#include <VaxiDiffSoCFull.h>
extern VaxiDiffSoCFull *dut;

#ifdef WITH_DRAMSIM3
#include "cosimulation.h"
CoDRAMsim3 *dram = NULL;
#endif

static uint64_t *ram;

#define EMU_RAM_SIZE (8 * 1024 * 1024 * 1024UL)

#ifdef WITH_DRAMSIM3
void init_dramsim3(void) {
#if !defined(DRAMSIM3_CONFIG) || !defined(DRAMSIM3_OUTDIR)
#error DRAMSIM3_CONFIG or DRAMSIM3_OUTDIR is not defined
#endif
  assert(dram == NULL);
  dram = new ComplexCoDRAMsim3(DRAMSIM3_CONFIG, DRAMSIM3_OUTDIR);
}
void dramsim3_finish() { delete dram; }

#define MAX_AXI_DATA_LEN 8

// currently does not support masked read or write
struct dramsim3_meta {
  uint8_t len;
  uint8_t size;
  uint8_t offset;
  uint8_t id;
  uint64_t data[MAX_AXI_DATA_LEN];
};

void axi_read_data(const axi_ar_channel &ar, dramsim3_meta *meta) {
  uint64_t address = ar.addr % EMU_RAM_SIZE;
  uint64_t beatsize = 1 << ar.size;
  uint8_t beatlen = ar.len + 1;
  uint64_t transaction_size = beatsize * beatlen;
  assert((transaction_size % sizeof(uint64_t)) == 0);
  // axi burst FIXED
  if (ar.burst == 0x0) {
    std::cout << "axi burst FIXED not supported!" << std::endl;
    assert(0);
  }
  // axi burst INCR
  else if (ar.burst == 1) {
    assert(transaction_size / sizeof(uint64_t) <= MAX_AXI_DATA_LEN);
    for (int i = 0; i < transaction_size / sizeof(uint64_t); i++) {
      meta->data[i] = ram[address / sizeof(uint64_t)];
      address += sizeof(uint64_t);
    }
  }
  // axi burst WRAP
  else if (ar.burst == 2) {
    uint64_t low = (address / transaction_size) * transaction_size;
    uint64_t high = low + transaction_size;
    assert(transaction_size / sizeof(uint64_t) <= MAX_AXI_DATA_LEN);
    for (int i = 0; i < transaction_size / sizeof(uint64_t); i++) {
      if (address == high) {
        address = low;
      }
      meta->data[i] = ram[address / sizeof(uint64_t)];
      address += sizeof(uint64_t);
    }
  } else {
    std::cout << "reserved arburst!" << std::endl;
    assert(0);
  }
  meta->len = beatlen;
  meta->size = beatsize;
  meta->offset = 0;
  meta->id = ar.id;
}

CoDRAMRequest *dramsim3_request(const axi_channel &axi, bool is_write) {
  uint64_t address = (is_write) ? axi.aw.addr : axi.ar.addr;
  dramsim3_meta *meta = new dramsim3_meta;
  // WRITE
  if (is_write) {
    meta->len = axi.aw.len + 1;
    meta->size = 1 << axi.aw.size;
    meta->offset = 0;
    meta->id = axi.aw.id;
  } else {
    axi_read_data(axi.ar, meta);
  }
  CoDRAMRequest *req = new CoDRAMRequest();
  req->address = address;
  req->is_write = is_write;
  req->meta = meta;
  return req;
}

static CoDRAMResponse *wait_resp_r = NULL;
static CoDRAMResponse *wait_resp_b = NULL;
static CoDRAMRequest *wait_req_w = NULL;
// currently only accept one in-flight read + one in-flight write
static uint64_t raddr, roffset = 0, rlen;
static uint64_t waddr, woffset = 0, wlen;

void dramsim3_helper_rising(const axi_channel &axi) {
  // ticks DRAMsim3 according to CPU_FREQ:DRAM_FREQ
  dram->tick();

  // read data fire: check the last read request
  if (axi_check_rdata_fire(axi)) {
    if (wait_resp_r == NULL) {
      printf("ERROR: There's no in-flight read request.\n");
      assert(wait_resp_r != NULL);
    }
    dramsim3_meta *meta = static_cast<dramsim3_meta *>(wait_resp_r->req->meta);
    meta->offset++;
    // check whether the last rdata response has finished
    if (meta->offset == meta->len) {
      delete meta;
      delete wait_resp_r->req;
      delete wait_resp_r;
      wait_resp_r = NULL;
    }
  }

  // read address fire: accept a new request
  if (axi_check_raddr_fire(axi)) {
    dram->add_request(dramsim3_request(axi, false));
  }

  // the last write transaction is acknowledged
  if (axi_check_wack_fire(axi)) {
    if (wait_resp_b == NULL) {
      printf("ERROR: write response fire for nothing in-flight.\n");
      assert(wait_resp_b != NULL);
    }
    // flush data to memory
    uint64_t waddr = wait_resp_b->req->address % EMU_RAM_SIZE;
    dramsim3_meta *meta = static_cast<dramsim3_meta *>(wait_resp_b->req->meta);
    void *start_addr = ram + (waddr / sizeof(uint64_t));
    memcpy(start_addr, meta->data, meta->len * meta->size);
    for (int i = 0; i < meta->len; i++) {
      //   uint64_t address = wait_resp_b->req->address % EMU_RAM_SIZE;
      //   ram[address / sizeof(uint64_t) + i] = meta->data[i];
      // printf("flush write to memory[0x%ld] = 0x%lx\n", address)
    }
    delete meta;
    delete wait_resp_b->req;
    delete wait_resp_b;
    wait_resp_b = NULL;
  }

  // write address fire: accept a new write request
  if (axi_check_waddr_fire(axi)) {
    if (wait_req_w != NULL) {
      printf("ERROR: The last write request has not finished.\n");
      assert(wait_req_w == NULL);
    }
    wait_req_w = dramsim3_request(axi, true);
    // printf("accept a new write request to addr = 0x%lx, len = %d\n",
    // axi.aw.addr, axi.aw.len);
  }

  // write data fire: for the last write transaction
  if (axi_check_wdata_fire(axi)) {
    if (wait_req_w == NULL) {
      printf("ERROR: wdata fire for nothing in-flight.\n");
      assert(wait_req_w != NULL);
    }
    dramsim3_meta *meta = static_cast<dramsim3_meta *>(wait_req_w->meta);
    void *data_start =
        meta->data + meta->offset * meta->size / sizeof(uint64_t);
    axi_get_wdata(axi, data_start, meta->size);
    meta->offset++;
    // printf("accept a new write data\n");
  }
  if (wait_req_w) {
    dramsim3_meta *meta = static_cast<dramsim3_meta *>(wait_req_w->meta);
    // if this is the last beat
    if (meta->offset == meta->len &&
        dram->will_accept(wait_req_w->address, true)) {
      dram->add_request(wait_req_w);
      wait_req_w = NULL;
    }
  }
}

void dramsim3_helper_falling(axi_channel &axi) {
  // default branch to avoid wrong handshake
  axi.aw.ready = 0;
  axi.w.ready = 0;
  axi.b.valid = 0;
  axi.ar.ready = 0;
  axi.r.valid = 0;

  // RDATA: if finished, we try the next rdata response
  if (!wait_resp_r) wait_resp_r = dram->check_read_response();
  // if there's some data response, put it onto axi bus
  if (wait_resp_r) {
    dramsim3_meta *meta = static_cast<dramsim3_meta *>(wait_resp_r->req->meta);
    // printf("meta->size %d offset %d\n", meta->size,
    // meta->offset*meta->size/sizeof(uint64_t));
    void *data_start =
        meta->data + meta->offset * meta->size / sizeof(uint64_t);
    axi_put_rdata(axi, data_start, meta->size, meta->offset == meta->len - 1,
                  meta->id);
  }

  // RADDR: check whether the read request can be accepted
  axi_addr_t raddr;
  if (axi_get_raddr(axi, raddr) && dram->will_accept(raddr, false)) {
    axi_accept_raddr(axi);
    // printf("try to accept read request to 0x%lx\n", raddr);
  }

  // WREQ: check whether the write request can be accepted
  // Note: block the next write here to simplify logic
  axi_addr_t waddr;
  if (wait_req_w == NULL && axi_get_waddr(axi, waddr) &&
      dram->will_accept(waddr, true)) {
    axi_accept_waddr(axi);
    axi_accept_wdata(axi);
    // printf("try to accept write request to 0x%lx\n", waddr);
  }

  // WDATA: check whether the write data can be accepted
  if (wait_req_w != NULL && dram->will_accept(wait_req_w->address, true)) {
    dramsim3_meta *meta = static_cast<dramsim3_meta *>(wait_req_w->meta);
    // we have to check whether the last finished write request has been
    // accepted by dram
    if (meta->offset != meta->len) {
      axi_accept_wdata(axi);
    }
  }

  // WRESP: if finished, we try the next write response
  if (!wait_resp_b) wait_resp_b = dram->check_write_response();
  // if there's some write response, put it onto axi bus
  if (wait_resp_b) {
    dramsim3_meta *meta = static_cast<dramsim3_meta *>(wait_resp_b->req->meta);
    axi_put_wack(axi, meta->id);
  }
}

// ar channel: (1) read raddr; (2) try to accept the address; (3) check raddr fire
bool axi_get_raddr(const axi_channel &axi, axi_addr_t &addr) {
  if (axi.ar.valid) {
    addr = axi.ar.addr;
    return true;
  }
  return false;
}

void axi_accept_raddr(axi_channel &axi) {
  axi.ar.ready = 1;
}

bool axi_check_raddr_fire(const axi_channel &axi) {
  if (axi.ar.valid && axi.ar.ready) {
#ifdef DEBUG_LOG_AXI4
    printf("axi ar channel fired addr = 0x%lx, id = %d\n", axi.ar.addr, axi.ar.id);
#endif
    return true;
  }
  return false;
}

// r channel: (1) put rdata; (2) check rdata fire
void axi_put_rdata(axi_channel &axi, void *src, size_t n, bool last, uint8_t id) {
  memcpy(axi.r.data, src, n);
  axi.r.valid = 1;
  axi.r.last = (last) ? 1 : 0;
  axi.r.id = id;
}

bool axi_check_rdata_fire(const axi_channel &axi) {
  if (axi.r.ready && axi.r.valid) {
#ifdef DEBUG_LOG_AXI4
    printf("axi r channel fired data = %lx, id = %d\n", axi.r.data[0], axi.r.id);
#endif
    return true;
  }
  return false;
}

// aw channel: (1) read waddr; (2) try to accept the address; (3) check waddr fire
bool axi_get_waddr(const axi_channel &axi, axi_addr_t &addr) {
  if (axi.aw.valid) {
    addr = axi.aw.addr;
    return true;
  }
  return false;
}

void axi_accept_waddr(axi_channel &axi) {
  axi.aw.ready = 1;
}

bool axi_check_waddr_fire(const axi_channel &axi) {
  if (axi.aw.valid && axi.aw.ready) {
    assert(axi.aw.burst == 1 || (axi.aw.burst == 2 && ((axi.aw.addr & 0x3f) == 0)));
#ifdef DEBUG_LOG_AXI4
    printf("axi aw channel fired\n");
#endif
    return true;
  }
  return false;
}

// w channel: (1) accept wdata; (2) get wdata; (3) check wdata fire
void axi_accept_wdata(axi_channel &axi) {
  axi.w.ready = 1;
}

bool axi_check_wdata_fire(const axi_channel &axi) {
  if (axi.w.valid && axi.w.ready) {
#ifdef DEBUG_LOG_AXI4
    printf("axi w channel fired\n");
#endif
    return true;
  }
  return false;
}

void axi_get_wdata(const axi_channel &axi, void *dest, size_t n) {
  memcpy(dest, axi.w.data, n);
}

// b channel: (1) put response; (2) check response fire
void axi_put_wack(axi_channel &axi, uint8_t id) {
  axi.b.valid = 1;
  axi.b.id = id;
}

bool axi_check_wack_fire(const axi_channel &axi) {
  if (axi.b.valid && axi.b.ready) {
#ifdef DEBUG_LOG_AXI4
    printf("axi b channel fired\n");
#endif
    return true;
  }
  return false;
}

void single_cycle_rising(void) {
  axi_channel axi;
  axi_copy_from_dut_ptr(dut, axi);
  axi.aw.addr -= 0x80000000UL;
  axi.ar.addr -= 0x80000000UL;
  dramsim3_helper_rising(axi);
}

void single_cycle_falling(void) {
  axi_channel axi;
  axi_copy_from_dut_ptr(dut, axi);
  axi.aw.addr -= 0x80000000UL;
  axi.ar.addr -= 0x80000000UL;
  dramsim3_helper_falling(axi);
  axi_set_dut_ptr(dut, axi);
}

#endif


