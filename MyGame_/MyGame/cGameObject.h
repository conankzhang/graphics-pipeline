/*
	This class represents game objects.
*/

#ifndef EAE6320_CGAMEOBJECT_H
#define EAE6320_CGAMEOBJECT_H

// Includes
//=========
#include <Engine/Physics/sRigidBodyState.h>

#include <Engine/Graphics/cMesh.h>
#include <Engine/Graphics/cEffect.h>

// Forward Declarations
//=====================

namespace eae6320
{
	namespace Graphics
	{
		class cMesh;
		class cEffect;
	}
}

// Class Declaration
//==================

namespace eae6320
{
	class cGameObject
	{
	public:
		cGameObject(eae6320::Math::sVector i_position, eae6320::Math::cQuaternion i_orientation, const char * const i_meshPath, const char * const i_effectPath);
		void CleanUp();

		void Update(const float i_elapsedSecondCount_sinceLastUpdate);

		void SetVelocity(eae6320::Math::sVector i_velocity);
		void SetAngularSpeed(float i_angularSpeed);

		uint_fast32_t GetMesh();
		uint_fast32_t GetEffect();

		eae6320::Math::cMatrix_transformation GetTransform(const float i_elapsedSecondCount_sinceLastSimulationUpdate);
		eae6320::Math::sVector GetForward();
		eae6320::Math::sVector GetPosition();

	protected:

		eae6320::Physics::sRigidBodyState m_rigidBody;

		eae6320::Graphics::cMesh::Handle m_mesh;
		eae6320::Graphics::cEffect::Handle m_effect;
	};
}

#endif	// EAE6320_CGAMEOBJECT_H
