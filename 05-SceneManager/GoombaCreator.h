#pragma once
#include "GameObject.h"
#include "EnemyCreator.h"
#include "Goomba.h"

class GoombaCreator: public EnemyCreator
{
public:
	CGameObject* Create(float x, float y, int model) override {
		return new CGoomba(x, y, model);
	}
};

