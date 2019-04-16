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
	Graphics::sColor color;
	size_t effectPathSize = 0;

	std::ofstream outFile ( m_path_target, std::ofstream::binary);
	{
		if ( !( result = cMaterialLoader::LoadAsset( m_path_source, effectPath, color)) )
		{
			EAE6320_ASSERTF( false, "Loading material failed" );
			goto OnExit;
		}
	}

	effectPath.insert(0, "data/");

	// + 1 for null terminator count
	effectPathSize = effectPath.size() + 1;

	// write to outfile
	if (!outFile.is_open())
	{
		goto OnExit;
	}

	outFile.write ( reinterpret_cast<char *>(&color), sizeof( color ) );
	outFile.write ( (char *)&effectPathSize, sizeof( uint16_t ) );
	outFile.write ( effectPath.c_str(), effectPath.size() );
	outFile.put('\0');

OnExit:
	outFile.close();

	return result;
}