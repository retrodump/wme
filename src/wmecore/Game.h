// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeGame_H__
#define __WmeGame_H__


#include "Object.h"
#include "GameTier.h"
#include <boost/serialization/access.hpp>


namespace Wme
{
	class Renderer;
	class LogManager;
	class ScriptManager;
	class ValueManager;
	class FileManager;
	class SpriteManager;
	class FontManager;
	class MaterialManager;
	class MeshManager;
	class SkeletonManager;
	class ContentManager;
	class SoundManager;
	class VideoManager;
	class DebugManager;
	class ScriptableObject;
	class GameScriptable;
	class SpriteSubFrame;
	class Scene;
	class InteractiveObject;
	class Sound;

	class WmeDllExport Game : public Object
	{
	public:
		virtual ~Game();
		static Game* GetInstance();

		bool Initialize(bool standalone);
		
		void Run();
		bool ProcessFrame();
		
		void Log(const WideString& msg, const WideString& file=L"", int line=-1) const;
		void Log(const Utf8String& msg, const Utf8String& file="", int line=-1) const;
		void QuickMessage(const WideString& msg) const;

		int RegisterObject(ScriptableObject* object);
		bool UnregisterObject(ScriptableObject* object);
		ScriptableObject* GetObjectByHandle(int handle);

		void DisplayContent();

		Renderer* GetRenderer() { return m_Renderer; }

		ScriptManager* GetScriptMgr() { return m_ScriptMgr; }
		ValueManager* GetValueMgr() { return m_ValueMgr; }
		FileManager* GetFileMgr() { return m_FileMgr; }
		SpriteManager* GetSpriteMgr() { return m_SpriteMgr; }
		FontManager* GetFontMgr() { return m_FontMgr; }
		MaterialManager* GetMaterialMgr() { return m_MaterialMgr; }
		MeshManager* GetMeshMgr() { return m_MeshMgr; }
		SkeletonManager* GetSkeletonMgr() { return m_SkeletonMgr; }
		ContentManager* GetContentMgr() { return m_ContentMgr; }
		SoundManager* GetSoundMgr() { return m_SoundMgr; }
		VideoManager* GetVideoMgr() { return m_VideoMgr; }
		DebugManager* GetDebugMgr() { return m_DebugMgr; }

		GameScriptable* GetGameScriptable() { return m_GameScriptable; }

		void AddRootsToGrayList(ValueManager* valMgr);

		InteractiveObject* GetActiveObject() const { return m_ActiveObject; }
		void SetActiveObject(InteractiveObject* activeObject);

		GameTier* GetLiveTier();
		GameTier* GetDefaultTier();

		WideString m_TestInfo;
		void Profile(const WideString& str);
		
		bool SaveGame(const WideString& fileName) const;
		bool LoadGame(const WideString& fileName);

		void SoundTest();
		void SoundTest2();
		
	protected:
		Game();		

	private:
		friend class boost::serialization::access; 
		template <class Archive>void serialize(Archive& ar, const unsigned int version);

		static Game* s_Instance;
		Renderer* m_Renderer;
		
		LogManager* m_LogMgr;
		ScriptManager* m_ScriptMgr;
		FontManager* m_FontMgr;
		MaterialManager* m_MaterialMgr;
		MeshManager* m_MeshMgr;
		SkeletonManager* m_SkeletonMgr;
		ValueManager* m_ValueMgr;
		FileManager* m_FileMgr;
		SpriteManager* m_SpriteMgr;
		ContentManager* m_ContentMgr;
		SoundManager* m_SoundMgr;
		VideoManager* m_VideoMgr;
		DebugManager* m_DebugMgr;
		
		GameScriptable* m_GameScriptable;

		InteractiveObject* m_ActiveObject;

		bool m_IsShuttingDown;

		int m_HandleSequence;
		typedef std::map<int, ScriptableObject*> ScriptableObjectMap;
		ScriptableObjectMap m_RegisteredObjects;

		typedef std::map<GameTier::TierType, GameTier*> GameTierMap;
		GameTierMap m_Tiers;
		void DeleteTiers();

		Ogre::Timer* m_LiveTimer;


		friend class GameScriptable;



		void Tests();
		SpriteSubFrame* sub;
		Scene* m_Scene;
		Sound* sound;
		Sound* sound2;
	};
}

#endif // __WmeGame_H__
