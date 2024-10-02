#include "CPU.h"

struct NextMoveCandi
{
	int putPos;//置く場所
	double winRate;//勝率
};


CPU::CPU(bool turn, Name name, int level) {
	m_name = name;
	m_turn = turn;
	m_endPutGudge = false;
	m_arrowMoving = false;
	m_moveStartTime = 0;
	m_putPos = { 0, 0 };
	m_level = level;
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
				if (PutGudge(temp, board, Get_m_turn())) {
					skip = false;
				}
			}
		}
		if (skip) { return Action::skip; }

		m_endPutGudge = true;
	}

	if (!m_arrowMoving) {
		int tempPos = 99;
		switch (m_level)
		{
		case 1:
			tempPos = Level1(board);
			break;
		case 2:
			tempPos = Level2(board);
			break;
		default:
			return error;
			break;
		}

		m_putPos = { tempPos % 8, tempPos / 8 };
		m_arrowMoving = true;
		m_moveStartTime = GetNowHiPerformanceCount();
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

int CPU::Level1(Board* board) {
	//置く場所候補を調べる
	int* candidates = new int[1];//置く場所候補
	candidates[0] = 99;
	int candidatesNum = 0;
	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j) {
			Vector2 temp = { j, i };
			if (PutGudge(temp, board, Get_m_turn())) {
				++candidatesNum;
				//動的配列のサイズを広げて置く場所候補を保存していく
				int* temp = candidates;
				candidates = new int[candidatesNum];
				for (int k = 0; k < candidatesNum - 1; ++k) {
					candidates[k] = temp[k];
				}
				candidates[candidatesNum - 1] = i * 8 + j;
				delete[] temp;
				temp = 0;
			}
		}
	}
	if (candidates[0] == 99) {
		//エラー
		delete[] candidates;
		candidates = 0;
		return error;
	}
	//候補内からランダムで置く
	int putPos = candidates[GetRand(candidatesNum - 1)];
	//後処理
	delete[] candidates;
	candidates = 0;

	return putPos;
}

int CPU::Level2(Board* board) {
	//置く場所候補を調べる
	NextMoveCandi* candidates = new NextMoveCandi[1];//置く場所候補
	candidates[0].putPos = 99;
	candidates[0].winRate = 0;
	int candidatesNum = 0;
	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j) {
			Vector2 temp = { j, i };
			if (PutGudge(temp, board, Get_m_turn())) {
				++candidatesNum;
				//動的配列のサイズを広げて置く場所候補を保存していく
				NextMoveCandi* temp = candidates;
				candidates = new NextMoveCandi[candidatesNum];
				for (int k = 0; k < candidatesNum - 1; ++k) {
					candidates[k] = temp[k];
				}
				candidates[candidatesNum - 1].putPos = i * 8 + j;
				candidates[candidatesNum - 1].winRate = 0;
				delete[] temp;
				temp = 0;
			}
		}
	}
	if (candidatesNum == 1) {
		//置く場所が１箇所の時はそのまま置くので処理無し
	}
	else {
		for (int i = 0; i < candidatesNum; ++i) {
			Vector2 tempPos = { candidates[i].putPos % 8, candidates[i].putPos / 8 };
			int winNum = 0;
			int routeNum = 0;
			Search(*board, tempPos, 1/*1手目からスタート*/, winNum, routeNum, Get_m_turn());
			candidates[i].winRate = (double)winNum / (double)routeNum;//置ける場所があるか既に判定してるので　routeNum = 0　の処理は無し
		}

		//最も勝率が高い手を先頭へ
		for (int i = 1; i < candidatesNum; ++i) {
			//今は左上が優先(番号の小さい順に優先)
			if (candidates[0].winRate < candidates[i].winRate) {
				NextMoveCandi temp = candidates[i];
				candidates[i] = candidates[0];
				candidates[0] = temp;
			}
		}
	}

	int putPos = candidates[0].putPos;
	//後処理
	delete[] candidates;
	candidates = 0;

	return putPos;
}

