/* The copyright in this software is being made available under the BSD
 * License, included below. This software may be subject to other third party
 * and contributor rights, including patent rights, and no such rights are
 * granted under this license.
 *
 * Copyright (c) 2010-2022, ISO/IEC
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

#include <TMIV/Common/verify.h>

#include <functional>

namespace TMIV::Common {
// This pattern is called a Lippincott function. It makes sure that exceptions are handled uniformly
// by all TMIV executables.
auto handleException() noexcept -> int32_t {
  try {
    throw;
  } catch (std::runtime_error &e) {
    std::cerr << "ERROR: " << e.what() << '\n';
    return 1;
  } catch (std::out_of_range &e) {
    std::cerr << "ERROR: " << e.what() << " [out_of_range]\n";
    return 4;
  } catch (std::bad_function_call &e) {
    std::cerr << "ERROR: " << e.what() << " [bad_function_call]\n";
    return 2;
  } catch (std::logic_error &e) {
    std::cerr << "ERROR: " << e.what() << " [logic_error]\n";
    return 3;
  } catch (std::exception &e) {
    std::cerr << "ERROR: " << e.what() << " [exception]\n";
    return 127;
  } catch (...) {
    std::cerr << "ERROR: Exception of unknown type.\n";
    return 128;
  }
}
} // namespace TMIV::Common