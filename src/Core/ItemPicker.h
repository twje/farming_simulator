#pragma once

#include <vector>
#include <initializer_list>
#include <cassert>

template<typename T>
class ItemPicker
{
public:
	ItemPicker(std::initializer_list<T> initList)
	{
		assert(initList.size() > 0);
		vectorList.reserve(initList.size());
		for (const T& element : initList) {
			vectorList.push_back(element);
		}
	}
	void Next() { mIndex = (mIndex + 1) % vectorList.size(); }
	
	const T& GetItem() const { return vectorList[mIndex]; }
	T& GetItem() { return vectorList[mIndex]; }

private:
	std::vector<T> vectorList;
	size_t mIndex{0};
};
