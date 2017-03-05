require "tundra.syntax.glob"

DefRule {
	Name = "Setup",
	Pass = "SetupPass",
	Command = "build.py setup",
	ConfigInvariant = true,
	
	Blueprint = {
	},
	Setup = function (env, data)
		return {
			InputFiles = {},
			OutputFiles = {},
		}
	end,
}

DefRule {
	Name = "VersionGenerator",
	Pass = "CodeGeneration",
	Command = "version.py $(@)",
	ConfigInvariant = true,
	
	Blueprint = {
		OutName = { Required = true, Type = "string" },
	},
	Setup = function (env, data)
		return {
			InputFiles = {},
			OutputFiles = { "Source/Runtime/Engine/" .. data.OutName },
		}
	end,
}

StaticLibrary {
	Name = "Foundation",
	Env = {
		CPPPATH = { 
			"Source/Runtime/Foundation",
			"Source/Runtime/",
			"External/lua-5.2.3/include",
		}, 
	},

	Sources = {
		FGlob {
			Dir = "Source/Runtime/Foundation",
			Extensions = { ".cpp", ".h", ".inl" },
			Filters = {
				{ Pattern = "Win"; Config = {"win32-*-*", "win64-*-*"}; },
				{ Pattern = "Mac"; Config = "macosx-*-*"; },
				{ Pattern = "Posix"; Config = "macosx-*-*"; },
				{ Pattern = "Tests"; Config = {}; },
			},
		},
	},
}

Program {
	Name = "Test_Foundation",
	Target = "Binaries/$(CURRENT_PLATFORM)/Test_Foundation-$(CURRENT_VARIANT).exe",
	Depends = { "Foundation" },
	Env = {
		CPPPATH = { 
			"Source/Tools/",
			"Source/Runtime/Foundation",
			"Source/Runtime/",
			"External/lua-5.2.3/include",
		}, 
		LIBPATH = {
			{ 
				"\"$(DXSDK_DIR)/Lib/x86\"",
				"External/lua-5.2.3/lib/x86"; 
				Config = "win32-*-*" 
			},
			
			{ 
				"\"$(DXSDK_DIR)/Lib/x64\""; 
				"External/lua-5.2.3/lib/x64";
				Config = "win64-*-*" 
			},
		},
	},

	Sources = {
		Setup {},
		FGlob {
			Dir = "Source/Runtime/Foundation/Tests",
			Extensions = { ".cpp", ".h", ".inl" },
			Filters = {
				{ Pattern = "Win"; Config = {"win32-*-*", "win64-*-*"}; },
				{ Pattern = "Mac"; Config = "macosx-*-*"; },
				{ Pattern = "Posix"; Config = "macosx-*-*"; },
			},
		},
		"Source/Tools/Testing/Framework.h",
		"Source/Tools/Testing/Framework.cpp"
	},

	Libs = { 
		{ 
			"kernel32.lib", 
			"user32.lib", 
			"gdi32.lib", 
			"comdlg32.lib", 
			"advapi32.lib", 
			"ws2_32.lib", 
			"liblua52.a";
			Config = { "win32-*-*", "win64-*-*" } 
		}
	},
}

StaticLibrary {
	Name = "Engine",
	Env = {
		CPPPATH = { 
			"Source/Runtime/Engine",
			"Source/Runtime/",
			".",
		}, 
	},

	Sources = {
		VersionGenerator { OutName = "EngineVersion.h" },
		FGlob {
			Dir = "Source/Runtime/Engine",
			Extensions = { ".cpp", ".h", ".inl" },
			Filters = {
				{ Pattern = "Win"; Config = {"win32-*-*", "win64-*-*"}; },
				{ Pattern = "Mac"; Config = "macosx-*-*"; },
			},
		},
	},
}

StaticLibrary {
	Name = "Framework",
	Env = {
		CPPPATH = { 
			"Source/Runtime/Framework",
			"Source/Runtime/",
			".",
		}, 
	},

	Sources = {
		FGlob {
			Dir = "Source/Runtime/Framework",
			Extensions = { ".cpp", ".h", ".inl" },
			Filters = {
				{ Pattern = "Win"; Config = {"win32-*-*", "win64-*-*"}; },
				{ Pattern = "Mac"; Config = "macosx-*-*"; },
			},
		},
	},
}

