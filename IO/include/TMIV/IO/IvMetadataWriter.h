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

#ifndef _TMIV_IO_IVMETADATAWRITER_H_
#define _TMIV_IO_IVMETADATAWRITER_H_

#include <TMIV/Metadata/Bitstream.h>
#include <TMIV/Metadata/IvAccessUnitParams.h>
#include <TMIV/Metadata/IvSequenceParams.h>

#include <fstream>

namespace TMIV::IO {
class IvMetadataWriter {
public:
  IvMetadataWriter(const Common::Json &config, const std::string &baseDirectoryField,
                   const std::string &fileNameField);

  void writeIvSequenceParams(Metadata::IvSequenceParams);
  void writeIvAccessUnitParams(Metadata::IvAccessUnitParams);

  auto cameraList() const -> const Metadata::ViewParamsList &;

private:
  std::string m_path;
  std::ofstream m_stream;
  Metadata::OutputBitstream m_bitstream{m_stream};
  Metadata::IvSequenceParams m_ivsParams;
  Metadata::IvAccessUnitParams m_ivAccessUnitParams;
};

inline auto IvMetadataWriter::cameraList() const -> const Metadata::ViewParamsList & {
  return m_ivsParams.cameraParamsList;
}
} // namespace TMIV::IO

#endif
