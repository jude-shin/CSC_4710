#include <cassert>
#include <stdexcept>
#include "Point.h"

using namespace std;

Point::Point(float x, float y, float z, int r, int g, int b) 
	: x(x), y(y), z(z), r(r), g(g), b(b) {
	// Ensure the point is within a valid 3D workspace boundary
	if (r < 0 || r > 255) {
		throw std::invalid_argument("r value not within [0, 255]");
	}

	if (g < 0 || g > 255) {
		throw std::invalid_argument("g value not within [0, 255]");
	}

	if (b < 0 || b > 255) {
		throw std::invalid_argument("b value not within [0, 255]");
	}
}

Point::Point(const Point& other) {
	x = other.x;
	y = other.y;
	z = other.z;
	r = other.r;
	g = other.g;
	b = other.b;
}

Point::Point()
	: Point(0, 0, 0) {}

Point::Point(float x, float y, float z)
	: x(x), y(y), z(z), r(191), g(0), b(255) {}

float Point::get_x() const { return x; }
float Point::get_y() const { return y; }
float Point::get_z() const { return z; }
float Point::get_r() const { return r; }
float Point::get_g() const { return g; }
float Point::get_b() const { return b; }
