#pragma once
#include "CString.h"

#include <fstream>

//very simple wrapper for std::fstream in binary mode

enum eSeekDir : uint8_t
{
    CURRENT,
    BEGGINING,
    END
};

class IFileStream
{
public:
    IFileStream(const char* filePath);

    ~IFileStream() = default;

    bool Open();
    void Close();

    const char* GetFilePath();
    CString GetFileName();
    CString GetFileNameNoExtension();
    size_t GetSize();

    void Seek(std::streamoff offset, eSeekDir dir = eSeekDir::CURRENT);

    inline bool Read(void* buffer, std::streamsize count);
    bool ReadByte(void* buffer);
    bool ReadWord(void* buffer);
    bool ReadDword(void* buffer);

private:
    CString mPath;
    size_t mSize;
    std::ifstream mFile;
};


class OFileStream
{
public:
    OFileStream(const char* filePath);

    ~OFileStream() = default;

    bool Open();
    void Close();

    const char* GetFilePath();
    CString GetFileName();
    CString GetFileNameNoExtension();

    void Seek(std::streamoff offset, eSeekDir dir = eSeekDir::CURRENT);

    inline bool Write(const void* buffer, std::streamsize count);
    bool WriteByte(const void* buffer);
    bool WriteWord(const void* buffer);
    bool WriteDword(const void* buffer);

private:
    CString mPath;
    std::ofstream mFile;
};