#include <iostream>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <numeric>

using std::cin;
using std::cout;
using std::size_t;
using std::vector;
using i64 = long long;

// ------------------------------------------------------------
// Balance the problem: if total supply ≠ total demand, add a
// dummy source (row) or destination (column) with zero costs.
// ------------------------------------------------------------
void balanceProblem(vector<i64>& supply,
                    vector<i64>& demand,
                    vector<vector<i64>>& cost)
{
    i64 totalSupply  = std::accumulate(supply.begin(),  supply.end(),  0LL);
    i64 totalDemand  = std::accumulate(demand.begin(),  demand.end(),  0LL);

    if (totalSupply == totalDemand) return;

    if (totalSupply > totalDemand) {               // add dummy destination
        demand.push_back(totalSupply - totalDemand);
        for (auto& row : cost) row.push_back(0);
    } else {                                       // add dummy source
        supply.push_back(totalDemand - totalSupply);
        cost.emplace_back(demand.size(), 0);
    }
}

// ------------------------------------------------------------
// North‑West Corner algorithm
// ------------------------------------------------------------
vector<vector<i64>> northwestCorner(const vector<i64>& supplyInit,
                                    const vector<i64>& demandInit,
                                    const vector<vector<i64>>& cost,
                                    i64& totalCostOut)
{
    size_t m = supplyInit.size();
    size_t n = demandInit.size();

    vector<i64> supply = supplyInit;   // working copies
    vector<i64> demand = demandInit;

    vector<vector<i64>> x(m, vector<i64>(n, 0));   // shipment matrix

    size_t i = 0, j = 0;
    while (i < m && j < n) {
        i64 ship = std::min(supply[i], demand[j]);
        x[i][j] = ship;

        supply[i] -= ship;
        demand[j] -= ship;

        if (supply[i] == 0) ++i;       // move down
        else               ++j;        // move right
    }

    // cost
    i64 z = 0;
    for (size_t r = 0; r < m; ++r)
        for (size_t c = 0; c < n; ++c)
            z += x[r][c] * cost[r][c];

    totalCostOut = z;
    return x;
}

// ------------------------------------------------------------
// Helper for pretty printing a matrix
// ------------------------------------------------------------
void printTable(const vector<vector<i64>>& x,
                const vector<i64>& supply,
                const vector<i64>& demand,
                i64 totalCost)
{
    size_t m = x.size();
    size_t n = x[0].size();

    cout << "\nAllocation table (blank = 0):\n";
    cout << std::setw(6) << " ";
    for (size_t j = 0; j < n; ++j) cout << std::setw(6) << "B" << j+1;
    cout << std::setw(8) << "ai\n";

    for (size_t i = 0; i < m; ++i) {
        cout << std::setw(5) << "A" << i+1;
        for (size_t j = 0; j < n; ++j)
            cout << std::setw(6) << (x[i][j] ? std::to_string(x[i][j]) : "");
        cout << std::setw(8) << supply[i] << "\n";
    }

    cout << std::setw(6) << "bj";
    for (size_t j = 0; j < n; ++j) cout << std::setw(6) << demand[j];
    cout << "\n\nTotal cost z = " << totalCost << "\n";
}

// ------------------------------------------------------------
// main(): simple demo driver
// ------------------------------------------------------------
int main() {
    std::ios::sync_with_stdio(false);

    size_t m, n;
    cout << "m (origins)  n (destinations): ";
    cin  >> m >> n;

    vector<i64> supply(m), demand(n);
    vector<vector<i64>> cost(m, vector<i64>(n));

    cout << "Enter supplies ai: ";
    for (auto& v : supply) cin >> v;

    cout << "Enter demands bj: ";
    for (auto& v : demand) cin >> v;

    cout << "Enter cost matrix cij row‑wise:\n";
    for (size_t i = 0; i < m; ++i)
        for (size_t j = 0; j < n; ++j)
            cin >> cost[i][j];

    // 1) balance
    balanceProblem(supply, demand, cost);

    // 2) NW corner
    i64 totalCost;
    auto alloc = northwestCorner(supply, demand, cost, totalCost);

    // 3) output
    printTable(alloc, supply, demand, totalCost);
    return 0;
}
