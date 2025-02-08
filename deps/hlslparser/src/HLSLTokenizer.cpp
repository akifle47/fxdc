//#include "Engine/Log.h"
//#include "Engine/String.h"
#include "Engine.h"

#include "HLSLTokenizer.h"

#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

namespace M4
{

// The order here must match the order in the Token enum.
static const char* _reservedWords[] =
    {
        "float",
        "float2",
        "float3",
        "float4",
		"float2x2",
        "float3x3",
        "float4x4",
        "float4x3",
        "float4x2",
        "half",
        "half2",
        "half3",
        "half4",
		"half2x2",
        "half3x3",
        "half4x4",
        "half4x3",
        "half4x2",
        "bool",
		"bool2",
		"bool3",
		"bool4",
        "int",
        "int2",
        "int3",
        "int4",
        "uint",
        "uint2",
        "uint3",
        "uint4",
        "string",
        "texture",
        "sampler",
        "sampler2D",
        "sampler3D",
        "samplerCUBE",
        "sampler2DShadow",
        "sampler2DMS",
        "sampler2DArray",
        "#include",
        "#define",
        "#ifdef",
        "#ifndef",
        "#else",
        "#endif",
        "if",
        "else",
        "for",
        "while",
        "break",
        "true",
        "false",
        "void",
        "struct",
        "cbuffer",
        "tbuffer",
        "register",
        "return",
        "continue",
        "discard",
        "const",
        "static",
        "inline",
        "uniform",
        "in",
        "out",
        "inout",
        "sampler_state",
        "technique",
        "pass",
        "shared",
        "VertexShader",
        "PixelShader",
        "asm"
    };

static bool GetIsSymbol(char c)
{
    switch (c)
    {
    case ';':
    case ':':
    case '(': case ')':
    case '[': case ']':
    case '{': case '}':
    case '-': case '+':
    case '*': case '/':
    case '?':
    case '!':
    case ',':
    case '=':
    case '.':
    case '<': case '>':
    case '|': case '&': case '^': case '~':
    case '@':
        return true;
    }
    return false;
}

/** Returns true if the character is a valid token separator at the end of a number type token */
static bool GetIsNumberSeparator(char c)
{
    return c == 0 || isspace(c) || GetIsSymbol(c);
}

HLSLTokenizer::HLSLTokenizer(const char* fileName, const char* buffer, size_t length)
{
    m_sValueLength = 10000;
    m_sValue = new char[m_sValueLength];
    memset(m_sValue, 0, m_sValueLength);

    m_source            = strdup(buffer);
    m_buffer            = m_source;
    m_bufferEnd         = m_buffer + length;
    m_fileName          = strdup(fileName);
    m_lineNumber        = 1;
    m_tokenLineNumber   = 1;
    m_error             = false;

    std::vector<std::string> macroIdentifiers;
    std::vector<bool> conditionalStack;
    PreProcess(macroIdentifiers, conditionalStack);

    Next();
}

HLSLTokenizer::HLSLTokenizer(const char* fileName, const char* buffer, size_t length, std::vector<std::string>& macroIdentifiers, std::vector<bool>& conditionalStack)
{
    m_sValueLength = 10000;
    m_sValue = new char[m_sValueLength];
    memset(m_sValue, 0, m_sValueLength);

    m_source = strdup(buffer);
    m_buffer = m_source;
    m_bufferEnd = m_buffer + length;
    m_fileName = strdup(fileName);
    m_lineNumber = 1;
    m_tokenLineNumber = 1;
    m_error = false;

    PreProcess(macroIdentifiers, conditionalStack);

    Next();
}

HLSLTokenizer::~HLSLTokenizer()
{
    if(m_source)
    {
        delete[] m_source;
        m_source = nullptr;
    }

    if(m_fileName)
    {
        delete[] m_fileName;
        m_fileName = nullptr;
    }

    if(m_sValue)
    {
        delete[] m_sValue;
        m_sValue = nullptr;
        m_sValueLength = 0;
    }
}

void HLSLTokenizer::Next()
{
	while( SkipWhitespace() || SkipComment() || ScanLineDirective() || SkipPragmaDirective())
    {
    }

    if (m_error)
    {
        m_token = HLSLToken_EndOfStream;
        return;
    }

    m_tokenLineNumber = m_lineNumber;

    if (m_buffer >= m_bufferEnd || *m_buffer == '\0')
    {
        m_token = HLSLToken_EndOfStream;
        return;
    }

    if(ScanPreProcessorDirectives())
        return;

    const char* start = m_buffer;

    // +=, -=, *=, /=, ==, <=, >=
    if (m_buffer[0] == '+' && m_buffer[1] == '=')
    {
        m_token = HLSLToken_PlusEqual;
        m_buffer += 2;
        return;
    }
    else if (m_buffer[0] == '-' && m_buffer[1] == '=')
    {
        m_token = HLSLToken_MinusEqual;
        m_buffer += 2;
        return;
    }
    else if (m_buffer[0] == '*' && m_buffer[1] == '=')
    {
        m_token = HLSLToken_TimesEqual;
        m_buffer += 2;
        return;
    }
    else if (m_buffer[0] == '/' && m_buffer[1] == '=')
    {
        m_token = HLSLToken_DivideEqual;
        m_buffer += 2;
        return;
    }
    else if (m_buffer[0] == '=' && m_buffer[1] == '=')
    {
        m_token = HLSLToken_EqualEqual;
        m_buffer += 2;
        return;
    }
    else if (m_buffer[0] == '!' && m_buffer[1] == '=')
    {
        m_token = HLSLToken_NotEqual;
        m_buffer += 2;
        return;
    }
    else if (m_buffer[0] == '<' && m_buffer[1] == '=')
    {
        m_token = HLSLToken_LessEqual;
        m_buffer += 2;
        return;
    }
    else if (m_buffer[0] == '>' && m_buffer[1] == '=')
    {
        m_token = HLSLToken_GreaterEqual;
        m_buffer += 2;
        return;
    }
    else if (m_buffer[0] == '&' && m_buffer[1] == '&')
    {
        m_token = HLSLToken_AndAnd;
        m_buffer += 2;
        return;
    }
    else if (m_buffer[0] == '|' && m_buffer[1] == '|')
    {
        m_token = HLSLToken_BarBar;
        m_buffer += 2;
        return;
    }

    // ++, --
    if ((m_buffer[0] == '-' || m_buffer[0] == '+') && (m_buffer[1] == m_buffer[0]))
    {
        m_token = (m_buffer[0] == '+') ? HLSLToken_PlusPlus : HLSLToken_MinusMinus;
        m_buffer += 2;
        return;
    }

    // Check for the start of a number.
    if (ScanNumber())
    {
        return;
    }
    
    if (GetIsSymbol(m_buffer[0]))
    {
        m_token = static_cast<HLSLToken>(m_buffer[0]);
        ++m_buffer;
        return;
    }

    if(m_buffer[0] == '"' && m_buffer[-1] != '\\')
    {
        m_buffer++;
        while(m_buffer < m_bufferEnd && m_buffer[0] != 0)
        {
            if(m_buffer[0] == '"' && m_buffer[-1] != '\\')
                break;
            ++m_buffer;
        }

        m_token = HLSLToken_StringLiteral;

        size_t length = m_buffer - (start + 1);
        if(m_sValueLength < length)
        {
            delete[] m_sValue;
            m_sValueLength = length + 1;
            m_sValue = new char[m_sValueLength];
        }
        memcpy(m_sValue, (start + 1), length);
        m_sValue[length] = 0;
        m_buffer++;

        return;
    }

    // Must be an identifier or a reserved word.
    while (m_buffer < m_bufferEnd && m_buffer[0] != 0 && !GetIsSymbol(m_buffer[0]) && !isspace(m_buffer[0]))
    {
        ++m_buffer;
    }

    size_t length = m_buffer - start;
    memcpy(m_identifier, start, length);
    m_identifier[length] = 0;
    
    if(length > s_maxIdentifier)
    {
        Error("Identifier name is too long! longest allowed identifier length is %d.", s_maxIdentifier);
        return;
    }

    const int numReservedWords = sizeof(_reservedWords) / sizeof(const char*);
    for (int i = 0; i < numReservedWords; ++i)
    {
        if (strcmp(_reservedWords[i], m_identifier) == 0)
        {
            m_token = (HLSLToken)(256 + i);
            if(m_token == HLSLToken_Asm)
                ScanAssemblyBlock();

            return;
        }
    }

    m_token = HLSLToken_Identifier;

}

void HLSLTokenizer::PreProcess(std::vector<std::string>& macroIdentifiers, std::vector<bool>& conditionalStack)
{
    std::string newSource;
    while(m_token != HLSLToken_EndOfStream && !m_error)
    {
        const char* prevBuffer = m_buffer;
        Next();

        if(m_token == HLSLToken_IncludeDirective)
        {
            Next();
            if(m_token != HLSLToken_StringLiteral)
            {
                Error("Syntax error: expected a file name");
                return;
            }

            char fileName[256] {};
            const char* separator = strrchr(m_fileName, '\\');
            if(!separator)
                separator = strrchr(m_fileName, '/');
            if(separator)
            {
                size_t len = (size_t)(separator - m_fileName) + 1;
                strncpy(fileName, m_fileName, len);
            }
            strcat(fileName, m_sValue);

            std::ifstream file(fileName, std::ios::binary | std::ios::ate);
            if(!file.good() || !file.is_open())
            {
                Error("Unable to open file \"%s\"", fileName);
                return;
            }

            size_t fileSize = (size_t)file.tellg();
            std::string source;
            source.resize(fileSize);
            file.seekg(0);
            file.read(source.data(), fileSize);

            HLSLTokenizer tokenizer(fileName, source.data(), source.length(), macroIdentifiers, conditionalStack);
            if(tokenizer.m_error)
            {
                return;
            }

            newSource.append(tokenizer.m_source);
            continue;
        }
        else if(m_token == HLSLToken_DefineDirective)
        {
            Next();
            if(m_token != HLSLToken_Identifier)
            {
                Error("Syntax error: expected identifier");
                return;
            }
            macroIdentifiers.emplace_back(m_identifier);
            continue;
        }
        else if(m_token == HLSLToken_IfDefDirective)
        {
            Next();
            if(m_token != HLSLToken_Identifier)
            {
                Error("Syntax error: expected identifier");
                return;
            }
            conditionalStack.push_back(std::find(macroIdentifiers.begin(), macroIdentifiers.end(), m_identifier) != macroIdentifiers.end());
            continue;
        }
        else if(m_token == HLSLToken_IfndefDirective)
        {
            Next();
            if(m_token != HLSLToken_Identifier)
            {
                Error("Syntax error: expected identifier");
                return;
            }
            conditionalStack.push_back(std::find(macroIdentifiers.begin(), macroIdentifiers.end(), m_identifier) == macroIdentifiers.end());
            continue;
        }
        else if(m_token == HLSLToken_ElseDirective)
        {
            if(conditionalStack.size())
                conditionalStack.at(conditionalStack.size() - 1) = !conditionalStack.at(conditionalStack.size() - 1);
            continue;
        }
        else if(m_token == HLSLToken_EndIfDirective)
        {
            if(conditionalStack.size())
                conditionalStack.pop_back();
            continue;
        }

        if(conditionalStack.size())
        {
            bool continueLoop = false;
            for(bool b : conditionalStack)
            {
                if(!b)
                {
                    continueLoop = true;
                    break;
                }
            }

            if(continueLoop)
                continue;
        }

        newSource.append(prevBuffer, m_buffer - prevBuffer);
    }

    delete[] m_source;
    m_source = strdup(newSource.data());
    m_buffer = m_source;
    m_bufferEnd = m_buffer + newSource.length();
    m_lineNumber = 1;
    m_tokenLineNumber = 1;
    m_token = (HLSLToken)0;
}

bool HLSLTokenizer::SkipWhitespace()
{
    bool result = false;
    while (m_buffer < m_bufferEnd && isspace(m_buffer[0]))
    {
        result = true;
        if (m_buffer[0] == '\n')
        {
            ++m_lineNumber;
        }
        ++m_buffer;
    }
    return result;
}

bool HLSLTokenizer::SkipComment()
{
    bool result = false;
    if (m_buffer[0] == '/')
    {
        if (m_buffer[1] == '/')
        {
            // Single line comment.
            result = true;
            m_buffer += 2;
            while (m_buffer < m_bufferEnd)
            {
                if (*(m_buffer++) == '\n')
                {
                    ++m_lineNumber;
                    break;
                }
            }
        }
        else if (m_buffer[1] == '*')
        {
            // Multi-line comment.
            result = true;
            m_buffer += 2;
            while (m_buffer < m_bufferEnd)
            {
                if (m_buffer[0] == '\n')
                {
                    ++m_lineNumber;
                }
                if (m_buffer[0] == '*' && m_buffer[1] == '/')
                {
                    break;
                }
                ++m_buffer;
            }
            if (m_buffer < m_bufferEnd)
            {
                m_buffer += 2;
            }
        }
    }
    return result;
}

bool HLSLTokenizer::SkipPragmaDirective()
{
	bool result = false;
	if( m_bufferEnd - m_buffer > 7 && *m_buffer == '#' )
	{
		const char* ptr = m_buffer + 1;
		while( isspace( *ptr ) )
			ptr++;

		if( strncmp( ptr, "pragma", 6 ) == 0 && isspace( ptr[ 6 ] ) )
		{
			m_buffer = ptr + 6;
			result = true;
			while( m_buffer < m_bufferEnd )
			{
				if( *( m_buffer++ ) == '\n' )
				{
					++m_lineNumber;
					break;
				}
			}
		}
	}
	return result;
}

bool HLSLTokenizer::ScanNumber()
{

    // Don't treat the + or - as part of the number.
    if (m_buffer[0] == '+' || m_buffer[0] == '-')
    {
        return false;
    }

    // Parse hex literals.
    if (m_bufferEnd - m_buffer > 2 && m_buffer[0] == '0' && m_buffer[1] == 'x')
    {
        char*   hEnd = NULL;
        int     iValue = (int)strtoul(m_buffer+2, &hEnd, 16);
        if (GetIsNumberSeparator(hEnd[0]))
        {
            m_buffer = hEnd;
            m_token  = HLSLToken_IntLiteral;
            m_iValue = iValue;
            return true;
        }
    }

    char* fEnd = NULL;
    double fValue = String_ToDouble(m_buffer, &fEnd);

    if (fEnd == m_buffer)
    {
        return false;
    }

    char*  iEnd = NULL;
    int    iValue = String_ToInteger(m_buffer, &iEnd);

    // If the character after the number is an f then the f is treated as part
    // of the number (to handle 1.0f syntax).
	if( ( fEnd[ 0 ] == 'f' || fEnd[ 0 ] == 'h' ) && fEnd < m_bufferEnd )
	{
		++fEnd;
	}

	if( fEnd > iEnd && GetIsNumberSeparator( fEnd[ 0 ] ) )
	{
		m_buffer = fEnd;
		m_token = fEnd[ 0 ] == 'h' ? HLSLToken_HalfLiteral : HLSLToken_FloatLiteral;
        m_fValue = static_cast<float>(fValue);
        return true;
    }
    else if (iEnd > m_buffer && GetIsNumberSeparator(iEnd[0]))
    {
        m_buffer = iEnd;
        m_token  = HLSLToken_IntLiteral;
        m_iValue = iValue;
        return true;
    }

    return false;
}

bool HLSLTokenizer::ScanLineDirective()
{
    
    if (m_bufferEnd - m_buffer > 5 && strncmp(m_buffer, "#line", 5) == 0 && isspace(m_buffer[5]))
    {

        m_buffer += 5;
        
        while (m_buffer < m_bufferEnd && isspace(m_buffer[0]))
        {
            if (m_buffer[0] == '\n')
            {
                Error("Syntax error: expected line number after #line");
                return false;
            }
            ++m_buffer;
        }

        char* iEnd = NULL;
        int lineNumber = String_ToInteger(m_buffer, &iEnd);

        if (!isspace(*iEnd))
        {
            Error("Syntax error: expected line number after #line");
            return false;
        }

        m_buffer = iEnd;
        while (m_buffer < m_bufferEnd && isspace(m_buffer[0]))
        {
            char c = m_buffer[0];
            ++m_buffer;
            if (c == '\n')
            {
                m_lineNumber = lineNumber;
                return true;
            }
        }

        if (m_buffer >= m_bufferEnd)
        {
            m_lineNumber = lineNumber;
            return true;
        }

        if (m_buffer[0] != '"')
        {
            Error("Syntax error: expected '\"' after line number near #line");
            return false;
        }
            
        ++m_buffer;
        
        int i = 0;
        while (i + 1 < s_maxIdentifier && m_buffer < m_bufferEnd && m_buffer[0] != '"')
        {
            if (m_buffer[0] == '\n')
            {
                Error("Syntax error: expected '\"' before end of line near #line");
                return false;
            }

            m_lineDirectiveFileName[i] = *m_buffer;
            ++m_buffer;
            ++i;
        }
        
        m_lineDirectiveFileName[i] = 0;
        
        if (m_buffer >= m_bufferEnd)
        {
            Error("Syntax error: expected '\"' before end of file near #line");
            return false;
        }

        if (i + 1 >= s_maxIdentifier)
        {
            Error("Syntax error: file name too long near #line");
            return false;
        }

        // Skip the closing quote
        ++m_buffer;
        
        while (m_buffer < m_bufferEnd && m_buffer[0] != '\n')
        {
            if (!isspace(m_buffer[0]))
            {
                Error("Syntax error: unexpected input after file name near #line");
                return false;
            }
            ++m_buffer;
        }

        // Skip new line
        ++m_buffer;

        m_lineNumber = lineNumber;
        m_fileName = m_lineDirectiveFileName;

        return true;

    }

    return false;

}

bool HLSLTokenizer::ScanPreProcessorDirectives()
{
    if(*m_buffer == '#')
    {
        const char* ptr = m_buffer + 1;
        while(isspace(*ptr))
            ptr++;

        if(strncmp(ptr, "include", 7) == 0)
        {
            m_token = HLSLToken_IncludeDirective;
            ptr += 7;
        }
        else if(strncmp(ptr, "ifdef", 5) == 0)
        {
            m_token = HLSLToken_IfDefDirective;
            ptr += 5;
        }
        else if(strncmp(ptr, "ifndef", 6) == 0)
        {
            m_token = HLSLToken_IfndefDirective;
            ptr += 6;
        }
        else if(strncmp(ptr, "endif", 5) == 0)
        {
            m_token = HLSLToken_EndIfDirective;
            ptr += 5;
        }
        else if(strncmp(ptr, "else", 5) == 0)
        {
            m_token = HLSLToken_ElseDirective;
            ptr += 4;
        }
        else if(strncmp(ptr, "define", 6) == 0)
        {
            m_token = HLSLToken_DefineDirective;
            ptr += 6;
        }
        else
        {
            return false;
        }

        m_buffer = ptr;
        return true;
    }

    return false;
}

void HLSLTokenizer::ScanAssemblyBlock()
{
    Next();
    if(m_token != '{')
    {
        Error("Expected '{'");
        return;
    }

    const char* start = m_buffer;
    while(m_buffer < m_bufferEnd && m_buffer[0] != 0 && m_buffer[0] != '}')
    {
        if(*m_buffer++ == '\n')
            m_lineNumber++;
    }

    size_t length = m_buffer - (start + 1);
    if(m_sValueLength < length)
    {
        delete[] m_sValue;
        m_sValueLength = length + 1;
        m_sValue = new char[m_sValueLength];
    }
    memcpy(m_sValue, (start + 1), length);
    m_sValue[length] = 0;

    Next();
    m_token = HLSLToken_Asm;
}

int HLSLTokenizer::GetToken() const
{
    return m_token;
}

float HLSLTokenizer::GetFloat() const
{
    return m_fValue;
}

int HLSLTokenizer::GetInt() const
{
    return m_iValue;
}

const char* HLSLTokenizer::GetString() const
{
    return m_sValue;
}

const char* HLSLTokenizer::GetIdentifier() const
{
    return m_identifier;
}

int HLSLTokenizer::GetLineNumber() const
{
    return m_tokenLineNumber;
}

const char* HLSLTokenizer::GetFileName() const
{
    return m_fileName;
}

void HLSLTokenizer::Error(const char* format, ...)
{
    // It's not always convenient to stop executing when an error occurs,
    // so just track once we've hit an error and stop reporting them until
    // we successfully bail out of execution.
    if (m_error)
    {
        return;
    }
    m_error = true;


    char buffer[1024];
    va_list args;
    va_start(args, format);
    int result = vsnprintf(buffer, sizeof(buffer) - 1, format, args);
    va_end(args);

    Log_Error("%s(%d) : %s\n", m_fileName, m_lineNumber, buffer);
} 

void HLSLTokenizer::GetTokenName(char buffer[s_maxIdentifier]) const
{
    if (m_token == HLSLToken_FloatLiteral || m_token == HLSLToken_HalfLiteral )
    {
        sprintf(buffer, "%f", m_fValue);
    }
    else if (m_token == HLSLToken_IntLiteral)
    {
        sprintf(buffer, "%d", m_iValue);
    }
    else if (m_token == HLSLToken_Identifier)
    {
        strcpy(buffer, m_identifier);
    }
    else
    {
        GetTokenName(m_token, buffer);
    }
}

void HLSLTokenizer::GetTokenName(int token, char buffer[s_maxIdentifier])
{
    if (token < 256)
    {
        buffer[0] = (char)token;
        buffer[1] = 0;
    }
    else if (token < HLSLToken_LessEqual)
    {
        strcpy(buffer, _reservedWords[token - 256]);
    }
    else
    {
        switch (token)
        {
        case HLSLToken_PlusPlus:
            strcpy(buffer, "++");
            break;
        case HLSLToken_MinusMinus:
            strcpy(buffer, "--");
            break;
        case HLSLToken_PlusEqual:
            strcpy(buffer, "+=");
            break;
        case HLSLToken_MinusEqual:
            strcpy(buffer, "-=");
            break;
        case HLSLToken_TimesEqual:
            strcpy(buffer, "*=");
            break;
        case HLSLToken_DivideEqual:
            strcpy(buffer, "/=");
            break;
		case HLSLToken_HalfLiteral:
			strcpy( buffer, "half" );
			break;
        case HLSLToken_FloatLiteral:
            strcpy(buffer, "float");
            break;
        case HLSLToken_IntLiteral:
            strcpy(buffer, "int");
            break;
        case HLSLToken_Identifier:
            strcpy(buffer, "identifier");
            break;
        case HLSLToken_EndOfStream:
            strcpy(buffer, "<eof>");
            break;
        default:
            strcpy(buffer, "unknown");
            break;
        }
    }

}

}
