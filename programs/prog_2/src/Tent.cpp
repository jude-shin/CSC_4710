#include "Tent.h"
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

Tent::Tent() {}

Tent::Tent(vector<tinyobj::shape_t>& TOshapes) 
{
	mesh = make_shared<Shape>(false);
	mesh->createShape(TOshapes[0]);
	mesh->measure();
	mesh->init();
};

void Tent::render(
		shared_ptr<Program> prog, 
		shared_ptr<MatrixStack> model) {
	model->pushMatrix();

	// Global scale rotate and translates
	model->translate(vec3(0, 0.1, -0.4));
	model->rotate(0.2, vec3(0, 0, 1));
	model->rotate(-1.57, vec3(1, 0, 0));
	model->scale(0.003);

	// Draw the Tent
	setModel(prog, model);
	mesh->draw(prog);
	
	model->popMatrix();
}
