#include "FileStream.h"
#include "Effect.h"
#include "rage/StringHash.h"
#include "Log.h"
#include "dx9/d3dx9.h"

#include <filesystem>
#include <cassert>

static constexpr uint8_t sParamTypeSizeFactor[] {0, 1, 1, 1, 1, 1, 0, 1, 3, 4, 0, 0, 0, 0, 0, 0};

static inline void WriteString(OFileStream& file, const CString& str)
{
    uint8_t strLen = (uint8_t)str.Length() + 1;
    file.WriteByte(&strLen);
    file.Write(str.Get(), strLen);
}

static inline CString ReadString(IFileStream& file)
{
    uint8_t strLen;
    file.ReadByte(&strLen);
    CString str = {(uint32_t)strLen};
    file.Read(str.Get(), strLen);

    return str;
}

Effect::Effect(IFileStream& file)
{
    uint32_t magic;
    file.ReadDword(&magic);
    if(magic != MAGIC)
    {
        Log::Error("\"%s\" is not an effect file.", file.GetFilePath());
        return;
    }

    uint8_t shaderCount;
    file.ReadByte(&shaderCount);
    mVertexPrograms = rage::atArray<VertexProgram>(shaderCount);
    for(uint16_t i = 0; i < shaderCount; i++)
    {
        mVertexPrograms.Append().Load(file);
    }

    file.ReadByte(&shaderCount);
    mPixelPrograms = rage::atArray<PixelProgram>(shaderCount);
    for(uint16_t i = 0; i < shaderCount; i++)
    {
        mPixelPrograms.Append().Load(file);
    }

    uint8_t paramCount;
    file.ReadByte(&paramCount);
    mGlobalParameters = rage::atArray<Parameter>(paramCount);
    for(uint8_t i = 0; i < paramCount; i++)
    {
        mGlobalParameters.Append().Load(file);
    }

    file.ReadByte(&paramCount);
    mParameters = rage::atArray<Parameter>(paramCount);
    for(uint8_t i = 0; i < paramCount; i++)
    {
        mParameters.Append().Load(file);
    }

    uint8_t techCount;
    file.ReadByte(&techCount);
    mTechniques = rage::atArray<EffectTechnique>(techCount);
    for(uint8_t i = 0; i < techCount; i++)
    {
        mTechniques.Append().Load(file);
    }
}

void Effect::Save(const std::filesystem::path& filePath) const
{
    OFileStream file(filePath.string().c_str());
    if(!file.Open())
        return;

    file.WriteDword(&Effect::MAGIC);

    uint16_t shaderCount = mVertexPrograms.GetCount();
    file.WriteByte(&shaderCount);
    for(uint16_t i = 0; i < shaderCount; i++)
    {
        mVertexPrograms[i].Save(file, *this);
    }

    shaderCount = mPixelPrograms.GetCount();
    file.WriteByte(&shaderCount);
    for(uint16_t i = 0; i < shaderCount; i++)
    {
        mPixelPrograms[i].Save(file, *this);
    }

    uint16_t paramCount = mGlobalParameters.GetCount();
    file.WriteByte(&paramCount);
    for(uint16_t i = 0; i < paramCount; i++)
    {
        mGlobalParameters[i].Save(file);
    }

    paramCount = mParameters.GetCount();
    file.WriteByte(&paramCount);
    for(uint8_t i = 0; i < paramCount; i++)
    {
        mParameters[i].Save(file);
    }

    uint16_t techCount = mTechniques.GetCount();
    file.WriteByte(&techCount);
    for(uint8_t i = 0; i < techCount; i++)
    {
        mTechniques[i].Save(file);
    }
}

