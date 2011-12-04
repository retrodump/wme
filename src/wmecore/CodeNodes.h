// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeCodeNodes_H__
#define __WmeCodeNodes_H__


// TODO:
//   'for in' ignores init (but it's useless anyway)


#include "ScriptParser.h"
#include "WmeScript.h"
#include <map>
#include <set>

namespace Wme
{
namespace Compiler
{
	class CodeGen;
	class Identifier;

	enum Operator
	{
		OpEqual,
		OpEqEq,
		OpNotEq,
		OpStrEq,
		OpStrNEq,
		OpPlusEq,
		OpMinusEq,
		OpMultEq,
		OpDivEq,
		OpPlusPlus,
		OpMinusMinus,
		OpLess,
		OpLessEq,
		OpGreater,
		OpGreaterEq,
		OpModEq,
		OpAnd,
		OpOr,
		OpIn,
		OpInstanceOf
	};


	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	class GCObject
	{
	public:
		GCObject()
		{
			ScriptParser::GetInstance().AddGarbage(this);
		}

		virtual ~GCObject()
		{
		}
	};

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	class Identifier : public GCObject
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		Identifier()
		{
			m_Name = NULL;
			m_Line = 0;
			m_Col = 0;
		};

		//////////////////////////////////////////////////////////////////////////
		Identifier(char* Name)
		{
			m_Name = new char[strlen(Name)+1];
			strcpy(m_Name, Name);
			m_Line = ScriptParser::GetInstance().GetCurrentLine();
			m_Col = ScriptParser::GetInstance().GetCurrentCol() - strlen(Name);
		};

		//////////////////////////////////////////////////////////////////////////
		Identifier(Identifier* SrcID)
		{
			m_Name = NULL;
			m_Line = 0;
			m_Col = 0;

			if (SrcID)
			{
				SetName(SrcID->m_Name);
				m_Line = SrcID->GetLine();
				m_Col = SrcID->GetCol();
			}
		};

		//////////////////////////////////////////////////////////////////////////
		virtual ~Identifier()
		{
			if (m_Name) delete [] m_Name;
		};

		//////////////////////////////////////////////////////////////////////////
		char* GetName()
		{
			if (m_Name) return m_Name;
			else return "";
		}

		//////////////////////////////////////////////////////////////////////////
		void SetName(const char* Name)
		{
			if (m_Name) delete [] m_Name;
			m_Name = new char[strlen(Name)+1];
			strcpy(m_Name, Name);
		}

		//////////////////////////////////////////////////////////////////////////
		const bool IsEmpty()
		{
			return m_Name==NULL;
		}

		//////////////////////////////////////////////////////////////////////////
		int GetLine()
		{
			return m_Line;
		}

		//////////////////////////////////////////////////////////////////////////
		int GetCol()
		{
			return m_Col;
		}

	private:
		char* m_Name;
		int m_Line;
		int m_Col;
	};

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	class Node : public GCObject
	{
	public:
		enum IdentifierType { Variable, Function, Class };

		//////////////////////////////////////////////////////////////////////////
		Node()
		{			
			m_ID = NULL;
			m_Line = ScriptParser::GetInstance().GetCurrentLine();
			
			m_File = ScriptParser::GetInstance().GetCurrentFile();
			m_Utf8Mode = ScriptParser::GetInstance().GetUtf8Mode();
		};
		
		//////////////////////////////////////////////////////////////////////////
		void AddJumpRequest(const AnsiString& Type, CodeGen* Context);

		//////////////////////////////////////////////////////////////////////////
		void ResolveJumpRequests(const AnsiString& Type, CodeGen* Context);

		//////////////////////////////////////////////////////////////////////////
		const WideString& GetFile()
		{
			return m_File;
		}

		//////////////////////////////////////////////////////////////////////////
		const int GetLine()
		{
			return m_Line;
		}

		//////////////////////////////////////////////////////////////////////////
		virtual void Generate(CodeGen* Context)
		{
			SetFileContext(Context);
		}

		//////////////////////////////////////////////////////////////////////////
		void SetFileContext(CodeGen* Context);

		//////////////////////////////////////////////////////////////////////////
		virtual void GetSymbols(CodeGen* Context) {};

		//////////////////////////////////////////////////////////////////////////
		virtual bool IsReference()
		{
			return false;
		}

		//////////////////////////////////////////////////////////////////////////
		virtual bool IsIdentifier()
		{
			return false;
		}

		//////////////////////////////////////////////////////////////////////////
		virtual bool IsAssignment()
		{
			return false;
		}

	protected:
		int m_Line;
		WideString m_File;
		Identifier* m_ID;
		bool m_Utf8Mode;
	private:
		typedef std::multimap<AnsiString, int> JumpRequestMap;
		typedef std::pair<AnsiString, int> JumpRequestPair;
		JumpRequestMap m_JumpRequests;
	};


	//////////////////////////////////////////////////////////////////////////
	class StatementNode : public Node
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		StatementNode()
		{

		};
		
		//////////////////////////////////////////////////////////////////////////
		virtual void AddLabel(Identifier* ID)
		{
			m_Labels.push_back(ID->GetName());
		};

		//////////////////////////////////////////////////////////////////////////
		bool HasLabel(const AnsiString& Label)
		{
			for (size_t i = 0; i < m_Labels.size(); i++)
			{
				if (m_Labels[i] == Label) return true;
			}
			return false;
		}

		enum BreakType { Break, Continue };

		//////////////////////////////////////////////////////////////////////////
		virtual bool SupportsBreak(BreakType Type)
		{
			return false;
		}

		//////////////////////////////////////////////////////////////////////////
		virtual int GetBreakAddress(BreakType Type)
		{
			return -1;
		}


		//////////////////////////////////////////////////////////////////////////
		virtual void Generate(CodeGen* Context)
		{
			Node::Generate(Context);
		}

