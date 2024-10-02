#pragma once
#include "DxLib.h"

struct Vector2 {
	int x;
	int y;
};

enum PlaceState {
	white = 0,
	black = 1,
	non = 2,
};

enum InputState
{
	NotPressed = 0,
	PressLeftAr = 1,
	PressDownAr = 2,
	PressRightAr = 3,
	PressUpAr = 4,
	PressReturn = 5,
	PressEscape = 6,
};

enum GameState {
	e_LevelSelect,
	e_Loading,
	e_MainGame,
	e_GameEnd,
};