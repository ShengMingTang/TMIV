cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

create_tmiv_library(
    TARGET
        PackerLib
    SOURCES
        "src/Cluster.cpp"
        "src/MaxRectPiP.cpp"
        "src/Packer.cpp"
        "src/Retriever.cpp"
    PUBLIC
        MivBitstreamLib
    )

create_catch2_unit_test(
    TARGET
        PackerTest
    SOURCES
        "src/Cluster.test.cpp"
        "src/MaxRectPiP.test.cpp"
        "src/Retriever.test.cpp"
    PRIVATE
        PackerLib
)
