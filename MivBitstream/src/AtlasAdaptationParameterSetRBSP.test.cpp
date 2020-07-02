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

#include <TMIV/MivBitstream/AtlasAdaptationParameterSetRBSP.h>

using namespace TMIV::MivBitstream;

TEST_CASE("camera_intrinsics", "[Atlas Adaptation Parameter Set RBSP]") {
  auto x = CameraIntrinsics{};

  SECTION("equirectangular") {
    x.ci_cam_type(CiCamType::equirectangular)
        .ci_erp_phi_min(-2.F)
        .ci_erp_phi_max(2.F)
        .ci_erp_theta_min(-1.F)
        .ci_erp_theta_max(1.F);

    REQUIRE(toString(x, 1) == R"(ci_cam_type[ 1 ]=equirectangular
ci_projection_plane_width_minus1[ 1 ]=0
ci_projection_plane_height_minus1[ 1 ]=0
ci_erp_phi_min[ 1 ]=-2
ci_erp_phi_max[ 1 ]=2
ci_erp_theta_min[ 1 ]=-1
ci_erp_theta_max[ 1 ]=1
)");

    REQUIRE(bitCodingTest(x, 168));
  }

  SECTION("perspective") {
    x.ci_cam_type(CiCamType::perspective)
        .ci_projection_plane_width_minus1(19)
        .ci_projection_plane_height_minus1(9)
        .ci_perspective_focal_hor(50.F)
        .ci_perspective_focal_ver(25.F)
        .ci_perspective_center_hor(10.F)
        .ci_perspective_center_ver(5.F);

    REQUIRE(toString(x, 1) == R"(ci_cam_type[ 1 ]=perspective
ci_projection_plane_width_minus1[ 1 ]=19
ci_projection_plane_height_minus1[ 1 ]=9
ci_perspective_focal_hor[ 1 ]=50
ci_perspective_focal_ver[ 1 ]=25
ci_perspective_center_hor[ 1 ]=10
ci_perspective_center_ver[ 1 ]=5
)");

    REQUIRE(bitCodingTest(x, 168));
  }

  SECTION("orthographic") {
    x.ci_cam_type(CiCamType::orthographic)
        .ci_projection_plane_width_minus1(1023)
        .ci_projection_plane_height_minus1(767)
        .ci_ortho_width(100.F)
        .ci_ortho_height(50.F);

    REQUIRE(toString(x, 1) == R"(ci_cam_type[ 1 ]=orthographic
ci_projection_plane_width_minus1[ 1 ]=1023
ci_projection_plane_height_minus1[ 1 ]=767
ci_ortho_width[ 1 ]=100
ci_ortho_height[ 1 ]=50
)");

    REQUIRE(bitCodingTest(x, 104));
  }
}

TEST_CASE("camera_extrinsics", "[Atlas Adaptation Parameter Set RBSP]") {
  auto x = CameraExtrinsics{};

  REQUIRE(toString(x, 1) == R"(ce_view_pos_x[ 1 ]=0
ce_view_pos_y[ 1 ]=0
ce_view_pos_z[ 1 ]=0
ce_view_quat_x[ 1 ]=0
ce_view_quat_y[ 1 ]=0
ce_view_quat_z[ 1 ]=0
)");

  REQUIRE(bitCodingTest(x, 192));

  SECTION("Example") {
    x.ce_view_pos_x(3.F)
        .ce_view_pos_y(1.F)
        .ce_view_pos_z(4.F)
        .ce_view_quat_x(5.F)
        .ce_view_quat_y(9.F)
        .ce_view_quat_z(14.F);

    REQUIRE(toString(x, 1) == R"(ce_view_pos_x[ 1 ]=3
ce_view_pos_y[ 1 ]=1
ce_view_pos_z[ 1 ]=4
ce_view_quat_x[ 1 ]=5
ce_view_quat_y[ 1 ]=9
ce_view_quat_z[ 1 ]=14
)");

    REQUIRE(bitCodingTest(x, 192));
  }
}

