// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "Json.h"
#include "Filesystem/FileSource.h"
#include "Filesystem/File.h"
#include "IO/InputBuffer.h"
#include "Container/ConfigValue.h"


namespace sb
{

	//-------------------------------------------------------------------------------

	namespace json_internal
	{
		void WriteTabs(int ilevel, stringstream& out);
		void WriteString(const char* str, stringstream& out, bool quotes = true);
	}

	//-------------------------------------------------------------------------------
	json::Writer::Writer() : _ilevel(0)
	{
	}
	json::Writer::~Writer()
	{
	}
	void json::Writer::Write(const ConfigValue& root, stringstream& out, bool format)
	{
		_format = format;
		WriteValue(root, out);
		out << "\n";
	}
	void json::Writer::WriteValue(const ConfigValue& node, stringstream& out)
	{
		switch (node.Type())
		{
		case ConfigValue::NULL_VALUE:
			out << "null";
			break;
		case ConfigValue::BOOL:
			out << node.AsBool();
			break;
		case ConfigValue::INTEGER:
			out << node.AsInt64();
			break;
		case ConfigValue::UINTEGER:
			out << node.AsUInt64();
			break;
		case ConfigValue::FLOAT:
			out << node.AsDouble();
			break;
		case ConfigValue::STRING:
			out << "\"";
			json_internal::WriteString(node.AsString(), out);
			out << "\"";
			break;
		case ConfigValue::ARRAY:
		{
			out << "[";

			_ilevel++;
			int size = node.Size();
			for (int i = 0; i < size; ++i)
			{
				if (i != 0)
					out << ",";

				if (_format)
				{
					out << "\n";
					json_internal::WriteTabs(_ilevel, out);
				}
				WriteValue(node[i], out);
			}
			if (_format)
				out << "\n";
			_ilevel--;
			if (_format)
				json_internal::WriteTabs(_ilevel, out);
			out << "]";
		}
			break;
		case ConfigValue::OBJECT:
		{
			out << "{";
			_ilevel++;
			ConfigValue::ConstIterator it, end;
			it = node.Begin(); end = node.End();
			for (; it != end; ++it)
			{
				if (it != node.Begin())
					out << ",";

				if (_format)
				{
					out << "\n";
					json_internal::WriteTabs(_ilevel, out);
				}
				out << "\"";
				json_internal::WriteString(it->first.c_str(), out);
				out << "\": ";
				WriteValue(it->second, out);
			}
			if (_format)
				out << "\n";
			_ilevel--;
			if (_format)
				json_internal::WriteTabs(_ilevel, out);
			out << "}";
		}
			break;
		};
	}

	//-------------------------------------------------------------------------------

} // namespace sb


