#pragma once
#ifndef _BOUNDING_BOX_H_
#define _BOUNDING_BOX_H_

#include <string>
#include <vector>
#include "Image.h" 

class BoundingBox {
public:
  BoundingBox(int v_a_x, int v_a_y, int v_b_x, int v_b_y, int v_c_x, int v_c_y);
  void draw(Image* image);

private:
  int min_x;
  int max_x;
  int min_y;
  int max_y;
};

#endif
