#pragma once

#include <assert.h>
#include <algorithm>

#define OBJ_MAX_NAME 256
#define OBJ_MAX_PATH 256
#define OBJ_COMMENT '#'

#ifndef TAT_WINDOWS
	#define memcpy_s(s,ss,d,ds) memcpy(s,d,ds)
#endif // TAT_WINDOWS

namespace obj
{
	enum primative
	{
		primative_none,
		primative_line,
		primative_polygon,
		primative_triangle,
	};

	struct face
	{
		uti::u32  pos, tex, nrm;
	};

	struct object
	{
		char			name[OBJ_MAX_NAME];

		float*		vertices;
		uti::u32	num_vertices;
		uti::u32    num_vertex_floats;
		bool		has_texcoords;

		uti::u32*	indices;
		uti::u32	num_indices;

		char		material_name[OBJ_MAX_NAME];
		bool		smooth;

		primative   primative;
	};

	enum class e_illum : uti::u8
	{
		Color = 0,						//Color on and Ambient off
		Color_Ambient,					//Color on and Ambient on
		Color_Ambient_Specular,			//Highlight on
		Reflection_Raytrace,			//Reflection on and Ray trace on
		Glass_Raytrace,					//Transparency: Glass on, Reflection : Ray trace on
		Fresnel_Raytrace,				//Reflection : Fresnel on and Ray trace on
		Refraction_NoFresnel_Raytrace,	//Transparency : Refraction on, Reflection : Fresnel off and Ray trace on
		Refraction_Fresnel_Raytrace,	//Transparency : Refraction on, Reflection : Fresnel on and Ray trace on
		Reflection_NoRaytrace,			//Reflection on and Ray trace off
		Glass_NoRaytrace,				//Transparency : Glass on, Reflection : Ray trace off
		Cast_To_Invisible,				//Casts shadows onto invisible surfaces
	};

	struct material
	{
		char	name[OBJ_MAX_NAME];
		float	diffuse_color[3];
		float	ambient_color[3];
		float	specular_color[3];
		float	emissive_color[3];
		float	specular_exponent;
		char	diffuse_texture[OBJ_MAX_PATH];
	};

	struct document
	{
		char		material_lib[OBJ_MAX_PATH];

		object*		objects;
		uti::u32	num_objects;

		material*	materials;
		uti::u32	num_materials;
	};

	enum line_type : char
	{
		line_type_none = 0,
		line_type_comment,
		line_type_object,
		line_type_vertex,
		line_type_normal,
		line_type_texcoord,
		line_type_smooth,
		line_type_face,
		line_type_line,
		line_type_material_library,
		line_type_material,
	};

	#define OBJ_LINE_RETURN_IF_TYPE(line, type, ret) if(strncmp(line, type, strlen(type)) == 0) return ret
	#define OBJ_LINE_COMMENT	"#"
	#define OBJ_LINE_OBJECT		"o "
	#define OBJ_LINE_VERTEX		"v "
	#define OBJ_LINE_NORMAL		"vn "
	#define OBJ_LINE_TEXCOORD	"vt "
	#define OBJ_LINE_SMOOTH		"s "
	#define OBJ_LINE_FACE		"f "
	#define OBJ_LINE_LINE		"l "
	#define OBJ_LINE_MTLLIB		"mtllib "
	#define OBJ_LINE_MATERIAL	"usemtl "

	line_type determine_line_type(char* line)
	{
		OBJ_LINE_RETURN_IF_TYPE(line, OBJ_LINE_COMMENT,		line_type_comment);
		OBJ_LINE_RETURN_IF_TYPE(line, OBJ_LINE_OBJECT,		line_type_object);
		OBJ_LINE_RETURN_IF_TYPE(line, OBJ_LINE_VERTEX,		line_type_vertex);
		OBJ_LINE_RETURN_IF_TYPE(line, OBJ_LINE_NORMAL,		line_type_normal);
		OBJ_LINE_RETURN_IF_TYPE(line, OBJ_LINE_TEXCOORD,	line_type_texcoord);
		OBJ_LINE_RETURN_IF_TYPE(line, OBJ_LINE_SMOOTH,		line_type_smooth);
		OBJ_LINE_RETURN_IF_TYPE(line, OBJ_LINE_FACE,		line_type_face);
		OBJ_LINE_RETURN_IF_TYPE(line, OBJ_LINE_LINE,		line_type_line);
		OBJ_LINE_RETURN_IF_TYPE(line, OBJ_LINE_MTLLIB,		line_type_material_library);
		OBJ_LINE_RETURN_IF_TYPE(line, OBJ_LINE_MATERIAL,	line_type_material);

		return line_type_none;
	}

