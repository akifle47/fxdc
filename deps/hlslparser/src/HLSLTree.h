#ifndef HLSL_TREE_H
#define HLSL_TREE_H

//#include "Engine/StringPool.h"
#include "Engine.h"

#include <new>

namespace M4
{

enum HLSLNodeType
{
    HLSLNodeType_Root,
    HLSLNodeType_Declaration,
    HLSLNodeType_Struct,
    HLSLNodeType_StructField,
    HLSLNodeType_Buffer,
    HLSLNodeType_BufferField,
    HLSLNodeType_Function,
    HLSLNodeType_Argument,
    HLSLNodeType_ExpressionStatement,
    HLSLNodeType_Expression,
    HLSLNodeType_ReturnStatement,
    HLSLNodeType_DiscardStatement,
    HLSLNodeType_BreakStatement,
    HLSLNodeType_ContinueStatement,
    HLSLNodeType_IfStatement,
    HLSLNodeType_ForStatement,
    HLSLNodeType_BlockStatement,
    HLSLNodeType_UnaryExpression,
    HLSLNodeType_BinaryExpression,
    HLSLNodeType_ConditionalExpression,
    HLSLNodeType_CastingExpression,
    HLSLNodeType_LiteralExpression,
    HLSLNodeType_IdentifierExpression,
    HLSLNodeType_ConstructorExpression,
    HLSLNodeType_ShaderObjectExpression,
    HLSLNodeType_MemberAccess,
    HLSLNodeType_ArrayAccess,
    HLSLNodeType_FunctionCall,
    HLSLNodeType_StateAssignment,
    HLSLNodeType_SamplerState,
    HLSLNodeType_Pass,
    HLSLNodeType_Technique,
    HLSLNodeType_Annotation,
    HLSLNodeType_Attribute,
    HLSLNodeType_Pipeline,
    HLSLNodeType_Stage,
};

enum HLSLTypeDimension
{
    HLSLTypeDimension_None,
    HLSLTypeDimension_Scalar,
    HLSLTypeDimension_Vector2,
    HLSLTypeDimension_Vector3,
    HLSLTypeDimension_Vector4,
    HLSLTypeDimension_Matrix2x2,
    HLSLTypeDimension_Matrix3x3,
    HLSLTypeDimension_Matrix4x4,
    HLSLTypeDimension_Matrix4x3,
    HLSLTypeDimension_Matrix4x2
};
    
enum HLSLBaseType
{
    HLSLBaseType_Unknown,
    HLSLBaseType_Void,    
    HLSLBaseType_Float,
    HLSLBaseType_FirstNumeric = HLSLBaseType_Float,
    HLSLBaseType_Float2,
    HLSLBaseType_Float3,
    HLSLBaseType_Float4,
	HLSLBaseType_Float2x2,
    HLSLBaseType_Float3x3,
    HLSLBaseType_Float4x4,
    HLSLBaseType_Float4x3,
    HLSLBaseType_Float4x2,
    HLSLBaseType_Half,
    HLSLBaseType_Half2,
    HLSLBaseType_Half3,
    HLSLBaseType_Half4,
	HLSLBaseType_Half2x2,
    HLSLBaseType_Half3x3,
    HLSLBaseType_Half4x4,
    HLSLBaseType_Half4x3,
    HLSLBaseType_Half4x2,
    HLSLBaseType_Bool,
    HLSLBaseType_FirstInteger = HLSLBaseType_Bool,
	HLSLBaseType_Bool2,
	HLSLBaseType_Bool3,
	HLSLBaseType_Bool4,
    HLSLBaseType_Int,
    HLSLBaseType_Int2,
    HLSLBaseType_Int3,
    HLSLBaseType_Int4,
    HLSLBaseType_Uint,
    HLSLBaseType_Uint2,
    HLSLBaseType_Uint3,
    HLSLBaseType_Uint4,
    /*HLSLBaseType_Short,   // @@ Separate dimension from Base type, this is getting out of control.
    HLSLBaseType_Short2,
    HLSLBaseType_Short3,
    HLSLBaseType_Short4,
    HLSLBaseType_Ushort,
    HLSLBaseType_Ushort2,
    HLSLBaseType_Ushort3,
    HLSLBaseType_Ushort4,*/
    HLSLBaseType_LastInteger = HLSLBaseType_Uint4,
    HLSLBaseType_LastNumeric = HLSLBaseType_Uint4,
    HLSLBaseType_String,
    HLSLBaseType_Texture,
    HLSLBaseType_Sampler,           // @@ use type inference to determine sampler type.
    HLSLBaseType_Sampler2D,
    HLSLBaseType_Sampler3D,
    HLSLBaseType_SamplerCube,
    HLSLBaseType_Sampler2DShadow,
    HLSLBaseType_Sampler2DMS,
    HLSLBaseType_Sampler2DArray,
    HLSLBaseType_VertexShader,
    HLSLBaseType_PixelShader,
    HLSLBaseType_UserDefined,       // struct
    HLSLBaseType_Expression,        // type argument for defined() sizeof() and typeof().
    HLSLBaseType_Auto,
    
