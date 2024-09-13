#include "Player.h"

Player::Player(bool turn, Name name) {
	m_name = name;
	m_turn = turn;
	m_endPutGudge = false;
	m_prevInputState = m_inputState = NotPressed;
}

Player::~Player(){
	//何もしない
}

Action Player::Update(Vector2* aPos, Board* board) {
	//行動の度１回だけ通る
	if (!m_endPutGudge) {
		bool skip = true;
		//駒を置けるマスがあるかどうか調べる
		for (int y = 0; y < 8 && skip; ++y) {
			for (int x = 0; x < 8 && skip; ++x) {
				//置ける場所が１つでもあったら調べるのを終了
				Vector2 temp = { x, y };
				if (PutGudge(temp, board)) {
					skip = false;
				}
			}
		}
		if (skip) { return Action::skip; }//置けるマスが無かったらスキップ

		m_endPutGudge = true;
	}

	//常に１つのキー入力しか受け付けないようにする
	if (m_inputState == NotPressed) {
		if (CheckHitKey(KEY_INPUT_LEFT) == 1) {
			m_inputState = PressLeftAr;
		}
		else if (CheckHitKey(KEY_INPUT_DOWN) == 1) {
			m_inputState = PressDownAr;
		}
		else if (CheckHitKey(KEY_INPUT_RIGHT) == 1) {
			m_inputState = PressRightAr;
		}
		else if (CheckHitKey(KEY_INPUT_UP) == 1) {
			m_inputState = PressUpAr;
		}
		else if (CheckHitKey(KEY_INPUT_RETURN) == 1) {
			m_inputState = PressReturn;
		}
	}
	else {
		switch (m_inputState) {
		case PressLeftAr: if (CheckHitKey(KEY_INPUT_LEFT) == 0)m_inputState = NotPressed; break;
		case PressDownAr: if (CheckHitKey(KEY_INPUT_DOWN) == 0)m_inputState = NotPressed; break;
		case PressRightAr: if (CheckHitKey(KEY_INPUT_RIGHT) == 0)m_inputState = NotPressed; break;
		case PressUpAr: if (CheckHitKey(KEY_INPUT_UP) == 0)m_inputState = NotPressed; break;
		case PressReturn: if (CheckHitKey(KEY_INPUT_RETURN) == 0)m_inputState = NotPressed; break;
		default:break;
		}
	}

	//離した状態からボタンを初めて押した時
	if (m_prevInputState == NotPressed) {
		if (m_inputState != NotPressed && m_inputState != PressReturn) {
			switch (m_inputState)
			{
			case PressLeftAr:
				if (aPos->x > 0) --aPos->x;
				break;
			case PressDownAr:
				if (aPos->y < 7) ++aPos->y;
				break;
			case PressRightAr:
				if (aPos->x < 7) ++aPos->x;
				break;
			case PressUpAr:
				if (aPos->y > 0) --aPos->y;
				break;
			}
		}
		else if (m_inputState == PressReturn) {
			if (TurnOver(*aPos, board)) {//駒を置く処理
				m_prevInputState = NotPressed;
				m_endPutGudge = false;
				return turnChange;
			}
			else {
				//置けなかった時は結果的に何も起こらない
			}
		}
	}

	m_prevInputState = m_inputState;
	return inAction;
}