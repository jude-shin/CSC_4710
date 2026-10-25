// Maze.h
#pragma once
#ifndef _MAZE_H_
#define _MAZE_H_

#include <vector>
#include <memory>
#include <random>
#include "Program.h"
#include "Shape.h"
#include "MatrixStack.h"

using namespace std;

struct Point {
  int x;
  int y;
};

class Maze {
  public:
    Maze(int w, int h);
    void render(
        shared_ptr<Program> prog, 
        shared_ptr<Shape> wallObject, 
        shared_ptr<MatrixStack> Model);
    void generateRecursiveBacktracker(int seed);

  private:
    int width;
    int height;

    vector<vector<int>> nodes;

    void generateRecursive(int x, int y, mt19937 &generator);
};

#endif
