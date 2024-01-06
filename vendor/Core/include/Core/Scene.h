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
		gameObject->SetUp(*this);

		T* ptr = gameObject.get();
		mGameObjects[ptr] = std::move(gameObject);
		return ptr;
	}

	Group* CreateGroup()
	{
		mGroups.emplace_back(std::make_unique<Group>());
		return mGroups.back().get();
	}

	void DeleteGameObject(GameObject* gameObject)
	{
		mDeadGameObjectList.insert(gameObject);
	}

	bool IsGameObjectAlive(GameObject* gameObject)
	{
		// The object has been deletd but still iterated on
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
	
		for (auto& groupPtr : mGroups)
		{
			groupPtr->Update();
		}
	}

private:
	std::unordered_map<void*, std::unique_ptr<GameObject>> mGameObjects;
	std::set<GameObject*> mDeadGameObjectList;
	std::vector<std::unique_ptr<Group>> mGroups;
};