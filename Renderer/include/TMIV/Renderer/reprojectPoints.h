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

#ifndef _TMIV_RENDERER_REPROJECTPOINTS_H_
#define _TMIV_RENDERER_REPROJECTPOINTS_H_

#include <TMIV/Common/Matrix.h>
#include <TMIV/Common/Vector.h>
#include <TMIV/Metadata/CameraParameterList.h>

namespace TMIV::Renderer {
using Vec2f = Common::Vec2f;
using Vec3f = Common::Vec3f;
using Mat1f = Common::Mat<float>;
using Mat2f = Common::Mat<Vec2f>;
using Mat3f = Common::Mat<Vec3f>;
using Metadata::CameraParameters;

// OMAF Referential: x forward, y left, z up
// Image plane: u right, v down

// Unproject points: From image positions to world positions (with the camera as
// reference frame)
Mat3f unprojectPoints(const CameraParameters &camera, const Mat2f &positions,
                      const Mat1f &depth);

// Change the reference frame from one to another camera (merging extrinsic
// parameters)
Mat3f changeReferenceFrame(const CameraParameters &fromCamera,
                           const CameraParameters &toCamera, Mat3f points);

// Project points: From world positions (with the camera as reference frame)
// to image positions
std::pair<Mat2f, Mat1f> projectPoints(const CameraParameters &camera,
                                      const Mat3f &points);

// Reproject points by combining above three steps:
//  1) Unproject to world points in the reference frame of the first camera
//  2) Change the reference frame from the first to the second camera
//  3) Project to image points
std::pair<Mat2f, Mat1f> reprojectPoints(const CameraParameters &fromCamera,
                                        const CameraParameters &toCamera,
                                        const Mat2f &positions,
                                        const Mat1f &depth);
} // namespace TMIV::Renderer

#endif
