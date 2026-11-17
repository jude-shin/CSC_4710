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
    
    // For those that have .mtl
    MyObject(const string &objFileName, const string &resourceDirectory);
  
    // For those that don't have .mtl (explicit texture listing)
    MyObject(
        const std::string &objFile,
        const std::vector<std::string> &textureFiles,
        const std::vector<size_t> &textureForShape
        );

    void render(
        std::shared_ptr<Program> shader,
        std::shared_ptr<MatrixStack> model
        );

  private:
    struct Part {
      std::shared_ptr<Shape> shape;
      size_t textureIndex;
    };

    std::vector<std::shared_ptr<Texture>> textures;
    std::vector<Part> parts;
};



#endif
