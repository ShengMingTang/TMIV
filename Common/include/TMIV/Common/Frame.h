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

#ifndef _TMIV_COMMON_FRAME_H_
#define _TMIV_COMMON_FRAME_H_

#include <cstdint>
#include <istream>
#include <ostream>

#include <TMIV/Common/Matrix.h>

namespace TMIV::Common {
class YUV400P8 {};
class YUV400P16 {};
class YUV420P8 {};
class YUV420P10 {};
class YUV420P16 {};

namespace detail {
template <class FORMAT> struct PixelFormatHelper {};
} // namespace detail

template <class FORMAT> class Frame {
public:
  using base_type = typename detail::PixelFormatHelper<FORMAT>::base_type;
  using plane_type = heap::Matrix<base_type>;

protected:
  static constexpr int nb_plane = detail::PixelFormatHelper<FORMAT>::nb_plane;

protected:
  int m_width = 0, m_height = 0;
  std::array<plane_type, nb_plane> m_planes;

public:
  Frame(int w = 0, int h = 0) { resize(w, h); }
  Frame(const Frame &) = default;
  Frame(Frame &&that) {
    m_width = that.m_width;
    m_height = that.m_height;
    m_planes = std::move(that.m_planes);

    that.m_width = 0;
    that.m_height = 0;
  }
  Frame &operator=(const Frame &) = default;
  Frame &operator=(Frame &&that) {
    m_width = that.m_width;
    m_height = that.m_height;
    m_planes = std::move(that.m_planes);

    that.m_width = 0;
    that.m_height = 0;

    return *this;
  }

  void resize(int w, int h);

  std::array<plane_type, nb_plane> &getPlanes() { return m_planes; }
  const std::array<plane_type, nb_plane> &getPlanes() const { return m_planes; }

  const plane_type &getPlane(int id) const { return m_planes[id]; }
  plane_type &getPlane(int id) { return m_planes[id]; }
  int getWidth() const { return m_width; }
  int getHeight() const { return m_height; }
  static constexpr int getNumberOfPlanes() { return nb_plane; }

  void read(std::istream &is, bool vFlip = false);
  void dump(std::ostream &os, bool vFlip = false) const;
};

template <class FROM, class TO>
void convert(const Frame<FROM> &inputFrame, Frame<TO> &outputFrame);
} // namespace TMIV::Common

#include "Frame.hpp"

namespace TMIV::Common {
using TextureFrame = Frame<YUV420P10>;
using DepthFrame = Frame<YUV400P16>;
using TextureDepthFrame = std::pair<TextureFrame, DepthFrame>;
using MVDFrame = std::vector<TextureDepthFrame>;
} // namespace TMIV::Common

#endif
