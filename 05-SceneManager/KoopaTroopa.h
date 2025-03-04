#pragma once
#include "GameObject.h"
#include "FallObject.h"
#include "AssetIDs.h"

#define KOOPATROOPA_GRAVITY 0.00035f
#define KOOPATROOPA_SPEED 0.035f
#define KOOPATROOPA_SHELL_SPEED 0.2f
#define KOOPATROOPA_SHELL_RELEASE_PICK_UP_SPEED_AXIS_X 0.2f
#define KOOPATROOPA_SHELL_RELEASE_PICK_UP_SPEED_AXIS_Y 0.1f

#define KOOPATROOPA_MAX_Y 50

#define PARA_KOOPATROOPA_JUMP_DEFLECT_SPEED 0.15f

#define KOOPATROOPA_BBOX_WIDTH 15
#define KOOPATROOPA_BBOX_HEIGHT 26
#define KOOPATROOPA_BBOX_HEIGHT_SHELL 16

#define KOOPATROOPA_DIE_TIMEOUT 5000
#define KOOPATROOPA_ALIVE_TIMEOUT 2000
#define KOOPATROOPA_JUMP_TIMEOUT 500

#define KOOPATROOPA_STATE_MOVING 100
#define KOOPATROOPA_STATE_ALIVE 200
#define KOOPATROOPA_STATE_SHELL 300
#define KOOPATROOPA_STATE_SHELL_ALIVE 400
#define KOOPATROOPA_STATE_SHELL_MOVING 500
#define KOOPATROOPA_STATE_TURN 600
#define KOOPATROOPA_STATE_SHELL_PICK_UP 700
#define PARA_KOOPATROOPA_STATE_JUMP 800

#define ID_ANI_KOOPATROOPA_WALKING_LEFT 7000
#define ID_ANI_KOOPATROOPA_WALKING_RIGHT 7004
#define ID_ANI_KOOPATROOPA_SHELL 7001
#define ID_ANI_KOOPATROOPA_KICKING 7002
#define ID_ANI_KOOPATROOPA_ALIVE 7003

#define KOOPATROOPA_TYPE_GREEN	0
#define KOOPATROOPA_TYPE_RED	1


class CKoopaTroopa : public CGameObject
{
protected:
	float ay;
	int type;

	boolean isDie;
	boolean isHaveFallObj;

	ULONGLONG count_start;

	CFallObject* fall_object;

	void createFallObject() {

		if (vx < 0) {
			CGame::GetInstance()->GetCurrentScene()->CreateNewObject(OBJECT_TYPE_FALLOBJECT, x - KOOPATROOPA_BBOX_WIDTH / 2 - FALLOBJECT_BBOX_WIDTH / 2, y + (KOOPATROOPA_BBOX_HEIGHT - FALLOBJECT_BBOX_HEIGHT) / 2, 0, 0, this);
		}
		else
		{
			CGame::GetInstance()->GetCurrentScene()->CreateNewObject(OBJECT_TYPE_FALLOBJECT, x + KOOPATROOPA_BBOX_WIDTH / 2 + FALLOBJECT_BBOX_WIDTH / 2, y + (KOOPATROOPA_BBOX_HEIGHT - FALLOBJECT_BBOX_HEIGHT) / 2, 0, 0, this);

		}
	}
	void updateState();
public:
	CKoopaTroopa(float x, float y, int obj_type);

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	virtual int IsCollidable() { return 1; };
	virtual int IsBlocking() { return 0; }
	virtual void OnNoCollision(DWORD dt);
	virtual void OnCollisionWith(LPCOLLISIONEVENT e);
	void OnCollisionWithQuestionBrick(LPCOLLISIONEVENT e);
	void OnCollisionWithGoomba(LPCOLLISIONEVENT e);
	void OnCollisionWithDownBrick(LPCOLLISIONEVENT e);

	virtual void SetState(int state);

	void addFallObject(CGameObject* obj) {
		if (!dynamic_cast<CFallObject*>(obj)) return;
		else if (!fall_object)
		{
			CFallObject* f_obj = dynamic_cast<CFallObject*>(obj);
			fall_object = f_obj;
		}
	}
	void removeFalObj() {
		if (fall_object) fall_object->Delete();
		fall_object = NULL;
	};
	void getKicked( int p_nx) {
		vx = p_nx * KOOPATROOPA_SHELL_SPEED;
		nx = p_nx;
		SetState(KOOPATROOPA_STATE_SHELL_MOVING);
	}
	void getJumpedDown(){
		SetState(KOOPATROOPA_STATE_SHELL);
	}
	void getPickUp(float p_speed)
	{
		// when shell is picking up, it gets player speed
		this->vx = p_speed;
	}
	bool IsDie() {
		return isDie;
	}
};