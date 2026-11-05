#pragma once
#ifndef _BAMBOO_WALL_H_
#define _BAMBOO_WALL_H_

#include <memory>
#include "MatrixStack.h"
#include "Shape.h"
#include "Texture.h"
#include "MyObject.h"
#include <glm/gtc/type_ptr.hpp>
#include <tiny_obj_loader/tiny_obj_loader.h>
#include <glm/fwd.hpp>

using namespace std;
using namespace glm;

class BambooWall {
  public:
    BambooWall();
    BambooWall(
        const std::string & texFile, 
        const std::string & objFile, 
        int density);
    void render(
        shared_ptr<Program> shader, 
        shared_ptr<MatrixStack> model,
        vec3 trans, 
        float scale, 
        bool flip);

  private:
    MyObject bambooObject; // One cluster
    vector<shared_ptr<Shape>> geometry; // List of all clusters
    int density;

    static float clusterRandom(vec3 wallPosition, int clusterIndex);
};

#endif
