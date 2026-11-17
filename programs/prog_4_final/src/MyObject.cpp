#include "MyObject.h"
#include "GLSL.h"
#include <assert.h>
#include <algorithm>
#include <iostream>
#include <glm/fwd.hpp>
#include "Program.h"

using namespace std;
using namespace glm;

MyObject::MyObject() {}

MyObject::MyObject(const string &objFileName, const string &resourceDirectory) {

	string errStr;
	vector<tinyobj::shape_t> TOshapes;
	vector<tinyobj::material_t> objMaterials;

	bool rc = tinyobj::LoadObj(
			TOshapes,
			objMaterials,
			errStr,
			(resourceDirectory + objFileName).c_str(),
			(resourceDirectory + "/").c_str()
			);

	if (!rc) {
		cerr << errStr << endl;
		return;
	}

	if (!errStr.empty()) {
		cerr << errStr << endl;
	}

	// One texture cache entry per MTL material. A material may be used by
	// multiple OBJ groups, but should only be uploaded to the GPU once.
	vector<int> textureIndexForMaterial(objMaterials.size(), -1);

	// This asset has one material per OBJ group. Preserve the model's authored
	// groups and UVs instead of guessing which texture belongs to each shape.
	for (size_t i = 0; i < TOshapes.size(); ++i) {
		const tinyobj::mesh_t &mesh = TOshapes[i].mesh;
		if (mesh.material_ids.empty()) {
			cerr << "OBJ shape " << i << " has no material assigned." << endl;
			return;
		}

		const int materialID = mesh.material_ids.front();
		if (materialID < 0 || materialID >= static_cast<int>(objMaterials.size())) {
			cerr << "OBJ shape " << i << " refers to an invalid material." << endl;
			return;
		}

		for (int faceMaterialID : mesh.material_ids) {
			if (faceMaterialID != materialID) {
				cerr << "OBJ shape " << i
					 << " uses more than one material; split it into material groups."
					 << endl;
				return;
			}
		}

		int textureIndex = textureIndexForMaterial[materialID];
		if (textureIndex == -1) {
			const string &textureName = objMaterials[materialID].diffuse_texname;
			if (textureName.empty()) {
				cerr << "Material '" << objMaterials[materialID].name
					 << "' has no map_Kd texture." << endl;
				return;
			}

			auto texture = make_shared<Texture>();
			texture->setFilename(resourceDirectory + "/" + textureName);
			texture->init();
			texture->setUnit(0); // Each part is drawn separately with Texture0.
			texture->setWrapModes(GL_REPEAT, GL_REPEAT);

			textureIndex = static_cast<int>(textures.size());
			textureIndexForMaterial[materialID] = textureIndex;
			textures.push_back(texture);
		}

		auto shape = make_shared<Shape>();
		shape->createShape(TOshapes[i]);
		shape->measure();
		shape->init();

		Part part;
		part.shape = shape;
		part.textureIndex = static_cast<size_t>(textureIndex);
		parts.push_back(part);
	}
}

// TODO: remove this
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
		// texture->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
		texture->setWrapModes(GL_REPEAT, GL_REPEAT);

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
