#pragma once
#ifndef _MY_OBJECT_H_
#define _MY_OBJECT_H_

#include <memory>
#include "MatrixStack.h"
#include "Shape.h"
#include <glm/gtc/type_ptr.hpp>
#include <tiny_obj_loader/tiny_obj_loader.h>
#include <glm/fwd.hpp>

class MyObject {
  public:
    MyObject();
    std::shared_ptr<Shape> getMesh();

  protected:
    void setModel(std::shared_ptr<Program>& prog, std::shared_ptr<MatrixStack>M);
  	void setModel(std::shared_ptr<Program>& curS, glm::vec3 trans, float rotY, float rotX, float sc);

};

#endif
