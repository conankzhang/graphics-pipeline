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

	const auto x_value = *reinterpret_cast<float*>( currentOffset );
	currentOffset += sizeof( x_value );

	const auto y_value = *reinterpret_cast<float*>( currentOffset );
	currentOffset += sizeof( y_value );

	const auto z_value = *reinterpret_cast<float*>( currentOffset );
	currentOffset += sizeof( z_value );

	const auto w_value = *reinterpret_cast<float*>( currentOffset );
	currentOffset += sizeof( w_value );

	const auto effectPathSize = *reinterpret_cast<uint16_t*>( currentOffset );
	currentOffset += sizeof( effectPathSize );

	char* effectPath = reinterpret_cast<char*>( currentOffset );

	// Allocate a new material
	{
		newMaterial = new (std::nothrow) cMaterial();
		if ( !newMaterial )
		{
			result = Results::OutOfMemory;
			goto OnExit;
		}
	}

	if ( !( result = eae6320::Graphics::cEffect::s_manager.Load( effectPath, newMaterial->m_effect) ) )
	{
		EAE6320_ASSERT( false );
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
}

eae6320::Graphics::cMaterial::cMaterial()
{

}

eae6320::Graphics::cMaterial::~cMaterial()
{
	CleanUp();
}
