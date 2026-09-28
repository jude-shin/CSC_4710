#include "Tree.h"
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

#define LEAF_COUNT 5

Tree::Tree() {}

Tree::Tree(vector<tinyobj::shape_t>& TOshapes) 
{
	trunkMesh = make_shared<Shape>(false);
	trunkMesh->createShape(TOshapes[0]);
	trunkMesh->measure();
	trunkMesh->init();

	for (int i = 1; i < LEAF_COUNT; ++i) {
		auto leaf = make_shared<Shape>(false);
		leaf->createShape(TOshapes[i]);
		leaf->measure();
		leaf->init();
		leafMeshes.push_back(leaf);
	}
};

void Tree::render(
		shared_ptr<Program> prog, 
		shared_ptr<MatrixStack> model,
		vec3 translate,
		float scale) {
	model->pushMatrix();

	// Global scale rotate and translates
	model->translate(translate);
	model->scale(scale);

	// Draw the Trunk (sways in the wind)
	model->rotate(trunkDelta, vec3(0, 0, 1));
	setModel(prog, model);
	trunkMesh->draw(prog);
	
	// Draw the Leaves (also swaying in the wind)
	for (const auto& leaf : leafMeshes) {
		model->pushMatrix();

		model->translate(vec3(leafDelta*0.07, leafDelta*0.1, leafDelta*0.3));
		setModel(prog, model);
		leaf->draw(prog);

		model->popMatrix();
	}

	model->popMatrix();

	// Animate the leaves
	leafDelta = sin(glfwGetTime());
	trunkDelta = sin(glfwGetTime())*0.03;
}
