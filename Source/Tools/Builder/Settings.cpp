// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "Settings.h"
#include <Foundation/Container/ConfigValue.h>

namespace sb
{

	void build_settings::Load(const ConfigValue& cfg, BuildSettings& settings)
	{
		if (cfg["hlsl_optimization_level"].IsNumber())
		{
			settings.hlsl_optimization_level = cfg["hlsl_optimization_level"].AsUInt();
			if (settings.hlsl_optimization_level > 3)
			{
				logging::Warning("hlsl_optimization_level = %d, need to be in range [0, 3], defaulting to 0.", settings.hlsl_optimization_level);
				settings.hlsl_optimization_level = 0;
			}
		}

		if (cfg["hlsl_debug"].IsBool())
		{
			settings.hlsl_debug = cfg["hlsl_debug"].AsBool();
		}


	}

} // namespace sb

