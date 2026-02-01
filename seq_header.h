//
// Created by giacomo on 01/02/26.
//

#ifndef PARALLEL_COMPUTING_LAB_SEQ_HEADER_H
#define PARALLEL_COMPUTING_LAB_SEQ_HEADER_H

#include <cstdlib>
#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>


const int CELL_SIZE = 5;
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int SCAN_SIZE = 3;


const int COLS = SCREEN_WIDTH / CELL_SIZE;
const int ROWS = SCREEN_HEIGHT / CELL_SIZE;

inline int get_idx(int row, int col) {
    return row * COLS + col;
}

void init_grid(std::vector<int>& grid) {

    for (int i = 0; i < ROWS * COLS; ++i) {
        //to have a 10% possibility of populated cell
        if (std::rand() % 10 > 8)
            grid[i] = 1;
        else grid[i] = 0;
    }
}

inline int count_neighbors(const std::vector<int>& grid, int r, int c) {
    int count = 0;
    int radius = SCAN_SIZE/2;

    //to scan in a block SCAN_SIZExSCAN_SIZE around the position
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

            count += grid[get_idx(nr, nc)];
        }
    }
    return count;
}

void update_grid(const std::vector<int>& current, std::vector<int>& next) {

    //this is the core of the simulation because it implements the main rules:
    //1. A live cell with fewer than two live neighbors, dies (underpopulation).
    //2. A live cell with two or three live neighbors stays alive.
    //3. A live cell with more than three live neighbors dies (overpopulation).
    //4. A dead cell with exactly three live neighbors becomes alive (reproduction).

    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {

            int neighbors = count_neighbors(current, i, j);
            int idx = get_idx(i, j);
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



#endif //PARALLEL_COMPUTING_LAB_SEQ_HEADER_H