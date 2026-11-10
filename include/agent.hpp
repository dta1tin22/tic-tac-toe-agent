#pragma once

#include <memory>

#include "board.hpp"
#include "strategy.hpp"

class Agent {
 private:
	Board board;
	Mark my_mark;
	Mark opponent_mark;
	std::unique_ptr<Strategy> strategy;

 public:
	Agent(Mark, std::unique_ptr<Strategy>);
	void receive_oppenent_move(Move);
	Move make_move();
	bool is_game_over() const;
};
