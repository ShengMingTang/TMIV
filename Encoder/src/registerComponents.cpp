/* The copyright in this software is being made available under the BSD
 * License, included below. This software may be subject to other third party
 * and contributor rights, including patent rights, and no such rights are
 * granted under this license.
 *
 * Copyright (c) 2010-2020, ISO/IEC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *  * Neither the name of the ISO/IEC nor the names of its contributors may
 *    be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <TMIV/Aggregator/Aggregator.h>
#include <TMIV/Common/Factory.h>
#include <TMIV/DepthQualityAssessor/DepthQualityAssessor.h>
#include <TMIV/Encoder/Encoder.h>
#include <TMIV/Encoder/GroupBasedEncoder.h>
#include <TMIV/GeometryQuantizer/GeometryQuantizer.h>
#include <TMIV/Packer/Packer.h>
#include <TMIV/Pruner/HierarchicalPruner.h>
#include <TMIV/ViewOptimizer/BasicViewAllocator.h>
#include <TMIV/ViewOptimizer/NoViewOptimizer.h>

namespace TMIV::Encoder {
void registerComponents() {
  using Common::Factory;

  auto &aggregators = Factory<Aggregator::IAggregator>::getInstance();
  aggregators.registerAs<Aggregator::Aggregator>("Aggregator");

  auto &assesors = Factory<DepthQualityAssessor::IDepthQualityAssessor>::getInstance();
  assesors.registerAs<DepthQualityAssessor::DepthQualityAssessor>("DepthQualityAssessor");

  auto &encoders = Factory<IEncoder>::getInstance();
  encoders.registerAs<Encoder>("Encoder");
  encoders.registerAs<GroupBasedEncoder>("GroupBasedEncoder");

  auto &geometryQuantizers = Factory<GeometryQuantizer::IGeometryQuantizer>::getInstance();
  geometryQuantizers.registerAs<GeometryQuantizer::GeometryQuantizer>("GeometryQuantizer");

  auto &packers = Factory<Packer::IPacker>::getInstance();
  packers.registerAs<Packer::Packer>("Packer");

  auto &pruners = Factory<Pruner::IPruner>::getInstance();
  pruners.registerAs<Pruner::HierarchicalPruner>("HierarchicalPruner");

  auto &viewOptimizers = Factory<ViewOptimizer::IViewOptimizer>::getInstance();
  viewOptimizers.registerAs<ViewOptimizer::BasicViewAllocator>("BasicViewAllocator");
  viewOptimizers.registerAs<ViewOptimizer::NoViewOptimizer>("NoViewOptimizer");
}
} // namespace TMIV::Encoder
