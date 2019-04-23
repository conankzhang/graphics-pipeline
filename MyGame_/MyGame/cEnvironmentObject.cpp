#include "cEnvironmentObject.h"

#include <Engine/Math/cMatrix_transformation.h>

eae6320::cEnvironmentObject::cEnvironmentObject(char * const i_environmentPath)
{
	// Initialize the environment object data
	if (i_environmentPath)
	{
		auto result = Results::Success;

		{
			if ( !( result = eae6320::Graphics::cEnvironment::s_manager.Load(i_environmentPath, m_environment) ) )
			{
				EAE6320_ASSERT( false );
			}
		}
	}
}

eae6320::cResult eae6320::cEnvironmentObject::CleanUp()
{
	auto result = Results::Success;
	if ( m_environment )
	{
		const auto localResult = Graphics::cEnvironment::s_manager.Release( m_environment );
		if ( !localResult )
		{
			EAE6320_ASSERT( false );
			if ( result )
			{
				result = localResult;
			}
		}
	}

	return result;
}

const eae6320::Graphics::cEnvironment::Handle& eae6320::cEnvironmentObject::GetEnvironment()
{
	return m_environment;
}
