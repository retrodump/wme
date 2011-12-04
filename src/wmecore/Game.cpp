// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "Game.h"
#include "serialize.h"
#include "Renderer.h"
#include "PathUtil.h"
#include "LogManager.h"
#include "ScriptManager.h"
#include "ValueManager.h"
#include "FileManager.h"
#include "FontManager.h"
#include "MaterialManager.h"
#include "MeshManager.h"
#include "SkeletonManager.h"
#include "SpriteManager.h"
#include "ContentManager.h"
#include "SoundManager.h"
#include "VideoManager.h"
#include "DebugManager.h"
#include "GameScriptable.h"
#include "Stage.h"
#include "GuiStage.h"
#include "MousePickParams.h"

#include "ErrorLog.h"
#include "CodeNodes.h"
#include "MainWindow.h"
#include "ViewportLayout.h"


#include "Stack.h"
#include "Value.h"
#include "StringUtil.h"
#include "DiskFile.h"
#include "PackageBuilder.h"
#include "SpriteSubFrame.h"
#include "Scene.h"
#include "Font.h"
#include "Timer.h"


namespace Wme
{

Game* Game::s_Instance = NULL;

//////////////////////////////////////////////////////////////////////////
Game::Game()
{
	m_Renderer = NULL;
	m_LogMgr = NULL;
	m_ScriptMgr = NULL;
	m_ValueMgr = NULL;
	m_FileMgr = NULL;
	m_SpriteMgr = NULL;
	m_FontMgr = NULL;
	m_MaterialMgr = NULL;
	m_MeshMgr = NULL;
	m_SkeletonMgr = NULL;
	m_ContentMgr = NULL;
	m_SoundMgr = NULL;
	m_VideoMgr = NULL;
	m_DebugMgr = NULL;

	m_IsShuttingDown = false;
	m_HandleSequence = 0;

	m_GameScriptable = NULL;

	m_ActiveObject = NULL;

	m_LiveTimer = NULL;

	sub = NULL;
	m_Scene = NULL;

	sound = sound2 = NULL;
}

//////////////////////////////////////////////////////////////////////////
Game* Game::GetInstance() 
{
	if (s_Instance == NULL)
	{  
		s_Instance = new Game();
	}
	return s_Instance;
}

//////////////////////////////////////////////////////////////////////////
Game::~Game()
{
	SAFE_DELETE(sub);
	SAFE_DELETE(m_Scene);

	SAFE_DELETE(m_GameScriptable);

	DeleteTiers();

	SAFE_DELETE(m_LiveTimer);
	SAFE_DELETE(m_ContentMgr);
	SAFE_DELETE(m_VideoMgr);
	SAFE_DELETE(m_SoundMgr);
	SAFE_DELETE(m_SpriteMgr);
	SAFE_DELETE(m_FontMgr);
	SAFE_DELETE(m_ScriptMgr);
	SAFE_DELETE(m_MaterialMgr);
	SAFE_DELETE(m_MeshMgr);
	SAFE_DELETE(m_SkeletonMgr);
	SAFE_DELETE(m_Renderer);	
	SAFE_DELETE(m_ValueMgr);
	SAFE_DELETE(m_DebugMgr);
	SAFE_DELETE(m_FileMgr);		
	SAFE_DELETE(m_LogMgr);
}

//////////////////////////////////////////////////////////////////////////
void Game::DeleteTiers()
{
	GameTierMap::iterator it;
	for (it = m_Tiers.begin(); it != m_Tiers.end(); it++)
	{
		SAFE_DELETE(it->second);
	}
	m_Tiers.clear();
}

//////////////////////////////////////////////////////////////////////////
bool Game::Initialize(bool standalone)
{
	m_LogMgr = new LogManager();
	m_DebugMgr = new DebugManager();
	m_ValueMgr = new ValueManager();
	
	m_FileMgr = new FileManager();
	m_FileMgr->Initialize();
	
	m_Renderer = new Renderer();
	if (!m_Renderer->Initialize())
	{
		SAFE_DELETE(m_Renderer);
		return false;
	}

	m_MaterialMgr = new MaterialManager();
	m_MaterialMgr->Initialize();

	m_MeshMgr = new MeshManager();
	m_MeshMgr->Initialize();

	m_SkeletonMgr = new SkeletonManager();
	m_SkeletonMgr->Initialize();

	m_SoundMgr = new SoundManager();
	m_SoundMgr->Initialize();

	m_VideoMgr = new VideoManager();
	m_VideoMgr->Initialize();

	m_ContentMgr = new ContentManager();
	m_ContentMgr->Initialize(standalone);

	m_LiveTimer = new Ogre::Timer;
	m_LiveTimer->reset();

	m_Tiers[GameTier::TIER_LIVE] = new GameTier();
	m_Tiers[GameTier::TIER_GAME] = new GameTier();
	m_Tiers[GameTier::TIER_GUI] = new GameTier();

	m_ScriptMgr = new ScriptManager();

	m_SpriteMgr = new SpriteManager();

	m_FontMgr = new FontManager();
	m_FontMgr->Initialize();

	// create the scriptable interface
	m_GameScriptable = new GameScriptable();
	m_GameScriptable->Register();


	//m_GameScriptable->AddScript(L"test.script");
	//m_ScriptMgr->AddVariable(L"test", Value::VAR_CONST);

	if (standalone) m_ContentMgr->TestData(m_ContentMgr->GetMainWindow());
	Tests();

	//sound = m_SoundMgr->AddSound(L"menu.ogg", false, Sound::SOUND_EFFECT, Sound::STREAMING_YES);
	//sound = m_SoundMgr->AddSound(L"C:/Program Files (x86)/OpenAL 1.1 SDK/samples/media/stereo.wav", true, Sound::SOUND_EFFECT, Sound::STREAMING_YES);	
	//sound = m_SoundMgr->AddSound(L"F:/games/Ghost in the Sheet/src/music/rats.ogg", true);
	//sound->SetPosition(65);
	//sound->Play();


	return true;
}

//////////////////////////////////////////////////////////////////////////
void Game::SoundTest()
{
	if (sound) sound->SetPitch(sound->GetPitch() + 0.1f);
	/*
	if (sound)
	{
		//sound->SetPosition(34);
		//sound->Pause();
		sound->Play();

		//sound->GiveUpSource();
	}
	else
	{
		sound = m_SoundMgr->AddSound(L"menu.ogg", false, Sound::SOUND_EFFECT, Sound::STREAMING_YES);
		sound->SetPosition(65);
		sound->Play();
	}
	*/
}

//////////////////////////////////////////////////////////////////////////
void Game::SoundTest2()
{
	if (sound) sound->SetPitch(sound->GetPitch() - 0.1f);
	/*
	if (!sound2)
	{
		sound2 = m_SoundMgr->AddSound(L"F:/games/Ghost in the Sheet/src/music/rats.ogg", true, Sound::SOUND_SPEECH);
		sound2->SetPosition(0);
		sound2->Play();
	}
	*/
	//if (sound) sound->Pause();
}

//////////////////////////////////////////////////////////////////////////
void Game::Run()
{
	while (!m_IsShuttingDown)
	{
		if (!ProcessFrame()) break;
		Ogre::WindowEventUtilities::messagePump();
	}
}

//////////////////////////////////////////////////////////////////////////
bool Game::ProcessFrame()
{
	bool ret = true;

	GameTierMap::iterator it;
	for (it = m_Tiers.begin(); it != m_Tiers.end(); it++)
	{
		it->second->ProcessFrame();
	}

	m_DebugMgr->ProcessFrame();
	m_VideoMgr->ProcessFrame();
	m_SoundMgr->ProcessFrame();

	if (m_ContentMgr->ProcessFrame())
	{
		m_ActiveObject = m_ContentMgr->GetActiveObject();
	}
	else
	{
		m_ActiveObject = NULL;
		ret = false;
	}

	m_ScriptMgr->ProcessScripts();
	m_ValueMgr->CollectGarbage();

/*
	if (sound)
	{
		//float pos = sound->GetPosition();
		//m_TestInfo = StringUtil::ToString(pos);

		if (sound->IsFinished())
		{
			m_SoundMgr->RemoveSound(sound);
		}
	}
	else m_TestInfo = L"no sound";
*/
	if (sound2 && sound2->IsFinished()) m_SoundMgr->RemoveSound(sound2);
	

	return ret;
}

//////////////////////////////////////////////////////////////////////////
GameTier* Game::GetLiveTier()
{
	return m_Tiers[GameTier::TIER_LIVE];
}

//////////////////////////////////////////////////////////////////////////
GameTier* Game::GetDefaultTier()
{
	return m_Tiers[GameTier::TIER_GAME];
}

//////////////////////////////////////////////////////////////////////////
void Game::DisplayContent()
{
	m_ContentMgr->DisplayContent();
}

//////////////////////////////////////////////////////////////////////////
void Game::Log(const WideString& msg, const WideString& file, int line) const
{
	if (m_LogMgr) m_LogMgr->AddLine(msg, file, line);
}

//////////////////////////////////////////////////////////////////////////
void Game::Log(const Utf8String& msg, const Utf8String& file, int line) const
{
	Log(StringUtil::Utf8ToWide(msg), StringUtil::Utf8ToWide(file), line);
}

//////////////////////////////////////////////////////////////////////////
void Game::QuickMessage(const WideString& msg) const
{
	if (m_DebugMgr) m_DebugMgr->AddMessage(msg);
}

//////////////////////////////////////////////////////////////////////////
int Game::RegisterObject(ScriptableObject* object)
{
	int handle = ++m_HandleSequence;
	m_RegisteredObjects[handle] = object;
	return handle;
}

//////////////////////////////////////////////////////////////////////////
bool Game::UnregisterObject(int handle)
{
	ScriptableObjectMap::iterator it;
	it = m_RegisteredObjects.find(handle);

	if (it == m_RegisteredObjects.end()) return false;
	else
	{
		m_RegisteredObjects.erase(it);
		return true;
	}
}

//////////////////////////////////////////////////////////////////////////
ScriptableObject* Game::GetObjectByHandle(int handle)
{
	ScriptableObjectMap::iterator it;
	it = m_RegisteredObjects.find(handle);

	if (it == m_RegisteredObjects.end()) return NULL;
	else return it->second;
}

//////////////////////////////////////////////////////////////////////////
void Game::AddRootsToGrayList(ValueManager* valMgr)
{
	GetScriptMgr()->AddRootsToGrayList(valMgr);
	
	ScriptableObjectMap::const_iterator it;
	for (it = m_RegisteredObjects.begin(); it != m_RegisteredObjects.end(); it++)
	{
		(*it).second->AddRootsToGrayList(valMgr);
	}
}

//////////////////////////////////////////////////////////////////////////
void Game::Profile(const WideString& str)
{
	unsigned long time = m_LiveTimer->getMilliseconds();
	Log(StringUtil::ToString((int)time) + L"  " + str);
}

//////////////////////////////////////////////////////////////////////////
bool Game::SaveGame(const WideString& fileName) const
{
	try
	{
		std::ofstream ofs(fileName.c_str(), std::ios::binary);
		boost::archive::binary_oarchive oa(ofs);
		oa << *this;

		return true;
	}
	catch (boost::archive::archive_exception& e)
	{
		Log(L"Error: SaveGame failed");
		Log(e.what());
		return false;
	}
}

//////////////////////////////////////////////////////////////////////////
bool Game::LoadGame(const WideString& fileName)
{
	try
	{
		std::ifstream ifs(fileName.c_str(), std::ios::binary);
		boost::archive::binary_iarchive ia(ifs);
		ia >> *this;

		return true;
	}
	catch (boost::archive::archive_exception& e)
	{
		Log(L"Error: LoadGame failed");
		Log(e.what());
		return false;
	}
}

//////////////////////////////////////////////////////////////////////////
template <class Archive>void Game::serialize(Archive& ar, const unsigned int version)
{
	ar & m_IsShuttingDown;
	ar & m_TestInfo;
	ar & *m_ContentMgr;
	ar & m_GameScriptable;
}

//////////////////////////////////////////////////////////////////////////
void Game::Tests()
{

	/*
	Font* f = new Font();
	f->LoadFromFile(L"data/arial.font");
	//f->LoadFace(L"cour.ttf", 10, 96, 96, true);
	f->PrepareGlyphs(L" ABCDabcdìšèø_12346567890Ž blahblah whateverABCDEFGHIJKLMNOPQRSTUVWXYZ");
	f->SaveToFile(L"c://test.font");

	float kern = f->GetKerning(L'A', L'B');
	float kern2 = f->GetKerning(L'A', L'V');

	delete f;
	*/

	/*
	SaveGame(L"c:/test.sav");
	m_TestInfo.clear();
	LoadGame(L"c:/test.sav");
	*/

	/*
	FontGlyphCache* cache = new FontGlyphCache();
	cache->Initialize(20, 20);

	byte pixels[25];

	for(int i = 0; i < 100; i++)
	{
		memset(pixels, i, 25);

		cache->AddGlyph(L'A', 5, 5, pixels);		
	}
	cache->DebugSave();
	delete cache;
	*/

	/*
	File* f = GetFileMgr()->OpenFile(L"actors\\Molly\\molly.actor");
	char* buf = new char[f->GetSize()];
	f->Read(buf, f->GetSize());
	GetFileMgr()->CloseFile(f);
	delete [] buf;
	*/

	/*
	PackageBuilder* builder = new PackageBuilder();
	builder->ProcessDirectory(L"F:\\projects\\wme2\\bin\\data", L"c:\\", 0, L"Demo data");
	delete builder;
	*/



	/*
	bool b = PathUtil::MatchesMask(L"somefile.txt", L"*.txt");
	b = PathUtil::MatchesMask(L"somefile.txt", L"*.t?t");
	b = PathUtil::MatchesMask(L"somefile.txxt", L"some*.t?t");
	b = PathUtil::MatchesMask(L"somefile.txt", L"awesome*.t?t");
	*/

	/*
	std::ifstream f;
	f.open(L"C:\umbrella.png", std::ios::binary);
	size_t size;
	f.seekg(0, std::ios::end);
	size = f.tellg();
	f.seekg(0, std::ios::beg);

	byte* buf = new byte[size];
	f.read(reinterpret_cast<char*>(buf), size);
	f.close();

	m_FileMgr->SaveBufferToFile(L"c:\\umbrella_compressed.dat", buf, size, true);
	*/


	/*
	std::ofstream str;
	str.open(L"c:\\umbr_test.png", std::ios::binary);

	DiskFile f;
	f.Open(L"C:\umbrella_compressed.dat");
	//f.Open(L"C:\umbrella.png");

	while (!f.IsEOF())
	{
		byte buffer[1024];
		size_t read = f.Read(buffer, 1024);

		str.write(reinterpret_cast<char*>(buffer), read);
	}

	f.Seek(-100, File::SEEK_FROM_END);
	Log(StringUtil::ToString(f.GetPos()));

	f.Close();
	str.close();
	*/


	/*
	WideString str = L"aaa,bbb,,ccc,ddd;eee;;fff";
	WideStringListPtr list = StringUtil::Split(str, L",;", true);
	int i = 100;
	*/

	/*
	WideString test = PathUtil::GetDirectoryName(L"path/path\\filename.ext");
	WideString test2 = PathUtil::GetFileName(L"path/path\\filename.ext");
	WideString test3 = PathUtil::GetFileNameWithoutExtension(L"path/path\\filename.ext");
	WideString test4 = PathUtil::GetExtension(L"path/path\\filename.ext");
	WideString test5 = PathUtil::GetSafeLogFileName();
	PathUtil::CreateDirectory(L"c:\\aaa");

	ScriptableObject* obj = new ScriptableObject();
	int h = obj->Register();
	ScriptableObject* obj2 = GetObjectByHandle(h);
	delete obj;
	obj2 = GetObjectByHandle(h);
	*/


/*
	ErrorLog* Log = new ErrorLog();
	FunctionBodyNode* Body = ScriptParser::GetInstance().Parse(L"c:\\test.script", Log);
	if (Body)
	{
		FILE* f = fopen("c:\\test.script.log", "wt");
		Body->Debug(f);
		fclose(f);
		printf("PARSE OK\n");

		CodeGen* Context = new CodeGen();
		Body->GetSymbols(Context);
		Body->Generate(Context);

		DynamicBuffer* Code = new DynamicBuffer();
		Context->GetCompiledScript(Code);
		Code->DumpToFile(L"c:\\bytecode.bin");

		Script* scr = new Script(m_ScriptMgr, m_GameScriptable);
		scr->LoadFromBuffer(Code);
		scr->DumpToFile(L"c:\\script_dump.txt");
		delete scr;

		delete Code;
		delete Context;
	}
	for (size_t i=0; i<Log->GetEntries().size(); i++)
	{
		printf("%d: %s\n", Log->GetEntries()[i]->GetLine(), Log->GetEntries()[i]->GetText().c_str());
	}
	delete Log;

	ScriptParser::GetInstance().Cleanup();
*/

/*
	GameScriptable* obj = new GameScriptable();
	obj->Register();

	Stack s;
	s.Push(Value(obj));
	Value v = s.Pop();
	s.Push(v);
	delete obj;
	s.Push(v);
	v.Clear();
	s.Pop();
	s.Pop();
*/

}


} // namespace Wme
