#include <algorithm>
#include "debug.h"

#include "Leaf.h"
#include "Effect.h"
#include "Mario.h"
#include "Bullet.h"
#include "PlantEnemy.h"
#include "Game.h"
#include "Koopa.h"
#include "MushRoom.h"
#include "Goomba.h"
#include "Coin.h"
#include "Platform.h"
#include "FlowerFire.h"
#include "FireFromPlant.h"
#include "BrickQuestion.h"
#include "Portal.h"
#include "PlayScene.h"
#include "Collision.h"

CMario::CMario(float x, float y) : CGameObject(x, y) {
	isShoot = false;
	isHolding = false;
	isSitting = false;
	maxVx = 0.0f;
	Up = 4;
	ax = 0.0f;
	clock = 300;
	ay = MARIO_GRAVITY;

	level = MARIO_LEVEL_SMALL;
	levelRun = 0;
	isFlying = false;
	isRunning = false;
	untouchable = 0;
	untouchable_start = -1;
	isOnPlatform = false;
	isChanging = false;
	isLower = false;
	coin = 0;
	score = 0;
	scoreUpCollision = 1;
}

void CMario::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	//DebugOutTitle(L"Up %d", Up);
	//DebugOutTitle(L"TIME %d", clock);
	//DebugOutTitle(L"POWERUP %d", levelRun);
	DebugOutTitle(L"[POSITION] %f %f", x, y);
	if (isChanging) {
		vx = 0;
		vy = 0;
	}
	else {
		vy += ay * dt;
		vx += ax * dt;
	}
	if (y > POSITION_Y_DIE) {
		SetState(MARIO_STATE_DIE);
	}
	if (coin > 99) {
		Up++;
		coin = 0;
	}
	CPlayScene* scene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();



	if (abs(vx) > abs(maxVx)) vx = maxVx;



	if (GetTickCount64() - start_score_up > TIME_SCORE_UP_MAX) {
		scoreUpCollision = 1;
		start_score_up = 0;
	}


	if (clock > 0) {
		if (GetTickCount64() - time_down_1_second > TIME_ONE_SECOND) {
			clock--;
			time_down_1_second = GetTickCount64();
		}
	}
	else {
		clock = 0;
		SetState(MARIO_STATE_DIE);
	}
	if (GetTickCount64() - untouchable_start > MARIO_UNTOUCHABLE_TIME)
	{
		untouchable_start = 0;
		untouchable = 0;
	}
	if (GetTickCount64() - start_changing > TIME_CHANGING) {
		isChanging = false;
		isLower = false;
		start_changing = 0;
	}

	//Xet cac truong hop ma power up xuong
	//- Khong Running
	//- Flying nhung vx  = 0
	//- Brace
	//- Luc bay xuong 
	// - Bi chan boi block (nhung block trong luc flying se khong bi)
	//=> Cac truong hop con lai se tang power up
	if ((!isRunning) || (!vx) || (IsBrace()) || ((!isOnPlatform) && (isFlying) && (vy>0)) || ((abs(vx) < SPEED_MARIO_WHEN_BLOCK)&&(!isFlying)))
	{
			if (GetTickCount64() - speed_stop > TIME_SPEED) {
				if (levelRun > 0) levelRun--;
				speed_stop = GetTickCount64();
			}
			start_prepare = GetTickCount64();
	}
	else {
		if (GetTickCount64() - start_prepare > TIME_PREPARE_RUN) {
			if (GetTickCount64() - speed_start > TIME_SPEED) {
				if (levelRun < LEVEL_RUN_MAX) {
					levelRun++;
				}
				speed_start = GetTickCount64();
			}
		}
	}
	if (isFlying) {
		if (isOnPlatform) {
			isFlying = false;
			ay = MARIO_GRAVITY;
		}
	}
	if (isTailAttack) {
		if (GetTickCount64() - start_tail_attack > TIME_TAIL_ATTACK) {
			isTailAttack = false;
			start_tail_attack = 0;
		}
	}
	if (isKicking) {
		if (GetTickCount64() - start_kick > TIME_KICK_ANIMATION) {
			isKicking = false;
			start_kick = 0;
		}
	}
	
	if (isShoot) {
		start_limit_shoot = GetTickCount64();
		if (GetTickCount64() - start_shoot > TIME_SHOOT_ANI) {
			isShoot = false;
			CBullet* bullet = new CBullet(x, y);
			scene->AddObject(bullet);
			start_shoot = 0;
		}
	}


	
	isOnPlatform = false;
	CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CMario::OnNoCollision(DWORD dt)
{
	x += vx * dt;
	y += vy * dt;
}

void CMario::OnCollisionWith(LPCOLLISIONEVENT e)
{
		if (e->ny != 0 && e->obj->IsBlocking())
		{
			vy = 0;
			if (e->ny < 0) {
				isOnPlatform = true; 
			}
		}
		else if ((e->nx != 0) && (e->obj->IsBlocking()))
		{
			vx = 0;
		}
		if (dynamic_cast<CGoomba*>(e->obj))
			OnCollisionWithGoomba(e);
		else if (dynamic_cast<CCoin*>(e->obj))
			OnCollisionWithCoin(e);
		else if (dynamic_cast<CPortal*>(e->obj))
			OnCollisionWithPortal(e);
		else if (dynamic_cast<CMushRoom*>(e->obj))
			OnCollisionWithMushRoom(e);
		else if (dynamic_cast<CLeaf*>(e->obj))
			OnCollisionWithLeaf(e);
		else if (dynamic_cast<CFlowerFire*>(e->obj))
			OnCollisionWithFlowerFire(e);
		else if (dynamic_cast<CBrickQuestion*>(e->obj))
			OnCollisionWithBrickQuestion(e);
		else if (dynamic_cast<CKoopa*>(e->obj))
			OnCollisionWithKoopa(e);
		else if (dynamic_cast<CPlatform*>(e->obj))
			OnCollisionWithPlatForm(e);
		else if (dynamic_cast<CPlantEnemy*>(e->obj))
			OnCollisionWithPlantEnemy(e);
		else if (dynamic_cast<CFireFromPlant*>(e->obj))
			OnCollisionWithFireFromPlant(e);
}


