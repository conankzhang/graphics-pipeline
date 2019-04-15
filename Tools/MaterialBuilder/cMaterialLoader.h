/*
	This class loads materials
*/

#ifndef EAE6320_CMATERIALLOADER_H
#define EAE6320_CMATERIALLOADER_H

// Includes
//=========

#include <Engine/Graphics/Configuration.h>
#include <Engine/Graphics/cEffect.h>
#include <Engine/Graphics/cMesh.h>

// Forward Declarations
//=====================

namespace eae6320
{
	namespace Platform
	{
		struct sDataFromFile;
	}

	namespace Graphics
	{
		namespace VertexFormats
		{
			struct sMesh;
		}
	}
}

// Class Declaration
//==================

namespace eae6320
{
	namespace Assets
	{
		class cMaterialLoader
		{
			// Inherited Implementation
			//=========================
		public:
			static eae6320::cResult LoadAsset( const char* const i_path, std::string& o_effectPath, std::vector<float>& o_colorData);

		private:

			// Build
			//------


			// Implementation
			//===============
			static eae6320::cResult LoadTableValues(lua_State& io_luaState, std::string& o_effectPath, std::vector<float>& o_colorData);

		};
	}
}

#endif	// EAE6320_CMATERIALLOADER_H
