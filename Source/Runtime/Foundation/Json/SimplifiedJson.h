// Copyright 2008-2014 Simon Ekström

#ifndef __FOUNDATION_SIMPLIFIEDJSON_H__
#define __FOUNDATION_SIMPLIFIEDJSON_H__


namespace sb
{

	class FileSource;
	class ConfigValue;

	///
	///	Simplified JSON (http://bitsquid.blogspot.se/2009/10/simplified-json-notation.html)
	///
	///	- Assume an object definition at the root level (no need to surround entire file with { } ).
	///	- Commas are optional
	///	- Quotes around object keys are optional if the keys are valid identifiers
	///	- Replace : with =
	///
	namespace simplified_json
	{
		class Reader
		{
		public:
			Reader();
			~Reader();

			/// Parses a simplified-JSON document into ConfigValues
			///	@param doc JSON docoument
			///	@param root This is going to be the root node
			///	@return True if the parsing was successful, else false
			bool Read(const char* doc, int64_t length, ConfigValue& root);

			/// Reads a simplified-JSON document from a file
			///	@return True if the parsing was successful, else false
			bool ReadFile(FileSource* source, const char* file_name, ConfigValue& root);

			/// Returns an error message if the last call to Parse failed.
			const string& GetErrorMessage();


		private:
			const char* _begin;
			const char* _cur;
			const char* _end;

			string _error;

			void Error(const char* msg);
			void GetCurrentPosition(int& line, int& column);

			bool ParseValue(ConfigValue& value);
			bool ParseNumber(ConfigValue& value);
			bool ParseDouble(ConfigValue& value);
			bool ParseArray(ConfigValue& value);
			bool ParseObject(ConfigValue& value);

			bool ParseString(string& str);
			void SkipSpaces();

		};


		class Writer
		{
		public:
			Writer();
			~Writer();

			/// @brief Generates JSON from the specified ConfigValue
			/// @param root Root config node
			/// @param out The generated JSON will be stored in this variable
			/// @param format Should we use any formatting. Formatting makes it more 
			///			human readable, otherwise everything is just printed on one line.
			void Write(const ConfigValue& root, stringstream& out, bool format);

		private:
			bool _format;
			int _ilevel; // Indent level

			void WriteValue(const ConfigValue& node, stringstream& out);

		};

	};

} // namespace sb



#endif // __FOUNDATION_JSON_H__

