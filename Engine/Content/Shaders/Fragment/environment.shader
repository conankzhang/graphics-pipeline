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
	in const float2 i_textureCoordinates : OTEXCOORD,
	in const float3 i_normal_world : ONORMAL,
	in const float3 i_tangent_world : OTANGENT,
	in const float3 i_bitangent_world : OBITANGENT,
	in const float3 i_position_world : OPOSITION,
	// Output
	//=======

	// Whatever color value is output from the fragment shader
	// will determine the color of the corresponding pixel on the screen
	out float4 o_color : SV_TARGET

	)
{
	float4 environmentColor = SampleTextureCube(g_environmentTexture, g_diffuse_samplerState, i_position_world);
	o_color = (environmentColor);
}