TEST_CASE("depth_quantization", "[Atlas Adaptation Parameter Set RBSP]") {
  auto x = DepthQuantization{};

  REQUIRE(toString(x, 7) == R"(dq_quantization_law[ 7 ]=0
dq_norm_disp_low[ 7 ]=0
dq_norm_disp_high[ 7 ]=0
dq_depth_occ_map_threshold_default[ 7 ]=0
)");

  REQUIRE(bitCodingTest(x, 73));

  SECTION("Example 2") {
    x.dq_norm_disp_low(0.02F);
    x.dq_norm_disp_high(2.F);
    x.dq_depth_occ_map_threshold_default(200);

    REQUIRE(toString(x, 2) == R"(dq_quantization_law[ 2 ]=0
dq_norm_disp_low[ 2 ]=0.02
dq_norm_disp_high[ 2 ]=2
dq_depth_occ_map_threshold_default[ 2 ]=200
)");

    REQUIRE(bitCodingTest(x, 87));
  }
}

TEST_CASE("pruning_parent", "[Atlas Adaptation Parameter Set RBSP]") {
  SECTION("Example 1") {
    const auto x = PruningParent{};
    REQUIRE(toString(x, 3) == R"(pp_is_root_flag[ 3 ]=true
)");

    const uint16_t mvp_num_views_minus1 = 10;
    REQUIRE(bitCodingTest(x, 1, mvp_num_views_minus1));
  }

  SECTION("Example 2") {
    const auto x = PruningParent{{2, 3, 5, 8}};
    REQUIRE(toString(x, 5) == R"(pp_is_root_flag[ 5 ]=false
pp_num_parent_minus1[ 5 ]=3
pp_parent_id[ 5 ][ 0 ]=2
pp_parent_id[ 5 ][ 1 ]=3
pp_parent_id[ 5 ][ 2 ]=5
pp_parent_id[ 5 ][ 3 ]=8
)");

    const uint16_t mvp_num_views_minus1 = 10;
    REQUIRE(bitCodingTest(x, 21, mvp_num_views_minus1));
  }
}

