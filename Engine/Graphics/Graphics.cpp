// Includes
//=========

#include "Graphics.h"

#include "cConstantBuffer.h"
#include "ConstantBufferFormats.h"
#include "cRenderState.h"
#include "cShader.h"
#include "cMesh.h"
#include "cEffect.h"
#include "sColor.h"
#include "sContext.h"
#include "VertexFormats.h"
#include "View.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Concurrency/cEvent.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Platform/Platform.h>
#include <Engine/Time/Time.h>
#include <Engine/UserOutput/UserOutput.h>
#include <utility>
#include <algorithm>

// Static Data Initialization
//===========================

namespace
{
	eae6320::Graphics::View s_View;

	// Constant buffer object
	eae6320::Graphics::cConstantBuffer s_constantBuffer_perFrame( eae6320::Graphics::ConstantBufferTypes::PerFrame );
	eae6320::Graphics::cConstantBuffer s_constantBuffer_perDrawCalls( eae6320::Graphics::ConstantBufferTypes::PerDrawCall );

	// Submission Data
	//----------------

	// This struct's data is populated at submission time;
	// it must cache whatever is necessary in order to render a frame
	struct sDataRequiredToRenderAFrame
	{
		eae6320::Graphics::ConstantBufferFormats::sPerDrawCall m_constantData[65536];
		eae6320::Graphics::cMesh* m_meshes[65536];
		eae6320::Graphics::cEffect* m_effects[65536];
		eae6320::Graphics::ConstantBufferFormats::sPerFrame constantData_perFrame;
		uint64_t m_renderCommands[65536];
		uint_fast32_t m_currentBoundEffectId = 10000;
		eae6320::Graphics::sColor m_color;
		uint16_t m_renderCount;
	};
	// In our class there will be two copies of the data required to render a frame:
	//	* One of them will be getting populated by the data currently being submitted by the application loop thread
	//	* One of them will be fully populated,
	sDataRequiredToRenderAFrame s_dataRequiredToRenderAFrame[2];
	auto* s_dataBeingSubmittedByApplicationThread = &s_dataRequiredToRenderAFrame[0];
	auto* s_dataBeingRenderedByRenderThread = &s_dataRequiredToRenderAFrame[1];
	// The following two events work together to make sure that
	// the main/render thread and the application loop thread can work in parallel but stay in sync:
	// This event is signaled by the application loop thread when it has finished submitting render data for a frame
	// (the main/render thread waits for the signal)
	eae6320::Concurrency::cEvent s_whenAllDataHasBeenSubmittedFromApplicationThread;
	// This event is signaled by the main/render thread when it has swapped render data pointers.
	// This means that the renderer is now working with all the submitted data it needs to render the next frame,
	// and the application loop thread can start submitting data for the following frame
	// (the application loop thread waits for the signal)
	eae6320::Concurrency::cEvent s_whenDataForANewFrameCanBeSubmittedFromApplicationThread;
}

// Interface
//==========

// Submission
//-----------

void eae6320::Graphics::SubmitElapsedTime( const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_simulationTime )
{
	EAE6320_ASSERT( s_dataBeingSubmittedByApplicationThread );
	auto& constantData_perFrame = s_dataBeingSubmittedByApplicationThread->constantData_perFrame;
	constantData_perFrame.g_elapsedSecondCount_systemTime = i_elapsedSecondCount_systemTime;
	constantData_perFrame.g_elapsedSecondCount_simulationTime = i_elapsedSecondCount_simulationTime;
}

eae6320::cResult eae6320::Graphics::WaitUntilDataForANewFrameCanBeSubmitted( const unsigned int i_timeToWait_inMilliseconds )
{
	return Concurrency::WaitForEvent( s_whenDataForANewFrameCanBeSubmittedFromApplicationThread, i_timeToWait_inMilliseconds );
}

eae6320::cResult eae6320::Graphics::SignalThatAllDataForAFrameHasBeenSubmitted()
{
	return s_whenAllDataHasBeenSubmittedFromApplicationThread.Signal();
}

// Render
//-------

