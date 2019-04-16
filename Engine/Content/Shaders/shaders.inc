/*
	This file should be #included by all shaders
*/

// Version Information
//====================

#if defined( EAE6320_PLATFORM_GL )

	// GLSL shaders require the version to be #defined before anything else in the shader
	#version 420

#endif

// Constant Buffers
//=================

#if defined( EAE6320_PLATFORM_D3D )

	#define DeclareConstantBuffer( i_name, i_id ) cbuffer i_name : register( b##i_id )
	#define DeclareTexture2d( i_name, i_id ) Texture2D i_name : register( t##i_id )
	#define DeclareSamplerState( i_name, i_id ) SamplerState i_name : register( s##i_id )

	#define SampleTexture2d( i_texture, i_samplerState, i_uv ) i_texture.Sample( i_samplerState, i_uv )

#elif defined( EAE6320_PLATFORM_GL )

	#define DeclareConstantBuffer( i_name, i_id ) layout( std140, binding = i_id ) uniform i_name

	#define DeclareTexture2d( i_name, i_id ) layout( binding = i_id ) uniform sampler2D i_name
	#define DeclareSamplerState( i_name, i_id )	// GLSL requires the sampler state to be set in C code

	#define SampleTexture2d( i_texture, i_samplerState, i_uv ) texture2D( i_texture, i_uv )

#endif
