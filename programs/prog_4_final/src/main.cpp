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
#include "Maze.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader/tiny_obj_loader.h>

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

// === MAGIC FILES =============================================================

// .obj mesh files
#define DUMMY_OBJ "/dummy.obj"
#define WALL_OBJ "/rock_wall.obj"

// .jpg texture files
#define GROUND_TEX "/dirt.jpg"
#define ROCK_TEX "/mossy_rock_texture.jpg"

// === MAGIC CONTROLS ==========================================================

// FPS sensitivity for wasd controls and mouse (for moving the camera)
#define WASD_FPS_SENSE 0.2
#define MOUSE_FPS_SENSE_PITCH 0.004	// vertical
#define MOUSE_FPS_SENSE_YAW 0.002		// horizontal
#define INVERTED_MOUSE_CONTROLS false  // TODO: set to true before submitting
#define PITCH_CLAMP_DEG 80.0f

// BIRD sensitivity for wasd controls (for moving the dummy)
// #define WASD_BIRD_SENSE 1.0 // TODO
#define BIRD_HEIGHT 60.0f

// Game Dimensions
#define MAZE_H 10
#define MAZE_W 10
#define MAZE_SEED 100

class Application : public EventCallbacks {

	public:
		WindowManager * windowManager = nullptr;

		shared_ptr<Program> prog;		// Blinn-Phong shader program
		shared_ptr<Program> texProg;	// Texture shader program

		// Loaded Meshes 
		vector<shared_ptr<Shape>> dummyObject; 
		vector<shared_ptr<Shape>> wallObject;

		// Images for the textures 
		shared_ptr<Texture> groundTexture;
		shared_ptr<Texture> rockTexture;

		// =========================================================================

		// Keymap Binary Array
		bool keys[1024] = { false };

		// Global data for ground plane
		// Direct load constant defined CPU data to GPU (not obj)
		GLuint GrndBuffObj, GrndNorBuffObj, GrndTexBuffObj, GIndxBuffObj;
		GLuint GroundVertexArrayID;		// Ground VAO
		int g_GiboLen;

		// =========================================================================

		// Camera Data
		enum CameraMode {
			FPS, 
			BIRD,
		};
		CameraMode cameraMode = BIRD;

		// FPS Camera State
		// TODO: move these to vec3 s
		float camFpsX = 0.0f;
		float camFpsZ = 0.0f;
		float camFpsYaw = 0.0f; // horizontal
		float camFpsPitch = 0.0f; // vertical
		double prevMouseX= 0.0f;
		double prevMouseY = 0.0f;
		bool initMousePos = true; // If we have no "prev" mouse positions

		// =========================================================================

		// Maze Data
		Maze maze = Maze(MAZE_W, MAZE_H);

		void processKeyboardState() {
			if (cameraMode == FPS) {
				float yaw = camFpsYaw * MOUSE_FPS_SENSE_YAW;
				vec3 forward(-sin(yaw), 0.0f, -cos(yaw));
				vec3 right(cos(yaw), 0.0f, -sin(yaw));

				// === WASD FPS Controls =================================================
				if (keys[GLFW_KEY_W]) {
					camFpsX += forward.x * WASD_FPS_SENSE;
					camFpsZ += forward.z * WASD_FPS_SENSE;
				}
				if (keys[GLFW_KEY_S]) {
					camFpsX -= forward.x * WASD_FPS_SENSE;
					camFpsZ -= forward.z * WASD_FPS_SENSE;
				}
				if (keys[GLFW_KEY_D]) {
					camFpsX += right.x * WASD_FPS_SENSE;
					camFpsZ += right.z * WASD_FPS_SENSE;
				}
				if (keys[GLFW_KEY_A]) {
					camFpsX -= right.x * WASD_FPS_SENSE;
					camFpsZ -= right.z * WASD_FPS_SENSE;
				}
			}
			else if (cameraMode == BIRD) {
				// TODO: Just move the person
			}
		}

