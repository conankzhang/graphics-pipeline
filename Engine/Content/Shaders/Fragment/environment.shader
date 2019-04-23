/*
	This is the standard fragment shader

	A fragment shader is responsible for telling the GPU what color a specific fragment should be
*/

#include <Shaders/shaders.inc>

// Constant Buffers
//=================

cbuffer g_constantBuffer_perFrame : register( b0 )
{
	float4x4 g_transform_worldToCamera;
	float4x4 g_transform_cameraToProjected;
	float3 g_camera_position;
	float g_padding0;

	float3 g_lightDirection;
	float g_padding1;

	float4 g_directionalLight_color;
	float4 g_ambient_color;

	float g_elapsedSecondCount_systemTime;
	float g_elapsedSecondCount_simulationTime;
	// For float4 alignment
	float2 g_padding2;
};

cbuffer g_constantBuffer_perMaterial : register( b1 )
{
  float4 g_color;
};

cbuffer g_constantBuffer_perDrawCall : register( b2 )
{
  float4x4 g_transform_localToWorld;
};

DeclareTexture2d(g_diffuseTexture, 0);
DeclareTextureCube(g_environmentTexture, 3);
DeclareSamplerState(g_diffuse_samplerState, 0);

// Entry Point
//============

void main(
	in const float3 i_position : OPOSITION,
	// Output
	//=======

	// Whatever color value is output from the fragment shader
	// will determine the color of the corresponding pixel on the screen
	out float4 o_color : SV_TARGET

	)
{
	float4 environmentColor = SampleTextureCube(g_environmentTexture, g_diffuse_samplerState, i_position);
	o_color = (environmentColor);
}
