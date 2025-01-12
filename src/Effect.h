#pragma once
#include "rage/math/Matrix.h"
#include "rage/Array.h"
#include "CString.h"
#include "EffectWriter.h"

class IFileStream;

struct eRenderStateType
{
    enum Enum : uint32_t
    {
        ZENABLE, FILLMODE, ZWRITEENABLE, ALPHATESTENABLE, SRCBLEND, DESTBLEND, CULLMODE, ZFUNC, ALPHAREF, ALPHAFUNC, ALPHABLENDENABLE, STENCILENABLE, STENCILFAIL,
        STENCILZFAIL, STENCILPASS, STENCILFUNC, STENCILREF, STENCILMASK, STENCILWRITEMASK, COLORWRITEENABLE, COLORWRITEENABLE1, COLORWRITEENABLE2, COLORWRITEENABLE3,
        BLENDOP, BLENDOPALPHA, SEPARATEALPHABLENDENABLE, SRCBLENDALPHA, DESTBLENDALPHA, INVALID_28, SLOPESCALEDEPTHBIAS, DEPTHBIAS, BLENDFACTOR, INVALID_32,
        INVALID_33, INVALID_34, TWOSIDEDSTENCILMODE, CCW_STENCILFAIL, CCW_STENCILZFAIL, CCW_STENCILPASS, CCW_STENCILFUNC, INVALID_40, INVALID_41, INVALID_42, COUNT
    };

    static const char* EnumToString(Enum type)
    {
        if((size_t)type <= std::size(msNames))
            return msNames[(uint32_t)type];
        else
            return "INVALID";
    }

    static Enum StringToEnum(const char* string)
    {
        for(size_t i = 0; i < std::size(msNames); i++)
        {
            if(strcmp(string, msNames[i]) == 0)
                return (Enum)i;
        }

        return Enum::COUNT;
    }

private:
    static constexpr const char* msNames[] {"ZEnable", "FillMode", "ZWriteEnable", "AlphaTestEnable", "SrcBlend", "DestBlend", "CullMode", "ZFunc", "AlphaRef",
                                            "AlphaFunc", "AlphaBlendEnable", "StencilEnable", "StencilFail", "StencilZFail", "StencilPass", "StencilFunc",
                                            "StencilRef", "StencilMask", "StencilWriteMask", "ColorWriteEnable", "ColorWriteEnable1", "ColorWriteEnable2",
                                            "ColorWriteEnable3", "BlendOp", "BlendOpAlpha", "SeparateAlphaBlendEnable", "SrcBlendAlpha",
                                            "DestBlendAlpha", "INVALID_28", "SlopeScaleDepthBias", "DepthBias", "BlendFactor", "INVALID_32", "INVALID_33", "INVALID_34",
                                            "TwoSideStencilMode", "CCW_StencilFail", "CCW_StencilZFail", "CCW_StencilPass",
                                            "CCWStencil_Func", "INVALID_40", "INVALID_41", "INVALID_42",};
};

struct eGrcBoolValue
{
    enum Enum : uint32_t
    {
        GRC_FALSE, GRC_TRUE, COUNT
    };

    static const char* EnumToString(Enum type)
    {
        if((size_t)type <= std::size(msNames))
            return msNames[(uint32_t)type];
        else
            return "INVALID";
    }

    static Enum StringToEnum(const char* string)
    {
        for(size_t i = 0; i < std::size(msNames); i++)
        {
            if(strcmp(string, msNames[i]) == 0)
                return (Enum)i;
        }

        return Enum::COUNT;
    }

private:
    static constexpr const char* msNames[] {"false", "true"};
};

struct eZBufferType
{
    enum Enum : uint32_t
    {
        GRC_FALSE, GRC_TRUE, USE_W, COUNT
    };

    static const char* EnumToString(Enum type)
    {
        if((size_t)type <= std::size(msNames))
            return msNames[(uint32_t)type];
        else
            return "INVALID";
    }

    static Enum StringToEnum(const char* string)
    {
        for(size_t i = 0; i < std::size(msNames); i++)
        {
            if(strcmp(string, msNames[i]) == 0)
                return (Enum)i;
        }

        return Enum::COUNT;
    }

private: static constexpr const char* msNames[] {"false", "true", "USEW"};
};

struct eFillMode
{
    BASIC_ENUM_REFLECTION_FIRST_INDEX_1(uint32_t, POINT, WIREFRAME, SOLID)
};

