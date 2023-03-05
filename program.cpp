#include <chrono>
#include <iostream>

#include "cxxopts.hpp"
#include "pattern_finder.hpp"

int main(int argc, char **argv) {
    cxxopts::Options options("race-patterns",
                             "Multithreaded program to generate race patterns "
                             "for 6x6 Breakthrough game");

    options.add_options()("h,help", "print usage")(
        "t,thread", "thread count", cxxopts::value<int>()->default_value("4"))(
        "r,row", "maximum row to search",
        cxxopts::value<int>()->default_value("3"))(
        "i,input", "manually add patterns")("v,verbose", "verbose output");

    auto args = options.parse(argc, argv);

    if (args.count("help")) {
        std::cout << options.help() << std::endl;
        return 0;
    }

    auto start = std::chrono::high_resolution_clock::now();

    PatternFinder pattern_finder(args["thread"].as<int>(),
                                 args.count("verbose"));
    for (int i = 1; i <= args["row"].as<int>(); i++)
        for (int j = 1; j <= BOARD_COL; j++)
            pattern_finder.add_pattern(i, j);

    auto end = std::chrono::high_resolution_clock::now();

    if (args.count("input")) {
        int row;
        while (std::cin >> row) {
            std::vector<std::string> input(row);
            for (int i = 0; i < row; i++)
                std::cin >> input[i];
            pattern_finder.add_pattern(board::parse(input));
        }
    }

    auto patterns = pattern_finder.get_patterns();
    for (auto &pattern : patterns)
        std::cout << "(" << pattern.second.b_mask << ", "
                  << pattern.second.w_mask << ", " << pattern.first << "),"
                  << std::endl;

    if (args.count("verbose")) {
        for (auto &pattern : patterns)
            std::cerr << board::to_string(pattern.second, true) << std::endl;

        std::cerr << "Total patterns: " << patterns.size() << std::endl;
        std::cerr << "Time taken: "
                  << std::chrono::duration_cast<std::chrono::milliseconds>(
                         end - start)
                         .count()
                  << "ms" << std::endl;
    }

    return 0;
}