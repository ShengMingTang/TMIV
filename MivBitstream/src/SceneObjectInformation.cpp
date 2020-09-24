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

#include <TMIV/MivBitstream/SceneObjectInformation.h>

namespace TMIV::MivBitstream {
auto SceneObjectInformation::soi_persistence_flag() const noexcept -> bool {
  return m_soi_persistence_flag;
}
auto SceneObjectInformation::soi_reset_flag() const noexcept -> bool { return m_soi_reset_flag; }
auto SceneObjectInformation::soi_num_object_updates() const noexcept -> std::size_t {
  return m_object_updates.soi_num_object_updates();
}
auto SceneObjectInformation::soi_simple_objects_flag() const noexcept -> bool {
  VERIFY_BITSTREAM(m_object_updates.has_value());
  return m_object_updates.soi_simple_objects_flag;
}
auto SceneObjectInformation::soi_object_label_present_flag() const noexcept -> bool {
  VERIFY_BITSTREAM(m_object_updates.has_value());
  return !m_object_updates.soi_simple_objects_flag;
}
auto SceneObjectInformation::soi_priority_present_flag() const noexcept -> bool {
  VERIFY_BITSTREAM(m_object_updates.has_value());
  return !m_object_updates.soi_simple_objects_flag;
}
auto SceneObjectInformation::soi_object_hidden_present_flag() const noexcept -> bool {
  VERIFY_BITSTREAM(m_object_updates.has_value());
  return !m_object_updates.soi_simple_objects_flag;
}
auto SceneObjectInformation::soi_object_dependency_present_flag() const noexcept -> bool {
  VERIFY_BITSTREAM(m_object_updates.has_value());
  return !m_object_updates.soi_simple_objects_flag;
}
auto SceneObjectInformation::soi_visibility_cones_present_flag() const noexcept -> bool {
  VERIFY_BITSTREAM(m_object_updates.has_value());
  return !m_object_updates.soi_simple_objects_flag;
}
auto SceneObjectInformation::soi_3d_bounding_box_present_flag() const noexcept -> bool {
  VERIFY_BITSTREAM(m_object_updates.has_value());
  return !m_object_updates.soi_simple_objects_flag;
}
auto SceneObjectInformation::soi_collision_shape_present_flag() const noexcept -> bool {
  VERIFY_BITSTREAM(m_object_updates.has_value());
  return !m_object_updates.soi_simple_objects_flag;
}
auto SceneObjectInformation::soi_point_style_present_flag() const noexcept -> bool {
  VERIFY_BITSTREAM(m_object_updates.has_value());
  return !m_object_updates.soi_simple_objects_flag;
}
auto SceneObjectInformation::soi_material_id_present_flag() const noexcept -> bool {
  VERIFY_BITSTREAM(m_object_updates.has_value());
  return !m_object_updates.soi_simple_objects_flag;
}
auto SceneObjectInformation::soi_extension_present_flag() const noexcept -> bool {
  VERIFY_BITSTREAM(m_object_updates.has_value());
  return !m_object_updates.soi_simple_objects_flag;
}
auto SceneObjectInformation::soi_3d_bounding_box_scale_log2() const noexcept -> std::uint8_t {
  VERIFY_BITSTREAM(m_object_updates.has_value());
  return m_object_updates.soi_3d_bounding_box_scale_log2;
}
auto SceneObjectInformation::soi_log2_max_object_idx_updated_minus1() const noexcept
    -> std::uint8_t {
  VERIFY_BITSTREAM(m_object_updates.has_value());
  return m_object_updates.soi_log2_max_object_idx_updated_minus1;
}
auto SceneObjectInformation::soi_log2_max_object_dependency_idx() const noexcept -> std::uint8_t {
  VERIFY_BITSTREAM(m_object_updates.has_value());
  return m_object_updates.soi_log2_max_object_dependency_idx;
}
auto SceneObjectInformation::soi_object_idx(std::size_t i) const noexcept -> std::uint8_t {
  return true;
}
auto SceneObjectInformation::soi_object_cancel_flag(std::size_t k) const noexcept -> bool {
  return true;
}
auto SceneObjectInformation::soi_object_label_update_flag(std::size_t k) const noexcept -> bool {
  return true;
}
auto SceneObjectInformation::soi_object_label_idx(std::size_t k) const noexcept -> std::size_t {
  return true;
}
auto SceneObjectInformation::soi_priority_update_flag(std::size_t k) const noexcept -> bool {
  return true;
}
auto SceneObjectInformation::soi_priority_value(std::size_t k) const noexcept -> std::uint8_t {
  return true;
}
auto SceneObjectInformation::soi_object_hidden_flag(std::size_t k) const noexcept -> bool {
  return true;
}
auto SceneObjectInformation::soi_object_dependency_update_flag(std::size_t k) const noexcept
    -> bool {
  return true;
}
auto SceneObjectInformation::soi_object_num_dependencies(std::size_t k) const noexcept
    -> std::uint8_t {
  return 0;
}
auto SceneObjectInformation::soi_object_dependency_idx(std::size_t k, std::size_t j) const noexcept
    -> std::uint8_t {
  return 0;
}
auto SceneObjectInformation::soi_visibility_cones_update_flag(std::size_t k) const noexcept
    -> bool {
  return true;
}
auto SceneObjectInformation::soi_direction_x(std::size_t k) const noexcept -> std::int16_t {
  return 0;
}
auto SceneObjectInformation::soi_direction_y(std::size_t k) const noexcept -> std::int16_t {
  return 0;
}
auto SceneObjectInformation::soi_direction_z(std::size_t k) const noexcept -> std::int16_t {
  return 0;
}
auto SceneObjectInformation::soi_angle(std::size_t k) const noexcept -> std::uint16_t { return 0; }
auto SceneObjectInformation::soi_3d_bounding_box_update_flag(std::size_t k) const noexcept -> bool {
  return true;
}
auto SceneObjectInformation::soi_3d_bounding_box_x(std::size_t k) const noexcept -> std::size_t {
  return 0;
}
auto SceneObjectInformation::soi_3d_bounding_box_y(std::size_t k) const noexcept -> std::size_t {
  return 0;
}
auto SceneObjectInformation::soi_3d_bounding_box_z(std::size_t k) const noexcept -> std::size_t {
  return 0;
}
auto SceneObjectInformation::soi_3d_bounding_box_size_x(std::size_t k) const noexcept
    -> std::size_t {
  return 0;
}
auto SceneObjectInformation::soi_3d_bounding_box_size_y(std::size_t k) const noexcept
    -> std::size_t {
  return 0;
}
auto SceneObjectInformation::soi_3d_bounding_box_size_z(std::size_t k) const noexcept
    -> std::size_t {
  return 0;
}
auto SceneObjectInformation::soi_collision_shape_update_flag(std::size_t k) const noexcept -> bool {
  return true;
}
auto SceneObjectInformation::soi_collision_shape_id(std::size_t k) const noexcept -> std::uint16_t {
  return 0;
}
auto SceneObjectInformation::soi_point_style_update_flag(std::size_t k) const noexcept -> bool {
  return true;
}
auto SceneObjectInformation::soi_point_shape_id(std::size_t k) const noexcept -> std::uint8_t {
  return true;
}
auto SceneObjectInformation::soi_point_size(std::size_t k) const noexcept -> std::uint16_t {
  return 0;
}
auto SceneObjectInformation::soi_material_id_update_flag(std::size_t k) const noexcept -> bool {
  return true;
}
auto SceneObjectInformation::soi_material_id(std::size_t k) const noexcept -> std::uint16_t {
  return 0;
}

auto operator<<(std::ostream &stream, const SceneObjectInformation &x) -> std::ostream & {
  stream << "soi_persistence_flag=" << std::boolalpha << x.soi_persistence_flag() << "\n";
  stream << "soi_reset_flag=" << std::boolalpha << x.soi_reset_flag() << "\n";
  stream << "soi_num_object_updates=" << x.soi_num_object_updates() << "\n";
  if (x.soi_num_object_updates() > 0) {
    stream << "soi_simple_objects_flag=" << std::boolalpha << x.soi_simple_objects_flag() << "\n";
    stream << "soi_object_label_present_flag=" << std::boolalpha
           << x.soi_object_label_present_flag() << "\n";
    stream << "soi_priority_present_flag=" << std::boolalpha << x.soi_priority_present_flag()
           << "\n";
    stream << "soi_object_hidden_present_flag=" << std::boolalpha
           << x.soi_object_hidden_present_flag() << "\n";
    stream << "soi_object_dependency_present_flag=" << std::boolalpha
           << x.soi_object_dependency_present_flag() << "\n";
    stream << "soi_visibility_cones_present_flag=" << std::boolalpha
           << x.soi_visibility_cones_present_flag() << "\n";
    stream << "soi_3d_bounding_box_present_flag=" << std::boolalpha
           << x.soi_3d_bounding_box_present_flag() << "\n";
    stream << "soi_collision_shape_present_flag=" << std::boolalpha
           << x.soi_collision_shape_present_flag() << "\n";
    stream << "soi_point_style_present_flag=" << std::boolalpha << x.soi_point_style_present_flag()
           << "\n";
    stream << "soi_material_id_present_flag=" << std::boolalpha << x.soi_material_id_present_flag()
           << "\n";
    stream << "soi_extension_present_flag=" << std::boolalpha << x.soi_extension_present_flag()
           << "\n";
    if (x.soi_3d_bounding_box_present_flag()) {
      stream << "soi_3d_bounding_box_scale_log2="
             << static_cast<unsigned>(x.soi_3d_bounding_box_scale_log2()) << "\n";
    }
    stream << "soi_log2_max_object_idx_updated_minus1="
           << static_cast<unsigned>(x.soi_log2_max_object_idx_updated_minus1()) << "\n";
    if (x.soi_object_dependency_present_flag()) {
      stream << "soi_log2_max_object_dependency_idx="
             << static_cast<unsigned>(x.soi_log2_max_object_dependency_idx()) << "\n";
    }
  }
  return stream;
}

auto SceneObjectInformation::operator==(const SceneObjectInformation &other) const noexcept
    -> bool {
  return (m_soi_persistence_flag == other.m_soi_persistence_flag) &&
         (m_soi_reset_flag == other.m_soi_reset_flag) &&
         (m_object_updates == other.m_object_updates);
}

auto SceneObjectInformation::operator!=(const SceneObjectInformation &other) const noexcept
    -> bool {
  return !operator==(other);
}

auto SceneObjectInformation::decodeFrom(Common::InputBitstream &bitstream)
    -> SceneObjectInformation {
  SceneObjectInformation result{};
  result.soi_persistence_flag(bitstream.getFlag());
  result.soi_reset_flag(bitstream.getFlag());

  SceneObjectUpdates sceneObjectUpdates{};
  sceneObjectUpdates.soi_num_object_updates(bitstream.getUExpGolomb<std::size_t>());
  if (sceneObjectUpdates.soi_num_object_updates() > 0) {
    sceneObjectUpdates.soi_simple_objects_flag = bitstream.getFlag();
    const int numberOfSimpleObjectFlags = 9;
    for (int i = 0; i < numberOfSimpleObjectFlags; ++i) {
      bitstream.getFlag();
    }
    if (sceneObjectUpdates.soi_3d_bounding_box_present_flag()) {
      sceneObjectUpdates.soi_3d_bounding_box_scale_log2 = bitstream.readBits<std::uint8_t>(5);
    }
    sceneObjectUpdates.soi_log2_max_object_idx_updated_minus1 = bitstream.readBits<std::uint8_t>(5);
    if (sceneObjectUpdates.soi_object_dependency_present_flag()) {
      sceneObjectUpdates.soi_log2_max_object_dependency_idx = bitstream.readBits<std::uint8_t>(5);
    }
  }
  result.setSceneObjectUpdates(std::move(sceneObjectUpdates));
  return result;
}

void SceneObjectInformation::encodeTo(Common::OutputBitstream &bitstream) const {
  bitstream.putFlag(soi_persistence_flag());
  bitstream.putFlag(soi_reset_flag());
  bitstream.putUExpGolomb(soi_num_object_updates());
  if (soi_num_object_updates() > 0) {
    bitstream.putFlag(soi_simple_objects_flag());
    bitstream.putFlag(soi_object_label_present_flag());
    bitstream.putFlag(soi_priority_present_flag());
    bitstream.putFlag(soi_object_hidden_present_flag());
    bitstream.putFlag(soi_visibility_cones_present_flag());
    bitstream.putFlag(soi_3d_bounding_box_present_flag());
    bitstream.putFlag(soi_collision_shape_present_flag());
    bitstream.putFlag(soi_point_style_present_flag());
    bitstream.putFlag(soi_material_id_present_flag());
    bitstream.putFlag(soi_extension_present_flag());
    if (soi_3d_bounding_box_present_flag()) {
      bitstream.writeBits(soi_3d_bounding_box_scale_log2(), 5);
    }
    bitstream.writeBits(soi_log2_max_object_idx_updated_minus1(), 5);
    if (soi_object_dependency_present_flag()) {
      bitstream.writeBits(soi_log2_max_object_dependency_idx(), 5);
    }
  }
}
} // namespace TMIV::MivBitstream