    HLSLBaseType_Count,
    HLSLBaseType_NumericCount = HLSLBaseType_LastNumeric - HLSLBaseType_FirstNumeric + 1
};
    
//only care about the ones supported by gta 4
enum HLSLAnnotationType
{
    HLSLAnnotationType_Int = HLSLBaseType_Int,
    HLSLAnnotationType_Float = HLSLBaseType_Float,
    HLSLAnnotationType_String = HLSLBaseType_String,
};

extern const HLSLTypeDimension BaseTypeDimension[HLSLBaseType_Count];
extern const HLSLBaseType ScalarBaseType[HLSLBaseType_Count];

inline bool IsSamplerType(HLSLBaseType baseType)
{
    return baseType == HLSLBaseType_Sampler ||
           baseType == HLSLBaseType_Sampler2D ||
           baseType == HLSLBaseType_Sampler3D ||
           baseType == HLSLBaseType_SamplerCube ||
           baseType == HLSLBaseType_Sampler2DShadow ||
           baseType == HLSLBaseType_Sampler2DMS ||
           baseType == HLSLBaseType_Sampler2DArray;
}

inline bool IsMatrixType(HLSLBaseType baseType)
{
    return baseType == HLSLBaseType_Float3x3 || baseType == HLSLBaseType_Float4x4 || baseType == HLSLBaseType_Float4x3 || baseType == HLSLBaseType_Float4x2 ||
        baseType == HLSLBaseType_Half3x3 || baseType == HLSLBaseType_Half4x4 || baseType == HLSLBaseType_Half4x3 || baseType == HLSLBaseType_Half4x2;
}

inline bool IsScalarType( HLSLBaseType baseType )
{
	return  baseType == HLSLBaseType_Float ||
			baseType == HLSLBaseType_Half ||
			baseType == HLSLBaseType_Bool ||
			baseType == HLSLBaseType_Int ||
			baseType == HLSLBaseType_Uint;
}

inline bool IsVectorType( HLSLBaseType baseType )
{
	return  baseType == HLSLBaseType_Float2 ||
		baseType == HLSLBaseType_Float3 ||
		baseType == HLSLBaseType_Float4 ||
		baseType == HLSLBaseType_Half2 ||
		baseType == HLSLBaseType_Half3 ||
		baseType == HLSLBaseType_Half4 ||
		baseType == HLSLBaseType_Bool2 ||
		baseType == HLSLBaseType_Bool3 ||
		baseType == HLSLBaseType_Bool4 ||
		baseType == HLSLBaseType_Int2  ||
		baseType == HLSLBaseType_Int3  ||
		baseType == HLSLBaseType_Int4  ||
		baseType == HLSLBaseType_Uint2 ||
		baseType == HLSLBaseType_Uint3 ||
		baseType == HLSLBaseType_Uint4;
}


enum HLSLBinaryOp
{
    HLSLBinaryOp_And,
    HLSLBinaryOp_Or,
    HLSLBinaryOp_Add,
    HLSLBinaryOp_Sub,
    HLSLBinaryOp_Mul,
    HLSLBinaryOp_Div,
    HLSLBinaryOp_Mod,
    HLSLBinaryOp_Less,
    HLSLBinaryOp_Greater,
    HLSLBinaryOp_LessEqual,
    HLSLBinaryOp_GreaterEqual,
    HLSLBinaryOp_Equal,
    HLSLBinaryOp_NotEqual,
    HLSLBinaryOp_BitAnd,
    HLSLBinaryOp_BitOr,
    HLSLBinaryOp_BitXor,
    HLSLBinaryOp_Assign,
    HLSLBinaryOp_AddAssign,
    HLSLBinaryOp_SubAssign,
    HLSLBinaryOp_MulAssign,
    HLSLBinaryOp_DivAssign,
};

inline bool IsCompareOp( HLSLBinaryOp op )
{
	return op == HLSLBinaryOp_Less ||
		op == HLSLBinaryOp_Greater ||
		op == HLSLBinaryOp_LessEqual ||
		op == HLSLBinaryOp_GreaterEqual ||
		op == HLSLBinaryOp_Equal ||
		op == HLSLBinaryOp_NotEqual;
}

inline bool IsArithmeticOp( HLSLBinaryOp op )
{
    return op == HLSLBinaryOp_Add ||
        op == HLSLBinaryOp_Sub ||
        op == HLSLBinaryOp_Mul ||
        op == HLSLBinaryOp_Div ||
        op == HLSLBinaryOp_Mod;
}

inline bool IsLogicOp( HLSLBinaryOp op )
{
    return op == HLSLBinaryOp_And ||
        op == HLSLBinaryOp_Or;
}

inline bool IsAssignOp( HLSLBinaryOp op )
{
    return op == HLSLBinaryOp_Assign ||
        op == HLSLBinaryOp_AddAssign ||
        op == HLSLBinaryOp_SubAssign ||
        op == HLSLBinaryOp_MulAssign ||
        op == HLSLBinaryOp_DivAssign;
}

    
enum HLSLUnaryOp
{
    HLSLUnaryOp_Negative,       // -x
    HLSLUnaryOp_Positive,       // +x
    HLSLUnaryOp_Not,            // !x
    HLSLUnaryOp_PreIncrement,   // ++x
    HLSLUnaryOp_PreDecrement,   // --x
    HLSLUnaryOp_PostIncrement,  // x++
    HLSLUnaryOp_PostDecrement,  // x++
    HLSLUnaryOp_BitNot,         // ~x
};

enum HLSLArgumentModifier
{
    HLSLArgumentModifier_None,
    HLSLArgumentModifier_In,
    HLSLArgumentModifier_Out,
    HLSLArgumentModifier_Inout,
    HLSLArgumentModifier_Uniform,
    HLSLArgumentModifier_Const,
};

enum HLSLTypeFlags
{
    HLSLTypeFlag_None = 0,
    HLSLTypeFlag_Const = 0x01,
    HLSLTypeFlag_Static = 0x02,
    //HLSLTypeFlag_Uniform = 0x04,
    //HLSLTypeFlag_Extern = 0x10,
    //HLSLTypeFlag_Volatile = 0x20,
    HLSLTypeFlag_Shared = 0x40,
    //HLSLTypeFlag_Precise = 0x80,

