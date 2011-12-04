// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "CodeNodes.h"
#include "CodeGen.h"


namespace Wme
{
namespace Compiler
{
	
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
ParameterNode::ParameterNode(Identifier* ID)
{
	m_ID = ID;
	m_Next = this;
}

//////////////////////////////////////////////////////////////////////////
ParameterNode::ParameterNode(ParameterNode* List, Identifier* ID)
{
	m_ID = ID;
	m_Next = List->m_Next;
	List->m_Next = this;
}

//////////////////////////////////////////////////////////////////////////
void ParameterNode::Generate(CodeGen* Context)
{
	Node::Generate(Context);

	Context->PutInstruction(ISTR(SI_POP_VAR));
	Context->PutString(m_ID->GetName(), m_Utf8Mode);

	if (m_Next) m_Next->Generate(Context);
}

//////////////////////////////////////////////////////////////////////////
void ParameterNode::GenerateDeclaration(CodeGen* Context)
{
	Context->PutInstruction(ISTR(SI_DEF_VAR));
	Context->PutString(m_ID->GetName(), m_Utf8Mode);
}

//////////////////////////////////////////////////////////////////////////
void ParameterNode::GetSymbols(CodeGen* Context)
{
	if (Context->HasSymbol(m_ID->GetName(), Wme::Compiler::Variable, true))
	{
		ScriptParser::GetInstance().AddError("'%s': formal parameter redefinition", m_ID->GetName());
		return;
	}
	else
	{
		Context->AddSymbol(m_ID->GetName(), Wme::Compiler::Variable);
		FunctionBodyNode* CurrentScope = Context->GetScope();
		if (CurrentScope) CurrentScope->AddParameter(this);
	}

	if (m_Next) m_Next->GetSymbols(Context);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
VarDeclNode::VarDeclNode(Identifier* ID, AssignExprNode* Init, Type Type)
{
	m_ID = ID;
	m_Init = Init;
	m_Type = Type;
}

//////////////////////////////////////////////////////////////////////////
void VarDeclNode::Generate(CodeGen* Context)
{
	if (m_Init)
	{
		Node::Generate(Context);
		m_Init->Generate(Context);
		Context->PutInstruction(ISTR(SI_POP_VAR));
		Context->PutString(m_ID->GetName(), m_Utf8Mode);
	}
}

//////////////////////////////////////////////////////////////////////////
void VarDeclNode::GenerateDeclaration(CodeGen* Context)
{
	switch (m_Type)
	{
	case Variable:
		Context->PutInstruction(ISTR(SI_DEF_VAR));
		break;
	case Global:
		Context->PutInstruction(ISTR(SI_DEF_GLOB));
		break;
	}
	Context->PutString(m_ID->GetName(), m_Utf8Mode);
}

//////////////////////////////////////////////////////////////////////////
void VarDeclNode::GetSymbols(CodeGen* Context)
{
	if (Context->HasSymbol(m_ID->GetName(), Wme::Compiler::Variable, true))
	{
		ScriptParser::GetInstance().AddError("'%s': variable redefinition", m_ID->GetName());
		return;
	}
	else Context->AddSymbol(m_ID->GetName(), Wme::Compiler::Variable);

	FunctionBodyNode* CurrentScope = Context->GetScope();
	if (CurrentScope) CurrentScope->AddVariable(this);

	if (m_Init) m_Init->GetSymbols(Context);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Node::AddJumpRequest(const AnsiString& Type, CodeGen* Context)
{
	m_JumpRequests.insert(JumpRequestPair(Type, Context->GetPosition()));
}

//////////////////////////////////////////////////////////////////////////
void Node::ResolveJumpRequests(const AnsiString& Type, CodeGen* Context)
{
	int OrigPos = Context->GetPosition();

	std::pair<JumpRequestMap::iterator, JumpRequestMap::iterator> Values;
	Values = m_JumpRequests.equal_range(Type);

	JumpRequestMap::iterator i;
	for (i = Values.first; i != Values.second; i++) 
	{
		Context->SeekToPosition(i->second);
		Context->PutInt(OrigPos);
	} 

	Context->SeekToPosition(OrigPos);
}

//////////////////////////////////////////////////////////////////////////
void Node::SetFileContext(CodeGen* Context)
{
	Context->SetFileContext(m_File, m_Line);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void FunctionBodyNode::Generate(CodeGen* Context)
{
	if (m_TopLevel)
	{
		Context->PutInstruction(ISTR(SI_START_GLOBAL_SCOPE));
		Context->PushScope(this); // for functions the scope is pushed by FuncDeclNode
		DeclareVars(Context);
	}

	BlockNode::Generate(Context);
	Context->PopScope();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void FunctionBodyNode::GetSymbols(CodeGen* Context)
{
	if (m_TopLevel) Context->PushScope(this); // for functions the scope is pushed by FuncDeclNode

	BlockNode::GetSymbols(Context);
	Context->PopScope();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void FuncDeclNode::Generate(CodeGen* Context)
{
	Node::Generate(Context);

	// jump over the function
	Context->PutInstruction(ISTR(SI_JUMP));
	AddJumpRequest("end", Context);
	Context->PutInt(-1);

	if (m_Type==Function)
		Context->AddFunctionEntry(m_ID->GetName(), Context->GetPosition());
	else
		Context->AddMethodEntry(m_ID->GetName(), Context->GetPosition());

	int NumParams = 0;
	ParameterNode* Param = m_Params;
	while (Param)
	{
		NumParams++;
		Param = Param->m_Next;
	}

	// this line is bit of a hack to get the correct script line
	// doesn't work if the function doesn't have any params
	if (m_Params) m_Params->SetFileContext(Context);


	Context->PutInstruction(ISTR(SI_CHECK_PARAMS));
	Context->PutInt(NumParams);

	if (m_Type==Function)
		Context->PutInstruction(ISTR(SI_START_FUNCTION_SCOPE));
	else
		Context->PutInstruction(ISTR(SI_START_METHOD_SCOPE));
	Context->PutString(m_ID->GetName(), m_Utf8Mode);


	Context->PushScope(m_Body);
	m_Body->DeclareVars(Context);
	if (m_Params) m_Params->Generate(Context);
	m_Body->Generate(Context);

	Context->PutInstruction(ISTR(SI_PUSH_NULL));
	Context->PutInstruction(ISTR(SI_RETURN));

	ResolveJumpRequests("end", Context);
}

//////////////////////////////////////////////////////////////////////////
void FuncDeclNode::GetSymbols(CodeGen* Context)
{
	if (Context->GetScopeNest() > 1)
	{
		ScriptParser::GetInstance().AddError("'%s': local function definitions are illegal", m_ID->GetName());
		return;
	}

	if (Context->HasSymbol(m_ID->GetName(), Wme::Compiler::Function, false))
	{
		ScriptParser::GetInstance().AddError("'%s': function already defined", m_ID->GetName());
		return;
	}
	else Context->AddSymbol(m_ID->GetName(), Wme::Compiler::Function);

	Context->PushScope(m_Body);
	if (m_Params) m_Params->GetSymbols(Context);
	m_Body->GetSymbols(Context);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void ArgumentsNode::Generate(CodeGen* Context)
{
	Node::Generate(Context);

	int NumArguments = 0;

	if (m_Arguments)
	{
		m_Arguments->Generate(Context);

		ArgumentListNode* Arg = m_Arguments;
		while (Arg)
		{
			NumArguments++;
			Arg = Arg->m_Next;
		}
	}
	Context->PutInstruction(ISTR(SI_PUSH_INT));
	Context->PutInt(NumArguments);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void CaseClauseNode::GenerateCondition(CodeGen* Context)
{
	if (m_Expr)
	{
		// clone the value on stack
		Context->PutInstruction(ISTR(SI_DUP_STACK));
		m_Expr->Generate(Context);
		Context->PutInstruction(ISTR(SI_CMP_EQUAL));
		Context->PutInstruction(ISTR(SI_JUMP_TRUE));
		AddJumpRequest("code", Context);
		Context->PutInt(-1);
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void CaseBlockNode::Generate(CodeGen* Context)
{
	Node::Generate(Context);

	// condition handling
	if (m_Cases1) m_Cases1->GenerateCondition(Context);
	if (m_Cases2) m_Cases2->GenerateCondition(Context);

	if (m_Default)
	{
		Context->PutInstruction(ISTR(SI_JUMP));
		m_Default->AddJumpRequest("code", Context);
		Context->PutInt(-1);
	}

	// the actual code
	if (m_Cases1) m_Cases1->GenerateCode(Context);
	if (m_Default) m_Default->GenerateCode(Context);
	if (m_Cases2) m_Cases1->GenerateCode(Context);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void SwitchNode::Generate(CodeGen* Context)
{
	Context->PushLoop(this);

	Node::Generate(Context);

	m_Expr->Generate(Context);
	m_Block->Generate(Context);

	ResolveJumpRequests("break target", Context);
	Context->PutInstruction(ISTR(SI_POP));

	Context->PopLoop();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void NullNode::Generate(CodeGen* Context)
{
	Node::Generate(Context);

	Context->PutInstruction(ISTR(SI_PUSH_NULL));
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void BooleanNode::Generate(CodeGen* Context)
{
	Node::Generate(Context);

	Context->PutInstruction(ISTR(SI_PUSH_BOOL));
	Context->PutByte(m_Value);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void NumberNode::Generate(CodeGen* Context)
{
	Node::Generate(Context);

	switch (m_Type)
	{
	case Integer:
		Context->PutInstruction(ISTR(SI_PUSH_INT));
		Context->PutInt(m_ValInt);
		break;
	case Double:
		Context->PutInstruction(ISTR(SI_PUSH_DOUBLE));
		Context->PutDouble(m_ValDouble);
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void StringNode::Generate(CodeGen* Context)
{
	Node::Generate(Context);

	Context->PutInstruction(ISTR(SI_PUSH_STRING));
	Context->PutString(m_Value, m_Utf8Mode);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void ThisNode::Generate(CodeGen* Context)
{
	Node::Generate(Context);

	Context->PutInstruction(ISTR(SI_PUSH_THIS));
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void ResolveNode::Generate(CodeGen* Context)
{
	Node::Generate(Context);

	switch (m_Type)
	{
	case Variable:
		if (!Context->HasSymbol(m_ID->GetName(), Wme::Compiler::Variable, false))
		{
			ScriptParser::GetInstance().AddError("'%s': Undefined variable", m_ID->GetName());
			return;
		}
		Context->PutInstruction(ISTR(SI_PUSH_VAR_REF));
		Context->PutString(m_ID->GetName(), m_Utf8Mode);
		break;

	case Function:
		if (!Context->HasSymbol(m_ID->GetName(), Wme::Compiler::Function, false))
		{
			ScriptParser::GetInstance().AddError("'%s': Call to undefined function", m_ID->GetName());
			return;
		}
		Context->PutInstruction(ISTR(SI_PUSH_STRING));
		Context->PutString(m_ID->GetName(), m_Utf8Mode);
		break;

	case Class:
		if (!Context->HasSymbol(m_ID->GetName(), Wme::Compiler::Class, false))
		{
			ScriptParser::GetInstance().AddError("'%s': Unknown class", m_ID->GetName());
			return;
		}
		Context->PutInstruction(ISTR(SI_PUSH_STRING));
		Context->PutString(m_ID->GetName(), m_Utf8Mode);
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void AccessorNode1::Generate(CodeGen* Context)
{
	Node::Generate(Context);

	m_Property->Generate(Context);
	m_Object->Generate(Context);
	Context->PutInstruction(ISTR(SI_PUSH_PROP_REF));
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void AccessorNode2::Generate(CodeGen* Context)
{
	Node::Generate(Context);

	Context->PutInstruction(ISTR(SI_PUSH_STRING));
	Context->PutString(m_ID->GetName(), m_Utf8Mode);

	m_Object->Generate(Context);
	Context->PutInstruction(ISTR(SI_PUSH_PROP_REF));
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void NewExprNode::Generate(CodeGen* Context)
{
	Node::Generate(Context);

	if (m_Arguments) m_Arguments->Generate(Context);
	else
	{
		// no arguments
		Context->PutInstruction(ISTR(SI_PUSH_INT));
		Context->PutInt(0);
	}

	if (m_Expr->IsIdentifier())
	{
		ResolveNode* ClassName = (ResolveNode*)m_Expr;
		ClassName->SetType(Class);
	}

	m_Expr->Generate(Context);
	Context->PutInstruction(ISTR(SI_NEW));
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void FunctionCallNode::Generate(CodeGen* Context)
{
	Node::Generate(Context);

	if (!m_Expr->IsReference())
	{
		ScriptParser::GetInstance().GetInstance().AddError("Term does not evaluate to a function");
		return;
	}

	if (m_Arguments) m_Arguments->Generate(Context);
	else
	{
		// no arguments
		Context->PutInstruction(ISTR(SI_PUSH_INT));
		Context->PutInt(0);			
	}

	if (m_Expr->IsIdentifier())
	{
		ResolveNode* FuncName = (ResolveNode*)m_Expr;
		FuncName->SetType(Function);
	}

	m_Expr->Generate(Context);

	if (m_Expr->IsIdentifier())
		Context->PutInstruction(ISTR(SI_CALL_BY_NAME));
	else
		Context->PutInstruction(ISTR(SI_CALL_BY_REF));
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void PostfixNode::Generate(CodeGen* Context)
{
	if (!m_Expr->IsReference())
	{
		ScriptParser::GetInstance().GetInstance().AddError("'%s' needs l-value", m_Oper==OpPlusPlus?"++":"--");
		return;
	}

	Node::Generate(Context);

	m_Expr->Generate(Context);

	switch (m_Oper)
	{
	case OpPlusPlus:
		Context->PutInstruction(ISTR(SI_INC_POST));
		break;
	case OpMinusMinus:
		Context->PutInstruction(ISTR(SI_DEC_POST));
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void VoidNode::Generate(CodeGen* Context)
{
	Node::Generate(Context);

	m_Expr->Generate(Context);
	Context->PutInstruction(ISTR(SI_POP));
	Context->PutInstruction(ISTR(SI_PUSH_NULL));
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void TypeOfNode::Generate(CodeGen* Context)
{
	Node::Generate(Context);

	m_Expr->Generate(Context);
	Context->PutInstruction(ISTR(SI_TYPEOF));
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void PrefixNode::Generate(CodeGen* Context)
{
	if (!m_Expr->IsReference())
	{
		ScriptParser::GetInstance().AddError("'%s' needs l-value", m_Oper==OpPlusPlus?"++":"--");
		return;
	}

	Node::Generate(Context);

	m_Expr->Generate(Context);

	switch (m_Oper)
	{
	case OpPlusPlus:
		Context->PutInstruction(ISTR(SI_INC_PRE));
		break;
	case OpMinusMinus:
		Context->PutInstruction(ISTR(SI_DEC_PRE));
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void UnaryPlusNode::Generate(CodeGen* Context)
{
	Node::Generate(Context);

	m_Expr->Generate(Context);
	Context->PutInt(0);
	Context->PutInstruction(ISTR(SI_ADD));
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void NegateNode::Generate(CodeGen* Context)
{
	Node::Generate(Context);

	m_Expr->Generate(Context);
	Context->PutInt(0);
	Context->PutInstruction(ISTR(SI_SUB));
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void LogicalNotNode::Generate(CodeGen* Context)
{
	Node::Generate(Context);

	m_Expr->Generate(Context);
	Context->PutInstruction(ISTR(SI_NEG));
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void MultNode::Generate(CodeGen* Context)
{
	Node::Generate(Context);

	m_Term2->Generate(Context);
	m_Term1->Generate(Context);

	switch (m_Oper)
	{
	case Multiply:
		Context->PutInstruction(ISTR(SI_MUL));
		break;
	case Divide:
		Context->PutInstruction(ISTR(SI_DIV));
		break;
	case Modulo:
		Context->PutInstruction(ISTR(SI_MOD));
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void AddNode::Generate(CodeGen* Context)
{
	Node::Generate(Context);

	m_Term2->Generate(Context);
	m_Term1->Generate(Context);

	switch (m_Oper)
	{
	case Add:
		Context->PutInstruction(ISTR(SI_ADD));
		break;
	case Subtract:
		Context->PutInstruction(ISTR(SI_SUB));
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void RelationalNode::Generate(CodeGen* Context)
{
	Node::Generate(Context);

	m_Expr2->Generate(Context);
	m_Expr1->Generate(Context);

	switch (m_Oper)
	{
	case OpLess:
		Context->PutInstruction(ISTR(SI_CMP_LESS));
		break;
	case OpGreater:
		Context->PutInstruction(ISTR(SI_CMP_GREATER));
		break;
	case OpLessEq:
		Context->PutInstruction(ISTR(SI_CMP_LESS_EQUAL));
		break;
	case OpGreaterEq:
		Context->PutInstruction(ISTR(SI_CMP_GREATER_EQUAL));
		break;
	case OpIn:
		Context->PutInstruction(ISTR(SI_IN));
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void EqualNode::Generate(CodeGen* Context)
{
	Node::Generate(Context);

	m_Expr2->Generate(Context);
	m_Expr1->Generate(Context);

	switch (m_Oper)
	{
	case OpEqEq:
		Context->PutInstruction(ISTR(SI_CMP_EQUAL));
		break;
	case OpNotEq:
		Context->PutInstruction(ISTR(SI_CMP_NOT_EQUAL));
		break;
	case OpStrEq:
		Context->PutInstruction(ISTR(SI_CMP_STRICT_EQUAL));
		break;
	case OpStrNEq:
		Context->PutInstruction(ISTR(SI_CMP_STRICT_NOT_EQUAL));
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void BinaryLogicalNode::Generate(CodeGen* Context)
{
	Node::Generate(Context);

	m_Expr1->Generate(Context);
	Context->PutInstruction(ISTR(SI_DUP_STACK));
	Context->PutInstruction(ISTR(SI_TO_BOOL));
	Context->PutInstruction(ISTR(SI_PUSH_BOOL));

	if (m_Oper == OpAnd)
		Context->PutByte(false);
	else
		Context->PutByte(true);

	Context->PutInstruction(ISTR(SI_CMP_EQUAL));
	Context->PutInstruction(ISTR(SI_JUMP_TRUE));
	AddJumpRequest("end", Context);
	Context->PutInt(-1);
	m_Expr2->Generate(Context);

	if (m_Oper == OpAnd)
		Context->PutInstruction(ISTR(SI_AND));
	else
		Context->PutInstruction(ISTR(SI_OR));

	ResolveJumpRequests("end", Context);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void ConditionalNode::Generate(CodeGen* Context)
{
	if (m_Cond->IsAssignment())
	{
		ScriptParser::GetInstance().AddError("Assignment in conditional expression is not allowed; did you mean to use == instead of = ?");
		return;
	}

	Node::Generate(Context);

	m_Cond->Generate(Context);
	Context->PutInstruction(ISTR(SI_TO_BOOL));
	Context->PutInstruction(ISTR(SI_PUSH_BOOL));
	Context->PutByte(true);
	Context->PutInstruction(ISTR(SI_CMP_EQUAL));
	Context->PutInstruction(ISTR(SI_JUMP_TRUE));
	AddJumpRequest("expr1", Context);
	Context->PutInt(-1);
	m_Expr2->Generate(Context);
	Context->PutInstruction(ISTR(SI_JUMP));
	AddJumpRequest("end", Context);
	Context->PutInt(-1);
	ResolveJumpRequests("expr1", Context);
	m_Expr1->Generate(Context);
	ResolveJumpRequests("end", Context);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void AssignNode::Generate(CodeGen* Context)
{
	if (!m_Left->IsReference())
	{
		ScriptParser::GetInstance().AddError("left operand must be l-value");
		return;
	}

	Node::Generate(Context);

	m_Expr->Generate(Context);

	switch (m_Oper)
	{
	case OpEqual:
		Context->PutInstruction(ISTR(SI_DUP_STACK));
		m_Left->Generate(Context);
		Context->PutInstruction(ISTR(SI_POP_BY_REF));
		break;
	case OpPlusEq:
		m_Left->Generate(Context);
		Context->PutInstruction(ISTR(SI_ADD_REF));
		break;
	case OpMinusEq:
		m_Left->Generate(Context);
		Context->PutInstruction(ISTR(SI_SUB_REF));
		break;
	case OpMultEq:
		m_Left->Generate(Context);
		Context->PutInstruction(ISTR(SI_MUL_REF));
		break;
	case OpDivEq:
		m_Left->Generate(Context);
		Context->PutInstruction(ISTR(SI_DIV_REF));
		break;
	case OpModEq:
		m_Left->Generate(Context);
		Context->PutInstruction(ISTR(SI_MOD_REF));
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void CommaNode::Generate(CodeGen* Context)
{
	Node::Generate(Context);

	m_Expr1->Generate(Context);
	Context->PutInstruction(ISTR(SI_POP));
	m_Expr2->Generate(Context);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void ExprStatementNode::Generate(CodeGen* Context)
{
	Node::Generate(Context);

	m_Expr->Generate(Context);
	// clean the stack
	Context->PutInstruction(ISTR(SI_POP));
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void IfNode::Generate(CodeGen* Context)
{
	if (m_Expr->IsAssignment())
	{
		ScriptParser::GetInstance().AddError("Assignment in conditional expression is not allowed; did you mean to use == instead of = ?");
		return;
	}

	Node::Generate(Context);

	m_Expr->Generate(Context);
	Context->PutInstruction(ISTR(SI_TO_BOOL));
	Context->PutInstruction(ISTR(SI_PUSH_BOOL));
	Context->PutByte(false);
	Context->PutInstruction(ISTR(SI_CMP_EQUAL));
	Context->PutInstruction(ISTR(SI_JUMP_TRUE));
	AddJumpRequest("statement2", Context);
	Context->PutInt(-1);

	m_Statement1->Generate(Context);
	Context->PutInstruction(ISTR(SI_JUMP));
	AddJumpRequest("end", Context);
	Context->PutInt(-1);

	ResolveJumpRequests("statement2", Context);
	if (m_Statement2) m_Statement2->Generate(Context);
	ResolveJumpRequests("end", Context);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void DoWhileNode::Generate(CodeGen* Context)
{
	Context->PushLoop(this);
	int Begin = Context->GetPosition();

	Node::Generate(Context);

	m_Statement->Generate(Context);

	ResolveJumpRequests("continue target", Context);
	m_Expr->Generate(Context);
	Context->PutInstruction(ISTR(SI_TO_BOOL));
	Context->PutInstruction(ISTR(SI_PUSH_BOOL));
	Context->PutByte(true);
	Context->PutInstruction(ISTR(SI_CMP_EQUAL));
	Context->PutInstruction(ISTR(SI_JUMP_TRUE));
	Context->PutInt(Begin);
	ResolveJumpRequests("break target", Context);

	Context->PopLoop();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void WhileNode::Generate(CodeGen* Context)
{
	Context->PushLoop(this);
	m_Begin = Context->GetPosition();

	Node::Generate(Context);

	m_Expr->Generate(Context);
	Context->PutInstruction(ISTR(SI_TO_BOOL));
	Context->PutInstruction(ISTR(SI_PUSH_BOOL));
	Context->PutByte(false);
	Context->PutInstruction(ISTR(SI_CMP_EQUAL));
	Context->PutInstruction(ISTR(SI_JUMP_TRUE));
	AddJumpRequest("end", Context);
	Context->PutInt(-1);

	m_Statement->Generate(Context);
	Context->PutInstruction(ISTR(SI_JUMP));
	Context->PutInt(m_Begin);

	ResolveJumpRequests("end", Context);
	ResolveJumpRequests("break target", Context);

	Context->PopLoop();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void ForNode::Generate(CodeGen* Context)
{
	Context->PushLoop(this);

	Node::Generate(Context);

	if (m_Expr1)
	{
		m_Expr1->Generate(Context);

		if (!m_Expr1IsDecl) Context->PutInstruction(ISTR(SI_POP));
	}

	int Begin = Context->GetPosition();

	if (m_Expr2)
	{
		m_Expr2->Generate(Context);
		Context->PutInstruction(ISTR(SI_TO_BOOL));
	}
	else
	{
		Context->PutInstruction(ISTR(SI_PUSH_BOOL));
		Context->PutByte(true);
	}

	Context->PutInstruction(ISTR(SI_PUSH_BOOL));
	Context->PutByte(false);
	Context->PutInstruction(ISTR(SI_CMP_EQUAL));
	Context->PutInstruction(ISTR(SI_JUMP_TRUE));
	AddJumpRequest("end", Context);
	Context->PutInt(-1);

	m_Statement->Generate(Context);

	ResolveJumpRequests("continue target", Context);
	if (m_Expr3)
	{
		m_Expr3->Generate(Context);
		Context->PutInstruction(ISTR(SI_POP));
	}

	Context->PutInstruction(ISTR(SI_JUMP));
	Context->PutInt(Begin);

	ResolveJumpRequests("end", Context);
	ResolveJumpRequests("break target", Context);

	Context->PopLoop();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void ForInNode::Generate(CodeGen* Context)
{
	if (!m_LeftExpr->IsReference())
	{
		ScriptParser::GetInstance().AddError("for...in needs l-value");
		return;
	}

	Context->PushLoop(this);

	Node::Generate(Context);

	m_Expr->Generate(Context);
	Context->PutInstruction(ISTR(SI_PUSH_ITERATOR));

	m_Begin = Context->GetPosition();		
	Context->PutInstruction(ISTR(SI_ITERATOR_NEXT));
	m_LeftExpr->Generate(Context);
	Context->PutInstruction(ISTR(SI_POP_BY_REF));

	m_LeftExpr->Generate(Context);
	Context->PutInstruction(ISTR(SI_PUSH_NULL));
	Context->PutInstruction(ISTR(SI_CMP_EQUAL));
	Context->PutInstruction(ISTR(SI_JUMP_TRUE));
	AddJumpRequest("end", Context);
	Context->PutInt(-1);

	m_Statement->Generate(Context);
	Context->PutInstruction(ISTR(SI_JUMP));
	Context->PutInt(m_Begin);

	// pop iterator id and obj reference
	ResolveJumpRequests("end", Context);
	ResolveJumpRequests("break target", Context);
	Context->PutInstruction(ISTR(SI_POP));
	Context->PutInstruction(ISTR(SI_POP));

	Context->PopLoop();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void ContinueNode::Generate(CodeGen* Context)
{
	Node::Generate(Context);

	StatementNode* LoopNode;

	if (m_ID) LoopNode = Context->GetLoop(m_ID->GetName());
	else LoopNode = Context->GetLoop();

	if (!LoopNode || !LoopNode->SupportsBreak(Continue))
	{
		ScriptParser::GetInstance().AddError("Illegal continue");
		return;
	}

	int Address = LoopNode->GetBreakAddress(Continue);
	if (Address <= 0)
	{
		Context->PutInstruction(ISTR(SI_JUMP));
		LoopNode->AddJumpRequest("continue target", Context);
		Context->PutInt(-1);
	}
	else
	{
		Context->PutInstruction(ISTR(SI_JUMP));
		Context->PutInt(Address);
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void BreakNode::Generate(CodeGen* Context)
{
	Node::Generate(Context);

	StatementNode* LoopNode;

	if (m_ID) LoopNode = Context->GetLoop(m_ID->GetName());
	else LoopNode = Context->GetLoop();

	if (!LoopNode || !LoopNode->SupportsBreak(Break))
	{
		ScriptParser::GetInstance().AddError("Illegal break");
		return;
	}

	int Address = LoopNode->GetBreakAddress(Break);
	if (Address <= 0)
	{
		Context->PutInstruction(ISTR(SI_JUMP));
		LoopNode->AddJumpRequest("break target", Context);
		Context->PutInt(-1);
	}
	else
	{
		Context->PutInstruction(ISTR(SI_JUMP));
		Context->PutInt(Address);
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void ReturnNode::Generate(CodeGen* Context)
{
	Node::Generate(Context);

	if (m_Value) m_Value->Generate(Context);
	else Context->PutInstruction(ISTR(SI_PUSH_NULL));

	Context->PutInstruction(ISTR(SI_RETURN));
}


} // namespace Compiler
} // namespace Wme
