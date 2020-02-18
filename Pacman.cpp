#include "Pacman.h"
#include <time.h>
#include <sstream>
#include <iostream>

Pacman::Pacman(int argc, char* argv[]) : Game(argc, argv), _cPacmanFrameTime(200)//, _cSpeedMultiplier(0.1f) //_cMunchieFrameTime(500)
{
	_paused = true;
	_pKeyDown = false;
	srand(time(NULL));

	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		_munchie[i] = new Enemy();
		_munchie[i]->_currentFrameTime = 0;
		_munchie[i]->_frameCount = rand() % 1;
		_munchie[i]->_frameTime = rand() % 500 + 50;
	}

	//intialising ghost(s).
	for (int i = 0; i < GHOSTCOUNT; i++)
	{
		_ghosts[i] = new MovingEnemy();
		_ghosts[i]->direction = 0;
		_ghosts[i]->speed = 0.1f;
	}

	
	//Intialising Cherry
	_cherry = new Cherry();
	_cherry->_currentFrameTime = 0;
	_cherry->_frameCount = 0;

	//Intialising Member variables
	_pacman = new Player();
	_pacman->dead = false;
	_pacman->direction = 0;
	_pacman->currentFrameTime = 0;
	_pacman->frame = 0;
	_pacman->speedMultiplier = 1.0f;
	_pacman->score = 0;

	Audio::Initialise();
	_eat = new SoundEffect();

	//Initialise important Game aspects
	Graphics::Initialise(argc, argv, this, 1024, 768, false, 25, 25, "Pacman", 60);
	Input::Initialise();

	// Start the Game Loop - This calls Update and Draw in game loop
	Graphics::StartGameLoop();
}

Pacman::~Pacman()
{
	delete _pacman->position;
	delete _pacman->texture;
	delete _pacman->sourceRect;
	delete _pacman;

	delete _cherry->_texture;
	delete _cherry->_invTexture;
	delete _cherry->_rect;
	delete _cherry;

	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		delete _munchie[i]->_blueTexture;
		delete _munchie[i]->_invertedTexture;
		delete _munchie[i]->_rect;
		delete _munchie[i];
	}
	for (int j = 0; j < GHOSTCOUNT; j++)
	{
		delete _ghosts[j]->texture;
		delete _ghosts[j]->sourceRect;
		delete _ghosts[j];
	}
	delete _eat;
}

void Pacman::LoadContent()
{
	_eat->Load("SoundEffects/eatingmunchie.wav");

	if (!Audio::IsInitialised())
	{
		std::cout << "Audio is not initalised" << std::endl;
	}

	if(!_eat->IsLoaded())
	{
		std::cout << "Eat Munchie sound has not loaded" << std::endl;
	}

	//Pause Menu Parameters
	_menuBackground = new Texture2D();
	_menuBackground->Load("Textures/Transparency.png", false);
	_menuRectangle = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());
	_menuStringPosition = new Vector2(Graphics::GetViewportWidth() / 3.0f, Graphics::GetViewportHeight() / 3.0f);

	//Pause Menu Parameters
	_endBackground = new Texture2D();
	_endBackground->Load("Textures/Transparency.png", false);
	_endRectangle = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());
	_endStringPosition = new Vector2(Graphics::GetViewportWidth() / 3.0f, Graphics::GetViewportHeight() / 3.0f);

	// Load Pacman
	_pacman->texture = new Texture2D();
	_pacman->texture->Load("Textures/Pacman.tga", false);
	_pacman->position = new Vector2(350.0f, 350.0f);
	_pacman->sourceRect = new Rect(0.0f, 0.0f, 32, 32);

	//Load Ghost
	_ghosts[0]->texture = new Texture2D();
	_ghosts[0]->texture->Load("Textures/GhostBlue.png", true);
	_ghosts[0]->sourceRect = new Rect(0.0f, 0.0f, 20, 20);
	_ghosts[0]->position = new Vector2((rand() & Graphics::GetViewportWidth()), (rand() % Graphics::GetViewportHeight()));

	//Load Cherry
	_cherry->_texture = new Texture2D();
	_cherry->_texture->Load("Textures/Cherry.png", false);
	_cherry->_invTexture = new Texture2D();
	_cherry->_invTexture->Load("Textures/CherryInverted.png", false);
	_cherry->_rect = new Rect(100.0f, 500.0f, 32, 32);

	// Load Munchie
	Texture2D* munchieTex = new Texture2D();
	munchieTex->Load("Textures/Munchie.tga", true);
	Texture2D* munchieInvTex = new Texture2D();
	munchieInvTex->Load("Textures/MunchieInverted.tga", false);

	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		_munchie[i]->_blueTexture = munchieTex;
		_munchie[i]->_invertedTexture = munchieInvTex;
		_munchie[i]->_rect = new Rect(rand() % Graphics::GetViewportWidth(), (rand() % Graphics::GetViewportHeight()), 12, 12);
	}

	// Set string position
	_stringPosition = new Vector2(10.0f, 25.0f);
}

