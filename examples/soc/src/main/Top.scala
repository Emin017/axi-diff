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
import org.chipsalliance.cde.config.Parameters
import freechips.rocketchip.system.DefaultConfig
import freechips.rocketchip.diplomacy.LazyModule

class Top extends Module {
  implicit val config: Parameters = new DefaultConfig

  val io = IO(new Bundle { })
  val dut = LazyModule(new axiDiffSoCFull)
  val mdut = Module(dut.module)
  mdut.dontTouchPorts()
  mdut.externalPins := DontCare
}

object Top extends App {
  val firtoolOptions = Array("--disable-annotation-unknown")
  circt.stage.ChiselStage.emitSystemVerilogFile(new Top, args, firtoolOptions)
}