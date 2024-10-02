#include "Share.h"

void LevelSelect(int& level, InputState& inputState, InputState& prevInputState, GameState& gameState);
void MainGame(int level, GameState& gameState);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	// �E�C���h�E���[�h�ŋN��
	ChangeWindowMode(TRUE);
	// ScreenFlip �����s���Ă����������M����҂��Ȃ�
	SetWaitVSyncFlag(FALSE);
	// �c�w���C�u�����̏�����
	if (DxLib_Init() < 0) return -1;
	// �`���𗠉�ʂɂ���
	SetDrawScreen(DX_SCREEN_BACK);

	SetFontSize(30);

	GameState gameState = e_LevelSelect;//�Q�[���̏�ԁA�ŏ��̓��x���I��
	int CPULevel = 1;//CPU�̃��x��
	InputState inputState, prevInputState;
	inputState = prevInputState = NotPressed;

	bool end = false;//�Q�[���I������

	while (ProcessMessage() == 0) {
		switch (gameState)
		{
		case e_LevelSelect:
			LevelSelect(CPULevel, inputState, prevInputState, gameState);
			break;
		case e_Loading:
			ClearDrawScreen();//��ʃN���A
			DrawString(192, 224, "�ǂݍ��ݒ��ł�...", GetColor(255, 255, 255));
			ScreenFlip();//�\��
			WaitTimer(2000);
			gameState = e_MainGame;
			break;
		case e_MainGame:
			MainGame(CPULevel, gameState);
			break;
		case e_GameEnd:
			ClearDrawScreen();//��ʃN���A
			DrawString(132, 224, "�Q�[�����I�����Ă��܂�...", GetColor(255, 255, 255));
			ScreenFlip();//�\��
			WaitTimer(2000);
			end = true;
			break;
		default:

			break;
		}
		if (end)break;
	}

	DxLib_End();
	return 0;
}