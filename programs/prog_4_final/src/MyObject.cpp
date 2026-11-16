#include "MyObject.h"
#include "GLSL.h"
#include <assert.h>
#include <iostream>
#include <glm/fwd.hpp>
#include "Program.h"

using namespace std;
using namespace glm;

MyObject::MyObject() {}

MyObject::MyObject(
		const string &objFile,
		const vector<string> &textureFiles,
		const vector<size_t> &textureForShape) {

	string errStr;
	vector<tinyobj::shape_t> TOshapes;
	vector<tinyobj::material_t> objMaterials;

	bool rc = tinyobj::LoadObj(
			TOshapes,
			objMaterials,
			errStr,
			objFile.c_str()
			);

	if (!rc) {
		cerr << errStr << endl;
		return;
	}

	if (textureFiles.empty() ||
			textureForShape.size() != TOshapes.size()) {
		cerr << "Need one texture mapping for each OBJ shape." << endl;
		cerr << "Objects Needed: " << TOshapes.size() << endl;
		cerr << "Textures Defined: " << textureForShape.size() << endl;
		return;
	}

	// Load each unique texture file once.
	for (const auto &texFile : textureFiles) {
		auto texture = make_shared<Texture>();
		texture->setFilename(texFile);
		texture->init();
		texture->setUnit(0); // Reuse Texture0 for each separate draw call.
		texture->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		textures.push_back(texture);
	}

	// Associate every OBJ shape with its assigned texture.
	for (size_t i = 0; i < TOshapes.size(); ++i) {
		if (textureForShape[i] >= textures.size()) {
			cerr << "Invalid texture index for OBJ shape " << i << endl;
			return;
		}

		auto shape = make_shared<Shape>();
		shape->createShape(TOshapes[i]);
		shape->measure();
		shape->init();

		Part part;
		part.shape = shape;
		part.textureIndex = textureForShape[i];
		parts.push_back(part);
	}
}

void MyObject::render(
		shared_ptr<Program> shader,
		shared_ptr<MatrixStack> model) {

	glUniformMatrix4fv(
			shader->getUniform("M"),
			1,
			GL_FALSE,
			value_ptr(model->topMatrix())
			);

	GLint textureHandle = shader->getUniform("Texture0");

	for (const auto &part : parts) {
		auto texture = textures[part.textureIndex];

		texture->bind(textureHandle);
		part.shape->draw(shader);
		texture->unbind();
	}
}