void eae6320::Graphics::RenderFrame()
{
	// Wait for the application loop to submit data to be rendered
	{
		const auto result = Concurrency::WaitForEvent( s_whenAllDataHasBeenSubmittedFromApplicationThread );
		if ( result )
		{
			// Switch the render data pointers so that
			// the data that the application just submitted becomes the data that will now be rendered
			std::swap( s_dataBeingSubmittedByApplicationThread, s_dataBeingRenderedByRenderThread );
			// Once the pointers have been swapped the application loop can submit new data
			const auto result = s_whenDataForANewFrameCanBeSubmittedFromApplicationThread.Signal();
			if ( !result )
			{
				EAE6320_ASSERTF( false, "Couldn't signal that new graphics data can be submitted" );
				Logging::OutputError( "Failed to signal that new render data can be submitted" );
				UserOutput::Print( "The renderer failed to signal to the application that new graphics data can be submitted."
					" The application is probably in a bad state and should be exited" );
				return;
			}
		}
		else
		{
			EAE6320_ASSERTF( false, "Waiting for the graphics data to be submitted failed" );
			Logging::OutputError( "Waiting for the application loop to submit data to be rendered failed" );
			UserOutput::Print( "The renderer failed to wait for the application to submit data to be rendered."
				" The application is probably in a bad state and should be exited" );
			return;
		}
	}

	EAE6320_ASSERT( s_dataBeingRenderedByRenderThread );
	std::sort(&s_dataBeingRenderedByRenderThread->m_renderCommands[0], &s_dataBeingRenderedByRenderThread->m_renderCommands[0] + s_dataBeingRenderedByRenderThread->m_renderCount);

	// Update the per-frame constant buffer
	{
		// Copy the data from the system memory that the application owns to GPU memory
		auto& constantData_perFrame = s_dataBeingRenderedByRenderThread->constantData_perFrame;
		s_constantBuffer_perFrame.Update( &constantData_perFrame );
	}
	size_t t = sizeof(*s_dataBeingRenderedByRenderThread);
	sColor clearColor = s_dataBeingRenderedByRenderThread->m_color;
	s_View.ClearColor(clearColor.red, clearColor.green, clearColor.blue, clearColor.alpha);

	for (uint16_t i = 0; i < s_dataBeingRenderedByRenderThread->m_renderCount; ++i)
	{
		DrawCommand drawCommand = *(DrawCommand *)&s_dataBeingRenderedByRenderThread->m_renderCommands[i];
		if(drawCommand.nCommand == RenderCommand::Draw)
		{
			// Copy the data from the system memory that the application owns to GPU memory
			auto& constantData_perDrawCall = s_dataBeingRenderedByRenderThread->m_constantData[drawCommand.nSubmitIndex];
			s_constantBuffer_perDrawCalls.Update( &constantData_perDrawCall );

			// Make sure to bind effect before rendering mesh
			if (s_dataBeingRenderedByRenderThread->m_currentBoundEffectId != drawCommand.nEffectId)
			{
				cEffect::s_manager.UnsafeGet(drawCommand.nEffectId)->RenderFrame();
				s_dataBeingRenderedByRenderThread->m_currentBoundEffectId = drawCommand.nEffectId;
			}

			cMesh::s_manager.UnsafeGet(drawCommand.nMeshId)->RenderFrame();
		}
	}

	//for (uint16_t i = 0; i < s_dataBeingRenderedByRenderThread->m_renderCount; ++i)
	//{
	//	if (s_dataBeingRenderedByRenderThread->m_effects[i])
	//	{
	//		s_dataBeingRenderedByRenderThread->m_effects[i]->DecrementReferenceCount();
	//		s_dataBeingRenderedByRenderThread->m_effects[i] = nullptr;
	//	}

	//	if (s_dataBeingRenderedByRenderThread->m_meshes[i])
	//	{
	//		s_dataBeingRenderedByRenderThread->m_meshes[i]->DecrementReferenceCount();
	//		s_dataBeingRenderedByRenderThread->m_meshes[i] = nullptr;
	//	}
	//}

	s_dataBeingRenderedByRenderThread->m_renderCount = 0;

	s_View.Swap();

	// Once everything has been drawn the data that was submitted for this frame
	// should be cleaned up and cleared.
	// so that the struct can be re-used (i.e. so that data for a new frame can be submitted to it)
	{
		// (At this point in the class there isn't anything that needs to be cleaned up)
	}
}

// Initialization / Clean Up
//--------------------------

