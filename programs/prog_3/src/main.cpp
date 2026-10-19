/*
 * Program 3 base code - includes modifications to shape and initGeom in preparation to load
 * multi shape objects 
 * CSC 471 Cal Poly Z. Wood + S. Sueda + I. Dunn
 */

#include <iostream>
#include <glad/glad.h>

#include "GLSL.h"
#include "Program.h"
#include "Shape.h"
#include "MatrixStack.h"
#include "WindowManager.h"
#include "Texture.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader/tiny_obj_loader.h>

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

#define BUNNY_OBJ "/bunny.obj"
#define SPHERE_OBJ "/sphere.obj"
#define DUMMY_OBJ "/dummy.obj"
#define GROUND_TEX "/flower_grass.jpg"

#define NUM_DUMMIES_X 3
#define NUM_DUMMIES_Y 4
#define NUM_BUNNIES_X 3
#define NUM_BUNNIES_Y 3

class Application : public EventCallbacks
{

	public:

		WindowManager * windowManager = nullptr;

		std::shared_ptr<Program> prog;		// Blinn-Phong shader program
		std::shared_ptr<Program> texProg;	// Texture shader program

		// Loaded Geometries
		shared_ptr<Shape> sphereObject;
		shared_ptr<Shape> bunnyObject; 
		vector<shared_ptr<Shape>> dummyObject; 

		// Global data for ground plane
		// Direct load constant defined CPU data to GPU (not obj)
		GLuint GrndBuffObj, GrndNorBuffObj, GrndTexBuffObj, GIndxBuffObj;
		int g_GiboLen;
		// Ground VAO
		GLuint GroundVertexArrayID;

		// The image to use as a texture (ground)
		shared_ptr<Texture> texture0;

		// Global data (larger program should be encapsulated)
		vec3 gMin;
		float gRot = 0;
		float gCamH = 0;

		// Animation data
		float lightPos1Trans = 2.0;
		float gTrans = -3;
		float sTheta = 0;
		float eTheta = 0;
		float hTheta = 0;

		bool materialToggle = true;

