// Includes
//=========

#include "cRenderState.h"

// Interface
//==========

// Alpha Transparency
constexpr bool eae6320::Graphics::RenderStates::IsAlphaTransparencyEnabled( const uint8_t i_renderStateBits )
{
	return ( i_renderStateBits & AlphaTransparency ) != 0;
}
void eae6320::Graphics::RenderStates::EnableAlphaTransparency( uint8_t& io_renderStateBits )
{
	io_renderStateBits |= AlphaTransparency;
}
void eae6320::Graphics::RenderStates::DisableAlphaTransparency( uint8_t& io_renderStateBits )
{
	io_renderStateBits &= ~AlphaTransparency;
}

// Depth Buffering
constexpr bool eae6320::Graphics::RenderStates::IsDepthBufferingEnabled( const uint8_t i_renderStateBits )
{
	return ( i_renderStateBits & DepthBuffering ) != 0;
}
void eae6320::Graphics::RenderStates::EnableDepthBuffering( uint8_t& io_renderStateBits )
{
	io_renderStateBits |= DepthBuffering;
}
void eae6320::Graphics::RenderStates::DisableDepthBuffering( uint8_t& io_renderStateBits )
{
	io_renderStateBits &= ~DepthBuffering;
}

// Draw Both Triangle Sides
constexpr bool eae6320::Graphics::RenderStates::ShouldBothTriangleSidesBeDrawn( const uint8_t i_renderStateBits )
{
	return ( i_renderStateBits & DrawBothTriangleSides ) != 0;
}
void eae6320::Graphics::RenderStates::EnableDrawingBothTriangleSides( uint8_t& io_renderStateBits )
{
	io_renderStateBits |= DrawBothTriangleSides;
}
void eae6320::Graphics::RenderStates::DisableDrawingBothTriangleSides( uint8_t& io_renderStateBits )
{
	io_renderStateBits &= ~DrawBothTriangleSides;
}

// Access
//-------

bool eae6320::Graphics::cRenderState::IsAlphaTransparencyEnabled() const
{
	return RenderStates::IsAlphaTransparencyEnabled( m_bits );
}

bool eae6320::Graphics::cRenderState::IsDepthBufferingEnabled() const
{
	return RenderStates::IsDepthBufferingEnabled( m_bits );
}

bool eae6320::Graphics::cRenderState::ShouldBothTriangleSidesBeDrawn() const
{
	return RenderStates::ShouldBothTriangleSidesBeDrawn( m_bits );
}

uint8_t eae6320::Graphics::cRenderState::GetRenderStateBits() const
{
	return m_bits;
}

// Initialization / Clean Up
//--------------------------

eae6320::cResult eae6320::Graphics::cRenderState::Initialize( const uint8_t i_renderStateBits )
{
	m_bits = i_renderStateBits;
	return InitializeFromBits();
}

eae6320::Graphics::cRenderState::~cRenderState()
{
	CleanUp();
}
