#include "Adjustment.h"
#include "Board.h"


Board::Board() {
	int tempCalc = oneSquareSize + 1;
	//��ɂȂ邽�ߐ�ɑ��
	m_pieces[0][0].pos = { boardLeft + tempCalc / 2, boardTop + tempCalc / 2 };
	//����̏�����
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
	//�������Ȃ�
}

//�Q�[���J�n���ɌĂ�
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

//�e��̖����X�V
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
	//�e����
	DrawFormatString(380, 15, GetColor(255, 255, 255), "���~%02d ���~%02d", m_BlackPieceNum, m_WhitePieceNum);
	//��
	//�c
	for (int i = 0; i < 9; ++i) {
		DrawLine(boardLeft + tempCalc * i, boardTop, boardLeft + tempCalc * i, boardTop + tempCalc * 8 + 1, GetColor(255, 255, 255));
	}
	//��
	for (int i = 0; i < 9; ++i) {
		DrawLine(boardLeft, boardTop + tempCalc * i, boardLeft + tempCalc * 8 + 1, boardTop + tempCalc * i, GetColor(255, 255, 255));
	}
	//��
	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j) {
			if (m_pieces[i][j].pState != non) {
				DrawCircle(m_pieces[i][j].pos.x, m_pieces[i][j].pos.y, pieceR, GetColor(255, 255, 255), m_pieces[i][j].pState == white ? TRUE : FALSE);
			}
		}
	}
}
