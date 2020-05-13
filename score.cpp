#ifdef _WIN64
#define _CRT_SECURE_NO_WARNINGS
#include<vector>
#include<unordered_map>
#include<stack>
#include<algorithm>
#include<iostream>
#include<fstream>
#else
#include <bits/stdc++.h>
#endif // 

#define NIL -1 

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
    vector<vector<ui>> G;
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
    int sccTime;
    bool isDenseGraph;
#ifdef _WIN64
    ofstream debugfile;
#endif

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
        G = vector<vector<ui>>(nodeCnt);
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

        if (((double)sz / (double)nodeCnt / (double)nodeCnt) > 0.00005)
            isDenseGraph = true;
        else
            isDenseGraph = false;
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

    void SCCUtil(int u, int disc[], int low[], stack<ui>* st,
        bool stackMember[])
    {

        // Initialize discovery time and low value 
        disc[u] = low[u] = ++sccTime;
        st->push(u);
        stackMember[u] = true;

        // Go through all vertices adjacent to this 
        for (ui &v: G[u])
        {
            // If v is not visited yet, then recur for it 
            if (disc[v] == -1)
            {
                SCCUtil(v, disc, low, st, stackMember);

                // Check if the subtree rooted with 'v' has a 
                // connection to one of the ancestors of 'u' 
                // Case 1 (per above discussion on Disc and Low value) 
                low[u] = min(low[u], low[v]);
            }

            // Update low value of 'u' only of 'v' is still in stack 
            // (i.e. it's a back edge, not cross edge). 
            // Case 2 (per above discussion on Disc and Low value) 
            else if (stackMember[v] == true)
                low[u] = min(low[u], disc[v]);
        }

        // head node found, pop the stack and print an SCC 
        int w = 0;  // To store stack extracted vertices 
        if (low[u] == disc[u])
        {
            vector<ui>* scc = new vector<ui>();
            while (st->top() != u)
            {
                w = (ui)st->top();
#ifdef _WIN64
                debugfile << ids[w] << ',';
#endif
                scc->push_back(w);
                stackMember[w] = false;
                st->pop();
            }
            w = (ui)st->top();
#ifdef _WIN64
            debugfile << ids[w] << endl;
#endif
            scc->push_back(w);
            stackMember[w] = false;
            st->pop();
            if (scc->size() > 2)
                solveInSubGraph(scc);
        }
    }

    // The function to do DFS traversal. It uses SCCUtil() 
    void SCC()
    {
        int* disc = new int[nodeCnt];
        int* low = new int[nodeCnt];
        bool* stackMember = new bool[nodeCnt];
        stack<ui>* st = new stack<ui>();

        // A static variable is used for simplicity, we can avoid use 
// of static variable by passing a pointer. 
       sccTime = 0;

        // Initialize disc and low, and stackMember arrays 
        memset(disc, NIL, sizeof(int)*nodeCnt);
        memset(low, NIL, sizeof(int)*nodeCnt);
        memset(stackMember, false, nodeCnt);

        // Call the recursive helper function to find strongly 
        // connected components in DFS tree with vertex 'i' 
        for (int i = 0; i < nodeCnt; i++)
            if (disc[i] == NIL)
                SCCUtil(i, disc, low, st, stackMember);

    }

    void dfs(int head, int pre, int cur, int depth, vector<int> &path) {
        vis[cur] = true;
        path.push_back(cur);
        for (ui &v:G[cur]) {
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

    void solveSCC()
    {
#ifdef _WIN64
        debugfile = ofstream("../data/official/debug.txt");
#endif
        vis = vector<bool>(nodeCnt, false);
        onestep_reach = vector<bool>(nodeCnt, false);
        ans = vector<vector<Path>>(5);

        SCC();
        for (int i = 0; i < 5; i++)
            sort(ans[i].begin(), ans[i].end());

#ifdef _WIN64
        debugfile.close();
#endif
    }

    void solveInSubGraph(vector<ui>* scc)
    {
        sort(scc->begin(), scc->end());
        for (ui &w: *scc)
        {
            for (vector<ui>::iterator iter = G[w].begin(); iter != G[w].end(); )
            {
                if (!binary_search(scc->begin(),scc->end(),*iter))
                    iter = G[w].erase(iter); // advances iter
                else
                    ++iter; // don't remove
            }
        }
        vector<int> path;
        for (vector<ui>::iterator iter = scc->begin(); iter != scc->end() - 2; iter++)
        {
            ui i = *iter;
            for (ui& v : invG[i])
            {
                if (v < i) continue;
                onestep_reach[v] = true;
                for (ui& vv : invG[v])
                {
                    if (vv < i) continue;
                    onestep_reach[vv] = true;
                    for (ui& vvv : invG[vv])
                    {
                        if (vvv < i) continue;
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
    if (solution.isDenseGraph)
        solution.solve();
    else
        solution.solveSCC();
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
    //string testFile = "../temp_data/2755223/test_data.txt";
    string outputFile = "../data/official/myresult.txt";
#else
    string testFile = "/data/test_data.txt";
    string outputFile = "/projects/student/result.txt";
#endif // _WIN64
    solve(testFile,outputFile);

    return 0;
}