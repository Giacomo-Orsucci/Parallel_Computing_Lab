//
// Created by giacomo on 02/02/26.
//

#include "../headers/game.h"
#include <omp.h>

//to achieve better performance, I've separated the count on boundary from the count
//in the "safe" zone, the zone with at least the radius size from the border.
//In this way it's possible to optimize saving on if branches.

//I choose this structure because thanks to some preliminary tests have been shown to be more rapid


//branches and checks, but only when necessary
inline int count_neighbors_border(const std::vector<unsigned char>& grid, int r, int c, int radius, int ROWS, int COLS) {
    int count = 0;

    //to scan in a block with dimension SCAN_SIZExSCAN_SIZE around the position
    //for classic game of life, radius=3
    for (int i = -radius; i <= radius; i++) {
        for (int j = -radius; j <= radius; j++) {
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

//no branches, but it's safe because far from the border
inline int count_neighbors_noBorder(const std::vector<unsigned char>& grid, int r, int c, int radius, int COLS) {
    int count = 0;
    int center_idx = r * COLS + c;

    for (int i = -radius; i <= radius; i++) {
        for (int j = -radius; j <= radius; j++) {
            count += grid[center_idx + (i * COLS) + j];
        }
    }
    count -= grid[center_idx];
    return count;
}

//also this method is divided in a part checking far form the border and one that
//does only the necessary checks on borders.
void update_grid(const std::vector<unsigned char>& current, std::vector<unsigned char>& next, const int ROWS, const int COLS, const int SCAN_SIZE) {
    int radius = SCAN_SIZE / 2;

    #pragma omp parallel for collapse(2) default(none) shared(current, next, ROWS, COLS, radius) schedule(static)
    for (int i = radius; i < ROWS - radius; ++i) {
        for (int j = radius; j < COLS - radius; ++j) {

            int neighbors = count_neighbors_noBorder(current, i, j, radius, COLS);
            int idx = i * COLS + j;
            unsigned char is_alive = current[idx];

            // branchless logic
            next[idx] = (neighbors == 3) | (is_alive & (neighbors == 2));
        }
    }

    //with this separation we have many branches below, but the performance are better because
    //the cell checked are very few and the optimization above compensates

    #pragma omp parallel for default(none) shared(current, next, ROWS, COLS, radius) schedule(static)
    for (int i = 0; i < ROWS; ++i) {

        if (i >= radius && i < ROWS - radius) {

            for (int j = 0; j < radius; ++j) {
                int neighbors = count_neighbors_border(current, i, j, radius, ROWS, COLS);
                int idx = i * COLS + j;
                next[idx] = (neighbors == 3) | (current[idx] & (neighbors == 2));
            }

            for (int j = COLS - radius; j < COLS; ++j) {
                int neighbors = count_neighbors_border(current, i, j, radius, ROWS, COLS);
                int idx = i * COLS + j;
                next[idx] = (neighbors == 3) | (current[idx] & (neighbors == 2));
            }
        } else {

            for (int j = 0; j < COLS; ++j) {
                int neighbors = count_neighbors_border(current, i, j, radius, ROWS, COLS);
                int idx = i * COLS + j;
                next[idx] = (neighbors == 3) | (current[idx] & (neighbors == 2));
            }
        }
    }
}
