/* The copyright in this software is being made available under the BSD
 * License, included below. This software may be subject to other third party
 * and contributor rights, including patent rights, and no such rights are
 * granted under this license.
 *
 * Copyright (c) 2010-2021, ISO/IEC
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

#include <catch2/catch.hpp>

#include <TMIV/Encoder/Configuration.h>

using namespace std::string_view_literals;

TEST_CASE("TMIV::Encoder::Configuration") {
  using Catch::Contains;
  using TMIV::Common::Json;
  using TMIV::Common::Vec2i;
  using TMIV::Encoder::Configuration;
  using TMIV::MivBitstream::PtlProfileCodecGroupIdc;
  using TMIV::MivBitstream::PtlProfilePccToolsetIdc;

  SECTION("MIV Main example") {
    auto root = Json::parse(R"({
    "intraPeriod": 1,
    "blockSizeDepthQualityDependent": [2, 4],
    "haveTextureVideo": false,
    "haveGeometryVideo": true,
    "haveOccupancyVideo": false,
    "oneViewPerAtlasFlag": false,
    "dynamicDepthRange": false,
    "randomAccess": false,
    "numGroups": 0,
    "maxEntityId": 0,
    "maxLumaSampleRate": 0,
    "maxLumaPictureSize": 0,
    "maxAtlases": 0,
    "codecGroupIdc": "HEVC Main10",
    "toolsetIdc": "MIV Main"
})"sv);

    const auto component = Json::parse(R"({
    "dilate": 0
})"sv);

    const auto unit = Configuration{root, component};

    CHECK(unit.intraPeriod == 1);
    CHECK(unit.blockSizeDepthQualityDependent == Vec2i{2, 4});
    CHECK_FALSE(unit.haveTexture);
    CHECK(unit.haveGeometry);
    CHECK_FALSE(unit.haveOccupancy);
    CHECK_FALSE(unit.oneViewPerAtlasFlag);
    CHECK_FALSE(unit.dynamicDepthRange);
    CHECK(unit.dqParamsPresentFlag);
    CHECK_FALSE(unit.randomAccess);
    CHECK(unit.numGroups == 0);
    CHECK(unit.maxEntityId == 0);
    CHECK(unit.maxLumaSampleRate == 0.);
    CHECK(unit.maxLumaPictureSize == 0);
    CHECK(unit.maxAtlases == 0);
    CHECK(unit.codecGroupIdc == PtlProfileCodecGroupIdc::HEVC_Main10);
    CHECK(unit.toolsetIdc == PtlProfilePccToolsetIdc::MIV_Main);
    CHECK(unit.dilationIter == 0);
    CHECK_FALSE(unit.viewingSpace.has_value());
    CHECK(unit.overrideAtlasFrameSizes.empty());

    SECTION("Add viewing space") {
      root.update(Json::parse(R"({
    "ViewingSpace": {
        "ElementaryShapes": [{
            "ElementaryShapeOperation": "add",
            "ElementaryShape": {
                "PrimitiveShapeOperation": "add",
                "PrimitiveShapes": [{
                    "PrimitiveShapeType": "cuboid",
                    "Center": [0, 0, 0],
                    "Size": [0, 0, 0]
                }]
            }
        }]
    }
})"));

      const auto unit2 = Configuration{root, component};

      CHECK(unit2.viewingSpace.has_value());
    }

    SECTION("Test for maxIntraPeriod") {
      root.update(Json::parse(R"({ "intraPeriod": 33 })"));
      REQUIRE_THROWS_AS((Configuration{root, component}), std::runtime_error);
    }

    SECTION("AVC Progressive High") {
      root.update(Json::parse(R"({ "codecGroupIdc": "AVC Progressive High" })"));
      const auto unit2 = Configuration{root, component};
      CHECK(unit2.codecGroupIdc == PtlProfileCodecGroupIdc::AVC_Progressive_High);
    }

    SECTION("V-PCC Basic is not supported") {
      root.update(Json::parse(R"({ "toolsetIdc": "V-PCC Basic" })"));
      REQUIRE_THROWS_WITH((Configuration{root, component}),
                          "The V-PCC Basic toolset IDC is not supported");
    }

    SECTION("V-PCC Extended is not supported") {
      root.update(Json::parse(R"({ "toolsetIdc": "V-PCC Extended" })"));
      REQUIRE_THROWS_WITH((Configuration{root, component}),
                          "The V-PCC Extended toolset IDC is not supported");
    }

    SECTION("Unknown codec group IDC") {
      root.update(Json::parse(R"({ "codecGroupIdc": "Fantasy" })"));
      REQUIRE_THROWS_AS((Configuration{root, component}), std::runtime_error);
    }

    SECTION("Unknown toolset IDC") {
      root.update(Json::parse(R"({ "toolsetIdc": "Hammer" })"));
      REQUIRE_THROWS_AS((Configuration{root, component}), std::runtime_error);
    }

    SECTION("MIV Main requires geometry") {
      root.update(Json::parse(R"({
    "haveGeometryVideo": false,
    "dqParamsPresentFlag": false
})"));
      REQUIRE_THROWS_WITH((Configuration{root, component}), Contains("haveGeometry"));
    }

    SECTION("MIV Main does not support occupancy") {
      root.update(Json::parse(R"({
    "haveOccupancyVideo": true
})"));
      REQUIRE_THROWS_WITH((Configuration{root, component}), Contains("!haveOccupancy"));
    }
  }

  SECTION("MIV Geometry Absent example") {
    auto root = Json::parse(R"({
    "intraPeriod": 13,
    "blockSizeDepthQualityDependent": [8, 4],
    "haveTextureVideo": true,
    "haveGeometryVideo": false,
    "haveOccupancyVideo": false,
    "oneViewPerAtlasFlag": true,
    "dynamicDepthRange": false,
    "dqParamsPresentFlag": false,
    "attributeOffsetEnabledFlag": false,
    "randomAccess": true,
    "numGroups": 3,
    "maxEntityId": 5,
    "EntityEncodeRange": [0, 4],
    "codecGroupIdc": "AVC Progressive High",
    "toolsetIdc": "MIV Geometry Absent"
})"sv);

    auto component = Json::parse(R"({
    "dilate": 5
})"sv);

    const auto unit = Configuration{root, component};

    CHECK(unit.intraPeriod == 13);
    CHECK(unit.blockSizeDepthQualityDependent == Vec2i{8, 4});
    CHECK(unit.haveTexture);
    CHECK_FALSE(unit.haveGeometry);
    CHECK_FALSE(unit.haveOccupancy);
    CHECK(unit.oneViewPerAtlasFlag);
    CHECK_FALSE(unit.dynamicDepthRange);
    CHECK_FALSE(unit.dqParamsPresentFlag);
    CHECK(unit.randomAccess);
    CHECK(unit.numGroups == 3);
    CHECK(unit.maxEntityId == 5);
    CHECK(unit.codecGroupIdc == PtlProfileCodecGroupIdc::AVC_Progressive_High);
    CHECK(unit.toolsetIdc == PtlProfilePccToolsetIdc::MIV_Geometry_Absent);
    CHECK(unit.dilationIter == 5);
    CHECK_FALSE(unit.viewingSpace.has_value());
    CHECK(unit.overrideAtlasFrameSizes.empty());
    CHECK_FALSE(unit.depthLowQualityFlag.has_value());

    SECTION("Override atlas frame sizes") {
      component.update(Json::parse(R"({ "overrideAtlasFrameSizes": [ [4, 5], [6, 7] ] })"));

      const auto unit2 = Configuration{root, component};

      CHECK(unit2.overrideAtlasFrameSizes == std::vector{Vec2i{4, 5}, Vec2i{6, 7}});
    }

    SECTION("Attribute offset") {
      root.update(Json::parse(R"({
    "attributeOffsetEnabledFlag": true,
    "attributeOffsetBitCount": 1
})"));

      const auto unit2 = Configuration{root, component};

      CHECK(unit2.attributeOffsetFlag);
      CHECK(unit2.attributeOffsetBitCount == 1);
    }

    SECTION("Depth low quality flag") {
      root.update(Json::parse(R"({ "depthLowQualityFlag": true })"));

      const auto unit2 = Configuration{root, component};

      REQUIRE(unit2.depthLowQualityFlag.has_value());
      CHECK(*unit2.depthLowQualityFlag);
    }

    SECTION("Geometry absent does not support geometry") {
      root.update(Json::parse(R"({
    "haveGeometryVideo": true,
    "geometryScaleEnabledFlag": false
})"));
      REQUIRE_THROWS_WITH((Configuration{root, component}), Contains("!haveGeometry"));
    }

    SECTION("Geometry absent does not support occupancy") {
      root.update(Json::parse(R"({
    "haveOccupancyVideo": true
})"));
      REQUIRE_THROWS_WITH((Configuration{root, component}), Contains("!haveOccupancy"));
    }
  }
}