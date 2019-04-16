// Includes
//=========

#include "cMaterial.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Platform/Platform.h>

// Implementation
//===============

// Static Data Initialization
//===========================

eae6320::Assets::cManager<eae6320::Graphics::cMaterial> eae6320::Graphics::cMaterial::s_manager;

// Initialization / Clean Up
//===========================

eae6320::cResult eae6320::Graphics::cMaterial::Load(const std::string& i_materialPath, eae6320::Graphics::cMaterial*& o_material)
{
	auto result = Results::Success;

	eae6320::Graphics::cMaterial* newMaterial = nullptr;

	eae6320::Platform::sDataFromFile dataFromFile;
	eae6320::Platform::LoadBinaryFile(i_materialPath.c_str(), dataFromFile);

	auto currentOffset = reinterpret_cast<uintptr_t>( dataFromFile.data );
	const auto finalOffset = currentOffset + dataFromFile.size;

	const auto color = *reinterpret_cast<sColor*>( currentOffset );
	currentOffset += sizeof( color );

	const auto effectPathSize = *reinterpret_cast<uint16_t*>( currentOffset );
	currentOffset += sizeof( effectPathSize );

	char* effectPath = reinterpret_cast<char*>( currentOffset );
	currentOffset += effectPathSize * sizeof( char );

	char* texturePath = reinterpret_cast<char*>( currentOffset );

	// Allocate a new material
	{
		newMaterial = new (std::nothrow) cMaterial(color);
		if ( !newMaterial )
		{
			result = Results::OutOfMemory;
			goto OnExit;
		}
	}

	if ( !( result = newMaterial->InitializeMaterialData( effectPath, texturePath) ) )
	{
		EAE6320_ASSERTF( false, "Initialization of new mesh failed" );
		goto OnExit;
	}

OnExit:

	if ( result )
	{
		EAE6320_ASSERT( newMaterial);
		o_material = newMaterial;
	}
	else
	{
		if ( newMaterial )
		{
			newMaterial->DecrementReferenceCount();
			newMaterial = nullptr;
		}
		o_material = nullptr;
	}

	return result;
}

eae6320::cResult eae6320::Graphics::cMaterial::InitializeMaterialData(const char i_effectPath[], const char i_texturePath[])
{
	auto result = Results::Success;

	if ( !( result = eae6320::Graphics::cEffect::s_manager.Load( i_effectPath, m_effect) ) )
	{
		EAE6320_ASSERT( false );
		goto OnExit;
	}

	if ( !( result = eae6320::Graphics::cTexture::s_manager.Load( i_texturePath, m_texture) ) )
	{
		EAE6320_ASSERT( false );
		goto OnExit;
	}

OnExit:

	return result;
}

eae6320::cResult eae6320::Graphics::cMaterial::CleanUp()
{
	auto result = Results::Success;

	if ( m_effect )
	{
		const auto localResult = cEffect::s_manager.Release( m_effect );
		if ( !localResult )
		{
			EAE6320_ASSERT( false );
			if ( result )
			{
				result = localResult;
			}
		}
	}

	if ( m_texture )
	{
		const auto localResult = cTexture::s_manager.Release( m_texture );
		if ( !localResult )
		{
			EAE6320_ASSERT( false );
			if ( result )
			{
				result = localResult;
			}
		}
	}

	return result;
}

void eae6320::Graphics::cMaterial::RenderFrame()
{
	// Effect
	{
		EAE6320_ASSERT( m_effect );
		auto* const effect = cEffect::s_manager.Get( m_effect );
		EAE6320_ASSERT(effect);
		effect->RenderFrame();
	}

	// Texture
	{
		EAE6320_ASSERT( m_texture );
		auto* const texture = cTexture::s_manager.Get( m_texture );
		EAE6320_ASSERT(texture);
		texture->Bind(0);
	}
}

uint_fast32_t eae6320::Graphics::cMaterial::GetEffectId()
{
	return m_effect.GetIndex();
}

const eae6320::Graphics::sColor& eae6320::Graphics::cMaterial::GetColor()
{
	return m_color;
}

bool eae6320::Graphics::cMaterial::IsAlphaTransparencyEnabled() const
{
	return cEffect::s_manager.Get(m_effect)->m_renderState.IsAlphaTransparencyEnabled();
}

eae6320::Graphics::cMaterial::cMaterial(const sColor& i_color) :
	m_color(i_color)
{

}

eae6320::Graphics::cMaterial::~cMaterial()
{
	CleanUp();
}