bool Effect::SaveToFx(const std::filesystem::path& filePath) const
{
    EffectWriter file(filePath.string().c_str());

    if(!file.IsOpen())
        return false;

    file.WriteLine("//Globals");
    for(const Parameter& param : mGlobalParameters)
    {
        param.SaveToFx(file, true);
    }

    file.NewLine();
    file.WriteLine("//Locals");
    for(const Parameter& param : mParameters)
    {
        param.SaveToFx(file);
    }

    file.NewLine();
    file.WriteLine("//Vertex shaders");
    for(uint32_t i = 0; i < mVertexPrograms.GetCount(); i++)
    {
        const GpuProgram& program = mVertexPrograms[i];
        CString disasm = program.GetDisassembly();
        char* line = disasm.Get();

        if(*line == '\0')
        {
            file.WriteLineIndented("PixelShader PixelShader%d = NULL;", i);
            file.NewLine();
            continue;
        }

        file.WriteLineIndented("VertexShader VertexShader%d =", i);
        file.WriteLineIndented("asm");
        file.WriteLineIndented("{");
        file.PushTab();

        while(strchr(line, '\n'))
        {
            char* nl = strchr(line, '\n');
            *nl = '\0';
            file.WriteLineIndented(line);
            line = ++nl;
        }
        file.PopTab();
        file.WriteLineIndented("};");
        if(i != mVertexPrograms.GetCount() - 1)
            file.NewLine();
    }

    file.NewLine();
    file.WriteLine("//Pixel shaders");
    for(uint32_t i = 0; i < mPixelPrograms.GetCount(); i++)
    {
        const GpuProgram& program = mPixelPrograms[i];
        CString disasm = program.GetDisassembly();
        char* line = disasm.Get();

        if(*line == '\0')
        {
            file.WriteLineIndented("PixelShader PixelShader%d = NULL;", i);
            file.NewLine();
            continue;
        }

        file.WriteLineIndented("PixelShader PixelShader%d =", i);
        file.WriteLineIndented("asm");
        file.WriteLineIndented("{");
        file.PushTab();

        while(strchr(line, '\n'))
        {
            char* nl = strchr(line, '\n');
            *nl = '\0';
            file.WriteLineIndented(line);
            line = ++nl;
        }
        file.PopTab();
        file.WriteLineIndented("};");
        if(i != mPixelPrograms.GetCount() - 1)
            file.NewLine();
    }

    file.NewLine();
    for(const auto& technique : mTechniques)
    {
        technique.SaveToFx(file, *this);
    }

    return true;
}

const Parameter* Effect::FindParameterByName(const char* name) const
{
    return FindParameterByHash(rage::atStringHash(name));
}

const Parameter* Effect::FindParameterByHash(uint32_t hash) const
{
    for(uint16_t i = 0; i < mParameters.GetCount(); i++)
    {
        if(mParameters[i].mNameHash == hash || mParameters[i].mSemanticHash == hash)
            return &mParameters[i];
    }

    return nullptr;
}

const Parameter* Effect::FindGlobalParameterByName(const char* name) const
{
    return FindGlobalParameterByHash(rage::atStringHash(name));
}

const Parameter* Effect::FindGlobalParameterByHash(uint32_t hash) const
{
    for(uint16_t i = 0; i < mGlobalParameters.GetCount(); i++)
    {
        if(mGlobalParameters[i].mNameHash == hash || mGlobalParameters[i].mSemanticHash == hash)
            return &mGlobalParameters[i];
    }

    return nullptr;
}

const Parameter* Effect::GetParameterAt(uint32_t index) const
{
    assert(index >= 0 && index < mParameters.GetCount());
    return &mParameters[index];
}

CString Effect::GetVertexShaderDisassembly(uint32_t index) const
{
    return mVertexPrograms[index].GetDisassembly();
}

CString Effect::GetPixelShaderDisassembly(uint32_t index) const
{
    return mPixelPrograms[index].GetDisassembly();
}


void GpuProgram::Save(OFileStream& file, const Effect& effect) const
{
    file.WriteByte(&mParamCount);
    if(mParamCount)
    {
        for(uint32_t i = 0; i < mParamCount; i++)
        {
            mParams[i].Save(file, effect);
        }
    }

    uint16_t shaderSize = mShaderData.GetCapacity();
    file.WriteWord(&shaderSize);
    file.WriteWord(&shaderSize);

    if(shaderSize)
        file.Write(&mShaderData[0], shaderSize);
}

void GpuProgram::Load(IFileStream& file)
{
    file.ReadByte(&mParamCount);
    if(mParamCount)
    {
        mParams = {(uint16_t)mParamCount};
        for(uint32_t i = 0; i < mParamCount; i++)
        {
            mParams.Append().Load(file);
        }
    }

    uint16_t shaderSize;
    file.ReadWord(&shaderSize);
    //compressed size. only supported by vertex shaders and not used by any of the game's shaders so i ignore it
    file.Seek(sizeof(uint16_t));

    if(shaderSize)
    {
        mShaderData = {shaderSize};
        file.Read(&mShaderData[0], shaderSize);
    }
}

