#pragma once
#ifndef GAME_HPP_
#define GAME_HPP_

#include "BitBoard.hpp"
#include "Save.hpp"

#include <map>
#include <string>
#include <vector>
#include <utility>

class SimpleAI;

class Game {
public:
	friend class SimpleAI;
	friend class NewAI;
	friend class SFMLGame;

	/* Constructor */
	Game(const bool debug, const bool interact);
	/* Constructor from memory */
	Game(const Save& record, const bool debug, const bool interact);
	/* Destructor */
	virtual ~Game();

	/* return save game */
	Save getSave();
	/* Print game */
	void print() const;

	void print(const Save& save) const;
	std::vector<Cell> toArr(const Save& save) const;

	/* Movement */
	MoveCode move(const Move& move)
	{
		if (move.jump)
			return jump(move);
		else
			return makeMove(move);
	}

	/* restore game to save */
	void restoreToSave(const Save& record);
	/* Receive input for CLI */
	MoveCode receiveInput();

	/* Get p1 score */
	unsigned getP1score() const
	{
		return Bit::bitCount(mBP & ~mK) + 2 * Bit::bitCount(mBP & mK);
	}
	/* Get p2 score */
	unsigned getP2score() const
	{
		return Bit::bitCount(mWP & ~mK) + 2 * Bit::bitCount(mWP & mK);
	}

	int grade() const;

	unsigned p1NumPieces() const
	{
		return Bit::bitCount(mBP);
	}
	unsigned p2NumPieces() const
	{
		return Bit::bitCount(mWP);
	}

	bool isLive() const
	{

		return (getMovers() || getJumpers());
	}

	std::vector<Cell> toArr() const;

private:
	BitBoard mWP;
	BitBoard mBP;
	BitBoard mK;

	typedef BitBoard BB;
	/* Tracks if it's P1's turn or not */
	bool mTurn;
	bool mDebug;
	Save mSave;
	bool mInteract;
	BB mMustJump;

	/* Update save game */
	void updateSave();

	BitBoard getJumpers() const;
	BitBoard getMovers() const;
	BitBoard getJumpers(const Save& save) const;
	BitBoard getMovers(const Save& save) const;
	BitBoard getEmpty() const
	{
		return ~(mWP | mBP);
	}
	BitBoard getEmpty(const Save& save) const
	{
		return ~(save.WP | save.BP);
	}

	inline BB canJump(const BB src, const BB vict) const;
	BB canJump(const Save& save, const BB src, const BB vict) const;

	/* Piece Movement */
	MoveCode makeMove(const Move& move);

	/* Jumping */
	MoveCode jump(const Move& move);
};

#endif /* GAME_HPP_ */
