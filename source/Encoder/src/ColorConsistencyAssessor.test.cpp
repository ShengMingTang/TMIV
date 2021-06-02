﻿/* The copyright in this software is being made available under the BSD
 * License, included below. This software may be subject to other third party
 * and contributor rights, including patent rights, and no such rights are
 * granted under this license.
 *
 * Copyright (c) 2010-2021, ISO/IEC
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

#include <TMIV/Common/Frame.h>
#include <TMIV/Encoder/Encoder.h>
#include <TMIV/Renderer/Rasterizer.h>
#include <catch2/catch.hpp>

namespace TMIV::Encoder {
auto calculateAvgColorDifference(Common::Mat<Common::Vec3i> colorCorrectionMap) -> Common::Vec3i {
  Common::Vec3i avg({0, 0, 0});
  int cnt = 0;
  for (size_t h = 0; h < colorCorrectionMap.height(); h++) {
    for (size_t w = 0; w < colorCorrectionMap.width(); w++) {
      for (int i = 0; i < 3; i++) {
        avg[i] += colorCorrectionMap(h, w)[i];
      }
      cnt++;
    }
  }
  for (int i = 0; i < 3; i++) {
    avg[i] /= cnt;
  }
  return avg;
}

SCENARIO("Color consistency assessment") {
  const int numOfCams = 2;
  const int W = 10;
  const int H = 2;
  auto viewParamsList =
      MivBitstream::ViewParamsList{std::vector<MivBitstream::ViewParams>(numOfCams)};
  for (int c = 0; c < numOfCams; c++) {
    viewParamsList[c].pose.position = Common::Vec3f{0.0F, 0.0F, 0.0F};
    viewParamsList[c].pose.orientation = Common::neutralOrientationF;
    viewParamsList[c].ci.ci_cam_type(MivBitstream::CiCamType::perspective);
    viewParamsList[c].ci.ci_perspective_center_hor(float(W) / 2.0F);
    viewParamsList[c].ci.ci_perspective_center_ver(float(H) / 2.0F);
    viewParamsList[c].ci.ci_perspective_focal_hor(1.0F);
    viewParamsList[c].ci.ci_perspective_focal_ver(1.0F);
    viewParamsList[c].ci.ci_projection_plane_height_minus1(H - 1);
    viewParamsList[c].ci.ci_projection_plane_width_minus1(W - 1);
    viewParamsList[c].dq.dq_norm_disp_high(100.0F);
    viewParamsList[c].dq.dq_norm_disp_low(1.0F);
  }

  auto views = Common::MVD16Frame(numOfCams);

  GIVEN("Equal colors") {
    for (int c = 0; c < numOfCams; c++) {
      views[c].texture.resize(W, H);
      views[c].depth.resize(W, H);
      views[c].texture.fillNeutral();
      views[c].depth.fillOne();
    }
    WHEN("Assessing color consistency") {
      auto colorCorrectionMaps = assessColorConsistency(views, viewParamsList);
      int maxDiff = 0;
      for (int c = 0; c < numOfCams; c++) {
        Common::Vec3i avg = calculateAvgColorDifference(colorCorrectionMaps[c]);
        for (int i = 0; i < 3; i++) {
          maxDiff = std::max(maxDiff, std::abs(avg[i]));
        }
      }
      THEN("Colors are identical") { REQUIRE(maxDiff == 0); }
    }
  }
  GIVEN("Color difference") {
    for (int c = 0; c < numOfCams; c++) {
      views[c].texture.resize(W, H);
      views[c].depth.resize(W, H);
      views[c].texture.fillNeutral();
      views[c].depth.fillOne();
    }
    for (int h = 0; h < H; h++) {
      for (int w = 0; w < W; w++) {
        views[0].texture.getPlane(0)(h, w) += static_cast<uint16_t>(w * 10);
      }
    }
    WHEN("Assessing color consistency") {
      auto colorCorrectionMaps = assessColorConsistency(views, viewParamsList);
      int maxDiff = 0;
      for (int c = 0; c < numOfCams; c++) {
        Common::Vec3i avg = calculateAvgColorDifference(colorCorrectionMaps[c]);
        for (int i = 0; i < 3; i++) {
          maxDiff = std::max(maxDiff, std::abs(avg[i]));
        }
      }
      std::cout << maxDiff << "okok";
      THEN("Colors are inconsistent") { REQUIRE(maxDiff == 9); }
    }
  }
}
} // namespace TMIV::Encoder