CString GpuProgram::GetDisassembly() const
{
    if(!mShaderData.GetCapacity())
        return "";

    ID3DXBuffer* disasm;
    D3DXDisassembleShader((DWORD*)&mShaderData[0], 0, nullptr, &disasm);
    CString disassembly = (char*)disasm->GetBufferPointer();
    disasm->Release();

    return disassembly;
}


void GpuProgram::Param::Save(OFileStream& file, const Effect& effect) const
{
    file.WriteByte(&mType);
    file.WriteByte(&mUnknown);
    file.WriteWord(&mRegisterIndex);

    auto param = effect.FindParameterByHash(mNameHash);
    if(!param)
        param = effect.FindGlobalParameterByHash(mNameHash);

    CString name;
    if(mNameHash == param->GetNameHash())
        name = param->GetName1();
    else
        name = param->GetSemantic();

    WriteString(file, name);
}

void GpuProgram::Param::Load(IFileStream& file)
{
    file.ReadByte(&mType);
    file.ReadByte(&mUnknown);
    file.ReadWord(&mRegisterIndex);

    CString name = ReadString(file);
    mNameHash = rage::atStringHash(name.Get());
}


Parameter& Parameter::operator=(const Parameter& rhs)
{
    mType = rhs.mType;
    mCount = rhs.mCount;
    mSize = rhs.mCount;
    mAnnotationCount = rhs.mAnnotationCount;

    mName = rhs.mName;
    mNameHash = rhs.mNameHash;
    mSemantic = rhs.mSemantic;
    mSemanticHash = rhs.mSemanticHash;

    mAnnotations = new Annotation[mAnnotationCount];
    for(uint8_t i = 0; i < mAnnotationCount; i++)
    {
        mAnnotations[i] = rhs.mAnnotations[i];
    }

    if(!rhs.mValue.AsVoid)
        return *this;

    if(mType == eType::TEXTURE)
    {
        mValue.AsVoid = new uint8_t[4 * mSize];
        memcpy(mValue.AsVoid, rhs.mValue.AsVoid, 4 * mSize);
    }
    else
    {
        uint32_t totalSize = 16 * mCount * sParamTypeSizeFactor[(uint8_t)mType];
        mValue.AsVoid = new uint8_t[totalSize];
        memcpy(mValue.AsVoid, rhs.mValue.AsVoid, totalSize);
    }

    return *this;
}

uint32_t Parameter::GetTotalSize() const
{
    return mCount * 16 * sParamTypeSizeFactor[(uint32_t)mType];
}

void Parameter::Save(OFileStream& file) const
{
    file.WriteByte(&mType);

    if(mCount == 1 && mType != eType::TEXTURE)
    {
        uint8_t b = 0;
        file.WriteByte(&b);
    }
    else
        file.WriteByte(&mCount);

    WriteString(file, mName);
    WriteString(file, mSemantic);

    file.WriteByte(&mAnnotationCount);

    if(mAnnotationCount)
    {
        for(uint8_t i = 0; i < mAnnotationCount; i++)
        {
            mAnnotations[i].Save(file);
        }
    }

    file.WriteByte(&mSize);

    if(!mSize)
        return;

    if(mType == eType::TEXTURE)
    {
        file.Write(mValue.AsVoid, 4 * mSize);
    }
    else
    {
        uint32_t totalSize = 4 * mCount * sParamTypeSizeFactor[(uint8_t)mType];

        uint32_t srcStride = (mSize / mCount) * 4;
        uint32_t dstStride = (totalSize / mCount) * 4;
        uint8_t* dst = (uint8_t*)mValue.AsVoid;

        for(uint8_t i = 0; i < mCount; i++)
        {
            file.Write(dst, srcStride);
            dst += dstStride;
        }
    }
}

