#include "MyObject.h"
#include "GLSL.h"
#include <assert.h>
#include <glm/fwd.hpp>
#include "Program.h"

using namespace std;
using namespace glm;

MyObject::MyObject() {}

/* helper for sending top of the matrix strack to GPU */
void MyObject::setModel(shared_ptr<Program>& prog, shared_ptr<MatrixStack>M) {
	glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
}

/* helper function to set model trasnforms */
void MyObject::setModel(shared_ptr<Program>& curS, vec3 trans, float rotY, float rotX, float sc) {
	mat4 Trans = glm::translate( glm::mat4(1.0f), trans);
	mat4 RotX = glm::rotate( glm::mat4(1.0f), rotX, vec3(1, 0, 0));
	mat4 RotY = glm::rotate( glm::mat4(1.0f), rotY, vec3(0, 1, 0));
	mat4 ScaleS = glm::scale(glm::mat4(1.0f), vec3(sc));
	mat4 ctm = Trans*RotX*RotY*ScaleS;
	glUniformMatrix4fv(curS->getUniform("M"), 1, GL_FALSE, value_ptr(ctm));
}

shared_ptr<Shape> MyObject::getMesh() {
	return mesh;
}
