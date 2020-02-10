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

#include "test.h"

#include <TMIV/VpccBitstream/AtlasTileGroupLayerRBSP.h>

using namespace TMIV::VpccBitstream;

TEST_CASE("atlas_tile_group_header", "[Atlas Tile Group Layer RBSP]") {
  auto asps = AtlasSequenceParameterSetRBSP{};
  asps.asps_num_ref_atlas_frame_lists_in_asps(1).asps_log2_patch_packing_block_size(7);

  const auto afps = AtlasFrameParameterSetRBSP{};

  auto x = AtlasTileGroupHeader{};
  x.atgh_patch_size_x_info_quantizer(asps.asps_log2_patch_packing_block_size())
      .atgh_patch_size_y_info_quantizer(asps.asps_log2_patch_packing_block_size());

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

    REQUIRE(bitCodingTest(x, 8, asps, afps));
  }

  SECTION("Example 2") {
    asps.asps_patch_size_quantizer_present_flag(true);

    x.atgh_type(AtghType::I_TILE_GRP)
        .atgh_patch_size_x_info_quantizer(6)
        .atgh_patch_size_y_info_quantizer(5);

    REQUIRE(toString(x) == R"(atgh_atlas_frame_parameter_set_id=0
atgh_address=0
atgh_type=I_TILE_GRP
atgh_atlas_frm_order_cnt_lsb=0
atgh_patch_size_x_info_quantizer=6
atgh_patch_size_y_info_quantizer=5
)");

    REQUIRE(bitCodingTest(x, 16, asps, afps));
  }
}

TEST_CASE("skip_patch_data_unit", "[Atlas Tile Group Layer RBSP]") {
  auto x = SkipPatchDataUnit{};
  REQUIRE(toString(x).empty());
  REQUIRE(bitCodingTest(x, 0));
}

TEST_CASE("patch_data_unit", "[Atlas Tile Group Layer RBSP]") {
  auto vps = VpccParameterSet{};
  vps.vps_atlas_count(1);
  const auto atlasId = 0;

  auto asps = AtlasSequenceParameterSetRBSP{};
  asps.asps_frame_width(4000);
  asps.asps_frame_height(2000);

  auto afps = AtlasFrameParameterSetRBSP{};
  afps.afps_2d_pos_x_bit_count(12);
  afps.afps_2d_pos_y_bit_count(11);

  const auto atgh = AtlasTileGroupHeader{};

  auto x = PatchDataUnit{};
  x.pdu_2d_size_x(1).pdu_2d_size_y(1);

  const auto previous = &x;

  REQUIRE(toString(x, 101) == R"(pdu_2d_pos_x( 101 )=0
pdu_2d_pos_y( 101 )=0
pdu_2d_size_x( 101 )=1
pdu_2d_size_y( 101 )=1
pdu_3d_pos_x( 101 )=0
pdu_3d_pos_y( 101 )=0
pdu_3d_pos_min_z( 101 )=0
pdu_projection_id( 101 )=0
pdu_orientation_index( 101 )=FPO_NULL
)");

  REQUIRE(bitCodingTest(x, 29, vps, atlasId, asps, afps, atgh, previous));

  SECTION("Example") {
    vps.overridePduProjectionIdNumBits(9);
    vps.geometry_information(atlasId).gi_geometry_3d_coordinates_bitdepth(10);

    asps.asps_use_eight_orientations_flag(true).asps_normal_axis_max_delta_value_enabled_flag(true);

    afps.afps_3d_pos_x_bit_count(11).afps_3d_pos_y_bit_count(15).afps_lod_bit_count(4);

    auto previous = PatchDataUnit{};
    previous.pdu_2d_size_x(10).pdu_2d_size_y(12);
    const auto pointer = &previous;

    x.pdu_2d_pos_x(34)
        .pdu_2d_pos_y(57)
        .pdu_2d_size_x(1234)
        .pdu_2d_size_y(1002)
        .pdu_3d_pos_x(1234)
        .pdu_3d_pos_y(21345)
        .pdu_3d_pos_min_z(623)
        .pdu_3d_pos_delta_max_z(789)
        .pdu_projection_id(300)
        .pdu_orientation_index(FlexiblePatchOrientation::FPO_MROT180)
        .pdu_lod({13});

    REQUIRE(toString(x, 102) == R"(pdu_2d_pos_x( 102 )=34
pdu_2d_pos_y( 102 )=57
pdu_2d_size_x( 102 )=1234
pdu_2d_size_y( 102 )=1002
pdu_3d_pos_x( 102 )=1234
pdu_3d_pos_y( 102 )=21345
pdu_3d_pos_min_z( 102 )=623
pdu_3d_pos_delta_max_z( 102 )=789
pdu_projection_id( 102 )=300
pdu_orientation_index( 102 )=FPO_MROT180
pdu_lod( 102 )=13
)");

    REQUIRE(bitCodingTest(x, 127, vps, atlasId, asps, afps, atgh, pointer));
  }
}

