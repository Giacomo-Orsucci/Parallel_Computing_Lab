# Game of Life - Parallelization Performance Analysis

This project implements a cellular automaton simulation based on Conway's **Game of Life** in C++, using **SFML** for graphical visualization.

The main goal is to analyze and compare the performance of different optimization techniques and memory access patterns, specifically focusing on:

*   Sequential vs Parallel execution.

*   Parallelization using OpenMP.

*   Vectorization using SIMD (AVX/AVX2) instructions and branchless logic.

*   Cache locality optimization using Tiling (Blocking).

The sequential version is the baseline implementation, utilizing a standard toroidal grid approach.


##  Requirements

*   C++ Compiler (C++20 support)

*   CMake

*   OpenMP Library

*   SFML Library (handled automatically via CMake FetchContent)

*   CPU with AVX/AVX2 support (for SIMD versions)

*   Python 3 (for benchmarking scripts)

## Project Structure

The codebase is divided into several variants to facilitate benchmarking:

*   Seq: Baseline sequential implementation using standard vectors.

*   Parallel: Optimized version using OpenMP. It minimizes conditional checks by separating the "safe" inner grid   computation from the boundary processing.

*   Simd: The strictly optimized version. It utilizes raw pointers and #pragma omp simd to leverage vector instructions, combined with specific branchless logic for the standard scanning radius.

*   Tile: Implementation using Tiling (Blocking). It divides the grid into smaller blocks to fit into the CPU cache, reducing cache misses during the update phase.

Each version is independent, but implemented including game.h. In detail:

*   headers: contains game.h, which defines the Config struct used to pass execution parameters injected via command line or python.

*   src: contains the source files for the different logic implementations (seq_game.cpp, parallel_game.cpp, simd_game.cpp, tiled_game.cpp).

*   root: contains run_benchmark.py to execute the benchmarks and stats_plot.py to analyze the results obtained and written in .csv files.

All the files are highly commented to allow the maximum comprehension and possibility of adaptation.
