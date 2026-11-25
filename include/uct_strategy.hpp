#pragma once

#include "strategy.hpp"

class UCTStrategy : public Strategy {
 private:
	int iterations;
	double exploration_constant;

 public:
	UCTStrategy(int iterations = 2000, double exploration_constant = 1.41421356);
	Move choose_move(const Board&, Mark) override;
};