//Level２で使用
void CPU::Search(Board board, Vector2 pos, int count, int& winNum, int& routeNum, bool turn, bool skipOnce) {
	ProcessMessage();
	if (pos.x != 99) {
		//渡された場所に駒を置いて裏返し
		PlaceState putColor = turn ? black : white;
		PlaceState otherColor = turn ? white : black;
		board.Set_m_pieces_pState(pos.x, pos.y, putColor);

		Vector2 dir[8] = {
				{1, 0},//右
				{1, 1},//右下
				{0, 1},//下
				{-1, 1},//左下
				{-1, 0},//左
				{-1, -1},//左上
				{0, -1},//上
				{1, -1}//右上
		};
		//右から時計周りで調べる
		int enclosedNum = 0;//各方向の裏返した数
		for (int i = 0; i < 8; ++i) {
			//隣が違う色なら
			if (board.Get_m_pieces_pState(pos.x + dir[i].x, pos.y + dir[i].y) == otherColor) {
				enclosedNum = 0;
				//調べる(同じ色を見つける、駒が置かれていないマスを見つける、調べる範囲を出るまでループ)
				for (int j = 2; (0 <= pos.x + dir[i].x * j && pos.x + dir[i].x * j < 8) && (0 <= pos.y + dir[i].y * j && pos.y + dir[i].y * j < 8); ++j) {
					if (board.Get_m_pieces_pState(pos.x + dir[i].x * j, pos.y + dir[i].y * j) == non) {
						break;
					}
					if (board.Get_m_pieces_pState(pos.x + dir[i].x * j, pos.y + dir[i].y * j) == putColor) {
						enclosedNum = j - 1;
						break;
					}
				}
				//裏返し
				for (int j = 0; j < enclosedNum; ++j) {
					board.Set_m_pieces_pState(pos.x + dir[i].x * (j + 1), pos.y + dir[i].y * (j + 1), putColor);
				}
			}
		}
		board.PieceNumUpdate();//駒の枚数更新
	}


	//置く場所候補を調べる
	// →候補有　調べた回数によって　繰り返す　か　枚数を調べて勝敗を返す
	// →候補無　枚数を調べて勝敗を返す
	
	//置く場所候補を調べる
	NextMoveCandi* candidates = new NextMoveCandi[1];//置く場所候補
	candidates[0].putPos = 99;
	candidates[0].winRate = 0;
	int candidatesNum = 0;
	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j) {
			Vector2 temp = { j, i };
			if (PutGudge(temp, &board, turn)) {
				++candidatesNum;
				//動的配列のサイズを広げて置く場所候補を保存していく
				NextMoveCandi* temp = candidates;
				candidates = new NextMoveCandi[candidatesNum];
				for (int k = 0; k < candidatesNum - 1; ++k) {
					candidates[k] = temp[k];
				}
				candidates[candidatesNum - 1].putPos = i * 8 + j;
				candidates[candidatesNum - 1].winRate = 0;
				delete[] temp;
				temp = 0;
			}
		}
	}
	//候補無し
	if (candidates[0].putPos == 99) {
		//スキップの処理
		if (skipOnce) {
			routeNum += 1;//探索終了したので
			//２人ともスキップなので探索終了
			if (board.Get_m_BlackPieceNum() == board.Get_m_WhitePieceNum()) {
				//何もしない
			}
			else if (board.Get_m_BlackPieceNum() > board.Get_m_WhitePieceNum()) {
				if (this->Get_m_turn() == 1/*黒*/) {
					winNum += 1;
				}
			}
			else {
				if (this->Get_m_turn() == 0/*白*/) {
					winNum += 1;
				}
			}
		}
		else {
			Vector2 temp = { 99, 0 };//スキップ時は存在しない場所を渡す
			Search(board, temp, count, winNum, routeNum, !turn, true);
		}
	}
	//候補有り
	else {
		//5手目なら終了
		if (count >= 5) {
			routeNum += 1;//探索終了したので
			if (board.Get_m_BlackPieceNum() == board.Get_m_WhitePieceNum()) {
				//何もしない
			}
			else if (board.Get_m_BlackPieceNum() > board.Get_m_WhitePieceNum()) {
				if (this->Get_m_turn() == 1/*黒*/) {
					winNum += 1;
				}
			}
			else {
				if (this->Get_m_turn() == 0/*白*/) {
					winNum += 1;
				}
			}
		}
		else {
			for (int i = 0; i < candidatesNum; ++i) {
				Vector2 tempPos = { candidates[i].putPos % 8, candidates[i].putPos / 8 };
				Search(board, tempPos, count + 1, winNum, routeNum, !turn);
			}
		}
	}

	//後処理
	delete[] candidates;
	candidates = 0;
}