	enum class mtl_line_type: char
	{
		none = 0,
		comment,
		name,
		diffuse_color,
		ambient_color,
		specular_color,
		emissive_color,
		specular_exponent,
		diffuse_texture,
	};

	#define MTL_LINE_COMMENT	"#"
	#define MTL_LINE_NAME		"newmtl "
	#define MTL_LINE_DIFFUSE	"Kd "
	#define MTL_LINE_AMBIENT	"Ka "
	#define MTL_LINE_SPECULAR	"Ks "
	#define MTL_LINE_EMISSIVE	"Ke "
	#define MTL_LINE_SPECEXP	"Ns "
	#define MTL_LINE_DIFFTEX	"map_Kd "

	mtl_line_type determine_line_type_mtl(char* line)
	{
		OBJ_LINE_RETURN_IF_TYPE(line, MTL_LINE_COMMENT	, mtl_line_type::comment			);
		OBJ_LINE_RETURN_IF_TYPE(line, MTL_LINE_NAME		, mtl_line_type::name				);
		OBJ_LINE_RETURN_IF_TYPE(line, MTL_LINE_DIFFUSE	, mtl_line_type::diffuse_color		);
		OBJ_LINE_RETURN_IF_TYPE(line, MTL_LINE_AMBIENT	, mtl_line_type::ambient_color		);
		OBJ_LINE_RETURN_IF_TYPE(line, MTL_LINE_SPECULAR	, mtl_line_type::specular_color		);
		OBJ_LINE_RETURN_IF_TYPE(line, MTL_LINE_EMISSIVE	, mtl_line_type::emissive_color		);
		OBJ_LINE_RETURN_IF_TYPE(line, MTL_LINE_SPECEXP	, mtl_line_type::specular_exponent	);
		OBJ_LINE_RETURN_IF_TYPE(line, MTL_LINE_DIFFTEX	, mtl_line_type::diffuse_texture	);

		return mtl_line_type::none;
	}

