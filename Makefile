SOUNDLIBS=-I/usr/include/opus -lasound -lopusfile
LDLIBS=-lncurses -lpthread
CPPFLAGS=-D NO_SOUND # used when compile without sound support

# Common object files for engine
ENGINE_OBJS=engine/bag.o engine/matrix.o engine/tetramine.o

# Common object files for view
VIEW_OBJS=view/grid.o

all: tetris_theme.o engine view game

no_sound: engine view game_mute

tetris_theme.o: tetris_theme.opus
	ld -z noexecstack -r -b binary -o tetris_theme.o tetris_theme.opus

# Compile engine object files
engine: $(ENGINE_OBJS)

engine/bag.o: engine/bag.cpp
	$(CXX) -c engine/bag.cpp -o engine/bag.o

engine/matrix.o: engine/matrix.cpp
	$(CXX) -c engine/matrix.cpp -o engine/matrix.o

engine/tetramine.o: engine/tetramine.cpp
	$(CXX) -c engine/tetramine.cpp -o engine/tetramine.o

# Compile view object files
view: $(VIEW_OBJS)

view/grid.o: view/grid.cpp
	$(CXX) -c view/grid.cpp -o view/grid.o

# Link the game executable with sound
game: tetris_theme.o engine view
	$(CXX) game.cpp $(ENGINE_OBJS) $(VIEW_OBJS) tetris_theme.o -o tetris $(LDLIBS) $(SOUNDLIBS)

# Link the game executable without sound
game_mute: engine view
	$(CXX) $(CPPFLAGS) game.cpp $(ENGINE_OBJS) $(VIEW_OBJS) -o tetris $(LDLIBS)

clean:
	rm -f *.o engine/*.o view/*.o
