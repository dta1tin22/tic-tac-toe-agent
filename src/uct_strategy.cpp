#include "uct_strategy.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <memory>
#include <random>
#include <stdexcept>
#include <vector>

#include "board.hpp"

namespace {

Mark opposite_mark(Mark mark) {
	return mark == Mark::X ? Mark::O : Mark::X;
}

std::vector<Move> get_candidate_moves(const Board& board, int radius = 2) {
	bool has_occupied[Board::SIZE][Board::SIZE] = {false};
	bool any_occupied = false;

	for (int r = 0; r < Board::SIZE; ++r) {
		for (int c = 0; c < Board::SIZE; ++c) {
			if (!board.is_valid_move({r, c})) {
				has_occupied[r][c] = true;
				any_occupied = true;
			}
		}
	}

	if (!any_occupied) {
		return {{Board::SIZE / 2, Board::SIZE / 2}};
	}

	std::vector<Move> candidates;
	for (int r = 0; r < Board::SIZE; ++r) {
		for (int c = 0; c < Board::SIZE; ++c) {
			if (has_occupied[r][c]) continue;

			bool near_stone = false;
			int r_min = std::max(0, r - radius);
			int r_max = std::min(Board::SIZE - 1, r + radius);
			int c_min = std::max(0, c - radius);
			int c_max = std::min(Board::SIZE - 1, c + radius);

			for (int nr = r_min; nr <= r_max && !near_stone; ++nr) {
				for (int nc = c_min; nc <= c_max; ++nc) {
					if (has_occupied[nr][nc]) {
						near_stone = true;
						break;
					}
				}
			}

			if (near_stone) {
				candidates.push_back({r, c});
			}
		}
	}

	if (candidates.empty()) {
		return board.get_empty_moves();
	}
	return candidates;
}

struct MCTSNode {
	Move move;
	Mark player_just_moved;
	MCTSNode* parent;
	std::vector<std::unique_ptr<MCTSNode>> children;
	std::vector<Move> untried_moves;
	int visits = 0;
	double wins = 0.0;

	MCTSNode(Move move, Mark player_just_moved, MCTSNode* parent,
					 std::vector<Move>&& candidates)
			: move(move),
				player_just_moved(player_just_moved),
				parent(parent),
				untried_moves(std::move(candidates)) {}

	bool is_fully_expanded() const { return untried_moves.empty(); }

	MCTSNode* uct_select_child(double c_param) const {
		MCTSNode* best_child = nullptr;
		double best_score = -std::numeric_limits<double>::infinity();
		double log_parent_visits = std::log(static_cast<double>(visits));

		for (const auto& child : children) {
			double exploitation = child->wins / static_cast<double>(child->visits);
			double exploration =
					c_param *
					std::sqrt(log_parent_visits / static_cast<double>(child->visits));
			double uct_score = exploitation + exploration;
			if (uct_score > best_score) {
				best_score = uct_score;
				best_child = child.get();
			}
		}
		return best_child;
	}
};

}  // namespace

UCTStrategy::UCTStrategy(int iterations, double exploration_constant)
		: iterations(iterations), exploration_constant(exploration_constant) {}

Move UCTStrategy::choose_move(const Board& board, Mark my_mark) {
	std::vector<Move> root_candidates = get_candidate_moves(board, 2);
	if (root_candidates.empty()) {
		throw std::runtime_error("No valid moves available");
	}
	if (root_candidates.size() == 1) {
		return root_candidates[0];
	}

	Mark opp_mark = opposite_mark(my_mark);

	// Check for immediate winning move or immediate block
	Board temp_board = board;
	for (const Move& m : root_candidates) {
		temp_board.place(m, my_mark);
		if (temp_board.get_winner() == my_mark) {
			return m;
		}
		temp_board.undo(m);
	}
	for (const Move& m : root_candidates) {
		temp_board.place(m, opp_mark);
		if (temp_board.get_winner() == opp_mark) {
			return m;
		}
		temp_board.undo(m);
	}

	std::mt19937 rng(std::random_device{}());
	std::shuffle(root_candidates.begin(), root_candidates.end(), rng);

	auto root = std::make_unique<MCTSNode>(Move{-1, -1}, opp_mark, nullptr,
																				 std::move(root_candidates));

	constexpr int MAX_ROLLOUT_DEPTH = 16;

	for (int iter = 0; iter < iterations; ++iter) {
		Board state = board;
		MCTSNode* node = root.get();

		// 1. Selection
		while (node->is_fully_expanded() && !node->children.empty()) {
			node = node->uct_select_child(exploration_constant);
			state.place(node->move, node->player_just_moved);
		}

		Mark winner = state.get_winner();

		// 2. Expansion
		if (winner == Mark::EMPTY && !node->untried_moves.empty()) {
			Move next_move = node->untried_moves.back();
			node->untried_moves.pop_back();

			Mark next_player = opposite_mark(node->player_just_moved);
			state.place(next_move, next_player);
			winner = state.get_winner();

			std::vector<Move> child_candidates;
			if (winner == Mark::EMPTY) {
				child_candidates = get_candidate_moves(state, 1);
				std::shuffle(child_candidates.begin(), child_candidates.end(), rng);
			}

			auto child = std::make_unique<MCTSNode>(next_move, next_player, node,
																							std::move(child_candidates));
			MCTSNode* child_ptr = child.get();
			node->children.push_back(std::move(child));
			node = child_ptr;
		}

		// 3. Simulation (Rollout)
		if (winner == Mark::EMPTY) {
			std::vector<Move> rollout_moves = get_candidate_moves(state, 1);
			std::shuffle(rollout_moves.begin(), rollout_moves.end(), rng);

			Mark current_player = opposite_mark(node->player_just_moved);
			int steps = 0;
			for (const Move& rm : rollout_moves) {
				if (steps >= MAX_ROLLOUT_DEPTH) break;
				if (!state.is_valid_move(rm)) continue;
				state.place(rm, current_player);
				steps++;
				winner = state.get_winner();
				if (winner != Mark::EMPTY) break;
				current_player = opposite_mark(current_player);
			}
		}

		// 4. Backpropagation
		while (node != nullptr) {
			node->visits++;
			if (winner == node->player_just_moved) {
				node->wins += 1.0;
			} else if (winner == Mark::EMPTY) {
				node->wins += 0.5;
			}
			node = node->parent;
		}
	}

	// Select the child with the highest visit count (robust child)
	const MCTSNode* best_child = nullptr;
	int most_visits = -1;
	for (const auto& child : root->children) {
		if (child->visits > most_visits) {
			most_visits = child->visits;
			best_child = child.get();
		}
	}

	return best_child ? best_child->move : root->children.front()->move;
}
