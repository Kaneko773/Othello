#include "Adjustment.h"
#include "Board.h"


Board::Board() {
	int tempCalc = oneSquareSize + 1;
	//基準になるため先に代入
	m_pieces[0][0].pos = { boardLeft + tempCalc / 2, boardTop + tempCalc / 2 };
	//駒情報の初期化
	for (int y = 0; y < 8; ++y) {
		for (int x = 0; x < 8; ++x) {
			Vector2 temp = { m_pieces[0][0].pos.x + tempCalc * x, m_pieces[0][0].pos.y + tempCalc * y };
			m_pieces[y][x].pos = temp;
			m_pieces[y][x].pState = non;
		}
	}
	m_WhitePieceNum = m_BlackPieceNum = 0;
}

Board::~Board() {
	//何もしない
}

//ゲーム開始時に呼ぶ
void Board::Reset() {
	for (int y = 0; y < 8; ++y) {
		for (int x = 0; x < 8; ++x) {
			m_pieces[y][x].pState = non;
		}
	}
	m_pieces[3][4].pState = m_pieces[4][3].pState = black;
	m_pieces[3][3].pState = m_pieces[4][4].pState = white;
	m_WhitePieceNum = m_BlackPieceNum = 2;
}

//各駒の枚数更新
void Board::PieceNumUpdate() {
	m_WhitePieceNum = m_BlackPieceNum = 0;
	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j) {
			switch (m_pieces[i][j].pState)
			{
			case white: ++m_WhitePieceNum; break;
			case black: ++m_BlackPieceNum; break;
			}
		}
	}
}

void Board::Show() const{
	int tempCalc = oneSquareSize + 1;
	//各枚数
	DrawFormatString(380, 15, GetColor(255, 255, 255), "○×%02d ●×%02d", m_BlackPieceNum, m_WhitePieceNum);
	//盤
	//縦
	for (int i = 0; i < 9; ++i) {
		DrawLine(boardLeft + tempCalc * i, boardTop, boardLeft + tempCalc * i, boardTop + tempCalc * 8 + 1, GetColor(255, 255, 255));
	}
	//横
	for (int i = 0; i < 9; ++i) {
		DrawLine(boardLeft, boardTop + tempCalc * i, boardLeft + tempCalc * 8 + 1, boardTop + tempCalc * i, GetColor(255, 255, 255));
	}
	//駒
	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j) {
			if (m_pieces[i][j].pState != non) {
				DrawCircle(m_pieces[i][j].pos.x, m_pieces[i][j].pos.y, pieceR, GetColor(255, 255, 255), m_pieces[i][j].pState == white ? TRUE : FALSE);
			}
		}
	}
}
