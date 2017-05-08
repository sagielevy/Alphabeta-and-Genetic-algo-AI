/*
 * NewAI.cpp
 *
 *  Created on: Nov 18, 2012
 *      Author: mark
 */

#include "NewAI.hpp"

#include <algorithm>
#include <iostream>
#include <limits>

Game NewAI::s_game(false, false);

NewAI::NewAI(const bool debug, const unsigned char difficulty, const unsigned char defendPiece, const unsigned char defendKing,
	const unsigned char attackPiece, const unsigned char attackKing, const bool isWhite) :
		_difficulty(difficulty),
		_defendPiece(defendPiece),
		_defendKing(defendKing),
		_attackPiece(attackPiece),
		_attackKing(attackKing),
		_isWhite(isWhite),
		_debug(debug)
{
	_root.depth = 0;

	if (_debug)
	{
		std::cout << ((_isWhite) ? "White" : "Black") << " player:\n" <<
			"Protect own piece: " << (unsigned)_defendPiece << ", protect own king: " << (unsigned)_defendKing << ", eat opponent piece: "
			<< (unsigned)_attackPiece << ", eat opponent king: " << (unsigned)_attackKing << std::endl;
	}
}

NewAI::~NewAI()
{
	
}

void NewAI::initialize(const Save& s)
{
	s_game.restoreToSave(s);
	_root.branches.clear();
	_root.save = s;
}

Move NewAI::evaluate_game(Game& g)
{
	const Save old_save = g.getSave();
	initialize(old_save);

	int alpha = std::numeric_limits<int>::min();
	int beta = std::numeric_limits<int>::max();

	alphabeta(_root, alpha, beta, old_save.turn);

	if (_root.branches.empty()) return {0,0,false};

	//Move best_move;
	int idx = 0;

	if (g.mTurn) {
		int max = alpha;
		for (size_t i = 0; i < _root.branches.size(); i++) {
			if (_root.branches[i]->val > max) {
				max = _root.branches[i]->val;
				idx = i;
			}

			if (_debug)
			{
				_root.branches[i]->print();
			}
		}
	} else {
		int min = beta;
		for (size_t i = 0; i < _root.branches.size(); i++) {
			if (_root.branches[i]->val < min) {
				min = _root.branches[i]->val;
				idx = i;
			}

			if (_debug)
			{
				_root.branches[i]->print();
			}
		}
	}

	if (_debug)
	{
		using std::cout;
		using std::endl;

		cout << ((g.mTurn) ? "Maximizer" : "Minimizer") << " selected " << _root.branches[idx]->val << endl;
	}

	return _root.branches[idx]->move;

}

int NewAI::alphabeta(Node& node, int alpha, int beta, bool player)
{
	using std::max;
	using std::min;
	using Bit::Masks::S;

	// Will be used for checking if player has another consecutive turn
	const BitBoard src = (player ? node.save.BP & S[node.move.src] : node.save.WP & S[node.move.src]);
	const BitBoard vict = (player ? node.save.WP & S[node.move.dst] : node.save.BP & S[node.move.dst]);
	BB nextLoc = s_game.canJump(src, vict);

	if (!s_game.getJumpers(node.save) && !s_game.getMovers(node.save)) {
		if (node.depth == 0 && _debug) std::cerr << "NO MOVES: FIRST CHECK.\n";
		return node.save.grade(_defendPiece, _defendKing, _attackPiece, _attackKing, _isWhite);
	}

//	if (node.depth == 0)
//		return node.save.grade();
	if (node.depth >= _difficulty) {
//		std::cerr << "TOO DEEP!!.!!" << (int)node.depth << " " << (int)_difficulty << std::endl;

		//std::cout << std::endl;
		//s_game.print(node.save);

		// Update leaf value first, then return it
		node.val = node.save.grade(_defendPiece, _defendKing, _attackPiece, _attackKing, _isWhite);
		//std::cout << "Heuristic for this board by this player is: " << node.val << std::endl;

		return node.val;
	}

	gen_outcomes(node);

	// Maximizer (black)
	if (player) {
		int v = INT_MIN;

		for (auto & child : node.branches) {
			// Check other player's turn ONLY if current player does not have another jump after this turn
			v = max(v, alphabeta(*(child), alpha, beta, (nextLoc & s_game.getJumpers(child->save)) ? player : !player));
			alpha = max(alpha, v);

//			Node nd = *child;
			if (beta <= alpha)
				break;
		}

		node.val = v;
		return v;
	} else {
		// Minimizer (white)
		int v = INT_MAX;

		for (auto & child : node.branches) {
			v = min(v, alphabeta(*(child), alpha, beta, (nextLoc & s_game.getJumpers(child->save)) ? player : !player));
			beta = min(beta, v);

			//			Node nd = *child;
			if (beta <= alpha)
				break;
		}

		node.val = v;
		return v;
	}

}