TEST_CASE("patch_information_data", "[Atlas Tile Group Layer RBSP]") {
  auto vps = VpccParameterSet{};
  vps.vps_atlas_count(1);
  const auto atlasId = 0;

  auto asps = AtlasSequenceParameterSetRBSP{};
  asps.asps_frame_width(4000);
  asps.asps_frame_height(2000);

  auto afps = AtlasFrameParameterSetRBSP{};
  afps.afps_2d_pos_x_bit_count(12);
  afps.afps_2d_pos_y_bit_count(11);

  const auto atgh = AtlasTileGroupHeader{};

  auto pdu = PatchDataUnit{};
  pdu.pdu_2d_size_x(1).pdu_2d_size_y(1);

  const auto previous = &pdu;

  auto x = PatchInformationData{};

  REQUIRE(toString(x, 77) == R"([unknown]
)");

  SECTION("Example skip_patch_data_unit") {
    const auto atghType = AtghType::SKIP_TILE_GRP;
    const auto patchMode = AtgduPatchMode::P_SKIP;
    x = PatchInformationData{SkipPatchDataUnit{}};

    REQUIRE(toString(x, 88).empty());
    REQUIRE(bitCodingTest(x, 0, atghType, patchMode, vps, atlasId, asps, afps, atgh, previous));
  }

  SECTION("Example patch_data_unit") {
    const auto atghType = AtghType::I_TILE_GRP;
    const auto patchMode = AtgduPatchMode::I_INTRA;
    x = PatchInformationData{pdu};

    REQUIRE(toString(x, 99) == R"(pdu_2d_pos_x( 99 )=0
pdu_2d_pos_y( 99 )=0
pdu_2d_size_x( 99 )=1
pdu_2d_size_y( 99 )=1
pdu_3d_pos_x( 99 )=0
pdu_3d_pos_y( 99 )=0
pdu_3d_pos_min_z( 99 )=0
pdu_projection_id( 99 )=0
pdu_orientation_index( 99 )=FPO_NULL
)");
    REQUIRE(bitCodingTest(x, 29, atghType, patchMode, vps, atlasId, asps, afps, atgh, previous));
  }
}

TEST_CASE("atlas_tile_group_data_unit", "[Atlas Tile Group Layer RBSP]") {
  SECTION("Empty") {
    const auto atghType = AtghType::I_TILE_GRP;
    const auto x = AtlasTileGroupDataUnit{};
    REQUIRE(toString(x, atghType).empty());

    auto vps = VpccParameterSet{};
    vps.vps_atlas_count(1);
    const auto atlasId = 0;
    const auto asps = AtlasSequenceParameterSetRBSP{};
    const auto afps = AtlasFrameParameterSetRBSP{};
    const auto atgh = AtlasTileGroupHeader{};
    REQUIRE(bitCodingTest(x, 8, atghType, vps, atlasId, asps, afps, atgh));
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
pdu_2d_size_x( 2 )=0
pdu_2d_size_y( 2 )=0
pdu_3d_pos_x( 2 )=0
pdu_3d_pos_y( 2 )=0
pdu_3d_pos_min_z( 2 )=0
pdu_projection_id( 2 )=0
pdu_orientation_index( 2 )=FPO_NULL
atgdu_patch_mode[ 3 ]=P_SKIP
)");
  }

  SECTION("I_TILE_GRP") {
    auto pdu = PatchDataUnit{};
    pdu.pdu_2d_size_x(1).pdu_2d_size_y(1);

    auto vec = AtlasTileGroupDataUnit::Vector{{AtgduPatchMode::I_INTRA, PatchInformationData{pdu}},
                                              {AtgduPatchMode::I_INTRA, PatchInformationData{pdu}}};

    const auto x = AtlasTileGroupDataUnit{vec};
    REQUIRE(toString(x, AtghType::I_TILE_GRP) == R"(atgdu_patch_mode[ 0 ]=I_INTRA
pdu_2d_pos_x( 0 )=0
pdu_2d_pos_y( 0 )=0
pdu_2d_size_x( 0 )=1
pdu_2d_size_y( 0 )=1
pdu_3d_pos_x( 0 )=0
pdu_3d_pos_y( 0 )=0
pdu_3d_pos_min_z( 0 )=0
pdu_projection_id( 0 )=0
pdu_orientation_index( 0 )=FPO_NULL
atgdu_patch_mode[ 1 ]=I_INTRA
pdu_2d_pos_x( 1 )=0
pdu_2d_pos_y( 1 )=0
pdu_2d_size_x( 1 )=1
pdu_2d_size_y( 1 )=1
pdu_3d_pos_x( 1 )=0
pdu_3d_pos_y( 1 )=0
pdu_3d_pos_min_z( 1 )=0
pdu_projection_id( 1 )=0
pdu_orientation_index( 1 )=FPO_NULL
)");

    auto vps = VpccParameterSet{};
    vps.vps_atlas_count(1);
    const auto atlasId = 0;

    auto asps = AtlasSequenceParameterSetRBSP{};
    asps.asps_frame_width(4000);
    asps.asps_frame_height(2000);

    auto afps = AtlasFrameParameterSetRBSP{};
    afps.afps_2d_pos_x_bit_count(12);
    afps.afps_2d_pos_y_bit_count(11);

    const auto atgh = AtlasTileGroupHeader{};

    REQUIRE(bitCodingTest(x, 72, AtghType::I_TILE_GRP, vps, atlasId, asps, afps, atgh));
  }
}

