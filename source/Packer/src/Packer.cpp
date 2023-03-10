/* The copyright in this software is being made available under the BSD
 * License, included below. This software may be subject to other third party
 * and contributor rights, including patent rights, and no such rights are
 * granted under this license.
 *
 * Copyright (c) 2010-2022, ISO/IEC
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

#include <TMIV/Packer/Packer.h>

#include <TMIV/Common/LoggingStrategyFmt.h>
#include <TMIV/Packer/Retriever.h>

#include "MaxRectPiP.h"

#include <queue>
#include <stdexcept>

namespace TMIV::Packer {
namespace {
void checkAtlasSize(const Common::SizeVector &atlasSizes, const int32_t blockSize) {
  for (const auto &sz : atlasSizes) {
    if (((sz.x() % blockSize) != 0) || ((sz.y() % blockSize) != 0)) {
      throw std::runtime_error("Atlas size should be a multiple of blocksize");
    }
  }
}

void adaptPatchParamsToMask(MivBitstream::PatchParams &p, int32_t maskWidth, int32_t maskHeight) {
  if (p.atlasPatch3dOffsetU() + p.atlasPatch3dSizeU() > maskWidth) {
    if (p.atlasPatch3dSizeU() <= maskWidth) {
      p.atlasPatch3dOffsetU(maskWidth - p.atlasPatch3dSizeU());
    } else {
      p.atlasPatch3dOffsetU(0);
      p.atlasPatch3dSizeU(maskWidth);
    }
  }
  if (p.atlasPatch3dOffsetV() + p.atlasPatch3dSizeV() > maskHeight) {
    if (p.atlasPatch3dSizeV() <= maskHeight) {
      p.atlasPatch3dOffsetV(maskHeight - p.atlasPatch3dSizeV());
    } else {
      p.atlasPatch3dOffsetV(0);
      p.atlasPatch3dSizeV(maskHeight);
    }
  }
}

} // namespace
Packer::Packer(const Common::Json &rootNode, const Common::Json &componentNode) {
  m_minPatchSize = componentNode.require("minPatchSize").as<int32_t>();
  m_overlap = componentNode.require("overlap").as<int32_t>();
  m_pip = componentNode.require("enablePatchInPatch").as<bool>();
  m_enableMerging = componentNode.require("enableMerging").as<bool>();

  if (const auto &node = componentNode.optional("prioritizeSSI")) {
    m_prioritizeSSI = node.as<bool>();
  }

  Common::logVerbose("[VT prep] prioritizeSSI = {}", m_prioritizeSSI);

  switch (auto sortingMethod = componentNode.require("sortingMethod").as<int32_t>()) {
  case 0:
    m_sortingMethod = AREA_DESCENDING;
    break;
  case 1:
    m_sortingMethod = VIEW_ID_ASCENDING;
    break;
  default:
    throw std::runtime_error(fmt::format("Sorting method {} is not available", sortingMethod));
  }
  m_enableRecursiveSplit = componentNode.require("enableRecursiveSplit").as<bool>();

  if (const auto node = rootNode.optional("maxEntityId")) {
    m_maxEntityId = node.as<int32_t>();
  }
  if (m_maxEntityId > 0) {
    m_entityEncodeRange = rootNode.require("entityEncodeRange").asVec<int32_t, 2>();
  }
}

Packer::~Packer() = default;

void Packer::updateAggregatedEntityMasks(
    const std::vector<Common::FrameList<uint8_t>> &entityMasks) {
  for (const auto &entityMask : entityMasks) {
    m_aggregatedEntityMasks.push_back(entityMask);
  }
}

auto Packer::computeClusterToPack(const MivBitstream::ViewParamsList &viewParamsList,
                                  const int32_t m_blockSize, ClusterList &clusterList,
                                  const ClusteringMapList &clusteringMap) const {
  auto comp = [this, &viewParamsList](const Cluster &p1, const Cluster &p2) -> bool {
    if (viewParamsList[p1.getViewIdx()].isBasicView !=
        viewParamsList[p2.getViewIdx()].isBasicView) {
      return viewParamsList[p2.getViewIdx()].isBasicView;
    }

    // Give priority to the server-side-inpainted view
    if (m_prioritizeSSI && viewParamsList[p1.getViewIdx()].viewInpaintFlag !=
                               viewParamsList[p2.getViewIdx()].viewInpaintFlag) {
      return viewParamsList[p2.getViewIdx()].viewInpaintFlag;
    }

    // NOTE(FT): added for packing patches from MPI ==> reading in writePatchInAtlas is done in
    // increasing mpiLayerId order
    if (m_sortingMethod == AREA_DESCENDING) {
      if (p1.getArea() != p2.getArea()) {
        return p1.getArea() < p2.getArea();
      }
    } else if (m_sortingMethod == VIEW_ID_ASCENDING) {
      if (p1.getViewIdx() != p2.getViewIdx()) {
        return p1.getViewIdx() > p2.getViewIdx();
      }
    }
    // NOTE(BK): Stable ordering
    return p1.getClusterId() > p2.getClusterId();
  };

  std::priority_queue<Cluster, std::vector<Cluster>, decltype(comp)> clusterToPack(comp);

  std::vector<Cluster> out{};
  for (const auto &cluster : clusterList) {
    if (m_maxEntityId > 0 || cluster.isBasicView()) {
      out.push_back(cluster);
    } else {
      if (m_enableRecursiveSplit) {
        cluster.recursiveSplit(clusteringMap[cluster.getViewIdx()], out, m_blockSize,
                               m_minPatchSize);
      } else {
        out.push_back(cluster);
      }
    }
  }

  for (const auto &cluster : out) {
    // modification to align the imin,jmin to even values to help renderer
    Cluster c = Cluster::align(cluster, 2);
    clusterToPack.push(c);
  }
  return clusterToPack;
}

void Packer::initialize(const std::vector<Common::SizeVector> &atlasSizes,
                        const int32_t blockSize) {
  m_packerList.clear();
  m_packerList.resize(atlasSizes.size());
  for (size_t atlasIdx = 0; atlasIdx < atlasSizes.size(); atlasIdx++) {
    for (const auto &t : atlasSizes[atlasIdx]) {
      m_packerList[atlasIdx].emplace_back(t.x(), t.y(), blockSize, m_pip);
    }
  }
}

void Packer::initialize(std::vector<std::vector<MivBitstream::TilePartition>> tileSizes) {
  m_tileList.clear();
  m_tileList = tileSizes;
}

namespace {
auto patchParamsFor(size_t atlasIdx, const MivBitstream::ViewParamsList &viewParamsList,
                    const Cluster &cluster, Common::Vec2i tilePosition,
                    MaxRectPiP::Output &packerOutput, int32_t blockSize)
    -> MivBitstream::PatchParams {
  return MivBitstream::PatchParams{}
      .atlasId(MivBitstream::AtlasId{static_cast<uint8_t>(atlasIdx)})
      .atlasPatchProjectionId(viewParamsList[cluster.getViewIdx()].viewId)
      .atlasPatch2dPosX(packerOutput.x() + tilePosition[0])
      .atlasPatch2dPosY(packerOutput.y() + tilePosition[1])
      .atlasPatch3dOffsetU(cluster.jmin())
      .atlasPatch3dOffsetV(cluster.imin())
      .atlasPatchOrientationIndex(packerOutput.isRotated()
                                      ? MivBitstream::FlexiblePatchOrientation::FPO_ROT270
                                      : MivBitstream::FlexiblePatchOrientation::FPO_NULL)
      .atlasPatch3dSizeU(Common::align(cluster.width(), blockSize))
      .atlasPatch3dSizeV(Common::align(cluster.height(), blockSize));
}
} // namespace

auto Packer::pack(const std::vector<Common::SizeVector> &atlasSizes,
                  const Common::FrameList<uint8_t> &masks,
                  const MivBitstream::ViewParamsList &viewParamsList, const int32_t blockSize)
    -> MivBitstream::PatchParamsList {
  for (const auto &atlassize : atlasSizes) {
    checkAtlasSize(atlassize, blockSize);
  }
  auto [clusterList, clusteringMap, clusteringMapIndex] = computeClusters(masks, viewParamsList);

  auto clusterToPack = computeClusterToPack(viewParamsList, blockSize, clusterList, clusteringMap);

  // Packing
  MivBitstream::PatchParamsList atlasParamsVector{};
  MaxRectPiP::Output packerOutput{};

  int32_t patchIdx = 0;
  int32_t clusteringMap_viewId = 0;
  int32_t numTotalClusterPixels{};
  int32_t numKeptClusterPixels{};

  auto clusterToPackCopy = clusterToPack;

  while (!clusterToPackCopy.empty()) {
    const Cluster &cluster = clusterToPackCopy.top();
    numTotalClusterPixels = numTotalClusterPixels + cluster.getNumActivePixels();
    clusterToPackCopy.pop();
  }

  while (!clusterToPack.empty()) {
    const Cluster &cluster = clusterToPack.top();

    clusteringMap_viewId = getViewId(cluster, clusteringMapIndex);

    if (m_minPatchSize * m_minPatchSize <= cluster.getArea()) {
      bool packed = false;

      for (size_t atlasIdx = 0; atlasIdx < m_packerList.size(); ++atlasIdx) {
        for (size_t tileIdx = 0; tileIdx < m_packerList[atlasIdx].size(); ++tileIdx) {
          MaxRectPiP &packer = m_packerList[atlasIdx][tileIdx];

          packer.setIsPushInFreeSpace(false);

          if (packer.push(cluster, clusteringMap[clusteringMap_viewId], packerOutput)) {
            Common::Vec2i tilePosition = {m_tileList[atlasIdx][tileIdx].partitionPosX(),
                                          m_tileList[atlasIdx][tileIdx].partitionPosY()};
            auto p = patchParamsFor(atlasIdx, viewParamsList, cluster, tilePosition, packerOutput,
                                    blockSize);

            adaptPatchParamsToMask(p, masks[cluster.getViewIdx()].getWidth(),
                                   masks[cluster.getViewIdx()].getHeight());

            if (m_maxEntityId > 0) {
              p.atlasPatchEntityId(cluster.getEntityId());
              Common::logInfo(
                  "Packing patch {} of entity {} from view {} with #active pixels {} in atlas {}",
                  patchIdx, p.atlasPatchEntityId(), p.atlasPatchProjectionId(),
                  cluster.getNumActivePixels(), p.atlasId());
            }

            if (packer.getIsPushInFreeSpace()) {
              packer.setAreaPatchPushedInFreeSpace((p.atlasPatch2dSizeX() * p.atlasPatch2dSizeY()));
            }

            atlasParamsVector.push_back(p);
            patchIdx++;

            numKeptClusterPixels = numKeptClusterPixels + cluster.getNumActivePixels();

            packed = true;
            break;
          }
        }

        if (packed) {
          break;
        }
      }

      if (!packed) {
        ifEntityOrBasic(cluster);
        auto cc = cluster.split(clusteringMap[clusteringMap_viewId], m_overlap);

        const auto alignPatch = [&, &clusteringMapIndex = clusteringMapIndex](auto member) {
          if (m_minPatchSize * m_minPatchSize <= (cc.*member).getArea()) {
            // modification to align the imin,jmin to even values to help renderer
            Cluster c = Cluster::align((cc.*member), 2);
            clusterToPack.push(c);
            clusteringMapIndex.push_back(clusteringMap_viewId);
          }
        };

        alignPatch(&decltype(cc)::first);
        alignPatch(&decltype(cc)::second);
      }
    }

    clusterToPack.pop();
  }

  for (size_t atlasIdx = 0; atlasIdx < m_packerList.size(); ++atlasIdx) {
    for (size_t tileIdx = 0; tileIdx < m_packerList[atlasIdx].size(); ++tileIdx) {
      Common::logVerbose(
          "Ratio of patch area in atlas {} tile {}: {:.4f}%", atlasIdx, tileIdx,
          ((static_cast<double>(m_packerList[atlasIdx][tileIdx].getAreaPatchPushedInFreeSpace()) /
            (atlasSizes[atlasIdx][tileIdx][0] * atlasSizes[atlasIdx][tileIdx][1])) *
           100));
      m_packerList[atlasIdx][tileIdx].setAreaPatchPushedInFreeSpace(0);
    }
  }

  Common::logVerbose(
      "Ratio of discarded pixels of cluster: {:.4f}%",
      (static_cast<double>(numTotalClusterPixels - numKeptClusterPixels) / numTotalClusterPixels) *
          100);

  return atlasParamsVector;
}

auto Packer::getViewId(const Cluster &cluster, const std::vector<int32_t> &clusteringMapIndex) const
    -> int32_t {
  if (m_maxEntityId > 0) {
    return clusteringMapIndex[cluster.getClusterId()];
  }
  return cluster.getViewIdx();
}

void Packer::ifEntityOrBasic(const Cluster &cluster) const {
  if (m_maxEntityId > 0) {
    Common::logInfo("Spliting cluster {}", cluster.getClusterId());
  }
  if (cluster.isBasicView()) {
    throw std::runtime_error("Failed to pack basic view");
  }
}

auto Packer::computeClusters(const Common::FrameList<uint8_t> &masks,
                             const MivBitstream::ViewParamsList &viewParamsList)
    -> std::tuple<ClusterList, ClusteringMapList, std::vector<int32_t>> {
  ClusterList clusterList{};
  ClusteringMapList clusteringMap{};
  std::vector<int32_t> clusteringMapIndex{};
  int32_t index = 0;
  for (auto viewIdx = 0; viewIdx < static_cast<int32_t>(masks.size()); viewIdx++) {
    if (m_maxEntityId > 0) {
      for (int32_t entityId = m_entityEncodeRange[0]; entityId < m_entityEncodeRange[1];
           entityId++) {
        // Entity clustering
        Common::Frame<uint8_t> mask =
            m_aggregatedEntityMasks[entityId - m_entityEncodeRange[0]][viewIdx];

        auto clusteringOutput = retrieveClusters(
            viewIdx, mask, static_cast<int32_t>(clusterList.size()),
            viewParamsList[viewIdx].isBasicView, m_enableMerging, m_maxEntityId > 0);

        for (auto &cluster : clusteringOutput.first) {
          cluster = Cluster::setEntityId(cluster, entityId);
        }

        std::move(clusteringOutput.first.begin(), clusteringOutput.first.end(),
                  back_inserter(clusterList));
        clusteringMap.push_back(std::move(clusteringOutput.second));

        for (size_t i = 0; i < clusteringOutput.first.size(); i++) {
          clusteringMapIndex.push_back(index);
        }

        if (!clusteringOutput.first.empty()) {
          Common::logVerbose("entity {} from view {} results in {} patches", entityId, viewIdx,
                             clusteringOutput.first.size());
        }
        ++index;
      }
    } else {
      auto clusteringOutput =
          retrieveClusters(viewIdx, masks[viewIdx], static_cast<int32_t>(clusterList.size()),
                           viewParamsList[viewIdx].isBasicView, m_enableMerging, m_maxEntityId > 0);

      std::move(clusteringOutput.first.begin(), clusteringOutput.first.end(),
                back_inserter(clusterList));
      clusteringMap.push_back(std::move(clusteringOutput.second));
    }
  }
  if (m_maxEntityId > 0) {
    Common::logInfo("clusteringMap size = {} with total # clusters = {}", clusteringMap.size(),
                    clusteringMapIndex.size());
  }
  return {clusterList, clusteringMap, clusteringMapIndex};
}
} // namespace TMIV::Packer
