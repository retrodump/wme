// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeCamera_H__
#define __WmeCamera_H__


#include "Object.h"


namespace Wme
{
	class Ogre::Camera;
	class Stage;
	class Viewport;
	class ScriptableObject;

	class WmeDllExport Camera : public Object
	{
	public:		
		Camera();
		virtual ~Camera();

		Ogre::Camera* GetOgreCamera() { return m_OgreCamera; }

		void Create(Stage* stage, const WideString& camName = L"");
		void CreateFromOgreCamera(Stage* stage, Ogre::Camera* camera);

		virtual void AdjustToViewport(Viewport* viewport) {};
		
		void SetStage(Stage* stage) { m_Stage = stage; }
		Stage* GetStage() const { return m_Stage; }

		void SetOwner(ScriptableObject* owner) { m_Owner = owner; }
		ScriptableObject* GetOwner() const { return m_Owner; }

		void SetCustomAspectRatio(bool customRatio) { m_CustomAspectRatio = customRatio; }
		bool HasCustomAspectRatio() const { return m_CustomAspectRatio; }

	private:
		void DestroyOgreCamera();

		Stage* m_Stage;
		ScriptableObject* m_Owner;

		bool m_OgreCameraOwned;
		Ogre::Camera* m_OgreCamera;

		bool m_CustomAspectRatio;

		static size_t s_CameraCount;
	};
}

#endif // __WmeCamera_H__
