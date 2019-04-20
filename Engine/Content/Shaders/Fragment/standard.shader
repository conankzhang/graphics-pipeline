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

	float3 g_pointLight_position;
	float g_padding3;
	float4 g_pointLight_color;
};

cbuffer g_constantBuffer_perMaterial : register( b1 )
{
	float4 g_color;
	float4 g_reflectivity;
	float g_gloss;
	float3 g_padding4;
};

cbuffer g_constantBuffer_perDrawCall : register( b2 )
{
  float4x4 g_transform_localToWorld;
};

DeclareTexture2d(g_diffuseTexture, 0);
DeclareSamplerState(g_diffuse_samplerState, 0);

// Entry Point
//============

void main(
	in const float2 i_textureCoordinates : OTEXCOORD,
	in const float3 i_normal_world : ONORMAL,
	in const float3 i_position_world : OPOSITION,
	// Output
	//=======

	// Whatever color value is output from the fragment shader
	// will determine the color of the corresponding pixel on the screen
	out float4 o_color : SV_TARGET

	)
{
	// Directional Light Diffuse
	const float dDotProduct = dot(g_lightDirection, normalize(i_normal_world));
	const float dClampedValue = saturate(dDotProduct);

	float4 directionalColor = g_directionalLight_color * dClampedValue;

	// Directional Light Specular
	float3 V = normalize(g_camera_position - i_position_world);

	float3 dH = normalize((V + g_lightDirection) * 0.5 );
	float dDotClamped = saturate(dot(normalize(i_normal_world), dH));
	float4 dSpecularLight = g_reflectivity * pow(dDotClamped, g_gloss) * directionalColor;

	// Point Light Diffuse
	float3 pointLightDirection = (g_pointLight_position - i_position_world);

	const float pDotProduct = dot(pointLightDirection, normalize(i_normal_world));
	const float pClampedValue = saturate(pDotProduct);

	float4 positionColor = g_pointLight_color * pClampedValue;

	// Point Light Specular
	float3 L = (g_pointLight_position - i_position_world);
	float3 pH = normalize((V + L) * 0.5 );
	float pDotClamped = saturate(dot(normalize(i_normal_world), pH));
	float attenuation = max(length(L), 1);
	float4 pSpecularLight = (1/attenuation) * g_reflectivity * pow(pDotClamped, g_gloss) * positionColor;

	float4 textureColor = SampleTexture2d(g_diffuseTexture, g_diffuse_samplerState, i_textureCoordinates);
	o_color = ((g_color * textureColor) + dSpecularLight + pSpecularLight) * (directionalColor + g_ambient_color + (positionColor * 1 / attenuation));
}
