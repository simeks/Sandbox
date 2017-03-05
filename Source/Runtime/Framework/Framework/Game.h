// Copyright 2008-2014 Simon Ekström

#ifndef __FRAMEWORK_GAME_H__
#define __FRAMEWORK_GAME_H__



namespace sb
{

	/// Interface for a game class
	class IGame
	{
	public:
		virtual ~IGame() {}

		virtual void Initialize() = 0;
		virtual void Shutdown() = 0;
		virtual void Update(float dtime) = 0;
		virtual void Render() = 0;
	};





#define GAME_FACTORY(game_type) \
	sb::IGame* CreateGame(sb::GameFramework& app) { \
	return new game_type(app); \
	} \
	void DestroyGame(sb::IGame* game) { \
	delete game; \
	} 


} // namespace sb

#endif // __FRAMEWORK_H__
