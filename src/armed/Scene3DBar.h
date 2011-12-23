// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __ArmedScene3DBar_H__
#define __ArmedScene3DBar_H__


#include <QWidget>
#include "ui_Scene3DBar.h"


namespace Armed
{
	class DocScene;

	class Scene3DBar : public QWidget
	{
		Q_OBJECT

	public:
		Scene3DBar(DocScene* parent = 0);
		~Scene3DBar();

		void Update();

	private:
		DocScene* m_Scene;

		Ui::Scene3DBar m_Ui;
	};
}

#endif // __ArmedScene3DBar_H__
