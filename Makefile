LDLIBS=-I/usr/include/opus -lasound -lopusfile -lncurses -lpthread

all: tetris_theme engine view game

tetris_theme: tetris_theme.opus
	ld -z noexecstack -r -b binary -o tetris_theme.o tetris_theme.opus

engine: tetris_theme
	$(CXX) -c engine/bag.cpp -o engine/bag.o 
	$(CXX) -c engine/matrix.cpp -o engine/matrix.o
	$(CXX) -c engine/tetramine.cpp -o engine/tetramine.o

view: tetris_theme engine
	$(CXX) -c view/grid.cpp -o view/grid.o

game: tetris_theme.opus engine view
	$(CXX) game.cpp engine/bag.o engine/matrix.o engine/tetramine.o view/grid.o tetris_theme.o -o tetris $(LDLIBS)

clean:
	rm -f *.o engine/*.o view/*.o
