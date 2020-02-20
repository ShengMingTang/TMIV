/* The copyright in this software is being made available under the BSD
 * License, included below. This software may be subject to other third party
 * and contributor rights, including patent rights, and no such rights are
 * granted under this license.
 *
 * Copyright (c) 2010-2019, ISO/IEC
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

#include <TMIV/AtlasDeconstructor/AtlasDeconstructor.h>

#include <TMIV/Common/Factory.h>
#include <TMIV/MivBitstream/DepthOccupancyTransform.h>
#include <cassert>
#include <iostream>

using namespace std;
using namespace TMIV::Common;
using namespace TMIV::MivBitstream;

namespace TMIV::AtlasDeconstructor {

AtlasDeconstructor::AtlasDeconstructor(const Json &rootNode, const Json &componentNode) {
  if (auto subnode = componentNode.optional("EntityDecodeRange")) {
    m_entityDecodeRange = subnode.asIntVector<2>();
  }

  if (auto node = rootNode.optional("depthDownScaleFlag"); node) {
    m_downscale_depth = node.asBool();
  }
}

auto AtlasDeconstructor::getPatchIdMap(const IvSequenceParams &ivSequenceParams,
                                       const IvAccessUnitParams &ivAccessUnitParams,
                                       const MVD10Frame &frame) -> PatchIdMapList {
  PatchIdMapList patchMapList;
  const auto &viewParamsList = ivSequenceParams.viewParamsList;
  const auto &patchParamsList = ivAccessUnitParams.patchParamsList;

  for (const auto &sz : ivAccessUnitParams.atlasSizes()) {
    auto w = m_downscale_depth ? sz.x() / 2 : sz.x();
    auto h = m_downscale_depth ? sz.y() / 2 : sz.y();

    PatchIdMap patchMap(w, h);
    fill(patchMap.getPlane(0).begin(), patchMap.getPlane(0).end(), unusedPatchId);
    patchMapList.push_back(move(patchMap));
  }

  if (ivSequenceParams.msp().msp_max_entities_minus1() > 0) {
    cout << "Entity-Based Atlas Deconstructor is applied for EntityDecodeRange [ "
         << m_entityDecodeRange[0] << ", " << m_entityDecodeRange[1] << ")\n";
  }

  for (size_t id = 0U; id < patchParamsList.size(); ++id) {
    assert(patchParamsList[id].pduViewId() < viewParamsList.size());
    if (ivSequenceParams.msp().msp_max_entities_minus1() == 0 ||
        (patchParamsList[id].pduEntityId() >= m_entityDecodeRange[0] &&
         patchParamsList[id].pduEntityId() < m_entityDecodeRange[1])) {
      writePatchIdInMap(patchParamsList[id], patchMapList, static_cast<uint16_t>(id), frame,
                        viewParamsList);
    }
  }

  return patchMapList;
}

void AtlasDeconstructor::writePatchIdInMap(const PatchParams &patch, PatchIdMapList &patchMapList,
                                           uint16_t patchId, const MVD10Frame &frame,
                                           const ViewParamsList &viewParamsList) {
  auto &patchMap = patchMapList[patch.vuhAtlasId];
  auto &depthMap = frame[patch.vuhAtlasId].second.getPlane(0);

  const Vec2i &q0 = patch.pdu2dPos();
  const auto sizeInAtlas = patch.pdu2dSize();
  int xMin = q0.x();
  int xLast = q0.x() + sizeInAtlas.x();
  int yMin = q0.y();
  int yLast = q0.y() + sizeInAtlas.y();

  const auto occupancyTransform = OccupancyTransform{viewParamsList[patch.pduViewId()], patch};

  if (m_downscale_depth) {
    yMin /= 2;
    yLast /= 2;
    xMin /= 2;
    xLast /= 2;
  }

  for (auto y = yMin; y < yLast; y++) {
    for (auto x = xMin; x < xLast; x++) {
      if (occupancyTransform.occupant(depthMap(y, x))) {
        patchMap.getPlane(0)(y, x) = patchId;
      }
    }
  }
}

auto AtlasDeconstructor::recoverPrunedView(const MVD10Frame &atlas,
                                           const ViewParamsList &viewParamsList,
                                           const PatchParamsList &patchParamsVector) -> MVD10Frame {
  // Initialization
  MVD10Frame frame;

  for (const auto &cam : viewParamsList) {
    TextureFrame tex(cam.ci.projectionPlaneSize().x(), cam.ci.projectionPlaneSize().y());
    Depth10Frame depth(cam.ci.projectionPlaneSize().x(), cam.ci.projectionPlaneSize().y());
    tex.fillNeutral();
    frame.push_back(TextureDepth10Frame{move(tex), move(depth)});
  }

  // Process patches
  MVD10Frame atlas_pruned = atlas;

  for (auto iter = patchParamsVector.rbegin(); iter != patchParamsVector.rend(); ++iter) {
    const auto &patch = *iter;
    const auto occupancyTransform = OccupancyTransform{viewParamsList[patch.pduViewId()], patch};

    auto &currentAtlas = atlas_pruned[patch.vuhAtlasId];
    auto &currentView = frame[patch.pduViewId()];

    auto &textureAtlasMap = currentAtlas.first;
    auto &depthAtlasMap = currentAtlas.second;

    auto &textureViewMap = currentView.first;
    auto &depthViewMap = currentView.second;

    const auto sizeInAtlas = patch.pdu2dSize();
    int wP = sizeInAtlas.x();
    int hP = sizeInAtlas.y();
    int xP = patch.pdu2dPos().x();
    int yP = patch.pdu2dPos().y();

    for (int dy = 0; dy < hP; dy++) {
      for (int dx = 0; dx < wP; dx++) {
        // get position
        Vec2i pAtlas = {xP + dx, yP + dy};
        Vec2i pView = atlasToView(pAtlas, patch);
        // Y
        if (occupancyTransform.occupant(depthAtlasMap.getPlane(0)(pAtlas.y(), pAtlas.x()))) {
          textureViewMap.getPlane(0)(pView.y(), pView.x()) =
              textureAtlasMap.getPlane(0)(pAtlas.y(), pAtlas.x());
          textureAtlasMap.getPlane(0)(pAtlas.y(), pAtlas.x()) = 0;
        }
        // UV
        if ((pView.x() % 2) == 0 && (pView.y() % 2) == 0) {
          for (int p = 1; p < 3; p++) {
            if (occupancyTransform.occupant(depthAtlasMap.getPlane(0)(pAtlas.y(), pAtlas.x()))) {
              textureViewMap.getPlane(p)(pView.y() / 2, pView.x() / 2) =
                  textureAtlasMap.getPlane(p)(pAtlas.y() / 2, pAtlas.x() / 2);
              textureAtlasMap.getPlane(p)(pAtlas.y() / 2, pAtlas.x() / 2) = 0x200;
            }
          }
        }
        // Depth
        if (occupancyTransform.occupant(depthAtlasMap.getPlane(0)(pAtlas.y(), pAtlas.x()))) {
          depthViewMap.getPlane(0)(pView.y(), pView.x()) =
              depthAtlasMap.getPlane(0)(pAtlas.y(), pAtlas.x());
          depthAtlasMap.getPlane(0)(pAtlas.y(), pAtlas.x()) = 0;
        }
      }
    }
  }

  return frame;
}
} // namespace TMIV::AtlasDeconstructor