		void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
			if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
				glfwSetWindowShouldClose(window, GL_TRUE);
			}

			// === WASD FPS Controls =================================================
			if (key == GLFW_KEY_W || 
					key == GLFW_KEY_A || 
					key == GLFW_KEY_S || 
					key == GLFW_KEY_D) {
				// No matter what, if the key is pressed, then set the flag to be true
				if (action == GLFW_PRESS) {
					keys[key] = true;
				}
				else if (action == GLFW_RELEASE) {
					keys[key] = false;
				}
			}

			// === Switching FPS to Birds Eye View ===================================
			// Try to get one key to toggle
			// TODO: I really want a switch case, but I guess not...
			if (key == GLFW_KEY_F && GLFW_PRESS) {
				cameraMode = FPS;
			}

			if (key == GLFW_KEY_B && GLFW_PRESS) {
				cameraMode = BIRD;
			}
		}

		void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
			double posX, posY;

			if (action == GLFW_PRESS) {
				// TODO: set a lamp to the closest lamp node position if you have any lamps left?
				cout << "Pos X " << posX <<  " Pos Y " << posY << endl;
			}
		}

		void cursorPosCallback(GLFWwindow *window, double pos_x, double pos_y) {
			if (INVERTED_MOUSE_CONTROLS) {
				pos_x = -pos_x;
				pos_y = -pos_y;
			}
			
			if (initMousePos) {
				prevMouseX = pos_x;
				prevMouseY = pos_y;
				initMousePos = false;
			}
			// Get the change in mouse position from the previous poll 
    	float deltax = prevMouseX - pos_x;
    	float deltay = prevMouseY - pos_y;
    	prevMouseX = pos_x;
    	prevMouseY = pos_y;

			// Use Deltas to calculate the yaw and pitch of the camera
			camFpsPitch += deltay;
			camFpsYaw += deltax;

			// Clamp pitch to +- 80
			camFpsPitch = clamp(camFpsPitch, -PITCH_CLAMP_DEG, PITCH_CLAMP_DEG);
		}

		void resizeCallback(GLFWwindow *window, int width, int height) {
			glViewport(0, 0, width, height);
		}

		void init(const std::string& resourceDirectory) {
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
			prog->addUniform("lightPos0");
			prog->addUniform("lightColor0");
			prog->addAttribute("vertPos");
			prog->addAttribute("vertNor");

			// TODO: remove if you don't ever need this again.
			// // NOTE: Added bc the tree objects have vt in them
			// // This will produce the following warning:
			// // "WARN: vertTex cannot be bound (it either doesn't exist or has been 
			// // optimized away). safe_glAttrib calls will silently ignore it."
			// prog->addAttribute("vertTex");

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
			texProg->addUniform("MatAmb");
			texProg->addUniform("MatDif");
			texProg->addUniform("MatSpec");
			texProg->addUniform("MatShine");
			texProg->addUniform("lightPos0");
			texProg->addUniform("lightColor0");

			groundTexture = make_shared<Texture>();
			groundTexture->setFilename(resourceDirectory + GROUND_TEX);
			groundTexture->init();
			groundTexture->setUnit(0);
			// groundTexture->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
			groundTexture->setWrapModes(GL_REPEAT, GL_REPEAT);

			// Load in the rock texture (for the walls)
			rockTexture = make_shared<Texture>();
			rockTexture->setFilename(resourceDirectory + ROCK_TEX);
			rockTexture->init();
			rockTexture->setUnit(0);
			rockTexture->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

			// === Generate the Maze =================================================
			maze.generateRecursiveBacktracker(MAZE_SEED);
		}
		
		void initGeom(const std::string& resourceDirectory) {
			string errStr;
			bool rc;
			vector<tinyobj::shape_t> TOshapes;
			vector<tinyobj::material_t> objMaterials;

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

			// -- Load Wall Mesh -----------------------------------------------------
			rc = tinyobj::LoadObj(
					TOshapes,
					objMaterials,
					errStr, 
					(resourceDirectory + WALL_OBJ).c_str());

			if (!rc) {
				cerr << errStr << endl;
			} 
			else {
				for (int i = 0; i < TOshapes.size(); ++i) {
					auto obj = make_shared<Shape>();
					obj->createShape(TOshapes[i]);
					obj->measure();
					obj->init();

					wallObject.push_back(obj);
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
				0, 0,
				0, 10.0,
				10.0, 10.0,
				10.0, 0 };

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

		// TODO: remove this
		void drawWall(shared_ptr<Program> curS) {
			rockTexture->bind(curS->getUniform("Texture0"));
			initModel(vec3(0.0f, -1.0f, -5.0f), 1.57f, 0.0f, 0.01f, curS);
			for (auto& rock : wallObject) { rock->draw(curS); }
		}

		void drawGround(shared_ptr<Program> curS) {
			curS->bind();
			glBindVertexArray(GroundVertexArrayID);
			groundTexture->bind(curS->getUniform("Texture0"));

			// Draw the ground plane 
			initModel(vec3(0, -1, 0), 0, 0, 1, curS);
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
		void setMaterial(shared_ptr<Program> curS, int i) {
			switch (i) {
				case 0: // Polished Gold
					glUniform3f(curS->getUniform("MatAmb"), 0.247, 0.225, 0.065);
					glUniform3f(curS->getUniform("MatDif"), 0.401, 0.365, 0.106);
					glUniform3f(curS->getUniform("MatSpec"), 0.797, 0.723, 0.210);
					glUniform1f(curS->getUniform("MatShine"), 83.2);
					break;
			}
		}

		// Helper function to set model trasnforms
		void initModel(vec3 trans, float rotY, float rotX, float sc, shared_ptr<Program> curS) {
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

			// Update the Global Camera
			// Rotate first, then (in the same stack) translate the camera.
			View->pushMatrix();
			View->loadIdentity();
			switch (cameraMode) {
				case FPS:
					View->rotate(-camFpsPitch*MOUSE_FPS_SENSE_PITCH, vec3(1, 0, 0));
    			View->rotate(-camFpsYaw*MOUSE_FPS_SENSE_YAW, vec3(0, 1, 0));
    			View->translate(vec3(-camFpsX, 0.0f, -camFpsZ));
					break;
				case BIRD:
					View->rotate(radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
					View->translate(vec3(0.0f, -BIRD_HEIGHT, 0.0f));
					break;
			}

			// == LIGHTING GLSL PROGRAM ==============================================
			prog->bind();

			glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
			glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix()));
			glUniform3f(prog->getUniform("lightPos0"), -2.0, 2.0, 2.0);
			glUniform3f(prog->getUniform("lightColor0"), 1.0, 1.0, 1.0);

			// -- Draw objects -------------------------------------------------------
			renderDummy(Model);
			prog->unbind();

			// == TEXTURE GLSL PROGRAM =================================================
			texProg->bind();

			glUniformMatrix4fv(texProg->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
			glUniformMatrix4fv(texProg->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix()));
			glUniformMatrix4fv(texProg->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));

			// Material for textured objects
			glUniform3f(texProg->getUniform("lightPos0"), -2.0f, 2.0f, 2.0f);
			glUniform3f(texProg->getUniform("lightColor0"), 1.0f, 1.0f, 1.0f);
			glUniform3f(texProg->getUniform("MatAmb"),  0.12f, 0.12f, 0.12f);
			glUniform3f(texProg->getUniform("MatDif"),  1.0f,  1.0f,  1.0f);
			glUniform3f(texProg->getUniform("MatSpec"), 0.25f, 0.25f, 0.25f);
			glUniform1f(texProg->getUniform("MatShine"), 32.0f);

			// -- Draw objects -------------------------------------------------------

			rockTexture->bind(texProg->getUniform("Texture0"));
			maze.render(texProg, wallObject, Model);

			drawGround(texProg);

			// =========================================================================

			texProg->unbind();

			// Pop matrix stacks.
			Projection->popMatrix();
			View->popMatrix();
		}

		// Render The Dummy (player lol)
		void renderDummy(std::shared_ptr<MatrixStack>Model) {
			initModel(vec3(2, -1, -4.0), 0.0f, -1.57f, 0.01f, prog);
			setMaterial(prog, 0);
			for (auto& o : dummyObject) { o->draw(prog); }
		}

};

int main(int argc, char *argv[]) {
	// Where the resources are loaded from
	std::string resourceDir = "../resources";

	if (argc >= 2) {
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
	while (!glfwWindowShouldClose(windowManager->getHandle())) {
		// Process Keyboard State
		application->processKeyboardState();

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
