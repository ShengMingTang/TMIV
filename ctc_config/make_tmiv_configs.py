#!/usr/bin/env python3
#
# The copyright in this software is being made available under the BSD
# License, included below. This software may be subject to other third party
# and contributor rights, including patent rights, and no such rights are
# granted under this license.
#
# Copyright (c) 2010-2019, ISO/IEC
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#  * Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#  * Neither the seqName of the ISO/IEC nor the names of its contributors may
#    be used to endorse or promote products derived from this software without
#    specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS'
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
# BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
# THE POSSIBILITY OF SUCH DAMAGE.

import json
import os
import sys

class DecoderConfiguration:
	def __init__(self, anchorId, seqId, testPoint):
		self.anchorId = anchorId
		self.seqId = seqId
		self.testPoint = testPoint
		with open(self.sequenceJsonPath(), 'r') as stream:			
			self.sequenceParams = json.load(stream)

	def numberOfFrames(self):
		if self.anchorId == 'anchor97':
			return 97
		return 17

	def intraPeriod(self):
		return 32

	def sourceDirectory(self):
		return '.'

	def seqName(self):
		return {
			'A': 'ClassroomVideo',
			'B': 'TechnicolorMuseum',
			'C': 'TechnicolorHijack',
			'D': 'TechnicolorPainter',
			'E': 'IntelFrog',
			'J': 'OrangeKitchen',
			'L': 'PoznanFencing'
		}[self.seqId]

	def sourceCameraParameters(self):
		return '{}.json'.format(self.seqName())	

	def sequenceJsonPath(self):
		return 'sequences/{}'.format(self.sourceCameraParameters())

	def firstSourceCamera(self):
		for camera in self.sequenceParams['cameras']:
			if camera['Name'] == self.firstSourceCameraName():
				return camera
		raise 'Could not find first source camera in sequence configuration'

	def viewWidth(self):
		return self.firstSourceCamera()['Resolution'][0]

	def viewHeight(self):
		return self.firstSourceCamera()['Resolution'][1]

	def atlasWidth(self):
		return self.viewWidth()

	def atlasHeight(self):
		return {
			'A': 3072,
			'B': 2368,
			'C': 5120,
			'D': 1088,
			'E': 1984,
			'J': 1080,
			'L': 1080
		}[self.seqId]

	def atlasTexturePathFmt(self):
		return 'ATL_S{}_{}_Tt_c%02d_{}x{}_yuv420p10le.yuv'.format(self.seqId, self.testPoint, self.atlasWidth(), self.atlasHeight())

	def atlasDepthPathFmt(self):
		return 'ATL_S{}_{}_Td_c%02d_{}x{}_yuv420p10le.yuv'.format(self.seqId, self.testPoint, self.atlasWidth(), self.atlasHeight())

	def atlasMetadataPath(self):
		return 'ATL_S{}_{}_Tm_c00.bit'.format(self.seqId, self.testPoint)

	def outputDirectory(self):
		return '.'

	def outputTexturePath(self):
		return 'TM1_S{}_{}_Tt_%s_{}x{}_yuv420p10le.yuv'.format(self.seqId, self.testPoint, self.viewWidth(), self.viewHeight())

	def outputCameraName(self):
		return self.viewNameFormat().format(1)

	def synthesizer(self):
		return {
			'rayAngleParameter': 10,
			'depthParameter': 50,
			'stretchingParameter': 3,
			'maxStretching': 5
		}

	def numberOfSourceViews(self):
		return {
			'A': 15,
			'B': 24,
			'C': 10,
			'D': 16,
			'E': 13,
			'J': 25,
			'L': 10
		}[self.seqId]

	def firstSourceView(self):
		if self.seqId == 'E':
			return 1
		return 0

	def viewNameFormat(self):
		if self.seqId == 'J' or self.seqId == 'L':
			return 'v{:02}'
		return 'v{}'

	def sourceCameraNames(self):
		if self.anchorId == 'view17':
			return {
				'A': [ 'v0', 'v7', 'v8', 'v9', 'v10', 'v11', 'v12', 'v13', 'v14' ],
				'B': [ 'v0', 'v1', 'v4', 'v8', 'v11', 'v12', 'v13', 'v17' ],
				'C': [ 'v1', 'v4', 'v5', 'v8', 'v9' ],
				'D': [ 'v0', 'v3', 'v5', 'v6', 'v9', 'v10', 'v12', 'v15' ],
				'E': [ 'v1', 'v3', 'v5', 'v7', 'v9', 'v11', 'v13' ],
				'J': [ 'v00', 'v02', 'v04', 'v10', 'v12', 'v14', 'v20', 'v22', 'v24' ],
				'L': [ 'v00', 'v02', 'v04', 'v06', 'v08' ]
			}[self.seqId]
		return list(map(lambda x: self.viewNameFormat().format(x + self.firstSourceView()), range(0, self.numberOfSourceViews())))

	def firstSourceCameraName(self):
		return self.viewNameFormat().format(self.firstSourceView())

	def numberOfCodedSourceViews(self):
		len(self.sourceCameraNames())

	def useMultipassRenderer(self):
		return self.anchorId == 'view17'

	def rendererMethod(self):
		if self.useMultipassRenderer():
			return 'MultipassRenderer'
		return 'Renderer'

	def renderer(self):
		config = {
			'SynthesizerMethod': 'Synthesizer',
			'Synthesizer': self.synthesizer(),
			'InpainterMethod': 'Inpainter',
			'Inpainter': {}
		}
		if self.useMultipassRenderer():
			config.update({
				'NumberOfPasses': 3,
				'NumberOfViewsPerPass': [2, 4, self.numberOfCodedSourceViews()]
			})
		return config

	def parameters(self):
		config = {
			'numberOfFrames': self.numberOfFrames(),
			'intraPeriod': self.intraPeriod(),
			'SourceDirectory': self.sourceDirectory(),
			'SourceCameraParameters': self.sourceCameraParameters(),
			'AtlasTexturePathFmt': self.atlasTexturePathFmt(),
			'AtlasDepthPathFmt': self.atlasDepthPathFmt(),
			'AtlasMetadataPath': self.atlasMetadataPath(),
			'OutputDirectory': self.outputDirectory(),
			'OutputTexturePath': self.outputTexturePath(),
			'OutputCameraName': self.outputCameraName(),
			'DecoderMethod': 'Decoder',
			'Decoder': {
				'AtlasDeconstructorMethod': 'AtlasDeconstructor',
				'AtlasDeconstructor': {},
				'RendererMethod': self.rendererMethod(),
				self.rendererMethod(): self.renderer()
			}
		}
		return config

	def path(self):
		return '{}/TMIV_{}_{}_{}.json'.format(self.testPoint, self.anchorId, self.seqId, self.testPoint)

