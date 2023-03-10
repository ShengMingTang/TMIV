cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

find_or_fetch(
    NAME HM
    VERSION "16.16"
    GIT_URL "https://vcgit.hhi.fraunhofer.de/jct-vc/HM.git"
    GIT_REF "HM-16.16"
    BUILD_FLAG BUILD_HM
)

if(FIND_OR_FETCH_FETCHED)
    fetchcontent_makeavailable(HM)
    set(HM_SOURCE_DIR "${CMAKE_BINARY_DIR}/_deps/hm-src/")

    option(BUILD_TAppDecoder "Build HM exectable TAppDecoder" FALSE)
    option(BUILD_TAppEncoder "Build HM exectable TAppEncoder" FALSE)

    if(BUILD_TAppDecoder OR BUILD_TAppEncoder)
        set(BUILD_TAppCommon ON)
        set(BUILD_TLibVideoIO ON)
    endif()

    set(appSourceDir "${HM_SOURCE_DIR}/source/App")
    set(libSourceDir "${HM_SOURCE_DIR}/source/Lib")

    function(add_hm_executable module)
        if(NOT TARGET ${module})
            file(GLOB cppSourceFiles "${appSourceDir}/${module}/*.cpp")
            file(GLOB cSourceFiles "${appSourceDir}/${module}/*.c")
            file(GLOB headerFiles "${appSourceDir}/${module}/*.h")
            add_executable(${module} ${cppSourceFiles} ${cSourceFiles} ${headerFiles})
            set_property(TARGET ${module} PROPERTY CXX_STANDARD 17) # HM is not valid C++20
            set_property(TARGET ${module} PROPERTY CXX_CLANG_TIDY) # no clang-tidy
            set_property(TARGET ${module} PROPERTY FOLDER "HM executables")
        endif()

        install(TARGETS ${module} EXPORT TMIVTargets RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
    endfunction()

    function(add_hm_library module)
        if(NOT TARGET ${module})
            file(GLOB cppSourceFiles "${libSourceDir}/${module}/*.cpp")
            file(GLOB cSourceFiles "${libSourceDir}/${module}/*.c")
            file(GLOB headerFiles "${libSourceDir}/${module}/*.h")
            add_library(${module} ${cppSourceFiles} ${cSourceFiles} ${headerFiles})
            add_library(HM::${module} ALIAS ${module})
            set_property(TARGET ${module} PROPERTY CXX_STANDARD 17) # HM is not valid C++20
            set_property(TARGET ${module} PROPERTY CXX_CLANG_TIDY) # no clang-tidy
            set_property(TARGET ${module} PROPERTY FOLDER "HM libraries")
        endif()

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
    target_compile_options(TLibCommon PUBLIC "$<$<CXX_COMPILER_ID:MSVC>:/wd4100;/wd4189;/wd4127;/wd4244;/wd4389;/wd4459;/wd4701;/wd4702>")

    add_hm_library(TLibDecoder)
    target_link_libraries(TLibDecoder PUBLIC TLibCommon)

    if(BUILD_TAppCommon)
        add_hm_library(TAppCommon)
        target_link_libraries(TAppCommon PUBLIC TLibCommon)
    endif()

    if(BUILD_TLibVideoIO)
        add_hm_library(TLibVideoIO)
        target_link_libraries(TLibVideoIO PUBLIC TLibCommon)
    endif()

    if(BUILD_TAppDecoder)
        add_hm_executable(TAppDecoder)
        target_link_libraries(TAppDecoder PRIVATE TLibDecoder TAppCommon TLibVideoIO)
    endif()

    if(BUILD_TAppEncoder)
        add_hm_library(TLibEncoder)
        target_link_libraries(TLibEncoder PUBLIC TLibCommon)
        target_compile_options(TLibEncoder PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/wd4267>")

        add_hm_executable(TAppEncoder)
        target_link_libraries(TAppEncoder PRIVATE TLibEncoder TAppCommon TLibVideoIO)
    
        FetchContent_GetProperties(HM)
        install(
            DIRECTORY "${hm_SOURCE_DIR}/cfg/"
            DESTINATION "${CMAKE_INSTALL_DATADIR}/config/hm")    
        install(
            FILES
                "${hm_SOURCE_DIR}/README"
                "${hm_SOURCE_DIR}/COPYING"
                "${hm_SOURCE_DIR}/doc/software-manual.pdf"
            DESTINATION "${CMAKE_INSTALL_DATADIR}/doc/HM")
    endif()
endif()

if(FIND_OR_FETCH_HAVE)
    set(HAVE_HM ON)
endif()
