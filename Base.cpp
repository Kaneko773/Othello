#include "Base.h"

//裏返し処理(１枚でも裏返したらtrueを返す)
bool Base::TurnOver(Vector2 aPos, Board* board) const {
	if (board->Get_m_pieces_pState(aPos.x, aPos.y) != non)return false;//既に駒が置かれていたら終了

	PlaceState turnColor = m_turn ? black : white;
	PlaceState otherColor = m_turn ? white : black;

	board->Set_m_pieces_pState(aPos.x, aPos.y, turnColor);//もし1枚も裏返せなかったら後で消す※１

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
	int turnOverNum = 0;//裏返した総数
	int enclosedNum = 0;//各方向の裏返した数
	for (int i = 0; i < 8; ++i) {
		//隣が違う色なら
		if (board->Get_m_pieces_pState(aPos.x + dir[i].x, aPos.y + dir[i].y) == otherColor) {
			enclosedNum = 0;
			//調べる(同じ色を見つける、駒が置かれていないマスを見つける、調べる範囲を出るまでループ)
			for (int j = 2; (0 <= aPos.x + dir[i].x * j && aPos.x + dir[i].x * j < 8) && (0 <= aPos.y + dir[i].y * j && aPos.y + dir[i].y * j < 8); ++j) {
				if (board->Get_m_pieces_pState(aPos.x + dir[i].x * j, aPos.y + dir[i].y * j) == non) {
					break;
				}
				if (board->Get_m_pieces_pState(aPos.x + dir[i].x * j, aPos.y + dir[i].y * j) == turnColor) {
					enclosedNum = j - 1;
					break;
				}
			}
			//裏返し
			for (int j = 0; j < enclosedNum; ++j) {
				board->Set_m_pieces_pState(aPos.x + dir[i].x * (j + 1), aPos.y + dir[i].y * (j + 1), turnColor);
			}
			turnOverNum += enclosedNum;
		}
	}

	//1枚以上裏返したらtrueを返す
	if (turnOverNum) {
		return true;
	}
	else {
		board->Set_m_pieces_pState(aPos.x, aPos.y, non);//※１
		return false;
	}
}

//置けるか判定
bool Base::PutGudge(Vector2 searchPos, Board* board, bool turn) const {
	if (board->Get_m_pieces_pState(searchPos.x, searchPos.y) != non)return false;//既に駒が置かれていたら終了

	PlaceState turnColor = turn ? black : white;
	PlaceState otherColor = turn ? white : black;

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
	for (int i = 0; i < 8; ++i) {
		//隣が違う色なら
		if (board->Get_m_pieces_pState(searchPos.x + dir[i].x, searchPos.y + dir[i].y) == otherColor) {
			//調べる(同じ色を見つける、駒が置かれていないマスを見つける、調べる範囲を出るまでループ)
			for (int j = 2; (0 <= searchPos.x + dir[i].x * j && searchPos.x + dir[i].x * j < 8) && (0 <= searchPos.y + dir[i].y * j && searchPos.y + dir[i].y * j < 8); ++j) {
				if (board->Get_m_pieces_pState(searchPos.x + dir[i].x * j, searchPos.y + dir[i].y * j) == non) {
					break;
				}
				if (board->Get_m_pieces_pState(searchPos.x + dir[i].x * j, searchPos.y + dir[i].y * j) == turnColor) {
					return true;
					break;
				}
			}
		}
	}

	return false;
}