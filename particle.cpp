#include "particle.h"
Particle::Particle() {
	position = Vec3f(0, 0, 0);
	velocity = Vec3f(0, 0, 0);
	force = Vec3f(0, 0, 0);
	mass = 1.0;
}
Particle::Particle(Vec3f p, Vec3f v, Vec3f f, float m) : position(p), velocity(v), force(f), mass(m) {

}
Particle::~Particle() {

}
