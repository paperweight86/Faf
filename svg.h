#pragma once

#include "types.h"
using namespace uti;

#include "str.h"
using namespace str;

#include <map>

#include "rapidxml.hpp"
using namespace rapidxml;

#include <errno.h>

namespace svg
{
	enum svg_path_opname : char
	{
		// Move the pen
		svg_op_move_abs = 'M',
		svg_op_move_rel = 'm',

		// End of ops
		svg_op_close = 'Z',
		svg_op_close_alt = 'z',

		// Line
		svg_op_line_abs = 'L',
		svg_op_line_rel = 'l',

		// Horizontal line
		svg_op_hline_abs = 'H',
		svg_op_hline_rel = 'h',

		// Vertical line
		svg_op_vline_abs = 'V',
		svg_op_vline_rel = 'v',

		// Curves

		// Cubic Bezier
		svg_op_cbezier_abs = 'C',
		svg_op_cbezier_rel = 'c',

		// Smooth Cubic bezier
		svg_op_scbezier_abs = 'S',
		svg_op_scbezier_rel = 's',

		// Quadratic Bezier
		svg_op_qbezier_abs = 'Q',
		svg_op_qbezier_rel = 'q',

		// Smooth Quadratic Bezier
		svg_op_sqbezier_abs = 'T',
		svg_op_sqbezier_rel = 't',

		// A, a
		svg_op_elarc_abs = 'A',
		svg_op_elarc_rel = 'a',
	};

	const char opChars[] = { 'M','m','Z','z','L','l','H','h','V','v','C','c','S','s','Q','q','T','t','A','a' };

	struct svg_path_op
	{
		svg_path_opname operation;
		float* values;
		u64	   num_values;
		float2 pos;
	};

	struct svg_path
	{
		svg_path_op* ops;
		size_t       num_ops;
	};

	struct svg_document
	{
		float width;
		float height;

		float viewport[4]; // x, y, w, h

		svg_path* paths;
		size_t    num_paths;
	};

