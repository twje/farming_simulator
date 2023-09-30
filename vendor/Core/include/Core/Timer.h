#pragma once

#include <functional>

#include <SFML/Graphics.hpp>

class Timer
{
public:
	Timer() = default;

	Timer(const sf::Time& duration)
		: mDuration(duration)
	{ }

	template <typename Callable>
	Timer(const sf::Time& duration, Callable&& callable)
		: mDuration(duration),
		  mCallback(std::forward<Callable>(callable))
	{ }

	void Update(const sf::Time& timestamp) 
	{ 
		if (mActive)
		{
			mElapsedTime += timestamp.asSeconds();
			if (IsFinished())
			{
				if (mCallback) { mCallback(); }
				mActive = false;
			}
		}
	}

	void Reset() { mElapsedTime = 0; }
	void Start() { mActive = true; }

	// Getters
	bool IsFinished() { return mElapsedTime >= mDuration.asSeconds(); }		
	bool IsActive() { return mActive; }

	// Setters
	void SetDuration(const sf::Time& duration) { mDuration = duration; }
	
	template <typename Callable>
	void SetCallback(Callable&& callable) { mCallback = std::forward<Callable>(callable); }

private:
	float mElapsedTime{ 0 };
	sf::Time mDuration;
	bool mActive{ false };
	std::function<void()> mCallback;
};