#pragma once

#include <vector>

#include <SFML/Graphics.hpp>

class Transformable : private sf::Transformable
{
public:
	// Getters
	const sf::Transform& GetTransform() const { return getTransform(); }
	const sf::Vector2f& GetPosition() const { return getPosition(); }

	// Setters
	void SetPosition(const sf::Vector2f& position) { setPosition(position); }
	void SetOrigin(const sf::Vector2f& origin) { setOrigin(origin); }
	void SetRotation(float value) { setRotation(sf::degrees(value)); }

	void Move(const sf::Vector2f& offset) { move(offset); }
	void MoveX(float value) { move(sf::Vector2f(value, 0)); }
	void MoveY(float value) { move(sf::Vector2f(0, value)); }
};

class GameObject : public Transformable
{
	friend class Scene;
	friend class Group;

public:
	// Hooks
	virtual void Update(const sf::Time& timestamp) { };
	virtual void Draw(sf::RenderWindow& window) { }

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
	// Hooks
	virtual sf::FloatRect GetLocalBounds() const { return sf::FloatRect(); }

	// Getters
	sf::FloatRect GetGlobalBounds() const { return GetTransform().transformRect(GetLocalBounds()); }
};

//// Custom hash function
//namespace std
//{
//	template<>
//	struct hash<GameObject*>
//	{
//		size_t operator()(const GameObject* obj) const
//		{
//			return std::hash<const GameObject*>{}(obj);
//		}
//	};
//}