#ifndef MATRIX_H_
#define MATRIX_H_

#include <array>
#include "tetramine.h"

namespace tetris {
    class Matrix {
        public:
            static constexpr short WIDTH = 10;
            static constexpr short HEIGHT = 20;

            Matrix();

            int collision(const Tetramine *tetramine, Movement movement);
            bool addTetramine(Tetramine *tetramine);
            bool collapse();

        private:
            bool isLineFilled(const std::array<Tetramine *, 10>& line);

            std::vector<std::array<Tetramine *, 10>> blocks;
    };
}

#endif