TEST_CASE("miv_view_params_list", "[Atlas Adaptation Parameter Set RBSP]") {
  auto x = MivViewParamsList{};

  SECTION("Example 1") {
    x.mvp_num_views_minus1(0)
        .mvp_atlas_count_minus1(0)
        .mvp_view_enabled_in_atlas_flag(0,0,true)
        .mvp_view_complete_in_atlas_flag(0,0,true)
        .mvp_explicit_view_id_flag(false)
        .mvp_intrinsic_params_equal_flag(false)
        .mvp_depth_quantization_params_equal_flag(false)
        .mvp_pruning_graph_params_present_flag(false);
    x.camera_intrinsics(0)
        .ci_cam_type(CiCamType::orthographic)
        .ci_ortho_width(4.F)
        .ci_ortho_height(3.F);

    REQUIRE(toString(x) == R"(mvp_num_views_minus1=0
mvp_view_enabled_in_atlas_flag[ 0 ]=[ true ]
mvp_view_complete_in_atlas_flag[ 0 ]=[ true ]
mvp_explicit_view_id_flag=false
ce_view_pos_x[ 0 ]=0
ce_view_pos_y[ 0 ]=0
ce_view_pos_z[ 0 ]=0
ce_view_quat_x[ 0 ]=0
ce_view_quat_y[ 0 ]=0
ce_view_quat_z[ 0 ]=0
mvp_intrinsic_params_equal_flag=false
ci_cam_type[ 0 ]=orthographic
ci_projection_plane_width_minus1[ 0 ]=0
ci_projection_plane_height_minus1[ 0 ]=0
ci_ortho_width[ 0 ]=4
ci_ortho_height[ 0 ]=3
mvp_depth_quantization_params_equal_flag=false
dq_quantization_law[ 0 ]=0
dq_norm_disp_low[ 0 ]=0
dq_norm_disp_high[ 0 ]=0
dq_depth_occ_map_threshold_default[ 0 ]=0
mvp_pruning_graph_params_present_flag=false
)");

    REQUIRE(bitCodingTest_decodeArgs(x, 391, x.mvp_atlas_count_minus1()));
  }

  SECTION("Example 2") {
    x.mvp_num_views_minus1(2)
        .mvp_atlas_count_minus1(1)
        .mvp_view_enabled_in_atlas_flag(0, 0, true)
        .mvp_view_enabled_in_atlas_flag(0, 1, true)
        .mvp_view_enabled_in_atlas_flag(1, 2, true)
        .mvp_view_complete_in_atlas_flag(0, 0, true)
        .mvp_explicit_view_id_flag(true)
        .mvp_view_id(0, 0)
        .mvp_view_id(1, 2)
        .mvp_view_id(2, 1)
		.mvp_intrinsic_params_equal_flag(true)
        .mvp_depth_quantization_params_equal_flag(true)
        .mvp_pruning_graph_params_present_flag(true);
    x.camera_intrinsics(0)
        .ci_cam_type(CiCamType::orthographic)
        .ci_ortho_width(4.F)
        .ci_ortho_height(3.F);

    REQUIRE(toString(x) == R"(mvp_num_views_minus1=2
mvp_view_enabled_in_atlas_flag[ 0 ]=[ true true false ]
mvp_view_enabled_in_atlas_flag[ 1 ]=[ false false true ]
mvp_view_complete_in_atlas_flag[ 0 ]=[ true false false ]
mvp_view_complete_in_atlas_flag[ 1 ]=[ false false false ]
mvp_explicit_view_id_flag=true
mvp_view_id=[ 0 2 1 ]
ce_view_pos_x[ 0 ]=0
ce_view_pos_y[ 0 ]=0
ce_view_pos_z[ 0 ]=0
ce_view_quat_x[ 0 ]=0
ce_view_quat_y[ 0 ]=0
ce_view_quat_z[ 0 ]=0
ce_view_pos_x[ 1 ]=0
ce_view_pos_y[ 1 ]=0
ce_view_pos_z[ 1 ]=0
ce_view_quat_x[ 1 ]=0
ce_view_quat_y[ 1 ]=0
ce_view_quat_z[ 1 ]=0
ce_view_pos_x[ 2 ]=0
ce_view_pos_y[ 2 ]=0
ce_view_pos_z[ 2 ]=0
ce_view_quat_x[ 2 ]=0
ce_view_quat_y[ 2 ]=0
ce_view_quat_z[ 2 ]=0
mvp_intrinsic_params_equal_flag=true
ci_cam_type[ 0 ]=orthographic
ci_projection_plane_width_minus1[ 0 ]=0
ci_projection_plane_height_minus1[ 0 ]=0
ci_ortho_width[ 0 ]=4
ci_ortho_height[ 0 ]=3
mvp_depth_quantization_params_equal_flag=true
dq_quantization_law[ 0 ]=0
dq_norm_disp_low[ 0 ]=0
dq_norm_disp_high[ 0 ]=0
dq_depth_occ_map_threshold_default[ 0 ]=0
mvp_pruning_graph_params_present_flag=true
pp_is_root_flag[ 0 ]=true
pp_is_root_flag[ 1 ]=true
pp_is_root_flag[ 2 ]=true
)");

    REQUIRE(bitCodingTest_decodeArgs(x, 836, x.mvp_atlas_count_minus1()));
  }
}

