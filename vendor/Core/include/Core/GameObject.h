#pragma once

#include "Core/Shader.h"

#include <SFML/Graphics.hpp>

#include <vector>

class GameObject : public sf::Drawable
{
	friend class Scene;
	friend class Group;

public:
	// Hooks
	virtual void Update(const sf::Time& timestamp) { };
	virtual uint16_t GetDepth() const { return 0; }

	// Helper methods
	bool IsMarkedForRemoval();
	void Kill();
	void RemoveFromGroups();
	
private:
	void SetScene(Scene* scene) { mScene = scene; }
	void AddGroup(Group* group) { mGroups.emplace_back(group); }

private:
	Scene* mScene;
	std::vector<Group*> mGroups;
};

class Sprite : public GameObject
{
public:
	// Getters
	sf::FloatRect GetGlobalBounds() const;
	sf::FloatRect GetLocalBounds() const;
	sf::Vector2f GetCenter() const;
	const sf::Vector2f& GetPosition() const { return mPosition; }

	// Setters
	void SetPosition(const sf::Vector2f& position) { mPosition = position; }
	void SetOrigin(const sf::Vector2f& origin) { mOrigin = origin; }	
	void SetShader(Shader* shader) { mShader = shader; }

	void Move(const sf::Vector2f& offset);
	void MoveX(float value) { Move(sf::Vector2f(value, 0)); }
	void MoveY(float value) { Move(sf::Vector2f(0, value)); }
	
	virtual sf::FloatRect GetHitbox() const { return { }; }

protected:
	// Hooks
	virtual sf::FloatRect GetLocalBoundsInternal() const = 0;
	virtual sf::FloatRect GetGlobalBoundsInternal() const = 0;
	virtual const sf::Drawable& GetDrawable() const = 0;

private:
	void draw(sf::RenderTarget& target, const sf::RenderStates& states) const override final;
	const sf::Transform& GetTransform() const;

private:
	sf::Vector2f mPosition;		
	sf::Vector2f mOrigin;
	Shader* mShader{ nullptr };
	mutable sf::Transform mTransform;
};