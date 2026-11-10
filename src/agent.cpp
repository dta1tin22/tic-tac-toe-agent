#include "agent.hpp"

#include "board.hpp"

Agent::Agent(Mark mark, std::unique_ptr<Strategy> strat)
		: board(Board()),
			my_mark(mark),
			opponent_mark(mark == Mark::X ? Mark::O : Mark::X),
			strategy(std::move(strat)) {}

void Agent::receive_oppenent_move(Move move) {
	if (!board.place(move, opponent_mark)) {
		throw std::invalid_argument("Invalid move received from opponent.");
	};
}

Move Agent::make_move() {
	Move move = strategy->choose_move(board, my_mark);
	if (!board.place(move, my_mark)) {
		throw std::invalid_argument("Invalid move chosen by strategy.");
	}

	board.place(move, my_mark);

	return move;
}

bool Agent::is_game_over() const {
	return board.get_winner() != Mark::EMPTY || board.is_full();
}
