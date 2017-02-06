#pragma once

#define MAX_OBJ_MDL_NAME 256
#define MAX_OBJ_MTL_INC 256
#define OBJ_COMMENT '#'

namespace obj
{
	struct face
	{
		uti::u32  pos, tex, nrm;
	};

	struct object
	{
		char			name[MAX_OBJ_MDL_NAME];

		float*		vertices;
		uti::u32	num_vertices;
		uti::u32    num_vertex_floats;
		bool		has_texcoords;

		uti::u32*	indices;
		uti::u32	num_indices;

		char		material_name[MAX_OBJ_MDL_NAME];
		bool		smooth;
	};

	struct document
	{
		char*		material_libs[MAX_OBJ_MTL_INC];
		uti::u32	num_material_libs;

		object*		objects;
		uti::u32	num_objects;
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
	};

	#define OBJ_LINE_RETURN_IF_TYPE(line, type, ret) if(strncmp(line, type, strlen(type)) == 0) return ret
	#define OBJ_LINE_COMMENT	"#"
	#define OBJ_LINE_OBJECT		"o "
	#define OBJ_LINE_VERTEX		"v "
	#define OBJ_LINE_NORMAL		"vn "
	#define OBJ_LINE_TEXCOORD	"vt "
	#define OBJ_LINE_SMOOTH		"s "
	#define OBJ_LINE_FACE		"f "

	line_type determine_line_type(char* line)
	{
		OBJ_LINE_RETURN_IF_TYPE(line, OBJ_LINE_COMMENT,		line_type_comment);
		OBJ_LINE_RETURN_IF_TYPE(line, OBJ_LINE_OBJECT,		line_type_object);
		OBJ_LINE_RETURN_IF_TYPE(line, OBJ_LINE_VERTEX,		line_type_vertex);
		OBJ_LINE_RETURN_IF_TYPE(line, OBJ_LINE_NORMAL,		line_type_normal);
		OBJ_LINE_RETURN_IF_TYPE(line, OBJ_LINE_TEXCOORD,	line_type_texcoord);
		OBJ_LINE_RETURN_IF_TYPE(line, OBJ_LINE_SMOOTH,		line_type_smooth);
		OBJ_LINE_RETURN_IF_TYPE(line, OBJ_LINE_FACE,		line_type_face);

		return line_type_none;
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
		while (data_pos < len_data)
		{
			// TODO: Skip space
			size_t line_end_off = str::find_char(data + data_pos, '\n', len_data - data_pos);
			if (line_end_off > len_data || data_pos + line_end_off > len_data)
			{
				data_pos = u32_max;
				break;
			}
			// Go past the \n
			line_end_off += 1;
			assert(line_end_off >= 2);
			cur_line_type = determine_line_type(data + data_pos);
			switch (cur_line_type)
			{
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
				break;
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
			}
			break;
			case line_type_comment:
			case line_type_none:
			default:
				data_pos += line_end_off;
				break;
			}
		}

		doc->objects = new object();
		doc->num_objects = 1;

		object* cur_obj = doc->objects;

		cur_obj->smooth = smooth;

		const int num_primative_verts = 3; // triangles only!
		int num_vertex_elements = 3; // pos nrm tex
		const int num_vertex_element_vals = 3; // x y z/u v w

		positions = new float[num_positions*num_vertex_element_vals];
		texcoords = new float[num_texcoords*num_vertex_element_vals];
		normals = new float[num_normals*num_vertex_element_vals];

