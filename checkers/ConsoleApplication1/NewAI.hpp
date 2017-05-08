/*
 * NewAI.hpp
 *
 *  Created on: Nov 18, 2012
 *      Author: mark
 */

#ifndef NEWAI_HPP_
#define NEWAI_HPP_

#include "AI.hpp"

struct Node {
	Save save;
	int val;
	std::vector<std::unique_ptr<Node>> branches;
	Move move;
	unsigned char depth;

	Node()  {}
	Node(const Save& s, const Move& m, unsigned char depth, int defaultVal) : save(s), move(m) , depth(depth), val(defaultVal){}
	void move_and_restore(const Move&, int defaultVal);
	void jump_and_restore(const Move&, int defaultVal);
	void print() const;

};

class NewAI : public BaseAI {
public:
	friend struct Node;

	NewAI(const bool debug, const unsigned char difficulty, const unsigned char defendPiece, const unsigned char defendKing,
		const unsigned char attackPiece, const unsigned char attackKing, const bool isWhite);
	virtual ~NewAI();

	//	void print_scene();

	Move get_random_move() const;

	Move evaluate_game(Game&);


	void difficulty(unsigned difficulty)
	{
		_difficulty = difficulty;
	}

	unsigned difficulty() const
	{
		return _difficulty;
	}

private:
	void gen_moves_black(Node&);
	void gen_moves_white(Node&);
	void gen_moves(Node& n) {
		if (n.save.turn) //(s_game.mTurn)
			gen_moves_black(n);
		else
			gen_moves_white(n);
	}


	void gen_jumps_black(Node&);
	void gen_jumps_white(Node&);
	void gen_jumps(Node& n) {
		if (n.save.turn) //(s_game.mTurn)
			gen_jumps_black(n);
		else
			gen_jumps_white(n);
	}

	void gen_outcomes(Node&);

	void initialize(const Save&);

	int alphabeta(Node& node, int alpha, int beta, bool player);

	Node _root;
	unsigned char _difficulty;
	static Game s_game;
	unsigned char _defendPiece;
	unsigned char _defendKing;
	unsigned char _attackPiece;
	unsigned char _attackKing;
	bool _isWhite;
	bool _debug;
};



#endif /* NEWAI_HPP_ */
