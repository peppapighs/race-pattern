#pragma once

const int BOARD_ROW = 6;
const int BOARD_COL = 6;

struct board_t {
    int row, col;
    long long b_mask;
    long long w_mask;

    friend bool operator==(const board_t &a, const board_t &b) {
        return a.row == b.row && a.col == b.col && a.b_mask == b.b_mask &&
               a.w_mask == b.w_mask;
    }

    friend bool operator<(const board_t &a, const board_t &b) {
        return std::make_tuple(a.row, a.col, __builtin_popcountll(a.b_mask),
                               -__builtin_popcountll(a.w_mask), a.b_mask,
                               a.w_mask) <
               std::make_tuple(b.row, b.col, __builtin_popcountll(b.b_mask),
                               -__builtin_popcountll(b.w_mask), b.b_mask,
                               b.w_mask);
    }
};

struct board_hash_t {
    std::size_t operator()(const board_t &board) const {
        return std::hash<int>{}(board.row) ^ std::hash<int>{}(board.col) ^
               std::hash<long long>{}(board.b_mask) ^
               std::hash<long long>{}(board.w_mask);
    }
};

namespace board {
bool is_subset(const board_t &a, const board_t &b) {
    return (a.b_mask & b.b_mask) == b.b_mask &&
           (a.w_mask & b.w_mask) == a.w_mask;
}

std::string to_string(const board_t &board, bool is_pattern = true) {
    std::stringstream ss;
    std::string h_line = "+";
    for (int i = 0; i < board.col; i++)
        h_line += "---+";
    ss << h_line << std::endl;
    for (int i = 0; i < board.row; i++) {
        ss << "| ";
        for (int j = 0; j < board.col; j++) {
            int idx = i * board.col + j;
            if (board.b_mask & 1LL << idx)
                ss << "B";
            else if (board.w_mask & 1LL << (board.row * board.col - 1 - idx))
                ss << (is_pattern ? "_" : "W");
            else
                ss << (is_pattern ? "P" : "_");
            ss << " |";
            if (j != board.col - 1)
                ss << " ";
        }
        ss << std::endl << h_line << std::endl;
    }
    return ss.str();
}

board_t flip(const board_t &board) {
    long long b_mask = 0, w_mask = 0;
    for (int i = 0; i < board.row; i++)
        for (int j = 0; j < board.col; j++) {
            int idx = i * board.col + j;
            int flip_idx = (i + 1) * board.col - j - 1;
            if (board.b_mask & 1LL << idx)
                b_mask |= 1LL << flip_idx;
            if (board.w_mask & 1LL << idx)
                w_mask |= 1LL << flip_idx;
        }
    return {board.row, board.col, b_mask, w_mask};
}

board_t pad(const board_t &board, int l, int r, int t) {
    long long b_mask = 0, w_mask = 0;
    for (int i = 0; i < board.row; i++)
        for (int j = 0; j < board.col; j++) {
            if (j + l < 0 || j + l >= board.col + l + r)
                continue;
            int idx = i * board.col + j;
            int pad_idx = (i + t) * (board.col + l + r) + j + l;
            if (board.b_mask & 1LL << idx)
                b_mask |= 1LL << pad_idx;
            if (~board.w_mask & 1LL << (board.row * board.col - 1 - idx))
                w_mask |= 1LL << ((board.row + t) * (board.col + l + r) - 1 -
                                  pad_idx);
        }
    w_mask = (1LL << (board.row + t) * (board.col + l + r)) - 1 - w_mask;
    return {board.row + t, board.col + l + r, b_mask, w_mask};
}

std::vector<board_t> expand(const board_t &board, bool pad_row = true) {
    std::vector<board_t> ret;
    bool has_l_wall = true, has_r_wall = true;
    for (int i = 0; i < board.row; i++) {
        if (board.b_mask & 1LL << (i * board.col))
            has_l_wall = false;
        if (board.b_mask & 1LL << ((i + 1) * board.col - 1))
            has_r_wall = false;
    }
    int row_pad = pad_row ? BOARD_ROW - board.row : 0;
    if (has_l_wall)
        ret.push_back(pad(board, -1, BOARD_COL - board.col + 1, row_pad));
    for (int i = 0; i <= BOARD_COL - board.col; i++)
        ret.push_back(pad(board, i, BOARD_COL - board.col - i, row_pad));
    if (has_r_wall)
        ret.push_back(pad(board, BOARD_COL - board.col + 1, -1, row_pad));
    return ret;
}
} // namespace board