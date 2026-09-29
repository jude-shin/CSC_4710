#pragma once
#ifndef _CLOUD_H_
#define _CLOUD_H_

#include <vector>
#include "MyObject.h"
#include <glm/gtc/type_ptr.hpp>
#include <tiny_obj_loader/tiny_obj_loader.h>

using namespace std;
using namespace glm;

class Cloud : public MyObject{
  public:
    Cloud();
    Cloud(vector<tinyobj::shape_t>& TOshapes);

    void render(
        shared_ptr<Program> prog, 
        shared_ptr<MatrixStack> model,
        vec3 translate,
        float scale);

  private:
    vector<shared_ptr<Shape>> fluffs;
};

#endif