void CMario::OnCollisionWithPlatForm(LPCOLLISIONEVENT e) {
	CPlatform* platform = dynamic_cast<CPlatform*>(e->obj);
	if (platform->IsBlocking()) { }
	else {
		if (e->ny < 0) {
			BlockIfNoBlock(platform);
		}
	}
}
void CMario::OnCollisionWithPlantEnemy(LPCOLLISIONEVENT e) {
	if (untouchable) return;
	
	CPlantEnemy* plant = dynamic_cast<CPlantEnemy*>(e->obj);
	
	if (isTailAttack) { 
		AddScore(plant->GetX(), plant->GetY(), 100);
		score += 100;
		plant->SetIsDeleted(true); 
	}
	else SetLevelLower();
}

void CMario::OnCollisionWithFireFromPlant(LPCOLLISIONEVENT e) {
	if (untouchable) return;
	CFireFromPlant* bullet = dynamic_cast<CFireFromPlant*>(e->obj);
	bullet->SetIsDeleted(true);
	SetLevelLower();

}

void CMario::OnCollisionWithKoopa(LPCOLLISIONEVENT e) {
	CKoopa* koopa = dynamic_cast<CKoopa*>(e->obj); 
	if(isTailAttack){
		AddScore(koopa->GetX(), koopa->GetY(), 100);
		score += 100;
		koopa->SetState(KOOPA_STATE_UPSIDE);
	}
	else {
		if (e->ny < 0) {
			IncreaseScoreUpCollision(x, y);
			koopa->SetVy(-KOOPA_ADJUST_KICKED_NOT_FALL);
			if (koopa->GetModel() != KOOPA_GREEN_WING) {
				if ((koopa->GetState() == KOOPA_STATE_WALKING) or (koopa->GetState() == KOOPA_STATE_IS_KICKED))
				{
					koopa->SetState(KOOPA_STATE_DEFEND);
					vy = -MARIO_JUMP_DEFLECT_SPEED;
				}
				else {
					koopa->SetState(KOOPA_STATE_IS_KICKED);
				}
			}
			else
			{
				if (koopa->GetState() == KOOPA_STATE_JUMP) {
					koopa->SetState(KOOPA_STATE_WALKING);
					vy = -MARIO_JUMP_DEFLECT_SPEED;
				}
				else if ((koopa->GetState() == KOOPA_STATE_WALKING) or (koopa->GetState() == KOOPA_STATE_IS_KICKED))
				{
					koopa->SetVy(KOOPA_ADJUST_KICKED_NOT_FALL);
					koopa->SetState(KOOPA_STATE_DEFEND);
					vy = -MARIO_JUMP_DEFLECT_SPEED;
				}
				else {
					koopa->SetState(KOOPA_STATE_IS_KICKED);
				}

			}
		}
		else if(e->nx!=0) {
			if (untouchable == 0)
			{
				if ((koopa->GetState() != KOOPA_STATE_JUMP) && (koopa->GetState() != KOOPA_STATE_ISDEAD) && (koopa->GetState() != KOOPA_STATE_WALKING) and (koopa->GetState() != KOOPA_STATE_IS_KICKED))
				{
					if (!isRunning)
					{
						koopa->SetIsHeld(false);
						isKicking = true;
						start_kick = GetTickCount64();

						koopa->SetState(KOOPA_STATE_IS_KICKED);
					}
					else {
						isHolding = true;
						koopa->SetIsHeld(true);
					}
				}
				else {
					SetLevelLower();
				}
			}
		}
	}
}


void CMario::OnCollisionWithGoomba(LPCOLLISIONEVENT e)
{
	CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);
	
	if (isTailAttack) {
		AddScore(goomba->GetX(), goomba->GetY(), 100);
		score += 100;
		goomba->SetState(GOOMBA_STATE_DIE_UPSIDE);
	}
	else {
		// jump on top >> kill Goomba and deflect a bit 
		if (e->ny < 0)
		{
			if (goomba->GetIsDead()) return;
			BlockIfNoBlock(goomba);//dam bao goomba khong roi xuong khi mario jump
			goomba->SetState(GOOMBA_STATE_IS_ATTACK);
			IncreaseScoreUpCollision(x,y);
			vy -= MARIO_JUMP_DEFLECT_SPEED;
		}
		else // hit by Goomba
		{
			if (untouchable == 0)
			{
				if (goomba->GetState() != GOOMBA_STATE_DIE)
				{
					SetLevelLower();
				}
			}
		}
	}
}

