// Copyright 2008-2014 Simon Ekström

#ifndef __BUILDER_SETTINGS_H__
#define __BUILDER_SETTINGS_H__

namespace sb
{

	class ConfigValue;
	struct BuildSettings
	{
		uint32_t hlsl_optimization_level;
		bool hlsl_debug;

		BuildSettings() : hlsl_optimization_level(0), hlsl_debug(false) {}
	};

	namespace build_settings
	{
		void Load(const ConfigValue& cfg, BuildSettings& settings);
	};

} // namespace sb

#endif // __BUILDER_SETTINGS_H__
