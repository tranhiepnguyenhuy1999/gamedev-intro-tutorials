#include <iostream>
#include <fstream>
#include "AssetIDs.h"

#include "PlayScene.h"
#include "Utils.h"
#include "Textures.h"
#include "Sprites.h"
#include "Portal.h"
#include "Coin.h"
#include "Platform.h"
#include "Pile.h"
#include "Flower.h"
#include "ShootingFlower.h"
#include "Fire.h"
#include "SmallCoin.h"
#include "KoopaTroopa.h"
#include "ParaGoomba.h"
#include "ParaKoopaTroopa.h"
#include "FallObject.h"
#include "Leaf.h"
#include "Tail.h"
#include "BreakBrick.h"
#include "Rock.h"
#include "Button.h"
#include "HiddenBrick.h"
#include "ChangePositionBlock.h"
#include "TransportPile.h"
#include "Card.h"
#include "MovingObject.h"
#include "StopMovingObject.h"
#include "ActiveQuestionBrick.h"
#include "NormalObjectWithAni.h"

#include "Camera.h"
#include "TileMap.h"
#include "Layer.h"

#include "SampleKeyEventHandler.h"

using namespace std;

CPlayScene::CPlayScene(int id, LPCWSTR filePath):
	CScene(id, filePath)
{
	player = NULL;
	key_handler = new CSampleKeyHandler(this);
}


#define SCENE_SECTION_UNKNOWN -1
#define SCENE_SECTION_ASSETS	1
#define SCENE_SECTION_OBJECTS	2
#define SCENE_SECTION_TILESET	3
#define SCENE_SECTION_TILEMAP	4
#define SCENE_SECTION_CAMERA	5


#define ASSETS_SECTION_UNKNOWN -1
#define ASSETS_SECTION_SPRITES 1
#define ASSETS_SECTION_ANIMATIONS 2

#define MAX_SCENE_LINE 1024