struct eBlendMode
{
    BASIC_ENUM_REFLECTION_FIRST_INDEX_1(uint32_t, ZERO, ONE, SRCCOLOR, INVSRCCOLOR, SRCALPHA, INVSRCALPHA, DESTALPHA, INVDESTALPHA, DESTCOLOR,
                                        INVDESTCOLOR, SRCALPHASAT, BOTHSRCALPHA, BOTHINVSRCALPHA, BLENDFACTOR, INVBLENDFACTOR, SRCCOLOR2, INVSRCCOLOR2)
};
struct eCullMode
{
    BASIC_ENUM_REFLECTION_FIRST_INDEX_1(uint32_t, NONE, CW, CCW)
};

struct eCmpFunc
{
    BASIC_ENUM_REFLECTION_FIRST_INDEX_1(uint32_t, NEVER, LESS, EQUAL, LESSEQUAL, GREATER, NOTEQUAL, GREATEREQUAL, ALWAYS)
};

struct eStencilOp
{
    BASIC_ENUM_REFLECTION_FIRST_INDEX_1(uint32_t, KEEP, ZERO, REPLACE, INCRSAT, DECRSAT, INVERT, INCR, DECR)
};

struct eColorWrite
{
    enum Enum : uint32_t
    {
        NONE = 0x0, RED = 0x1, GREEN = 0x2, BLUE = 0x4, ALPHA = 0x8, ALL = 0xF
    };
};
BITMASK_ENUM_OPERATORS(eColorWrite::Enum)

struct eBlendOp
{
    BASIC_ENUM_REFLECTION_FIRST_INDEX_1(uint32_t, ADD, SUBTRACT, REVSUBTRACT, MIN, MAX)
};

struct RenderState
{
    eRenderStateType::Enum State;
    struct
    {
        union
        {
            eZBufferType::Enum  ZEnable;
            eFillMode::Enum     FillMode;
            eGrcBoolValue::Enum ZWriteEnable;
            eGrcBoolValue::Enum AlphaTestEnable;
            eBlendMode::Enum    SrcBlend;
            eBlendMode::Enum    DestBlend;
            eCullMode::Enum     CullMode;
            eCmpFunc::Enum      ZFunc;
            uint32_t            AlphaRef;
            eCmpFunc::Enum      AlphaFunc;
            eGrcBoolValue::Enum AlphaBlendEnable;
            eGrcBoolValue::Enum StencilEnable;
            eStencilOp::Enum    StencilFail;
            eStencilOp::Enum    StencilZFail;
            eStencilOp::Enum    StencilPass;
            eCmpFunc::Enum      StencilFunc;
            uint32_t            StencilRef;
            uint32_t            StencilMask;
            uint32_t            StencilWriteMask;
            eColorWrite::Enum   ColorWriteEnable;
            eColorWrite::Enum   ColorWriteEnable1;
            eColorWrite::Enum   ColorWriteEnable2;
            eColorWrite::Enum   ColorWriteEnable3;
            eBlendOp::Enum      BlendOp;
            eBlendOp::Enum      BlendOpAlpha;
            eGrcBoolValue::Enum SeparateBlendAlphaEnable;
            eBlendOp::Enum      SrcBlendAlpha;
            eBlendOp::Enum      DestBlendAlpha;
            float               SlopeScaleDepthBias;
            float               DepthBias;
            uint32_t            BlendFactor;
            eGrcBoolValue::Enum TwoSidedStencilMode;
            eStencilOp::Enum    CounterClockwiseStencilFail;
            eStencilOp::Enum    CounterClockwiseStencilZFail;
            eStencilOp::Enum    CounterClockwiseStencilPass;
            eCmpFunc::Enum      CounterClockwiseStencilFunc;
        };
    } Value;
};


class EffectPass
{
public:
    friend class Effect;

    void Save(class OFileStream& file) const;
    void Load(class IFileStream& file);
    void SaveToFx(EffectWriter& file, const class Effect& effect, uint16_t index) const;

private:
    uint8_t mVertexProgramIndex;
    uint8_t mPixelProgramIndex;
    rage::atArray<RenderState> mRenderStates;
};


class EffectTechnique
{
public:
    friend class Effect;

    EffectTechnique() = default;
    ~EffectTechnique() = default;

