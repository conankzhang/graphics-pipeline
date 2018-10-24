// Includes
//=========

#include "cMeshBuilder.h"
#include "cMeshLoader.h"

#include <Tools/AssetBuildLibrary/Functions.h>
#include <Engine/Platform/Platform.h>
#include <Engine/Graphics/VertexFormats.h>

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

	{
		if ( !( result = cMeshLoader::LoadAsset( m_path_source, vertexData, indexData )) )
		{
			EAE6320_ASSERTF( false, "Initialization of new mesh failed" );
			goto OnExit;
		}
	}

	if (vertexData.size() > 65536)
	{
		EAE6320_ASSERTF( false, "Mesh has too many vertices." );
		Logging::OutputError( "%s has too many vertices. Stopping application. ", m_path_source);
		result = Results::Failure;
		goto OnExit;
	}

	result = eae6320::Platform::CopyFile(m_path_source, m_path_target, false, true, &errorMessage);

	{
		if ( !( result = eae6320::Platform::CopyFile(m_path_source, m_path_target) ) )
		{
			OutputErrorMessageWithFileInfo( m_path_source, errorMessage.c_str());
			EAE6320_ASSERT( false );
			goto OnExit;
		}
	}

OnExit:

	return result;
}
