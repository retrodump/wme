// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeScaleGizmo_H__
#define __WmeScaleGizmo_H__


#include "Gizmo.h"


namespace Wme
{
	class LineElement3D;

	class WmeDllExport ScaleGizmo : public Gizmo
	{
	public:
		ScaleGizmo(SceneNodeEditor* editor);
		~ScaleGizmo();

		void Create();

		void ConformTo(NodeSelection* selection);

		void HighlightPart(Ogre::Entity* entity = NULL);
		void HighlightDragPart();


		bool GetAxis(Ogre::Entity* part, Ogre::Vector3& axis);
		bool GetNormal(Camera* camera, Ogre::Entity* part, Ogre::Vector3& normal);

		bool StartDrag(Viewport* viewport, Ogre::Entity* part);
		void EndDrag();
		void UpdateDrag();
		void Reset();

		void SetVisible(bool visible, NodeSelection* selection);

		void PreRender(Viewport* viewport, NodeSelection* selection);

	private:
		/*
		   1-----2
		  /|    /|
		 / |   / |
		5-----4  |
		|  0--|--3
		| /   | /
		|/    |/
		6-----7
		*/
		typedef enum {
			CORNER_FAR_LEFT_BOTTOM = 0,
			CORNER_FAR_LEFT_TOP = 1,
			CORNER_FAR_RIGHT_TOP = 2,
			CORNER_FAR_RIGHT_BOTTOM = 3,
			CORNER_NEAR_RIGHT_TOP = 4,
			CORNER_NEAR_LEFT_TOP = 5,
			CORNER_NEAR_LEFT_BOTTOM = 6,
			CORNER_NEAR_RIGHT_BOTTOM = 7,
			
			FACE_X_POS = 8,
			FACE_X_NEG = 9,
			FACE_Y_POS = 10,
			FACE_Y_NEG = 11,
			FACE_Z_POS = 12,
			FACE_Z_NEG = 13,
			NUM_HANDLES = 14
		} HandleEnum;

		Ogre::Entity* m_HandleEnt[NUM_HANDLES];
		Ogre::SceneNode* m_HandleNode[NUM_HANDLES];
		
		void GetHandlePosition(int handleType, Ogre::Vector3& pos);
		int GetPartType(Ogre::Entity* part);

		LineElement3D* m_Box;

		void PositionHandles(const Ogre::Vector3& min, const Ogre::Vector3& max);

		bool GetDragOrigin(Ogre::Entity* part, Viewport* viewport);
		Ogre::Entity* m_DragPart;
		Viewport* m_DragViewport;
		Ogre::Ray m_DragRay;
		Ogre::Vector3 m_DragCenter;
		Ogre::Vector3 m_DragOrigin;
		Ogre::Vector3 m_DragStoredScale;

		Ogre::Vector3 m_LastScale;

	};
}

#endif // __WmeScaleGizmo_H__