#include "Player.h"
#include "CPU.h"

enum Menu_Select {
	BackGame = 0,
	BackLevelSelect = 1,
	GameEnd = 2,
};

//��ʒ����ɕ\�����郁�b�Z�[�W
const char* messages[] = {
		"  �Q�[���X�^�[�g  ",
		"  CPU����s�ł�   ",
		" Player����s�ł� ",
		"�^�[�����΂��܂�",
		"    CPU�̏���!    ",
		"  Player�̏���!!  ",
		"     ��������     "
};

struct MessageManager
{
	const char* currentMes;
	const char* nextMes;
	bool messageDisplaying;
	LONGLONG startTime;

	void Set(int messageNum, int  nextMesNum = 99) {//�A���ŕ�����\������Ƃ��͈����Q��
		currentMes = messages[messageNum];
		if (nextMesNum != 99) {
			nextMes = messages[nextMesNum];
		}
		else {
			nextMes = 0;
		}
		messageDisplaying = true;
		startTime = GetNowHiPerformanceCount();
	}
	void Reset() {
		currentMes = 0;
		messageDisplaying = false;
		startTime = 0;
	}
	void MesChange() {
		//���̃��b�Z�[�W����������ؑ�
		if (nextMes) {
			currentMes = nextMes;
			nextMes = 0;
			startTime = GetNowHiPerformanceCount();
			return;
		}
		Reset();
	}
};

