#include "board.hpp"

#include <vector>

Board::Board() {
	for (int i = 0; i < SIZE; ++i) {
		for (int j = 0; j < SIZE; ++j) {
			grid[i][j] = Mark::EMPTY;
		}
	}
}

bool Board::place(Move move, Mark mark) {
	if (is_valid_move(move)) {
		grid[move.row][move.col] = mark;
		return true;
	}
	return false;
}

void Board::undo(Move move) {
	if (is_in_bounds(move)) {
		grid[move.row][move.col] = Mark::EMPTY;
	}
}

bool Board::is_in_bounds(Move move) const {
	return move.row >= 0 && move.row < SIZE && move.col >= 0 && move.col < SIZE;
}

bool Board::is_valid_move(Move move) const {
	return is_in_bounds(move) && grid[move.row][move.col] == Mark::EMPTY;
}

std::vector<Move> Board::get_empty_moves() const {
	std::vector<Move> empty_moves;
	for (int i = 0; i < SIZE; ++i) {
		for (int j = 0; j < SIZE; ++j) {
			if (grid[i][j] == Mark::EMPTY) {
				empty_moves.push_back({i, j});
			}
		}
	}
	return empty_moves;
}

Mark Board::get_winner() const {
	int dr[4] = {0, 1, 1, -1};
	int dc[4] = {1, 0, 1, 1};

	for (int r = 0; r < SIZE; ++r) {
		for (int c = 0; c < SIZE; ++c) {
			if (grid[r][c] == Mark::EMPTY) continue;
			Mark m = grid[r][c];

			for (int d = 0; d < 4; d++) {
				if (!is_in_bounds(
								{r + dr[d] * (WIN_LEN - 1), c + dc[d] * (WIN_LEN - 1)}))
					continue;

				int k = 1;
				while (k < WIN_LEN && grid[r + dr[d] * k][c + dc[d] * k] == m) {
					k++;
				}

				if (k == WIN_LEN) return m;
			}
		}
	}

	return Mark::EMPTY;
}

bool Board::is_full() const {
	for (int i = 0; i < SIZE; ++i) {
		for (int j = 0; j < SIZE; ++j) {
			if (grid[i][j] == Mark::EMPTY) {
				return false;
			}
		}
	}
	return true;
}