    HLSLTypeFlag_Input = 0x100,
    HLSLTypeFlag_Output = 0x200,

    // Interpolation modifiers.
    HLSLTypeFlag_Linear = 0x10000,
    HLSLTypeFlag_Centroid = 0x20000,
    HLSLTypeFlag_NoInterpolation = 0x40000,
    HLSLTypeFlag_NoPerspective = 0x80000,
    HLSLTypeFlag_Sample = 0x100000,

    // Misc.
    HLSLTypeFlag_NoPromote = 0x200000,
};

enum HLSLAttributeType
{
    HLSLAttributeType_Unknown,
    HLSLAttributeType_Unroll,
    HLSLAttributeType_Branch,
    HLSLAttributeType_Flatten,
    HLSLAttributeType_NoFastMath,
};

enum HLSLAddressSpace
{
    HLSLAddressSpace_Undefined,
    HLSLAddressSpace_Constant,
    HLSLAddressSpace_Device,
    HLSLAddressSpace_Thread,
    HLSLAddressSpace_Shared,
};


struct HLSLNode;
struct HLSLRoot;
struct HLSLStatement;
struct HLSLAttribute;
struct HLSLDeclaration;
struct HLSLStruct;
struct HLSLStructField;
struct HLSLBuffer;
struct HLSLFunction;
struct HLSLArgument;
struct HLSLExpressionStatement;
struct HLSLExpression;
struct HLSLBinaryExpression;
struct HLSLLiteralExpression;
struct HLSLIdentifierExpression;
struct HLSLConstructorExpression;
struct HLSLFunctionCall;
struct HLSLArrayAccess;
struct HLSLAttribute;

struct HLSLType
{
    explicit HLSLType(HLSLBaseType _baseType = HLSLBaseType_Unknown)
    { 
        baseType    = _baseType;
        samplerType = HLSLBaseType_Float;
        typeName    = NULL;
        array       = false;
        arraySize   = NULL;
        flags       = 0;
        addressSpace = HLSLAddressSpace_Undefined;
    }
    HLSLBaseType        baseType;
    HLSLBaseType        samplerType;    // Half or Float
    const char*         typeName;       // For user defined types.
    bool                array;
    HLSLExpression*     arraySize;
    int                 flags;
    HLSLAddressSpace    addressSpace;
};

inline bool IsSamplerType(const HLSLType & type)
{
    return IsSamplerType(type.baseType);
}

inline bool IsScalarType(const HLSLType & type)
{
	return IsScalarType(type.baseType);
}

inline bool IsVectorType(const HLSLType & type)
{
	return IsVectorType(type.baseType);
}


/** Base class for all nodes in the HLSL AST */
struct HLSLNode
{
    HLSLNodeType        nodeType;
    const char*         fileName;
    int                 line;
};

struct HLSLRoot : public HLSLNode
{
    static const HLSLNodeType s_type = HLSLNodeType_Root;
    HLSLRoot()          { statement = NULL; }
    HLSLStatement*      statement;          // First statement.
};

struct HLSLStatement : public HLSLNode
{
    HLSLStatement() 
    { 
        nextStatement   = NULL; 
        attributes      = NULL;
        hidden          = false;
    }
    HLSLStatement*      nextStatement;      // Next statement in the block.
    HLSLAttribute*      attributes;
    mutable bool        hidden;
};

struct HLSLAttribute : public HLSLNode
{
    static const HLSLNodeType s_type = HLSLNodeType_Attribute;
	HLSLAttribute()
	{
		attributeType = HLSLAttributeType_Unknown;
		argument      = NULL;
		nextAttribute = NULL;
	}
    HLSLAttributeType   attributeType;
    HLSLExpression*     argument;
    HLSLAttribute*      nextAttribute;
};

struct HLSLAnnotation : public HLSLNode
{
    static const HLSLNodeType s_type = HLSLNodeType_Annotation;
    HLSLAnnotation()
    {
        name = NULL;
        type = HLSLAnnotationType_Int;
        iValue = 0;
        nextAnnotation = NULL;
    }
    const char*        name;
    HLSLAnnotationType type;
    union
    {
        int         iValue;
        float       fValue;
        const char* sValue;
    };
    HLSLAnnotation* nextAnnotation;
};

struct HLSLDeclaration : public HLSLStatement
{
    static const HLSLNodeType s_type = HLSLNodeType_Declaration;
    HLSLDeclaration()
    {
        name            = NULL;
        registerIndex   = -1;
        semantic        = NULL;
        nextDeclaration = NULL;
        assignment      = NULL;
        buffer          = NULL;
        annotations     = NULL;
    }
    const char*         name;
    HLSLType            type;
    int                 registerIndex;
    const char*         semantic;
    HLSLDeclaration*    nextDeclaration;    // If multiple variables declared on a line.
    HLSLExpression*     assignment;
    HLSLBuffer*         buffer;
    HLSLAnnotation*     annotations;
};

struct HLSLStruct : public HLSLStatement
{
    static const HLSLNodeType s_type = HLSLNodeType_Struct;
    HLSLStruct()
    {
        name            = NULL;
        field           = NULL;
    }
    const char*         name;
    HLSLStructField*    field;              // First field in the structure.
};

struct HLSLStructField : public HLSLNode
{
    static const HLSLNodeType s_type = HLSLNodeType_StructField;
    HLSLStructField()
    {
        name            = NULL;
        semantic        = NULL;
        sv_semantic     = NULL;
        nextField       = NULL;
        hidden          = false;
    }
    const char*         name;
    HLSLType            type;
    const char*         semantic;
    const char*         sv_semantic;
    HLSLStructField*    nextField;      // Next field in the structure.
    bool                hidden;
};

/** A cbuffer or tbuffer declaration. */
struct HLSLBuffer : public HLSLStatement
{
    static const HLSLNodeType s_type = HLSLNodeType_Buffer;
    HLSLBuffer()
    {
        name            = NULL;
        registerName    = NULL;
        field           = NULL;
    }
    const char*         name;
    const char*         registerName;
    HLSLDeclaration*    field;
};


/** Function declaration */
struct HLSLFunction : public HLSLStatement
{
    static const HLSLNodeType s_type = HLSLNodeType_Function;
    HLSLFunction()
    {
        name            = NULL;
        semantic        = NULL;
        sv_semantic     = NULL;
        statement       = NULL;
        argument        = NULL;
        numArguments    = 0;
        numOutputArguments = 0;
        forward         = NULL;
    }
    const char*         name;
    HLSLType            returnType;
    const char*         semantic;
    const char*         sv_semantic;
    int                 numArguments;
    int                 numOutputArguments;     // Includes out and inout arguments.
    HLSLArgument*       argument;
    HLSLStatement*      statement;
    HLSLFunction*       forward; // Which HLSLFunction this one forward-declares
};

/** Declaration of an argument to a function. */
struct HLSLArgument : public HLSLNode
{
    static const HLSLNodeType s_type = HLSLNodeType_Argument;
    HLSLArgument()
    {
        name            = NULL;
        modifier        = HLSLArgumentModifier_None;
        semantic        = NULL;
        sv_semantic     = NULL;
        defaultValue    = NULL;
        nextArgument    = NULL;
        hidden          = false;
    }
    const char*             name;
    HLSLArgumentModifier    modifier;
    HLSLType                type;
    const char*             semantic;
    const char*             sv_semantic;
    HLSLExpression*         defaultValue;
    HLSLArgument*           nextArgument;
    bool                    hidden;
};

/** A expression which forms a complete statement. */
struct HLSLExpressionStatement : public HLSLStatement
{
    static const HLSLNodeType s_type = HLSLNodeType_ExpressionStatement;
    HLSLExpressionStatement()
    {
        expression = NULL;
    }
    HLSLExpression*     expression;
};

struct HLSLReturnStatement : public HLSLStatement
{
    static const HLSLNodeType s_type = HLSLNodeType_ReturnStatement;
    HLSLReturnStatement()
    {
        expression = NULL;
    }
    HLSLExpression*     expression;
};

struct HLSLDiscardStatement : public HLSLStatement
{
    static const HLSLNodeType s_type = HLSLNodeType_DiscardStatement;
};

struct HLSLBreakStatement : public HLSLStatement
{
    static const HLSLNodeType s_type = HLSLNodeType_BreakStatement;
};

struct HLSLContinueStatement : public HLSLStatement
{
    static const HLSLNodeType s_type = HLSLNodeType_ContinueStatement;
};

struct HLSLIfStatement : public HLSLStatement
{
    static const HLSLNodeType s_type = HLSLNodeType_IfStatement;
    HLSLIfStatement()
    {
        condition     = NULL;
        statement     = NULL;
        elseStatement = NULL;
        isStatic      = false;
    }
    HLSLExpression*     condition;
    HLSLStatement*      statement;
    HLSLStatement*      elseStatement;
    bool                isStatic;
};

struct HLSLForStatement : public HLSLStatement
{
    static const HLSLNodeType s_type = HLSLNodeType_ForStatement;
    HLSLForStatement()
    {
        initialization = NULL;
        condition = NULL;
        increment = NULL;
        statement = NULL;
    }
    HLSLDeclaration*    initialization;
    HLSLExpression*     condition;
    HLSLExpression*     increment;
    HLSLStatement*      statement;
};

struct HLSLBlockStatement : public HLSLStatement
{
    static const HLSLNodeType s_type = HLSLNodeType_BlockStatement;
    HLSLBlockStatement()
    {
        statement = NULL;
    }
    HLSLStatement*      statement;
};


/** Base type for all types of expressions. */
struct HLSLExpression : public HLSLNode
{
    static const HLSLNodeType s_type = HLSLNodeType_Expression;
    HLSLExpression()
    {
        nextExpression = NULL;
    }
    HLSLType            expressionType;
    HLSLExpression*     nextExpression; // Used when the expression is part of a list, like in a function call.
};

struct HLSLUnaryExpression : public HLSLExpression
{
    static const HLSLNodeType s_type = HLSLNodeType_UnaryExpression;
    HLSLUnaryExpression()
    {
        expression = NULL;
    }
    HLSLUnaryOp         unaryOp;
    HLSLExpression*     expression;
};

struct HLSLBinaryExpression : public HLSLExpression
{
    static const HLSLNodeType s_type = HLSLNodeType_BinaryExpression;
    HLSLBinaryExpression()
    {
        expression1 = NULL;
        expression2 = NULL;
    }
    HLSLBinaryOp        binaryOp;
    HLSLExpression*     expression1;
    HLSLExpression*     expression2;
};

/** ? : construct */
struct HLSLConditionalExpression : public HLSLExpression
{
    static const HLSLNodeType s_type = HLSLNodeType_ConditionalExpression;
    HLSLConditionalExpression()
    {
        condition       = NULL;
        trueExpression  = NULL;
        falseExpression = NULL;
    }
    HLSLExpression*     condition;
    HLSLExpression*     trueExpression;
    HLSLExpression*     falseExpression;
};

struct HLSLCastingExpression : public HLSLExpression
{
    static const HLSLNodeType s_type = HLSLNodeType_CastingExpression;
    HLSLCastingExpression()
    {
        expression = NULL;
    }
    HLSLType            type;
    HLSLExpression*     expression;
};

/** Float, integer, boolean, etc. literal constant. */
struct HLSLLiteralExpression : public HLSLExpression
{
    static const HLSLNodeType s_type = HLSLNodeType_LiteralExpression;
    HLSLBaseType        type;   // Note, not all types can be literals.
    union
    {
        bool            bValue;
        float           fValue;
        int             iValue;
    };
};

/** An identifier, typically a variable name or structure field name. */
struct HLSLIdentifierExpression : public HLSLExpression
{
    static const HLSLNodeType s_type = HLSLNodeType_IdentifierExpression;
    HLSLIdentifierExpression()
    {
        name     = NULL;
        global  = false;
    }
    const char*         name;
    bool                global; // This is a global variable.
};

/** float2(1, 2) */
struct HLSLConstructorExpression : public HLSLExpression
{
    static const HLSLNodeType s_type = HLSLNodeType_ConstructorExpression;
	HLSLConstructorExpression()
	{
		argument = NULL;
	}
    HLSLType            type;
    HLSLExpression*     argument;
};

struct HLSLShaderObjectExpression : public HLSLExpression
{
    static const HLSLNodeType s_type = HLSLNodeType_ShaderObjectExpression;
    HLSLShaderObjectExpression()
    {
        source = NULL;
    }
    const char* source;
};

/** object.member **/
struct HLSLMemberAccess : public HLSLExpression
{
    static const HLSLNodeType s_type = HLSLNodeType_MemberAccess;
	HLSLMemberAccess()
	{
		object  = NULL;
		field   = NULL;
		swizzle = false;
	}
    HLSLExpression*     object;
    const char*         field;
    bool                swizzle;
};

/** array[index] **/
struct HLSLArrayAccess : public HLSLExpression
{
    static const HLSLNodeType s_type = HLSLNodeType_ArrayAccess;
	HLSLArrayAccess()
	{
		array = NULL;
		index = NULL;
	}
    HLSLExpression*     array;
    HLSLExpression*     index;
};

struct HLSLFunctionCall : public HLSLExpression
{
    static const HLSLNodeType s_type = HLSLNodeType_FunctionCall;
	HLSLFunctionCall()
	{
		function     = NULL;
		argument     = NULL;
		numArguments = 0;
	}
    const HLSLFunction* function;
    HLSLExpression*     argument;
    int                 numArguments;
};

struct HLSLStateAssignment : public HLSLNode
{
    static const HLSLNodeType s_type = HLSLNodeType_StateAssignment;
    HLSLStateAssignment()
    {
        stateName = NULL;
        sValue = NULL;
        nextStateAssignment = NULL;
    }

