// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __ArmedDocScript_H__
#define __ArmedDocScript_H__


#include "DocumentView.h"


namespace Armed
{
	class ScriptEditor;
	class SearchBar;

	class DocScript : public DocumentView
	{
		Q_OBJECT

	public:
		DocScript(QWidget* parent);
		~DocScript();

		bool LoadData(const QString& fileName);
		bool SaveData(const QString& fileName);
		bool NewData();

		QString GetDocExtension() const { return tr("script"); }
		QString GetDocDescription() const { return tr("Script files"); }
		virtual QString GetDefaultName() const { return tr("Script"); }

		void OnActivate();
		void BuildActions();

	private slots:
		void OnModified(bool modified);
		void FindNext();
		void FindPrev();
		void Find(const QString& text, bool forward, bool inPlace = true);
		void FindNew();
		void FocusEditor();

	private:
		ScriptEditor* m_Editor;
		SearchBar* m_SearchBar;	
	};
}

#endif // __ArmedDocScript_H__
