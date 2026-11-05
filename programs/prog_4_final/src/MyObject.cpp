#include "MyObject.h"
#include "GLSL.h"
#include <assert.h>
#include <iostream>
#include <glm/fwd.hpp>
#include "Program.h"

using namespace std;
using namespace glm;

MyObject::MyObject() {}

MyObject::MyObject(const std::string & texFile, const std::string & objFile) {
	// Init Geometries
	string errStr;
	bool rc;
	vector<tinyobj::shape_t> TOshapes;
	vector<tinyobj::material_t> objMaterials;

	// -- Load Dummy Mesh ----------------------------------------------------
	rc = tinyobj::LoadObj(
			TOshapes,
			objMaterials,
			errStr, 
			(objFile).c_str());

	if (!rc) {
		cerr << errStr << endl;
	} 
	else {
		for (int i = 0; i < TOshapes.size(); ++i) {
			auto obj = make_shared<Shape>();
			obj->createShape(TOshapes[i]);
			obj->measure();
			obj->init();

			geometry.push_back(obj);
		}
	}

	// Init Textures
	texture = make_shared<Texture>();
	texture->setFilename(texFile);
	texture->init();
	texture->setUnit(0);
	texture->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
}


void MyObject::render(
		shared_ptr<Program> shader,
		shared_ptr<MatrixStack> model) {

	// Send to the shader
	glUniformMatrix4fv(
			shader->getUniform("M"),
			1,
			GL_FALSE,
			value_ptr(model->topMatrix()));

	// Use this object's texture.
	texture->bind(shader->getUniform("Texture0"));

	// Draw every shape in this geometry
	for (auto & shape : geometry) {
		shape->draw(shader);
	}

	texture->unbind();
}

shared_ptr<Texture> MyObject::getTexture() {
	return texture;
}

vector<shared_ptr<Shape>> MyObject::getGeometry() {
	return geometry;
}