    const char*             stateName;
    int                     d3dRenderState;
    union {
        int                 iValue;
        float               fValue;
        const char *        sValue;
    };
    HLSLStateAssignment*    nextStateAssignment;
};

struct HLSLSamplerState : public HLSLExpression // @@ Does this need to be an expression? Does it have a type? I guess type is useful.
{
    static const HLSLNodeType s_type = HLSLNodeType_SamplerState;
    HLSLSamplerState()
    {
        numStateAssignments = 0;
        stateAssignments = NULL;
    }

    int                     numStateAssignments;
    HLSLStateAssignment*    stateAssignments;
};

struct HLSLPass : public HLSLNode
{
    static const HLSLNodeType s_type = HLSLNodeType_Pass;
    HLSLPass()
    {
        name = NULL;
        numStateAssignments = 0;
        stateAssignments = NULL;
        nextPass = NULL;
    }
    
    const char*             name;
    int                     numStateAssignments;
    HLSLStateAssignment*    stateAssignments;
    HLSLPass*               nextPass;
};

struct HLSLTechnique : public HLSLStatement
{
    static const HLSLNodeType s_type = HLSLNodeType_Technique;
    HLSLTechnique()
    {
        name = NULL;
        numPasses = 0;
        passes = NULL;
    }

    const char*         name;
    int                 numPasses;
    HLSLPass*           passes;
};

struct HLSLPipeline : public HLSLStatement
{
    static const HLSLNodeType s_type = HLSLNodeType_Pipeline;
    HLSLPipeline()
    {
        name = NULL;
        numStateAssignments = 0;
        stateAssignments = NULL;
    }
    
