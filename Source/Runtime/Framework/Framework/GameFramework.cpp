// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "GameFramework.h"
#include "World/World.h"
#include "World/RenderWorld.h"
#include "Rendering/Renderer.h"
#include "Window/Window.h"
#include "Game.h"

#include <Foundation/Profiler/Profiler.h>
#include <Foundation/Filesystem/FileSystem.h>
#include <Foundation/Resource/ResourceManager.h>
#include <Foundation/Resource/ResourcePackage.h>
#include <Foundation/IO/InputBuffer.h>
#include <Foundation/Json/Json.h>
#include <Foundation/Debug/Console.h>
#include <Foundation/Debug/ConsoleServer.h>
#include <Foundation/Timer/Timer.h>
#include <Foundation/Thread/TaskScheduler.h>

#include <Engine/Input/Input.h>



namespace sb
{

//-------------------------------------------------------------------------------

// Callbacks

namespace
{
#ifdef SANDBOX_DEVELOPMENT
	static const char* level_to_string[] = {
		/* LOG_INFO		*/	"info",
		/* LOG_WARNING	*/	"warning",
		/* LOG_ERROR	*/	"error"
	};

	void LoggingCallback(void*, Log::LogLevel level, const char* msg)
	{
		ConfigValue msg_data; // TODO: Temp Alloc
		msg_data.SetEmptyObject();
		msg_data["type"].SetString("log_msg");
		msg_data["level"].SetString(level_to_string[level]);
		msg_data["msg"].SetString(msg);

		if (console::Server())
			console::Server()->Send(msg_data);
	}
#endif // SANDBOX_DEVELOPMENT


