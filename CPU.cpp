#include "CPU.h"

CPU::CPU(bool turn, Name name) {
	m_name = name;
	m_turn = turn;
	m_endPutGudge = false;
	m_arrowMoving = false;
	m_moveStartTime = 0;
	m_putPos = { 0, 0 };
}

CPU::~CPU() {
	//何もしない
}

Action CPU::Update(Vector2* aPos, Board* board) {
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
		if (skip) { return Action::skip; }

		m_endPutGudge = true;
	}

	if (!m_arrowMoving) {
		//置く場所候補を調べる
		int* m_candidates = new int[1];//置く場所候補
		m_candidates[0] = 99;
		int candidatesNum = 0;
		for (int i = 0; i < 8; ++i) {
			for (int j = 0; j < 8; ++j) {
				Vector2 temp = { j, i };
				if (PutGudge(temp, board)) {
					++candidatesNum;
					//動的配列のサイズを広げて置く場所候補を保存していく
					int* temp = m_candidates;
					m_candidates = new int[candidatesNum];
					for (int k = 0; k < candidatesNum - 1; ++k) {
						m_candidates[k] = temp[k];
					}
					m_candidates[candidatesNum - 1] = i * 8 + j;
					delete[] temp;
					temp = 0;
				}
			}
		}
		if (m_candidates[0] == 99) {
			//エラー
			delete[] m_candidates;
			m_candidates = 0;
			return error;
		}
		//候補内からランダムで置く
		int tempNum = m_candidates[GetRand(candidatesNum - 1)];
		m_putPos = { tempNum % 8, tempNum / 8 };
		m_arrowMoving = true;
		m_moveStartTime = GetNowHiPerformanceCount();
		//後処理
		delete[] m_candidates;
		m_candidates = 0;
	}
	else 
	{
		//ここでCPUの矢印移動をする
		if (GetNowHiPerformanceCount() - m_moveStartTime > 200000) {//１マスを0.2秒で移動
			//x軸移動
			if (aPos->x < m_putPos.x) {
				++aPos->x;
			}
			else if (aPos->x > m_putPos.x) {
				--aPos->x;
			}
			//y軸移動
			else {
				if (aPos->y < m_putPos.y) {
					++aPos->y;
				}
				else if (aPos->y > m_putPos.y) {
					--aPos->y;
				}
				//目的地まで着いたら駒を置く
				else {
					if (!TurnOver(m_putPos, board)) {
						//エラー
						return error;
					}

					m_arrowMoving = false;
					m_endPutGudge = false;
					return turnChange;
				}
			}
			m_moveStartTime = GetNowHiPerformanceCount();//更新
		}
	}

	return inAction;
}