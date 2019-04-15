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

	float g_elapsedSecondCount_systemTime;
	float g_elapsedSecondCount_simulationTime;

	// For float4 alignment
	float g_padding;
};

cbuffer g_constantBuffer_perDrawCall : register( b2 )
{
  float4x4 g_transform_localToWorld;
};
// Entry Point
//============

void main(
	// Input
	//=======

	in float4 i_vertexPosition : VERTEX_POSITION,


	// Output
	//=======

	// Whatever color value is output from the fragment shader
	// will determine the color of the corresponding pixel on the screen
	out float4 o_color : SV_TARGET

	)
{
	o_color = float4(
		sin(i_vertexPosition.x * 50),
		sin(i_vertexPosition.y * 50),
		sin(i_vertexPosition.z * 50),
		1.0
	);
}
