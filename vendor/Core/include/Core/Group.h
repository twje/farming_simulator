#pragma once

#include <functional>
#include <vector>
#include <algorithm>

#include "Core/GameObject.h"

class Group
{
public:
    void Add(GameObject* gameObject);
    void Remove(GameObject* gameObject);
    GameObject* GetRandomGameObject();
    size_t GetSize();

    std::vector<GameObject*>::iterator begin() { return mGameObjects.begin(); }
    std::vector<GameObject*>::iterator end(){ return mGameObjects.end(); }
    void Sort(const std::function<bool(const GameObject*, const GameObject*)>& compareFunc);

public:
	std::vector<GameObject*> mGameObjects;
};
