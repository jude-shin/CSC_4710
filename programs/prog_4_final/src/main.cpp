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
#include "MyObject.h"
#include "Ground.h"
#include "BambooWall.h"

// TODO: remove this macro?
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
#define WALL_TEX "/mossy_rock_texture.jpg"

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
#define MAZE_H 20
#define MAZE_W 20
#define MAZE_SCALE 1.00
#define MAZE_SEED 100
#define BAMBOO_DENSITY 3

class Application : public EventCallbacks {
	public:
		WindowManager * windowManager = nullptr;

		shared_ptr<Program> shader;	// Texture shader program

		// Objects
		MyObject wallObject; // TODO: remove the wall object
		Ground groundObject;
		BambooWall bambooWall;
		// TODO: make a skybox object?

		// =========================================================================

		// Keymap Binary Array
		bool keys[1024] = { false };

		// =========================================================================
		// Camera Data
		enum CameraMode {
			FPS, 
			BIRD,
		};
		CameraMode cameraMode = BIRD;

		// FPS Camera State
		vec3 camFpsPos = vec3(0.0f, 0.0f, 0.0f);

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
				vec3 delta(0.0f, 0.0f, 0.0f);

				// === WASD FPS Controls ===============================================
				if (keys[GLFW_KEY_W]) {
					delta += vec3(forward.x * WASD_FPS_SENSE, 0.0f, forward.z * WASD_FPS_SENSE);
				}
				if (keys[GLFW_KEY_S]) {
					delta -= vec3(forward.x * WASD_FPS_SENSE, 0.0f, forward.z * WASD_FPS_SENSE);
				}
				if (keys[GLFW_KEY_D]) {
					delta += vec3(right.x * WASD_FPS_SENSE, 0.0f, right.z * WASD_FPS_SENSE);
				}
				if (keys[GLFW_KEY_A]) {
					delta -= vec3(right.x * WASD_FPS_SENSE, 0.0f, right.z * WASD_FPS_SENSE);
				}

				camFpsPos += delta;
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

		void initObjects(const std::string& resourceDirectory) {
			GLSL::checkVersion();

			// wallObject = MyObject(resourceDirectory+WALL_TEX, resourceDirectory+WALL_OBJ); // TODO: remove the wall object
			groundObject = Ground(resourceDirectory+GROUND_TEX);
			bambooWall = BambooWall(resourceDirectory, BAMBOO_DENSITY);
		}

		// TODO: rename this to initShaders or something
		void init(const std::string& resourceDirectory) {
			GLSL::checkVersion();

			// Set background color.
			glClearColor(.72f, .84f, 1.06f, 1.0f);
			// Enable z-buffer test.
			glEnable(GL_DEPTH_TEST);

			// Initialize the GLSL program that we will use for texture mapping
			shader = make_shared<Program>();
			shader->setVerbose(true);
			shader->setShaderNames(resourceDirectory + "/tex_vert.glsl", resourceDirectory + "/tex_frag0.glsl");
			shader->init();
			shader->addUniform("P");
			shader->addUniform("V");
			shader->addUniform("M");
			shader->addUniform("Texture0");
			shader->addAttribute("vertPos");
			shader->addAttribute("vertNor");
			shader->addAttribute("vertTex");
			shader->addUniform("MatAmb");
			shader->addUniform("MatDif");
			shader->addUniform("MatSpec");
			shader->addUniform("MatShine");
			shader->addUniform("lightPos0");
			shader->addUniform("lightColor0");

			// === Generate the Maze =================================================
			maze.generateRecursiveBacktracker(MAZE_SEED);
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
    			View->translate(-camFpsPos);
					break;
				case BIRD:
					View->rotate(radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
					View->translate(vec3(0.0f, -BIRD_HEIGHT, 0.0f));
					break;
			}

			// == TEXTURE GLSL PROGRAM =================================================
			shader->bind();

			glUniformMatrix4fv(shader->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
			glUniformMatrix4fv(shader->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix()));
			glUniformMatrix4fv(shader->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));

			// Material for textured objects
			glUniform3f(shader->getUniform("lightPos0"), camFpsPos.x, camFpsPos.y, camFpsPos.z);
			glUniform3f(shader->getUniform("lightColor0"), 1.0f, 1.0f, 1.0f);

			glUniform3f(shader->getUniform("MatAmb"),  0.12f, 0.12f, 0.12f);
			glUniform3f(shader->getUniform("MatDif"),  1.0f,  1.0f,  1.0f);
			glUniform3f(shader->getUniform("MatSpec"), 0.25f, 0.25f, 0.25f);
			glUniform1f(shader->getUniform("MatShine"), 32.0f);

			// -- Draw objects -------------------------------------------------------
			// wallObject.getTexture()->bind(shader->getUniform("Texture0")); // TODO: remove the wall object
			maze.render(shader, bambooWall, Model, MAZE_SCALE);
			groundObject.render(shader);

			shader->unbind();

			// Pop matrix stacks.
			Projection->popMatrix();
			View->popMatrix();
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
	application->initObjects(resourceDir);

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
