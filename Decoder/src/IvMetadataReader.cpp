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

#include "IvMetadataReader.h"

#include <TMIV/IO/IO.h>

#include <iostream>

using namespace std;
using namespace TMIV::Common;
using namespace TMIV::Decoder;
using namespace TMIV::IO;

namespace TMIV::Decoder {
auto bitstreamPath(const Json &config) -> string {
  return getFullPath(config, "OutputDirectory", "BitstreamPath");
}

IvMetadataReader::IvMetadataReader(const Json &config)
    : m_stream{bitstreamPath(config), ios::binary} {
  if (!m_stream.good()) {
    ostringstream what;
    what << "Failed to open \"" << bitstreamPath(config) << "\" for reading";
    throw runtime_error(what.str());
  }
  m_vssDecoder = make_unique<V3cSampleStreamDecoder>(m_stream);
  m_decoder = make_unique<MivDecoder>([this]() { return (*m_vssDecoder)(); });
  m_decoder->setGeoFrameServer([&config](uint8_t atlasId, uint32_t frameId, Vec2i frameSize) {
    return readFrame<YUV400P10>(config, "OutputDirectory", "GeometryVideoDataPathFmt", frameId,
                                frameSize, int(atlasId));
  });
  m_decoder->setAttrFrameServer([&config](uint8_t atlasId, uint32_t frameId, Vec2i frameSize) {
    return yuv444p(readFrame<YUV420P10>(config, "OutputDirectory", "AttributeVideoDataPathFmt",
                                        frameId, frameSize, "T", int(atlasId)));
  });
}
} // namespace TMIV::Decoder