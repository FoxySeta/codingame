#include <algorithm>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

using namespace std;

struct Tree;

struct Cell {
  int index, richness, neigh0, neigh1, neigh2, neigh3, neigh4, neigh5;
  Tree *tree;
};

istream &operator>>(istream &is, Cell &c) {
  return is >> c.index >> c.richness >> c.neigh0 >> c.neigh1 >> c.neigh2 >>
         c.neigh3 >> c.neigh4 >> c.neigh5;
}

struct Tree {
  int cellIndex, size;
  bool isMine, isDormant;
};

istream &operator>>(istream &is, Tree &t) {
  return is >> t.cellIndex >> t.size >> t.isMine >> t.isDormant;
}

struct Action {
  static constexpr char wait_name[]{"WAIT"}, complete_name[]{"COMPLETE"},
      grow_name[]{"GROW"};
  enum class Type { wait, complete, grow } type;
  int index;
};

istream &operator>>(istream &is, Action &a) {
  string name{};
  is >> name;
  if (name == a.wait_name)
    a.type = Action::Type::wait;
  else if (name == a.complete_name) {
    a.type = Action::Type::complete;
    is >> a.index;
  } else if (name == a.grow_name) {
    a.type = Action::Type::grow;
    is >> a.index;
  }
  return is;
}

ostream &operator<<(ostream &os, const Action &a) {
  switch (a.type) {
  case Action::Type::wait:
    return os << a.wait_name;
  case Action::Type::complete:
    return os << a.complete_name << ' ' << a.index;
  case Action::Type::grow:
    return os << a.grow_name << ' ' << a.index;
  }
}

struct Game {
  int day{-1}, nutrients, mySun, myScore, oppSun, oppScore;
  bool oppIsWaiting;
  vector<Cell> cells{};
  vector<Tree> trees{};
  vector<int> treeCountersBySize{0, 0, 0};
  vector<Action> possibleActions{};

  int evaluateAction(Action a) {
    static constexpr int completingCost{4};
    switch (a.type) {
    case Action::Type::complete:
      return nutrients + 2 * (cells.at(a.index).richness - 1);
    case Action::Type::grow:
      return nutrients + 2 * (cells.at(a.index).richness - 1) -
             treeCountersBySize.at(cells.at(a.index).tree->size);
    case Action::Type::wait:
      return 0;
    }
  }

  Action pickAction() {
    const auto &c = cells;
    auto cmp = [this](Action a, Action b) {
      return evaluateAction(a) <= evaluateAction(b);
    };
    sort(possibleActions.begin(), possibleActions.end(), cmp);
    return possibleActions.back();
  }
};

istream &operator>>(istream &is, Game &g) {
  if (g.day == -1) {
    size_t numberOfCells;
    is >> numberOfCells;
    g.cells.resize(numberOfCells);
    for (int i{0}; i < numberOfCells; ++i) {
      Cell x;
      is >> x;
      g.cells.at(x.index) = x;
    }
  }
  for (auto &x : g.cells)
    x.tree = nullptr;
  is >> g.day >> g.nutrients >> g.mySun >> g.myScore >> g.oppSun >>
      g.oppScore >> g.oppIsWaiting;
  size_t numberOfTrees;
  is >> numberOfTrees;
  g.trees.resize(numberOfTrees);
  fill(g.treeCountersBySize.begin(), g.treeCountersBySize.end(), 0);
  for (auto &x : g.trees) {
    is >> x;
    g.cells.at(x.cellIndex).tree = &x;
    ++g.treeCountersBySize.at(x.size - 1);
  }
  size_t numberOfPossibleActions;
  is >> numberOfPossibleActions;
  g.possibleActions.resize(numberOfPossibleActions);
  for (auto &x : g.possibleActions)
    is >> x;
  return is;
}

int main() {
  try {
    Game game{};
    while (true) {
      cin >> game;
      cout << game.pickAction() << endl;
    }
  } catch (exception e) {
    cerr << e.what() << endl;
  }
}
