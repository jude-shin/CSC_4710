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

// TODO: make drawWall() a private function
void Maze::render(
		shared_ptr<Program> prog, 
		vector<shared_ptr<Shape>> wallObject, 
		shared_ptr<MatrixStack> Model) {

	const float MAZE_SCALE = 1.5f;

	const float WALL_LENGTH = 0.007f;
	const float WALL_HEIGHT = 0.01f;
	const float WALL_THICKNESS = 0.01f;

	const float WALL_Y_LOCATION = -1.5f;

	// Draws a wall whose unrotated long direction is the X axis.
	auto drawWall = [&](float x, float z, bool horizontal) {
		Model->pushMatrix();

		Model->scale(vec3(MAZE_SCALE));

		Model->translate(vec3(x, WALL_Y_LOCATION, z));
	
		// Rotate the walls depending on their orientation.
		if (horizontal) {
			Model->rotate(radians(90.0f), vec3(0.0f, 1.0f, 0.0f));
		}

		Model->scale(vec3(WALL_THICKNESS, WALL_HEIGHT, WALL_LENGTH));

		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
		for (auto& rock : wallObject) { rock->draw(prog); }

		Model->popMatrix();
	};

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			// Center of this maze cell in world coordinates.
			float cellX = 2.0f * x + 1.0f - width;
			float cellZ = 2.0f * y + 1.0f - height;

			// North/front wall: horizontal, extends along X.
			if (nodes[y][x] & NORTH) {
				drawWall(cellX, cellZ - 1.0f, true);
			}

			// West/left wall: vertical, extends along Z.
			if (nodes[y][x] & WEST) {
				drawWall(cellX - 1.0f, cellZ, false);
			}

			// Draw these only at the outside edge, avoiding duplicates.
			if (y == height - 1 && (nodes[y][x] & SOUTH)) {
				drawWall(cellX, cellZ + 1.0f, true);
			}

			if (x == width - 1 && (nodes[y][x] & EAST)) {
				drawWall(cellX + 1.0f, cellZ, false);
			}
		}
	}
}
