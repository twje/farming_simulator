#pragma once

#include "Core/Scene.h"
#include "Core/Group.h"
#include "Player.h"
#include "Overlay.h"

#include <iostream>
class Level : public Scene
{
public:
	void Create() override
	{
		const ApplicationConfig& config = GetResourceLocator().GetApplicationConfig();
		sf::Vector2f windowSize = sf::Vector2f(config.GetWindowSize());

		mWorldView.setSize(windowSize);
		mWorldView.setCenter(windowSize * 0.5f);

		mHUDView.setSize(windowSize);
		mHUDView.setCenter(windowSize * 0.5f);

		mPlayer = CreateGameObject<Player>(GetResourceLocator(), sf::Vector2f(0, 0));

		mAllSprites.Add(mPlayer);

		mOverlay = std::make_unique<Overlay>(GetResourceLocator(), *mPlayer);
	}

	void Update(const sf::Time& timestamp) override
	{
		for (GameObject* gameObject : mAllSprites)
		{
			if (!gameObject->IsMarkedForRemoval()) { continue; }

			gameObject->Update(timestamp);
		}
	}

	virtual void Draw(sf::RenderWindow& window)
	{
		mWorldView.setCenter(mPlayer->GetCenter());
		window.setView(mWorldView);

		for (GameObject* gameObject : mAllSprites)
		{
			if (!gameObject->IsMarkedForRemoval()) { continue; }

			window.draw(*gameObject);
		}

		window.setView(mHUDView);
		mOverlay->Draw(window);
	}

	virtual void OnWindowResize(const sf::Vector2u& size)
	{
		mWorldView.setSize(sf::Vector2f(size));
		mHUDView.setSize(sf::Vector2f(size));
	}

private:
	Player* mPlayer;
	Group mAllSprites;
	std::unique_ptr<Overlay> mOverlay;
	sf::View mWorldView;
	sf::View mHUDView;
};