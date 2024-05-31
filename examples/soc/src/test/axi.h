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

#ifndef __AXI4_H
#define __AXI4_H

#include <stdint.h>
#include <cstddef> // size_t
#include <string.h> // memcpy

// #define DEBUG_LOG_AXI4

// 4*64 bits
#define AXI_DATA_WIDTH_64 1

typedef uint64_t axi_addr_t;
typedef uint64_t axi_data_t[AXI_DATA_WIDTH_64];
#define axi_copy_data(dest, src) \
  memcpy(dest, src, sizeof(uint64_t)*AXI_DATA_WIDTH_64);

struct axi_aw_channel {
  uint8_t       ready;
  uint8_t       valid;
  axi_addr_t    addr;
  uint8_t       prot;
  uint8_t       id;
  uint8_t       user;
  uint8_t       len;
  uint8_t       size;
  uint8_t       burst;
  uint8_t       lock;
  uint8_t       cache;
  uint8_t       qos;
};

struct axi_w_channel {
  uint8_t       ready;
  uint8_t       valid;
  axi_data_t    data;
  uint8_t       strb;
  uint8_t       last;
};

struct axi_b_channel {
  uint8_t       ready;
  uint8_t       valid;
  uint8_t       resp;
  uint8_t       id;
  uint8_t       user;
};

struct axi_ar_channel {
  uint8_t       ready;
  uint8_t       valid;
  axi_addr_t    addr;
  uint8_t       prot;
  uint8_t       id;
  uint8_t       user;
  uint8_t       len;
  uint8_t       size;
  uint8_t       burst;
  uint8_t       lock;
  uint8_t       cache;
  uint8_t       qos;
};

struct axi_r_channel {
  uint8_t       ready;
  uint8_t       valid;
  uint8_t       resp;
  axi_data_t    data;
  uint8_t       last;
  uint8_t       id;
  uint8_t       user;
};

struct axi_channel {
  struct axi_aw_channel aw;
  struct axi_w_channel  w;
  struct axi_b_channel  b;
  struct axi_ar_channel ar;
  struct axi_r_channel  r;
};

// dut helper for AXI

// NOTE: change this when migrating between different hardware designs
#define DUT_AXI(name) externalPins_axiDiff_##name

#define axi_aw_copy_from_dut_ptr(dut_ptr, aw)             \
  do {                                                    \
    aw.ready = dut_ptr->DUT_AXI(awready);                \
    aw.valid = dut_ptr->DUT_AXI(awvalid);                \
    aw.addr = dut_ptr->DUT_AXI(awaddr);             \
    aw.id = dut_ptr->DUT_AXI(awid);                 \
    aw.len = dut_ptr->DUT_AXI(awlen);               \
    aw.size = dut_ptr->DUT_AXI(awsize);             \
    aw.burst = dut_ptr->DUT_AXI(awburst);           \
  } while (0);

#define axi_aw_set_dut_ptr(dut_ptr, aw)                   \
  do {                                                    \
    dut_ptr->DUT_AXI(awready) = aw.ready;                \
  } while (0);

#define axi_w_copy_from_dut_ptr(dut_ptr, w)               \
  do {                                                    \
    w.ready = dut_ptr->DUT_AXI(wready);                  \
    w.valid = dut_ptr->DUT_AXI(wvalid);                  \
    axi_copy_data(w.data, &dut_ptr->DUT_AXI(wdata))  \
    w.strb = dut_ptr->DUT_AXI(wstrb);               \
    w.last = dut_ptr->DUT_AXI(wlast);               \
  } while (0);

#define axi_w_set_dut_ptr(dut_ptr, w)                     \
  do {                                                    \
    dut_ptr->DUT_AXI(wready) = w.ready;                  \
  } while (0);

#define axi_b_copy_from_dut_ptr(dut_ptr, b)               \
  do {                                                    \
    b.ready = dut_ptr->DUT_AXI(bready);                  \
    b.valid = dut_ptr->DUT_AXI(bvalid);                  \
    b.resp = dut_ptr->DUT_AXI(bresp);               \
    b.id = dut_ptr->DUT_AXI(bid);                   \
  } while (0);

#define axi_b_set_dut_ptr(dut_ptr, b)                     \
  do {                                                    \
    dut_ptr->DUT_AXI(bvalid) = b.valid;                  \
    dut_ptr->DUT_AXI(bresp) = b.resp;               \
    dut_ptr->DUT_AXI(bid) = b.id;                   \
  } while (0);

