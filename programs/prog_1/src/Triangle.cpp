#include <cassert>
#include <algorithm>
#include <iostream>
#include "Triangle.h"
#include "Point.h"
#include "Image.h"

using namespace std;

Triangle::Triangle(Point* a, Point* b, Point* c)
	: a(*a), b(*b), c(*c) {}

int Triangle::draw_triangle(Image* image, vector<float>* zbuff, int width, int height, Mode mode) {
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


			// Calculate depth values
			float cur_pixel_z = 
				alpha * a.get_z() + 
				beta * b.get_z() + 
				gamma * c.get_z();
			int z_idx = y * width + x;
			float depth = (cur_pixel_z+1)*0.5;

			int red, green, blue = 0;
			switch(mode) {
				case Mode::DEPTH: 
					red = depth*DEPTH_R;
					green = depth*DEPTH_G;
					red = depth*DEPTH_B;
					break;

				case Mode::SPECIAL:
					// If all barycentric weights are greater than or equal to 0.2: cyan
					if (alpha > SPECIAL_CYAN_THRESH && 
							beta > SPECIAL_CYAN_THRESH && 
							gamma > SPECIAL_CYAN_THRESH) {
						red = SPECIAL_CYAN_R;
						green = SPECIAL_CYAN_G;
						blue = SPECIAL_CYAN_B;
						break;
					}

					// Pixels wehre the minimum barycentric weight falls within the range
					// (0.05, 0.2) will be yellow
					if (std::min({alpha, beta, gamma}) > SPECIAL_YELLOW_THRESH && 
							std::min({alpha, beta, gamma}) < SPECIAL_CYAN_THRESH) {
						red = SPECIAL_YELLOW_R;
						green = SPECIAL_YELLOW_G;
						blue = SPECIAL_YELLOW_B;
						break;
					}

					// Oherwise, just color it magenta
					red = SPECIAL_MAGENTA_R;
					green = SPECIAL_MAGENTA_G;
					blue = SPECIAL_MAGENTA_B;
					break;

				default:
					cout << "Unknown Mode Provided" << endl;
					return -1;
			}


			if (cur_pixel_z > (*zbuff)[z_idx]) {
				(*zbuff)[z_idx] = cur_pixel_z;
				image->setPixel(x, y, red, green, blue);
			}

			// // Blend the colors based on the points and the alpha, beta, gamma vals
			// int blend_r = static_cast<int>(alpha*ar + beta*br + gamma*cr);
			// int blend_g = static_cast<int>(alpha*ag + beta*bg + gamma*cg);
			// int blend_b = static_cast<int>(alpha*ab + beta*bb + gamma*cb);

		}
	}

	return 0;
}
