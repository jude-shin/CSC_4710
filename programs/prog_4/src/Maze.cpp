// Maze.cpp
#include "Maze.h"

#include <algorithm>
#include <array>
#include <stdexcept>
#include <random>
#include <glad/glad.h>

using namespace std;
using namespace glm;

// Wall-bit values
constexpr int NORTH = 1;
constexpr int EAST  = 2;
constexpr int SOUTH = 4;
constexpr int WEST  = 8;
constexpr int ALL_WALLS = NORTH | EAST | SOUTH | WEST;

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

// This is done before the render function is called (you don't have to worry,
// everything works):
// tex->bind(curS->getUniform("Texture0"));
// initModel(vec3(0.0f, -1.0f, -5.0f), 1.57f, 0.0f, 0.01f, curS);
void Maze::render(
		shared_ptr<Program> prog, 
		shared_ptr<Shape> wallObject, 
		shared_ptr<MatrixStack> Model) {

	// true means wall; false means traversable space.
	vector<vector<bool>> walls(
			2 * height + 1,
			vector<bool>(2 * width + 1, true)
			);

	for (int y = 0; y < 2 * height + 1; y++) {
		for (int x = 0; x < 2 * width + 1; x++) {
			if (walls[y][x]) {
				Model->pushMatrix();

				// Centers the maze around the origin.
				Model->translate(vec3(
					static_cast<float>(x - width),
					0.0f,
					static_cast<float>(y - height)
				));

				// Adjust these dimensions to fit your wall mesh.
				Model->scale(vec3(0.5f, 1.0f, 0.5f));

				wallObject->draw(prog);

				Model->popMatrix();
			}
		}
	}
}
