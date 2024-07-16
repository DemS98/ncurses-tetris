#include "bag.h"
#include "ncurses.h"
#include <random>
#include <stdexcept>
#include "matrix.h"

using namespace tetris;

Bag::Bag(std::vector<Tetramine *>& tetramineList) {
    if (tetramineList.size() % 7 != 0) {
        throw std::invalid_argument("Tetramines in the bag must be 7!");
    }

    std::mt19937 generator(std::random_device{}());
    std::vector<short> container({1, 2, 3, 4, 5, 6, 7});

    auto line = -2;
    while (!container.empty()) {
        std::uniform_int_distribution<short> distr(0, container.size() - 1);
        auto i = distr(generator);
        auto index = container[i];
        container.erase(container.begin() + i);

        auto tetramine = tetramineList[index - 1];

        switch (index)
        {
            case 1:
                tetramine->addBlock({line, 1});
                tetramine->addBlock({line, 2});
                tetramine->addBlock({line + 1, 1});
                tetramine->addBlock({line + 1, 2});
                break;
            case 2:
                tetramine->addBlock({line, 1});
                tetramine->addBlock({line, 2});
                tetramine->addBlock({line + 1, 2});
                tetramine->addBlock({line + 1, 3});
                break;
            case 3:
                tetramine->addBlock({line, 2});
                tetramine->addBlock({line, 3});
                tetramine->addBlock({line + 1, 1});
                tetramine->addBlock({line + 1, 2});
                break;
            case 4:
                tetramine->addBlock({line, 1});
                tetramine->addBlock({line + 1, 1});
                tetramine->addBlock({line + 1, 2});
                tetramine->addBlock({line + 1, 3});
                break;
            case 5:
                tetramine->addBlock({line, 3});
                tetramine->addBlock({line + 1, 1});
                tetramine->addBlock({line + 1, 2});
                tetramine->addBlock({line + 1, 3});
                break;
            case 6:
                tetramine->addBlock({line, 2});
                tetramine->addBlock({line + 1, 1});
                tetramine->addBlock({line + 1, 2});
                tetramine->addBlock({line + 1, 3});
                break;
            case 7:
                tetramine->addBlock({line + 1, 1});
                tetramine->addBlock({line + 1, 2});
                tetramine->addBlock({line + 1, 3});
                tetramine->addBlock({line + 1, 4});
                break;
        }

        tetramines.push_back(tetramine);
        line += 3;
    }
}

Tetramine* Bag::pop() {
    Tetramine* tetramine = tetramines[0];

    tetramines.erase(tetramines.begin());

    tetramine->scaleVertical(-3);
    tetramine->scaleHorizontal((Matrix::WIDTH - (tetramine->getColor() == COLOR_BLUE ?
                2 : (tetramine->getColor() == COLOR_RED ? 4 : 3))) / 2);

    for(auto other : tetramines) {
        other->scaleVertical(-3);
    }
    
    return tetramine;
}

bool Bag::empty() {
    return tetramines.empty();
}

const std::vector<Tetramine *>& Bag::getTetramines() const {
    return tetramines;
}

size_t Bag::size() {
    return tetramines.size();
}