#pragma once

#ifndef CMDMODE_GAME
#define CMDMODE_GAME


#include "BitBoard.hpp"
#include "Save.hpp"
#include "Game.hpp"
#include "NewAI.hpp"
#include <deque>

#define WIN 1
#define LOSE -1

class CmdModeGame
{
public:
	CmdModeGame(const bool print, const unsigned char depth, const unsigned char p1dp, const unsigned char p1dk, const unsigned char p1ap, const unsigned char p1ak,
		const unsigned char p2dp, const unsigned char p2dk, const unsigned char p2ap, const unsigned char p2ak);
	~CmdModeGame();

	int loop();

private:
	typedef std::vector<Cell> Board;
	typedef std::deque<Save> LastMoves;
	Game mGame;
	NewAI mAi1;
	NewAI mAi2;
	Board mBoard;
	LastMoves mLastMoves;
	bool mPrint;

	bool isStalemate() const;
	void addSave(const Save& save);
};

inline int clamp(int x, int min, int max)
{
	if (x > max)
		x = max;
	
	if (x < min)
		x = min;

	return x;
}

#endif // !CMDMODE_GAME