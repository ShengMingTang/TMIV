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

#ifndef _TMIV_MIVBITSTREAM_ATLASADAPTATIONPARAMETERSETRBSP_H_
#error "Include the .h, not the .hpp"
#endif

namespace TMIV::MivBitstream {
constexpr auto AapsVpccExtension::operator==(const AapsVpccExtension & /* other */) const noexcept {
  return true;
}
constexpr auto AapsVpccExtension::operator!=(const AapsVpccExtension & /* other */) const noexcept {
  return false;
}

constexpr auto AapsMivExtension::aame_omaf_v1_compatible_flag() const noexcept {
  return m_aame_omaf_v1_compatible_flag;
}

constexpr auto AapsMivExtension::aame_vui_params_present_flag() const noexcept {
  return m_aame_vui_params_present_flag;
}

constexpr auto AapsMivExtension::aame_omaf_v1_compatible_flag(bool value) noexcept -> auto & {
  m_aame_omaf_v1_compatible_flag = value;
  return *this;
}

constexpr auto AapsMivExtension::aame_vui_params_present_flag(bool value) noexcept -> auto & {
  m_aame_vui_params_present_flag = value;
  return *this;
}

constexpr auto
AtlasAdaptationParameterSetRBSP::aaps_atlas_adaptation_parameter_set_id() const noexcept {
  return m_aaps_atlas_adaptation_parameter_set_id;
}

constexpr auto AtlasAdaptationParameterSetRBSP::aaps_log2_max_afoc_present_flag() const noexcept {
  return m_aaps_log2_max_afoc_present_flag;
}

constexpr auto AtlasAdaptationParameterSetRBSP::aaps_extension_flag() const noexcept {
  return m_aaps_extension_flag;
}

constexpr auto AtlasAdaptationParameterSetRBSP::aaps_vpcc_extension_flag() const noexcept {
  return m_aaps_vpcc_extension_flag.value_or(false);
}

constexpr auto AtlasAdaptationParameterSetRBSP::aaps_miv_extension_flag() const noexcept {
  return m_aaps_miv_extension_flag.value_or(false);
}

constexpr auto AtlasAdaptationParameterSetRBSP::aaps_extension_6bits() const noexcept {
  return m_aaps_extension_6bits.value_or(0);
}

constexpr auto AtlasAdaptationParameterSetRBSP::aaps_atlas_adaptation_parameter_set_id(
    const std::uint8_t value) noexcept -> auto & {
  m_aaps_atlas_adaptation_parameter_set_id = value;
  return *this;
}

constexpr auto AtlasAdaptationParameterSetRBSP::aaps_log2_max_afoc_present_flag(bool value) noexcept
    -> auto & {
  m_aaps_log2_max_afoc_present_flag = value;
  return *this;
}

constexpr auto AtlasAdaptationParameterSetRBSP::aaps_extension_flag(bool value) noexcept -> auto & {
  m_aaps_extension_flag = value;
  return *this;
}
} // namespace TMIV::MivBitstream
