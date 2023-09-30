#pragma once

#include <vector>
#include <algorithm>

#include "Core/GameObject.h"

class Group
{
public:
    void Add(GameObject* gameObject);
    void Remove(GameObject* gameObject);

    std::vector<GameObject*>::iterator begin() { return mGameObjects.begin(); }
    std::vector<GameObject*>::iterator end(){ return mGameObjects.end(); }

public:
	std::vector<GameObject*> mGameObjects;
};