eae6320::cResult eae6320::Graphics::Initialize( const sInitializationParameters& i_initializationParameters )
{
	auto result = Results::Success;

	// Initialize the platform-specific context
	if ( !( result = sContext::g_context.Initialize( i_initializationParameters ) ) )
	{
		EAE6320_ASSERT( false );
		goto OnExit;
	}
	// Initialize the asset managers
	{
		if ( !( result = cShader::s_manager.Initialize() ) )
		{
			EAE6320_ASSERT( false );
			goto OnExit;
		}
	}

	{
		if ( !( result = cMesh::s_manager.Initialize() ) )
		{
			EAE6320_ASSERT( false );
			goto OnExit;
		}
	}

	{
		if ( !( result = cEffect::s_manager.Initialize() ) )
		{
			EAE6320_ASSERT( false );
			goto OnExit;
		}
	}

	// Initialize the platform-independent graphics objects
	{
		if ( result = s_constantBuffer_perFrame.Initialize() )
		{
			// There is only a single per-frame constant buffer that is re-used
			// and so it can be bound at initialization time and never unbound
			s_constantBuffer_perFrame.Bind(
				// In our class both vertex and fragment shaders use per-frame constant data
				ShaderTypes::Vertex | ShaderTypes::Fragment );
		}
		else
		{
			EAE6320_ASSERT( false );
			goto OnExit;
		}
	}

	// Initialize the platform-independent graphics objects
	{
		if ( result = s_constantBuffer_perDrawCalls.Initialize() )
		{
			// There is only a single per-frame constant buffer that is re-used
			// and so it can be bound at initialization time and never unbound
			s_constantBuffer_perDrawCalls.Bind(
				// In our class both vertex and fragment shaders use per-frame constant data
				ShaderTypes::Vertex | ShaderTypes::Fragment );
		}
		else
		{
			EAE6320_ASSERT( false );
			goto OnExit;
		}
	}

	// Initialize the events
	{
		if ( !( result = s_whenAllDataHasBeenSubmittedFromApplicationThread.Initialize( Concurrency::EventType::ResetAutomaticallyAfterBeingSignaled ) ) )
		{
			EAE6320_ASSERT( false );
			goto OnExit;
		}
		if ( !( result = s_whenDataForANewFrameCanBeSubmittedFromApplicationThread.Initialize( Concurrency::EventType::ResetAutomaticallyAfterBeingSignaled,
			Concurrency::EventState::Signaled ) ) )
		{
			EAE6320_ASSERT( false );
			goto OnExit;
		}
	}

	// Initialize the views
	{
		result = s_View.InitializeViews(i_initializationParameters);
		if ( !( result ) )
		{
			EAE6320_ASSERT( false );
			goto OnExit;
		}
	}

OnExit:

	return result;
}

eae6320::cResult eae6320::Graphics::CleanUp()
{
	auto result = Results::Success;

	s_View.CleanUp();

	{
		const auto localResult = s_constantBuffer_perFrame.CleanUp();
		if ( !localResult )
		{
			EAE6320_ASSERT( false );
			if ( result )
			{
				result = localResult;
			}
		}
	}

	{
		const auto localResult = s_constantBuffer_perDrawCalls.CleanUp();
		if ( !localResult )
		{
			EAE6320_ASSERT( false );
			if ( result )
			{
				result = localResult;
			}
		}
	}

	for (uint16_t i = 0; i < s_dataBeingRenderedByRenderThread->m_renderCount; ++i)
	{
		if (s_dataBeingRenderedByRenderThread->m_effects[i])
		{
			s_dataBeingRenderedByRenderThread->m_effects[i]->DecrementReferenceCount();
			s_dataBeingRenderedByRenderThread->m_effects[i] = nullptr;
		}

		if (s_dataBeingRenderedByRenderThread->m_meshes[i])
		{
			s_dataBeingRenderedByRenderThread->m_meshes[i]->DecrementReferenceCount();
			s_dataBeingRenderedByRenderThread->m_meshes[i] = nullptr;
		}
	}

	for (uint16_t i = 0; i < s_dataBeingSubmittedByApplicationThread->m_renderCount; ++i)
	{
		if (s_dataBeingSubmittedByApplicationThread->m_effects[i])
		{
			s_dataBeingSubmittedByApplicationThread->m_effects[i]->DecrementReferenceCount();
			s_dataBeingSubmittedByApplicationThread->m_effects[i] = nullptr;
		}

		if (s_dataBeingSubmittedByApplicationThread->m_meshes[i])
		{
			s_dataBeingSubmittedByApplicationThread->m_meshes[i]->DecrementReferenceCount();
			s_dataBeingSubmittedByApplicationThread->m_meshes[i] = nullptr;
		}
	}

	s_dataBeingRenderedByRenderThread->m_renderCount = 0;
	s_dataBeingSubmittedByApplicationThread->m_renderCount = 0;

	{
		const auto localResult = cShader::s_manager.CleanUp();
		if ( !localResult )
		{
			EAE6320_ASSERT( false );
			if ( result )
			{
				result = localResult;
			}
		}
	}

	{
		const auto localResult = cMesh::s_manager.CleanUp();
		if ( !localResult )
		{
			EAE6320_ASSERT( false );
			if ( result )
			{
				result = localResult;
			}
		}
	}

	{
		const auto localResult = cEffect::s_manager.CleanUp();
		if ( !localResult )
		{
			EAE6320_ASSERT( false );
			if ( result )
			{
				result = localResult;
			}
		}
	}

	{
		const auto localResult = sContext::g_context.CleanUp();
		if ( !localResult )
		{
			EAE6320_ASSERT( false );
			if ( result )
			{
				result = localResult;
			}
		}
	}

	return result;
}

