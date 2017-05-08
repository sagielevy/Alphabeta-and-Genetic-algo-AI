/*
 * AI_test.cpp
 *
 *  Created on: Sep 7, 2012
 *      Author: msinclair94
 */

#include <iostream>
#include "AI.hpp"

void testmJump() {
	using std::cout;
	using std::endl;

	Game g(false, false);

	SimpleAI a;

	Move move;

	do {
		cout << "P1: " << g.getP1score() <<"P2: " << g.getP2score() << endl;
		g.print();
		move = a.evaluate_game(g);

		cout << errorTable[g.move(move)];
	} while (g.getP1score() > 0 &&  g.getP2score() > 0);

}

//int main() {
//	testmJump();
//
//	return 0;
//}

