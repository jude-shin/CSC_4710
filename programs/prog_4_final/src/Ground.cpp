#include "Ground.h"
#include "GLSL.h"
#include <assert.h>
#include <iostream>
#include <glm/fwd.hpp>
#include "Program.h"

using namespace std;
using namespace glm;

Ground::Ground() {}

Ground::Ground(const std::string & texFile) {
	// Load the Texutre
	texture = make_shared<Texture>();
	texture->setFilename(texFile);
	texture->init();
	texture->setUnit(0);
	texture->setWrapModes(GL_REPEAT, GL_REPEAT);

	// Init the quad geometry
	float g_groundSize = 20;
	float g_groundY = -0.25;

	// A x-z plane at y = g_groundY of dimension [-g_groundSize, g_groundSize]^2
	float GrndPos[] = {
		-g_groundSize, g_groundY, -g_groundSize,
		-g_groundSize, g_groundY,  g_groundSize,
		g_groundSize, g_groundY,  g_groundSize,
		g_groundSize, g_groundY, -g_groundSize
	};

	float GrndNorm[] = {
		0, 1, 0,
		0, 1, 0,
		0, 1, 0,
		0, 1, 0,
		0, 1, 0,
		0, 1, 0
	};

	static GLfloat GrndTex[] = {
		0, 0,
		0, 10.0,
		10.0, 10.0,
		10.0, 0 };

	unsigned short idx[] = {0, 1, 2, 0, 2, 3};

	// Generate the ground VAO
	glGenVertexArrays(1, &GroundVertexArrayID);
	glBindVertexArray(GroundVertexArrayID);

	g_GiboLen = 6;
	glGenBuffers(1, &GrndBuffObj);
	glBindBuffer(GL_ARRAY_BUFFER, GrndBuffObj);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GrndPos), GrndPos, GL_STATIC_DRAW);

	glGenBuffers(1, &GrndNorBuffObj);
	glBindBuffer(GL_ARRAY_BUFFER, GrndNorBuffObj);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GrndNorm), GrndNorm, GL_STATIC_DRAW);

	glGenBuffers(1, &GrndTexBuffObj);
	glBindBuffer(GL_ARRAY_BUFFER, GrndTexBuffObj);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GrndTex), GrndTex, GL_STATIC_DRAW);

	glGenBuffers(1, &GIndxBuffObj);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GIndxBuffObj);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);
}

void Ground::render(shared_ptr<Program> shader) {
	shader->bind();
	glBindVertexArray(GroundVertexArrayID);
	texture->bind(shader->getUniform("Texture0"));

	// Draw the ground plane 
	// To Init, we only need the translation vector
	mat4 t = translate( glm::mat4(1.0f), vec3(0, -1, 0));
	glUniformMatrix4fv(shader->getUniform("M"), 1, GL_FALSE, value_ptr(t));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, GrndBuffObj);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, GrndNorBuffObj);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, GrndTexBuffObj);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

	// Draw!
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GIndxBuffObj);
	glDrawElements(GL_TRIANGLES, g_GiboLen, GL_UNSIGNED_SHORT, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	shader->unbind();
}
