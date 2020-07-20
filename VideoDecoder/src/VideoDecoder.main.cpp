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

#include <TMIV/VideoDecoder/IVideoDecoder.h>

#include <TMIV/MivBitstream/V3cParameterSet.h>
#include <TMIV/VideoDecoder/VideoServer.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>

using namespace std;
using namespace TMIV::Common;
using namespace TMIV::MivBitstream;
using namespace TMIV::VideoDecoder;

constexpr auto defaultCodecGroupIdc = PtlProfileCodecGroupIdc::HEVC_Main10;

auto usage() -> int {
  cout << "Usage: -b BITSTREAM -o RECONSTRUCTION [-c CODEC_GROUP_IDC] [-s] [-S]\n";
  cout << '\n';
  cout << "The default codec group IDC is " << int(defaultCodecGroupIdc) << " ("
       << defaultCodecGroupIdc << ")\n";
  cout << "The reconstructed output is in YUV 4:2:0 10le\n";
  return 1;
}

auto main(int argc, char *argv[]) -> int {
  auto args = vector(argv, argv + argc);
  auto bitstreamPath = optional<string>{};
  auto reconstructionPath = optional<string>{};
  auto codecGroupIdc = optional<PtlProfileCodecGroupIdc>{};
  auto useServer = false;
  auto stressTest = false;

  args.erase(args.begin());

  while (!args.empty()) {
    if (args.front() == "-b"s) {
      if (args.size() == 1) {
        return usage();
      }
      bitstreamPath = args[1];
      args.erase(args.begin(), args.begin() + 2);
    } else if (args.front() == "-o"s) {
      if (args.size() == 1) {
        return usage();
      }
      reconstructionPath = args[1];
      args.erase(args.begin(), args.begin() + 2);
    } else if (args.front() == "-c"s) {
      if (args.size() == 1) {
        return usage();
      }
      codecGroupIdc = PtlProfileCodecGroupIdc(stoi(args[1]));
      args.erase(args.begin(), args.begin() + 2);
    } else if (args.front() == "-s"s) {
      useServer = true;
      args.erase(args.begin());
    } else if (args.front() == "-S"s) {
      stressTest = true;
      args.erase(args.begin());
    } else {
      return usage();
    }
  }

  if (!bitstreamPath || !reconstructionPath) {
    return usage();
  }

  if (codecGroupIdc) {
    cout << "Codec group IDC is set to " << *codecGroupIdc << '\n';
  } else {
    codecGroupIdc = defaultCodecGroupIdc;
  }

  ifstream in{*bitstreamPath, ios::binary};

  if (!in.good()) {
    cout << "Failed to open bitstream \"" << *bitstreamPath << "\" for reading\n";
    return 1;
  }

  ofstream out{*reconstructionPath, ios::binary};

  if (!out.good()) {
    cout << "Failed to open reconstruction file \"" << *reconstructionPath << "\" for writing\n";
    return 1;
  }

  if (stressTest) {
    // Stress-test the video server
    ostringstream buffer;
    buffer << in.rdbuf();
    auto servers = vector<unique_ptr<VideoServer>>{};
    for (int i = 0; i < 20; ++i) {
      servers.push_back(
          make_unique<VideoServer>(IVideoDecoder::create(*codecGroupIdc), buffer.str()));
    }
    for (;;) {
      auto frame = unique_ptr<AnyFrame>{};
      for (auto &server : servers) {
        frame = server->getFrame();
      }
      if (!frame) {
        return 0;
      }
      frame->as<YUV420P10>().dump(out);
    }
  } else if (useServer) {
    // Example of using the video server (with the decoder on a separate thread)
    ostringstream buffer;
    buffer << in.rdbuf();
    auto server = VideoServer{IVideoDecoder::create(*codecGroupIdc), buffer.str()};
    auto frame = server.getFrame();
    while (frame) {
      frame->as<YUV420P10>().dump(out);
      frame = server.getFrame();
    }
  } else {
    // Example of using the video decoder on the same thread
    auto decoder = IVideoDecoder::create(*codecGroupIdc);

    decoder->addFrameListener([&out](const AnyFrame &picture) {
      auto frame = picture.as<YUV420P10>();
      frame.dump(out);
    });
    decoder->decode(in);
  }

  return 0;
}