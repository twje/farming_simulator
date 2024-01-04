#include "Core/Group.h"
#include "Core/Utils.h"

void Group::Add(GameObject* gameObject)
{
    mGameObjects.emplace_back(gameObject);
    gameObject->AddGroup(this);
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
        return GetRandomElement(mGameObjects);
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