/*
	This file defines the layout of the constant data
	that the CPU sends to the GPU

	These must exactly match the constant buffer definitions in shader programs.
*/

#ifndef EAE6320_GRAPHICS_CONSTANTBUFFERFORMATS_H
#define EAE6320_GRAPHICS_CONSTANTBUFFERFORMATS_H

// Includes
//=========

#include "Configuration.h"

#include <Engine/Math/cMatrix_transformation.h>
#include <Engine/Math/sVector.h>
#include <Engine/Graphics/sColor.h>

// Format Definitions
//===================

namespace eae6320
{
	namespace Graphics
	{
		namespace ConstantBufferFormats
		{
			struct sPerFrame
			{
				Math::cMatrix_transformation g_transform_worldToCamera;
				Math::cMatrix_transformation g_transform_cameraToProjected;
				Math::sVector g_vector_cameraPosition;
				float padding0[1];	// For float4 alignment

				Math::sVector g_lightDirection;
				float padding1[1];	// For float4 alignment

				sColor g_directionalLight_color;
				sColor g_ambient_color;

				float g_elapsedSecondCount_systemTime = 0.0f;
				float g_elapsedSecondCount_simulationTime = 0.0f;
				float padding2[2];	// For float4 alignment
			};

			struct sPerDrawCall
			{
				Math::cMatrix_transformation g_transform_localToWorld;
				Math::cMatrix_transformation g_transform_localToProjected;

				float g_scale_x;
				float g_scale_y;

				float padding[2];	// For float4 alignment
			};

			struct sPerMaterial
			{
				sColor g_color;
			};
		}
	}
}

#endif	// EAE6320_GRAPHICS_CONSTANTBUFFERFORMATS_H
