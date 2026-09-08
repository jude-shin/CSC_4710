/* Release code for program 1 CPE 471 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <cstring>
#include <assert.h>

#include "tiny_obj_loader.h"
#include "Image.h"
#include "Point.h"
#include "Triangle.h"

// This allows you to skip the `std::` in front of C++ standard library
// functions. You can also say `using std::cout` to be more selective.
// You should never do this in a header file.
using namespace std;

int g_width, g_height;

// Magic Numbers
int EXPECTED_ARGC = 5;

// =============================================================================

int parse_inputs(
		int argc, 
		char** argv, 
		string* input_filename,
		string* output_filename,
		int* width, 
		int* height,
		int* color_mode);

void resize_obj(std::vector<tinyobj::shape_t> &shapes);
int rasterize_all(vector<tinyobj::shape_t>* shapes, Image* image, int width, int height);
int scale_point(int wh, float xy);

// =============================================================================

int main(int argc, char **argv) {
	string input_filename;
	string output_filename;
	int width = 0;
	int height = 0;
	int color_mode = 0;
	
	// Get 
	if (parse_inputs(
			argc, 
			argv, 
			&input_filename, 
			&output_filename, 
			&width, 
			&height, 
			&color_mode) < 0) {
		cout << "USAGE: ./raster meshfile imagefile width height color_mode" << endl;
		cout << "EXAMPLE: ./raster ../resources/bunny.obj out.png 512 512 1" << endl;
		return 0;
	}

	// Set g_width and g_height appropriately!
	g_width = width;
	g_height = height;

	// Create an image
	auto image = make_shared<Image>(g_width, g_height);

	// Triangle buffer
	vector<unsigned int> triBuf;
	// Position buffer
	vector<float> posBuf;

	// Some obj files contain material information.
	// We'll ignore them for this assignment.
	vector<tinyobj::shape_t> shapes; // geometry
	vector<tinyobj::material_t> objMaterials; // material
	string errStr;

	bool rc = tinyobj::LoadObj(shapes, objMaterials, errStr, input_filename.c_str());
	/* error checking on read */
	if(!rc) {
		cerr << errStr << endl;
	} else {
		//keep this code to resize your object to be within -1 -> 1
		resize_obj(shapes); 
		posBuf = shapes[0].mesh.positions;
		triBuf = shapes[0].mesh.indices;
	}
	cout << "Number of vertices: " << posBuf.size()/3 << endl;
	cout << "Number of triangles: " << triBuf.size()/3 << endl;

	// Rasterize all the triangles
	if (rasterize_all(&shapes, image.get(), width, height) < 0) { 
		cout << "Unknown error rasterizing the traingles."  << endl;
		return -1;
	}

	//write out the image
	image->writeToFile(output_filename);

	return 0;
}

/*
 * Rasterizes all the traingles for all the shapes.
 * Args: 
 *	shapes: All the shapes (which have triangles) that we will rasterize, 
 *				parsed from the obj file
 *	image: the buffer we are writing our rasterized traingles to (which will 
 *				be saved to a .png later)
 *	width: width of the screen to draw to
 *	height: height of the screen to draw to
 * Returns: 0 upon completion, -1 if any error occured.
 */
int rasterize_all(vector<tinyobj::shape_t>* shapes, Image* image, int width, int height) {
	// Which dimension we should scale
	int min_dimension = min(width, height);

	// ZBuffer (default set to -2, which is outside the scaled range of [-1, 1])
	float zbuff[width][height];
	std::memset(zbuff, -2, sizeof(zbuff));

	// For each of the shapes, get their meshes
	for (tinyobj::shape_t shape : *shapes) {
		// for each traingle in the mesh
		for (unsigned int i = 0; i < shape.mesh.indices.size(); i+=3) {
			unsigned int idx = 0;
			// i: the index of one point in the triangle
			// i+1: the index of another point in the triangle
			// i+2: the index of the final point in the triangle

			// Parse out each x, y, and z for this point
			idx = shape.mesh.indices[i];
			int ax = scale_point(min_dimension, shape.mesh.positions[(idx*3)+0]);  // get the point from the index, adding 0 to the real index to get the x position
			int ay = scale_point(min_dimension, shape.mesh.positions[(idx*3)+1]);  // get the point from the index, adding 1 to the real index to get the x position

			float az = shape.mesh.positions[(idx*3)+2];  // get the point from the index, adding 2 to the real index to get the x position
			// If the point in the z buffer (ax, and ay) is greater than az, then abort this triangle and don't draw it. OTHERWISE, log it in the zbuffer and then continue
			if (az < zbuff[ax][ay]) {
				continue;
			}
			Point a = Point(ax, ay);

			idx = shape.mesh.indices[i+1];
			int bx = scale_point(min_dimension, shape.mesh.positions[(idx*3)+0]);  // get the point from the index, adding 0 to the real index to get the x position
			int by = scale_point(min_dimension, shape.mesh.positions[(idx*3)+1]);  // get the point from the index, adding 1 to the real index to get the x position

			float bz = shape.mesh.positions[(idx*3)+2];  // get the point from the index, adding 2 to the real index to get the x position
			// If the point in the z buffer (bx, and by) is greater than bz, then abort this triangle and don't draw it. OTHERWISE, log it in the zbuffer and then continue
			if (bz < zbuff[bx][by]) {
				continue;
			}
			Point b = Point(bx, by);

			idx = shape.mesh.indices[i+2];
			int cx = scale_point(min_dimension, shape.mesh.positions[(idx*3)+0]);  // get the point from the index, adding 0 to the real index to get the x position
			int cy = scale_point(min_dimension, shape.mesh.positions[(idx*3)+1]);  // get the point from the index, adding 1 to the real index to get the x position

			float cz = shape.mesh.positions[(idx*3)+2];  // get the point from the index, adding 2 to the real index to get the x position
			// If the point in the z buffer (cx, and cy) is greater than cz, then abort this triangle and don't draw it. OTHERWISE, log it in the zbuffer and then continue
			if (cz < zbuff[cx][cy]) {
				continue;
			}
			Point c = Point(cx, cy);

			// Rasterize that triangle!
			Triangle tri = Triangle(&a, &b, &c);
			tri.draw_triangle(image);

			// TODO: do something with that zbuffer later or something
		}
	}

	return 0;
}

