#include <iostream>
#include <string>
#include <memory>
#include "Image.h"
#include "Triangle.h"

// This allows you to skip the `std::` in front of C++ standard library
// functions. You can also say `using std::cout` to be more selective.
// You should never do this in a header file.
using namespace std;

int validate_inputs(int argc, char** argv);

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
	int v_c_x = atoi(argv[8]);
	int v_c_y = atoi(argv[9]);

	if (validate_inputs(argc, argv) != 0) {
		cout << "inputs are not valid" << endl;
		return -1;
	}

	// Create the image. We're using a `shared_ptr`, a C++11 feature.
	auto image = make_shared<Image>(width, height);

	Triangle tri = Triangle(v_a_x, v_a_y, v_b_x, v_b_y, v_c_x, v_c_y);
	tri.draw_bounding_box(image.get());

	// Draw a traingle from the verticies
	// Sanity check to draw the three values
	image->setPixel(v_a_x, v_a_y, 0, 255, 0);
	image->setPixel(v_b_x, v_b_y, 0, 255, 0);
	image->setPixel(v_c_x, v_c_y, 0, 255, 0);

	// Write image to file
	image->writeToFile(filename);
	return 0;
}

int validate_inputs(int argc, char** argv) {
	// TODO: check that the verticies are within the bounds?
	return 0;
}


