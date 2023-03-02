#pragma once

#include <thread>

#include "concurrent_queue.hpp"
#include "game.hpp"
#include "scope_guard.hpp"

inline long long next_permutation(long long mask) {
    long long t = mask | (mask - 1);
    return (t + 1) | (((~t & -~t) - 1) >> (__builtin_ctzll(mask) + 1));
}

class PatternFinder {
  private:
    concurrent_queue<board_t> queue;
    std::vector<std::jthread> threads;

    PatternList pattern_list;

    void worker_thread() {
        while (true) {
            auto item = queue.pop();
            if (!item)
                break;
            board_t &board = *item;
            scope_guard guard([&] { queue.done(); });

            if (pattern_list.contains(board))
                continue;
            if (game::is_winnable(board, pattern_list))
                pattern_list.add(board);
        }
    }

  public:
    PatternFinder(int thread_count) {
        for (int i = 0; i < thread_count; i++)
            threads.emplace_back(&PatternFinder::worker_thread, this);
    }

    ~PatternFinder() { queue.terminate(); }

    void add_pattern(int row, int col, int row_limit = 1) {
        for (long long b_mask = 1; b_mask < 1LL << (row_limit * col);
             b_mask++) {
            long long rev_b_mask = 0;
            for (int i = 0; i < row_limit * col; i++)
                if (b_mask & 1LL << i)
                    rev_b_mask |= 1LL << (row * col - 1 - i);
            for (int p_cnt = row * col; p_cnt > 0; p_cnt--)
                for (long long w_mask = (1LL << p_cnt) - 1;
                     w_mask < 1LL << (row * col);
                     w_mask = next_permutation(w_mask))
                    if ((~w_mask & rev_b_mask) == rev_b_mask) {
                        board_t board = {row, col, b_mask, w_mask};
                        queue.push(board);
                    }
        }
        queue.join();
    }

    std::vector<std::pair<board_t, int>> get_patterns() {
        return pattern_list.get_patterns();
    }
};
