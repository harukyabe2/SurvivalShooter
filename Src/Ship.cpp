// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Ship.h"
#include "SpriteComponent.h"
#include "InputComponent.h"
#include "Game.h"
#include "Rocket.h"
#include "CircleComponent.h"
#include "Asteroid.h"

Ship::Ship(Game* game)
:Actor(game)
,mRocketCooldown(0.0f)
{
    SpriteComponent* sc = new SpriteComponent(this, 150);
    sc->SetTexture(game->GetTexture("Assets/Ship01.png"));

    InputComponent* ic = new InputComponent(this);
    ic->SetClockwiseKey(SDL_SCANCODE_A);
    ic->SetCounterClockwiseKey(SDL_SCANCODE_D);
    ic->SetMaxAngularSpeed(Math::TwoPi);

    mCircle = new CircleComponent(this);
    mCircle->SetRadius(20.0f);
}

void Ship::UpdateActor(float deltaTime)
{
    mRocketCooldown -= deltaTime;
    for (auto ast : GetGame()->GetAsteroids())
    {
        if (Intersect(*mCircle, *(ast->GetCircle())))
        {
            SetState(EDead);
            ast->SetState(EDead);
            break;
        }
    }
}

void Ship::ActorInput(const uint8_t* keyState)
{
    if (keyState[SDL_SCANCODE_SPACE] && mRocketCooldown <= 0.0f)
    {
        Rocket* rocket = new Rocket(GetGame());
        rocket->SetPosition(GetPosition());
        rocket->SetRotation(GetRotation());

        mRocketCooldown = 0.5f;
    }
}