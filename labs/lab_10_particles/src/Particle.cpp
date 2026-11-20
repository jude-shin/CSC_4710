//
// sueda - geometry edits Z. Wood
// 3/16
//

#include <iostream>
#include "Particle.h"
#include "GLSL.h"
#include "MatrixStack.h"
#include "Program.h"
#include "Texture.h"


float randFloat(float l, float h)
{
	float r = rand() / (float) RAND_MAX;
	return (1.0f - r) * l + r * h;
}

Particle::Particle(vec3 start) :
	charge(1.0f),
	m(1.0f),
	d(0.2f),
	x(start),
	v(0.0f, 0.0f, 0.0f),
	lifespan(0.1f),
	tEnd(0.0f),
	scale(1.0f),
	color(1.0f, 1.0f, 1.0f, 1.0f)
{
}

Particle::~Particle()
{
}

void Particle::load(vec3 start)
{
	// Random initialization
	rebirth(0.0f, start);
}

/* all particles born at the origin */
void Particle::rebirth(float t, vec3 start)
{
	charge = randFloat(0.0f, 1.0f) < 0.5 ? -1.0f : 1.0f;	
	m = 1.0f;
	d = randFloat(0.0f, 0.2f);
	x = start;
	v.x = randFloat(-0.27f, 0.3f);
	v.y = randFloat(-0.1f, 0.9f);
	v.z = randFloat(-0.3f, 0.27f);
	a = vec3(0.0f, -0.4f, 0.0f); // constant downward acceleration
	lifespan = randFloat(1.0f, 7.0f); 
	tEnd = t + lifespan;
	scale = randFloat(0.2, 1.0f);

	
	// Make this randomized
	color.r = randFloat(0.0f, 1.0f);
	color.g = randFloat(0.0f, 1.0f);
	color.b = randFloat(0.0f, 1.0f);
	color.a = 1.0f;
}

void Particle::update(float t, float h, const vec3 &g, const vec3 start) {

	if(t > tEnd) {
		rebirth(t, start);
	}
	
	// Accelerate the particle towards somewhere
	v += a * h;

	// Particles slow down over time with drag
	v *= exp(-d * h);

	// very simple update
	x += h*v;

	// Have the particles "stop" to the floor
	x.y = max(0.0f, x.y);

	//To do - how do you want to update the forces?
	color.a = (tEnd-t)/lifespan;
}
