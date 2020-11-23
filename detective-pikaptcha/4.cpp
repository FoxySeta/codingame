#include <algorithm>
#include <iostream>
#include <tuple>
#include <vector>

using namespace std;

constexpr char wall {'#'}, LEFT {'L'}, RIGHT {'R'};
const vector<char> dir{'^', '>', 'v', '<'};
const vector<pair<int, int>> delta{ {-1, 0}, {0, 1}, {1, 0}, {0, -1} }; // deltaY, deltaX

bool isStart(char);
bool isWalkable(const vector<vector<char>>&, int y, int x);
tuple<int, int, int> mobius (int oldY, int oldX, int oldDir, int N);

int main() {
    int N;
    cin >> N;
    cin.ignore();
    vector<vector<char>> grid(6 * N, vector<char>(N));
    int pikaY, pikaX, pikaDir;
    for (int y {0}; y < 6 * N; ++y) {
        for (int x {0}; x < N; ++x) {
            cin >> grid.at(y).at(x);
            // cerr << grid.at(y).at(x);
            if (isStart(grid.at(y).at(x))) {
                pikaY = y;
                pikaX = x;
                pikaDir = find(dir.cbegin(), dir.cend(), grid.at(y).at(x)) - dir.cbegin();
            }
        }
        cin.ignore();
        // cerr << '\n';
    }
    char sideChar;
    cin >> sideChar;
    // cerr << sideChar << '\n';
    const int side {sideChar == LEFT ? -1 : 1};
    char pikaFree {'1'};
    do {
        int newPikaDir {(pikaDir + dir.size() + side) % dir.size()},
            newPikaY {pikaY + delta.at(newPikaDir).first},
            newPikaX {pikaX + delta.at(newPikaDir).second};
            auto mob = mobius(newPikaY, newPikaX, newPikaDir, N);
            newPikaY = get<0>(mob);
            newPikaX = get<1>(mob);
            newPikaDir = get<2>(mob);
        if (!isWalkable(grid, newPikaY, newPikaX)) {
            newPikaDir = pikaDir;
            newPikaY = pikaY + delta.at(pikaDir).first;
            newPikaX = pikaX + delta.at(pikaDir).second;
            auto mob = mobius(newPikaY, newPikaX, newPikaDir, N);
            newPikaY = get<0>(mob);
            newPikaX = get<1>(mob);
            newPikaDir = get<2>(mob);
            if (!isWalkable(grid, newPikaY, newPikaX)) {
                newPikaDir = (pikaDir + dir.size() - side) % dir.size();
                newPikaY = pikaY + delta.at(newPikaDir).first;
                newPikaX = pikaX + delta.at(newPikaDir).second;
                auto mob = mobius(newPikaY, newPikaX, newPikaDir, N);
                newPikaY = get<0>(mob);
                newPikaX = get<1>(mob);
                newPikaDir = get<2>(mob);
                if (!isWalkable(grid, newPikaY, newPikaX)) {
                    newPikaDir = (pikaDir + 2) % dir.size();
                    newPikaY = pikaY + delta.at(newPikaDir).first;
                    newPikaX = pikaX + delta.at(newPikaDir).second;
                    auto mob = mobius(newPikaY, newPikaX, newPikaDir, N);
                    newPikaY = get<0>(mob);
                    newPikaX = get<1>(mob);
                    newPikaDir = get<2>(mob);
                    if (!isWalkable(grid, newPikaY, newPikaX)) {
                        pikaFree = '0';
                        newPikaY = pikaY;
                        newPikaX = pikaX;
                        newPikaDir = pikaDir;
                    }
                }
            }
        }
        pikaY = newPikaY;
        pikaX = newPikaX;
        pikaDir = newPikaDir;
        // cerr << pikaDir << '\n';
        if (!isStart(grid.at(pikaY).at(pikaX)))
            ++grid.at(pikaY).at(pikaX);
        /*for (auto y : grid) {
            for (auto x : y)
                cerr << x;
            cerr << '\n';
        }
        cerr << pikaDir << '\n';*/
    } while (!isStart(grid.at(pikaY).at(pikaX)));
    grid.at(pikaY).at(pikaX) = pikaFree;
    for (auto y : grid) {
        for (auto x : y)
            cout << x;
        cout << '\n';
    }
}

bool isStart(char x) {
    return find(dir.cbegin(), dir.cend(), x) != dir.cend();
}

bool isWalkable(const vector<vector<char>>& grid, int y, int x) {
    return y > -1 && y < grid.size() && x > -1 && x < grid.at(y).size() && grid.at(y).at(x) != wall;
}

tuple<int, int, int> mobius (int oldY, int oldX, int oldDir, int N) {
    if (oldY == -1) // 1 N
        return {6 * N - 1, oldX, oldDir};
    if (oldY == N && oldDir == 2) // 1 S
        return {2 * N, oldX, oldDir};
    if (oldY == N - 1 && oldDir == 0) // 2 N
        return {oldX, 0, 1};
    if (oldY == 2 * N && oldDir == 2) // 2 S
        return {5 * N - 1 - oldX, 0, 1};
    if (oldY == 2 * N - 1 && oldDir == 0) // 3 N
        return {N - 1, oldX, 0};
    if (oldY == 3 * N && oldDir == 2) // 3 S
        return {4 * N, oldX, 2};
    if (oldY == 3 * N - 1 && oldDir == 0) // 4 N
        return {N - 1 - oldX, N - 1, 3};
    if (oldY == 4 * N && oldDir == 2) // 4 S
        return {4 * N + oldX, N - 1, 3};
    if (oldY == 4 * N - 1 && oldDir == 0) // 5 N
        return {3 * N - 1, oldX, 0};
    if (oldY == 6 * N) // 6 S
        return {0, oldX, oldDir};
    if (oldX == -1) // W
        switch (oldY / N) {
            case 0: // 1 W
                return {N, oldY, 2};
            case 1: // 2 W
                return {6 * N - 1 - (oldY - N), 0, 1};
            case 2: // 3 W
                return {oldY - N, N - 1, 3};
            case 3: // 4 W
                return {oldY - N, N - 1, 3};
            case 4: // 5 W
                return {2 * N - 1, N - 1 - (oldY - 4 * N), 0};
            case 5: // 6 W
                return {2 * N - 1 - (oldY - 5 * N), 0, 1};
        }
    if (oldX == N) // E
        switch (oldY / N) {
            case 0: // 1 E
                return {3 * N, N - 1 - oldY, 2};
            case 1: // 2 E
                return {oldY + N, 0, 1};
            case 2: // 3 E
                return {oldY + N, 0, 1};
            case 3: // 4 E
                return {6 * N - 1 - (oldY - 3 * N), N - 1, 3};
            case 4: // 5 E
                return {4 * N - 1, oldY - 4 * N, 0};
            case 5: // 6 E
                return {4 * N - 1 - (oldY - 5 * N), N - 1, 3};
        }
    return {oldY, oldX, oldDir};
}