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

#include <TMIV/GeometryQuantizer/ExplicitOccupancy.h>

#include <TMIV/MivBitstream/DepthOccupancyTransform.h>

#include <iostream>
#include <stdexcept>

using namespace std;
using namespace TMIV::Common;
using namespace TMIV::MivBitstream;

namespace TMIV::GeometryQuantizer {
ExplicitOccupancy::ExplicitOccupancy(const Json & /*unused*/, const Json &componentNode) {
  if (auto subnode = componentNode.optional("occupancyScale")) {
    m_occupancyScaleConfig = true;
    m_occupancyScale = subnode.asIntVector<2>();
  } else {
    m_occupancyScaleConfig = false;
  }
}

auto ExplicitOccupancy::setOccupancyParams(MivBitstream::EncoderParams params)
    -> const MivBitstream::EncoderParams & {
  m_inParams = move(params);
  m_outParams = m_inParams;

  m_outParams.vme().vme_embedded_occupancy_flag(false);
  m_outParams.vme().vme_occupancy_scale_enabled_flag(true);
  for (uint8_t i = 0; i <= m_outParams.vps.vps_atlas_count_minus1(); i++) {
    m_outParams.vps.vps_occupancy_video_present_flag(i, true);
  }
  m_depthLowQualityFlag = m_outParams.vme().vme_depth_low_quality_flag();

  m_embeddedOccupancyFlag = m_outParams.vme().vme_embedded_occupancy_flag();
  m_occupancyScaleEnabledFlag = m_outParams.vme().vme_occupancy_scale_enabled_flag();

  if (!m_embeddedOccupancyFlag && m_occupancyScaleEnabledFlag) {
    for (auto &atlas : m_outParams.atlas) {
      if (m_occupancyScaleConfig) {
        atlas.asme().asme_occupancy_scale_factor_x_minus1(m_occupancyScale[0] - 1);
        atlas.asme().asme_occupancy_scale_factor_y_minus1(m_occupancyScale[1] - 1);
      } else {
        atlas.asme().asme_occupancy_scale_factor_x_minus1(
            (1 << atlas.asps.asps_log2_patch_packing_block_size()) - 1);
        atlas.asme().asme_occupancy_scale_factor_y_minus1(
            (1 << atlas.asps.asps_log2_patch_packing_block_size()) - 1);
      }
    }
  }
  return m_outParams;
}

auto ExplicitOccupancy::transformParams(MivBitstream::EncoderParams params)
    -> const MivBitstream::EncoderParams & {
  m_inParams = move(params);
  m_outParams = m_inParams;

  return m_outParams;
}

auto ExplicitOccupancy::transformAtlases(const Common::MVD16Frame &inAtlases)
    -> Common::MVD10Frame {
  auto outAtlases = MVD10Frame{};
  outAtlases.reserve(inAtlases.size());

  for (const auto &inAtlas : inAtlases) {
    outAtlases.emplace_back(inAtlas.texture,
                            Depth10Frame{inAtlas.depth.getWidth(), inAtlas.depth.getHeight()},
                            inAtlas.occupancy);
  }

  for (const auto &patch : m_outParams.patchParamsList) {
    const auto &inViewParams = m_inParams.viewParamsList[patch.pduViewIdx()];
    const auto &outViewParams = m_outParams.viewParamsList[patch.pduViewIdx()];
    const auto inOccupancyTransform = OccupancyTransform{inViewParams};
    const auto inDepthTransform = DepthTransform<16>{inViewParams.dq};
    const auto outDepthTransform = DepthTransform<10>{outViewParams.dq, patch};

    for (auto i = 0; i < patch.pdu2dSize().y(); ++i) {
      for (auto j = 0; j < patch.pdu2dSize().x(); ++j) {
        const auto n = i + patch.pdu2dPos().y();
        const auto m = j + patch.pdu2dPos().x();

        const auto &plane = inAtlases[patch.vuhAtlasId].depth.getPlane(0);

        if (n < 0 || n >= int(plane.height()) || m < 0 || m >= int(plane.width())) {
          abort();
        }

        const auto inLevel = plane(n, m);

        if (inOccupancyTransform.occupant(inLevel)) {
          const auto normDisp = inDepthTransform.expandNormDisp(inLevel);
          const auto outLevel = outDepthTransform.quantizeNormDisp(normDisp, 0);
          outAtlases[patch.vuhAtlasId].depth.getPlane(0)(n, m) = outLevel;
        }
      }
    }
  }

  // Apply depth padding
  if (!m_depthLowQualityFlag) {
    padGeometryFromLeft(outAtlases);
  }

  return outAtlases;
}

void ExplicitOccupancy::padGeometryFromLeft(MVD10Frame &atlases) {
  for (size_t i = 0; i < atlases.size(); ++i) {
    if (m_outParams.vps.vps_occupancy_video_present_flag(uint8_t(i))) {
      auto &depthAtlasMap = atlases[i].depth;
      auto depthScale =
          std::array{m_outParams.atlas[i].asps.asps_frame_height() / depthAtlasMap.getHeight(),
                     m_outParams.atlas[i].asps.asps_frame_width() / depthAtlasMap.getWidth()};
      const auto &occupancyAtlasMap = atlases[i].occupancy;
      auto occupancyScale =
          std::array{m_outParams.atlas[i].asps.asps_frame_height() / occupancyAtlasMap.getHeight(),
                     m_outParams.atlas[i].asps.asps_frame_width() / occupancyAtlasMap.getWidth()};
      int yOcc = 0, xOcc = 0;
      for (int y = 0; y < depthAtlasMap.getHeight(); y++) {
        for (int x = 1; x < depthAtlasMap.getWidth(); x++) {
          auto depth = depthAtlasMap.getPlane(0)(y, x);
          yOcc = y * depthScale[0] / occupancyScale[0];
          xOcc = x * depthScale[1] / occupancyScale[1];
          if (occupancyAtlasMap.getPlane(0)(yOcc, xOcc) == 0 ||
              (depth == 0 &&
               atlases[i].texture.getPlane(0)(y * depthScale[0], x * depthScale[1]) == 512)) {
            depthAtlasMap.getPlane(0)(y, x) = depthAtlasMap.getPlane(0)(y, x - 1);
          }
        }
      }
    }
  }
}
} // namespace TMIV::GeometryQuantizer