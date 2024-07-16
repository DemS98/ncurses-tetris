#include "matrix.h"
#include <map>

using namespace tetris;

Matrix::Matrix() {}

int Matrix::collision(const Tetramine *tetramine, Movement movement) {
    for(const auto& block : tetramine->getShape()) {
        if (block.first == HEIGHT) {
            return 2;
        }

        switch (movement)
        {
            case Movement::LEFT:
                if (block.second == 1) {
                    return 1;
                }

                if (HEIGHT - block.first < blocks.size()) {
                    const std::array<Tetramine *, 10>& blockLine = blocks[HEIGHT - block.first];
                    if (blockLine[block.second - 2]) {
                        return 1;
                    }
                }
                break;
            case Movement::RIGHT:
                if (block.second == WIDTH) {
                    return 1;
                }

                if (HEIGHT - block.first < blocks.size()) {
                    const std::array<Tetramine *, 10>& blockLine = blocks[HEIGHT - block.first];
                    if (blockLine[block.second]) {
                        return 1;
                    }
                }
                break;
            case Movement::DOWN:
                if (block.first >= HEIGHT - (short) blocks.size()) {
                    const std::array<Tetramine *, 10>& blockLine = blocks[HEIGHT - block.first - 1];
                    if (blockLine[block.second - 1]) {
                        return 2;
                    }
                }
                break;
            case Movement::ROTATION:
                const std::pair<short, short> rotatedBlock = tetramine->rotateBlock(block.first, block.second);

                if (rotatedBlock.second == 1 || rotatedBlock.second == WIDTH) {
                    return 1;
                }

                if (HEIGHT - rotatedBlock.first < blocks.size()) {
                    const std::array<Tetramine *, 10>& blockLine = blocks[HEIGHT - rotatedBlock.first];
                    if (blockLine[rotatedBlock.second - 1]) {
                        return 1;
                    }
                }
                break;
        }
    }

    return 0;
}

bool Matrix::addTetramine(Tetramine* tetramine) {
    std::map<short, std::array<Tetramine *, 10>, std::greater<short>> helperMap;

    for(const auto& block : tetramine->getShape()) {
        if (block.first < 1) {
            return false;
        }

        auto blockLineIndex = HEIGHT - block.first;
        if (blockLineIndex < blocks.size())  {
            std::array<Tetramine *, 10>& blockLine = blocks[blockLineIndex];
            blockLine[block.second - 1] = tetramine;
        } else {
            helperMap[block.first][block.second - 1] = tetramine;
        }
    }

    for(const auto& entry : helperMap) {
        blocks.push_back(entry.second);
    }

    return true;
}

bool Matrix::collapse() {
    for(auto line = 0; line < blocks.size();) {

        if (isLineFilled(blocks[line])) {
            for(auto col = 0; col < blocks[line].size(); col++) {
                blocks[line][col]->removeBlock({HEIGHT - line, col + 1});
            }
            
            blocks.erase(blocks.begin() + line);

            for(auto i = line; i < blocks.size(); i++) {
                for(auto j=0; j < blocks[i].size(); j++) {
                    if (blocks[i][j]) {
                        int row;
                        for(row = i; row > 0 && !blocks[row - 1][j]; row--) {
                            blocks[row-1][j] = blocks[row][j];
                            blocks[row][j] = nullptr;
                        }
                        blocks[row][j]->moveBlockDown({HEIGHT - i - 1, j + 1}, i - row + 1);
                    }
                }
            }

            return true;
        } else {
            line++;
        }
    }

    return false;
}

bool Matrix::isLineFilled(const std::array<Tetramine *, 10>& line) {
    for(const auto block : line) {
        if (!block) {
            return false;
        }
    }
    return true;
}

