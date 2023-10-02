#pragma once

#include <vector>

#include <SFML/Graphics.hpp>

class GameObject: public sf::Drawable
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

class Sprite : public GameObject, private sf::Transformable
{
public:
	// Hooks
	virtual sf::FloatRect GetLocalBounds() const { return sf::FloatRect(); }
	virtual void Draw(sf::RenderTarget& target, const sf::RenderStates& states) const { }

	// Getters
	sf::Vector2f GetCenter() const { return getPosition() + 0.5f * GetGlobalBounds().getSize(); }
	sf::FloatRect GetGlobalBounds() const { return GetTransform().transformRect(GetLocalBounds()); }
	const sf::Transform& GetTransform() const { return getTransform(); }
	const sf::Vector2f& GetPosition() const { return getPosition(); }

	// Setters
	void SetPosition(const sf::Vector2f& position) { setPosition(position); }
	void SetOrigin(const sf::Vector2f& origin) { setOrigin(origin); }
	void SetRotation(float value) { setRotation(sf::degrees(value)); }

	void Move(const sf::Vector2f& offset) { move(offset); }
	void MoveX(float value) { move(sf::Vector2f(value, 0)); }
	void MoveY(float value) { move(sf::Vector2f(0, value)); }

private:
	void draw(sf::RenderTarget& target, const sf::RenderStates& states) const override final
	{
		sf::RenderStates statesCopy(states);
		statesCopy.transform *= getTransform();
		Draw(target, statesCopy);
	}
};