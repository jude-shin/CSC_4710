#include "BambooWall.h"
#include "GLSL.h"
#include <GLFW/glfw3.h>
#include <assert.h>
#include <algorithm>
#include <glm/fwd.hpp>
#include "Program.h"

// One bamboo cluster spans roughly 2 maze units.
#define WALL_SPAN 2.0f
#define CLUSTER_ANGLE 20.0f

#define WIND_MAX_ANGLE 6.0f
#define WIND_SPEED 1.25f

#define BAMBOO_OBJ "/bamboo.obj"
#define BAMBOO_TEX "/bamboo.jpg"

using namespace std;
using namespace glm;

BambooWall::BambooWall() {}

BambooWall::BambooWall(const std::string & resourcesDir, int density) {

	// Init the single bamboo object
	bambooObject = MyObject(resourcesDir+BAMBOO_TEX, resourcesDir+BAMBOO_OBJ);
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
	float time = static_cast<float>(glfwGetTime());
	float windAngle = WIND_MAX_ANGLE * sin(time * WIND_SPEED);

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
		
		vec3 windAxis = flip ? vec3(-1.0f, 0.0f, 0.0f) : vec3(0.0f, 0.0f, 1.0f);
		model->rotate(radians(windAngle), windAxis);
		
		// Each cluster retains its unique fixed orientation.
		float randomValue = clusterRandom(trans, i);
		float randomAngle = -CLUSTER_ANGLE + randomValue * (CLUSTER_ANGLE * 2);
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
