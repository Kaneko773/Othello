#include "Share.h"

void InPutStateJudge(InputState& inputState, InputState& prevInputState, int& level, bool& levelDec);

void LevelSelect(int& level, InputState& inputState, InputState& prevInputState, GameState& gameState) {
	ClearDrawScreen();//画面クリア

	bool levelDecision = false;
	//レベル選択
	InPutStateJudge(inputState, prevInputState, level, levelDecision);

	

	//表示
	DrawString(259, 104, "レベル 1", GetColor(255, 255, 255));
	DrawString(259, 224, "レベル 2", GetColor(255, 255, 255));
	DrawString(140, 344, "Enterを押してゲーム開始", GetColor(255, 255, 255));
	switch (level)
	{
	case 1:
		DrawTriangle(230, 104, 230, 134, 252, 119, GetColor(255, 255, 0), TRUE);
		break;
	case 2:
		DrawTriangle(230, 224, 230, 254, 252, 239, GetColor(255, 255, 0), TRUE);
		break;
	}

	ScreenFlip();//表示

	if (levelDecision) {
		gameState = e_Loading;
	}
}

void InPutStateJudge(InputState& inputState, InputState& prevInputState, int& level, bool& levelDec) {
	//常に１つのキー入力しか受け付けないようにする
	if (inputState == NotPressed) {
		if (CheckHitKey(KEY_INPUT_DOWN) == 1) {
			inputState = PressDownAr;
		}
		else if (CheckHitKey(KEY_INPUT_UP) == 1) {
			inputState = PressUpAr;
		}
		else if (CheckHitKey(KEY_INPUT_RETURN) == 1) {
			inputState = PressReturn;
		}
	}
	else {
		switch (inputState) {
		case PressDownAr: if (CheckHitKey(KEY_INPUT_DOWN) == 0)inputState = NotPressed; break;
		case PressUpAr: if (CheckHitKey(KEY_INPUT_UP) == 0)inputState = NotPressed; break;
		case PressReturn: if (CheckHitKey(KEY_INPUT_RETURN) == 0)inputState = NotPressed; break;
		}
	}

	//離した状態からボタンを初めて押した時
	if (prevInputState == NotPressed) {
		if (inputState != NotPressed && inputState != PressReturn) {
			switch (inputState)
			{
			//レベルを増やしたらここを変更
			case PressDownAr:
				--level;
				if (level < 1)level = 2;
				break;
			case PressUpAr:
				++level;
				if (level > 2)level = 1;
				break;
			}
		}
		else if (inputState == PressReturn) {
			levelDec = true;
		}
	}

	prevInputState = inputState;
}