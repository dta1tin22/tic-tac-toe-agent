#include <iostream>
#include <memory>

#include "agent.hpp"
#include "argparse.hpp"
#include "board.hpp"
#include "minimax_strategy.hpp"

Move get_opponent_move() {
	int row, col;
	std::cin >> row >> col;
	return Move{row, col};
}

void print_move(const Move& move) {
	std::cout << move.row << " " << move.col << std::endl;
}

int main(int argc, char* argv[]) {
	argparse::ArgumentParser program("Tic Tac Toe Agent");
	program.add_argument("-f", "--first")
			.help("Whether the agent plays first")
			.default_value(false)
			.implicit_value(true);

	try {
		program.parse_args(argc, argv);
	} catch (const std::runtime_error& err) {
		std::cerr << err.what() << std::endl;
		std::cerr << program;
		exit(1);
	}

	auto first = program.get<bool>("-f");

	Agent agent(Mark::O, std::make_unique<MinimaxStrategy>());

	if (first) {
		Move agent_move = agent.make_move();
		print_move(agent_move);
	}

	while (!agent.is_game_over()) {
		Move opponent_move = get_opponent_move();
		agent.receive_oppenent_move(opponent_move);
		if (agent.is_game_over()) break;
		Move agent_move = agent.make_move();
		print_move(agent_move);
	}

	return 0;
}
