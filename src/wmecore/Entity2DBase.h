// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeEntity2DBase_H__
#define __WmeEntity2DBase_H__


#include "Entity.h"


namespace Wme
{
	class Scene2DBase;
	class SceneNode2D;

	class WmeDllExport Entity2DBase : public Entity
	{
	public:
		Entity2DBase();
		virtual ~Entity2DBase();

		bool IsInStage() const  { return m_Stage != NULL; }
		Scene2DBase* GetStage() const { return m_Stage; }

		virtual void PutToStage(Scene2DBase* stage, Entity2DBase* parentEntity = NULL);
		virtual void RemoveFromStage();

		SceneNode2D* GetSceneNode() const { return m_SceneNode; }

		const Ogre::Vector2& GetPosition() const;
		void SetPosition(const Ogre::Vector2& pos);
		void SetPosition(float x, float y);

		const Ogre::Degree& GetRotation() const;
		void SetRotation(const Ogre::Degree& angle);
		void SetRotation(float degrees);

		const Ogre::Vector2& GetScale() const;
		void SetScale(const Ogre::Vector2& scale);
		void SetScale(float x, float y);

		// DocumentAwareObject
		virtual bool LoadFromXml(TiXmlElement* rootNode);
		virtual bool SaveToXml(TiXmlElement* rootNode);

	protected:
		Scene2DBase* m_Stage;
		SceneNode2D* m_SceneNode;

		mutable Ogre::Vector2 m_Position;
		mutable Ogre::Degree m_Rotation;
		mutable Ogre::Vector2 m_Scale;
	};
}

#endif // __WmeEntity2DBase_H__