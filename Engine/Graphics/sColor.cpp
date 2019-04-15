// Includes
//=========

#include "sColor.h"

// Static Data Initialization
//===========================

// Interface
//==========

// Initialization / Clean Up
//--------------------------

eae6320::Graphics::sColor::sColor() :
	red(1.0f),
	green(1.0f),
	blue(1.0f),
	alpha(1.0f)
{

}

eae6320::Graphics::sColor::sColor(float i_red, float i_green, float i_blue, float i_alpha) :
	red(i_red),
	green(i_green),
	blue(i_blue),
	alpha(i_alpha)
{

}

eae6320::Graphics::sColor::~sColor()
{

}