void MainGame(int level, GameState& gameState) {
	MessageManager message = { 0, 0, false, 0 };

	bool turn = 1;//�����P�A�����O(���͍�)
	int centerFontSize = CreateFontToHandle(NULL, 50, 1);//��ʒ����ɕ\������p�̃t�H���g���
	bool gameEnd = false;//�Q�[���I���p�̃t���O
	LONGLONG resultStartTime = 0;//���ʕ\���̊J�n����
	bool menuOrNot = false;//���j���[��ʂ��ǂ���( true : ���j���[��ʂ��J���Ă��� )
	//���j���[�Ɏg�p
	InputState inputState, prevInputState;
	inputState = prevInputState = NotPressed;
	Menu_Select menu_select = BackGame;

	int arrowSize = 20;//���̃T�C�Y
	Vector2 arrowPos = { 4, 3 };//���̏ꏊ

	//�Ղ̏�����
	Board board;
	board.Reset();

	//���Ԃ̓����_��
	bool tempturn = GetRand(1);
	Player player(tempturn, e_Player1);
	CPU cpu(!tempturn, e_CPU, level);
	Base* participants[2];//[0]�͔��A[1]�͍�������
	participants[player.Get_m_turn()] = &player;
	participants[cpu.Get_m_turn()] = &cpu;
	if (participants[1]->Get_m_name() == e_Player1) {
		message.Set(0, 2);
	}
	else {
		message.Set(0, 1);
	}

	//�ȉ����C���Q�[���̃��[�v
	while (ProcessMessage() == 0) {
		ClearDrawScreen();//��ʃN���A

		if (!message.messageDisplaying && !menuOrNot) {
			bool gameConti = false;//�ȉ�skip�Ŏg�p
			switch (participants[turn]->Update(&arrowPos, &board))
			{
			case inAction:
				//�����������[�v
				break;
			case turnChange:
				board.PieceNumUpdate();//��̖����X�V
				turn = !turn;//�^�[���ؑ�
				break;
			case skip:
				//���̐l���u���邩���`�F�b�N
				for (int y = 0; y < 8 && !gameConti; ++y) {
					for (int x = 0; x < 8 && !gameConti; ++x) {
						Vector2 temp = { x, y };
						//�P���ł��u������Q�[���p��
						if (participants[!turn]->PutGudge(temp, &board, participants[!turn]->Get_m_turn())) {
							gameConti = true;
						}
					}
				}
				//�Q�l�X�L�b�v������Q�[���I��
				if (!gameConti) {
					gameEnd = true;
					resultStartTime = GetNowHiPerformanceCount();
					message.messageDisplaying = true;
					break;
				}
				//�P�l�X�L�b�v�Ȃ�p��
				else {
					message.Set(3);
					turn = !turn;//�^�[���ؑ�
					break;
				}
			case error:
				//�����ɗ��Ƃ�
				gameState = e_GameEnd;
				return;
				break;
			}
		}

		//<�`��>
		board.Show();
		//�^�[��
		if (participants[1]->Get_m_name() == e_Player1) {//player�����Ȃ�
			DrawString(59, 14, turn == player.Get_m_turn() ? "Player(��)�̃^�[��" : " CPU(��)�̃^�[��", GetColor(255, 255, 255));
		}
		else {//player�����Ȃ�
			DrawString(59, 14, turn == player.Get_m_turn() ? "Player(��)�̃^�[��" : " CPU(��)�̃^�[��", GetColor(255, 255, 255));
		}
		//���
		Vector2 DrawPos = board.Get_m_pieces_pos(arrowPos.x, arrowPos.y);//����`�悷���̏ꏊ
		DrawPos.x += 12;//�P�}�X�̃T�C�Y��ύX����Ƃ��͂���������
		DrawPos.y -= 12;
		DrawTriangle(DrawPos.x, DrawPos.y - arrowSize, DrawPos.x, DrawPos.y, DrawPos.x + arrowSize, DrawPos.y, GetColor(255, 255, 0), TRUE);
		DrawTriangle(DrawPos.x + arrowSize / 4, DrawPos.y - arrowSize / 4 * 3, DrawPos.x + arrowSize * 1.5, DrawPos.y - arrowSize, DrawPos.x + arrowSize, DrawPos.y - arrowSize * 1.5, GetColor(255, 255, 0), TRUE);
		DrawTriangle(DrawPos.x + arrowSize / 4, DrawPos.y - arrowSize / 4 * 3, DrawPos.x + arrowSize / 4 * 3, DrawPos.y - arrowSize / 4, DrawPos.x + arrowSize * 1.5, DrawPos.y - arrowSize, GetColor(255, 255, 0), TRUE);
		//��ʒ����ɕ\�����郁�b�Z�[�W
		if (message.messageDisplaying) {
			//�Q�[���I����
			if (gameEnd) {
				//1�b��ɕ\��
				if (GetNowHiPerformanceCount() - resultStartTime >= 1000000) {
					DrawBox(80, 200, 559, 250, GetColor(0, 0, 0), TRUE);
					//��������
					if (board.Get_m_WhitePieceNum() > board.Get_m_BlackPieceNum()) {
						switch (participants[0]->Get_m_name()) {
						case e_CPU:
							DrawStringToHandle(113, 200, messages[4], GetColor(255, 255, 255), centerFontSize);
							break;
						case e_Player1:
							DrawStringToHandle(100, 200, messages[5], GetColor(255, 255, 255), centerFontSize);
							break;
						}
					}
					//��������
					else if (board.Get_m_WhitePieceNum() < board.Get_m_BlackPieceNum()) {
						switch (participants[1]->Get_m_name()) {
						case e_CPU:
							DrawStringToHandle(113, 200, messages[4], GetColor(255, 255, 255), centerFontSize);
							break;
						case e_Player1:
							DrawStringToHandle(100, 200, messages[5], GetColor(255, 255, 255), centerFontSize);
							break;
						}
					}
					//��������
					else {
						DrawStringToHandle(100, 200, messages[6], GetColor(255, 255, 255), centerFontSize);
					}
				}
				if (GetNowHiPerformanceCount() - resultStartTime >= 5000000) {
					break;
				}
			}
			//�Q�[���J�n���A�Q�[����
			else {
				DrawBox(80, 200, 559, 250, GetColor(0, 0, 0), TRUE);
				DrawStringToHandle(94, 200, message.currentMes, GetColor(255, 255, 255), centerFontSize);
				//2�b�ԕ\�����I�������
				if (GetNowHiPerformanceCount() - message.startTime >= 2000000) {
					message.MesChange();
				}
			}
		}
		
		//���j���[���J��
		if (inputState == NotPressed) {
			if (CheckHitKey(KEY_INPUT_ESCAPE) == 1) {
				inputState = PressEscape;
			}
		}
		else {
			if (CheckHitKey(KEY_INPUT_ESCAPE) == 0)inputState = NotPressed;
		}
		if (prevInputState == NotPressed) {
			if (inputState == PressEscape && !message.messageDisplaying) {
				//���j���[��ʂ��J���Ă���
				if (menuOrNot) {
					menuOrNot = false;
				}
				//���j���[��ʂ����Ă�����
				else {
					menuOrNot = true;
					menu_select = BackGame;
				}
			}
		}
		//���j���[���
		if (menuOrNot) {
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 192);//�I�Z����ʂ𔼓����ɂ���
			DrawBox(0, 0, 640, 480, GetColor(0, 0, 0), TRUE);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
			DrawString(199, 104, "�@�Q�[���ɖ߂�@", GetColor(255, 255, 255));
			DrawString(199, 224, "���x���I���֖߂�", GetColor(255, 255, 255));
			DrawString(199, 344, "�Q�[�����I������", GetColor(255, 255, 255));
			switch (menu_select)
			{
			case BackGame:
				DrawTriangle(170, 104, 170, 134, 192, 119, GetColor(255, 255, 0), TRUE);
				break;
			case BackLevelSelect:
				DrawTriangle(170, 224, 170, 254, 192, 239, GetColor(255, 255, 0), TRUE);
				break;
			case GameEnd:
				DrawTriangle(170, 344, 170, 374, 192, 359, GetColor(255, 255, 0), TRUE);
				break;
			}
			
			//��ɂP�̃L�[���͂����󂯕t���Ȃ��悤�ɂ���
			if (inputState == NotPressed) {
				if (CheckHitKey(KEY_INPUT_DOWN) == 1) {
					inputState = PressDownAr;
				}
				else if (CheckHitKey(KEY_INPUT_UP) == 1) {
					inputState = PressUpAr;
				}
				else if (CheckHitKey(KEY_INPUT_RETURN) == 1) {
					inputState = PressReturn;
				}
			}
			else {
				switch (inputState) {
				case PressDownAr: if (CheckHitKey(KEY_INPUT_DOWN) == 0)inputState = NotPressed; break;
				case PressUpAr: if (CheckHitKey(KEY_INPUT_UP) == 0)inputState = NotPressed; break;
				case PressReturn: if (CheckHitKey(KEY_INPUT_RETURN) == 0)inputState = NotPressed; break;
				}
			}
			//��������Ԃ���{�^�������߂ĉ�������
			if (prevInputState == NotPressed) {
				if (inputState != NotPressed && inputState != PressReturn) {
					switch (inputState)
					{
					case PressDownAr:
						if (menu_select + 1 > 2) {
							menu_select = BackGame;//0
						}
						else {
							menu_select = static_cast<Menu_Select>(menu_select + 1);
						}
						break;
					case PressUpAr:
						if (menu_select - 1 < 0) {
							menu_select = GameEnd;//2
						}
						else {
							menu_select = static_cast<Menu_Select>(menu_select - 1);
						}
						break;
					}
				}
				else if (inputState == PressReturn) {
					switch (menu_select)
					{
					case BackGame:
						menuOrNot = false;
						break;
					case BackLevelSelect:
						gameState = e_LevelSelect;
						return;
						break;
					case GameEnd:
						gameState = e_GameEnd;
						return;
						break;
					default:
						break;
					}
				}
			}
		}
		prevInputState = inputState;

		ScreenFlip();//�\��
	}

	gameState = e_GameEnd;
}