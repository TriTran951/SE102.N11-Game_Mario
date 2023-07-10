#pragma once
#include "GameObject.h"
#include "EnemyCreator.h"
#include "PlantEnemy.h"

class PlantCreator : public EnemyCreator
{
public:
	CGameObject* Create(float x, float y, int model) override {
		return new CPlantEnemy(x, y, model);
	}
};

