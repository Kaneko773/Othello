#include "Player.h"
#include "CPU.h"

//画面中央に表示するメッセージ
const char* messages[] = {
		"  ゲームスタート  ",
		"  CPUが先行です   ",
		" Playerが先行です ",
		"ターンを飛ばします",
		"    CPUの勝利!    ",
		"  Playerの勝利!!  ",
		"     引き分け     "
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	// ウインドウモードで起動
	ChangeWindowMode(TRUE);
	// ScreenFlip を実行しても垂直同期信号を待たない
	SetWaitVSyncFlag(FALSE);
	// ＤＸライブラリの初期化
	if (DxLib_Init() < 0) return -1;
	// 描画先を裏画面にする
	SetDrawScreen(DX_SCREEN_BACK);


	
	struct MessageManager
	{
		const char* currentMes;
		const char* nextMes;
		bool messageDisplaying;
		LONGLONG startTime;

		void Set(int messageNum, int  nextMesNum = 99) {//連続で文字を表示するときは引数２つ
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
			//次のメッセージがあったら切替
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

	bool turn = 1;//黒が１、白が０(先手は黒)
	SetFontSize(30);//画面上部の文字のサイズ
	int centerFontSize = CreateFontToHandle(NULL, 50, 1);//画面中央に表示する用のフォント情報
	bool gameEnd = false;//ゲーム終了用のフラグ
	LONGLONG resultStartTime = 0;//結果表示の開始時間

	int arrowSize = 20;//矢印のサイズ
	Vector2 arrowPos = { 4, 3 };//矢印の場所

	//盤の初期化
	Board board;
	board.Reset();

	//順番はランダム
	bool tempturn = GetRand(1);
	Player player(tempturn, e_Player1);
	CPU cpu(!tempturn, e_CPU);
	Base* participants[2];//[0]は白、[1]は黒が入る
	participants[player.Get_m_turn()] = &player;
	participants[cpu.Get_m_turn()] = &cpu;
	if (participants[1]->Get_m_name() == e_Player1) {
		message.Set(0, 2);
	}
	else {
		message.Set(0, 1);
	}
		
	//以下メインゲームのループ
	while (ProcessMessage() == 0) {
		ClearDrawScreen();//画面クリア

		if (!message.messageDisplaying) {
			bool end = false;//以下errorで使用
			bool gameConti = false;//以下skipで使用
			switch (participants[turn]->Update(&arrowPos, &board))
			{
				case inAction: 
					//何もせずループ
					break;
				case turnChange:
					board.PieceNumUpdate();//駒の枚数更新
					turn = !turn;//ターン切替
					break;
				case skip:
					//次の人が置けるかをチェック
					for (int y = 0; y < 8 && !gameConti; ++y) {
						for (int x = 0; x < 8 && !gameConti; ++x) {
							Vector2 temp = { x, y };
							//１枚でも置けたらゲーム継続
							if (participants[!turn]->PutGudge(temp, &board)) {
								gameConti = true;
							}
						}
					}
					//２人スキップしたらゲーム終了
					if (!gameConti) {
						gameEnd = true;
						resultStartTime = GetNowHiPerformanceCount();
						message.messageDisplaying = true;
						break;
					}
					//１人スキップなら継続
					else {
						message.Set(3);
						turn = !turn;//ターン切替
						break;
					}
				case error:
					//即座に落とす
					end = true;
					break;
			}
			if (end)break;
		}

		//<描画>
		board.Show();
		//ターン
		if (participants[1]->Get_m_name() == e_Player1) {//playerが黒なら
			DrawString(59, 14, turn == player.Get_m_turn() ? "Player(○)のターン" : " CPU(●)のターン", GetColor(255, 255, 255));
		}
		else {//playerが白なら
			DrawString(59, 14, turn == player.Get_m_turn() ? "Player(●)のターン" : " CPU(○)のターン", GetColor(255, 255, 255));
		}
		//矢印
		Vector2 DrawPos = board.Get_m_pieces_pos(arrowPos.x, arrowPos.y);//矢印を描画する基準の場所
		DrawPos.x += 12;//１マスのサイズを変更するときはここも調整
		DrawPos.y -= 12;
		DrawTriangle(DrawPos.x, DrawPos.y - arrowSize, DrawPos.x, DrawPos.y, DrawPos.x + arrowSize, DrawPos.y, GetColor(255, 255, 0), TRUE);
		DrawTriangle(DrawPos.x + arrowSize / 4, DrawPos.y - arrowSize / 4 * 3, DrawPos.x + arrowSize * 1.5, DrawPos.y - arrowSize, DrawPos.x + arrowSize, DrawPos.y - arrowSize * 1.5, GetColor(255, 255, 0), TRUE);
		DrawTriangle(DrawPos.x + arrowSize / 4, DrawPos.y - arrowSize / 4 * 3, DrawPos.x + arrowSize / 4 * 3, DrawPos.y - arrowSize / 4, DrawPos.x + arrowSize * 1.5, DrawPos.y - arrowSize, GetColor(255, 255, 0), TRUE);
		//画面中央に表示するメッセージ
		if (message.messageDisplaying) {
			//ゲーム終了時
			if (gameEnd) {
				//1秒後に表示
				if (GetNowHiPerformanceCount() - resultStartTime >= 1000000) {
					DrawBox(80, 200, 559, 250, GetColor(0, 0, 0), TRUE);
					//白が勝ち
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
					//黒が勝ち
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
					//引き分け
					else {
						DrawStringToHandle(100, 200, messages[6], GetColor(255, 255, 255), centerFontSize);
					}
				}
				if (GetNowHiPerformanceCount() - resultStartTime >= 5000000) {
					break;
				}
			}
			//ゲーム開始時、ゲーム中
			else {
				DrawBox(80, 200, 559, 250, GetColor(0, 0, 0), TRUE);
				DrawStringToHandle(94, 200, message.currentMes, GetColor(255, 255, 255), centerFontSize);
				//2秒間表示し終わったら
				if (GetNowHiPerformanceCount() - message.startTime >= 2000000) {
					message.MesChange();
				}
			}
		}

		//デバッグ
		if (CheckHitKey(KEY_INPUT_ESCAPE)) {
			break;
		}

		ScreenFlip();//表示
	}

	DxLib_End();
	return 0;
}

