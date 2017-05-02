#pragma warning(disable : 4786)

#include "particleSystem.h"


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <FL/gl.h>
#include "modelerdraw.h"


/***************
 * Constructors
 ***************/

ParticleSystem::ParticleSystem() 
{
	// TODO
}





/*************
 * Destructor
 *************/

ParticleSystem::~ParticleSystem() 
{
	// TODO
	ParticleArray.clear();
	BakeArray.clear();
}


/******************
 * Simulation fxns
 ******************/

/** Start the simulation */
void ParticleSystem::startSimulation(float t)
{
    
	// TODO
	ParticleArray.clear();
	clearBaked();
	bake_start_time = t;
	// These values are used by the UI ...
	// -ve bake_end_time indicates that simulation
	// is still progressing, and allows the
	// indicator window above the time slider
	// to correctly show the "baked" region
	// in grey.
	bake_end_time = -1;
	simulate = true;
	dirty = true;

}

/** Stop the simulation */
void ParticleSystem::stopSimulation(float t)
{
    
	// TODO
	bake_end_time = t;
	// These values are used by the UI
	simulate = false;
	dirty = true;

}

/** Reset the simulation */
void ParticleSystem::resetSimulation(float t)
{
    
	// TODO
	bake_start_time = 0;
	bake_end_time = 0;
	// These values are used by the UI
	simulate = false;
	dirty = true;

}

/** Compute forces and update particles **/
void ParticleSystem::computeForcesAndUpdateParticles(float t, Vec3f x_one)
{

	// TODO
	if (simulate) {
		if (BakeArray.count(round(t*30)) > 0) {
			return;
		}
		for (vector<Particle>::iterator temp = ParticleArray.begin(); temp != ParticleArray.end(); temp++) {
			float drag_coeff = 1.0;
			Vec3f net_force = temp->force;
			Vec3f drag_force(drag_coeff * temp->velocity[0], drag_coeff * temp->velocity[1], drag_coeff * temp->velocity[2]);

			Vec3f v_one(0, 0, 0);
			Vec3f delta_x = (x_one - temp->position);
			Vec3f delta_v = (v_one - temp->velocity);
			float delta_v_Dot_delta_x = delta_x[0]* delta_v[0] + delta_x[1]* delta_v[1] + delta_x[2]* delta_v[2];
			net_force += (delta_x/ delta_x.length()) * (5*(delta_x.length() - 5) + 5*(delta_v_Dot_delta_x/delta_x.length()));

			for (int i = 0; i < 3; i++) {
				if (net_force[i] * (net_force[i] - drag_force[i]) < 0) {
					net_force[i] = 0.0;
				}
				else {
					net_force[i] = net_force[i] - drag_force[i];
				}
			}
			temp->position += 0.05 * temp->velocity;
			temp->velocity += 0.05 * (net_force / temp->mass);
		}

		bakeParticles(t);
	}
}


/** Render particles */
void ParticleSystem::drawParticles(float t)
{

	// TODO
	if (BakeArray.count(round(t * 30)) > 0) {
		setAmbientColor(.1f, .1f, .1f);
		setDiffuseColor(.5f, .5f, .5f);
		glPushMatrix();
		for (vector<Particle>::iterator temp = BakeArray.at(round(t * 30)).begin(); temp != BakeArray.at(round(t * 30)).end(); temp++) {
			glPushMatrix();
			glTranslatef(temp->position[0], temp->position[1], temp->position[2]);
			drawSphere(0.05);
			glPopMatrix();
		}
		glPopMatrix();
	}
	if (simulate) {
		setAmbientColor(.1f, .1f, .1f);
		setDiffuseColor(.5f, .5f, .5f);
		glPushMatrix();
		for (vector<Particle>::iterator temp = ParticleArray.begin(); temp != ParticleArray.end(); temp++) {
			glPushMatrix();
			glTranslatef(temp->position[0], temp->position[1], temp->position[2]);
			drawSphere(0.05);
			glPopMatrix();
		}
		glPopMatrix();
		
	}

}





/** Adds the current configuration of particles to
  * your data structure for storing baked particles **/
void ParticleSystem::bakeParticles(float t) 
{
	// TODO
	BakeArray.insert(std::make_pair(round(t * 30), ParticleArray));
}

/** Clears out your data structure of baked particles */
void ParticleSystem::clearBaked()
{
	// TODO
	BakeArray.clear();
}

void ParticleSystem::addParticle(Vec3f p, Vec3f v, Vec3f f, float m)
{
	Particle temp(p,v,f,m);
	ParticleArray.push_back(temp);
}



