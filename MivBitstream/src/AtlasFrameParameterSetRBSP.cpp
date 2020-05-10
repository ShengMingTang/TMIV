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

#include <TMIV/MivBitstream/AtlasFrameParameterSetRBSP.h>

#include <TMIV/Common/Bitstream.h>
#include <TMIV/MivBitstream/MivDecoderMode.h>
#include <TMIV/MivBitstream/verify.h>

#include <ostream>

using namespace std;
using namespace TMIV::Common;

namespace TMIV::MivBitstream {
auto operator<<(ostream &stream, const AtlasFrameTileInformation &x) -> ostream & {
  stream << "afti_single_tile_in_atlas_frame_flag=" << boolalpha
         << x.afti_single_tile_in_atlas_frame_flag() << '\n';
  stream << "afti_signalled_tile_id_flag=" << boolalpha << x.afti_signalled_tile_id_flag() << '\n';
  return stream;
}

auto AtlasFrameTileInformation::decodeFrom(InputBitstream &bitstream) -> AtlasFrameTileInformation {
  const auto afti_single_tile_in_atlas_frame_flag = bitstream.getFlag();

  // NOTE(BK): The proposal is to restrict to afti_single_tile_in_atlas_frame_flag == 1, but for
  // sake of being able to parse the provided V3C bitstream, this implementation accepts more
  // as long as there is only a single partition and tile.
  if (!afti_single_tile_in_atlas_frame_flag) {
    const auto afti_uniform_partition_spacing_flag = bitstream.getFlag();
    VERIFY_MIVBITSTREAM(!afti_uniform_partition_spacing_flag);

    const auto afti_num_partition_columns_minus1 = bitstream.getUExpGolomb<size_t>();
    VERIFY_MIVBITSTREAM(afti_num_partition_columns_minus1 == 0);

    const auto afti_num_partition_rows_minus1 = bitstream.getUExpGolomb<size_t>();
    VERIFY_MIVBITSTREAM(afti_num_partition_rows_minus1 == 0);

    const auto afti_single_partition_per_tile_flag = bitstream.getFlag();

    if (!afti_single_partition_per_tile_flag) {
      const auto afti_num_tiles_in_atlas_frame_minus1 = bitstream.getUExpGolomb<size_t>();
      VERIFY_MIVBITSTREAM(afti_num_tiles_in_atlas_frame_minus1 == 0);
    }
  }

  const auto afti_signalled_tile_id_flag = bitstream.getFlag();
  VERIFY_MIVBITSTREAM(!afti_signalled_tile_id_flag);

  return {};
}

void AtlasFrameTileInformation::encodeTo(OutputBitstream &bitstream) {
  constexpr auto afti_single_tile_in_atlas_frame_flag = true;
  bitstream.putFlag(afti_single_tile_in_atlas_frame_flag);

  const auto afti_signalled_tile_id_flag = false;
  bitstream.putFlag(afti_signalled_tile_id_flag);
}

auto operator<<(ostream &stream, const AfpsVpccExtension & /* x */) -> ostream & { return stream; }

auto AfpsVpccExtension::decodeFrom(InputBitstream & /* bitstream */) -> AfpsVpccExtension {
  return {};
}

void AfpsVpccExtension::encodeTo(OutputBitstream & /* stream */) const {}

auto operator<<(ostream &stream, const AfpsMivExtension & /* x */) -> ostream & { return stream; }

auto AfpsMivExtension::decodeFrom(InputBitstream & /* bitstream */) -> AfpsMivExtension {
  return {};
}

void AfpsMivExtension::encodeTo(OutputBitstream & /* stream */) const {}

auto AtlasFrameParameterSetRBSP::afps_vpcc_extension() const noexcept -> AfpsVpccExtension {
  VERIFY_V3CBITSTREAM(afps_vpcc_extension_flag());
  VERIFY_V3CBITSTREAM(m_afve.has_value());
  return *m_afve;
}

auto AtlasFrameParameterSetRBSP::afps_miv_extension() const noexcept -> AfpsMivExtension {
  VERIFY_V3CBITSTREAM(afps_miv_extension_flag());
  VERIFY_V3CBITSTREAM(m_afme.has_value());
  return *m_afme;
}

auto AtlasFrameParameterSetRBSP::afpsExtensionData() const noexcept -> const vector<bool> & {
  VERIFY_V3CBITSTREAM(afps_extension_6bits() != 0);
  VERIFY_V3CBITSTREAM(m_afpsExtensionData.has_value());
  return *m_afpsExtensionData;
}

auto AtlasFrameParameterSetRBSP::afps_vpcc_extension_flag(bool value) noexcept
    -> AtlasFrameParameterSetRBSP & {
  VERIFY_V3CBITSTREAM(afps_extension_present_flag());
  m_afps_vpcc_extension_flag = value;
  return *this;
}

auto AtlasFrameParameterSetRBSP::afps_miv_extension_flag(bool value) noexcept
    -> AtlasFrameParameterSetRBSP & {
  VERIFY_V3CBITSTREAM(afps_extension_present_flag());
  m_afps_miv_extension_flag = value;
  return *this;
}

auto AtlasFrameParameterSetRBSP::afps_extension_6bits(uint8_t value) noexcept
    -> AtlasFrameParameterSetRBSP & {
  VERIFY_V3CBITSTREAM(afps_extension_present_flag());
  m_afps_extension_6bits = value;
  return *this;
}

auto AtlasFrameParameterSetRBSP::afps_vpcc_extension(const AfpsVpccExtension &value) noexcept
    -> AtlasFrameParameterSetRBSP & {
  VERIFY_V3CBITSTREAM(afps_vpcc_extension_flag());
  m_afve = value;
  return *this;
}

auto AtlasFrameParameterSetRBSP::afps_miv_extension(const AfpsMivExtension &value) noexcept
    -> AtlasFrameParameterSetRBSP & {
  VERIFY_V3CBITSTREAM(afps_miv_extension_flag());
  m_afme = value;
  return *this;
}

auto AtlasFrameParameterSetRBSP::afpsExtensionData(vector<bool> value) noexcept
    -> AtlasFrameParameterSetRBSP & {
  VERIFY_V3CBITSTREAM(afps_extension_6bits() != 0);
  m_afpsExtensionData = move(value);
  return *this;
}

auto operator<<(ostream &stream, const AtlasFrameParameterSetRBSP &x) -> ostream & {
  stream << "afps_atlas_frame_parameter_set_id=" << int(x.afps_atlas_frame_parameter_set_id())
         << '\n';
  stream << "afps_atlas_sequence_parameter_set_id=" << int(x.afps_atlas_sequence_parameter_set_id())
         << '\n';
  stream << x.atlas_frame_tile_information();
  stream << "afps_output_flag_present_flag=" << boolalpha << x.afps_output_flag_present_flag()
         << '\n';
  stream << "afps_num_ref_idx_default_active_minus1="
         << int(x.afps_num_ref_idx_default_active_minus1()) << '\n';
  stream << "afps_additional_lt_afoc_lsb_len=" << int(x.afps_additional_lt_afoc_lsb_len()) << '\n';
  stream << "afps_3d_pos_x_bit_count_minus1=" << int(x.afps_3d_pos_x_bit_count_minus1()) << '\n';
  stream << "afps_3d_pos_y_bit_count_minus1=" << int(x.afps_3d_pos_y_bit_count_minus1()) << '\n';
  stream << "afps_lod_mode_enabled_flag=" << boolalpha << x.afps_lod_mode_enabled_flag() << '\n';
  stream << "afps_raw_3d_pos_bit_count_explicit_mode_flag=" << boolalpha
         << x.afps_raw_3d_pos_bit_count_explicit_mode_flag() << '\n';
  stream << "afps_extension_present_flag=" << boolalpha << x.afps_extension_present_flag() << '\n';
  if (x.afps_extension_present_flag()) {
    stream << "afps_vpcc_extension_flag=" << boolalpha << x.afps_vpcc_extension_flag() << '\n';
    stream << "afps_miv_extension_flag=" << boolalpha << x.afps_miv_extension_flag() << '\n';
    stream << "afps_extension_6bits=" << int(x.afps_extension_6bits()) << '\n';
  }
  if (x.afps_vpcc_extension_flag()) {
    stream << x.afps_vpcc_extension();
  }
  if (x.afps_miv_extension_flag()) {
    stream << x.afps_miv_extension();
  }
  if (x.afps_extension_6bits() != 0) {
    for (auto bit : x.afpsExtensionData()) {
      stream << "afps_extension_data_flag=" << boolalpha << bit << '\n';
    }
  }
  return stream;
}

auto AtlasFrameParameterSetRBSP::operator==(const AtlasFrameParameterSetRBSP &other) const noexcept
    -> bool {
  if (afps_atlas_frame_parameter_set_id() != other.afps_atlas_frame_parameter_set_id() ||
      afps_atlas_sequence_parameter_set_id() != other.afps_atlas_sequence_parameter_set_id() ||
      atlas_frame_tile_information() != other.atlas_frame_tile_information() ||
      afps_output_flag_present_flag() != other.afps_output_flag_present_flag() ||
      afps_num_ref_idx_default_active_minus1() != other.afps_num_ref_idx_default_active_minus1() ||
      afps_additional_lt_afoc_lsb_len() != other.afps_additional_lt_afoc_lsb_len() ||
      afps_3d_pos_x_bit_count_minus1() != other.afps_3d_pos_x_bit_count_minus1() ||
      afps_3d_pos_y_bit_count_minus1() != other.afps_3d_pos_y_bit_count_minus1() ||
      afps_lod_mode_enabled_flag() != other.afps_lod_mode_enabled_flag() ||
      afps_raw_3d_pos_bit_count_explicit_mode_flag() !=
          other.afps_raw_3d_pos_bit_count_explicit_mode_flag() ||
      afps_extension_present_flag() != other.afps_extension_present_flag() ||
      afps_vpcc_extension_flag() != other.afps_vpcc_extension_flag() ||
      afps_miv_extension_flag() != other.afps_miv_extension_flag() ||
      afps_extension_6bits() != other.afps_extension_6bits()) {
    return false;
  }
  if (afps_vpcc_extension_flag() && afps_vpcc_extension() != other.afps_vpcc_extension()) {
    return false;
  }
  if (afps_miv_extension_flag() && afps_miv_extension() != other.afps_miv_extension()) {
    return false;
  }
  if (afps_extension_6bits() && afpsExtensionData() != other.afpsExtensionData()) {
    return false;
  }
  return true;
}

auto AtlasFrameParameterSetRBSP::operator!=(const AtlasFrameParameterSetRBSP &other) const noexcept
    -> bool {
  return !operator==(other);
}

auto AtlasFrameParameterSetRBSP::decodeFrom(istream &stream,
                                            const vector<AtlasSequenceParameterSetRBSP> &aspsV)
    -> AtlasFrameParameterSetRBSP {
  auto x = AtlasFrameParameterSetRBSP{};
  InputBitstream bitstream{stream};

  x.afps_atlas_frame_parameter_set_id(bitstream.getUExpGolomb<uint8_t>());
  VERIFY_V3CBITSTREAM(x.afps_atlas_frame_parameter_set_id() <= 63);

  x.afps_atlas_sequence_parameter_set_id(bitstream.getUExpGolomb<uint8_t>());
  VERIFY_V3CBITSTREAM(x.afps_atlas_sequence_parameter_set_id() <= 15);
  VERIFY_V3CBITSTREAM(x.afps_atlas_sequence_parameter_set_id() < aspsV.size());
  const auto &asps = aspsV[x.afps_atlas_sequence_parameter_set_id()];

  x.atlas_frame_tile_information(AtlasFrameTileInformation::decodeFrom(bitstream));

  x.afps_output_flag_present_flag(bitstream.getFlag());

  x.afps_num_ref_idx_default_active_minus1(bitstream.getUExpGolomb<uint8_t>());
  VERIFY_V3CBITSTREAM(x.afps_num_ref_idx_default_active_minus1() <= 14);

  x.afps_additional_lt_afoc_lsb_len(bitstream.getUExpGolomb<uint8_t>());
  VERIFY_V3CBITSTREAM(x.afps_additional_lt_afoc_lsb_len() <=
                      32 - (asps.asps_log2_max_atlas_frame_order_cnt_lsb_minus4() + 4));
  VERIFY_V3CBITSTREAM(asps.asps_long_term_ref_atlas_frames_flag() ||
                      x.afps_additional_lt_afoc_lsb_len() == 0);

  x.afps_3d_pos_x_bit_count_minus1(bitstream.readBits<uint8_t>((5)));
  x.afps_3d_pos_y_bit_count_minus1(bitstream.readBits<uint8_t>((5)));
  x.afps_lod_mode_enabled_flag(bitstream.getFlag());
  x.afps_raw_3d_pos_bit_count_explicit_mode_flag(bitstream.getFlag());
  x.afps_extension_present_flag(bitstream.getFlag());

  if (x.afps_extension_present_flag()) {
    x.afps_vpcc_extension_flag(bitstream.getFlag());
    x.afps_miv_extension_flag(bitstream.getFlag());
    x.afps_extension_6bits(bitstream.readBits<uint8_t>(6));
  }
  if (x.afps_vpcc_extension_flag()) {
    x.afps_vpcc_extension(AfpsVpccExtension::decodeFrom(bitstream));
  }
  if (x.afps_miv_extension_flag()) {
    x.afps_miv_extension(AfpsMivExtension::decodeFrom(bitstream));
  }
  if (x.afps_extension_6bits() != 0) {
    auto afpsExtensionData = vector<bool>{};
    while (bitstream.moreRbspData()) {
      afpsExtensionData.push_back(bitstream.getFlag());
    }
    x.afpsExtensionData(move(afpsExtensionData));
  }
  bitstream.rbspTrailingBits();

  return x;
}

void AtlasFrameParameterSetRBSP::encodeTo(
    ostream &stream, const vector<AtlasSequenceParameterSetRBSP> &aspsV) const {
  OutputBitstream bitstream{stream};

  VERIFY_V3CBITSTREAM(afps_atlas_frame_parameter_set_id() <= 63);
  bitstream.putUExpGolomb(afps_atlas_frame_parameter_set_id());

  VERIFY_V3CBITSTREAM(afps_atlas_sequence_parameter_set_id() <= 15);
  VERIFY_V3CBITSTREAM(afps_atlas_sequence_parameter_set_id() < aspsV.size());
  bitstream.putUExpGolomb(afps_atlas_sequence_parameter_set_id());
  const auto &asps = aspsV[afps_atlas_sequence_parameter_set_id()];

  atlas_frame_tile_information().encodeTo(bitstream);

  bitstream.putFlag(afps_output_flag_present_flag());

  VERIFY_V3CBITSTREAM(afps_num_ref_idx_default_active_minus1() <= 14);
  bitstream.putUExpGolomb(afps_num_ref_idx_default_active_minus1());

  VERIFY_V3CBITSTREAM(afps_additional_lt_afoc_lsb_len() <=
                      32 - (asps.asps_log2_max_atlas_frame_order_cnt_lsb_minus4() + 4));
  VERIFY_V3CBITSTREAM(asps.asps_long_term_ref_atlas_frames_flag() ||
                      afps_additional_lt_afoc_lsb_len() == 0);
  bitstream.putUExpGolomb(afps_additional_lt_afoc_lsb_len());

  VERIFY_V3CBITSTREAM(afps_3d_pos_x_bit_count_minus1() < 32);
  bitstream.writeBits(afps_3d_pos_x_bit_count_minus1(), 5);

  VERIFY_V3CBITSTREAM(afps_3d_pos_y_bit_count_minus1() < 32);
  bitstream.writeBits(afps_3d_pos_y_bit_count_minus1(), 5);

  bitstream.putFlag(afps_lod_mode_enabled_flag());
  bitstream.putFlag(afps_raw_3d_pos_bit_count_explicit_mode_flag());
  bitstream.putFlag(afps_extension_present_flag());

  if (afps_extension_present_flag()) {
    bitstream.putFlag(afps_vpcc_extension_flag());
    bitstream.putFlag(afps_miv_extension_flag());
    bitstream.writeBits(afps_extension_6bits(), 6);
  }
  if (afps_vpcc_extension_flag()) {
    afps_vpcc_extension().encodeTo(bitstream);
  }
  if (afps_miv_extension_flag()) {
    afps_miv_extension().encodeTo(bitstream);
  }
  if (afps_extension_6bits() != 0) {
    for (auto bit : afpsExtensionData()) {
      bitstream.putFlag(bit);
    }
  }
  bitstream.rbspTrailingBits();
}
} // namespace TMIV::MivBitstream