void Parameter::Load(IFileStream& file)
{
    file.ReadByte(&mType);
    file.ReadByte(&mCount);

    if(!mCount && mType != eType::TEXTURE)
    {
        mCount = 1;
    }

    mName = ReadString(file);
    mNameHash = rage::atStringHash(mName.Get());
    mSemantic = ReadString(file);
    mSemanticHash = rage::atStringHash(mSemantic.Get());

    file.ReadByte(&mAnnotationCount);

    if(mAnnotationCount)
    {
        mAnnotations = new Annotation[mAnnotationCount];
        for(uint8_t i = 0; i < mAnnotationCount; i++)
        {
            mAnnotations[i].Load(file);
        }
    }

    file.ReadByte(&mSize);

    if(!mSize)
        return;

    if(mType == eType::TEXTURE)
    {
        mValue.AsVoid = new uint8_t[4 * mSize];
        file.Read(mValue.AsVoid, 4 * mSize);
    }
    else
    {
        uint32_t totalSize = 4 * mCount * sParamTypeSizeFactor[(uint8_t)mType];
        mValue.AsVoid = new uint8_t[4 * totalSize];
        memset(mValue.AsVoid, 0, 4 * totalSize);

        uint32_t srcStride = (mSize / mCount) * 4;
        uint32_t dstStride = (totalSize / mCount) * 4;
        uint8_t* dst = (uint8_t*)mValue.AsVoid;

        for(uint8_t i = 0; i < mCount; i++)
        {
            file.Read(dst, srcStride);
            dst += dstStride;
        }
    }
}

