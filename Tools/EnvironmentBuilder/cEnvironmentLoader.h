/*
	This class loads environments
*/

#ifndef EAE6320_CENVIRONMENTLOADER_H
#define EAE6320_CENVIRONMENTLOADER_H

// Includes
//=========
#include <Engine/Graphics/Configuration.h>
#include <Engine/Graphics/cMesh.h>

// Forward Declarations
//=====================

// Class Declaration
//==================

namespace eae6320
{
	namespace Assets
	{
		class cEnvironmentLoader
		{
			// Inherited Implementation
			//=========================
		public:
			static cResult LoadAsset( const char* const i_path, std::string& o_environmentPath);

		private:

			// Build
			//------


			// Implementation
			//===============
			static cResult LoadTableValues(lua_State& io_luaState, std::string& o_environmentPath);

		};
	}
}

#endif	// EAE6320_CMATERIALLOADER_H
