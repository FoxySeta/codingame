//S01
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <limits.h>

const int safeRange = 5;//arbitrario

struct entity {
    char type[21];
    int id, x, y, param[3];
};

int W, H;
char map[26][26];
int sanityLossLonely, sanityLossGroup, wandererSpawnTime, wandererLifeTime;
int planLeft = 2, lastPlan = 5, lightLeft = 3, lastLight = 3, lastYell = 2;
bool yellLeft[] =  {true, true, true, true};

int dij(int, int , int , int);
int d(struct entity, struct entity);
int est(struct entity, struct entity[], int, char);
bool betterThan(struct entity, struct entity, struct entity[], int);
bool enemy(struct entity, struct entity[], int);
bool deadEnd(struct entity, struct entity);
bool slasherSeeable(struct entity, struct entity);

int main(){
    //Initialization
    scanf("%d%d", &W, &H);
    fgetc(stdin);
    for (int i = 0; i < H; ++i)
        fgets(map[i], 26, stdin);
    scanf("%d%d%d%d", &sanityLossLonely, &sanityLossGroup, &wandererSpawnTime, &wandererLifeTime);
    //Game loop
    while (1) {
        //Input
        int entityCount;
        scanf("%d", &entityCount);
        struct entity pc[4], npc[20], shelt[8];
        int pcCount = 0, npcCount = 0, sheltCount = 0;
        for (int i = 0; i < entityCount; ++i){
            struct entity input;
            scanf("%s%d%d%d%d%d%d", input.type, &input.id, &input.x, &input.y, &input.param[0], &input.param[1], &input.param[2]);
            if (!strcmp(input.type, "EXPLORER"))
                pc[pcCount++] = input;
            else if (!strcmp(input.type, "EFFECT_SHELTER") && input.param[0])
                shelt[sheltCount++] = input;
            else  if (!strcmp(input.type, "WANDERER") || !strcmp(input.type, "SLASHER"))
                npc[npcCount++] = input;
        }
        //Process
        struct entity obj;
        if (pc[0].param[0] <= sanityLossLonely * 5 && d(pc[0], npc[est(pc[0], npc, npcCount, "<")]) > 1) {
            printf("PLAN Doki! Doki! <3\n");
            --planLeft;
            lastPlan = 0;
            continue;             
        } else if (!npcCount || d(pc[0], npc[est(pc[0], npc, npcCount, '<')]) > 3 && d (pc[0], pc[est(pc[0], pc + 1, pcCount - 1, '<') + 1]) > 2){
            fprintf(stderr, "Following a friend!\n");
            obj = pc[est(pc[0], pc + 1, pcCount - 1, '<') + 1];
            struct entity U = shelt[est(pc[0], shelt, sheltCount, '<')];
            if (d(pc[0], U) <= d(pc[0], obj) + safeRange)
                obj = U;
        } else if (lightLeft && lastLight > 2 && d(pc[0], npc[est(npc[0], npc, npcCount, '<')]) < 6 && d(pc[0], npc[est(pc[0], npc, npcCount, '<')]) > 1 /*&& npc[est(npc[0], npc, npcCount, '<')].param[2] != pc[0].id*/ && npc[est(npc[0], npc, npcCount, '<')].param[2] != -1 && !enemy(pc[0], npc, npcCount)){
            printf("LIGHT I'll switch one on! <3\n");
            --lightLeft;
            lastLight = 0;
            continue;
        } else if (planLeft && lastPlan > 4 && d(pc[0], npc[est(pc[0], npc, npcCount, '<')]) > 3 /*&& d(pc[0], pc[est(pc[0], pc + 1, pcCount - 1, '<') + 1]) < 3*/ && pc[0].param[0] < 245) {
            printf("PLAN Doki! Doki! <3\n");
            --planLeft;
            lastPlan = 0;
            continue;
        } else if (lastYell > 1 && d(pc[0], pc[est(pc[0], pc + 1, pcCount - 1, '<') + 1]) < 2 && d(pc[0], npc[est(pc[0], npc, npcCount, '<')]) >= safeRange && yellLeft[pc[est(pc[0], pc + 1, pcCount - 1, '<') + 1].id]) {
            printf("YELL BeEeHiIndD YooOUu!\n");
            lastYell = 0;
            yellLeft[pc[est(pc[0], pc + 1, pcCount - 1, '<') + 1].id] = false;
            continue;
        } else {
            obj = pc[0];
            for (int i = -2; i <= 2; ++i)
                for (int j = -2; j <= 2; ++j){
                    if (abs(i) + abs(j) > 2)
                        continue;
                    struct entity hyp;
                    hyp.y = pc[0].y + i;
                    hyp.x = pc[0].x + j;
                    if (abs(i) == 2 && map[(pc[0].y + hyp.y)/2][hyp.x] == '#')
                        continue;
                    if (abs(j) == 2 && map[hyp.y][(pc[0].x + hyp.x)/2] == '#')
                        continue;
                    if(map[obj.y][obj.x] == '#' || deadEnd(pc[0], obj) || -1 < hyp.y && hyp.y < H - 1 && -1 < hyp.x && hyp.x < W - 1 && map[hyp.y][hyp.x] != '#' && (betterThan(hyp, obj, npc, npcCount) || enemy(obj, npc, npcCount) && !enemy(hyp, npc, npcCount)) && !deadEnd(pc[0], hyp))
                        obj = hyp;
                }
        }
        //Output
        if (obj.x != pc[0].x || obj.y != pc[0].y)
            printf("MOVE %d %d ...\n", obj.x, obj.y);
        else if (planLeft && lastPlan > 4 && pc[0].param[0] < 245  /*&& d(pc[0], npc[est(pc[0], npc, npcCount, '<')]) > 3*/) {
            printf("PLAN Whatevs. <3\n");
            --planLeft;
            lastPlan = 0;
        } else {
            printf("WAIT Yawn... <3 <3\n");
        }
        //Aftermath
        ++lastYell;
        ++lastLight;
        ++lastPlan;
    }
    return 0;
}

