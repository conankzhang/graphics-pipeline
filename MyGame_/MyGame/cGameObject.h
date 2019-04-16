/*
	This class represents game objects.
*/

#ifndef EAE6320_CGAMEOBJECT_H
#define EAE6320_CGAMEOBJECT_H

// Includes
//=========
#include <Engine/Physics/sRigidBodyState.h>

#include <Engine/Graphics/cMesh.h>
#include <Engine/Graphics/cMaterial.h>

// Forward Declarations
//=====================

namespace eae6320
{
	namespace Graphics
	{
		class cMesh;
		class cMaterial;
	}
}

// Class Declaration
//==================

namespace eae6320
{
	class cGameObject
	{
	public:
		cGameObject(Math::sVector i_position, Math::cQuaternion i_orientation, const char * const i_meshPath, const char * const i_materialPath);
		cResult CleanUp();

		void Update(const float i_elapsedSecondCount_sinceLastUpdate);

		void SetVelocity(Math::sVector i_velocity);
		void SetAngularSpeed(float i_angularSpeed);

		uint_fast32_t GetMeshId();
		uint_fast32_t GetMaterialId();

		const Graphics::cMaterial::Handle& GetMaterial();
		const Graphics::cMesh::Handle& GetMesh();

		Math::cMatrix_transformation GetTransform(const float i_elapsedSecondCount_sinceLastSimulationUpdate);
		Math::sVector GetForward();
		Math::sVector GetPosition();

	protected:

		Physics::sRigidBodyState m_rigidBody;

		Graphics::cMesh::Handle m_mesh;
		Graphics::cMaterial::Handle m_material;
	};
}

#endif	// EAE6320_CGAMEOBJECT_H
