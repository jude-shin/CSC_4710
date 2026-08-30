#pragma once
#ifndef _POINT_H_
#define _POINT_H_

#include <string>
#include <vector>
#include "Image.h"

class Point {
public:
  Point(int x, int y, int r, int g, int b);
  Point(const Point& other);
  Point();

  int get_x() const;
  int get_y() const;
  int get_r() const;
  int get_g() const;
  int get_b() const;

  void draw_points(Image* image);


private:
  int x;
  int y;
  int r;
  int g;
  int b;
};

#endif
