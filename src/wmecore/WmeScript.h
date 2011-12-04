// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeScriptHeader_H__
#define __WmeScriptHeader_H__


#define ISTR(inst_name) inst_name, #inst_name

#define WME_SCRIPT_MAGIC_1	0x32454D57 // WME2
#define WME_SCRIPT_MAGIC_2	0x52435343 // CSCR
#define WME_SCRIPT_VERSION	0x0100

namespace Wme
{
	typedef enum ScriptInstruction
	{
		SI_NOP = 0,
		SI_JUMP, // int Address
		SI_CHANGE_CONTEXT, // int File, int Line
		SI_POP_VAR, // string VarName
		SI_POP,
		SI_POP_BY_REF, // pop VarRef, pop Value
		SI_CHECK_PARAMS, // int NumParams
		SI_PUSH_NULL,
		SI_PUSH_THIS,
		SI_PUSH_VAR_REF, // string VarName
		SI_PUSH_INT, // int IntConst
		SI_PUSH_DOUBLE, // double DoubleConst
		SI_PUSH_BOOL, // byte BoolConst
		SI_PUSH_STRING, // string StringConst
		SI_PUSH_PROP_REF, // pop ObjRef, pop PropName
		SI_RETURN,		
		SI_DEF_VAR, // string VarName
		SI_DEF_GLOB, // string VarName
		SI_DUP_STACK,
		SI_JUMP_TRUE, // int address -> pop value, jump if true
		SI_NEW, // pop ClassName, pop NumArgs, pop Args ... push Result

		SI_CALL_BY_NAME, // pop Name, pop NumArgs, pop Args ... push Result
		SI_CALL_BY_REF, // pop PropRef, pop NumArgs, pop Args ... push Result

		SI_INC_POST, // pop Ref, push Value, increment
		SI_DEC_POST, // pop Ref, push Value, decrement
		SI_INC_PRE, // pop Ref, increment, push Value
		SI_DEC_PRE, // pop Ref, decrement, push Value

		SI_TYPEOF, // pop Value, push TypeName

		SI_ADD, // pop Val1, pop Val2, push Result
		SI_SUB, // pop Val1, pop Val2, push Result (Val1 - Val2)	
		SI_MUL, // pop Val1, pop Val2, push Result (Val1 * Val2)
		SI_DIV, // pop Val1, pop Val2, push Result (Val1 / Val2)
		SI_MOD, // pop Val1, pop Val2, push Result (Val1 % Val2)
		SI_NEG, // pop Val, negate, push Result

		SI_ADD_REF, // pop VarRef, pop Val, Var+=Val, push Result
		SI_SUB_REF, // pop VarRef, pop Val, Var-=Val, push Result
		SI_MUL_REF, // pop VarRef, pop Val, Var*=Val, push Result
		SI_DIV_REF, // pop VarRef, pop Val, Var/=Val, push Result	
		SI_MOD_REF, // pop VarRef, pop Val, Var%=Val, push Result	

		SI_CMP_LESS,
		SI_CMP_GREATER,
		SI_CMP_LESS_EQUAL,
		SI_CMP_GREATER_EQUAL,
		SI_IN,
		SI_CMP_EQUAL, // pop val1, pop val2, push comparison
		SI_CMP_NOT_EQUAL,
		SI_CMP_STRICT_EQUAL,
		SI_CMP_STRICT_NOT_EQUAL,

		SI_AND,
		SI_OR,

		SI_PUSH_ITERATOR, // peek ObjRef, push IteratorID
		SI_ITERATOR_NEXT, // peek IteratorID, peek ObjRef, push PropName (or null)

		SI_START_GLOBAL_SCOPE,
		SI_START_FUNCTION_SCOPE, // string FuncName
		SI_START_METHOD_SCOPE, // string FuncName

		SI_TO_BOOL // pop, convert to bool, push
	};
	
	namespace Compiler
	{
		typedef enum SymbolType
		{
			Function,
			Variable,
			Class
		};
	} // namespace Compiler
} // namespace Wme
#endif // __WmeScriptHeader_H__
