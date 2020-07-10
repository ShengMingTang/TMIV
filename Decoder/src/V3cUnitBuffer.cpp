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

#include <TMIV/Decoder/V3cUnitBuffer.h>

#include <iostream>
#include <utility>


namespace TMIV::Decoder {
V3cUnitBuffer::V3cUnitBuffer(V3cUnitSource source) : m_source{std::move(source)} {}

auto V3cUnitBuffer::operator()(const MivBitstream::V3cUnitHeader &vuh)
    -> std::optional<MivBitstream::V3cUnit> {
  auto i = m_buffer.begin();

  for (;;) {
    if (i == m_buffer.end()) {
      if (auto vu = m_source()) {
        m_buffer.push_back(std::move(*vu));
        i = std::prev(m_buffer.end());
      } else {
        return {};
      }
    }
    if (i->v3c_unit_header() == vuh) {
      auto vu = std::move(*i);
      i = m_buffer.erase(i);
      return vu;
    }
    if (vuh.vuh_unit_type() == MivBitstream::VuhUnitType::V3C_VPS) {
      std::cout << "WARNING: Ignoring V3C unit:\n" << *i;
      i = m_buffer.erase(i);
    } else {
      ++i;
    }
  }
}
} // namespace TMIV::Decoder
