#pragma once
#include "Board.h"
#include "Share.h"

//Player‚ÆCPU‚Ìƒx[ƒX

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
	bool PutGudge(Vector2 searchPos, Board* board) const;

	bool Get_m_turn() const {
		return m_turn;
	}
	Name Get_m_name() const {
		return m_name;
	}

protected:
	Name m_name;

	bool m_turn;////•‚ª‚PA”’‚ª‚O(æè‚Í•)
	bool m_endPutGudge;//s“®‚P‰ñ‚É‚Â‚«PutGudge‚ª‚P‰ñ‚¾‚¯’Ê‚é‚æ‚¤‚É‚·‚é‚½‚ß
};