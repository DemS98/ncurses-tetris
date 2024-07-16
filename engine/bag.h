#ifndef BAG_H_
#define BAG_H_

#include <vector>
#include <functional>
#include "tetramine.h"

namespace tetris {
    class Bag {
        public:
            Bag(std::vector<Tetramine *>& tetramines);

            Tetramine* pop();
            bool empty();
            size_t size();

            const std::vector<Tetramine *>& getTetramines() const;
        private:
            std::vector<Tetramine *> tetramines;
    };
}

#endif