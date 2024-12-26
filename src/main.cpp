#define WIN32_LEAN_AND_MEAN

#include "Effect.h"
#include "FileStream.h"

#include <Windows.h>
#include <filesystem>
#include <vector>

//returns whether it should quit
bool ProcessInput(const char* input);
bool EffectTest(std::filesystem::path fileIn, std::filesystem::path dirOut);

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
        return (int)EffectTest(inFile, outDir) - 1;
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
        EffectTest(inFile, outDir);
        return false;
    }

    return false;
}

bool EffectTest(std::filesystem::path fileIn, std::filesystem::path dirOut)
{
    IFileStream file(fileIn.string().c_str());
    if(file.Open())
    {
        Effect effect(file);

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

        dirOut.replace_extension(".fx");
        return effect.SaveToFx(dirOut);
    }

    return false;
}