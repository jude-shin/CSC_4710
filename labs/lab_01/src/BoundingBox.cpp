#include <cassert>
#include <algorithm> // min and max values
#include "BoundingBox.h"

#include "Image.h"

using namespace std;

BoundingBox::BoundingBox(int v_a_x, int v_a_y, int v_b_x, int v_b_y, int v_c_x, int v_c_y) {
	min_x = std::min({v_a_x, v_b_x, v_c_x});
	max_x = std::max({v_a_x, v_b_x, v_c_x});
	min_y = std::min({v_a_y, v_b_y, v_c_y});
	max_y = std::max({v_a_y, v_b_y, v_c_y});
}

void BoundingBox::draw(Image* image) {
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