void CMario::OnCollisionWithCoin(LPCOLLISIONEVENT e)
{
	CCoin* coin1 = dynamic_cast<CCoin*>(e->obj);
	if (coin1->CanCollect()) {
		e->obj->Delete();
		score += 100;
		coin++;
	}
}
void CMario::OnCollisionWithLeaf(LPCOLLISIONEVENT e) {
	e->obj->Delete();
	score += 1000;
	AddScore(x, y, 1000);
	if (level == MARIO_LEVEL_TAIL) {
	}
	else if (level != MARIO_LEVEL_SMALL) {
		AddChangeAnimation();

		SetLevel(MARIO_LEVEL_TAIL);
	}
	else SetLevel(MARIO_LEVEL_BIG);
}
void CMario::OnCollisionWithMushRoom(LPCOLLISIONEVENT e) 
{
	CMushRoom* mushroom = dynamic_cast<CMushRoom*>(e->obj);
	if (mushroom->GetModel() == MUSHROOM_RED) {
		if (!mushroom->IsDeleted()) {
			score += 1000;
			AddScore(x, y - MARIO_BIG_BBOX_HEIGHT, 1000);
		}
		if (GetLevel() == MARIO_LEVEL_SMALL)
		{
			isLower = false;
			SetLevel(MARIO_LEVEL_BIG);
		}
	}
	else if(mushroom->GetModel() == MUSHROOM_GREEN){
		if(!mushroom->IsDeleted()) AddScore(x,y-MARIO_BIG_BBOX_HEIGHT,0);
	}
	mushroom->Delete();

}

void CMario::OnCollisionWithBrickQuestion(LPCOLLISIONEVENT e) {
	CBrickQuestion* questionBrick = dynamic_cast<CBrickQuestion*>(e->obj);
	CPlayScene* scene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
	BOOLEAN isUnbox, isEmpty;
	isUnbox = questionBrick->GetIsUnbox();
	isEmpty = questionBrick->GetIsEmpty();
	if(e->ny < 0) BlockIfNoBlock(questionBrick);
	else if (((e->ny > 0) || (isTailAttack && (e->nx!=0))) && !isUnbox && !isEmpty ) {
		float xTemp, yTemp, minY;
		xTemp = questionBrick->GetX();
		yTemp = questionBrick->GetY();
		minY = questionBrick->GetMinY();

		questionBrick->SetState(QUESTION_BRICK_STATE_UP);

		if (questionBrick->GetModel() == QUESTION_BRICK_ITEM) {
			if (GetLevel() == MARIO_LEVEL_SMALL) {
				CMushRoom* mushroom = new CMushRoom(xTemp, yTemp);
				scene->AddObject(mushroom);
			}
			else if (GetLevel() >= MARIO_LEVEL_BIG) {
				CLeaf* leaf = new CLeaf(xTemp, yTemp);
				scene->AddObject(leaf);
			}
			questionBrick->SetIsEmpty(true);
		}
		else if (questionBrick->GetModel() == QUESTION_BRICK_COIN) {
			SetCoin(GetCoin() + 1);
			CCoin* coin = new CCoin(xTemp, yTemp);
			coin->SetState(COIN_SUMMON_STATE);
			questionBrick->SetIsEmpty(true);
			scene->AddObject(coin);
		}
		else {
			CMushRoom* mushroom = new CMushRoom(xTemp, yTemp, MUSHROOM_GREEN);
			scene->AddObject(mushroom);
			questionBrick->SetIsEmpty(true);

		}
	}

}




void CMario::OnCollisionWithFlowerFire(LPCOLLISIONEVENT e) {
	if (!e->obj->IsDeleted())
	{
		score += 1000;
		AddScore(x, y, 1000);
	}
	e->obj->Delete();

	if(level==MARIO_LEVEL_FIRE){}
	else if (level != MARIO_LEVEL_SMALL) {
		AddChangeAnimation();
		SetLevel(MARIO_LEVEL_FIRE);
	}
	else SetLevel(MARIO_LEVEL_BIG);
}
void CMario::OnCollisionWithPortal(LPCOLLISIONEVENT e)
{
	CPortal* p = (CPortal*)e->obj;
	CGame::GetInstance()->InitiateSwitchScene(p->GetSceneId());
}

