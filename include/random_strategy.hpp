#pragma once

#include "strategy.hpp"

class RandomStrategy : public Strategy {
 private:
	int seed;

 public:
	RandomStrategy(int);
	Move choose_move(const Board&, Mark) override;
};
