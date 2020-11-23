#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <algorithm>

using namespace std;

enum BuildingType {
    HQ
};

enum CommandType {
    WAIT,
    MOVE,
    TRAIN
};

ostream &operator<<(ostream &os, CommandType cmdType) {
    switch (cmdType) {
        case WAIT:
            return os << "WAIT";
        case MOVE:
            return os << "MOVE";
        case TRAIN:
            return os << "TRAIN";
    }
    return os;
}

class Position {
public:

    int x, y;

    Position(int x, int y) : x(x), y(y) {
    }
};

class Command {

public:

    CommandType t;
    Position p;
    int idOrLevel;

    Command(CommandType t, int idOrLevel, const Position &p) : t(t), idOrLevel(idOrLevel), p(p) {
    }

    void print() {
        cout << t << " " << idOrLevel << " " << p.x << " " << p.y << ";";
    }
};

class Unit {

public:

    int id;
    int owner;
    int level;
    Position p;

    Unit(int x, int y, int id, int level, int owner) : p(x, y), id(id), level(level), owner(owner) {
    }

    void debug() {
        cerr << "unit of level " << level << " at " << p.x << " " << p.y << " owned by " << owner;
    }

    bool isOwned() {
        return owner == 0;
    }
};

class Building {

public:

    Position p;
    BuildingType t;
    int owner;

    Building(int x, int y, int t, int owner) : p(x, y), t(static_cast<BuildingType>(t)), owner(owner) {
    }

    void debug() {
        cerr << t << " at " << p.x << " " << p.y << " owned by " << owner;
    }

    bool isHQ() {
        return t == HQ;
    }

    bool isOwned() {
        return owner == 0;
    }
    
};

class Game {
    constexpr static int size {12};
    
    vector<vector<char>> grid = vector<vector<char>>(size, vector<char>(size));

    list<Unit> units;

    list<Building> buildings;

    int gold, income;

    list<Command> commandes;

public:

    void debug() {
        for_each(units.begin(), units.end(), [](Unit &u) { u.debug(); });
        for_each(buildings.begin(), buildings.end(), [](Building &u) { u.debug(); });
    }

    // not useful in Wood 3
    void init() {
        int numberMineSpots;
        cin >> numberMineSpots; cin.ignore();
        for (int i = 0; i < numberMineSpots; i++) {
            int x;
            int y;
            cin >> x >> y; cin.ignore();
        }
    }

    void update() {

        units.clear();
        buildings.clear();
        commandes.clear();

        // READ TURN INPUT
        cin >> gold; cin.ignore();
        cin >> income; cin.ignore();

        int opponentGold;
        cin >> opponentGold; cin.ignore();
        int opponentIncome;
        cin >> opponentIncome; cin.ignore();

        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; ++j)
                cin >> grid.at(j).at(i);
            cin.ignore();
            // cerr << line << endl;
        }
        int buildingCount;
        cin >> buildingCount; cin.ignore();
        for (int i = 0; i < buildingCount; i++) {
            int owner;
            int buildingType;
            int x;
            int y;
            cin >> owner >> buildingType >> x >> y; cin.ignore();
            buildings.push_back(Building(x, y, buildingType, owner));
        }
        int unitCount;
        cin >> unitCount; cin.ignore();
        for (int i = 0; i < unitCount; i++) {
            int owner;
            int unitId;
            int level;
            int x;
            int y;
            cin >> owner >> unitId >> level >> x >> y; cin.ignore();
            units.push_back(Unit(x, y, unitId, level, owner));
        }
    }

    void buildOutput() {
        // @TODO "core" of the AI
        moveUnits();
        trainUnits();
    }

    void output() {
        for_each(commandes.begin(), commandes.end(), [](Command &c) {
            c.print();
        });
        cout << "MSG Team Ice FTW!" <<  endl;
    }


private:

    void trainUnits() {
        while (gold >= 5) {
            int level;
            if (gold < 30)
                level = 1;
            else if (gold < 50)
                level = 2;
            else
                level = 3;
            gold -= level * 10;
            commandes.push_back(Command(TRAIN, level, findTrainingPosition()));
        }
    }
    
    void moveUnits() {
        const Position &OpponentHQ = getOpponentHQ().p;
        for (auto &unit : units) {
            if (unit.isOwned()) {
                Position target = OpponentHQ;
                for (int i = unit.p.x - 1; i <= unit.p.x + 1; ++i)
                    for (int j = unit.p.y - 1; j <= unit.p.y + 1; ++j) {
                        if ((i == unit.p.x) != (j == unit.p.y) && i >= 0 && j >= 0 && i < size && j < size && grid.at(i).at(j) != '#' && grid.at(i).at(j) != 'O') {
                            // cerr << i << " " << j << "\n";
                            bool valid = true;
                            for (auto a : units)
                                if (i == a.p.x && j == a.p.y)
                                    valid = false;
                            if (valid)
                                target = {i, j};
                        }
                    }
                commandes.push_back(Command(MOVE, unit.id, target));
                if (target.x != OpponentHQ.x || target.y != OpponentHQ.y)
                    grid.at(target.x).at(target.y) = 'O';
            }
        }
    }

    // train near the HQ for now
    Position findTrainingPosition() {
        srand(time(NULL));
        int x, y;
        bool valid;
        do {
            do {
                x = rand() % size;
                y = rand() % size;
            } while (!(x || y) || (x == size - 1 && y == size - 1));
            valid = false;
            for (int i = x - 1; !valid && i <= x + 1; ++i)
                for (int j = y - 1; !valid && j <= y + 1; ++j)
                    if ((i == x || j == y) && i >= 0 && i < size && j >= 0 && j < size && grid.at(j).at(i) == 'O')
                        valid = true;
            if (valid) {
                for (auto a : buildings)
                    if (a.p.x == x && a.p.y == y) {
                        valid = false;
                        break;
                    }
            }
            if (valid) {
                for (auto a : units)
                    if (a.p.x == x && a.p.y == y) {
                        valid = false;
                        break;
                    }
            }
        } while (!valid);
        units.push_back(Unit(x, y, 0, 1, 0));
        return {x, y};
    }

    const Building &getHQ() {
        for (auto &b : buildings) {
            if (b.isHQ() && b.isOwned()) {
                return b;
            }
        }
    }

    const Building &getOpponentHQ() {
        for (auto &b : buildings) {
            if (b.isHQ() && !b.isOwned()) {
                return b;
            }
        }
    }
};

int main() {
    Game g;
    g.init();

    // game loop
    while (true) {
        g.update();
        //g.debug();
        g.buildOutput();
        g.output();
    }
    return 0;
}