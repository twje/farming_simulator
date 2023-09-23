#include "Core/GameObject.h"
#include "Core/Scene.h"
#include "Core/Group.h"

bool GameObject::IsMarkedForRemoval()
{
	return mScene->IsGameObjectAlive(this);
}

void GameObject::Kill()
{
	mScene->DeleteGameObject(this);
}

void GameObject::RemoveFromGroups()
{
	for (Group* group : mGroups)
	{
		group->Remove(this);
	}
}