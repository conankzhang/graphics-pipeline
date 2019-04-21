/*
	A vertex format determines the layout of the geometric data
	that the CPU will send to the GPU
*/

#ifndef EAE6320_GRAPHICS_VERTEXFORMATS_H
#define EAE6320_GRAPHICS_VERTEXFORMATS_H

// Includes
//=========

#include "Configuration.h"

// Vertex Formats
//===============

namespace eae6320
{
	namespace Graphics
	{
		namespace VertexFormats
		{
			// In our class we will only have a single vertex format for all meshes.
			// In a real game it would be more common to have several different formats
			// (with simpler/smaller formats for simpler shading
			// and more complex and bigger formats for more complicated shading).
			struct sMesh
			{
				// POSITION
				// 3 floats == 12 bytes
				// Offset = 0
				float x, y, z;

				// TEXTURE_COORDINATES
				// 2 floats == 8 bytes
				// Offset = 0
				float u, v;

				// NORMAL
				// 3 floats == 12 bytes
				// Offset = 0
				float nx, ny, nz;

				//	* TANGENT	-> tx, ty, -tz
				float tx, ty, tz;

				//	* BITANGENT	-> -btx, -bty, btz
				float btx, bty, btz;
			};

			struct sSprite
			{
				// POSITION
				int8_t x, y;

				// TEXTURE_COORDINATES
				uint8_t u, v;
			};
		}
	}
}

#endif	// EAE6320_GRAPHICS_VERTEXFORMATS_H
