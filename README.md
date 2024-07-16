# ncurses-tetris

Tetris implementation with [Ncurses](https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/) library.

## Required dependencies

* *g++*
* *libasound2-dev*
* *libopusfile-dev*
* *libncurses-dev*

Example Debian installation:

```shell
sudo apt install g++ libasound2-dev libopusfile-dev libncurses-dev
```

## Compile the binary

Just run `make`.

To remove intermediate object files, `make clean`

> Tetris theme will be embedded inside the binary

## Play

Game is played with the keyboard:

| Key | Action |
| --- | ------ |
| ↑   | Rotate block |
| →   | Move block to right |
| ←   | Move block to left |
| ↓   | Speed up block fall (can be held down for more speed) |
| +   | Increase music volume (bugged) |
| -   | Decrease music volume (bugged) |
| ↲   | Close when game over           |
| CTRL + C | Force close the game      |


## Known limitations

This was just a fun project to experiment with C++ so there could be some bugs.

For now, the only known bug is relative to the music volume actions: when the volume is changed for the first time since starting the game, it's muted; then the *+* and *-* keys work as expected.