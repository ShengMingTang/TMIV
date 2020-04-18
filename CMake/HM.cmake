cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

option(BUILD_HM "Build and use HM in TMIV when available" ON)

if (BUILD_HM)
	get_filename_component(defaultHmSourceDir "${CMAKE_SOURCE_DIR}/../HM-16.16" REALPATH)
	set(HM_SOURCE_DIR ${defaultHmSourceDir} CACHE PATH "Source directory of the HM project")

	if(EXISTS "${HM_SOURCE_DIR}/source/Lib/TLibCommon/TComSlice.h")
		set(HAVE_HM ON)
	else()
		message(WARNING "HM is disabled.")
	endif()
endif()

if (HAVE_HM)
	option(BUILD_TAppDecoder "Build HM exectable TAppDecoder" FALSE)
	option(BUILD_TAppEncoder "Build HM exectable TAppEncoder" FALSE)

	if(BUILD_TAppDecoder OR BUILD_TAppEncoder)
		set(BUILD_TAppCommon ON)
		set(BUILD_TLibVideoIO ON)
	endif()

	set(appSourceDir "${HM_SOURCE_DIR}/source/App")
	set(libSourceDir "${HM_SOURCE_DIR}/source/Lib")

	function(add_hm_executable module)
		file(GLOB cppSourceFiles "${appSourceDir}/${module}/*.cpp")
		file(GLOB cSourceFiles "${appSourceDir}/${module}/*.c")
		file(GLOB headerFiles "${appSourceDir}/${module}/*.h")
		add_executable(${module} ${cppSourceFiles} ${cSourceFiles} ${headerFiles})
		set_property(TARGET ${module} PROPERTY CXX_CLANG_TIDY) # no clang-tidy
		install(TARGETS ${module} EXPORT TMIVTargets RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
	endfunction()

	function(add_hm_library module)
		file(GLOB cppSourceFiles "${libSourceDir}/${module}/*.cpp")
		file(GLOB cSourceFiles "${libSourceDir}/${module}/*.c")
		file(GLOB headerFiles "${libSourceDir}/${module}/*.h")
		add_library(${module} ${cppSourceFiles} ${cSourceFiles} ${headerFiles})
		set_property(TARGET ${module} PROPERTY CXX_CLANG_TIDY) # no clang-tidy
		add_library(TMIV::${module} ALIAS ${module})
		install(TARGETS ${module} EXPORT TMIVTargets ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})	
	endfunction()

	add_hm_library(libmd5)
	target_compile_features(libmd5 PUBLIC cxx_std_11)
	target_include_directories(libmd5 PUBLIC "$<BUILD_INTERFACE:${libSourceDir}>")

	add_hm_library(TLibCommon)
	target_link_libraries(TLibCommon PRIVATE libmd5)
	target_compile_features(TLibCommon PUBLIC cxx_std_11)
	target_include_directories(TLibCommon PUBLIC "$<BUILD_INTERFACE:${libSourceDir}>")
	target_compile_definitions(TLibCommon PUBLIC "$<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>")
	target_compile_options(TLibCommon PUBLIC "$<$<CXX_COMPILER_ID:Clang>:-w>")
	target_compile_options(TLibCommon PUBLIC "$<$<CXX_COMPILER_ID:GNU>:-w>")

	add_hm_library(TLibDecoder)
	target_link_libraries(TLibDecoder PUBLIC TLibCommon)

	if(HM_BUILD_TAPPCOMMON)
		add_hm_library(TAppCommon)
		target_link_libraries(TAppCommon PUBLIC TLibCommon)
	endif()

	if(HM_BUILD_TLIBVIDEOIO)
		add_hm_library(TLibVideoIO)
		target_link_libraries(TLibVideoIO PUBLIC TLibCommon)
	endif()

	if(HM_BUILD_TAPPDECODER)
		add_hm_executable(TAppDecoder)
		target_link_libraries(TAppDecoder PRIVATE TLibDecoder TAppCommon TLibVideoIO)
	endif()

	if(HM_BUILD_TAPPENCODER)
		add_hm_library(TLibEncoder)
		target_link_libraries(TLibEncoder PUBLIC TLibCommon)
		target_compile_options(TLibEncoder PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/wd4267>")

		add_hm_executable(TAppEncoder)
		target_link_libraries(TAppEncoder PRIVATE TLibEncoder TAppCommon TLibVideoIO)
	endif()
endif()