// Includes
//=========

#include "sColor.h"

#include <cmath>

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

void eae6320::Graphics::sColor::SetColor(float i_red, float i_green, float i_blue, float i_alpha)
{
	red = i_red;
	green = i_green;
	blue = i_blue;
	alpha = i_alpha;
}

eae6320::Graphics::sColor eae6320::Graphics::sColor::GetLinearColor()
{
	return sColor(CalculateLinearValue(red), CalculateLinearValue(green), CalculateLinearValue(blue), CalculateLinearValue(alpha));
}

float eae6320::Graphics::sColor::CalculateLinearValue(float i_sRGB)
{
	if (i_sRGB < 0.04045f)
	{
		return i_sRGB * (1.0f / 12.92f);
	}

	return pow((i_sRGB + 0.055f) * (1.0f / 1.055f), 2.4f);
}
