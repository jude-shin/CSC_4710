#pragma once
#ifndef _TREE_H_
#define _TREE_H_

#include <vector>
#include "MyObject.h"
#include <glm/gtc/type_ptr.hpp>
#include <tiny_obj_loader/tiny_obj_loader.h>

#define OBJ_FILE "/tree.obj"

class Tree : public MyObject{
  public:
    Tree();
    Tree(std::vector<tinyobj::shape_t>& TOshapes);

    void render(std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack>& model);
};

#endif