void Pacman::Update(int elapsedTime)
{
	Input::MouseState* mouseState = Input::Mouse::GetState();
	Input::KeyboardState* keyboardState = Input::Keyboard::GetState();
	Input::Keys pauseKey = Input::Keys::P;

	CheckPaused(keyboardState, pauseKey);

	Input(elapsedTime, keyboardState, mouseState);
	if (!_paused)
	{
		UpdatePacman(elapsedTime);
		CheckViewportCollision();
		for (int i = 0; i < MUNCHIECOUNT; i++)
		{
			if (CollisonCheck(_pacman->position->X, _pacman->position->Y, _pacman->sourceRect->Width, _pacman->sourceRect->Height, _munchie[i]->_rect->X, _munchie[i]->_rect->Y, _munchie[i]->_rect->Width, _munchie[i]->_rect->Height))
			{
				Audio::Play(_eat);
				_pacman->score += 10;
				_munchie[i]->_rect->X = -100.0f;
				_munchie[i]->_rect->Y = -100.0f;
			}
		}
		UpdateMunchie(elapsedTime);
		UpdateCherry(elapsedTime);
		if (CollisonCheck(_pacman->position->X, _pacman->position->Y, _pacman->sourceRect->Width, _pacman->sourceRect->Height, _cherry->_rect->X, _cherry->_rect->Y, _cherry->_rect->Width, _cherry->_rect->Height))
		{
			Audio::Play(_eat);
			_pacman->score += 50;
			_cherry->_rect->X = -101.0f;
			_cherry->_rect->Y = -101.0f;
		}
		UpdateGhost(elapsedTime, mouseState);
		CheckGhostCollisions();
	}
}

