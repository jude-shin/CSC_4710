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

#define PETAL_HEIGHT 0.3
#define PETAL_ANGLE -0.7

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
	model->rotate(tiltDelta, vec3(0, 0, 1));
	
	// Animate the Center of the flower
	setModel(prog, model);
	centerMesh->draw(prog);
	
	// Animate the Petals Rotating
	model->pushMatrix();
	model->translate(vec3(0, PETAL_HEIGHT, 0));		// Translate the petals to 0,0
	model->rotate(PETAL_ANGLE, vec3(1, 0, 0));		// Rotate the petals to 9o deg
	model->rotate(rotationDelta, vec3(0, 0, 1));	// Rotate the Petals in circle
	model->rotate(-PETAL_ANGLE, vec3(1, 0, 0));		// Rotate the Petals back
	model->translate(vec3(0, -PETAL_HEIGHT, 0));	// Translate the petals back
	setModel(prog, model);
	petalMesh->draw(prog);
	model->popMatrix();
	
	// Animate the stem and leaves
	model->pushMatrix();
	model->scale(vec3(stretchDelta, 1, 1));
	setModel(prog, model);
	stemMesh->draw(prog);
	model->popMatrix();

	model->popMatrix();

	// Change the Deltas for Animations
	tiltDelta = sin(glfwGetTime()*5)/4;
	stretchDelta = clamp(pow(sin(glfwGetTime()*5), 2), 0.5, 2.0);
	rotationDelta =  sin(glfwGetTime()*5);
}
