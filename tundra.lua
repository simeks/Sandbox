native = require "tundra.native"

local win32_config = {
	Env = {
		DXSDK_DIR = native.getenv("DXSDK_DIR", "."),
		CPPDEFS = { "SANDBOX_PLATFORM_WIN" },
		CXXOPTS = {
			"/WX", "/W4", "/EHsc", "/D_CRT_SECURE_NO_WARNINGS",
			"/D_MBCS",
			"/wd4512", -- C4512: assignment operator could not be generated.
			"/wd28251", -- C28251: Inconsistent annotation for '*': this instance has no annotations.
			"/wd4127", -- C4127: conditional expression is constant.
			{ 
				--"/analyze",
				"/MDd", 
				"/Od"; 
				Config = "*-*-debug" 
			},
			{ "/MD", "/O2"; Config = {"*-*-release", "*-*-production"} },
		},
		GENERATE_PDB = {
			{ "0"; Config = "*-vs2013-release" },
			{ "1"; Config = { "*-vs2013-debug", "*-vs2013-production" } },
		}
	},	
}

local macosx_config = {
	Env = {
		CPPDEFS = { "SANDBOX_PLATFORM_MACOSX" },
		CXXOPTS = {
			"-Werror", "-Wall", "-std=c++11",
			{ "-O0", "-g"; Config = "*-*-debug" },
			{ "-O2"; Config = {"*-*-release", "*-*-production"} },
		},
		LD = { "-lc++" },
	}
}

Build {
	Units = "units.lua",
	Passes = {
		SetupPass = { Name = "Setup Pass", BuildOrder = 1 },
		CodeGeneration = { Name = "Code Generation", BuildOrder = 2 },
	},
	
	Configs = {
		Config {
			Name = "macosx-gcc",
            Inherit = macosx_config,
			DefaultOnHost = "macosx",
			Tools = { "clang-osx" },
		},
		Config {
			Name = "win64-vs2013",
			Inherit = win32_config,
			Tools = { { "msvc-vs2013"; TargetArch = "x64" }, },
			SupportedHosts = { "windows" },	
			Env = {
				CPPDEFS = { "SANDBOX_PLATFORM_WIN64" },
			}
		},
		Config {
			Name = "win32-vs2013",
			Inherit = win32_config,
			Tools = { { "msvc-vs2013"; TargetArch = "x86" }, },
			DefaultOnHost = "windows",
		},
	},

	Env = {
		CPPDEFS = {
			{ "_DEBUG"; Config = "*-*-debug" },
			{ "SANDBOX_BUILD_DEBUG"; Config = "*-*-debug" },
			{ "SANDBOX_BUILD_RELEASE"; Config = "*-*-release" },
			{ "SANDBOX_BUILD_PRODUCTION"; Config = "*-*-production" },

			{ "SANDBOX_DEVELOPMENT"; Config = {"*-*-debug", "*-*-production"} },

			{ "SANDBOX_FILE_TRACKING"; Config = "*-*-debug" },
			{ "SANDBOX_MEMORY_TRACKING"; Config = "*-*-debug" },
			-- { "SANDBOX_VERBOSE_TRACKING"; Config = "*-*-debug" }
			
		},
	},

	IdeGenerationHints = {
		Msvc = {
			-- Remap config names to MSVC platform names (affects things like header scanning & debugging)
			PlatformMappings = {
				['win64-vs2013'] = 'x64',
				['win32-vs2013'] = 'Win32',
			},
			-- Remap variant names to MSVC friendly names
			VariantMappings = {
				['release']    = 'Release',
				['debug']      = 'Debug',
				['production'] = 'Production',
			},
		},

		-- Override solutions to generate and what units to put where.
		MsvcSolutions = {
			['Sandbox.sln'] = {},          -- receives all the units due to empty set
		},

		BuildAllByDefault = true,
	}
}
