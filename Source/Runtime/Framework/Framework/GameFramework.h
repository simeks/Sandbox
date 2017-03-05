// Copyright 2008-2014 Simon Ekström

#ifndef __FRAMEWORK_H__
#define __FRAMEWORK_H__

#include "FrameTimer.h"

#include <Foundation/Container/ConfigValue.h>

namespace sb
{

	struct Viewport;

	class IGame;
	class Window;
	class Renderer;
	class FileSystem;
	class TaskScheduler;
	class ResourceManager;
	class ResourcePackage;
	class InputManager;
	class RenderContext;
	class World;
	class Camera;
	class ShadingEnvironment;
	class GUICanvas;

	class GameFramework
	{
	public:
		struct Params
		{
			HINSTANCE hinstance;
			string base_directory;
			string data_directory;
		};

		typedef IGame* (*CreateGameFn)(GameFramework&);
		typedef void(*DestroyGameFn)(IGame*);

	public:
		GameFramework(Params& params, CreateGameFn create_game, DestroyGameFn destroy_game);
		~GameFramework();

		/// @brief Runs the application
		///	This method will not return until the application is stopped
		void Run();

		/// @brief Stops the application
		void Stop();

		void DrawWorld(World* world, Camera* camera, Viewport* viewport, ShadingEnvironment* shading_env, Camera* external_frustum = nullptr);
		void FlushGUI(GUICanvas* gui);

		World* CreateWorld();
		void ReleaseWorld(World* world);

		TaskScheduler* GetTaskScheduler() const;
		FileSystem*	GetFileSystem() const;
		ResourceManager* GetResourceManager() const;
		Window*	GetPrimaryWindow() const;
		InputManager* GetInputManager() const;

		uint32_t GetSwapChain() const;
		Renderer* GetRenderer() const;


	private:
		void Initialize();
		void Shutdown();

		void InitalizeRenderer();
		void ShutdownRenderer();

		/// Fills the specified ConfigValue with the default application settings 
		void SetDefaultSettings(ConfigValue& settings);

		FrameTimer _frame_timer;
		Params _params; ///< Application parameters

		// Game interface
		CreateGameFn _create_game;
		DestroyGameFn _destroy_game;
		IGame* _game;

		// Engine systems
		TaskScheduler* _scheduler;
		FileSystem*	_file_system;
		ResourceManager* _resource_manager;
		Renderer* _renderer;
		InputManager* _input;

		// Application specific
		Window* _window;
		uint32_t _swap_chain;
		ResourcePackage* _base_package;
		ConfigValue _settings;

		bool _stop; ///< Indicates if the application should stop

		vector<World*> _worlds;
	};

} // namespace sb

#endif // __FRAMEWORK_H__
