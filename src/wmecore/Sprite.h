// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeSprite_H__
#define __WmeSprite_H__


#include "ScriptableObject.h"
#include "DocumentAwareObject.h"
#include "Rect.h"


namespace Wme
{
	class Viewport;
	class SpriteFrame;
	class ElementCollection;
	class InteractiveObject;

	//////////////////////////////////////////////////////////////////////////
	class WmeDllExport SpriteDrawingParams
	{
	public:
		SpriteDrawingParams()
		{
			Viewport = NULL;
			Parent = NULL;
			Scale = Ogre::Vector2(1.0, 1.0);
			Color = Ogre::ColourValue::White;
			Rotation = 0.0;
			MirrorX = MirrorY = false;
			AbsolutePos = true;
		}

		InteractiveObject* Parent;
		Wme::Viewport* Viewport;
		Ogre::Vector2 Scale;
		Ogre::ColourValue Color;
		Ogre::Real Rotation;
		bool MirrorX;
		bool MirrorY;
		bool AbsolutePos;
	};

	//////////////////////////////////////////////////////////////////////////
	class WmeDllExport SpritePlacement
	{
	public:
		SpritePlacement()
		{
			CenterPoint = Ogre::Vector2::ZERO;
			for (int i = 0; i < 4; i++)
			{
				Vertices[i] = Ogre::Vector2::ZERO;
			}
			Rotation = 0.0f;
		}

		Ogre::Vector2 Vertices[4];
		Ogre::Vector2 CenterPoint;
		float Rotation;
	};


	//////////////////////////////////////////////////////////////////////////
	class WmeDllExport Sprite : public ScriptableObject, public DocumentAwareObject
	{
	public:

		//////////////////////////////////////////////////////////////////////////
		class WmeDllExport Listener 
		{
		public:
			Listener() {}
			virtual ~Listener() {}

			virtual void OnSpriteChanged() {}
			virtual void OnSpriteFrameChanged() {}
			virtual void OnSpriteFinished() {}

		};

		Sprite(InteractiveObject* ownerObject);
		virtual ~Sprite();

		void Display(ElementCollection* elementCol, int targetX, int targetY, const SpriteDrawingParams& params) const;
		void Update();
		void Restart();

		typedef std::vector<SpriteFrame*> FrameCollection;
		FrameCollection& GetFrames() { return m_Frames; }

		bool IsLooping() { return m_IsLooping; }
		void SetLooping(bool looping);

		bool IsFinished() { return m_IsFinished; }

		virtual bool LoadFromFile(const WideString& fileName);
		virtual bool LoadFromXml(TiXmlElement* rootNode);
		virtual bool SaveToXml(TiXmlElement* rootNode);
		virtual Utf8String GetDocRootName() { return "WmeSprite"; }

		bool IsLoadedFromImage() { return m_IsLoadedFromImage; }

		void GetBoundingRect(Rect& rect) const;

		bool IsDirty() const { return m_IsDirty; }
		void SetDirty(bool isDirty);

		void AddListener(Listener* listener);
		void RemoveListener(Listener* listener);

        // ScriptableObject
		RTTI(Sprite);
		virtual WideString ConvertToString() { return L"Sprite object"; }
		virtual WideString GetTypeName() const { return L"Sprite"; }

	private:
		void Clear();

		bool LoadFromImage(const WideString& fileName);
		bool m_IsLoadedFromImage;

		FrameCollection m_Frames;
		int m_CurrentFrame;
		SpriteFrame* GetCurrentFrame() const;
		void SwitchToFrame(int frameIndex);
		void SwitchToNextFrame();
		int GetNextFrameIndex(bool& finished);
		int GetNextFrameIndex();
		void OnSpriteFinished();

		InteractiveObject* m_OwnerObject;

		size_t m_InitialFrame;
		bool m_IsLooping;
		bool m_IsReversed;
		bool m_IsFinished;
		unsigned long m_FrameStartTime;

		bool m_IsDirty;

		typedef std::vector<Listener*> ListenerList;
		ListenerList m_Listeners;
	};
}

#endif // __WmeSprite_H__
