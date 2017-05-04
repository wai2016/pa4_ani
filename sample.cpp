// The sample model.  You should build a file
// very similar to this for when you make your model.
#pragma warning (disable : 4305)
#pragma warning (disable : 4244)
#pragma warning(disable : 4786)

#include "modelerview.h"
#include "modelerapp.h"
#include "modelerdraw.h"
#include "particleSystem.h"

#include <FL/gl.h>
#include <string>

#include "modelerglobals.h"
#include "mat.h"
#include "vec.h"

#include "bitmap.h"

using namespace std;
extern Vec3f old_x_one;

// To make a SampleModel, we inherit off of ModelerView
class SampleModel : public ModelerView 
{
public:
    SampleModel(int x, int y, int w, int h, char *label) 
        : ModelerView(x,y,w,h,label) { }

    virtual void draw();
};

// We need to make a creator function, mostly because of
// nasty API stuff that we'd rather stay away from.
ModelerView* createSampleModel(int x, int y, int w, int h, char *label)
{ 
    return new SampleModel(x,y,w,h,label); 
}

Mat4f getModelViewMatrix() {
	GLfloat m[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, m);
	Mat4f matMV(m[0], m[1], m[2], m[3],
		m[4], m[5], m[6], m[7],
		m[8], m[9], m[10], m[11],
		m[12], m[13], m[14], m[15]);

	return matMV.transpose();
}

void SpawnParticles(Mat4f cameraTransform, Vec3f& past_x_one) {
	Mat4f WorldMatrix = cameraTransform.inverse() * getModelViewMatrix();
	GLfloat* fWorldMatrix = new GLfloat[16];
	WorldMatrix.getGLMatrix(fWorldMatrix);
	Vec4f WorldPoint(fWorldMatrix[12], fWorldMatrix[13], fWorldMatrix[14], fWorldMatrix[15]);
	GLfloat* glWorldMatrix = new GLfloat[16];
	ParticleSystem* ps = ModelerApplication::Instance()->GetParticleSystem();
	WorldMatrix.inverse().getGLMatrix(glWorldMatrix);
	glMultMatrixf(glWorldMatrix);
	int a = rand() % 20 - 10;
	int b = rand() % 20 - 10;
	int c = rand() % 20 - 10;
	int d = rand() % 20 - 10;
	Vec3f p = Vec3f(WorldPoint[0], WorldPoint[1], WorldPoint[2]);
	Vec3f x_one = Vec3f(WorldPoint[0]-1, WorldPoint[1], WorldPoint[2]);
	if (ps->ParticleArraySize() < 100) {
		ps->addParticle(p, Vec3f(a, WorldPoint[1] - 4.8, b), Vec3f(c, -9.81, d), 1);
	}
	delete[] glWorldMatrix;
	delete[] fWorldMatrix;
	float t = ModelerApplication::Instance()->GetTime();
	if (t < 0.1) {
		past_x_one = x_one;
	}
	ps->computeForcesAndUpdateParticles(t, x_one, past_x_one);
	ps->drawParticles(t);
	past_x_one = x_one;
}

void drawLsystem(int depth)
{
	string Lsystem = "A";
	string temp = "";
	for (int j = 0; j < depth; j++)
	{
		for (unsigned int i = 0; i < Lsystem.length(); i++)
		{
			if (Lsystem.at(i) == 'A')
			{
				temp += "B[A]A";
			}
			else if (Lsystem.at(i) == 'B')
			{
				temp += "BB";
			}
			else if (Lsystem.at(i) == '[')
			{
				temp += "[";
			}
			else if (Lsystem.at(i) == ']')
			{
				temp += "]";
			}
		}
		Lsystem.clear();
		Lsystem = temp;
		temp.clear();
	}
	for (unsigned int i = 0; i < Lsystem.length(); i++)
	{
		if (Lsystem.at(i) == 'A')
		{
			glTranslatef(0, 0.4, 0);
			drawSphere(0.2);
		}
		else if (Lsystem.at(i) == 'B')
		{
			glTranslatef(0, 0.4, 0);
			drawSphere(0.2);
		}
		else if (Lsystem.at(i) == '[')
		{
			glPushMatrix();
			glRotated(VAL(LANGLE), 0.0, 0.0, 1.0);
		}
		else if (Lsystem.at(i) == ']')
		{
			glPopMatrix();
			glRotated(-VAL(LANGLE), 0.0, 0.0, 1.0);
		}
	}
	return;
}

