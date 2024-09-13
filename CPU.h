#pragma once
#include "Base.h"

class CPU : public Base
{
public:
	CPU(bool turn, Name name);
	~CPU();
	Action Update(Vector2* aPos, Board* board);

private:
	bool m_arrowMoving = false;
	LONGLONG m_moveStartTime;
	Vector2 m_putPos;
};

