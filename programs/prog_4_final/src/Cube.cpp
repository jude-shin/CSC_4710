#include "Cube.h"
#include "Program.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace {
	const GLfloat vertices[] = {
		// FRONT
		-0.5, 0.5, 0.5,  -0.5,-0.5, 0.5,   0.5,-0.5, 0.5,
		-0.5, 0.5, 0.5,   0.5,-0.5, 0.5,   0.5, 0.5, 0.5,

		// LEFT
		-0.5, 0.5,-0.5,  -0.5,-0.5,-0.5,  -0.5,-0.5, 0.5,
		-0.5, 0.5,-0.5,  -0.5,-0.5, 0.5,  -0.5, 0.5, 0.5,

		// RIGHT
		0.5, 0.5, 0.5,   0.5,-0.5, 0.5,   0.5,-0.5,-0.5,
		0.5, 0.5, 0.5,   0.5,-0.5,-0.5,   0.5, 0.5,-0.5,

		// TOP
		-0.5, 0.5,-0.5,  -0.5, 0.5, 0.5,   0.5, 0.5, 0.5,
		-0.5, 0.5,-0.5,   0.5, 0.5, 0.5,   0.5, 0.5,-0.5,

		// BACK
		0.5, 0.5,-0.5,   0.5,-0.5,-0.5,  -0.5, 0.5,-0.5,
		-0.5, 0.5,-0.5,   0.5,-0.5,-0.5,  -0.5,-0.5,-0.5,

		// BOTTOM
		-0.5,-0.5, 0.5,  -0.5,-0.5,-0.5,   0.5,-0.5,-0.5,
		-0.5,-0.5, 0.5,   0.5,-0.5,-0.5,   0.5,-0.5, 0.5
	};

	// skybox.jpg is 736 x 552 pixels.
	// Move every sampled edge two pixels inward.
	constexpr GLfloat du = 2.0f / 736.0f;
	constexpr GLfloat dv = 2.0f / 552.0f;
	
	// Horizontal tile boundaries, inset from each tile edge.
	constexpr GLfloat u0 = 0.00f + du;
	constexpr GLfloat u1 = 0.25f - du;
	constexpr GLfloat u2 = 0.25f + du;
	constexpr GLfloat u3 = 0.50f - du;
	constexpr GLfloat u4 = 0.50f + du;
	constexpr GLfloat u5 = 0.75f - du;
	constexpr GLfloat u6 = 0.75f + du;
	constexpr GLfloat u7 = 1.00f - du;
	
	// Vertical tile boundaries, inset from each tile edge.
	constexpr GLfloat v0 = 0.00f + dv;
	constexpr GLfloat v1 = 1.0f / 3.0f - dv;
	constexpr GLfloat v2 = 1.0f / 3.0f + dv;
	constexpr GLfloat v3 = 2.0f / 3.0f - dv;
	constexpr GLfloat v4 = 2.0f / 3.0f + dv;
	constexpr GLfloat v5 = 1.00f - dv;
	
	const GLfloat uvs[] = {
	    // FRONT: center tile
	    u2,v3,  u2,v2,  u3,v2,
	    u2,v3,  u3,v2,  u3,v3,
	
	    // LEFT: left-middle tile
	    u0,v3,  u0,v2,  u1,v2,
	    u0,v3,  u1,v2,  u1,v3,
	
	    // RIGHT: right-middle tile
	    u4,v3,  u4,v2,  u5,v2,
	    u4,v3,  u5,v2,  u5,v3,
	
	    // TOP: upper-middle tile
	    u2,v5,  u2,v4,  u3,v4,
	    u2,v5,  u3,v4,  u3,v5,
	
	    // BACK: far-right-middle tile
	    u6,v3,  u6,v2,  u7,v3,
	    u7,v3,  u6,v2,  u7,v2,
	
	    // BOTTOM: lower-middle tile
	    u2,v1,  u2,v0,  u3,v0,
	    u2,v1,  u3,v0,  u3,v1
	};
	static_assert(sizeof(vertices) / sizeof(GLfloat) == 36 * 3, "36 cube vertices");
	static_assert(sizeof(uvs) / sizeof(GLfloat) == 36 * 2, "One UV per vertex");
}

CubeMesh::~CubeMesh() {
	glDeleteBuffers(2, buffers);
	if (vao) glDeleteVertexArrays(1, &vao);
}

void CubeMesh::init(const Program& program) {
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(2, buffers);
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	const GLint pos = program.getAttribute("vertPos");
	glEnableVertexAttribArray(pos);
	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uvs), uvs, GL_STATIC_DRAW);
	const GLint uv = program.getAttribute("vertTex");
	glEnableVertexAttribArray(uv);
	glVertexAttribPointer(uv, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void CubeMesh::draw() const {
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

glm::mat4 Cube::modelMatrix() const {
	glm::mat4 m = glm::translate(glm::mat4(1.0f), position);
	m = glm::rotate(m, glm::radians(rotation.x), glm::vec3(1, 0, 0));
	m = glm::rotate(m, glm::radians(rotation.y), glm::vec3(0, 1, 0));
	m = glm::rotate(m, glm::radians(rotation.z), glm::vec3(0, 0, 1));
	return glm::scale(m, scale);
}

void Cube::render(const Program& program, const CubeMesh& mesh) const {
	const auto model = modelMatrix();
	glUniformMatrix4fv(program.getUniform("M"), 1, GL_FALSE, glm::value_ptr(model));
	mesh.draw();
}
