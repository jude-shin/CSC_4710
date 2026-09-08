#pragma once
#ifndef _POINT_H_
#define _POINT_H_

#include <string>
#include <vector>
#include "Image.h"

class Point {
public:
  Point(float x, float y, float z, int r, int g, int b);
  Point(const Point& other);
  Point(float x, float y, float z);
  Point();

  float get_x() const;
  float get_y() const;
  float get_z() const;
  float get_r() const;
  float get_g() const;
  float get_b() const;

private:
  float x;
  float y;
  float z;
  float r;
  float g;
  float b;
};

#endif
