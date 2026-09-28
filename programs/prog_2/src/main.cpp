/*
 * Example two meshes and two shaders (could also be used for Program 2)
 * includes modifications to shape and initGeom in preparation to load
 * multi shape objects 
 * CPE 471 Cal Poly Z. Wood + S. Sueda + I. Dunn
 */

#include <iostream>
#include <glad/glad.h>

#include "GLSL.h"
#include "Program.h"
#include "Shape.h"
#include "MatrixStack.h"
#include "WindowManager.h"

#include "Tree.h"
#include "Mountain.h"
#include "Tent.h"
#include "Flower.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader/tiny_obj_loader.h>

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Filenames for loaded obj files
#define TREE_FILE "/big_tree.obj"
#define MOUNTAIN_FILE "/mountain.obj"
#define TENT_FILE "/tent.obj"
// #define FLOWER_FILE "/daffodil.obj"
#define FLOWER_FILE "/cartoon_flower.obj"

using namespace std;
using namespace glm;

class Application : public EventCallbacks {

public:

	WindowManager * windowManager = nullptr;

	// Our shader program
	std::shared_ptr<Program> prog;

	// Tree 1 (Holds a Tree Mesh)
	Tree tree1;
	// Tree 2 (Holds a Tree Mesh)
	Tree tree2;
	// Tree 3 (Holds a Tree Mesh)
	Tree tree3;
	// Mountain (Holds a Mountain Mesh)
	Mountain mountain;
	// Tent (Holds a Tent Mesh)
	Tent tent;
	// Tent (Holds a Flower Mesh)
	Flower flower;

	// TODO: remove this if you don't use it
	// Example data that might be useful when trying to compute bounds on multi-shape
	// vec3 gMin;

	// Camera/Scene Updates
	float gTransX = 0.0f;
	float gTransY = -0.4f;
	float gTransZ = -2.4f;

	float targetRotation = 0;

	float tempDelta = 0;

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		// TODO: (change a and d to rotate around the flower) Move around in the scene (WASD)
		if (key == GLFW_KEY_A && action == GLFW_PRESS) {
			// gTransX -= 0.2;
			targetRotation -= 0.2;
		}
		if (key == GLFW_KEY_D && action == GLFW_PRESS) {
			// gTransX += 0.2;
			targetRotation += 0.2;
		}
		if (key == GLFW_KEY_W && action == GLFW_PRESS) {
			gTransY -= 0.2;
		}
		if (key == GLFW_KEY_S && action == GLFW_PRESS) {
			gTransY += 0.2;
		}

		// Zoom in the scene
		if (key == GLFW_KEY_EQUAL && mods & GLFW_MOD_SHIFT && action == GLFW_PRESS) {
			gTransZ += 0.2;
		}
		if (key == GLFW_KEY_MINUS && action == GLFW_PRESS) {
			gTransZ -= 0.2;
		}

		if (key == GLFW_KEY_X && action == GLFW_PRESS) {
			if (mods & GLFW_MOD_SHIFT) {
				tempDelta -= 0.2;
			}
			else{
				tempDelta += 0.2;
			}

			cout << "delta: " << tempDelta << endl;
		}

