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

#include <TMIV/AtlasConstructor/HierarchicalPruner.h>

#include "../../Renderer/src/Rasterizer.h"
#include <TMIV/Image/Image.h>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <future>
#include <iostream>
#include <numeric>

using namespace TMIV::Common;
using namespace TMIV::Metadata;
using namespace TMIV::Renderer;
using namespace TMIV::Image;
using namespace std;

namespace TMIV::AtlasConstructor {
class HierarchicalPruner::Impl {
private:
  struct Synthesizer {
    Synthesizer(const AccumulatingPixel<Vec3f> &config, Vec2i size, size_t index_)
        : rasterizer{config, size}, index{index_} {}

    Rasterizer<Vec3f> rasterizer;
    size_t index;
    float maskAverage{0.F};
  };

  const float m_maxStretching{};
  const int m_erode{};
  const int m_dilate{};
  const AccumulatingPixel<Vec3f> m_config;
  bool m_intra{true};
  bool m_firstFrame{true};
  CameraParametersList m_cameras;
  vector<uint8_t> m_isReferenceView;
  vector<unique_ptr<Synthesizer>> m_synthesizers;
  vector<size_t> m_pruningOrder;
  vector<Frame<YUV400P8>> m_masks;

public:
  explicit Impl(const Json &nodeConfig)
      : m_maxStretching{nodeConfig.require("maxStretching").asFloat()},
        m_erode{nodeConfig.require("erode").asInt()},
        m_dilate{nodeConfig.require("dilate").asInt()},
        m_config{nodeConfig.require("rayAngleParameter").asFloat(),
                 nodeConfig.require("depthParameter").asFloat(),
                 nodeConfig.require("stretchingParameter").asFloat(), m_maxStretching} {}

  auto prune(const CameraParametersList &cameras, const MVD16Frame &views,
             const vector<uint8_t> &isReferenceView) -> MaskList {
    m_intra = m_cameras != cameras || m_isReferenceView != isReferenceView;
    cout << "prune: m_intra=" << m_intra << "\n";
    if (m_intra) {
      m_cameras = cameras;
      m_isReferenceView = isReferenceView;
    }
    prepareFrame(views);
    pruneFrame(views);

    m_firstFrame = false;
    return move(m_masks);
  }

private:
  void prepareFrame(const MVD16Frame &views) {
    createInitialMasks();
    createSynthesizerPerPartialView();
    synthesizeReferenceViews(views);
  }

  void createInitialMasks() {
    m_masks.clear();
    for (auto &camera : m_cameras) {
      m_masks.emplace_back(Frame<YUV400P8>{camera.size.x(), camera.size.y()});
      auto &mask = m_masks.back();
      fill(begin(mask.getPlane(0)), end(mask.getPlane(0)), uint8_t(255));
    }
  }

  void createSynthesizerPerPartialView() {
    m_synthesizers.clear();
    for (size_t i = 0; i < m_cameras.size(); ++i) {
      if (m_isReferenceView[i] == 0) {
        m_synthesizers.emplace_back(make_unique<Synthesizer>(m_config, m_cameras[i].size, i));
      }
    }
  }

  void synthesizeReferenceViews(const MVD16Frame &views) {
    for (size_t i = 0; i < m_cameras.size(); ++i) {
      if (m_isReferenceView[i] > 0) {
        cout << "synthesizeReferenceViews: view=" << i << "\n";
        synthesizeViews(i, views[i]);
      }
    }
  }

  void pruneFrame(const MVD16Frame &views) {
    if (m_intra) {
      // Redetermine the pruning order on camera configuration changes...
      pruneIntraFrame(views);
    } else {
      // ... otherwise maintain the same pruning order
      pruneInterFrame(views);
    }

    auto sumValues = 0.;
    auto sumPixels = 0.;
    for (const auto &mask : m_masks) {
      sumValues = accumulate(begin(mask.getPlane(0)), end(mask.getPlane(0)), sumValues);
      sumPixels += mask.getWidth() * mask.getHeight();
    }
    auto numAtlases = double(m_masks.size()) * sumValues / (255. * sumPixels);
    cout << "Theoretical number of atlases is " << numAtlases << '\n';
  }

  void pruneIntraFrame(const MVD16Frame &views) {
    m_pruningOrder.clear();
    while (!m_synthesizers.empty()) {
      auto it = max_element(
          begin(m_synthesizers), end(m_synthesizers),
          [](const auto &s1, const auto &s2) { return s1->maskAverage < s2->maskAverage; });
      cout << "pruneIntraFrame: view=" << (*it)->index << ", maskAverage=" << (*it)->maskAverage
           << "\n";
      const auto i = (*it)->index;
      m_synthesizers.erase(it);
      synthesizeViews(i, views[i]);
      m_pruningOrder.push_back(i);
    }
  }

