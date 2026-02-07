// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Rocket.h"
#include "SpriteComponent.h"
#include "MoveComponent.h"
#include "Game.h"
#include "CircleComponent.h"
#include "Asteroid.h"

Rocket::Rocket(Game* game)
:Actor(game)
,mDeathTimer(1.0f)
{
    SpriteComponent* sc = new SpriteComponent(this);
    sc->SetTexture(game->GetTexture("Assets/Rocket01.png"));

    MoveComponent* mc = new MoveComponent(this);
    mc->SetForwardSpeed(800.0f);

    mCircle = new CircleComponent(this);
    mCircle->SetRadius(11.0f);
}

void Rocket::UpdateActor(float deltaTime)
{
    mDeathTimer -= deltaTime;
    Vector2 pos = this->GetPosition();
    if (mDeathTimer <= 0.0f)
    {
        SetState(EDead);
    }
    else if (pos.x < 0.0f || pos.x > 1024.0f || pos.y < 0.0f || pos.y > 768.0f)
    {
        this->SetState(EDead);
    }
    else
    { 
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
}