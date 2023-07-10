#pragma once
#include "GameObject.h"
class EnemyCreator
{
public:
	virtual CGameObject* Create(float x, float y, int model) = 0;
};

