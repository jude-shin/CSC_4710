#pragma once
#ifndef _FLOWER_H_
#define _FLOWER_H_

#include <vector>
#include "MyObject.h"
#include <glm/gtc/type_ptr.hpp>
#include <tiny_obj_loader/tiny_obj_loader.h>

using namespace std;
using namespace glm;

class Flower : public MyObject{
  public:
    Flower();
    Flower(vector<tinyobj::shape_t>& TOshapes);

    void render(
        shared_ptr<Program> prog, 
        shared_ptr<MatrixStack> model,
        vec3 translate,
        float scale);

  private:
    shared_ptr<Shape> centerMesh;
    shared_ptr<Shape> stemMesh;
    shared_ptr<Shape> petalMesh;

    float rotationDelta = 0;
    float tiltDelta = 0;
    float stretchDelta = 0;
};

#endif
