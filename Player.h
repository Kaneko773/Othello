#pragma once
#include "Base.h"

enum InputState
{
	NotPressed = 0,
	PressLeftAr = 1,
	PressDownAr = 2,
	PressRightAr = 3,
	PressUpAr = 4,
	PressReturn = 5,
};

class Player : public Base
{
public:
	Player(bool turn, Name name);
	~Player();
	Action Update(Vector2* aPos, Board* board);

private:
	InputState m_inputState, m_prevInputState;
};

