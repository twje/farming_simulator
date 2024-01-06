#pragma once

#include "Core/GameObject.h"
#include "Core/ConditionalIterable.h"

#include <functional>
#include <vector>
#include <algorithm>

class Group
{
public:
    void Update();
    void Add(GameObject* gameObject);
    void Remove(GameObject* gameObject);
    void Sort(const std::function<bool(const GameObject*, const GameObject*)>& compareFunc);
    
    GameObject* GetRandomGameObject();
    size_t GetSize();
    
    ConditionalIterator<GameObject*> begin();
    ConditionalIterator<GameObject*> end();

private:
    static bool IsMarkedForRemoval(GameObject* gameObject);

	std::vector<GameObject*> mGameObjects;
    std::vector<GameObject*> mPostFrameAddGameObjectList;
};
