#pragma once

#include "board.hpp"

class Strategy {
 public:
	virtual ~Strategy() = default;
	virtual Move choose_move(const Board&, Mark) = 0;
};
