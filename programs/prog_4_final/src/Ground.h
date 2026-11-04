#pragma once
#ifndef _GROUND_H_
#define _GROUND_H_

#include <memory>
#include "MatrixStack.h"
#include "Shape.h"
#include "Texture.h"
#include <glm/gtc/type_ptr.hpp>
#include <tiny_obj_loader/tiny_obj_loader.h>
#include <glm/fwd.hpp>

using namespace std;

class Ground {
  public:
    Ground();
    Ground(const std::string & texFile);
    void render(shared_ptr<Program> shader);

  private:
    shared_ptr<Texture> texture;
		GLuint GrndBuffObj, GrndNorBuffObj, GrndTexBuffObj, GIndxBuffObj;
		GLuint GroundVertexArrayID;
		int g_GiboLen;
};

#endif
