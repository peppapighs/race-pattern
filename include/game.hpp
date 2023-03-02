#pragma once

#include "board.hpp"
#include "pattern_list.hpp"

struct move_t {
    int src, dst;
};

namespace game {
bool is_game_over(const board_t &board, bool is_b_turn) {
    if (!is_b_turn && board.w_mask >= 1LL << ((board.row - 1) * board.col))
        return true;
    if (is_b_turn && !board.b_mask)
        return true;
    return false;
}

board_t transition(const board_t &board, const move_t &move) {
    long long b_mask = board.b_mask, w_mask = board.w_mask;
    b_mask ^= 1LL << move.src;
    if (move.dst != -1) {
        b_mask ^= 1LL << move.dst;
        w_mask &= ~(1LL << (board.row * board.col - 1 - move.dst));
    }
    return {board.row, board.col, w_mask, b_mask};
}

std::vector<board_t> neighbors(const board_t &board, bool is_b_turn) {
    std::vector<board_t> ret;
    for (int i = board.row * board.col - 1; i >= 0; i--) {
        if (!(board.b_mask & 1LL << i))
            continue;
        int r = i / board.col, c = i % board.col;
        for (int dc = -1; dc <= 1; dc++) {
            int nr = r + 1, nc = c + dc;
            int dst = nr * board.col + nc;

            if (nc < 0 || nc >= board.col)
                continue;
            if (is_b_turn && nr >= board.row)
                continue;

            if (nr >= board.row) {
                ret.push_back(transition(board, {i, -1}));
                continue;
            }

            if (board.b_mask & 1LL << dst)
                continue;
            if (dc == 0 &&
                (board.w_mask & 1LL << (board.row * board.col - 1 - dst)))
                continue;
            if (is_b_turn && nr == board.row - 1)
                return {transition(board, {i, dst})};
            ret.push_back(transition(board, {i, dst}));
        }
    }
    return ret;
}

int negamax(const board_t &board, int alpha, int beta, bool is_b_turn,
            const PatternList &pattern_list) {
    if (is_game_over(board, is_b_turn))
        return -1;

    if (is_b_turn && pattern_list.contains(board))
        return 1;

    int best = -2;
    for (auto &neighbor : neighbors(board, is_b_turn)) {
        int score = -negamax(neighbor, -beta, -alpha, !is_b_turn, pattern_list);
        if (score >= beta)
            return score;
        if (score > best)
            best = score;
        if (score > alpha)
            alpha = score;
    }
    return best;
}

bool is_winnable(const board_t &board, const PatternList &pattern_list) {
    for (auto &board : board::expand(board))
        if (negamax(board, -2, 2, true, pattern_list) != 1)
            return false;
    return true;
}
}; // namespace game