void Pacman::Draw(int elapsedTime)
{
	// Allows us to easily create a string
	std::stringstream stream;
	//Pacman score and position for debugging purposes
	stream << " Score: " << _pacman->score; //<< " Pacman X: " << _pacman->position->X << " Y: " << _pacman->position->Y; 



	SpriteBatch::BeginDraw(); // Starts Drawing
	//Doesn't draw when dead.
	if (!_pacman->dead)
	{
		SpriteBatch::Draw(_pacman->texture, _pacman->position, _pacman->sourceRect); // Draws Pacman
	}
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		if (_munchie[i]->_frameCount == 0)
		{
			// Draws Red Munchie
			SpriteBatch::Draw(_munchie[i]->_invertedTexture, _munchie[i]->_rect, nullptr, Vector2::Zero, 1.0f, 0.0f, Color::White, SpriteEffect::NONE);
		}
		else
		{
			// Draw Blue Munchie
			SpriteBatch::Draw(_munchie[i]->_blueTexture, _munchie[i]->_rect, nullptr, Vector2::Zero, 1.0f, 0.0f, Color::White, SpriteEffect::NONE);

		}
	}

	//Draws and animates cherry
	if (_cherry->_frameCount == 0)
	{
		SpriteBatch::Draw(_cherry->_texture, _cherry->_rect);
	}
	else
	{
		SpriteBatch::Draw(_cherry->_invTexture, _cherry->_rect);
	}


	//Draws ghost
	SpriteBatch::Draw(_ghosts[0]->texture, _ghosts[0]->position, _ghosts[0]->sourceRect);


	// Draws String
	SpriteBatch::DrawString(stream.str().c_str(), _stringPosition, Color::Green);


	if (_paused)
	{
		std::stringstream menuStream;
		menuStream << "To Start or Pause Press 'P'\nInstructions: WASD and LSHIFT to move pacman.\n2nd player use the mouse\nthe ghost will follow your cursor.";

		SpriteBatch::Draw(_menuBackground, _menuRectangle, nullptr);
		SpriteBatch::DrawString(menuStream.str().c_str(), _menuStringPosition, Color::Red);
	}
	SpriteBatch::EndDraw();// Ends Drawing

	if (_pacman->dead || _pacman->score == MUNCHIECOUNT * 10 + 50)
	{
		SpriteBatch::BeginDraw();
		//Adds an end screen, specific to the player who won then exits game.
		std::stringstream endStream;
		if (_pacman->dead)
			endStream << "Game Over. Congratz Ghost Win!";
		else
			endStream << "Game Over. Congrats Pacman Win!";

		SpriteBatch::Draw(_endBackground, _endRectangle, nullptr);
		SpriteBatch::DrawString(endStream.str().c_str(), _endStringPosition, Color::Red);
		SpriteBatch::EndDraw();
		Sleep(5000);
		exit(0);
	}


}

void Pacman::CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey)
{
	//Un/pausing the game
	if (state->IsKeyDown(Input::Keys::P) && !_pKeyDown)
	{
		_pKeyDown = true;
		_paused = !_paused;
	}
	if (state->IsKeyUp(Input::Keys::P))
		_pKeyDown = false;
}

void Pacman::CheckViewportCollision()
{
	//If goes off screen in x direction it teleports to the other side
	if (_pacman->position->X > Graphics::GetViewportWidth())
	{
		//Resets pacman position if hits the right wall
		_pacman->position->X = 0;
	}
	else if (_pacman->position->X + _pacman->sourceRect->Width < 0)
	{
		_pacman->position->X = Graphics::GetViewportWidth() - _pacman->sourceRect->Width;
	}
	//Will stop at edges in Y direction
	if (_pacman->position->Y + _pacman->sourceRect->Height > Graphics::GetViewportHeight())
	{
		_pacman->position->Y = Graphics::GetViewportHeight() - _pacman->sourceRect->Height;
	}
	else if (_pacman->position->Y < 0)
	{
		_pacman->position->Y = 0;
	}
}

void Pacman::Input(int elapsedTime, Input::KeyboardState* state, Input::MouseState* mouseState)
{
	if (!_paused)
	{
		float pacmanSpeed = _pacman->speedMultiplier * elapsedTime;
		if (state->IsKeyDown(Input::Keys::ESCAPE))
		{
			exit(0);
		}
		if (state->IsKeyDown(Input::Keys::LEFTSHIFT))
		{
			_pacman->speedMultiplier = 0.2f;
		}
		else
		{
			_pacman->speedMultiplier = 0.1f;
		}
		/// <summary> Checks if any of the WASD keys are pressed and does movement </summary>
		if (state->IsKeyDown(Input::Keys::D))
		{
			_pacman->position->X += pacmanSpeed; //Moves Pacman across X axis to the right
			_pacman->direction = 0;
		}
		else if (state->IsKeyDown(Input::Keys::A))
		{
			_pacman->position->X -= pacmanSpeed;
			_pacman->direction = 2;
		}
		else if (state->IsKeyDown(Input::Keys::S))
		{
			_pacman->position->Y += pacmanSpeed;
			_pacman->direction = 1;
		}
		else if (state->IsKeyDown(Input::Keys::W))
		{
			_pacman->position->Y -= pacmanSpeed;
			_pacman->direction = 3;
		}
	}
}

