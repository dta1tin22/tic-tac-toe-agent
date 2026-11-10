#include "minimax_strategy.hpp"

#include "board.hpp"

MinimaxStrategy::MinimaxStrategy() {}

Move MinimaxStrategy::choose_move(const Board& board, Mark mark) {
	if (board.get_winner() == Mark::EMPTY && mark == Mark::X) {
		return Move(0, 0);
	} else if (board.get_winner() == Mark::EMPTY && mark == Mark::O) {
		return Move(1, 1);
	}
	return Move(1, 2);
}
