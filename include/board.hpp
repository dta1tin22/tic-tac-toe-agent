#pragma once

#include <array>
#include <vector>
enum class Mark { X, O, EMPTY };

struct Move {
	int row;
	int col;
};

class Board {
 public:
	static constexpr int SIZE = 30;
	static constexpr int WIN_LEN = 5;

 private:
	std::array<std::array<Mark, SIZE>, SIZE> grid;

 public:
	Board();
	bool place(Move, Mark);
	void undo(Move);
	bool is_in_bounds(Move) const;
	bool is_valid_move(Move) const;
	std::vector<Move> get_empty_moves() const;
	Mark get_winner() const;
	bool is_full() const;
};
