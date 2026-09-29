#include "Cloud.h"
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

#define FLUFF_COUNT 24

Cloud::Cloud() {}

Cloud::Cloud(vector<tinyobj::shape_t>& TOshapes) 
{
	for (int i = 0; i < FLUFF_COUNT; ++i) {
		auto leaf = make_shared<Shape>(false);
		leaf->createShape(TOshapes[i]);
		leaf->measure();
		leaf->init();
		fluffs.push_back(leaf);
	}
};

void Cloud::render(
		shared_ptr<Program> prog, 
		shared_ptr<MatrixStack> model,
		vec3 translate,
		float scale) {
	model->pushMatrix();

	// Global scale and translate
	model->translate(translate);
	model->scale(scale);
	model->rotate(-1.57, vec3(0, 1, 0));

	model->pushMatrix();
	// Draw the Cloud
	for (const auto& fluff : fluffs) {
		setModel(prog, model);
		fluff->draw(prog);
	}
	model->popMatrix();

	model->popMatrix();
}