void eae6320::Graphics::SubmitBackgroundColor(const sColor& i_color)
{
	s_dataBeingSubmittedByApplicationThread->m_color = i_color;
}

void eae6320::Graphics::SubmitGameObject(eae6320::Graphics::cMesh* i_mesh, eae6320::Graphics::cEffect* i_effect, eae6320::Math::cMatrix_transformation& i_transform)
{
	i_mesh->IncrementReferenceCount();
	s_dataBeingSubmittedByApplicationThread->m_meshes[s_dataBeingSubmittedByApplicationThread->m_renderCount] = i_mesh;

	i_effect->IncrementReferenceCount();
	s_dataBeingSubmittedByApplicationThread->m_effects[s_dataBeingSubmittedByApplicationThread->m_renderCount] = i_effect;

	s_dataBeingSubmittedByApplicationThread->m_constantData[s_dataBeingSubmittedByApplicationThread->m_renderCount].g_transform_localToWorld = i_transform;

	s_dataBeingSubmittedByApplicationThread->m_renderCount++;
}

void eae6320::Graphics::SubmitCamera(eae6320::Math::cMatrix_transformation i_transform_worldToCamera, eae6320::Math::cMatrix_transformation i_transform_cameraToProjected, const Math::sVector& i_vector_cameraPosition, float i_elapsedSecondCount_systemTime, float i_elapsedSecondCount_simulationTime)
{
	s_dataBeingSubmittedByApplicationThread->constantData_perFrame.g_transform_worldToCamera = i_transform_worldToCamera;
	s_dataBeingSubmittedByApplicationThread->constantData_perFrame.g_transform_cameraToProjected = i_transform_cameraToProjected;
	s_dataBeingSubmittedByApplicationThread->constantData_perFrame.g_elapsedSecondCount_systemTime = i_elapsedSecondCount_systemTime;
	s_dataBeingSubmittedByApplicationThread->constantData_perFrame.g_elapsedSecondCount_simulationTime = i_elapsedSecondCount_simulationTime;
	s_dataBeingSubmittedByApplicationThread->constantData_perFrame.g_vector_cameraPosition = i_vector_cameraPosition;;
}

void eae6320::Graphics::SubmitDrawCommand(RenderCommand i_command, unsigned int i_effectId, unsigned int i_distance, unsigned int i_meshId, eae6320::Math::cMatrix_transformation& i_transform_localToWorld, const Math::cMatrix_transformation& i_transform_localToProjected)
{
	DrawCommand drawCommand;
	drawCommand.nCommand = i_command;
	drawCommand.nEffectId = i_effectId;
	drawCommand.nDistance = i_distance;
	drawCommand.nMeshId = i_meshId;
	drawCommand.nSubmitIndex = s_dataBeingSubmittedByApplicationThread->m_renderCount;

	s_dataBeingSubmittedByApplicationThread->m_constantData[s_dataBeingSubmittedByApplicationThread->m_renderCount].g_transform_localToWorld = i_transform_localToWorld;
	s_dataBeingSubmittedByApplicationThread->m_constantData[s_dataBeingSubmittedByApplicationThread->m_renderCount].g_transform_localToProjected = i_transform_localToProjected;
	s_dataBeingSubmittedByApplicationThread->m_renderCommands[s_dataBeingSubmittedByApplicationThread->m_renderCount] = *(uint64_t*)&drawCommand;

	s_dataBeingSubmittedByApplicationThread->m_renderCount++;
}
