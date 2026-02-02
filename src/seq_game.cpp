//
// Created by giacomo on 02/02/26.
//

#include "../headers/game.h"
#include <vector>

int count_neighbors(const std::vector<int>& grid, int r, int c, const int SCAN_SIZE, const int ROWS, const int COLS) {
    int count = 0;
    int radius = SCAN_SIZE/2;

    //to scan in a block with dimension SCAN_SIZExSCAN_SIZE around the position
    for (int i = -radius; i <= radius; i++) {
        for (int j = -radius; j <= radius; j++) {
            if (i == 0 && j == 0) continue;

            int nr = r + i;
            int nc = c + j;

            //checks on border
            if (nr < 0) nr = ROWS - 1;
            else if (nr >= ROWS) nr = 0;
            if (nc < 0) nc = COLS - 1;
            else if (nc >= COLS) nc = 0;

            count += grid[get_idx(nr, nc, COLS)];
        }
    }
    return count;
}

void update_grid(const std::vector<int>& current, std::vector<int>& next, const int ROWS, const int COLS, const int SCAN_SIZE, const int THREADS) {

    //this is the core of the simulation because it implements the main rules:
    //1. A live cell with fewer than two live neighbors, dies (underpopulation).
    //2. A live cell with two or three live neighbors stays alive.
    //3. A live cell with more than three live neighbors dies (overpopulation).
    //4. A dead cell with exactly three live neighbors becomes alive (reproduction).

    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {

            int neighbors = count_neighbors(current, i, j, SCAN_SIZE, ROWS, COLS);
            int idx = get_idx(i, j, COLS);
            int is_alive = current[idx];

            if (is_alive) {
                if (neighbors == 2 || neighbors == 3) {
                    next[idx] = 1;
                }else {
                    next[idx] = 0;
                }

            } else {
                if (neighbors == 3) {
                    next[idx] = 1; // reproduction
                } else {
                    next[idx] = 0;
                }
            }
        }
    }
}

