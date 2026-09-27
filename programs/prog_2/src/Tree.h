#pragma once
#ifndef _TREE_H_
#define _TREE_H_

#include <string>
#include <vector>
#include <memory>
#include "Shape.h"
#include <glm/gtc/type_ptr.hpp>
#include <tiny_obj_loader/tiny_obj_loader.h>

#define OBJ_FILE "/tree.obj"

class Tree {
  public:
    Tree();
    Tree(std::vector<tinyobj::shape_t>& TOshapes);
    std::shared_ptr<Shape> getMesh();

  private:
    // Tree Mesh
    std::shared_ptr<Shape> mesh;
};

#endif
