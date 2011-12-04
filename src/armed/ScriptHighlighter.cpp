// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "StdAfx.h"
#include "ScriptHighlighter.h"


namespace Armed
{


//////////////////////////////////////////////////////////////////////////
ScriptHighlighter::ScriptHighlighter(QTextDocument* document) : QSyntaxHighlighter(document)
{
	m_Formats[FORMAT_NUMBER].setForeground(QColor("#CC0000"));
	m_Formats[FORMAT_COMMENT].setForeground(QColor("#007F00"));
	m_Formats[FORMAT_STRING].setForeground(QColor("gray"));
	m_Formats[FORMAT_ESCAPE].setForeground(QColor("black"));
	m_Formats[FORMAT_KEYWORD].setForeground(QColor("blue"));
	m_Formats[FORMAT_FUNCTION].setForeground(QColor("maroon"));
	m_Formats[FORMAT_PROPERTY].setForeground(QColor("#2B91AF"));
	m_Formats[FORMAT_PREPROCESSOR].setForeground(QColor("purple"));

	m_Keywords << "global";
	m_Keywords << "var";
	m_Keywords << "and";
	m_Keywords << "or";
	m_Keywords << "if";
	m_Keywords << "else";
	m_Keywords << "while";
	m_Keywords << "for";
	m_Keywords << "in";
	m_Keywords << "do";
	m_Keywords << "break";
	m_Keywords << "continue";
	m_Keywords << "null";
	m_Keywords << "return";
	m_Keywords << "function";
	m_Keywords << "method";
	m_Keywords << "new";
	m_Keywords << "true";
	m_Keywords << "false";
	m_Keywords << "switch";
	m_Keywords << "case";
	m_Keywords << "default";
	m_Keywords << "void";
	m_Keywords << "delete";
	m_Keywords << "this";
	m_Keywords << "typeof";
	m_Keywords << "with";
	m_Keywords << "reserved";

	m_Preprocessor << "#include";


	int comment = 15;
	int bracket = 21;
	int state = 5;
}

//////////////////////////////////////////////////////////////////////////
ScriptHighlighter::~ScriptHighlighter()
{

}

//////////////////////////////////////////////////////////////////////////
void ScriptHighlighter::highlightBlock(const QString& text)
{
	// states
	enum
	{
		Initial = 0,
		Number = 1,
		Identifier = 2,
		String = 3,
		Comment = 4,
	};

	//restore previous state
	int state, bracketNest, commentNest;
	DecodeState(previousBlockState(), state, bracketNest, commentNest);


	int startPos = 0;
	int i = 0;
	while (i <= text.length())
	{
		QChar ch = (i < text.length()) ? text.at(i) : QChar();
		QChar next = (i < text.length() - 1) ? text.at(i + 1) : QChar();
		QChar prev = (i > 0) ? text.at(i - 1) : QChar();

		switch (state)
		{
		case Initial:
			startPos = i;
			if (ch.isSpace())
			{
				i++;
			}
			else if (ch.isDigit())
			{
				i++;
				state = Number;
			}
			else if (ch.isLetter() || ch == '_'  || ch == '#')
			{
				i++;
				state = Identifier;
			}
			else if (ch == '\'' || ch == '\"')
			{
				setFormat(i, 1, m_Formats[FORMAT_STRING]);
				i++;
				state = String;
			}
			else if (ch == '/' && next == '*')
			{
				i += 2;
				state = Comment;
				commentNest++;
			}
			else if (ch == '/' && next == '/')
			{
				i = text.length();
				setFormat(startPos, text.length() - startPos, m_Formats[FORMAT_COMMENT]);
			}
			else
			{
				if (!QString("(){}[]").contains(ch))
					setFormat(startPos, 1, m_Formats[FORMAT_SYMBOL]);

				if (ch =='{' || ch == '}')
				{
					//bracketPositions += i;
					if (ch == '{')
						bracketNest++;
					else
						bracketNest--;
				}
				i++;
				state = Initial;
			}
			break;

		case Number:
			if (ch.isSpace() || !ch.isDigit())
			{
				setFormat(startPos, i - startPos, m_Formats[FORMAT_NUMBER]);
				state = Initial;
			}
			else i++;
			break;

		case Identifier:
			if (ch.isSpace() || !(ch.isDigit() || ch.isLetter() || ch == '_'))
			{
				bool isFunctionCall = (GetNextNonWhite(text, i) == '(');

				QString token = text.mid(startPos, i - startPos).trimmed();

				if (m_Keywords.contains(token))
					setFormat(startPos, i - startPos, m_Formats[FORMAT_KEYWORD]);
				else if (m_Preprocessor.contains(token))
					setFormat(startPos, i - startPos, m_Formats[FORMAT_PREPROCESSOR]);
				else
				{
					// todo known attributes and functions
					if (isFunctionCall)
					{
						setFormat(startPos, i - startPos, m_Formats[FORMAT_FUNCTION]);
					}
				}


				state = Initial;
			}
			else i++;
			break;

		case String:
			if (ch == text.at(startPos))
			{				
				if (prev != '\\')
				{
					setFormat(i, 1, m_Formats[FORMAT_STRING]);
					i++;					
					state = Initial;
				}
				else
				{
					setFormat(i - 1, 2, m_Formats[FORMAT_ESCAPE]);
					i++;
				}
			}
			else if (prev == '\\' && ch != '\\')
			{
				setFormat(i - 1, 2, m_Formats[FORMAT_ESCAPE]);
				i++;				
			}
			else
			{
				setFormat(i, 1, m_Formats[FORMAT_STRING]);
				i++;
			}
			break;

		case Comment:
			if (ch == '*' && next == '/')
			{
				i += 2;
				setFormat(startPos, i - startPos, m_Formats[FORMAT_COMMENT]);
				commentNest--;
				if (commentNest <= 0) state = Initial;
			}
			else if (ch == '/' && next == '*')
			{
				i += 2;
				setFormat(startPos, i - startPos, m_Formats[FORMAT_COMMENT]);
				commentNest++;
			}
			else i++;
			break;

		default:
			state = Initial;
			break;

		}
	}

	if (state == Comment)
		setFormat(startPos, text.length(), m_Formats[FORMAT_COMMENT]);
	else
		state = Initial;


	// store current state
	setCurrentBlockState(EncodeState(state, bracketNest, commentNest));
}

//////////////////////////////////////////////////////////////////////////
void ScriptHighlighter::DecodeState(int encodedVal, int& state, int& bracketNest, int& commentNest)
{
	if (encodedVal < 0)
	{
		state = 0;
		bracketNest = 0;
		commentNest = 0;
		return;
	}

	commentNest = encodedVal >> 12;
	bracketNest = encodedVal >> 4 & 255;
	state = encodedVal & 15;
}

//////////////////////////////////////////////////////////////////////////
int ScriptHighlighter::EncodeState(int state, int bracketNest, int commentNest)
{
	// 000000000000CCCCCCCCBBBBBBBBSSSS
	int encodedVal;
    encodedVal = (state & 15) | (bracketNest << 4) | (commentNest << 12);

	return encodedVal;
}

//////////////////////////////////////////////////////////////////////////
QChar ScriptHighlighter::GetNextNonWhite(const QString& text, int pos)
{
	for (int i = pos; i < text.length(); i++)
	{
		QChar ret = text.at(i);
		QString ch = ret;
		if (ret != ' ' && ret != '\t' && ret != '\n') return ret;
	}
	return QChar();	
}


} // namespace Armed
