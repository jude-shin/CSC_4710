#pragma once
#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_

#include <string>
#include <vector>
#include "Image.h" 

class Triangle {
public:
  Triangle(int vax, int vay, int vbx, int vby, int vcx, int vcy);
  void draw_bounding_box(Image* image);

private:
  int v_a_x; 
  int v_a_y; 
  int v_b_x; 
  int v_b_y; 
  int v_c_x; 
  int v_c_y;
};

#endif