void Parameter::SaveToFx(EffectWriter& file, bool isGlobal) const
{
    bool isTexture = mType == Parameter::eType::TEXTURE;
    if(isTexture)
    {
        if(isGlobal)
        {
            file.WriteLine("shared texture %s;", mSemantic.Get());
            file.Write("shared %s %s", eType::EnumToString(mType), mName.Get());
        }
        else
        {
            file.WriteLine("texture %s;", mSemantic.Get());
            file.Write("%s %s", eType::EnumToString(mType), mName.Get());
        }
    }
    else if(isGlobal)
        file.Write("shared %s %s", eType::EnumToString(mType), mName.Get());
    else
        file.Write("%s %s", eType::EnumToString(mType), mName.Get());

    if(!mSize)
    {
        if(mCount > 1 && !isTexture)
            file.Write("[%d]", mCount);

        file.Write(" : %s", mSemantic.Get());

        if(mAnnotationCount)
        {
            file.Write(" <");
            for(uint8_t i = 0; i < mAnnotationCount; i++)
            {
                const auto& annotation = mAnnotations[i];

                file.Write("%s %s = ", eAnnotationType::EnumToString(annotation.mType), annotation.mName);

                if(annotation.mType == eAnnotationType::INT)
                    file.Write("%d;", annotation.mValue.AsInt);
                else if(annotation.mType == eAnnotationType::FLOAT)
                    file.Write("%f;", annotation.mValue.AsFloat);
                else
                {
                    //replaces " characters with \"
                    CString newString(512);
                    char* src = annotation.mValue.AsString; char* dst = newString.Get();
                    while(*src)
                    {
                        if(*src == '"')
                        {
                            *dst++ = '\\'; *dst++ = '"';
                        }
                        else
                            *dst++ = *src;

                        src++;
                    }
                    file.Write("\"%s\";", newString.Get());
                }

                if(i != mAnnotationCount - 1)
                    file.Write(" ");
            }
            file.Write(">");
        }

        file.WriteLine(";");
        return;
    }
    else
    {
        auto value = mValue;

        const uint32_t totalSize = 4 * mCount * sParamTypeSizeFactor[(uint8_t)mType];
        const uint32_t stride = (totalSize / (mCount == 0 ? 1 : mCount)) * 4;

        const uint32_t paramCount = mCount + (uint32_t)(isTexture ? (4 * mSize) / sizeof(SamplerState) : 0);

        bool isArray = paramCount > 1;
        if(isArray && !isTexture)
            file.Write("[%d]", paramCount);

        if(!isTexture)
            file.Write(" : %s", mSemantic.Get());

        if(mAnnotationCount)
        {
            file.Write("<");
            for(uint8_t i = 0; i < mAnnotationCount; i++)
            {
                const auto& annotation = mAnnotations[i];

                file.WriteIndented("%s %s = ", eAnnotationType::EnumToString(annotation.mType), annotation.mName);

                if(annotation.mType == eAnnotationType::INT)
                    file.Write("%d;", annotation.mValue.AsInt);
                else if(annotation.mType == eAnnotationType::FLOAT)
                    file.Write("%f;", annotation.mValue.AsFloat);
                else
                {
                    //replaces " characters with \"
                    CString newString(256);
                    char* src = annotation.mValue.AsString; char* dst = newString.Get();
                    while(*src)
                    {
                        if(*src == '"')
                        {
                            *dst++ = '\\'; *dst++ = '"';
                        }
                        else
                            *dst++ = *src;

                        src++;
                    }
                    file.Write("\"%s\";", newString.Get());
                }

                if(i != mAnnotationCount - 1)
                    file.Write(" ");
            }
            file.Write(">");
        }

        file.Write(" = ");

        if(isArray)
        {
            file.NewLine();
            file.PushTab();
            if(isTexture)
            {
                file.WriteLine("sampler_state");
                file.WriteLine("{");
                file.WriteLineIndented("Texture = <%s>;", mSemantic.Get());
            }
            else
            {
                file.WriteLine("{");
            }
        }

        bool samplerStatesWritten[eSamplerStateType::COUNT] {0};

        for(uint8_t i = 0; i < paramCount; i++)
        {
            switch(mType)
            {
                default:
                    __debugbreak();
                break;

                case Parameter::eType::INT:
                    file.WriteIndented("%d", *value.AsInt);
                break;

                case Parameter::eType::FLOAT:
                    file.WriteIndented("%f", *value.AsFloat);
                break;

                case Parameter::eType::VECTOR2:
                {
                    rage::Vector2 v = *value.AsVector2;
                    file.WriteIndented("float2(%f, %f)", v.x, v.y);
                }
                break;

                case Parameter::eType::VECTOR3:
                {
                    rage::Vector3 v = *value.AsVector3;
                    file.WriteIndented("float3(%f, %f, %f)", v.x, v.y, v.z);
                }
                break;

                case Parameter::eType::VECTOR4:
                {
                    rage::Vector4 v = *value.AsVector4;
                    file.WriteIndented("float4(%f, %f, %f, %f)", v.x, v.y, v.z, v.w);
                }
                break;

                case Parameter::eType::TEXTURE:
                {
                    if(samplerStatesWritten[value.AsSamplerState->Type])
                        continue;

                    auto samplerState = value.AsSamplerState;
                    file.WriteIndented("%s = ", eSamplerStateType::EnumToString(samplerState->Type));

                    switch(samplerState->Type)
                    {
                        default:
                            __debugbreak();
                        break;

                        case eSamplerStateType::ADDRESSU:
                        case eSamplerStateType::ADDRESSV:
                        case eSamplerStateType::ADDRESSW:
                            file.Write("%s", eTextureAddress::EnumToString(samplerState->Value.AddressU));
                        break;

                        //integers
                        case eSamplerStateType::MAXMIPLEVEL:
                        case eSamplerStateType::MAXANISOTROPY:
                        case eSamplerStateType::DMAPOFFSET:
                        case eSamplerStateType::DMAPOFFSET2:
                            file.Write("%d", samplerState->Value.BorderColor);
                        break;

                        case eSamplerStateType::BORDERCOLOR:
                            file.Write("0x%x", samplerState->Value.BorderColor);
                        break;

                        case eSamplerStateType::MAGFILTER:
                        case eSamplerStateType::MINFILTER:
                        case eSamplerStateType::MIPFILTER:
                            file.Write("%s", eTextureFilterType::EnumToString(samplerState->Value.MagFilter));
                        break;

                        case eSamplerStateType::MIPMAPLODBIAS:
                            file.Write("%f", samplerState->Value.MipMapLodBias);
                        break;

                        case eSamplerStateType::SRGBTEXTURE:
                            file.Write(samplerState->Value.IsSRGB ? "true" : "false");
                        break;
                    }
                    samplerStatesWritten[value.AsSamplerState->Type] = true;
                }
                break;

                case Parameter::eType::BOOL:
                    file.WriteIndented(value.AsInt ? "true" : "false");
                break;

                case Parameter::eType::MATRIX3X4:
                {
                    rage::Matrix34 mtx = *value.AsMatrix34;
                    file.NewLine();
                    file.PushTab();
                    file.WriteLineIndented("float3x4(%f, %f, %f,", mtx.a.x, mtx.a.y, mtx.a.z);
                    file.WriteLineIndented("         %f, %f, %f,", mtx.b.x, mtx.b.y, mtx.b.z);
                    file.WriteLineIndented("         %f, %f, %f,", mtx.c.x, mtx.c.y, mtx.c.z);
                    file.WriteIndented    ("         %f, %f, %f)", mtx.d.x, mtx.d.y, mtx.d.z);
                    file.PopTab();
                }
                break;

                case Parameter::eType::MATRIX4X4:
                {
                    rage::Matrix44 mtx = *value.AsMatrix44;
                    file.NewLine();
                    file.PushTab();
                    file.WriteLineIndented("float4x4(%f, %f, %f, %f,", mtx.a.x, mtx.a.y, mtx.a.z, mtx.a.w);
                    file.WriteLineIndented("         %f, %f, %f, %f,", mtx.b.x, mtx.b.y, mtx.b.z, mtx.b.w);
                    file.WriteLineIndented("         %f, %f, %f, %f,", mtx.c.x, mtx.c.y, mtx.c.z, mtx.c.w);
                    file.WriteIndented    ("         %f, %f, %f, %f)",  mtx.d.x, mtx.d.y, mtx.d.z, mtx.d.w);
                    file.PopTab();
                }
                break;

                case Parameter::eType::STRING:
                    file.WriteIndented(value.AsString);
                break;
            }

            if(mType == Parameter::eType::TEXTURE)
                value.AsSamplerState++;
            else
                value.AsVoid = (void*)((uintptr_t)value.AsVoid + (uintptr_t)stride);

            if(isArray)
            {
                if(isTexture)
                {
                    file.Write(";");
                    if(i != paramCount - 1)
                        file.NewLine();
                }
                else if(i != paramCount - 1)
                    file.WriteLine(", ");
            }
        }

        if(isArray)
        {
            file.PopTab();
            file.NewLine();
            file.Write("}");
        }
        file.WriteLine(";");
    }
}