	// There's a bunch of gotos in here as I've never tried using them so I wanted to see (for this case) how they fit
	bool load_material_library(char* data, uti::u64 len_data, material** materials_out, uti::u32* num_materials_out)
	{
		uti::u64 data_pos = 0;
		mtl_line_type cur_line_type = mtl_line_type::none;
		uti::rearray<uti::u64> pos_materials;

		while (data_pos < len_data)
		{
			size_t line_end_off = str::find_char(data + data_pos, '\n', len_data - data_pos);
			if (line_end_off > len_data || data_pos + line_end_off > len_data)
			{
				line_end_off = len_data - data_pos;
			}

			// Go past the \n
			line_end_off += 1;
			//assert(line_end_off >= 2);
			cur_line_type = determine_line_type_mtl(data + data_pos);
			switch (cur_line_type)
			{
			case mtl_line_type::name:
				pos_materials.add_end(data_pos);
				break;
			default:
				break;
			}
			data_pos += line_end_off;
		}

		*materials_out = new material[pos_materials.count];
		memset(&(*materials_out)[0], 0, sizeof(material) * pos_materials.count);
		*num_materials_out = (uti::u32)pos_materials.count;

		const int float_buffer_len = 32;
		char float_buffer[float_buffer_len] = {};
		size_t off_to_float = 0;
		size_t off_to_end_float = 0;

		for (uti::i64 i = 0; i < pos_materials.count; ++i)
		{
			material* cur_mat = &(*materials_out)[i];
			data_pos = pos_materials[i];

			uti::u64 pos_next_mtl = len_data;
			if (i != pos_materials.count - 1)
				pos_next_mtl = pos_materials[i + 1];

			while (data_pos < pos_next_mtl)
			{
				size_t line_end_off = str::find_char(data + data_pos, '\n', len_data - data_pos);
				if (line_end_off > len_data || data_pos + line_end_off > len_data)
				{
					line_end_off = len_data - data_pos;
				}

				// Go past the \n
				line_end_off += 1;

				//assert(line_end_off >= 2);
				cur_line_type = determine_line_type_mtl(data + data_pos);

				uti::i64 idx = 0;
				float* cur_colour = nullptr;

				size_t sub_off = 0;

				using type = mtl_line_type;
				switch (cur_line_type)
				{
				case type::comment:
					break;
				case type::name:
					memcpy_s(cur_mat->name, OBJ_MAX_NAME,
						data + data_pos + strlen(MTL_LINE_NAME), line_end_off - 1 - strlen(MTL_LINE_NAME));
					break;
				case type::diffuse_color:
					cur_colour = &cur_mat->diffuse_color[0];
					goto dothreefloatread;
				case type::ambient_color:
					cur_colour = &cur_mat->ambient_color[0];
					goto dothreefloatread;
					break;
				case type::specular_color:
					cur_colour = &cur_mat->specular_color[0];
					goto dothreefloatread;
					break;
				case type::emissive_color:
					cur_colour = &cur_mat->emissive_color[0];
					goto dothreefloatread;
					break;
				case type::specular_exponent:
					cur_colour = &cur_mat->emissive_color[0];
					goto doonefloatread;
					break;
				case type::diffuse_texture:
					memcpy_s(cur_mat->diffuse_texture, OBJ_MAX_PATH,
						data + data_pos + strlen(MTL_LINE_DIFFTEX), line_end_off - 1 - strlen(MTL_LINE_DIFFTEX));
					data_pos += line_end_off;
					break;
				default:
					break;
				}

				goto end;

			dothreefloatread:
				assert(cur_colour != nullptr);
				off_to_float = str::strOffToNextFloat(data + data_pos + sub_off);
				off_to_end_float = str::strOffToEndFloat(data + data_pos + sub_off + off_to_float);
				memcpy_s(float_buffer, float_buffer_len, data + data_pos + sub_off + off_to_float, off_to_end_float);
				cur_colour[++idx] = (float)atof(float_buffer);

				sub_off += off_to_float + off_to_end_float;
				memset(float_buffer, 0, float_buffer_len);

				off_to_float = str::strOffToNextFloat(data + data_pos + sub_off);
				off_to_end_float = str::strOffToEndFloat(data + data_pos + sub_off + off_to_float);
				memcpy_s(float_buffer, float_buffer_len, data + data_pos + sub_off + off_to_float, off_to_end_float);
				cur_colour[++idx] = (float)atof(float_buffer);

				sub_off += off_to_float + off_to_end_float;
				memset(float_buffer, 0, float_buffer_len);

			doonefloatread:
				assert(cur_colour != nullptr);
				off_to_float = str::strOffToNextFloat(data + data_pos + sub_off);
				off_to_end_float = str::strOffToEndFloat(data + data_pos + sub_off + off_to_float);
				memcpy_s(float_buffer, float_buffer_len, data + data_pos + sub_off + off_to_float, off_to_end_float);
				cur_colour[++idx] = (float)atof(float_buffer);

			end:

				data_pos += line_end_off;
			}
		}

		return true;
	}