void Pacman::UpdatePacman(int elapsedTime)
{
	_pacman->currentFrameTime += elapsedTime;
	if (_pacman->currentFrameTime > _cPacmanFrameTime)
	{
		_pacman->frame++;
		if (_pacman->frame >= 2)
			_pacman->frame = 0;
		_pacman->currentFrameTime = 0;

	}

	//Updates the image direction
	_pacman->sourceRect->Y = _pacman->sourceRect->Height * _pacman->direction;
	_pacman->sourceRect->X = _pacman->sourceRect->Width * _pacman->frame;

}

void Pacman::UpdateMunchie(int elapsedTime)
{
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		_munchie[i]->_currentFrameTime += elapsedTime;
		if (_munchie[i]->_currentFrameTime > _munchie[i]->_frameTime)
		{
			_munchie[i]->_frameCount++;
			if (_munchie[i]->_frameCount >= 2)
			{
				_munchie[i]->_frameCount = 0;
			}
			_munchie[i]->_currentFrameTime = 0;
		}
	}
}

void Pacman::UpdateCherry(int elapsedTime)
{
	_cherry->_currentFrameTime += elapsedTime;
	if (_cherry->_currentFrameTime > 500)
	{
		_cherry->_frameCount++;
		if (_cherry->_frameCount >= 2)
		{
			_cherry->_frameCount = 0;
		}
		_cherry->_currentFrameTime = 0;
	}
}

bool Pacman::CollisonCheck(int x1, int y1, int width1, int height1, int x2, int y2, int width2, int height2)
{
	int left1 = x1;
	int left2 = x2;
	int right1 = x1 + width1;
	int right2 = x2 + width2;
	int top1 = y1;
	int top2 = y2;
	int bottom1 = y1 + height1;
	int bottom2 = y2 + height2;

	if (bottom1 < top2)
		return false;
	else if (top1 > bottom2)
		return false;
	else if (right1 < left2)
		return false;
	else if (left1 > right2)
		return false;

	return true;
}

void Pacman::UpdateGhost(int elapsedTime, Input::MouseState* mouseState) //MovingEnemy* ghost, 
{
	///<summary> Makes it multiplayer by one person playing as a ghost as it follows the mouse </summary>
	if (mouseState->X > _ghosts[0]->position->X)
	{
		_ghosts[0]->position->X += _ghosts[0]->speed * elapsedTime;
	}
	if (mouseState->X < _ghosts[0]->position->X)
	{
		_ghosts[0]->position->X -= _ghosts[0]->speed * elapsedTime;
	}

	if (mouseState->Y > _ghosts[0]->position->Y)
	{
		_ghosts[0]->position->Y += _ghosts[0]->speed * elapsedTime;
	}
	if (mouseState->Y < _ghosts[0]->position->Y)
	{
		_ghosts[0]->position->Y -= _ghosts[0]->speed * elapsedTime;
	}
}

void Pacman::CheckGhostCollisions()
{
	int i = 0;
	int bottom1 = _pacman->position->Y + _pacman->sourceRect->Height;
	int bottom2 = 0;
	int left1 = _pacman->position->X;
	int left2 = 0;
	int right1 = _pacman->position->X + _pacman->sourceRect->Width;
	int right2 = 0;
	int top1 = _pacman->position->Y;
	int top2 = 0;

	for (i = 0; i < GHOSTCOUNT; i++)
	{
		//Populating variables with ghost data.
		bottom2 = _ghosts[i]->position->Y + _ghosts[i]->sourceRect->Height;
		left2 = _ghosts[i]->position->X;
		right2 = _ghosts[i]->position->X + _ghosts[i]->sourceRect->Width;
		top2 = _ghosts[i]->position->Y;
		if ((bottom1 >= top2) && (top1 <= bottom2) && (right1 >= left2) && (left1 <= right2))
		{
			_pacman->dead = true;
			i = GHOSTCOUNT;
		}
	}
}