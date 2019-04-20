// Includes
//=========

#include "cMyGame.h"

#include "cGameObject.h"
#include "cSpriteObject.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/UserInput/UserInput.h>
#include <Engine/Graphics/Graphics.h>
#include <Engine/Math/cMatrix_transformation.h>

// Inherited Implementation
//=========================

void eae6320::cMyGame::SubmitDataToBeRendered(const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_sinceLastSimulationUpdate)
{
	Graphics::SubmitBackgroundColor(clearColor.GetLinearColor());
	Graphics::SubmitLighting(ambientColor.GetLinearColor(), directionalLightColor.GetLinearColor(), lightDirection.GetNormalized(), m_player->GetPosition(), pointLightColor.GetLinearColor());

	Math::cMatrix_transformation transform_worldToCamera = m_camera->GetWorldToCameraTransform(i_elapsedSecondCount_sinceLastSimulationUpdate);
	Math::cMatrix_transformation transform_cameraToProjected = m_camera->GetCameraToProjectedTransform();
	Graphics::SubmitCamera(transform_worldToCamera, transform_cameraToProjected, m_camera->GetPosition(), i_elapsedSecondCount_systemTime, i_elapsedSecondCount_sinceLastSimulationUpdate);

	Math::cMatrix_transformation transform_worldToProjected = transform_cameraToProjected * transform_worldToCamera;

	//Math::cMatrix_transformation sprite_transform_localToWorld = m_sprite->GetTransform(i_elapsedSecondCount_sinceLastSimulationUpdate);
	//Graphics::SubmitSpriteCommand(m_sprite->GetScale(), m_sprite->GetMaterial(), sprite_transform_localToWorld);
	//sprite_transform_localToWorld = m_spritePad->GetTransform(i_elapsedSecondCount_sinceLastSimulationUpdate);
	//Graphics::SubmitSpriteCommand(m_spritePad->GetScale(), m_spritePad->GetMaterial(), sprite_transform_localToWorld);

	for (auto GameObject : m_gameObjects)
	{
		Math::cMatrix_transformation transform_localToWorld = GameObject->GetTransform(i_elapsedSecondCount_sinceLastSimulationUpdate);
		Graphics::SubmitDrawCommand(m_camera->CalculateNormalizedCameraDistance(GameObject->GetPosition()), GameObject->GetMesh(), GameObject->GetMaterial(), transform_localToWorld, transform_worldToProjected * transform_localToWorld);
	}
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

	float z_camera = 0.0f;
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::W))
	{
		z_camera = cameraMoveSpeed;
	}

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::S))
	{
		z_camera = -cameraMoveSpeed;
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
		angularSpeed += 0.5f;
	}

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Right))
	{
		angularSpeed -= 0.5f;
	}

	m_camera->SetAngularSpeed(angularSpeed);

	float x_player = 0.0f;
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::J))
	{
		x_player = -playerMoveSpeed;
	}

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::L))
	{
		x_player = playerMoveSpeed;
	}

	float y_player = 0.0f;
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::U))
	{
		y_player = playerMoveSpeed;
	}

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::O))
	{
		y_player = -playerMoveSpeed;
	}

	float z_player = 0.0f;
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::I))
	{
		z_player = playerMoveSpeed;
	}

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::K))
	{
		z_player = -playerMoveSpeed;
	}

	Math::sVector forward_player = m_camera->GetForward() * z_player;

	Math::sVector lateral_player = Math::Cross(m_camera->GetForward(), Math::sVector(0.0f, 1.0f, 0.0f));
	lateral_player *= x_player;

	Math::sVector vertical_player = Math::sVector(0.0f, 1.0f, 0.0f);
	vertical_player *= y_player;

	(forward_player + lateral_player + vertical_player);

	if (m_player)
	{
		m_player->SetVelocity(forward_player + lateral_player + vertical_player);
	}

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::F1))
	{
		if (lightDirection.x > -lightClamp)
		{
			lightDirection.x += -lightMoveSpeed;
		}
	}

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::F2))
	{
		if (lightDirection.x < lightClamp)
		{
			lightDirection.x += lightMoveSpeed;
		}
	}

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::F3))
	{
		if (lightDirection.y > -lightClamp)
		{
			lightDirection.y += -lightMoveSpeed;
		}
	}

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::F4))
	{
		if (lightDirection.y < lightClamp)
		{
			lightDirection.y += lightMoveSpeed;
		}
	}

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::F5))
	{
		if (lightDirection.z > -lightClamp)
		{
			lightDirection.z += -lightMoveSpeed;
		}
	}

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::F6))
	{
		if (lightDirection.z < lightClamp)
		{
			lightDirection.z += lightMoveSpeed;
		}
	}
}