TEST_CASE("atlas_adaptation_parameter_set_rbsp", "[Atlas Adaptation Parameter Set RBSP]") {
  auto x = AtlasAdaptationParameterSetRBSP{};

  REQUIRE(toString(x) == R"(aaps_atlas_adaptation_parameter_set_id=0
aaps_log2_max_afoc_present_flag=false
aaps_extension_present_flag=false
)");

  REQUIRE(byteCodingTest_decodeArgs(x, 1, 0));

  SECTION("Example 1") {
    x.aaps_atlas_adaptation_parameter_set_id(63)
        .aaps_log2_max_afoc_present_flag(true)
        .aaps_log2_max_atlas_frame_order_cnt_lsb_minus4(12)
        .aaps_extension_present_flag(true)
        .aaps_vpcc_extension_flag(true)
        .aaps_vpcc_extension({})
        .aaps_miv_extension_flag(true)
        .aaps_extension_6bits(63)
        .aapsExtensionData({true})
        .aaps_miv_extension()
        .aame_miv_view_params_list_update_mode(MvpUpdateMode::VPL_INITLIST)
        .miv_view_params_list()
        .mvp_num_views_minus1(2)
        .mvp_atlas_count_minus1(0)
        .mvp_view_enabled_in_atlas_flag(0, 0, false)
        .mvp_view_complete_in_atlas_flag(0, 0, false)
        .mvp_explicit_view_id_flag(false)
        .mvp_intrinsic_params_equal_flag(true)
        .mvp_depth_quantization_params_equal_flag(true)
        .mvp_pruning_graph_params_present_flag(true)
        .camera_intrinsics(0)
        .ci_cam_type(CiCamType::orthographic)
        .ci_ortho_width(4.F)
        .ci_ortho_height(3.F);

    REQUIRE(toString(x) == R"(aaps_atlas_adaptation_parameter_set_id=63
aaps_log2_max_afoc_present_flag=true
aaps_log2_max_atlas_frame_order_cnt_lsb_minus4=12
aaps_extension_present_flag=true
aaps_vpcc_extension_flag=true
aaps_miv_extension_flag=true
aaps_extension_6bits=63
aaps_vpcc_camera_parameters_present_flag=false
aame_omaf_v1_compatible_flag=false
aame_miv_view_params_list_update_mode=VPL_INITLIST
mvp_num_views_minus1=2
mvp_view_enabled_in_atlas_flag[ 0 ]=[ false false false ]
mvp_view_complete_in_atlas_flag[ 0 ]=[ false false false ]
mvp_explicit_view_id_flag=false
ce_view_pos_x[ 0 ]=0
ce_view_pos_y[ 0 ]=0
ce_view_pos_z[ 0 ]=0
ce_view_quat_x[ 0 ]=0
ce_view_quat_y[ 0 ]=0
ce_view_quat_z[ 0 ]=0
ce_view_pos_x[ 1 ]=0
ce_view_pos_y[ 1 ]=0
ce_view_pos_z[ 1 ]=0
ce_view_quat_x[ 1 ]=0
ce_view_quat_y[ 1 ]=0
ce_view_quat_z[ 1 ]=0
ce_view_pos_x[ 2 ]=0
ce_view_pos_y[ 2 ]=0
ce_view_pos_z[ 2 ]=0
ce_view_quat_x[ 2 ]=0
ce_view_quat_y[ 2 ]=0
ce_view_quat_z[ 2 ]=0
mvp_intrinsic_params_equal_flag=true
ci_cam_type[ 0 ]=orthographic
ci_projection_plane_width_minus1[ 0 ]=0
ci_projection_plane_height_minus1[ 0 ]=0
ci_ortho_width[ 0 ]=4
ci_ortho_height[ 0 ]=3
mvp_depth_quantization_params_equal_flag=true
dq_quantization_law[ 0 ]=0
dq_norm_disp_low[ 0 ]=0
dq_norm_disp_high[ 0 ]=0
dq_depth_occ_map_threshold_default[ 0 ]=0
mvp_pruning_graph_params_present_flag=true
pp_is_root_flag[ 0 ]=true
pp_is_root_flag[ 1 ]=true
pp_is_root_flag[ 2 ]=true
aaps_extension_data_flag=true
)");

    REQUIRE(byteCodingTest_decodeArgs(x, 103, x.aaps_miv_extension().miv_view_params_list().mvp_atlas_count_minus1()));
  }
}