void CPlayScene::_ParseSection_SPRITES(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 6) return; // skip invalid lines

	int ID = atoi(tokens[0].c_str());
	int l = atoi(tokens[1].c_str());
	int t = atoi(tokens[2].c_str());
	int r = atoi(tokens[3].c_str());
	int b = atoi(tokens[4].c_str());
	int texID = atoi(tokens[5].c_str());

	LPTEXTURE tex = CTextures::GetInstance()->Get(texID);
	if (tex == NULL)
	{
		DebugOut(L"[ERROR] Texture ID %d not found!\n", texID);
		return; 
	}

	CSprites::GetInstance()->Add(ID, l, t, r, b, tex);
}
void CPlayScene::_ParseSection_ASSETS(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 1) return;

	wstring path = ToWSTR(tokens[0]);
	
	LoadAssets(path.c_str());
}
void CPlayScene::_ParseSection_ANIMATIONS(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 3) return; // skip invalid lines - an animation must at least has 1 frame and 1 frame time

	//DebugOut(L"--> %s\n",ToWSTR(line).c_str());

	LPANIMATION ani = new CAnimation();

	int ani_id = atoi(tokens[0].c_str());
	for (int i = 1; i < tokens.size(); i += 2)	// why i+=2 ?  sprite_id | frame_time  
	{
		int sprite_id = atoi(tokens[i].c_str());
		int frame_time = atoi(tokens[i+1].c_str());
		ani->Add(sprite_id, frame_time);
	}

	CAnimations::GetInstance()->Add(ani_id, ani);
}
void CPlayScene::_ParseSection_TILESET(string line)
{
	vector<string> tokens = split(line);
	// skip flag line
	if (tokens.size() < 7) return;
	else DebugOut(L"Start loading tileset firsttime!\n");

	int width = atoi(tokens[0].c_str());
	int height = atoi(tokens[1].c_str());
	int maxTiles = atoi(tokens[2].c_str());
	int	sizeTilesX = atoi(tokens[3].c_str());
	int sizeTilesY = atoi(tokens[4].c_str());
	int frameTime = atoi(tokens[5].c_str());
	int texID = atoi(tokens[6].c_str());

	LPTEXTURE tex = CTextures::GetInstance()->Get(texID);

	if (tex == NULL)
	{
		DebugOut(L"[ERROR] Texture ID %d not found!\n", texID);
		return;
	}

	for (int i = 0; i < sizeTilesX; i++)
	{
		for (int j = 0; j < sizeTilesY; j++)
		{
			if (i * sizeTilesY + (j+1) > maxTiles) continue;
			// load sprite
			int ID = 1000 + i * sizeTilesY + j;
			int l = j * width;
			int r = l + width - 1;
			int t = i * height;
			int b = t + height - 1;
			CSprites::GetInstance()->Add(ID, l, t, r, b, tex);

			// load animation
			LPANIMATION ani = new CAnimation();
			int ani_id = i * sizeTilesY + j;
			int sprite_id = ID;
			int frame_time = frameTime;
			ani->Add(sprite_id, frame_time);

			CAnimations::GetInstance()->Add(ani_id, ani);
		}
	}

}
void CPlayScene::_ParseSection_CAMERA(string line)
{
	vector<string> tokens = split(line);
	// skip flag line
	if (tokens.size() < 3) return;
	else DebugOut(L"Start setting camera!\n");

	int cleft = atoi(tokens[0].c_str());
	int ctop = atoi(tokens[1].c_str());
	bool isFixed = atoi(tokens[2].c_str());
	
	Camera::GetInstance()->setInitialCamProps((float)cleft, (float)ctop, isFixed);
}
void CPlayScene::_ParseSection_TILEDMAP(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 1) return;
	CGameObject* obj = NULL;

	wstring fname = ToWSTR(tokens[0]);

	DebugOut(L"Tile map file: %s \n", fname.c_str());

	vector<vector<string>> content;
	vector<string> row;
	string fline, word;
	fstream file(fname, ios::in);

	if (file.is_open())
	{
		while (getline(file, fline))
		{
			row.clear();

			stringstream str(fline);

			while (getline(str, word, ','))
				row.push_back(word);
			content.push_back(row);
		}
	}
	else
		DebugOut(L"[INFO] Could not open the tilemap file\n");

	CTileMap::GetInstance()->createTileMap(content);
}
/*
	Parse a line in section [OBJECTS] 
*/
void CPlayScene::_ParseSection_OBJECTS(string line)
{
	vector<string> tokens = split(line);

	// skip invalid lines - an object set must have at least id, x, y
	if (tokens.size() < 2) return;

	int object_type = atoi(tokens[0].c_str());
	float x = (float)atof(tokens[1].c_str());
	float y = (float)atof(tokens[2].c_str());

	CGameObject *obj = NULL;

	switch (object_type)
	{
	case OBJECT_TYPE_MARIO:
	{
		if (player!=NULL) 
		{
			DebugOut(L"[ERROR] MARIO object was created before!\n");
			return;
		}
		float type = (float)atof(tokens[3].c_str());
		obj = new CMario(x, y, type);
		player = (CMario*)obj;
		Camera::GetInstance()->setCamPosition(x, y);
		DebugOut(L"[INFO] Player object has been created!\n");
		break;
	}
	case OBJECT_TYPE_GOOMBA: obj = new CGoomba(x,y); break;
	case OBJECT_TYPE_PARAGOOMBA: obj = new CParaGoomba(x, y); break;
	case OBJECT_TYPE_KOOPATROOPA:
	{
		int type = atoi(tokens[3].c_str());

		obj = new CKoopaTroopa(x, y, type); break;
	}
	case OBJECT_TYPE_NORMALOBJECT_WITH_ANIMATION:
	{
		int ani = atoi(tokens[3].c_str());

		obj = new CNormalObjectWithAni(x, y, ani);
		
		break;
	}
	case OBJECT_TYPE_PARAKOOPATROOPA:
	{
		int type = atoi(tokens[3].c_str());

		obj = new CParaKoopaTroopa(x, y, type); break;
	}
	case OBJECT_TYPE_MAP: obj = new CMap(x, y); break;
	case OBJECT_TYPE_BRICK: obj = new CBrick(x,y); break;
	case OBJECT_TYPE_BREAKBRICK: 
	{
		float type = (float)atof(tokens[3].c_str());
		obj = new CBreakBrick(x, y, type);
		break;
	}
	case OBJECT_TYPE_QUESTIONBRICK:
	{
		float type = (float)atof(tokens[3].c_str());
	
		obj = new CQuestionBrick(x, y, type); break;
	}
	case OBJECT_TYPE_CHANGE_POSITON_BLOCK:
	{
		float npx = (float)atof(tokens[3].c_str());
		float npy = (float)atof(tokens[4].c_str());
		float is_down = (float)atof(tokens[5].c_str());
		float ncl = (float)atof(tokens[6].c_str());
		float nct = (float)atof(tokens[7].c_str());
		float is_fixed = (float)atof(tokens[8].c_str());
		float dpx = (float)atof(tokens[9].c_str());
		float dpy = (float)atof(tokens[10].c_str());

		obj = new CChangePositionBlock(x, y,npx, npy, is_down, ncl, nct, is_fixed, dpx, dpy);
		break;
	}
	case OBJECT_TYPE_DOWNBRICK:
	{
		float length = (float)atof(tokens[3].c_str());
	obj = new CDownBrick(x, y, (int)length); break;
	}
	case OBJECT_TYPE_PILE:
	{
		float height = (float)atof(tokens[3].c_str());
		obj = new CPile(x, y, height); break;
	}
	case OBJECT_TYPE_TRANSPORT_PILE:
	{
		int height = (float)atof(tokens[3].c_str());
		int type = (float)atoi(tokens[4].c_str());

		obj = new CTransportPile(x, y, height, type); break;
	}
	case OBJECT_TYPE_CARD:
	{
		obj = new CCard(x, y); break;
	}
	case OBJECT_TYPE_SMALLCOIN: obj = new CSmallCoin(x, y); break;
	case OBJECT_TYPE_FLOWER: obj = new CFlower(x, y); break;
	case OBJECT_TYPE_SHOOTING_FLOWER:
	{
		float type = (float)atof(tokens[3].c_str());
		obj = new CShootingFlower(x, y, type); break;
	}
	case OBJECT_TYPE_COIN: obj = new CCoin(x, y); break;
	case OBJECT_TYPE_PLATFORM:
	{

		float cell_width = (float)atof(tokens[3].c_str());
		float cell_height = (float)atof(tokens[4].c_str());
		int length = atoi(tokens[5].c_str());
		int sprite_begin = atoi(tokens[6].c_str());
		int sprite_middle = atoi(tokens[7].c_str());
		int sprite_end = atoi(tokens[8].c_str());

		obj = new CPlatform(
			x, y,
			cell_width, cell_height, length,
			sprite_begin, sprite_middle, sprite_end
		);

		break;
	}

	case OBJECT_TYPE_PORTAL:
	{
		float r = (float)atof(tokens[3].c_str());
		float b = (float)atof(tokens[4].c_str());
		int scene_id = atoi(tokens[5].c_str());
		obj = new CPortal(x, y, r, b, scene_id);
		break;
	}
	default:
		DebugOut(L"[ERROR] Invalid object type: %d\n", object_type);
		return;
	}

	// General object setup
	obj->SetPosition(x, y);


	objects.push_back(obj);
}
void CPlayScene::AddMovingObject(float x, float y, float nx, float ny, float ax, float ay, int ani_id, int delete_times) {
	CGameObject* obj = NULL;

	obj = new CMovingObject(x, y, nx, ny, ax, ay, ani_id, delete_times);
	
	// General object setup
	obj->SetPosition(x, y);

	objects.push_back(obj);
};
void CPlayScene::AddStopMovingObjectAxisY(float x, float y, float nx, float ny, float ax, float ay, int ani_id, int delete_times, bool direct) {
	
	CGameObject* obj = NULL;

	obj = new CStopMovingObjectAxisY(x, y, nx, ny, ax, ay, ani_id, delete_times, direct);

	// General object setup
	obj->SetPosition(x, y);

	objects.push_back(obj);
};
void CPlayScene::CreateNewObject(int id, float x, float y, float  nx, float ny, LPGAMEOBJECT obj_src, int type) // nx, ny : can be direction and int v at the same time
{
	CGameObject* obj = NULL;

	switch (id)
	{
	case OBJECT_TYPE_BRICK: obj = new CBrick(x, y); break;
	case OBJECT_TYPE_ACTIVE_QUESTIONBRICK: obj = new CActiveQuestionBrick(x, y); break;
	case OBJECT_TYPE_MUSHROOM: obj = new CMushroom(x, y, type, nx); break;
	case OBJECT_TYPE_LEAF: obj = new CLeaf(x, y); break;
	case OBJECT_TYPE_TAIL: obj = new CTail(x, y, nx); break;
	case OBJECT_TYPE_ROCK: obj = new CRock(x, y, nx, ny); break;
	case OBJECT_TYPE_BUTTON: obj = new CButton(x, y); break;
	case OBJECT_TYPE_STATIC_COIN: obj = new CCoin(x, y, COIN_TYPE_STATIC); break;
	case OBJECT_TYPE_FALLOBJECT:	
	{
		float src_vx, src_vy;
		obj_src->GetSpeed(src_vx, src_vy);
		obj = new CFallObject(x, y, src_vx);
		if (dynamic_cast<CKoopaTroopa*>(obj_src))
		{
			CKoopaTroopa* src = dynamic_cast<CKoopaTroopa*>(obj_src);
			src->addFallObject(obj);
		}
		break;
	}
	case OBJECT_TYPE_SMALLCOIN: obj = new CSmallCoin(x, y); break;
	case OBJECT_TYPE_FIRE:
	{
		obj = new CFire(x, y, nx, ny); break;
	}
	default:
		DebugOut(L"[ERROR] Invalid object type: %d\n", id);
		return;
	}

	// General object setup
	obj->SetPosition(x, y);


	objects.push_back(obj);
}
void CPlayScene::playerTouchedButtonP()
{
	for (int i = 0; i < objects.size(); i++)
	{
		if (dynamic_cast<CBreakBrick*>(objects[i]))
		{
			objects[i]->SetState(BREAK_BRICK_STATE_TRANSFORM_TO_COIN);
		}
	}
}
void CPlayScene::LoadAssets(LPCWSTR assetFile)
{
	DebugOut(L"[INFO] Start loading assets from : %s \n", assetFile);

	ifstream f;
	f.open(assetFile);

	int section = ASSETS_SECTION_UNKNOWN;

	char str[MAX_SCENE_LINE];
	while (f.getline(str, MAX_SCENE_LINE))
	{
		string line(str);

		if (line[0] == '#') continue;	// skip comment lines	

		if (line == "[SPRITES]") { section = ASSETS_SECTION_SPRITES; continue; };
		if (line == "[ANIMATIONS]") { section = ASSETS_SECTION_ANIMATIONS; continue; };
		if (line[0] == '[') { section = SCENE_SECTION_UNKNOWN; continue; }

		//
		// data section
		//
		switch (section)
		{
		case ASSETS_SECTION_SPRITES: _ParseSection_SPRITES(line); break;
		case ASSETS_SECTION_ANIMATIONS: _ParseSection_ANIMATIONS(line); break;
		}
	}

	f.close();

	DebugOut(L"[INFO] Done loading assets from %s\n", assetFile);
}
void CPlayScene::Load()
{
	DebugOut(L"[INFO] Start loading scene from : %s \n", sceneFilePath);

	ifstream f;
	f.open(sceneFilePath);

	// current resource section flag
	int section = SCENE_SECTION_UNKNOWN;

	char str[MAX_SCENE_LINE];
	while (f.getline(str, MAX_SCENE_LINE))
	{
		string line(str);

		if (line[0] == '#') continue;	// skip comment lines	
		if (line == "[ASSETS]") { section = SCENE_SECTION_ASSETS; continue; };
		if (line == "[TILESET]") { section = SCENE_SECTION_TILESET; continue; };
		if (line == "[TILEDMAP]") { section = SCENE_SECTION_TILEMAP; continue; };
		if (line == "[CAMERA]") { section = SCENE_SECTION_CAMERA; continue; };
		if (line == "[OBJECTS]") { section = SCENE_SECTION_OBJECTS; continue; };
		if (line[0] == '[') { section = SCENE_SECTION_UNKNOWN; continue; }
		//
		// data section
		//
		switch (section)
		{
		case SCENE_SECTION_ASSETS: _ParseSection_ASSETS(line); break;
		case SCENE_SECTION_TILESET: _ParseSection_TILESET(line); break;
		case SCENE_SECTION_TILEMAP: _ParseSection_TILEDMAP(line); break;
		case SCENE_SECTION_CAMERA: _ParseSection_CAMERA(line); break;
		case SCENE_SECTION_OBJECTS: _ParseSection_OBJECTS(line); break;
		}
	}

	f.close();

	DebugOut(L"[INFO] Done loading scene  %s\n", sceneFilePath);
}
void CPlayScene::Update(DWORD dt)
{
	// We know that Mario is the first object in the list hence we won't add him into the colliable object list
	// TO-DO: This is a "dirty" way, need a more organized way 

	vector<LPGAMEOBJECT> coObjects;
	for (size_t i = 1; i < objects.size(); i++)
	{
		coObjects.push_back(objects[i]);
	}

	for (size_t i = 0; i < objects.size(); i++)
	{
		if(objects[i]->IsTrigger()) objects[i]->Update(dt, &coObjects);
	}
	
	CLayer::GetInstance()->Update(dt, &coObjects);

	// skip the rest if scene was already unloaded (Mario::Update might trigger PlayScene::Unload)
	if (player == NULL) return;

	// Update camera to follow mario
	float px, py;
	player->GetPosition(px, py);

	Camera::GetInstance()->setCamPosition(px,py);
	Camera::GetInstance()->triggerObjectByCamera(objects);

	CUserBoard::GetInstance()->SetPosition(200, 432);

	PurgeDeletedObjects();
}
void CPlayScene::Render()
{
	CTileMap::GetInstance()->Render();

	// obj render
	for (int i = 0; i < objects.size(); i++)
	{
		if(Camera::GetInstance()->isCamContainObject(objects[i])) objects[i]->Render();
	}

	float cx, cy;
	if (CLayer::GetInstance()->IsActive())
	{
		Camera::GetInstance()->getCamPosition(cx, cy);
		CLayer::GetInstance()->SetPosition(cx, cy);
		CLayer::GetInstance()->Render();
	}

	CUserBoard::GetInstance()->Render();
}
/*
*	Clear all objects from this scene
*/
void CPlayScene::Clear()
{
	vector<LPGAMEOBJECT>::iterator it;
	for (it = objects.begin(); it != objects.end(); it++)
	{
		delete (*it);
	}
	objects.clear();
}
/*
	Unload scene

	TODO: Beside objects, we need to clean up sprites, animations and textures as well 

*/
void CPlayScene::Unload()
{
	for (int i = 0; i < objects.size(); i++)
		delete objects[i];

	objects.clear();
	player = NULL;

	DebugOut(L"[INFO] Scene %d unloaded! \n", id);
}
bool CPlayScene::IsGameObjectDeleted(const LPGAMEOBJECT& o) { return o == NULL; }
void CPlayScene::PurgeDeletedObjects()
{
	vector<LPGAMEOBJECT>::iterator it;
	for (it = objects.begin(); it != objects.end(); it++)
	{
		LPGAMEOBJECT o = *it;
		if (o->IsDeleted())
		{
			delete o;
			*it = NULL;
		}
	}

	// NOTE: remove_if will swap all deleted items to the end of the vector
	// then simply trim the vector, this is much more efficient than deleting individual items
	objects.erase(
		std::remove_if(objects.begin(), objects.end(), CPlayScene::IsGameObjectDeleted),
		objects.end());
}