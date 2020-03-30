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

#include <TMIV/MivBitstream/RecViewport.h>

#include "verify.h"

using namespace std;
using namespace TMIV::Common;

namespace TMIV::MivBitstream {
// Getters
auto RecViewport::rec_viewport_id() const noexcept { return m_rec_viewport_id; }
auto RecViewport::rec_viewport_cancel_flag() const noexcept { return m_rec_viewport_cancel_flag; }
auto RecViewport::rec_viewport_persistence_flag() const noexcept {
  VERIFY_MIVBITSTREAM(m_rec_viewport_persistence_flag.has_value());
  return *m_rec_viewport_persistence_flag;
}
auto RecViewport::rec_viewport_center_view_flag() const noexcept {
  VERIFY_MIVBITSTREAM(m_rec_viewport_center_view_flag.has_value());
  return *m_rec_viewport_center_view_flag;
}
auto RecViewport::rec_viewport_left_view_flag() const noexcept {
  VERIFY_MIVBITSTREAM(m_rec_viewport_left_view_flag.has_value());
  return *m_rec_viewport_left_view_flag;
}
auto RecViewport::rec_viewport_pos_x() const noexcept {
  VERIFY_MIVBITSTREAM(m_rec_viewport_pos_x.has_value());
  return *m_rec_viewport_pos_x;
}
auto RecViewport::rec_viewport_pos_y() const noexcept {
  VERIFY_MIVBITSTREAM(m_rec_viewport_pos_y.has_value());
  return *m_rec_viewport_pos_y;
}
auto RecViewport::rec_viewport_pos_z() const noexcept {
  VERIFY_MIVBITSTREAM(m_rec_viewport_pos_z.has_value());
  return *m_rec_viewport_pos_z;
}
auto RecViewport::rec_viewport_quat_x() const noexcept {
  VERIFY_MIVBITSTREAM(m_rec_viewport_quat_x.has_value());
  return *m_rec_viewport_quat_x;
}
auto RecViewport::rec_viewport_quat_y() const noexcept {
  VERIFY_MIVBITSTREAM(m_rec_viewport_quat_y.has_value());
  return *m_rec_viewport_quat_y;
}
auto RecViewport::rec_viewport_quat_z() const noexcept {
  VERIFY_MIVBITSTREAM(m_rec_viewport_quat_z.has_value());
  return *m_rec_viewport_quat_z;
}
auto RecViewport::rec_viewport_hor_range() const noexcept {
  VERIFY_MIVBITSTREAM(m_rec_viewport_hor_range.has_value());
  return *m_rec_viewport_hor_range;
}
auto RecViewport::rec_viewport_ver_range() const noexcept {
  VERIFY_MIVBITSTREAM(m_rec_viewport_ver_range.has_value());
  return *m_rec_viewport_ver_range;
}
// Setters
constexpr auto &RecViewport::rec_viewport_id(const std::uint16_t value) noexcept {
  m_rec_viewport_id = value;
  return *this;
}
constexpr auto &RecViewport::rec_viewport_cancel_flag(const bool value) noexcept {
  m_rec_viewport_cancel_flag = value;
  return *this;
}
constexpr auto &RecViewport::rec_viewport_persistence_flag(const bool value) noexcept {
  m_rec_viewport_persistence_flag = value;
  return *this;
}
constexpr auto &RecViewport::rec_viewport_center_view_flag(const bool value) noexcept {
  m_rec_viewport_center_view_flag = value;
  return *this;
}
constexpr auto &RecViewport::rec_viewport_left_view_flag(const bool value) noexcept {
  m_rec_viewport_left_view_flag = value;
  return *this;
}
constexpr auto &RecViewport::rec_viewport_pos_x(const float value) noexcept {
  m_rec_viewport_pos_x = value;
  return *this;
}
constexpr auto &RecViewport::rec_viewport_pos_y(const float value) noexcept {
  m_rec_viewport_pos_y = value;
  return *this;
}
constexpr auto &RecViewport::rec_viewport_pos_z(const float value) noexcept {
  m_rec_viewport_pos_z = value;
  return *this;
}
constexpr auto &RecViewport::rec_viewport_quat_x(const float value) noexcept {
  m_rec_viewport_quat_x = value;
  return *this;
}
constexpr auto &RecViewport::rec_viewport_quat_y(const float value) noexcept {
  m_rec_viewport_quat_y = value;
  return *this;
}
constexpr auto &RecViewport::rec_viewport_quat_z(const float value) noexcept {
  m_rec_viewport_quat_z = value;
  return *this;
}
constexpr auto &RecViewport::rec_viewport_hor_range(const float value) noexcept {
  m_rec_viewport_hor_range = value;
  return *this;
}
constexpr auto &RecViewport::rec_viewport_ver_range(const float value) noexcept {
  m_rec_viewport_ver_range = value;
  return *this;
}
// functions
auto RecViewport::operator==(const RecViewport &other) const noexcept -> bool {
  bool Equal = true;

  if (rec_viewport_id() != other.rec_viewport_id() ||
      rec_viewport_cancel_flag() != other.rec_viewport_cancel_flag())
    Equal = false;

  if (m_rec_viewport_persistence_flag.has_value())
    if (rec_viewport_persistence_flag() != other.rec_viewport_persistence_flag())
      Equal = false;

  if (m_rec_viewport_center_view_flag.has_value())
    if (rec_viewport_persistence_flag() != other.rec_viewport_persistence_flag())
      Equal = false;

  if (m_rec_viewport_left_view_flag.has_value())
    if (rec_viewport_left_view_flag() != other.rec_viewport_left_view_flag())
      Equal = false;

  if (m_rec_viewport_pos_x.has_value())
    if (rec_viewport_pos_x() != other.rec_viewport_pos_x())
      Equal = false;

  if (m_rec_viewport_pos_y.has_value())
    if (rec_viewport_pos_y() != other.rec_viewport_pos_y())
      Equal = false;

  if (m_rec_viewport_pos_z.has_value())
    if (rec_viewport_pos_z() != other.rec_viewport_pos_z())
      Equal = false;

  if (m_rec_viewport_quat_x.has_value())
    if (rec_viewport_quat_x() != other.rec_viewport_quat_x())
      Equal = false;

  if (m_rec_viewport_quat_y.has_value())
    if (rec_viewport_quat_y() != other.rec_viewport_quat_y())
      Equal = false;

  if (m_rec_viewport_quat_z.has_value())
    if (rec_viewport_quat_z() != other.rec_viewport_quat_z())
      Equal = false;

  if (m_rec_viewport_hor_range.has_value())
    if (rec_viewport_hor_range() != other.rec_viewport_hor_range())
      Equal = false;

  if (m_rec_viewport_ver_range.has_value())
    if (rec_viewport_ver_range() != other.rec_viewport_ver_range())
      Equal = false;

  return Equal;
}

auto RecViewport::operator!=(const RecViewport &other) const noexcept -> bool {
  return !operator==(other);
}

RecViewport::RecViewport(uint16_t value1, bool value2) {
  rec_viewport_id(value1);
  rec_viewport_cancel_flag(value2);
}

RecViewport::RecViewport(uint16_t value1, bool value2, bool value3, bool value4, float value5,
                         float value6, float value7, float value8, float value9, float value10,
                         float value11, float value12) {
  rec_viewport_id(value1);
  rec_viewport_cancel_flag(value2);
  rec_viewport_persistence_flag(value3);
  rec_viewport_center_view_flag(value4);
  rec_viewport_pos_x(value5);
  rec_viewport_pos_y(value6);
  rec_viewport_pos_z(value7);
  rec_viewport_quat_x(value8);
  rec_viewport_quat_y(value9);
  rec_viewport_quat_z(value10);
  rec_viewport_hor_range(value11);
  rec_viewport_ver_range(value12);
}

RecViewport::RecViewport(uint16_t value1, bool value2, bool value3, bool value4, bool value5,
                         float value6, float value7, float value8, float value9, float value10,
                         float value11, float value12, float value13) {
  rec_viewport_id(value1);
  rec_viewport_cancel_flag(value2);
  rec_viewport_persistence_flag(value3);
  rec_viewport_center_view_flag(value4);
  rec_viewport_left_view_flag(value5);
  rec_viewport_pos_x(value6);
  rec_viewport_pos_y(value7);
  rec_viewport_pos_z(value8);
  rec_viewport_quat_x(value9);
  rec_viewport_quat_y(value10);
  rec_viewport_quat_z(value11);
  rec_viewport_hor_range(value12);
  rec_viewport_ver_range(value13);
}

auto operator<<(ostream &stream, const RecViewport &x) -> ostream & {
  stream << "rec_viewport_id=" << x.rec_viewport_id() << '\n';
  stream << "rec_viewport_cancel_flag=" << x.rec_viewport_cancel_flag() << '\n';
  if (!x.rec_viewport_cancel_flag()) {
    stream << "rec_viewport_persistence_flag=" << x.rec_viewport_persistence_flag() << '\n';
    stream << "rec_viewport_center_view_flag=" << x.rec_viewport_center_view_flag() << '\n';
    if (!x.rec_viewport_center_view_flag())
      stream << "rec_viewport_left_view_flag=" << x.rec_viewport_left_view_flag() << '\n';
    stream << "rec_viewport_pos_x=" << x.rec_viewport_pos_x() << '\n';
    stream << "rec_viewport_pos_y=" << x.rec_viewport_pos_y() << '\n';
    stream << "rec_viewport_pos_z=" << x.rec_viewport_pos_z() << '\n';
    stream << "rec_viewport_quat_x=" << x.rec_viewport_quat_x() << '\n';
    stream << "rec_viewport_quat_y=" << x.rec_viewport_quat_y() << '\n';
    stream << "rec_viewport_quat_z=" << x.rec_viewport_quat_z() << '\n';
    stream << "rec_viewport_hor_range=" << x.rec_viewport_hor_range() << '\n';
    stream << "rec_viewport_ver_range=" << x.rec_viewport_ver_range() << '\n';
  }
  return stream;
}

auto RecViewport::decodeFrom(InputBitstream &bitstream) -> RecViewport {
  RecViewport x = RecViewport();
  x.rec_viewport_id(bitstream.readBits<uint16_t>(10));
  x.rec_viewport_cancel_flag(bitstream.getFlag());
  if (!x.rec_viewport_cancel_flag()) {
    x.rec_viewport_persistence_flag(bitstream.getFlag());
    x.rec_viewport_center_view_flag(bitstream.getFlag());
    if (!x.rec_viewport_center_view_flag())
      x.rec_viewport_left_view_flag(bitstream.getFlag());
    x.rec_viewport_pos_x(bitstream.getFloat32());
    x.rec_viewport_pos_y(bitstream.getFloat32());
    x.rec_viewport_pos_z(bitstream.getFloat32());
    x.rec_viewport_quat_x(bitstream.getFloat32());
    x.rec_viewport_quat_y(bitstream.getFloat32());
    x.rec_viewport_quat_z(bitstream.getFloat32());
    x.rec_viewport_hor_range(bitstream.getFloat32());
    x.rec_viewport_ver_range(bitstream.getFloat32());
  }
  return x;
}

void RecViewport::encodeTo(OutputBitstream &bitstream) const {
  bitstream.writeBits(rec_viewport_id(), 10);
  bitstream.putFlag(rec_viewport_cancel_flag());
  if (!rec_viewport_cancel_flag()) {
    bitstream.putFlag(rec_viewport_persistence_flag());
    bitstream.putFlag(rec_viewport_center_view_flag());
    if (!rec_viewport_center_view_flag()) {
      if (rec_viewport_left_view_flag())
        bitstream.putFlag(rec_viewport_left_view_flag());
    }
    bitstream.putFloat32(rec_viewport_pos_x());
    bitstream.putFloat32(rec_viewport_pos_y());
    bitstream.putFloat32(rec_viewport_pos_z());
    bitstream.putFloat32(rec_viewport_quat_x());
    bitstream.putFloat32(rec_viewport_quat_y());
    bitstream.putFloat32(rec_viewport_quat_z());
    bitstream.putFloat32(rec_viewport_hor_range());
    bitstream.putFloat32(rec_viewport_ver_range());
  }
}
} // namespace TMIV::MivBitstream