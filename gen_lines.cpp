#include <bits/stdc++.h>
using namespace std;

constexpr int Y = 7, X = 9, L = 4;
using P = pair<int, int>;   // (y, x)
using line_t = array<P, L>;
unsigned long l2ul(const line_t &line) {
    unsigned long res = 0;
    for (const auto &[y, x] : line) {
        res |= 1ULL << (7 * x + y);
    }
    return res;
}
int score(const line_t &line) {
    int res = 0;
    for (const auto &[y, x] : line) {
        res += y;
    }
    return res;
}

int main(void) {
    vector<line_t> lines;
    // row
    for (int y = 0; y < Y; ++y) {
        for (int x0 = 0; x0 <= X - L; ++x0) {
            line_t line;
            for (int i = 0; i < L; ++i) line[i] = P{y, x0 + i};
            lines.push_back(line);
        }
    }
    // column
    for (int y0 = 0; y0 <= Y - L; ++y0) {
        for (int x = 0; x < X; ++x) {
            line_t line;
            for (int i = 0; i < L; ++i) line[i] = P{y0 + i, x};
            lines.push_back(line);
        }
    }
    // diag
    for (int y0 = 0; y0 <= Y - L; ++y0) {
        for (int x0 = 0; x0 <= X - L; ++x0) {
            line_t line;
            for (int i = 0; i < L; ++i) line[i] = P{y0 + i, x0 + i};
            lines.push_back(line);
        }
    }
    for (int y0 = 0; y0 <= Y - L; ++y0) {
        for (int x0 = 0; x0 <= X - L; ++x0) {
            line_t line;
            for (int i = 0; i < L; ++i) line[i] = P{y0 + (L - 1 - i), x0 + i};
            lines.push_back(line);
        }
    }

    stable_sort(lines.begin(), lines.end(), [](const line_t &l, const line_t &r) {
        return score(l) < score(r);
    });

    printf("std::array<unsigned long, %ld> lines = { ", lines.size());
    for (const line_t &line : lines) printf("0x%016lx, ", l2ul(line));
    printf("};\n");
}