/* The copyright in this software is being made available under the BSD
 * License, included below. This software may be subject to other third party
 * and contributor rights, including patent rights, and no such rights are
 * granted under this license.
 *
 * Copyright (c) 2010-2019, ISO/IEC
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

#ifndef _TMIV_COMMON_COMMON_H_
#define _TMIV_COMMON_COMMON_H_

// Common data types and functions that are often used and do not need a
// separate header file

#include <algorithm>
#include <cmath>
#include <limits>
#include <string>

namespace TMIV::Common {
// Format a string using printf syntax
template <class... Args> std::string format(char const *fmt, Args &&... args);

constexpr auto radperdeg{0.01745329251994329576923690768489F};
constexpr auto degperrad{57.295779513082320876798154814092F};
constexpr auto fullCycle{360.F};
constexpr auto halfCycle{180.F};
constexpr auto quarterCycle{90.F};
constexpr auto NaN{std::numeric_limits<float>::quiet_NaN()};
constexpr auto inf{std::numeric_limits<float>::infinity()};

template <typename Float> int ifloor(Float x) { return static_cast<int>(std::floor(x)); }

template <typename Float> int iceil(Float x) { return static_cast<int>(std::ceil(x)); }

template <typename Float> auto square(Float x) { return x * x; }

// http://open-std.org/JTC1/SC22/WG21/docs/papers/2018/p0051r3.pdf
template <typename... Ts> struct Overload : public Ts... {
  template <typename... Us> Overload(Us &&... values) : Ts{std::forward<Us>(values)}... {}
  using Ts::operator()...;
};
template <typename... Ts>
auto overload(Ts &&... values) -> Overload<std::remove_reference_t<Ts>...> {
  return {std::forward<Ts>(values)...};
}

// The maximum level for an unsigned integer of the specified number of bits
constexpr unsigned maxLevel(unsigned bits);

// Expand an integral value to floating-point using a linear transfer function
template <unsigned bits> float expandValue(uint16_t x);

// Quantize a value using a linear transfer function
template <unsigned bits> uint16_t quantizeValue(float x);

// Does a collection contain a specified value?
template <typename Collection, typename Value>
auto contains(const Collection &collection, Value &&value) -> bool {
  using std::cbegin;
  using std::cend;
  return std::any_of(cbegin(collection), cend(collection),
                     [&value](const auto &x) { return x == value; });
}
} // namespace TMIV::Common

#include "Common.hpp"

#endif