		if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		}
		if (key == GLFW_KEY_Z && action == GLFW_RELEASE) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		}
	}

	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		double posX, posY;

		if (action == GLFW_PRESS)
		{
			 glfwGetCursorPos(window, &posX, &posY);
			 cout << "Pos X " << posX <<  " Pos Y " << posY << endl;
		}
	}

	void resizeCallback(GLFWwindow *window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	void init(const std::string& resourceDirectory)
	{
		GLSL::checkVersion();

		// Set background color.
		glClearColor(.12f, .34f, .56f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);

		// Initialize the GLSL program.
		prog = make_shared<Program>();
		prog->setVerbose(true);
		prog->setShaderNames(resourceDirectory + "/simple_vert.glsl", resourceDirectory + "/simple_frag.glsl");
		prog->init();
		prog->addUniform("P");
		prog->addUniform("V");
		prog->addUniform("M");
		prog->addAttribute("vertPos");
		prog->addAttribute("vertNor");
	}

	void initGeom(const std::string& resourceDirectory) {
		//EXAMPLE set up to read one shape from one obj file - convert to read several
		// Initialize mesh
		// Load geometry
 		// Some obj files contain material information.We'll ignore them for this assignment.
 		vector<tinyobj::material_t> objMaterials;
 		string errStr;
 		vector<tinyobj::shape_t> TOshapes;
		bool rc = false;

		// =========================================================================

		// Load Tree 1
		if (!tinyobj::LoadObj(TOshapes, objMaterials, errStr, (resourceDirectory + TREE_FILE).c_str())) {
			cerr << errStr << endl;
		} 
		else {
			tree1 = Tree(TOshapes);
		}

		// =========================================================================

		// Load Tree 2
		if (!tinyobj::LoadObj(TOshapes, objMaterials, errStr, (resourceDirectory + TREE_FILE).c_str())) {
			cerr << errStr << endl;
		} 
		else {
			tree2 = Tree(TOshapes);
		}

		// =========================================================================

		// Load Tree 3
		if (!tinyobj::LoadObj(TOshapes, objMaterials, errStr, (resourceDirectory + TREE_FILE).c_str())) {
			cerr << errStr << endl;
		} 
		else {
			tree3 = Tree(TOshapes);
		}

		// =========================================================================

		// Load Mountain
		if (!tinyobj::LoadObj(TOshapes, objMaterials, errStr, (resourceDirectory + MOUNTAIN_FILE).c_str())) {
			cerr << errStr << endl;
		} 
		else {
			 mountain = Mountain(TOshapes);
		}

		// =========================================================================

		// Load Tent 
		if (!tinyobj::LoadObj(TOshapes, objMaterials, errStr, (resourceDirectory + TENT_FILE).c_str())) {
			cerr << errStr << endl;
		} 
		else {
			 tent = Tent(TOshapes);
		}

		// =========================================================================

		// Load Flower
		if (!tinyobj::LoadObj(TOshapes, objMaterials, errStr, (resourceDirectory + FLOWER_FILE).c_str())) {
			cerr << errStr << endl;
		} 
		else {
			 flower = Flower(TOshapes);
		}


		// TODO: remove this if you don't use it
		//read out information stored in the shape about its size - something like this...
		//then do something with that information.....
		// gMin.x = mountainMesh->min.x;
		// gMin.y = mountainMesh->min.y;
	}

	void render() {
		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		glViewport(0, 0, width, height);

		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Use the matrix stack for Lab 6
		float aspect = width/(float)height;

		// Create the matrix stacks - please leave these alone for now
		auto Projection = make_shared<MatrixStack>();
		auto View = make_shared<MatrixStack>();
		auto Model = make_shared<MatrixStack>();
		
		// =========================================================================

		// Apply perspective projection.
		Projection->pushMatrix();
		Projection->perspective(45.0f, aspect, 0.01f, 100.0f);

		// =========================================================================

		// View is global translation along negative z for now
		View->pushMatrix();
		View->loadIdentity();
		// TODO: remove changing the view directly. This should all be done in the 
		// Model for this project

		// =========================================================================

		// Draw mountain in the background 
		prog->bind();
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix()));


		// Draw hierarchical mesh using matrix stack
		Model->pushMatrix();

		// Global Scene Translation and Scale
		Model->loadIdentity();
		Model->translate(vec3(gTransX, gTransY, gTransZ));
		Model->scale(vec3(0.7, 0.7, 0.7));
		Model->rotate(targetRotation, vec3(0, 1, 0));
	
		// Scene rendering
		// tree1.render(prog, Model, vec3(-1, 0, 0), 0.05);
		// tree2.render(prog, Model, vec3(-0.75, 0, 0.25), 0.017);
		// tree3.render(prog, Model, vec3(1, 0.25, 0), 0.037);
		// mountain.render(prog, Model);
		// tent.render(prog, Model);

		flower.render(prog, Model, vec3(0, 0, 0), 1);

		Model->popMatrix();
		prog->unbind();

		// =========================================================================

		// Pop matrix stacks.
		Projection->popMatrix();
		View->popMatrix();
	}
};

int main(int argc, char *argv[])
{
	// Where the resources are loaded from
	std::string resourceDir = "../resources";

	if (argc >= 2)
	{
		resourceDir = argv[1];
	}

	Application *application = new Application();

	// Your main will always include a similar set up to establish your window
	// and GL context, etc.

	WindowManager *windowManager = new WindowManager();
	windowManager->init(640, 480);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	// This is the code that will likely change program to program as you
	// may need to initialize or set up different data and state

	application->init(resourceDir);
	application->initGeom(resourceDir);

	// Loop until the user closes the window.
	while (! glfwWindowShouldClose(windowManager->getHandle()))
	{
		// Render scene.
		application->render();

		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();
	}

	// Quit program.
	windowManager->shutdown();
	return 0;
}
