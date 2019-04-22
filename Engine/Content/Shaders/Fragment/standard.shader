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
DeclareTexture2d(g_normalTexture, 1);
DeclareTexture2d(g_roughTexture, 2);
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
	// Calculate Tangent Normal
	float3 normal = normalize(i_normal_world);
	float3 tangent = normalize(i_tangent_world);
	float3 bitangent = normalize(i_bitangent_world);

	float3x3 rotation_textureToSomeOtherSpace = float3x3(
		tangent.x, bitangent.x, normal.x,
		tangent.y, bitangent.y, normal.y,
		tangent.z, bitangent.z, normal.z
	);

	float4 mapNormal = (SampleTexture2d( g_normalTexture, g_diffuse_samplerState, i_textureCoordinates) - 0.5) * 2.0;
	float3 tangentNormal = normalize(mul(rotation_textureToSomeOtherSpace, mapNormal.xyz));

	// Directional Light Diffuse
	const float dDotProduct = dot(g_lightDirection, tangentNormal);
	const float dClampedValue = saturate(dDotProduct);

	float4 directionalColor = g_directionalLight_color * dClampedValue;

	// Directional Light Specular
	float3 V = normalize(g_camera_position - i_position_world);

	float3 dH = normalize((V + g_lightDirection) * 0.5 );
	float dDotClamped = saturate(dot(tangentNormal, dH));
	float4 dSpecularLight = g_reflectivity * pow(dDotClamped, g_gloss) * directionalColor;

	// Point Light Diffuse
	float3 L = (g_pointLight_position - i_position_world);

	const float pDotProduct = dot(L, tangentNormal);
	const float pClampedValue = saturate(pDotProduct);

	float4 positionColor = g_pointLight_color * pClampedValue;

	// Point Light Specular
	float3 pH = normalize((V + L) * 0.5 );
	float pDotClamped = saturate(dot(tangentNormal, pH));
	float attenuation = 1 - (saturate(length(L) / 5));
	float4 pSpecularLight = (attenuation) * g_reflectivity * pow(pDotClamped, g_gloss) * positionColor;

	float4 textureColor = SampleTexture2d(g_diffuseTexture, g_diffuse_samplerState, i_textureCoordinates);
	o_color = ((g_color * textureColor) + dSpecularLight + pSpecularLight) * (directionalColor + g_ambient_color + (positionColor * attenuation));
}
