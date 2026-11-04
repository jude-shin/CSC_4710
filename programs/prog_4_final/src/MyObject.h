#pragma once
#ifndef _MY_OBJECT_H_
#define _MY_OBJECT_H_

#include <memory>
#include "MatrixStack.h"
#include "Shape.h"
#include "Texture.h"
#include <glm/gtc/type_ptr.hpp>
#include <tiny_obj_loader/tiny_obj_loader.h>
#include <glm/fwd.hpp>

using namespace std;

class MyObject {
  public:
    MyObject();
    MyObject(const std::string & texFile, const std::string & objFile);
    shared_ptr<Texture> getTexture();
    vector<shared_ptr<Shape>> getGeometry();

  private:
    shared_ptr<Texture> texture;
    vector<shared_ptr<Shape>> geometry;
};

#endif
