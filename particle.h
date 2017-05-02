#ifndef PARTICLE_H
#define PARTICLE_H

#include "vec.h"


class Particle
{
public:
	Particle();
	Particle(Vec3f p, Vec3f v, Vec3f f, float m);
	~Particle();

	Vec3f position;
	Vec3f velocity;
	Vec3f force;
	float mass;
};

#endif