#ifndef TETRAMINE_H_
#define TETRAMINE_H_

#include <vector>

namespace tetris {

    enum class Movement {
        LEFT,
        RIGHT,
        DOWN,
        ROTATION
    };

    class Tetramine {
        public:
            // initial_shape refer to (0,0) position
            Tetramine(short color);
            
            void scaleVertical(short lines);
            void scaleHorizontal(short col);
            void move(Movement movement);
            
            void addBlock(std::pair<short, short> block);
            void removeBlock(std::pair<short, short> block);
            void moveBlockDown(std::pair<short, short> block, short lines);
            
            bool insideBounds() const;

            short getColor() const;
            const std::pair<short, short> rotateBlock(short line, short col, std::pair<short, short> *minCoordinates = nullptr,
                short *length = nullptr) const;
            const std::vector<std::pair<short, short>>& getShape() const;

        private:
            const std::pair<short, short> getMinCoordinates() const;
            const short length(short minCol) const;

            short color;
            // shape is the position of each block in the board
            std::vector<std::pair<short, short>> shape;
    };
}

#endif