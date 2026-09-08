#pragma once
#ifndef _POINT_H_
#define _POINT_H_

#include <string>
#include <vector>
#include "Image.h"

class Point {
public:
  Point(int x, int y, float z, int r, int g, int b);
  Point(const Point& other);
  Point(int x, int y, float z);
  Point();

  int get_x() const;
  int get_y() const;
  float get_z() const;
  int get_r() const;
  int get_g() const;
  int get_b() const;

private:
  int x;
  int y;
  float z;
  int r;
  int g;
  int b;
};

#endif
