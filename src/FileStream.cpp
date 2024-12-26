#include "FileStream.h"
#include "Log.h"

#include <cassert>
#include <filesystem>

IFileStream::IFileStream(const char* filePath) : mSize(0)
{
    mPath = std::filesystem::absolute(filePath).string().c_str();
    for(char* c = mPath.Get(); *c; c++)
    {
        if(*c == '/')
            *c = '\\';
    }
}

bool IFileStream::Open()
{
    if(mFile.is_open())
        return true;

    mFile = std::ifstream(mPath.Get(), std::ios::binary | std::ios::ate);
    if(!mFile.good() || !mFile.is_open())
    {
        Log::Error("Unable to open file \"%s\"", mPath.Get());
        return false;
    }

    mSize = (size_t)mFile.tellg();
    mFile.seekg(0);

    return true;
}

void IFileStream::Close()
{
    mFile.close();
}

const char* IFileStream::GetFilePath()
{
    return mPath.Get();
}

CString IFileStream::GetFileName()
{
    const char* separator = strrchr(mPath.Get(), '\\');

    CString fileName;
    if(!separator)
        fileName = {mPath};
    else
        fileName = {++separator};

    return fileName;
}

CString IFileStream::GetFileNameNoExtension()
{
    const char* separator = strrchr(mPath.Get(), '\\');

    CString fileName;
    if(!separator)
        fileName = {mPath};
    else
        fileName = {++separator};

    char* ext = strrchr(fileName.Get(), '.');
    if(ext)
        *ext = '\0';

    return fileName;
}

size_t IFileStream::GetSize()
{
    return mSize;
}

void IFileStream::Seek(std::streamoff offset, eSeekDir dir)
{
    std::ios::seekdir dirStd {};
    if(dir == eSeekDir::CURRENT)
        dirStd = std::ios::cur;
    else if(dirStd == eSeekDir::BEGGINING)
        dirStd = std::ios::beg;
    else if(dir == eSeekDir::END)
        dirStd = std::ios::end;

    mFile.seekg(offset, dirStd);
}

inline bool IFileStream::Read(void* buffer, std::streamsize count)
{
    assert(mFile.is_open());
    
    mFile.read((char*)buffer, count);
    return mFile.rdstate() & std::ios::badbit;
}

bool IFileStream::ReadByte(void* buffer)
{
    return Read(buffer, sizeof(uint8_t));
}

bool IFileStream::ReadWord(void* buffer)
{
    return Read(buffer, sizeof(uint16_t));
}

bool IFileStream::ReadDword(void* buffer)
{
    return Read(buffer, sizeof(uint32_t));
}


//OFileStream
OFileStream::OFileStream(const char* filePath)
{
    mPath = std::filesystem::absolute(filePath).string().c_str();
    for(char* c = mPath.Get(); *c; c++)
    {
        if(*c == '/')
            *c = '\\';
    }
}

bool OFileStream::Open()
{
    if(mFile.is_open())
        return true;

    mFile = std::ofstream(mPath.Get(), std::ios::binary);
    if(!mFile.good() || !mFile.is_open())
    {
        Log::Error("Unable to open file \"%s\"", mPath.Get());
        return false;
    }

    return true;
}

void OFileStream::Close()
{
    mFile.close();
}

const char* OFileStream::GetFilePath()
{
    return mPath.Get();
}

CString OFileStream::GetFileName()
{
    const char* separator = strrchr(mPath.Get(), '\\');

    CString fileName;
    if(!separator)
        fileName = {mPath};
    else
        fileName = {++separator};

    return fileName;
}

CString OFileStream::GetFileNameNoExtension()
{
    const char* separator = strrchr(mPath.Get(), '\\');

    CString fileName;
    if(!separator)
        fileName = {mPath};
    else
        fileName = {++separator};

    char* ext = strrchr(fileName.Get(), '.');
    if(ext)
        *ext = '\0';

    return fileName;
}

void OFileStream::Seek(std::streamoff offset, eSeekDir dir)
{
    std::ios::seekdir dirStd {};
    if(dir == eSeekDir::CURRENT)
        dirStd = std::ios::cur;
    else if(dirStd == eSeekDir::BEGGINING)
        dirStd = std::ios::beg;
    else if(dir == eSeekDir::END)
        dirStd = std::ios::end;

    mFile.seekp(offset, dirStd);
}

inline bool OFileStream::Write(const void* buffer, std::streamsize count)
{
    assert(mFile.is_open());

    mFile.write((char*)buffer, count);
    return mFile.rdstate() & std::ios::badbit;
}

bool OFileStream::WriteByte(const void* buffer)
{
    return Write(buffer, sizeof(uint8_t));
}

bool OFileStream::WriteWord(const void* buffer)
{
    return Write(buffer, sizeof(uint16_t));
}

bool OFileStream::WriteDword(const void* buffer)
{
    return Write(buffer, sizeof(uint32_t));
}