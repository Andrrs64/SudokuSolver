# Sudoku Solver

A small pet project of mine written in c using raylib

https://www.raylib.com/

This is in a very early stage and is very far from perfect.
It cannot solve all sudoku puzzles and it is not user friendly at all, considering the puzzle is hardcoded for now, and there is no form of user interface.
The code also is in need of a refactor, as it is hard to debug.

## Build and run
This project uses cmake. I've only tested it on mac, but it's a simple project so it shouldn't be too hard to figure out how to do on linux or windows.

### MacOS
To build on mac you need Xcode or Xcode command line tools:
```
xcode-select --install
```

You also need CMake
Install via homebrew:
```
brew install cmake
```

Run following commands from the root directory.
```
mkdir build
cd build
cmake ..
make
./SudokuSolver
```

## Usage
As stated earlier the puzzle is hardcoded and you have to change the values variable in main.c, and recompile to change the puzzle.

The tiles with a full size number in it is known by the program to be correct. The others will have smaller numbers, and that shows all the values that tile could possibly have.

It will solve one 3x3 group at the time, and it will highlight the group to be solved next.
To solve the group, press 'SPACE'.
You can press 'U' to undo, but you can only undo once at the time as it only has one buffer that stores the state before the last solve.
