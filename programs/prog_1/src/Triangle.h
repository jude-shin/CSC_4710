#pragma once
#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_

#include <string>
#include <vector>
#include "Image.h" 
#include "Point.h" 

class Triangle {
public:
  Triangle(Point* a, Point* b, Point* c);
  void draw_bounding_box(Image* image);
  void draw_triangle(Image* image);

private:
  Point a;
  Point b;
  Point c;
};

#endif
