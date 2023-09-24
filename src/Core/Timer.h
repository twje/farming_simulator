#pragma once

#include <SFML/Graphics.hpp>

class Timer
{
public:
	Timer() = default;

	Timer(float seconds)
		: mDuration(seconds)		  
	{ }

	void Update(const sf::Time& timestamp) { mElapsedTime += timestamp.asSeconds(); }
	bool IsFinished() { return mElapsedTime >= mDuration; }
	void Reset() { mElapsedTime = 0; }
	void Reset(float seconds) 
	{ 
		mElapsedTime = 0;
		mDuration = seconds;
	}

private:
	float mElapsedTime{ 0 };
	float mDuration{ 0 };
};