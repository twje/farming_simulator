#pragma once

// Includes
//------------------------------------------------------------------------------
#include <vector>
#include <memory>

//------------------------------------------------------------------------------
template <typename T>
class ObjectPool
{
public:
	std::unique_ptr<T> acquire()
	{
		if (pool.empty())
		{
			return std::make_unique<T>();
		}

		auto obj = std::move(pool.back());
		pool.pop_back();
		return obj;
	}

	void release(std::unique_ptr<T> obj)
	{
		pool.push_back(std::move(obj));
	}

private:
	std::vector<std::unique_ptr<T>> pool;
};