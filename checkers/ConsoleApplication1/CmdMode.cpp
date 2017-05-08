#include "CmdMode.hpp"

CmdModeGame::CmdModeGame(const bool print, const unsigned char depth, const unsigned char p1dp, const unsigned char p1dk, const unsigned char p1ap, const unsigned char p1ak,
	const unsigned char p2dp, const unsigned char p2dk, const unsigned char p2ap, const unsigned char p2ak) :
	mGame(false, false),
	mBoard(64),
	mLastMoves(), // Helps check for any loops that cause stalemate
	mAi1(print, depth, p1dp, p1dk, p1ap, p1ak, false), // 14 depth originally
	mAi2(print, depth, p2dp, p2dk, p2ap, p2ak, true) // White player
{
	mPrint = print;
}

CmdModeGame::~CmdModeGame() {}

// Has the last move repeated a previous one? If so, since move choice is deterministic,
// there's a loop and so the game is at stalemate
bool CmdModeGame::isStalemate() const
{
	// At least four moves have occurred (otherwise board[0] can not be equal to board[3]
	if (mLastMoves.size() > 4)
	{
		auto& lastMove = mLastMoves.at(mLastMoves.size() - 1);

		// Run up to the last potential repeated move
		for (size_t i = 0; i < mLastMoves.size() - 4; i++)
		{
			if (lastMove.BP == mLastMoves[i].BP &&
				lastMove.WP == mLastMoves[i].WP &&
				lastMove.K == mLastMoves[i].K) {
				return true;
			}
		}
	}
		
	return false;
}

void CmdModeGame::addSave(const Save& save)
{
	// If last move has different pieces count (different kings/pieces count) than current move,
	// There's no possibility of a loop happening between this move and the previous ones, so clear the queue
	if (mLastMoves.size() > 1 && !piecesEqual(mLastMoves.at(mLastMoves.size() - 1), save))
	{
		mLastMoves.clear();
	}

	// Add last move to queue
	mLastMoves.push_back(save);
}

int CmdModeGame::loop()
{
	using std::cout;
	using std::endl;

	if (mPrint)
	{
		mGame.print();
	}

	while (mGame.isLive())
	{
		Move move = mAi1.evaluate_game(mGame);
		//cout << "AI Move: " << move << endl;

		addSave(mGame.getSave());

		// If no move available or stalemate reached
		if ((move.dst == 0 && move.src == 0) || isStalemate())
			break;

		mGame.move(move);

		if (mPrint)
		{
			mGame.print();
			cout << endl;
		}
		//	cout << errorTable[mGame.move(move)] << endl;

		//if (mGame.mTurn)
		//	mState = NORMAL;
		//}

		move = mAi2.evaluate_game(mGame);
		//cout << "AI Move: " << move << endl;

		addSave(mGame.getSave());

		// If no move available or stalemate reached
		if ((move.dst == 0 && move.src == 0) || isStalemate())
			break;

		mGame.move(move);

		if (mPrint)
		{
			mGame.print();
			cout << endl;
		}
	}

	// 1 if p1 won, -1 if p2 won, 0 if tie. 
	// If stalemate has happened, judge winner by grade (who has more pieces & kings on the board)
	//return (mGame.grade() != 0 && !isStalemate()) ? ((mGame.grade() > 0) ? 1 : -1) : 0;
	return clamp(mGame.grade(), LOSE, WIN);
}