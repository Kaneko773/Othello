#include "CPU.h"

struct NextMoveCandi
{
	int putPos;//�u���ꏊ
	double winRate;//����
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

int CPU::Level1(Board* board) {
	//�u���ꏊ���𒲂ׂ�
	int* candidates = new int[1];//�u���ꏊ���
	candidates[0] = 99;
	int candidatesNum = 0;
	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j) {
			Vector2 temp = { j, i };
			if (PutGudge(temp, board, Get_m_turn())) {
				++candidatesNum;
				//���I�z��̃T�C�Y���L���Ēu���ꏊ����ۑ����Ă���
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
		//�G���[
		delete[] candidates;
		candidates = 0;
		return error;
	}
	//�������烉���_���Œu��
	int putPos = candidates[GetRand(candidatesNum - 1)];
	//�㏈��
	delete[] candidates;
	candidates = 0;

	return putPos;
}

int CPU::Level2(Board* board) {
	//�u���ꏊ���𒲂ׂ�
	NextMoveCandi* candidates = new NextMoveCandi[1];//�u���ꏊ���
	candidates[0].putPos = 99;
	candidates[0].winRate = 0;
	int candidatesNum = 0;
	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j) {
			Vector2 temp = { j, i };
			if (PutGudge(temp, board, Get_m_turn())) {
				++candidatesNum;
				//���I�z��̃T�C�Y���L���Ēu���ꏊ����ۑ����Ă���
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
		//�u���ꏊ���P�ӏ��̎��͂��̂܂ܒu���̂ŏ�������
	}
	else {
		for (int i = 0; i < candidatesNum; ++i) {
			Vector2 tempPos = { candidates[i].putPos % 8, candidates[i].putPos / 8 };
			int winNum = 0;
			int routeNum = 0;
			Search(*board, tempPos, 1/*1��ڂ���X�^�[�g*/, winNum, routeNum, Get_m_turn());
			candidates[i].winRate = (double)winNum / (double)routeNum;//�u����ꏊ�����邩���ɔ��肵�Ă�̂Ł@routeNum = 0�@�̏����͖���
		}

		//�ł��������������擪��
		for (int i = 1; i < candidatesNum; ++i) {
			//���͍��オ�D��(�ԍ��̏��������ɗD��)
			if (candidates[0].winRate < candidates[i].winRate) {
				NextMoveCandi temp = candidates[i];
				candidates[i] = candidates[0];
				candidates[0] = temp;
			}
		}
	}

	int putPos = candidates[0].putPos;
	//�㏈��
	delete[] candidates;
	candidates = 0;

	return putPos;
}

//Level�Q�Ŏg�p
void CPU::Search(Board board, Vector2 pos, int count, int& winNum, int& routeNum, bool turn, bool skipOnce) {
	ProcessMessage();
	if (pos.x != 99) {
		//�n���ꂽ�ꏊ�ɋ��u���ė��Ԃ�
		PlaceState putColor = turn ? black : white;
		PlaceState otherColor = turn ? white : black;
		board.Set_m_pieces_pState(pos.x, pos.y, putColor);

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
		int enclosedNum = 0;//�e�����̗��Ԃ�����
		for (int i = 0; i < 8; ++i) {
			//�ׂ��Ⴄ�F�Ȃ�
			if (board.Get_m_pieces_pState(pos.x + dir[i].x, pos.y + dir[i].y) == otherColor) {
				enclosedNum = 0;
				//���ׂ�(�����F��������A��u����Ă��Ȃ��}�X��������A���ׂ�͈͂��o��܂Ń��[�v)
				for (int j = 2; (0 <= pos.x + dir[i].x * j && pos.x + dir[i].x * j < 8) && (0 <= pos.y + dir[i].y * j && pos.y + dir[i].y * j < 8); ++j) {
					if (board.Get_m_pieces_pState(pos.x + dir[i].x * j, pos.y + dir[i].y * j) == non) {
						break;
					}
					if (board.Get_m_pieces_pState(pos.x + dir[i].x * j, pos.y + dir[i].y * j) == putColor) {
						enclosedNum = j - 1;
						break;
					}
				}
				//���Ԃ�
				for (int j = 0; j < enclosedNum; ++j) {
					board.Set_m_pieces_pState(pos.x + dir[i].x * (j + 1), pos.y + dir[i].y * (j + 1), putColor);
				}
			}
		}
		board.PieceNumUpdate();//��̖����X�V
	}


	//�u���ꏊ���𒲂ׂ�
	// �����L�@���ׂ��񐔂ɂ���ā@�J��Ԃ��@���@�����𒲂ׂď��s��Ԃ�
	// ����△�@�����𒲂ׂď��s��Ԃ�
	
	//�u���ꏊ���𒲂ׂ�
	NextMoveCandi* candidates = new NextMoveCandi[1];//�u���ꏊ���
	candidates[0].putPos = 99;
	candidates[0].winRate = 0;
	int candidatesNum = 0;
	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j) {
			Vector2 temp = { j, i };
			if (PutGudge(temp, &board, turn)) {
				++candidatesNum;
				//���I�z��̃T�C�Y���L���Ēu���ꏊ����ۑ����Ă���
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
	//��△��
	if (candidates[0].putPos == 99) {
		//�X�L�b�v�̏���
		if (skipOnce) {
			routeNum += 1;//�T���I�������̂�
			//�Q�l�Ƃ��X�L�b�v�Ȃ̂ŒT���I��
			if (board.Get_m_BlackPieceNum() == board.Get_m_WhitePieceNum()) {
				//�������Ȃ�
			}
			else if (board.Get_m_BlackPieceNum() > board.Get_m_WhitePieceNum()) {
				if (this->Get_m_turn() == 1/*��*/) {
					winNum += 1;
				}
			}
			else {
				if (this->Get_m_turn() == 0/*��*/) {
					winNum += 1;
				}
			}
		}
		else {
			Vector2 temp = { 99, 0 };//�X�L�b�v���͑��݂��Ȃ��ꏊ��n��
			Search(board, temp, count, winNum, routeNum, !turn, true);
		}
	}
	//���L��
	else {
		//5��ڂȂ�I��
		if (count >= 5) {
			routeNum += 1;//�T���I�������̂�
			if (board.Get_m_BlackPieceNum() == board.Get_m_WhitePieceNum()) {
				//�������Ȃ�
			}
			else if (board.Get_m_BlackPieceNum() > board.Get_m_WhitePieceNum()) {
				if (this->Get_m_turn() == 1/*��*/) {
					winNum += 1;
				}
			}
			else {
				if (this->Get_m_turn() == 0/*��*/) {
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

	//�㏈��
	delete[] candidates;
	candidates = 0;
}