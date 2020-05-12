#ifdef _WIN64
#define _CRT_SECURE_NO_WARNINGS
#include<vector>
#include<unordered_map>
#include<algorithm>
#include<iostream>
#include<fstream>
#else
#include <bits/stdc++.h>
#endif // 

using namespace std;

typedef unsigned long long ull;
typedef unsigned int ui;


struct Path {
    int length;
    vector<ui> path;

    Path(int length, const vector<ui> &path) : length(length), path(path) {}

    bool operator<(const Path &rhs) const {
        if (length != rhs.length) return length < rhs.length;
        for (int i = 0; i < length; i++) {
            if (path[i] != rhs.path[i])
                return path[i] < rhs.path[i];
        }
    }
};

class Solution {
public:
    vector<vector<int>> G;
    vector<vector<ui>> invG;
    unordered_map<ui, int> idHash; //sorted id to 0...n
    //bad case, don't follow
    unordered_map<ull, int> migic;
    vector<ui> ids; //0...n to sorted id
    vector<ui> inputs; //u-v pairs
    vector<int> inDegrees;
    vector<bool> vis;
    vector<bool> onestep_reach;
    vector<vector<Path>> ans;
    int nodeCnt;

    inline bool check(int x, int y) {
        if(x==0 || y==0) return false;
        return x <= 5ll * y && y <= 3ll * x;
    }

    void parseInput(string &testFile) {
        FILE *file = fopen(testFile.c_str(), "r");
        ui u, v, c;
        int cnt = 0;
        while (fscanf(file, "%u,%u,%u", &u, &v, &c) != EOF) {
            inputs.push_back(u);
            inputs.push_back(v);
            migic[(ull) u << 32 | v] = c;
            ++cnt;
        }
#ifdef TEST
        printf("%d Records in Total\n", cnt);
#endif
    }

    void constructGraph() {
        auto tmp = inputs;
        sort(tmp.begin(), tmp.end());
        tmp.erase(unique(tmp.begin(), tmp.end()), tmp.end());
        ids = tmp;
        nodeCnt = 0;
        for (ui &x:tmp) {
            idHash[x] = nodeCnt++;
        }
#ifdef TEST
        printf("%d Nodes in Total\n", nodeCnt);
#endif
        int sz = inputs.size();
        G = vector<vector<int>>(nodeCnt);
        invG = vector<vector<ui>>(nodeCnt);
        inDegrees = vector<int>(nodeCnt, 0);
        for (int i = 0; i < sz; i += 2) {
            int u = idHash[inputs[i]], v = idHash[inputs[i + 1]];
            G[u].push_back(v);
            invG[v].push_back(u);
            ++inDegrees[v];
        }

        for (int i = 0; i < nodeCnt; i++)
        {
            sort(G[i].begin(), G[i].end());
        }
    }

    //magic code,don't touch
    //bool checkAns(vector<ui> tmp, int depth) {
    //    for (int i = depth - 2; i < depth; i++) {
    //        int l = tmp[(i + depth - 1) % depth], m = tmp[i], r = tmp[(i + 1) % depth];
    //        if (!check(migic[(ull) l << 32 | m], migic[(ull) m << 32 | r])) return false;
    //    }
    //    return true;
    //}

    bool checkAns(vector<ui> tmp, int depth) {
        int i = 0;
        int l = tmp[(i + depth - 1) % depth], m = tmp[i], r = tmp[(i + 1) % depth];
        if (!check(migic[(ull)l << 32 | m], migic[(ull)m << 32 | r])) return false;
        i = depth - 1;
        l = tmp[(i + depth - 1) % depth]; m = tmp[i]; r = tmp[(i + 1) % depth];
        if (!check(migic[(ull)l << 32 | m], migic[(ull)m << 32 | r])) return false;
        return true;
    }

    void dfs(int head, int pre, int cur, int depth, vector<int> &path) {
        vis[cur] = true;
        path.push_back(cur);
        for (int &v:G[cur]) {
            if (depth == 1 && v > head)
                dfs(head, cur, v, depth + 1, path);
            else if (v == head && depth >= 3) {
                vector<ui> tmp;
                for (int &x:path)
                    tmp.push_back(ids[x]);
                if (checkAns(tmp, depth))
                    ans[depth - 3].emplace_back(Path(depth, tmp));
            }
            else if (depth < 7 && !vis[v] && v > head ) {
                {
                    if (check(migic[(ull)ids[pre] << 32 | ids[cur]], migic[(ull)ids[cur] << 32 | ids[v]]))
                    {
                        if (depth < 4)
                            dfs(head, cur, v, depth + 1, path);
                        else
                        {
                            if (onestep_reach[v])
                                dfs(head, cur, v, depth + 1, path);
                        }
                    }
                }
            }
        }
        vis[cur] = false;
        path.pop_back();
    }

    //search from 0...n
    //????id?????????????????????id?????
    void solve() {
        vis = vector<bool>(nodeCnt, false);
        onestep_reach = vector<bool>(nodeCnt, false);
        ans = vector<vector<Path>>(5);

        vector<int> path;
        for (int i = 0; i < nodeCnt; i++) {
            //if (i % 100 == 0)
            //    cout << i << "/" << nodeCnt << endl;
            for (ui& v : invG[i])
            {
                if(v<i) continue;
                onestep_reach[v] = true;
                for (ui& vv : invG[v])
                {
                    if(vv<i) continue;
                    onestep_reach[vv] = true;
                    for (ui& vvv : invG[vv])
                    {
                        if(vvv<i) continue;
                        onestep_reach[vvv] = true;
                    }
                }
            }

            if (!G[i].empty()) {
                dfs(i, i, i, 1, path);
            }

            for (ui& v : invG[i])
            {
                if (v < i) continue;
                onestep_reach[v] = false;
                for (ui& vv : invG[v])
                {
                    if (vv < i) continue;
                    onestep_reach[vv] = false;
                    for (ui& vvv : invG[vv])
                    {
                        if (vvv < i) continue;
                        onestep_reach[vvv] = false;
                    }
                }
            }
        }
        //sort(ans.begin(), ans.end());
    }

    void save(string &outputFile) {
        int ansSize = 0;
        for (int i = 0; i < 5; i++)
        {
            ansSize += ans[i].size();
        }
        ofstream out(outputFile);
        out << ansSize << endl;
        for (int i = 0; i < 5; i++)
        {
            for (auto& x : ans[i]) {
                auto path = x.path;
                int sz = path.size();
                out << path[0];
                for (int i = 1; i < sz; i++)
                    out << "," << path[i];
                out << endl;
            }
        }
    }
};


void solve(string testFile,string outputFile){
    Solution solution;
    solution.parseInput(testFile);
    solution.constructGraph();
    solution.solve();
    solution.save(outputFile);
}

int main() {
    // for (int i=1;i<=9;i++){
    //     string testFile = "test_data"+to_string(i)+".fs.txt";
    //     string outputFile = "result"+to_string(i)+".fs.txt";
    //     auto t = clock();
    //     solve(testFile,outputFile);
    //     cout << clock() - t << endl;
    // }

#ifdef _WIN64
    string testFile = "../data/official/test_data.txt";
    string outputFile = "../data/official/myresult.txt";
#else
    string testFile = "/data/test_data.txt";
    string outputFile = "/projects/student/result.txt";
#endif // _WIN64
    solve(testFile,outputFile);

    return 0;
}