#pragma once
#include "GameObject.h"
#include "EnemyCreator.h"
#include "Koopa.h"

class KoopaCreator : public EnemyCreator
{
public:
	CGameObject* Create(float x, float y, int model) override {
		return new CKoopa(x, y, model);
	}
};