class EncoderConfiguration(DecoderConfiguration):
	def __init__(self, anchorId, seqId):
		DecoderConfiguration.__init__(self, anchorId, seqId, 'R0')

	def startFrame(self):
		# TODO(BK): Decide on startFrame for 17-frame anchors
		return {
			'A': 23,
			'B': 100,
			'C': 0,
			'D': 10,
			'E': 135,
			'J': 0,
			'L': 30
		}[self.seqId]		

	def viewOptimizerMethod(self):
		if anchorId == 'view17':
			return 'NoViewOptimizer'
		return 'ViewReducer'

	def sourceTexturePathFmt(self):
		return '%s_texture_{}x{}_yuv420p10le.yuv'.format(self.viewWidth(), self.viewHeight())

	def sourceDepthBitDepth(self):
		return self.firstSourceCamera()['BitDepthDepth']

	def sourceDepthVideoFormat(self):
		return {
			8: 'yuv420p',
			10: 'yuv420p10le',
			16: 'yuv420p16le'
		}[self.sourceDepthBitDepth()]

	def sourceDepthPathFmt(self):
		return '%s_depth_{}x{}_{}.yuv'.format(self.viewWidth(), self.viewHeight(), self.sourceDepthVideoFormat())

	def omafV1CompatibleFlag(self):
		# Just to do something slightly more interesting then False
		return self.seqId == 'A' or self.seqId == 'C'

	def pruner(self):
		config = self.synthesizer()
		config.update({
			'maxDepthError': 0.1,
			'erode': 2,
			'dilate': 5
		})
		return config

	def packer(self):
		return {
			'Alignment': 8,
			'MinPatchSize': 16,
			'Overlap': 1,
			'PiP': 1
		}

	def lumaSamplesPerView(self):
		return 2 * self.atlasWidth() * self.atlasHeight()

	def maxLumaSamplesPerFrame(self):
		if self.anchorId == 'anchor97' or self.anchorId == 'anchor17':
			return {
				'A': 1,
				'B': 3,
				'C': 2,
				'D': 4,
				'E': 3,
				'J': 4,
				'L': 3
			}[self.seqId] * self.lumaSamplesPerView()
		return 0

	def atlasConstructor(self):
		return {
			'PrunerMethod': 'Pruner',
			'Pruner': self.pruner(),
			'AggregatorMethod': 'Aggregator',
			'Aggregator': {},
			'PackerMethod': 'Packer',
			'Packer': self.packer(),
			'AtlasResolution': [
				self.atlasWidth(),
				self.atlasHeight()
			],
			'MaxLumaSamplesPerFrame': self.maxLumaSamplesPerFrame()
		}

	def encoder(self):		
		return {
			'ViewOptimizerMethod': self.viewOptimizerMethod(),
			self.viewOptimizerMethod(): {},
			'AtlasConstructorMethod': 'AtlasConstructor',
			'AtlasConstructor': self.atlasConstructor()
		}

	def parameters(self):
		# The encoder configuration includes the decoder configuration for testing.
		# Enabling reconstruction will run the decoder while encoding.
		config = DecoderConfiguration.parameters(self)
		config['reconstruct'] = False

		config['startFrame'] = self.startFrame()
		config['SourceTexturePathFmt'] = self.sourceTexturePathFmt()
		config['SourceDepthPathFmt'] = self.sourceDepthPathFmt()
		config['SourceDepthBitDepth'] = self.sourceDepthBitDepth()
		config['SourceCameraNames'] = self.sourceCameraNames()
		config['OmafV1CompatibleFlag'] = self.omafV1CompatibleFlag()
		config['EncoderMethod'] = 'Encoder'
		config['Encoder'] = self.encoder()
		return config
		
	def path(self):
		return 'TMIV_{}_{}.json'.format(self.anchorId, self.seqId)

def saveJson(data, path):	
	with open(path, 'w') as stream:
		json.dump(data, stream, indent=4, sort_keys=True, separators=(',', ': '))

if __name__ == '__main__':
	if sys.version_info[0] < 3:
		print ('Error: Python version < 3')
		exit(-1)

	anchors = [ 'anchor97', 'anchor17', 'view17' ]
	seqIds = ['A', 'B', 'C', 'D', 'E', 'J', 'L']
	testPoints = ['R0', 'QP1', 'QP2', 'QP3', 'QP4', 'QP5']

	for anchorId in anchors:
		for seqId in seqIds:
			config = EncoderConfiguration(anchorId, seqId)
			print(config.path())
			saveJson(config.parameters(), config.path())

	for testPoint in testPoints:
		if not os.path.exists(testPoint):
			os.makedirs(testPoint)
		for anchorId in anchors:
			for seqId in seqIds:
				config = DecoderConfiguration(anchorId, seqId, testPoint)
				print(config.path())
				saveJson(config.parameters(), config.path())
