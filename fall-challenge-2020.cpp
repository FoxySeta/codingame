#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include <deque>
#include <unordered_set>
#include <unordered_map>

#pragma GCC optimize("Ofast")
#pragma GCC optimize("inline")
#pragma GCC optimize("omit-frame-pointer")
#pragma GCC optimize("unroll-loops")
#pragma ide diagnostic ignored "EndlessLoop"

#define CAN_DO_ACTION(INV, ACTION) \
    (((INV).inv.inv0 + (ACTION).delta0 >= 0) && \
    ((INV).inv.inv1 + (ACTION).delta1 >= 0) && \
    ((INV).inv.inv2 + (ACTION).delta2 >= 0) && \
    ((INV).inv.inv3 + (ACTION).delta3 >= 0) && \
    ((INV).inv.inv0 + (ACTION).delta0 + (INV).inv.inv1 + (ACTION).delta1 + \
    (INV).inv.inv2 + (ACTION).delta2 + (INV).inv.inv3 + (ACTION).delta3) < 11)

#define APPLY_ACTION(INV, ACTION) \
if ((ACTION).actionType == Cast) { \
    (INV).inv.inv0 += (ACTION).delta0; \
    (INV).inv.inv1 += (ACTION).delta1; \
    (INV).inv.inv2 += (ACTION).delta2; \
    (INV).inv.inv3 += (ACTION).delta3; \
}

#define REST_ACTION_ID (100)

#define SPELLS_TO_LEARN (10)
#define TREE_DEPTH (8)
#define MIN_PRICE (9)
#define MIN_INV_FOR_SEARCH (3)

using namespace std;

enum ActionType {
    Brew,
    Cast,
    OpponentCast,
    Learn,
};

inline string ActionToString(ActionType a){
    switch (a) {
        case Brew:   return "BREW";
        case Cast:   return "CAST";
        case OpponentCast: return "OPPONENT_CAST";
        case Learn: return "LEARN";
    }
    throw runtime_error("Invalid Action");
}

inline ActionType StringToAction(const string &str) {
    if (str == "BREW")
        return Brew;
    if (str == "CAST")
        return Cast;
    if (str == "OPPONENT_CAST")
        return OpponentCast;
    if (str == "LEARN")
        return Learn;
    throw runtime_error("Invalid string for Action");
}

struct PlayerInfo {
    int inv0{ 0 }, // tier-0 ingredients in inventory
        inv1{ 0 },
        inv2{ 0 },
        inv3{ 0 },
        score{ 0 }; // amount of rupees

    inline static void readNextPlayerInfo(PlayerInfo &p) {
        cin >> p.inv0 >> p.inv1 >> p.inv2 >> p.inv3 >> p.score;
        cin.ignore();
    }
};

struct Action {
    int actionId; // the unique ID of this spell or recipe
    ActionType actionType; // in the first league: BREW; later: CAST, OPPONENT_CAST, LEARN, BREW
    int delta0, // tier-0 ingredient change
        delta1, // tier-1 ingredient change
        delta2, // tier-2 ingredient change
        delta3, // tier-3 ingredient change
        price, // the price in rupees if this is a potion
        tomeIndex, // in the first two leagues: always 0; later: the index in the tome if this is a tome spell, equal to the read-ahead tax
        taxCount; // in the first two leagues: always 0; later: the amount of taxed tier-0 ingredients you gain from learning this spell
    bool castable, // in the first league: always 0; later: 1 if this is a castable player spell
         repeatable; // for the first two leagues: always 0; later: 1 if this is a repeatable player spell

    inline static Action readNextAction() {
        Action a{ };
        string actionType;
        cin >> a.actionId >> actionType >> a.delta0 >> a.delta1 >> a.delta2 >> a.delta3 >> a.price >> a.tomeIndex >>
            a.taxCount >> a.castable >> a.repeatable; cin.ignore();
        a.actionType = StringToAction(actionType);
        return a;
    }

    inline static void readNextActions(Action (&actions)[], const int actionCount) {
        for (int i{ 0 }; i < actionCount; ++i)
            actions[i] = readNextAction();
    }
};

union InvUnion {
    unsigned int packedInv;
    struct {
        unsigned char inv0, inv1, inv2, inv3;
    } inv;

