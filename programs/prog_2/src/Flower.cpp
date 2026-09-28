#include "Flower.h"
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

#define PETAL_COUNT 3

Flower::Flower() {}

Flower::Flower(vector<tinyobj::shape_t>& TOshapes) {
	petalMesh = make_shared<Shape>(false);
	petalMesh->createShape(TOshapes[0]);
	petalMesh->measure();
	petalMesh->init();

	centerMesh = make_shared<Shape>(false);
	centerMesh->createShape(TOshapes[1]);
	centerMesh->measure();
	centerMesh->init();

	stemMesh = make_shared<Shape>(false);
	stemMesh->createShape(TOshapes[2]);
	stemMesh->measure();
	stemMesh->init();
};

void Flower::render(
		shared_ptr<Program> prog, 
		shared_ptr<MatrixStack> model,
		vec3 translate,
		float scale) {
	model->pushMatrix();

	// Global scale rotate and translates
	model->translate(translate);
	model->scale(scale);

	// // Draw the Trunk (sways in the wind)
	// model->rotate(trunkDelta, vec3(0, 0, 1));
	// setModel(prog, model);
	// trunkMesh->draw(prog);
	// 

	setModel(prog, model);
	centerMesh->draw(prog);

	setModel(prog, model);
	petalMesh->draw(prog);

	setModel(prog, model);
	stemMesh->draw(prog);

	model->popMatrix();

	// Animate Everything
	tiltDelta = sin(glfwGetTime());
	stretchDelta = sin(glfwGetTime());
	rotationDelta = sin(glfwGetTime())*0.03;
}
