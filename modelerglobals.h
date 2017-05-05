#ifndef _MODELER_GLOBALS_H
#define _MODELER_GLOBALS_H

#ifndef M_PI
#define M_PI 3.141592653589793238462643383279502
#endif

#ifndef M_PHI
#define M_PHI 1.6180339887498948482
#endif

#ifndef M_PHI2
#define M_PHI2 0.6180339887498948482
#endif

// This is a list of the controls for the SampleModel
// We'll use these constants to access the values 
// of the controls from the user interface.
enum SampleModelControls
{ 
	XPOS, YPOS, ZPOS, LOD, LAROTATE, LLAROTATE, RAROTATE, RLAROTATE, LLROTATE, LLLROTATE, LFROTATE, RLROTATE, RLLROTATE, RFROTATE, HEADTYPE, HROTATE, LBROTATE, HANDA, HEADA, XLIGHT, YLIGHT, ZLIGHT, LDEPTH, LANGLE, Y1, Y2, Y3, Y4, Z1, Z2, NUMCONTROLS
};

// Colors
#define COLOR_RED		1.0f, 0.0f, 0.0f
#define COLOR_GREEN		0.0f, 1.0f, 0.0f
#define COLOR_BLUE		0.0f, 0.0f, 1.0f

// We'll be getting the instance of the application a lot; 
// might as well have it as a macro.
#define VAL(x) (ModelerApplication::Instance()->GetControlValue(x))

#endif