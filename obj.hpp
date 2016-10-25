#pragma once

#define MAX_OBJ_MDL_NAME 256
#define MAX_OBJ_MTL_INC 256
#define OBJ_COMMENT '#'

namespace obj
{
	struct face
	{
		int pos, tex, nrm;
	};

	struct object
	{
		char			name[MAX_OBJ_MDL_NAME];

		float*		vertices;
		uti::u32	num_vertices;
		uti::u32    num_vertex_floats;
		uti::u32	position_offset;
		uti::u32	normal_offset;
		uti::u32	texcoord_offset;

		uti::u32*	indices;
		uti::u32	num_indices;

		//float*		normals;
		//uti::u32	num_normals;

		//float*		texcoords;
		//uti::u32	num_texcoords;

		char		material_name[MAX_OBJ_MDL_NAME];
		bool		smooth;

		//face*		faces;
		//uti::u32	num_faces;
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

	line_type determine_line_type(char* line, uti::i32 line_len)
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

	bool load_obj(char* data, uti::u32 len_data, document* doc)
	{
		//uti::u32 vertices_start = 0;
		//uti::u32 num_vertices = 0;
		//uti::u32 normals_start = 0;
		//uti::u32 num_normals = 0;
		//uti::u32 faces_start = 0;
		//uti::u32 num_faces = 0;
		//uti::u32 data_pos = 0;
		//bool	 smooth = true;
		//line_type cur_line_type = line_type_none;
		//while (data_pos < len_data)
		//{
		//	// TODO: Skip space
		//	size_t line_end_off = str::find_char(data + data_pos, '\n', len_data- data_pos);
		//	if (line_end_off > len_data || data_pos + line_end_off > len_data)
		//	{
		//		data_pos = u32_max;
		//		break;
		//	}
		//	// Go past the \n
		//	line_end_off += 1;
		//	cur_line_type = determine_line_type(data + data_pos, line_end_off);
		//	switch (cur_line_type)
		//	{
		//	case line_type_vertex:
		//		if(vertices_start == 0)
		//			vertices_start = data_pos;
		//		++num_vertices;
		//		data_pos += line_end_off;
		//		break;
		//	case line_type_normal:
		//		if(normals_start == 0)
		//			normals_start = data_pos;
		//		++num_normals;
		//		data_pos += line_end_off;
		//		break;
		//	case line_type_face:
		//		if(faces_start == 0)
		//			faces_start = data_pos;
		//		++num_faces;
		//		data_pos += line_end_off;
		//		break;
		//	case line_type_smooth:
		//	{
		//		data_pos += 1;
		//		line_end_off -= 1;
		//		size_t start_smooth_off = str::find_not_char(data + data_pos, ' ', line_end_off);
		//		if (line_end_off - start_smooth_off >= 2)
		//		{
		//			if (strncmp("off", data + data_pos + start_smooth_off, 3) == 0)
		//				smooth = false;
		//		}
		//		data_pos += line_end_off;
		//	}
		//		break;
		//	case line_type_comment:
		//	case line_type_none:
		//	default:
		//		data_pos += line_end_off;
		//		break;
		//	}
		//}

		//doc->objects = new object();
		//doc->num_objects = 1;

		//object* cur_obj = doc->objects;

		//cur_obj->smooth = smooth;

		//cur_obj->vertices = new float[num_vertices * 3];
		//cur_obj->num_vertices = num_vertices;

		//data_pos = vertices_start;
		//for (int i = 0; i < num_vertices; ++i)
		//{
		//	const int float_buffer_len = 32;
		//	char float_buffer[float_buffer_len] = {};
		//	float* cur_vert = cur_obj->vertices + i*3;
		//	size_t off_to_float = str::strOffToNextFloat(data + data_pos);
		//	size_t off_to_end_float = str::strOffToEndFloat(data + data_pos + off_to_float);
		//	memcpy_s(float_buffer, float_buffer_len, data + data_pos + off_to_float, off_to_end_float);
		//	cur_vert[0] = atof(float_buffer);

		//	data_pos += off_to_float + off_to_end_float;
		//	memset(float_buffer, 0, float_buffer_len);

		//	off_to_float = str::strOffToNextFloat(data + data_pos);
		//	off_to_end_float = str::strOffToEndFloat(data + data_pos + off_to_float);
		//	memcpy_s(float_buffer, float_buffer_len, data + data_pos + off_to_float, off_to_end_float);
		//	cur_vert[1] = atof(float_buffer);

		//	data_pos += off_to_float + off_to_end_float;

		//	off_to_float = str::strOffToNextFloat(data + data_pos);
		//	off_to_end_float = str::strOffToEndFloat(data + data_pos + off_to_float);
		//	memcpy_s(float_buffer, float_buffer_len, data + data_pos + off_to_float, off_to_end_float);
		//	cur_vert[2] = atof(float_buffer);

		//	data_pos += off_to_float + off_to_end_float;
		//}

		//cur_obj->normals = new float[num_normals];
		//cur_obj->num_normals = num_normals;

		//data_pos = normals_start;
		//for (int i = 0; i < num_normals; ++i)
		//{
		//	const int float_buffer_len = 32;
		//	char float_buffer[float_buffer_len] = {};
		//	float* cur_norm = cur_obj->normals + i*3;
		//	size_t off_to_float = str::strOffToNextFloat(data + data_pos);
		//	size_t off_to_end_float = str::strOffToEndFloat(data + data_pos + off_to_float);
		//	memcpy_s(float_buffer, float_buffer_len, data + data_pos + off_to_float, off_to_end_float);
		//	cur_norm[0] = atof(float_buffer);

		//	data_pos += off_to_float + off_to_end_float;
		//	memset(float_buffer, 0, float_buffer_len);

		//	off_to_float = str::strOffToNextFloat(data + data_pos);
		//	off_to_end_float = str::strOffToEndFloat(data + data_pos + off_to_float);
		//	memcpy_s(float_buffer, float_buffer_len, data + data_pos + off_to_float, off_to_end_float);
		//	cur_norm[1] = atof(float_buffer);

		//	data_pos += off_to_float + off_to_end_float;

		//	off_to_float = str::strOffToNextFloat(data + data_pos);
		//	off_to_end_float = str::strOffToEndFloat(data + data_pos + off_to_float);
		//	memcpy_s(float_buffer, float_buffer_len, data + data_pos + off_to_float, off_to_end_float);
		//	cur_norm[2] = atof(float_buffer);

		//	data_pos += off_to_float + off_to_end_float;
		//}

		//const int num_primative_verts = 3; // triangles only!
		//cur_obj->faces = new face[num_faces*num_primative_verts];
		//cur_obj->num_faces = num_faces;

		//data_pos = faces_start;
		//for (int i = 0; i < num_faces*num_primative_verts; ++i)
		//{
		//	const int str_int_buffer_len = 32;
		//	char str_int_buffer[str_int_buffer_len] = {};
		//	face* cur_face = cur_obj->faces + i;
		//	size_t off_to_int	  = str::strOffToNextFloat(data + data_pos);
		//	size_t off_to_end_int = str::find_char(data + data_pos + off_to_int, '/', len_data - (data_pos + off_to_int));
		//	memcpy_s(str_int_buffer, str_int_buffer_len, data + data_pos + off_to_int, off_to_end_int);
		//	cur_face->pos = atoi(str_int_buffer);

		//	data_pos += off_to_int + off_to_end_int + 1;
		//	memset(str_int_buffer, 0, str_int_buffer_len);

		//	off_to_end_int = str::find_char(data + data_pos, '/', len_data - data_pos);
		//	if (off_to_end_int != 0)
		//	{
		//		memcpy_s(str_int_buffer, str_int_buffer_len, data + data_pos, off_to_end_int);
		//		cur_face->tex = atoi(str_int_buffer);
		//		data_pos += off_to_int + off_to_end_int;
		//	}
		//	else
		//	{
		//		cur_face->tex = 0;
		//	}

		//	off_to_int = str::strOffToNextFloat(data + data_pos);
		//	off_to_end_int = str::strOffToEndFloat(data + data_pos + off_to_int);
		//	memcpy_s(str_int_buffer, str_int_buffer_len, data + data_pos + off_to_int, off_to_end_int);
		//	cur_face->nrm = atoi(str_int_buffer);

		//	data_pos += off_to_int + off_to_end_int;
		//}

		float*		positions = nullptr;
		uti::u32    num_positions = 0;

		float*		normals = nullptr;
		uti::u32	num_normals = 0;

		float*		texcoords = nullptr;
		uti::u32	num_texcoords = 0;

		face*		faces = nullptr;
		uti::u32	num_faces = 0;

		uti::u32 vertices_start = 0;
		uti::u32 normals_start = 0;
		uti::u32 faces_start = 0;
		uti::u32 data_pos = 0;
		bool	 smooth = true;
		line_type cur_line_type = line_type_none;
		while (data_pos < len_data)
		{
			// TODO: Skip space
			size_t line_end_off = str::find_char(data + data_pos, '\n', len_data- data_pos);
			if (line_end_off > len_data || data_pos + line_end_off > len_data)
			{
				data_pos = u32_max;
				break;
			}
			// Go past the \n
			line_end_off += 1;
			cur_line_type = determine_line_type(data + data_pos, line_end_off);
			switch (cur_line_type)
			{
			case line_type_vertex:
				if(vertices_start == 0)
					vertices_start = data_pos;
				++num_positions;
				data_pos += line_end_off;
				break;
			case line_type_normal:
				if(normals_start == 0)
					normals_start = data_pos;
				++num_normals;
				data_pos += line_end_off;
				break;
			case line_type_face:
				if(faces_start == 0)
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
		for (int i = 0; i < num_positions; ++i)
		{
			const int float_buffer_len = 32;
			char float_buffer[float_buffer_len] = {};
			float* cur_vert = positions + i*3;
			size_t off_to_float = str::strOffToNextFloat(data + data_pos);
			size_t off_to_end_float = str::strOffToEndFloat(data + data_pos + off_to_float);
			memcpy_s(float_buffer, float_buffer_len, data + data_pos + off_to_float, off_to_end_float);
			cur_vert[0] = atof(float_buffer);

			data_pos += off_to_float + off_to_end_float;
			memset(float_buffer, 0, float_buffer_len);

			off_to_float = str::strOffToNextFloat(data + data_pos);
			off_to_end_float = str::strOffToEndFloat(data + data_pos + off_to_float);
			memcpy_s(float_buffer, float_buffer_len, data + data_pos + off_to_float, off_to_end_float);
			cur_vert[1] = atof(float_buffer);

			data_pos += off_to_float + off_to_end_float;

			off_to_float = str::strOffToNextFloat(data + data_pos);
			off_to_end_float = str::strOffToEndFloat(data + data_pos + off_to_float);
			memcpy_s(float_buffer, float_buffer_len, data + data_pos + off_to_float, off_to_end_float);
			cur_vert[2] = atof(float_buffer);

			data_pos += off_to_float + off_to_end_float;
		}

		data_pos = normals_start;
		for (int i = 0; i < num_normals; ++i)
		{
			const int float_buffer_len = 32;
			char float_buffer[float_buffer_len] = {};
			float* cur_norm = normals + i*3;
			size_t off_to_float = str::strOffToNextFloat(data + data_pos);
			size_t off_to_end_float = str::strOffToEndFloat(data + data_pos + off_to_float);
			memcpy_s(float_buffer, float_buffer_len, data + data_pos + off_to_float, off_to_end_float);
			cur_norm[0] = atof(float_buffer);

			data_pos += off_to_float + off_to_end_float;
			memset(float_buffer, 0, float_buffer_len);

			off_to_float = str::strOffToNextFloat(data + data_pos);
			off_to_end_float = str::strOffToEndFloat(data + data_pos + off_to_float);
			memcpy_s(float_buffer, float_buffer_len, data + data_pos + off_to_float, off_to_end_float);
			cur_norm[1] = atof(float_buffer);

			data_pos += off_to_float + off_to_end_float;

			off_to_float = str::strOffToNextFloat(data + data_pos);
			off_to_end_float = str::strOffToEndFloat(data + data_pos + off_to_float);
			memcpy_s(float_buffer, float_buffer_len, data + data_pos + off_to_float, off_to_end_float);
			cur_norm[2] = atof(float_buffer);

			data_pos += off_to_float + off_to_end_float;
		}

		faces = new face[num_faces*num_primative_verts];
		num_faces = num_faces*num_primative_verts;

		data_pos = faces_start;
		for (int i = 0; i < num_faces; ++i)
		{
			const int str_int_buffer_len = 32;
			char str_int_buffer[str_int_buffer_len] = {};
			face* cur_face = faces + i;
			size_t off_to_int	  = str::strOffToNextFloat(data + data_pos);
			size_t off_to_end_int = str::find_char(data + data_pos + off_to_int, '/', len_data - (data_pos + off_to_int));
			memcpy_s(str_int_buffer, str_int_buffer_len, data + data_pos + off_to_int, off_to_end_int);
			cur_face->pos = atoi(str_int_buffer) - 1;

			data_pos += off_to_int + off_to_end_int + 1;
			memset(str_int_buffer, 0, str_int_buffer_len);

			off_to_end_int = str::find_char(data + data_pos, '/', len_data - data_pos);
			if (off_to_end_int != 0)
			{
				memcpy_s(str_int_buffer, str_int_buffer_len, data + data_pos, off_to_end_int);
				cur_face->tex = atoi(str_int_buffer) - 1;
				data_pos += off_to_int + off_to_end_int;
			}
			else
			{
				cur_face->tex = 0;
			}

			off_to_int = str::strOffToNextFloat(data + data_pos);
			off_to_end_int = str::strOffToEndFloat(data + data_pos + off_to_int);
			memcpy_s(str_int_buffer, str_int_buffer_len, data + data_pos + off_to_int, off_to_end_int);
			cur_face->nrm = atoi(str_int_buffer) - 1;

			data_pos += off_to_int + off_to_end_int;
		}


	    num_vertex_elements = (num_positions > 0) + (num_texcoords > 0) + (num_normals > 0);
		cur_obj->num_vertex_floats = num_positions * num_vertex_elements * num_vertex_element_vals;
		cur_obj->num_vertices = num_positions;
		cur_obj->vertices = new float[cur_obj->num_vertex_floats];
		const uti::u32 size_vertices = cur_obj->num_vertex_floats * sizeof(float);

		cur_obj->indices = new uti::u32[num_faces];
		cur_obj->num_indices = num_faces;

		// TODO: deal with missing normals and texcoords

		int cur_ver_idx = 0;
		for (int i = 0; i < num_faces; ++i)
		{
			auto cur_face = faces[i];
			float* cur_pos = positions  + cur_face.pos * num_vertex_element_vals;
			float* cur_tex = texcoords  + cur_face.tex * num_vertex_element_vals;
			float* cur_nrm = normals	+ cur_face.nrm * num_vertex_element_vals;

			// TODO: [DanJ] slow as balls just have an array of bools
			bool already_added = false;
			for (int k = 0; k < i; ++k)
			{
				if (cur_obj->indices[k] == cur_face.pos)
				{
					already_added = true;
					break;
				}
			}
			cur_obj->indices[i] = cur_face.pos;

			if (!already_added)
			{
				uti::u32 offset = cur_ver_idx * num_vertex_element_vals * num_vertex_elements;
				memcpy_s(cur_obj->vertices + offset, size_vertices - (cur_ver_idx + offset),
					cur_pos, num_vertex_element_vals * sizeof(float));
				offset += num_vertex_element_vals;
				if (num_texcoords != 0)
				{
					memcpy_s(cur_obj->vertices + cur_ver_idx + num_vertex_element_vals, size_vertices,
						cur_tex, num_vertex_element_vals * sizeof(float));
					offset += num_vertex_element_vals;
				}
				memcpy_s(cur_obj->vertices + offset, size_vertices-offset,
					cur_nrm, num_vertex_element_vals * sizeof(float));
				++cur_ver_idx;
			}
		}

		delete[] positions;
		delete[] normals;
		delete[] texcoords;

		return true;
	}

	void free_obj_buffers(document* doc)
	{
		for (int i = 0; i < doc->num_objects; ++i)
		{
			delete [] doc->objects[i].vertices;
			doc->objects[i].num_vertices = 0;
			delete [] doc->objects[i].indices;
			doc->objects[i].num_indices = 0;
			//delete [] doc->objects[i].normals;
			//delete [] doc->objects[i].texcoords;
		}

		delete [] doc->objects;
		doc->num_objects = 0;

		doc->objects = nullptr;
	}
}
