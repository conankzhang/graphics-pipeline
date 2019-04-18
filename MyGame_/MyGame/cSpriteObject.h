/*
	This class represents sprite objects.
*/

#ifndef EAE6320_CSPRITEOBJECT_H
#define EAE6320_CSPRITEOBJECT_H

// Includes
//=========
#include <Engine/Physics/sRigidBodyState.h>

#include <Engine/Graphics/cSprite.h>
#include <Engine/Graphics/cMaterial.h>

// Forward Declarations
//=====================

namespace eae6320
{
	namespace Graphics
	{
		class cSprite;
		class cMaterial;
	}
}

// Class Declaration
//==================

namespace eae6320
{
	class cSpriteObject
	{
	public:
		cSpriteObject(Math::sVector i_position, Math::cQuaternion i_orientation, const char * const i_materialPath);
		cResult CleanUp();

		void Update(const float i_elapsedSecondCount_sinceLastUpdate);

		void SetVelocity(Math::sVector i_velocity);
		void SetAngularSpeed(float i_angularSpeed);

		const Graphics::cMaterial::Handle& GetMaterial();

		Math::cMatrix_transformation GetTransform(const float i_elapsedSecondCount_sinceLastSimulationUpdate);
		Math::sVector GetForward();
		Math::sVector GetPosition();

	protected:

		Physics::sRigidBodyState m_rigidBody;

		Graphics::cMaterial::Handle m_material;
	};
}

#endif	// EAE6320_CSPRITEOBJECT_H
