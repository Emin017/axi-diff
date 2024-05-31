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
import freechips.rocketchip.amba.axi4._

class Dummy extends Module {
  val io = IO(new Bundle {
    val master = new AXI4Bundle(CPUAXI4BundleParameters())
  })
  dontTouch(io.master)
  io.master <> DontCare
}