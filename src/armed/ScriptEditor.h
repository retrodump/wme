// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __ArmedScriptEditor_H__
#define __ArmedScriptEditor_H__


#include <QPlainTextEdit>


namespace Armed
{
	class ScriptHighlighter;
	class SideBar;

	//////////////////////////////////////////////////////////////////////////
	class ScriptEditor : public QPlainTextEdit
	{
		Q_OBJECT

	public:
		ScriptEditor(QWidget* parent);
		~ScriptEditor();

		void PaintSideBar(QPaintEvent* event);

	protected:
		void resizeEvent(QResizeEvent* event);
		void keyPressEvent(QKeyEvent* event);

	private slots:
		void UpdateSideBarWidth();
		void UpdateSideBar(const QRect& rect, int dy);
		void UpdateCursor();

	private:
		ScriptHighlighter* m_Highlighter;
		SideBar* m_SideBar;
	
	};


	//////////////////////////////////////////////////////////////////////////
	class SideBar : public QWidget
	{
	public:
		SideBar(ScriptEditor* parent) : QWidget(parent)
		{
			m_Editor = parent;
			setAutoFillBackground(true);
			RefreshFont();
		}

		int GetWidth(int numLines) const
		{
			int digits = QString::number(numLines).length();
			digits = std::max(2, digits);

			return fontMetrics().width(QString(digits + 1, '0')) + 5;
		}

		void RefreshFont()
		{
			QFont newFont = m_Editor->font();
			newFont.setPointSize(newFont.pointSize() - 2);
			setFont(newFont);
		}

	protected:
		void paintEvent(QPaintEvent* event)
		{
			m_Editor->PaintSideBar(event);
		}

	private:
		ScriptEditor* m_Editor;
	};

}

#endif // __ArmedScriptEditor_H__