  void pruneInterFrame(const MVD16Frame &views) {
    for (auto i : m_pruningOrder) {
      auto it = find_if(begin(m_synthesizers), end(m_synthesizers),
                        [i](const auto &s) { return s->index == i; });
      cout << "pruneInterFrame: view=" << i << ", maskAverage=" << (*it)->maskAverage << "\n";
      m_synthesizers.erase(it);
      synthesizeViews(i, views[i]);
    }
  }

  // Unproject a pruned (masked) view, resulting in a mesh in the reference
  // frame of the source view
  void maskedUnproject(size_t index, const TextureDepth16Frame &view,
                       SceneVertexDescriptorList &vertices, TriangleDescriptorList &triangles,
                       vector<Vec3f> &attributes) const {
    const auto &camera = m_cameras[index];
    switch (camera.type) {
    case ProjectionType::ERP: {
      Engine<ProjectionType::ERP> engine{camera};
      return maskedUnproject(engine, index, view, vertices, triangles, attributes);
    }
    case ProjectionType::Perspective: {
      Engine<ProjectionType::Perspective> engine{camera};
      return maskedUnproject(engine, index, view, vertices, triangles, attributes);
    }
    default:
      abort();
    }
  }

  template <typename E>
  void maskedUnproject(E &engine, size_t index, const TextureDepth16Frame &view,
                       SceneVertexDescriptorList &vertices, TriangleDescriptorList &triangles,
                       vector<Vec3f> &attributes) const {
    const auto &camera = m_cameras[index];
    const auto &mask = m_masks[index].getPlane(0);
    const auto size = camera.size;
    const auto numPixels = size.x() * size.y();

    const auto &Y = view.first.getPlane(0);
    const auto &U = view.first.getPlane(1);
    const auto &V = view.first.getPlane(2);
    const auto &D = view.second.getPlane(0);

    assert(vertices.empty());
    vertices.reserve(numPixels);
    assert(attributes.empty());
    attributes.reserve(numPixels);

    vector<int> key;
    key.reserve(vertices.size());

    for (int y = 0; y < size.y(); ++y) {
      for (int x = 0; x < size.x(); ++x) {
        key.push_back(int(vertices.size()));

        if (mask(y, x) > 0) {
          const auto uv = Vec2f{float(x) + 0.5F, float(y) + 0.5F};
          const auto d = expandDepthValue<16>(camera, D(y, x));
          vertices.push_back({engine.unprojectVertex(uv, d), NaN});
          attributes.emplace_back(Vec3f{expandValue<10U>(Y(y, x)),
                                        expandValue<10U>(U(y / 2, x / 2)),
                                        expandValue<10U>(V(y / 2, x / 2))});
        }
      }
    }

    if (vertices.capacity() > 2 * vertices.size()) {
      vertices.shrink_to_fit();
      attributes.shrink_to_fit();
    }

    cout << "Mesh has " << vertices.size() << " vertices (" << double(vertices.size()) / numPixels
         << " of full view)\n";

    assert(triangles.empty());
    const auto maxTriangles = 2 * vertices.size();
    triangles.reserve(maxTriangles);
    cout << "Reserving for " << maxTriangles << " triangles\n";

    const auto considerTriangle = [&](Vec2i a, Vec2i b, Vec2i c) {
      if (mask(a.y(), a.x()) == 0 || mask(b.y(), b.x()) == 0 || mask(c.y(), c.x()) == 0) {
        return;
      }

      const auto ia = key[a.y() * size.x() + a.x()];
      const auto ib = key[b.y() * size.x() + b.x()];
      const auto ic = key[c.y() * size.x() + c.x()];
      triangles.push_back({{ia, ib, ic}, 0.5F});
    };

    for (int y = 1; y < size.y(); ++y) {
      for (int x = 1; x < size.x(); ++x) {
        considerTriangle({x - 1, y - 1}, {x, y - 1}, {x, y});
        considerTriangle({x - 1, y - 1}, {x, y}, {x - 1, y});
      }
    }

    cout << "The mesh has " << triangles.size() << " triangles\n";
  }

  // Change reference frame and project vertices
  void project(const SceneVertexDescriptorList &in, size_t iview, ImageVertexDescriptorList &out,
               size_t oview) const {
    const auto &camera = m_cameras[oview];
    switch (camera.type) {
    case ProjectionType::ERP: {
      Engine<ProjectionType::ERP> engine{camera};
      return project(engine, in, iview, out, oview);
    }
    case ProjectionType::Perspective: {
      Engine<ProjectionType::Perspective> engine{camera};
      return project(engine, in, iview, out, oview);
    }
    default:
      abort();
    }
  }

  template <typename E>
  void project(const E &engine, const SceneVertexDescriptorList &in, size_t iview,
               ImageVertexDescriptorList &out, size_t oview) const {
    const auto [R, t] = affineParameters(m_cameras[iview], m_cameras[oview]);
    out.clear();
    out.reserve(in.size());
    transform(begin(in), end(in), back_inserter(out),
              [&engine, R = R, t = t](SceneVertexDescriptor v) {
                const auto p = R * v.position + t;
                return engine.projectVertex({p, angle(p, p - t)});
              });
  }