    void Save(class OFileStream& file) const;
    void Load(class IFileStream& file);
    void SaveToFx(EffectWriter& file, const class Effect& effect) const;

private:
    uint32_t mNameHash;
    CString mName;
    rage::atArray<EffectPass> mPasses;
};


struct eAnnotationType
{
    enum Enum : uint32_t
    {
        INT, FLOAT, STRING, COUNT
    };

    static const char* EnumToString(Enum type)
    {
        if((size_t)type <= std::size(msNames))
            return msNames[(uint32_t)type];
        else
            return "INVALID";
    }

    static Enum StringToEnum(const char* string)
    {
        for(size_t i = 0; i < std::size(msNames); i++)
        {
            if(strcmp(string, msNames[i]) == 0)
                return (Enum)i;
        }

        return Enum::COUNT;
    }

private:
    static constexpr const char* msNames[] {"int", "float", "string"};
};

class Annotation
{
public:
    Annotation() = default;
    ~Annotation()
    {
        if(mType == eAnnotationType::STRING)
        {
            delete[] mValue.AsString;
            mValue.AsString = nullptr;
        }
    }

    Annotation& operator=(const Annotation& rhs)
    {
        mName = rhs.mName;
        mType = rhs.mType;

        if(mType == eAnnotationType::STRING)
        {
            mValue.AsString = strdup(rhs.mValue.AsString);
        }
        else
        {
            mValue.AsVoid = rhs.mValue.AsVoid;
        }

        return *this;
    }

    void Save(class OFileStream& file) const;
    void Load(class IFileStream& file);

    CString mName;
    eAnnotationType::Enum mType;
    struct
    {
        union
        {
            int32_t AsInt;
            float AsFloat;
            char* AsString;
            void* AsVoid;
        };
    } mValue;
};


struct eSamplerStateType
{
    enum Enum : uint32_t
    {
        ADDRESSU, ADDRESSV, ADDRESSW, BORDERCOLOR, MAGFILTER, MINFILTER, MIPFILTER, MIPMAPLODBIAS, MAXMIPLEVEL, MAXANISOTROPY, SRGBTEXTURE, DMAPOFFSET, DMAPOFFSET2, COUNT
    };

    static const char* EnumToString(Enum type)
    {
        if((size_t)type <= std::size(msNames))
            return msNames[(uint32_t)type];
        else
            return "INVALID";
    }

    static Enum StringToEnum(const char* string)
    {
        for(size_t i = 0; i < std::size(msNames); i++)
        {
            if(strcmp(string, msNames[i]) == 0)
                return (Enum)i;
        }

        return Enum::COUNT;
    }

private:
    static constexpr const char* msNames[]
    {"AddressU", "AddressV", "AddressW", "BorderColor", "MagFilter", "MinFilter", "MipFilter", "MipMapLodBias", "MaxMipLevel", "MaxAnisotropy", "SRGBTexture", "DMapOffset", "DMapOffset"};
};

struct eTextureAddress
{
    BASIC_ENUM_REFLECTION_FIRST_INDEX_1(uint32_t, WRAP, MIRROR, CLAMP, BORDER, MIRROR_ONCE)
};

struct eTextureFilterType
{
    BASIC_ENUM_REFLECTION(uint32_t, NONE, POINT, LINEAR, ANISOTROPIC, PYRAMIDAL_QUAD, GAUSSIAN_QUAD, CONVOLUTION_MONO)
};

struct SamplerState
{
    eSamplerStateType::Enum Type;
    struct
    {
        union
        {
            eTextureAddress::Enum AddressU;
            eTextureAddress::Enum AddressV;
            eTextureAddress::Enum AddressW;
            uint32_t BorderColor;
            eTextureFilterType::Enum MagFilter;
            eTextureFilterType::Enum MinFilter;
            eTextureFilterType::Enum MipFilter;
            float MipMapLodBias;
            uint32_t MaxMipLevel;
            uint32_t MaxAnisotropy;
            bool IsSRGB;
            uint32_t DisplacementMapOffset;
        } Value;
    };
};


class Parameter
{
public:
    friend class Effect;

    struct eType
    {
        enum Enum : uint8_t
        {
            NONE, INT, FLOAT, VECTOR2, VECTOR3, VECTOR4, TEXTURE, BOOL, MATRIX3X4, MATRIX4X4, STRING, COUNT
        };

