#include "Base.h"

//���Ԃ�����(�P���ł����Ԃ�����true��Ԃ�)
bool Base::TurnOver(Vector2 aPos, Board* board) const {
	if (board->Get_m_pieces_pState(aPos.x, aPos.y) != non)return false;//���ɋ�u����Ă�����I��

	PlaceState turnColor = m_turn ? black : white;
	PlaceState otherColor = m_turn ? white : black;

	board->Set_m_pieces_pState(aPos.x, aPos.y, turnColor);//����1�������Ԃ��Ȃ��������ŏ������P

	Vector2 dir[8] = {
			{1, 0},//�E
			{1, 1},//�E��
			{0, 1},//��
			{-1, 1},//����
			{-1, 0},//��
			{-1, -1},//����
			{0, -1},//��
			{1, -1}//�E��
	};
	//�E���玞�v����Œ��ׂ�
	int turnOverNum = 0;//���Ԃ�������
	int enclosedNum = 0;//�e�����̗��Ԃ�����
	for (int i = 0; i < 8; ++i) {
		//�ׂ��Ⴄ�F�Ȃ�
		if (board->Get_m_pieces_pState(aPos.x + dir[i].x, aPos.y + dir[i].y) == otherColor) {
			enclosedNum = 0;
			//���ׂ�(�����F��������A��u����Ă��Ȃ��}�X��������A���ׂ�͈͂��o��܂Ń��[�v)
			for (int j = 2; (0 <= aPos.x + dir[i].x * j && aPos.x + dir[i].x * j < 8) && (0 <= aPos.y + dir[i].y * j && aPos.y + dir[i].y * j < 8); ++j) {
				if (board->Get_m_pieces_pState(aPos.x + dir[i].x * j, aPos.y + dir[i].y * j) == non) {
					break;
				}
				if (board->Get_m_pieces_pState(aPos.x + dir[i].x * j, aPos.y + dir[i].y * j) == turnColor) {
					enclosedNum = j - 1;
					break;
				}
			}
			//���Ԃ�
			for (int j = 0; j < enclosedNum; ++j) {
				board->Set_m_pieces_pState(aPos.x + dir[i].x * (j + 1), aPos.y + dir[i].y * (j + 1), turnColor);
			}
			turnOverNum += enclosedNum;
		}
	}

	//1���ȏ㗠�Ԃ�����true��Ԃ�
	if (turnOverNum) {
		return true;
	}
	else {
		board->Set_m_pieces_pState(aPos.x, aPos.y, non);//���P
		return false;
	}
}

//�u���邩����
bool Base::PutGudge(Vector2 searchPos, Board* board, bool turn) const {
	if (board->Get_m_pieces_pState(searchPos.x, searchPos.y) != non)return false;//���ɋ�u����Ă�����I��

	PlaceState turnColor = turn ? black : white;
	PlaceState otherColor = turn ? white : black;

	Vector2 dir[8] = {
			{1, 0},//�E
			{1, 1},//�E��
			{0, 1},//��
			{-1, 1},//����
			{-1, 0},//��
			{-1, -1},//����
			{0, -1},//��
			{1, -1}//�E��
	};

	//�E���玞�v����Œ��ׂ�
	for (int i = 0; i < 8; ++i) {
		//�ׂ��Ⴄ�F�Ȃ�
		if (board->Get_m_pieces_pState(searchPos.x + dir[i].x, searchPos.y + dir[i].y) == otherColor) {
			//���ׂ�(�����F��������A��u����Ă��Ȃ��}�X��������A���ׂ�͈͂��o��܂Ń��[�v)
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