void Annotation::Save(OFileStream& file) const
{
    WriteString(file, mName);

    file.WriteByte(&mType);

    if(mType == eAnnotationType::STRING)
    {
        auto nameLen = strlen(mValue.AsString) + 1;
        file.WriteByte(&nameLen);
        file.Write(mValue.AsString, nameLen);
    }
    else
    {
        file.WriteDword(&mValue.AsFloat);
    }
}

void Annotation::Load(IFileStream& file)
{
    mName = ReadString(file);

    uint8_t type;
    file.ReadByte(&type);
    mType = (eAnnotationType::Enum)type;

    if(mType == eAnnotationType::STRING)
    {
        uint8_t nameLen;
        file.ReadByte(&nameLen);
        mValue.AsString = new char[nameLen];
        file.Read(mValue.AsString, nameLen);
    }
    else
    {
        file.ReadDword(&mValue.AsFloat);
    }
}


void EffectTechnique::Save(OFileStream& file) const
{
    WriteString(file, mName);

    uint16_t passCount = mPasses.GetCount();
    file.WriteByte(&passCount);
    for(uint8_t i = 0; i < passCount; i++)
    {
        mPasses[i].Save(file);
    }
}

void EffectTechnique::Load(IFileStream& file)
{
    mName = ReadString(file);
    mNameHash = rage::atStringHash(mName.Get());

    uint8_t passCount;
    file.ReadByte(&passCount);
    mPasses = rage::atArray<EffectPass>(passCount);
    for(uint8_t i = 0; i < passCount; i++)
    {
        mPasses.Append().Load(file);
    }
}

void EffectTechnique::SaveToFx(EffectWriter& file, const Effect& effect) const
{
    file.WriteLine("technique %s", mName.Get());
    file.WriteLine("{");
    file.PushTab();
    for(uint16_t i = 0; i < mPasses.GetCount(); i++)
    {
        mPasses[i].SaveToFx(file, effect, i);
    }
    file.PopTab();
    file.WriteLine("}");
    file.NewLine();
}


void EffectPass::Save(OFileStream& file) const
{
    file.WriteByte(&mVertexProgramIndex);
    file.WriteByte(&mPixelProgramIndex);

    uint16_t stateCount = mRenderStates.GetCount();
    file.WriteByte(&stateCount);
    for(uint8_t i = 0; i < stateCount; i++)
    {
        file.WriteDword(&mRenderStates[i].State);
        file.WriteDword(&mRenderStates[i].Value);
    }
}

void EffectPass::Load(IFileStream& file)
{
    file.ReadByte(&mVertexProgramIndex);
    file.ReadByte(&mPixelProgramIndex);

    uint8_t stateCount;
    file.ReadByte(&stateCount);
    mRenderStates = rage::atArray<RenderState>(stateCount);
    for(uint8_t i = 0; i < stateCount; i++)
    {
        RenderState& renderState = mRenderStates.Append();
        file.ReadDword(&renderState.State);
        file.ReadDword(&renderState.Value);
    }
}

