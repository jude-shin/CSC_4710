#include <cassert>
#include <algorithm> // min and max values
#include "Triangle.h"
#include "Point.h"
#include "Image.h"

using namespace std;

Triangle::Triangle(Point* a, Point* b, Point* c)
	: a(*a), b(*b), c(*c)
{}

void Triangle::draw_bounding_box(Image* image) {
	int min_x = std::min({a.get_x(), b.get_x(), c.get_x()});
	int max_x = std::max({a.get_x(), b.get_x(), c.get_x()});
	int min_y = std::min({a.get_y(), b.get_y(), c.get_y()});
	int max_y = std::max({a.get_y(), b.get_y(), c.get_y()});

	// Draw the bounding recangle
	for(int y = min_y; y <= max_y; ++y) {
		for(int x = min_x; x <= max_x; ++x) {
			if (x%2 == 0) {
				image->setPixel(x, y, 0, 0, 255);
			}
			else {
				image->setPixel(x, y, 255, 0, 0);
			}
		}
	}
}

void Triangle::draw_points(Image* image) {
	a.draw_points(image);
	b.draw_points(image);
	c.draw_points(image);
}

