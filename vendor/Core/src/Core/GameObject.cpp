#include "Core/GameObject.h"
#include "Core/Scene.h"
#include "Core/Group.h"

//--------------------------------------------------------------------------------
sf::FloatRect Sprite::GetGlobalBounds() const
{
	return GetTransform().transformRect(GetGlobalBoundsInternal());
}

//--------------------------------------------------------------------------------
sf::FloatRect Sprite::GetLocalBounds() const 
{ 
	return GetLocalBoundsInternal(); 
}

//--------------------------------------------------------------------------------
sf::Vector2f Sprite::GetCenter() const
{
	sf::FloatRect globalBounds = GetGlobalBounds();
	return sf::Vector2f(globalBounds.left + globalBounds.width / 2.f,
						globalBounds.top + globalBounds.height / 2.f);
}

//--------------------------------------------------------------------------------
void Sprite::Move(const sf::Vector2f& offset) 
{ 
	mPosition.x += offset.x; 
	mPosition.y += offset.y; 
}

//--------------------------------------------------------------------------------
void Sprite::draw(sf::RenderTarget& target, const sf::RenderStates& states) const
{
	sf::RenderStates statesCopy(states);
	statesCopy.transform *= GetTransform();
	target.draw(GetDrawable(), statesCopy);
}

//--------------------------------------------------------------------------------
const sf::Transform& Sprite::GetTransform() const
{
	mTransform = sf::Transform::Identity;
	mTransform.translate(mPosition);

	sf::FloatRect localBounds = GetLocalBoundsInternal();
	sf::Vector2f spriteOrigin = sf::Vector2f(
		mOrigin.x * localBounds.width,
		mOrigin.y * localBounds.height
	);
	mTransform.translate(-spriteOrigin);

	return mTransform;
}

//--------------------------------------------------------------------------------
bool GameObject::IsMarkedForRemoval()
{
	return mScene->IsGameObjectAlive(this);
}

//--------------------------------------------------------------------------------
void GameObject::Kill()
{
	mScene->DeleteGameObject(this);
}

//--------------------------------------------------------------------------------
void GameObject::RemoveFromGroups()
{
	for (Group* group : mGroups)
	{
		group->Remove(this);
	}
}