/**
 * Base code for particle systems lab
 */

#include <iostream>
#include <algorithm>
#include <glad/glad.h>

#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"
#include "Shape.h"
#include "Texture.h"
#include "WindowManager.h"
#include "particleSys.h"

 // value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

#define TREE_FILE "/big_tree.obj"
#define LEAF_COUNT 5

class Application : public EventCallbacks
{

public:

	WindowManager *windowManager = nullptr;

	// Our shader program for particles
	std::shared_ptr<Program> partProg;

	// Our shader program for meshes
	std::shared_ptr<Program> meshProg;

	//the partricle system
	particleSys *thePartSystem;

	// Shape to be used (from  file) - modify to support multiple
	// shared_ptr<Shape> myMesh;

	// TREE
  shared_ptr<Shape> trunkMesh;
  vector<shared_ptr<Shape>> leafMeshes;
  float leafDelta = 0;
  float trunkDelta = 0;

	// OpenGL handle to texture data used in particle
	shared_ptr<Texture> texture;

	bool keyToggles[256] = { false };

	//some particle variables
	float t = 0.0f; //reset in init
	float h = 0.01f;

	glm::vec3 g = glm::vec3(0.0f, -0.01f, 0.0f);
	float camRot;

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		keyToggles[key] = ! keyToggles[key];

		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}

		if (key == GLFW_KEY_A && action == GLFW_PRESS)
		{
			camRot -= 0.314f;
		}
		if (key == GLFW_KEY_D && action == GLFW_PRESS)
		{
			camRot += 0.314f;
		}
		if (key == GLFW_KEY_R && action == GLFW_PRESS)
		{
			thePartSystem->reSet();
		}
		if (key == GLFW_KEY_Z && action == GLFW_RELEASE) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		}
	}

	void scrollCallback(GLFWwindow* window, double deltaX, double deltaY)
	{
	}

	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		double posX, posY;

		if (action == GLFW_PRESS)
		{
			glfwGetCursorPos(window, &posX, &posY);
			cout << "Pos X " << posX << " Pos Y " << posY << endl;
		}
	}

	void cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
	{
	}

	void resizeCallback(GLFWwindow *window, int width, int height)
	{
		CHECKED_GL_CALL(glViewport(0, 0, width, height));
	}

	//code to set up the two shaders - a diffuse shader and texture mapping
	void init(const std::string& resourceDirectory)
	{
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		GLSL::checkVersion();

		// Set background color.
		CHECKED_GL_CALL(glClearColor(.12f, .34f, .56f, 1.0f));

		// Enable z-buffer test.
		CHECKED_GL_CALL(glEnable(GL_DEPTH_TEST));
		CHECKED_GL_CALL(glEnable(GL_BLEND));
		CHECKED_GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
		CHECKED_GL_CALL(glPointSize(24.0f));

		// Initialize the GLSL program.
		partProg = make_shared<Program>();
		partProg->setVerbose(true);
		partProg->setShaderNames(
			resourceDirectory + "/lab10_vert.glsl",
			resourceDirectory + "/lab10_frag.glsl");
		if (! partProg->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		partProg->addUniform("P");
		partProg->addUniform("M");
		partProg->addUniform("V");
		partProg->addUniform("pColor");
		partProg->addUniform("alphaTexture");
		partProg->addAttribute("vertPos");
		partProg->addAttribute("vertCol");

				// Initialize the GLSL program.
		meshProg = make_shared<Program>();
		meshProg->setVerbose(true);
		meshProg->setShaderNames(
			resourceDirectory + "/simple_vert.glsl",
			resourceDirectory + "/simple_frag.glsl");
		if (! meshProg->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		meshProg->addUniform("P");
		meshProg->addUniform("M");
		meshProg->addUniform("V");
		meshProg->addAttribute("vertPos");
		meshProg->addAttribute("vertNor");


		meshProg->addAttribute("vertTex");
		meshProg->addUniform("MatAmb");
		meshProg->addUniform("MatDif");
		meshProg->addUniform("MatSpec");
		meshProg->addUniform("MatShine");
		meshProg->addUniform("lightPos0");
		meshProg->addUniform("lightColor0");


		thePartSystem = new particleSys(vec3(0, 1, 0));
		thePartSystem->gpuSetup();
	}

	void initGeom(const std::string& resourceDirectory){
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
			trunkMesh = make_shared<Shape>(false);
			trunkMesh->createShape(TOshapes[0]);
			trunkMesh->measure();
			trunkMesh->init();

			for (int i = 1; i < LEAF_COUNT; ++i) {
				auto leaf = make_shared<Shape>(false);
				leaf->createShape(TOshapes[i]);
				leaf->measure();
				leaf->init();
				leafMeshes.push_back(leaf);
			}
		}
	}

	// Code to load in the texture
	void initTex(const std::string& resourceDirectory)
	{
		texture = make_shared<Texture>();
		texture->setFilename(resourceDirectory + "/alpha.bmp");
		texture->init();
		texture->setUnit(0);
		texture->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	}


	/* helper for sending top of the matrix strack to GPU */
	void setModel(shared_ptr<Program>& prog, shared_ptr<MatrixStack>M) {
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
	}
	
	/* helper function to set model trasnforms */
	void setModel(shared_ptr<Program>& curS, vec3 trans, float rotY, float rotX, float sc) {
		mat4 Trans = glm::translate( glm::mat4(1.0f), trans);
		mat4 RotX = glm::rotate( glm::mat4(1.0f), rotX, vec3(1, 0, 0));
		mat4 RotY = glm::rotate( glm::mat4(1.0f), rotY, vec3(0, 1, 0));
		mat4 ScaleS = glm::scale(glm::mat4(1.0f), vec3(sc));
		mat4 ctm = Trans*RotX*RotY*ScaleS;
		glUniformMatrix4fv(curS->getUniform("M"), 1, GL_FALSE, value_ptr(ctm));
	}

	void render()
	{
		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		glViewport(0, 0, width, height);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float aspect = width / (float) height;

		// Create the matrix stacks
		auto P = make_shared<MatrixStack>();
		auto V = make_shared<MatrixStack>();
		auto M = make_shared<MatrixStack>();

		// Apply perspective projection.
		P->pushMatrix();
		P->perspective(45.0f, aspect, 0.01f, 100.0f);

		// View is identity - for now
		V->pushMatrix();
		V->translate(vec3(0, -1, -5));
		V->rotate(camRot, vec3(0, 1, 0));

		M->pushMatrix();
		M->loadIdentity();

		// camera rotate
		thePartSystem->setCamera(V->topMatrix());

		meshProg->bind();
		CHECKED_GL_CALL(glUniformMatrix4fv(meshProg->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix())));
		CHECKED_GL_CALL(glUniformMatrix4fv(meshProg->getUniform("V"), 1, GL_FALSE, value_ptr(V->topMatrix())));

		// draw mesh 
		M->pushMatrix();
			M->loadIdentity();
			//"global" translate
			M->pushMatrix();

				glUniform3f(meshProg->getUniform("lightPos0"), 2.0f, 2.0f, 2.0f);
				glUniform3f(meshProg->getUniform("lightColor0"), 1.0f, 1.0f, 1.0f);
				glUniform3f(meshProg->getUniform("MatAmb"),  0.12f, 0.12f, 0.12f);
				glUniform3f(meshProg->getUniform("MatDif"),  1.0f,  1.0f,  1.0f);
				glUniform3f(meshProg->getUniform("MatSpec"), 0.25f, 0.25f, 0.25f);
				glUniform1f(meshProg->getUniform("MatShine"), 32.0f);

				// M->pushMatrix();

				// Global scale rotate and translates
				M->translate(vec3(0.0f, 0.0f, 0.0f));
				M->scale(vec3(0.1f, 0.1f, 0.1f));

				// Draw the Trunk (sways in the wind)
				M->rotate(trunkDelta, vec3(0, 0, 1));
				setModel(meshProg, M);
				trunkMesh->draw(meshProg);
				
				// Draw the Leaves (also swaying in the wind)
				for (const auto& leaf : leafMeshes) {
					M->pushMatrix();

					M->translate(vec3(leafDelta*0.07, leafDelta*0.1, leafDelta*0.3));
					setModel(meshProg, M);
					leaf->draw(meshProg);

					M->popMatrix();
				}

				// M->popMatrix();

				// Animate the leaves
				leafDelta = sin(glfwGetTime());
				trunkDelta = sin(glfwGetTime())*0.03;


				// myMesh->draw(meshProg);
			M->popMatrix();
		M->popMatrix();

		meshProg->unbind();

		// Draw
		partProg->bind();
		texture->bind(partProg->getUniform("alphaTexture"));
		CHECKED_GL_CALL(glUniformMatrix4fv(partProg->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix())));
		CHECKED_GL_CALL(glUniformMatrix4fv(partProg->getUniform("V"), 1, GL_FALSE, value_ptr(V->topMatrix())));
		CHECKED_GL_CALL(glUniformMatrix4fv(partProg->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix())));
		
		CHECKED_GL_CALL(glUniform3f(partProg->getUniform("pColor"), 0.9, 0.7, 0.7));
		
		thePartSystem->drawMe(partProg);
		thePartSystem->update();

		partProg->unbind();

		// Pop matrix stacks.
		V->popMatrix();
		M->popMatrix();
		P->popMatrix();
	}

};

int main(int argc, char **argv)
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
	windowManager->init(512, 512);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	// This is the code that will likely change program to program as you
	// may need to initialize or set up different data and state

	application->init(resourceDir);
	application->initTex(resourceDir);
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
