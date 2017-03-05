// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "SimplifiedJson.h"
#include "Filesystem/FileSource.h"
#include "Filesystem/File.h"
#include "IO/FileInputBuffer.h"
#include "Container/ConfigValue.h"


namespace sb
{

	//-------------------------------------------------------------------------------
	simplified_json::Reader::Reader()
	{
		_cur = _end = _begin = 0;
	}
	simplified_json::Reader::~Reader()
	{
	}
	//-------------------------------------------------------------------------------
	const string& simplified_json::Reader::GetErrorMessage()
	{
		return _error;
	}
	void simplified_json::Reader::Error(const char* msg)
	{
		int line, column;
		GetCurrentPosition(line, column);

		stringstream ss; ss << "(Line: " << line << ", Column: " << column << ") Error: " << msg;
		_error = ss.str();
	}
	void simplified_json::Reader::GetCurrentPosition(int& line, int& column)
	{
		line = 0; column = 0;

		const char* current = _begin;
		const char* line_start = _begin;
		while (current < _cur)
		{
			if (*current == '\r')
			{
				if (*(current + 1) == '\n')
					++current;

				line_start = current;
				++line;
			}
			else if (*current == '\n')
			{
				line_start = current;
				++line;
			}
			++current;
		}
		++line;
		column = (int)(_cur - line_start) - 1;
	}
	//-------------------------------------------------------------------------------
	bool simplified_json::Reader::Read(const char* doc, int64_t length, ConfigValue& root)
	{
		_cur = _begin = doc;
		_end = doc + length;

		SkipSpaces();
		if (*_cur == '{')
			return ParseObject(root);

		// Assume root is an object
		root.SetEmptyObject();

		string name;
		while (1)
		{
			SkipSpaces();
			if (_cur == _end)
				break;


			name = "";
			if (!ParseString(name))
			{
				Error("Failed to parse string");
				return false;
			}

			SkipSpaces();
			if (*_cur != '=' && *_cur != ':')
			{
				Error("Expected '=' or ':'");
				return false;
			}
			_cur++;

			ConfigValue& elem = root[name.c_str()];
			if (!ParseValue(elem))
			{
				return false; // Failed to parse value
			}

			SkipSpaces();

			char c = *_cur;
			if (c == ',') // Separator between elements (Optional)
			{
				_cur++;
				continue;
			}
		}

		return true;
	}
	bool simplified_json::Reader::ReadFile(FileSource* source, const char* file_name, ConfigValue& root)
	{
		// and then load any settings from settings file
		FileStreamPtr file = source->OpenFile(file_name, File::READ);
		if (!file.Get())
		{
			stringstream ss; ss << "Failed to open file '" << file_name << "'";
			_error = ss.str();
			return false;
		}

		bool res = true;

		FileInputBuffer buffer(file);
		if (buffer.Length() == 0)
		{
			// Empty file, just set root as empty object
			root.SetEmptyObject();
			res = true;
		}
		else
			res = Read((const char*)buffer.Ptr(), buffer.Length(), root);

		return res;
	}

	void simplified_json::Reader::SkipSpaces()
	{
		while (_cur != _end)
		{
			char c = *_cur;
			if (c == ' ' || c == '\t' || c == '\r' || c == '\n')
			{
				++_cur;
			}
			else
				break;
		}
	}

	bool simplified_json::Reader::ParseString(string& str)
	{
		// Typically a string is surrounded by quotes but we also support strings without quotes
		//	a string without quotes is considered to end at the first whitespace character (E.g. ' ' or '\t')

		// Count length first
		const char* str_end = _cur;
		bool quotes = (*_cur == '"'); // Keep track if this string is surrounded by quotes
		if (quotes)
			++str_end; // Skip starting "
		while (str_end != _end)
		{
			char c = *str_end++;
			if (c == '\\')
				str_end++; // Skip checking next character	
			else if ((quotes && (c == '"')) || (!quotes && (c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '=' || c == ':')))
				break;
		}
		if (quotes)
		{
			_cur += 1; // Skip starting "
		}
		str_end -= 1; // Skip trailing " or any trailing whitespace

		while (_cur != str_end)
		{
			char c = *_cur++;
			if (c == '\\')
			{
				char esc = *_cur++;
				switch (esc)
				{
				case '"':
					c = '"';
					break;
				case '/':
					c = '/';
					break;
				case '\\':
					c = '\\';
					break;
				case '\n':
					c = '\n';
					break;
				case '\r':
					c = '\r';
					break;
				case '\t':
					c = '\t';
					break;
				default:
					c = esc;
				};
			}

			str += c;
		}
		if (quotes)
			_cur++; // Trailing "

		return true;
	}

	bool simplified_json::Reader::ParseObject(ConfigValue& value)
	{
		value.SetEmptyObject();

		_cur++; // Skip '{'
		SkipSpaces();
		if (*_cur == '}') // Empty object
		{
			_cur++;
			return true;
		}

		string name;
		while (1)
		{
			SkipSpaces();

			name = "";
			if (!ParseString(name))
			{
				Error("Failed to parse string");
				break; // Failed to parse string
			}

			SkipSpaces();
			if (*_cur != '=' && *_cur != ':')
			{
				Error("Expected '=' or ':'");
				return false;
			}
			_cur++;

			ConfigValue& elem = value[name.c_str()];
			if (!ParseValue(elem))
				break; // Failed to parse value

			SkipSpaces();

			char c = *_cur;
			if (c == ',') // Separator between elements (Optional)
			{
				_cur++;
				continue;
			}
			if (c == '}') // End of object
			{
				_cur++;
				return true;
			}
		}

		return false;
	}
	bool simplified_json::Reader::ParseArray(ConfigValue& value)
	{
		value.SetEmptyArray();

		_cur++; // Skip '['
		SkipSpaces();
		if (*_cur == ']')
		{
			_cur++;
			return true;
		}
		while (1)
		{
			ConfigValue& elem = value.Append();

			if (!ParseValue(elem))
				return false;

			SkipSpaces();

			char c = *_cur;
			if (c == ',') // Separator between elements (Optional)
			{
				_cur++;
				continue;
			}
			if (c == ']') // End of array
			{
				_cur++;
				break;
			}
		}
		return true;
	}


