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
import freechips.rocketchip.diplomacy._
import org.chipsalliance.cde.config.Parameters
import freechips.rocketchip.util._
import freechips.rocketchip.amba.axi4._

class axiDiffSoCASIC(implicit p: Parameters) extends LazyModule {
  val xbar = AXI4Xbar()
  val cpu = LazyModule(new CPU(idBits = 4))

  val sramNode = AXI4RAM(AddressSet.misaligned(0x0f000000, 0x2000).head, false, true, 8, None, Nil, false)
  val lsdram_axi = Some(LazyModule(new AXI4SDRAM(AddressSet.misaligned(0xa0000000L, 0x2000000))))

  List(sramNode).map(_ := xbar)
  lsdram_axi.get.node := soc.AXI4Delayer() := xbar
  xbar := cpu.masterNode

  override lazy val module = new Impl
  class Impl extends LazyModuleImp(this) with DontTouch {
    val axiDiff = IO(chiselTypeOf(cpu.module.axiDiff))
    axiDiff <> cpu.module.axiDiff
  }
}

class axiDiffSoCFull(implicit p: Parameters) extends LazyModule {
  val asic = LazyModule(new axiDiffSoCASIC)
  ElaborationArtefacts.add("graphml", graphML)

  override lazy val module = new Impl
  class Impl extends LazyModuleImp(this) with DontTouch {
    val masic = asic.module

    val externalPins = IO(new Bundle{
      val axiDiff = chiselTypeOf(masic.axiDiff)
    })
    externalPins.axiDiff <> masic.axiDiff
  }
}