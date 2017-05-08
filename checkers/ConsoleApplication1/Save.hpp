/*
 * Save.hpp
 *
 *  Created on: Aug 23, 2012
 *      Author: msinclair94
 */

#ifndef SAVE_HPP_
#define SAVE_HPP_

#include "BitBoard.hpp"

#include <fstream>
#include <map>
#include <string>
#include <vector>

struct Save {

	bool turn;
	BitBoard mustJump;
	BitBoard WP;
	BitBoard BP;
	BitBoard K;

	Save();
	~Save();
	friend void swap(Save&, Save&);

	/* Equality test */
	friend bool operator==(const Save&, const Save&);

	/* Read and write to file */
	void write(std::string) const;
	void read(std::string);

	//int grade()
	//{
	//	int p1 = Bit::bitCount(BP & ~K) + 0*2 * Bit::bitCount(BP & K);
	//	int p2 = Bit::bitCount(WP & ~K) + 0*2 * Bit::bitCount(WP & K);
	//	p2 *= -1;

	//	return p1 + p2;
	//}

	// AI heuristic grading, unique for each AI
	int grade(const unsigned char defendPiece, const unsigned char defendKing,
		const unsigned char attackPiece, const unsigned char attackKing, bool isWhite)
	{
		// Assuming K is the bitboard of the kings, WP are white pieces and BP are black pieces
		// White is minimizer black is maximizer.
		// P1 represents black player's value
		int black;
		int white;

		if (!isWhite)
		{
			// Black
			black = defendPiece * Bit::bitCount(BP & ~K) + defendKing * Bit::bitCount(BP & K);
			white = attackPiece * Bit::bitCount(WP & ~K) + attackKing * Bit::bitCount(WP & K);
		}
		else
		{
			// White
			black = attackPiece * Bit::bitCount(BP & ~K) + attackKing * Bit::bitCount(BP & K);
			white = defendPiece * Bit::bitCount(WP & ~K) + defendKing * Bit::bitCount(WP & K);
		}
		
		// P2, white, is minimizer
		white *= -1;

		return black + white;

		//int p1 = Bit::bitCount(BP & ~K) + 0 * 2 * Bit::bitCount(BP & K);
		//int p2 = Bit::bitCount(WP & ~K) + 0*2 * Bit::bitCount(WP & K);
		//p2 *= -1;

		//return p1 + p2;
	}

private:
};

// Checks if both boards have equal pieces (same amount of black & white pieces and kings)
inline bool piecesEqual(const Save& a, const Save& b)
{
	return Bit::bitCount(a.BP) == Bit::bitCount(b.BP) &&
		Bit::bitCount(a.WP) == Bit::bitCount(b.WP) &&
		Bit::bitCount(a.BP & a.K) == Bit::bitCount(b.BP & b.K) &&
		Bit::bitCount(a.BP & a.K) == Bit::bitCount(b.BP & b.K);
}

#endif /* SAVE_HPP_ */
