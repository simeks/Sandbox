// Copyright 2008-2014 Simon Ekström

#ifndef __GAME_H__
#define __GAME_H__

#include <Framework/Framework/Game.h>
#include <Framework/Framework/GameFramework.h>
#include <Foundation/Math/Vec3.h>

#include <Engine/Rendering/Rendering.h>

namespace sb
{

class RenderContext;
class GameObject;
class World;
class DebugGUI;

class CameraMover;

class Game : public IGame
{
public:
	Game(GameFramework& framework);
	virtual ~Game();

	void Initialize();
	void Shutdown();
	void Update(float dtime);
	void Render();

private:
	GameFramework& _framework;

	ResourcePackage* _resource_package;
	World* _world;
	Viewport _viewport;
	ShadingEnvironment* _shading_env;

	GameObject* _camera_entity;
	Camera* _camera;

	GameObject* _external_camera_entity;
	Camera* _external_camera;

	GameObject* _test_map_entity;

	CameraMover* _camera_mover;
	Vec3f _light_dir_angles;

	DebugGUI* _debug_gui;

};

} // namespace sb



#endif // __GAME_H__
