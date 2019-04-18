--[[
	This file lists every asset that must be built by the AssetBuildSystem
]]

return
{
	shaders =
	{
		{ path = "Shaders/Vertex/vertexInputLayout.shader", arguments = { "vertex" } },
		{ path = "Shaders/Vertex/sprite.shader", arguments = { "vertex" } },
	},

	meshes =
	{
		{ path = "Meshes/sphere.mesh" },
	},

	materials =
	{
		{ path = "Materials/standard.material" },
		{ path = "Materials/brick.material" },
		{ path = "Materials/sprite.material" },
	},
}
