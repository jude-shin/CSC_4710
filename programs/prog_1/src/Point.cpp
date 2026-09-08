#include <cassert>
#include <stdexcept>
#include "Point.h"
#include "Image.h"

using namespace std;

Point::Point(int x, int y, int r, int g, int b) 
	: x(x), y(y), r(r), g(g), b(b) {
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
	r = other.r;
	g = other.g;
	b = other.b;
}

Point::Point()
	: Point(0, 0) {
}

Point::Point(int x, int y)
	: x(x), y(y), r(191), g(0), b(255) {
}


int Point::get_x() const { return x; }
int Point::get_y() const { return y; }
int Point::get_r() const { return r; }
int Point::get_g() const { return g; }
int Point::get_b() const { return b; }


void Point::draw_points(Image* image) {
	image->setPixel(x, y, r, g, b);
}
