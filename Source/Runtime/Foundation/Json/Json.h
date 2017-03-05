// Copyright 2008-2014 Simon Ekström

#ifndef __FOUNDATION_JSON_H__
#define __FOUNDATION_JSON_H__

#include "SimplifiedJson.h"


namespace sb
{

	class FileSource;

	namespace json
	{
		typedef simplified_json::Reader Reader; // Simplified-JSON parser also works for standard JSON

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