//
// Get animdation ID for Mario
//
int CMario::GetAniIdTail()
{
	int aniId = -1;
	if (!isFlying) {
		if (!isHolding) {
			if (!isTailAttack) {
				if (!isOnPlatform)
				{

					if (abs(ax) == MARIO_ACCEL_RUN_X) {
						if (nx > 0)
							aniId = ID_ANI_MARIO_TAIL_JUMP_RUN_RIGHT;
						else
							aniId = ID_ANI_MARIO_TAIL_JUMP_RUN_LEFT;
					}
					else
					{
						if (nx >= 0)
							aniId = ID_ANI_MARIO_TAIL_JUMP_WALK_RIGHT;
						else
							aniId = ID_ANI_MARIO_TAIL_JUMP_WALK_LEFT;
					}

				}
				else {
					if (!isKicking) {
						if (isSitting)
						{
							if (nx > 0)
							{
								aniId = ID_ANI_MARIO_TAIL_SIT_RIGHT;

							}
							else
								aniId = ID_ANI_MARIO_TAIL_SIT_LEFT;
						}
						else
						{

							if (vx == 0)
							{
								if (nx > 0) aniId = ID_ANI_MARIO_TAIL_IDLE_RIGHT;
								//ID_ANI_MARIO_TAIL_ATTACK;
								//ID_ANI_MARIO_TAIL_IDLE_RIGHT;
								else aniId = ID_ANI_MARIO_TAIL_IDLE_LEFT;
							}
							else if (vx > 0)
							{
								if (ax < 0)
									aniId = ID_ANI_MARIO_TAIL_BRACE_RIGHT;
								else if (ax == MARIO_ACCEL_RUN_X) {
									if (levelRun == LEVEL_RUN_MAX) aniId = ID_ANI_MARIO_TAIL_RUNNING_RIGHT;
									else aniId = ID_ANI_MARIO_TAIL_RUNNING_PREPARE_RIGHT;
								}
								else if (ax == MARIO_ACCEL_WALK_X)
									aniId = ID_ANI_MARIO_TAIL_WALKING_RIGHT;
							}
							else // vx < 0
							{
								if (ax > 0)
									aniId = ID_ANI_MARIO_TAIL_BRACE_LEFT;
								else if (ax == -MARIO_ACCEL_RUN_X) {
									if (levelRun == LEVEL_RUN_MAX) aniId = ID_ANI_MARIO_TAIL_RUNNING_LEFT;
									else aniId = ID_ANI_MARIO_TAIL_RUNNING_PREPARE_LEFT;
								}
								else if (ax == -MARIO_ACCEL_WALK_X)
									aniId = ID_ANI_MARIO_TAIL_WALKING_LEFT;
							}

						}
					}
					else {
						if (nx > 0) aniId = ID_ANI_MARIO_TAIL_KICK_RIGHT;
						else aniId = ID_ANI_MARIO_TAIL_KICK_LEFT;
					}
				}
			}
			else aniId = ID_ANI_MARIO_TAIL_ATTACK;
		}
		else {
			if (!isOnPlatform) {
				if (nx >= 0) aniId = ID_ANI_MARIO_TAIL_HOLD_JUMP_RIGHT;
				else  aniId = ID_ANI_MARIO_TAIL_HOLD_JUMP_LEFT;
			}
			else {
				if (vx == 0) {
					if (nx > 0) aniId = ID_ANI_MARIO_TAIL_HOLD_IDLE_RIGHT;
					else aniId = ID_ANI_MARIO_TAIL_HOLD_IDLE_LEFT;
				}
				else {
					if (nx > 0) aniId = ID_ANI_MARIO_TAIL_HOLD_RUNNING_RIGHT;
					else aniId = ID_ANI_MARIO_TAIL_HOLD_RUNNING_LEFT;
				}
			}
		}
	}
	else {
		if (!isHolding) {
			if (!isOnPlatform) {
				if (levelRun == LEVEL_RUN_MAX) {
					if (nx > 0) {
						aniId = ID_ANI_MARIO_FLY_RIGHT;
					}
					else aniId = ID_ANI_MARIO_FLY_LEFT;
				}
				else {
					if (nx > 0) aniId = ID_ANI_MARIO_TAIL_FLY_DOWN_RIGHT;
					else aniId = ID_ANI_MARIO_TAIL_FLY_DOWN_LEFT;
				}
			}
			else {
				if (nx > 0) aniId = ID_ANI_MARIO_TAIL_IDLE_RIGHT;
				else aniId = ID_ANI_MARIO_TAIL_IDLE_LEFT;
			}
		}
		else
		{
			if (nx >= 0) aniId = ID_ANI_MARIO_TAIL_HOLD_JUMP_RIGHT;
			else  aniId = ID_ANI_MARIO_TAIL_HOLD_JUMP_LEFT;
		}
	}
	if (aniId == -1) aniId = ID_ANI_MARIO_TAIL_ATTACK;
	//ID_ANI_MARIO_TAIL_IDLE_RIGHT;

	return aniId;
}


