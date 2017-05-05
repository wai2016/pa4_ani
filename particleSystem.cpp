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


ParticleSystem::ParticleSystem(bool c)
{
	// TODO
	simulate = false;
	cloth = c;
	if (cloth) {
		int a = 0;
		cloth_position = Vec3f(0,0,0);
		for (int i = 0; i < 41; i++) {
			if (a % 2 == 0) {
				for (int j = 0; j < 21; j++) {
					addParticle(Vec3f(i*0.05, j*0.1, 0), Vec3f(0, 0, 0), Vec3f(0, 0, 0), 1);
				}
			}
			else {
				for (int k = 0; k < 20; k++) {
					addParticle(Vec3f(0.05*i, 0.05 + k*0.1, 0), Vec3f(0, 0, 0), Vec3f(0, 0, 0), 1);
				}
			}
			a++;
		}
	}
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
	if (cloth) {
		int a = 0;
		cloth_position = Vec3f(0, 0, 0);
		for (int i = 0; i < 41; i++) {
			if (a % 2 == 0) {
				for (int j = 0; j < 21; j++) {
					addParticle(Vec3f(i*0.05, j*0.1, 0), Vec3f(0, 0, 0), Vec3f(0, 0, 0), 1);
				}
			}
			else {
				for (int k = 0; k < 20; k++) {
					addParticle(Vec3f(0.05*i, 0.05 + k*0.1, 0), Vec3f(0, 0, 0), Vec3f(0, 0, 0), 1);
				}
			}
			a++;
		}
	}
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
void ParticleSystem::computeForcesAndUpdateParticles(float t, Vec3f x_one, Vec3f past_x_one)
{

	// TODO
	if (simulate) {
		if (BakeArray.count(round(t*30)) > 0) {
			return;
		}
		int a = 0;
		Vec3f* collision = new Vec3f[ParticleArray.size()];
		for (vector<Particle>::iterator temp2 = ParticleArray.begin(); temp2 != ParticleArray.end(); temp2++) {
			int b = 0;
			for (vector<Particle>::iterator temp1 = ParticleArray.begin(); temp1 != ParticleArray.end(); temp1++) {
				if (a != b) {
					float dx = fabs(temp2->position[0] - temp1->position[0]);
					float dy = fabs(temp2->position[1] - temp1->position[1]);
					float dz = fabs(temp2->position[2] - temp1->position[2]);
					if (dx < 0.06 && dy < 0.06 && dz < 0.06) {
						collision[a][0] += temp2->velocity[0] * (temp2->mass - temp1->mass) + 2 * temp1->velocity[0] * temp1->mass / (temp2->mass + temp1->mass);
						collision[a][1] += temp2->velocity[1] * (temp2->mass - temp1->mass) + 2 * temp1->velocity[1] * temp1->mass / (temp2->mass + temp1->mass);
						collision[a][2] += temp2->velocity[2] * (temp2->mass - temp1->mass) + 2 * temp1->velocity[2] * temp1->mass / (temp2->mass + temp1->mass);
					}
				}
				b++;
			}
			a++;
		}
		int c = 0;
		for (vector<Particle>::iterator temp = ParticleArray.begin(); temp != ParticleArray.end();) {
			if (temp->position[1] < 0.0) {
				ParticleArray.erase(ParticleArray.begin() + c);
				temp = ParticleArray.begin() + c;
				continue;
			}
			if (!collision[c].iszero()) {
				temp->velocity = collision[c];
			}
			Vec3f net_force = temp->force;

			Vec3f v_one = (x_one - past_x_one);
			Vec3f delta_x = (x_one - temp->position);
			Vec3f delta_v = (v_one - temp->velocity);
			float delta_v_Dot_delta_x = delta_x[0]* delta_v[0] + delta_x[1]* delta_v[1] + delta_x[2]* delta_v[2];
			net_force += (delta_x/ delta_x.length()) * (5*(delta_x.length() - 2) + 5*(delta_v_Dot_delta_x/delta_x.length()));

			float drag_coeff = 1.0;
			Vec3f drag_force(drag_coeff * temp->velocity[0], drag_coeff * temp->velocity[1], drag_coeff * temp->velocity[2]);
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
			c++;
			temp++;
		}
		delete[] collision;
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

int ParticleSystem::ParticleArraySize()
{
	return ParticleArray.size();
}

void ParticleSystem::drawClothParticles(float t)
{
	if (BakeArray.count(round(t * 30)) > 0) {
		setAmbientColor(.1f, .1f, .1f);
		setDiffuseColor(.5f, .5f, .5f);
		glPushMatrix();
		for (int i = 0; i < 20; i++) {
			for (int j = 0; j < 20; j++) {
				glPushMatrix();
				Vec3f center_point = BakeArray.at(round(t * 30))[21 + 41 * i + j].position;
				Vec3f left_lower_point = BakeArray.at(round(t * 30))[41 * i + j].position;
				Vec3f right_lower_point = BakeArray.at(round(t * 30))[41 * (i + 1) + j].position;
				Vec3f left_upper_point = BakeArray.at(round(t * 30))[41 * i + j + 1].position;
				Vec3f right_upper_point = BakeArray.at(round(t * 30))[41 * (i + 1) + j + 1].position;
				drawTriangle(center_point[0], center_point[1], center_point[2], right_upper_point[0], right_upper_point[1], right_upper_point[2], left_upper_point[0], left_upper_point[1], left_upper_point[2]);
				drawTriangle(center_point[0], center_point[1], center_point[2], left_upper_point[0], left_upper_point[1], left_upper_point[2], left_lower_point[0], left_lower_point[1], left_lower_point[2]);
				drawTriangle(center_point[0], center_point[1], center_point[2], right_lower_point[0], right_lower_point[1], right_lower_point[2], right_upper_point[0], right_upper_point[1], right_upper_point[2]);
				drawTriangle(center_point[0], center_point[1], center_point[2], left_lower_point[0], left_lower_point[1], left_lower_point[2], right_lower_point[0], right_lower_point[1], right_lower_point[2]);
				glPopMatrix();
			}
		}
		glPopMatrix();
	}
	if (simulate) {
		setAmbientColor(.1f, .1f, .1f);
		setDiffuseColor(.5f, .5f, .5f);
		glPushMatrix();
		for (int i = 0; i < 20; i++) {
			for (int j = 0; j < 20; j++) {
				
				glPushMatrix();
				Vec3f center_point = ParticleArray[21 + (41 * i) + j].position;
				Vec3f left_lower_point = ParticleArray[41 * i + j].position;
				Vec3f right_lower_point = ParticleArray[41 * (i + 1) + j].position;
				Vec3f left_upper_point = ParticleArray[41 * i + j + 1].position;
				Vec3f right_upper_point = ParticleArray[41 * (i + 1) + j + 1].position;
				drawTriangle(center_point[0], center_point[1], center_point[2], right_upper_point[0], right_upper_point[1], right_upper_point[2], left_upper_point[0], left_upper_point[1], left_upper_point[2]);
				drawTriangle(center_point[0], center_point[1], center_point[2], left_upper_point[0], left_upper_point[1], left_upper_point[2], left_lower_point[0], left_lower_point[1], left_lower_point[2]);
				drawTriangle(center_point[0], center_point[1], center_point[2], right_lower_point[0], right_lower_point[1], right_lower_point[2], right_upper_point[0], right_upper_point[1], right_upper_point[2]);
				drawTriangle(center_point[0], center_point[1], center_point[2], left_lower_point[0], left_lower_point[1], left_lower_point[2], right_lower_point[0], right_lower_point[1], right_lower_point[2]);
				glPopMatrix();
			}
		}
		glPopMatrix();

	}
}

void ParticleSystem::computeForcesAndUpdateClothParticles(float t, Vec3f current_position)
{
	if (simulate) {
		if (BakeArray.count(round(t * 30)) > 0) {
			return;
		}
		/*if (t > 2 && t < 3) {
			ParticleArray[0].velocity = Vec3f(-1, -1, 0);
			ParticleArray[820].velocity = Vec3f(1, -1, 0);
		}*/
		//if (t > 2 && t < 3) {
		//	ParticleArray[0].velocity = Vec3f(-2, -1, 0);
		//	ParticleArray[820].velocity = Vec3f(2, -1, 0);
		//}
		//if (t > 7 && t < 8) {
		//	for (int q = 0; q < 21; q++) {
		//		ParticleArray[41 * q].velocity = Vec3f((41 * q - 451) / 902.0, -1, -1);
		//	}
		//}
		if (!((cloth_position - current_position).iszero())) {
			for (int q = 0; q < 21; q++) {
				ParticleArray[41 * q].velocity = (cloth_position - current_position);
			}
			cloth_position = current_position;
		}
		int a = 0;
		vector <Particle> NearParticle;
		vector <Particle> DiagonalParticle;
		for (int i = 0; i < 41; i++) {
			if (a % 2 == 0) {
				for (int j = 0; j < 21; j++) {
					int p = 41 * (i / 2) + j;
					Vec3f net_force = ParticleArray[p].force;
					if (!(p == 20 || p == 840)) {
						if (p <= 20) {
							if (p == 0) {
								NearParticle.push_back(ParticleArray[p + 41]);
								NearParticle.push_back(ParticleArray[p + 1]);
								DiagonalParticle.push_back(ParticleArray[p + 21]);
							}
							/*else if (p == 20) {
								NearParticle.push_back(ParticleArray[p + 41]);
								NearParticle.push_back(ParticleArray[p - 1]);
								DiagonalParticle.push_back(ParticleArray[p + 20]);
							}*/
							else {
								NearParticle.push_back(ParticleArray[p + 41]);
								NearParticle.push_back(ParticleArray[p - 1]);
								NearParticle.push_back(ParticleArray[p + 1]);
								DiagonalParticle.push_back(ParticleArray[p + 20]);
								DiagonalParticle.push_back(ParticleArray[p + 21]);
							}
						}
						else if (p >= 820) {
							if (p == 820) {
								NearParticle.push_back(ParticleArray[p - 41]);
								NearParticle.push_back(ParticleArray[p + 1]);
								DiagonalParticle.push_back(ParticleArray[p - 20]);
							}
							/*else if (p == 840) {
								NearParticle.push_back(ParticleArray[p - 41]);
								NearParticle.push_back(ParticleArray[p - 1]);
								DiagonalParticle.push_back(ParticleArray[p - 21]);
							}*/
							else {
								NearParticle.push_back(ParticleArray[p - 41]);
								NearParticle.push_back(ParticleArray[p + 1]);
								NearParticle.push_back(ParticleArray[p - 1]);
								DiagonalParticle.push_back(ParticleArray[p - 20]);
								DiagonalParticle.push_back(ParticleArray[p - 21]);
							}
						}
						else {
							if (p % 41 == 0) {
								NearParticle.push_back(ParticleArray[p - 41]);
								NearParticle.push_back(ParticleArray[p + 1]);
								NearParticle.push_back(ParticleArray[p + 41]);
								DiagonalParticle.push_back(ParticleArray[p - 20]);
								DiagonalParticle.push_back(ParticleArray[p + 21]);
							}
							else if ((p - 20) % 41 == 0) {
								NearParticle.push_back(ParticleArray[p - 41]);
								NearParticle.push_back(ParticleArray[p - 1]);
								NearParticle.push_back(ParticleArray[p + 41]);
								DiagonalParticle.push_back(ParticleArray[p - 21]);
								DiagonalParticle.push_back(ParticleArray[p + 20]);
							}
							else {
								NearParticle.push_back(ParticleArray[p - 41]);
								NearParticle.push_back(ParticleArray[p + 1]);
								NearParticle.push_back(ParticleArray[p + 1]);
								NearParticle.push_back(ParticleArray[p + 41]);
								DiagonalParticle.push_back(ParticleArray[p - 21]);
								DiagonalParticle.push_back(ParticleArray[p + 20]);
								DiagonalParticle.push_back(ParticleArray[p - 20]);
								DiagonalParticle.push_back(ParticleArray[p + 21]);
							}
						}
						for (int l = 0; l < NearParticle.size(); l++) {
							Vec3f delta_x = (NearParticle[l].position - ParticleArray[p].position);
							Vec3f delta_v = (NearParticle[l].velocity - ParticleArray[p].velocity);
							float delta_v_Dot_delta_x = delta_x[0] * delta_v[0] + delta_x[1] * delta_v[1] + delta_x[2] * delta_v[2];
							net_force += (delta_x / delta_x.length()) * (5 * (delta_x.length() - 0.1) + 5 * (delta_v_Dot_delta_x / delta_x.length()));
						}
						for (int m = 0; m < DiagonalParticle.size(); m++) {
							Vec3f delta_x = (DiagonalParticle[m].position - ParticleArray[p].position);
							Vec3f delta_v = (DiagonalParticle[m].velocity - ParticleArray[p].velocity);
							float delta_v_Dot_delta_x = delta_x[0] * delta_v[0] + delta_x[1] * delta_v[1] + delta_x[2] * delta_v[2];
							net_force += (delta_x / delta_x.length()) * (5 * (delta_x.length() - 0.0707) + 5 * (delta_v_Dot_delta_x / delta_x.length()));
						}
						if ((ParticleArray[p].start_position - ParticleArray[p].position).length() < 1) {
							net_force += (ParticleArray[p].start_position - ParticleArray[p].position);
						}
						float drag_coeff = 1.0;
						Vec3f drag_force(drag_coeff * ParticleArray[p].velocity[0], drag_coeff * ParticleArray[p].velocity[1], drag_coeff * ParticleArray[p].velocity[2]);
						for (int i = 0; i < 3; i++) {
							if (net_force[i] * (net_force[i] - drag_force[i]) < 0) {
								net_force[i] = 0.0;
							}
							else {
								net_force[i] = net_force[i] - drag_force[i];
							}
						}
						ParticleArray[p].position += 0.05 * ParticleArray[p].velocity;
						ParticleArray[p].velocity += 0.05 * (net_force / ParticleArray[p].mass);
						NearParticle.clear();
						DiagonalParticle.clear();
					}
					
				}
			}
			else {
				for (int k = 0; k < 20; k++) {
					int p = 21 + 41 * (i/2) + k;
					Vec3f net_force = ParticleArray[p].force;
					DiagonalParticle.push_back(ParticleArray[p - 21]);
					DiagonalParticle.push_back(ParticleArray[p + 20]);
					DiagonalParticle.push_back(ParticleArray[p - 20]);
					DiagonalParticle.push_back(ParticleArray[p + 21]);
					for (int m = 0; m < DiagonalParticle.size(); m++) {
						Vec3f delta_x = (DiagonalParticle[m].position - ParticleArray[p].position);
						Vec3f delta_v = (DiagonalParticle[m].velocity - ParticleArray[p].velocity);
						float delta_v_Dot_delta_x = delta_x[0] * delta_v[0] + delta_x[1] * delta_v[1] + delta_x[2] * delta_v[2];
						net_force += (delta_x / delta_x.length()) * (5 * (delta_x.length() - 0.0707) + 5 * (delta_v_Dot_delta_x / delta_x.length()));
					}
					if ((ParticleArray[p].start_position - ParticleArray[p].position).length() < 1) {
						net_force += (ParticleArray[p].start_position - ParticleArray[p].position);
					}
					float drag_coeff = 1.0;
					Vec3f drag_force(drag_coeff * ParticleArray[p].velocity[0], drag_coeff * ParticleArray[p].velocity[1], drag_coeff * ParticleArray[p].velocity[2]);
					for (int i = 0; i < 3; i++) {
						if (net_force[i] * (net_force[i] - drag_force[i]) < 0) {
							net_force[i] = 0.0;
						}
						else {
							net_force[i] = net_force[i] - drag_force[i];
						}
					}
					ParticleArray[p].position += 0.05 * ParticleArray[p].velocity;
					ParticleArray[p].velocity += 0.05 * (net_force / ParticleArray[p].mass);
					DiagonalParticle.clear();
				}
			}
			a++;
		}
		bakeParticles(t);
	}
}
