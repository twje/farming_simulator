#include "Core/Group.h"

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