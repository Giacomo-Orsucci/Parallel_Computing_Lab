cmake_minimum_required(VERSION 4.1)
project(Game_of_Life)

set(CMAKE_CXX_STANDARD 20)


#To use openmp
if(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Xpreprocessor -fopenmp -lomp")
endif ()

#To use SIMD instructions. Includes -mavx.
add_compile_options("-mavx2")

#From the SFML site for CMake configurations
include(FetchContent)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.0.2
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM)

FetchContent_MakeAvailable(SFML)

#Differentiations depending on CMake profile. Debug includes sanitizers, Benchmark includes all
#optimization options.
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options("-fsanitize=address,leak,undefined")
    add_link_options("-fsanitize=address,leak,undefined")
    add_compile_options("-g")

elseif (CMAKE_BUILD_TYPE STREQUAL "Benchmark")
    add_compile_options("-O3")
    add_compile_options("-mfma")
    add_compile_options("-march=native")
    add_compile_options("-fno-math-errno")
    add_compile_options("-ffast-math")
endif()

add_executable(Seq main.cpp ./headers/game.h ./src/seq_game.cpp)
target_link_libraries(Seq  PRIVATE SFML::Graphics)

add_executable(Parallel main.cpp ./headers/game.h ./src/parallel_game.cpp)
target_link_libraries(Parallel  PRIVATE SFML::Graphics)

add_executable(Simd main.cpp ./headers/game.h ./src/simd_game.cpp)
target_link_libraries(Simd  PRIVATE SFML::Graphics)

add_executable(Tile main.cpp ./headers/game.h ./src/tiled_game.cpp)
target_link_libraries(Tile  PRIVATE SFML::Graphics)
