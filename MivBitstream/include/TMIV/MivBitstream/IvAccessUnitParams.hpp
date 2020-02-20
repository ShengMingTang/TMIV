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

#ifndef _TMIV_MIVBITSTREAM_IVACCESSUNITPARAMS_H_
#error "Include the .h, not the .hpp"
#endif

namespace TMIV::MivBitstream {
inline auto PatchParams::pdu2dPos() const noexcept { return m_pdu2dPos; }

inline auto PatchParams::patchSizeInView() const noexcept { return m_patchSizeInView; }

inline auto PatchParams::pduViewPos() const noexcept { return m_pduViewPos; }

inline auto PatchParams::pduDepthStart() const noexcept { return m_pduDepthStart; }

inline auto PatchParams::pduDepthEnd() const noexcept { return m_pduDepthEnd; }

inline auto PatchParams::pduViewId() const noexcept { return m_pduViewId; }

inline auto PatchParams::pduOrientationIndex() const noexcept { return m_pduOrientationIndex; }

inline auto PatchParams::pduEntityId() const noexcept { return m_pduEntityId; }

inline auto PatchParams::pduDepthOccMapThreshold() const noexcept {
  return m_pduDepthOccMapThreshold;
}

inline auto PatchParams::pdu2dPos(const Common::Vec2i value) noexcept -> PatchParams & {
  m_pdu2dPos = value;
  return *this;
}
inline auto PatchParams::patchSizeInView(const Common::Vec2i value) noexcept -> PatchParams & {
  m_patchSizeInView = value;
  return *this;
}
inline auto PatchParams::pduViewPos(const Common::Vec2i value) noexcept -> PatchParams & {
  m_pduViewPos = value;
  return *this;
}
inline auto PatchParams::pduDepthStart(const std::uint16_t value) noexcept -> PatchParams & {
  m_pduDepthStart = value;
  return *this;
}
inline auto PatchParams::pduDepthEnd(const std::uint16_t value) noexcept -> PatchParams & {
  m_pduDepthEnd = value;
  return *this;
}
inline auto PatchParams::pduViewId(const std::uint16_t value) noexcept -> PatchParams & {
  m_pduViewId = value;
  return *this;
}
inline auto PatchParams::pduOrientationIndex(const FlexiblePatchOrientation value) noexcept
    -> PatchParams & {
  m_pduOrientationIndex = value;
  return *this;
}
inline auto PatchParams::pduEntityId(const std::uint16_t value) noexcept -> PatchParams & {
  m_pduEntityId = value;
  return *this;
}
inline auto PatchParams::pduDepthOccMapThreshold(const std::uint16_t value) noexcept
    -> PatchParams & {
  m_pduDepthOccMapThreshold = value;
  return *this;
}

inline auto PatchParams::isRotated() const -> bool {
  return pduOrientationIndex() == FlexiblePatchOrientation::FPO_ROT90 ||
         pduOrientationIndex() == FlexiblePatchOrientation::FPO_SWAP ||
         pduOrientationIndex() == FlexiblePatchOrientation::FPO_ROT270 ||
         pduOrientationIndex() == FlexiblePatchOrientation::FPO_MROT90;
}

inline auto PatchParams::patchSizeInAtlas() const -> Common::Vec2i {
  if (isRotated()) {
    return {patchSizeInView().y(), patchSizeInView().x()};
  }
  return patchSizeInView();
}

inline auto PatchParams::operator==(const PatchParams &other) const -> bool {
  return vuhAtlasId == other.vuhAtlasId && pduViewId() == other.pduViewId() &&
         pduEntityId() == other.pduEntityId() && patchSizeInView() == other.patchSizeInView() &&
         pduViewPos() == other.pduViewPos() && pdu2dPos() == other.pdu2dPos() &&
         pduOrientationIndex() == other.pduOrientationIndex() &&
         pduDepthOccMapThreshold() == other.pduDepthOccMapThreshold() &&
         pduDepthStart() == other.pduDepthStart();
}

inline auto viewToAtlas(Common::Vec2i viewPosition, const PatchParams &patch) -> Common::Vec2i {
  int w = patch.patchSizeInView().x();
  int h = patch.patchSizeInView().y();
  int xM = patch.pduViewPos().x();
  int yM = patch.pduViewPos().y();
  int xP = patch.pdu2dPos().x();
  int yP = patch.pdu2dPos().y();
  int x = viewPosition.x();
  int y = viewPosition.y();

  switch (patch.pduOrientationIndex()) {
  case FlexiblePatchOrientation::FPO_NULL: // (x, y)
    return {x - xM + xP, y - yM + yP};
  case FlexiblePatchOrientation::FPO_SWAP: // (y, x)
    return {y - yM + xP, x - xM + yP};
  case FlexiblePatchOrientation::FPO_ROT90: // (-y, x)
    return {-y + yM + xP + h - 1, x - xM + yP};
  case FlexiblePatchOrientation::FPO_ROT180: // (-x, -y)
    return {-x + xM + xP + w - 1, -y + yM + yP + h - 1};
  case FlexiblePatchOrientation::FPO_ROT270: // (y, -x)
    return {y - yM + xP, -x + xM + yP + w - 1};
  case FlexiblePatchOrientation::FPO_MIRROR: // (-x, y)
    return {-x + xM + xP + w - 1, y - yM + yP};
  case FlexiblePatchOrientation::FPO_MROT90: // (-y, -x)
    return {-y + yM + xP + h - 1, -x + xM + yP + w - 1};
  case FlexiblePatchOrientation::FPO_MROT180: // (x, -y)
    return {x - xM + xP, -y + yM + yP + h - 1};
  default:
    abort();
  }
}

inline auto atlasToView(Common::Vec2i atlasPosition, const PatchParams &patch) -> Common::Vec2i {
  int w = patch.patchSizeInView().x();
  int h = patch.patchSizeInView().y();
  int xM = patch.pduViewPos().x();
  int yM = patch.pduViewPos().y();
  int xP = patch.pdu2dPos().x();
  int yP = patch.pdu2dPos().y();
  int x = atlasPosition.x();
  int y = atlasPosition.y();

  switch (patch.pduOrientationIndex()) {
  case FlexiblePatchOrientation::FPO_NULL: // (x, y)
    return {x - xP + xM, y - yP + yM};
  case FlexiblePatchOrientation::FPO_SWAP: // (y, x)
    return {y - yP + xM, x - xP + yM};
  case FlexiblePatchOrientation::FPO_ROT90: // (y, -x)
    return {y - yP + xM, -x + xP + yM + h - 1};
  case FlexiblePatchOrientation::FPO_ROT180: // (-x, -y)
    return {-x + xP + xM + w - 1, -y + yP + yM + h - 1};
  case FlexiblePatchOrientation::FPO_ROT270: // (-y, x)
    return {-y + yP + xM + w - 1, x - xP + yM};
  case FlexiblePatchOrientation::FPO_MIRROR: // (-x, y)
    return {-x + xP + xM + w - 1, y - yP + yM};
  case FlexiblePatchOrientation::FPO_MROT90: // (-y, -x)
    return {-y + yP + xM + w - 1, -x + xP + yM + h - 1};
  case FlexiblePatchOrientation::FPO_MROT180: // (x, -y)
    return {x - xP + xM, -y + yP + yM + h - 1};
  default:
    abort();
  }
}

inline auto operator<<(std::ostream &stream, const IvAccessUnitParams &ivAccessUnitParams)
    -> std::ostream & {
  return stream << ivAccessUnitParams.atlasParamsList;
}

inline auto IvAccessUnitParams::operator==(const IvAccessUnitParams &other) const -> bool {
  return atlasParamsList == other.atlasParamsList;
}
} // namespace TMIV::MivBitstream
