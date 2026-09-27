#include "Tree.h"
#include <vector>
#include <assert.h>

#include "GLSL.h"
#include "Program.h"

Tree::Tree() {}

Tree::Tree(std::vector<tinyobj::shape_t>& TOshapes) 
{
	mesh = std::make_shared<Shape>(false);
	mesh->createShape(TOshapes[0]);
	mesh->measure();
	mesh->init();
};

std::shared_ptr<Shape> Tree::getMesh() {
	return mesh;
}
