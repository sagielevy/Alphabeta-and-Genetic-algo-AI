#include "CmdMode.hpp"
#include <sstream>

int main(int argc, char* argv[])
{
	/*
		Arguments:
		0. Path. Ignore.
		1. Print flag. True if game is to print to console and perform saves, etc. False if to run without any GUI
		2. Depth value. How deep is the alpha beta tree for both AIs
		3..6 Values for the following variables for the heuristic calculation: Protect own piece, protect own king, eat opponent piece, eat opponent king
		7..10 Same variables for the opposing player (white)
	*/

	// Bad args
	if (argc != 11)
		return 100 + argc;

	bool print;
	std::stringstream ss(argv[1]);

	if (!(ss >> std::boolalpha >> print)) {
		// Parsing error.
	}

	// print has the correct value.
	unsigned char depth = atoi(argv[2]);
	unsigned char p1dp = atoi(argv[3]);
	unsigned char p1dk = atoi(argv[4]);
	unsigned char p1ap = atoi(argv[5]);
	unsigned char p1ak = atoi(argv[6]);
	unsigned char p2dp = atoi(argv[7]);
	unsigned char p2dk = atoi(argv[8]);
	unsigned char p2ap = atoi(argv[9]);
	unsigned char p2ak = atoi(argv[10]);

	CmdModeGame game = CmdModeGame::CmdModeGame(print, depth, p1dp, p1dk, p1ap, p1ak, p2dp, p2dk, p2ap, p2ak);
	int result = game.loop();

	std::cout << "Game ended with result = " << result << std::endl;

	return result;
}