void eae6320::cMyGame::UpdateSimulationBasedOnTime(const float i_elapsedSecondCount_sinceLastUpdate)
{
	if (m_player)
	{
		m_player->Update(i_elapsedSecondCount_sinceLastUpdate);
	}

	if (m_camera)
	{
		m_camera->Update(i_elapsedSecondCount_sinceLastUpdate);
	}
}

eae6320::cResult eae6320::cMyGame::DeleteGameObject(cGameObject* i_gameObject)
{
	auto result = Results::Success;

	if (i_gameObject)
	{
		const auto localResult = i_gameObject->CleanUp();

		if ( !localResult )
		{
			EAE6320_ASSERT( false );
			if ( result )
			{
				result = localResult;
			}
		}

		delete i_gameObject;
		i_gameObject = nullptr;
	}

	return result;
}

// Initialization / Clean Up
//--------------------------

eae6320::cResult eae6320::cMyGame::Initialize()
{
	auto result = Results::Success;

	clearColor.SetColor(0.13f, 0.24f, 0.33f, 1.0f);
	ambientColor.SetColor(0.1f, 0.1f, 0.1f, 1.0f);
	directionalLightColor.SetColor(1.0f, 1.0f, 1.0f, 1.0f);
	pointLightColor.SetColor(1.0f, 1.0f, 1.0f, 1.0f);

	lightDirection.x = 1.0f;
	lightDirection.y = 1.0f;
	lightDirection.z = 1.0f;

	m_player = new cGameObject(Math::sVector(0.0f, 0.0f, 1.0f), Math::cQuaternion(), "data/Meshes/smallSphere.mesh", "data/Materials/unlit.material");
	m_gameObjects.push_back(m_player);

	m_camera = new cCamera(Math::sVector(0.0f, 0.0f, 10.0f), Math::cQuaternion());
	//m_sprite = new cSpriteObject(Math::sVector(0.7f, 0.7f, 0.0f), Math::sVector(0.3f, 0.3f, 1.0f), Math::cQuaternion(), "data/Materials/sprite.material");
	//m_spritePad = new cSpriteObject(Math::sVector(-0.7f, 0.7f, 0.0f), Math::sVector(0.3f, 0.3f, 1.0f), Math::cQuaternion(), "data/Materials/spritePad.material");

	// Grid
	m_gameObjects.push_back( new cGameObject(Math::sVector(-3.0f, 0.0f, 0.0f), Math::cQuaternion(), "data/Meshes/sphere.mesh", "data/Materials/brick.material") );
	m_gameObjects.push_back( new cGameObject(Math::sVector(0.0f, 0.0f, 0.0f), Math::cQuaternion(), "data/Meshes/quad.mesh", "data/Materials/brick.material") );
	m_gameObjects.push_back( new cGameObject(Math::sVector(3.0f, 0.0f, 0.0f), Math::cQuaternion(), "data/Meshes/sphere.mesh", "data/Materials/green.material") );

	m_gameObjects.push_back( new cGameObject(Math::sVector(-3.0f, 3.0f, 0.0f), Math::cQuaternion(), "data/Meshes/sphere.mesh", "data/Materials/blue.material") );
	m_gameObjects.push_back( new cGameObject(Math::sVector(0.0f, 3.0f, 0.0f), Math::cQuaternion(), "data/Meshes/sphere.mesh", "data/Materials/brick.material") );
	m_gameObjects.push_back( new cGameObject(Math::sVector(3.0f, 3.0f, 0.0f), Math::cQuaternion(), "data/Meshes/sphere.mesh", "data/Materials/red.material") );

	m_gameObjects.push_back( new cGameObject(Math::sVector(-3.0f, -3.0f, 0.0f), Math::cQuaternion(), "data/Meshes/sphere.mesh", "data/Materials/red.material") );
	m_gameObjects.push_back( new cGameObject(Math::sVector(0.0f, -3.0f, 0.0f), Math::cQuaternion(), "data/Meshes/sphere.mesh", "data/Materials/green.material") );
	m_gameObjects.push_back( new cGameObject(Math::sVector(3.0f, -3.0f, 0.0f), Math::cQuaternion(), "data/Meshes/sphere.mesh", "data/Materials/blue.material") );

	// Line
	//m_gameObjects.push_back( new cGameObject(Math::sVector(-1.5f, 0.0f, 0.0f), Math::cQuaternion(), "data/Meshes/sphere.mesh", "data/Materials/blue.material") );
	//m_gameObjects.push_back( new cGameObject(Math::sVector(-0.5f, 0.0f, 0.0f), Math::cQuaternion(), "data/Meshes/sphere.mesh", "data/Materials/transparent.material") );
	//m_gameObjects.push_back( new cGameObject(Math::sVector(0.5f, 0.0f, 0.0f), Math::cQuaternion(), "data/Meshes/sphere.mesh", "data/Materials/green.material") );
	//m_gameObjects.push_back( new cGameObject(Math::sVector(1.5f, 0.0f, 0.0f), Math::cQuaternion(), "data/Meshes/sphere.mesh", "data/Materials/transparent.material") );
	//m_gameObjects.push_back( new cGameObject(Math::sVector(2.5f, 0.0f, 0.0f), Math::cQuaternion(), "data/Meshes/sphere.mesh", "data/Materials/red.material") );
	//m_gameObjects.push_back( new cGameObject(Math::sVector(3.5f, 0.0f, 0.0f), Math::cQuaternion(), "data/Meshes/sphere.mesh", "data/Materials/transparent.material") );
	//m_gameObjects.push_back( new cGameObject(Math::sVector(4.5f, 0.0f, 0.0f), Math::cQuaternion(), "data/Meshes/sphere.mesh", "data/Materials/standard.material") );

	//m_gameObjects.push_back( new cGameObject(Math::sVector(0.0f, -1.0f, 0.0f), Math::cQuaternion(), "data/Meshes/quad.mesh", "data/Materials/standard.material") );

	return result;
}

eae6320::cResult eae6320::cMyGame::CleanUp()
{
	auto result = Results::Success;

	for (auto GameObject : m_gameObjects)
	{
		result = DeleteGameObject(GameObject);
	}

	m_gameObjects.clear();

	DeleteGameObject(m_camera);

	//if (m_sprite)
	//{
	//	const auto localResult = m_sprite->CleanUp();

	//	if ( !localResult )
	//	{
	//		EAE6320_ASSERT( false );
	//		if ( result )
	//		{
	//			result = localResult;
	//		}
	//	}

	//	delete m_sprite;
	//	m_sprite = nullptr;
	//}

	//if (m_spritePad)
	//{
	//	const auto localResult = m_spritePad->CleanUp();

	//	if ( !localResult )
	//	{
	//		EAE6320_ASSERT( false );
	//		if ( result )
	//		{
	//			result = localResult;
	//		}
	//	}

	//	delete m_spritePad;
	//	m_spritePad = nullptr;
	//}

	return result;
}
