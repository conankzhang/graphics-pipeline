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
		{ path = "Meshes/smallSphere.mesh" },
	},

	materials =
	{
		{ path = "Materials/standard.material" },
		{ path = "Materials/red.material" },
		{ path = "Materials/unlit.material" },
		{ path = "Materials/brick.material" },
	},
}
