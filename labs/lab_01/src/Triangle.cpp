#include <cassert>
#include <algorithm> // min and max values
#include "Triangle.h"

#include "Image.h"

using namespace std;

Triangle::Triangle(int vax, int vay, int vbx, int vby, int vcx, int vcy) {
	v_a_x = vax;
	v_a_y = vay;
	v_b_x = vbx; 
	v_b_y = vby; 
	v_c_x = vcx; 
	v_c_y = vcy;
}

void Triangle::draw_bounding_box(Image* image) {
	int min_x = std::min({v_a_x, v_b_x, v_c_x});
	int max_x = std::max({v_a_x, v_b_x, v_c_x});
	int min_y = std::min({v_a_y, v_b_y, v_c_y});
	int max_y = std::max({v_a_y, v_b_y, v_c_y});

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

