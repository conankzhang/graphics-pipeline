// Includes
//=========

#include "cMaterialLoader.h"

#include <External/Lua/Includes.h>
#include <Tools/AssetBuildLibrary/Functions.h>
#include <Engine/Platform/Platform.h>
#include <iostream>
#include <Engine/Graphics/VertexFormats.h>
#include <Engine/Graphics/cRenderState.h>

// Inherited Implementation
//=========================

// Build
//------

eae6320::cResult eae6320::Assets::cMaterialLoader::LoadAsset(const char* const i_path, std::string& o_effectPath, std::vector<float>& o_colorData)

{
	auto result = eae6320::Results::Success;

	// Create a new Lua state
	lua_State* luaState = nullptr;
	{
		luaState = luaL_newstate();
		if ( !luaState )
		{
			result = eae6320::Results::OutOfMemory;
			std::cerr << "Failed to create a new Lua state" << std::endl;
			return result;
		}
	}

	// Load the asset file as a "chunk",
	// meaning there will be a callable function at the top of the stack
	const auto stackTopBeforeLoad = lua_gettop( luaState );
	{
		const auto luaResult = luaL_loadfile( luaState, i_path );
		if ( luaResult != LUA_OK )
		{
			result = eae6320::Results::Failure;
			std::cerr << lua_tostring( luaState, -1 ) << std::endl;
			// Pop the error message
			lua_pop( luaState, 1 );
			goto OnExit;
		}
	}
	// Execute the "chunk", which should load the asset
	// into a table at the top of the stack
	{
		constexpr int argumentCount = 0;
		constexpr int returnValueCount = LUA_MULTRET;	// Return _everything_ that the file returns
		constexpr int noMessageHandler = 0;
		const auto luaResult = lua_pcall( luaState, argumentCount, returnValueCount, noMessageHandler );
		if ( luaResult == LUA_OK )
		{
			// A well-behaved asset file will only return a single value
			const auto returnedValueCount = lua_gettop( luaState ) - stackTopBeforeLoad;
			if ( returnedValueCount == 1 )
			{
				// A correct asset file _must_ return a table
				if ( !lua_istable( luaState, -1 ) )
				{
					result = eae6320::Results::InvalidFile;
					std::cerr << "Asset files must return a table (instead of a "
						<< luaL_typename( luaState, -1 ) << ")" << std::endl;
					// Pop the returned non-table value
					lua_pop( luaState, 1 );
					goto OnExit;
				}
			}
			else
			{
				result = eae6320::Results::InvalidFile;
				std::cerr << "Asset files must return a single table (instead of "
					<< returnedValueCount << " values)" << std::endl;
				// Pop every value that was returned
				lua_pop( luaState, returnedValueCount );
				goto OnExit;
			}
		}
		else
		{
			result = eae6320::Results::InvalidFile;
			std::cerr << lua_tostring( luaState, -1 );
			// Pop the error message
			lua_pop( luaState, 1 );
			goto OnExit;
		}
	}

	// If this code is reached the asset file was loaded successfully,
	// and its table is now at index -1
	result = LoadTableValues(*luaState, o_effectPath, o_colorData);

	// Pop the table
	lua_pop( luaState, 1 );

OnExit:

	if ( luaState )
	{
		// If I haven't made any mistakes
		// there shouldn't be anything on the stack,
		// regardless of any errors encountered while loading the file:
		EAE6320_ASSERT( lua_gettop( luaState ) == 0 );

		lua_close( luaState );
		luaState = nullptr;
	}

	return result;
}

eae6320::cResult eae6320::Assets::cMaterialLoader::LoadTableValues(lua_State& io_luaState, std::string& o_effectPath, std::vector<float>& o_colorData)
{
	auto result = eae6320::Results::Success;

	auto* key = "path_effect";
	lua_pushstring( &io_luaState, key );
	lua_gettable( &io_luaState, -2 );

	if ( lua_isstring( &io_luaState, -1 ) )
	{
		const auto value = lua_tostring( &io_luaState, -1 );
		o_effectPath = static_cast<std::string>(value);
		lua_pop( &io_luaState, 1 );
	}
	else
	{
		result = eae6320::Results::InvalidFile;
		std::cerr << "The value at \"" << key << "\" must be a table "
			"(instead of a " << luaL_typename( &io_luaState, -1 ) << ")" << std::endl;
		goto OnExit;
	}

	key = "color";
	lua_pushstring( &io_luaState, key );
	lua_gettable( &io_luaState, -2 );

	if ( lua_istable( &io_luaState, -1 ) )
	{
		const auto valueCount = luaL_len( &io_luaState, -1 );
		for ( int i = 1; i <= valueCount; ++i )
		{
			lua_pushinteger( &io_luaState, i );
			lua_gettable( &io_luaState, -2 );
			const auto value = lua_tonumber( &io_luaState, -1 );

			o_colorData.push_back(static_cast<float>(value));
			lua_pop( &io_luaState, 1 );
		}

		lua_pop( &io_luaState, 1 );
	}
	else
	{
		o_colorData.push_back(1.0f);
		o_colorData.push_back(1.0f);
		o_colorData.push_back(1.0f);
		o_colorData.push_back(1.0f);

		lua_pop( &io_luaState, 1 );
	}

OnExit:

	return result;
}
