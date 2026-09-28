#include "Mountain.h"
#include <vector>
#include <assert.h>
#include <glm/fwd.hpp>
#include <cmath>
#include <GLFW/glfw3.h>

// #include "GLSL.h"
// #include "Program.h"
#include "MatrixStack.h"

using namespace std;
using namespace glm;

Mountain::Mountain() {}

Mountain::Mountain(vector<tinyobj::shape_t>& TOshapes) 
{
	mesh = make_shared<Shape>(false);
	mesh->createShape(TOshapes[0]);
	mesh->measure();
	mesh->init();
};

void Mountain::render(
		shared_ptr<Program> prog, 
		shared_ptr<MatrixStack> model) {
	model->pushMatrix();

	// Global scale rotate and translates
	model->translate(vec3(3, 1.75, -4));

	// Draw the Mountain
	setModel(prog, model);
	mesh->draw(prog);
	
	model->popMatrix();
}
