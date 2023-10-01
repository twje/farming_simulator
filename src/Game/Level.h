#pragma once

#include "Core/Scene.h"
#include "Core/Group.h"
#include "Player.h"
#include "Overlay.h"

class Level : public Scene
{
public:
	void Create() override
	{
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
		for (GameObject* gameObject : mAllSprites)
		{
			if (!gameObject->IsMarkedForRemoval()) { continue; }

			gameObject->Draw(window);
		}
		mOverlay->Draw(window);
	}

private:
	Player* mPlayer;
	Group mAllSprites;
	std::unique_ptr<Overlay> mOverlay;
};