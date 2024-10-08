#pragma once
#include "Share.h"

struct PiecesInfo {
	Vector2 pos;
	PlaceState pState;
};

class Board
{
public:

	Board();
	~Board();
	void Reset();

	PlaceState Get_m_pieces_pState(int x, int y) const {
		return m_pieces[y][x].pState;
	}
	void Set_m_pieces_pState(int x, int y, PlaceState state) {
		m_pieces[y][x].pState = state;
	}
	Vector2 Get_m_pieces_pos(int x, int y) const {
		return m_pieces[y][x].pos;
	}
	int Get_m_WhitePieceNum() const {
		return m_WhitePieceNum;
	}
	int Get_m_BlackPieceNum() const {
		return m_BlackPieceNum;
	}

	void PieceNumUpdate();//各駒の数を数える
	void Show() const;

private:

	int m_WhitePieceNum, m_BlackPieceNum;//各色の枚数
	PiecesInfo m_pieces[8][8];//駒
};

