#pragma once
/*
	A struct to hold 4 floats for color.
*/

#ifndef EAE6320_GRAPHICS_CCOLOR_H
#define EAE6320_GRAPHICS_CCOLOR_H

// Includes
//=========

// Forward Declarations
//=====================

// Class Declaration
//==================

namespace eae6320
{
	namespace Graphics
	{
		struct sColor
		{
			// Data
			//=====

			float red;
			float green;
			float blue;
			float alpha;

			// Interface
			//==========

			// Access
			//-------


			// Initialization / Clean Up
			//--------------------------
			sColor();
			sColor(float i_red, float i_green, float i_blue, float i_alpha);
			~sColor();

			// Implementation
			//===============
			void SetColor(float i_red, float i_green, float i_blue, float i_alpha);
		};
	}
}

#endif	// EAE6320_GRAPHICS_CCOLOR_H
