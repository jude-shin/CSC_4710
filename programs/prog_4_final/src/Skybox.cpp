#include "Skybox.h"
#include "Program.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

using namespace std;
using namespace glm;

void Skybox::init(
		const shared_ptr<Program> &skyboxShader,
		const string &textureFile,
		float size)
{
	if (initialized) {
		cerr << "Skybox::init() should only be called once." << endl;
		return;
	}

	mesh.init(*skyboxShader);

	texture = make_shared<Texture>();
	texture->setFilename(textureFile);
	texture->setUnit(0);
	texture->init();

	// The vertex shader removes view translation, so this cube stays centered
	// around the camera. It only needs to be large enough to cover the view.
	cube.scale = vec3(size);

	initialized = true;
}

void Skybox::render(
		const shared_ptr<Program> &skyboxShader,
		const shared_ptr<MatrixStack> &projection,
		const shared_ptr<MatrixStack> &view) const
{
	if (!initialized) {
		cerr << "Skybox::render() called before Skybox::init()." << endl;
		return;
	}

	glUniformMatrix4fv(
			skyboxShader->getUniform("P"),
			1,
			GL_FALSE,
			value_ptr(projection->topMatrix())
			);

	// Remove camera translation so the skybox never appears to move closer
	// when the player walks through the maze.
	const mat4 rotationOnlyView = mat4(mat3(view->topMatrix()));
	glUniformMatrix4fv(
			skyboxShader->getUniform("V"),
			1,
			GL_FALSE,
			value_ptr(rotationOnlyView)
			);

	GLint oldDepthFunc;
	GLint oldCullFace;
	GLboolean oldDepthMask;
	const GLboolean cullingWasEnabled = glIsEnabled(GL_CULL_FACE);

	glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFunc);
	glGetIntegerv(GL_CULL_FACE_MODE, &oldCullFace);
	glGetBooleanv(GL_DEPTH_WRITEMASK, &oldDepthMask);

	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);

	if (cullingWasEnabled) {
		glCullFace(GL_FRONT);
	}

	texture->bind(skyboxShader->getUniform("Texture0"));
	cube.render(*skyboxShader, mesh);
	texture->unbind();

	glDepthMask(oldDepthMask);
	glDepthFunc(oldDepthFunc);

	if (cullingWasEnabled) {
		glCullFace(oldCullFace);
	}
}
