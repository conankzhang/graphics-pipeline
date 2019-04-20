#include "cSpriteObject.h"

#include <Engine/Math/cMatrix_transformation.h>

eae6320::cSpriteObject::cSpriteObject(Math::sVector i_position, Math::sVector i_scale, Math::cQuaternion i_orientation, const char * const i_materialPath)
{
	m_rigidBody.position = i_position;
	m_rigidBody.orientation = i_orientation;
	m_scale = i_scale;

	// Initialize the game object data
	if (i_materialPath)
	{
		auto result = Results::Success;

		{
			if ( !( result = eae6320::Graphics::cMaterial::s_manager.Load(i_materialPath, m_material) ) )
			{
				EAE6320_ASSERT( false );
			}
		}
	}
}

eae6320::cResult eae6320::cSpriteObject::CleanUp()
{
	auto result = Results::Success;
	if ( m_material )
	{
		const auto localResult = Graphics::cMaterial::s_manager.Release( m_material );
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

void eae6320::cSpriteObject::Update(const float i_elapsedSecondCount_sinceLastUpdate)
{
	m_rigidBody.Update(i_elapsedSecondCount_sinceLastUpdate);
}

void eae6320::cSpriteObject::SetVelocity(Math::sVector i_velocity)
{
	m_rigidBody.velocity = i_velocity;
}

void eae6320::cSpriteObject::SetAngularSpeed(float i_angularSpeed)
{
	m_rigidBody.angularSpeed = i_angularSpeed;
}

const eae6320::Graphics::cMaterial::Handle& eae6320::cSpriteObject::GetMaterial()
{
	return m_material;
}

eae6320::Math::cMatrix_transformation eae6320::cSpriteObject::GetTransform(const float i_elapsedSecondCount_sinceLastSimulationUpdate)
{
	return Math::cMatrix_transformation(m_rigidBody.PredictFutureOrientation(i_elapsedSecondCount_sinceLastSimulationUpdate), m_rigidBody.PredictFuturePosition(i_elapsedSecondCount_sinceLastSimulationUpdate));
}

eae6320::Math::sVector eae6320::cSpriteObject::GetForward()
{
	return m_rigidBody.orientation.CalculateForwardDirection();
}

eae6320::Math::sVector eae6320::cSpriteObject::GetPosition()
{
	return m_rigidBody.position;
}

eae6320::Math::sVector eae6320::cSpriteObject::GetScale()
{
	return m_scale;
}