    const char*             name;
    int                     numStateAssignments;
    HLSLStateAssignment*    stateAssignments;
};

struct HLSLStage : public HLSLStatement
{
    static const HLSLNodeType s_type = HLSLNodeType_Stage;
    HLSLStage()
    {
        name = NULL;
        statement = NULL;
        inputs = NULL;
        outputs = NULL;
    }

    const char*             name;
    HLSLStatement*          statement;
    HLSLDeclaration*        inputs;
    HLSLDeclaration*        outputs;
};


/**
 * Abstract syntax tree for parsed HLSL code.
 */
class HLSLTree
{

public:

    explicit HLSLTree(Allocator* allocator);
    ~HLSLTree();

    /** Adds a string to the string pool used by the tree. */
    const char* AddString(const char* string);
    const char* AddStringFormat(const char* string, ...);

    /** Returns true if the string is contained within the tree. */
    bool GetContainsString(const char* string) const;

    /** Returns the root block in the tree */
    HLSLRoot* GetRoot() const;

    /** Adds a new node to the tree with the specified type. */
    template <class T>
    T* AddNode(const char* fileName, int line)
    {
        HLSLNode* node = new (AllocateMemory(sizeof(T))) T();
        node->nodeType  = T::s_type;
        node->fileName  = fileName;
        node->line      = line;
        return static_cast<T*>(node);
    }

