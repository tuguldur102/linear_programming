#include <iostream>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <climits>

using std::vector;
using std::cin;
using std::cout;
using std::size_t;
using i64 = long long;

// Balancing function: ensures total supply == total demand
void balance(vector<i64>& a, vector<i64>& b, vector<vector<i64>>& c) {
    i64 sa = std::accumulate(a.begin(), a.end(), 0LL);
    i64 sb = std::accumulate(b.begin(), b.end(), 0LL);
    if (sa == sb) return;

    if (sa > sb) {
        b.push_back(sa - sb);
        for (auto& row : c) row.push_back(0);
    } else {
        a.push_back(sb - sa);
        c.emplace_back(b.size(), 0);
    }
}

// Pretty printer for solution table
void print(const vector<vector<i64>>& x,
           const vector<i64>& a,
           const vector<i64>& b,
           i64 z)
{
    size_t m = x.size(), n = x[0].size();
    cout << "\n   ";
    for (size_t j = 0; j < n; ++j) cout << std::setw(5) << "B" << j+1;
    cout << std::setw(7) << "ai\n";

    for (size_t i = 0; i < m; ++i) {
        cout << "A" << i+1;
        for (size_t j = 0; j < n; ++j)
            cout << std::setw(5) << (x[i][j] ? std::to_string(x[i][j]) : ".");
        cout << std::setw(7) << a[i] << "\n";
    }

    cout << "bj ";
    for (size_t j = 0; j < n; ++j) cout << std::setw(5) << b[j];
    cout << "\n\nTotal cost z = " << z << "\n";
}

// Russell Approximation Method implementation
vector<vector<i64>> russell(const vector<i64>& a0,
                            const vector<i64>& b0,
                            const vector<vector<i64>>& c,
                            i64& totalCost)
{
    size_t m = a0.size(), n = b0.size();
    vector<i64> a = a0, b = b0;
    vector<vector<i64>> x(m, vector<i64>(n, 0));
    vector<char> rowDone(m, 0), colDone(n, 0);
    size_t remainingRows = m, remainingCols = n;

    auto stillOpen = [&](size_t i, size_t j) {
        return !rowDone[i] && !colDone[j];
    };

    while (remainingRows && remainingCols) {
        vector<i64> u(m, LLONG_MIN), v(n, LLONG_MIN);
        for (size_t i = 0; i < m; ++i)
            if (!rowDone[i])
                for (size_t j = 0; j < n; ++j)
                    if (stillOpen(i, j))
                        u[i] = std::max(u[i], c[i][j]);

        for (size_t j = 0; j < n; ++j)
            if (!colDone[j])
                for (size_t i = 0; i < m; ++i)
                    if (stillOpen(i, j))
                        v[j] = std::max(v[j], c[i][j]);

        i64 bestΔ = 0;
        size_t bi = 0, bj = 0;
        bool first = true;

        for (size_t i = 0; i < m; ++i)
            if (!rowDone[i])
                for (size_t j = 0; j < n; ++j)
                    if (stillOpen(i, j)) {
                        i64 Δ = c[i][j] - u[i] - v[j];
                        if (first || Δ < bestΔ ||
                           (Δ == bestΔ && c[i][j] < c[bi][bj])) {
                            bestΔ = Δ; bi = i; bj = j; first = false;
                        }
                    }

        i64 ship = std::min(a[bi], b[bj]);
        x[bi][bj] = ship;
        a[bi] -= ship;
        b[bj] -= ship;

        if (a[bi] == 0) { rowDone[bi] = 1; --remainingRows; }
        if (b[bj] == 0) { colDone[bj] = 1; --remainingCols; }
    }

    totalCost = 0;
    for (size_t i = 0; i < m; ++i)
        for (size_t j = 0; j < n; ++j)
            totalCost += x[i][j] * c[i][j];

    return x;
}

// Main driver to test the method
int main() {
    vector<i64> a = {18, 12, 30};
    vector<i64> b = {8, 8, 8, 28, 8};
    vector<vector<i64>> c = {
        {11, 12, 2, 9, 0},
        {17, 4, 8, 14, 0},
        {3, 6, 15, 5, 0}
    };

    balance(a, b, c); // Make sure it's balanced

    i64 z;
    auto x = russell(a, b, c, z);

    print(x, a, b, z);
    return 0;
}