// Scales the -1 to 1 floating point to the desired dimension (width or height)
int scale_point(int wh, float xy) {
	return (xy + 1.0) * 0.5 * (wh - 1);
}

/*
 * parses and validates cmd line inputs for the program
 */
int parse_inputs(
		int argc, 
		char** argv, 
		string* input_filename,
		string* output_filename,
		int* width, 
		int* height,
		int* color_mode) {

	// Check that the number of arguments provided is valid
	if(argc-1 < EXPECTED_ARGC) {
		cout << "WARNING: Too little arguments" << endl;
		return -1;
	}

	if(argc-1 > EXPECTED_ARGC) {
		cout << "WARNING: Too many arguments" << endl;
		return -1;
	}

	// Get the input .obj filename to rasterize
	*input_filename = argv[1];

	// Get the output filename
	*output_filename = argv[2];

	// Get the width and height
	*width = atoi(argv[3]);
	*height = atoi(argv[4]);

	*color_mode = atoi(argv[5]);

	// Validate the parsed inputs
	if (*color_mode != 1 && *color_mode != 2) {
		cout << "WARNING: Color Mode must be 1 or 2" << endl;
		return -1;
	}

	return 0;
}

/*
 * Helper function you will want all quarter
 * Given a vector of shapes which has already been read from an obj file
 * resize all vertices to the range [-1, 1]
 */
void resize_obj(std::vector<tinyobj::shape_t> &shapes) {
	float minX, minY, minZ;
	float maxX, maxY, maxZ;
	float scaleX, scaleY, scaleZ;
	float shiftX, shiftY, shiftZ;
	float epsilon = 0.001;

	minX = minY = minZ = 1.1754E+38F;
	maxX = maxY = maxZ = -1.1754E+38F;

	//Go through all vertices to determine min and max of each dimension
	for (size_t i = 0; i < shapes.size(); i++) {
		for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
			if(shapes[i].mesh.positions[3*v+0] < minX) minX = shapes[i].mesh.positions[3*v+0];
			if(shapes[i].mesh.positions[3*v+0] > maxX) maxX = shapes[i].mesh.positions[3*v+0];

			if(shapes[i].mesh.positions[3*v+1] < minY) minY = shapes[i].mesh.positions[3*v+1];
			if(shapes[i].mesh.positions[3*v+1] > maxY) maxY = shapes[i].mesh.positions[3*v+1];

			if(shapes[i].mesh.positions[3*v+2] < minZ) minZ = shapes[i].mesh.positions[3*v+2];
			if(shapes[i].mesh.positions[3*v+2] > maxZ) maxZ = shapes[i].mesh.positions[3*v+2];
		}
	}

	//From min and max compute necessary scale and shift for each dimension
	float maxExtent, xExtent, yExtent, zExtent;
	xExtent = maxX-minX;
	yExtent = maxY-minY;
	zExtent = maxZ-minZ;
	if (xExtent >= yExtent && xExtent >= zExtent) {
		maxExtent = xExtent;
	}
	if (yExtent >= xExtent && yExtent >= zExtent) {
		maxExtent = yExtent;
	}
	if (zExtent >= xExtent && zExtent >= yExtent) {
		maxExtent = zExtent;
	}
	scaleX = 2.0 /maxExtent;
	shiftX = minX + (xExtent/ 2.0);
	scaleY = 2.0 / maxExtent;
	shiftY = minY + (yExtent / 2.0);
	scaleZ = 2.0/ maxExtent;
	shiftZ = minZ + (zExtent)/2.0;

	//Go through all verticies shift and scale them
	for (size_t i = 0; i < shapes.size(); i++) {
		for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
			shapes[i].mesh.positions[3*v+0] = (shapes[i].mesh.positions[3*v+0] - shiftX) * scaleX;
			assert(shapes[i].mesh.positions[3*v+0] >= -1.0 - epsilon);
			assert(shapes[i].mesh.positions[3*v+0] <= 1.0 + epsilon);
			shapes[i].mesh.positions[3*v+1] = (shapes[i].mesh.positions[3*v+1] - shiftY) * scaleY;
			assert(shapes[i].mesh.positions[3*v+1] >= -1.0 - epsilon);
			assert(shapes[i].mesh.positions[3*v+1] <= 1.0 + epsilon);
			shapes[i].mesh.positions[3*v+2] = (shapes[i].mesh.positions[3*v+2] - shiftZ) * scaleZ;
			assert(shapes[i].mesh.positions[3*v+2] >= -1.0 - epsilon);
			assert(shapes[i].mesh.positions[3*v+2] <= 1.0 + epsilon);
		}
	}
}

