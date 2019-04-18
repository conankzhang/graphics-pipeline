// Includes
//=========

#include "cSprite.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Platform/Platform.h>
#include <Engine/Time/Time.h>
#include <new>
#include "VertexFormats.h"

#include <External/Lua/Includes.h>
#include <iostream>
#include <string>

// Static Data Initialization
//===========================

// Interface
//==========

// Initialization / Clean Up
//--------------------------

eae6320::cResult eae6320::Graphics::cSprite::CleanUp()
{
	return Results::Success;
}
