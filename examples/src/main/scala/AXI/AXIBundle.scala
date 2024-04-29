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

package AXI

import chisel3._
import chisel3.experimental.dataview.DataView
import chisel3.util._

class AXIBundleAR(width: Int) extends Bundle {
  val addr: UInt = UInt(width.W)
  val prot: UInt = UInt(3.W)
  val id: UInt = UInt(4.W)
  val user: Unit = UInt(4.W)
  val len: UInt = UInt(8.W)
  val size: UInt = UInt(3.W)
  val burst: UInt = UInt(2.W)
  val lock: UInt = UInt(1.W)
  val cache: UInt = UInt(4.W)
  val qos: UInt = UInt(3.W)
}

class AXIBundleR(width: Int) extends Bundle {
  val resp: UInt = UInt(2.W)
  val data: UInt = UInt(width.W)
  val last: Bool = Bool()
  val id: UInt = UInt(4.W)
}

class AXIBundleAW(width: Int) extends Bundle {
  val addr: UInt = UInt(width.W)
  val prot: UInt = UInt(3.W)
  val id: UInt = UInt(4.W)
  val user: UInt = UInt(4.W)
  val len: UInt = UInt(8.W)
  val size: UInt = UInt(3.W)
  val burst: UInt = UInt(2.W)
  val lock: UInt = UInt(1.W)
  val cache: UInt = UInt(4.W)
  val qos: UInt = UInt(3.W)
}

class AXIBundleW extends Bundle {
  val data: UInt = UInt(64.W)
  val strb: UInt = UInt(8.W)
  val last: Bool = Bool()
}

class AXIBundleB extends Bundle {
  val resp: UInt = UInt(2.W)
  val id: UInt = UInt(4.W)
}

class AXIBundle(width: Int) extends Bundle {
  val addrWidth = width / 2
  val ar = Irrevocable(new AXIBundleAR(addrWidth))
  val r = Flipped(Irrevocable(new AXIBundleR(width)))
  val aw = Irrevocable(new AXIBundleAW(addrWidth))
  val w = Irrevocable(new AXIBundleW)
  val b = Flipped(Irrevocable(new AXIBundleB))
}