#define axi_ar_copy_from_dut_ptr(dut_ptr, ar)             \
  do {                                                    \
    ar.ready = dut_ptr->DUT_AXI(arready);                \
    ar.valid = dut_ptr->DUT_AXI(arvalid);                \
    ar.addr = dut_ptr->DUT_AXI(araddr);             \
    ar.id = dut_ptr->DUT_AXI(arid);                 \
    ar.len = dut_ptr->DUT_AXI(arlen);               \
    ar.size = dut_ptr->DUT_AXI(arsize);             \
    ar.burst = dut_ptr->DUT_AXI(arburst);           \
  } while (0);

#define axi_ar_set_dut_ptr(dut_ptr, ar)                   \
  do {                                                    \
    dut_ptr->DUT_AXI(arready) = ar.ready;                \
  } while (0);

#define axi_r_copy_from_dut_ptr(dut_ptr, r)               \
  do {                                                    \
    r.ready = dut_ptr->DUT_AXI(rready);                  \
    r.valid = dut_ptr->DUT_AXI(rvalid);                  \
    r.resp = dut_ptr->DUT_AXI(rresp);               \
    axi_copy_data(r.data, &dut_ptr->DUT_AXI(rdata))  \
    r.last = dut_ptr->DUT_AXI(rlast);               \
    r.id = dut_ptr->DUT_AXI(rid);                   \
  } while (0);

#define axi_r_set_dut_ptr(dut_ptr, r)                     \
  do {                                                    \
    dut_ptr->DUT_AXI(rvalid) = r.valid;                  \
    dut_ptr->DUT_AXI(rresp) = r.resp;               \
    axi_copy_data(&dut_ptr->DUT_AXI(rdata), r.data)  \
    dut_ptr->DUT_AXI(rlast) = r.last;               \
    dut_ptr->DUT_AXI(rid) = r.id;                   \
  } while (0);

#define axi_copy_from_dut_ptr(dut_ptr, axi)               \
  do {                                                    \
    axi_aw_copy_from_dut_ptr(dut_ptr, axi.aw)             \
    axi_w_copy_from_dut_ptr(dut_ptr, axi.w)               \
    axi_b_copy_from_dut_ptr(dut_ptr, axi.b)               \
    axi_ar_copy_from_dut_ptr(dut_ptr, axi.ar)             \
    axi_r_copy_from_dut_ptr(dut_ptr, axi.r)               \
  } while (0);

#define axi_set_dut_ptr(dut_ptr, axi)                     \
  do {                                                    \
    axi_aw_set_dut_ptr(dut_ptr, axi.aw)                   \
    axi_w_set_dut_ptr(dut_ptr, axi.w)                     \
    axi_b_set_dut_ptr(dut_ptr, axi.b)                     \
    axi_ar_set_dut_ptr(dut_ptr, axi.ar)                   \
    axi_r_set_dut_ptr(dut_ptr, axi.r)                     \
  } while (0);

// ar channel: (1) read raddr; (2) try to accept the address; (3) check raddr fire
bool axi_get_raddr(const axi_channel &axi, axi_addr_t &addr);
void axi_accept_raddr(axi_channel &axi);
bool axi_check_raddr_fire(const axi_channel &axi);

// r channel: (1) put rdata; (2) check rdata fire
void axi_put_rdata(axi_channel &axi, void *src, size_t n, bool last, uint8_t id);
bool axi_check_rdata_fire(const axi_channel &axi);

// aw channel: (1) read waddr; (2) try to accept the address; (3) check waddr fire
bool axi_get_waddr(const axi_channel &axi, axi_addr_t &addr);
void axi_accept_waddr(axi_channel &axi);
bool axi_check_waddr_fire(const axi_channel &axi);

// w channel: (1) accept wdata; (2) get wdata; (3) check wdata fire
void axi_accept_wdata(axi_channel &axi);
void axi_get_wdata(const axi_channel &axi, void *dest, size_t n);
bool axi_check_wdata_fire(const axi_channel &axi);

// b channel: (1) put response; (2) check response fire
void axi_put_wack(axi_channel &axi, uint8_t id);
bool axi_check_wack_fire(const axi_channel &axi);

void init_dramsim3(void);
void dramsim3_helper_rising(const axi_channel &axi);
void dramsim3_helper_falling(axi_channel &axi);
void single_cycle_falling(void);
void single_cycle_rising(void);
#endif
