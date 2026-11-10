#include "random_strategy.hpp"

#include <stdexcept>

#include "board.hpp"

RandomStrategy::RandomStrategy(int seed) : seed(seed) { srand(seed); }

Move RandomStrategy::choose_move(const Board& board, Mark _) {
	std::vector<Move> valid_moves = board.get_empty_moves();
	if (valid_moves.empty()) {
		throw std::runtime_error("No valid moves available");
	}
	int random_index = rand() % valid_moves.size();
	return valid_moves[random_index];
}
