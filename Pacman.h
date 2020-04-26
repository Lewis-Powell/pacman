#pragma once

// If Windows and not in Debug, this will run without a console window
// You can use this to output information when debugging using cout or cerr
#ifdef WIN32 
#ifndef _DEBUG
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif
#endif

#define MUNCHIECOUNT 50
#define GHOSTCOUNT 1

// Just need to include main header file
#include "S2D/S2D.h"

// Reduces the amount of typing by including all classes in S2D namespace
using namespace S2D;

//A structure to hold all variables relevant to the player
struct Player
{
	bool dead;
	Vector2* position = {};
	Rect* sourceRect;
	Texture2D* texture;
	int direction = 0;
	int frame = 0;
	int currentFrameTime = 0;
	float speedMultiplier;
	int score;
};

struct Enemy
{
	int _frameCount;
	Rect* _rect;
	Texture2D* _blueTexture;
	Texture2D* _invertedTexture;
	int _currentFrameTime;
	int _frameTime;
};

struct MovingEnemy
{
	Vector2* position;
	Texture2D* texture;
	Rect* sourceRect;
	int direction;
	float speed;
};

struct Cherry
{
	Rect* _rect;
	Texture2D* _texture;
	Texture2D* _invTexture;
	int _frameCount;
	int _currentFrameTime;
	int _frameTime;
};



// Declares the Pacman class which inherits from the Game class.
// This allows us to overload the Game class methods to help us
// load content, draw and update our game.
class Pacman : public Game
{
private:

	// Data to represent Pacman
	Player* _pacman;
	// Data to represent number of munchies
	Enemy* _munchie[MUNCHIECOUNT];
	//Number of ghosts
	MovingEnemy* _ghosts[GHOSTCOUNT];

	Cherry* _cherry;
	//Const data Game Variable
	const int _cPacmanFrameTime;
	
	// Position for String
	Vector2* _stringPosition;

	//Data for Menu
	Texture2D* _menuBackground;
	Rect* _menuRectangle;
	Vector2* _menuStringPosition;
	bool _paused;
	bool _pKeyDown;

	//Setting up an end screen
	Texture2D* _endBackground;
	Rect* _endRectangle;
	Vector2* _endStringPosition;

	SoundEffect* _eat;


	void Input(int elapsedTime, Input::KeyboardState* state, Input::MouseState* mouseState);

	void CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey);

	void CheckViewportCollision();

	void UpdatePacman(int elapsedTime);

	void UpdateMunchie(int elapsedTime);

	void UpdateCherry(int elapsedTime);

	void CheckGhostCollisions();

	void UpdateGhost(int elapsedTime, Input::MouseState* mouseState); //MovingEnemy*

	bool CollisonCheck(int x1, int y1, int width1, int height1, int x2, int y2, int width2, int height2);


public:
	/// <summary> Constructs the Pacman class. </summary>
	Pacman(int argc, char* argv[]);

	/// <summary> Destroys any data associated with Pacman class. </summary>
	virtual ~Pacman();

	/// <summary> All content should be loaded in this method. </summary>
	void virtual LoadContent();

	/// <summary> Called every frame - update game logic here. </summary>
	void virtual Update(int elapsedTime);

	/// <summary> Called every frame - draw game here. </summary>
	void virtual Draw(int elapsedTime);
	
};