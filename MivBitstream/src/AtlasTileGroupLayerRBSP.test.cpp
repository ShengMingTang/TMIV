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

#include "test.h"

#include <TMIV/MivBitstream/AtlasTileGroupLayerRBSP.h>

using namespace TMIV::MivBitstream;

TEST_CASE("atlas_tile_group_header", "[Atlas Tile Group Layer RBSP]") {
  auto aspsV = std::vector<AtlasSequenceParameterSetRBSP>(1);
  aspsV.front().asps_num_ref_atlas_frame_lists_in_asps(1).asps_log2_patch_packing_block_size(7);

  auto afpsV = std::vector<AtlasFrameParameterSetRBSP>(1);
  afpsV.front().afps_fixed_camera_model_flag(true);

  auto x = AtlasTileGroupHeader{};
  x.atgh_patch_size_x_info_quantizer(aspsV.front().asps_log2_patch_packing_block_size())
      .atgh_patch_size_y_info_quantizer(aspsV.front().asps_log2_patch_packing_block_size());

  REQUIRE(toString(x) == R"(atgh_atlas_frame_parameter_set_id=0
atgh_address=0
atgh_type=P_TILE_GRP
atgh_atlas_frm_order_cnt_lsb=0
atgh_patch_size_x_info_quantizer=7
atgh_patch_size_y_info_quantizer=7
)");

  SECTION("Example 1") {
    x.atgh_type(AtghType::SKIP_TILE_GRP);

    REQUIRE(toString(x) == R"(atgh_atlas_frame_parameter_set_id=0
atgh_address=0
atgh_type=SKIP_TILE_GRP
atgh_atlas_frm_order_cnt_lsb=0
)");

    REQUIRE(bitCodingTest(x, 16, aspsV, afpsV));
  }

  SECTION("Example 2") {
    aspsV.front().asps_patch_size_quantizer_present_flag(true);
    afpsV.front().afps_fixed_camera_model_flag(false);

    x.atgh_type(AtghType::I_TILE_GRP)
        .atgh_patch_size_x_info_quantizer(6)
        .atgh_patch_size_y_info_quantizer(5)
        .atgh_adaptation_parameter_set_id(4);

    REQUIRE(toString(x) == R"(atgh_atlas_frame_parameter_set_id=0
atgh_adaptation_parameter_set_id=4
atgh_address=0
atgh_type=I_TILE_GRP
atgh_atlas_frm_order_cnt_lsb=0
atgh_patch_size_x_info_quantizer=6
atgh_patch_size_y_info_quantizer=5
)");

    REQUIRE(bitCodingTest(x, 24, aspsV, afpsV));
  }
}

TEST_CASE("skip_patch_data_unit", "[Atlas Tile Group Layer RBSP]") {
  auto x = SkipPatchDataUnit{};
  REQUIRE(toString(x).empty());
  REQUIRE(bitCodingTest(x, 0));
}

