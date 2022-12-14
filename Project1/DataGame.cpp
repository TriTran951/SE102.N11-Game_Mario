#include "DataGame.h"
#include "Mario.h"

CDataGame::CDataGame() {
	ResetFullData();
}

void CDataGame::SavePositionXWorldMap(float f) { position_x_world_map = f; }
void CDataGame::SavePositionYWorldMap(float f) { position_y_world_map = f; }
void CDataGame::SaveLevel(int l) { level = l; }
void CDataGame::SaveScore(int s) { score = s; }
void CDataGame::SaveCoin(int c) { coin = c; }
void CDataGame::SaveUp(int u) { Up = u; }
void CDataGame::SaveCard1(int c1) { card1 = c1; }
void CDataGame::SaveCard2(int c2) { card2 = c2; }
void CDataGame::SaveCard3(int c3) { card3 = c3; }
void CDataGame::SaveDoorProcess(int n) { saveDoorProcess = n; }
void CDataGame::SetIsDisplayHUD(bool b) { isDisplayHUD = b; }
void CDataGame::SetPrepareContinue(bool b) { prepareContinue = b; }
void CDataGame::SetPrepareEnd(bool b) { prepareEnd = b; }

void CDataGame::SaveAllowKey(bool isAllowLeft, bool isAllowTop, bool isAllowRight, bool isAllowBottom) {
	this->isAllowLeft = isAllowLeft;
	this->isAllowTop = isAllowTop;
	this->isAllowRight = isAllowRight;
	this->isAllowBottom = isAllowBottom;
}