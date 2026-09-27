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

#define DEG_90 90.0*(M_PI/180.0)

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
		vec3 pos,
		float scale) {
	model->pushMatrix();

	model->scale(vec3(scale, scale, scale));
	model->translate(pos);
	// model->rotate(-DEG_90, vec3(1, 0, 0));

	// Draw the Trunk
	setModel(prog, model);
	trunkMesh->draw(prog);
	
	// Draw the Leaves
	for (const auto& leaf : leafMeshes) {
		model->pushMatrix();

		model->translate(vec3(leafDelta*0.5, leafDelta*0.3, leafDelta*0.2));
		setModel(prog, model);
		leaf->draw(prog);

		model->popMatrix();
	}

	model->popMatrix();

	// Animate the leaves
	leafDelta = sin(glfwGetTime());
}
