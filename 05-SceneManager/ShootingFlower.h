#pragma once
#include "GameObject.h"

#define SHOOTING_FLOWER_Y_AXIS 0.0001f

#define SHOOTING_FLOWER_BBOX_WIDTH 16
#define SHOOTING_FLOWER_BBOX_HEIGHT 32

#define SHOOTING_FLOWER_SHOOTING_RANGE 100.0f
#define SHOOTING_FLOWER_UNWORKING_RANGE 50.0f


#define SHOOTING_FLOWER_PRESHOOT_TIMEOUT 1500
#define SHOOTING_FLOWER_AFSHOOT_TIMEOUT 750
#define SHOOTING_FLOWER_LOOP_TIMEOUT 3000

#define SHOOTING_FLOWER_STATE_MOVE 100
#define SHOOTING_FLOWER_STATE_STATIC 200

#define SHOOTING_FLOWER_TYPE_GREEN 1
#define SHOOTING_FLOWER_TYPE_RED 2


// id ani type 1
#define ID_ANI_SHOOTING_FLOWER_UP_LEFT_IDLE 19001
#define ID_ANI_SHOOTING_FLOWER_DOWN_LEFT_IDLE 19002

#define ID_ANI_SHOOTING_FLOWER_UP_LEFT_MOVING 19003
#define ID_ANI_SHOOTING_FLOWER_DOWN_LEFT_MOVING 19004

#define ID_ANI_SHOOTING_FLOWER_UP_RIGHT_IDLE 19005
#define ID_ANI_SHOOTING_FLOWER_DOWN_RIGHT_IDLE 19006

#define ID_ANI_SHOOTING_FLOWER_UP_RIGHT_MOVING 19007
#define ID_ANI_SHOOTING_FLOWER_DOWN_RIGHT_MOVING 19008

// id ani type 2
#define ID_ANI_SHOOTING_RED_FLOWER_UP_LEFT_IDLE 12001
#define ID_ANI_SHOOTING_RED_FLOWER_DOWN_LEFT_IDLE 12002

#define ID_ANI_SHOOTING_RED_FLOWER_UP_LEFT_MOVING 12003
#define ID_ANI_SHOOTING_RED_FLOWER_DOWN_LEFT_MOVING 12004

#define ID_ANI_SHOOTING_RED_FLOWER_UP_RIGHT_IDLE 12005
#define ID_ANI_SHOOTING_RED_FLOWER_DOWN_RIGHT_IDLE 12006

#define ID_ANI_SHOOTING_RED_FLOWER_UP_RIGHT_MOVING 12007
#define ID_ANI_SHOOTING_RED_FLOWER_DOWN_RIGHT_MOVING 12008

class CShootingFlower : public CGameObject
{

protected:
	float ay;
	float top;
	float bot;
	float type;

	boolean is_after_shooting;
	boolean isWorking;

	ULONGLONG loop_start;
	
	int getMovingFlowerAniId(int flag);
	int getStaticFlowerAniId(int flag);
	int getMovingFlowerType2AniId(int flag);
	int getStaticFlowerType2AniId(int flag);

	int getFlowerPosition();
	int getPlayerPosition();
	float translateToPercent(float data, boolean isXAxis);

	void shooting();

public:
	CShootingFlower(float x, float y, float obj_type);

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();

	virtual int IsBlocking() { return 0; }
	virtual void OnNoCollision(DWORD dt);

	virtual void OnCollisionWith(LPCOLLISIONEVENT e);
	virtual void SetState(int state);

	bool IsFlowerAtBottom() {
		return y == bot;
	}
};