int CMario::GetAniIdFire()
{
	int aniId = -1;
	if (!isShoot) {
		if (!isOnPlatform)
		{
			if (!isHolding) {
				if (abs(ax) == MARIO_ACCEL_RUN_X)
				{
					if (nx >= 0)
						aniId = ID_ANI_MARIO_FIRE_JUMP_RUN_RIGHT;
					else
						aniId = ID_ANI_MARIO_FIRE_JUMP_RUN_LEFT;
				}
				else
				{
					if (nx >= 0)
						aniId = ID_ANI_MARIO_FIRE_JUMP_WALK_RIGHT;
					else
						aniId = ID_ANI_MARIO_FIRE_JUMP_WALK_LEFT;
				}
			}
			else {
				if (nx >= 0) aniId = ID_ANI_MARIO_FIRE_HOLD_JUMP_RIGHT;
				else  aniId = ID_ANI_MARIO_FIRE_HOLD_JUMP_LEFT;
			}
		}
		else
			if (!isHolding) {
				if (!isKicking) {
					if (isSitting)
					{
						if (nx > 0)
						{
							aniId = ID_ANI_MARIO_FIRE_SIT_RIGHT;

						}
						else
							aniId = ID_ANI_MARIO_FIRE_SIT_LEFT;
					}
					else
						if (vx == 0)
						{
							if (nx > 0) aniId = ID_ANI_MARIO_FIRE_IDLE_RIGHT;
							else aniId = ID_ANI_MARIO_FIRE_IDLE_LEFT;
						}
						else if (vx > 0)
						{
							if (ax < 0)
								aniId = ID_ANI_MARIO_FIRE_BRACE_RIGHT;
							else if (ax == MARIO_ACCEL_RUN_X) {
								if (levelRun == LEVEL_RUN_MAX) aniId = ID_ANI_MARIO_FIRE_RUNNING_RIGHT;
								else aniId = ID_ANI_MARIO_FIRE_RUNNING_PREPARE_RIGHT;
							}
							else if (ax == MARIO_ACCEL_WALK_X)
								aniId = ID_ANI_MARIO_FIRE_WALKING_RIGHT;
						}
						else // vx < 0
						{
							if (ax > 0)
								aniId = ID_ANI_MARIO_FIRE_BRACE_LEFT;
							else if (ax == -MARIO_ACCEL_RUN_X) {
								if (levelRun == LEVEL_RUN_MAX) aniId = ID_ANI_MARIO_FIRE_RUNNING_LEFT;
								else aniId = ID_ANI_MARIO_FIRE_RUNNING_PREPARE_LEFT;
							}
							else if (ax == -MARIO_ACCEL_WALK_X)
								aniId = ID_ANI_MARIO_FIRE_WALKING_LEFT;
						}
				}
				else {
					if (nx > 0) aniId = ID_ANI_MARIO_FIRE_KICK_RIGHT;
					else aniId = ID_ANI_MARIO_FIRE_KICK_LEFT;
				}
			}
			else {
				if (vx == 0) {
					if (nx > 0) aniId = ID_ANI_MARIO_FIRE_HOLD_IDLE_RIGHT;
					else aniId = ID_ANI_MARIO_FIRE_HOLD_IDLE_LEFT;
				}
				else {
					if (nx > 0) aniId = ID_ANI_MARIO_FIRE_HOLD_RUNNING_RIGHT;
					else aniId = ID_ANI_MARIO_FIRE_HOLD_RUNNING_LEFT;
				}
			}
	}
	else { 
		if (nx >= 0) aniId = ID_ANI_MARIO_FIRE_SHOOT_RIGHT;
		else aniId = ID_ANI_MARIO_FIRE_SHOOT_LEFT;
	}
	if (aniId == -1) aniId = ID_ANI_MARIO_FIRE_IDLE_RIGHT;

	return aniId;
}

int CMario::GetAniIdSmall()
{
	int aniId = -1;
	if (!isChanging) {
		if (!isOnPlatform)
		{
			if (!isHolding) {
				if (abs(ax) == MARIO_ACCEL_RUN_X)
				{
					if (nx >= 0)
						aniId = ID_ANI_MARIO_SMALL_JUMP_RUN_RIGHT;
					else
						aniId = ID_ANI_MARIO_SMALL_JUMP_RUN_LEFT;
				}
				else
				{
					if (nx >= 0)
						aniId = ID_ANI_MARIO_SMALL_JUMP_WALK_RIGHT;
					else
						aniId = ID_ANI_MARIO_SMALL_JUMP_WALK_LEFT;
				}
			}
			else {
				if (nx >= 0) aniId = ID_ANI_MARIO_SMALL_HOLD_JUMP_RIGHT;
				else  aniId = ID_ANI_MARIO_SMALL_HOLD_JUMP_LEFT;
			}
		}
		else {
			if (!isHolding) {
				if (!isKicking)
					if (isSitting)
					{
						if (nx > 0)
							aniId = ID_ANI_MARIO_BIG_SIT_RIGHT;
						else
							aniId = ID_ANI_MARIO_BIG_SIT_LEFT;
					}
					else
						if (vx == 0)
						{
							if (nx > 0) aniId = ID_ANI_MARIO_SMALL_IDLE_RIGHT;
							else aniId = ID_ANI_MARIO_SMALL_IDLE_LEFT;
						}
						else if (vx > 0)
						{
							if (ax < 0)
								aniId = ID_ANI_MARIO_SMALL_BRACE_RIGHT;
							else if (ax == MARIO_ACCEL_RUN_X) {
								if (levelRun == LEVEL_RUN_MAX) aniId = ID_ANI_MARIO_SMALL_RUNNING_RIGHT;
								else aniId = ID_ANI_MARIO_SMALL_RUNNING_PREPARE_RIGHT;
							}
							else if (ax == MARIO_ACCEL_WALK_X)
								aniId = ID_ANI_MARIO_SMALL_WALKING_RIGHT;
						}
						else // vx < 0
						{
							if (ax > 0)
								aniId = ID_ANI_MARIO_SMALL_BRACE_LEFT;
							else if (ax == -MARIO_ACCEL_RUN_X) {
								if (levelRun == LEVEL_RUN_MAX) aniId = ID_ANI_MARIO_SMALL_RUNNING_LEFT;
								else aniId = ID_ANI_MARIO_SMALL_RUNNING_PREPARE_LEFT;
							}
							else if (ax == -MARIO_ACCEL_WALK_X)
								aniId = ID_ANI_MARIO_SMALL_WALKING_LEFT;
						}
				else {
					if (nx > 0) aniId = ID_ANI_MARIO_SMALL_KICK_RIGHT;
					else aniId = ID_ANI_MARIO_SMALL_KICK_LEFT;
				}
			}
			else {
				if (vx == 0) {
					if (nx > 0) aniId = ID_ANI_MARIO_SMALL_HOLD_IDLE_RIGHT;
					else aniId = ID_ANI_MARIO_SMALL_HOLD_IDLE_LEFT;
				}
				else {
					if (nx > 0) aniId = ID_ANI_MARIO_SMALL_HOLD_RUNNING_RIGHT;
					else aniId = ID_ANI_MARIO_SMALL_HOLD_RUNNING_LEFT;
				}
			}
		}
	}
	else {
		if (nx > 0) aniId = ID_ANI_MARIO_CHANGE_SMALL_TO_BIG_RIGHT;
		else aniId = ID_ANI_MARIO_CHANGE_SMALL_TO_BIG_LEFT;
	}

	if (aniId == -1) aniId = ID_ANI_MARIO_TAIL_ATTACK;

	return aniId;
}



