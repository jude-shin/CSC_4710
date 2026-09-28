#pragma once
#ifndef _MOUNTAIN_H_
#define _MOUNTAIN_H_

#include <vector>
#include "MyObject.h"
#include <glm/gtc/type_ptr.hpp>
#include <tiny_obj_loader/tiny_obj_loader.h>

using namespace std;
using namespace glm;

class Mountain : public MyObject{
  public:
    Mountain();
    Mountain(vector<tinyobj::shape_t>& TOshapes);

    void render(
        shared_ptr<Program> prog, 
        shared_ptr<MatrixStack> model);

  private:
    shared_ptr<Shape> mesh;
};

#endif
