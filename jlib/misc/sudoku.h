#pragma once

#include <string.h> // memset
#include <initializer_list>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include "../util/rand.h"

namespace jlib {
namespace misc {
namespace sudoku {

constexpr int N = 81; // 81 格
constexpr int NEIGHBORS = 20; // 每格有20个邻居
constexpr int GROUPS_OF = 3; // 每格属于3个组
constexpr int GROUPS = 27; // 27个组 = 9行+9列+9块

// 辅助结构体
// 内部数据初始化后就不会变化了，批量求多个数独时可以复用同一个 Helper
struct Helper {
	int neighbors[N][NEIGHBORS]; 
	int groups_of[N][GROUPS_OF]; 
	int groups[GROUPS][9]; 

    std::default_random_engine rng = jlib::seeded_random_engine();

    // 初始化辅助结构体，用户调用 solve 之前手动调用一次即可
    Helper() {
        memset(neighbors, -1, sizeof(neighbors));
        memset(groups, -1, sizeof(groups));
        memset(groups_of, -1, sizeof(groups_of));

        // (1) 初始化组: 每个cell都有3个组：同行，同列，同块
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                int k = i * 9 + j;
                for (int g : { i, 9 + j, 18 + i / 3 * 3 + j / 3 }) { // g: 组号
                    for (int gg = 0; gg < 9; gg++) {
                        if (groups[g][gg] == k) { break; } // 去重
                        if (groups[g][gg] == -1) {
                            groups[g][gg] = k;
                            break;
                        }
                    }
                    for (int gg = 0; gg < GROUPS_OF; gg++) {
                        if (groups_of[k][gg] == g) { break; } // 去重
                        if (groups_of[k][gg] == -1) {
                            groups_of[k][gg] = g;
                            break;
                        }
                    }
                }
            }
        }

