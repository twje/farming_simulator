#include <iterator>
#include <functional>
#include <vector>
#include <iostream>

template<typename T>
class ConditionalIterator
{
public:
    ConditionalIterator(typename std::vector<T>::iterator start, typename std::vector<T>::iterator end, std::function<bool(T)> condition)
        : mCurrent(start)
        , mEnd(end)
        , mCondition(condition)
    {
        // Ensure we start at a valid position
        if (mCurrent != mEnd && !mCondition(*mCurrent)) {
            ++(*this);
        }
    }

    ConditionalIterator<T> begin() const
    {
        return ConditionalIterator<T>(mCurrent, mEnd, mCondition);
    }

    ConditionalIterator<T> end() const
    {
        return ConditionalIterator<T>(mCurrent, mEnd, mCondition);
    }

    ConditionalIterator& operator++()
    {
        do {
            ++mCurrent;
        } while (mCurrent != mEnd && !mCondition(*mCurrent));

        return *this;
    }

    T operator*() const { return *mCurrent; }

    bool operator!=(const ConditionalIterator& other) const
    {
        return mCurrent != other.mCurrent;
    }

private:
    typename std::vector<T>::iterator mCurrent;
    typename std::vector<T>::iterator mEnd;
    std::function<bool(T)> mCondition;
};