#include "BambooWall.h"
#include "GLSL.h"
#include <assert.h>
#include <algorithm>
#include <glm/fwd.hpp>
#include "Program.h"

// One bamboo cluster spans roughly 2 maze units.
#define WALL_SPAN 2.0f
#define CLUSTER_ANGLE 20.0f

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

	int clusterSize = std::max(1, density);
	float spacing = WALL_SPAN / static_cast<float>(clusterSize);

	for (int i = 0; i < clusterSize; i++) {
		float offset = -WALL_SPAN / 2.0f
			+ spacing / 2.0f
			+ i * spacing;

		model->pushMatrix();

		model->translate(trans);

		if (flip) {
			model->rotate(radians(90.0f), vec3(0.0f, 1.0f, 0.0f));
		}

		model->translate(vec3(offset, 0.0f, 0.0f));

		// Give this cluster a stable, unique Y-axis rotation.
		float randomValue = clusterRandom(trans, i);
		float randomAngle = -CLUSTER_ANGLE + randomValue * (CLUSTER_ANGLE*2);
		model->rotate(radians(randomAngle), vec3(0.0f, 1.0f, 0.0f));

		model->scale(vec3(scale));

		bambooObject.render(shader, model);

		model->popMatrix();
	}
}

// Returns the same pseudo-random value in [0, 1) for the same wall/cluster.
float BambooWall::clusterRandom(vec3 wallPosition, int clusterIndex) {
	float value = sin(
		wallPosition.x * 12.9898f +
		wallPosition.z * 78.233f +
		static_cast<float>(clusterIndex) * 37.719f
	) * 43758.5453f;

	return value - floor(value);
}
