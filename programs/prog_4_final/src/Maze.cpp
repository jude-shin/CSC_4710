// Maze.cpp
#include "Maze.h"
#include "BambooWall.h"
#include <algorithm>
#include <array>
#include <stdexcept>
#include <random>
#include <glad/glad.h>

#define WALL_Y_LOCATION -1.2f
#define SCALE 0.005

using namespace std;
using namespace glm;

// Wall-bit values
constexpr int NORTH = 1;
constexpr int EAST  = 2;
constexpr int SOUTH = 4;
constexpr int WEST  = 8;
constexpr int ALL_WALLS = NORTH | EAST | SOUTH | WEST;

// Collision is intentionally a little wider than a zero-thickness line so the
// camera cannot pass between or visibly into bamboo clusters.
constexpr float WALL_HALF_THICKNESS = 0.15f;
constexpr float WALL_COLLISION_BOTTOM = WALL_Y_LOCATION;
constexpr float WALL_COLLISION_TOP = 0.8f;

static bool pointHitsWall(
		const vec3 &position,
		float radius,
		float minX, float maxX,
		float minZ, float maxZ) {
	return position.x >= minX - radius && position.x <= maxX + radius &&
		position.z >= minZ - radius && position.z <= maxZ + radius;
}

Maze::Maze(int w, int h) : width(w), height(h) {
	if (w <= 0 || h <= 0) {
		throw invalid_argument("Maze width and height must be positive.");
	}

	// Every cell initially has all four walls.
	nodes = vector<vector<int>>(
			height,
			vector<int>(width, ALL_WALLS)
			);
}

void Maze::generateRecursiveBacktracker(int seed) {
	// Reset maze before generating a new one.
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			nodes[y][x] = ALL_WALLS;
		}
	}

	mt19937 generator(seed);
	generateRecursive(0, 0, generator);
}

void Maze::generateRecursive(int x, int y, mt19937 &generator) {
	// A cell is visited once one of its walls has been removed.
	array<Point, 4> directions = {{
		{0, -1},  // north
		{1, 0},   // east
		{0, 1},   // south
		{-1, 0}   // west
	}};

	shuffle(directions.begin(), directions.end(), generator);

	for (const Point &direction : directions) {
		int nextX = x + direction.x;
		int nextY = y + direction.y;

		// Ignore directions outside the maze.
		if (nextX < 0 || nextX >= width || nextY < 0 || nextY >= height) {
			continue;
		}

		// An untouched cell still has all four walls.
		if (nodes[nextY][nextX] != ALL_WALLS) {
			continue;
		}

		// Remove the shared wall between current and next cell.
		if (direction.x == 1) {
			nodes[y][x] &= ~EAST;
			nodes[nextY][nextX] &= ~WEST;
		} else if (direction.x == -1) {
			nodes[y][x] &= ~WEST;
			nodes[nextY][nextX] &= ~EAST;
		} else if (direction.y == 1) {
			nodes[y][x] &= ~SOUTH;
			nodes[nextY][nextX] &= ~NORTH;
		} else {
			nodes[y][x] &= ~NORTH;
			nodes[nextY][nextX] &= ~SOUTH;
		}

		generateRecursive(nextX, nextY, generator);
	}
}

bool Maze::collides(const vec3 &position, float radius, float mazeScale) const {
	// The walls only block the camera while it is within the bamboo height.
	// Moving above the top lets the player fly over the maze.
	if (position.y < WALL_COLLISION_BOTTOM || position.y > WALL_COLLISION_TOP) {
		return false;
	}

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			float cellX = (2.0f * x + 1.0f - width) * mazeScale;
			float cellZ = (2.0f * y + 1.0f - height) * mazeScale;
			float halfCell = mazeScale;
			float halfWall = WALL_HALF_THICKNESS * mazeScale;

			// Use the same wall selection and locations as render(). Each test is
			// one full maze-wall rectangle, never an individual bamboo cluster.
			if ((nodes[y][x] & NORTH) &&
					pointHitsWall(position, radius,
						cellX - halfCell, cellX + halfCell,
						cellZ - halfCell - halfWall, cellZ - halfCell + halfWall)) {
				return true;
			}
			if ((nodes[y][x] & WEST) &&
					pointHitsWall(position, radius,
						cellX - halfCell - halfWall, cellX - halfCell + halfWall,
						cellZ - halfCell, cellZ + halfCell)) {
				return true;
			}
			if (y == height - 1 && (nodes[y][x] & SOUTH) &&
					pointHitsWall(position, radius,
						cellX - halfCell, cellX + halfCell,
						cellZ + halfCell - halfWall, cellZ + halfCell + halfWall)) {
				return true;
			}
			if (x == width - 1 && (nodes[y][x] & EAST) &&
					pointHitsWall(position, radius,
						cellX + halfCell - halfWall, cellX + halfCell + halfWall,
						cellZ - halfCell, cellZ + halfCell)) {
				return true;
			}
		}
	}

	return false;
}

// TODO: make drawWall() a private function
void Maze::render(
		shared_ptr<Program> shader, 
		BambooWall bambooWall,
		shared_ptr<MatrixStack> model,
		float mazeScale) {

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			// Center of this maze cell in world coordinates.
			float cellX = 2.0f * x + 1.0f - width;
			float cellZ = 2.0f * y + 1.0f - height;

			// North/front wall: extends along X.
			if (nodes[y][x] & NORTH) {
				bambooWall.render(
						shader,
						model,
						vec3(cellX * mazeScale,
							WALL_Y_LOCATION * mazeScale,
							(cellZ - 1.0f) * mazeScale),
						SCALE,
						false);
			}

			// West/left wall: extends along Z.
			if (nodes[y][x] & WEST) {
				bambooWall.render(
						shader,
						model,
						vec3((cellX - 1.0f) * mazeScale,
							WALL_Y_LOCATION * mazeScale,
							cellZ * mazeScale),
						SCALE,
						true);
			}

			// South/back wall: extends along X.
			if (y == height - 1 && (nodes[y][x] & SOUTH)) {
				bambooWall.render(
						shader,
						model,
						vec3(cellX * mazeScale,
							WALL_Y_LOCATION * mazeScale,
							(cellZ + 1.0f) * mazeScale),
						SCALE,
						false);
			}

			// East/right wall: extends along Z.
			if (x == width - 1 && (nodes[y][x] & EAST)) {
				bambooWall.render(
						shader,
						model,
						vec3((cellX + 1.0f) * mazeScale,
							WALL_Y_LOCATION * mazeScale,
							cellZ * mazeScale),
						SCALE,
						true);
			}
		}
	}
}