    InvUnion() {
        packedInv = inv.inv0 = inv.inv1 = inv.inv2 = inv.inv3 = 0;
    }
};

static const vector<int> EMPTY_VECTOR = vector<int>();

struct SearchNode {
    InvUnion inv;
    vector<int> actions; // -100 = rest
    unsigned char depth{ 0 };
    SearchNode(const InvUnion &inv, const vector<int> &actions) : inv(inv), actions(actions) {}
};

inline vector<int> bfs(const InvUnion startInv, const vector<Action> &casts, const vector<Action> &brews, const int maxDepth) {
    deque<SearchNode> nodes{ deque<SearchNode>() };
    nodes.emplace_back(startInv, vector<int>());
    InvUnion tmpInv;
    while (!nodes.empty()) {
        const SearchNode node{ nodes.front() };
        unordered_set<int> exhaustedActions{ };
        nodes.pop_front();
        bool restPresent{ false };
        for (auto it = node.actions.rbegin(); it != node.actions.rend(); ++it) {
            int value{ *it };
            if (value == REST_ACTION_ID) {
                restPresent = true;
                break;
            }
            exhaustedActions.insert(value);
        }
        for (int cast{ 0 }; cast < casts.size(); ++cast) {
            tmpInv = node.inv;
            if ((restPresent || casts[cast].castable) && (exhaustedActions.find(cast) == exhaustedActions.end() || node.actions.back() == cast && casts[cast].repeatable)) {
                if (CAN_DO_ACTION(tmpInv, casts[cast])) {
                    APPLY_ACTION(tmpInv, casts[cast]);
                    for (int brew{ 0 }; brew < brews.size(); ++brew) {
                        if (brews[brew].price + brews[brew].tomeIndex > MIN_PRICE && tmpInv.inv.inv0 >= -brews[brew].delta0 && tmpInv.inv.inv1 >= -brews[brew].delta1 &&
                            tmpInv.inv.inv2 >= -brews[brew].delta2 && tmpInv.inv.inv3 >= -brews[brew].delta3) {
                            vector<int> actions{ node.actions };
                            actions.push_back(cast);
                            actions.push_back(-brew - 1);
                            std::reverse(actions.begin(), actions.end());
                            return actions;
                        }
                    }
                    if (node.depth < maxDepth) {
                        SearchNode searchNode{ SearchNode(tmpInv, node.actions) };
                        if (!node.actions.empty() && node.actions.back() == cast && casts[cast].repeatable) {
                            searchNode.depth = node.depth;
                            searchNode.actions.push_back(cast);
                            nodes.push_front(searchNode);
                        } else {
                            searchNode.depth = node.depth + 1;
                            searchNode.actions.push_back(cast);
                            nodes.push_back(searchNode);
                        }
                    }
                }
            }
            if (!exhaustedActions.empty()) {
                SearchNode searchNode{ SearchNode(node.inv, node.actions) };
                searchNode.depth = node.depth + 1;
                searchNode.actions.push_back(REST_ACTION_ID);
                nodes.push_back(searchNode);
            }
        }
    }
    return EMPTY_VECTOR;
}

inline vector<Action> getAllCastAndLearnAsCast(const Action (&actions)[], const int actionCount) {
    vector<Action> a{ };
    for (int i{ 0 }; i < actionCount; ++i) {
        const Action &it{ actions[i] };
        if (it.actionType == Cast)
            a.push_back(it);
        else if (it.actionType == Learn) {
            a.push_back(it);
            a.back().actionType = Cast;
        }
    }
    return a;
}

inline vector<Action> getAllCast(const Action (&actions)[], const int actionCount) {
    vector<Action> a{ };
    for (int i{ 0 }; i < actionCount; ++i) {
        const Action &it{ actions[i] };
        if (it.actionType == Cast)
            a.push_back(it);
    }
    return a;
}

inline vector<Action> getAllBrews(const Action (&actions)[], const int actionCount) {
    vector<Action> a{ vector<Action>() };
    for (int i{ 0 }; i < actionCount; ++i) {
        const Action &it{ actions[i] };
        if (it.actionType == Brew)
            a.push_back(it);
    }
    return a;
}

