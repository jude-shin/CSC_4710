#pragma once
#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_

#include <string>
#include <vector>
#include "Image.h" 
#include "Point.h" 

// Special Render Mode Colors
#define SPECIAL_CYAN_R 0
#define SPECIAL_CYAN_G 255
#define SPECIAL_CYAN_B 255
#define SPECIAL_CYAN_THRESH 0.2

#define SPECIAL_YELLOW_R 255
#define SPECIAL_YELLOW_G 255
#define SPECIAL_YELLOW_B 0
#define SPECIAL_YELLOW_THRESH 0.05

#define SPECIAL_MAGENTA_R 255
#define SPECIAL_MAGENTA_G 0
#define SPECIAL_MAGENTA_B 255


// Depth Render Mode Colors
// TODO: implement
#define DEPTH_R 191
#define DEPTH_G 0 
#define DEPTH_B 255 


enum Mode {
  DEPTH = 1,
  SPECIAL = 2
};

class Triangle {
public:
  Triangle(Point* a, Point* b, Point* c);
  void draw_bounding_box(Image* image);
  int draw_triangle(Image* image, std::vector<float>* zbuff, int width, int height, Mode mode);

private:
  Point a;
  Point b;
  Point c;
};

#endif
