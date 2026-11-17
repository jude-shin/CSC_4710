#pragma once
#ifndef _SKYBOX_H_
#define _SKYBOX_H_

#include <memory>
#include <string>

#include "Cube.h"
#include "MatrixStack.h"
#include "Texture.h"

class Program;

class Skybox {
  public:
    Skybox() = default;

    // Call once after skyboxShader has been initialized.
    void init(
        const std::shared_ptr<Program> &skyboxShader,
        const std::string &textureFile,
        float size = 500.0f
        );

    // Draw before all normal scene objects.
    void render(
        const std::shared_ptr<Program> &skyboxShader,
        const std::shared_ptr<MatrixStack> &projection,
        const std::shared_ptr<MatrixStack> &view
        ) const;

  private:
    CubeMesh mesh;
    Cube cube;
    std::shared_ptr<Texture> texture;
    bool initialized = false;
};

#endif