  // Weighted sphere compensation of stretching as performed by
  // Engine<ERP>::project
  void weightedSphere(const CameraParameters &target, const ImageVertexDescriptorList &vertices,
                      TriangleDescriptorList &triangles) const {
    switch (target.type) {
    case ProjectionType::ERP: {
      Engine<ProjectionType::ERP> engine{target};
      return weightedSphere(engine, vertices, triangles);
    }
    case ProjectionType::Perspective:
      return;
    default:
      abort();
    }
  }

  void weightedSphere(const Engine<ProjectionType::ERP> &engine,
                      const ImageVertexDescriptorList &vertices,
                      TriangleDescriptorList &triangles) const {
    for (auto &triangle : triangles) {
      auto v = 0.F;
      for (auto index : triangle.indices) {
        v += vertices[index].position.y() / 3.F;
      }
      const auto theta = engine.theta0 + engine.dtheta_dv * v;
      triangle.area = 0.5F / cos(theta);
    }
  }

  // Synthesize the specified view to all remaining partial views.
  //
  // Special care is taken to make a pruned (masked) mesh once and re-use that
  // multiple times.
  void synthesizeViews(size_t index, const TextureDepth16Frame &view) {
    SceneVertexDescriptorList ivertices;
    TriangleDescriptorList triangles;
    vector<Vec3f> attributes;
    maskedUnproject(index, view, ivertices, triangles, attributes);

    ImageVertexDescriptorList overtices; // allocate once

    for (auto &s : m_synthesizers) {
      project(ivertices, index, overtices, s->index);
      weightedSphere(m_cameras[s->index], overtices, triangles);
      s->rasterizer.submit(overtices, attributes, triangles);
      s->rasterizer.run();
      updateMask(*s);
    }
  }

  // Visit all pixels
  template <typename F> static void forPixels(array<size_t, 2> sizes, F f) {
    for (int i = 0; i < int(sizes[0]); ++i) {
      for (int j = 0; j < int(sizes[1]); ++j) {
        f(i, j);
      }
    }
  }

  // Visit all pixel neighbors (in between 3 and 8)
  template <typename F> static bool forNeighbors(int i, int j, array<size_t, 2> sizes, F f) {
    const int n1 = max(0, i - 1);
    const int n2 = min(int(sizes[0]), i + 2);
    const int m1 = max(0, j - 1);
    const int m2 = min(int(sizes[1]), j + 2);

    for (int n = n1; n < n2; ++n) {
      for (int m = m1; m < m2; ++m) {
        if (!f(n, m)) {
          return false;
        }
      }
    }
    return true;
  }

  Mat<uint8_t> erode(Mat<uint8_t> &mask) const {
    Mat<uint8_t> result{mask.sizes()};
    forPixels(mask.sizes(), [&](int i, int j) {
      result(i, j) =
          forNeighbors(i, j, mask.sizes(), [&mask](int n, int m) { return mask(n, m) > 0; }) ? 255
                                                                                             : 0;
    });
    return result;
  }

  Mat<uint8_t> dilate(Mat<uint8_t> &mask) const {
    Mat<uint8_t> result{mask.sizes()};
    forPixels(mask.sizes(), [&](int i, int j) {
      result(i, j) =
          forNeighbors(i, j, mask.sizes(), [&mask](int n, int m) { return mask(n, m) == 0; }) ? 0
                                                                                              : 255;
    });
    return result;
  }

  void updateMask(Synthesizer &synthesizer) {
    auto &mask = m_masks[synthesizer.index].getPlane(0);
    auto i = begin(mask);
    synthesizer.rasterizer.visit([&i](const PixelValue<Vec3f> &x) {
      *i++ = uint8_t(x.normDisp > 0 ? 0 : 255);
      return true;
    });
    for (int n = 0; n < m_erode; ++n) {
      mask = erode(mask);
    }
    for (int n = 0; n < m_dilate; ++n) {
      mask = dilate(mask);
    }
    synthesizer.maskAverage =
        float(accumulate(begin(mask), end(mask), 0)) / float(mask.width() * mask.height());
    cout << "  New maskAverage for " << synthesizer.index << " is " << synthesizer.maskAverage
         << "\n";
  }
}; // namespace TMIV::AtlasConstructor

HierarchicalPruner::HierarchicalPruner(const Json & /* unused */, const Json &nodeConfig)
    : m_impl(new Impl{nodeConfig}) {}

HierarchicalPruner::~HierarchicalPruner() = default;

auto HierarchicalPruner::prune(const CameraParametersList &cameras, const MVD16Frame &views,
                               const vector<uint8_t> &shouldNotBePruned) -> MaskList {
  return m_impl->prune(cameras, views, shouldNotBePruned);
}
} // namespace TMIV::AtlasConstructor
