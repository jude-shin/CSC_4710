#include "BambooWall.h"
#include "GLSL.h"
#include <assert.h>
#include <algorithm>
#include <glm/fwd.hpp>
#include "Program.h"

using namespace std;
using namespace glm;

BambooWall::BambooWall() {}

BambooWall::BambooWall(
		const std::string & texFile, 
		const std::string & objFile, 
		int density){

	// Init the single bamboo object
	bambooObject = MyObject(texFile, objFile);
	this->density = density;
}

void BambooWall::render(
		shared_ptr<Program> shader,
		shared_ptr<MatrixStack> model,
		vec3 trans,
		float scale,
		bool flip) {

	// One bamboo cluster spans roughly 2 maze units.
	const float WALL_SPAN = 2.0f;

	// Avoid division by zero if an invalid density was provided.
	int clusterSize = std::max(1, density);

	// Each bamboo stalk gets an evenly spaced position in the cluster.
	float spacing = WALL_SPAN / static_cast<float>(clusterSize);

	for (int i = 0; i < clusterSize; i++) {
		float offset = -WALL_SPAN / 2.0f
			+ spacing / 2.0f
			+ i * spacing;

		model->pushMatrix();

		// Move to this maze wall's center.
		model->translate(trans);

		// By default, the bamboo cluster extends along X.
		// Rotate it to extend along Z for the other wall orientation.
		if (flip) {
			model->rotate(radians(90.0f), vec3(0.0f, 1.0f, 0.0f));
		}

		// Place this individual bamboo object within the cluster.
		model->translate(vec3(offset, 0.0f, 0.0f));

		// Scale only this individual bamboo model uniformly.
		model->scale(vec3(scale));

		bambooObject.render(shader, model);

		model->popMatrix();
	}
}
