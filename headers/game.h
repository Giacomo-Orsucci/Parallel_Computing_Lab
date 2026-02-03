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
#include <iostream>
#include <fstream>



struct Config {

    // this is the parameter to define the cell dimension. The cell dimension is equal
    // to CELL_SIZExCELL_SIZE. If CELL_SIZE is set to 1, the cell is graphically
    // invisible. So, for a graphical execution is suggested a CELL_SIZE = 2 at least.

    int CELL_SIZE = 1;

    int SCREEN_WIDTH = 200;
    int SCREEN_HEIGHT = 200;
    int ROWS = SCREEN_HEIGHT/CELL_SIZE;
    int COLS = SCREEN_WIDTH/CELL_SIZE;
    int SCAN_SIZE = 3;
    int FRAMES = 1000;
    int THREADS = 1;

    bool GUI = true;

    std::string CSV;

    bool first = true;


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
        COLS = SCREEN_WIDTH / CELL_SIZE;
        ROWS = SCREEN_HEIGHT / CELL_SIZE;
    }

};

inline int get_idx(int row, int col, const int COLS) {
    return row * COLS + col;
}

inline void init_grid(std::vector<unsigned char>& grid, const int ROWS, const int COLS) {

    for (int i = 0; i < ROWS * COLS; ++i) {
        //to have a 10% possibility of populated cell
        if (std::rand() % 10 > 8)
            grid[i] = 1;
        else grid[i] = 0;
    }
}

int count_neighbors(const std::vector<unsigned char>& grid, int r, int c, const int SCAN_SIZE, const int ROWS, const int COLS);

void update_grid(const std::vector<unsigned char>& current, std::vector<unsigned char>& next, const int ROWS, const int COLS, const int SCAN_SIZE, const int THREADS);

inline void append_csv(Config cfg, const double TIME_MS)
{
    auto exist = std::filesystem::exists(cfg.CSV);

    std::ofstream out(cfg.CSV, std::ios::app);

    if (!exist) {
        out << "SCREEN_WIDTH, SCREEN_HEIGHT, ROWS, COLS, SCAN_SIZE, FRAMES, THREADS, time_ms\n";
        cfg.first = false;
    }

    out << cfg.SCREEN_WIDTH << ","
        << cfg.SCREEN_HEIGHT << ","
        << cfg.ROWS << ","
        << cfg.COLS << ","
        << cfg.SCAN_SIZE << ","
        << cfg.FRAMES << ","
        << cfg.THREADS << ","
        << TIME_MS << "\n";
}

#endif //PARALLEL_COMPUTING_LAB_SEQ_HEADER_H