        static const char* EnumToString(Enum type)
        {
            if((size_t)type <= std::size(msNames))
                return msNames[(uint32_t)type];
            else
                return "INVALID";
        }

        static Enum StringToEnum(const char* string)
        {
            for(size_t i = 0; i < std::size(msNames); i++)
            {
                if(strcmp(string, msNames[i]) == 0)
                    return (Enum)i;
            }

            return Enum::COUNT;
        }

    private:
        static constexpr const char* msNames[]
        {"NONE", "int", "float", "float2", "float3", "float4", "sampler", "bool", "float3x4", "float4x4", "string"};
    };

public:
    Parameter() : mType(eType::NONE), mCount(0), mSize(0), mAnnotationCount(0), mNameHash(0), mSemanticHash(0), mValue{.AsInt = 0}, mAnnotations(nullptr)
    {}

    ~Parameter()
    {
        if(mValue.AsVoid)
        {
            delete[] mValue.AsVoid;
            mValue.AsVoid = nullptr;
        }

        if(mAnnotations)
        {
            delete[] mAnnotations;
            mAnnotations = nullptr;
            mAnnotationCount = 0;
        }
    }

    Parameter& operator=(const Parameter& rhs);

    const char* GetName1() const
    {
        return mName.Get();
    };
    const char* GetSemantic() const
    {
        return mSemantic.Get();
    };
    uint32_t GetNameHash() const
    {
        return mNameHash;
    };
    uint32_t GetSemanticHash() const
    {
        return mSemanticHash;
    };

    eType::Enum GetType() const
    {
        return mType;
    }

    uint32_t GetTotalSize() const;

    void Save(OFileStream& file) const;
    void Load(class IFileStream& file);
    void SaveToFx(EffectWriter& file, bool isGlobal = false) const;

private:
    eType::Enum mType;
    uint8_t mCount;
    uint8_t mSize;
    uint8_t mAnnotationCount;
    CString mName;
    CString mSemantic;
    uint32_t mNameHash;
    uint32_t mSemanticHash;
    Annotation* mAnnotations;
    struct
    {
        union
        {
            SamplerState* AsSamplerState;
            int32_t* AsInt;
            float* AsFloat;
            rage::Vector2* AsVector2;
            rage::Vector3* AsVector3;
            rage::Vector4* AsVector4;
            rage::Matrix34* AsMatrix34;
            rage::Matrix44* AsMatrix44;
            char* AsString;
            void* AsVoid;
        };
    } mValue;
};


class GpuProgram
{
public:
    class Param
    {
    public:
        Param() : mType(Parameter::eType::NONE), mUnknown(0), mRegisterIndex(0xFFFF), mNameHash(0)
        {};

        void Save(OFileStream& file, const class Effect& effect) const;
        void Load(class IFileStream& file);

        Parameter::eType::Enum mType;
        uint8_t mUnknown;
        uint16_t mRegisterIndex;
        uint32_t mNameHash;
    };

public:
    GpuProgram() : mParamCount(0), mShaderData()
    {}

    void Save(OFileStream& file, const class Effect& effect) const;
    void Load(class IFileStream& file);

    CString GetDisassembly() const;

protected:
    uint32_t mParamCount;
    rage::atArray<Param> mParams;
    rage::atArray<uint8_t> mShaderData;
};

using VertexProgram = GpuProgram;
using PixelProgram = GpuProgram;


class Effect
{
public:
    Effect(IFileStream& file);
    Effect() = default;
    ~Effect() = default;

    void Save(const std::filesystem::path& filePath) const;
    bool SaveToFx(const std::filesystem::path& filePath) const;

    const Parameter* FindParameterByName(const char* name) const;
    const Parameter* FindParameterByHash(uint32_t hash) const;
    const Parameter* FindGlobalParameterByName(const char* name) const;
    const Parameter* FindGlobalParameterByHash(uint32_t hash) const;
    const Parameter* GetParameterAt(uint32_t index) const;

    CString GetVertexShaderDisassembly(uint32_t index) const;
    CString GetPixelShaderDisassembly(uint32_t index) const;

    static constexpr uint32_t MAGIC = (uint32_t)'axgr';

private:
    rage::atArray<EffectTechnique> mTechniques;
    rage::atArray<Parameter> mParameters;
    rage::atArray<Parameter> mGlobalParameters;
    rage::atArray<VertexProgram> mVertexPrograms;
    rage::atArray<PixelProgram> mPixelPrograms;
};