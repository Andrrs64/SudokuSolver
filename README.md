# Sudoku Solver

A small pet project of mine written in c using raylib

https://www.raylib.com/

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
