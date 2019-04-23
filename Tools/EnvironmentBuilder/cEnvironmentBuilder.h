/*
	This class builds environment maps
*/

#ifndef EAE6320_CENVIRONMENTBUILDER_H
#define EAE6320_CENVIRONMENTBUILDER_H

// Includes
//=========

#include <Tools/AssetBuildLibrary/cbBuilder.h>

#include <Engine/Graphics/Configuration.h>
#include <Engine/Graphics/cMesh.h>

// Class Declaration
//==================

namespace eae6320
{
	namespace Assets
	{
		class cEnvironmentBuilder : public cbBuilder
		{
			// Inherited Implementation
			//=========================

		private:

			// Build
			//------

			virtual cResult Build( const std::vector<std::string>& i_arguments ) override;

			// Implementation
			//===============
		};
	}
}

#endif	// EAE6320_CENVIRONMENTBUILDER_H
