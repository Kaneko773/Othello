#pragma once
#include "Base.h"

class CPU : public Base
{
public:
	CPU(bool turn, Name name, int level);
	~CPU();
	Action Update(Vector2* aPos, Board* board);

private:
	int Level1(Board* board);
	int Level2(Board* board);
	void Search(Board board, Vector2 pos, int count, int& winNum, int& routeNum, bool turn, bool skipOnece = false);

	bool m_arrowMoving;
	LONGLONG m_moveStartTime;
	Vector2 m_putPos;
	int m_level;
};

