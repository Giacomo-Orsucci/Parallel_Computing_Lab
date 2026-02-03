//
// Created by giacomo on 03/02/26.
//


#include "../headers/game.h"
#include <omp.h>
#include <algorithm>

__attribute__((always_inline)) //to force inline inside SIMD loop in update_grid
//implemented with branchless logic to better use SIMD instructions and compiler optimizations
//no simd directives here because, given the small data, can be inefficient
inline int count_neighbors(const std::vector<unsigned char>& grid, int r, int c, const int SCAN_SIZE, const int ROWS, const int COLS) {
    int count = 0;
    int radius = SCAN_SIZE/2;

    //to scan in a block with dimension SCAN_SIZExSCAN_SIZE around the position
    for (int i = -radius; i <= radius; i++) {
        for (int j = -radius; j <= radius; j++) {

            int nr = r + i;
            int nc = c + j;

            //if the row is less than 0, we have to check the last row. Toroidal world, pac-man effect
            nr += ROWS * (nr < 0);
            nr -= ROWS * (nr >= ROWS);

            nc += COLS * (nc < 0);
            nc -= COLS * (nc >= COLS);


            count += grid[nr * COLS + nc];
        }
    }
    //without branch logic I have to manually remove the central value
    count -= grid[r * COLS + c];
    return count;
}

//in this version the idea is to do a sort of tiling to improve cache locality
void update_grid(const std::vector<unsigned char>& current, std::vector<unsigned char>& next, const int ROWS, const int COLS, const int SCAN_SIZE, const int THREADS) {

    //this is the core of the simulation because it implements the main rules:
    //1. A live cell with fewer than two live neighbors, dies (underpopulation).
    //2. A live cell with two or three live neighbors stays alive.
    //3. A live cell with more than three live neighbors dies (overpopulation).
    //4. A dead cell with exactly three live neighbors becomes alive (reproduction).

    const int TILE_SIZE = 32;

#pragma omp parallel default(none) shared(current, next, ROWS, COLS, SCAN_SIZE, TILE_SIZE)

//collapse(2) necessary to parallelize on the blocks
    #pragma omp  for collapse(2) schedule(static)
    for (int ii = 0; ii < ROWS; ii += TILE_SIZE) {
        for (int jj = 0; jj < COLS; jj += TILE_SIZE) {

            //to handle cases without precise multiple of TILE_SIZE
            int i_end = std::min(ii + TILE_SIZE, ROWS);
            int j_end = std::min(jj + TILE_SIZE, COLS);

            for (int i = ii; i < i_end; ++i) {

                #pragma omp simd
                for (int j = jj; j < j_end; ++j) {

                    int neighbors = count_neighbors(current, i, j, SCAN_SIZE, ROWS, COLS);
                    int idx = get_idx(i, j, COLS);

                    int is_alive = current[idx];
                    next[idx] = (neighbors == 3) | (is_alive & (neighbors == 2));
                }
            }
        }
    }

}