TEST_CASE("patch_data_unit", "[Atlas Tile Group Layer RBSP]") {
  const auto vuh = VpccUnitHeader{VuhUnitType::VPCC_AD};

  auto vps = VpccParameterSet{};
  vps.vps_extension_present_flag(true);

  auto aspsV = std::vector<AtlasSequenceParameterSetRBSP>(1);
  aspsV.front().asps_frame_width(4000).asps_frame_height(2000);

  auto afpsV = std::vector<AtlasFrameParameterSetRBSP>(1);
  afpsV.front().afps_2d_pos_x_bit_count_minus1(11).afps_2d_pos_y_bit_count_minus1(10);

  const auto atgh = AtlasTileGroupHeader{};

  auto x = PatchDataUnit{};

  REQUIRE(toString(x, 101) == R"(pdu_2d_pos_x( 101 )=0
pdu_2d_pos_y( 101 )=0
pdu_2d_size_x_minus1( 101 )=0
pdu_2d_size_y_minus1( 101 )=0
pdu_view_pos_x( 101 )=0
pdu_view_pos_y( 101 )=0
pdu_depth_start( 101 )=0
pdu_view_id( 101 )=0
pdu_orientation_index( 101 )=FPO_NULL
pdu_entity_id( 101 )=0
)");

  REQUIRE(bitCodingTest(x, 33, vuh, vps, aspsV, afpsV, atgh));

  SECTION("Example") {
    vps.geometry_information(vuh.vuh_atlas_id())
        .gi_geometry_3d_coordinates_bitdepth_minus1(9)
        .gi_geometry_nominal_2d_bitdepth_minus1(9);
    vps.vps_miv_extension_flag(true).miv_sequence_params().msp_max_entities_minus1(100);

    aspsV.front()
        .asps_use_eight_orientations_flag(true)
        .asps_normal_axis_max_delta_value_enabled_flag(true)
        .asps_extended_projection_enabled_flag(true)
        .asps_max_projections_minus1(511)
        .asps_extension_present_flag(true)
        .asps_miv_extension_present_flag(true)
        .miv_atlas_sequence_params()
        .masp_depth_occ_map_threshold_flag(true);

    afpsV.front().afps_3d_pos_x_bit_count_minus1(10).afps_3d_pos_y_bit_count_minus1(14);

    x.pdu_2d_pos_x(34)
        .pdu_2d_pos_y(57)
        .pdu_2d_size_x_minus1(1234)
        .pdu_2d_size_y_minus1(1002)
        .pdu_view_pos_x(1234)
        .pdu_view_pos_y(21345)
        .pdu_depth_start(623)
        .pdu_depth_end(789)
        .pdu_view_id(300)
        .pdu_orientation_index(FlexiblePatchOrientation::FPO_MROT180)
        .pdu_entity_id(35)
        .pdu_depth_occ_map_threshold(600);

    REQUIRE(toString(x, 102) == R"(pdu_2d_pos_x( 102 )=34
pdu_2d_pos_y( 102 )=57
pdu_2d_size_x_minus1( 102 )=1234
pdu_2d_size_y_minus1( 102 )=1002
pdu_view_pos_x( 102 )=1234
pdu_view_pos_y( 102 )=21345
pdu_depth_start( 102 )=623
pdu_depth_end( 102 )=789
pdu_view_id( 102 )=300
pdu_orientation_index( 102 )=FPO_MROT180
pdu_entity_id( 102 )=35
pdu_depth_occ_map_threshold( 102 )=600
)");

    REQUIRE(bitCodingTest(x, 140, vuh, vps, aspsV, afpsV, atgh));
  }
}

TEST_CASE("patch_information_data", "[Atlas Tile Group Layer RBSP]") {
  const auto vuh = VpccUnitHeader{VuhUnitType::VPCC_AD};

  auto vps = VpccParameterSet{};
  vps.vps_extension_present_flag(true);

  auto aspsV = std::vector<AtlasSequenceParameterSetRBSP>(1);
  aspsV.front().asps_frame_width(4000).asps_frame_height(2000);

  auto afpsV = std::vector<AtlasFrameParameterSetRBSP>(1);
  afpsV.front().afps_2d_pos_x_bit_count_minus1(11).afps_2d_pos_y_bit_count_minus1(10);

  const auto pdu = PatchDataUnit{};

  auto x = PatchInformationData{};

  REQUIRE(toString(x, 77) == R"([unknown]
)");

  SECTION("Example skip_patch_data_unit") {
    auto atgh = AtlasTileGroupHeader{};
    atgh.atgh_type(AtghType::SKIP_TILE_GRP);
    const auto patchMode = AtgduPatchMode::P_SKIP;
    x = PatchInformationData{SkipPatchDataUnit{}};

    REQUIRE(toString(x, 88).empty());
    REQUIRE(bitCodingTest(x, 0, vuh, vps, aspsV, afpsV, atgh, patchMode));
  }

  SECTION("Example patch_data_unit") {
    auto atgh = AtlasTileGroupHeader{};
    atgh.atgh_type(AtghType::I_TILE_GRP);
    const auto patchMode = AtgduPatchMode::I_INTRA;
    x = PatchInformationData{pdu};

    REQUIRE(toString(x, 99) == R"(pdu_2d_pos_x( 99 )=0
pdu_2d_pos_y( 99 )=0
pdu_2d_size_x_minus1( 99 )=0
pdu_2d_size_y_minus1( 99 )=0
pdu_view_pos_x( 99 )=0
pdu_view_pos_y( 99 )=0
pdu_depth_start( 99 )=0
pdu_view_id( 99 )=0
pdu_orientation_index( 99 )=FPO_NULL
pdu_entity_id( 99 )=0
)");
    REQUIRE(bitCodingTest(x, 33, vuh, vps, aspsV, afpsV, atgh, patchMode));
  }
}