void EffectPass::SaveToFx(EffectWriter& file, const Effect& effect, uint16_t index) const
{
    file.WriteLineIndented("pass p%d", index);

    file.WriteLineIndented("{");
    file.PushTab();

    for(uint16_t i = 0; i < mRenderStates.GetCount(); i++)
    {
        const auto& renderState = mRenderStates[i];

        file.WriteIndented("%s = ", eRenderStateType::EnumToString(renderState.State));

        auto value = renderState.Value;
        switch(renderState.State)
        {
            case eRenderStateType::ZENABLE:
                file.Write("%s", eZBufferType::EnumToString(value.ZEnable));
            break;

            case eRenderStateType::FILLMODE:
                file.Write("%s", eFillMode::EnumToString(value.FillMode));
            break;

            case eRenderStateType::ZWRITEENABLE:
            case eRenderStateType::ALPHATESTENABLE:
            case eRenderStateType::ALPHABLENDENABLE:
            case eRenderStateType::STENCILENABLE:
            case eRenderStateType::SEPARATEALPHABLENDENABLE:
            case eRenderStateType::TWOSIDEDSTENCILMODE:
                file.Write(eGrcBoolValue::EnumToString(value.ZWriteEnable));
            break;

            case eRenderStateType::SRCBLEND:
            case eRenderStateType::DESTBLEND:
                file.Write(eBlendMode::EnumToString(value.SrcBlend));
            break;

            case eRenderStateType::CULLMODE:
                file.Write(eCullMode::EnumToString(value.CullMode));
            break;

            case eRenderStateType::ZFUNC:
            case eRenderStateType::ALPHAFUNC:
            case eRenderStateType::STENCILFUNC:
            case eRenderStateType::CCW_STENCILFUNC:
                file.Write(eCmpFunc::EnumToString(value.ZFunc));
            break;

            case eRenderStateType::ALPHAREF:
            case eRenderStateType::STENCILWRITEMASK:
            case eRenderStateType::STENCILMASK:
            case eRenderStateType::BLENDFACTOR:
                file.Write("%X", value.AlphaRef);
            break;

            case eRenderStateType::STENCILFAIL:
            case eRenderStateType::STENCILZFAIL:
            case eRenderStateType::CCW_STENCILFAIL:
            case eRenderStateType::CCW_STENCILPASS:
                file.Write(eStencilOp::EnumToString(value.StencilFail));
            break;

            case eRenderStateType::STENCILREF:
                file.Write("%d", value.StencilRef);
            break;

            case eRenderStateType::COLORWRITEENABLE:
            case eRenderStateType::COLORWRITEENABLE1:
            case eRenderStateType::COLORWRITEENABLE2:
            case eRenderStateType::COLORWRITEENABLE3:
            {
                if(((uint32_t)value.ColorWriteEnable & 0xF) == 0)
                {
                    file.Write("0");
                    break;
                }

                if(value.ColorWriteEnable & eColorWrite::RED)
                {
                    file.Write("RED");
                }
                if(value.ColorWriteEnable & eColorWrite::GREEN)
                {
                    if(value.ColorWriteEnable & eColorWrite::RED)
                        file.Write(" | ");
                    file.Write("GREEN");
                }
                if(value.ColorWriteEnable & eColorWrite::BLUE)
                {
                    if(value.ColorWriteEnable & eColorWrite::GREEN)
                        file.Write(" | ");
                    file.Write("BLUE");
                }
                if(value.ColorWriteEnable & eColorWrite::ALPHA)
                {
                    if(value.ColorWriteEnable & eColorWrite::BLUE)
                        file.Write(" | ");
                    file.Write("ALPHA");
                }
            }
            break;

            case eRenderStateType::BLENDOP:
            case eRenderStateType::BLENDOPALPHA:
            case eRenderStateType::SRCBLENDALPHA:
            case eRenderStateType::DESTBLENDALPHA:
                file.Write(eBlendOp::EnumToString(value.BlendOp));
            break;

            case eRenderStateType::SLOPESCALEDEPTHBIAS:
            case eRenderStateType::DEPTHBIAS:
            file.Write("%f", value.SlopeScaleDepthBias);
            break;
        }
        file.WriteLine(";");

        if(i == mRenderStates.GetCount() - 1)
            file.NewLine();
    }

    file.WriteLineIndented("VertexShader = VertexShader%d;", mVertexProgramIndex);
    file.WriteLineIndented("PixelShader = PixelShader%d;", mPixelProgramIndex);

    file.PopTab();
    file.WriteLineIndented("}");
}