#define WIN32_LEAN_AND_MEAN

#include "Effect.h"
#include "FileStream.h"
#include "hlslparser/src/HLSLParser.h"

#include <Windows.h>
#include <filesystem>
#include <vector>

//returns whether it should quit
bool ProcessInput(const char* input);
bool ProcessEffect(std::filesystem::path fileIn, std::filesystem::path dirOut);

int main(int32_t argc, char** argv)
{
    if(argc == 1)
    {
        char input[256] {0};
        wchar_t inputW[256] {0};
        DWORD bytesRead = 0;
        
        while(true)
        {
            printf("> ");

            ZeroMemory(input, std::size(input));
            ZeroMemory(inputW, std::size(inputW));

            std::ignore = ReadConsoleW(GetStdHandle(STD_INPUT_HANDLE), inputW, 256, &bytesRead, nullptr);
            WideCharToMultiByte(CP_UTF8, 0, inputW, -1, input, std::size(input), nullptr, nullptr);

            if(ProcessInput(input))
            {
                return 0;
            }
        }
    }
    else if(argc == 3)
    {
        const char* inFile = argv[1];
        const char* outDir = argv[2];
        return (int)ProcessEffect(inFile, outDir) - 1;
    }

    return 0;
}

bool ProcessInput(const char* input)
{
    if(std::iscntrl(input[0]))
        return false;

    if(memcmp(input, "-q", 2) == 0 || memcmp(input, "-quit", 5) == 0)
        return true;

    std::vector<CString> tokens;
    tokens.reserve(4);

    for(uint32_t beg = 0; beg < strlen(input) - 1; beg++)
    {
        if(input[beg] > ' ')
        {
            uint32_t end = beg;
            while(input[end] > ' ') end++;

            tokens.emplace_back((end - beg) + 1);
            memcpy(tokens.back().Get(), &input[beg], end - beg);
            beg = end;
        }
    }

    if(tokens.size() > 0)
    {
        const char* inFile = tokens[0].Get();
        const char* outDir = tokens.size() == 1 ? "" : tokens[1].Get();
        ProcessEffect(inFile, outDir);
        return false;
    }

    return false;
}

bool ProcessEffect(std::filesystem::path fileIn, std::filesystem::path dirOut)
{
    if(!dirOut.empty())
    {
        if(dirOut.has_filename())
        {
            //assume that a path with no file extension is a directory without a separator
            if(dirOut.has_extension())
                dirOut.concat("\\");
            else
                dirOut.replace_filename(dirOut.filename());
        }
        else
        {
            dirOut.concat(fileIn.filename().string());
        }
    }
    else
    {
        dirOut = fileIn;
    }

    if(fileIn.extension() == ".fxc")
    {
        IFileStream file(fileIn.string().c_str());
        if(file.Open())
        {
            Effect effect(file);

            dirOut.replace_extension(".fx");
            return (bool)effect.SaveToFx(dirOut);
        }
    }
    else if(fileIn.extension() == ".fx")
    {
        std::ifstream file(fileIn, std::ios::ate);
        if(!file.good() || !file.is_open())
        {
            Log::Error("Unable to open file \"%s\"", fileIn.string().c_str());
            return false;
        }

        CString cFileName = fileIn.string().c_str();
        size_t fileSize = (size_t)file.tellg();
        CString source(fileSize);
        file.seekg(0);
        file.read(source.Get(), fileSize);

        M4::Allocator allocator;
        M4::HLSLParser parser(&allocator, cFileName.Get(), source.Get(), fileSize);
        M4::HLSLTree tree(&allocator);
        if(!parser.Parse(&tree))
        {
            return false;
        }
        
        Effect effect;
        if(!effect.LoadFromFx(parser))
            return false;
        return (bool)effect.Save(dirOut.replace_extension(".fxc"));
    }

    return false;
}