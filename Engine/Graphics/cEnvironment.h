/*
	A environment encapsulates a texture
*/

#ifndef EAE6320_GRAPHICS_CENVIRONMENT_H
#define EAE6320_GRAPHICS_CENVIRONMENT_H

// Includes
//=========

#include "Configuration.h"

#include <Engine/Assets/ReferenceCountedAssets.h>

#include <cstdint>
#include <Engine/Assets/ReferenceCountedAssets.h>
#include <Engine/Assets/cHandle.h>
#include <Engine/Assets/cManager.h>
#include <Engine/Results/Results.h>
#include <Engine/Graphics/cTexture.h>
#include <Engine/Graphics/cEffect.h>

#ifdef EAE6320_PLATFORM_GL
	#include "OpenGL/Includes.h"
#endif

// Forward Declarations
//=====================

namespace eae6320
{
	namespace Platform
	{
		struct sDataFromFile;
	}
}

// Class Declaration
//==================

namespace eae6320
{
	namespace Graphics
	{
		class cEnvironment
		{
			// Interface
			//==========

		public:
			// Assets
			//-------

			using Handle = Assets::cHandle<cEnvironment>;
			static Assets::cManager<cEnvironment> s_manager;

			// Initialization / Clean Up
			//--------------------------
			static cResult Load(const std::string& i_environmentPath, cEnvironment*& o_Environment);

			void Bind();

			EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS(cEnvironment);
			EAE6320_ASSETS_DECLAREREFERENCECOUNTINGFUNCTIONS();

			// Data
			//=====

			// In this initial assignment the Graphics system needs access to the shader API objects.
			// In future assignments you can make the data private and decide who needs access
			// and the best way to provide it.

		public:
			cTexture::Handle m_environment;
			cEffect::Handle m_effect;

			EAE6320_ASSETS_DECLAREREFERENCECOUNT();

			// Implementation
			//===============

		private:

			// Initialization / Clean Up
			//--------------------------
			cResult InitializeEnvironmentData(const char i_environmentPath[], const char i_effectPath[]);
			cResult CleanUp();

			cEnvironment();
			~cEnvironment();
		};
	}
}

#endif	// EAE6320_GRAPHICS_CENVIRONMENT_H
