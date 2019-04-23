// Includes
//=========

#include "cEnvironment.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Platform/Platform.h>

// Implementation
//===============

// Static Data Initialization
//===========================

eae6320::Assets::cManager<eae6320::Graphics::cEnvironment> eae6320::Graphics::cEnvironment::s_manager;

// Initialization / Clean Up
//===========================

eae6320::cResult eae6320::Graphics::cEnvironment::Load(const std::string& i_environmentPath, eae6320::Graphics::cEnvironment*& o_environment)
{
	auto result = Results::Success;

	eae6320::Graphics::cEnvironment* newEnvironment = nullptr;

	eae6320::Platform::sDataFromFile dataFromFile;
	eae6320::Platform::LoadBinaryFile(i_environmentPath.c_str(), dataFromFile);

	auto currentOffset = reinterpret_cast<uintptr_t>( dataFromFile.data );
	const auto finalOffset = currentOffset + dataFromFile.size;

	const auto environmentPathSize = *reinterpret_cast<uint16_t*>( currentOffset );
	currentOffset += sizeof( environmentPathSize );

	const auto effectPathSize = *reinterpret_cast<uint16_t*>( currentOffset );
	currentOffset += sizeof( effectPathSize );

	char* environmentPath = reinterpret_cast<char*>( currentOffset );
	currentOffset += environmentPathSize * sizeof( char );

	char* effectPath = reinterpret_cast<char*>( currentOffset );

	// Allocate a new material
	{
		newEnvironment = new (std::nothrow) cEnvironment();
		if ( !newEnvironment)
		{
			result = Results::OutOfMemory;
			goto OnExit;
		}
	}

	if ( !( result = newEnvironment->InitializeEnvironmentData( environmentPath, effectPath) ) )
	{
		EAE6320_ASSERTF( false, "Initialization of new environment failed" );
		goto OnExit;
	}

OnExit:

	if ( result )
	{
		EAE6320_ASSERT( newEnvironment);
		o_environment = newEnvironment;
	}
	else
	{
		if ( newEnvironment )
		{
			newEnvironment->DecrementReferenceCount();
			newEnvironment = nullptr;
		}
		o_environment = nullptr;
	}

	return result;
}

eae6320::cResult eae6320::Graphics::cEnvironment::InitializeEnvironmentData(const char i_environmentPath[], const char i_effectPath[])
{
	auto result = Results::Success;

	if ( !( result = eae6320::Graphics::cTexture::s_manager.Load( i_environmentPath, m_environment) ) )
	{
		EAE6320_ASSERT( false );
		goto OnExit;
	}

	if ( !( result = eae6320::Graphics::cEffect::s_manager.Load( i_effectPath, m_effect) ) )
	{
		EAE6320_ASSERT( false );
		goto OnExit;
	}

OnExit:

	return result;
}

eae6320::cResult eae6320::Graphics::cEnvironment::CleanUp()
{
	auto result = Results::Success;

	if ( m_environment )
	{
		const auto localResult = cTexture::s_manager.Release( m_environment );
		if ( !localResult )
		{
			EAE6320_ASSERT( false );
			if ( result )
			{
				result = localResult;
			}
		}
	}

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

void eae6320::Graphics::cEnvironment::Bind()
{
	// Texture
	{
		EAE6320_ASSERT( m_environment );
		auto* const environment = cTexture::s_manager.Get( m_environment );
		EAE6320_ASSERT(environment);
		environment->Bind(3);
	}

	// Effect
	{
		EAE6320_ASSERT( m_effect );
		auto* const effect = cEffect::s_manager.Get( m_effect );
		EAE6320_ASSERT(effect);
		effect->RenderFrame();
	}
}

eae6320::Graphics::cEnvironment::cEnvironment()
{

}

eae6320::Graphics::cEnvironment::~cEnvironment()
{
	CleanUp();
}
