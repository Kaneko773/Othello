#include "Player.h"
#include "CPU.h"

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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	// �E�C���h�E���[�h�ŋN��
	ChangeWindowMode(TRUE);
	// ScreenFlip �����s���Ă����������M����҂��Ȃ�
	SetWaitVSyncFlag(FALSE);
	// �c�w���C�u�����̏�����
	if (DxLib_Init() < 0) return -1;
	// �`���𗠉�ʂɂ���
	SetDrawScreen(DX_SCREEN_BACK);


	
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
	MessageManager message = { 0, 0, false, 0 };

	bool turn = 1;//�����P�A�����O(���͍�)
	SetFontSize(30);//��ʏ㕔�̕����̃T�C�Y
	int centerFontSize = CreateFontToHandle(NULL, 50, 1);//��ʒ����ɕ\������p�̃t�H���g���
	bool gameEnd = false;//�Q�[���I���p�̃t���O
	LONGLONG resultStartTime = 0;//���ʕ\���̊J�n����

	int arrowSize = 20;//���̃T�C�Y
	Vector2 arrowPos = { 4, 3 };//���̏ꏊ

	//�Ղ̏�����
	Board board;
	board.Reset();

	//���Ԃ̓����_��
	bool tempturn = GetRand(1);
	Player player(tempturn, e_Player1);
	CPU cpu(!tempturn, e_CPU);
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

		if (!message.messageDisplaying) {
			bool end = false;//�ȉ�error�Ŏg�p
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
							if (participants[!turn]->PutGudge(temp, &board)) {
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
					end = true;
					break;
			}
			if (end)break;
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

		//�f�o�b�O
		if (CheckHitKey(KEY_INPUT_ESCAPE)) {
			break;
		}

		ScreenFlip();//�\��
	}

	DxLib_End();
	return 0;
}

