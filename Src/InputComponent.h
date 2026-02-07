// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "MoveComponent.h"
#include <cstdint>

class InputComponent : public MoveComponent
{
	public:
		InputComponent(class Actor* owner);

		void ProcessInput(const uint8_t* keyState) override;

		// Getters/setters
		float GetMaxAngular() const { return mMaxAngularSpeed;}
		int GetClockwiseKey() const { return mClockwiseKey;}
		int GetCounterClockwiseKey() const { return mCounterClockwiseKey;}
		void SetMaxAngularSpeed(float speed) { mMaxAngularSpeed = speed;}
		void SetClockwiseKey(int key) { mClockwiseKey = key;}
		void SetCounterClockwiseKey(int key) { mCounterClockwiseKey = key;}
	private:
		float mMaxAngularSpeed;
		int mClockwiseKey;
		int mCounterClockwiseKey;
};