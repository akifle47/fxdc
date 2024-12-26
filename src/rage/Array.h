#pragma once
#include "Utils.h"
#include <cstdint>
#include <cassert>

namespace rage
{
    template <typename T, typename CounterT = uint16_t>
    class atArray
    {
    public:
        atArray() : mElements(nullptr), mCount(0), mCapacity(0)
        {}

        atArray(CounterT capacity)
        {
            mElements = new T[capacity];
            memset(mElements, 0, capacity);
            mCount = 0;
            mCapacity = capacity;
        }

        atArray(const atArray<T, CounterT>& that)
        {
            mCount = that.mCount;
            mCapacity = that.mCapacity;
            mElements = new T[mCapacity];

            for(CounterT i = 0; i < mCount; i++)
            {
                mElements[i] = that.mElements[i];
            }
        }

        ~atArray()
        {
            Clear();
        }

        atArray<T, CounterT>& operator=(const atArray<T, CounterT>& that)
        {
            if(this == &that)
            {
                return *this;
            }

            Clear();

            mCount = that.mCount;
            mCapacity = that.mCapacity;
            mElements = new T[mCapacity];

            for(CounterT i = 0; i < mCount; i++)
            {
                mElements[i] = that.mElements[i];
            }

            return *this;
        }

        void Clear()
        {
            if(mElements)
            {
                delete[] mElements;
                mElements = nullptr;
            }
        }

        T& operator[](CounterT index)
        {
            return mElements[index];
        }

        const T& operator[](CounterT index) const
        {
            return mElements[index];
        }

        T& Front()
        {
            assert(mCount > 0);
            return mElements[0];
        }

        T& Back()
        {
            assert(mCount > 0);
            return mElements[mCount - 1];
        }

        T& Insert(CounterT index)
        {
            if(mCount == mCapacity)
                Grow();

            for(CounterT i = mCount; i > index; i--)
            {
                mElements[i] = mElements[i - 1];
            }

            mCount++;
            return mElements[index];
        }

        T& Append()
        {
            assert(mCount <= mCapacity);
            return mElements[mCount++];
        }

        T& Grow(CounterT allocStep = 16)
        {
            if(mCount == mCapacity)
            {
                mCapacity = CounterT(mCapacity + allocStep);

                T* newElements = new T[mCapacity];
                for(CounterT i = 0; i < mCount; i++)
                {
                    newElements[i] = mElements[i];
                }

                if(mElements)
                    delete[] mElements;

                mElements = newElements;
            }

            return mElements[mCount++];
        }

        CounterT GetCount() const
        {
            return mCount;
        }

        CounterT GetCapacity() const
        {
            return mCapacity;
        }

        T* begin()
        {
            return &mElements[0];
        }

        T* end()
        {
            return &mElements[mCount];
        }

        const T* begin() const
        {
            return &mElements[0];
        }

        const T* end() const
        {
            return &mElements[mCount];
        }

    private:
        T* mElements;
        CounterT mCount;
        CounterT mCapacity;
    };
    ASSERT_SIZE(atArray<uint32_t>, 0x8);
}