//
// Created by giacomo on 01/02/26.
//


#include <ctime>
#include <iostream>
#include "headers/game.h"


int main(int argc, char* argv[]) {
    Config cfg;
    cfg.parse(argc, argv);

    const unsigned SCREEN_WIDTH = cfg.SCREEN_WIDTH;
    const unsigned SCREEN_HEIGHT = cfg.SCREEN_HEIGHT;
    const int CELL_SIZE = cfg.CELL_SIZE;
    const int SCAN_SIZE = cfg.SCAN_SIZE;
    const int ROWS = cfg.ROWS;
    const int COLS = cfg.COLS;
    const int FRAMES = cfg.FRAMES;
    const int THREADS = cfg.THREADS;
    const bool GUI = cfg.GUI;


    std::chrono::microseconds total_duration_us = std::chrono::microseconds::zero();
    int iterations = 0;


    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Memory allocation
    std::vector<int> grid(ROWS * COLS);
    std::vector<int> next_grid(ROWS * COLS);

    init_grid(grid, ROWS, COLS);

    //in this case I explicitly excluded graphics during the benchmark process. Given that the measurement is done only
    //on update_grid, it is not necessary but in this way the process is more clean.
    if (GUI == true) {
        // SFML Setup
        sf::RenderWindow window(sf::VideoMode({SCREEN_WIDTH, SCREEN_HEIGHT}), "Game of life");
        window.setFramerateLimit(60);

        sf::RectangleShape cellShape(sf::Vector2f(CELL_SIZE - 1.0f, CELL_SIZE - 1.0f));
        cellShape.setFillColor(sf::Color::White);

        // Game Loop with graphic
        while (window.isOpen() && iterations < FRAMES) {


            window.clear(sf::Color::Black);
            while (const std::optional event = window.pollEvent())
            {

                if (event->is<sf::Event::Closed>())
                    window.close();
            }


            window.clear(sf::Color::Black);


            for (int i = 0; i < ROWS; ++i) {
                for (int j = 0; j < COLS; ++j) {

                    if (grid[get_idx(i, j, COLS)] == 1) {

                        cellShape.setPosition(sf::Vector2f(static_cast<float>(j * CELL_SIZE), static_cast<float>(i * CELL_SIZE)));
                        window.draw(cellShape);
                    }
                }
            }

            // without counting the graphic
            const auto start = std::chrono::high_resolution_clock::now();
            update_grid(grid, next_grid, ROWS, COLS, SCAN_SIZE, THREADS);
            std::swap(grid, next_grid);

            const auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            total_duration_us += duration;

            iterations++;

            window.display();
        }
    }else { //without graphic at all

        while (iterations < FRAMES) {

            // excluding totally the graphic. Pure and only game loop

            const auto start = std::chrono::high_resolution_clock::now();

            update_grid(grid, next_grid, ROWS, COLS, SCAN_SIZE, THREADS);

            std::swap(grid, next_grid);

            const auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            total_duration_us += duration;

            iterations++;

        }

    }

    const double total_ms = std::chrono::duration<double, std::milli>(total_duration_us).count();

    append_csv(cfg.CSV, SCREEN_WIDTH, SCREEN_HEIGHT, ROWS, COLS, SCAN_SIZE, FRAMES, THREADS, total_ms);


    return 0;
}