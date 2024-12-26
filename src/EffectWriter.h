#pragma once
#include "Log.h"

#include <fstream>
#include <filesystem>
#include <vector>

class EffectWriter
{
public:
    EffectWriter(const char* filePath) : mTabCount(0), mTempString(MAX_STRLEN)
    {
        std::filesystem::path absFilePath = std::filesystem::absolute(filePath).string().c_str();
        absFilePath.make_preferred();

        mFile = std::ofstream(absFilePath);
        if(!mFile.good() || !mFile.is_open())
        {
            Log::Error("Unable to open file \"%s\"", absFilePath.string().c_str());
        }
    }

    ~EffectWriter() = default;
    
    bool IsOpen()
    {
        return mFile.good() && mFile.is_open();
    }

    inline void PushTab()
    {
        ++mTabCount;
    }

    inline void PopTab()
    {
        mTabCount -= mTabCount == 0 ? 0 : 1;
    }

    inline void NewLine()
    {
        mFile << '\n';
    }

    inline void NewLineIndented()
    {
        mFile << '\n';
        WriteTabs();
    }

    template<typename ...Args>
    inline void Write(const char* fmt, Args ...args)
    {
        sprintf(mTempString.data(), fmt, args...);
        mFile << mTempString.data();
    }

    template<typename ...Args>
    inline void WriteIndented(const char* fmt, Args ...args)
    {
        sprintf(mTempString.data(), fmt, args...);
        WriteTabs();
        mFile << mTempString.data();
    }

    template<typename ...Args>
    inline void WriteLine(const char* fmt, Args ...args)
    {
        sprintf(mTempString.data(), fmt, args...);
        mFile << mTempString.data() << "\n";
    }

    template<typename ...Args>
    inline void WriteLineIndented(const char* fmt, Args ...args)
    {
        sprintf(mTempString.data(), fmt, args...);
        WriteTabs();
        mFile << mTempString.data() << "\n";
    }

private:
    inline void WriteTabs()
    {
        for(uint32_t i = 0; i < mTabCount; i++)
            mFile << TAB;
    }

    std::ofstream mFile;
    uint32_t mTabCount;
    std::vector<char> mTempString;
    static constexpr uint32_t MAX_STRLEN = 2 << 14;
    static constexpr char TAB[] = "    ";
};