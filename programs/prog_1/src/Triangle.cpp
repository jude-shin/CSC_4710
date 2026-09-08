#include <cassert>
#include <algorithm>
#include "Triangle.h"
#include "Point.h"
#include "Image.h"

using namespace std;

Triangle::Triangle(Point* a, Point* b, Point* c)
	: a(*a), b(*b), c(*c) {}

void Triangle::draw_triangle(Image* image) {
	// I am too lazy to make a rectangle class. Maybe do this a different time.
	int min_x = std::min({a.get_x(), b.get_x(), c.get_x()});
	int max_x = std::max({a.get_x(), b.get_x(), c.get_x()});
	int min_y = std::min({a.get_y(), b.get_y(), c.get_y()});
	int max_y = std::max({a.get_y(), b.get_y(), c.get_y()});

	// Do this math before hand so it is faster
	// barycentric
	float	_a = (b.get_x()-a.get_x());
	float _b = (c.get_x()-a.get_x());
	float _c = (b.get_y()-a.get_y());
	float _d = (c.get_y()-a.get_y());
	float deter = (_a*_d)-(_b*_c);

	// Colors
	int ar = a.get_r();
	int ag = a.get_g();
	int ab = a.get_b();

	int br = b.get_r();
	int bg = b.get_g();
	int bb = b.get_b();

	int cr = c.get_r();
	int cg = c.get_g();
	int cb = c.get_b();

	// =========================================================

	// For every pixel in the bounding box
	for(int y = min_y; y <= max_y; ++y) {
		for(int x = min_x; x <= max_x; ++x) {
			// =========================================================
			float _e = (x-a.get_x()); // Toss in that other constant
			float _f = (y-a.get_y()); // Toss that in some other constant

			// Barycentric coordinates
			float beta = ((_e*_d)-(_f*_b))/deter;
			float gamma = ((_a*_f)-(_c*_e))/deter;
			float alpha = (1-beta-gamma);

			// =========================================================
	
			// IFF this is true, then the point is not within the triangle.
			// Just break early and start computing the next point.
			if ((alpha < 0) || (alpha > 1) ||
					(beta < 0) || (beta > 1)||
					(gamma < 0) || (gamma > 1)) {
				continue;
			}

			// TODO: blend it differently based on the new criteria

			// Blend the colors based on the points and the alpha, beta, gamma vals
			int blend_r = static_cast<int>(alpha*ar + beta*br + gamma*cr);
			int blend_g = static_cast<int>(alpha*ag + beta*bg + gamma*cg);
			int blend_b = static_cast<int>(alpha*ab + beta*bb + gamma*cb);

			image->setPixel(x, y, a.get_r(), a.get_g(), a.get_b());
		}
	}
}
