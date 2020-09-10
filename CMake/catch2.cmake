cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

option(BUILD_CATCH2 "Build and use Catch2 for unit tests in TMIV" ON)

if(BUILD_CATCH2)
    include(FetchContent)
    fetchcontent_declare(CATCH2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG "v2.11.1"
    )
    fetchcontent_makeavailable(CATCH2)

    # Make catch2 cmake scripts available
    set(CMAKE_MODULE_PATH
        ${CMAKE_MODULE_PATH} "${CMAKE_BINARY_DIR}/_deps/catch2-src/contrib/")

    # Make catch2 header file available
    include_directories(${CMAKE_BINARY_DIR}/_deps/catch2-src/single_include/)
endif()
