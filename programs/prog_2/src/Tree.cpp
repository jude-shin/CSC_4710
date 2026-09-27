#include "Tree.h"
#include <vector>
#include <assert.h>
#include <glm/fwd.hpp>

#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"

using namespace std;

Tree::Tree() {}

Tree::Tree(vector<tinyobj::shape_t>& TOshapes) 
{
	mesh = make_shared<Shape>(false);
	mesh->createShape(TOshapes[0]);
	mesh->measure();
	mesh->init();
};

void Tree::render(shared_ptr<Program> prog, shared_ptr<MatrixStack>& model) {
	model->pushMatrix();
	model->rotate(0.5, glm::vec3(1, 0, 0));
	setModel(prog, model);
	mesh->draw(prog);
	model->popMatrix();

}
