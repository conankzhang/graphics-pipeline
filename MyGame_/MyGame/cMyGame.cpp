// Includes
//=========

#include "cMyGame.h"

#include "cGameObject.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/UserInput/UserInput.h>
#include <Engine/Graphics/Graphics.h>
#include <Engine/Graphics/cEffect.h>
#include <Engine/Graphics/cMesh.h>
#include <Engine/Graphics/VertexFormats.h>
#include <Engine/Math/cMatrix_transformation.h>

// Inherited Implementation
//=========================

void eae6320::cMyGame::SubmitDataToBeRendered(const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_sinceLastSimulationUpdate)
{
	eae6320::Graphics::SubmitBackgroundColor(0.0f, 1.0f, 0.0f, 1.0f);
	eae6320::Graphics::SubmitCamera(m_camera->GetWorldToCameraTransform(i_elapsedSecondCount_sinceLastSimulationUpdate), m_camera->GetCameraToProjectedTransform(), i_elapsedSecondCount_systemTime, i_elapsedSecondCount_sinceLastSimulationUpdate);

	if (m_shiftPressed)
	{
		eae6320::Graphics::SubmitGameObject(m_object2->GetMesh(), m_object1->GetEffect(), m_object1->GetTransform(i_elapsedSecondCount_sinceLastSimulationUpdate));
	}
	else
	{
		eae6320::Graphics::SubmitGameObject(m_object1->GetMesh(), m_object1->GetEffect(), m_object1->GetTransform(i_elapsedSecondCount_sinceLastSimulationUpdate));
	}
	eae6320::Graphics::SubmitGameObject(m_object2->GetMesh(), m_object2->GetEffect(), m_object2->GetTransform(i_elapsedSecondCount_sinceLastSimulationUpdate));
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
	float z_camera = 0.0f;
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Up))
	{
		z_camera = -1.0f;
	}

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Down))
	{
		z_camera = 1.0f;
	}

	float x_camera = 0.0f;
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Left))
	{
		x_camera = -1.0f;
	}

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Right))
	{
		x_camera = 1.0f;
	}

	m_camera->SetVelocity(eae6320::Math::sVector(x_camera, 0.0f, z_camera));

	float x_object = 0.0f;
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::A))
	{
		x_object = -1.0f;
	}

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::D))
	{
		x_object = 1.0f;
	}

	float y_object = 0.0f;
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::W))
	{
		y_object = 1.0f;
	}

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::S))
	{
		y_object = -1.0f;
	}

	m_object1->SetVelocity(eae6320::Math::sVector(x_object, y_object, 0.0f));


	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Shift))
	{
		m_shiftPressed = true;
	}
	else
	{
		m_shiftPressed = false;
	}
}

void eae6320::cMyGame::UpdateSimulationBasedOnTime(const float i_elapsedSecondCount_sinceLastUpdate)
{
	m_object1->Update(i_elapsedSecondCount_sinceLastUpdate);
	m_object2->Update(i_elapsedSecondCount_sinceLastUpdate);
	m_camera->Update(i_elapsedSecondCount_sinceLastUpdate);
}

// Initialization / Clean Up
//--------------------------

eae6320::cResult eae6320::cMyGame::Initialize()
{
	auto result = Results::Success;

	m_object1 = new eae6320::cGameObject(eae6320::Math::sVector(), eae6320::Math::cQuaternion());
	m_object2 = new eae6320::cGameObject(eae6320::Math::sVector(), eae6320::Math::cQuaternion());
	m_camera = new eae6320::cCamera(eae6320::Math::sVector(0.0f, 0.0f, 5.0f), eae6320::Math::cQuaternion());

	// Initialize the shading data
	{
		if ( !( result = eae6320::Graphics::cEffect::Load(s_Effect1, "data/Shaders/Vertex/standard.shader", "data/Shaders/Fragment/standard.shader", 1) ) )
		{
			EAE6320_ASSERT( false );
			goto OnExit;
		}
	}

	// Initialize the geometry
	{
		std::vector<eae6320::Graphics::VertexFormats::sMesh> vertexData(6);
		vertexData[0].x = 0.0f;
		vertexData[0].y = 0.0f;
		vertexData[0].z = 0.0f;

		vertexData[1].x = 0.0f;
		vertexData[1].y = 1.0f;
		vertexData[1].z = 0.0f;

		vertexData[2].x = 1.0f;
		vertexData[2].y = 1.0f;
		vertexData[2].z = 0.0f;

		vertexData[3].x = 0.0f;
		vertexData[3].y = 0.0f;
		vertexData[3].z = 0.0f;

		vertexData[4].x = 1.0f;
		vertexData[4].y = 1.0f;
		vertexData[4].z = 0.0f;

		vertexData[5].x = 1.0f;
		vertexData[5].y = 0.0f;
		vertexData[5].z = 0.0f;

		std::vector<uint16_t> indexData(6);
		indexData[0] = 0;
		indexData[1] = 1;
		indexData[2] = 2;
		indexData[3] = 3;
		indexData[4] = 4;
		indexData[5] = 5;

		if ( !( result = eae6320::Graphics::cMesh::Load( s_Mesh1, vertexData, indexData ) ) )
		{
			EAE6320_ASSERT( false );
			goto OnExit;
		}
	}

	m_object1->SetMeshAndEffect(s_Mesh1, s_Effect1);

	// Initialize the shading data
	{
		if ( !( result = eae6320::Graphics::cEffect::Load( s_Effect2, "data/Shaders/Vertex/standard.shader", "data/Shaders/Fragment/animated.shader", 1) ) )
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

	m_object2->SetMeshAndEffect(s_Mesh2, s_Effect2);

OnExit:

	return result;
}

eae6320::cResult eae6320::cMyGame::CleanUp()
{
	auto result = Results::Success;

	if (m_object1)
	{
		m_object1->CleanUp();
		delete m_object1;
		m_object1 = nullptr;
	}

	if (m_object2)
	{
		m_object2->CleanUp();
		delete m_object2;
		m_object2 = nullptr;
	}

	if (m_camera)
	{
		delete m_camera;
		m_camera = nullptr;
	}

	return Results::Success;
}
