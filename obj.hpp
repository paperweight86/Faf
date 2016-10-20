#pragma once

namespace obj
{
	struct model
	{
		char [MAX_OBJ_MDL_NAME]

		uti::float3*	verts;
		uti::u32		num_verts;

		uti::float3*	normals;
		uti::u32		num_normals;

		uti::float3*    texcoords;
		uti::u32		num_texcoords;
	};
}
