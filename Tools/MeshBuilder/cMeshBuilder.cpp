// Includes
//=========

#include "cMeshBuilder.h"

#include <Tools/AssetBuildLibrary/Functions.h>
#include <Engine/Platform/Platform.h>

// Inherited Implementation
//=========================

// Build
//------

eae6320::cResult eae6320::Assets::cMeshBuilder::Build( const std::vector<std::string>& i_arguments )
{
	auto result = Results::Success;
	std::string errorMessage;
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

	/**
	// Decide which kind of shader program to compile
	Graphics::ShaderTypes::eType shaderType = Graphics::ShaderTypes::Unknown;
	{
		if ( i_arguments.size() >= 1 )
		{
			const std::string& argument = i_arguments[0];
			if ( argument == "vertex" )
			{
				shaderType = Graphics::ShaderTypes::Vertex;
			}
			else if ( argument == "fragment" )
			{
				shaderType = Graphics::ShaderTypes::Fragment;
			}
			else
			{
				OutputErrorMessageWithFileInfo( m_path_source,
					"\"%s\" is not a valid shader program type", argument );
				return Results::Failure;
			}
		}
		else
		{
			OutputErrorMessageWithFileInfo( m_path_source,
				"A Shader must be built with an argument defining which type of shader program (e.g. \"vertex\" or \"fragment\") to compile" );
			return Results::Failure;
		}
	}

	return Build( shaderType, i_arguments );
	*/
}