TEST_CASE("atlas_tile_group_layer_rbsp", "[Atlas Tile Group Layer RBSP]") {
  SECTION("SKIP_TILE_GRP") {
    auto vps = VpccParameterSet{};
    vps.vps_atlas_count(1);
    const auto atlasId = 0;

    auto asps = AtlasSequenceParameterSetRBSP{};
    asps.asps_frame_width(4000).asps_frame_height(2000).asps_num_ref_atlas_frame_lists_in_asps(1);

    auto afps = AtlasFrameParameterSetRBSP{};
    afps.afps_2d_pos_x_bit_count(12).afps_2d_pos_y_bit_count(11);

    auto atgh = AtlasTileGroupHeader{};
    atgh.atgh_type(AtghType::SKIP_TILE_GRP);

    auto x = AtlasTileGroupLayerRBSP{atgh};

    REQUIRE(toString(x) == R"(atgh_atlas_frame_parameter_set_id=0
atgh_address=0
atgh_type=SKIP_TILE_GRP
atgh_atlas_frm_order_cnt_lsb=0
)");
    REQUIRE(byteCodingTest(x, 2, vps, atlasId, asps, afps, atgh));
  }

  SECTION("I_TILE_GRP") {
    auto vps = VpccParameterSet{};
    vps.vps_atlas_count(1);
    const auto atlasId = 0;

    auto asps = AtlasSequenceParameterSetRBSP{};
    asps.asps_frame_width(4000).asps_frame_height(2000).asps_num_ref_atlas_frame_lists_in_asps(1);

    auto afps = AtlasFrameParameterSetRBSP{};
    afps.afps_2d_pos_x_bit_count(12).afps_2d_pos_y_bit_count(11);

    auto atgh = AtlasTileGroupHeader{};
    atgh.atgh_type(AtghType::I_TILE_GRP);

    auto pdu1 = PatchDataUnit{};
    pdu1.pdu_2d_size_x(10).pdu_2d_size_y(20);
    auto pdu2 = PatchDataUnit{};
    pdu2.pdu_2d_size_x(30).pdu_2d_size_y(40);
    auto pdu3 = PatchDataUnit{};
    pdu3.pdu_2d_size_x(50).pdu_2d_size_y(60);

    auto x = AtlasTileGroupLayerRBSP{
        atgh, std::in_place, std::pair{AtgduPatchMode::I_INTRA, PatchInformationData{pdu1}},
        std::pair{AtgduPatchMode::I_INTRA, PatchInformationData{pdu2}},
        std::pair{AtgduPatchMode::I_INTRA, PatchInformationData{pdu3}}};

    REQUIRE(toString(x) == R"(atgh_atlas_frame_parameter_set_id=0
atgh_address=0
atgh_type=I_TILE_GRP
atgh_atlas_frm_order_cnt_lsb=0
atgh_patch_size_x_info_quantizer=0
atgh_patch_size_y_info_quantizer=0
atgdu_patch_mode[ 0 ]=I_INTRA
pdu_2d_pos_x( 0 )=0
pdu_2d_pos_y( 0 )=0
pdu_2d_size_x( 0 )=10
pdu_2d_size_y( 0 )=20
pdu_3d_pos_x( 0 )=0
pdu_3d_pos_y( 0 )=0
pdu_3d_pos_min_z( 0 )=0
pdu_projection_id( 0 )=0
pdu_orientation_index( 0 )=FPO_NULL
atgdu_patch_mode[ 1 ]=I_INTRA
pdu_2d_pos_x( 1 )=0
pdu_2d_pos_y( 1 )=0
pdu_2d_size_x( 1 )=30
pdu_2d_size_y( 1 )=40
pdu_3d_pos_x( 1 )=0
pdu_3d_pos_y( 1 )=0
pdu_3d_pos_min_z( 1 )=0
pdu_projection_id( 1 )=0
pdu_orientation_index( 1 )=FPO_NULL
atgdu_patch_mode[ 2 ]=I_INTRA
pdu_2d_pos_x( 2 )=0
pdu_2d_pos_y( 2 )=0
pdu_2d_size_x( 2 )=50
pdu_2d_size_y( 2 )=60
pdu_3d_pos_x( 2 )=0
pdu_3d_pos_y( 2 )=0
pdu_3d_pos_min_z( 2 )=0
pdu_projection_id( 2 )=0
pdu_orientation_index( 2 )=FPO_NULL
)");
    REQUIRE(byteCodingTest(x, 20, vps, atlasId, asps, afps, atgh));
  }
}
