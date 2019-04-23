// Includes
//=========

#include "cEnvironmentBuilder.h"
#include "cEnvironmentLoader.h"

#include <Tools/AssetBuildLibrary/Functions.h>
#include <Engine/Platform/Platform.h>
#include <fstream>
#include <iostream>
#include <Engine/Time/Time.h>

// Inherited Implementation
//=========================

// Build
//------

eae6320::cResult eae6320::Assets::cEnvironmentBuilder::Build(const std::vector<std::string>& i_arguments)
{
	auto result = Results::Success;
	std::string errorMessage;

	std::string environmentPath;
	size_t environmentPathSize = 0;

	std::ofstream outFile ( m_path_target, std::ofstream::binary);
	{
		if ( !( result = cEnvironmentLoader::LoadAsset( m_path_source, environmentPath)) )
		{
			EAE6320_ASSERTF( false, "Loading material failed" );
			goto OnExit;
		}
	}

	environmentPath.insert(0, "data/");

	// + 1 for null terminator count
	environmentPathSize = environmentPath.size() + 1;

	// write to outfile
	if (!outFile.is_open())
	{
		goto OnExit;
	}

	outFile.write ( (char *)&environmentPathSize, sizeof( uint16_t ) );
	outFile.write ( environmentPath.c_str(), environmentPath.size() );
	outFile.put('\0');

OnExit:
	outFile.close();

	return result;
}