    HLSLFunction * FindFunction(const char * name);
    HLSLDeclaration * FindGlobalDeclaration(const char * name, HLSLBuffer ** buffer_out = NULL);
    HLSLStruct * FindGlobalStruct(const char * name);
    HLSLTechnique * FindTechnique(const char * name);
    HLSLPipeline * FindFirstPipeline();
    HLSLPipeline * FindNextPipeline(HLSLPipeline * current);
    HLSLPipeline * FindPipeline(const char * name);
    HLSLBuffer * FindBuffer(const char * name);

    bool GetExpressionValue(HLSLExpression * expression, int & value);
    int GetExpressionValue(HLSLExpression * expression, float values[4]);

    bool NeedsFunction(const char * name);

private:

    void* AllocateMemory(size_t size);
    void  AllocatePage();

private:

    static const size_t s_nodePageSize = 1024 * 4;

    struct NodePage
    {
        NodePage*   next;
        char        buffer[s_nodePageSize];
    };

    Allocator*      m_allocator;
    StringPool      m_stringPool;
    HLSLRoot*       m_root;

    NodePage*       m_firstPage;
    NodePage*       m_currentPage;
    size_t          m_currentPageOffset;

};



class HLSLTreeVisitor
{
public:
    virtual void VisitType(HLSLType & type);

