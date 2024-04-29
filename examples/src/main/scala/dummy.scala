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

package dummy

import chisel3._
import chisel3.util._
import AXI._
import _root_.circt.stage.ChiselStage

class Dummy extends Module {
  val io = IO(new Bundle {
    val in = Input(UInt(8.W))
    val out = Output(UInt(8.W))
    val axi = new AXIBundle(64)
  })
  io.out := io.in
  io.axi <> DontCare
  dontTouch(io.axi)
}

object Dummy extends App {
  ChiselStage.emitSystemVerilogFile(new Dummy(), firtoolOpts = Array("-disable-all-randomization", "-strip-debug-info"))
}