	bool load_obj(char* data, uti::u64 len_data, document* doc)
	{
		float*		positions = nullptr;
		uti::u32    num_positions = 0;

		float*		normals = nullptr;
		uti::u32	num_normals = 0;

		float*		texcoords = nullptr;
		uti::u32	num_texcoords = 0;

		face*		faces = nullptr;
		uti::u32	num_faces = 0;

		uti::u64 vertices_start = 0;
		uti::u64 normals_start = 0;
		uti::u64 texcoords_start = 0;
		uti::u64 faces_start = 0;
		uti::u64 data_pos = 0;
		bool	 smooth = true;
		line_type cur_line_type = line_type_none;

		uti::rearray<uti::u64> pos_objects;

		while (data_pos < len_data)
		{
			size_t line_end_off = str::find_char(data + data_pos, '\n', len_data - data_pos);
			if (line_end_off > len_data || data_pos + line_end_off > len_data)
			{
				line_end_off = len_data - data_pos;
			}

			// Go past the \n
			line_end_off += 1;
			//assert(line_end_off >= 2);
			cur_line_type = determine_line_type(data + data_pos);
			switch (cur_line_type)
			{
			case line_type_object:
			{
				pos_objects.add_end(data_pos);
				break;
			}
			case line_type_material_library:
			{
				memcpy_s(doc->material_lib, OBJ_MAX_PATH, data + data_pos + strlen(OBJ_LINE_MTLLIB), line_end_off - 1 - strlen(OBJ_LINE_MTLLIB));
				break;
			}
			default:
				break;
			}
			data_pos += line_end_off;
		}

		data_pos = 0;

		doc->objects = new object[pos_objects.count];
		memset(doc->objects, 0, pos_objects.count * sizeof(object));
		doc->num_objects = (uti::u32)pos_objects.count;

		// Indices are based on the total number of verts in the file rather than from those defined for the object
		// this is space saving but a faff
		uti::u32 positions_offset = 0;

		uti::i32 max_last_pos_idx	= 1;
		uti::i32 max_last_tex_idx	= 1;
		uti::i32 max_last_norm_idx	= 1;

		for (uti::u32 i = 0; i < pos_objects.count; ++i)
		{

			num_faces = 0;
			vertices_start = 0;
			faces_start = 0;
			normals_start = 0;
			texcoords_start = 0;
			object* cur_obj = doc->objects+i;
			cur_obj->primative = primative_none;

			data_pos = pos_objects[i];

			uti::u64 pos_next_obj = len_data;
			if (i != pos_objects.count - 1)
				pos_next_obj = pos_objects[i + 1];

			while (data_pos < pos_next_obj)
			{
				// TODO: Skip space
				size_t line_end_off = str::find_char(data + data_pos, '\n', len_data - data_pos);
				if (line_end_off > len_data || data_pos + line_end_off > len_data)
				{
					break;
				}
				// Go past the \n
				line_end_off += 1;
//				assert(line_end_off >= 2);
				cur_line_type = determine_line_type(data + data_pos);
				switch (cur_line_type)
				{
				case line_type_object:
					memcpy_s(cur_obj->name,		  OBJ_MAX_NAME,
							 data + data_pos + 2, line_end_off-1-2);
					data_pos += line_end_off;
					break;
				case line_type_vertex:
					if (vertices_start == 0)
						vertices_start = data_pos;
					++num_positions;
					data_pos += line_end_off;
					break;
				case line_type_normal:
					if (normals_start == 0)
						normals_start = data_pos;
					++num_normals;
					data_pos += line_end_off;
					break;
				case line_type_texcoord:
					if (texcoords_start == 0)
						texcoords_start = data_pos;
					++num_texcoords;
					data_pos += line_end_off;
					break;
				case line_type_face:
					if (faces_start == 0)
						faces_start = data_pos;
					++num_faces;
					data_pos += line_end_off;
					// TODO: Calculate number of face components instead
					cur_obj->primative = primative_triangle;
					break;
				case line_type_line:
				{
					if (faces_start == 0)
						faces_start = data_pos;
					uti::u64 tmp_data_pos = data_pos;
					line_end_off = data_pos + line_end_off;
					//we are only getting two lines!
					while (tmp_data_pos < line_end_off - 1)
					{
						uti::u64 off_to_next_idx = str::find_number(data + tmp_data_pos, line_end_off - tmp_data_pos);
						if (off_to_next_idx < line_end_off - tmp_data_pos - 1)
						{
							++num_faces;

							off_to_next_idx = tmp_data_pos + off_to_next_idx + str::find_end_number(data + tmp_data_pos + off_to_next_idx, line_end_off - tmp_data_pos - off_to_next_idx);
						}
						tmp_data_pos = off_to_next_idx;
					}
					data_pos = line_end_off;
					cur_obj->primative = primative_line;
					//++num_faces; // for a break between lines
					break;
				}
				case line_type_smooth:
				{
					data_pos += 1;
					line_end_off -= 1;
					size_t start_smooth_off = str::find_not_char(data + data_pos, ' ', line_end_off);
					if (line_end_off - start_smooth_off >= 2)
					{
						if (strncmp("off", data + data_pos + start_smooth_off, 3) == 0)
							smooth = false;
					}
					data_pos += line_end_off;
					break;
				}
				case line_type_material:
				{
					memcpy_s(cur_obj->material_name, OBJ_MAX_NAME,
						data + data_pos + strlen(OBJ_LINE_MATERIAL), line_end_off - 1 - strlen(OBJ_LINE_MATERIAL));
					data_pos += line_end_off;
					break;
				}
				case line_type_comment:
				case line_type_none:
				default:
					data_pos += line_end_off;
					break;
				}
			}

			cur_obj->smooth = smooth;

			int num_primative_verts = 3;
			if (cur_obj->primative == primative_line)
				num_primative_verts = 1;
			int num_vertex_elements = 3; // pos nrm tex
			if (cur_obj->primative == primative_line)
				num_vertex_elements = 1;
			const int num_vertex_element_vals = 3; // x y z/u v w

			positions = new float[num_positions*num_vertex_element_vals];
			texcoords = new float[num_texcoords*num_vertex_element_vals];
			normals = new float[num_normals*num_vertex_element_vals];

			data_pos = vertices_start;
			for (uti::u32 j = 0; j < num_positions; ++j)
			{
				const int float_buffer_len = 32;
				char float_buffer[float_buffer_len] = {};
				float* cur_vert = positions + j * 3;
				size_t off_to_float = str::strOffToNextFloat(data + data_pos);
				size_t off_to_end_float = str::strOffToEndFloat(data + data_pos + off_to_float);
				memcpy_s(float_buffer, float_buffer_len, data + data_pos + off_to_float, off_to_end_float);
				cur_vert[0] = (float)atof(float_buffer);

				data_pos += off_to_float + off_to_end_float;
				memset(float_buffer, 0, float_buffer_len);

				off_to_float = str::strOffToNextFloat(data + data_pos);
				off_to_end_float = str::strOffToEndFloat(data + data_pos + off_to_float);
				memcpy_s(float_buffer, float_buffer_len, data + data_pos + off_to_float, off_to_end_float);
				cur_vert[1] = (float)atof(float_buffer);

				data_pos += off_to_float + off_to_end_float;

				off_to_float = str::strOffToNextFloat(data + data_pos);
				off_to_end_float = str::strOffToEndFloat(data + data_pos + off_to_float);
				memcpy_s(float_buffer, float_buffer_len, data + data_pos + off_to_float, off_to_end_float);
				cur_vert[2] = (float)atof(float_buffer);

				data_pos += off_to_float + off_to_end_float;
			}

			data_pos = normals_start;
			for (uti::u32 j = 0; j < num_normals; ++j)
			{
				const int float_buffer_len = 32;
				char float_buffer[float_buffer_len] = {};
				float* cur_norm = normals + j * 3;
				size_t off_to_float = str::strOffToNextFloat(data + data_pos);
				size_t off_to_end_float = str::strOffToEndFloat(data + data_pos + off_to_float);
				memcpy_s(float_buffer, float_buffer_len, data + data_pos + off_to_float, off_to_end_float);
				cur_norm[0] = (float)atof(float_buffer);

				data_pos += off_to_float + off_to_end_float;
				memset(float_buffer, 0, float_buffer_len);

				off_to_float = str::strOffToNextFloat(data + data_pos);
				off_to_end_float = str::strOffToEndFloat(data + data_pos + off_to_float);
				memcpy_s(float_buffer, float_buffer_len, data + data_pos + off_to_float, off_to_end_float);
				cur_norm[1] = (float)atof(float_buffer);

				data_pos += off_to_float + off_to_end_float;

				off_to_float = str::strOffToNextFloat(data + data_pos);
				off_to_end_float = str::strOffToEndFloat(data + data_pos + off_to_float);
				memcpy_s(float_buffer, float_buffer_len, data + data_pos + off_to_float, off_to_end_float);
				cur_norm[2] = (float)atof(float_buffer);

				data_pos += off_to_float + off_to_end_float;
			}

			data_pos = texcoords_start;
			for (uti::u32 j = 0; j < num_texcoords; ++j)
			{
				const int float_buffer_len = 32;
				char float_buffer[float_buffer_len] = {};
				float* cur_tex = texcoords + j * 3;
				size_t off_to_float = str::strOffToNextFloat(data + data_pos);
				size_t off_to_end_float = str::strOffToEndFloat(data + data_pos + off_to_float);
				memcpy_s(float_buffer, float_buffer_len, data + data_pos + off_to_float, off_to_end_float);
				cur_tex[0] = (float)atof(float_buffer);

				data_pos += off_to_float + off_to_end_float;
				memset(float_buffer, 0, float_buffer_len);

				off_to_float = str::strOffToNextFloat(data + data_pos);
				off_to_end_float = str::strOffToEndFloat(data + data_pos + off_to_float);
				memcpy_s(float_buffer, float_buffer_len, data + data_pos + off_to_float, off_to_end_float);
				cur_tex[1] = (float)atof(float_buffer);

				data_pos += off_to_float + off_to_end_float;

				//off_to_float = str::strOffToNextFloat(data + data_pos);
				//off_to_end_float = str::strOffToEndFloat(data + data_pos + off_to_float);
				//memcpy_s(float_buffer, float_buffer_len, data + data_pos + off_to_float, off_to_end_float);
				cur_tex[2] = 0.0f;

				//data_pos += off_to_float + off_to_end_float;
			}

			if (cur_obj->primative == primative_line)
			{
				cur_obj->num_vertex_floats = num_positions * num_vertex_elements * num_vertex_element_vals;
				cur_obj->num_vertices = num_positions;
				cur_obj->vertices = new float[cur_obj->num_vertex_floats];
				const uti::u32 size_vertices = cur_obj->num_vertex_floats * sizeof(float);
				memcpy_s(cur_obj->vertices, size_vertices,
						 positions,			size_vertices);

				cur_obj->indices = new uti::u32[num_faces];
				num_faces = num_faces;

				data_pos = faces_start;

				uti::u32 line_idx = 0;

				while (determine_line_type(data + data_pos) == line_type_line)
				{
					size_t line_end_off = str::find_char(data + data_pos, '\n', len_data - data_pos);
					if (line_end_off > len_data || data_pos + line_end_off > len_data)
					{
						line_end_off = len_data - data_pos;
						//data_pos = u32_max;
						//break;
					}
					// Go past the \n
					line_end_off += 1;
					assert(line_end_off >= 2);
					
					const int str_int_buffer_len = 32;
					char str_int_buffer[str_int_buffer_len] = {};

					line_end_off = data_pos + line_end_off;

					while (data_pos < line_end_off)
					{
						uti::u64 off_to_next_idx = str::find_number(data + data_pos, line_end_off - data_pos);
						if (off_to_next_idx < line_end_off - data_pos - 1)
						{
							uti::u64 off_to_end_idx = str::find_end_number(data + data_pos + off_to_next_idx, line_end_off - data_pos - off_to_next_idx);
									
							memcpy_s(str_int_buffer, str_int_buffer_len, 
								    data + data_pos + off_to_next_idx, off_to_end_idx);
							cur_obj->indices[line_idx] = atoi(str_int_buffer) - 1 - positions_offset;

							++line_idx;
							off_to_next_idx = off_to_end_idx + off_to_next_idx;
						}
						data_pos += off_to_next_idx;
					}
					data_pos = line_end_off;
					// Indicate end of line
					//cur_obj->indices[line_idx] = cur_obj->indices[line_idx-1];

					//++line_idx;
				}

				cur_obj->num_indices = num_faces;
			}
			else if (cur_obj->primative == primative_triangle)
			{
				faces = new face[num_faces*num_primative_verts];
				num_faces = num_faces*num_primative_verts;

				data_pos = faces_start;

				for (uti::u32 j = 0; j < num_faces; ++j)
				{
					const int str_int_buffer_len = 32;
					char str_int_buffer[str_int_buffer_len] = {};
					face* cur_face = faces + j;
					uti::i64 to_int = str::strOffToNextFloat(data + data_pos);
					uti::i64 to_slash_1 = 0;
					uti::i64 to_slash_2 = 0;
					if (num_vertex_elements > 1)
					{
						to_slash_1 = str::find_char(data + data_pos, '/', len_data - data_pos);
						assert(to_slash_1 != UTI_STR_FIND_NOT_FOUND);
						if (to_slash_1 == UTI_STR_FIND_NOT_FOUND)
						{
							// 1st slash is missing
							// TODO: Report syntax is wrong / my shitty algorithm can't understand it

							delete[] positions;
							delete[] normals;
							delete[] texcoords;

							return false;
						}

						if (to_slash_1 < to_int)
						{
							// Position is missing
							// TODO: Report syntax is wrong / my shitty algorithm can't understand it

							delete[] positions;
							delete[] normals;
							delete[] texcoords;

							return false;
						}

						to_slash_2 = str::find_char(data + data_pos + to_slash_1 + 1, '/', len_data - (data_pos + to_slash_1 + 1));
						assert(to_slash_2 != UTI_STR_FIND_NOT_FOUND);
						if (to_slash_2 == UTI_STR_FIND_NOT_FOUND)
						{
							// 2nd slash is missing
							// TODO: Report syntax is wrong / my shitty algorithm can't understand it

							delete[] positions;
							delete[] normals;
							delete[] texcoords;

							return false;
						}
					}

					size_t to_space = str::find_char(data + data_pos + to_slash_1 + 1 + to_slash_2, ' ', len_data - (data_pos + to_slash_1 + 1 + to_slash_2));
					to_space = std::min(to_space, len_data - (data_pos + to_slash_1 + 1 + to_slash_2));

					size_t to_line_end = str::find_char(data + data_pos + to_slash_1 + 1 + to_slash_2, '\n', len_data - (data_pos + to_slash_1 + 1 + to_slash_2));
					to_line_end = std::min(to_line_end, len_data - (data_pos + to_slash_1 + 1 + to_slash_2));

					uti::u64 to_end_face = to_space < to_line_end ? to_space : to_line_end;
					to_end_face = std::min(to_end_face, len_data - (data_pos + to_slash_1 + 1 + to_slash_2));

					if (to_slash_1 != 0)
					{
						memcpy_s(str_int_buffer, str_int_buffer_len, data + data_pos + to_int, to_slash_1 - to_int);
						uti::i32 abs_pos_idx = atoi(str_int_buffer);
						assert(abs_pos_idx >= max_last_pos_idx && abs_pos_idx > 0);
						cur_face->pos = abs_pos_idx - max_last_pos_idx;
					}

					memset(str_int_buffer, 0, str_int_buffer_len);

					if (to_slash_2 != 0)
					{
						memcpy_s(str_int_buffer, str_int_buffer_len, data + data_pos + to_slash_1 + 1, to_slash_2);
						uti::i32 abs_tex_idx = atoi(str_int_buffer);
						assert(abs_tex_idx >= max_last_tex_idx && abs_tex_idx > 0);
						cur_face->tex = abs_tex_idx - max_last_tex_idx;
					}

					memset(str_int_buffer, 0, str_int_buffer_len);

					memcpy_s(str_int_buffer, str_int_buffer_len, data + data_pos + to_slash_1 + 1 + to_slash_2 + 1, to_end_face);
					uti::i32 abs_nrm_idx = atoi(str_int_buffer);
					assert(abs_nrm_idx >= max_last_norm_idx && abs_nrm_idx > 0);
					cur_face->nrm = abs_nrm_idx - max_last_norm_idx;

					data_pos += to_slash_1 + 1 + to_slash_2 + 1 + to_end_face;
				}

				if (cur_obj->smooth)
				{
					num_vertex_elements = (num_positions > 0) + (num_texcoords > 0) + (num_normals > 0);
					cur_obj->num_vertex_floats = num_positions * num_vertex_elements * num_vertex_element_vals;
					cur_obj->num_vertices = num_positions;
					cur_obj->vertices = new float[cur_obj->num_vertex_floats];
					const uti::u32 size_vertices = cur_obj->num_vertex_floats * sizeof(float);

					cur_obj->indices = new uti::u32[num_faces];
					cur_obj->num_indices = num_faces;
					int cur_ver_idx = 0;
					for (uti::u32 j = 0; j < num_faces; ++j)
					{
						auto cur_face = faces[j];

						float* cur_pos = positions + cur_face.pos * num_vertex_element_vals;
						float* cur_tex = texcoords + cur_face.tex * num_vertex_element_vals;
						float* cur_nrm = normals + cur_face.nrm * num_vertex_element_vals;

						// TODO: [DanJ] just have an array of bools?
						bool already_added = false;
						for (uti::u32 k = 0; k < j; ++k)
						{
							if (cur_obj->indices[k] == cur_face.pos)
							{
								uti::u32 offsetToNorm = num_vertex_element_vals;
								if (num_texcoords != 0)
									offsetToNorm += num_vertex_element_vals;
								uti::u32 offset = cur_face.pos * num_vertex_element_vals * num_vertex_elements + num_vertex_element_vals;

								uti::float4 nrm = uti::make_float4(cur_obj->vertices[offset] + cur_nrm[0],
									cur_obj->vertices[offset + 1] + cur_nrm[1],
									cur_obj->vertices[offset + 2] + cur_nrm[2]);

								nrm = uti::norm(nrm);
								cur_obj->vertices[offset] = uti::get_x(nrm);
								cur_obj->vertices[offset + 1] = uti::get_y(nrm);
								cur_obj->vertices[offset + 2] = uti::get_z(nrm);
								already_added = true;
								break;
							}
						}
						cur_obj->indices[j] = cur_face.pos;
						cur_ver_idx = cur_face.pos;

						if (!already_added)
						{
							uti::u32 offset = cur_ver_idx * num_vertex_element_vals * num_vertex_elements;
							memcpy_s(cur_obj->vertices + offset, size_vertices - (cur_ver_idx + offset),
								cur_pos, num_vertex_element_vals * sizeof(float));
							offset += num_vertex_element_vals;
							memcpy_s(cur_obj->vertices + offset, size_vertices - offset,
								cur_nrm, num_vertex_element_vals * sizeof(float));
							if (num_texcoords != 0)
							{
								offset += num_vertex_element_vals;
								memcpy_s(cur_obj->vertices + cur_ver_idx + num_vertex_element_vals, size_vertices,
									cur_tex, num_vertex_element_vals * sizeof(float));
							}
							++cur_ver_idx;
						}
					}
				}
				else
				{
					uti::u32 final_num_verts = num_faces;

					num_vertex_elements = (final_num_verts > 0) + (num_texcoords > 0) + (num_normals > 0);
					cur_obj->num_vertex_floats = final_num_verts * num_vertex_elements * num_vertex_element_vals;
					cur_obj->num_vertices = final_num_verts;
					cur_obj->vertices = new float[cur_obj->num_vertex_floats];
					const uti::u32 size_vertices = cur_obj->num_vertex_floats * sizeof(float);

					cur_obj->indices = new uti::u32[num_faces];
					cur_obj->num_indices = num_faces;
					uti::u32 cur_ver_idx = 0;
					for (uti::u32 j = 0; j < num_faces; ++j)
					{
						face& cur_face = faces[j];
						float* cur_pos = positions + cur_face.pos * num_vertex_element_vals;
						assert(cur_face.pos < num_positions);
						float* cur_tex = texcoords + cur_face.tex * num_vertex_element_vals;
						assert(num_texcoords == 0 || cur_face.tex < num_texcoords);
						float* cur_nrm = normals + cur_face.nrm * num_vertex_element_vals;
						assert(cur_face.nrm < num_normals);

						cur_obj->indices[j] = cur_ver_idx;
						uti::u32 offset = cur_ver_idx * num_vertex_element_vals * num_vertex_elements;
						memcpy_s(cur_obj->vertices + offset, size_vertices - offset,
							cur_pos, num_vertex_element_vals * sizeof(float));
						if (num_normals != 0)
						{
							offset += num_vertex_element_vals;
							memcpy_s(cur_obj->vertices + offset, size_vertices - offset,
								cur_nrm, num_vertex_element_vals * sizeof(float));
						}
						if (num_texcoords != 0)
						{
							offset += num_vertex_element_vals;
							memcpy_s(cur_obj->vertices + offset, size_vertices - offset,
								cur_tex, num_vertex_element_vals * sizeof(float));
						}
						++cur_ver_idx;
					}
				}
			}

			delete[] positions;
			delete[] normals;
			delete[] texcoords;

			positions_offset = num_positions;

			doc->objects[i].has_texcoords = num_texcoords > 0;

			max_last_pos_idx  += num_positions;
			max_last_tex_idx  += num_texcoords;
			max_last_norm_idx += num_normals;

			num_positions = 0;
			num_normals = 0;
			num_texcoords = 0;
		}

		return true;
	}

	void free_obj_buffers(document* doc)
	{
		for (uti::u32 i = 0; i < doc->num_objects; ++i)
		{
			delete [] doc->objects[i].vertices;
			doc->objects[i].vertices = nullptr;
			doc->objects[i].num_vertices = 0;
			delete [] doc->objects[i].indices;
			doc->objects[i].indices = nullptr;
			doc->objects[i].num_indices = 0;
		}

		delete [] doc->materials;
		doc->materials = nullptr;
		doc->num_materials = 0;

		delete [] doc->objects;
		doc->objects = nullptr;
		doc->num_objects = 0;
	}
}
