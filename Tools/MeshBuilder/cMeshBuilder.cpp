// Includes
//=========

#include "cMeshBuilder.h"
#include "cMeshLoader.h"

#include <Tools/AssetBuildLibrary/Functions.h>
#include <Engine/Platform/Platform.h>
#include <Engine/Graphics/VertexFormats.h>
#include <fstream>
#include <iostream>
#include <Engine/Time/Time.h>

// Inherited Implementation
//=========================

// Build
//------

eae6320::cResult eae6320::Assets::cMeshBuilder::Build( const std::vector<std::string>& i_arguments )
{
	auto result = Results::Success;
	std::string errorMessage;

	std::vector<Graphics::VertexFormats::sMesh> vertexData;
	std::vector<uint16_t> indexData;
	std::ofstream outFile ( m_path_target, std::ofstream::binary);
	size_t vertexCount = 0;
	size_t indexCount = 0;
	int i = 0;

	{
		if ( !( result = cMeshLoader::LoadAsset( m_path_source, vertexData, indexData )) )
		{
			EAE6320_ASSERTF( false, "Initialization of new mesh failed" );
			goto OnExit;
		}
	}

	// write to outfile
	if (!outFile.is_open())
	{
		goto OnExit;
	}

	vertexCount = vertexData.size();
	indexCount = indexData.size();

	outFile.write ( (char *)&vertexCount, sizeof( uint16_t ) );
	outFile.write ( (char *)&indexCount, sizeof( uint16_t ) );
	outFile.write ( (char *)&vertexData[0], vertexCount * sizeof( Graphics::VertexFormats::sMesh ) );
	outFile.write ( (char *)&indexData[0], indexCount * sizeof( uint16_t ) );

OnExit:
	outFile.close();

	return result;
}
