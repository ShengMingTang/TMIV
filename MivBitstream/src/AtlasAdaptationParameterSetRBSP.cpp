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

#include <TMIV/MivBitstream/AtlasAdaptationParameterSetRBSP.h>

#include <TMIV/MivBitstream/verify.h>

#include <cmath>

using namespace std;
using namespace TMIV::Common;

namespace TMIV::MivBitstream {
auto operator<<(ostream &stream, const AapsVpccExtension & /* x */) -> ostream & {
  stream << "aaps_vpcc_camera_parameters_present_flag=false\n";
  return stream;
}

auto AapsVpccExtension::decodeFrom(InputBitstream &bitstream) -> AapsVpccExtension {
  const auto aaps_vpcc_camera_parameters_present_flag = bitstream.getFlag();
  LIMITATION(!aaps_vpcc_camera_parameters_present_flag);
  return {};
}

void AapsVpccExtension::encodeTo(OutputBitstream &bitstream) const {
  const auto aaps_vpcc_camera_parameters_present_flag = false;
  bitstream.putFlag(aaps_vpcc_camera_parameters_present_flag);
}

auto operator<<(ostream &stream, const AapsMivExtension &x) -> ostream & {
  return stream << "aame_omaf_v1_compatible_flag=" << boolalpha << x.aame_omaf_v1_compatible_flag()
                << '\n';
}

auto AapsMivExtension::operator==(const AapsMivExtension &other) const noexcept -> bool {
  return aame_omaf_v1_compatible_flag() == other.aame_omaf_v1_compatible_flag();
}

auto AapsMivExtension::operator!=(const AapsMivExtension &other) const noexcept -> bool {
  return !operator==(other);
}

auto AapsMivExtension::decodeFrom(InputBitstream &bitstream) -> AapsMivExtension {
  auto x = AapsMivExtension{};
  x.aame_omaf_v1_compatible_flag(bitstream.getFlag());
  return x;
}

void AapsMivExtension::encodeTo(OutputBitstream &bitstream) const {
  bitstream.putFlag(aame_omaf_v1_compatible_flag());
}

auto AtlasAdaptationParameterSetRBSP::aaps_log2_max_atlas_frame_order_cnt_lsb_minus4()
    const noexcept -> uint8_t {
  VERIFY_V3CBITSTREAM(aaps_log2_max_afoc_present_flag());
  VERIFY_V3CBITSTREAM(m_aaps_log2_max_atlas_frame_order_cnt_lsb_minus4.has_value());
  return *m_aaps_log2_max_atlas_frame_order_cnt_lsb_minus4;
}

auto AtlasAdaptationParameterSetRBSP::aaps_vpcc_extension() const noexcept
    -> const AapsVpccExtension & {
  VERIFY_V3CBITSTREAM(aaps_vpcc_extension_flag());
  VERIFY_V3CBITSTREAM(m_aaps_vpcc_extension.has_value());
  return *m_aaps_vpcc_extension;
}

auto AtlasAdaptationParameterSetRBSP::aaps_miv_extension() const noexcept
    -> const AapsMivExtension & {
  VERIFY_V3CBITSTREAM(aaps_miv_extension_flag());
  VERIFY_V3CBITSTREAM(m_aaps_miv_extension.has_value());
  return *m_aaps_miv_extension;
}

auto AtlasAdaptationParameterSetRBSP::aapsExtensionData() const noexcept -> const vector<bool> & {
  VERIFY_V3CBITSTREAM(aaps_extension_6bits() != 0);
  VERIFY_V3CBITSTREAM(m_aapsExtensionData.has_value());
  return *m_aapsExtensionData;
}

auto AtlasAdaptationParameterSetRBSP::aaps_log2_max_atlas_frame_order_cnt_lsb_minus4(
    std::uint8_t value) noexcept -> AtlasAdaptationParameterSetRBSP & {
  VERIFY_V3CBITSTREAM(aaps_log2_max_afoc_present_flag());
  m_aaps_log2_max_atlas_frame_order_cnt_lsb_minus4 = value;
  return *this;
}

auto AtlasAdaptationParameterSetRBSP::aaps_vpcc_extension_flag(bool value) noexcept
    -> AtlasAdaptationParameterSetRBSP & {
  VERIFY_V3CBITSTREAM(aaps_extension_flag());
  m_aaps_vpcc_extension_flag = value;
  return *this;
}

auto AtlasAdaptationParameterSetRBSP::aaps_miv_extension_flag(bool value) noexcept
    -> AtlasAdaptationParameterSetRBSP & {
  VERIFY_V3CBITSTREAM(aaps_extension_flag());
  m_aaps_miv_extension_flag = value;
  return *this;
}

auto AtlasAdaptationParameterSetRBSP::aaps_extension_6bits(std::uint8_t value) noexcept
    -> AtlasAdaptationParameterSetRBSP & {
  VERIFY_V3CBITSTREAM(aaps_extension_flag());
  m_aaps_extension_6bits = value;
  return *this;
}

auto AtlasAdaptationParameterSetRBSP::aaps_vpcc_extension(const AapsVpccExtension &value) noexcept
    -> AtlasAdaptationParameterSetRBSP & {
  VERIFY_V3CBITSTREAM(aaps_vpcc_extension_flag());
  m_aaps_vpcc_extension = value;
  return *this;
}

auto AtlasAdaptationParameterSetRBSP::aaps_miv_extension(AapsMivExtension value) noexcept
    -> AtlasAdaptationParameterSetRBSP & {
  VERIFY_V3CBITSTREAM(aaps_miv_extension_flag());
  m_aaps_miv_extension = value;
  return *this;
}

auto AtlasAdaptationParameterSetRBSP::aapsExtensionData(std::vector<bool> value) noexcept
    -> AtlasAdaptationParameterSetRBSP & {
  VERIFY_V3CBITSTREAM(aaps_extension_6bits() != 0);
  m_aapsExtensionData = move(value);
  return *this;
}

auto AtlasAdaptationParameterSetRBSP::aaps_miv_extension() noexcept -> AapsMivExtension & {
  VERIFY_V3CBITSTREAM(aaps_miv_extension_flag());
  if (!m_aaps_miv_extension) {
    m_aaps_miv_extension = AapsMivExtension{};
  }
  return *m_aaps_miv_extension;
}

auto operator<<(ostream &stream, const AtlasAdaptationParameterSetRBSP &x) -> ostream & {
  stream << "aaps_atlas_adaptation_parameter_set_id="
         << int(x.aaps_atlas_adaptation_parameter_set_id()) << '\n';
  stream << "aaps_log2_max_afoc_present_flag=" << boolalpha << x.aaps_log2_max_afoc_present_flag()
         << '\n';
  if (x.aaps_log2_max_afoc_present_flag()) {
    stream << "aaps_log2_max_atlas_frame_order_cnt_lsb_minus4="
           << int(x.aaps_log2_max_atlas_frame_order_cnt_lsb_minus4()) << '\n';
  }
  stream << "aaps_extension_flag=" << boolalpha << x.aaps_extension_flag() << '\n';
  if (x.aaps_extension_flag()) {
    stream << "aaps_vpcc_extension_flag=" << boolalpha << x.aaps_vpcc_extension_flag() << '\n';
    stream << "aaps_miv_extension_flag=" << boolalpha << x.aaps_miv_extension_flag() << '\n';
    stream << "aaps_extension_6bits=" << int(x.aaps_extension_6bits()) << '\n';
  }
  if (x.aaps_vpcc_extension_flag()) {
    stream << x.aaps_vpcc_extension();
  }
  if (x.aaps_miv_extension_flag()) {
    stream << x.aaps_miv_extension();
  }
  if (x.aaps_extension_6bits()) {
    for (auto bit : x.aapsExtensionData()) {
      stream << "aaps_extension_data_flag=" << boolalpha << bit << '\n';
    }
  }
  return stream;
}

auto AtlasAdaptationParameterSetRBSP::operator==(
    const AtlasAdaptationParameterSetRBSP &other) const noexcept -> bool {
  if (aaps_atlas_adaptation_parameter_set_id() != other.aaps_atlas_adaptation_parameter_set_id() ||
      aaps_log2_max_afoc_present_flag() != other.aaps_log2_max_afoc_present_flag() ||
      aaps_extension_flag() != other.aaps_extension_flag() ||
      aaps_vpcc_extension_flag() != other.aaps_vpcc_extension_flag() ||
      aaps_miv_extension_flag() != other.aaps_miv_extension_flag() ||
      aaps_extension_6bits() != other.aaps_extension_6bits()) {
    return false;
  }
  if (aaps_log2_max_afoc_present_flag() &&
      aaps_log2_max_atlas_frame_order_cnt_lsb_minus4() !=
          other.aaps_log2_max_atlas_frame_order_cnt_lsb_minus4()) {
    return false;
  }
  if (aaps_vpcc_extension_flag() && aaps_vpcc_extension() != other.aaps_vpcc_extension()) {
    return false;
  }
  if (aaps_miv_extension_flag() && aaps_miv_extension() != other.aaps_miv_extension()) {
    return false;
  }
  if (aaps_extension_6bits() != 0 && aapsExtensionData() != other.aapsExtensionData()) {
    return false;
  }
  return true;
}

auto AtlasAdaptationParameterSetRBSP::operator!=(
    const AtlasAdaptationParameterSetRBSP &other) const noexcept -> bool {
  return !operator==(other);
}

auto AtlasAdaptationParameterSetRBSP::decodeFrom(istream &stream)
    -> AtlasAdaptationParameterSetRBSP {
  InputBitstream bitstream{stream};

  auto x = AtlasAdaptationParameterSetRBSP{};

  x.aaps_atlas_adaptation_parameter_set_id(bitstream.getUExpGolomb<uint8_t>());
  x.aaps_log2_max_afoc_present_flag(bitstream.getFlag());

  if (x.aaps_log2_max_afoc_present_flag()) {
    x.aaps_log2_max_atlas_frame_order_cnt_lsb_minus4(bitstream.getUExpGolomb<uint8_t>());
  }
  x.aaps_extension_flag(bitstream.getFlag());

  if (x.aaps_extension_flag()) {
    x.aaps_vpcc_extension_flag(bitstream.getFlag());
    x.aaps_miv_extension_flag(bitstream.getFlag());
    x.aaps_extension_6bits(bitstream.readBits<uint8_t>(6));
  }
  if (x.aaps_vpcc_extension_flag()) {
    x.aaps_vpcc_extension(AapsVpccExtension::decodeFrom(bitstream));
  }
  if (x.aaps_miv_extension_flag()) {
    x.aaps_miv_extension(AapsMivExtension::decodeFrom(bitstream));
  }
  if (x.aaps_extension_6bits() != 0) {
    auto aapsExtensionData = vector<bool>{};
    while (bitstream.moreRbspData()) {
      aapsExtensionData.push_back(bitstream.getFlag());
    }
    x.aapsExtensionData(move(aapsExtensionData));
  }
  bitstream.rbspTrailingBits();

  return x;
}

void AtlasAdaptationParameterSetRBSP::encodeTo(ostream &stream) const {
  OutputBitstream bitstream{stream};

  bitstream.putUExpGolomb(aaps_atlas_adaptation_parameter_set_id());
  bitstream.putFlag(aaps_log2_max_afoc_present_flag());

  if (aaps_log2_max_afoc_present_flag()) {
    bitstream.putUExpGolomb(aaps_log2_max_atlas_frame_order_cnt_lsb_minus4());
  }
  bitstream.putFlag(aaps_extension_flag());

  if (aaps_extension_flag()) {
    bitstream.putFlag(aaps_vpcc_extension_flag());
    bitstream.putFlag(aaps_miv_extension_flag());
    bitstream.writeBits(aaps_extension_6bits(), 6);
  }
  if (aaps_vpcc_extension_flag()) {
    aaps_vpcc_extension().encodeTo(bitstream);
  }
  if (aaps_miv_extension_flag()) {
    aaps_miv_extension().encodeTo(bitstream);
  }
  if (aaps_extension_6bits() != 0) {
    for (auto bit : aapsExtensionData()) {
      bitstream.putFlag(bit);
    }
  }
  bitstream.rbspTrailingBits();
}

auto aapsById(const std::vector<AtlasAdaptationParameterSetRBSP> &aapsV, int id) noexcept
    -> const AtlasAdaptationParameterSetRBSP & {
  for (auto &x : aapsV) {
    if (id == x.aaps_atlas_adaptation_parameter_set_id()) {
      return x;
    }
  }
  V3CBITSTREAM_ERROR("Unknown AAPS ID");
}
} // namespace TMIV::MivBitstream