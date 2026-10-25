// Maze.cpp
#include "Maze.h"

#include <algorithm>
#include <array>
#include <stdexcept>

using namespace std;

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

vector<vector<bool>> Maze::render() {
	// true means wall; false means traversable space.
	vector<vector<bool>> walls(
			2 * height + 1,
			vector<bool>(2 * width + 1, true)
			);

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int renderX = 2 * x + 1;
			int renderY = 2 * y + 1;

			// The center of every cell is open.
			walls[renderY][renderX] = false;

			// Remove a rendered wall when the maze cell has no matching wall.
			if (!(nodes[y][x] & NORTH)) {
				walls[renderY - 1][renderX] = false;
			}
			if (!(nodes[y][x] & EAST)) {
				walls[renderY][renderX + 1] = false;
			}
			if (!(nodes[y][x] & SOUTH)) {
				walls[renderY + 1][renderX] = false;
			}
			if (!(nodes[y][x] & WEST)) {
				walls[renderY][renderX - 1] = false;
			}
		}
	}

	return walls;
}