int CMario::GetAniIdBig()
{
	int aniId = -1;
	if (!isChanging) {
		if (!isOnPlatform)
		{
			if (!isHolding) {
				if (abs(ax) == MARIO_ACCEL_RUN_X)
				{
					if (nx >= 0)
						aniId = ID_ANI_MARIO_BIG_JUMP_RUN_RIGHT;
					else
						aniId = ID_ANI_MARIO_BIG_JUMP_RUN_LEFT;
				}
				else
				{
					if (nx >= 0)
						aniId = ID_ANI_MARIO_BIG_JUMP_WALK_RIGHT;
					else
						aniId = ID_ANI_MARIO_BIG_JUMP_WALK_LEFT;
				}
			}
			else {
				if (nx >= 0) aniId = ID_ANI_MARIO_BIG_HOLD_JUMP_RIGHT;
				else  aniId = ID_ANI_MARIO_BIG_HOLD_JUMP_LEFT;
			}
		}
		else
			if (!isHolding) {
				if (!isKicking) {
					if (isSitting)
					{
						if (nx > 0)
							aniId = ID_ANI_MARIO_BIG_SIT_RIGHT;
						else
							aniId = ID_ANI_MARIO_BIG_SIT_LEFT;
					}
					else
						if (vx == 0)
						{
							if (nx > 0) aniId = ID_ANI_MARIO_BIG_IDLE_RIGHT;
							else aniId = ID_ANI_MARIO_BIG_IDLE_LEFT;
						}
						else if (vx > 0)
						{
							if (ax < 0)
								aniId = ID_ANI_MARIO_BIG_BRACE_RIGHT;
							else if (ax == MARIO_ACCEL_RUN_X) {
								if (levelRun == LEVEL_RUN_MAX) aniId = ID_ANI_MARIO_BIG_RUNNING_RIGHT;
								else aniId = ID_ANI_MARIO_BIG_RUNNING_PREPARE_RIGHT;
							}
							else if (ax == MARIO_ACCEL_WALK_X)
								aniId = ID_ANI_MARIO_BIG_WALKING_RIGHT;
						}
						else // vx < 0
						{
							if (ax > 0)
								aniId = ID_ANI_MARIO_BIG_BRACE_LEFT;
							else if (ax == -MARIO_ACCEL_RUN_X) {
								if (levelRun == LEVEL_RUN_MAX) aniId = ID_ANI_MARIO_BIG_RUNNING_LEFT;
								else aniId = ID_ANI_MARIO_BIG_RUNNING_PREPARE_LEFT;
							}
							else if (ax == -MARIO_ACCEL_WALK_X)
								aniId = ID_ANI_MARIO_BIG_WALKING_LEFT;
						}
				}
				else {
					if (nx > 0) aniId = ID_ANI_MARIO_BIG_KICK_RIGHT;
					else aniId = ID_ANI_MARIO_BIG_KICK_LEFT;
				}
			}
			else {
				if (vx == 0) {
					if (nx > 0) aniId = ID_ANI_MARIO_BIG_HOLD_IDLE_RIGHT;
					else aniId = ID_ANI_MARIO_BIG_HOLD_IDLE_LEFT;
				}
				else {
					if (nx > 0) aniId = ID_ANI_MARIO_BIG_HOLD_RUNNING_RIGHT;
					else aniId = ID_ANI_MARIO_BIG_HOLD_RUNNING_LEFT;
				}
			}
	}
	else {
		if (nx > 0) aniId = ID_ANI_MARIO_CHANGE_SMALL_TO_BIG_RIGHT;
		else aniId = ID_ANI_MARIO_CHANGE_SMALL_TO_BIG_LEFT;
	}

	if (aniId == -1) aniId = ID_ANI_MARIO_BIG_IDLE_RIGHT;

	return aniId;
}

void CMario::Render()
{
	CAnimations* animations = CAnimations::GetInstance();
	
	int aniId = -1;
	if (state == MARIO_STATE_DIE)
		aniId = ID_ANI_MARIO_DIE;
	else if (level == MARIO_LEVEL_BIG)
		aniId = GetAniIdBig();
	else if (level == MARIO_LEVEL_SMALL)
		aniId = GetAniIdSmall();
	else if (level == MARIO_LEVEL_FIRE)
		aniId = GetAniIdFire();
	else if (level == MARIO_LEVEL_TAIL)
		aniId = GetAniIdTail();
	if((level>MARIO_LEVEL_BIG)) {
		if(!isChanging) animations->Get(aniId)->Render(x, y);
	}
	else {
		if((level==MARIO_LEVEL_BIG) && (isLower)){
		}
		else {
			if (!untouchable)
			{
				animations->Get(aniId)->Render(x, y);
			}
			else {
				int check = rand() % 2;
				if (check == 0) {
					animations->Get(aniId)->Render(x, y);
				}
			}
		}
	}
	

	//RenderBoundingBox();
	
	//DebugOutTitle(L"Coins: %d", coin);
}

