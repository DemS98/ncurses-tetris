#include "grid.h"
#include <string>
#include <iostream>
#include <unistd.h>

using namespace tetris::view;

Grid::Grid() {
    short lines, columns;

    // init screen
    initscr();
    // enable color
    start_color();
    // get terminal size
    getmaxyx(stdscr, lines, columns);

    if (lines < Matrix::HEIGHT + 2 || columns < Matrix::WIDTH + 2 + 16) {
        endwin();
        std::cerr << "Terminal too small! Please, enlarge it.";
        std::exit(1);
    }

    // enable function keys and arrow keys
    keypad(stdscr, TRUE);
    // do not echo input
    noecho();
    // no cursor
    curs_set(0);

    short bagColumn = (columns - (Matrix::WIDTH + 2)) / 2 + Matrix::WIDTH + 2 + 6;
    matrixLine = (lines - (Matrix::HEIGHT + 2)) / 2;
    matrixColumn = (columns - (Matrix::WIDTH + 2)) / 2;

    bag = newwin(18, 6, matrixLine + 4, bagColumn);
    next = newwin(4, 6, matrixLine, bagColumn);
    matrix = newwin(Matrix::HEIGHT + 2, Matrix::WIDTH + 2, matrixLine, matrixColumn);
    upMatrix = newwin(4, Matrix::WIDTH + 2, matrixLine - 4, matrixColumn);
    score = newwin(2, 10, matrixLine, matrixColumn - 15);
    time = newwin(2, 11, matrixLine + 4, matrixColumn - 15);

    // Loading
    mvwprintw(matrix, 0, (Matrix::WIDTH + 2 - 8) / 2, "Loading...");
    wrefresh(matrix);
}

Grid::~Grid() {
    delwin(bag);
    delwin(next);
    delwin(matrix);
    delwin(upMatrix);
    delwin(score);
    delwin(time);
    endwin();
}

void Grid::drawBag() {
    /* The parameters taken are 
	 * 1. win: the window on which to operate
	 * 2. ls: character to be used for the left side of the window 
	 * 3. rs: character to be used for the right side of the window 
	 * 4. ts: character to be used for the top side of the window 
	 * 5. bs: character to be used for the bottom side of the window 
	 * 6. tl: character to be used for the top left corner of the window 
	 * 7. tr: character to be used for the top right corner of the window 
	 * 8. bl: character to be used for the bottom left corner of the window 
	 * 9. br: character to be used for the bottom right corner of the window
	 */
    wborder(bag, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
    wborder(next, ACS_VLINE, ACS_VLINE, ' ', ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
    mvwprintw(next, 0, 1, "Next");
    wrefresh(bag);
    wrefresh(next);
}


void Grid::drawMatrix() {
    wborder(matrix, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
    mvwprintw(matrix, 0, (Matrix::WIDTH + 2 - 6) / 2, "Matrix");
    wrefresh(matrix);
}

void Grid::drawScore(unsigned int scoreValue) {
    mvwprintw(score, 0, 2, "Score");
    mvwprintw(score, 1, 0, "%010u", scoreValue);
    wrefresh(score);
}

void Grid::drawTime(long millis) {
    short decSec = (millis / 10) % 100;
    short sec = (millis / 1000) % 60;
    short min = (millis / 1000 / 60) % 60;
    short hour = (millis / 1000 / 60 / 60) % 60;

    mvwprintw(time, 0, 3, "Time");
    mvwprintw(time, 1, 0, "%02d:%02d:%02d.%02d", hour, min, sec, decSec);
    wrefresh(time);
}

void Grid::drawScore(unsigned int oldScore, unsigned int newScore) {
    short digits = std::to_string(newScore).size();
    if (digits > matrixColumn) {
        digits = matrixColumn;
    }

    while (oldScore < newScore) {
        drawScore(++oldScore);
        usleep(10 * 1000);
    }
}

void Grid::drawTetramine(const Tetramine& tetramine, Window window) {
    init_pair(tetramine.getColor(), tetramine.getColor(), tetramine.getColor());
    
    for(const auto& block : tetramine.getShape()) {
        switch (window)
        {
            case Window::BAG:
                wattron(bag, COLOR_PAIR(tetramine.getColor()));
                mvwaddch(bag, block.first, block.second, ' ');
                wattroff(bag, COLOR_PAIR(tetramine.getColor()));
                wrefresh(bag);
                break;
            case Window::NEXT:
                wattron(next, COLOR_PAIR(tetramine.getColor()));
                mvwaddch(next, block.first + 3, block.second, ' ');
                wattroff(next, COLOR_PAIR(tetramine.getColor()));
                wrefresh(next);
                break;
            case Window::MATRIX:
                if (block.first >= 1) {
                    wattron(matrix, COLOR_PAIR(tetramine.getColor()));
                    mvwaddch(matrix, block.first, block.second, ' ');
                    wattroff(matrix, COLOR_PAIR(tetramine.getColor()));
                    wrefresh(matrix);
                } else {
                    wattron(upMatrix, COLOR_PAIR(tetramine.getColor()));
                    mvwaddch(upMatrix, block.first + 4, block.second, ' ');
                    wattroff(upMatrix, COLOR_PAIR(tetramine.getColor()));
                    wrefresh(upMatrix);
                }
                break;
        }
        
    }
}

void Grid::clearMatrixTetramine(const Tetramine& tetramine) {
    for(const auto& block : tetramine.getShape()) {
        if (block.first >= 1) {
            mvwaddch(matrix, block.first, block.second, ' ');
            wrefresh(matrix);
        } else {
            mvwaddch(upMatrix, block.first + 4, block.second, ' ');
            wrefresh(upMatrix);
        }
    }
}

void Grid::clearBag() {
    wclear(bag);
    wclear(next);
    wrefresh(bag);
    wrefresh(next);
}

void Grid::clearMatrix() {
    wclear(matrix);
    wrefresh(matrix);
}