// Includes
//=========

#include "../cMesh.h"
#include "../VertexFormats.h"

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
eae6320::cResult eae6320::Graphics::cMesh::InitializeGeometry()
{
	auto result = eae6320::Results::Success;
	// Create a vertex array object and make it active
	{
		constexpr GLsizei arrayCount = 1;
		glGenVertexArrays( arrayCount, &m_vertexArrayId );
		const auto errorCode = glGetError();
		if ( errorCode == GL_NO_ERROR )
		{
			glBindVertexArray( m_vertexArrayId );
			const auto errorCode = glGetError();
			if ( errorCode != GL_NO_ERROR )
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
				eae6320::Logging::OutputError( "OpenGL failed to bind a new vertex array: %s",
					reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
				goto OnExit;
			}
		}
		else
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
			eae6320::Logging::OutputError( "OpenGL failed to get an unused vertex array ID: %s",
				reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
			goto OnExit;
		}
	}
	// Create a vertex buffer object and make it active
	{
		constexpr GLsizei bufferCount = 1;
		glGenBuffers( bufferCount, &m_vertexBufferId );
		const auto errorCode = glGetError();
		if ( errorCode == GL_NO_ERROR )
		{
			glBindBuffer( GL_ARRAY_BUFFER, m_vertexBufferId );
			const auto errorCode = glGetError();
			if ( errorCode != GL_NO_ERROR )
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
				eae6320::Logging::OutputError( "OpenGL failed to bind a new vertex buffer: %s",
					reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
				goto OnExit;
			}
		}
		else
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
			eae6320::Logging::OutputError( "OpenGL failed to get an unused vertex buffer ID: %s",
				reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
			goto OnExit;
		}
	}
	// Assign the data to the buffer
	{
		constexpr unsigned int triangleCount = 2;
		constexpr unsigned int vertexCountPerTriangle = 3;
		const auto vertexCount = triangleCount * vertexCountPerTriangle;
		eae6320::Graphics::VertexFormats::sMesh vertexData[vertexCount];
		{
			vertexData[0].x = 0.0f;
			vertexData[0].y = 0.0f;
			vertexData[0].z = 0.0f;

			vertexData[1].x = 1.0f;
			vertexData[1].y = 0.0f;
			vertexData[1].z = 0.0f;

			vertexData[2].x = 1.0f;
			vertexData[2].y = 1.0f;
			vertexData[2].z = 0.0f;

			vertexData[3].x = 0.0f;
			vertexData[3].y = 0.0f;
			vertexData[3].z = 0.0f;

			vertexData[4].x = 1.0f;
			vertexData[4].y = 1.0f;
			vertexData[4].z = 0.0f;

			vertexData[5].x = 0.0f;
			vertexData[5].y = 1.0f;
			vertexData[5].z = 0.0f;
		}
		const auto bufferSize = vertexCount * sizeof( *vertexData );
		EAE6320_ASSERT( bufferSize < ( uint64_t( 1u ) << ( sizeof( GLsizeiptr ) * 8 ) ) );
		glBufferData( GL_ARRAY_BUFFER, static_cast<GLsizeiptr>( bufferSize ), reinterpret_cast<GLvoid*>( vertexData ),
			// In our class we won't ever read from the buffer
			GL_STATIC_DRAW );
		const auto errorCode = glGetError();
		if ( errorCode != GL_NO_ERROR )
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
			eae6320::Logging::OutputError( "OpenGL failed to allocate the vertex buffer: %s",
				reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
			goto OnExit;
		}
	}
	// Initialize vertex format
	{
		// The "stride" defines how large a single vertex is in the stream of data
		// (or, said another way, how far apart each position element is)
		const auto stride = static_cast<GLsizei>( sizeof( eae6320::Graphics::VertexFormats::sMesh ) );

		// Position (0)
		// 3 floats == 12 bytes
		// Offset = 0
		{
			constexpr GLuint vertexElementLocation = 0;
			constexpr GLint elementCount = 3;
			constexpr GLboolean notNormalized = GL_FALSE;	// The given floats should be used as-is
			glVertexAttribPointer( vertexElementLocation, elementCount, GL_FLOAT, notNormalized, stride,
				reinterpret_cast<GLvoid*>( offsetof( eae6320::Graphics::VertexFormats::sMesh, x ) ) );
			const auto errorCode = glGetError();
			if ( errorCode == GL_NO_ERROR )
			{
				glEnableVertexAttribArray( vertexElementLocation );
				const GLenum errorCode = glGetError();
				if ( errorCode != GL_NO_ERROR )
				{
					result = eae6320::Results::Failure;
					EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
					eae6320::Logging::OutputError( "OpenGL failed to enable the POSITION vertex attribute at location %u: %s",
						vertexElementLocation, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
					goto OnExit;
				}
			}
			else
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
				eae6320::Logging::OutputError( "OpenGL failed to set the POSITION vertex attribute at location %u: %s",
					vertexElementLocation, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
				goto OnExit;
			}
		}
	}

OnExit:

	return result;
}

eae6320::cResult eae6320::Graphics::cMesh::CleanUp()
{
	auto result = Results::Success;

	if ( m_vertexBufferId != 0 )
	{
		constexpr GLsizei bufferCount = 1;
		glDeleteBuffers( bufferCount, &m_vertexBufferId );
		const auto errorCode = glGetError();
		if ( errorCode != GL_NO_ERROR )
		{
			if ( result )
			{
				result = Results::Failure;
			}
			EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
			Logging::OutputError( "OpenGL failed to delete the vertex buffer: %s",
				reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
		}
		m_vertexBufferId = 0;
	}
	if ( m_vertexArrayId != 0 )
	{
		// Make sure that the vertex array isn't bound
		{
			// Unbind the vertex array
			glBindVertexArray( 0 );
			const auto errorCode = glGetError();
			if ( errorCode != GL_NO_ERROR )
			{
				if ( result )
				{
					result = Results::Failure;
				}
				EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
				Logging::OutputError( "OpenGL failed to unbind all vertex arrays before cleaning up geometry: %s",
					reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
			}
		}
		constexpr GLsizei arrayCount = 1;
		glDeleteVertexArrays( arrayCount, &m_vertexArrayId );
		const auto errorCode = glGetError();
		if ( errorCode != GL_NO_ERROR )
		{
			if ( result )
			{
				result = Results::Failure;
			}
			EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
			Logging::OutputError( "OpenGL failed to delete the vertex array: %s",
				reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
		}
		m_vertexArrayId = 0;
	}
	return result;
}

void eae6320::Graphics::cMesh::RenderFrame()
{
	glBindVertexArray( m_vertexArrayId );
	EAE6320_ASSERT( glGetError() == GL_NO_ERROR );
}
