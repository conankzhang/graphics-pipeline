--[[
	This file lists every asset that must be built by the AssetBuildSystem
]]

return
{
	shaders =
	{
		{ path = "Shaders/Vertex/vertexInputLayout.shader", arguments = { "vertex" } },
	},

	meshes =
	{
		{ path = "Meshes/plane.mesh" },
		{ path = "Meshes/sphere.mesh" },
	},

	materials =
	{
		{ path = "Materials/standard.material" },
	},
}
