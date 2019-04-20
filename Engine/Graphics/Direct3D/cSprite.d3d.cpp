// Includes
//=========

#include "../cSprite.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Platform/Platform.h>
#include <Engine/Time/Time.h>
#include <new>
#include "../VertexFormats.h"

#include <External/Lua/Includes.h>
#include <iostream>
#include <string>

#include "Includes.h"
#include "../sContext.h"

// Static Data Initialization
//===========================

eae6320::Graphics::cSprite::cSprite()
{
	m_spriteVertices[0].x = -128;
	m_spriteVertices[1].x = -128;
	m_spriteVertices[2].x = 127;
	m_spriteVertices[3].x = 127;

	m_spriteVertices[0].y = -128;
	m_spriteVertices[1].y = 127;
	m_spriteVertices[2].y = -128;
	m_spriteVertices[3].y = 127;

	m_spriteVertices[0].u = 0;
	m_spriteVertices[1].u = 0;
	m_spriteVertices[2].u = 255;
	m_spriteVertices[3].u = 255;

	m_spriteVertices[0].v = 255;
	m_spriteVertices[1].v = 0;
	m_spriteVertices[2].v = 255;
	m_spriteVertices[3].v = 0;
}

eae6320::Graphics::cSprite::~cSprite()
{

}

// Interface
//==========

void eae6320::Graphics::cSprite::SetSprite()
{
	auto* const direct3dImmediateContext = sContext::g_context.direct3dImmediateContext;
	EAE6320_ASSERT( direct3dImmediateContext );

	// Bind a specific vertex buffer to the device as a data source
	{
		EAE6320_ASSERT( m_spriteVertices );
		constexpr unsigned int startingSlot = 0;
		constexpr unsigned int vertexBufferCount = 1;
		// The "stride" defines how large a single vertex is in the stream of data
		constexpr unsigned int bufferStride = sizeof( VertexFormats::sSprite );
		// It's possible to start streaming data in the middle of a vertex buffer
		constexpr unsigned int bufferOffset = 0;
		direct3dImmediateContext->IASetVertexBuffers( startingSlot, vertexBufferCount, &m_vertexBuffer, &bufferStride, &bufferOffset );
	}

	// Specify what kind of data the vertex buffer holds
	{
		// Set the layout (which defines how to interpret a single vertex)
		{
			EAE6320_ASSERT( m_vertexInputLayout );
			direct3dImmediateContext->IASetInputLayout( m_vertexInputLayout );
		}

		// Set the topology (which defines how to interpret multiple vertices as a single "primitive";
		// the vertex buffer was defined as a triangle list
		// (meaning that every primitive is a triangle and will be defined by three vertices)
		direct3dImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
	}
}

void eae6320::Graphics::cSprite::Draw()
{
	auto* const direct3dImmediateContext = sContext::g_context.direct3dImmediateContext;
	EAE6320_ASSERT( direct3dImmediateContext );

	direct3dImmediateContext->Draw(4, 0);
}

// Initialization / Clean Up
//--------------------------

eae6320::cResult eae6320::Graphics::cSprite::CleanUp()
{
	auto result = Results::Success;

	if ( m_vertexBuffer )
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
	}

	if ( m_vertexInputLayout )
	{
		m_vertexInputLayout->Release();
		m_vertexInputLayout = nullptr;
	}

	return result;
}

