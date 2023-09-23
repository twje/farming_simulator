#pragma once

#include <memory>
#include <set>

#include "Core/ILayer.h"
#include "Core/GameObject.h"
#include "Core/Group.h"

class Scene : public ILayer
{
public:
	template<typename T, typename... Args>
	T* CreateGameObject(Args&&... args)
	{
		auto gameObject = std::make_unique<T>(std::forward<Args>(args)...);
		gameObject->SetScene(this);
		
		T* ptr = gameObject.get();
		mGameObjects[ptr] = std::move(gameObject);
		return ptr;
	}
	
	void DeleteGameObject(GameObject* gameObject)
	{		
		mDeadGameObjectList.insert(gameObject);
	}

	bool IsGameObjectAlive(GameObject* gameObject)
	{
		return mDeadGameObjectList.find(gameObject) == mDeadGameObjectList.end();
	}

	// ILayer Interface
	void EndFrame() override
	{
		for (auto gameObject : mDeadGameObjectList)
		{
			gameObject->RemoveFromGroups();
			mGameObjects.erase(gameObject);
		}
		mDeadGameObjectList.clear();
	}

private:
	std::unordered_map<void*, std::unique_ptr<GameObject>> mGameObjects;
	std::set<GameObject*> mDeadGameObjectList;
};