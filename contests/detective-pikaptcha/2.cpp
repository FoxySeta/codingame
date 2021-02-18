#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

constexpr char wall {'#'}, LEFT {'L'}, RIGHT {'R'};
const vector<char> dir{'^', '>', 'v', '<'};
const vector<pair<int, int>> delta{ {-1, 0}, {0, 1}, {1, 0}, {0, -1} }; // deltaY, deltaX

bool isStart(char);
bool isWalkable(const vector<vector<char>>&, int y, int x);

int main() {
    int width, height;
    cin >> width >> height; cin.ignore();
    vector<vector<char>> grid(height, vector<char>(width));
    int pikaY, pikaX, pikaDir;
    for (int y {0}; y < height; ++y) {
        for (int x {0}; x < width; ++x) {
            cin >> grid.at(y).at(x);
            if (isStart(grid.at(y).at(x))) {
                pikaY = y;
                pikaX = x;
                pikaDir = find(dir.cbegin(), dir.cend(), grid.at(y).at(x)) - dir.cbegin();
            }
        }
        cin.ignore();
    }
    char sideChar;
    cin >> sideChar;
    const int side {sideChar == LEFT ? -1 : 1};
    char pikaFree {'1'};
    do {
        int newPikaDir {(pikaDir + dir.size() + side) % dir.size()},
            newPikaY {pikaY + delta.at(newPikaDir).first},
            newPikaX {pikaX + delta.at(newPikaDir).second};
        if (!isWalkable(grid, newPikaY, newPikaX)) {
            newPikaDir = pikaDir;
            newPikaY = pikaY + delta.at(pikaDir).first;
            newPikaX = pikaX + delta.at(pikaDir).second;
            if (!isWalkable(grid, newPikaY, newPikaX)) {
                newPikaDir = (pikaDir + dir.size() - side) % dir.size();
                newPikaY = pikaY + delta.at(newPikaDir).first;
                newPikaX = pikaX + delta.at(newPikaDir).second;
                if (!isWalkable(grid, newPikaY, newPikaX)) {
                    newPikaDir = (pikaDir + 2) % dir.size();
                    newPikaY = pikaY + delta.at(newPikaDir).first;
                    newPikaX = pikaX + delta.at(newPikaDir).second;
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