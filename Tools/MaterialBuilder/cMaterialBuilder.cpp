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

// Inherited Implementation
//=========================

// Build
//------

eae6320::cResult eae6320::Assets::cMaterialBuilder::Build(const std::vector<std::string>& i_arguments)
{
	auto result = Results::Success;
	std::string errorMessage;

	std::string effectPath;
	std::vector<float> colorData;
	size_t effectPathSize = 0;

	std::ofstream outFile ( m_path_target, std::ofstream::binary);
	{
		if ( !( result = cMaterialLoader::LoadAsset( m_path_source, effectPath, colorData)) )
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

	outFile.write ( (char *)&colorData[0], colorData.size() * sizeof( float ) );
	outFile.write ( (char *)&effectPathSize, sizeof( uint16_t ) );
	outFile.write ( effectPath.c_str(), effectPath.size() );
	outFile.put('\0');

OnExit:
	outFile.close();

	return result;
}
