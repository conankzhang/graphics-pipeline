// Includes
//=========

#include "../cEffect.h"

#include <cstdlib>
#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Platform/Platform.h>

// Helper Function Declarations
//=============================

namespace
{
	// This helper struct exists to be able to dynamically allocate memory to get "log info"
	// which will automatically be freed when the struct goes out of scope
	struct sLogInfo
	{
		GLchar* memory = nullptr;
		sLogInfo( const size_t i_size ) { memory = static_cast<GLchar*>( malloc( i_size ) ); }
		~sLogInfo() { if ( memory ) free( memory ); }
	};
}

// Implementation
//===============

// Initialization / Clean Up
//--------------------------

eae6320::cResult eae6320::Graphics::cEffect::InitializeShadingData()
{
	auto result = eae6320::Results::Success;

	if ( !( result = eae6320::Graphics::cShader::s_manager.Load( "data/Shaders/Vertex/standard.shader",
		m_vertexShader, eae6320::Graphics::ShaderTypes::Vertex ) ) )
	{
		EAE6320_ASSERT( false );
		goto OnExit;
	}
	if ( !( result = eae6320::Graphics::cShader::s_manager.Load( "data/Shaders/Fragment/animated.shader",
		m_fragmentShader, eae6320::Graphics::ShaderTypes::Fragment ) ) )
	{
		EAE6320_ASSERT( false );
		goto OnExit;
	}
	{
		constexpr uint8_t defaultRenderState = 0;
		if ( !( result = m_renderState.Initialize( defaultRenderState ) ) )
		{
			EAE6320_ASSERT( false );
			goto OnExit;
		}
	}

	// Create a program
	{
		m_programId = glCreateProgram();
		const auto errorCode = glGetError();
		if ( errorCode != GL_NO_ERROR )
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
			eae6320::Logging::OutputError( "OpenGL failed to create a program: %s",
				reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
			goto OnExit;
		}
		else if ( m_programId == 0 )
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERT( false );
			eae6320::Logging::OutputError( "OpenGL failed to create a program" );
			goto OnExit;
		}
	}
	// Attach the shaders to the program
	{
		// Vertex
		{
			glAttachShader( m_programId, eae6320::Graphics::cShader::s_manager.Get( m_vertexShader )->m_shaderId );
			const auto errorCode = glGetError();
			if ( errorCode != GL_NO_ERROR )
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
				eae6320::Logging::OutputError( "OpenGL failed to attach the vertex shader to the program: %s",
					reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
				goto OnExit;
			}
		}
		// Fragment
		{
			glAttachShader( m_programId, eae6320::Graphics::cShader::s_manager.Get( m_fragmentShader )->m_shaderId );
			const auto errorCode = glGetError();
			if ( errorCode != GL_NO_ERROR )
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
				eae6320::Logging::OutputError( "OpenGL failed to attach the fragment shader to the program: %s",
					reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
				goto OnExit;
			}
		}
	}
	// Link the program
	{
		glLinkProgram( m_programId );
		const auto errorCode = glGetError();
		if ( errorCode == GL_NO_ERROR )
		{
			// Get link info
			// (this won't be used unless linking fails
			// but it can be useful to look at when debugging)
			std::string linkInfo;
			{
				GLint infoSize;
				glGetProgramiv( m_programId, GL_INFO_LOG_LENGTH, &infoSize );
				const auto errorCode = glGetError();
				if ( errorCode == GL_NO_ERROR )
				{
					struct sLogInfo
					{
						GLchar* memory;
						sLogInfo( const size_t i_size ) { memory = reinterpret_cast<GLchar*>( malloc( i_size ) ); }
						~sLogInfo() { if ( memory ) free( memory ); }
					} info( static_cast<size_t>( infoSize ) );
					constexpr GLsizei* const dontReturnLength = nullptr;
					glGetProgramInfoLog( m_programId, static_cast<GLsizei>( infoSize ), dontReturnLength, info.memory );
					const auto errorCode = glGetError();
					if ( errorCode == GL_NO_ERROR )
					{
						linkInfo = info.memory;
					}
					else
					{
						result = eae6320::Results::Failure;
						EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
						eae6320::Logging::OutputError( "OpenGL failed to get link info of the program: %s",
							reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
						goto OnExit;
					}
				}
				else
				{
					result = eae6320::Results::Failure;
					EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
					eae6320::Logging::OutputError( "OpenGL failed to get the length of the program link info: %s",
						reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
					goto OnExit;
				}
			}
			// Check to see if there were link errors
			GLint didLinkingSucceed;
			{
				glGetProgramiv( m_programId, GL_LINK_STATUS, &didLinkingSucceed );
				const auto errorCode = glGetError();
				if ( errorCode == GL_NO_ERROR )
				{
					if ( didLinkingSucceed == GL_FALSE )
					{
						result = eae6320::Results::Failure;
						EAE6320_ASSERTF( false, linkInfo.c_str() );
						eae6320::Logging::OutputError( "The program failed to link: %s",
							linkInfo.c_str() );
						goto OnExit;
					}
				}
				else
				{
					result = eae6320::Results::Failure;
					EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
					eae6320::Logging::OutputError( "OpenGL failed to find out if linking of the program succeeded: %s",
						reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
					goto OnExit;
				}
			}
		}
		else
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
			eae6320::Logging::OutputError( "OpenGL failed to link the program: %s",
				reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
			goto OnExit;
		}
	}

OnExit:

	if ( !result )
	{
		if ( m_programId != 0 )
		{
			glDeleteProgram( m_programId );
			const auto errorCode = glGetError();
			if ( errorCode != GL_NO_ERROR )
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
				eae6320::Logging::OutputError( "OpenGL failed to delete the program: %s",
					reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
			}
			m_programId = 0;
		}
	}
	return result;
}

eae6320::cResult eae6320::Graphics::cEffect::CleanUp()
{
	auto result = Results::Success;

	if ( m_programId != 0 )
	{
		glDeleteProgram( m_programId );
		const auto errorCode = glGetError();
		if ( errorCode != GL_NO_ERROR )
		{
			if ( result )
			{
				result = eae6320::Results::Failure;
			}
			EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
			eae6320::Logging::OutputError( "OpenGL failed to delete the program: %s",
				reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
		}
		m_programId = 0;
	}

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

void eae6320::Graphics::cEffect::RenderFrame()
{
	{
		EAE6320_ASSERT( m_programId != 0 );
		glUseProgram( m_programId );
		EAE6320_ASSERT( glGetError() == GL_NO_ERROR );
	}
	m_renderState.Bind();
}
