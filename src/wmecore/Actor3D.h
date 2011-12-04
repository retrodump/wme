// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeActor3D_H__
#define __WmeActor3D_H__


#include "MeshEntity.h"
#include "NavMesh.h"
#include "AnimProp.h"


namespace Wme
{
	class Ogre::SceneNode;
	class Ogre::Entity;
	class Stage;
	class NavPath;


	class WmeDllExport Actor3D : public MeshEntity
	{
		friend class TaskActorTurn;
		friend class TaskActorMove;
		friend class TaskActorAnim;

	public:		
		Actor3D();
		virtual ~Actor3D();

		static Entity* CreateInstance() { return new Actor3D(); }
		virtual WideString GetEntitySubtype() const { return L"Actor3D"; }


		virtual void Update();

		void MoveTo(const Ogre::Vector3& targetPoint, bool enqueue = false);
		void TurnTo(const Ogre::Vector3& targetPoint, bool enqueue = false);
		void PlayFullBodyAnim(AnimProp& animProp, bool enqueue = false);

		void SetIdleAnimName(const WideString& name) { m_IdleAnimName = name; }
		WideString GetIdleAnimName() const { return m_IdleAnimName; }

		// ScriptableObject
		RTTI(Actor3D);
		virtual WideString ConvertToString() { return L"Actor3D object"; }
		virtual WideString GetTypeName() const { return L"Actor3D"; }

		// script interface
		virtual void RegisterScriptInterface();

		bool ScTest(Script* script, const WideString& methodName, bool async);

		// DocumentAwareObject
		virtual bool LoadFromXml(TiXmlElement* rootNode);
		virtual bool SaveToXml(TiXmlElement* rootNode);
		virtual Utf8String GetDocRootName() { return "WmeActor3D"; }


	private:
		void FollowPath();
		Ogre::Quaternion GetRotationTo(const Ogre::Vector3& point);

		void PrepareTurn(Ogre::Degree targetAngle, Ogre::Degree& newTargetAngle, bool& clockwise);

		float m_TurnSpeedMove;
		float m_TurnSpeedStand;
		float m_MoveSpeed;

		float m_MinTurningAngle;

		WideString m_IdleAnimName;
		WideString m_WalkAnimName;
		WideString m_TurnCWAnimName;
		WideString m_TurnCCWAnimName;
		int m_MovementAnimChannel;
		
		NavPath* m_Path;
	};
}

#endif // __WmeActor3D_H__
