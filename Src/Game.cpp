// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Game.h"
#include "Actor.h"
#include <algorithm>
#include "Random.h"
#include "SDL2/SDL_image.h"
#include "SpriteComponent.h"
#include "Ship.h"
#include "Asteroid.h"

Game::Game()
:mWindow(nullptr)
,mRenderer(nullptr)
,mIsRunning(true)
,mUpdatingActors(false)
,mSpawnTimer(0.0f)
{

}

bool Game::Initialize()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}

	if (IMG_Init(IMG_INIT_PNG) == 0)
	{
		SDL_Log("Unable to initialize SDL_image: %s", SDL_GetError());
		return false;
	}

	mWindow = SDL_CreateWindow("Game", 100, 100, 1024, 768, 0);
	if (!mWindow)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}

	mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!mRenderer)
	{
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		return false;
	}

	LoadData();

	Random::Init();
	mTicksCount = SDL_GetTicks();
	return true;
}

void Game::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void Game::ProcessInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
				mIsRunning = false;
				break;
		}
	}

	const Uint8* keyState = SDL_GetKeyboardState(NULL);
	if (keyState[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}

	mUpdatingActors = true;
	for (auto actor : mActors)
	{
		actor->ProcessInput(keyState);
	}
	mUpdatingActors = false;
}

void Game::UpdateGame()
{
	// Wait until 16ms has elapsed since last frame
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16));
	// Compute delta time (ms)
	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}
	mTicksCount = SDL_GetTicks();

	// Spawn asteroids
	mSpawnTimer -= deltaTime;
	if (mSpawnTimer <= 0.0f)
	{
		Asteroid* ast = new Asteroid(this);

		Vector2 pos = GetRandomSpawnPosition();
		ast->SetPosition(pos);
		Vector2 dir = mShip->GetPosition() - pos;
		dir.Normalize();
		float angle = Math::Atan2(-dir.y, dir.x);
		ast->SetRotation(angle);

		mSpawnTimer = 1.5f;
	}

	// Update all the actors
	mUpdatingActors = true;
	for (auto actor : mActors)
	{
		actor->Update(deltaTime);
	}
	mUpdatingActors = false;

	// Move pending actors to mActors
	for (auto pending : mPendingActors)
	{
		mActors.emplace_back(pending);
	}
	mPendingActors.clear();

	// Delete dead actors
	std::vector<class Actor*> deadActors;
	for (auto actor : mActors)
	{
		if (actor->GetState() == Actor::EDead)
		{
			deadActors.emplace_back(actor);
		}
	}
	for (auto actor : deadActors)
	{
		delete actor;
	}
}

void Game::GenerateOutput()
{
	SDL_SetRenderDrawColor(mRenderer, 220, 220, 220, 255);
	SDL_RenderClear(mRenderer);

	for (auto sprite : mSprites)
	{
		sprite->Draw(mRenderer);
	}

	SDL_RenderPresent(mRenderer);
}

void Game::Shutdown()
{
	UnloadData();
	IMG_Quit();
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}

void Game::LoadData()
{
	mShip = new Ship(this);
	mShip->SetPosition(Vector2(512.0f, 384.0f));
	mShip->SetRotation(Math::PiOver2);
}

void Game::UnloadData()
{
	while (!mActors.empty())
	{
		delete mActors.back();
	}

	for (auto i : mTextures)
	{
		SDL_DestroyTexture(i.second);
	}
	mTextures.clear();
}

SDL_Texture* Game::GetTexture(const std::string& fileName)
{
	SDL_Texture* tex = nullptr;
	auto iter = mTextures.find(fileName);
	if (iter != mTextures.end())
	{
		tex = iter->second;
	}
	else
	{
		SDL_Surface* surf = IMG_Load(fileName.c_str());
		if (!surf)
		{
			SDL_Log("Failed to load texture file %s", fileName.c_str());
			return nullptr;
		}

		tex = SDL_CreateTextureFromSurface(mRenderer, surf);
		SDL_FreeSurface(surf);
		if (!tex)
		{
			SDL_Log("Failed to convert surface to texture for %s", fileName.c_str());
			return nullptr;
		}
		mTextures.emplace(fileName.c_str(), tex);
	}
	return tex;
}

void Game::AddActor(Actor* actor)
{
	if (mUpdatingActors)
	{
		mPendingActors.emplace_back(actor);
	}
	else
	{
		mActors.emplace_back(actor);
	}
}

void Game::RemoveActor(Actor* actor)
{
	auto iter = std::find(mPendingActors.begin(), mPendingActors.end(), actor);
	if (iter != mPendingActors.end())
	{
		std::iter_swap(iter, mPendingActors.end() - 1);
		mPendingActors.pop_back();
	}

	iter = std::find(mActors.begin(), mActors.end(), actor);
	if (iter != mActors.end())
	{
		std::iter_swap(iter, mActors.end() - 1);
		mActors.pop_back();
	}

}

void Game::AddSprite(SpriteComponent* sprite)
{
	int myDrawOrder = sprite->GetDrawOrder();
	auto iter = mSprites.begin();
	for (; iter != mSprites.end(); ++iter)
	{
		if (myDrawOrder < (*iter)->GetDrawOrder())
		{
			break;
		}
	}
	mSprites.insert(iter, sprite);
}

void Game::RemoveSprite(SpriteComponent* sprite)
{
	auto iter = std::find(mSprites.begin(), mSprites.end(), sprite);
	mSprites.erase(iter);
}

void Game::AddAsteroid(Asteroid* ast)
{
	mAsteroids.emplace_back(ast);
}

void Game::RemoveAsteroid(Asteroid* ast)
{
	auto iter = std::find(mAsteroids.begin(), mAsteroids.end(), ast);
	if (iter != mAsteroids.end())
	{
		mAsteroids.erase(iter);
	}
}

Vector2 Game::GetRandomSpawnPosition()
{
	const float screenW = 1024.0f;
	const float screenH = 768.0f;
	const float offset = 50.0f;

	Vector2 randPos = Vector2::Zero;
	int edge = Random::GetIntRange(0, 3);

	switch (edge)
	{
		case 0:
			randPos.x = Random::GetFloatRange(0.0f, screenW);
			randPos.y = -offset;
			break;
		case 1:
			randPos.x = Random::GetFloatRange(0.0f, screenW);
			randPos.y = screenH + offset;
			break;
		case 2:
			randPos.x = -offset;
			randPos.y = Random::GetFloatRange(0.0f, screenH);
			break;
		case 3:
			randPos.x = screenW + offset;
			randPos.y = Random::GetFloatRange(0.0f, screenH);
			break;
	}
	return randPos;
}