	LRESULT WndMessageCallback(void* data, HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		InputManager* input_manager = (InputManager*)data;
		switch (msg)
		{
		case WM_CLOSE: // Sent when the program should terminate
		case WM_DESTROY: // Sent when a window is destroyed
		{
			PostQuitMessage(0);
			break;
		}
		};
		input_manager->HandleMessage(hwnd, msg, wparam, lparam);

		return DefWindowProc(hwnd, msg, wparam, lparam);
	}

}

//-------------------------------------------------------------------------------
GameFramework::GameFramework(Params& params, CreateGameFn create_game, DestroyGameFn destroy_game)
	: _params(params),
	_create_game(create_game),
	_destroy_game(destroy_game),
	_game(nullptr),
	_scheduler(nullptr),
	_file_system(nullptr),
	_resource_manager(nullptr),
	_window(nullptr),
	_input(nullptr),
	_renderer(nullptr),
	_swap_chain(Invalid<uint32_t>()),
	_stop(false),
	_base_package(nullptr)
{
}
GameFramework::~GameFramework()
{
	Assert(_worlds.empty()); // Check for unreleased worlds
}
void GameFramework::SetDefaultSettings(ConfigValue& settings)
{
	settings.SetEmptyObject();

	settings["window_name"].SetString("Application");
	settings["console_server_port"].SetInt(25016);

	settings["renderer"].SetEmptyObject();
	settings["renderer"]["resolution_width"].SetInt(800);
	settings["renderer"]["resolution_height"].SetInt(600);
	settings["renderer"]["fullscreen"].SetBool(false);
	settings["renderer"]["adapter_index"].SetInt(0);
	settings["renderer"]["vsync"].SetBool(true);

}
void GameFramework::Initialize()
{
	_scheduler = new TaskScheduler();
	_scheduler->Initialize();

	_file_system = new FileSystem(_params.base_directory.c_str());
	FileSource* file_source = _file_system->OpenFileSource(_params.data_directory.c_str(), FileSystem::ADD_TO_HEAD);

	// First set default settings
	SetDefaultSettings(_settings);

	// and then load any settings from settings file
	simplified_json::Reader reader;
	if (!reader.ReadFile(file_source, "application.settings", _settings))
	{
		logging::Warning("Failed to load \"application.settings\", using default settings.");
	}


#ifdef SANDBOX_DEVELOPMENT
	console::Initialize(_settings["console_server_port"].AsInt());
	logging::SetCallback(LoggingCallback, nullptr);
#endif

	_resource_manager = new ResourceManager(_file_system);
	_resource_manager->Initialize();

	InitalizeRenderer();

	// Load base resource package, TODO: Specify boot package in config
	_resource_manager->Load("package", "core", nullptr);
	_resource_manager->Flush();
	_base_package = (ResourcePackage*)_resource_manager->GetResource("package", "core");
	_base_package->Load();
	_base_package->Flush();

	_window = new Window();
	_window->Create(_settings["window_name"].AsString(),
		_settings["renderer"]["resolution_width"].AsInt(),
		_settings["renderer"]["resolution_height"].AsInt(),
		_settings["renderer"]["fullscreen"].AsBool());

	_swap_chain = _renderer->GetDevice()->CreateSwapChain(_window,
		!_settings["renderer"]["fullscreen"].AsBool());

	_renderer->GetDevice()->SetActiveSwapChain(_swap_chain);

	_input = new InputManager(_window->_hwnd);
	_window->SetMessageCallback(WndMessageCallback, _input);


	ConfigValue render_setup;

	// Set up renderer
	if (_settings["render_setup"].IsString())
	{
		if (!reader.ReadFile(file_source, _settings["render_setup"].AsString(), render_setup))
		{
			logging::Warning("Failed to load render setup: %s", reader.GetErrorMessage().c_str());
		}
	}

	_renderer->LoadRenderSetup(render_setup);

	_game = _create_game(*this);
	_game->Initialize();

}
void GameFramework::Shutdown()
{
	_game->Shutdown();
	_destroy_game(_game);
	_game = nullptr;

	_renderer->UnloadRenderSetup();

	delete _input;
	_input = nullptr;

	_renderer->GetDevice()->ReleaseSwapChain(_swap_chain);
	_swap_chain = Invalid<uint32_t>();

	_window->Destroy();
	delete _window;
	_window = nullptr;

	_base_package->Unload();
	_base_package = nullptr;

	_resource_manager->Unload("package", "core");

	ShutdownRenderer();

	_resource_manager->Shutdown();
	delete _resource_manager;
	_resource_manager = nullptr;

	delete _file_system;
	_file_system = nullptr;

	_scheduler->Shutdown();
	delete _scheduler;
	_scheduler = nullptr;

#ifdef SANDBOX_DEVELOPMENT
	console::Shutdown();
#endif

}
void GameFramework::InitalizeRenderer()
{
	_renderer = new Renderer(_resource_manager, _scheduler);
	Assert(_renderer);

	RenderDevice::InitParams render_params;
	render_params.vsync = _settings["renderer"]["vsync"].AsBool();
	render_params.adapter_index = _settings["renderer"]["adapter_index"].AsInt();

	_renderer->Initialize(render_params);

}
void GameFramework::ShutdownRenderer()
{
	_renderer->Shutdown();
	delete _renderer;
	_renderer = nullptr;
}

void GameFramework::Run()
{
	Initialize();

	_stop = false;


	while (!_stop)
	{
		PROFILER_SCOPE("Frame");
		float dtime = _frame_timer.Tick();

		// Message pump
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				_stop = true;
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		_input->Update();

#ifdef SANDBOX_DEVELOPMENT
		console::Server()->Update();
#endif
		_game->Update(dtime);
		_game->Render();

		_renderer->GetDevice()->Present();

		profiler::Flip();
	}

	Shutdown();
}
void GameFramework::Stop()
{
	_stop = true;
}

//-------------------------------------------------------------------------------

void GameFramework::DrawWorld(World* world, Camera* camera, Viewport* viewport, ShadingEnvironment* shading_env, Camera* external_frustum)
{
	_renderer->DrawWorld(world, camera, viewport, shading_env, external_frustum);
}

void GameFramework::FlushGUI(GUICanvas* gui)
{
	_renderer->FlushGUI(gui);
}

//-------------------------------------------------------------------------------

World* GameFramework::CreateWorld()
{
	World* world = new World(_renderer);
	_worlds.push_back(world);
	return world;
}
void GameFramework::ReleaseWorld(World* world)
{
	vector<World*>::iterator it = std::find(_worlds.begin(), _worlds.end(), world);
	if (it != _worlds.end())
	{
		delete world;
		_worlds.erase(it);
		return;
	}
	AssertMsg(false, "World object not found in application.");
}

//-------------------------------------------------------------------------------

TaskScheduler* GameFramework::GetTaskScheduler() const
{
	Assert(_scheduler);
	return _scheduler;
}
FileSystem* GameFramework::GetFileSystem() const
{
	Assert(_file_system);
	return _file_system;
}
ResourceManager* GameFramework::GetResourceManager() const
{
	Assert(_resource_manager);
	return _resource_manager;
}
Window* GameFramework::GetPrimaryWindow() const
{
	Assert(_window);
	return _window;
}
InputManager* GameFramework::GetInputManager() const
{
	Assert(_input);
	return _input;
}
uint32_t GameFramework::GetSwapChain() const
{
	return _swap_chain;
}
Renderer* GameFramework::GetRenderer() const
{
	Assert(_renderer);
	return _renderer;
}

//-------------------------------------------------------------------------------

} // namespace sb