/*int dij(int ay, int ax, int by, int bx){
    int d[H][W];
    bool seen[H][W];
    for (int i = 0; i < H; ++i)
        for (int j = 0; j < W; ++j){
            d[i][j] = INT_MAX;
            seen[i][j] = false;
        }
    d[ay][ax] = 0;
    do {
        seen[ay][ax] = true;
        if (map[ay - 1][ax] != '#' && d[ay][ax] + 1 < d[ay - 1][ax])
            d[ay - 1][ax] = d[ay][ax] + 1;
        if (map[ay][ax + 1] != '#' && d[ay][ax] + 1 < d[ay][ax + 1])
            d[ay][ax + 1] = d[ay][ax] + 1;
        if (map[ay + 1][ax] != '#' && d[ay][ax] + 1 < d[ay + 1][ax])
            d[ay + 1][ax] = d[ay][ax] + 1;
        if (map[ay][ax - 1] != '#' && d[ay][ax] + 1 < d[ay][ax - 1])
            d[ay][ax - 1] = d[ay][ax] + 1;
        for (int i = 0; i < H; ++i)
            for (int j = 0; j < W; ++j)
                if (map[i][j] != '#' && (seen[ay][ax] || d[i][j] < d[ay][ax])) {
                    ay = i;
                    ax = j;
                }
    } while (!seen[by][bx]);
    return d[by][bx];
}*/

int d(struct entity a, struct entity b){
    return abs(a.x - b.x) + abs(a.y - b.y);//dij(a.y, a.x, b.y, b.x);
}

int est(struct entity a, struct entity v[], int N, char c){
    int res = 0;
    for (int i = 1; i < N; ++i){
        if (v[i].x == 52)
            continue;
        if ((v[res].param[2] == -1 || (v[res].param[2] != a.id && v[i].param[2] != -1) || v[i].param[2] == a.id) && (c == '<' ? d(a, v[i]) < d(a, v[res]) : d(a, v[i]) > d(a, v[res])))
            res = i;
    }
    return res;
}

bool betterThan(struct entity a, struct entity b, struct entity v[], int N){
    //fprintf(stderr, "betterThan((%d, %d), (%d, %d))? ", a.x, a.y, b.x, b.y);
    if (d(a, v[est(a, v, N, '<')]) != d(b, v[est(b, v, N, '<')])){
    //    fprintf(stderr, "[1] %d\n", d(a, v[est(a, v, N, '<')]) > d(b, v[est(b, v, N, '<')]));
        return d(a, v[est(a, v, N, '<')]) > d(b, v[est(b, v, N, '<')]);
    }
    struct entity vA[20], vB[20];
    for (int i = 0; i < N; ++i)
        vA[i] = vB[i] = v[i];
    int delete = est(a, vA, N, '<');
    vA[delete].x = vA[delete].y = 52;
    delete = est(b, vB, N, '<');
    vB[delete].x = vB[delete].y = 52;
    //fprintf(stderr, "[2] %d > %d = %d\n", d(a, v[est(a, vA, N, '<')]), d(b, v[est(b, vB, N, '<')]), d(a, v[est(a, vA, N, '<')]) > d(b, v[est(b, vB, N, '<')]));
    return d(a, v[est(a, vA, N, '<')]) > d(b, v[est(b, vB, N, '<')]);
}

bool enemy(struct entity a, struct entity v[], int N){
    for (int i = 0; i < N; ++i)
        if (v[i].x == a.x && v[i].y == a.y || !strcmp(v[i].type, "SLASHER") && slasherSeeable(a, v[i]))
            return true;
    return false;
}

bool deadEnd(struct entity a, struct entity b){
    int count;
    count = 0;
    struct entity hyp;
    for (int i = 0; i < 4; ++i){
        hyp.x = b.x;
        hyp.y = b.y;
        switch(i){
            case 0:
                --hyp.y;
                break;
            case 1:
                ++hyp.x;
                break;
            case 2:
                ++hyp.y;
                break;
            case 3:
                --hyp.x;
                break;
        }
        if (map[hyp.y][hyp.x] != '#')
            ++count;
    }
    if (count < 2)
        return true;
    return false;
}

bool slasherSeeable(struct entity a , struct entity s){
    //fprintf(stderr, "slahSeeable((%d, %d), (%d, %d))? ", a.x, a.y, s.x, s.y);
    if (a.x != s.x && a.y != s.y /*|| s.param[1] == 4 && s.param[0] == 6*/){//4 == stunned
        //fprintf(stderr, "N\n");
        return false;
    }    
    int min, max;
    bool Xwork;
    if (a.x == s.x){
        Xwork = true;
        if (a.y < s.y){
            min = a.y;
            max = s.y;
        } else {
            min = s.y;
            max = a.y;
        }
    } else {
        Xwork = false;
        if (a.x < s.x){
            min = a.x;
            max = s.x;
        } else {
            min = s.x;
            max = a.x;
        }
    }
    for (int i = min + 1; i < max - 1; ++i)
        if ((Xwork ? map[i][a.x] : (map[a.y][i]) == '#')){
            //fprintf(stderr, "N\n");
            return false;
        }
    //fprintf(stderr, "S\n");
     return true;
}