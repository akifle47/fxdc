#define WIN32_LEAN_AND_MEAN

#include "Effect.h"
#include "FileStream.h"
#include "hlslparser/src/HLSLParser.h"

#include <Windows.h>
#include <filesystem>
#include <vector>
#include <span>

struct CmdOption
{
    CString Name;
    CString Description;
};
CmdOption gCmdOptions[]
{
    {"/Out", "</Out> <in_file> <out_file or out_folder>        compile an effect to a specified file or folder"},

    {"/Od",  "/Od                                              disable optimizations"},
    {"/Zi",  "/Zi                                              enable debugging information"},
    {"/Zpr", "/Zpr                                             pack matrices in row-major order"},
    {"/Zpc", "/Zpc                                             pack matrices in column-major order"},
                                                    
    {"/Gpp", "/Gpp                                             force partial precision"},
    {"/Gfa", "/Gfa                                             avoid flow control constructs"},
    {"/Gfp", "/Gfp                                             prefer flow control constructs"},
    {"/Gis", "/Gis                                             force IEE strictness"},

    {"/D",  "/D<name> <definition>                             define a macro"},
};

//returns whether it should quit
bool ProcessArguments(std::span<CString> args);
bool ProcessEffect(std::filesystem::path fileIn, std::filesystem::path fileOut, DWORD shaderFlags, const D3DXMACRO* macros);

int main(int32_t argc, char** argv)
{
    if(argc == 1)
    {
        #ifdef _DEBUG
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

                if(std::iscntrl(input[0]))
                    return false;

                if(memcmp(input, "-q", 2) == 0 || memcmp(input, "-quit", 5) == 0)
                    return true;

                std::vector<CString> args;
                args.reserve(12);

                for(uint32_t beg = 0; beg < strlen(input) - 1; beg++)
                {
                    if(input[beg] > ' ')
                    {
                        uint32_t end = beg;
                        while(input[end] > ' ') 
                            end++;

                        args.emplace_back((end - beg) + 1);
                        memcpy(args.back().Get(), &input[beg], end - beg);
                        beg = end;
                    }
                }

                if(ProcessArguments(args))
                {
                    return 0;
                }
            }
        #endif //_DEBUG
    }
    else
    {
        std::vector<CString> args;
        args.reserve(argc);

        for(int32_t i = 1; i < argc; i++)
        {
            args.emplace_back(argv[i]);
        }

        ProcessArguments(args);
    }

    return 0;
}

void PrintHelp()
{
    printf("usage: fxdc <options> </Out> <in_file> <out_file or out_folder>\n\n");

    for(size_t i = 0; i < std::size(gCmdOptions); i++)
    {
        const CmdOption& option = gCmdOptions[i];

        printf("   %s    %s", option.Name.Get(), option.Description.Get());

        if(option.Name == "/Out" || option.Name == "/Zpc" || option.Name == "/Gis")
            printf("\n\n");
        else
            printf("\n");
    }
}

bool ProcessArguments(std::span<CString> args)
{
    CString inFile;
    CString outFile;
    DWORD shaderFlags = 0;
    std::vector<D3DXMACRO> macros;
    for(size_t i = 0; i < args.size(); i++)
    {
        const CString& arg = args[i];

        for(size_t j = 0; j < std::size(gCmdOptions); j++)
        {
            CmdOption& option = gCmdOptions[j];
            if(arg == option.Name)
            {
                if(arg == "/Out")
                {
                    if(i + 1 < args.size())
                    {
                        inFile = args[++i];
                    }
                    else
                    {
                        Log::Error("expected a file");
                        PrintHelp();
                        return false;
                    }

                    if(i + 1 < args.size())
                    {
                        outFile = args[++i];
                    }
                    else
                    { 
                        Log::Error("expected a file");
                        PrintHelp();
                        return false;
                    }
                }
                else if(arg == "/D")
                {
                    D3DXMACRO macro;
                    if(i + 1 < args.size())
                    {
                        macro.Name = args[++i].Get();
                    }
                    else
                    {
                        Log::Error("expected macro identifier");
                        PrintHelp();
                        return false;
                    }

                    if(i + 1 < args.size())
                    {
                        macro.Definition = args[++i].Get();
                        macros.push_back(macro);
                    }
                    else
                    {
                        Log::Error("expected macro defintion");
                        PrintHelp();
                        return false;
                    }
                }
                else if(arg == "/Od")
                {
                    shaderFlags |= D3DXSHADER_SKIPOPTIMIZATION;
                }
                else if(arg == "/Zi")
                {
                    shaderFlags |= D3DXSHADER_DEBUG;
                }
                else if(arg == "/Zpr")
                {
                    shaderFlags |= D3DXSHADER_PACKMATRIX_ROWMAJOR;
                }
                else if(arg == "/Zpc")
                {
                    shaderFlags |= D3DXSHADER_PACKMATRIX_COLUMNMAJOR;
                }
                else if(arg == "/Gpp")
                {
                    shaderFlags |= D3DXSHADER_PARTIALPRECISION;
                }
                else if(arg == "/Gfa")
                {
                    shaderFlags |= D3DXSHADER_AVOID_FLOW_CONTROL;
                }
                else if(arg == "/Gfp")
                {
                    shaderFlags |= D3DXSHADER_PREFER_FLOW_CONTROL;
                }
                else if(arg == "/Gis")
                {
                    shaderFlags |= D3DXSHADER_IEEE_STRICTNESS;
                }
                else
                {
                    Log::Error("unknown or invalid option \"%s\"", arg.Get());
                    PrintHelp();
                    return false;
                }
            }
        }
    }

    if(!inFile.Get() || !outFile.Get())
    {
        Log::Error("no files specified");
        PrintHelp();
        return false;
    }

    //last one has to be null
    macros.emplace_back(nullptr, nullptr);

    ProcessEffect(inFile.Get(), outFile.Get(), shaderFlags, macros.data());
    return false;
}

bool ProcessEffect(std::filesystem::path fileIn, std::filesystem::path fileOut, DWORD shaderFlags, const D3DXMACRO* macros)
{
    if(!fileOut.has_filename())
    {
        fileOut.concat(fileIn.filename().string());
    }

    if(fileIn.extension() == ".fxc")
    {
        IFileStream file(fileIn.string().c_str());
        if(file.Open())
        {
            Effect effect(file);

            fileOut.replace_extension(".fx");
            if(effect.SaveToFx(fileOut))
            {
                Log::Info("successfully unpacked effect \"%s\"", fileOut.string().c_str());
                return false;
            }
            else
            {
                return false;
            }
        }
    }
    else if(fileIn.extension() == ".fx")
    {
        std::ifstream file(fileIn, std::ios::ate);
        if(!file.good() || !file.is_open())
        {
            Log::Error("unable to open file \"%s\"", fileIn.string().c_str());
            return false;
        }

        CString cFileName = fileIn.string().c_str();
        size_t fileSize = (size_t)file.tellg();
        CString source(fileSize);
        file.seekg(0);
        file.read(source.Get(), fileSize);

        M4::Allocator allocator;
        M4::HLSLParser parser(&allocator, cFileName.Get(), source.Get(), fileSize, macros);
        M4::HLSLTree tree(&allocator);
        if(!parser.Parse(&tree))
        {
            return false;
        }
        
        Effect effect;
        if(!effect.LoadFromFx(parser, shaderFlags, macros))
            return false;

        if(effect.Save(fileOut.replace_extension(".fxc")))
        {
            Log::Info("successfully compiled effect \"%s\"", fileOut.string().c_str());
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        Log::Error("\"%s\" is not an effect file.", fileIn.string().c_str());
    }

    return false;
}