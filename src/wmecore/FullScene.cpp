// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "FullScene.h"
#include "Viewport.h"
#include "View.h"
#include "Actor3D.h" // temp
#include "Timer.h" // temp
#include "StringUtil.h"
#include "MousePickParams.h"
#include "SceneNodeEditor.h"
#include "NavMeshEditor.h"
#include "ContentManager.h"

// temp
#include "ScriptManager.h"
#include "VideoManager.h"
#include "VideoClip.h"
#include "AnimProp.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
FullScene::FullScene()
{
	m_Actor = NULL;
	m_Actor2 = NULL;
}

//////////////////////////////////////////////////////////////////////////
FullScene::~FullScene()
{
	SAFE_DELETE(m_Actor);
	SAFE_DELETE(m_Actor2);
}


//////////////////////////////////////////////////////////////////////////
void FullScene::Create()
{
	Scene3DBase::Create();

	ImportDotSceneFile(L"scene/aaa.scene");
	AddScript(L"scene.script");

/*
	GetSceneMgr()->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_MODULATIVE);
	GetSceneMgr()->setShadowColour(Ogre::ColourValue(0.0, 0.0, 0.0, 0.0f));
	GetSceneMgr()->setShadowFarDistance(1500);
*/


	Ogre::MeshPtr navTest;

	try
	{
		navTest = Ogre::MeshManager::getSingleton().load(StringUtil::WideToUtf8(L"navmesh.mesh"), Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	}
	catch (Ogre::Exception&)
	{
		navTest = GetSceneMgr()->getEntity("Plane01")->getMesh();
	}
	
	GetNavMesh()->LoadFromOgreMesh(navTest);

/*
	m_Actor = new Actor3D();
	m_Actor->SetName(L"ninja");
	m_Actor->Create(L"ninja.mesh", L"ninja.skeleton", L"ninja.material");
	m_Actor->PutToStage(this);
*/

	/*
	m_Actor = new Actor3D();
	m_Actor->Register();
	m_Actor->SetName(L"male");
	m_Actor->Create(L"male.mesh", L"male.skeleton");
	m_Actor->SetMaterial(L"test.wmemat", 0);
	m_Actor->PutToStage(this);
	m_Actor->SetOwnedByStage(true);
	m_Actor->SetScale(Ogre::Vector3(100, 100, 100));
	*/
	//m_Actor->AddScript(L"actor.script");

	m_Actor = new Actor3D();
	m_Actor->Register();
	m_Actor->SetName(L"sinbad");
	m_Actor->Create(L"sinbad/sinbad.mesh", L"sinbad/sinbad.skeleton");
	//m_Actor->SetMaterial(L"test.wmemat", 0);
	m_Actor->PutToStage(this);
	m_Actor->SetOwnedByStage(true);
	m_Actor->SetScale(Ogre::Vector3(18, 18, 18));
	m_Actor->SetMaterial(L"sinbad/sinbad_body.wmemat", 0);
	m_Actor->SetMaterial(L"sinbad/sinbad_body.wmemat", 1);
	m_Actor->SetMaterial(L"sinbad/sinbad_clothes.wmemat", 2);
	m_Actor->SetMaterial(L"sinbad/sinbad_body.wmemat", 3);
	m_Actor->SetMaterial(L"sinbad/sinbad_sword.wmemat", 4);
	m_Actor->SetMaterial(L"sinbad/sinbad_clothes.wmemat", 5);
	m_Actor->SetMaterial(L"sinbad/sinbad_clothes.wmemat", 6);
	m_Actor->SetIdleAnimName(L"IdleTop");

	m_Actor->SetPosition(Ogre::Vector3(86, 90, 309));

	Ogre::Quaternion rot;
	rot.FromAngleAxis(Ogre::Radian(Ogre::Degree(0)), Ogre::Vector3::UNIT_Y);
	m_Actor->SetOrientation(rot);

	Game::GetInstance()->GetScriptMgr()->AddVariable(L"actor");
	Value* val = Game::GetInstance()->GetScriptMgr()->GetVariableRef(L"actor");
	val->SetValue(m_Actor);
	
	//m_Actor->StartAnimation(0, L"idle", 0, false);


/*
	m_Actor2 = new Actor3D();
	m_Actor2->Register();
	m_Actor2->SetName(L"female");
	m_Actor2->Create(L"female_test.mesh", L"female_test.skeleton");
	m_Actor2->PutToStage(this);
	m_Actor2->SetOwnedByStage(true);
	m_Actor2->SetMaterial(L"female_test.wmemat", 0);
	m_Actor2->SetScale(Ogre::Vector3(2, 2, 2));
*/
	/*
	m_Actor2 = new Actor3D();
	m_Actor2->Register();
	m_Actor2->SetName(L"male2");
	m_Actor2->Create(L"male.mesh", L"male.skeleton");
	m_Actor2->PutToStage(this);
	m_Actor2->SetMaterial(L"test.wmemat", 0);
	m_Actor2->SetScale(Ogre::Vector3(100, 100, 100));
	*/
	m_Actor2 = new Actor3D();
	m_Actor2->Register();
	m_Actor2->SetName(L"emma");
	m_Actor2->Create(L"emma/emma.mesh", L"emma/emma.skeleton");
	m_Actor2->PutToStage(this);
	m_Actor2->SetOwnedByStage(true);
	m_Actor2->SetMaterial(L"emma/emma_body.wmemat", 0);
	m_Actor2->SetMaterial(L"emma/emma_hair.wmemat", 1);
	m_Actor2->SetMaterial(L"emma/emma_head.wmemat", 2);
	m_Actor2->SetMaterial(L"emma/emma_head.wmemat", 3);
	m_Actor2->SetScale(Ogre::Vector3(1, 1, 1));



	m_Actor2->SetPosition(Ogre::Vector3(0, 0, 512));


	Game::GetInstance()->GetScriptMgr()->AddVariable(L"actor2");
	Value* val2 = Game::GetInstance()->GetScriptMgr()->GetVariableRef(L"actor2");
	val2->SetValue(m_Actor2);


	m_Actor->AddAttachment(m_Actor2, L"Head");


	Game::GetInstance()->m_TestInfo = L"Game mode (press F1 to switch to editor mode)";


	// Rtt texture
	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().create("RttMat", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	Ogre::TextureUnitState* t = mat->getTechnique(0)->getPass(0)->createTextureUnitState("RttTest");
	t->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);

	Ogre::Entity* ent = GetSceneMgr()->getEntity("Box47");
	ent->setMaterial(mat);

	Ogre::Entity* ent2 = GetSceneMgr()->getEntity("Box45");
	ent2->setMaterial(mat);

	View* rttView = Game::GetInstance()->GetContentMgr()->GetViewByName(L"RttTest");
	if (rttView)
	{
		ent->getUserObjectBindings().setUserAny("pickable", Ogre::Any(PickableObject(PickableObject::PICKABLE_RTT_VIEW, rttView)));
		ent2->getUserObjectBindings().setUserAny("pickable", Ogre::Any(PickableObject(PickableObject::PICKABLE_RTT_VIEW, rttView)));
	}


	// theora texture
	/*
	VideoClip* clip = Game::GetInstance()->GetVideoMgr()->AddVideoClip(L"F:/projects/dependencies/libtheoraplayer-1.0RC2/demos/media/brawe_E.ogg", true);
	//VideoClip* clip = Game::GetInstance()->GetVideoMgr()->AddVideoClip(L"F:/projects/dependencies/libtheoraplayer-1.0RC2/demos/media/bunny.ogg", true);
	if (clip)
	{
		Ogre::Entity* ent3 = GetSceneMgr()->getEntity("Box44");
		clip->AttachToMaterial(StringUtil::Utf8ToWide(ent3->getSubEntity(0)->getMaterialName()));
	}
	*/


/*
	NavMeshEditor* editor = new NavMeshEditor(this);
	editor->Create();
	editor->SetNavMesh(m_NavMesh);
	SetEditor(editor);
*/

/*
	m_Actor2 = new Actor3D();
	m_Actor2->SetName(L"ninja2");
	m_Actor2->Create(L"ninja.mesh", L"ninja.skeleton", L"ninja_blue.material");
	m_Actor2->PutToStage(this);
	m_Actor2->GetOgreEntity()->setMaterialName("Examples/Ninja_blue");

	m_Actor2->SetPosition(Ogre::Vector3(600, 0, 65));
	m_Actor2->SetScale(Ogre::Vector3(0.5, 0.5, 0.5));

	m_Actor2->StartAnimation(L"Walk", false, 0, false);
*/
}

//////////////////////////////////////////////////////////////////////////
void FullScene::Update()
{
	Scene3DBase::Update();

	//if (m_Actor) m_Actor->Update();
	//if (m_Actor2) m_Actor2->Update();
}

//////////////////////////////////////////////////////////////////////////
void FullScene::Shutdown()
{
	Scene3DBase::Shutdown();

	m_Actor = NULL;
	//m_Actor2 = NULL;
}

//////////////////////////////////////////////////////////////////////////
bool FullScene::HandleMouseEvent(Viewport* viewport, MouseEvent& event)
{
	Stage::HandleMouseEvent(viewport, event);
	return true;

	if ((event.GetButton() == MouseEvent::BUTTON_LEFT || event.GetButton() == MouseEvent::BUTTON_RIGHT) && event.GetType() == MouseEvent::EVENT_PRESS)
	{
		MousePickResult pickResult;
		int mouseX, mouseY;
		viewport->GetMousePos(mouseX, mouseY, false);
		viewport->ViewportToScreen(mouseX, mouseY);
		if (!viewport->GetObjectAt(mouseX, mouseY, pickResult)) return false;

		Ogre::Vector3 point = pickResult.HitPoint;
		/*
		GetNavMesh()->MovePointToMesh(point);
		NavMesh::PathPointList path;
		GetNavMesh()->FindPath(m_Actor->GetPosition(), point, path);

		m_Actor->SetPath(path);
		m_Actor->FollowPath();
		*/

		if (event.GetButton() == MouseEvent::BUTTON_LEFT)
		{
			m_Actor->MoveTo(point);
			m_Actor->TurnTo(point, true);
		}
		else
		{
			//m_Actor->TurnTo(point);
			/*
			if (m_Actor->IsAnimationPlaying(1, L"wave")) m_Actor->StopAnimation(L"wave", 500);
			else m_Actor->StartAnimation(1, L"wave", 200);
			*/
			AnimProp animProp;
			animProp.Name = L"wave";

			m_Actor->PlayFullBodyAnim(animProp);

		}

		return true;
	}
	else return false;
}

//////////////////////////////////////////////////////////////////////////
bool FullScene::HandleKeyboardEvent(Viewport* viewport, KeyboardEvent& event)
{
	if (event.GetType() == KeyboardEvent::EVENT_PRESS)
	{
		switch (event.GetKeyCode())
		{
		case OIS::KC_F1:
			{
				Game::GetInstance()->SoundTest();
				if (GetEditor())
				{
					SetEditor(NULL);
					Game::GetInstance()->m_TestInfo = L"Game mode (press F1 to switch to editor mode)";
				}
				else
				{
					SceneNodeEditor* editor = new SceneNodeEditor(this);
					editor->Create();
					SetEditor(editor);
					Game::GetInstance()->m_TestInfo = L"Editor mode (right-click to change editing mode, F1 to switch back to game mode)";
				}
				return true;
			}

		case OIS::KC_F2:
			{
				Game::GetInstance()->SoundTest2();
				if (GetEditor()) SetEditor(NULL);

				NavMeshEditor* editor = new NavMeshEditor(this);
				editor->Create();
				editor->SetNavMesh(GetNavMesh());
				SetEditor(editor);
				
				return true;
			}

		case OIS::KC_F3:
			//SAFE_DELETE(m_Actor);
			if (m_Actor->IsInStage()) m_Actor->RemoveFromStage();
			else m_Actor->PutToStage(this);
			m_Actor->SaveToFile(L"c:/test.entity");

			return true;

		case OIS::KC_F4:
			m_Actor->AddScript(L"test.script");
			return true;

		}
	}
	return false;
}


} // namespace Wme
