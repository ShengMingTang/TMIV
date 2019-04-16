/* The copyright in this software is being made available under the BSD
 * License, included below. This software may be subject to other third party
 * and contributor rights, including patent rights, and no such rights are
 * granted under this license.
 *
 * Copyright (c) 2010-2019, ITU/ISO/IEC
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
 *  * Neither the name of the ITU/ISO/IEC nor the names of its contributors may
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

#include <TMIV/IO/IO.h>

#include <cassert>
#include <fstream>
#include <iostream>

#include <TMIV/Common/Common.h>

using namespace std;
using namespace TMIV::Common;
using namespace TMIV::Metadata;

// TODO: Extract generic load/save functions to reduce code duplication in this
// module

namespace TMIV::IO
{

CameraParameterList loadSourceMetadata(const Json &config)
{
	cout << "Loading source metadata\n";
	ifstream stream{config.require("SourceCameraParameters").asString()};
	
	if (!stream.good())
		throw runtime_error("Failed to load source camera parameters");

	auto cameras = loadCamerasFromJson(Json{stream}.require("cameras"), config.require("SourceCameraNames").asStringVector());

	for (const auto &camera : cameras)
		cout << camera << '\n';

	return cameras;
}

namespace
{

/////////////////////////////////////////////////
template<typename FORMAT>
Frame<FORMAT> load(const std::string& path, int frameIndex, Vec2i resolution)
{
	Frame<FORMAT> result(resolution.x(), resolution.y());
	ifstream stream{path, std::ios::binary};
	
	if (!stream.good())
		throw runtime_error("Failed to open file: " + path);
	
	stream.seekg(streampos(frameIndex) * resolution.x() * resolution.y() * 2);
	result.read(stream);
	
	if (!stream.good())
		throw runtime_error("Failed to read from file: " + path);
	
	return result;
}

template<typename FORMAT>
void save(const std::string& path, int frameIndex, const Frame<FORMAT>& frame, size_t cameraId)
{
	ofstream stream(path, ((frameIndex == 0) ? ofstream::trunc : ofstream::app) | ofstream::binary);

	if (!stream.good())
		throw runtime_error("Failed to open depth map for writing: " + path);

	frame.dump(stream);

	if (!stream.good())
		throw runtime_error("Failed to write to depth map: " + path);
}


/////////////////////////////////////////////////
CameraParameters loadCameraFromFile(std::istream& is)
{
	CameraParameters camera;
	
	is.read((char *) &(camera.id), sizeof(uint16_t));
	is.read((char *) &(camera.size), sizeof(Vec2i));
	is.read((char *) &(camera.position), sizeof(Vec3f));
	is.read((char *) &(camera.rotation), sizeof(Vec3f));
	is.read((char *) &(camera.type), sizeof(ProjectionType));
	is.read((char *) &(camera.erpPhiRange), sizeof(Vec2f));
	is.read((char *) &(camera.erpThetaRange), sizeof(Vec2f));
	is.read((char *) &(camera.cubicMapType), sizeof(CubicMapType));
	is.read((char *) &(camera.perspectiveFocal), sizeof(Vec2f));
	is.read((char *) &(camera.perspectiveCenter), sizeof(Vec2f));
	is.read((char *) &(camera.depthRange), sizeof(Vec2f));

	return camera;
}

void saveCameraToFile(std::ofstream& os, const CameraParameters& camera)
{
	os.write((const char *) &(camera.id), sizeof(uint16_t));
	os.write((const char *) &(camera.size), sizeof(Vec2i));
	os.write((const char *) &(camera.position), sizeof(Vec3f));
	os.write((const char *) &(camera.rotation), sizeof(Vec3f));
	os.write((const char *) &(camera.type), sizeof(ProjectionType));
	os.write((const char *) &(camera.erpPhiRange), sizeof(Vec2f));
	os.write((const char *) &(camera.erpThetaRange), sizeof(Vec2f));
	os.write((const char *) &(camera.cubicMapType), sizeof(CubicMapType));
	os.write((const char *) &(camera.perspectiveFocal), sizeof(Vec2f));
	os.write((const char *) &(camera.perspectiveCenter), sizeof(Vec2f));
	os.write((const char *) &(camera.depthRange), sizeof(Vec2f));
}

/////////////////////////////////////////////////
PatchParameters readPatchFromFile(std::ifstream& is)
{
	PatchParameters patch;
	
	is.read((char *) &(patch.atlasId), sizeof(uint8_t));
	is.read((char *) &(patch.virtualCameraId), sizeof(uint8_t));
	is.read((char *) &(patch.patchSize), sizeof(Vec2i));
	is.read((char *) &(patch.patchMappingPos), sizeof(Vec2i));
	is.read((char *) &(patch.patchPackingPos), sizeof(Vec2i));
	is.read((char *) &(patch.patchRotation), sizeof(PatchRotation));

	return patch;
}

void savePatchToFile(std::ofstream& os, const PatchParameters& patch)
{
	os.write((const char *) &(patch.atlasId), sizeof(uint8_t));
	os.write((const char *) &(patch.virtualCameraId), sizeof(uint8_t));
	os.write((const char *) &(patch.patchSize), sizeof(Vec2i));
	os.write((const char *) &(patch.patchMappingPos), sizeof(Vec2i));
	os.write((const char *) &(patch.patchPackingPos), sizeof(Vec2i));
	os.write((const char *) &(patch.patchRotation), sizeof(PatchRotation));
}


	

TextureFrame loadSourceTexture(const Json &config, int frameIndex, Vec2i resolution, size_t cameraId)
{
	TextureFrame result(resolution.x(), resolution.y());
	
	auto path = format(config.require("SourceTexturePathFmt").asString().c_str(), cameraId);
	ifstream stream{path, std::ios::binary};
	
	if (!stream.good())
		throw runtime_error("Failed to open source texture file");

	stream.seekg(streampos(frameIndex) * resolution.x() * resolution.y() * 3);
	result.read(stream);
	
	if (!stream.good())
		throw runtime_error("Failed to read from source texture file");
	
	return result;
}

Depth16Frame loadSourceDepth(const Json &config, int frameIndex, Vec2i resolution, size_t cameraId)
{
	Depth16Frame result(resolution.x(), resolution.y());
	auto path = format(config.require("SourceDepthPathFmt").asString().c_str(), cameraId);
	ifstream stream{path, std::ios::binary};
	
	if (!stream.good())
		throw runtime_error("Failed to open source texture file");

	stream.seekg(streampos(frameIndex) * resolution.x() * resolution.y() * 3);
	result.read(stream);

	if (!stream.good())
		throw runtime_error("Failed to read from source texture file");
	
	return result;
}

} // namespace

MVD16Frame loadSourceFrame(const Json &config, const CameraParameterList &cameras, int frameIndex)
{
	frameIndex += config.require("startFrame").asInt();
	cout << "Loading source frame " << frameIndex << '\n';
	MVD16Frame result(cameras.size());

	for (size_t id = 0; id < result.size(); ++id)
	{
		auto size = cameras[id].size;
		result[id].first = loadSourceTexture(config, frameIndex, size, id);
		result[id].second = loadSourceDepth(config, frameIndex, size, id);
	}
	
	return result;
}

namespace
{

void saveBaseTexture(const Json &config, int frameIndex, TextureFrame const &frame, size_t cameraId)
{
	auto path = format(config.require("BaseTexturePathFmt").asString().c_str(), cameraId);
	ofstream stream{path, (frameIndex == 0 ? ofstream::trunc : ofstream::app) | ofstream::binary};
	
	if (!stream.good())
		throw runtime_error("Failed to open optimized texture for writing");

	frame.dump(stream);
	
	if (!stream.good())
		throw runtime_error("Failed to write to optimized texture");
}

void saveBaseDepth(const Json &config, int frameIndex, Depth16Frame const &frame, size_t cameraId)
{
	auto path = format(config.require("BaseDepthPathFmt").asString().c_str(), cameraId);
	ofstream stream(path, ((frameIndex == 0) ? ofstream::trunc : ofstream::app) | ofstream::binary);

	if (!stream.good())
		throw runtime_error("Failed to open optimized depth map for writing");

	Frame<YUV420P16> frameYuv{frame.getWidth(), frame.getHeight()};
	convert(frame, frameYuv);
	frameYuv.dump(stream);

	if (!stream.good())
		throw runtime_error("Failed to write to optimized depth map");
}

void saveAdditionalTexture(const Json &config, int frameIndex, TextureFrame const &frame, size_t cameraId)
{
	auto path = format(config.require("AdditionalTexturePathFmt").asString().c_str(), cameraId);
	ofstream stream{path, (frameIndex == 0 ? ofstream::trunc : ofstream::app) | ofstream::binary};

	if (!stream.good())
		throw runtime_error("Failed to open additional texture for writing");

	frame.dump(stream);

	if (!stream.good())
		throw runtime_error("Failed to write to additional texture");
}

void saveAdditionalDepth(const Json &config, int frameIndex, Depth16Frame const &frame, size_t cameraId)
{
	auto path = format(config.require("AdditionalDepthPathFmt").asString().c_str(), cameraId);
	ofstream stream(path, ((frameIndex == 0) ? ofstream::trunc : ofstream::app) | ofstream::binary);
	
	if (!stream.good())
		throw runtime_error("Failed to open additonal depth map for writing");

	Frame<YUV420P16> frameYuv{frame.getWidth(), frame.getHeight()};
	convert(frame, frameYuv);
	frameYuv.dump(stream);
	
	if (!stream.good())
		throw runtime_error("Failed to write to additonal depth map");
}

} // namespace

void saveOptimizedFrame(const Json &config, int frameIndex,
                        const BaseAdditional<CameraParameterList> &cameras,
                        const BaseAdditional<MVD16Frame> &frame)
{
	cout << "Saving optimized frame " << frameIndex << '\n';

	assert(cameras.base.size() == frame.base.size());
	
	for (size_t i = 0; i < cameras.base.size(); ++i)
	{
		saveBaseTexture(config, frameIndex, frame.base[i].first, cameras.base[i].id);
		saveBaseDepth(config, frameIndex, frame.base[i].second, cameras.base[i].id);
	}

	assert(cameras.additional.size() == frame.additional.size());
	
	for (size_t i = 0; i < cameras.additional.size(); ++i)
	{
		saveAdditionalTexture(config, frameIndex, frame.additional[i].first, cameras.additional[i].id);
		saveAdditionalDepth(config, frameIndex, frame.additional[i].second, cameras.additional[i].id);
	}
}

auto loadOptimizedFrame(const Json &config, const BaseAdditional<Metadata::CameraParameterList> &cameras, int frameIndex) -> BaseAdditional<MVD16Frame>
{
// 	cout << "Loading optimized frame " << frameIndex << '\n';
// 			
// 	BaseAdditional<MVD16Frame> frame;
// 			
// 	assert(cameras.base.size() == frame.base.size());
// 	
// 	for (size_t i = 0; i < cameras.base.size(); ++i)
// 	{
// 		TextureFrame texture(cameras.base[i].size.x(), cameras.base[i].size.y());
// 		
// 		
// 		
// 		
// 		Depth16Frame depth;
// 		
// 		TextureDepth16Frame f;
// 		
// 		
// 		using TextureDepth16Frame = std::pair<TextureFrame, Depth16Frame>;
// using MVD16Frame = std::vector<TextureDepth16Frame>;
// 		
// 		saveBaseTexture(config, frameIndex, frame.base[i].first, cameras.base[i].id);
// 		saveBaseDepth(config, frameIndex, frame.base[i].second, cameras.base[i].id);
// 	}
// 
// 	assert(cameras.additional.size() == frame.additional.size());
// 	
// 	for (size_t i = 0; i < cameras.additional.size(); ++i)
// 	{
// 		saveAdditionalTexture(config, frameIndex, frame.additional[i].first, cameras.additional[i].id);
// 		saveAdditionalDepth(config, frameIndex, frame.additional[i].second, cameras.additional[i].id);
// 	}
			
			
			
			
		// TODO
	return {};
}

void saveOptimizedMetadata(const Json &config, int frameIndex, const BaseAdditional<CameraParameterList> &metadata)
{
	cout << "Saving metadata of optimized frame " << frameIndex << '\n';

	// TODO
}

auto loadOptimizedMetadata(const Json &config, int frameIndex) -> BaseAdditional<CameraParameterList>
{
	// TODO

	return {};
}




void saveMivMetadata(const Json &config, int frameIndex, const MivMetadata &metadata)
{
	// TODO
}

auto loadMivMetadata(const Json &config, int frameIndex) -> MivMetadata
{
	// TODO

	return {};
}

namespace
{

void saveAtlasTexture(const Json &config, int frameIndex, TextureFrame const &frame, size_t atlasId)
{
	auto path = format(config.require("AtlasTexturePathFmt").asString().c_str(), atlasId);
	ofstream stream{path, (frameIndex == 0 ? ofstream::trunc : ofstream::app) | ofstream::binary};
	
	if (!stream.good())
		throw runtime_error("Failed to open atlas texture for writing");

	frame.dump(stream);

	if (!stream.good())
		throw runtime_error("Failed to write to atlas texture");
}

void saveAtlasDepth(const Json &config, int frameIndex, Frame<YUV420P10> const &frame, size_t atlasId)
{
	auto path = format(config.require("AtlasDepthPathFmt").asString().c_str(), atlasId);
	ofstream stream(path, ((frameIndex == 0) ? ofstream::trunc : ofstream::app) | ofstream::binary);
	
	if (!stream.good())
		throw runtime_error("Failed to open atlas depth map for writing");
	
	frame.dump(stream);
	
	if (!stream.good())
		throw runtime_error("Failed to write to atlas depth map");
}

} // namespace

void saveAtlas(const Json &config, int frameIndex, MVD16Frame const &frame)
{
	Frame<YUV420P10> depth10;

	for (size_t id = 0; id < frame.size(); ++id)
	{
		saveAtlasTexture(config, frameIndex, frame[id].first, id);
		depth10.resize(frame[id].second.getWidth(), frame[id].second.getHeight());
		convert(frame[id].second, depth10);
		saveAtlasDepth(config, frameIndex, depth10, id);
	}
}

auto loadAtlas(const Json &config, int frameIndex) -> MVD10Frame
{ 
	// TODO
	
	return {};
}

void savePatchIdMaps(const Json &config, int frameIndex, const PatchIdMapList &maps)
{
	// TODO
}

auto loadPatchIdMaps(const Json &config, int frameIndex) -> PatchIdMapList
{
	// TODO
	return {};
}

auto loadViewportMetadata(const Json &config, int frameIndex) -> CameraParameters
{

	// TODO
	return {};
}

void saveViewport(const Json &config, int frameIndex, const TextureDepth10Frame &frame)
{
	// TODO
}

} // namespace TMIV::IO
