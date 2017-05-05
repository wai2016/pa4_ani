#include "particle.h"
Particle::Particle(Vec3f p, Vec3f v, Vec3f f, float m) : position(p), velocity(v), force(f), mass(m) {
	start_position = p;
}
Particle::~Particle() {

}
