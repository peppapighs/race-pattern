#pragma once

#include <mutex>
#include <shared_mutex>
#include <vector>

#include "board.hpp"

class PatternList {
  private:
    std::vector<std::pair<board_t, int>> patterns;
    mutable std::shared_mutex mutex;

  public:
    bool contains(const board_t &pattern) const {
        board_t board = board::pad(pattern, 0, BOARD_COL - pattern.col,
                                   BOARD_ROW - pattern.row);
        std::shared_lock<std::shared_mutex> lock(mutex);
        auto p = [&](const std::pair<board_t, int> &b) {
            return board::is_subset(board, b.first);
        };
        return std::find_if(patterns.begin(), patterns.end(), p) !=
               patterns.end();
    }

    void add(const board_t &pattern) {
        for (auto &board : board::expand(pattern)) {
            if (contains(board))
                continue;
            std::unique_lock<std::shared_mutex> lock(mutex);
            patterns.emplace_back(board, pattern.row - 1);
        }
        for (auto &board : board::expand(board::flip(pattern))) {
            if (contains(board))
                continue;
            std::unique_lock<std::shared_mutex> lock(mutex);
            patterns.emplace_back(board, pattern.row - 1);
        }
    }

    std::vector<std::pair<board_t, int>> get_patterns() {
        std::sort(patterns.begin(), patterns.end());

        std::vector<std::pair<board_t, int>> ret;

        for (auto &pattern : patterns) {
            auto p = [&](const std::pair<board_t, int> &b) {
                return board::is_subset(pattern.first, b.first);
            };
            if (std::find_if(ret.begin(), ret.end(), p) != ret.end())
                continue;
            ret.push_back(pattern);
        }
        return ret;
    }
};