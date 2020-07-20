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

#ifndef _TMIV_DECODER_COMMONATLASDECODER_H_
#define _TMIV_DECODER_COMMONATLASDECODER_H_

#include <TMIV/MivBitstream/AtlasAdaptationParameterSetRBSP.h>
#include <TMIV/MivBitstream/AtlasSubBitstream.h>
#include <TMIV/MivBitstream/CommonAtlasFrameRBSP.h>
#include <TMIV/MivBitstream/SeiRBSP.h>
#include <TMIV/MivBitstream/V3cUnit.h>

#include <TMIV/Common/Frame.h>

#include <functional>
#include <list>

namespace TMIV::Decoder {
using V3cUnitSource = std::function<std::optional<MivBitstream::V3cUnit>()>;

class CommonAtlasDecoder {
public:
  CommonAtlasDecoder() = default;
  explicit CommonAtlasDecoder(V3cUnitSource source, MivBitstream::V3cParameterSet vps, int32_t foc);

  struct AccessUnit {
    int32_t foc{};
    MivBitstream::AtlasAdaptationParameterSetRBSP aaps;
    MivBitstream::CommonAtlasFrameRBSP caf;
    std::vector<MivBitstream::SeiMessage> prefixNSei;
    std::vector<MivBitstream::SeiMessage> prefixESei;
    std::vector<MivBitstream::SeiMessage> suffixNSei;
    std::vector<MivBitstream::SeiMessage> suffixESei;
  };

  auto operator()() -> std::optional<AccessUnit>;

private:
  auto decodeAsb() -> bool;
  auto decodeAu() -> AccessUnit;
  void decodePrefixNalUnit(AccessUnit &au, const MivBitstream::NalUnit &nu);
  void decodeCafNalUnit(AccessUnit &au, const MivBitstream::NalUnit &nu);
  void decodeSuffixNalUnit(AccessUnit &au, const MivBitstream::NalUnit &nu);
  void decodeAaps(std::istream &stream);
  void decodeSei(std::vector<MivBitstream::SeiMessage> &messages, std::istream &stream);

  V3cUnitSource m_source;
  MivBitstream::V3cParameterSet m_vps;

  std::list<MivBitstream::NalUnit> m_buffer;
  int32_t m_foc{};

  std::vector<MivBitstream::AtlasAdaptationParameterSetRBSP> m_aapsV;
  unsigned m_maxFrmOrderCntLsb{};
};
} // namespace TMIV::Decoder

#endif