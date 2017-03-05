// Copyright 2008-2014 Simon Ekström

#include "Common.h"
#include "Game.h"
#include "CameraMover.h"
#include "DebugGUI.h"

#include <Framework/Framework/GameFramework.h>
#include <Framework/Rendering/Renderer.h>
#include <Framework/World/World.h>
#include <Framework/World/RenderWorld.h>
#include <Framework/World/GameObject.h>
#include <Framework/World/Camera.h>
#include <Framework/World/LightComponent.h>
#include <Framework/World/MeshComponent.h>
#include <Framework/Rendering/ShadingEnvironment.h>
#include <Framework/Window/Window.h>

#include <Foundation/Resource/ResourcePackage.h>
#include <Foundation/Profiler/Profiler.h>

#include <Engine/Input/input.h>

GAME_FACTORY(sb::Game);


namespace sb
{

Game::Game(GameFramework& framework)
	: _framework(framework),
	_world(nullptr),
	_camera(nullptr),
	_test_map_entity(nullptr),
	_shading_env(nullptr),
	_debug_gui(nullptr)
{

}
Game::~Game()
{

}

void Game::Initialize()
{
	_framework.GetResourceManager()->Load("package", "game", nullptr);
	_framework.GetResourceManager()->Flush();

	_resource_package = (ResourcePackage*)_framework.GetResourceManager()->GetResource("package", "game");
	_resource_package->Load();
	_resource_package->Flush();

	Renderer* renderer = _framework.GetRenderer();

	_shading_env = renderer->GetShadingEnvironment("main");

	_viewport.x = 0;
	_viewport.y = 0;
	_viewport.min_depth = 0.0f;
	_viewport.max_depth = 1.0f;

	uint32_t w, h;
	renderer->GetBackbufferSize(w, h);
	_viewport.width = (float)w;
	_viewport.height = (float)h;

	_world = _framework.CreateWorld();

	_debug_gui = new DebugGUI(renderer->GetMaterialManager(), 
								_framework.GetResourceManager(),
								renderer->GetGlobalResourceSet(),
								renderer->GetResourceAllocator(),
								w, h);

	profiler::SetDrawCallback(_debug_gui);

	_camera_entity = _world->CreateObject();
	_camera = _camera_entity->CreateComponent<Camera>("camera");
	_camera->SetPerspective(0.1f, 250.0f, 16.0f / 10.0f, CAMERA_DEFAULT_FOV);
	_camera_entity->GetTransform().SetLocalPosition(Vec3f(0, -10, 0));

	_external_camera_entity = _world->CreateObject();
	_external_camera = _external_camera_entity->CreateComponent<Camera>("");
	_external_camera->SetPerspective(0.1f, 25.0f, 16.0f / 10.0f, 25.0f*MATH_DEG_TO_RAD);
	_external_camera_entity->GetTransform().SetLocalPosition(Vec3f(0, -10, 0));

	Mat3x3f rot = Mat3x3f::CreateOrientation(Vec3f(0, 1, 0), Vec3f(0, 0, 1));
	_external_camera_entity->GetTransform().SetLocalRotation(rot);

	InputManager* input = _framework.GetInputManager();
	_camera_mover = new CameraMover(input->GetKeyboard(), input->GetMouse(), _camera_entity);

	Mesh* test_map_mesh = (Mesh*)_framework.GetResourceManager()->GetResource("mesh", "models/test_map");

	_test_map_entity = _world->CreateObject();
	MeshComponent* mesh_component = _test_map_entity->CreateComponent<MeshComponent>("mesh");
	mesh_component->SetMesh(test_map_mesh, _framework.GetRenderer()->GetMaterialManager());

	_test_map_entity->GetTransform().SetLocalScale(Vec3f(1, 1, 1));
	_test_map_entity->GetTransform().SetLocalPosition(Vec3f(0, 0, 0));

	Mesh* sphere_mesh = (Mesh*)_framework.GetResourceManager()->GetResource("mesh", "models/sphere");
	for (uint32_t i = 0; i < 32; ++i)
	{
		GameObject* ent = _world->CreateObject();
		ent->CreateComponent<MeshComponent>("")->SetMesh(sphere_mesh, _framework.GetRenderer()->GetMaterialManager());

		//ent->SetPosition(Vec3f(5,0,0));
		ent->GetTransform().SetLocalPosition(Vec3f(float(rand() % 32) - 16.0f, float(rand() % 32) - 16.0f, 0.0f));
	}
	//for (uint32_t i = 0; i < 1; ++i)
	//{
	//	Entity* ent = _world->CreateEntity();
	//	Light* light = ent->CreateLight("");
	//	light->SetColor(Vec3f(float(rand() % 256) / 256.0f, float(rand() % 256) / 256.0f, float(rand() % 256) / 256.0f));
	//	light->SetLightType(Light::LT_POINT);
	//	light->SetRange(5.0f);
	//	ent->SetPosition(Vec3f(0, 0, 0));
	//	//ent->SetPosition(Vec3f(float(rand() % 64) - 32.0f, float(rand() % 64) - 32.0f, 0.0f));
	//}
	_shading_env->SetVector3("sun_light_color", Vec3f(0.7f, 0.7f, 0.7f));
}
void Game::Shutdown()
{
	_world->ReleaseObject(_test_map_entity);

	delete _debug_gui;
	_debug_gui = NULL;

	delete _camera_mover;
	_camera_mover = NULL;

	_framework.ReleaseWorld(_world);
	_world = NULL;

	_resource_package->Unload();
	_framework.GetResourceManager()->Unload("package", "game");

}

void Game::Update(float dtime)
{
	PROFILER_SCOPE("Game::Update");

	Keyboard* keyboard = _framework.GetInputManager()->GetKeyboard();
	Mouse* mouse = _framework.GetInputManager()->GetMouse();

	if (keyboard->Pressed(input::KC_ESCAPE))
		_framework.Stop();

	if (keyboard->Pressed(input::KC_F5))
		profiler::ToggleDisplayMode();

	if (keyboard->Pressed(input::KC_F6))
		profiler::TogglePause();

	if (mouse->IsDown(input::MB_RIGHT))
	{
		_light_dir_angles.x += mouse->Axis().x * dtime;
		_light_dir_angles.y += mouse->Axis().y * dtime;

		_light_dir_angles.x = math::AngleNormalize2PI(_light_dir_angles.x);
		_light_dir_angles.y = math::AngleNormalize2PI(_light_dir_angles.y);

		Vec3f light_dir = Vec3f(math::Sin(_light_dir_angles.x), math::Cos(_light_dir_angles.x), math::Sin(_light_dir_angles.y));
		_shading_env->SetVector3("sun_light_direction", light_dir);


	}

	if (mouse->Pressed(input::MB_LEFT))
	{
		MicroProfileMouseButton(1, 0);
	}
	if (mouse->Released(input::MB_LEFT))
	{
		MicroProfileMouseButton(0, 0);
	}

	if (mouse->Pressed(input::MB_RIGHT))
	{
		MicroProfileMouseButton(0, 1);
	}
	if (mouse->Released(input::MB_RIGHT))
	{
		MicroProfileMouseButton(0, 0);
	}

	if (mouse->Axis().Length() > 0.0f) 
	{
		Vec2f m = mouse->Position();
		MicroProfileMousePosition((uint32_t) m.x, (uint32_t) m.y, (uint32_t) -mouse->Axis().z);
	}

	_camera_mover->Update(dtime);
	_world->Update(dtime);
}
void Game::Render()
{
	PROFILER_SCOPE("Game::Render");

	_framework.DrawWorld(_world, _camera, &_viewport, _shading_env);

	profiler::Draw((uint32_t) _viewport.width, (uint32_t) _viewport.height);

	_debug_gui->Draw();
	_framework.FlushGUI(_debug_gui->GetCanvas());
}

} // namespace sb

