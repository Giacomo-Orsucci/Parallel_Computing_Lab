//
// Created by giacomo on 01/02/26.
//

#ifndef PARALLEL_COMPUTING_LAB_SEQ_HEADER_H
#define PARALLEL_COMPUTING_LAB_SEQ_HEADER_H

#include <cstdlib>
#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <string>
#include <bits/stdc++.h>



struct Config {

    // this is the parameter to define the cell dimension. The cell dimension is equal
    // to CELL_SIZExCELL_SIZE. If CELL_SIZE is set to 1, the cell is graphically
    // invisible.

    int CELL_SIZE = 2;

    int SCREEN_WIDTH = 200;
    int SCREEN_HEIGHT = 200;
    int SCAN_SIZE = 3;
    int FRAMES = 1000;
    int THREADS = 1;

    bool GUI = false;

    std::string CSV;


    //Parsing params passed via command line
    void parse(int argc, char* argv[]) {
        for (int i = 1; i < argc; ++i) { //i = 1 because for i=0 we always have the exe path
            std::string arg = argv[i];
            if (arg == "--width" && i + 1 < argc) {
                SCREEN_WIDTH = std::stoi(argv[++i]);
            } else if (arg == "--height" && i + 1 < argc) {
                SCREEN_HEIGHT = std::stoi(argv[++i]);
            } else if (arg == "--frames" && i + 1 < argc) {
                FRAMES = std::stoi(argv[++i]);
            }else if (arg == "--threads" && i + 1 < argc) {
                THREADS = std::stoi(argv[++i]);
            }else if (arg == "--csv" && i + 1 < argc) {
                CSV = argv[++i];
            }else if (arg == "--cell_size" && i + 1 < argc) {
                CELL_SIZE = std::stoi(argv[++i]);
            }else if (arg == "--scan_size" && i + 1 < argc) {
                SCAN_SIZE = std::stoi(argv[++i]);
            }else if (arg == "--gui" && i + 1 < argc) {
                std::string GUI_app = argv[++i];
                if (!GUI_app.compare("True"))
                    GUI = true;
                else
                    GUI = false;
                std::cerr << "GUI: " << GUI << std::endl;
            }
            else {
                std::cerr << "Unknown argument: " << arg << std::endl;
            }
        }
    }
    int COLS = SCREEN_WIDTH / CELL_SIZE;
    int ROWS = SCREEN_HEIGHT / CELL_SIZE;
};

inline int get_idx(int row, int col, const int COLS) {
    return row * COLS + col;
}

void init_grid(std::vector<int>& grid, const int ROWS, const int COLS) {

    for (int i = 0; i < ROWS * COLS; ++i) {
        //to have a 10% possibility of populated cell
        if (std::rand() % 10 > 8)
            grid[i] = 1;
        else grid[i] = 0;
    }
}

inline int count_neighbors(const std::vector<int>& grid, int r, int c, const int SCAN_SIZE, const int ROWS, const int COLS) {
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

void update_grid(const std::vector<int>& current, std::vector<int>& next, const int ROWS, const int COLS, const int SCAN_SIZE) {

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

void append_csv(const std::string& CSV, int SCREEN_WIDTH, int SCREEN_HEIGHT, int ROWS, int COLS, int SCAN_SIZE, int FRAMES, int THREADS, const double TIME_MS)
{
    static bool first = true;
    std::ofstream out(CSV, std::ios::app);

    if (first) {
        out << "SCREEN_WIDTH, SCREEN_HEIGHT, ROWS, COLS, SCAN_SIZE, FRAMES, THREADS, time_ms\n";
        first = false;
    }

    out << SCREEN_WIDTH << ","
        << SCREEN_HEIGHT << ","
        << ROWS << ","
        << COLS << ","
        << SCAN_SIZE << ","
        << FRAMES << ","
        << THREADS << ","
        << TIME_MS << "\n";
}

#endif //PARALLEL_COMPUTING_LAB_SEQ_HEADER_H