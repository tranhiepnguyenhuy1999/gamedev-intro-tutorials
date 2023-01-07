#pragma once
#include "debug.h"
#include "GameObject.h"
#include "Animation.h"
#include "Animations.h"

#define QUESTIONBRICK_MOVING_SPEED 0.05f

#define ID_ANI_UNTOUCHED_QUESTION_BRICK 10002
#define ID_ANI_TOUCHED_QUESTION_BRICK 10003

#define BRICK_WIDTH 16
#define BRICK_BBOX_WIDTH 16
#define BRICK_BBOX_HEIGHT 16

#define QUESTION_BRICK_TYPE_NORMAL 1
#define QUESTION_BRICK_TYPE_SPECIAL 2

#define QUESTIONBRICK_STATE_UNTOUCHED 100
#define QUESTIONBRICK_STATE_TOUCHED_1 200
#define QUESTIONBRICK_STATE_TOUCHED_2 300
#define QUESTIONBRICK_STATE_STATIC 400
class CQuestionBrick : public CGameObject {
	float type;
	float yLimit;
	ULONGLONG count_start;
public:
	CQuestionBrick(float x, float y, float type=1) : CGameObject(x, y) {
		SetState(QUESTIONBRICK_STATE_UNTOUCHED);
		this->type = type;
		yLimit = y - 2;
		count_start = -1;
	}
	void Render();
	void OnNoCollision(DWORD dt);
	void OnCollisionWith(LPCOLLISIONEVENT e);
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void GetBoundingBox(float& l, float& t, float& r, float& b);
	virtual void SetState(int state);
	float getType() { return this->type; };
	//int IsBlocking() { 
	//	if (state == QUESTIONBRICK_STATE_UNTOUCHED)
	//		return 0;
	//	else return 1;
	//}
};