//
// Created by giacomo on 02/02/26.
//

#include "../headers/game.h"
#include <omp.h>

__attribute__((always_inline)) //to force inline inside SIMD loop in update_grid
//implemented with branchless logic to better use SIMD instructions and compiler optimizations
//no simd directives here because, given the small datas, can be inefficient
inline int count_neighbors(const std::vector<int>& grid, int r, int c, const int SCAN_SIZE, const int ROWS, const int COLS) {
    int count = 0;
    int radius = SCAN_SIZE/2;

    //to scan in a block with dimension SCAN_SIZExSCAN_SIZE around the position
    for (int i = -radius; i <= radius; i++) {
        for (int j = -radius; j <= radius; j++) {

            int nr = (r + i + ROWS) % ROWS;
            int nc = (c + j + COLS) % COLS;

            count += grid[nr*COLS +nc];
        }
    }
    //without branch logic I have to manually remove the central value
    count -= grid[r * COLS + c];
    return count;
}


void update_grid(const std::vector<int>& current, std::vector<int>& next, const int ROWS, const int COLS, const int SCAN_SIZE, const int THREADS) {

    //this is the core of the simulation because it implements the main rules:
    //1. A live cell with fewer than two live neighbors, dies (underpopulation).
    //2. A live cell with two or three live neighbors stays alive.
    //3. A live cell with more than three live neighbors dies (overpopulation).
    //4. A dead cell with exactly three live neighbors becomes alive (reproduction).
    omp_set_num_threads(THREADS);
#pragma omp parallel for default(none) shared(current, next, ROWS, COLS, SCAN_SIZE)
    for (int i = 0; i < ROWS; ++i) {

#pragma omp simd
        for (int j = 0; j < COLS; ++j) {

            int neighbors = count_neighbors(current, i, j, SCAN_SIZE, ROWS, COLS);
            int idx = get_idx(i, j, COLS);
            int is_alive = current[idx];

            //branchless logic
            int birth_survive = (neighbors==3);

            int survive = is_alive & (neighbors==2);

            next[idx] = birth_survive | survive;

        }
    }
}
