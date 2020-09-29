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

#include <TMIV/MivBitstream/PackedIndependentRegions.h>

namespace TMIV::MivBitstream {
// TODO does _minus1 mean that there must be at least one? Implemented like this
auto PackedIndependentRegions::pir_num_packed_frames_minus1() const noexcept -> std::uint8_t {
  VERIFY_BITSTREAM(!m_pirPackedFrames.empty());
  return m_pirPackedFrames.size() - 1U;
}
auto PackedIndependentRegions::pir_packed_frame_id(std::uint8_t j) const noexcept -> std::uint8_t {
  VERIFY_BITSTREAM(j <= pir_num_packed_frames_minus1());
  return m_pirPackedFrames[j].pir_packed_frame_id;
}
auto PackedIndependentRegions::pir_description_type_idc(std::uint8_t k) const noexcept
    -> std::uint8_t {
  VERIFY_BITSTREAM(k <= pir_num_packed_frames_minus1());
  if (std::holds_alternative<TileRegions>(m_pirPackedFrames[k].regions)) {
    return 0U;
  }
  return 1U;
}
auto PackedIndependentRegions::pir_num_regions_minus1(std::uint8_t k) const noexcept
    -> std::uint8_t {
  VERIFY_BITSTREAM(k <= pir_num_packed_frames_minus1());
  if (pir_description_type_idc(k) == 0) {
    return std::get<TileRegions>(m_pirPackedFrames[k].regions).size() - 1U;
  }
  return std::get<subPicIds>(m_pirPackedFrames[k].regions).size() - 1U;
}
auto PackedIndependentRegions::pir_top_left_tile_idx(std::uint8_t k, std::uint8_t i) const noexcept
    -> std::size_t {
  VERIFY_BITSTREAM(k <= pir_num_packed_frames_minus1() && pir_description_type_idc(k) == 0 &&
                   i <= pir_num_regions_minus1(k));
  const auto &tileRegions = std::get<TileRegions>(m_pirPackedFrames[k].regions);
  return tileRegions[i].pir_top_left_tile_idx;
}
auto PackedIndependentRegions::pir_bottom_right_tile_idx(std::uint8_t k,
                                                         std::uint8_t i) const noexcept
    -> std::size_t {
  VERIFY_BITSTREAM(k <= pir_num_packed_frames_minus1() && pir_description_type_idc(k) == 0 &&
                   i <= pir_num_regions_minus1(k));
  const auto &tileRegions = std::get<TileRegions>(m_pirPackedFrames[k].regions);
  return tileRegions[i].pir_bottom_right_tile_idx;
}
auto PackedIndependentRegions::pir_subpic_id(std::uint8_t k, std::uint8_t i) const noexcept
    -> std::size_t {
  VERIFY_BITSTREAM(k <= pir_num_packed_frames_minus1() && pir_description_type_idc(k) == 0 &&
                   i <= pir_num_regions_minus1(k));
  const auto &subPicId = std::get<subPicIds>(m_pirPackedFrames[k].regions);
  return subPicId[i];
}

auto operator<<(std::ostream &stream, const PackedIndependentRegions &x) -> std::ostream & {
  stream << "pir_num_packed_frames_minus1="
         << static_cast<unsigned>(x.pir_num_packed_frames_minus1()) << "\n";
  for (int j = 0; j <= x.pir_num_packed_frames_minus1(); ++j) {
    stream << "pir_packed_frame_id=" << static_cast<unsigned>(x.pir_packed_frame_id(j)) << "\n";
    stream << "pir_description_type_idc=" << static_cast<unsigned>(x.pir_description_type_idc(j))
           << "\n";
  }
  return stream;
}

auto PackedIndependentRegions::operator==(const PackedIndependentRegions &other) const noexcept
    -> bool {
  return m_pirPackedFrames == other.m_pirPackedFrames;
}

auto PackedIndependentRegions::decodeFrom(Common::InputBitstream &bitstream)
    -> PackedIndependentRegions {
  PackedIndependentRegions result{};
  result.pir_num_packed_frames_minus1(bitstream.readBits<std::uint8_t>(5));
  for (int j = 0; j <= result.pir_num_packed_frames_minus1(); ++j) {
    result.pir_packed_frame_id(j, bitstream.readBits<std::uint8_t>(5));
    const auto k = result.pir_packed_frame_id(j);
    result.pir_description_type_idc(k, bitstream.readBits<std::uint8_t>(2));
  }
  return result;
}

void PackedIndependentRegions::encodeTo(Common::OutputBitstream &bitstream) const {
  bitstream.writeBits(pir_num_packed_frames_minus1(), 5);
  for (int j = 0; j <= pir_num_packed_frames_minus1(); ++j) {
    bitstream.writeBits(pir_packed_frame_id(j), 5);
    const auto k = pir_packed_frame_id(j);
    bitstream.writeBits(pir_description_type_idc(k), 2);
  }
}
} // namespace TMIV::MivBitstream