void NewAI::gen_outcomes(Node& n)
{
	// First of all load the current node state to the game board
	s_game.restoreToSave(n.save);

	gen_jumps(n);
	if (n.branches.empty())
		gen_moves(n);
}

void NewAI::gen_moves_black(Node& n)
{
//	using namespace std;
	using namespace Bit::Masks;
	using Bit::rol;
	using Bit::ror;
	using Bit::highBit;

	BB Movers = s_game.getMovers(n.save);
//	std::cout << "++++++++++++" << hex << Movers << dec <<std::endl;

	BB empty = s_game.getEmpty(n.save);
	BB target;

	while (Movers) {
		const BB mover = highBit(Movers);
//		std::cout << "++++++++++++mover = :" << hex << mover << dec <<std::endl;
		Movers ^= mover;
		
		if ((target = ((rol(mover & CAN_UPLEFT, 7) & empty))))
			n.move_and_restore( { bbUMap[mover], bbUMap[target], false }, INT_MIN);
	
		if ((target = ((rol(mover & CAN_UPRIGHT, 1) & empty))))
//			mMoves.push_back( { bbUMap[mover], bbUMap[target], false });
			n.move_and_restore( { bbUMap[mover], bbUMap[target], false }, INT_MIN);

		if (mover & n.save.K) { //s_game.mK) {
			if ((target = ((ror(mover & CAN_DOWNRIGHT, 7) & empty))))
				n.move_and_restore( { bbUMap[mover], bbUMap[target], false }, INT_MIN);
			if ((target = ((ror(mover & CAN_DOWNLEFT, 1) & empty))))
				n.move_and_restore( { bbUMap[mover], bbUMap[target], false }, INT_MIN);
		}
	}
}

void NewAI::gen_moves_white(Node& n)
{
	using namespace std;
	using namespace Bit::Masks;
	using Bit::ror;
	using Bit::rol;
	using Bit::highBit;

	BB Movers = s_game.getMovers(n.save);
//	std::cout << "++++++++++++" << hex << Movers << dec <<std::endl;

	BB empty = s_game.getEmpty(n.save);
	BB target;

	while (Movers) {
		BB mover = highBit(Movers);
//		std::cout << "++++++++++++mover = :" << hex << mover << dec <<std::endl;
		Movers ^= mover;

		if ((target = ((ror(mover & CAN_DOWNRIGHT, 7) & empty))))
//			mMoves.push_back( { bbUMap[mover], bbUMap[target], false });
			n.move_and_restore( { bbUMap[mover], bbUMap[target], false }, INT_MAX);

		if ((target = ((ror(mover & CAN_DOWNLEFT, 1) & empty))))
			n.move_and_restore( { bbUMap[mover], bbUMap[target], false }, INT_MAX);

		if (mover & n.save.K) { //s_game.mK) {
			if ((target = ((rol(mover & CAN_UPLEFT, 7) & empty))))
				n.move_and_restore( { bbUMap[mover], bbUMap[target], false }, INT_MAX);
			if ((target = ((rol(mover & CAN_UPRIGHT, 1) & empty))))
				n.move_and_restore( { bbUMap[mover], bbUMap[target], false }, INT_MAX);
		}
	}
}