void CMario::SetState(int state)
{
	// DIE is the end state, cannot be changed! 
	if (this->state == MARIO_STATE_DIE) return; 

	switch (state)
	{
	case MARIO_STATE_RUNNING_RIGHT:
		if (isSitting) {
			vx = 0;
			break;

		}
		SetMarioTailAttack();
		maxVx = MARIO_RUNNING_SPEED + levelRun*SPEED_LEVEL_RUN;
		ax = MARIO_ACCEL_RUN_X;
		isRunning = true;
		nx = 1;
		break;

	case MARIO_STATE_RUNNING_LEFT:
		if (isSitting) { 
			vx = 0;
			break; 
		}
		SetMarioTailAttack();
		maxVx = -MARIO_RUNNING_SPEED - levelRun* SPEED_LEVEL_RUN;
		ax = -MARIO_ACCEL_RUN_X;
		isRunning = true;
		nx = -1;
		break;
	case MARIO_STATE_WALKING_RIGHT:
		if (isSitting)
		{
			vx = 0;
			break;
		}
		isRunning = false;

		maxVx = MARIO_WALKING_SPEED;
		ax = MARIO_ACCEL_WALK_X;
		nx = 1;
		break;
	case MARIO_STATE_WALKING_LEFT:
		if (isSitting) {
			vx = 0;
			break;
		}
		isRunning = false;

		maxVx = -MARIO_WALKING_SPEED;
		ax = -MARIO_ACCEL_WALK_X;
		nx = -1;
		break;
	case MARIO_STATE_FALL:
		if (vy < 0) vy += MARIO_JUMP_SPEED_Y / 5;
		isRunning = false;

		break;
	case MARIO_STATE_JUMP:
		if (isSitting) break;

		if (isOnPlatform)
		{
			if (abs(this->vx) == MARIO_RUNNING_SPEED)
				vy = -MARIO_JUMP_RUN_SPEED_Y;
			else
				vy = -MARIO_JUMP_SPEED_Y;
		}
		break;

	case MARIO_STATE_RELEASE_JUMP:
		if (vy < 0) vy += MARIO_JUMP_SPEED_Y / 2;
		break;

	case MARIO_STATE_SIT:
		if (isOnPlatform && level != MARIO_LEVEL_SMALL)
		{
			isSitting = true;
			isRunning = false;
			state = MARIO_STATE_IDLE;
			y += MARIO_SIT_HEIGHT_ADJUST;
		}
		break;

	case MARIO_STATE_SIT_RELEASE:
		if (isSitting)
		{
			isSitting = false;
			state = MARIO_STATE_IDLE;
			y -= MARIO_SIT_HEIGHT_ADJUST;
		}
		break;

	case MARIO_STATE_IDLE:
		ax = 0.0f;
		vx = 0.0f;

		break;
	case MARIO_STATE_TAIL_ATTACK:
		isTailAttack = true;
		start_tail_attack = GetTickCount64();
		break;
	case MARIO_STATE_FLY: 
		isFlying = true;
		isOnPlatform = false;
		SetFly();
		break;
	case MARIO_STATE_SHOOT:
		if (GetTickCount64() - start_limit_shoot > TIME_SHOOT_LIMIT) {
			isShoot = true;
			start_shoot = GetTickCount64();
		}
		break;
	case MARIO_STATE_DIE:
		vy = -MARIO_JUMP_DEFLECT_SPEED_DIE/2;
		ay = MARIO_GRAVITY / 3;
		untouchable = false;
		vx = 0;
		ax = 0;
		break;
	}
	CGameObject::SetState(state);
}

void CMario::GetBoundingBox(float &left, float &top, float &right, float &bottom)
{

	if (level != MARIO_LEVEL_SMALL)
	{

		if (isSitting)
		{
			left = x - MARIO_BIG_SITTING_BBOX_WIDTH / 2;
			top = y - MARIO_BIG_SITTING_BBOX_HEIGHT / 2;
			right = left + MARIO_BIG_SITTING_BBOX_WIDTH;
			bottom = top + MARIO_BIG_SITTING_BBOX_HEIGHT;
		}
		else {
				left = x - MARIO_BIG_BBOX_WIDTH / 2;
				top = y - MARIO_BIG_BBOX_HEIGHT / 2;
				right = left + MARIO_BIG_BBOX_WIDTH;
				bottom = top + MARIO_BIG_BBOX_HEIGHT;
		}

	}
	else
	{
		left = x - MARIO_SMALL_BBOX_WIDTH/2;
		top = y - MARIO_SMALL_BBOX_HEIGHT/2;
		right = left + MARIO_SMALL_BBOX_WIDTH;
		bottom = top + MARIO_SMALL_BBOX_HEIGHT;
	}
}

void CMario::SetLevel(int l)
{
	// Adjust position to avoid falling off platform
	if (this->level == MARIO_LEVEL_SMALL)
	{
		y -= (MARIO_BIG_BBOX_HEIGHT - MARIO_SMALL_BBOX_HEIGHT) / 2;
	}
	
	isChanging = true;
	start_changing = GetTickCount64();
	
	level = l;
}

