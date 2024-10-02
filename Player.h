#pragma once
#include "Base.h"

class Player : public Base
{
public:
	Player(bool turn, Name name);
	~Player();
	Action Update(Vector2* aPos, Board* board);

private:
	InputState m_inputState, m_prevInputState;
};