	private:
		std::vector<AnsiString> m_Labels;
	};

	//////////////////////////////////////////////////////////////////////////
	class SourceElementsNode : public StatementNode
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		SourceElementsNode(StatementNode* Node)
		{
			m_Node = Node;
			m_Next = this;
		};
		
		//////////////////////////////////////////////////////////////////////////
		SourceElementsNode(SourceElementsNode* Elements, StatementNode* Node)
		{
			// the list is circular, hence Elements->Next points to the first element of the list
			// (this constructor always receives the last member of the list)

			m_Next = Elements->m_Next;
			Elements->m_Next = this;
			m_Node = Node;
		}

		//////////////////////////////////////////////////////////////////////////
		virtual void Generate(CodeGen* Context)
		{
			Node::Generate(Context);
			m_Node->Generate(Context);
			if (m_Next) m_Next->Generate(Context);
		}

		//////////////////////////////////////////////////////////////////////////
		virtual void GetSymbols(CodeGen* Context)
		{
			m_Node->GetSymbols(Context);
			if (m_Next) m_Next->GetSymbols(Context);
		}


	private:
		friend class BlockNode;
		StatementNode* m_Node; // 'this' element
		SourceElementsNode* m_Next; // pointer to next
	};

	//////////////////////////////////////////////////////////////////////////
	class BlockNode : public StatementNode
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		BlockNode()
		{
			m_Source = NULL;
		};

		//////////////////////////////////////////////////////////////////////////
		BlockNode(SourceElementsNode* Source)
		{
			// 'Source' is the last of the source element list, i.e. the one pointing to the first

			if (Source)
			{
				m_Source = Source->m_Next;
				Source->m_Next = NULL;
			}
			else
			{
				m_Source = NULL;
			}
		};

		//////////////////////////////////////////////////////////////////////////
		virtual void Generate(CodeGen* Context)
		{
			Node::Generate(Context);
			if (m_Source) m_Source->Generate(Context);
		}

		//////////////////////////////////////////////////////////////////////////
		virtual void GetSymbols(CodeGen* Context)
		{
			if (m_Source) m_Source->GetSymbols(Context);
		}

	protected:
		SourceElementsNode* m_Source;

	};


	//////////////////////////////////////////////////////////////////////////
	class AssignExprNode : public Node
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		AssignExprNode(Node* Expr)
		{
			m_Expr = Expr;
		};

		//////////////////////////////////////////////////////////////////////////
		virtual void Generate(CodeGen* Context)
		{
			Node::Generate(Context);
			m_Expr->Generate(Context);
		}

		//////////////////////////////////////////////////////////////////////////
		virtual void GetSymbols(CodeGen* Context)
		{
			m_Expr->GetSymbols(Context);
		}

	private:
		Node* m_Expr;

	};


	//////////////////////////////////////////////////////////////////////////
	class ParameterNode : public Node
	{
	public:
		ParameterNode(Identifier* ID);
		ParameterNode(ParameterNode* List, Identifier* ID);
		virtual void Generate(CodeGen* Context);
		virtual void GenerateDeclaration(CodeGen* Context);
		virtual void GetSymbols(CodeGen* Context);

	private:
		friend class FuncDeclNode;
		friend class FuncExprNode;
		ParameterNode* m_Next;
	};

	//////////////////////////////////////////////////////////////////////////
	class VarDeclNode : public Node
	{
	public:
		enum Type { Variable, Global };

		VarDeclNode(Identifier* ID, AssignExprNode* Init, Type Type);
		virtual void Generate(CodeGen* Context);
		virtual void GenerateDeclaration(CodeGen* Context);
		virtual void GetSymbols(CodeGen* Context);

	private:
		Type m_Type;
		AssignExprNode* m_Init;
	};


	//////////////////////////////////////////////////////////////////////////
	class FunctionBodyNode : public BlockNode
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		FunctionBodyNode(SourceElementsNode* Source, bool TopLevel) : BlockNode(Source)
		{
			m_TopLevel = TopLevel;
		};

		//////////////////////////////////////////////////////////////////////////
		virtual void Generate(CodeGen* Context);

		//////////////////////////////////////////////////////////////////////////
		void DeclareVars(CodeGen* Context)
		{
			for (Parameters::iterator it = m_Parameters.begin(); it != m_Parameters.end(); it++)
			{
				(*it)->GenerateDeclaration(Context);
			}

			for (VarDeclarations::iterator it = m_Declarations.begin(); it != m_Declarations.end(); it++)
			{
				(*it)->GenerateDeclaration(Context);
			}
		}

		//////////////////////////////////////////////////////////////////////////
		virtual void GetSymbols(CodeGen* Context);

		//////////////////////////////////////////////////////////////////////////
		void AddSymbol(const AnsiString& Name, SymbolType Type)
		{
			switch (Type)
			{
			case Wme::Compiler::Variable:
				m_Variables.insert(Name);
				break;
			case Wme::Compiler::Function:
				m_Functions.insert(Name);
				break;
			}
		}

		//////////////////////////////////////////////////////////////////////////
		bool HasSymbol(const AnsiString& Name, SymbolType Type)
		{
			Symbols::const_iterator it;

			switch (Type)
			{
			case Wme::Compiler::Variable:
				it = m_Variables.find(Name);
				return (it != m_Variables.end());

			case Wme::Compiler::Function:
				it = m_Functions.find(Name);
				return (it != m_Functions.end());
			}
			return false;
		}

		//////////////////////////////////////////////////////////////////////////
		void AddVariable(VarDeclNode* Decl)
		{
			m_Declarations.push_back(Decl);
		}

		//////////////////////////////////////////////////////////////////////////
		void AddParameter(ParameterNode* Param)
		{
			m_Parameters.push_back(Param);
		}

	private:
		bool m_TopLevel;

		typedef std::set<AnsiString> Symbols;
		Symbols m_Variables;
		Symbols m_Functions;

		typedef std::vector<VarDeclNode*> VarDeclarations;
		typedef std::vector<ParameterNode*> Parameters;
		VarDeclarations m_Declarations;
		Parameters m_Parameters;
	};

	//////////////////////////////////////////////////////////////////////////
	class FuncDeclNode : public StatementNode
	{
	public:
		enum Type { Function, Method };

		//////////////////////////////////////////////////////////////////////////
		FuncDeclNode(Identifier* ID, FunctionBodyNode* Body, Type Type)
		{
			m_ID = ID;
			m_Body = Body;
			m_Params = NULL;
			m_Type = Type;
		};

		//////////////////////////////////////////////////////////////////////////
		FuncDeclNode(Identifier* ID, ParameterNode* Params, FunctionBodyNode* Body, Type Type)
		{
			m_ID = ID;
			m_Body = Body;
			m_Type = Type;

			// claim the first member of the list and break the circular list
			m_Params = Params->m_Next;
			Params->m_Next = NULL;
		};

		//////////////////////////////////////////////////////////////////////////
		virtual void Generate(CodeGen* Context);


		//////////////////////////////////////////////////////////////////////////
		virtual void GetSymbols(CodeGen* Context);

	private:
		ParameterNode* m_Params;
		FunctionBodyNode* m_Body;
		Type m_Type;
	};


	//////////////////////////////////////////////////////////////////////////
	class StatListNode : public StatementNode
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		StatListNode(StatementNode* Statement)
		{
			m_Statement = Statement;
			m_Next = this;
		};

		//////////////////////////////////////////////////////////////////////////
		StatListNode(StatListNode* Statements, StatementNode* Statement)
		{
			m_Statement = Statement;

			m_Next = Statements->m_Next;
			Statements->m_Next = this;
		};

		//////////////////////////////////////////////////////////////////////////
		virtual void Generate(CodeGen* Context)
		{
			Node::Generate(Context);
			m_Statement->Generate(Context);
			if (m_Next) m_Next->Generate(Context);
		}

		//////////////////////////////////////////////////////////////////////////
		virtual void GetSymbols(CodeGen* Context)
		{
			m_Statement->GetSymbols(Context);
			if (m_Next) m_Next->GetSymbols(Context);
		}

	private:
		friend class CaseClauseNode;
		StatementNode* m_Statement;
		StatListNode* m_Next;

	};

	//////////////////////////////////////////////////////////////////////////
	class ArgumentListNode : public Node
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		ArgumentListNode(Node* Expr)
		{
			m_Next = this;
			m_Expr = Expr;
		};

		//////////////////////////////////////////////////////////////////////////
		ArgumentListNode(ArgumentListNode* Arguments, Node* Expr)
		{
			m_Next = Arguments->m_Next;
			Arguments->m_Next = this;
			m_Expr = Expr;
		};

		//////////////////////////////////////////////////////////////////////////
		virtual void Generate(CodeGen* Context)
		{
			Node::Generate(Context);
			m_Expr->Generate(Context);
			if (m_Next) m_Next->Generate(Context);
		}

		//////////////////////////////////////////////////////////////////////////
		virtual void GetSymbols(CodeGen* Context)
		{
			m_Expr->GetSymbols(Context);
			if (m_Next) m_Next->GetSymbols(Context);
		}

	private:
		friend class ArgumentsNode;
		ArgumentListNode* m_Next;
		Node* m_Expr;
	};

	//////////////////////////////////////////////////////////////////////////
	class ArgumentsNode : public Node
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		ArgumentsNode()
		{
			m_Arguments = NULL;
		};

		//////////////////////////////////////////////////////////////////////////
		ArgumentsNode(ArgumentListNode* Arguments)
		{
			// claim the first member of the list and break the circular list
			m_Arguments = Arguments->m_Next;
			Arguments->m_Next = NULL;
		};

		//////////////////////////////////////////////////////////////////////////
		virtual void Generate(CodeGen* Context);

		//////////////////////////////////////////////////////////////////////////
		virtual void GetSymbols(CodeGen* Context)
		{
			if (m_Arguments) m_Arguments->GetSymbols(Context);
		}

	private:
		ArgumentListNode* m_Arguments;

	};


	//////////////////////////////////////////////////////////////////////////
	class VarDeclListNode : public Node
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		VarDeclListNode(VarDeclNode* Var)
		{
			m_Var = Var;
			m_Next = this;
		};

		//////////////////////////////////////////////////////////////////////////
		VarDeclListNode(VarDeclListNode* Declarations, VarDeclNode* Var)
		{
			m_Var = Var;
			m_Next = Declarations->m_Next;
			Declarations->m_Next = this;
		};

		//////////////////////////////////////////////////////////////////////////
		virtual void Generate(CodeGen* Context)
		{
			Node::Generate(Context);
			m_Var->Generate(Context);
			if (m_Next) m_Next->Generate(Context);
		}

		//////////////////////////////////////////////////////////////////////////
		virtual void GetSymbols(CodeGen* Context)
		{
			m_Var->GetSymbols(Context);
			if (m_Next) m_Next->GetSymbols(Context);
		}

	private:
		friend class ForNode;
		friend class VarStatementNode;
		VarDeclListNode* m_Next;
		VarDeclNode* m_Var;
	};

	//////////////////////////////////////////////////////////////////////////
	class CaseClauseNode : public Node
	{
	// case Expr:
	//   Statements
	//
	// for 'default:' Expr == NULL

	public:
		//////////////////////////////////////////////////////////////////////////
		CaseClauseNode(Node* Expr)
		{
			m_Expr = Expr;
			m_Statements = NULL;
		};
		
		//////////////////////////////////////////////////////////////////////////
		CaseClauseNode(Node* Expr, StatListNode* Statements)
		{
			m_Expr = Expr;

			// claim the first member of the list and break the circular list
			m_Statements = Statements->m_Next;
			Statements->m_Next = NULL;
		}

		//////////////////////////////////////////////////////////////////////////
		virtual void GenerateCondition(CodeGen* Context);

		//////////////////////////////////////////////////////////////////////////
		virtual void GenerateCode(CodeGen* Context)
		{
			Node::Generate(Context);

			ResolveJumpRequests("code", Context);
			if (m_Statements) m_Statements->Generate(Context);
		}

		//////////////////////////////////////////////////////////////////////////
		virtual void GetSymbols(CodeGen* Context)
		{
			if (m_Expr) m_Expr->GetSymbols(Context);
			if (m_Statements) m_Statements->GetSymbols(Context);
		}

	private:
		Node* m_Expr;
		StatListNode* m_Statements;
	};

	//////////////////////////////////////////////////////////////////////////
	class ClauseListNode : public Node
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		ClauseListNode(CaseClauseNode* Case)
		{
			m_Case = Case;
			m_Next = this;
		};

		//////////////////////////////////////////////////////////////////////////
		ClauseListNode(ClauseListNode* Cases, CaseClauseNode* Case)
		{
			m_Case = Case;
			m_Next = Cases->m_Next;
			Cases->m_Next = this;
		};

		//////////////////////////////////////////////////////////////////////////
		virtual void GenerateCondition(CodeGen* Context)
		{
			m_Case->GenerateCondition(Context);
			if (m_Next) m_Next->GenerateCondition(Context);
		}

		//////////////////////////////////////////////////////////////////////////
		virtual void GenerateCode(CodeGen* Context)
		{
			m_Case->GenerateCode(Context);
			if (m_Next) m_Next->GenerateCode(Context);
		}

		//////////////////////////////////////////////////////////////////////////
		virtual void GetSymbols(CodeGen* Context)
		{
			m_Case->GetSymbols(Context);
			if (m_Next) m_Next->GetSymbols(Context);
		}

	private:
		friend class CaseBlockNode;
		CaseClauseNode* m_Case;
		ClauseListNode* m_Next;
	};

	//////////////////////////////////////////////////////////////////////////
	class CaseBlockNode : public Node
	{
	// case a:   \ cases1 (opt)
	// case b:   /
	// default:  - default (opt)
	// case c:   \ cases2 (opt)
	// case d:   /

	public:
		//////////////////////////////////////////////////////////////////////////
		CaseBlockNode(ClauseListNode* Cases1, CaseClauseNode* Default, ClauseListNode* Cases2)
		{
			m_Default = Default;

			if (Cases1)
			{
				m_Cases1 = Cases1->m_Next;
				Cases1->m_Next = NULL;
			}
			else m_Cases1 = NULL;

			
			if (Cases2)
			{
				m_Cases2 = Cases2->m_Next;
				Cases2->m_Next = NULL;
			}
			else m_Cases2 = NULL;
		};

		//////////////////////////////////////////////////////////////////////////
		virtual void Generate(CodeGen* Context);

		//////////////////////////////////////////////////////////////////////////
		virtual void GetSymbols(CodeGen* Context)
		{
			if (m_Cases1) m_Cases1->GetSymbols(Context);
			if (m_Default) m_Default->GetSymbols(Context);
			if (m_Cases2) m_Cases2->GetSymbols(Context);
		}

	private:
		ClauseListNode* m_Cases1;
		CaseClauseNode* m_Default;
		ClauseListNode* m_Cases2;

	};

	//////////////////////////////////////////////////////////////////////////
	class SwitchNode : public StatementNode
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		SwitchNode(Node* Expr, CaseBlockNode* Block)
		{
			m_Expr = Expr;
			m_Block = Block;
		};

		//////////////////////////////////////////////////////////////////////////
		virtual bool SupportsBreak(BreakType Type)
		{
			switch (Type)
			{
			case Break:
				return true;
			default:
				return false;
			}
		}

		//////////////////////////////////////////////////////////////////////////
		virtual int GetBreakAddress(BreakType Type)
		{
			return -1;
		}

		//////////////////////////////////////////////////////////////////////////
		virtual void Generate(CodeGen* Context);

		//////////////////////////////////////////////////////////////////////////
		virtual void GetSymbols(CodeGen* Context)
		{
			m_Expr->GetSymbols(Context);
			m_Block->GetSymbols(Context);
		}

	private:
		Node* m_Expr;
		CaseBlockNode* m_Block;
	};


	//////////////////////////////////////////////////////////////////////////
	class NullNode : public Node
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		NullNode(){};

		//////////////////////////////////////////////////////////////////////////
		virtual void Generate(CodeGen* Context);

	};

	//////////////////////////////////////////////////////////////////////////
	class BooleanNode : public Node
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		BooleanNode(bool Value)
		{
			m_Value = Value;
		};

		//////////////////////////////////////////////////////////////////////////
		virtual void Generate(CodeGen* Context);

	private:
		bool m_Value;
	};

	//////////////////////////////////////////////////////////////////////////
	class NumberNode : public Node
	{
	public:
		enum Type { Integer, Double };
		//////////////////////////////////////////////////////////////////////////
		NumberNode(double Value)
		{
			m_Type = Double;
			m_ValDouble = Value;
			m_ValInt = 0;
		};

		//////////////////////////////////////////////////////////////////////////
		NumberNode(int Value)
		{
			m_Type = Integer;
			m_ValInt = Value;
			m_ValDouble = 0.0f;
		};

		//////////////////////////////////////////////////////////////////////////
		virtual void Generate(CodeGen* Context);

	private:
		double m_ValDouble;
		int m_ValInt;
		Type m_Type;
	};

	//////////////////////////////////////////////////////////////////////////
	class StringNode : public Node
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		StringNode(char* Value)
		{
			m_Value = new char[strlen(Value)+1];
			strcpy(m_Value, Value);
		};

		//////////////////////////////////////////////////////////////////////////
		virtual ~StringNode()
		{
			if (m_Value) delete m_Value;
		}

		//////////////////////////////////////////////////////////////////////////
		virtual void Generate(CodeGen* Context);

	private:
		char* m_Value;
	};

	//////////////////////////////////////////////////////////////////////////
	class ThisNode : public Node
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		ThisNode(){};

		//////////////////////////////////////////////////////////////////////////
		virtual void Generate(CodeGen* Context);

	};

	//////////////////////////////////////////////////////////////////////////
	class ResolveNode : public Node
	{
	public:
		
		//////////////////////////////////////////////////////////////////////////
		ResolveNode(Identifier* ID)
		{
			m_ID = ID;
			m_Type = Variable;
		};

		//////////////////////////////////////////////////////////////////////////
		void SetType(IdentifierType Type)
		{
			m_Type = Type;
		}

		//////////////////////////////////////////////////////////////////////////
		const AnsiString GetName()
		{
			return m_ID->GetName();
		}

		//////////////////////////////////////////////////////////////////////////
		virtual bool IsReference()
		{
			return true;
		}

		//////////////////////////////////////////////////////////////////////////
		virtual bool IsIdentifier()
		{
			return true;
		}

		//////////////////////////////////////////////////////////////////////////
		virtual void Generate(CodeGen* Context);

	private:
		IdentifierType m_Type;
	};

	//////////////////////////////////////////////////////////////////////////
	class GroupNode : public Node
	{
	// ( Expr )
	public:
		//////////////////////////////////////////////////////////////////////////
		GroupNode(Node* Group)
		{
			m_Group = Group;
		};

		//////////////////////////////////////////////////////////////////////////
		virtual void Generate(CodeGen* Context)
		{
			Node::Generate(Context);

			m_Group->Generate(Context);
		}

		//////////////////////////////////////////////////////////////////////////
		virtual void GetSymbols(CodeGen* Context)
		{
			m_Group->GetSymbols(Context);
		}

	private:
		Node* m_Group;
	};

	//////////////////////////////////////////////////////////////////////////
	class AccessorNode1 : public Node
	{
	// Expr1[Expr2]
	public:
		//////////////////////////////////////////////////////////////////////////
		AccessorNode1(Node* Object, Node* Property)
		{
			m_Object = Object;
			m_Property = Property;
		};

		//////////////////////////////////////////////////////////////////////////
		virtual bool IsReference()
		{
			return true;
		}

		//////////////////////////////////////////////////////////////////////////
		virtual void Generate(CodeGen* Context);

		//////////////////////////////////////////////////////////////////////////
		virtual void GetSymbols(CodeGen* Context)
		{
			m_Object->GetSymbols(Context);
			m_Property->GetSymbols(Context);		
		}

	private:
		Node* m_Object;
		Node* m_Property;
	};

	//////////////////////////////////////////////////////////////////////////
	class AccessorNode2 : public Node
	{
	// Object.Ident
	public:
		//////////////////////////////////////////////////////////////////////////
		AccessorNode2(Node* Object, Identifier* ID)
		{
			m_Object = Object;
			m_ID = ID;
		};

		//////////////////////////////////////////////////////////////////////////
		virtual bool IsReference()
		{
			return true;
		}

		//////////////////////////////////////////////////////////////////////////
		virtual void Generate(CodeGen* Context);

		//////////////////////////////////////////////////////////////////////////
		virtual void GetSymbols(CodeGen* Context)
		{
			m_Object->GetSymbols(Context);
		}

	private:
		Node* m_Object;
	};

	//////////////////////////////////////////////////////////////////////////
	class NewExprNode : public Node
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		NewExprNode(Node* Expr)
		{
			m_Expr = Expr;
			m_Arguments = NULL;
		};

		//////////////////////////////////////////////////////////////////////////
		NewExprNode(Node* Expr, ArgumentsNode* Arguments)
		{
			m_Expr = Expr;
			m_Arguments = Arguments;
		};

		//////////////////////////////////////////////////////////////////////////
		virtual void Generate(CodeGen* Context);

		//////////////////////////////////////////////////////////////////////////
		virtual void GetSymbols(CodeGen* Context)
		{
			m_Expr->GetSymbols(Context);
			if (m_Arguments) m_Arguments->GetSymbols(Context);		
		}

	private:
		Node* m_Expr;
		ArgumentsNode* m_Arguments;
	};

	//////////////////////////////////////////////////////////////////////////
	class FunctionCallNode : public Node
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		FunctionCallNode(Node* Expr, ArgumentsNode* Arguments)
		{
			m_Expr = Expr;
			m_Arguments = Arguments;
		};

		//////////////////////////////////////////////////////////////////////////
		virtual bool IsReference()
		{
			return true;
		}

		//////////////////////////////////////////////////////////////////////////
		virtual void Generate(CodeGen* Context);

		//////////////////////////////////////////////////////////////////////////
		virtual void GetSymbols(CodeGen* Context)
		{
			m_Expr->GetSymbols(Context);
			if (m_Arguments) m_Arguments->GetSymbols(Context);
		}

	private:
		Node* m_Expr;
		ArgumentsNode* m_Arguments;
	};

	//////////////////////////////////////////////////////////////////////////
	class PostfixNode : public Node
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		PostfixNode(Node* Expr, Operator Oper)
		{
			m_Expr = Expr;
			m_Oper = Oper;
		};

		//////////////////////////////////////////////////////////////////////////
		virtual void Generate(CodeGen* Context);

		//////////////////////////////////////////////////////////////////////////
		virtual void GetSymbols(CodeGen* Context)
		{
			m_Expr->GetSymbols(Context);
		}

	private:
		Node* m_Expr;
		Operator m_Oper;
	};


	//////////////////////////////////////////////////////////////////////////
	class VoidNode : public Node
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		VoidNode(Node* Expr)
		{
			m_Expr = Expr;
		};

		//////////////////////////////////////////////////////////////////////////
		virtual void Generate(CodeGen* Context);

		//////////////////////////////////////////////////////////////////////////
		virtual void GetSymbols(CodeGen* Context)
		{
			m_Expr->GetSymbols(Context);
		}

	private:
		Node* m_Expr;
	};

	//////////////////////////////////////////////////////////////////////////
	class TypeOfNode : public Node
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		TypeOfNode(Node* Expr)
		{
			m_Expr = Expr;
		};

		//////////////////////////////////////////////////////////////////////////
		virtual void Generate(CodeGen* Context);

		//////////////////////////////////////////////////////////////////////////
		virtual void GetSymbols(CodeGen* Context)
		{
			m_Expr->GetSymbols(Context);
		}

	private:
		Node* m_Expr;
	};

	//////////////////////////////////////////////////////////////////////////
	class PrefixNode : public Node
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		PrefixNode(Operator Oper, Node* Expr)
		{
			m_Oper = Oper;
			m_Expr = Expr;
		};

		//////////////////////////////////////////////////////////////////////////
		virtual void Generate(CodeGen* Context);

		//////////////////////////////////////////////////////////////////////////
		virtual void GetSymbols(CodeGen* Context)
		{
			m_Expr->GetSymbols(Context);
		}

	private:
		Operator m_Oper;
		Node* m_Expr;
	};

	//////////////////////////////////////////////////////////////////////////
	class UnaryPlusNode : public Node
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		UnaryPlusNode(Node* Expr)
		{
			m_Expr = Expr;
		};

		//////////////////////////////////////////////////////////////////////////
		virtual void Generate(CodeGen* Context);

		//////////////////////////////////////////////////////////////////////////
		virtual void GetSymbols(CodeGen* Context)
		{
			m_Expr->GetSymbols(Context);
		}

	private:
		Node* m_Expr;
	};

	//////////////////////////////////////////////////////////////////////////
	class NegateNode : public Node
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		NegateNode(Node* Expr)
		{
			m_Expr = Expr;
		};

		//////////////////////////////////////////////////////////////////////////
		virtual void Generate(CodeGen* Context);

		//////////////////////////////////////////////////////////////////////////
		virtual void GetSymbols(CodeGen* Context)
		{
			m_Expr->GetSymbols(Context);
		}

	private:
		Node* m_Expr;
	};

	//////////////////////////////////////////////////////////////////////////
	class LogicalNotNode : public Node
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		LogicalNotNode(Node* Expr)
		{
			m_Expr = Expr;
		};

		//////////////////////////////////////////////////////////////////////////
		virtual void Generate(CodeGen* Context);

		//////////////////////////////////////////////////////////////////////////
		virtual void GetSymbols(CodeGen* Context)
		{
			m_Expr->GetSymbols(Context);
		}

	private:
		Node* m_Expr;
	};

	//////////////////////////////////////////////////////////////////////////
	class MultNode : public Node
	{
	public:
		enum Type { Multiply, Divide, Modulo };
		//////////////////////////////////////////////////////////////////////////
		MultNode(Node* Term1, Node* Term2, Type Oper)
		{
			m_Term1 = Term1;
			m_Term2 = Term2;
			m_Oper = Oper;
		};

		//////////////////////////////////////////////////////////////////////////
		virtual void Generate(CodeGen* Context);

		//////////////////////////////////////////////////////////////////////////
		virtual void GetSymbols(CodeGen* Context)
		{
			m_Term1->GetSymbols(Context);
			m_Term2->GetSymbols(Context);
		}

	private:
		Node* m_Term1;
		Node* m_Term2;
		Type m_Oper;
	};

	//////////////////////////////////////////////////////////////////////////
	class AddNode : public Node
	{
	public:
		enum Type { Add, Subtract };
		//////////////////////////////////////////////////////////////////////////
		AddNode(Node* Term1, Node* Term2, Type Oper)
		{
			m_Term1 = Term1;
			m_Term2 = Term2;
			m_Oper = Oper;
		};

		//////////////////////////////////////////////////////////////////////////
		virtual void Generate(CodeGen* Context);

		//////////////////////////////////////////////////////////////////////////
		virtual void GetSymbols(CodeGen* Context)
		{
			m_Term1->GetSymbols(Context);
			m_Term2->GetSymbols(Context);
		}

	private:
		Node* m_Term1;
		Node* m_Term2;
		Type m_Oper;
	};

	//////////////////////////////////////////////////////////////////////////
	class RelationalNode : public Node
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		RelationalNode(Node* Expr1, Operator Oper, Node* Expr2)
		{
			m_Expr1 = Expr1;
			m_Expr2 = Expr2;
			m_Oper = Oper;
		};

		//////////////////////////////////////////////////////////////////////////
		virtual void Generate(CodeGen* Context);

		//////////////////////////////////////////////////////////////////////////
		virtual void GetSymbols(CodeGen* Context)
		{
			m_Expr1->GetSymbols(Context);
			m_Expr2->GetSymbols(Context);
		}

	private:
		Node* m_Expr1;
		Node* m_Expr2;
		Operator m_Oper;
	};

	//////////////////////////////////////////////////////////////////////////
	class EqualNode : public Node
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		EqualNode(Node* Expr1, Operator Oper, Node* Expr2)
		{
			m_Expr1 = Expr1;
			m_Expr2 = Expr2;
			m_Oper = Oper;
		};

		//////////////////////////////////////////////////////////////////////////
		virtual void Generate(CodeGen* Context);

		//////////////////////////////////////////////////////////////////////////
		virtual void GetSymbols(CodeGen* Context)
		{
			m_Expr1->GetSymbols(Context);
			m_Expr2->GetSymbols(Context);
		}

	private:
		Node* m_Expr1;
		Node* m_Expr2;
		Operator m_Oper;
	};

	//////////////////////////////////////////////////////////////////////////
	class BinaryLogicalNode : public Node
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		BinaryLogicalNode(Node* Expr1, Operator Oper, Node* Expr2)
		{
			m_Expr1 = Expr1;
			m_Expr2 = Expr2;
			m_Oper = Oper;
		};

		//////////////////////////////////////////////////////////////////////////
		virtual void Generate(CodeGen* Context);

		//////////////////////////////////////////////////////////////////////////
		virtual void GetSymbols(CodeGen* Context)
		{
			m_Expr1->GetSymbols(Context);
			m_Expr2->GetSymbols(Context);
		}

	private:
		Node* m_Expr1;
		Node* m_Expr2;
		Operator m_Oper;
	};

	//////////////////////////////////////////////////////////////////////////
	class ConditionalNode : public Node
	{
	// Cond ? Expr1 : Expr2
	public:
		//////////////////////////////////////////////////////////////////////////
		ConditionalNode(Node* Cond, Node* Expr1, Node* Expr2)
		{
			m_Cond = Cond;
			m_Expr1 = Expr1;
			m_Expr2 = Expr2;
		}

		//////////////////////////////////////////////////////////////////////////
		virtual void Generate(CodeGen* Context);

		//////////////////////////////////////////////////////////////////////////
		virtual void GetSymbols(CodeGen* Context)
		{
			m_Cond->GetSymbols(Context);
			m_Expr1->GetSymbols(Context);
			m_Expr2->GetSymbols(Context);
		}

	private:
		Node* m_Cond;
		Node* m_Expr1;
		Node* m_Expr2;
	};

	//////////////////////////////////////////////////////////////////////////
	class AssignNode : public Node
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		AssignNode(Node* Left, Operator Oper, Node* Expr)
		{
			m_Left = Left;
			m_Oper = Oper;
			m_Expr = Expr;
		};

		//////////////////////////////////////////////////////////////////////////
		virtual void Generate(CodeGen* Context);

		//////////////////////////////////////////////////////////////////////////
		virtual void GetSymbols(CodeGen* Context)
		{
			m_Left->GetSymbols(Context);
			m_Expr->GetSymbols(Context);
		}

		//////////////////////////////////////////////////////////////////////////
		virtual bool IsAssignment()
		{
			return true;
		}

	private:
		Node* m_Left;
		Operator m_Oper;
		Node* m_Expr;
	};

	//////////////////////////////////////////////////////////////////////////
	class CommaNode : public Node
	{
	// expr1, expr2 (returns expr2)
	public:
		//////////////////////////////////////////////////////////////////////////
		CommaNode(Node* Expr1, Node* Expr2)
		{
			m_Expr1 = Expr1;
			m_Expr2 = Expr2;
		};

		//////////////////////////////////////////////////////////////////////////
		virtual void Generate(CodeGen* Context);

		//////////////////////////////////////////////////////////////////////////
		virtual void GetSymbols(CodeGen* Context)
		{
			m_Expr1->GetSymbols(Context);
			m_Expr2->GetSymbols(Context);
		}

	private:
		Node* m_Expr1;
		Node* m_Expr2;
	};


	//////////////////////////////////////////////////////////////////////////
	class VarStatementNode : public StatementNode
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		VarStatementNode(VarDeclListNode* Declarations)
		{
			// claim the first member of the list and break the circular list
			m_Declarations = Declarations->m_Next;
			Declarations->m_Next = NULL;
		};

		//////////////////////////////////////////////////////////////////////////
		virtual void Generate(CodeGen* Context)
		{
			Node::Generate(Context);
			m_Declarations->Generate(Context);
		}

		//////////////////////////////////////////////////////////////////////////
		virtual void GetSymbols(CodeGen* Context)
		{
			m_Declarations->GetSymbols(Context);
		}

	private:
		VarDeclListNode* m_Declarations;
	};

	//////////////////////////////////////////////////////////////////////////
	class EmptyStatementNode : public StatementNode
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		EmptyStatementNode(){};

	};

	//////////////////////////////////////////////////////////////////////////
	class ExprStatementNode : public StatementNode
	{
	public:	
		//////////////////////////////////////////////////////////////////////////
		ExprStatementNode(Node* Expr)
		{
			m_Expr = Expr;
		};

		//////////////////////////////////////////////////////////////////////////
		virtual void Generate(CodeGen* Context);

		//////////////////////////////////////////////////////////////////////////
		virtual void GetSymbols(CodeGen* Context)
		{
			m_Expr->GetSymbols(Context);
		}

	private:
		Node* m_Expr;
	};

	//////////////////////////////////////////////////////////////////////////
	class IfNode : public StatementNode
	{
	// if Expr Statement1 else Statement2(opt)
	public:
		//////////////////////////////////////////////////////////////////////////
		IfNode(Node* Expr, StatementNode* Statement1, StatementNode* Statement2)
		{
			m_Expr = Expr;
			m_Statement1 = Statement1;
			m_Statement2 = Statement2;
		};

		//////////////////////////////////////////////////////////////////////////
		virtual void Generate(CodeGen* Context);

		//////////////////////////////////////////////////////////////////////////
		virtual void GetSymbols(CodeGen* Context)
		{
			m_Expr->GetSymbols(Context);
			m_Statement1->GetSymbols(Context);
			if (m_Statement2) m_Statement2->GetSymbols(Context);
		}

	private:
		Node* m_Expr;
		StatementNode* m_Statement1;
		StatementNode* m_Statement2;
	};

	//////////////////////////////////////////////////////////////////////////
	class DoWhileNode : public StatementNode
	{
	// do Statement while (Expr)
	public:
		//////////////////////////////////////////////////////////////////////////
		DoWhileNode(StatementNode* Statement, Node* Expr)
		{
			m_Expr = Expr;
			m_Statement = Statement;
		};

		//////////////////////////////////////////////////////////////////////////
		virtual bool SupportsBreak(BreakType Type)
		{
			// supports both break and continue
			return true;
		}

		//////////////////////////////////////////////////////////////////////////
		virtual int GetBreakAddress(BreakType Type)
		{
			// both break and continue need to be resolved later
			return -1;
		}

		//////////////////////////////////////////////////////////////////////////
		virtual void Generate(CodeGen* Context);

		//////////////////////////////////////////////////////////////////////////
		virtual void GetSymbols(CodeGen* Context)
		{
			m_Statement->GetSymbols(Context);
			m_Expr->GetSymbols(Context);
		}

	private:
		StatementNode* m_Statement;
		Node* m_Expr;
	};

	//////////////////////////////////////////////////////////////////////////
	class WhileNode : public StatementNode
	{
	// while (Expr) Statement
	public:
		//////////////////////////////////////////////////////////////////////////
		WhileNode(Node* Expr, StatementNode* Statement)
		{
			m_Expr = Expr;
			m_Statement = Statement;
			m_Begin = -1;
		};

		//////////////////////////////////////////////////////////////////////////
		virtual bool SupportsBreak(BreakType Type)
		{
			// supports both break and continue
			return true;
		}

		//////////////////////////////////////////////////////////////////////////
		virtual int GetBreakAddress(BreakType Type)
		{
			switch (Type)
			{
			case Continue:
				return m_Begin;
			case Break:
				return -1;
			}
			return -1;
		}

		//////////////////////////////////////////////////////////////////////////
		virtual void Generate(CodeGen* Context);

		//////////////////////////////////////////////////////////////////////////
		virtual void GetSymbols(CodeGen* Context)
		{
			m_Expr->GetSymbols(Context);
			m_Statement->GetSymbols(Context);		
		}

	private:
		StatementNode* m_Statement;
		Node* m_Expr;
		int m_Begin;
	};

	//////////////////////////////////////////////////////////////////////////
	class ForNode : public StatementNode
	{
	public:
		// for (Expr1; Expr2; Expr3) Statement
		//////////////////////////////////////////////////////////////////////////
		ForNode(Node* Expr1, Node* Expr2, Node* Expr3, StatementNode* Statement)
		{
			m_Expr1 = Expr1;
			m_Expr2 = Expr2;
			m_Expr3 = Expr3;
			m_Statement = Statement;
			m_Expr1IsDecl = false;
		};

		// for (var Expr1; Expr2; Expr3) Statement
		//////////////////////////////////////////////////////////////////////////
		ForNode(VarDeclListNode* Expr1, Node* Expr2, Node* Expr3, StatementNode* Statement)
		{
			// claim the first member of the list and break the circular list
			m_Expr1 = Expr1->m_Next;
			Expr1->m_Next = NULL;

			m_Expr2 = Expr2;
			m_Expr3 = Expr3;
			m_Statement = Statement;
			m_Expr1IsDecl = true;
		}

		//////////////////////////////////////////////////////////////////////////
		virtual bool SupportsBreak(BreakType Type)
		{
			// supports both break and continue
			return true;
		}

		//////////////////////////////////////////////////////////////////////////
		virtual int GetBreakAddress(BreakType Type)
		{
			// both break and contine need to be resolved later
			return -1;
		}

		//////////////////////////////////////////////////////////////////////////
		virtual void Generate(CodeGen* Context);

		//////////////////////////////////////////////////////////////////////////
		virtual void GetSymbols(CodeGen* Context)
		{
			if (m_Expr1) m_Expr1->GetSymbols(Context);
			if (m_Expr2) m_Expr2->GetSymbols(Context);
			if (m_Expr3) m_Expr3->GetSymbols(Context);
			m_Statement->GetSymbols(Context);		
		}

	private:
		Node* m_Expr1;
		Node* m_Expr2;
		Node* m_Expr3;
		bool m_Expr1IsDecl;
		StatementNode* m_Statement;
	};

	//////////////////////////////////////////////////////////////////////////
	class ForInNode : public StatementNode
	{
	public:
		// for (LeftExpr in Expr) Statement
		//////////////////////////////////////////////////////////////////////////
		ForInNode(Node* LeftExpr, Node* Expr, StatementNode* Statement)
		{
			m_Init = NULL;
			m_LeftExpr = LeftExpr;
			m_Expr = Expr;
			m_Declaration = NULL;
			m_Statement = Statement;
			m_Begin = -1;
		};

		// for (var ID = Init in Expr) Statement
		//////////////////////////////////////////////////////////////////////////
		ForInNode(Identifier* ID, AssignExprNode* Init, Node* Expr, StatementNode* Statement)
		{
			m_ID = ID;
			m_Init = Init;
			m_Expr = Expr;
			m_Statement = Statement;
			m_Declaration = new VarDeclNode(new Identifier(*ID), Init, VarDeclNode::Variable);
			m_LeftExpr = new ResolveNode(new Identifier(*ID));
			m_Begin = -1;
		};

		//////////////////////////////////////////////////////////////////////////
		virtual bool SupportsBreak(BreakType Type)
		{
			// supports both break and continue
			return true;
		}

		//////////////////////////////////////////////////////////////////////////
		virtual int GetBreakAddress(BreakType Type)
		{
			switch (Type)
			{
			case Continue:
				return m_Begin;
			case Break:
				return -1;
			}
			return -1;
		}

		//////////////////////////////////////////////////////////////////////////
		virtual void Generate(CodeGen* Context);

		//////////////////////////////////////////////////////////////////////////
		virtual void GetSymbols(CodeGen* Context)
		{
			m_LeftExpr->GetSymbols(Context);
			if (m_Init) m_Init->GetSymbols(Context);
			m_Expr->GetSymbols(Context);
			m_Statement->GetSymbols(Context);		
		}

	private:
		AssignExprNode* m_Init;
		Node* m_LeftExpr;
		Node* m_Expr;
		VarDeclNode* m_Declaration;
		StatementNode* m_Statement;
		int m_Begin;
	};

	//////////////////////////////////////////////////////////////////////////
	class ContinueNode : public StatementNode
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		ContinueNode(){};

		//////////////////////////////////////////////////////////////////////////
		ContinueNode(Identifier* ID)
		{
			m_ID = ID;
		};

		//////////////////////////////////////////////////////////////////////////
		virtual void Generate(CodeGen* Context);

	};

	//////////////////////////////////////////////////////////////////////////
	class BreakNode : public StatementNode
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		BreakNode(){};
		
		//////////////////////////////////////////////////////////////////////////
		BreakNode(Identifier* ID)
		{
			m_ID = ID;
		};

		//////////////////////////////////////////////////////////////////////////
		virtual void Generate(CodeGen* Context);

	};

	//////////////////////////////////////////////////////////////////////////
	class ReturnNode : public StatementNode
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		ReturnNode(Node* Value)
		{
			m_Value = Value;
		};

		//////////////////////////////////////////////////////////////////////////
		virtual void Generate(CodeGen* Context);

		//////////////////////////////////////////////////////////////////////////
		virtual void GetSymbols(CodeGen* Context)
		{
			if (m_Value) m_Value->GetSymbols(Context);
		}

	private:
		Node* m_Value;
	};


	//////////////////////////////////////////////////////////////////////////
	class LabelNode : public StatementNode
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		LabelNode(Identifier* Label, StatementNode* Statement)
		{
			m_ID = Label;
			m_Statement = Statement;
		};

		//////////////////////////////////////////////////////////////////////////
		virtual void AddLabel(Identifier* ID)
		{
			m_Statement->AddLabel(ID);
		}

		//////////////////////////////////////////////////////////////////////////
		virtual void Generate(CodeGen* Context)
		{
			Node::Generate(Context);
			m_Statement->Generate(Context);
		}

		//////////////////////////////////////////////////////////////////////////
		virtual void GetSymbols(CodeGen* Context)
		{
			m_Statement->GetSymbols(Context);
		}

	private:
		StatementNode* m_Statement;
	};
}
}
#endif // __WmeCodeNodes_H_
