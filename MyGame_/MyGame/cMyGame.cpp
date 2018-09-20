// Includes
//=========

#include "cMyGame.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/UserInput/UserInput.h>
#include <Engine/Graphics/Graphics.h>
#include <Engine/Graphics/cEffect.h>
#include <Engine/Graphics/cMesh.h>
#include <Engine/Graphics/VertexFormats.h>

// Inherited Implementation
//=========================

void eae6320::cMyGame::SubmitDataToBeRendered(const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_sinceLastSimulationUpdate)
{
	eae6320::Graphics::SubmitBackgroundColor(0.0f, 1.0f, 0.0f, 1.0f);
	if (!m_f1IsPressed)
	{
		eae6320::Graphics::SubmitMeshAndEffect(s_Mesh1, s_Effect1);
	}

	m_f2IsPressed ? eae6320::Graphics::SubmitMeshAndEffect(s_Mesh2, s_Effect1) : eae6320::Graphics::SubmitMeshAndEffect(s_Mesh2, s_Effect2);
}

// Run
//----

void eae6320::cMyGame::UpdateBasedOnInput()
{
	// Is the user pressing the ESC key?
	if ( UserInput::IsKeyPressed( UserInput::KeyCodes::Escape ) )
	{
		// Exit the application
		const auto result = Exit( EXIT_SUCCESS );
		EAE6320_ASSERT( result );
	}
}

void eae6320::cMyGame::UpdateSimulationBasedOnInput()
{
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::F1))
	{
		m_f1IsPressed = true;
	}
	else
	{
		m_f1IsPressed = false;
	}

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::F2))
	{

		m_f2IsPressed = true;
	}
	else
	{
		m_f2IsPressed = false;
	}
}

// Initialization / Clean Up
//--------------------------

eae6320::cResult eae6320::cMyGame::Initialize()
{
	auto result = Results::Success;

	// Initialize the shading data
	{
		if ( !( result = eae6320::Graphics::cEffect::Load(s_Effect1, "data/Shaders/Vertex/standard.shader", "data/Shaders/Fragment/standard.shader") ) )
		{
			EAE6320_ASSERT( false );
			goto OnExit;
		}
	}

	// Initialize the geometry
	{
		std::vector<eae6320::Graphics::VertexFormats::sMesh> vertexData(3);
		vertexData[0].x = 0.0f;
		vertexData[0].y = 0.0f;
		vertexData[0].z = 0.0f;

		vertexData[1].x = 1.0f;
		vertexData[1].y = 1.0f;
		vertexData[1].z = 0.0f;

		vertexData[2].x = 1.0f;
		vertexData[2].y = 0.0f;
		vertexData[2].z = 0.0f;

		std::vector<uint16_t> indexData(3);
		indexData[0] = 0;
		indexData[1] = 1;
		indexData[2] = 2;

		if ( !( result = eae6320::Graphics::cMesh::Load( s_Mesh1, vertexData, indexData ) ) )
		{
			EAE6320_ASSERT( false );
			goto OnExit;
		}
	}

	// Initialize the shading data
	{
		if ( !( result = eae6320::Graphics::cEffect::Load( s_Effect2, "data/Shaders/Vertex/standard.shader", "data/Shaders/Fragment/animated.shader") ) )
		{
			EAE6320_ASSERT( false );
			goto OnExit;
		}
	}
	// Initialize the geometry
	{
		std::vector<eae6320::Graphics::VertexFormats::sMesh> vertexData(3);
		vertexData[0].x = 0.0f;
		vertexData[0].y = 0.0f;
		vertexData[0].z = 0.0f;

		vertexData[1].x = 0.0f;
		vertexData[1].y = 1.0f;
		vertexData[1].z = 0.0f;

		vertexData[2].x = 1.0f;
		vertexData[2].y = 1.0f;
		vertexData[2].z = 0.0f;

		std::vector<uint16_t> indexData(3);
		indexData[0] = 0;
		indexData[1] = 1;
		indexData[2] = 2;

		if ( !( result =  eae6320::Graphics::cMesh::Load( s_Mesh2, vertexData, indexData ) ) )
		{
			EAE6320_ASSERT( false );
			goto OnExit;
		}
	}

OnExit:

	return result;
}

eae6320::cResult eae6320::cMyGame::CleanUp()
{
	auto result = Results::Success;

	s_Mesh1->DecrementReferenceCount();
	s_Mesh1 = nullptr;

	s_Effect1->DecrementReferenceCount();
	s_Effect1 = nullptr;

	s_Mesh2->DecrementReferenceCount();
	s_Mesh2 = nullptr;

	s_Effect2->DecrementReferenceCount();
	s_Effect2 = nullptr;

	return Results::Success;
}
