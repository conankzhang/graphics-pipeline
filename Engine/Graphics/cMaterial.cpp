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

	const auto reflectivity = *reinterpret_cast<sColor*>( currentOffset );
	currentOffset += sizeof( reflectivity );

	const auto gloss = *reinterpret_cast<float*>( currentOffset );
	currentOffset += sizeof( gloss );

	const auto fresnel = *reinterpret_cast<float*>( currentOffset );
	currentOffset += sizeof( fresnel );

	const auto effectPathSize = *reinterpret_cast<uint16_t*>( currentOffset );
	currentOffset += sizeof( effectPathSize );

	const auto texturePathSize = *reinterpret_cast<uint16_t*>( currentOffset );
	currentOffset += sizeof( texturePathSize );

	const auto normalPathSize = *reinterpret_cast<uint16_t*>( currentOffset );
	currentOffset += sizeof( normalPathSize );

	const auto roughPathSize = *reinterpret_cast<uint16_t*>( currentOffset );
	currentOffset += sizeof( roughPathSize );

	const auto parallaxPathSize = *reinterpret_cast<uint16_t*>( currentOffset );
	currentOffset += sizeof( parallaxPathSize );

	char* effectPath = reinterpret_cast<char*>( currentOffset );
	currentOffset += effectPathSize * sizeof( char );

	char* texturePath = reinterpret_cast<char*>( currentOffset );
	currentOffset += texturePathSize* sizeof( char );

	char* normalPath = reinterpret_cast<char*>( currentOffset );
	currentOffset += normalPathSize* sizeof( char );

	char* roughPath = reinterpret_cast<char*>( currentOffset );
	currentOffset += roughPathSize * sizeof( char );

	char* parallaxPath = reinterpret_cast<char*>( currentOffset );

	// Allocate a new material
	{
		newMaterial = new (std::nothrow) cMaterial(color, reflectivity, gloss, fresnel);
		if ( !newMaterial )
		{
			result = Results::OutOfMemory;
			goto OnExit;
		}
	}

	if ( !( result = newMaterial->InitializeMaterialData( effectPath, texturePath, normalPath, roughPath, parallaxPath) ) )
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

eae6320::cResult eae6320::Graphics::cMaterial::InitializeMaterialData(const char i_effectPath[], const char i_texturePath[], const char i_normalPath[], const char i_roughPath[], const char i_parallaxPath[])
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

	if (!(result = eae6320::Graphics::cTexture::s_manager.Load(i_normalPath, m_normal)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}

	if (!(result = eae6320::Graphics::cTexture::s_manager.Load(i_roughPath, m_rough)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}

	if (!(result = eae6320::Graphics::cTexture::s_manager.Load(i_parallaxPath, m_parallax)))
	{
		EAE6320_ASSERT(false);
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

	if ( m_normal )
	{
		const auto localResult = cTexture::s_manager.Release( m_normal );
		if ( !localResult )
		{
			EAE6320_ASSERT( false );
			if ( result )
			{
				result = localResult;
			}
		}
	}

	if ( m_rough )
	{
		const auto localResult = cTexture::s_manager.Release( m_rough );
		if ( !localResult )
		{
			EAE6320_ASSERT( false );
			if ( result )
			{
				result = localResult;
			}
		}
	}

	if ( m_parallax )
	{
		const auto localResult = cTexture::s_manager.Release( m_parallax );
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

void eae6320::Graphics::cMaterial::BindEffect()
{
	// Effect
	{
		EAE6320_ASSERT( m_effect );
		auto* const effect = cEffect::s_manager.Get( m_effect );
		EAE6320_ASSERT(effect);
		effect->RenderFrame();
	}
}

void eae6320::Graphics::cMaterial::BindTexture()
{
	// Texture
	{
		EAE6320_ASSERT( m_texture );
		auto* const texture = cTexture::s_manager.Get( m_texture );
		EAE6320_ASSERT(texture);
		texture->Bind(0);
	}

	{
		EAE6320_ASSERT( m_normal );
		auto* const normal = cTexture::s_manager.Get( m_normal );
		EAE6320_ASSERT(normal);
		normal->Bind(1);
	}

	{
		EAE6320_ASSERT( m_rough );
		auto* const rough = cTexture::s_manager.Get( m_rough );
		EAE6320_ASSERT(rough);
		rough->Bind(2);
	}

	{
		EAE6320_ASSERT( m_parallax );
		auto* const parallax = cTexture::s_manager.Get( m_parallax );
		EAE6320_ASSERT(parallax);
		parallax->Bind(4);
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

const eae6320::Graphics::sColor& eae6320::Graphics::cMaterial::GetReflectivity()
{
	return m_reflectivity;
}

float eae6320::Graphics::cMaterial::GetGloss()
{
	return m_gloss;
}

float eae6320::Graphics::cMaterial::GetFresnel()
{
	return m_fresnel;
}

bool eae6320::Graphics::cMaterial::IsAlphaTransparencyEnabled() const
{
	return cEffect::s_manager.Get(m_effect)->m_renderState.IsAlphaTransparencyEnabled();
}

eae6320::Graphics::cMaterial::cMaterial(const sColor& i_color, const sColor& i_reflectivity, float i_gloss, float i_fresnel) :
	m_color(i_color),
	m_reflectivity(i_reflectivity),
	m_gloss(i_gloss),
	m_fresnel(i_fresnel)
{

}

eae6320::Graphics::cMaterial::~cMaterial()
{
	CleanUp();
}
