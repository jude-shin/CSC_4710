#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
class Program;

// One shared GPU mesh; each Cube has its own Lab 3 transform.
class CubeMesh {
public:
    CubeMesh() = default;
    ~CubeMesh();
    CubeMesh(const CubeMesh&) = delete;
    CubeMesh& operator=(const CubeMesh&) = delete;
    void init(const Program& program);
    void draw() const;
private:
    GLuint vao = 0;
    GLuint buffers[2] = {0, 0};
};

class Cube {
public:
    glm::vec3 position{0.0f};
    glm::vec3 rotation{0.0f}; // degrees, matching the JavaScript source
    glm::vec3 scale{1.0f};
    glm::mat4 modelMatrix() const;
    void render(const Program& program, const CubeMesh& mesh) const;
};
