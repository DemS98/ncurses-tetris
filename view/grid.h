#ifndef GRID_H_
#define GRID_H_

#include "../engine/tetramine.h"
#include "../engine/matrix.h"
#include <ncurses.h>
#include <vector>

namespace tetris {
    namespace view {

        enum class Window {
            BAG,
            NEXT,
            MATRIX
        };

        class Grid {
            public:

                Grid();
                ~Grid();

                void drawBag();
                void drawMatrix();
                void drawScore(unsigned int score);
                void drawScore(unsigned int oldScore, unsigned int newScore);
                void drawTime(long time);
                void drawTetramine(const Tetramine& tetramine, Window window);
                void clearMatrixTetramine(const Tetramine& tetramine);
                void clearBag();
                void clearMatrix();
                
            private:
                short matrixLine;
                short matrixColumn;

                WINDOW *bag;
                WINDOW *next;
                WINDOW *upMatrix;
                WINDOW *matrix;
                WINDOW *score;
                WINDOW *time;
        };
    }
}

#endif