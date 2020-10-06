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

#ifndef _TMIV_MIVBITSTREAM_V3CPARAMETERSET_H_
#error "Include the .h, not the .hpp"
#endif

#include <TMIV/MivBitstream/verify.h>

namespace TMIV::MivBitstream {
constexpr auto ProfileTierLevel::ptl_tier_flag() const noexcept { return m_ptl_tier_flag; }

constexpr auto ProfileTierLevel::ptl_profile_codec_group_idc() const noexcept {
  return m_ptl_profile_codec_group_idc;
}

constexpr auto ProfileTierLevel::ptl_profile_toolset_idc() const noexcept {
  return m_ptl_profile_toolset_idc;
}

constexpr auto ProfileTierLevel::ptl_profile_reconstruction_idc() const noexcept {
  return m_ptl_profile_reconstruction_idc;
}

constexpr auto ProfileTierLevel::ptl_max_decodes_idc() const noexcept {
  return m_ptl_max_decodes_idc;
}

constexpr auto ProfileTierLevel::ptl_level_idc() const noexcept { return m_ptl_level_idc; }

constexpr auto ProfileTierLevel::ptl_extended_sub_profile_flag() const noexcept {
  return m_ptl_extended_sub_profile_flag;
}

constexpr auto ProfileTierLevel::ptl_tool_constraints_present_flag() const noexcept {
  return m_ptl_tool_constraints_present_flag;
}

constexpr auto ProfileTierLevel::ptl_tier_flag(bool value) noexcept -> auto & {
  m_ptl_tier_flag = value;
  return *this;
}

constexpr auto ProfileTierLevel::ptl_profile_codec_group_idc(PtlProfileCodecGroupIdc value) noexcept
    -> auto & {
  m_ptl_profile_codec_group_idc = value;
  return *this;
}

constexpr auto ProfileTierLevel::ptl_profile_toolset_idc(PtlProfilePccToolsetIdc value) noexcept
    -> auto & {
  m_ptl_profile_toolset_idc = value;
  return *this;
}

constexpr auto
ProfileTierLevel::ptl_profile_reconstruction_idc(PtlProfileReconstructionIdc value) noexcept
    -> auto & {
  m_ptl_profile_reconstruction_idc = value;
  return *this;
}

constexpr auto ProfileTierLevel::ptl_max_decodes_idc(PtlMaxDecodesIdc value) noexcept -> auto & {
  m_ptl_max_decodes_idc = value;
  return *this;
}

constexpr auto ProfileTierLevel::ptl_level_idc(PtlLevelIdc value) noexcept -> auto & {
  m_ptl_level_idc = value;
  return *this;
}

constexpr auto ProfileTierLevel::ptl_tool_constraints_present_flag(bool value) noexcept -> auto & {
  m_ptl_tool_constraints_present_flag = value;
  return *this;
}

constexpr auto OccupancyInformation::oi_occupancy_codec_id() const noexcept {
  return m_oi_occupancy_codec_id;
}

constexpr auto OccupancyInformation::oi_lossy_occupancy_compression_threshold() const noexcept {
  return m_oi_lossy_occupancy_compression_threshold;
}

constexpr auto OccupancyInformation::oi_occupancy_2d_bit_depth_minus1() const noexcept {
  return m_oi_occupancy_2d_bit_depth_minus1;
}

constexpr auto OccupancyInformation::oi_occupancy_MSB_align_flag() const noexcept {
  return m_oi_occupancy_MSB_align_flag;
}

constexpr auto OccupancyInformation::oi_occupancy_codec_id(std::uint8_t value) noexcept -> auto & {
  m_oi_occupancy_codec_id = value;
  return *this;
}

constexpr auto
OccupancyInformation::oi_lossy_occupancy_compression_threshold(std::uint8_t value) noexcept
    -> auto & {
  m_oi_lossy_occupancy_compression_threshold = value;
  return *this;
}

constexpr auto OccupancyInformation::oi_occupancy_2d_bit_depth_minus1(std::uint8_t value) noexcept
    -> auto & {
  m_oi_occupancy_2d_bit_depth_minus1 = value;
  return *this;
}

constexpr auto OccupancyInformation::oi_occupancy_MSB_align_flag(bool value) noexcept -> auto & {
  m_oi_occupancy_MSB_align_flag = value;
  return *this;
}

constexpr auto GeometryInformation::gi_geometry_codec_id() const noexcept {
  return m_gi_geometry_codec_id;
}

constexpr auto GeometryInformation::gi_geometry_2d_bit_depth_minus1() const noexcept {
  return m_gi_geometry_2d_bit_depth_minus1;
}

constexpr auto GeometryInformation::gi_geometry_MSB_align_flag() const noexcept {
  return m_gi_geometry_MSB_align_flag;
}

constexpr auto GeometryInformation::gi_geometry_3d_coordinates_bit_depth_minus1() const noexcept {
  return m_gi_geometry_3d_coordinates_bit_depth_minus1;
}

constexpr auto GeometryInformation::gi_geometry_codec_id(std::uint8_t value) noexcept -> auto & {
  m_gi_geometry_codec_id = value;
  return *this;
}

constexpr auto GeometryInformation::gi_geometry_2d_bit_depth_minus1(std::uint8_t value) noexcept
    -> auto & {
  m_gi_geometry_2d_bit_depth_minus1 = value;
  return *this;
}

constexpr auto GeometryInformation::gi_geometry_MSB_align_flag(bool value) noexcept -> auto & {
  m_gi_geometry_MSB_align_flag = value;
  return *this;
}

constexpr auto
GeometryInformation::gi_geometry_3d_coordinates_bit_depth_minus1(std::uint8_t value) noexcept
    -> auto & {
  m_gi_geometry_3d_coordinates_bit_depth_minus1 = value;
  return *this;
}

constexpr auto PackingInformation::pin_codec_id(std::uint8_t value) noexcept -> auto & {
  m_pin_codec_id = value;
  return *this;
}

inline auto PackingInformation::pin_regions_count_minus1(std::size_t value) -> auto & {
  m_pinRegions = std::vector<PinRegion>(value + 1U);
  return *this;
}

inline auto PackingInformation::pin_region_tile_id(std::size_t i, std::uint8_t value) -> auto & {
  VERIFY_V3CBITSTREAM(i <= pin_regions_count_minus1());
  m_pinRegions[i].pin_region_tile_id = value;
  return *this;
}

inline auto PackingInformation::pin_region_type_id_minus2(std::size_t i, VuhUnitType value)
    -> auto & {
  VERIFY_V3CBITSTREAM(i <= pin_regions_count_minus1());
  m_pinRegions[i].pin_region_type_id_minus2 = value;
  return *this;
}

inline auto PackingInformation::pin_region_top_left_x(std::size_t i, std::uint16_t value)
    -> auto & {
  VERIFY_V3CBITSTREAM(i <= pin_regions_count_minus1());
  m_pinRegions[i].pin_region_top_left_x = value;
  return *this;
}

inline auto PackingInformation::pin_region_top_left_y(std::size_t i, std::uint16_t value)
    -> auto & {
  VERIFY_V3CBITSTREAM(i <= pin_regions_count_minus1());
  m_pinRegions[i].pin_region_top_left_y = value;
  return *this;
}

inline auto PackingInformation::pin_region_width_minus1(std::size_t i, std::uint16_t value)
    -> auto & {
  VERIFY_V3CBITSTREAM(i <= pin_regions_count_minus1());
  m_pinRegions[i].pin_region_width_minus1 = value;
  return *this;
}

inline auto PackingInformation::pin_region_height_minus1(std::size_t i, std::uint16_t value)
    -> auto & {
  VERIFY_V3CBITSTREAM(i <= pin_regions_count_minus1());
  m_pinRegions[i].pin_region_height_minus1 = value;
  return *this;
}

inline auto PackingInformation::pin_region_map_index(std::size_t i, std::uint8_t value) -> auto & {
  VERIFY_V3CBITSTREAM(i <= pin_regions_count_minus1());
  m_pinRegions[i].pin_region_map_index = value;
  return *this;
}

inline auto PackingInformation::pin_region_rotation_flag(std::size_t i, bool value) -> auto & {
  VERIFY_V3CBITSTREAM(i <= pin_regions_count_minus1());
  m_pinRegions[i].pin_region_rotation_flag = value;
  return *this;
}

inline auto PackingInformation::pin_region_auxiliary_data_flag(std::size_t i, bool value)
    -> auto & {
  VERIFY_V3CBITSTREAM(i <= pin_regions_count_minus1());
  m_pinRegions[i].pin_region_auxiliary_data_flag = value;
  return *this;
}

inline auto PackingInformation::pin_region_attr_type_id(std::size_t i, std::uint8_t value)
    -> auto & {
  VERIFY_V3CBITSTREAM(i <= pin_regions_count_minus1());
  m_pinRegions[i].pin_region_attr_type_id = value;
  return *this;
}

inline auto PackingInformation::pin_region_attr_partitions_flag(std::size_t i, bool value)
    -> auto & {
  VERIFY_V3CBITSTREAM(i <= pin_regions_count_minus1());
  m_pinRegions[i].pin_region_attr_partitions_flag = value;
  return *this;
}

inline auto PackingInformation::pin_region_attr_partition_index(std::size_t i, std::uint8_t value)
    -> auto & {
  VERIFY_V3CBITSTREAM(i <= pin_regions_count_minus1());
  m_pinRegions[i].pin_region_attr_partition_index = value;
  return *this;
}

inline auto PackingInformation::pin_region_attr_partitions_minus1(std::size_t i, std::uint8_t value)
    -> auto & {
  VERIFY_V3CBITSTREAM(i <= pin_regions_count_minus1());
  m_pinRegions[i].pin_region_attr_partitions_minus1 = value;
  return *this;
}

constexpr auto VpsMivExtension::vme_depth_low_quality_flag() const noexcept {
  return m_vme_depth_low_quality_flag;
}

constexpr auto VpsMivExtension::vme_geometry_scale_enabled_flag() const noexcept {
  return m_vme_geometry_scale_enabled_flag;
}

constexpr auto VpsMivExtension::vme_num_groups_minus1() const noexcept {
  return m_vme_num_groups_minus1;
}

constexpr auto VpsMivExtension::vme_max_entities_minus1() const noexcept {
  return m_vme_max_entities_minus1;
}

constexpr auto VpsMivExtension::vme_embedded_occupancy_flag() const noexcept {
  return m_vme_embedded_occupancy_flag;
}

constexpr auto VpsMivExtension::vme_occupancy_scale_enabled_flag() const noexcept {
  return !vme_embedded_occupancy_flag() && m_vme_occupancy_scale_enabled_flag;
}

constexpr auto VpsMivExtension::vme_depth_low_quality_flag(const bool value) noexcept -> auto & {
  m_vme_depth_low_quality_flag = value;
  return *this;
}

constexpr auto VpsMivExtension::vme_geometry_scale_enabled_flag(const bool value) noexcept
    -> auto & {
  m_vme_geometry_scale_enabled_flag = value;
  return *this;
}

constexpr auto VpsMivExtension::vme_num_groups_minus1(const unsigned value) noexcept -> auto & {
  m_vme_num_groups_minus1 = value;
  return *this;
}
constexpr auto VpsMivExtension::vme_max_entities_minus1(const unsigned value) noexcept -> auto & {
  m_vme_max_entities_minus1 = value;
  return *this;
}

constexpr auto VpsMivExtension::vme_embedded_occupancy_flag(const bool value) noexcept -> auto & {
  m_vme_embedded_occupancy_flag = value;
  return *this;
}

constexpr auto VpsMivExtension::operator==(const VpsMivExtension &other) const noexcept {
  return vme_depth_low_quality_flag() == other.vme_depth_low_quality_flag() &&
         vme_geometry_scale_enabled_flag() == other.vme_geometry_scale_enabled_flag() &&
         vme_num_groups_minus1() == other.vme_num_groups_minus1() &&
         vme_max_entities_minus1() == other.vme_max_entities_minus1() &&
         vme_embedded_occupancy_flag() == other.vme_embedded_occupancy_flag() &&
         vme_occupancy_scale_enabled_flag() == other.vme_occupancy_scale_enabled_flag();
}

constexpr auto VpsMivExtension::operator!=(const VpsMivExtension &other) const noexcept {
  return !operator==(other);
}

constexpr auto V3cParameterSet::vps_v3c_parameter_set_id() const noexcept {
  return m_vps_v3c_parameter_set_id;
}

constexpr auto V3cParameterSet::vps_extension_present_flag() const noexcept {
  return m_vps_extension_present_flag;
}

constexpr auto V3cParameterSet::vps_miv_extension_present_flag() const noexcept {
  return m_vps_miv_extension_present_flag.value_or(false);
}

constexpr auto V3cParameterSet::vps_extension_7bits() const noexcept {
  return m_vps_extension_7bits.value_or(0);
}

constexpr auto V3cParameterSet::vps_v3c_parameter_set_id(std::uint8_t value) noexcept -> auto & {
  m_vps_v3c_parameter_set_id = value;
  return *this;
}

constexpr auto V3cParameterSet::vps_extension_present_flag(bool value) noexcept -> auto & {
  m_vps_extension_present_flag = value;
  return *this;
}

constexpr auto V3cParameterSet::profile_tier_level() noexcept -> auto & {
  return m_profile_tier_level;
}
} // namespace TMIV::MivBitstream
