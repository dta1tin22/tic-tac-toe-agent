#pragma once

#include "strategy.hpp"

class MinimaxStrategy : public Strategy {
 private:
 public:
	MinimaxStrategy();
	Move choose_move(const Board&, Mark) override;
};
