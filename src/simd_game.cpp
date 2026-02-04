//
// Created by giacomo on 02/02/26.
//


#include "../headers/game.h"
#include <omp.h>

//Implementation logic similar to the one in parallel_game.cpp but with "fine-grained" optimization
//in update_grids to achieve the best performance possible


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

//in this version the update is more decomposed to achieve better performance and is written to be more optimized as possible
//Attention! this method is optimized for the classic Game of Life, so radius=1 (SCAN_SIZE=3).
void update_grid(const std::vector<unsigned char>& current, std::vector<unsigned char>& next, const int ROWS, const int COLS, const int SCAN_SIZE) {

    int radius = SCAN_SIZE / 2;


    // Raw pointers, in theory better for compile optimization
    const unsigned char* in_ptr = current.data();
    unsigned char* out_ptr = next.data();

    // Far from border, so no checks
    #pragma omp parallel for default(none) shared(in_ptr, out_ptr, ROWS, COLS, radius, SCAN_SIZE) schedule(static)
    for (int i = radius; i < ROWS - radius; ++i) {

        // The idea is that is very computationally inefficient to calculate the position for every cell.
        // So, here are precomputed the 3 rows
        const unsigned char* row_up  = &in_ptr[(i - 1) * COLS];
        const unsigned char* row_mid = &in_ptr[i * COLS];
        const unsigned char* row_down = &in_ptr[(i + 1) * COLS];
        unsigned char* row_out = &out_ptr[i * COLS];

        //the optimization core
        if (SCAN_SIZE == 3) {
            #pragma omp simd
            for (int j = 1; j < COLS - 1; ++j) {
                //here the idea is to count the neighbors "manually" to use very well SIMD instructions.
                int count = row_up[j-1]    + row_up[j]   + row_up[j+1] +
                            row_mid[j-1]+                   row_mid[j+1] +
                            row_down[j-1] + row_down[j] + row_down[j+1];

                unsigned char is_alive = row_mid[j];

                //branchless logic
                row_out[j] = (count == 3) | (is_alive & (count == 2));
            }
        }
        else { //version not optimized, but functioning for radius != 1. Is a more general version.

            for (int j = radius; j < COLS - radius; ++j) {

                int count = 0;
                int center_idx = i * COLS + j;

                for (int dy = -radius; dy <= radius; ++dy) {
                    for (int dx = -radius; dx <= radius; ++dx) {
                        count += in_ptr[center_idx + (dy * COLS) + dx];
                    }
                }
                count -= in_ptr[center_idx];
                unsigned char is_alive = in_ptr[center_idx];
                out_ptr[center_idx] = (count == 3) | (is_alive & (count == 2));
            }
        }
    }

    // Border checks, divided in Top & Bottom and Left & Right (still connected to toroidal world)

    // (Top & Bottom)
    #pragma omp parallel for default(none) shared(current, next, ROWS, COLS, radius) schedule(static)
    for (int i = 0; i < radius; ++i) { // Top rows
        for (int j = 0; j < COLS; ++j) {
            int count = count_neighbors_border(current, i, j, radius, ROWS, COLS);
            next[i*COLS + j] = (count == 3) | (current[i*COLS + j] & (count == 2));
        }
    }
    #pragma omp parallel for default(none) shared(current, next, ROWS, COLS, radius) schedule(static)
    for (int i = ROWS - radius; i < ROWS; ++i) { // Bottom rows
        for (int j = 0; j < COLS; ++j) {
            int count = count_neighbors_border(current, i, j, radius, ROWS, COLS);
            next[i*COLS + j] = (count == 3) | (current[i*COLS + j] & (count == 2));
        }
    }

    // (Left & Right)
    #pragma omp parallel for default(none) shared(current, next, ROWS, COLS, radius) schedule(static)
    for (int i = radius; i < ROWS - radius; ++i) {
        // Left
        for (int j = 0; j < radius; ++j) {
            int count = count_neighbors_border(current, i, j, radius, ROWS, COLS);
            next[i*COLS + j] = (count == 3) | (current[i*COLS + j] & (count == 2));
        }
        // Right
        for (int j = COLS - radius; j < COLS; ++j) {
            int count = count_neighbors_border(current, i, j, radius, ROWS, COLS);
            next[i*COLS + j] = (count == 3) | (current[i*COLS + j] & (count == 2));
        }
    }
}