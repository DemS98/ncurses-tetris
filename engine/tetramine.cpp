#include "tetramine.h"
#include "matrix.h"
#include <cmath>

using namespace tetris;

Tetramine::Tetramine(short color) : color {color} {
    shape.reserve(4);
}

void Tetramine::scaleVertical(short lines) {
    for(std::pair<short, short>& block : shape) {
        block.first += lines;
    }
}

void Tetramine::scaleHorizontal(short columns) {
    for(std::pair<short, short>& block : shape) {
        block.second += columns;
    }
}

void Tetramine::move(Movement movement) {
    std::pair<short, short> minCoordinates = getMinCoordinates();
    short len = length(minCoordinates.second);
    for(std::pair<short, short>& block : shape) {
        switch (movement)
        {
            case Movement::LEFT:
                block.second--;
                break;
            case Movement::RIGHT:
                block.second++;
                break;
            case Movement::DOWN:
                block.first++;
                break;
            case Movement::ROTATION:
                block = rotateBlock(block.first, block.second, &minCoordinates, &len);
                break;
        }
    }
}

const std::pair<short, short> Tetramine::rotateBlock(short line, short col, std::pair<short, short> *minCoordinates,
    short *len) const {
    std::pair<short, short> temp;
    short tempLength;

    if (!minCoordinates) {
        temp = getMinCoordinates();
        minCoordinates = &temp;
    }
    if (!len) {
        tempLength = length(minCoordinates->second);
        len = &tempLength;
    }

    short deltaCol = std::round(*len / 2.0);
    auto x = col - minCoordinates->second;
    auto y = line - minCoordinates->first;

    short rotatedCol = col - (x + y) + deltaCol;
    short rotatedLine = line - (y - x);

    return {rotatedLine, rotatedCol};
}

void Tetramine::addBlock(std::pair<short, short> block) {
    shape.push_back(block);
}

void Tetramine::removeBlock(std::pair<short, short> block) {
    for(auto b = shape.begin(); b != shape.end(); b++) {
        if (b->first == block.first && b->second == block.second) {
            shape.erase(b);
            break;
        }
    }
}

void Tetramine::moveBlockDown(std::pair<short, short> block, short lines) {
    for(std::pair<short, short>& b : shape) {
        if (b.first == block.first && b.second == block.second) {
            b.first += lines;
            break;
        }
    }
}

bool Tetramine::insideBounds() const {
    for(const std::pair<short, short>& block : shape) {
        if (block.first < 1) {
            return false;
        }
    }

    return true;
}

short Tetramine::getColor() const {
    return color;
}

const std::vector<std::pair<short, short>>& Tetramine::getShape() const {
    return shape;
}

const std::pair<short, short> Tetramine::getMinCoordinates() const {
    short minLine = Matrix::HEIGHT;
    short minCol = Matrix::WIDTH;

    for(const auto& block : shape) {
        if (block.first < minLine) {
            minLine = block.first;
        }

        if (block.second < minCol) {
            minCol = block.second;
        }
    }

    return {minLine, minCol};
}

const short Tetramine::length(short minCol) const {
    short maxCol = 0;

    for(const auto& block : shape) {
        if (block.second > maxCol) {
            maxCol = block.second;
        }
    }

    return maxCol - minCol + 1;
}