	bool simplified_json::Reader::ParseDouble(ConfigValue& value)
	{
		char* number_end;
		double number = std::strtod(_cur, &number_end);
		value.SetDouble(number);
		_cur = number_end;
		return true;
	}

	bool simplified_json::Reader::ParseNumber(ConfigValue& value)
	{
		bool integer = true; // Number is either integer or float
		for (const char* c = _cur; c != _end; ++c)
		{
			if ((*c >= '0' && *c <= '9') || ((*c == '-' || *c == '+') && (c == _cur))) // Allow a negative sign at the start for integers
				continue;
			else if (*c == '.' || *c == 'e' || *c == 'E' || *c == '+')
			{
				integer = false;
				break;
			}
			else
				break;
		}
		if (!integer)
			return ParseDouble(value);


		bool negative = (*_cur == '-');
		if (negative)
			_cur++;

		uint64_t number = 0;
		while (_cur != _end)
		{
			if (*_cur >= '0' && *_cur <= '9')
			{
				uint32_t digit = *_cur - '0';
				number = number * 10 + digit;
				_cur++;
			}
			else
				break;
		}
		if (negative)
		{
			value.SetInt(-int64_t(number));
		}
		else if (number <= INT64_MAX)
		{
			value.SetInt(int64_t(number));
		}
		else
		{
			value.SetUInt(number);
		}

		return true;
	}

	bool simplified_json::Reader::ParseValue(ConfigValue& value)
	{
		SkipSpaces();
		bool b = true;
		char c = *_cur;
		switch (c)
		{
		case '{':
			b = ParseObject(value);
			break;
		case '[':
			b = ParseArray(value);
			break;
		case '"':
		{
			string str;
			b = ParseString(str);
			if (b)
				value.SetString(str.c_str());
			else
				Error("Failed to parse string");
		}
			break;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case '-':
			b = ParseNumber(value);
			break;
		case 't': // true
			if (*(++_cur) != 'r' || *(++_cur) != 'u' || *(++_cur) != 'e')
			{
				Error("Expected \"true\"");
				return false;
			}
			++_cur;
			value.SetBool(true);
			break;
		case 'f': // false
			if (*(++_cur) != 'a' || *(++_cur) != 'l' || *(++_cur) != 's' || *(++_cur) != 'e')
			{
				Error("Expected \"false\"");
				return false;
			}
			++_cur;
			value.SetBool(false);
			break;
		case 'n': // null
			if (*(++_cur) != 'u' || *(++_cur) != 'l' || *(++_cur) != 'l')
			{
				Error("Expected \"null\"");
				return false;
			}
			++_cur;
			value.SetNull();
			break;
		default:
			b = false;
		};
		return b;
	}
	//-------------------------------------------------------------------------------

	namespace json_internal
	{
		void WriteTabs(int ilevel, stringstream& out)
		{
			for (int i = 0; i < ilevel; ++i)
			{
				out << "\t";
			}
		}

		/// @param quotes Specifies whetever the string is supposed to be surrounded by quotes
		void WriteString(const char* str, stringstream& out, bool quotes = true)
		{
			// We need to escape any special characters before writing them to the JSON doc
			for (const char* c = str; *c != '\0'; ++c)
			{
				switch (*c)
				{
				case '\\':
					out << "\\\\";
					break;
				case '\"':
					out << "\\\"";
					break;
				case '\n':
					out << "\\n";
					break;
				case '\r':
					out << "\\r";
					break;
				case '\t':
					out << "\\t";
					break;
				case ' ':
					if (!quotes) // If string isn't supposed to be surrouneded by quotes, we escape spaces
						out << "\\ ";
					else
						out << " ";
					break;
				default:
					out << *c;
					break;

				}
			}
		}

	}

	//-------------------------------------------------------------------------------
	simplified_json::Writer::Writer() : _format(false), _ilevel(0)
	{
	}
	simplified_json::Writer::~Writer()
	{
	}
	void simplified_json::Writer::Write(const ConfigValue& root, stringstream& out, bool format)
	{
		_format = format;
		ConfigValue::ConstIterator it, end;
		it = root.Begin(); end = root.End();
		for (; it != end; ++it)
		{
			if (_format)
				json_internal::WriteTabs(_ilevel, out);
			json_internal::WriteString(it->first.c_str(), out, false);
			out << " = ";
			WriteValue(it->second, out);
			if (!format)
				out << " ";
		}
		out << "\n";
	}
	void simplified_json::Writer::WriteValue(const ConfigValue& node, stringstream& out)
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
				if (_format)
				{
					out << "\n";
					json_internal::WriteTabs(_ilevel, out);
				}
				json_internal::WriteString(it->first.c_str(), out, false);
				out << " = ";
				WriteValue(it->second, out);
				out << " ";
			}
			if (_format)
				out << "\n";

			_ilevel--;
			if (_format)
				json_internal::WriteTabs(_ilevel, out);
			out << "}";
			if (_format)
				out << "\n";
		}
			break;
		};
	}

	//-------------------------------------------------------------------------------

} // namespace sb


