#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "Image.h"

// This allows you to skip the `std::` in front of C++ standard library
// functions. You can also say `using std::cout` to be more selective.
// You should never do this in a header file.
using namespace std;

int main(int argc, char **argv)
{
	if(argc < 4) {
		cout << "Usage: Lab01 <out_image_name>.png width height vax vay vbx vby vcx vcy" << endl;
		return 0;
	}
	// Output filename
	string filename(argv[1]);
	// Width of image
	int width = atoi(argv[2]);
	// Height of image
	int height = atoi(argv[3]);

	// Vertex a xy coordinates
	int v_a_x = atoi(argv[4]);
	int v_a_y = atoi(argv[5]);

	// Vertex b xy coordinates
	int v_b_x = atoi(argv[6]);
	int v_b_y = atoi(argv[7]);

	// Vertex c xy coordinates
	int v_c_x = atoi(argv[6]);
	int v_c_y = atoi(argv[7]);

	// Create the image. We're using a `shared_ptr`, a C++11 feature.
	auto image = make_shared<Image>(width, height);
	// Draw a rectangle
	for(int y = 10; y < 20; ++y) {
		for(int x = 20; x < 40; ++x) {
			unsigned char r = 255;
			unsigned char g = 0;
			unsigned char b = 0;
			image->setPixel(x, y, r, g, b);
		}
	}
	// Write image to file
	image->writeToFile(filename);
	return 0;
}
