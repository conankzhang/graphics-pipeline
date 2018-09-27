// Includes
//=========

#include "../cEffect.h"

#include "Includes.h"
#include "../sContext.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Platform/Platform.h>

// Implementation
//===============

// Initialization / Clean Up
//--------------------------

eae6320::cResult eae6320::Graphics::cEffect::InitializeShadingData( const char i_vertexShaderPath[], const char i_fragmentShaderPath[], const uint8_t i_renderStateBits)
{
	auto result = eae6320::Results::Success;

	if ( !( result = eae6320::Graphics::cShader::s_manager.Load( i_vertexShaderPath,
		m_vertexShader, eae6320::Graphics::ShaderTypes::Vertex ) ) )
	{
		EAE6320_ASSERT( false );
		goto OnExit;
	}
	if ( !( result = eae6320::Graphics::cShader::s_manager.Load( i_fragmentShaderPath,
		m_fragmentShader, eae6320::Graphics::ShaderTypes::Fragment ) ) )
	{
		EAE6320_ASSERT( false );
		goto OnExit;
	}
	{
		if ( !( result = m_renderState.Initialize( i_renderStateBits ) ) )
		{
			EAE6320_ASSERT( false );
			goto OnExit;
		}
	}

OnExit:

	return result;
}

eae6320::cResult eae6320::Graphics::cEffect::CleanUp()
{
	auto result = Results::Success;

	if ( m_vertexShader )
	{
		const auto localResult = cShader::s_manager.Release( m_vertexShader );
		if ( !localResult )
		{
			EAE6320_ASSERT( false );
			if ( result )
			{
				result = localResult;
			}
		}
	}
	if ( m_fragmentShader )
	{
		const auto localResult = cShader::s_manager.Release( m_fragmentShader );
		if ( !localResult )
		{
			EAE6320_ASSERT( false );
			if ( result )
			{
				result = localResult;
			}
		}
	}


	{
		const auto localResult = m_renderState.CleanUp();
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

eae6320::cResult eae6320::Graphics::cEffect::Load(eae6320::Graphics::cEffect*& o_effect, const char i_vertexShaderPath[], const char i_fragmentShaderPath[], const uint8_t i_renderStateBits)
{
	auto result = Results::Success;

	eae6320::Graphics::cEffect* newEffect = nullptr;

	// Allocate a new shader
	{
		newEffect = new (std::nothrow) cEffect();
		if ( !newEffect )
		{
			result = Results::OutOfMemory;
			goto OnExit;
		}
	}
	if ( !( result = newEffect->InitializeShadingData( i_vertexShaderPath, i_fragmentShaderPath, i_renderStateBits ) ) )
	{
		EAE6320_ASSERTF( false, "Initialization of new mesh failed" );
		goto OnExit;
	}

OnExit:

	if ( result )
	{
		EAE6320_ASSERT( newEffect );
		o_effect = newEffect;
	}
	else
	{
		if ( newEffect )
		{
			newEffect->DecrementReferenceCount();
			newEffect = nullptr;
		}
		o_effect = nullptr;
	}

	return result;
}

void eae6320::Graphics::cEffect::RenderFrame()
{
	{
		auto* const direct3dImmediateContext = sContext::g_context.direct3dImmediateContext;
		EAE6320_ASSERT( direct3dImmediateContext );

		constexpr ID3D11ClassInstance* const* noInterfaces = nullptr;
		constexpr unsigned int interfaceCount = 0;
		// Vertex shader
		{
			EAE6320_ASSERT( m_vertexShader );
			auto* const shader = cShader::s_manager.Get( m_vertexShader );
			EAE6320_ASSERT( shader && shader->m_shaderObject.vertex );
			direct3dImmediateContext->VSSetShader( shader->m_shaderObject.vertex, noInterfaces, interfaceCount );
		}
		// Fragment shader
		{
			EAE6320_ASSERT( m_fragmentShader );
			auto* const shader = cShader::s_manager.Get( m_fragmentShader );
			EAE6320_ASSERT( shader && shader->m_shaderObject.fragment );
			direct3dImmediateContext->PSSetShader( shader->m_shaderObject.fragment, noInterfaces, interfaceCount );
		}
	}

	m_renderState.Bind();
}

eae6320::Graphics::cEffect::cEffect()
{

}

eae6320::Graphics::cEffect::~cEffect()
{
	CleanUp();
}
