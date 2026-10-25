// Maze.h
#pragma once
#ifndef _MAZE_H_
#define _MAZE_H_

#include <vector>
#include <random>

using namespace std;

struct Point {
  int x;
  int y;
};

class Maze {
  public:
    Maze(int w, int h);

    vector<vector<bool>> render();
    void generateRecursiveBacktracker(int seed);

  private:
    int width;
    int height;

    vector<vector<int>> nodes;

    void generateRecursive(int x, int y, std::mt19937 &generator);
};

#endif
