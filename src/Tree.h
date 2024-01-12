#pragma once

#include <SFML/Graphics.hpp>

#include "Core/GameObject.h"
#include "Core/Tiled/TiledMap.h"
#include "Core/RectUtils.h"
#include "Core/Texture.h"
#include "Core/Utils.h"
#include "Core/Scene.h"
#include "Core/ResourceLocator.h"

#include "Settings.h"
#include "Sprites.h"


// --------------------------------------------------------------------------------
class ITreeObserver
{
public:
	virtual void AddItem(const std::string& item) { }
};

//------------------------------------------------------------------------------
class TreeSubject
{
public:
	void Subscribe(ITreeObserver* observer) { mObservers.emplace_back(observer); }

	void AddItem(const std::string& item)
	{
		for (auto& observer : mObservers)
		{
			observer->AddItem(item);
		}
	}

private:
	std::vector<ITreeObserver*> mObservers;
};

//------------------------------------------------------------------------------
class Tree : public TiledMapObjectSprite, public TreeSubject
{
public:
	Tree(const TiledMapObjectDefinition& definition, Group& spriteGroup, uint16_t depth = LAYERS.at("main"))
		: TiledMapObjectSprite(definition, depth)
		, mName(definition.GetName())
		, mSpriteGroup(spriteGroup)
		, mAlive(true)
		, mHealth(5)
	{
		SetOrigin(definition.GetOrigin());
	}

	virtual void SetUp(Scene& scene) override
	{
		mAppleGroup = scene.CreateGroup();
		CreateFruit();
	}

	virtual void Update(const sf::Time& timestamp) override
	{
		if (mAlive)
		{
			CheckDeath();
		}
	}

	void ChopWood()
	{
		if (mAlive)
		{
			mHealth -= 1;
		}
		PickApple();
	}

private:
	void CreateFruit()
	{
		sf::FloatRect bounds = GetGlobalBounds();
		const sf::Vector2f position = sf::Vector2f(bounds.left, bounds.top);

		AssetManager& assetManager = ResourceLocator::GetInstance().GetAssetManager();
		for (const sf::Vector2f& positionOffset : APPLE_POSITIONS.at(mName))
		{
			if (IsRandomNumberLessThanOrEqualTo(0, 10, 2))
			{
				const sf::Texture& texture = assetManager.GetAsset<Texture>("apple").GetRawTexture();
				const sf::IntRect textureRegion(sf::Vector2i(), sf::Vector2i(texture.getSize()));

				Generic* apple = GetScene().CreateGameObject<Generic>(texture,
					textureRegion,
					sf::Vector2f(),
					position + positionOffset,
					LAYERS.at("fruit"));
				mSpriteGroup.Add(apple);
				mAppleGroup->Add(apple);
			}
		}
	}

	void CheckDeath()
	{
		if (mHealth <= 0)
		{
			ReplaceTreeWithStump();
			mAlive = false;
			AddItem("wood");
		}
	}

	void ReplaceTreeWithStump()
	{
		AssetManager& assetManager = ResourceLocator::GetInstance().GetAssetManager();

		CreateSilhouetteFlash(static_cast<Generic*>(this), LAYERS.at("fruit"), 200);

		static const std::unordered_map<std::string, std::string> textureMap =
		{
			{"Small", "small_stump"},
			{"Large", "large_stump"}
		};

		auto it = textureMap.find(mName);
		if (it == textureMap.end())
		{
			throw std::logic_error("Invalid mName value: " + mName);
		}

		sf::FloatRect oldBounds = GetGlobalBounds();
		sf::Texture* texture = &assetManager.GetAsset<Texture>(it->second).GetRawTexture();

		// Update sprite texture
		SetTexture(*texture, { sf::Vector2i(), sf::Vector2i(texture->getSize()) });
		SetOrigin({ 0.5f, 1.0f });
		SetPosition({ oldBounds.left + oldBounds.width / 2.0f, GetPosition().y });  // Tiled map origin is BL

		// Update hitbox
		sf::FloatRect newBounds = GetGlobalBounds();
		SetHitbox(InflateRect(newBounds, -10.0f, -newBounds.height * 0.6f));


		KillAllApples();
		mAlive = false;
	}

	void PickApple()
	{
		GameObject* apple = mAppleGroup->GetRandomGameObject();
		if (apple)
		{
			CreateSilhouetteFlash(static_cast<Generic*>(apple), LAYERS.at("fruit"), 200);
			apple->Kill();
			AddItem("apple");
		}
	}

	void KillAllApples()
	{
		for (GameObject* apple : *mAppleGroup)
		{
			if (!apple->IsMarkedForRemoval())
			{
				apple->Kill();
			}
		}
	}

	void CreateSilhouetteFlash(const Generic* source, uint16_t depth, int32_t msDuration)
	{
		const sf::FloatRect& bounds = source->GetGlobalBounds();
		const sf::Texture& texture = source->GetSprite().getTexture();
		const sf::IntRect textureRegion(sf::Vector2i(), sf::Vector2i(texture.getSize()));

		Particle* particle = GetScene().CreateGameObject<Particle>(texture,
			textureRegion,
			sf::Vector2f(),
			sf::Vector2f(bounds.left, bounds.top),
			depth,
			msDuration);
		mSpriteGroup.Add(particle);
	}

	std::string mName;
	int32_t mHealth;
	bool mAlive;
	Group* mAppleGroup;
	Group& mSpriteGroup;
};