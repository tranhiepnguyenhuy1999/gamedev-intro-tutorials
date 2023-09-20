#include "KoopaTroopa.h"
#include "DownBrick.h"
#include "QuestionBrick.h"
#include "Goomba.h"
#include "Mario.h"

CKoopaTroopa::CKoopaTroopa(float x, float y) :CGameObject(x, y)
{
	this->ay = KOOPATROOPA_GRAVITY;
	ready_jump_start = -1;
	count_start = -1;
	vx = -KOOPATROOPA_SPEED;
	nx = -1;
	isDie = false;
	fall_object = NULL;
	SetState(KOOPATROOPA_STATE_MOVING);
}

void CKoopaTroopa::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (isDie)
	{
		left = x - KOOPATROOPA_BBOX_WIDTH / 2;
		top = y - (KOOPATROOPA_BBOX_HEIGHT_SHELL / 2);	
		right = left + KOOPATROOPA_BBOX_WIDTH;
		bottom = top + KOOPATROOPA_BBOX_HEIGHT_SHELL;
	}
	else
	{
		left = x - KOOPATROOPA_BBOX_WIDTH / 2;
		top = y - (KOOPATROOPA_BBOX_HEIGHT/2);
		right = left + KOOPATROOPA_BBOX_WIDTH;
		bottom = top + KOOPATROOPA_BBOX_HEIGHT ;
	}
}

void CKoopaTroopa::OnNoCollision(DWORD dt)
{
	x += vx * dt;
	y += vy * dt;
};

void CKoopaTroopa::OnCollisionWith(LPCOLLISIONEVENT e)
{
	if (e->ny != 0 && e->obj->IsBlocking())
	{
		vy = 0;
	}
	else if (e->nx != 0 && e->obj->IsBlocking() && !dynamic_cast<CDownBrick*>(e->obj))
	{
		vx = -vx;
	}
	if (dynamic_cast<CQuestionBrick*>(e->obj))OnCollisionWithQuestionBrick(e);
	else if (dynamic_cast<CGoomba*>(e->obj))OnCollisionWithGoomba(e);
}
void CKoopaTroopa::OnCollisionWithGoomba(LPCOLLISIONEVENT e)
{
	CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);

	if (goomba->GetState() != GOOMBA_STATE_DIE)
	{
		goomba->SetState(GOOMBA_STATE_DIE);
	}
}
void CKoopaTroopa::OnCollisionWithQuestionBrick(LPCOLLISIONEVENT e)
{
		CQuestionBrick* obj = dynamic_cast<CQuestionBrick*>(e->obj);
		int pLevel;
		CGame::GetInstance()->GetCurrentScene()->getPlayerLevel(pLevel);
		if (obj->GetState() == QUESTIONBRICK_STATE_UNACTIVE)
		{
			float qx, qy, qvx;
			obj->GetPosition(qx, qy);

			if (obj->getType() == 1)
			{
				CGame::GetInstance()->GetCurrentScene()->createNewObject(OBJECT_TYPE_SMALLCOIN, qx, qy - 16);
			}
			else
			{
				// check direction
				if (this->x <= qx + 8) {
					qvx = -1;
				}
				else
				{
					qvx = 1;
				}
				if (pLevel == MARIO_LEVEL_BIG)
					CGame::GetInstance()->GetCurrentScene()->createNewObject(OBJECT_TYPE_LEAF, qx, qy);
				else
					CGame::GetInstance()->GetCurrentScene()->createNewObject(OBJECT_TYPE_MUSHROOM, qx, qy, qvx);
			}

			obj->SetState(QUESTIONBRICK_STATE_TOUCHED);
		}

}
void CKoopaTroopa::OnCollisionWithDownBrick(LPCOLLISIONEVENT e)
{
	if (e->ny < 0)
		vy = 0;
}
void CKoopaTroopa::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	float cx, cy;
	CGame::GetInstance()->GetCamPos(cx, cy);

	vy += ay * dt;
	
	if (!isDie)
	{
		if (!fall_object)
		{
			createFallObject();
		}
		else {
			if (fall_object->isFalling())
			{
				SetState(KOOPATROOPA_STATE_TURN);
				SetState(KOOPATROOPA_STATE_MOVING);
				return;
			}
		}
	}

	if (state == KOOPATROOPA_STATE_SHELL && (GetTickCount64() - count_start > KOOPATROOPA_DIE_TIMEOUT))
	{
		SetState(KOOPATROOPA_STATE_ALIVE);
		return;
	}
	else if (state == KOOPATROOPA_STATE_ALIVE && (GetTickCount64() - count_start > KOOPATROOPA_ALIVE_TIMEOUT))
	{
		count_start = -1;
		y -= (KOOPATROOPA_BBOX_HEIGHT - KOOPATROOPA_BBOX_HEIGHT_SHELL) / 2;
		vx = nx*KOOPATROOPA_SPEED;
		SetState(KOOPATROOPA_STATE_MOVING);
		return;
	}
	CGameObject::Update(dt, coObjects);
	CCollision::GetInstance()->Process(this, dt, coObjects);
}


void CKoopaTroopa::Render()
{
	int aniId = ID_ANI_KOOPATROOPA_WALKING_LEFT;
	if (isDie)
	{
		if (state == KOOPATROOPA_STATE_SHELL) aniId = ID_ANI_KOOPATROOPA_SHELL;
		else aniId = ID_ANI_KOOPATROOPA_KICKING;
	}
	else if (vx > 0) {
		aniId = ID_ANI_KOOPATROOPA_WALKING_RIGHT;
	}
	else if (state == KOOPATROOPA_STATE_ALIVE)
	{
		aniId = ID_ANI_KOOPATROOPA_ALIVE;
	}
	CAnimations::GetInstance()->Get(aniId)->Render(x, y);
	//RenderBoundingBox();
}

void CKoopaTroopa::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case KOOPATROOPA_STATE_MOVING:
		isDie = false;
		break;
	case KOOPATROOPA_STATE_SHELL:
		removeFalObj();
		if (!isDie)
		{
			y += (KOOPATROOPA_BBOX_HEIGHT - KOOPATROOPA_BBOX_HEIGHT_SHELL) / 2;
			isDie = true;
		}
		count_start = GetTickCount64();
		vx = 0;
		break;
	case KOOPATROOPA_STATE_SHELL_MOVING:
		break;
	case KOOPATROOPA_STATE_ALIVE:
		count_start = GetTickCount64();
		break;
	case KOOPATROOPA_STATE_TURN:
		removeFalObj();
		count_start = GetTickCount64();
		vx = -vx;
		nx = -nx;
		break;
	}
}
