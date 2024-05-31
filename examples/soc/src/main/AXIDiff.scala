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

package soc

import chisel3._
import chisel3.util._
import freechips.rocketchip.amba.axi4.AXI4Bundle

class AXIDiffBundle extends Bundle {
  val arready = Bool()
  val arvalid = Bool()
  val araddr  = UInt(32.W)
  val arid    = UInt(4.W)
  val arlen   = UInt(8.W)
  val arsize  = UInt(3.W)
  val arburst = UInt(2.W)
  val rready  = Bool()
  val rvalid  = Bool()
  val rresp   = UInt(2.W)
  val rdata   = UInt(32.W)
  val rlast   = Bool()
  val rid     = UInt(4.W)
  val awready = Bool()
  val awvalid = Bool()
  val awaddr  = UInt(32.W)
  val awid    = UInt(4.W)
  val awlen   = UInt(8.W)
  val awsize  = UInt(3.W)
  val awburst = UInt(2.W)
  val wready  = Bool()
  val wvalid  = Bool()
  val wdata   = UInt(32.W)
  val wstrb   = UInt(4.W)
  val wlast   = Bool()
  val bready  = Bool()
  val bvalid  = Bool()
  val bresp   = UInt(2.W)
  val bid     = UInt(4.W)
}

object AXIDiffBundle {
  def connect(abus: AXI4Bundle) = {
    val axi = IO(new AXIDiffBundle)
    axi.arready := abus.ar.ready
    axi.arvalid := abus.ar.valid
    axi.araddr  := abus.ar.bits.addr
    axi.arid    := abus.ar.bits.id
    axi.arlen   := abus.ar.bits.len
    axi.arsize  := abus.ar.bits.size
    axi.arburst := abus.ar.bits.burst
    axi.rready  := abus.r.ready
    axi.rvalid  := abus.r.valid
    axi.rresp   := abus.r.bits.resp
    axi.rdata   := abus.r.bits.data
    axi.rlast   := abus.r.bits.last
    axi.rid     := abus.r.bits.id
    axi.awready := abus.aw.ready
    axi.awvalid := abus.aw.valid
    axi.awaddr  := abus.aw.bits.addr
    axi.awid    := abus.aw.bits.id
    axi.awlen   := abus.aw.bits.len
    axi.awsize  := abus.aw.bits.size
    axi.awburst := abus.aw.bits.burst
    axi.wready  := abus.w.ready
    axi.wvalid  := abus.w.valid
    axi.wdata   := abus.w.bits.data
    axi.wstrb   := abus.w.bits.strb
    axi.wlast   := abus.w.bits.last
    axi.bready  := abus.b.ready
    axi.bvalid  := abus.b.valid
    axi.bresp   := abus.b.bits.resp
    axi.bid     := abus.b.bits.id
    axi
  }
}