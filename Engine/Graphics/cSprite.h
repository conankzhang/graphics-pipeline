/*
	A sprite is 2D graphical object
*/

#ifndef EAE6320_GRAPHICS_CSPRITE_H
#define EAE6320_GRAPHICS_CSPRITE_H

// Includes
//=========

#include "Configuration.h"

#include <Engine/Assets/ReferenceCountedAssets.h>

#include <cstdint>
#include <Engine/Assets/ReferenceCountedAssets.h>
#include <Engine/Assets/cHandle.h>
#include <Engine/Assets/cManager.h>
#include <Engine/Results/Results.h>
#include "VertexFormats.h"

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

#ifdef EAE6320_PLATFORM_D3D
	struct ID3D11Buffer;
	struct ID3D11InputLayout;
#endif

// Class Declaration
//==================

namespace eae6320
{
	namespace Graphics
	{
		class cSprite
		{
			// Interface
			//==========

		public:
			// Assets
			//-------

			// Initialization / Clean Up
			//--------------------------
			cSprite();
			~cSprite();

			void SetSprite();
			void Draw();

			cResult InitializeGeometry();
			cResult CleanUp();

			// Data
			//=====

		public:
#if defined( EAE6320_PLATFORM_D3D )
			// A vertex buffer holds the data for each vertex
			ID3D11Buffer* m_vertexBuffer = nullptr;
			// D3D has an "input layout" object that associates the layout of the vertex format struct
			// with the input from a vertex shader
			ID3D11InputLayout* m_vertexInputLayout = nullptr;
#elif defined( EAE6320_PLATFORM_GL )
			// A vertex buffer holds the data for each vertex
			GLuint m_vertexBufferId = 0;
			// A vertex array encapsulates the vertex data as well as the vertex input layout
			GLuint m_vertexArrayId = 0;
			// An index buffer holds the index for each vertex to draw
			GLuint m_indexBufferId = 0;
			// An index array encapsulates the index data as well as the index input layout
			GLuint m_indexArrayId = 0;
#endif

			// Implementation
			//===============
		private:
			VertexFormats::sSprite m_spriteVertices[4];
		};
	}
}

#endif	// EAE6320_GRAPHICS_CSPRITE_H
