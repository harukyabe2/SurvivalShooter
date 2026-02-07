// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "SDL2/SDL.h"
#include <unordered_map>
#include <string>
#include <vector>
#include "Math.h"

class Game
{
	public:
		Game();
		bool Initialize();
		void RunLoop();
		void Shutdown();

		void AddActor(class Actor* actor);
		void RemoveActor(class Actor* actor);
		void AddSprite(class SpriteComponent* sprite);
		void RemoveSprite(class SpriteComponent* sprite);
		void AddAsteroid(class Asteroid* ast);
		void RemoveAsteroid(class Asteroid* ast);

		std::vector<class Asteroid*>& GetAsteroids() { return mAsteroids;}
		
		SDL_Texture* GetTexture(const std::string& fileName);
	private:
		void ProcessInput();
		void UpdateGame();
		void GenerateOutput();

		void LoadData();
		void UnloadData();

		Vector2 GetRandomSpawnPosition();

		// Map of textures
		std::unordered_map<std::string, SDL_Texture*> mTextures;

		// All the actors
		std::vector<class Actor*> mActors;
		// Pending actors
		std::vector<class Actor*> mPendingActors;

		// All the sprite components
		std::vector<class SpriteComponent*> mSprites;

		class Ship* mShip;
		std::vector<class Asteroid*> mAsteroids;

		SDL_Window* mWindow;
		SDL_Renderer* mRenderer;
		Uint32 mTicksCount;
		bool mIsRunning;
		bool mUpdatingActors;
		float mSpawnTimer;
};