eae6320::cResult eae6320::Graphics::cSprite::InitializeGeometry()
{
	auto result = eae6320::Results::Success;

	auto* const direct3dDevice = eae6320::Graphics::sContext::g_context.direct3dDevice;
	EAE6320_ASSERT( direct3dDevice );

	// Initialize vertex format
	{
		// Load the compiled binary vertex shader for the input layout
		eae6320::Platform::sDataFromFile vertexShaderDataFromFile;
		std::string errorMessage;
		if ( result = eae6320::Platform::LoadBinaryFile( "data/Shaders/Vertex/sprite.shader", vertexShaderDataFromFile, &errorMessage ) )
		{
			// Create the vertex layout

			// These elements must match the VertexFormats::sMesh layout struct exactly.
			// They instruct Direct3D how to match the binary data in the vertex buffer
			// to the input elements in a vertex shader
			// (by using so-called "semantic" names so that, for example,
			// "POSITION" here matches with "POSITION" in shader code).
			// Note that OpenGL uses arbitrarily assignable number IDs to do the same thing.
			constexpr unsigned int vertexElementCount = 2;
			D3D11_INPUT_ELEMENT_DESC layoutDescription[vertexElementCount] = {};
			{
				// Slot 0

				// POSITION
				// 3 floats == 12 bytes
				// Offset = 0
				{
					auto& positionElement = layoutDescription[0];

					positionElement.SemanticName = "POSITION";
					positionElement.SemanticIndex = 0;	// (Semantics without modifying indices at the end can always use zero)
					positionElement.Format = DXGI_FORMAT_R8G8_SNORM;
					positionElement.InputSlot = 0;
					positionElement.AlignedByteOffset = offsetof( eae6320::Graphics::VertexFormats::sSprite, x );
					positionElement.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
					positionElement.InstanceDataStepRate = 0;	// (Must be zero for per-vertex data)
				}

				// Slot 1

				// TEXCOORD
				// 2 floats == 8 bytes
				// Offset = 0
				{
					auto& textureCoordinateElement = layoutDescription[1];

					textureCoordinateElement.SemanticName = "TEXCOORD";
					textureCoordinateElement.SemanticIndex = 0;	// (Semantics without modifying indices at the end can always use zero)
					textureCoordinateElement.Format = DXGI_FORMAT_R8G8_UNORM;
					textureCoordinateElement.InputSlot = 0;
					textureCoordinateElement.AlignedByteOffset = offsetof( eae6320::Graphics::VertexFormats::sSprite, u );
					textureCoordinateElement.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
					textureCoordinateElement.InstanceDataStepRate = 0;	// (Must be zero for per-vertex data)
				}
			}

			const auto d3dResult = direct3dDevice->CreateInputLayout( layoutDescription, vertexElementCount,
				vertexShaderDataFromFile.data, vertexShaderDataFromFile.size, &m_vertexInputLayout );
			if ( FAILED( result ) )
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF( false, "Geometry vertex input layout creation failed (HRESULT %#010x)", d3dResult );
				eae6320::Logging::OutputError( "Direct3D failed to create the geometry vertex input layout (HRESULT %#010x)", d3dResult );
			}

			vertexShaderDataFromFile.Free();
		}
		else
		{
			EAE6320_ASSERTF( false, errorMessage.c_str() );
			eae6320::Logging::OutputError( "The geometry vertex input layout shader couldn't be loaded: %s", errorMessage.c_str() );
			goto OnExit;
		}
	}
	// Vertex Buffer
	{
		D3D11_BUFFER_DESC bufferDescription{};
		{
			const auto bufferSize = 4 * sizeof( eae6320::Graphics::VertexFormats::sSprite );
			EAE6320_ASSERT( bufferSize < ( uint64_t( 1u ) << ( sizeof( bufferDescription.ByteWidth ) * 8 ) ) );
			bufferDescription.ByteWidth = static_cast<unsigned int>( bufferSize );
			bufferDescription.Usage = D3D11_USAGE_IMMUTABLE;	// In our class the buffer will never change after it's been created
			bufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bufferDescription.CPUAccessFlags = 0;	// No CPU access is necessary
			bufferDescription.MiscFlags = 0;
			bufferDescription.StructureByteStride = 0;	// Not used
		}
		D3D11_SUBRESOURCE_DATA initialData{};
		{
			initialData.pSysMem = &m_spriteVertices[0];
			// (The other data members are ignored for non-texture buffers)
		}

		const auto d3dResult = direct3dDevice->CreateBuffer( &bufferDescription, &initialData, &m_vertexBuffer );
		if ( FAILED( d3dResult ) )
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF( false, "Geometry vertex buffer creation failed (HRESULT %#010x)", d3dResult );
			eae6320::Logging::OutputError( "Direct3D failed to create a geometry vertex buffer (HRESULT %#010x)", d3dResult );
			goto OnExit;
		}

	}

OnExit:

	return result;
}