void CMario::BlockIfNoBlock(LPGAMEOBJECT gameobject) {
	if (level == MARIO_LEVEL_SMALL) {
		//SetY(platform->GetY() - 15);
		if (gameobject->GetY() - GetY() < (MARIO_SMALL_BBOX_HEIGHT + 4))
		{
			SetY(gameobject->GetY() - MARIO_SMALL_BBOX_HEIGHT - 2);
			vy = 0;
			isOnPlatform = true;
		}
	}
	else {
		if (!isSitting) {
			if (gameobject->GetY() - GetY() < MARIO_BIG_BBOX_HEIGHT)
			{
				SetY(gameobject->GetY() - MARIO_BIG_BBOX_HEIGHT + 4);
				vy = 0;
				isOnPlatform = true;
			}
		}
		else {
			if (gameobject->GetY() - GetY() < MARIO_BIG_BBOX_HEIGHT / 2 + 4)
			{
				SetY(gameobject->GetY() - MARIO_BIG_BBOX_HEIGHT / 2 - 4);
				vy = 0;
				isOnPlatform = true;
			}
		}
	}
}

void CMario::SetLevelLower() {
	isLower = true;
	
	if (level > MARIO_LEVEL_SMALL)
	{
		StartUntouchable();
		if (level == MARIO_LEVEL_BIG) {

			SetLevel(MARIO_LEVEL_SMALL);
		}
		else {
			AddChangeAnimation();
			SetLevel(MARIO_LEVEL_BIG);
		}
	}
	else
	{
		DebugOut(L">>> Mario DIE >>> \n");
		SetState(MARIO_STATE_DIE);
	}
}

void CMario::SetFly() {
	if (levelRun == LEVEL_RUN_MAX) {
		vy = -MARIO_FLYING;
	}
	else vy = -MARIO_FLY_FALL;
	isFlying = true;
}

void CMario::SetMarioTailAttack() {
	if (level == MARIO_LEVEL_TAIL) {
		isTailAttack = true;
	}
}

void CMario::AddChangeAnimation() {
	CPlayScene* scene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
	CEffect* effect = new CEffect(x, y, EFFECT_CHANGE);
	scene->AddObject(effect);
}

void CMario::AddEffectAttack(float xTemp, float yTemp) {
	CPlayScene* scene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
	CEffect* effect = new CEffect(xTemp, yTemp, EFFECT_ATTACK);
	scene->AddObject(effect);
}
void CMario::AddScore(float xTemp, float yTemp, int scoreAdd) {
	CPlayScene* scene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
	if (scoreAdd == 100) {
		CEffect* effect = new CEffect(xTemp, yTemp, EFFECT_SCORE_100);
		scene->AddObject(effect);
	}
	else if(scoreAdd == 200) {
		CEffect* effect = new CEffect(xTemp, yTemp, EFFECT_SCORE_200);
		scene->AddObject(effect);
	}
	else if (scoreAdd == 400) {
		CEffect* effect = new CEffect(xTemp, yTemp, EFFECT_SCORE_400);
		scene->AddObject(effect);
	}
	else if (scoreAdd == 800) {
		CEffect* effect = new CEffect(xTemp, yTemp, EFFECT_SCORE_800);
		scene->AddObject(effect);
	}
	else if (scoreAdd == 1000) {
		CEffect* effect = new CEffect(xTemp, yTemp, EFFECT_SCORE_1000);
		scene->AddObject(effect);
	}
	else if (scoreAdd == 2000) {
		CEffect* effect = new CEffect(xTemp, yTemp, EFFECT_SCORE_2000);
		scene->AddObject(effect);
	}
	else if (scoreAdd == 4000) {
		CEffect* effect = new CEffect(xTemp, yTemp, EFFECT_SCORE_4000);
		scene->AddObject(effect);
	}
	else if (scoreAdd == 8000) {
		CEffect* effect = new CEffect(xTemp, yTemp, EFFECT_SCORE_8000);
		scene->AddObject(effect);
	}
	else if (scoreAdd == 0){
		Up++;
		CEffect* effect = new CEffect(xTemp, yTemp, EFFECT_UP);
		scene->AddObject(effect);
	}
}


void CMario::IncreaseScoreUpCollision(float xTemp, float yTemp) {
	start_score_up = GetTickCount64();
	if (scoreUpCollision == 1) {
		score += 100;
		AddScore(xTemp, yTemp, 100);
		scoreUpCollision++;
	}
	else if (scoreUpCollision == 2) {
		score += 200;
		AddScore(xTemp, yTemp, 200);
		scoreUpCollision++;
	}
	else if (scoreUpCollision == 3) {
		score += 400;
		AddScore(xTemp, yTemp, 400);
		scoreUpCollision++;
	}
	else if (scoreUpCollision == 4) {
		score += 800;
		AddScore(xTemp, yTemp, 800);
		scoreUpCollision++;
	}
	else if (scoreUpCollision == 5) {
		score += 1000;
		AddScore(xTemp, yTemp, 1000);
		scoreUpCollision++;
	}
	else if (scoreUpCollision == 6) {
		score += 2000;
		AddScore(xTemp, yTemp, 2000);
		scoreUpCollision++;
	}
	else if (scoreUpCollision == 7) {
		score += 4000;
		AddScore(xTemp, yTemp, 4000);
		scoreUpCollision++;
	}
	else if (scoreUpCollision == 8) {
		score += 8000;
		AddScore(xTemp, yTemp, 8000);
		scoreUpCollision++;
	}
	else if (scoreUpCollision > 8) {
		scoreUpCollision = 9;
		AddScore(xTemp, yTemp, 0);
	}

}