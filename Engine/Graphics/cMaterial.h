/*
	A material encapsulates an effect and constannt data.
*/

#ifndef EAE6320_GRAPHICS_CMATERIAL_H
#define EAE6320_GRAPHICS_CMATERIAL_H

// Includes
//=========

#include "Configuration.h"

#include <Engine/Assets/ReferenceCountedAssets.h>

#include <cstdint>
#include <Engine/Assets/ReferenceCountedAssets.h>
#include <Engine/Assets/cHandle.h>
#include <Engine/Assets/cManager.h>
#include <Engine/Results/Results.h>
#include <Engine/Graphics/cEffect.h>
#include <Engine/Graphics/cTexture.h>
#include <Engine/Graphics/sColor.h>

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
		class cMaterial
		{
			// Interface
			//==========

		public:
			// Assets
			//-------

			using Handle = Assets::cHandle<cMaterial>;
			static Assets::cManager<cMaterial> s_manager;

			// Initialization / Clean Up
			//--------------------------
			static cResult Load(const std::string& i_materialPath, cMaterial*& o_material);

			void BindEffect();
			void BindTexture();

			EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS(cMaterial);
			EAE6320_ASSETS_DECLAREREFERENCECOUNTINGFUNCTIONS();

			// Data
			//=====

			// In this initial assignment the Graphics system needs access to the shader API objects.
			// In future assignments you can make the data private and decide who needs access
			// and the best way to provide it.

		public:

			cEffect::Handle m_effect;
			cTexture::Handle m_texture;

			sColor m_color;

			EAE6320_ASSETS_DECLAREREFERENCECOUNT();

			// Implementation
			//===============

			uint_fast32_t GetEffectId();
			const sColor& GetColor();

			bool IsAlphaTransparencyEnabled() const;

		private:

			// Initialization / Clean Up
			//--------------------------
			cResult InitializeMaterialData(const char i_effectPath[], const char i_texturePath[]);
			cResult CleanUp();

			cMaterial(const sColor& i_color);
			~cMaterial();
		};
	}
}

#endif	// EAE6320_GRAPHICS_CMATERIAL_H
