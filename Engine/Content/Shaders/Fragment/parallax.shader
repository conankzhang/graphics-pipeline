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
	float g_height;
	float g_padding4;
};

cbuffer g_constantBuffer_perDrawCall : register( b2 )
{
  float4x4 g_transform_localToWorld;
};

DeclareTexture2d(g_diffuseTexture, 0);
DeclareTexture2d(g_normalTexture, 1);
DeclareTexture2d(g_roughTexture, 2);
DeclareTextureCube(g_environmentTexture, 3);
DeclareTexture2d(g_parallaxTexture, 4);
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

	// Calculate Parallax Texture
	float3 eyeDirection = normalize(g_camera_position - i_position_world);
	float3 viewDirection = -eyeDirection;

	float3 viewDirectionTS = mul(viewDirection, rotation_textureToSomeOtherSpace);
	float2 maxParallaxOffset = -viewDirectionTS.xy * g_height / viewDirectionTS.z;

	int sampleCount = (int)lerp(64, 8, dot(eyeDirection, normal));
	float zStep = 1.0f / (float)sampleCount;

	float2 texStep = maxParallaxOffset * zStep;

	float2 dx = ddx(i_textureCoordinates);
	float2 dy = ddy(i_textureCoordinates);

	int sampleIndex = 0;
	float2 currentCoordinateOffset = 0;
	float2 previousCoordinateOffset = 0;
	float2 finalCoordinateOffset = 0;
	float currentRayZ = 1.0f - zStep;
	float previousRayZ = 1.0f;
	float currentHeight = 0.0f;
	float previousHeight = 0.0f;

	while(sampleIndex < sampleCount + 1)
	{
		currentHeight = g_parallaxTexture.SampleGrad(g_diffuse_samplerState, i_textureCoordinates + currentCoordinateOffset, dx, dy).a;

		if(currentHeight > currentRayZ)
		{
			float t = (previousHeight - previousRayZ) / (previousHeight - currentHeight + currentRayZ - previousRayZ);
			finalCoordinateOffset = previousCoordinateOffset + t * texStep;
			sampleIndex = sampleCount + 1;
		}
		else
		{
			++sampleIndex;
			previousCoordinateOffset = currentCoordinateOffset;
			previousRayZ = currentRayZ;
			previousHeight = currentHeight;
			currentCoordinateOffset += texStep;
			currentRayZ -= zStep;
		}
	}

	float2 parallaxCoordinates = i_textureCoordinates + finalCoordinateOffset;

	// Get Normal
	float4 mapNormal = (SampleTexture2d( g_normalTexture, g_diffuse_samplerState, parallaxCoordinates) - 0.5) * 2.0;
	float3 tangentNormal = normalize(mul(rotation_textureToSomeOtherSpace, mapNormal.xyz));

	// Material Color
	float4 textureColor = SampleTexture2d(g_diffuseTexture, g_diffuse_samplerState, parallaxCoordinates);
	float4 dFLV = (g_color * textureColor) / PI;

	// Directional Diffuse
	float4 dLi = g_directionalLight_color * PI;
	float4 dLoD = dLi * saturate(dot(g_lightDirection, tangentNormal));

	// Point Diffuse
	float3 pL = normalize(g_pointLight_position - i_position_world);
	float4 pLi = g_pointLight_color * PI;
	float attenuation = 1 / max(length(g_pointLight_position - i_position_world), 1);
	float4 pLoD =  pLi * saturate(dot(pL, tangentNormal)) * attenuation;

	// Calculate reflectance
	float3 dReflectedDirection = normalize(reflect(tangentNormal, i_position_world - g_camera_position));
	float4 environmentColor = SampleTextureCube(g_environmentTexture, g_diffuse_samplerState, dReflectedDirection);

	float4 FLeft = (1 - g_fresnel);
	float4 dFRightE = pow((1 - dot(normalize(g_camera_position - i_position_world), tangentNormal)), 5);

	environmentColor *= g_fresnel + (FLeft * dFRightE);

	// Final Diffuse
	float4 diffuse = (dFLV + environmentColor) * (dLoD + pLoD + g_ambient_color);

	float roughness = SampleTexture2d( g_roughTexture, g_diffuse_samplerState, parallaxCoordinates).r;
	float gloss = pow(2, roughness * 8);

	// Directional Specular
	float4 DhLeft = ((gloss) + 2 )/ 8;

	float3 V = normalize(g_camera_position - i_position_world);
	float3 dH = normalize( (V + g_lightDirection) * 0.5 );

	float4 dDhRight = pow(saturate(dot(tangentNormal, dH)), gloss);
	float4 dDh = DhLeft * dDhRight;

	float4 dFRight = pow((1 - dot(g_lightDirection, dH)), 5);
	float4 dF = g_fresnel + (FLeft * dFRight);

	float4 directionalSpecular = dDh * dF * dot(tangentNormal, g_lightDirection);

	// Positional Specular
	float3 pH = normalize( (V + pL) * 0.5 );

	float4 pDhRight = pow(saturate(dot(tangentNormal, pH)), gloss);
	float4 pDh = DhLeft * pDhRight;

	float4 pFRight = pow((1 - dot(pL, pH)), 5);
	float4 pF = g_fresnel + (FLeft * pFRight);

	float4 positionSpecular = pDh * pF * dot(tangentNormal, pL);

	// Final Specular
	float4 specular = directionalSpecular + positionSpecular;

	// Combine diffuse and specular
	o_color = diffuse + specular;
}
