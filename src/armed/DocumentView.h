// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __ArmedDocumentView_H__
#define __ArmedDocumentView_H__


#include <QObject>


namespace Armed
{
	class DocumentView : public QWidget
	{
		Q_OBJECT

	public:
		DocumentView(QWidget *parent);
		virtual ~DocumentView();

		virtual void SetFileName(const QString& fileName);
		virtual QString GetFileName() const { return m_FileName; }
		virtual QString GetTitle(bool includeModifyMark = true) const;
		virtual QString GetDescription() const;

		virtual QString GetDefaultName() const { return tr("Document"); }
		virtual QString GetDocExtension() const = 0;
		virtual QString GetDocDescription() const = 0;

		virtual bool IsDirty() const { return m_IsDirty; }
		virtual void SetDirty(bool value);

		virtual bool LoadData(const QString& fileName) = 0;
		virtual bool SaveData(const QString& fileName) = 0;
		virtual bool NewData() = 0;

		virtual void OnActivate() {};
		virtual void OnDocumentAdded();

		virtual void BuildActions();

	public slots:
		bool Load(const QString& fileName);
		bool Save();
		bool SaveAs();

	signals:
		void TitleChanged(const QString& newTitle, const QString& newDesc);

	protected:
		void ReadError(const QString& fileName);
		void WriteError(const QString& fileName);

	private:
		QString m_FileName;
		bool m_IsDirty;

		QAction* m_SaveAction;
		QAction* m_SaveAsAction;
	};
}

#endif // __ArmedDocumentView_H__
