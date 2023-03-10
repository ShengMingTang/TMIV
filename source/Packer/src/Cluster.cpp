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

#include <TMIV/Packer/Cluster.h>

auto roundToAlignment(int32_t val, int32_t alignment) -> int32_t {
  return ((val - 1) / alignment + 1) * alignment;
}

namespace TMIV::Packer {
Cluster::Cluster(int32_t viewIdx, bool isBasicView, int32_t clusterId, int32_t entityId)
    : viewIdx_(viewIdx), m_isBasicView{isBasicView}, clusterId_(clusterId), entityId_(entityId) {}

void Cluster::push(int32_t i, int32_t j) {
  if (i < imin_) {
    imin_ = i;
  }
  if (imax_ < i) {
    imax_ = i;
  }
  if (j < jmin_) {
    jmin_ = j;
  }
  if (jmax_ < j) {
    jmax_ = j;
  }

  numActivePixels_++;
}

auto Cluster::setEntityId(const Cluster &c, int32_t entityId) -> Cluster {
  Cluster d(c.viewIdx_, c.isBasicView(), c.clusterId_, entityId);
  d.imin_ = c.imin_;
  d.imax_ = c.imax_;
  d.jmin_ = c.jmin_;
  d.jmax_ = c.jmax_;
  d.numActivePixels_ = c.numActivePixels_;
  return d;
}

auto Cluster::align(const Cluster &c, int32_t alignment) -> Cluster {
  Cluster d(c.viewIdx_, c.isBasicView(), c.clusterId_, c.entityId_);

  d.imin_ = c.imin_ - (c.imin_ % alignment);
  d.imax_ = c.imax_; // modification to align the imin,jmin to even values to
                     // help renderer

  d.jmin_ = c.jmin_ - (c.jmin_ % alignment);
  d.jmax_ = c.jmax_; // modification to align the imin,jmin to even values to
                     // help renderer

  d.numActivePixels_ = c.numActivePixels_;

  return d;
}

auto Cluster::merge(const Cluster &c1, const Cluster &c2) -> Cluster {
  Cluster c(c1.viewIdx_, false, c1.clusterId_, c1.entityId_);

  c.imin_ = std::min(c1.imin_, c2.imin_);
  c.imax_ = std::max(c1.imax_, c2.imax_);

  c.jmin_ = std::min(c1.jmin_, c2.jmin_);
  c.jmax_ = std::max(c1.jmax_, c2.jmax_);

  c.numActivePixels_ = c1.numActivePixels_ + c2.numActivePixels_;

  return c;
}

auto Cluster::splitLPatchHorizontally(const ClusteringMap &clusteringMap, std::vector<Cluster> &out,
                                      int32_t alignment, int32_t minPatchSize,
                                      const std::array<std::deque<int32_t>, 2> &min_w_agg,
                                      const std::array<std::deque<int32_t>, 2> &max_w_agg) const
    -> bool {
  double splitThresholdL = 0.9;

  const Cluster &c = (*this);
  const auto &clusteringBuffer = clusteringMap.getPlane(0);
  int32_t H = c.height();
  int32_t W = c.width();

  int32_t alignedImsize = roundToAlignment(W, alignment) * roundToAlignment(H, alignment);

  int32_t minArea = alignedImsize;
  int32_t bestSplitPos = 0;
  for (int32_t h = minPatchSize; h < H - minPatchSize; h++) {
    int32_t currArea = roundToAlignment(h + 1, alignment) *
                           roundToAlignment(max_w_agg[0][h] - min_w_agg[0][h], alignment) +
                       roundToAlignment(H - h - 1, alignment) *
                           roundToAlignment(max_w_agg[1][h] - min_w_agg[1][h], alignment);

    if (minArea > currArea) {
      minArea = currArea;
      bestSplitPos = h;
    }
  }

  if ((bestSplitPos != 0) && static_cast<double>(minArea) / alignedImsize < splitThresholdL) {
    Cluster c1(c.getViewIdx(), c.isBasicView(), c.getClusterId(), c.getEntityId());
    Cluster c2(c.getViewIdx(), c.isBasicView(), c.getClusterId(), c.getEntityId());

    for (int32_t i = c.imin(); i < c.imin() + bestSplitPos + 1; i++) {
      for (int32_t j = c.jmin(); j <= c.jmax(); j++) {
        if (clusteringBuffer(i, j) == c.getClusterId()) {
          c1.push(i, j);
        }
      }
    }
    for (int32_t i = c.imin() + bestSplitPos - 1; i <= c.imax(); i++) {
      for (int32_t j = c.jmin(); j <= c.jmax(); j++) {
        if (clusteringBuffer(i, j) == c.getClusterId()) {
          c2.push(i, j);
        }
      }
    }

    c1.recursiveSplit(clusteringMap, out, alignment, minPatchSize);
    c2.recursiveSplit(clusteringMap, out, alignment, minPatchSize);

    return true;
  }
  return false;
}

auto Cluster::splitCPatchVertically(const ClusteringMap &clusteringMap, std::vector<Cluster> &out,
                                    int32_t alignment, int32_t minPatchSize) const -> bool {
  double splitThresholdC = 0.3;

  const Cluster &c = (*this);
  const auto &clusteringBuffer = clusteringMap.getPlane(0);
  int32_t H = c.height();
  int32_t W = c.width();

  int32_t numOfEmptyBlocks = 0;
  int32_t numOfNonEmptyBlocks = 0;

  for (int32_t h = 0; h < H; h += alignment) {
    for (int32_t w = 0; w < W; w += alignment) {
      bool isEmpty = true;

      for (int32_t hh = h; hh < std::min(h + alignment, H); hh++) {
        int32_t i = hh + c.imin();
        for (int32_t ww = w; ww < std::min(w + alignment, W); ww++) {
          int32_t j = ww + c.jmin();

          if (clusteringBuffer(i, j) == c.getClusterId()) {
            isEmpty = false;
            break;
          }
        } // ww
        if (!isEmpty) {
          break;
        }
      } // hh
      if (isEmpty) {
        numOfEmptyBlocks++;
      } else {
        numOfNonEmptyBlocks++;
      }
    } // w
  }   // h

  if (static_cast<double>(numOfNonEmptyBlocks) / (numOfEmptyBlocks + numOfNonEmptyBlocks) <
      splitThresholdC) {
    int32_t bestSplitPos = roundToAlignment(W / 2, alignment);

    Cluster c1(c.getViewIdx(), c.isBasicView(), c.getClusterId(), c.getEntityId());
    Cluster c2(c.getViewIdx(), c.isBasicView(), c.getClusterId(), c.getEntityId());

    for (int32_t i = c.imin(); i <= c.imax(); i++) {
      for (int32_t j = c.jmin(); j < c.jmin() + bestSplitPos + 1; j++) {
        if (clusteringBuffer(i, j) == c.getClusterId()) {
          c1.push(i, j);
        }
      }
    }
    for (int32_t i = c.imin(); i <= c.imax(); i++) {
      for (int32_t j = c.jmin() + bestSplitPos - 1; j <= c.jmax(); j++) {
        if (clusteringBuffer(i, j) == c.getClusterId()) {
          c2.push(i, j);
        }
      }
    }

    c1.recursiveSplit(clusteringMap, out, alignment, minPatchSize);
    c2.recursiveSplit(clusteringMap, out, alignment, minPatchSize);

    return true;
  }
  return false;
}

auto Cluster::splitCPatchHorizontally(const ClusteringMap &clusteringMap, std::vector<Cluster> &out,
                                      int32_t alignment, int32_t minPatchSize) const -> bool {
  double splitThresholdC = 0.3;

  const Cluster &c = (*this);
  const auto &clusteringBuffer = clusteringMap.getPlane(0);
  int32_t H = c.height();
  int32_t W = c.width();

  int32_t numOfEmptyBlocks = 0;
  int32_t numOfNonEmptyBlocks = 0;

  for (int32_t h = 0; h < H; h += alignment) {
    for (int32_t w = 0; w < W; w += alignment) {
      bool isEmpty = true;

      for (int32_t hh = h; hh < std::min(h + alignment, H); hh++) {
        int32_t i = hh + c.imin();
        for (int32_t ww = w; ww < std::min(w + alignment, W); ww++) {
          int32_t j = ww + c.jmin();

          if (clusteringBuffer(i, j) == c.getClusterId()) {
            isEmpty = false;
            break;
          }
        } // ww
        if (!isEmpty) {
          break;
        }
      } // hh
      if (isEmpty) {
        numOfEmptyBlocks++;
      } else {
        numOfNonEmptyBlocks++;
      }
    } // w
  }   // h

  if (static_cast<double>(numOfNonEmptyBlocks) / (numOfEmptyBlocks + numOfNonEmptyBlocks) <
      splitThresholdC) {
    int32_t bestSplitPos = roundToAlignment(H / 2, alignment);

    Cluster c1(c.getViewIdx(), c.isBasicView(), c.getClusterId(), c.getEntityId());
    Cluster c2(c.getViewIdx(), c.isBasicView(), c.getClusterId(), c.getEntityId());

    for (int32_t i = c.imin(); i < c.imin() + bestSplitPos + 1; i++) {
      for (int32_t j = c.jmin(); j <= c.jmax(); j++) {
        if (clusteringBuffer(i, j) == c.getClusterId()) {
          c1.push(i, j);
        }
      }
    }
    for (int32_t i = c.imin() + bestSplitPos - 1; i <= c.imax(); i++) {
      for (int32_t j = c.jmin(); j <= c.jmax(); j++) {
        if (clusteringBuffer(i, j) == c.getClusterId()) {
          c2.push(i, j);
        }
      }
    }

    c1.recursiveSplit(clusteringMap, out, alignment, minPatchSize);
    c2.recursiveSplit(clusteringMap, out, alignment, minPatchSize);

    return true;
  }
  return false;
}

auto Cluster::splitLPatchVertically(const ClusteringMap &clusteringMap, std::vector<Cluster> &out,
                                    int32_t alignment, int32_t minPatchSize,
                                    const std::array<std::deque<int32_t>, 2> &min_h_agg,
                                    const std::array<std::deque<int32_t>, 2> &max_h_agg) const
    -> bool {
  double splitThresholdL = 0.9;

  const Cluster &c = (*this);
  const auto &clusteringBuffer = clusteringMap.getPlane(0);
  int32_t H = c.height();
  int32_t W = c.width();

  int32_t alignedImsize = roundToAlignment(W, alignment) * roundToAlignment(H, alignment);

  int32_t minArea = alignedImsize;
  int32_t bestSplitPos = 0;
  for (int32_t w = minPatchSize; w < W - minPatchSize; w++) {
    int32_t currArea = roundToAlignment(w + 1, alignment) *
                           roundToAlignment(max_h_agg[0][w] - min_h_agg[0][w], alignment) +
                       roundToAlignment(W - w - 1, alignment) *
                           roundToAlignment(max_h_agg[1][w] - min_h_agg[1][w], alignment);

    if (minArea > currArea) {
      minArea = currArea;
      bestSplitPos = w;
    }
  }

  if ((bestSplitPos != 0) && static_cast<double>(minArea) / alignedImsize < splitThresholdL) {
    Cluster c1(c.getViewIdx(), c.isBasicView(), c.getClusterId(), c.getEntityId());
    Cluster c2(c.getViewIdx(), c.isBasicView(), c.getClusterId(), c.getEntityId());

    for (int32_t i = c.imin(); i <= c.imax(); i++) {
      for (int32_t j = c.jmin(); j < c.jmin() + bestSplitPos + 1; j++) {
        if (clusteringBuffer(i, j) == c.getClusterId()) {
          c1.push(i, j);
        }
      }
    }
    for (int32_t i = c.imin(); i <= c.imax(); i++) {
      for (int32_t j = c.jmin() + bestSplitPos - 1; j <= c.jmax(); j++) {
        if (clusteringBuffer(i, j) == c.getClusterId()) {
          c2.push(i, j);
        }
      }
    }

    c1.recursiveSplit(clusteringMap, out, alignment, minPatchSize);
    c2.recursiveSplit(clusteringMap, out, alignment, minPatchSize);

    return true;
  }
  return false;
}

void Cluster::recursiveSplit(const ClusteringMap &clusteringMap, std::vector<Cluster> &out,
                             int32_t alignment, int32_t minPatchSize) const {
  bool splitted = false;
  const int32_t maxNonSplitTableSize = 64;

  if (width() > height()) { // split vertically
    if (width() > maxNonSplitTableSize) {
      const auto [min_h_agg, max_h_agg] = createAggregatedQueues(clusteringMap, false);
      splitted =
          splitLPatchVertically(clusteringMap, out, alignment, minPatchSize, min_h_agg, max_h_agg);
      if (!splitted) {
        splitted = splitCPatchVertically(clusteringMap, out, alignment, minPatchSize);
      }
    }
  } else { // split horizontally
    if (height() > maxNonSplitTableSize) {
      const auto [min_w_agg, max_w_agg] = createAggregatedQueues(clusteringMap, true);
      splitted = splitLPatchHorizontally(clusteringMap, out, alignment, minPatchSize, min_w_agg,
                                         max_w_agg);
      if (!splitted) {
        splitted = splitCPatchHorizontally(clusteringMap, out, alignment, minPatchSize);
      }
    }
  }

  if (!splitted) {
    out.push_back(*this);
  }
}

auto Cluster::createAggregatedQueues(const ClusteringMap &clusteringMap,
                                     const bool aggregateHorizontally) const
    -> std::tuple<std::array<std::deque<int32_t>, 2>, std::array<std::deque<int32_t>, 2>> {
  const auto aggregationDimensionSize = aggregateHorizontally ? height() : width();

  std::array<std::deque<int32_t>, 2> min_agg{};
  std::array<std::deque<int32_t>, 2> max_agg{};
  const auto [minima, maxima] = computeMinAndMaxVectors(clusteringMap, aggregateHorizontally);

  min_agg[0].push_back(minima[0]);
  max_agg[0].push_back(maxima[0]);
  for (int32_t i = 1; i < aggregationDimensionSize; i++) {
    min_agg[0].push_back(std::min(min_agg[0][i - 1], minima[i]));
    max_agg[0].push_back(std::max(max_agg[0][i - 1], maxima[i]));
  }
  min_agg[1].push_front(minima[aggregationDimensionSize - 1]);
  max_agg[1].push_front(maxima[aggregationDimensionSize - 1]);
  for (int32_t i = aggregationDimensionSize - 2; i >= 0; i--) {
    min_agg[1].push_front(std::min(min_agg[1][0], minima[i]));
    max_agg[1].push_front(std::max(max_agg[1][0], maxima[i]));
  }
  return {min_agg, max_agg};
}

auto Cluster::computeMinAndMaxVectors(const ClusteringMap &clusteringMap,
                                      bool aggregateHorizontally) const
    -> std::tuple<std::vector<int32_t>, std::vector<int32_t>> {
  const auto dim1 = aggregateHorizontally ? height() : width();
  const auto dim2 = aggregateHorizontally ? width() : height();
  const auto minDim1 = aggregateHorizontally ? imin() : jmin();
  const auto minDim2 = aggregateHorizontally ? jmin() : imin();

  const auto &clusteringBuffer = clusteringMap.getPlane(0);
  std::vector<int32_t> minima(dim1, dim2 - 1);
  std::vector<int32_t> maxima(dim1, 0);

  for (int32_t k = 0; k < dim1; k++) {
    int32_t i = k + minDim1;

    for (int32_t l = 0; l < dim2; l++) {
      int32_t j = l + minDim2;
      const auto bufferValue =
          aggregateHorizontally ? clusteringBuffer(i, j) : clusteringBuffer(j, i);
      if (bufferValue == getClusterId()) {
        minima[k] = l;
        break;
      }
    }

    for (int32_t l = dim2 - 1; l >= 0; l--) {
      int32_t j = l + minDim2;
      const auto bufferValue =
          aggregateHorizontally ? clusteringBuffer(i, j) : clusteringBuffer(j, i);
      if (bufferValue == getClusterId()) {
        maxima[k] = l;
        break;
      }
    }
  }

  return {minima, maxima};
}

auto Cluster::split(const ClusteringMap &clusteringMap, int32_t overlap) const
    -> std::pair<Cluster, Cluster> {
  const auto &clusteringBuffer = clusteringMap.getPlane(0);
  const Cluster &c = *this;
  PRECONDITION(!c.isBasicView());
  Cluster c1(c.getViewIdx(), false, c.getClusterId(), c.getEntityId());
  Cluster c2(c.getViewIdx(), false, c.getClusterId(), c.getEntityId());

  if (c.width() < c.height()) {
    int32_t imid = (c.imin() + c.imax()) / 2;
    int32_t imid1 = std::min(imid + overlap, static_cast<int32_t>(clusteringBuffer.m()) - 1);
    int32_t imid2 = std::max(0, imid - overlap);

    for (int32_t i = c.imin(); i < imid1; i++) {
      for (int32_t j = c.jmin(); j <= c.jmax(); j++) {
        if (clusteringBuffer(i, j) == c.getClusterId()) {
          c1.push(i, j);
        }
      }
    }

    for (int32_t i = imid2; i <= c.imax(); i++) {
      for (int32_t j = c.jmin(); j <= c.jmax(); j++) {
        if (clusteringBuffer(i, j) == c.getClusterId()) {
          c2.push(i, j);
        }
      }
    }
  } else {
    int32_t jmid = (c.jmin() + c.jmax()) / 2;
    int32_t jmid1 = std::min(jmid + overlap, static_cast<int32_t>(clusteringBuffer.n()) - 1);
    int32_t jmid2 = std::max(0, jmid - overlap);

    for (int32_t i = c.imin(); i <= c.imax(); i++) {
      for (int32_t j = c.jmin(); j < jmid1; j++) {
        if (clusteringBuffer(i, j) == c.getClusterId()) {
          c1.push(i, j);
        }
      }
    }

    for (int32_t i = c.imin(); i <= c.imax(); i++) {
      for (int32_t j = jmid2; j <= c.jmax(); j++) {
        if (clusteringBuffer(i, j) == c.getClusterId()) {
          c2.push(i, j);
        }
      }
    }
  }
  return {c1, c2};
}
} // namespace TMIV::Packer
