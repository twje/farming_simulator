#include "Core/Group.h"
#include "Core/Utils.h"

void Group::Update()
{
    for (GameObject* gameObject : mPostFrameAddGameObjectList)
    {
        mGameObjects.push_back(gameObject);
        gameObject->AddGroup(this);
    }
    mPostFrameAddGameObjectList.clear();
}

void Group::Add(GameObject* gameObject)
{
    mPostFrameAddGameObjectList.push_back(gameObject);    
}

void Group::Remove(GameObject* gameObject)
{
    mGameObjects.erase(
        std::remove_if(
            mGameObjects.begin(),
            mGameObjects.end(),
            [gameObject](GameObject* obj) {
                return obj == gameObject;
            }
        ),
        mGameObjects.end()
    );
}

GameObject* Group::GetRandomGameObject()
{
    if (mGameObjects.size() > 0)
    {        
        size_t attempts = 0;
        while (attempts < 10)
        {
            GameObject* gameObject = GetRandomElement(mGameObjects);
            if (!gameObject->IsMarkedForRemoval())
            {
                return gameObject;
            }
            attempts++;
        }
    }
    return nullptr;
}

size_t Group::GetSize()
{
    return mGameObjects.size();
}

void Group::Sort(const std::function<bool(const GameObject*, const GameObject*)>& compareFunc)
{
    std::sort(mGameObjects.begin(), mGameObjects.end(), compareFunc);
}

ConditionalIterator<GameObject*> Group::begin()
{
    return ConditionalIterator<GameObject*>(mGameObjects.begin(), mGameObjects.end(), IsMarkedForRemoval);
}

ConditionalIterator<GameObject*> Group::end()
{
    return ConditionalIterator<GameObject*>(mGameObjects.end(), mGameObjects.end(), IsMarkedForRemoval);
}

bool Group::IsMarkedForRemoval(GameObject* gameObject)
{
    return !gameObject->IsMarkedForRemoval();
}