TEST_CASE("atlas_tile_group_data_unit", "[Atlas Tile Group Layer RBSP]") {
  SECTION("Empty") {
    const auto vuh = VpccUnitHeader{VuhUnitType::VPCC_AD};

    auto atgh = AtlasTileGroupHeader{};
    atgh.atgh_type(AtghType::I_TILE_GRP);

    const auto x = AtlasTileGroupDataUnit{};
    REQUIRE(toString(x, atgh.atgh_type()).empty());

    const auto vps = VpccParameterSet{};

    const auto aspsV = std::vector<AtlasSequenceParameterSetRBSP>(1);
    const auto afpsV = std::vector<AtlasFrameParameterSetRBSP>(1);

    REQUIRE(bitCodingTest(x, 8, vuh, vps, aspsV, afpsV, atgh));
  }

  SECTION("P_TILE_GRP") {
    auto vec = AtlasTileGroupDataUnit::Vector{
        {AtgduPatchMode::P_SKIP, PatchInformationData{SkipPatchDataUnit{}}},
        {AtgduPatchMode::P_SKIP, PatchInformationData{SkipPatchDataUnit{}}},
        {AtgduPatchMode::P_INTRA, PatchInformationData{PatchDataUnit{}}},
        {AtgduPatchMode::P_SKIP, PatchInformationData{SkipPatchDataUnit{}}}};

    const auto x = AtlasTileGroupDataUnit{vec};
    REQUIRE(toString(x, AtghType::P_TILE_GRP) == R"(atgdu_patch_mode[ 0 ]=P_SKIP
atgdu_patch_mode[ 1 ]=P_SKIP
atgdu_patch_mode[ 2 ]=P_INTRA
pdu_2d_pos_x( 2 )=0
pdu_2d_pos_y( 2 )=0
pdu_2d_size_x_minus1( 2 )=0
pdu_2d_size_y_minus1( 2 )=0
pdu_view_pos_x( 2 )=0
pdu_view_pos_y( 2 )=0
pdu_depth_start( 2 )=0
pdu_view_id( 2 )=0
pdu_orientation_index( 2 )=FPO_NULL
pdu_entity_id( 2 )=0
atgdu_patch_mode[ 3 ]=P_SKIP
)");
  }

  SECTION("I_TILE_GRP") {
    const auto pdu = PatchDataUnit{};

    auto vec = AtlasTileGroupDataUnit::Vector{{AtgduPatchMode::I_INTRA, PatchInformationData{pdu}},
                                              {AtgduPatchMode::I_INTRA, PatchInformationData{pdu}}};

    const auto x = AtlasTileGroupDataUnit{vec};
    REQUIRE(toString(x, AtghType::I_TILE_GRP) == R"(atgdu_patch_mode[ 0 ]=I_INTRA
pdu_2d_pos_x( 0 )=0
pdu_2d_pos_y( 0 )=0
pdu_2d_size_x_minus1( 0 )=0
pdu_2d_size_y_minus1( 0 )=0
pdu_view_pos_x( 0 )=0
pdu_view_pos_y( 0 )=0
pdu_depth_start( 0 )=0
pdu_view_id( 0 )=0
pdu_orientation_index( 0 )=FPO_NULL
pdu_entity_id( 0 )=0
atgdu_patch_mode[ 1 ]=I_INTRA
pdu_2d_pos_x( 1 )=0
pdu_2d_pos_y( 1 )=0
pdu_2d_size_x_minus1( 1 )=0
pdu_2d_size_y_minus1( 1 )=0
pdu_view_pos_x( 1 )=0
pdu_view_pos_y( 1 )=0
pdu_depth_start( 1 )=0
pdu_view_id( 1 )=0
pdu_orientation_index( 1 )=FPO_NULL
pdu_entity_id( 1 )=0
)");

    const auto vuh = VpccUnitHeader{VuhUnitType::VPCC_AD};

    auto vps = VpccParameterSet{};
    vps.vps_atlas_count_minus1(1).vps_extension_present_flag(true);

    auto aspsV = std::vector<AtlasSequenceParameterSetRBSP>(1);
    aspsV.front().asps_frame_width(4000).asps_frame_height(2000);

    auto afpsV = std::vector<AtlasFrameParameterSetRBSP>(1);
    afpsV.front().afps_2d_pos_x_bit_count_minus1(11).afps_2d_pos_y_bit_count_minus1(10);

    auto atgh = AtlasTileGroupHeader{};
    atgh.atgh_type(AtghType::I_TILE_GRP);

    REQUIRE(bitCodingTest(x, 80, vuh, vps, aspsV, afpsV, atgh));
  }
}