	bool load_svg_file( tstr filepath, svg_document** doc_out)
	{
		assert((*doc_out) == nullptr);
		// TODO: arrrrrrr don't alloc here
		(*doc_out) = new svg_document();

		auto doc = (*doc_out);

		FILE* svgFile = NULL;
		errno_t err = _tfopen_s(&svgFile, filepath, _T("rt"));
		if (err != 0)
		{
			//TODO: Not thread safe should use strerrorlen_s & strerror_s
			char* errStr = std::strerror(err);
			printf("[Error] Unable to load \"%s\": %s (%d)", filepath, errStr, err);
			return false;
		}
		fseek(svgFile, 0, SEEK_END);
		auto fSize = _ftelli64(svgFile);
		fseek(svgFile, 0, SEEK_SET);
		char* svgText = new char[fSize + 1];
		memset(svgText, 0, fSize + 1);
		size_t read = fread_s(svgText, fSize, fSize, 1, svgFile);
		fclose(svgFile);
		xml_document<> xmlDoc;    // character type defaults to char
		xmlDoc.parse<0>(svgText);    // 0 means default parse flags

		auto root = xmlDoc.first_node("svg");

		auto widthAtt  = root->first_attribute("width");

		if (widthAtt != nullptr)
		{
			auto widthStr = widthAtt->value();
			doc->width = atof(widthStr);
			doc->viewport[2] = doc->width;
		}

		auto heightAtt = root->first_attribute("height");

		if (heightAtt != nullptr)
		{
			auto heightStr = heightAtt->value();
			doc->height = atof(heightStr);
			doc->viewport[3] = doc->height;
		}

		// Read the view port
		// TODO: sometimes absent use <svg width="..." height="..." instead 
		auto viewBoxAtt = root->first_attribute("viewBox");

		if (viewBoxAtt != nullptr)
		{
			auto viewBoxStr = viewBoxAtt->value();

			int dimIdx = 0;
			int lastIdx = 0;
			int i = 0;
			for (; i < strlen(viewBoxStr); ++i)
			{
				if (viewBoxStr[i] == ' ')
				{
					char tmp[256] = {};
					strncpy_s<256>(tmp, (const char*)(viewBoxStr + lastIdx), i - lastIdx);
					doc->viewport[dimIdx++] = atof(tmp);
					lastIdx = i + 1;
				}
			}

			char tmp[256] = {};
			strncpy_s<256>(tmp, (const char*)(viewBoxStr + lastIdx), i - lastIdx);
			doc->viewport[dimIdx++] = atof(tmp);
		}

		auto group = root->first_node("g");

		auto path = group->first_node("path");

		float2 pos = float2(0.0, 0.0);
		std::vector<svg_path_op> pathOpList;

		std::vector<svg_path> paths;

		while (path != nullptr)
		{
			auto curPath = svg_path();

			// TODO: we don't want to reset the position as it may be used by the next object - need a 'current pen position'
			//pos = float2(0.0, 0.0);
			auto pathId = path->first_attribute("id");

			auto pathData = path->first_attribute("d");

			char* data = pathData->value();

			auto pathStyle = path->first_attribute("style");
			if (pathStyle != nullptr)
			{
				char* style = pathStyle->value();

				// Parse the CSS-esque style "key:val;key:val..."
				std::string strStyle = std::string(style);
				size_t styleOff = 0;
				size_t stylePos = strStyle.find(';', styleOff);
				std::vector<std::string> styleParts;
				while (stylePos != std::string::npos && stylePos < strStyle.length())
				{
					std::string styleKeyVal = strStyle.substr(styleOff, stylePos - styleOff);
					size_t pos = styleKeyVal.find(':', 0);
					auto styleKey = styleKeyVal.substr(0, pos);
					auto styleVal = styleKeyVal.substr(pos + 1);

					styleParts.push_back(styleKey);
					styleParts.push_back(styleVal);

					styleOff = stylePos + 1;
					// Next
					stylePos = strStyle.find(';', styleOff);
				}
			}

			std::string strData = std::string(data);
			size_t dataPos = std::string::npos;
			size_t p = 0;
			while (p < ARRAYSIZE(opChars))
			{
				size_t a = strData.find(opChars[p++], 0);
				if (a < dataPos && a != std::string::npos)
					dataPos = a;
			}
			--p;

			while (dataPos != std::string::npos)
			{
				svg_path_opname op = (svg_path_opname)strData[dataPos];

				p = 0;
				size_t off = dataPos + 1;
				dataPos = std::string::npos;
				while (p < ARRAYSIZE(opChars))
				{
					size_t a = strData.find(opChars[p++], off);
					if (a < dataPos && a >= off)
						dataPos = a;
				}
				--p;

				// Read the data between them
				std::string strOpData = strData.substr(off, dataPos - off);
				off = 0;
				
				std::vector<float> values;
				if (strOpData.length() > 0)
				{
					const char* c = strOpData.c_str();

					while (off < strOpData.length())
					{
						off += strOffToNextFloat(c + off);

						if (off >= strOpData.length())
							break;

						size_t start = off;

						off += strOffToEndFloat(c + off);

						size_t end = off;

						auto a = strOpData.substr(start, end - start);

						values.push_back(atof(a.c_str()));
					}
				}

				// Create a list of operations
				int valIdx = 0;
				svg_path_op pathOp = {};
				pathOp.operation = op;
				pathOp.pos = pos;
				size_t numVals = values.size();
				switch (op)
				{
				case svg_op_scbezier_rel:
				case svg_op_scbezier_abs:
					numVals = values.size() - valIdx;
					if (numVals > 0)
					{
						pathOp.values = new float[numVals];
						memcpy_s(pathOp.values, numVals*sizeof(float), &values[valIdx], numVals*sizeof(float));
					}
					pathOp.num_values = numVals;
					pathOp.pos = pos;
					break;
				case svg_op_move_rel:
				case svg_op_move_abs:
					pathOp.pos = float2(values[0], values[1]);
					pathOpList.push_back(pathOp);
					if (op == svg_op_move_rel && values.size() > 2)
						pathOp.operation = svg_op_line_rel;
					if (op == svg_op_move_abs && values.size() > 2)
						pathOp.operation = svg_op_line_abs;
					if (values.size() == 2)
						continue;
					else
						valIdx = 2;
				default:
					numVals = values.size() - valIdx;
					if (numVals > 0)
					{
						pathOp.values = new float[numVals];
						memcpy_s(pathOp.values, numVals*sizeof(float), &values[valIdx], numVals*sizeof(float));
					}
					pathOp.num_values = numVals;
					pathOp.pos = pos;
					break;
				}

				pathOpList.push_back(pathOp);
			}

			// TODO: Dumb dumbety dumb
			curPath.ops = new svg_path_op[pathOpList.size()];
			memcpy(curPath.ops, &pathOpList[0], sizeof(svg_path_op) * pathOpList.size());
			curPath.num_ops = pathOpList.size();

			paths.push_back(curPath);

			pathOpList.clear();
			path = path->next_sibling("path");
		}
		doc->paths = new svg_path[paths.size()];
		memcpy(doc->paths, &paths[0], sizeof(svg_path) * paths.size());
		doc->num_paths = paths.size();

		return true;
	}
}
