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
#include "BambooWall.h"

using namespace std;

struct Point {
  int x;
  int y;
};

class Maze {
  public:
    Maze(int w, int h);
    void render(
        shared_ptr<Program> shader, 
        BambooWall bambooWall,
        shared_ptr<MatrixStack> model,
        float mazeScale);
    void generateRecursiveBacktracker(int seed);

    // Returns true when the camera's XZ footprint overlaps a bamboo wall.
    bool collides(const glm::vec3 &position, float radius, float mazeScale) const;

  private:
    int width;
    int height;

    vector<vector<int>> nodes;

    void generateRecursive(int x, int y, mt19937 &generator);
};

#endif
