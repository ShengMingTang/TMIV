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

#include <TMIV/MivBitstream/AtlasObjectAssociation.h>

namespace TMIV::MivBitstream {
auto AtlasObjectAssociation::aoa_persistence_flag() const noexcept -> bool {
  return m_aoa_persistence_flag;
}

auto AtlasObjectAssociation::aoa_reset_flag() const noexcept -> bool { return m_aoa_reset_flag; }

auto AtlasObjectAssociation::aoa_num_atlases_minus1() const noexcept -> std::uint8_t {
  return m_aoa_num_atlases_minus1;
}

auto AtlasObjectAssociation::aoa_num_updates() const noexcept -> std::uint8_t {
  if (!m_aoa_parameters) {
    return 0U;
  }
  return m_aoa_parameters->aoa_object_idx.size();
}

[[nodiscard]] auto AtlasObjectAssociation::aoa_log2_max_object_idx_tracked_minus1() const noexcept
    -> std::uint8_t {
  VERIFY_BITSTREAM(0 < aoa_num_updates() && m_aoa_parameters.has_value());
  return m_aoa_parameters->aoa_log2_max_object_idx_tracked_minus1;
}

[[nodiscard]] auto AtlasObjectAssociation::aoa_atlas_id(std::size_t j) const noexcept
    -> std::uint8_t {
  VERIFY_BITSTREAM(0 < aoa_num_updates() && m_aoa_parameters.has_value() &&
                   j <= aoa_num_atlases_minus1());
  return m_aoa_parameters->aoa_atlas_id[j];
}

[[nodiscard]] auto AtlasObjectAssociation::aoa_object_idx(std::size_t i) const noexcept
    -> std::uint8_t {

  VERIFY_BITSTREAM(0 < aoa_num_updates() && m_aoa_parameters.has_value() && i < aoa_num_updates());
  return m_aoa_parameters->aoa_object_idx[i];
}

[[nodiscard]] auto
AtlasObjectAssociation::aoa_object_in_atlas_present_flag(std::size_t j,
                                                         std::size_t i) const noexcept -> bool {
  VERIFY_BITSTREAM(0 < aoa_num_updates() && m_aoa_parameters.has_value());
  // TODO do we need to check ranges here?
  return m_aoa_parameters->aoa_object_in_atlas_present_flag[aoa_object_idx(i)][aoa_atlas_id(j)];
}

constexpr auto AtlasObjectAssociation::aoa_persistence_flag(const bool value) noexcept -> auto & {
  m_aoa_persistence_flag = value;
  return *this;
}

auto operator<<(std::ostream &stream, const AtlasObjectAssociation &x) -> std::ostream & {
  stream << "aoa_persistence_flag=" << std::boolalpha << x.aoa_persistence_flag() << "\n";
  stream << "aoa_reset_flag=" << std::boolalpha << x.aoa_reset_flag() << "\n";
  stream << "aoa_num_atlases_minus1=" << static_cast<unsigned>(x.aoa_num_atlases_minus1()) << "\n";
  stream << "aoa_num_updates=" << static_cast<unsigned>(x.aoa_num_updates()) << "\n";
  if (x.aoa_num_updates() > 0) {
    stream << "aoa_log2_max_object_idx_tracked_minus1="
           << static_cast<unsigned>(x.aoa_log2_max_object_idx_tracked_minus1()) << "\n";
    for (std::size_t j = 0; j <= x.aoa_num_atlases_minus1(); ++j) {
      stream << "aoa_atlas_id(" << j << ")=" << static_cast<unsigned>(x.aoa_atlas_id(j)) << "\n";
    }
    for (std::size_t i = 0; i < x.aoa_num_updates(); ++i) {
      stream << "aoa_object_idx(" << i << ")=" << static_cast<unsigned>(x.aoa_object_idx(i))
             << "\n";
    }
    for (std::size_t i = 0; i < x.aoa_num_updates(); ++i) {
      for (std::size_t j = 0; j <= x.aoa_num_atlases_minus1(); ++j) {
        stream << "aoa_object_in_atlas_present_flag(" << j << ", " << i << ")=" << std::boolalpha
               << x.aoa_object_in_atlas_present_flag(j, i) << "\n";
      }
    }
  }
  return stream;
}

auto AtlasObjectAssociation::operator==(const AtlasObjectAssociation &other) const noexcept
    -> bool {
  return (this->aoa_persistence_flag() == other.aoa_persistence_flag()) &&
         (this->aoa_reset_flag() == other.aoa_reset_flag()) &&
         (this->aoa_num_atlases_minus1() == other.aoa_num_atlases_minus1()) &&
         (this->aoa_num_updates() == other.aoa_num_updates()) &&
         (this->m_aoa_parameters == other.m_aoa_parameters);
}

auto AtlasObjectAssociation::operator!=(const AtlasObjectAssociation &other) const noexcept
    -> bool {
  return !operator==(other);
}

auto AtlasObjectAssociation::decodeFrom(Common::InputBitstream &bitstream)
    -> AtlasObjectAssociation {
  auto result = AtlasObjectAssociation{};
  result.aoa_persistence_flag(bitstream.getFlag());
  // TODO continue
  return result;
}

void AtlasObjectAssociation::encodeTo(Common::OutputBitstream &bitstream) const {
  bitstream.putFlag(aoa_persistence_flag());
  bitstream.putFlag(aoa_reset_flag());
  // TODO continue
}
} // namespace TMIV::MivBitstream