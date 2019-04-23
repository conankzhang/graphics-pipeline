/*
	This class represents environment objects.
*/

#ifndef EAE6320_CENVIRONMENTOBJECT_H
#define EAE6320_CENVIRONMENTOBJECT_H

// Includes
//=========
#include <Engine/Graphics/cEnvironment.h>

// Forward Declarations
//=====================

// Class Declaration
//==================

namespace eae6320
{
	class cEnvironmentObject
	{
	public:
		cEnvironmentObject(char * const i_environmentPath);
		cResult CleanUp();

		const Graphics::cEnvironment::Handle& GetEnvironment();

	protected:
		Graphics::cEnvironment::Handle m_environment;
	};
}

#endif	// EAE6320_CENVIRONMENTOBJECT_H