inline deque<int> convertTreeSteps(const deque<int> &steps, const vector<Action> &initialCastsAndLearn) {
    deque<int> convertedSteps{ };
    for (int i{ 0 }; i < steps.size(); ++i) {
        const int step{ steps[i] };
        if (step == REST_ACTION_ID)
            convertedSteps.push_back(-1); // rest
        else {
            const Action &a{ initialCastsAndLearn[step] };
            convertedSteps.push_back(a.actionId);
        }
    }
    return convertedSteps;
}

int main()
{
    PlayerInfo me, enemy;
    deque<int> steps{ };
    int targetId{ -1 },
        actionCount, // the number of spells and recipes in play
        round{ 0 },
        targetIdx{ -1 };
    unordered_map<unsigned char, unsigned char> learnToCastIdx{ };
    InvUnion invToSearch{ };

    while (true) {
        cin >> actionCount;
        cin.ignore();
        Action actions[actionCount];
        Action::readNextActions(reinterpret_cast<Action (&)[]>(actions), actionCount);
        PlayerInfo::readNextPlayerInfo(me);
        PlayerInfo::readNextPlayerInfo(enemy);
        invToSearch.inv.inv0 = me.inv0;
        invToSearch.inv.inv1 = me.inv1;
        invToSearch.inv.inv2 = me.inv2;
        invToSearch.inv.inv3 = me.inv3;
        if (round < SPELLS_TO_LEARN) {
            steps.clear();
            for (int i{ 0 }; i < actionCount; ++i)
                if (actions[i].actionType == Learn && !actions[i].tomeIndex) {
                    steps.push_back(actions[i].actionId);
                    break;
                }
        } else if (targetIdx == -1 && steps.empty() || actions[targetIdx].actionId != targetId) {
            steps.clear();
            const vector<Action> &casts = getAllCast(reinterpret_cast<Action (&)[]>(actions), actionCount),
                                 &brews = getAllBrews(reinterpret_cast<Action (&)[]>(actions), actionCount);
            if ((invToSearch.inv.inv0 + invToSearch.inv.inv1 + invToSearch.inv.inv2 + invToSearch.inv.inv3) < MIN_INV_FOR_SEARCH) {
                for (auto &a : casts)
                    if (a.castable && a.actionType == Cast && CAN_DO_ACTION(invToSearch, a)) {
                        steps.push_front(a.actionId);
                        break;
                    }
                if (steps.empty())
                    steps.push_front(-1);
            } else {
                const vector<int> solution{ bfs(invToSearch, casts, brews, TREE_DEPTH) };
                if (solution.empty()) {
                    for (auto &a : casts)
                        if (a.castable && a.actionType == Cast && CAN_DO_ACTION(invToSearch, a)) {
                            steps.push_front(a.actionId);
                            break;
                        }
                    if (steps.empty())
                        steps.push_front(-1);
                } else {
                    const int brewIdx{ -solution.front() - 1 };
                    const Action &brew{ brews[brewIdx] };
                    targetId = brew.actionId;
                    for (int i{ 0 }; i < actionCount; ++i) {
                        const Action &a{ actions[i] };
                        if (a.actionId == targetId) {
                            targetIdx = i;
                            break;
                        }
                    }
                    for (int i = 1; i < solution.size(); ++i)
                        steps.push_front(solution[i]);
                    steps = convertTreeSteps(steps, casts);
                    steps.push_back(targetId);
                }
            }
        }
        if (!steps.empty()) {
            int actionId{ steps.front() },
                actionIdx{ -1 };
            if (actionId == -1)
                cout << "REST _(*_*)_ zzz...\n";
            else {
                int nbRepeat{ 0 };
                while (steps.size() > 1 && actionId == steps[1]) {
                    ++nbRepeat;
                    steps.pop_front();
                }
                for (int i{ 0 }; i < actionCount; ++i)
                    if (actions[i].actionId == actionId) {
                        actionIdx = i;
                        break;
                    }
                if (nbRepeat)
                    cout << ActionToString(actions[actionIdx].actionType) << " " << actions[actionIdx].actionId << " " << to_string(nbRepeat + 1) << '\n';
                else
                    cout << ActionToString(actions[actionIdx].actionType) << " " << actions[actionIdx].actionId << '\n';
            }
            if (!steps.empty())
                steps.pop_front();
        }
        ++round;
    }
}