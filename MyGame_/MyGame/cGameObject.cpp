// Includes
//=========

#include "cGameObject.h"

#include <Engine/Math/cMatrix_transformation.h>

eae6320::cGameObject::cGameObject(eae6320::Math::sVector i_position, eae6320::Math::cQuaternion i_orientation, const char * const i_meshPath, const char * const i_effectPath)
{
	m_rigidBody.position = i_position;
	m_rigidBody.orientation = i_orientation;

	// Initialize the shading data
	if (i_meshPath && i_effectPath)
	{

		auto result = Results::Success;

		{
			if ( !( result =  eae6320::Graphics::cMesh::m_manager.Load(i_meshPath, m_mesh) ) )
			{
				EAE6320_ASSERT( false );
			}

			if (m_mesh.IsValid())
			{
				Graphics::cMesh::m_manager.UnsafeIncrementReferenceCount(m_mesh.GetIndex());
			}
		}

		{
			if ( !( result = eae6320::Graphics::cEffect::e_manager.Load(i_effectPath, m_effect) ) )
			{
				EAE6320_ASSERT( false );
			}

			if (m_effect.IsValid())
			{
				Graphics::cEffect::e_manager.UnsafeIncrementReferenceCount(m_effect.GetIndex());
			}
		}
	}
}

void eae6320::cGameObject::CleanUp()
{
	if (m_mesh.IsValid())
	{
		Graphics::cMesh::m_manager.UnsafeDecrementReferenceCount(m_mesh.GetIndex());
	}

	if (m_effect.IsValid())
	{
		Graphics::cEffect::e_manager.UnsafeDecrementReferenceCount(m_effect.GetIndex());
	}
}

void eae6320::cGameObject::Update(const float i_elapsedSecondCount_sinceLastUpdate)
{
	m_rigidBody.Update(i_elapsedSecondCount_sinceLastUpdate);
}

void eae6320::cGameObject::SetVelocity(eae6320::Math::sVector i_velocity)
{
	m_rigidBody.velocity = i_velocity;
}

void eae6320::cGameObject::SetAngularSpeed(float i_angularSpeed)
{
	m_rigidBody.angularSpeed = i_angularSpeed;
}

eae6320::Graphics::cMesh* eae6320::cGameObject::GetMesh()
{
	return Graphics::cMesh::m_manager.Get(m_mesh);
}

eae6320::Graphics::cEffect* eae6320::cGameObject::GetEffect()
{
	return Graphics::cEffect::e_manager.Get(m_effect);
}

eae6320::Math::cMatrix_transformation eae6320::cGameObject::GetTransform(const float i_elapsedSecondCount_sinceLastSimulationUpdate)
{
	return eae6320::Math::cMatrix_transformation(m_rigidBody.PredictFutureOrientation(i_elapsedSecondCount_sinceLastSimulationUpdate), m_rigidBody.PredictFuturePosition(i_elapsedSecondCount_sinceLastSimulationUpdate));
}

eae6320::Math::sVector eae6320::cGameObject::GetForward()
{
	return m_rigidBody.orientation.CalculateForwardDirection();
}
