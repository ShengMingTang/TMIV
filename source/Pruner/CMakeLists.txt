cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

create_tmiv_library(
    TARGET
        PrunerLib
    SOURCES
        "src/HierarchicalPruner.cpp"
        "src/LumaStdDev.cpp"
        "src/PrunedMesh.cpp"
        "src/NoPruner.cpp"
    PRIVATE
        RendererLib
        fmt::fmt
    )

create_catch2_unit_test(
    TARGET
        PrunerTest
    SOURCES
        "src/LumaStdDev.test.cpp"
        "src/HierarchicalPruner.test.cpp"
    PRIVATE
        PrunerLib
        RendererLib
    )
