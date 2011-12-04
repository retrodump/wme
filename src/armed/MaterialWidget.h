// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __ArmedMaterialWidget_H__
#define __ArmedMaterialWidget_H__


#include <QWidget>

using namespace Wme;

namespace Wme
{
	class MaterialInstance;
	class GenericProperty;
}


namespace Armed
{
	class FileButton;

	class MaterialWidget : public QWidget
	{
		Q_OBJECT

	public:
		MaterialWidget(QWidget* parent);
		~MaterialWidget();

	signals:
		void RebuildNeeded();


	private slots:
		void OnMaterialChanged(const QString& fileName);
		void OnMaterialRemoved();
		void OnMaterialPropChanged();
		void OnMaterialDefChanged(const QString& fileName);		

	protected:
		virtual void ResetMaterial(const QString& fileName) = 0;
		virtual void ReApplyMaterial() = 0;
		MaterialInstance* m_Material;

		QVBoxLayout* m_Layout;
		QFormLayout* m_FormLayout;
		FileButton* m_MatButton;
		QPushButton* m_DelButton;

		bool m_IgnoreEvents;

		void Rebuild();
		void SetState();
		void BuildPropGui();
		QWidget* GetPropWidget(GenericProperty* prop);	
	};
}

#endif // __ArmedMaterialWidget_H__
