/*
	This is the standard vertex shader

	A vertex shader is responsible for two things:
		* Telling the GPU where the vertex (one of the three in a triangle) should be drawn on screen in a given window
			* The GPU will use this to decide which fragments (i.e. pixels) need to be shaded for a given triangle
		* Providing any data that a corresponding fragment shader will need
			* This data will be interpolated across the triangle and thus vary for each fragment of a triangle that gets shaded
*/

#include <Shaders/shaders.inc>

// Constant Buffers
//=================

cbuffer g_constantBuffer_perFrame : register( b0 )
{
	float4x4 g_transform_worldToCamera;
	float4x4 g_transform_cameraToProjected;
	float3 g_camera_position;

	float g_elapsedSecondCount_systemTime;
	float g_elapsedSecondCount_simulationTime;
	// For float4 alignment
	float g_padding;
};

cbuffer g_constantBuffer_perDrawCall : register( b2 )
{
  float4x4 g_transform_localToWorld;
  float4x4 g_transform_localToProjected;
};

DeclareTexture2d(g_diffuseTexture, 0);
DeclareSamplerState(g_diffuse_samplerState, 0);

// Entry Point
//============

void main(

	// Input
	//======

	// The "semantics" (the keywords in all caps after the colon) are arbitrary,
	// but must match the C call to CreateInputLayout()

	// These values come from one of the VertexFormats::sMesh that the vertex buffer was filled with in C code
	in const float3 i_vertexPosition_local : POSITION,
	in const float2 i_textureCoordinates : TEXCOORD,

	// Output
	//=======

	// An SV_POSITION value must always be output from every vertex shader
	// so that the GPU can figure out which fragments need to be shaded
	out float2 o_textureCoordinates : OTEXCOORD,
	out float4 o_vertexPosition_projected : SV_POSITION
	)
{
	{
		// Project the vertex from local space into projected space
		float4 vertexPosition_local = float4( i_vertexPosition_local, 1.0 );
		o_vertexPosition_projected = mul( g_transform_localToProjected, vertexPosition_local );

		float2 scrollTextureCoordinates = i_textureCoordinates;
		scrollTextureCoordinates.x += sin(g_elapsedSecondCount_simulationTime);
		scrollTextureCoordinates.y += cos(g_elapsedSecondCount_simulationTime);
		o_textureCoordinates = scrollTextureCoordinates;;
	}
}