GLuint LoadTex(const char* filename)
{
	int width, height;
	unsigned char* image;

	image = readBMP(filename, width, height);

	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	delete image;

	return tex;
}


// We are going to override (is that the right word?) the draw()
// method of ModelerView to draw out SampleModel
void SampleModel::draw()
{
    // This call takes care of a lot of the nasty projection 
    // matrix stuff.  Unless you want to fudge directly with the 
	// projection matrix, don't bother with this ...
    ModelerView::draw();
	Mat4f CameraMatrix = getModelViewMatrix();

	//Lighting
	glEnable(GL_LIGHT2);
	GLfloat light_position2[] = { VAL(XLIGHT), VAL(YLIGHT), VAL(ZLIGHT), 0.0 };
	GLfloat lightDiffuse2[] = { 1, 1, 1, 1 };
	glLightfv(GL_LIGHT2, GL_POSITION, light_position2);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, lightDiffuse2);

	// variable
	double sizeJ = 0.2; // size of joint
	double sizeA = 0.5; // size of arm
	double lengthA = 1; // length bewteen arm and upper body(torso?)
	double sizeL = 0.7; // size of leg
	double lengthL = 0.5; // length between leg and lower body
	int LLR, LLLR, LFR, RLR, RLLR, RFR;
	LLR = VAL(LLROTATE);
	RLR = VAL(RLROTATE);
	LLLR = VAL(LLLROTATE);
	RLLR = VAL(RLLROTATE);
	LFR = VAL(LFROTATE);
	RFR = VAL(RFROTATE);

	// ----------------------------------------------------------skybox--------------------------------------------------------------
	GLuint _skybox[6]; //the array for our texture
	
	_skybox[0] = LoadTex("cliffFront.bmp");
	_skybox[1] = LoadTex("cliffLeft.bmp");
	_skybox[2] = LoadTex("cliffBack.bmp");
	_skybox[3] = LoadTex("cliffRight.bmp");
	_skybox[4] = LoadTex("cliffTop.bmp");
	_skybox[5] = LoadTex("cliffBottom.bmp");

	// Store the current matrix
	glPushMatrix();

	// Enable/Disable features
	glPushAttrib(GL_ENABLE_BIT);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);

	// Just in case we set all vertices to white.
	glColor4f(1, 1, 1, 1);

	// Render the front quad
	glBindTexture(GL_TEXTURE_2D, _skybox[0]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(20.0f, -20.0f, -20.0f);
	glTexCoord2f(1, 0); glVertex3f(-20.0f, -20.0f, -20.0f);
	glTexCoord2f(1, 1); glVertex3f(-20.0f, 20.0f, -20.0f);
	glTexCoord2f(0, 1); glVertex3f(20.0f, 20.0f, -20.0f);
	glEnd();

	// Render the left quad
	glBindTexture(GL_TEXTURE_2D, _skybox[1]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(20.0f, -20.0f, 20.0f);
	glTexCoord2f(1, 0); glVertex3f(20.0f, -20.0f, -20.0f);
	glTexCoord2f(1, 1); glVertex3f(20.0f, 20.0f, -20.0f);
	glTexCoord2f(0, 1); glVertex3f(20.0f, 20.0f, 20.0f);
	glEnd();

	// Render the back quad
	glBindTexture(GL_TEXTURE_2D, _skybox[2]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(-20.0f, -20.0f, 20.0f);
	glTexCoord2f(1, 0); glVertex3f(20.0f, -20.0f, 20.0f);
	glTexCoord2f(1, 1); glVertex3f(20.0f, 20.0f, 20.0f);
	glTexCoord2f(0, 1); glVertex3f(-20.0f, 20.0f, 20.0f);

	glEnd();

	// Render the right quad
	glBindTexture(GL_TEXTURE_2D, _skybox[3]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(-20.0f, -20.0f, -20.0f);
	glTexCoord2f(1, 0); glVertex3f(-20.0f, -20.0f, 20.0f);
	glTexCoord2f(1, 1); glVertex3f(-20.0f, 20.0f, 20.0f);
	glTexCoord2f(0, 1); glVertex3f(-20.0f, 20.0f, -20.0f);
	glEnd();

	// Render the top quad
	glBindTexture(GL_TEXTURE_2D, _skybox[4]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1); glVertex3f(-20.0f, 20.0f, -20.0f);
	glTexCoord2f(0, 0); glVertex3f(-20.0f, 20.0f, 20.0f);
	glTexCoord2f(1, 0); glVertex3f(20.0f, 20.0f, 20.0f);
	glTexCoord2f(1, 1); glVertex3f(20.0f, 20.0f, -20.0f);
	glEnd();

	// Render the bottom quad
	glBindTexture(GL_TEXTURE_2D, _skybox[5]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(-20.0f, -20.0f, -20.0f);
	glTexCoord2f(0, 1); glVertex3f(-20.0f, -20.0f, 20.0f);
	glTexCoord2f(1, 1); glVertex3f(20.0f, -20.0f, 20.0f);
	glTexCoord2f(1, 0); glVertex3f(20.0f, -20.0f, -20.0f);
	glEnd();

	// Restore enable bits and matrix
	glPopAttrib();
	glPopMatrix();


	// draw the floor
	setAmbientColor(.1f,.1f,.1f);
	setDiffuseColor(COLOR_RED);
	glPushMatrix();
	glTranslated(-5,0,-5);
	drawBox(10,0.01f,10);
	glPopMatrix();

	// draw the sample model
	setAmbientColor(.1f,.1f,.1f);
	setDiffuseColor(COLOR_GREEN);
	glPushMatrix();
	glTranslated(0, 0, -5);
	drawLsystem(VAL(LDEPTH));
	glPopMatrix();

	glPushMatrix();
		glTranslated(VAL(XPOS), VAL(YPOS), VAL(ZPOS));

		glPushMatrix();
		glTranslated(0,5,-2);
		ParticleSystem* cloth = ModelerApplication::Instance()->GetParticleSystemCloth();

		glPopMatrix();

		if (VAL(LOD) >= 1)
		{
			// Torso
			glPushMatrix();
				glTranslated(-0.5, 3, -0.5);
				glScaled(1, 1, 1);
				drawBox(1, 2, 1); 

				if (VAL(LOD) >= 2)
				{
					// Head
					glPushMatrix();
						setAmbientColor(.1f, .1f, .1f);
						setDiffuseColor(COLOR_BLUE);
						glTranslated(0, 2, 0);
						glTranslated(0.5, 0.5, 0.5);
						glRotated(VAL(HROTATE), 0, 1, 0);
						if (VAL(HEADTYPE) == 1)
						{
							glTranslated(-0.5, -0.5, -0.5);
							glScaled(1, 1, 1);
							drawBox(1, 1, 1);
							glTranslated(0.5, 0.5, 0.5); // Go to center
						}
						else if (VAL(HEADTYPE) == 2)
						{
							glScaled(1, 1, 1);
							drawSphere(0.5);
						}

						if (VAL(HEADA))
						{
							setAmbientColor(.1f, .1f, .1f);
							setDiffuseColor(1.0, 1.0, 0); // Yellow
							glTranslated(0, 1, 0);
							glRotated(90, 1, 0, 0);
							drawTorus(0.3, 0.1);
						}

					glPopMatrix();

					// Left upper arm
					glPushMatrix();
						setAmbientColor(.1f, .1f, .1f);
						setDiffuseColor(COLOR_RED);
						glTranslated(0.5, -sizeJ, 0.5);
						glTranslated(lengthA, 2, 0);
						glScaled(1, -1, 1); // Turn the y axis downward
						glRotatef(VAL(LAROTATE), 1, 0, 0);
						drawSphere(sizeJ); // Joint

						glPushMatrix();
						glRotated(90, 1, 0, 0);
						drawTorus(sizeJ, sizeJ);
						glPopMatrix();

						setAmbientColor(.1f, .1f, .1f);
						setDiffuseColor(COLOR_GREEN);
						glTranslated(-sizeA / 2, sizeJ, -sizeA / 2);
						drawBox(sizeA, 1 - sizeJ * 2, sizeA); // Arm
						glTranslated(sizeA / 2, 0, sizeA / 2); // Go to center

						if (VAL(LOD) >= 3)
						{
							// Left lower arm
							setAmbientColor(.1f, .1f, .1f);
							setDiffuseColor(COLOR_RED);
							glTranslated(0, 1 - sizeJ, 0);
							glScaled(1, 1, 1);
							glRotatef(VAL(LLAROTATE), 1, 0, 0);
							drawSphere(sizeJ); // Joint

							setAmbientColor(.1f, .1f, .1f);
							setDiffuseColor(COLOR_GREEN);
							glTranslated(-sizeA / 2, sizeJ, -sizeA / 2);
							drawBox(sizeA, 1 - sizeJ * 2, sizeA); // Arm
							glTranslated(sizeA / 2, 0, sizeA / 2); // Go to center

							if (VAL(LOD) >= 4)
							{
								// Left hand
								glTranslated(0, 1 - sizeJ * 2 + 0.2, 0);
								glScaled(1, 1, 1);
								drawSphere(0.2);
								glPushMatrix();
								SpawnParticles(CameraMatrix, old_x_one);
								glPopMatrix();
							}
						}
					glPopMatrix();

					// Right upper arm
					glPushMatrix();
						setAmbientColor(.1f, .1f, .1f);
						setDiffuseColor(COLOR_RED);
						glTranslated(0.5, -sizeJ, 0.5); 
						glTranslated(-lengthA, 2, 0);
						glScaled(1, -1, 1); // Turn the y axis downward
						glRotatef(VAL(RAROTATE), 1, 0, 0);
						drawSphere(sizeJ); // Joint

						glPushMatrix();
						glRotated(90, 1, 0, 0);
						drawTorus(sizeJ, sizeJ);
						glPopMatrix();

						setAmbientColor(.1f, .1f, .1f);
						setDiffuseColor(COLOR_GREEN);
						glTranslated(-sizeA / 2, sizeJ, -sizeA / 2);
						drawBox(sizeA, 1 - sizeJ * 2, sizeA); // Arm
						glTranslated(sizeA / 2, 0, sizeA / 2); // Go to center

						if (VAL(LOD) >= 3)
						{
							// Right lower arm
							setAmbientColor(.1f, .1f, .1f);
							setDiffuseColor(COLOR_RED);
							glTranslated(0, 1 - sizeJ, 0);
							glScaled(1, 1, 1);		
							glRotatef(VAL(RLAROTATE), 1, 0, 0);
							drawSphere(sizeJ); // Joint

							setAmbientColor(.1f, .1f, .1f);
							setDiffuseColor(COLOR_GREEN);
							glTranslated(-sizeA / 2, sizeJ, -sizeA / 2);
							drawBox(sizeA, 1 - sizeJ * 2, sizeA); // Arm
							glTranslated(sizeA / 2, 0, sizeA / 2); // Go to center

							if (VAL(LOD) >= 4)
							{
								// Right hand
								glTranslated(0, 1 - sizeJ * 2 + 0.2, 0);
								glScaled(1, 1, 1);
								drawSphere(0.2);

								if (VAL(HANDA))
								{
									glRotated(180, 1, 0, 0);
									drawCylinder(3, 0.1, 0.1);
									glRotated(180, 1, 0, 0);
									drawCylinder(1, 0.1, 0.1);
									glTranslated(0, 0, 1);
									// Dodecahedron
									glScaled(0.25, 0.25, 0.25);
									int faces[12][5] =
									{
										{ 0, 16, 2, 10, 8 },
										{ 0, 8, 4, 14, 12 },
										{ 16, 17, 1, 12, 0 },
										{ 1, 9, 11, 3, 17 },
										{ 1, 12, 14, 5, 9 },
										{ 2, 13, 15, 6, 10 },
										{ 13, 3, 17, 16, 2 },
										{ 3, 11, 7, 15, 13 },
										{ 4, 8, 10, 6, 18 },
										{ 14, 5, 19, 18, 4 },
										{ 5, 19, 7, 11, 9 },
										{ 15, 7, 19, 18, 6 }
									};

									double points[20][3] =
									{
										{ 1, 1, 1 },
										{ 1, 1, -1 },
										{ 1, -1, 1 },
										{ 1, -1, -1 },
										{ -1, 1, 1 },
										{ -1, 1, -1 },
										{ -1, -1, 1 },
										{ -1, -1, -1 },
										{ 0, M_PHI2, M_PHI },
										{ 0, M_PHI2, -M_PHI },
										{ 0, -M_PHI2, M_PHI },
										{ 0, -M_PHI2, -M_PHI },
										{ M_PHI2, M_PHI, 0 },
										{ M_PHI2, -M_PHI, 0 },
										{ -M_PHI2, M_PHI, 0 },
										{ -M_PHI2, -M_PHI, 0 },
										{ M_PHI, 0, M_PHI2 },
										{ M_PHI, 0, -M_PHI2 },
										{ -M_PHI, 0, M_PHI2 },
										{ -M_PHI, 0, -M_PHI2 }
									};

									for (int i = 0; i < 12; i++)
									{
										for (int j = 0; j < 3; j++)
										{
											drawTriangle(points[faces[i][0]][0], points[faces[i][0]][1], points[faces[i][0]][2],
												points[faces[i][j + 1]][0], points[faces[i][j + 1]][1], points[faces[i][j + 1]][2],
												points[faces[i][j + 2]][0], points[faces[i][j + 2]][1], points[faces[i][j + 2]][2]
											);
										}
									}
								}
							}
						}
					glPopMatrix();

					// Lower Body
					glPushMatrix();
						glTranslated((0.5 - 0.4 / 2), 0, 0);
						glTranslated(0.2, 0.2, 0.5);
						glRotated(VAL(LBROTATE), 0, 1, 0);
						glTranslated(-0.2, -0.2, -0.5);
						glScaled(1, -1, 1);
						drawBox(0.4, 0.4, 1);
						glTranslated((0.4 / 2), (0.4 / 2), 0.5); // Go to center

						if (VAL(LOD) >= 3)
						{
							// Left upper leg
							glPushMatrix();
								setAmbientColor(.1f, .1f, .1f);
								setDiffuseColor(COLOR_RED);
								glTranslated(lengthL, 0, 0);
								glScaled(1, 1, 1);
								glRotatef(LLR, 1, 0, 0);
								drawSphere(sizeJ);

								setAmbientColor(.1f, .1f, .1f);
								setDiffuseColor(COLOR_GREEN);
								glTranslated(-sizeL / 2, sizeJ, -sizeL / 2);
								drawBox(sizeL, 1 - sizeJ * 2, sizeL);
								glTranslated(sizeL / 2, 0, sizeL / 2);

								if (VAL(LOD) >= 4)
								{
									// Left lower leg
									setAmbientColor(.1f, .1f, .1f);
									setDiffuseColor(COLOR_RED);
									glTranslated(0, 1 - sizeJ, 0);
									glScaled(1, 1, 1);
									glRotatef(LLLR, 1, 0, 0);
									drawSphere(sizeJ);

									setAmbientColor(.1f, .1f, .1f);
									setDiffuseColor(COLOR_GREEN);
									glTranslated(-sizeL / 2, sizeJ, -sizeL / 2);
									drawBox(sizeL, 1 - sizeJ * 2, sizeL);
									glTranslated(sizeL / 2, 0, sizeL / 2);

									if (VAL(LOD) >= 5)
									{
										// Left foot
										setAmbientColor(.1f, .1f, .1f);
										setDiffuseColor(COLOR_RED);
										glTranslated(0, 1 - sizeJ, 0);
										glRotatef(LFR, 1, 0, 0);
										glScaled(1, 1, 1);
										drawSphere(sizeJ); // Joint

										setAmbientColor(.1f, .1f, .1f);
										setDiffuseColor(COLOR_GREEN);
										glTranslated(-sizeL / 2, sizeJ, -sizeL / 2);
										glScaled(1, 1, 1);
										// Foot(triangle prism)
										drawTriangle(0, 0, 0, 0, -sizeJ, sizeL, 0, 0, 1.2); // Side
										drawTriangle(0, 0, 0, 0, -sizeJ, 0, 0, -sizeJ, sizeL);
										drawTriangle(sizeL, 0, 0, sizeL, -sizeJ, sizeL, sizeL, 0, 1.2);
										drawTriangle(sizeL, 0, 0, sizeL, -sizeJ, 0, sizeL, -sizeJ, sizeL);
										drawTriangle(0, 0, 0, 0, 0, 1.2, sizeL, 0, 1.2); // Bottom
										drawTriangle(0, 0, 0, sizeL, 0, 1.2, sizeL, 0, 0);
										drawTriangle(0, 0, 0, sizeL, 0, 0, 0, -sizeJ, 0); // Back
										drawTriangle(0, -sizeJ, 0, sizeL, 0, 0, sizeL, -sizeJ, 0);
										drawTriangle(0, -sizeJ, 0, sizeL, -sizeJ, sizeL, 0, -sizeJ, sizeL); // Top
										drawTriangle(0, -sizeJ, 0, sizeL, -sizeJ, 0, sizeL, -sizeJ, sizeL);
										drawTriangle(0, -sizeJ, sizeL, sizeL, -sizeJ, sizeL, 0, 0, 1.2);
										drawTriangle(sizeL, -sizeJ, sizeL, sizeL, 0, 1.2, 0, 0, 1.2);
									}
								}
							glPopMatrix();

							// Right upper leg
							glPushMatrix();
								setAmbientColor(.1f, .1f, .1f);
								setDiffuseColor(COLOR_RED);
								glTranslated(-lengthL, 0, 0);
								glScaled(1, 1, 1);
								glRotatef(RLR, 1, 0, 0);
								drawSphere(sizeJ);

								setAmbientColor(.1f, .1f, .1f);
								setDiffuseColor(COLOR_GREEN);
								glTranslated(-sizeL / 2, sizeJ, -sizeL / 2);
								drawBox(sizeL, 1 - sizeJ * 2, sizeL);
								glTranslated(sizeL / 2, 0, sizeL / 2);

								if (VAL(LOD) >= 4)
								{
									// Right lower leg
									setAmbientColor(.1f, .1f, .1f);
									setDiffuseColor(COLOR_RED);
									glTranslated(0, 1 - sizeJ, 0);
									glScaled(1, 1, 1);
									glRotatef(RLLR, 1, 0, 0);
									drawSphere(sizeJ);

									setAmbientColor(.1f, .1f, .1f);
									setDiffuseColor(COLOR_GREEN);
									glTranslated(-sizeL / 2, sizeJ, -sizeL / 2);
									drawBox(sizeL, 1 - sizeJ * 2, sizeL);
									glTranslated(sizeL / 2, 0, sizeL / 2);

									if (VAL(LOD) >= 5)
									{
										// Right foot
										setAmbientColor(.1f, .1f, .1f);
										setDiffuseColor(COLOR_RED);
										glTranslated(0, 1 - sizeJ, 0);
										glRotatef(RFR, 1, 0, 0);
										glScaled(1, 1, 1);
										drawSphere(sizeJ); // Joint

										setAmbientColor(.1f, .1f, .1f);
										setDiffuseColor(COLOR_GREEN);
										glTranslated(-sizeL / 2, sizeJ, -sizeL / 2);
										glScaled(1, 1, 1);
										// Foot(triangle prism)
										drawTriangle(0, 0, 0, 0, -sizeJ, sizeL, 0, 0, 1.2); // Side
										drawTriangle(0, 0, 0, 0, -sizeJ, 0, 0, -sizeJ, sizeL);
										drawTriangle(sizeL, 0, 0, sizeL, -sizeJ, sizeL, sizeL, 0, 1.2);
										drawTriangle(sizeL, 0, 0, sizeL, -sizeJ, 0, sizeL, -sizeJ, sizeL);
										drawTriangle(0, 0, 0, 0, 0, 1.2, sizeL, 0, 1.2); // Bottom
										drawTriangle(0, 0, 0, sizeL, 0, 1.2, sizeL, 0, 0);
										drawTriangle(0, 0, 0, sizeL, 0, 0, 0, -sizeJ, 0); // Back
										drawTriangle(0, -sizeJ, 0, sizeL, 0, 0, sizeL, -sizeJ, 0);
										drawTriangle(0, -sizeJ, 0, sizeL, -sizeJ, sizeL, 0, -sizeJ, sizeL); // Top
										drawTriangle(0, -sizeJ, 0, sizeL, -sizeJ, 0, sizeL, -sizeJ, sizeL);
										drawTriangle(0, -sizeJ, sizeL, sizeL, -sizeJ, sizeL, 0, 0, 1.2);
										drawTriangle(sizeL, -sizeJ, sizeL, sizeL, 0, 1.2, 0, 0, 1.2);
									}
								}
							glPopMatrix();
						}
					glPopMatrix();
				}
			glPopMatrix();
		}
	glPopMatrix();

	//*** DON'T FORGET TO PUT THIS IN YOUR OWN CODE **/
	endDraw();
}
Vec3f old_x_one;
int main()
{
	// Initialize the controls
	// Constructor is ModelerControl(name, minimumvalue, maximumvalue, 
	// stepsize, defaultvalue)
    ModelerControl controls[NUMCONTROLS];
    controls[XPOS] = ModelerControl("X Position", -5, 5, 0.1f, 0);
    controls[YPOS] = ModelerControl("Y Position", 0, 5, 0.1f, 0);
    controls[ZPOS] = ModelerControl("Z Position", -5, 5, 0.1f, 0);
	controls[LOD] = ModelerControl("Level of detail", 1, 5, 1, 5);
	controls[LAROTATE] = ModelerControl("Left Arm Rotate X", -90, 180, 1, 0);
	controls[LLAROTATE] = ModelerControl("Left Lower Arm Rotate X", 0, 90, 1, 0);
	controls[RAROTATE] = ModelerControl("Right Arm Rotate X", -90, 180, 1, 0);
	controls[RLAROTATE] = ModelerControl("Right Lower Arm Rotate X", 0, 90, 1, 0);
	controls[LLROTATE] = ModelerControl("Left Leg Rotate X", -90, 90, 1, 0);
	controls[LFROTATE] = ModelerControl("Left Foot Rotate X", -45, 10, 1, 0);
	controls[LLLROTATE] = ModelerControl("Left Lower Leg Rotate X", -90, 0, 1, 0);
	controls[RLROTATE] = ModelerControl("Right Leg Rotate X", -90, 90, 1, 0);
	controls[RLLROTATE] = ModelerControl("Right Lower Leg Rotate X", -90, 0, 1, 0);
	controls[RFROTATE] = ModelerControl("Right Foot Rotate X", -45, 10, 1, 0);
	controls[HEADTYPE] = ModelerControl("Type of Head", 1, 2, 1, 1);
	controls[HROTATE] = ModelerControl("Head Rotate Y", -90, 90, 1, 0);
	controls[LBROTATE] = ModelerControl("Lower Body Rotate Y", -90, 90, 1, 0);
	controls[HANDA] = ModelerControl("Hand?", 0, 1, 1, 0);
	controls[HEADA] = ModelerControl("Head?", 0, 1, 1, 0);
	controls[XLIGHT] = ModelerControl("X Position of Light", -5, 5, 0.10f, 0);
	controls[YLIGHT] = ModelerControl("Y Position of Light", 0, 5, 0.10f, 5);
	controls[ZLIGHT] = ModelerControl("Z Position of Light", -5, 5, 0.10f, 0);
	controls[LDEPTH] = ModelerControl("Depth of L-system", 0, 6, 1, 0);
	controls[LANGLE] = ModelerControl("Angle of L-system", 0, 359, 1, 60);

	ParticleSystem *ps = new ParticleSystem();
	ModelerApplication::Instance()->SetParticleSystem(ps);
	ParticleSystem *cloth = new ParticleSystem(true);
	ModelerApplication::Instance()->SetParticleSystemCloth(cloth);
    ModelerApplication::Instance()->Init(&createSampleModel, controls, NUMCONTROLS);
    return ModelerApplication::Instance()->Run();
}
