//
// Created by giacomo on 03/02/26.
//


#include "../headers/game.h"
#include <omp.h>
#include <algorithm>
#include <vector>
#include <omp.h>
#include <algorithm>


//branches and checks, but only when necessary
inline int count_neighbors_border(const std::vector<unsigned char>& grid, int r, int c, int radius, int ROWS, int COLS) {

    int count = 0;

    //to scan in a block with dimension SCAN_SIZExSCAN_SIZE around the position
    //for classic game of life, radius=3
    for (int i = -radius; i <= radius; ++i) {
        for (int j = -radius; j <= radius; ++j) {
            if (i == 0 && j == 0) continue;

            int nr = r + i;
            int nc = c + j;

            //checks on border
            //if the row is less than 0, we have to check the last row. Toroidal world, pac-man effect
            if (nr < 0) nr = ROWS - 1;
            else if (nr >= ROWS) nr = 0;
            if (nc < 0) nc = COLS - 1;
            else if (nc >= COLS) nc = 0;

            count += grid[nr * COLS + nc];
        }
    }
    return count;
}
//this gris still divide in safe zones (far from borders) and border zones, but uses also
//a sort of tiling or blocking to try to decrease cache misses.
void update_grid(const std::vector<unsigned char>& current, std::vector<unsigned char>& next, const int ROWS, const int COLS, const int SCAN_SIZE) {


    const int TILE_SIZE = 128; //dimension hardcoded for my hardware
    int radius = SCAN_SIZE / 2;

    #pragma omp parallel default(none) shared(current, next, ROWS, COLS, SCAN_SIZE, TILE_SIZE, radius)
    {
        // to better distribute blocks on threads
        #pragma omp for collapse(2) schedule(dynamic)
        for (int ii = 0; ii < ROWS; ii += TILE_SIZE) {
            for (int jj = 0; jj < COLS; jj += TILE_SIZE) {

                int i_end = std::min(ii + TILE_SIZE, ROWS);
                int j_end = std::min(jj + TILE_SIZE, COLS);

                //to check if we are on borders
                bool touch_border = (ii == 0) || (jj == 0) || (i_end == ROWS) || (j_end == COLS);

                if (touch_border) {
                    // if we are on borders, we must slow down and checks
                    for (int i = ii; i < i_end; ++i) {
                        for (int j = jj; j < j_end; ++j) {
                            
                            int neighbors = count_neighbors_border(current, i, j, radius, ROWS, COLS);
                            int idx = i * COLS + j;
                            int is_alive = current[idx];
                            next[idx] = (neighbors == 3) | (is_alive & (neighbors == 2));
                        }
                    }
                }
                else {
                    //else we can optimize with the same logic that is in simd_game.cpp

                    for (int i = ii; i < i_end; ++i) {

                       
                        int r_up = (i - 1) * COLS;
                        int r_mid  = i * COLS;
                        int r_down  = (i + 1) * COLS;

                        #pragma omp simd
                        for (int j = jj; j < j_end; ++j) {
                            //costant offset, here the computation is really optimized
                            int neighbors =
                                current[r_up + j - 1] + current[r_up + j] + current[r_up + j + 1] +
                                current[r_mid  + j - 1] +                        current[r_mid  + j + 1] +
                                current[r_down  + j - 1] + current[r_down  + j] + current[r_down  + j + 1];

                            int idx = r_mid + j;
                            int is_alive = current[idx];

                            // branchless logic
                            next[idx] = (neighbors == 3) | (is_alive & (neighbors == 2));
                        }
                    }
                }
            }
        }
    }
}
