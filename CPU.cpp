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
	//�������Ȃ�
}

Action CPU::Update(Vector2* aPos, Board* board) {
	//�s���̓x�P�񂾂��ʂ�
	if (!m_endPutGudge) {
		bool skip = true;
		//���u����}�X�����邩�ǂ������ׂ�
		for (int y = 0; y < 8 && skip; ++y) {
			for (int x = 0; x < 8 && skip; ++x) {
				//�u����ꏊ���P�ł��������璲�ׂ�̂��I��
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
		//�u���ꏊ���𒲂ׂ�
		int* m_candidates = new int[1];//�u���ꏊ���
		m_candidates[0] = 99;
		int candidatesNum = 0;
		for (int i = 0; i < 8; ++i) {
			for (int j = 0; j < 8; ++j) {
				Vector2 temp = { j, i };
				if (PutGudge(temp, board)) {
					++candidatesNum;
					//���I�z��̃T�C�Y���L���Ēu���ꏊ����ۑ����Ă���
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
			//�G���[
			delete[] m_candidates;
			m_candidates = 0;
			return error;
		}
		//�������烉���_���Œu��
		int tempNum = m_candidates[GetRand(candidatesNum - 1)];
		m_putPos = { tempNum % 8, tempNum / 8 };
		m_arrowMoving = true;
		m_moveStartTime = GetNowHiPerformanceCount();
		//�㏈��
		delete[] m_candidates;
		m_candidates = 0;
	}
	else 
	{
		//������CPU�̖��ړ�������
		if (GetNowHiPerformanceCount() - m_moveStartTime > 200000) {//�P�}�X��0.2�b�ňړ�
			//x���ړ�
			if (aPos->x < m_putPos.x) {
				++aPos->x;
			}
			else if (aPos->x > m_putPos.x) {
				--aPos->x;
			}
			//y���ړ�
			else {
				if (aPos->y < m_putPos.y) {
					++aPos->y;
				}
				else if (aPos->y > m_putPos.y) {
					--aPos->y;
				}
				//�ړI�n�܂Œ���������u��
				else {
					if (!TurnOver(m_putPos, board)) {
						//�G���[
						return error;
					}

					m_arrowMoving = false;
					m_endPutGudge = false;
					return turnChange;
				}
			}
			m_moveStartTime = GetNowHiPerformanceCount();//�X�V
		}
	}

	return inAction;
}