StaticLibrary {
	Name = "RenderD3D11",
	Env = {
		CPPPATH = { 
			"Source/Runtime/RenderD3D11",
			"Source/Runtime/",
			".",
		}, 
	},

	Sources = {
		FGlob {
			Dir = "Source/Runtime/RenderD3D11",
			Extensions = { ".cpp", ".h", ".inl" },
			Filters = {
				{ Pattern = "Win"; Config = {"win32-*-*", "win64-*-*"}; },
				{ Pattern = "Mac"; Config = "macosx-*-*"; },
			},
		},
	},
	
}

StaticLibrary {
	Name = "Game",
	Env = {
		CPPPATH = { 
			"Source/Runtime/Game",
			"Source/Runtime/",
			".",
		}, 
	},

	Sources = {
		FGlob {
			Dir = "Source/Runtime/Game",
			Extensions = { ".cpp", ".h", ".inl" },
			Filters = {
				{ Pattern = "Win"; Config = {"win32-*-*", "win64-*-*"}; },
				{ Pattern = "Mac"; Config = "macosx-*-*"; },
			},
		},
	},
	
}
Program {
	Name = "Builder",
	Target = "Binaries/$(CURRENT_PLATFORM)/Builder-$(CURRENT_VARIANT).exe",
	Env = {
		CPPPATH = { 
			"Source/Tools/Builder",
			"Source/Runtime/",
			"External/nvidia-texture-tools/include",
			".",
		}, 
		LIBPATH = {
			{ 
				"External/nvidia-texture-tools/lib/x86"; 
				Config = "win32-*-*" 
			},
			
			{ 
				"External/nvidia-texture-tools/lib/x64"; 
				Config = "win64-*-*" 
			},
		},
		PROGOPTS = {
			{ "/SUBSYSTEM:CONSOLE"; Config = {"win32-*-*", "win64-*-*"} },
		},
	},
	Sources = {
		Setup {},
		FGlob {
			Dir = "Source/Tools/Builder",
			Extensions = { ".cpp", ".c", ".h", ".inl", ".rc" },
			Filters = {
				{ Pattern = "Win"; Config = {"win32-*-*", "win64-*-*"}; },
				{ Pattern = "Mac"; Config = "macosx-*-*"; },
			},
		},
	},
	Depends = { "Foundation", "Engine", "RenderD3D11", "Framework" },

	Libs = 
		{ 
			{ "kernel32.lib", "user32.lib", "gdi32.lib", "comdlg32.lib", "advapi32.lib", "Shell32.lib", "WS2_32.lib",
				"nvtt.lib", "jpeg.lib", "libpng.lib", "tiff.lib", "zlib.lib";
				Config = { "win32-*-*", "win64-*-*" } 
			},
			{
				 "nvimage.lib", "nvcore.lib"; Config = { "win32-*-release", "win64-*-release", "win32-*-production", "win64-*-production" } 
			},
			{
				 "nvimaged.lib", "nvcored.lib"; Config = { "win32-*-debug", "win64-*-debug" } 
			},
		},

}


Program {
	Name = "Launcher",
	Target = "Binaries/$(CURRENT_PLATFORM)/Launcher-$(CURRENT_VARIANT).exe",
	
	Env = {
		CPPDEFS = { "SANDBOX_PROFILE"; Config = "*-*-production" },
		CPPPATH = { 
			"Source/Runtime/Launcher",
			"Source/Runtime/",
			".",
		}, 
		PROGOPTS = {
			{ "/SUBSYSTEM:WINDOWS"; Config = {"win32-*-*", "win64-*-*"} },
		},
		LIBPATH = {
			{ 
				"\"$(DXSDK_DIR)/Lib/x86\"",
				"External/lua-5.2.3/lib/x86"; 
				Config = "win32-*-*" 
			},
			
			{ 
				"\"$(DXSDK_DIR)/Lib/x64\""; 
				"External/lua-5.2.3/lib/x64";
				Config = "win64-*-*" 
			},
		},
	},
	Sources = {
		Setup {},
		FGlob {
			Dir = "Source/Runtime/Launcher",
			Extensions = { ".cpp", ".h", ".inl" },
			Filters = {
				{ Pattern = "Win"; Config = {"win32-*-*", "win64-*-*"}; },
				{ Pattern = "Mac"; Config = "macosx-*-*"; },
			},
		},
	},
	Depends = { "Foundation", "Engine", "Framework", "RenderD3D11", "Game" },

	Libs = { 
		{ 
			"kernel32.lib", 
			"user32.lib", 
			"gdi32.lib", 
			"comdlg32.lib", 
			"advapi32.lib", 
			"ws2_32.lib", 
			"d3d11.lib",
			"DXGI.lib",
			"liblua52.a";
			Config = { "win32-*-*", "win64-*-*" } 
		}
	},
}

Default "Launcher"
