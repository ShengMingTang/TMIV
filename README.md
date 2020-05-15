# Test Model for Immersive Video

## Introduction

This repository includes the current reference software of the MPEG _Immersive
Video_ project that is part of the future MPEG-I standard (ISO/IEC 23090) 
dedicated to immersive media technology. This standard part 12 will enable 
visual media consumption with large or very large field of view – along the 3 
degrees of viewer head’s orientation yaw, pitch and roll - and with a sense of 
parallax within a small head displacement volume along x, y and z axis for 
improved viewer visual comfort. The related use case is therefore also known as 
3DoF+. Such a 3DoF+ bitstream is based on legacy HEVC coding technology preceded
by a preprocessing whose decoding requires a small number of metadata specified 
in the current draft _Metadata for Immersive Video (MIV) specification_ document.

This related _Test Model for Immersive Video_ (TMIV) gets as input a 
multiplicity of texture + depth videos - as it would be generated by a camera 
rig and a depth computation workflow -, sets up the whole chain of
preprocessing, HM video encoding, HM video decoding and post-processing, and 
outputs images according to viewport configuration. It can be used to compute 
objective metrics as defined by the latest version of the _Common Test 
Conditions for Immersive Video_ document and can also be used to generate videos 
along predefined navigation path for subjective evaluations. 

The list of parameters for the TMIV configuration and the used algorithms in 
each TMIV block are detailed and explained in the current version of the _Test 
Model for Immersive Video_ document.

All mentioned documents are or will be publically available on
https://mpeg.chiariglione.org/ and on http://wg11.sc29.org/ for MPEG members.

## Build and installation instructions

The software is ISO C++17 conformant and does not require external libraries. 
The optional dependencies are however highly recommended:

 *	Catch2 test framework
 *	HEVC test model (HM)
	
The following steps collect the software projects in a main working directory,
arbitrary called /Workspace in this description. Other directory names are also
not more than examples.

### Prerequisites to follow this instruction

Prerequisites are:

  * C++17 compiler and build tools
  * CMake 3.10 or newer
  * Git and SVN command tools

This description works for Windows and Linux. 

### Instructions to build and install Catch2

Catch2 is optional but highly recommended. Clone a recent version of
[Catch2](https://github.com/catchorg/Catch2.git) to enable the tests:

    cd /Workspace
    git clone https://github.com/catchorg/Catch2.git
    cd Catch2
    git checkout v2.11.1

Open the CMake GUI and specify:

  * Where is the source directory: /Workspace/Catch2
  * Where to build the binaries: /Workspace/Catch2/build
  * Click Configure, Yes, Finish
  * Set CMAKE_INSTALL_PREFIX to /Workspace/Catch2-2.11.1
  * Click Generate

Build and install the generated project.

### Instructions to download HM

Look in the CTC document for the version of HM and corresponding URL. This
description uses 16.16.

    cd /Workspace
    svn co https://hevc.hhi.fraunhofer.de/svn/svn_HEVCSoftware/tags/HM-16.16

TMIV includes a build script for HM.

### Instructions to build and install TMIV

To obtain the branch to this document:

    cd /Workspace
    git clone http://mpegx.int-evry.fr/software/MPEG/MIV/RS/TM1.git
    cd TM1
    git checkout master
    
To obtain the latest release of TMIV instead:

    cd /Workspace
    git clone https://gitlab.com/mpeg-i-visual/tmiv.git
    
Open the CMake GUI and specify:

  * Where is the source directory: /Workspace/TM1
  * Where to build the binaries: /Workspace/TM1/build
  * Click Configure, Yes, Finish
  * Set CMAKE_INSTALL_PREFIX to /Workspace/TM1-master
  * Set Catch2_DIR to /Workspace/Catch2-v2.11.1/lib/cmake/Catch2
  * Set HM_SOURCE_DIR to /Workspace/HM-16.16
  * Click Generate

Build and install the generated project. 

After this the TMIV executables Encoder and Decoder will be available under the
directory /Workspace/TM1-master/bin. By default TMIV only builds the HM modules
that are required for TMIV (TLibCommon and TLibDecoder). When
HM_BUILD_TAPPDECODER and HM_BUILD¬_TAPPENCODER are selected, then the
TAppDecoder and TAppEncoder tools respectively will also be installed to this
directory. 

## Instructions to run TMIV

Template configuration files are available under ctc_config/ and test_configs/. The file names of
and in template configuration files are examples.

  * *best_reference* uses all source views without coding to achieve the best possible result
  * *miv_anchor* is the MIV anchor with patches
  * *miv_view_anchor* is the MIV view anchor which codes a subset of views completely
  * *entity_based_coding_reference* is the (non-CTC) reference condition for entity-based coding
  * *entity_based_coding_test* is for integration testing of entity-based coding

Use the following steps to encode a bistream and render a viewport:

 1. Run TMIV encoder
 2. Run HM encoder on all video sub bitstreams
 3. Run TMIV multiplexer to form the output bitstream
 4. Run TMIV decoder to decode the bitstream and render a viewport

Use the following steps for uncoded video (i.e. best_reference):

 1. Run TMIV encoder
 4. Run TMIV decoder to render a viewport

## Structure of the test model

This software consists of multiple executables and static libraries. Below
figure is the CMake module dependency graph of TMIV 5.0. The most important
executables are Encoder and Decoder. When enabled, the project also includes the
HM executables TAppEncoder and TAppDecoder.

![CMake module graph](doc/module_graph.png)

Each module consists of one or more components. Most components derive from an
interface and some interfaces have multiple alternative components. For instance,
the ViewOptimizerLib includes an IViewOptimizer interface with NoViewOptimizer
and ViewReducer components that implement that interface.

## Improving the test model

Core experiments are expected to include the reference software as a subproject
and introduce new components. Alternatively core experiments may branch the test 
model. Contributions should be in the form of git pull requests to the
MPEG-internal repository.
