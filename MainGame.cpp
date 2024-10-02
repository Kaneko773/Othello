#include "Player.h"
#include "CPU.h"

enum Menu_Select {
	BackGame = 0,
	BackLevelSelect = 1,
	GameEnd = 2,
};

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

void MainGame(int level, GameState& gameState) {
	MessageManager message = { 0, 0, false, 0 };

	bool turn = 1;//黒が１、白が０(先手は黒)
	int centerFontSize = CreateFontToHandle(NULL, 50, 1);//画面中央に表示する用のフォント情報
	bool gameEnd = false;//ゲーム終了用のフラグ
	LONGLONG resultStartTime = 0;//結果表示の開始時間
	bool menuOrNot = false;//メニュー画面かどうか( true : メニュー画面が開いてる状態 )
	//メニューに使用
	InputState inputState, prevInputState;
	inputState = prevInputState = NotPressed;
	Menu_Select menu_select = BackGame;

	int arrowSize = 20;//矢印のサイズ
	Vector2 arrowPos = { 4, 3 };//矢印の場所

	//盤の初期化
	Board board;
	board.Reset();

	//順番はランダム
	bool tempturn = GetRand(1);
	Player player(tempturn, e_Player1);
	CPU cpu(!tempturn, e_CPU, level);
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

		if (!message.messageDisplaying && !menuOrNot) {
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
						if (participants[!turn]->PutGudge(temp, &board, participants[!turn]->Get_m_turn())) {
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
				gameState = e_GameEnd;
				return;
				break;
			}
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
		
		//メニューを開く
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
				//メニュー画面が開いてたら
				if (menuOrNot) {
					menuOrNot = false;
				}
				//メニュー画面が閉じていたら
				else {
					menuOrNot = true;
					menu_select = BackGame;
				}
			}
		}
		//メニュー画面
		if (menuOrNot) {
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 192);//オセロ画面を半透明にする
			DrawBox(0, 0, 640, 480, GetColor(0, 0, 0), TRUE);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
			DrawString(199, 104, "　ゲームに戻る　", GetColor(255, 255, 255));
			DrawString(199, 224, "レベル選択へ戻る", GetColor(255, 255, 255));
			DrawString(199, 344, "ゲームを終了する", GetColor(255, 255, 255));
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
			
			//常に１つのキー入力しか受け付けないようにする
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
			//離した状態からボタンを初めて押した時
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

		ScreenFlip();//表示
	}

	gameState = e_GameEnd;
}