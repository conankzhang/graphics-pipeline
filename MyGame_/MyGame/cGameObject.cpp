// Includes
//=========

#include "cGameObject.h"

#include <Engine/Math/cMatrix_transformation.h>

eae6320::cGameObject::cGameObject(eae6320::Math::sVector i_position, eae6320::Math::cQuaternion i_orientation, const char * const i_meshPath, const char * const i_materialPath)
{
	m_rigidBody.position = i_position;
	m_rigidBody.orientation = i_orientation;

	// Initialize the game object data
	if (i_meshPath && i_materialPath)
	{

		auto result = Results::Success;

		{
			if ( !( result =  eae6320::Graphics::cMesh::s_manager.Load(i_meshPath, m_mesh) ) )
			{
				EAE6320_ASSERT( false );
			}
		}

		{
			if ( !( result = eae6320::Graphics::cMaterial::s_manager.Load(i_materialPath, m_material) ) )
			{
				EAE6320_ASSERT( false );
			}
		}
	}
}

eae6320::cResult eae6320::cGameObject::CleanUp()
{
	auto result = Results::Success;

	if ( m_mesh )
	{
		const auto localResult = Graphics::cMesh::s_manager.Release( m_mesh );
		if ( !localResult )
		{
			EAE6320_ASSERT( false );
			if ( result )
			{
				result = localResult;
			}
		}
	}

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

uint_fast32_t eae6320::cGameObject::GetMeshId()
{
	return m_mesh.GetIndex();
}

uint_fast32_t eae6320::cGameObject::GetMaterialId()
{
	return m_material.GetIndex();
}

eae6320::Math::cMatrix_transformation eae6320::cGameObject::GetTransform(const float i_elapsedSecondCount_sinceLastSimulationUpdate)
{
	return eae6320::Math::cMatrix_transformation(m_rigidBody.PredictFutureOrientation(i_elapsedSecondCount_sinceLastSimulationUpdate), m_rigidBody.PredictFuturePosition(i_elapsedSecondCount_sinceLastSimulationUpdate));
}

eae6320::Math::sVector eae6320::cGameObject::GetForward()
{
	return m_rigidBody.orientation.CalculateForwardDirection();
}

eae6320::Math::sVector eae6320::cGameObject::GetPosition()
{
	return m_rigidBody.position;
}