		void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
			if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
				glfwSetWindowShouldClose(window, GL_TRUE);
			}
			// Update global camera rotate
			if (key == GLFW_KEY_A && action == GLFW_PRESS) {
				gRot -= 0.2;
			}
			if (key == GLFW_KEY_D && action == GLFW_PRESS) {
				gRot += 0.2;
			}

			// Update camera height
			if (key == GLFW_KEY_S && action == GLFW_PRESS) {
				gCamH  += 0.25;
			}
			if (key == GLFW_KEY_W && action == GLFW_PRESS) {
				gCamH  -= 0.25;
			}

			// Update one of the light's position along an axis
			if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
				lightPos1Trans -= 0.25;
			}
			if (key == GLFW_KEY_E && action == GLFW_PRESS) {
				lightPos1Trans += 0.25;
			}

			// Show/Hide wireframe of all objects
			if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
				glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
			}
			if (key == GLFW_KEY_Z && action == GLFW_RELEASE) {
				glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
			}

			// Toggle one of the materials between the two varients 
			if (key == GLFW_KEY_M && action == GLFW_PRESS) {
				materialToggle = !materialToggle;
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
			glClearColor(.72f, .84f, 1.06f, 1.0f);
			// Enable z-buffer test.
			glEnable(GL_DEPTH_TEST);

			// Initialize the GLSL program that we will use for local shading
			prog = make_shared<Program>();
			prog->setVerbose(true);
			prog->setShaderNames(resourceDirectory + "/simple_vert.glsl", resourceDirectory + "/simple_frag.glsl");
			prog->init();
			prog->addUniform("P");
			prog->addUniform("V");
			prog->addUniform("M");
			prog->addUniform("MatAmb");
			prog->addUniform("MatDif");
			prog->addUniform("MatShine");
			prog->addUniform("MatSpec");
			prog->addUniform("lightPos0"); // Static light source {-2, 2, 2} color {1, 1, 1}
			prog->addUniform("lightColor0");
			prog->addUniform("lightPos1"); // Moving with "q" and "e" color {1, 1, 1}
			prog->addUniform("lightColor1");
			prog->addAttribute("vertPos");
			prog->addAttribute("vertNor");

			// Initialize the GLSL program that we will use for texture mapping
			texProg = make_shared<Program>();
			texProg->setVerbose(true);
			texProg->setShaderNames(resourceDirectory + "/tex_vert.glsl", resourceDirectory + "/tex_frag0.glsl");
			texProg->init();
			texProg->addUniform("P");
			texProg->addUniform("V");
			texProg->addUniform("M");
			texProg->addUniform("Texture0");
			texProg->addAttribute("vertPos");
			texProg->addAttribute("vertNor");
			texProg->addAttribute("vertTex");

			//read in a load the texture
			texture0 = make_shared<Texture>();
			texture0->setFilename(resourceDirectory + GROUND_TEX);
			texture0->init();
			texture0->setUnit(0);
			texture0->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
		}

		void initGeom(const std::string& resourceDirectory) {
			string errStr;
			bool rc;
			vector<tinyobj::shape_t> TOshapes;
			vector<tinyobj::material_t> objMaterials;

			// -- Load Sphere Mesh -----------------------------------------------------
			rc = tinyobj::LoadObj(
					TOshapes,
					objMaterials,
					errStr, (resourceDirectory + SPHERE_OBJ).c_str());

			if (!rc) {
				cerr << errStr << endl;
			} 
			else {
				sphereObject = make_shared<Shape>();
				sphereObject->createShape(TOshapes[0]);
				sphereObject->measure();
				sphereObject->init();
			}
	
			// Clean up shapes for the next object setup
			TOshapes.clear();
			objMaterials.clear();
			errStr = "";

			// -- Load Bunny Mesh ----------------------------------------------------
			rc = tinyobj::LoadObj(
					TOshapes,
					objMaterials,
					errStr, 
					(resourceDirectory + BUNNY_OBJ).c_str());

			if (!rc) {
				cerr << errStr << endl;
			} 
			else {
				bunnyObject = make_shared<Shape>();
				bunnyObject->createShape(TOshapes[0]);
				bunnyObject->measure();
				bunnyObject->init();
			}

			// Clean up shapes for the next object setup
			TOshapes.clear();
			objMaterials.clear();
			errStr = "";

			// -- Load Dummy Mesh ----------------------------------------------------
			rc = tinyobj::LoadObj(
					TOshapes,
					objMaterials,
					errStr, 
					(resourceDirectory + DUMMY_OBJ).c_str());

			if (!rc) {
				cerr << errStr << endl;
			} 
			else {
				for (int i = 0; i < TOshapes.size(); ++i) {
					auto obj = make_shared<Shape>();
					obj->createShape(TOshapes[i]);
					obj->measure();
					obj->init();

					dummyObject.push_back(obj);
				}
			}

			// Clean up shapes for the next object setup
			TOshapes.clear();
			objMaterials.clear();
			errStr = "";

			// -- Load Ground --------------------------------------------------------
			initGround();
		}

		// Directly pass quad for the ground to the GPU
		void initGround() {

			float g_groundSize = 20;
			float g_groundY = -0.25;

			// A x-z plane at y = g_groundY of dimension [-g_groundSize, g_groundSize]^2
			float GrndPos[] = {
				-g_groundSize, g_groundY, -g_groundSize,
				-g_groundSize, g_groundY,  g_groundSize,
				g_groundSize, g_groundY,  g_groundSize,
				g_groundSize, g_groundY, -g_groundSize
			};

			float GrndNorm[] = {
				0, 1, 0,
				0, 1, 0,
				0, 1, 0,
				0, 1, 0,
				0, 1, 0,
				0, 1, 0
			};

			static GLfloat GrndTex[] = {
				0, 0, // back
				0, 1,
				1, 1,
				1, 0 };

			unsigned short idx[] = {0, 1, 2, 0, 2, 3};

			// Generate the ground VAO
			glGenVertexArrays(1, &GroundVertexArrayID);
			glBindVertexArray(GroundVertexArrayID);

			g_GiboLen = 6;
			glGenBuffers(1, &GrndBuffObj);
			glBindBuffer(GL_ARRAY_BUFFER, GrndBuffObj);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GrndPos), GrndPos, GL_STATIC_DRAW);

			glGenBuffers(1, &GrndNorBuffObj);
			glBindBuffer(GL_ARRAY_BUFFER, GrndNorBuffObj);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GrndNorm), GrndNorm, GL_STATIC_DRAW);

			glGenBuffers(1, &GrndTexBuffObj);
			glBindBuffer(GL_ARRAY_BUFFER, GrndTexBuffObj);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GrndTex), GrndTex, GL_STATIC_DRAW);

			glGenBuffers(1, &GIndxBuffObj);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GIndxBuffObj);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);
		}

		// Code to draw the ground plane
		void drawGround(shared_ptr<Program> curS) {
			curS->bind();
			glBindVertexArray(GroundVertexArrayID);
			texture0->bind(curS->getUniform("Texture0"));
			// Draw the ground plane 
			SetModel(vec3(0, -1, 0), 0, 0, 1, curS);
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, GrndBuffObj);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, GrndNorBuffObj);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

			glEnableVertexAttribArray(2);
			glBindBuffer(GL_ARRAY_BUFFER, GrndTexBuffObj);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

			// Draw!
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GIndxBuffObj);
			glDrawElements(GL_TRIANGLES, g_GiboLen, GL_UNSIGNED_SHORT, 0);

			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(2);
			curS->unbind();
		}

		// Helper function to pass material data to the GPU
		void SetMaterial(shared_ptr<Program> curS, int i) {
			switch (i) {
				case 0: //
					glUniform3f(curS->getUniform("MatAmb"), 0.096, 0.046, 0.095);
					glUniform3f(curS->getUniform("MatDif"), 0.96, 0.46, 0.95);
					glUniform3f(curS->getUniform("MatSpec"), 0.45, 0.23, 0.45);
					glUniform1f(curS->getUniform("MatShine"), 120.0);
					break;
				case 1: // 
					glUniform3f(curS->getUniform("MatAmb"), 0.063, 0.038, 0.1);
					glUniform3f(curS->getUniform("MatDif"), 0.63, 0.38, 1.0);
					glUniform3f(curS->getUniform("MatSpec"), 0.3, 0.2, 0.5);
					glUniform1f(curS->getUniform("MatShine"), 4.0);
					break;
				case 2: //
					glUniform3f(curS->getUniform("MatAmb"), 0.004, 0.05, 0.09);
					glUniform3f(curS->getUniform("MatDif"), 0.04, 0.5, 0.9);
					glUniform3f(curS->getUniform("MatSpec"), 0.02, 0.25, 0.45);
					glUniform1f(curS->getUniform("MatShine"), 27.9);
					break;
			}
		}

		// Helper function to set model trasnforms
		void SetModel(vec3 trans, float rotY, float rotX, float sc, shared_ptr<Program> curS) {
			mat4 Trans = glm::translate( glm::mat4(1.0f), trans);
			mat4 RotX = glm::rotate( glm::mat4(1.0f), rotX, vec3(1, 0, 0));
			mat4 RotY = glm::rotate( glm::mat4(1.0f), rotY, vec3(0, 1, 0));
			mat4 ScaleS = glm::scale(glm::mat4(1.0f), vec3(sc));
			mat4 ctm = Trans*RotX*RotY*ScaleS;
			glUniformMatrix4fv(curS->getUniform("M"), 1, GL_FALSE, value_ptr(ctm));
		}

		void setModel(std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack>M) {
			glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
		}

		void render() {
			// Get current frame buffer size.
			int width, height;
			glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
			glViewport(0, 0, width, height);

			// Clear framebuffer
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Create the matrix stacks - please leave these alone for now
			auto Projection = make_shared<MatrixStack>();
			auto View = make_shared<MatrixStack>();
			auto Model = make_shared<MatrixStack>();

			// Apply perspective projection.
			float aspect = width/(float)height;
			Projection->pushMatrix();
			Projection->perspective(45.0f, aspect, 0.01f, 100.0f);

			// Update the Global Camera independent of anything else
			View->pushMatrix();
			View->loadIdentity();
			View->translate(vec3(0, gCamH, 0)); // Move the camera up and down
			View->rotate(gRot, vec3(0, 1, 0)); // Rotate the camera left and right

			// == LIGHTING GLSL PROGRAM ==============================================
			prog->bind();

			glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
			glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix()));
			glUniform3f(prog->getUniform("lightPos0"), -2.0, 2.0, 2.0);
			glUniform3f(prog->getUniform("lightColor0"), 1.0, 1.0, 1.0);
			glUniform3f(prog->getUniform("lightPos1"), lightPos1Trans, 4.0, 3.0);
			glUniform3f(prog->getUniform("lightColor1"), 0.9, 0.8, 0.05);

			// -- Draw objects WITH Normals ------------------------------------------
			renderBunnies(Model);
			renderSphere(Model);
			renderDummies(Model);

			// -- Draw object WITHOUT normals ----------------------------------------
			// TODO

			prog->unbind();

			// == TEXTURE GLSL PROGRAM =================================================
			texProg->bind();

			glUniformMatrix4fv(texProg->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
			glUniformMatrix4fv(texProg->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix()));
			glUniformMatrix4fv(texProg->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));

			drawGround(texProg);
			// =========================================================================

			texProg->unbind();

			//animation update example
			sTheta = sin(glfwGetTime());
			eTheta = std::max(0.0f, (float)sin(glfwGetTime()));
			hTheta = std::max(0.0f, (float)cos(glfwGetTime()));

			// Pop matrix stacks.
			Projection->popMatrix();
			View->popMatrix();
		}

		void renderBunnies(std::shared_ptr<MatrixStack>Model) {
			Model->pushMatrix();

			float sp = 3.0;
			float off = -3.5;
			for (int i=0; i < NUM_BUNNIES_X; i++) {
				for (int j=0; j < NUM_BUNNIES_Y; j++) {
					Model->pushMatrix();
					Model->translate(vec3(off+sp*i, -1, off+sp*j));
					Model->scale(vec3(0.85, 0.85, 0.85));
					Model->rotate(sp*i+sp*j, vec3(0.0, 1.0, 0.0));
					SetMaterial(prog, 0);
					glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
					bunnyObject->draw(prog);
					Model->popMatrix();
				}
			}

			Model->popMatrix();
		}

		void renderDummies(std::shared_ptr<MatrixStack>Model) {
			Model->pushMatrix();

			float sp = 1.5;
			float off = -2.7;
			for (int i=0; i < NUM_DUMMIES_X; i++) {
				for (int j=0; j < NUM_DUMMIES_Y; j++) {
					Model->pushMatrix();
					Model->translate(vec3(off+sp*i, -1, off+sp*j));
					Model->scale(vec3(0.01, 0.01, 0.01));
					Model->rotate(sp*i+sp*j, vec3(0.0, 1.0, 0.0));
					Model->rotate(-1.57, vec3(1.0, 0.0, 0.0)); // Rotate the dummy upright
					SetMaterial(prog, 0);
					glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
					for (auto& o : dummyObject) { o->draw(prog); }
					Model->popMatrix();
				}
			}

			Model->popMatrix();
		}

		void renderSphere(std::shared_ptr<MatrixStack>Model) {
			Model->pushMatrix();

			if (materialToggle) {
				SetMaterial(prog, 1);
			}
			else {
				SetMaterial(prog, 2);
			}

			Model->translate(vec3(gTrans, 0, 6));
			Model->scale(vec3(1.15, 1.35, 1.0));
			setModel(prog, Model);
			sphereObject->draw(prog);

			Model->popMatrix();
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
