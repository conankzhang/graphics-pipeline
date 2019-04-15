/*
	This class loads materials
*/

#ifndef EAE6320_CMATERIALLOADER_H
#define EAE6320_CMATERIALLOADER_H

// Includes
//=========
#include <Engine/Graphics/Configuration.h>
#include <Engine/Graphics/cMesh.h>

// Forward Declarations
//=====================

namespace eae6320
{
	namespace Graphics
	{
		struct sColor;
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
			static cResult LoadAsset( const char* const i_path, std::string& o_effectPath, Graphics::sColor& o_color);

		private:

			// Build
			//------


			// Implementation
			//===============
			static cResult LoadTableValues(lua_State& io_luaState, std::string& o_effectPath, Graphics::sColor& o_color);

		};
	}
}

#endif	// EAE6320_CMATERIALLOADER_H
