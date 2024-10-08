#pragma once
#include "Board.h"
#include "Share.h"

//PlayerとCPUのベース

enum Name {
	e_CPU,
	e_Player1,
};

enum Action
{
	inAction,
	turnChange,
	skip,
	error,
};

class Base {
public:
	virtual ~Base(){}

	virtual Action Update(Vector2* aPos, Board* board) = 0;

	bool TurnOver(Vector2 aPos, Board* board) const;
	bool PutGudge(Vector2 searchPos, Board* board, bool turn) const;

	bool Get_m_turn() const {
		return m_turn;
	}
	Name Get_m_name() const {
		return m_name;
	}

protected:
	Name m_name;

	bool m_turn;////黒が１、白が０(先手は黒)
	bool m_endPutGudge;//行動１回につきPutGudgeが１回だけ通るようにするため
};