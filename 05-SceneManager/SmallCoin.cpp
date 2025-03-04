#include "SmallCoin.h"

void CSmallCoin::Render()
{
	CAnimations* animations = CAnimations::GetInstance();
	animations->Get(ID_ANI_SMALL_COIN)->Render(x, y);

	//RenderBoundingBox();
}

void CSmallCoin::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - SMALL_COIN_BBOX_WIDTH / 2;
	t = y - SMALL_COIN_BBOX_HEIGHT / 2;
	r = l + SMALL_COIN_BBOX_WIDTH;
	b = t + SMALL_COIN_BBOX_HEIGHT;
}