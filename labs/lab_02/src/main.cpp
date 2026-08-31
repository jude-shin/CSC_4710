#include <iostream>
#include <string>
#include <memory>
#include <format>
#include "Image.h"
#include "Triangle.h"
#include "Point.h"

// This allows you to skip the `std::` in front of C++ standard library
// functions. You can also say `using std::cout` to be more selective.
// You should never do this in a header file.
using namespace std;

int parse_inputs(
		int argc, 
		char** argv, 
		string* filename,
		int* width, 
		int* height, 
		Point* pa, 
		Point* pb, 
		Point* pc);

int main(int argc, char **argv)
{
	// Output filename
	string filename;
	int width = 0;
	int height = 0;
	Point pa;
	Point pb;
	Point pc;

	if (parse_inputs(
				argc,
				argv, 
				&filename,
				&width, 
				&height, 
				&pa,
				&pb, 
				&pc) < 0) {
		cout << "Usage: Lab02 <out_image_name>.png width height vax vay vbx vby vcx vcy" << endl;
		return -1;
	}

	// Create the image. We're using a `shared_ptr`, a C++11 feature.
	auto image = make_shared<Image>(width, height);

	// Create the Triangle
	Triangle tri = Triangle(&pa, &pb, &pc);
	// tri.draw_bounding_box(image.get());

	// Draw a traingle's verticies
	// tri.draw_points(image.get());

	// Draw the triangle
	tri.draw_triangle(image.get());

	// Write image to file
	image->writeToFile(filename);

	return 0;
}

int parse_inputs(
		int argc, 
		char** argv, 
		string* filename,
		int* width, 
		int* height, 
		Point* pa, 
		Point* pb, 
		Point* pc) {

	// Check that the number of arguments provided is valid
	if(argc < 10) {
		cout << "Too little arguments" << endl;
		return -1;
	}

	if(argc > 10) {
		cout << "Too many arguments" << endl;
		return -1;
	}

	// Get the output filename
	*filename = argv[1];
	
	// Get the width and height
	*width = atoi(argv[2]);
	*height = atoi(argv[3]);

	// Assign all points, ensuring they're within the bounds of the width and height
	// Vertex a xy coordinates
	int vax = atoi(argv[4]);
	if (vax < 0 || *width <= vax) {
		cout << "vax out of bounds" << endl;
		return -1;
	}

	int vay = atoi(argv[5]);
	if (vay < 0 || *height <= vay) {
		cout << "vay out of bounds" << endl;
		return -1;
	}

	// Vertex b xy coordinates
	int vbx = atoi(argv[6]);
	if (vbx < 0 || *width <= vbx) {
		cout << "vbx out of bounds" << endl;
		return -1;
	}

	int vby = atoi(argv[7]);
	if (vby < 0 || *height <= vby) {
		cout << "vby out of bounds" << endl;
		return -1;
	}

	// Vertex c xy coordinates
	int vcx = atoi(argv[8]);
	if (vcx < 0 || *width <= vcx) {
		cout << "vcx out of bounds" << endl;
		return -1;
	}

	int vcy = atoi(argv[9]);
	if (vcy < 0 || *height <= vcy) {
		cout << "vcy out of bounds" << endl;
		return -1;
	}

	// Defaults for Colors
	int a_color_r = 255;
	int a_color_g = 0;
	int a_color_b = 0;

	int b_color_r = 0;
	int b_color_g = 255;
	int b_color_b = 0;

	int c_color_r = 0;
	int c_color_g = 0;
	int c_color_b = 255;

	// Create those points
	*pa = Point(vax, vay, a_color_r, a_color_g, a_color_b);
	*pb = Point(vbx, vby, b_color_r, b_color_g, b_color_b);
	*pc = Point(vcx, vcy, c_color_r, c_color_g, c_color_b);

	return 0;
}
