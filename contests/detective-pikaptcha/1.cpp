#include <iostream>
#include <vector>

using namespace std;

int main() {
    constexpr char wall {'#'};
    
    int width, height;
    cin >> width >> height;
    cerr << width << ' ' << height;
    cin.ignore();
    vector<vector<char>> grid(height, vector<char>(width));
    for (int i {0}; i < height; ++i) {
        for (int j {0}; j < width; ++j){
            cin >> grid.at(i).at(j);
            if (grid.at(i).at(j) != wall) {
                if (i && grid.at(i - 1).at(j) != wall) { // N
                    ++grid.at(i - 1).at(j);
                    ++grid.at(i).at(j);
                }
                if (j && grid.at(i).at(j - 1) != wall) { // W
                    ++grid.at(i).at(j - 1);
                    ++grid.at(i).at(j);
                }
            }
            if (i)
                cout << grid.at(i - 1).at(j);
        }
        cin.ignore();
        if (i)
            cout << '\n';
    }
    for (auto x : grid.at(height - 1))
        cout << x;
    cout << '\n';
/*    for (auto y : grid)
        for (auto x : y)
            cout << x;*/
}