    virtual void VisitRoot(HLSLRoot * node);
    virtual void VisitTopLevelStatement(HLSLStatement * node);
    virtual void VisitStatements(HLSLStatement * statement);
    virtual void VisitStatement(HLSLStatement * node);
    virtual void VisitDeclaration(HLSLDeclaration * node);
    virtual void VisitStruct(HLSLStruct * node);
    virtual void VisitStructField(HLSLStructField * node);
    virtual void VisitBuffer(HLSLBuffer * node);
    //virtual void VisitBufferField(HLSLBufferField * node);
    virtual void VisitFunction(HLSLFunction * node);
    virtual void VisitArgument(HLSLArgument * node);
    virtual void VisitExpressionStatement(HLSLExpressionStatement * node);
    virtual void VisitExpression(HLSLExpression * node);
    virtual void VisitReturnStatement(HLSLReturnStatement * node);
    virtual void VisitDiscardStatement(HLSLDiscardStatement * node);
    virtual void VisitBreakStatement(HLSLBreakStatement * node);
    virtual void VisitContinueStatement(HLSLContinueStatement * node);
    virtual void VisitIfStatement(HLSLIfStatement * node);
    virtual void VisitForStatement(HLSLForStatement * node);
    virtual void VisitBlockStatement(HLSLBlockStatement * node);
    virtual void VisitUnaryExpression(HLSLUnaryExpression * node);
    virtual void VisitBinaryExpression(HLSLBinaryExpression * node);
    virtual void VisitConditionalExpression(HLSLConditionalExpression * node);
    virtual void VisitCastingExpression(HLSLCastingExpression * node);
    virtual void VisitLiteralExpression(HLSLLiteralExpression * node);
    virtual void VisitIdentifierExpression(HLSLIdentifierExpression * node);
    virtual void VisitConstructorExpression(HLSLConstructorExpression * node);
    virtual void VisitMemberAccess(HLSLMemberAccess * node);
    virtual void VisitArrayAccess(HLSLArrayAccess * node);
    virtual void VisitFunctionCall(HLSLFunctionCall * node);
    virtual void VisitStateAssignment(HLSLStateAssignment * node);
    virtual void VisitSamplerState(HLSLSamplerState * node);
    virtual void VisitPass(HLSLPass * node);
    virtual void VisitTechnique(HLSLTechnique * node);
    virtual void VisitPipeline(HLSLPipeline * node);


    virtual void VisitFunctions(HLSLRoot * root);
    virtual void VisitParameters(HLSLRoot * root);

    HLSLFunction * FindFunction(HLSLRoot * root, const char * name);
    HLSLDeclaration * FindGlobalDeclaration(HLSLRoot * root, const char * name);
    HLSLStruct * FindGlobalStruct(HLSLRoot * root, const char * name);
};


// Tree transformations:
extern void PruneTree(HLSLTree* tree, const char* entryName0, const char* entryName1 = NULL);
extern void SortTree(HLSLTree* tree);
extern void GroupParameters(HLSLTree* tree);
extern void HideUnusedArguments(HLSLFunction * function);
extern bool EmulateAlphaTest(HLSLTree* tree, const char* entryName, float alphaRef = 0.5f);
extern void FlattenExpressions(HLSLTree* tree);
    
} // M4

#endif
