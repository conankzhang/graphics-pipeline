// Includes
//=========

#include "cMaterialLoader.h"

#include <External/Lua/Includes.h>
#include <Tools/AssetBuildLibrary/Functions.h>
#include <Engine/Platform/Platform.h>
#include <iostream>
#include <Engine/Graphics/sColor.h>

// Inherited Implementation
//=========================

// Build
//------

eae6320::cResult eae6320::Assets::cMaterialLoader::LoadAsset(const char* const i_path, std::string& o_effectPath, std::string& o_texturePath, std::string& o_normalPath, std::string& o_roughPath, std::string& o_parallaxPath, Graphics::sColor& o_color, Graphics::sColor& o_reflectivity, float& o_gloss, float& o_fresnel)
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
	result = LoadTableValues(*luaState, o_effectPath, o_texturePath, o_normalPath, o_roughPath, o_parallaxPath, o_color, o_reflectivity, o_gloss, o_fresnel);

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

eae6320::cResult eae6320::Assets::cMaterialLoader::LoadTableValues(lua_State& io_luaState, std::string& o_effectPath, std::string& o_texturePath, std::string& o_normalPath, std::string& o_roughPath, std::string& o_parallaxPath, Graphics::sColor& o_color, Graphics::sColor& o_reflectivity, float& o_gloss, float& o_fresnel)
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
		for (int i = 1; i <= valueCount; ++i)
		{
			lua_pushinteger(&io_luaState, i);
			lua_gettable(&io_luaState, -2);
			const auto value = lua_tonumber(&io_luaState, -1);

			if (i == 1)
			{
				o_color.red = static_cast<float>(value);
			}
			else if (i == 2)
			{
				o_color.green = static_cast<float>(value);
			}
			else if (i == 3)
			{
				o_color.blue = static_cast<float>(value);
			}
			else if (i == 4)
			{
				o_color.alpha = static_cast<float>(value);
			}

			lua_pop(&io_luaState, 1);
		}

		lua_pop(&io_luaState, 1);
	}
	else
	{
		o_color.red = 1.0f;
		o_color.green = 1.0f;
		o_color.blue = 1.0f;
		o_color.alpha = 1.0f;

		lua_pop( &io_luaState, 1 );
	}

	key = "path_texture";
	lua_pushstring( &io_luaState, key );
	lua_gettable( &io_luaState, -2 );

	if ( lua_isstring( &io_luaState, -1 ) )
	{
		const auto value = lua_tostring( &io_luaState, -1 );
		o_texturePath = static_cast<std::string>(value);
		lua_pop( &io_luaState, 1 );
	}
	else
	{
		o_texturePath = "Textures/default_diffuse.tga";
		lua_pop( &io_luaState, 1 );
	}

	key = "reflectivity";
	lua_pushstring( &io_luaState, key );
	lua_gettable( &io_luaState, -2 );

	if ( lua_istable( &io_luaState, -1 ) )
	{
		const auto valueCount = luaL_len( &io_luaState, -1 );
		for (int i = 1; i <= valueCount; ++i)
		{
			lua_pushinteger(&io_luaState, i);
			lua_gettable(&io_luaState, -2);
			const auto value = lua_tonumber(&io_luaState, -1);

			if (i == 1)
			{
				o_reflectivity.red = static_cast<float>(value);
			}
			else if (i == 2)
			{
				o_reflectivity.green = static_cast<float>(value);
			}
			else if (i == 3)
			{
				o_reflectivity.blue = static_cast<float>(value);
			}
			else if (i == 4)
			{
				o_reflectivity.alpha = static_cast<float>(value);
			}

			lua_pop(&io_luaState, 1);
		}

		lua_pop(&io_luaState, 1);
	}
	else
	{
		o_reflectivity.red = 0.5f;
		o_reflectivity.green = 0.5f;
		o_reflectivity.blue = 0.5f;
		o_reflectivity.alpha = 0.5f;

		lua_pop( &io_luaState, 1 );
	}

	key = "gloss";
	lua_pushstring( &io_luaState, key );
	lua_gettable( &io_luaState, -2 );

	if ( lua_isnumber( &io_luaState, -1 ) )
	{
		const auto value = lua_tonumber( &io_luaState, -1 );
		o_gloss = static_cast<float>(value);
		lua_pop( &io_luaState, 1 );
	}
	else
	{
		o_gloss = 1000.0f;
		lua_pop( &io_luaState, 1 );
	}

	key = "fresnel";
	lua_pushstring( &io_luaState, key );
	lua_gettable( &io_luaState, -2 );

	if ( lua_isnumber( &io_luaState, -1 ) )
	{
		const auto value = lua_tonumber( &io_luaState, -1 );
		o_fresnel = static_cast<float>(value);
		lua_pop( &io_luaState, 1 );
	}
	else
	{
		o_fresnel = 0.04f;
		lua_pop( &io_luaState, 1 );
	}

	key = "path_normal";
	lua_pushstring( &io_luaState, key );
	lua_gettable( &io_luaState, -2 );

	if ( lua_isstring( &io_luaState, -1 ) )
	{
		const auto value = lua_tostring( &io_luaState, -1 );
		o_normalPath = static_cast<std::string>(value);
		lua_pop( &io_luaState, 1 );
	}
	else
	{
		o_normalPath = "Textures/default_normal.tga";
		lua_pop( &io_luaState, 1 );
	}

	key = "path_rough";
	lua_pushstring( &io_luaState, key );
	lua_gettable( &io_luaState, -2 );

	if ( lua_isstring( &io_luaState, -1 ) )
	{
		const auto value = lua_tostring( &io_luaState, -1 );
		o_roughPath = static_cast<std::string>(value);
		lua_pop( &io_luaState, 1 );
	}
	else
	{
		o_roughPath = "Textures/default_rough.tga";
		lua_pop( &io_luaState, 1 );
	}

	key = "path_parallax";
	lua_pushstring( &io_luaState, key );
	lua_gettable( &io_luaState, -2 );

	if ( lua_isstring( &io_luaState, -1 ) )
	{
		const auto value = lua_tostring( &io_luaState, -1 );
		o_parallaxPath = static_cast<std::string>(value);
		lua_pop( &io_luaState, 1 );
	}
	else
	{
		o_parallaxPath = "Textures/default_parallax.tga";
		lua_pop( &io_luaState, 1 );
	}

OnExit:

	return result;
}