TEST_CASE("atlas_tile_group_layer_rbsp", "[Atlas Tile Group Layer RBSP]") {
  SECTION("SKIP_TILE_GRP") {
    const auto vuh = VpccUnitHeader{VuhUnitType::VPCC_AD};

    const auto vps = VpccParameterSet{};

    auto aspsV = std::vector<AtlasSequenceParameterSetRBSP>(1);
    aspsV.front()
        .asps_frame_width(4000)
        .asps_frame_height(2000)
        .asps_num_ref_atlas_frame_lists_in_asps(1);

    auto afpsV = std::vector<AtlasFrameParameterSetRBSP>(1);
    afpsV.front()
        .afps_2d_pos_x_bit_count_minus1(11)
        .afps_2d_pos_y_bit_count_minus1(10)
        .afps_fixed_camera_model_flag(true);

    auto atgh = AtlasTileGroupHeader{};
    atgh.atgh_type(AtghType::SKIP_TILE_GRP);

    const auto x = AtlasTileGroupLayerRBSP{atgh};

    REQUIRE(toString(x) == R"(atgh_atlas_frame_parameter_set_id=0
atgh_address=0
atgh_type=SKIP_TILE_GRP
atgh_atlas_frm_order_cnt_lsb=0
)");
    REQUIRE(byteCodingTest(x, 3, vuh, vps, aspsV, afpsV));
  }

  SECTION("I_TILE_GRP") {
    const auto vuh = VpccUnitHeader{VuhUnitType::VPCC_AD};

    auto vps = VpccParameterSet{};
    vps.vps_extension_present_flag(true);

    auto aspsV = std::vector<AtlasSequenceParameterSetRBSP>(1);
    aspsV.front()
        .asps_frame_width(4000)
        .asps_frame_height(2000)
        .asps_num_ref_atlas_frame_lists_in_asps(1);

    auto afpsV = std::vector<AtlasFrameParameterSetRBSP>(1);
    afpsV.front().afps_2d_pos_x_bit_count_minus1(11).afps_2d_pos_y_bit_count_minus1(10);

    auto atgh = AtlasTileGroupHeader{};
    atgh.atgh_type(AtghType::I_TILE_GRP);
    atgh.atgh_adaptation_parameter_set_id(0);

    auto pdu1 = PatchDataUnit{};
    pdu1.pdu_2d_size_x_minus1(10).pdu_2d_size_y_minus1(20);
    auto pdu2 = PatchDataUnit{};
    pdu2.pdu_2d_size_x_minus1(30).pdu_2d_size_y_minus1(40);
    auto pdu3 = PatchDataUnit{};
    pdu3.pdu_2d_size_x_minus1(50).pdu_2d_size_y_minus1(60);

    const auto x = AtlasTileGroupLayerRBSP{
        atgh, std::in_place, std::pair{AtgduPatchMode::I_INTRA, PatchInformationData{pdu1}},
        std::pair{AtgduPatchMode::I_INTRA, PatchInformationData{pdu2}},
        std::pair{AtgduPatchMode::I_INTRA, PatchInformationData{pdu3}}};

    REQUIRE(toString(x) == R"(atgh_atlas_frame_parameter_set_id=0
atgh_adaptation_parameter_set_id=0
atgh_address=0
atgh_type=I_TILE_GRP
atgh_atlas_frm_order_cnt_lsb=0
atgh_patch_size_x_info_quantizer=0
atgh_patch_size_y_info_quantizer=0
atgdu_patch_mode[ 0 ]=I_INTRA
pdu_2d_pos_x( 0 )=0
pdu_2d_pos_y( 0 )=0
pdu_2d_size_x_minus1( 0 )=10
pdu_2d_size_y_minus1( 0 )=20
pdu_view_pos_x( 0 )=0
pdu_view_pos_y( 0 )=0
pdu_depth_start( 0 )=0
pdu_view_id( 0 )=0
pdu_orientation_index( 0 )=FPO_NULL
pdu_entity_id( 0 )=0
atgdu_patch_mode[ 1 ]=I_INTRA
pdu_2d_pos_x( 1 )=0
pdu_2d_pos_y( 1 )=0
pdu_2d_size_x_minus1( 1 )=30
pdu_2d_size_y_minus1( 1 )=40
pdu_view_pos_x( 1 )=0
pdu_view_pos_y( 1 )=0
pdu_depth_start( 1 )=0
pdu_view_id( 1 )=0
pdu_orientation_index( 1 )=FPO_NULL
pdu_entity_id( 1 )=0
atgdu_patch_mode[ 2 ]=I_INTRA
pdu_2d_pos_x( 2 )=0
pdu_2d_pos_y( 2 )=0
pdu_2d_size_x_minus1( 2 )=50
pdu_2d_size_y_minus1( 2 )=60
pdu_view_pos_x( 2 )=0
pdu_view_pos_y( 2 )=0
pdu_depth_start( 2 )=0
pdu_view_id( 2 )=0
pdu_orientation_index( 2 )=FPO_NULL
pdu_entity_id( 2 )=0
)");
    REQUIRE(byteCodingTest(x, 24, vuh, vps, aspsV, afpsV));
  }
}
