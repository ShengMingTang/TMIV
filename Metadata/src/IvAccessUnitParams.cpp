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

#include <TMIV/Metadata/IvAccessUnitParams.h>

#include <TMIV/Metadata/Bitstream.h>
#include <TMIV/Metadata/IvSequenceParams.h>

#include <algorithm>
#include <cassert>
#include <iostream>
#include <map>

#include "verify.h"

using namespace std;
using namespace TMIV::Common;

namespace TMIV::Metadata {
bool AtlasParameters::isRotated() const {
  return rotation == PatchRotation::rot90 || rotation == PatchRotation::swap ||
         rotation == PatchRotation::rot270 || rotation == PatchRotation::mrot90;
}

auto AtlasParameters::patchSizeInAtlas() const -> Vec2i {
  if (isRotated()) {
    return {patchSizeInView.y(), patchSizeInView.x()};
  }
  return patchSizeInView;
}

bool AtlasParameters::operator==(const AtlasParameters &other) const {
  return atlasId == other.atlasId && viewId == other.viewId &&
         patchSizeInView == other.patchSizeInView && posInView == other.posInView &&
         posInAtlas == other.posInAtlas && rotation == other.rotation;
}

auto AtlasParamsList::decodeFrom(InputBitstream &bitstream, const ViewParamsList &cameras)
    -> AtlasParamsList {
  auto atlasParamsList = AtlasParamsList{};
  auto numAtlases = bitstream.getUExpGolomb() + 1;
  atlasParamsList.omafV1CompatibleFlag = bitstream.getFlag();

  while (numAtlases-- > 0) {
    AtlasParameters patch;
    patch.atlasId = bitstream.getUint8();
    auto numPatches = bitstream.getUint16() + 1;

    // It is not guaranteed that atlas ID's are consecutive
    while (patch.atlasId >= atlasParamsList.atlasSizes.size()) {
      atlasParamsList.atlasSizes.emplace_back();
    }
    atlasParamsList.atlasSizes[patch.atlasId].x() = bitstream.getUint16();
    atlasParamsList.atlasSizes[patch.atlasId].y() = bitstream.getUint16();
    const auto atlasSize = atlasParamsList.atlasSizes[patch.atlasId];

    while (numPatches-- > 0) {
      patch.viewId = uint16_t(bitstream.getUVar(cameras.size()));
      const auto viewSize = cameras[patch.viewId].size;

      patch.patchSizeInView.x() = int(bitstream.getUVar(viewSize.x()) + 1);
      patch.patchSizeInView.y() = int(bitstream.getUVar(viewSize.y()) + 1);
      patch.posInAtlas.x() = int(bitstream.getUVar(atlasSize.x()));
      patch.posInAtlas.y() = int(bitstream.getUVar(atlasSize.y()));
      patch.posInView.x() = int(bitstream.getUVar(viewSize.x()));
      patch.posInView.y() = int(bitstream.getUVar(viewSize.y()));
      patch.rotation = PatchRotation(bitstream.readBits(3));

      verify(patch.posInView.x() + patch.patchSizeInView.x() <= viewSize.x());
      verify(patch.posInView.y() + patch.patchSizeInView.y() <= viewSize.y());
      verify(patch.posInAtlas.x() + patch.patchSizeInAtlas().x() <= atlasSize.x());
      verify(patch.posInAtlas.y() + patch.patchSizeInAtlas().y() <= atlasSize.y());

      atlasParamsList.push_back(patch);
    }
  }
  return atlasParamsList;
}

void AtlasParamsList::encodeTo(OutputBitstream &bitstream, const ViewParamsList &cameras) const {
  // Count patches per atlas ID
  auto atlasIds = map<uint8_t, uint_least16_t>{};
  for (const auto &patch : *this) {
    ++atlasIds.insert({patch.atlasId, 0}).first->second;
  }

  assert(!atlasIds.empty());
  bitstream.putUExpGolomb(atlasIds.size() - 1);
  bitstream.putFlag(omafV1CompatibleFlag);

  for (auto [atlasId, numPatches] : atlasIds) {
    assert(numPatches > 0 && numPatches - 1 <= UINT16_MAX);
    assert(atlasId < atlasSizes.size());
    const auto atlasSize = atlasSizes[atlasId];

    bitstream.putUint8(atlasId);
    bitstream.putUint16(uint16_t(numPatches - 1));
    bitstream.putUint16(atlasSize.x());
    bitstream.putUint16(atlasSize.y());

    for (const auto &patch : *this) {
      if (patch.atlasId == atlasId) {
        const auto viewSize = cameras[patch.viewId].size;

#ifndef NDEBUG
        assert(0 < patch.patchSizeInView.x() && 0 < patch.patchSizeInView.y());
        assert(0 <= patch.posInView.x() && 0 <= patch.posInView.y());
        assert(patch.posInView.x() + patch.patchSizeInView.x() <= viewSize.x());
        assert(patch.posInView.y() + patch.patchSizeInView.y() <= viewSize.y());
        assert(patch.posInAtlas.x() + patch.patchSizeInAtlas().x() <= atlasSize.x());
        assert(patch.posInAtlas.y() + patch.patchSizeInAtlas().y() <= atlasSize.y());
#endif

        bitstream.putUVar(patch.viewId, cameras.size());
        bitstream.putUVar(patch.patchSizeInView.x() - 1, viewSize.x());
        bitstream.putUVar(patch.patchSizeInView.y() - 1, viewSize.y());
        bitstream.putUVar(patch.posInAtlas.x(), atlasSize.x());
        bitstream.putUVar(patch.posInAtlas.y(), atlasSize.y());
        bitstream.putUVar(patch.posInView.x(), viewSize.x());
        bitstream.putUVar(patch.posInView.y(), viewSize.y());
        bitstream.writeBits(unsigned(patch.rotation), 3);
      }
    }
  }
}

Vec2i viewToAtlas(Vec2i viewPosition, const AtlasParameters &patch) {
  int w = patch.patchSizeInView.x();
  int h = patch.patchSizeInView.y();
  int xM = patch.posInView.x();
  int yM = patch.posInView.y();
  int xP = patch.posInAtlas.x();
  int yP = patch.posInAtlas.y();
  int x = viewPosition.x();
  int y = viewPosition.y();

  switch (patch.rotation) {
  case PatchRotation::none: // (x, y)
    return {x - xM + xP, y - yM + yP};
  case PatchRotation::swap: // (y, x)
    return {y - yM + xP, x - xM + yP};
  case PatchRotation::rot90: // (-y, x)
    return {-y + yM + xP + h - 1, x - xM + yP};
  case PatchRotation::rot180: // (-x, -y)
    return {-x + xM + xP + w - 1, -y + yM + yP + h - 1};
  case PatchRotation::rot270: // (y, -x)
    return {y - yM + xP, -x + xM + yP + w - 1};
  case PatchRotation::mirror: // (-x, y)
    return {-x + xM + xP + w - 1, y - yM + yP};
  case PatchRotation::mrot90: // (-y, -x)
    return {-y + yM + xP + h - 1, -x + xM + yP + w - 1};
  case PatchRotation::mrot180: // (x, -y)
    return {x - xM + xP, -y + yM + yP + h - 1};
  default:
    abort();
  }
}

Vec2i atlasToView(Vec2i atlasPosition, const AtlasParameters &patch) {
  int w = patch.patchSizeInView.x();
  int h = patch.patchSizeInView.y();
  int xM = patch.posInView.x();
  int yM = patch.posInView.y();
  int xP = patch.posInAtlas.x();
  int yP = patch.posInAtlas.y();
  int x = atlasPosition.x();
  int y = atlasPosition.y();

  switch (patch.rotation) {
  case PatchRotation::none: // (x, y)
    return {x - xP + xM, y - yP + yM};
  case PatchRotation::swap: // (y, x)
    return {y - yP + xM, x - xP + yM};
  case PatchRotation::rot90: // (y, -x)
    return {y - yP + xM, -x + xP + yM + h - 1};
  case PatchRotation::rot180: // (-x, -y)
    return {-x + xP + xM + w - 1, -y + yP + yM + h - 1};
  case PatchRotation::rot270: // (-y, x)
    return {-y + yP + xM + w - 1, x - xP + yM};
  case PatchRotation::mirror: // (-x, y)
    return {-x + xP + xM + w - 1, y - yP + yM};
  case PatchRotation::mrot90: // (-y, -x)
    return {-y + yP + xM + w - 1, -x + xP + yM + h - 1};
  case PatchRotation::mrot180: // (x, -y)
    return {x - xP + xM, -y + yP + yM + h - 1};
  default:
    abort();
  }
}

bool IvAccessUnitParams::operator==(const IvAccessUnitParams &other) const {
  return atlasParamsList == other.atlasParamsList;
}

auto IvAccessUnitParams::decodeFrom(InputBitstream &bitstream, const ViewParamsList &cameras)
    -> IvAccessUnitParams {
  auto ivsAccessUnitParams = IvAccessUnitParams{};
  const auto atlasParamsPresentFlag = bitstream.getFlag();
  if (atlasParamsPresentFlag) {
    ivsAccessUnitParams.atlasParamsList = AtlasParamsList::decodeFrom(bitstream, cameras);
  }
  const auto ivsAupExtensionPresentFlag = bitstream.getFlag();
  cout << "ivs_aup_extension_present_flag=" << boolalpha << ivsAupExtensionPresentFlag << '\n';
  return ivsAccessUnitParams;
}

void IvAccessUnitParams::encodeTo(OutputBitstream &bitstream, const ViewParamsList &cameras) const {
  bitstream.putFlag(!!atlasParamsList);
  if (atlasParamsList) {
    atlasParamsList->encodeTo(bitstream, cameras);
  }
  bitstream.putFlag(false);
}
} // namespace TMIV::Metadata