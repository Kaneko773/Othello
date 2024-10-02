#include "Share.h"

void LevelSelect(int& level, InputState& inputState, InputState& prevInputState, GameState& gameState);
void MainGame(int level, GameState& gameState);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	// ウインドウモードで起動
	ChangeWindowMode(TRUE);
	// ScreenFlip を実行しても垂直同期信号を待たない
	SetWaitVSyncFlag(FALSE);
	// ＤＸライブラリの初期化
	if (DxLib_Init() < 0) return -1;
	// 描画先を裏画面にする
	SetDrawScreen(DX_SCREEN_BACK);

	SetFontSize(30);

	GameState gameState = e_LevelSelect;//ゲームの状態、最初はレベル選択
	int CPULevel = 1;//CPUのレベル
	InputState inputState, prevInputState;
	inputState = prevInputState = NotPressed;

	bool end = false;//ゲーム終了判定

	while (ProcessMessage() == 0) {
		switch (gameState)
		{
		case e_LevelSelect:
			LevelSelect(CPULevel, inputState, prevInputState, gameState);
			break;
		case e_Loading:
			ClearDrawScreen();//画面クリア
			DrawString(192, 224, "読み込み中です...", GetColor(255, 255, 255));
			ScreenFlip();//表示
			WaitTimer(2000);
			gameState = e_MainGame;
			break;
		case e_MainGame:
			MainGame(CPULevel, gameState);
			break;
		case e_GameEnd:
			ClearDrawScreen();//画面クリア
			DrawString(132, 224, "ゲームを終了しています...", GetColor(255, 255, 255));
			ScreenFlip();//表示
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