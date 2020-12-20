#include "../../jlib/misc/sudoku.h"
#include <chrono>
#include <fstream>
#include <sstream>
#include <stdio.h>


std::vector<std::string> solved_gridss{};
void on_solve(const std::string& grid) {
    solved_gridss.push_back(grid);
}

void print_grid(const char* grid) {
    const std::string sep(6, '-');
    const std::string sepline = sep + "+" + sep + "+" + sep;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            printf("%c ", (grid[i * 9 + j] != '0' ? grid[i * 9 + j] : '.'));
            if (j == 2 || j == 5) {
                printf("|");
            }
        }
        printf("\n");
        if (i == 2 || i == 5) {
            printf("%s\n", sepline.c_str());
        }
    }
    printf("\n");
}

// show_if == -1 dont show board
// show_if = ms >= 0, show board if sovle time exceeds ms
void solve_all(std::vector<std::string> grids, std::string name = "", int show_if = 0, int max_solves = 0) {
    int total = 0, ok = 0;
    long long ms = 0, max_ms = 0;
    jlib::misc::sudoku::Helper helper;
    for (const auto& grid : grids) {
        solved_gridss.clear();
        auto start = std::chrono::steady_clock::now();
        int solves = jlib::misc::sudoku::solve_n(grid.c_str(), on_solve, max_solves, &helper); // 最多求2个解
        auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
        if (show_if != -1 && diff >= show_if) {
            print_grid(grid.c_str());
            if (solves == 0) {
                printf("#%d of puzzle %s has No solution\n", total + 1, name.c_str());
            } else {
                printf("#%d of puzzle %s has %s %d solutions:\n", total + 1, name.c_str(), solves > 1 ? "at least" : "", solves);
                for (const auto& solved_grid : solved_gridss) {
                    print_grid(solved_grid.c_str());
                }
            }
            printf("#%d of puzzle %s, cost %lldms\n", total + 1, name.c_str(), diff);
        }
        ms += diff;
        if (diff > max_ms) { max_ms = diff; }
        total++;
        if (solves) { ok++; }
    }

    if (total > 0) {
        printf("solved %d of %d %s puzzles, total %lldms, avg %.2fms, max %lldms\n", ok, total, name.c_str(), ms, ms * 1.0 / total, max_ms);
    }
}

std::vector<std::string> from_file(std::string path) {
    std::vector<std::string> res;
    std::ifstream in(path);
    if (in.is_open()) {
        std::stringstream ss;
        ss << in.rdbuf();
        in.close();
        auto content = ss.str();
        size_t prev = 0;
        auto pos = content.find_first_of('\n');
        while (pos != content.npos && pos - prev >= 81) {
            auto str = content.substr(prev, 81);
            std::string board;
            for (int i = 0; i < 9; i++) {
                std::string line;
                for (int j = 0; j < 9; j++) {
                    int val = str[i * 9 + j];
                    if ('1' <= val && val <= '9') {
                        line.push_back(val);
                    } else if (val == '.' || val == '0') {
                        line.push_back('.');
                    } else {
                        return res;
                    }
                }
                board += line;
            }
            res.push_back(board);
            prev = pos + 1;
            pos = content.find_first_of('\n', prev);
        }
    }
    return res;
}

int main(int argc, char** argv)
{
    int show_if = 0;
    if (argc > 1) {
        show_if = atoi(argv[1]);
    }

    solve_all(from_file("./easy50.txt"), "easy50", show_if);
    solve_all(from_file("./top95.txt"), "top95", show_if);
    solve_all(from_file("./hardest.txt"), "hardest", show_if);
    solve_all(jlib::misc::sudoku::random_puzzles(99), "random", show_if);


    //std::string grid1 = "003020600900305001001806400008102900700000008006708200002609500800203009005010300";
    std::string grid1 = "..3.2.6..9..3.5..1..18.64....81.29..7.......8..67.82....26.95..8..2.3..9..5.1.3..";
    std::string grid2 = "4.....8.5.3..........7......2.....6.....8.4......1.......6.3.7.5..2.....1.4......";
    std::string hard1 = ".....6....59.....82....8....45........3........6..3.54...325..6.................."; // 有多解。只求单解：924ms on i7-8700, 1400ms on i5-4590

    solve_all(std::vector<std::string>{grid1, grid2, hard1 }, "test", show_if, 1);


}
