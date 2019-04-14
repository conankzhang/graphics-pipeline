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
#include <Engine/EntityComponentSystem/IEntity.h>

// Inherited Implementation
//=========================

void eae6320::cMyGame::SubmitDataToBeRendered(const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_sinceLastSimulationUpdate)
{
	eae6320::Graphics::SubmitBackgroundColor(0.13f, 0.24f, 0.33f, 1.0f);
	eae6320::Graphics::SubmitCamera(m_camera->GetWorldToCameraTransform(i_elapsedSecondCount_sinceLastSimulationUpdate), m_camera->GetCameraToProjectedTransform(), i_elapsedSecondCount_systemTime, i_elapsedSecondCount_sinceLastSimulationUpdate);

	eae6320::Graphics::SubmitDrawCommand(eae6320::Graphics::RenderCommand::Draw, m_object1->GetEffect(), m_camera->CalculateNormalizedCameraDistance(m_object1->GetPosition()), m_object1->GetMesh(), m_object1->GetTransform(i_elapsedSecondCount_sinceLastSimulationUpdate));
	eae6320::Graphics::SubmitDrawCommand(eae6320::Graphics::RenderCommand::Draw, m_object2->GetEffect(), m_camera->CalculateNormalizedCameraDistance(m_object2->GetPosition()), m_object2->GetMesh(), m_object2->GetTransform(i_elapsedSecondCount_sinceLastSimulationUpdate));
	eae6320::Graphics::SubmitDrawCommand(eae6320::Graphics::RenderCommand::Draw, m_object3->GetEffect(), m_camera->CalculateNormalizedCameraDistance(m_object3->GetPosition()), m_object3->GetMesh(), m_object3->GetTransform(i_elapsedSecondCount_sinceLastSimulationUpdate));
	eae6320::Graphics::SubmitDrawCommand(eae6320::Graphics::RenderCommand::Draw, m_object4->GetEffect(), m_camera->CalculateNormalizedCameraDistance(m_object4->GetPosition()), m_object4->GetMesh(), m_object4->GetTransform(i_elapsedSecondCount_sinceLastSimulationUpdate));
	eae6320::Graphics::SubmitDrawCommand(eae6320::Graphics::RenderCommand::Draw, m_object5->GetEffect(), m_camera->CalculateNormalizedCameraDistance(m_object5->GetPosition()), m_object5->GetMesh(), m_object5->GetTransform(i_elapsedSecondCount_sinceLastSimulationUpdate));
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
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::W))
	{
		z_camera = cameraMoveSpeed;
	}

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::S))
	{
		z_camera = -cameraMoveSpeed;
	}

	float x_camera = 0.0f;
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::A))
	{
		x_camera = -cameraMoveSpeed;
	}

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::D))
	{
		x_camera = cameraMoveSpeed;
	}

	float y_camera = 0.0f;
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Up))
	{
		y_camera = cameraMoveSpeed;
	}

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Down))
	{
		y_camera = -cameraMoveSpeed;
	}

	Math::sVector forward = m_camera->GetForward() * z_camera;

	Math::sVector lateral = Math::Cross(m_camera->GetForward(), Math::sVector(0.0f, 1.0f, 0.0f));
	lateral *= x_camera;

	Math::sVector vertical = Math::sVector(0.0f, 1.0f, 0.0f);
	vertical *= y_camera;

	(forward + lateral + vertical);

	m_camera->SetVelocity(forward + lateral + vertical);

	float angularSpeed = 0.0f;
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Left))
	{
		angularSpeed += 1.0f;
	}

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Right))
	{
		angularSpeed -= 1.0f;
	}

	m_camera->SetAngularSpeed(angularSpeed);
}

void eae6320::cMyGame::UpdateSimulationBasedOnTime(const float i_elapsedSecondCount_sinceLastUpdate)
{
	m_object1->Update(i_elapsedSecondCount_sinceLastUpdate);
	m_object2->Update(i_elapsedSecondCount_sinceLastUpdate);
	m_object3->Update(i_elapsedSecondCount_sinceLastUpdate);
	m_object4->Update(i_elapsedSecondCount_sinceLastUpdate);
	m_object5->Update(i_elapsedSecondCount_sinceLastUpdate);
	m_camera->Update(i_elapsedSecondCount_sinceLastUpdate);
}

// Initialization / Clean Up
//--------------------------

eae6320::cResult eae6320::cMyGame::Initialize()
{
	auto result = Results::Success;

	m_object1 = new eae6320::cGameObject(eae6320::Math::sVector(-1.5f, 0.0f, 0.0f), eae6320::Math::cQuaternion(), "data/Meshes/sphere.mesh", "data/Effects/white.effect");
	m_object2 = new eae6320::cGameObject(eae6320::Math::sVector(-0.5f, 0.0f, 0.0f), eae6320::Math::cQuaternion(), "data/Meshes/sphere.mesh", "data/Effects/red.effect");
	m_object3 = new eae6320::cGameObject(eae6320::Math::sVector(0.5f, 0.0f, 0.0f), eae6320::Math::cQuaternion(), "data/Meshes/sphere.mesh", "data/Effects/white.effect");
	m_object4 = new eae6320::cGameObject(eae6320::Math::sVector(1.5f, 0.0f, 0.0f), eae6320::Math::cQuaternion(), "data/Meshes/sphere.mesh", "data/Effects/red.effect");
	m_object5 = new eae6320::cGameObject(eae6320::Math::sVector(0.0f, -1.0f, 0.0f), eae6320::Math::cQuaternion(), "data/Meshes/plane.mesh", "data/Effects/white.effect");


	m_camera = new eae6320::cCamera(eae6320::Math::sVector(0.0f, 0.0f, 5.0f), eae6320::Math::cQuaternion());

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

	if (m_object3)
	{
		m_object3->CleanUp();
		delete m_object3;
		m_object3 = nullptr;
	}

	if (m_object4)
	{
		m_object4->CleanUp();
		delete m_object4;
		m_object4 = nullptr;
	}

	if (m_object5)
	{
		m_object5->CleanUp();
		delete m_object5;
		m_object5 = nullptr;
	}

	if (m_camera)
	{
		delete m_camera;
		m_camera = nullptr;
	}

	return Results::Success;
}
