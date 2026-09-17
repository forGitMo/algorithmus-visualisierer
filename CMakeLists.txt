cmake_minimum_required(VERSION 3.20)

project(
        SDL3Demo
        VERSION 1.0.0
        DESCRIPTION "SDL3 demo project for advanced C++"
        LANGUAGES CXX
)

# ------------------------------------------------------------
# Optionen
# ------------------------------------------------------------

option(SDL3DEMO_ENABLE_WARNINGS "Enable compiler warnings" ON)
option(SDL3DEMO_WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)
option(SDL3DEMO_ENABLE_UNITY_BUILD "Enable unity build for faster compilation" OFF)

# ------------------------------------------------------------
# Gemeinsame Algorithmus-Quellen (kein SDL3 erforderlich)
# ------------------------------------------------------------

set(ALGO_SOURCES
        src/SortingEvent.h
        src/SortingStats.h
        src/AlgorithmInfo.h
        src/algorithmen/BubbleSort.h
        src/algorithmen/BubbleSort.cpp
        src/algorithmen/SelectionSort.h
        src/algorithmen/SelectionSort.cpp
        src/algorithmen/HeapSort.cpp
        src/algorithmen/HeapSort.h
        src/algorithmen/InsertionSort.h
        src/algorithmen/InsertionSort.cpp
        src/algorithmen/RadixSort.h
        src/algorithmen/RadixSort.cpp
        src/algorithmen/BucketSort.h
        src/algorithmen/BucketSort.cpp
)

# ------------------------------------------------------------
# Executable 1: SDL3Demo (Visualisierung)
# ------------------------------------------------------------

add_executable(SDL3Demo
        src/Renderer.cpp
        src/Renderer.h
        src/SoundEngine.cpp
        src/SoundEngine.h
        ${ALGO_SOURCES}
)

target_sources(SDL3Demo
        PRIVATE
        src/main.cpp
)

# ------------------------------------------------------------
# Executable 2: AlgoShell (reine Shell, kein SDL3)
# ------------------------------------------------------------

add_executable(AlgoShell
        ${ALGO_SOURCES}
        src/main_shell.cpp
)

# ------------------------------------------------------------
# C++ Standard
# ------------------------------------------------------------

target_compile_features(SDL3Demo
        PRIVATE
        cxx_std_17
)

target_compile_features(AlgoShell
        PRIVATE
        cxx_std_17
)

set_target_properties(SDL3Demo PROPERTIES
        CXX_EXTENSIONS OFF
        UNITY_BUILD ${SDL3DEMO_ENABLE_UNITY_BUILD}
)

set_target_properties(AlgoShell PROPERTIES
        CXX_EXTENSIONS OFF
)

# ------------------------------------------------------------
# Include-Verzeichnisse
# ------------------------------------------------------------

target_include_directories(SDL3Demo
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_include_directories(AlgoShell
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# ------------------------------------------------------------
# SDL3 & SDL3_ttf
# ------------------------------------------------------------

find_package(SDL3 REQUIRED CONFIG)
find_package(SDL3_ttf REQUIRED CONFIG)

target_link_libraries(SDL3Demo
        PRIVATE
        SDL3::SDL3
        SDL3_ttf::SDL3_ttf
)

# ------------------------------------------------------------
# Compiler-Warnungen
# ------------------------------------------------------------

if(SDL3DEMO_ENABLE_WARNINGS)
    if(MSVC)
        target_compile_options(SDL3Demo PRIVATE /W4)
        target_compile_options(AlgoShell PRIVATE /W4)

        if(SDL3DEMO_WARNINGS_AS_ERRORS)
            target_compile_options(SDL3Demo PRIVATE /WX)
            target_compile_options(AlgoShell PRIVATE /WX)
        endif()
    else()
        target_compile_options(SDL3Demo
                PRIVATE
                -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion
        )
        target_compile_options(AlgoShell
                PRIVATE
                -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion
        )

        if(SDL3DEMO_WARNINGS_AS_ERRORS)
            target_compile_options(SDL3Demo  PRIVATE -Werror)
            target_compile_options(AlgoShell PRIVATE -Werror)
        endif()
    endif()
endif()

# ------------------------------------------------------------
# Debug-Hilfen
# ------------------------------------------------------------

target_compile_definitions(SDL3Demo
        PRIVATE
        $<$<CONFIG:Debug>:SDL3DEMO_DEBUG>
)

target_compile_definitions(AlgoShell
        PRIVATE
        $<$<CONFIG:Debug>:SDL3DEMO_DEBUG>
)