		data_pos = vertices_start;
		for (uti::u32 i = 0; i < num_positions; ++i)
		{
			const int float_buffer_len = 32;
			char float_buffer[float_buffer_len] = {};
			float* cur_vert = positions + i * 3;
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
		for (uti::u32 i = 0; i < num_normals; ++i)
		{
			const int float_buffer_len = 32;
			char float_buffer[float_buffer_len] = {};
			float* cur_norm = normals + i * 3;
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
		for (uti::u32 i = 0; i < num_texcoords; ++i)
		{
			const int float_buffer_len = 32;
			char float_buffer[float_buffer_len] = {};
			float* cur_tex = texcoords + i * 3;
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

		faces = new face[num_faces*num_primative_verts];
		num_faces = num_faces*num_primative_verts;

		data_pos = faces_start;
		for (uti::u32  i = 0; i < num_faces; ++i)
		{
			const int str_int_buffer_len = 32;
			char str_int_buffer[str_int_buffer_len] = {};
			face* cur_face = faces + i;
			size_t to_int = str::strOffToNextFloat(data + data_pos);
			size_t to_slash_1 = str::find_char(data + data_pos, '/', len_data - data_pos);
			assert(to_slash_1 != UTI_FIND_CHAR_NOT_FOUND);
			if (to_slash_1 == UTI_FIND_CHAR_NOT_FOUND)
			{
				// 1st slash is missing
				// TODO: [DanJ] Report syntax is wrong / my shitty algorithm can't understand it

				delete[] positions;
				delete[] normals;
				delete[] texcoords;

				return false;
			}

			if (to_slash_1 < to_int)
			{
				// Position is missing
				// TODO: [DanJ] Report syntax is wrong / my shitty algorithm can't understand it

				delete[] positions;
				delete[] normals;
				delete[] texcoords;

				return false;
			}

			size_t to_slash_2 = str::find_char(data + data_pos + to_slash_1 + 1, '/', len_data - (data_pos + to_slash_1 + 1));
			assert(to_slash_2 != UTI_FIND_CHAR_NOT_FOUND);
			if (to_slash_2 == UTI_FIND_CHAR_NOT_FOUND)
			{
				// 2nd slash is missing
				// TODO: [DanJ] Report syntax is wrong / my shitty algorithm can't understand it

				delete[] positions;
				delete[] normals;
				delete[] texcoords;

				return false;
			}

			size_t to_space = str::find_char(data + data_pos + to_slash_1 + 1 + to_slash_2, ' ', len_data - (data_pos + to_slash_1 + 1 + to_slash_2));

			size_t to_line_end = str::find_char(data + data_pos + to_slash_1 + 1 + to_slash_2, '\n', len_data - (data_pos + to_slash_1 + 1 + to_slash_2));

			uti::u64 to_end_face = to_space < to_line_end ? to_space : to_line_end;

			memcpy_s(str_int_buffer, str_int_buffer_len, data + data_pos + to_int, to_slash_1 - to_int);
			cur_face->pos = atoi(str_int_buffer) - 1;

			if (to_slash_2 != 0)
			{
				memcpy_s(str_int_buffer, str_int_buffer_len, data + data_pos + to_slash_1 + 1, to_slash_2);
				cur_face->tex = atoi(str_int_buffer) - 1;
			}

			memcpy_s(str_int_buffer, str_int_buffer_len, data + data_pos + to_slash_1 + 1 + to_slash_2 + 1, to_end_face);
			cur_face->nrm = atoi(str_int_buffer) - 1;

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
			for (uti::u32  i = 0; i < num_faces; ++i)
			{
				auto cur_face = faces[i];
				float* cur_pos = positions + cur_face.pos * num_vertex_element_vals;
				float* cur_tex = texcoords + cur_face.tex * num_vertex_element_vals;
				float* cur_nrm = normals + cur_face.nrm * num_vertex_element_vals;

				// TODO: [DanJ] just have an array of bools?
				bool already_added = false;
				for (uti::u32 k = 0; k < i; ++k)
				{
					if (cur_obj->indices[k] == cur_face.pos)
					{
						uti::u32 offsetToNorm = num_vertex_element_vals;
						if(num_texcoords != 0)
							offsetToNorm += num_vertex_element_vals;
						uti::u32 offset = cur_face.pos * num_vertex_element_vals * num_vertex_elements + num_vertex_element_vals;

						uti::float4 nrm = uti::make( cur_obj->vertices[offset]	 + cur_nrm[0],
													 cur_obj->vertices[offset+1] + cur_nrm[1],
													 cur_obj->vertices[offset+2] + cur_nrm[2] );

						nrm = uti::norm(nrm);
						cur_obj->vertices[offset]	= uti::get_x(nrm);
						cur_obj->vertices[offset+1] = uti::get_y(nrm);
						cur_obj->vertices[offset+2] = uti::get_z(nrm);
						already_added = true;
						break;
					}
				}
				cur_obj->indices[i] = cur_face.pos;
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
			int cur_ver_idx = 0;
			for (uti::u32  i = 0; i < num_faces; ++i)
			{
				auto cur_face = faces[i];
				float* cur_pos = positions + cur_face.pos * num_vertex_element_vals;
				float* cur_tex = texcoords + cur_face.tex * num_vertex_element_vals;
				float* cur_nrm = normals + cur_face.nrm * num_vertex_element_vals;

				cur_obj->indices[i] = cur_ver_idx;
				uti::u32 offset = cur_ver_idx * num_vertex_element_vals * num_vertex_elements;
				memcpy_s(cur_obj->vertices + offset, size_vertices - (cur_ver_idx + offset),
					cur_pos, num_vertex_element_vals * sizeof(float));
				offset += num_vertex_element_vals;
				memcpy_s(cur_obj->vertices + offset, size_vertices - offset,
					cur_nrm, num_vertex_element_vals * sizeof(float));
				if (num_texcoords != 0)
				{
					offset += num_vertex_element_vals;
					memcpy_s(cur_obj->vertices + offset, size_vertices - offset,
						cur_tex, num_vertex_element_vals * sizeof(float));
				}
				++cur_ver_idx;
			}
		}

		delete[] positions;
		delete[] normals;
		delete[] texcoords;

		doc->objects[0].has_texcoords = num_texcoords > 0;

		return true;
	}

	void free_obj_buffers(document* doc)
	{
		for (uti::u32 i = 0; i < doc->num_objects; ++i)
		{
			delete [] doc->objects[i].vertices;
			doc->objects[i].num_vertices = 0;
			delete [] doc->objects[i].indices;
			doc->objects[i].num_indices = 0;
		}

		delete [] doc->objects;
		doc->num_objects = 0;

		doc->objects = nullptr;
	}
}