void NewAI::gen_jumps_black(Node& n)
{
	using namespace std;
	using Bit::Masks::bbUMap;
	using Bit::rol;
	using Bit::ror;
	using Bit::highBit;

	BB jumpers = s_game.getJumpers(n.save);
//	_game->print();
//	std::cout << "++++++++++++" << hex << jumpers << dec << std::endl;

	while (jumpers) {
		BB j = highBit(jumpers);
//		std::cout << "++++++++++++jumper = :" << hex << j << dec <<std::endl;
		jumpers ^= j;
		BB victims = n.save.WP; //s_game.mWP;
		BB vict;
		vict = rol(j, 7) & victims;

		if (s_game.canJump(n.save, j, vict)) {
//			mMoves.push_back( { bbUMap[j], bbUMap[vict], true });
			n.move_and_restore( { bbUMap[j], bbUMap[vict], true }, INT_MIN);
		}
		vict = rol(j, 1) & victims;

		if (s_game.canJump(n.save, j, vict)) {
//			mMoves.push_back( { bbUMap[j], bbUMap[vict], true });
			n.move_and_restore( { bbUMap[j], bbUMap[vict], true }, INT_MIN);
		}

		if (j & n.save.K) { //s_game.mK) {
			vict = ror(j, 7) & victims;
			
			if (s_game.canJump(n.save, j, vict)) {
				n.move_and_restore( { bbUMap[j], bbUMap[vict], true }, INT_MIN);
			}
			vict = ror(j, 1) & victims;

			if (s_game.canJump(n.save, j, vict)) {
				n.move_and_restore( { bbUMap[j], bbUMap[vict], true }, INT_MIN);
			}
		}
	}
}

void NewAI::gen_jumps_white(Node& n)
{
	using namespace std;
	using Bit::Masks::bbUMap;
	using Bit::rol;
	using Bit::ror;
	using Bit::highBit;

	BB jumpers = s_game.getJumpers(n.save);

	while (jumpers) {
		BB j = highBit(jumpers);
		jumpers ^= j;
		BB victims = n.save.BP; //s_game.mBP;

		BB vict = ror(j, 7) & victims;

		if (s_game.canJump(n.save, j, vict)) {
//			mMoves.push_back( { bbUMap[j], bbUMap[vict], true });
			n.move_and_restore( { bbUMap[j], bbUMap[vict], true }, INT_MAX); // White is minimizer, default is max
		}
		vict = ror(j, 1) & victims;
		if (s_game.canJump(n.save, j, vict)) {
			n.move_and_restore( { bbUMap[j], bbUMap[vict], true }, INT_MAX);
		}

		if (j & n.save.K) { // s_game.mK) {
			vict = rol(j, 7) & victims;
			if (s_game.canJump(n.save, j, vict)) {
				n.move_and_restore( { bbUMap[j], bbUMap[vict], true }, INT_MAX);
			}
			vict = rol(j, 1) & victims;
			if (s_game.canJump(n.save, j, vict)) {
				n.move_and_restore( { bbUMap[j], bbUMap[vict], true }, INT_MAX);
			}
		}
	}
}

Move NewAI::get_random_move() const
{
	unsigned index = rand() % _root.branches.size();

	return _root.branches[index]->move;
}

void Node::move_and_restore(const Move& m, int defaultVal)
{
	auto org = NewAI::s_game.getSave();
	NewAI::s_game.move(m);

	branches.push_back(
			std::unique_ptr<Node>(
					new Node(NewAI::s_game.getSave(), m, depth + 1, defaultVal)));

	NewAI::s_game.restoreToSave(save);
}

void Node::print() const
{
	using namespace std;

	cout << "This is a degree " << (int)depth << " Node with Value: " << val << "."
			<< endl;
}