        // (2) 初始化邻居
        for (int k = 0; k < N; k++) { // k: cells 下标
            for (int i = 0; i < 3; i++) {
                int g = groups_of[k][i]; // g: 组号
                for (int j = 0; j < 9; j++) {
                    int kk = groups[g][j]; // kk: 组内 cell 下标
                    if (kk != k) {
                        for (int n = 0; n < NEIGHBORS; n++) {
                            if (neighbors[k][n] == kk) { break; } // 去重
                            if (neighbors[k][n] == -1) {
                                neighbors[k][n] = kk;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
};


// 第 k 个 cell 是否可以选择 val
inline bool cell_on(bool cells[N][9], int k, int val) {
    return cells[k][val - 1];
}

// 消除第 k 个 cell 的可选值 val
inline void cell_eliminate(bool cells[N][9], int k, int val) {
    cells[k][val - 1] = false;
}

// 找到第 k 个 cell 的第一个可选值
inline int cell_val(bool cells[N][9], int k) {
    for (int i = 0; i < 9; i++) { if (cells[k][i]) { return i + 1; } }
    return -1;
}

// 计算第 k 个 cell 的可选数值数量
inline int cell_count(bool cells[N][9], int k) {
    int n = 0;
    for (int i = 0; i < 9; i++) { if (cells[k][i]) { n++; } }
    return n;
}

// 获取可选数量最少的 cell
inline int least_cell_count(bool cells[N][9]) {
    int k = -1, n;
    for (int i = 0; i < N; i++) {
        int nn = cell_count(cells, i);
        if (nn > 1 && (k == -1 || nn < n)) {
            k = i; n = nn;
        }
    }
    return k;
}

// 是否已解
inline bool solved(bool cells[N][9]) {
    for (int k = 0; k < N; k++) {
        if (cell_count(cells, k) != 1) { return false; }
    }
    return true;
}


inline bool eliminate(bool cells[N][9], int k, int val, Helper* helper);

inline bool assign(bool cells[N][9], int k, int val, Helper* helper) {
    for (int i = 1; i <= 9; i++) {
        if (i != val) {
            if (!eliminate(cells, k, i, helper)) {
                return false;
            }
        }
    }
    return true;
}

inline bool eliminate(bool cells[N][9], int k, int val, Helper* helper) {
    if (!cell_on(cells, k, val)) { return true; }
    cell_eliminate(cells, k, val);

    // (1) If a square is reduced to one value v, then eliminate v from the peers.
    int n = cell_count(cells, k);
    if (n == 0) { // 若移除 val 后 k 没有其他备选了，说明失败了
        return false;
    } else if (n == 1) { // 若移除 val 后 k 仅剩一个备选 v，那么 k 的邻居们就都不能放置 v 了
        int v = cell_val(cells, k);
        for (int i = 0; i < NEIGHBORS; i++) { // 尝试移除邻居们的备选元素 v
            if (!eliminate(cells, helper->neighbors[k][i], v, helper)) {  // 无法移除必然失败
                return false;
            }
        }
    }

    // (2) If a unit u is reduced to only one place for a value val, then put it there.
    for (int i = 0; i < GROUPS_OF; i++) { // 遍历 k 所在的组
        int g = helper->groups_of[k][i];
        int n = 0, k1;
        for (int j = 0; j < 9; j++) { // 遍历组内元素 kk
            int kk = helper->groups[g][j];
            if (cell_on(cells, kk, val)) { // 记录 val 在该组内出现的次数 n
                n++; k1 = kk; // 顺便记录仅能放置 val 的位置 k1
            }
        }
        if (n == 0) { // k 所在的组内至少要出现一次 val
            return false; // 否则不满足数独的特性：每个组内都要包含 1~9 等9个数字
        } else if (n == 1) { // 若组内仅有一个位置 k1 可以放置 val了，尝试放置
            if (!assign(cells, k1, val, helper)) { // 不能放置必然失败
                return false;
            }
        }
    }

    return true;
}

inline bool read_grid(const std::string& grid, bool cells[N][9], Helper* helper) {
    if (grid.size() != 81) { return false; }
    for (int i = 0; i < 81; i++) {
        if ('1' <= grid[i] && grid[i] <= '9') {
            if (!assign(cells, i, grid[i] - '0', helper)) {
                return false;
            }
        } else if ('0' == grid[i] || '.' == grid[i]) {

        } else {
            return false;
        }
    }
    return true;
}

inline bool search(bool cells[N][9], Helper* helper) {
    if (solved(cells)) { return true; }
    int k = least_cell_count(cells);
    for (int val = 1; val <= 9; val++) {
        if (cell_on(cells, k, val)) {
            bool cells1[N][9];
            memcpy(cells1, cells, sizeof(cells1));
            if (assign(cells1, k, val, helper)) {
                if (search(cells1, helper)) {
                    memcpy(cells, cells1, sizeof(cells1));
                    return true;
                }
            }
        }
    }
    return false;
}

inline std::string cells_to_grid(bool cells[N][9]) {
    std::string grid(81, '.');
    for (int i = 0; i < N; i++) {
        int val = cell_val(cells, i);
        if (1 <= val && val <= 9) {
            grid[i] = val + '0';
        }
    }
    return grid;
}


//////////////////////////  求数独单解 ///////////////////////////////

// 解数独，找到一个解就停止
inline bool solve(const std::string& grid, std::string& solved_grid, Helper* helper = nullptr) {
    if (!helper) {
        static Helper h;
        helper = &h;
    }
    bool cells[N][9];
    memset(cells, true, sizeof(cells));
    if (!read_grid(grid, cells, helper)) {
        return false;
    }
    if (search(cells, helper)) {
        solved_grid = cells_to_grid(cells);
        return true;
    }
    return false;
}


//////////////////////////  求数独多解 ///////////////////////////////

typedef void(*OnSolved)(const std::string& grid);

inline int search_n(bool cells[N][9], OnSolved on_solved, int solves, int max_solves, Helper* helper) {
    if (solved(cells)) {
        if (on_solved) {
            on_solved(cells_to_grid(cells));
        }
        return ++solves;
    }
    int k = least_cell_count(cells);
    for (int val = 1; val <= 9; val++) {
        if (cell_on(cells, k, val)) {
            bool cells1[N][9];
            memcpy(cells1, cells, sizeof(cells1));
            if (assign(cells1, k, val, helper)) {
                solves += search_n(cells1, on_solved, 0, max_solves, helper);
                if (max_solves != 0 && solves >= max_solves) {
                    return solves;
                }
            }
        }
    }
    return solves;
}

// 求数独的多个解
// max_solves, 0 for solve all solutions, > 0 for stop if reached
inline int solve_n(const std::string& grid, OnSolved on_solved, int max_solves = 0, Helper* helper = nullptr) {
    if (!helper) {
        static Helper h;
        helper = &h;
    }
    bool cells[N][9];
    memset(cells, true, sizeof(cells));
    if (!read_grid(grid, cells, helper)) {
        return 0;
    }
    return search_n(cells, on_solved, 0, max_solves, helper);
}


//////////////////////////  生成数独 ///////////////////////////////

// 随机获取可选数量最少的 cell
inline int random_least_cell_count(bool cells[N][9], Helper* helper) {
    int ks[N], ki = 0, n = 9; ks[0] = 0;
    for (int i = 0; i < N; i++) {
        int nn = cell_count(cells, i);
        if (nn > 1) {
            if (nn < n) {
                n = nn; ki = 0; ks[ki++] = i;
            } else if (nn == n) {
                ks[ki++] = i;
            }
        }
    }
    std::shuffle(ks, ks + ki, helper->rng);
    return ks[0];
}

inline bool random_search(bool cells[N][9], Helper* helper) {
    if (solved(cells)) { return true; }
    int k = random_least_cell_count(cells, helper);
    for (int val = 1; val <= 9; val++) {
        if (cell_on(cells, k, val)) {
            bool cells1[N][9];
            memcpy(cells1, cells, sizeof(cells1));
            if (assign(cells1, k, val, helper)) {
                if (random_search(cells1, helper)) {
                    memcpy(cells, cells1, sizeof(cells1));
                    return true;
                }
            }
        }
    }
    return false;
}

// 随机生成一个已解决的（所有数字已经填满的）数独
inline std::string random_solved_puzzle(Helper* helper) {
    bool cells[N][9];
    memset(cells, true, sizeof(cells));
    if (random_search(cells, helper)) {
        return cells_to_grid(cells);
    } else {
        return random_solved_puzzle(helper);
    }
}

// 随机生成一个有唯一解的数独 puzzle
inline std::string random_puzzle(Helper* helper = nullptr) {
    if (!helper) {
        static Helper h;
        helper = &h;
    }
    auto solved_grid = random_solved_puzzle(helper);
    std::vector<int> random_N = [&helper]() {
        std::vector<int> n(N, 0);
        for (int i = 0; i < N; i++) { n[i] = i; }
        std::shuffle(n.begin(), n.end(), helper->rng);
        return n;
    }();

    for (auto k : random_N) { // 随机挖洞
        char old = solved_grid[k];
        solved_grid[k] = '.'; // 挖洞
        if (solve_n(solved_grid, nullptr, 2) > 1) { // 有多解则恢复，继续挖
            solved_grid[k] = old;
        }
    }
    if (solve_n(solved_grid, nullptr, 2) == 1) {
        return solved_grid;
    }
    return random_puzzle(helper);
}

inline std::vector<std::string> random_puzzles(int n, Helper* helper = nullptr) {
    if (!helper) {
        static Helper h;
        helper = &h;
    }
    std::vector<std::string> puzzles;
    for (int i = 0; i < n; i++) {
        puzzles.push_back(random_puzzle(helper));
    }
    return puzzles;
}

}
}
}
