// Includes
//=========

#include "cMaterialBuilder.h"
#include "cMaterialLoader.h"

#include <Tools/AssetBuildLibrary/Functions.h>
#include <Engine/Platform/Platform.h>
#include <fstream>
#include <iostream>
#include <Engine/Time/Time.h>
#include <Engine/Graphics/cRenderState.h>
#include <Engine/Graphics/sColor.h>

// Inherited Implementation
//=========================

// Build
//------

eae6320::cResult eae6320::Assets::cMaterialBuilder::Build(const std::vector<std::string>& i_arguments)
{
	auto result = Results::Success;
	std::string errorMessage;

	std::string effectPath;
	std::string texturePath;
	std::string normalPath;
	std::string roughPath;
	Graphics::sColor color;
	Graphics::sColor reflectivity;
	float gloss;
	float fresnel;
	size_t effectPathSize = 0;
	size_t texturePathSize = 0;
	size_t normalPathSize = 0;

	std::ofstream outFile ( m_path_target, std::ofstream::binary);
	{
		if ( !( result = cMaterialLoader::LoadAsset( m_path_source, effectPath, texturePath, normalPath, roughPath, color, reflectivity, gloss, fresnel)) )
		{
			EAE6320_ASSERTF( false, "Loading material failed" );
			goto OnExit;
		}
	}

	effectPath.insert(0, "data/");
	texturePath.insert(0, "data/");
	normalPath.insert(0, "data/");
	roughPath.insert(0, "data/");

	// + 1 for null terminator count
	effectPathSize = effectPath.size() + 1;
	texturePathSize = texturePath.size() + 1;
	normalPathSize = normalPath.size() + 1;

	// write to outfile
	if (!outFile.is_open())
	{
		goto OnExit;
	}

	outFile.write ( reinterpret_cast<char *>(&color.GetLinearColor()), sizeof( color ) );
	outFile.write ( reinterpret_cast<char *>(&reflectivity.GetLinearColor()), sizeof( reflectivity) );
	outFile.write ( reinterpret_cast<char *>(&gloss), sizeof( gloss ) );
	outFile.write ( reinterpret_cast<char *>(&fresnel), sizeof( fresnel ) );
	outFile.write ( (char *)&effectPathSize, sizeof( uint16_t ) );
	outFile.write ( (char *)&texturePathSize, sizeof( uint16_t ) );
	outFile.write ( (char *)&normalPathSize, sizeof( uint16_t ) );
	outFile.write ( effectPath.c_str(), effectPath.size() );
	outFile.put('\0');
	outFile.write ( texturePath.c_str(), texturePath.size());
	outFile.put('\0');
	outFile.write ( normalPath.c_str(), normalPath.size());
	outFile.put('\0');
	outFile.write ( roughPath.c_str(), roughPath.size());
	outFile.put('\0');

OnExit:
	outFile.close();

	return result;
}
