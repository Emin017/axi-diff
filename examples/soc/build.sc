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

import mill._
import scalalib._
import $file.`rocket-chip`.common
import $file.`rocket-chip`.cde.common
import $file.`rocket-chip`.hardfloat.build

val chiselVersion = "7.0.0-M1"
val defaultScalaVersion = "2.13.12"

trait HasChisel extends SbtModule {

  def chiselModule: Option[ScalaModule] = None

  def chiselPluginJar: T[Option[PathRef]] = None

  def chiselIvy: Option[Dep] = Some(ivy"org.chipsalliance::chisel:${chiselVersion}")

  def chiselPluginIvy: Option[Dep] = Some(ivy"org.chipsalliance:::chisel-plugin:${chiselVersion}")

  override def scalaVersion = defaultScalaVersion

  override def scalacOptions = super.scalacOptions() ++
    Agg("-language:reflectiveCalls", "-Ymacro-annotations", "-Ytasty-reader")

  override def ivyDeps = super.ivyDeps() ++ Agg(chiselIvy.get)

  override def scalacPluginIvyDeps = super.scalacPluginIvyDeps() ++ Agg(chiselPluginIvy.get)
}

object rocketchip extends RocketChip

trait RocketChip extends millbuild.`rocket-chip`.common.RocketChipModule with HasChisel {
  def scalaVersion: T[String] = T(defaultScalaVersion)

  override def millSourcePath = os.pwd / "rocket-chip"

  def dependencyPath = millSourcePath

  def macrosModule = macros

  def hardfloatModule = hardfloat

  def cdeModule = cde

  def mainargsIvy = ivy"com.lihaoyi::mainargs:0.5.4"

  def json4sJacksonIvy = ivy"org.json4s::json4s-jackson:4.0.6"

  object macros extends Macros
  trait Macros extends millbuild.`rocket-chip`.common.MacrosModule with SbtModule {
    def scalaVersion: T[String] = T(defaultScalaVersion)

    def scalaReflectIvy = ivy"org.scala-lang:scala-reflect:${defaultScalaVersion}"
  }

  object hardfloat extends Hardfloat
  trait Hardfloat extends millbuild.`rocket-chip`.hardfloat.common.HardfloatModule with HasChisel {
    def scalaVersion: T[String] = T(defaultScalaVersion)

    override def millSourcePath = dependencyPath / "hardfloat" / "hardfloat"
  }

  object cde extends CDE
  trait CDE extends millbuild.`rocket-chip`.cde.common.CDEModule with ScalaModule {
    def scalaVersion: T[String] = T(defaultScalaVersion)

    override def millSourcePath = dependencyPath / "cde" / "cde"
  }
}

trait axiDiffModule extends ScalaModule {
  def rocketModule: ScalaModule

  override def moduleDeps = super.moduleDeps ++ Seq(
    rocketModule,
  )
}

object axidiff extends axiDiff
trait axiDiff extends axiDiffModule with HasChisel {

  override def millSourcePath = os.pwd

  def rocketModule = rocketchip

  override def sources = T.sources {
    super.sources() ++ Seq(PathRef(millSourcePath / "src"))
  }
}
