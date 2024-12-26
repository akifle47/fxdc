#pragma once
#include <cstring>
#include <cstdint>

class CString
{
public:
    CString()
    {
        mString = nullptr;
    }

    CString(uint32_t size)
    {
        mString = new char[size];
        memset(mString, 0, size);
    }

    CString(const char* str, uint32_t extraSize)
    {
        mString = new char[strlen(str) + extraSize];
        strcpy(mString, str);
    }

    CString(const CString& that, uint32_t extraSize)
    {
        mString = new char[strlen(that.Get()) + extraSize];
        strcpy(mString, that.Get());
    }

    CString(const char* str)
    {
        mString = strdup(str);
    }

    CString(const char* str1, const char* str2)
    {
        mString = new char[strlen(str1) + strlen(str2) + 1];
        strcpy(mString, str1);
        strcat(mString, str2);
    }

    CString(const CString& str1, const char* str2)
    {
        mString = new char[strlen(str1.Get()) + strlen(str2) + 1];
        strcpy(mString, str1.Get());
        strcat(mString, str2);
    }

    CString(const char* str1, const CString& str2)
    {
        mString = new char[strlen(str1) + strlen(str2.Get()) + 1];
        strcpy(mString, str1);
        strcat(mString, str2.Get());
    }

    CString(const CString& that)
    {
        mString = strdup(that.mString);
    }

    ~CString()
    {
        if(mString)
        {
            delete[] mString;
            mString = nullptr;
        }
    }

    CString& operator=(const CString& that)
    {
        if(this == &that)
        {
            return *this;
        }

        if(mString)
        {
            delete[] mString;
            mString = nullptr;
        }

        if(that.mString)
        {
            mString = strdup(that.mString);
        }

        return *this;
    }

    CString& operator=(const char* str)
    {
        if(mString)
        {
            delete[] mString;
            mString = nullptr;
        }

        if(str)
        {
            mString = strdup(str);
        }

        return *this;
    }

    uint32_t Length() const
    {
        if(!mString)
            return 0;
        else
            return strlen(mString);
    }

    bool operator==(const CString& rhs) const
    {
        return strcmp(mString, rhs.mString) == 0;
    }

    bool operator==(const char* rhs) const
    {
        return strcmp(mString, rhs) == 0;
    }

    char& operator[](uint32_t index)
    {
        return mString[index];
    }

    char& At(uint32_t index)
    {
        return mString[index];
    }

    char* Get() const
    {
        return mString;
    }

private:
    char* mString;
};