TEST_CASE("miv_view_params_update_extrinsics", "[Atlas Adaptation Parameter Set RBSP]") {
  auto x = MivViewParamsUpdateExtrinsics{};

  SECTION("Example 1: Test with 1 update.") {
    x.mvpue_num_view_updates_minus1(0);
    x.mvpue_view_idx(0, 6)
        .camera_extrinsics(0)
        .ce_view_pos_x(3.F)
        .ce_view_pos_y(1.F)
        .ce_view_pos_z(4.F)
        .ce_view_quat_x(5.F)
        .ce_view_quat_y(9.F)
        .ce_view_quat_z(14.F);

    REQUIRE(toString(x) == R"(mvpue_num_view_updates_minus1=0
mvpue_view_idx[ 0 ]=6
ce_view_pos_x[ 0 ]=3
ce_view_pos_y[ 0 ]=1
ce_view_pos_z[ 0 ]=4
ce_view_quat_x[ 0 ]=5
ce_view_quat_y[ 0 ]=9
ce_view_quat_z[ 0 ]=14
)");

    REQUIRE(bitCodingTest(x, 224));
  }
  SECTION("Example 1: Test with 2 update.") {
    x.mvpue_num_view_updates_minus1(1);
    x.mvpue_view_idx(0, 6)
        .camera_extrinsics(0)
        .ce_view_pos_x(3.F)
        .ce_view_pos_y(1.F)
        .ce_view_pos_z(4.F)
        .ce_view_quat_x(5.F)
        .ce_view_quat_y(9.F)
        .ce_view_quat_z(14.F);
    x.mvpue_view_idx(1, 3)
        .camera_extrinsics(1)
        .ce_view_pos_x(7.F)
        .ce_view_pos_y(8.F)
        .ce_view_pos_z(3.F)
        .ce_view_quat_x(21.F)
        .ce_view_quat_y(12.F)
        .ce_view_quat_z(10.F);

    REQUIRE(toString(x) == R"(mvpue_num_view_updates_minus1=1
mvpue_view_idx[ 0 ]=6
ce_view_pos_x[ 0 ]=3
ce_view_pos_y[ 0 ]=1
ce_view_pos_z[ 0 ]=4
ce_view_quat_x[ 0 ]=5
ce_view_quat_y[ 0 ]=9
ce_view_quat_z[ 0 ]=14
mvpue_view_idx[ 1 ]=3
ce_view_pos_x[ 1 ]=7
ce_view_pos_y[ 1 ]=8
ce_view_pos_z[ 1 ]=3
ce_view_quat_x[ 1 ]=21
ce_view_quat_y[ 1 ]=12
ce_view_quat_z[ 1 ]=10
)");

    REQUIRE(bitCodingTest(x, 432));
  }
}

TEST_CASE("miv_view_params_update_intrinsics", "[Atlas Adaptation Parameter Set RBSP]") {
  auto x = MivViewParamsUpdateIntrinsics{};

  SECTION("Example 1: Test with 1 update.") {
    x.mvpue_num_view_updates_minus1(0);
    x.mvpue_view_idx(0, 6)
        .camera_intrinsics(0)
        .ci_cam_type(CiCamType::equirectangular)
        .ci_erp_phi_min(-2.F)
        .ci_erp_phi_max(2.F)
        .ci_erp_theta_min(-1.F)
        .ci_erp_theta_max(1.F);

    REQUIRE(toString(x) == R"(mvpue_num_view_updates_minus1=0
mvpue_view_idx[ 0 ]=6
ci_cam_type[ 0 ]=equirectangular
ci_projection_plane_width_minus1[ 0 ]=0
ci_projection_plane_height_minus1[ 0 ]=0
ci_erp_phi_min[ 0 ]=-2
ci_erp_phi_max[ 0 ]=2
ci_erp_theta_min[ 0 ]=-1
ci_erp_theta_max[ 0 ]=1
)");
    REQUIRE(bitCodingTest(x, 200));
  }

  SECTION("Example 1: Test with 2 updates.") {
    x.mvpue_num_view_updates_minus1(1);
    x.mvpue_view_idx(0, 3)
        .camera_intrinsics(0)
        .ci_cam_type(CiCamType::equirectangular)
        .ci_erp_phi_min(-90.F)
        .ci_erp_phi_max(90.F)
        .ci_erp_theta_min(-180.F)
        .ci_erp_theta_max(90.F);
    x.mvpue_view_idx(1, 12)
        .camera_intrinsics(1)
        .ci_cam_type(CiCamType::orthographic)
        .ci_projection_plane_width_minus1(1023)
        .ci_projection_plane_height_minus1(767)
        .ci_ortho_width(100.F)
        .ci_ortho_height(50.F);

    REQUIRE(toString(x) == R"(mvpue_num_view_updates_minus1=1
mvpue_view_idx[ 0 ]=3
ci_cam_type[ 0 ]=equirectangular
ci_projection_plane_width_minus1[ 0 ]=0
ci_projection_plane_height_minus1[ 0 ]=0
ci_erp_phi_min[ 0 ]=-90
ci_erp_phi_max[ 0 ]=90
ci_erp_theta_min[ 0 ]=-180
ci_erp_theta_max[ 0 ]=90
mvpue_view_idx[ 1 ]=12
ci_cam_type[ 1 ]=orthographic
ci_projection_plane_width_minus1[ 1 ]=1023
ci_projection_plane_height_minus1[ 1 ]=767
ci_ortho_width[ 1 ]=100
ci_ortho_height[ 1 ]=50
)");
    REQUIRE(bitCodingTest(x, 320));
  }
}
