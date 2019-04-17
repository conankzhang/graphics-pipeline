/*
	This file declares the external interface for the graphics system
*/

#ifndef EAE6320_GRAPHICS_H
#define EAE6320_GRAPHICS_H

// Includes
//=========

#include "Configuration.h"

#include <cstdint>
#include <Engine/Results/Results.h>

#include "cMesh.h"
#include "cMaterial.h"

#if defined( EAE6320_PLATFORM_WINDOWS )
	#include <Engine/Windows/Includes.h>
#endif

// Forward Declarations
//=====================

namespace eae6320
{
	namespace Graphics
	{
		class cMesh;
		class cEffect;
		struct sColor;
	}

	namespace Math
	{
		class cMatrix_transformation;
		struct sVector;
	}
}

// Interface
//==========

namespace eae6320
{
	namespace Graphics
	{
		enum class RenderCommand : unsigned int
		{
			IndependentDraw,
			DependentDraw,
		};

		struct DrawCommand
		{
			unsigned int nSubmitIndex: 8;
			unsigned int nMeshId: 8;
			unsigned int nPriority3: 8;
			unsigned int nPriority2: 8;
			unsigned int nPriority1: 8;
			RenderCommand nCommand : 8;
		};

		// Submission
		//-----------

		// These functions should be called from the application (on the application loop thread)

		// As the class progresses you will add your own functions for submitting data,
		// but the following is an example (that gets called automatically)
		// of how the application submits the total elapsed times
		// for the frame currently being submitted
		void SubmitElapsedTime( const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_simulationTime );

		// When the application is ready to submit data for a new frame
		// it should call this before submitting anything
		// (or, said another way, it is not safe to submit data for a new frame
		// until this function returns successfully)
		cResult WaitUntilDataForANewFrameCanBeSubmitted( const unsigned int i_timeToWait_inMilliseconds );
		// When the application has finished submitting data for a frame
		// it must call this function
		cResult SignalThatAllDataForAFrameHasBeenSubmitted();

		// Render
		//-------

		// This is called (automatically) from the main/render thread.
		// It will render a submitted frame as soon as it is readyshort
		// (i.e. as soon as SignalThatAllDataForAFrameHasBeenSubmitted() has been called)
		void RenderFrame();

		// Initialization / Clean Up
		//--------------------------

		struct sInitializationParameters
		{
#if defined( EAE6320_PLATFORM_WINDOWS )
			HWND mainWindow = NULL;
	#if defined( EAE6320_PLATFORM_D3D )
			uint16_t resolutionWidth, resolutionHeight;
	#elif defined( EAE6320_PLATFORM_GL )
			HINSTANCE thisInstanceOfTheApplication = NULL;
	#endif
#endif
		};

		cResult Initialize( const sInitializationParameters& i_initializationParameters );
		cResult CleanUp();

		void SubmitBackgroundColor(const sColor& i_color);
		void SubmitCamera(Math::cMatrix_transformation i_transform_worldToCamera, Math::cMatrix_transformation i_transform_cameraToProjected, const Math::sVector& i_vector_cameraPasition, float i_elapsedSecondCount_systemTime, float i_elapsedSecondCount_simulationTime);
		void SubmitDrawCommand(unsigned int i_distance, const cMesh::Handle& i_mesh, const cMaterial::Handle& i_material, Math::cMatrix_transformation& i_transform_localToWorld, const Math::cMatrix_transformation& i_transform_localToProjected);
	}
}

#endif	// EAE6320_GRAPHICS_H
