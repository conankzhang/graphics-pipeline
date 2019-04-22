/*
	This is the standard fragment shader

	A fragment shader is responsible for telling the GPU what color a specific fragment should be
*/

#include <Shaders/shaders.inc>

// Constants
//=================

static const float PI = 3.14159265f;

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
	float g_fresnel;
	float2 g_padding4;
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

	// Material Color
	float4 textureColor = SampleTexture2d(g_diffuseTexture, g_diffuse_samplerState, i_textureCoordinates);
	float4 dFLV = (g_color * textureColor) / PI;

	// Directional Diffuse
	float4 dLi = g_directionalLight_color * PI;
	float4 dLoD = dLi * saturate(dot(g_lightDirection, tangentNormal));

	// Point Diffuse
	float3 pL = normalize(g_pointLight_position - i_position_world);
	float4 pLi = g_pointLight_color * PI;
	float attenuation = 1 / max(length(g_pointLight_position - i_position_world), 1);
	float4 pLoD =  pLi * saturate(dot(pL, tangentNormal)) * attenuation;

	// Final Diffuse
	float4 diffuse = dFLV * (dLoD + pLoD + g_ambient_color);

	// Directional Specular
	float4 DhLeft = ((g_gloss) + 2 )/ 8;

	float3 V = normalize(g_camera_position - i_position_world);
	float3 dH = normalize( (V + g_lightDirection) * 0.5 );

	float4 dDhRight = pow(dot(tangentNormal, dH), g_gloss);
	float4 dDh = DhLeft * dDhRight;

	float4 FLeft = g_fresnel + (1 - g_fresnel);
	float4 dFRight = pow((1 - dot(g_lightDirection, dH)), 5);
	float4 dF = FLeft * dFRight;

	float4 directionalSpecular = dDh * dF * dot(tangentNormal, g_lightDirection);

	// Positional Specular
	float3 pH = normalize( (V + pL) * 0.5 );

	float4 pDhRight = pow(dot(tangentNormal, pH), g_gloss);
	float4 pDh = DhLeft * pDhRight;

	float4 pFRight = pow((1 - dot(pL, pH)), 5);
	float4 pF = FLeft * pFRight;

	float4 positionSpecular = pDh * pF * dot(tangentNormal, pL);

	// Final Specular
	float4 specular = directionalSpecular + positionSpecular;

	// Combine diffuse and specular
	o_color = diffuse + specular;
}
