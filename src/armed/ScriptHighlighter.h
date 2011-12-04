// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __ArmedScriptHighlighter_H__
#define __ArmedScriptHighlighter_H__


#include <QSyntaxHighlighter>


namespace Armed
{
	class ScriptHighlighter : public QSyntaxHighlighter
	{
		Q_OBJECT

	public:
		ScriptHighlighter(QTextDocument* document);
		~ScriptHighlighter();

	protected:
		void highlightBlock(const QString& text);

	private:
		enum Formats
		{
			FORMAT_NORMAL,
			FORMAT_NUMBER,
			FORMAT_COMMENT,
			FORMAT_STRING,
			FORMAT_ESCAPE,
			FORMAT_KEYWORD,
			FORMAT_FUNCTION,
			FORMAT_PROPERTY,
			FORMAT_PREPROCESSOR,
			FORMAT_SYMBOL,
			NUM_FORMATS
		};
		QTextCharFormat m_Formats[NUM_FORMATS];
		QSet<QString> m_Keywords;
		QSet<QString> m_Preprocessor;

		void DecodeState(int encodedVal, int& state, int& bracketNest, int& commentNest);
		int EncodeState(int state, int bracketNest, int commentNest);

		static QChar GetNextNonWhite(const QString& text, int pos);
	
	};
}

#endif // __ArmedScriptHighlighter_H__
