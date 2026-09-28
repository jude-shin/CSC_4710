#pragma once
#ifndef _TREE_H_
#define _TREE_H_

#include <vector>
#include "MyObject.h"
#include <glm/gtc/type_ptr.hpp>
#include <tiny_obj_loader/tiny_obj_loader.h>

using namespace std;
using namespace glm;

class Tree : public MyObject{
  public:
    Tree();
    Tree(vector<tinyobj::shape_t>& TOshapes);

    void render(
        shared_ptr<Program> prog, 
        shared_ptr<MatrixStack> model,
        vec3 translate,
        float scale);

  private:
    shared_ptr<Shape> trunkMesh;
    vector<shared_ptr<Shape>> leafMeshes;
    float leafDelta = 0;
    float trunkDelta = 0;
};

#endif
