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

#ifndef _TMIV_MIVBITSTREAM_ATLASOBJECTASSOCIATION_H_
#define _TMIV_MIVBITSTREAM_ATLASOBJECTASSOCIATION_H_

#include <TMIV/Common/Bitstream.h>

#include <optional>
#include <vector>

namespace TMIV::MivBitstream {

struct AtlasObjectAssociationUpdateParameters {
  std::uint8_t aoa_log2_max_object_idx_tracked_minus1;
  std::vector<std::uint8_t> aoa_atlas_idx;
  std::vector<std::uint8_t> aoa_object_idx;
  std::vector<std::vector<bool>> aoa_object_in_atlas_present_flag;
};

// 23090-12: atlas_object_association()
class AtlasObjectAssociation {
public:
  AtlasObjectAssociation() = default;
  //  explicit AtlasObjectAssociation(AtlasObjectAssociationUpdateList);  //TODO do we need this?

  // TODO enter getter fields here
  [[nodiscard]] auto aoa_persistence_flag() const noexcept -> bool;
  [[nodiscard]] auto aoa_reset_flag() const noexcept -> bool;
  [[nodiscard]] auto aoa_num_atlases_minus1() const noexcept -> std::uint8_t;
  [[nodiscard]] auto aoa_num_updates() const noexcept -> std::uint8_t;

  friend auto operator<<(std::ostream &stream, const AtlasObjectAssociation &x) -> std::ostream &;

  //  auto operator==(const AtlasObjectAssociation &other) const noexcept -> bool;
  //  auto operator!=(const AtlasObjectAssociation &other) const noexcept -> bool;

  static auto decodeFrom(Common::InputBitstream &bitstream) -> AtlasObjectAssociation;

  //  void encodeTo(Common::OutputBitstream &bitstream) const;

private:
  bool m_aoa_persistence_flag;
  bool m_aoa_reset_flag;
  std::uint8_t m_aoa_num_atlases_minus1; // TODO replace by content of aoa_parameters
  std::uint8_t m_aoa_num_updates;        // TODO replace by content of aoa_parameters
  std::optional<AtlasObjectAssociationUpdateParameters> m_aoa_parameters;
};
} // namespace TMIV::MivBitstream

#endif
