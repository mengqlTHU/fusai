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
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#endif // 

#define NIL -1 

using namespace std;

typedef unsigned long long ull;
typedef unsigned int ui;


struct Path {
    int length;
    vector<ui> path;

    Path(int length, const vector<ui>& path) : length(length), path(path) {}

    bool operator<(const Path& rhs) const {
        if (length != rhs.length) return length < rhs.length;
        for (int i = 0; i < length; i++) {
            if (path[i] != rhs.path[i])
                return path[i] < rhs.path[i];
        }
    }
};

struct connection {
    ui next;
    ull amount;
    bool operator==(const connection b) const
    {
        return this->next == b.next;
    }
    bool operator<=(const connection b) const
    {
        return this->next <= b.next;
    }
    bool operator<(const connection b) const
    {
        return this->next < b.next;
    }
    bool operator!=(const connection b) const
    {
        return this->next != b.next;
    }
    bool operator>=(const connection b) const
    {
        return this->next >= b.next;
    }
    bool operator>(const connection b) const
    {
        return this->next > b.next;
    }
};

int sizeTable[9] = { 9, 99, 999, 9999, 99999, 999999, 9999999,
  99999999, 999999999 };

int intSize(int x)
{
    for (int i = 0; i < 9; ++i)
    {
        if (x <= sizeTable[i]) return i + 1;
    }
    return 10;
};

int append_uint_to_str(char* s, unsigned int i, int size)
{
    s[size] = ',';
    int tmp = size;
    while (i > 0)
    {
        s[--size] = '0' + i % 10;
        i /= 10;
    }
    return tmp + 1;
};

const char digit_pairs[201] = {
  "00010203040506070809"
  "10111213141516171819"
  "20212223242526272829"
  "30313233343536373839"
  "40414243444546474849"
  "50515253545556575859"
  "60616263646566676869"
  "70717273747576777879"
  "80818283848586878889"
  "90919293949596979899"
};

class Solution {
public:
    vector<vector<connection>> G;
    vector<vector<connection>> invG;
    unordered_map<ui, int> idHash; //sorted id to 0...n
    //bad case, don't follow
    unordered_map<ull, int> migic;
    vector<ull> inputs_amount;
    vector<ui> ids; //0...n to sorted id
    vector<ui> inputs; //u-v pairs
    //vector<int> inDegrees;
    vector<bool> vis;
    vector<bool> onestep_reach;
    vector<ull> direct_reach;
    vector<vector<Path>> ans;
    int nodeCnt;
    int sccTime;
    bool isDenseGraph;
#ifdef _WIN64
    ofstream debugfile;
#endif

    inline bool check(ull x, ull y) {
        
        return x <= 5ll * y && y <= 3ll * x;
    }

    void parseInput(string& testFile) {
        FILE* file = fopen(testFile.c_str(), "r");
        ui u, v;
        double c;
        while (fscanf(file, "%u,%u,%lf", &u, &v, &c) != EOF) {
            inputs.push_back(u);
            inputs.push_back(v);
            //migic[(ull) u << 32 | v] = c;
            inputs_amount.push_back((ull)round(c*100));
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
        for (ui& x : tmp) {
            idHash[x] = nodeCnt++;
        }
#ifdef TEST
        printf("%d Nodes in Total\n", nodeCnt);
#endif
        int sz = inputs.size();
        G = vector<vector<connection>>(nodeCnt);
        invG = vector<vector<connection>>(nodeCnt);
        for (int i = 0; i < sz; i += 2) {
            int u = idHash[inputs[i]], v = idHash[inputs[i + 1]];
            struct connection conn = { v, inputs_amount[i / 2] };
            G[u].push_back(conn);
            struct connection invConn = { u, inputs_amount[i / 2] };
            invG[v].push_back(invConn);
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
        for (connection& conn : G[u])
        {
            ui v = conn.next;
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
        memset(disc, NIL, sizeof(int) * nodeCnt);
        memset(low, NIL, sizeof(int) * nodeCnt);
        memset(stackMember, false, nodeCnt);

        // Call the recursive helper function to find strongly 
        // connected components in DFS tree with vertex 'i' 
        for (int i = 0; i < nodeCnt; i++)
            if (disc[i] == NIL)
                SCCUtil(i, disc, low, st, stackMember);

    }

    void dfs(int head, int pre, int cur, int depth, vector<int>& path, ull startAmount, ull preAmount) {
        vis[cur] = true;
        path.push_back(cur);
        for (connection& conn : G[cur]) {
            ui v = conn.next;
            ull amount = conn.amount;
            if (depth == 1 && v > head)
            {
                startAmount = amount;
                dfs(head, cur, v, depth + 1, path, startAmount, startAmount);
            }
            else if (v == head && depth >= 3) {
                vector<ui> tmp;
                for (int& x : path)
                    tmp.push_back(ids[x]);
                //if (checkAns(tmp, depth))
                if (check(preAmount, amount) && check(amount, startAmount))
                    ans[depth - 3].emplace_back(Path(depth, tmp));
            }
            else if (depth < 8 && !vis[v] && v > head) {
                //if (check(migic[(ull)ids[pre] << 32 | ids[cur]], migic[(ull)ids[cur] << 32 | ids[v]]))
                if (check(preAmount, amount))
                {
                    if (depth < 5)
                        dfs(head, cur, v, depth + 1, path, startAmount, amount);
                    else if (depth < 7)
                    {
                        if (onestep_reach[v])
                            dfs(head, cur, v, depth + 1, path, startAmount, amount);
                    }
                    else
                    {
                        if (direct_reach[v])
                        {
                            if (check(amount, direct_reach[v]) && check(direct_reach[v], startAmount))
                            {
                                vector<ui> tmp;
                                path.push_back(v);
                                for (int& x : path)
                                    tmp.push_back(ids[x]);
                                //if (checkAns(tmp, 7))
                                ans[5].emplace_back(Path(8, tmp));
                                path.pop_back();
                            }
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
        direct_reach = vector<ull>(nodeCnt, 0);
        ans = vector<vector<Path>>(6);

        vector<int> path;
        for (int i = 0; i < nodeCnt; i++) {
            //if (i % 100 == 0)
            //    cout << i << "/" << nodeCnt << endl;
            for (connection& c : invG[i])
            {
                ui v = c.next;
                ull amount = c.amount;
                if (v < i) continue;
                onestep_reach[v] = true;
                direct_reach[v] = amount;
                for (connection& cc : invG[v])
                {
                    ui vv = cc.next;
                    if (vv < i) continue;
                    onestep_reach[vv] = true;
                    for (connection& ccc : invG[vv])
                    {
                        ui vvv = ccc.next;
                        if (vvv < i) continue;
                        onestep_reach[vvv] = true;
                    }
                }
            }

            if (!G[i].empty()) {
                dfs(i, i, i, 1, path, 0, 0);
//#ifdef _WIN64
//                cout << i << endl;
//#endif
            }

            for (connection& c : invG[i])
            {
                ui v = c.next;
                ull amount = c.amount;
                if (v < i) continue;
                onestep_reach[v] = false;
                direct_reach[v] = 0;
                for (connection& cc : invG[v])
                {
                    ui vv = cc.next;
                    if (vv < i) continue;
                    onestep_reach[vv] = false;
                    for (connection& ccc : invG[vv])
                    {
                        ui vvv = ccc.next;
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
        //for (ui &w: *scc)
        //{
        //    for (vector<connection>::iterator iter = G[w].begin(); iter != G[w].end(); )
        //    {
        //        if (!binary_search(scc->begin(),scc->end(),*iter))
        //            iter = G[w].erase(iter); // advances iter
        //        else
        //            ++iter; // don't remove
        //    }
        //}
        vector<int> path;
        //for (vector<ui>::iterator iter = scc->begin(); iter != scc->end() - 2; iter++)
        //{
        //    ui i = *iter;
        //    for (ui& v : invG[i])
        //    {
        //        if (v < i) continue;
        //        onestep_reach[v] = true;
        //        for (ui& vv : invG[v])
        //        {
        //            if (vv < i) continue;
        //            onestep_reach[vv] = true;
        //            for (ui& vvv : invG[vv])
        //            {
        //                if (vvv < i) continue;
        //                onestep_reach[vvv] = true;
        //            }
        //        }
        //    }

        //    if (!G[i].empty()) {
        //        dfs(i, i, i, 1, path);
        //    }

        //    for (ui& v : invG[i])
        //    {
        //        if (v < i) continue;
        //        onestep_reach[v] = false;
        //        for (ui& vv : invG[v])
        //        {
        //            if (vv < i) continue;
        //            onestep_reach[vv] = false;
        //            for (ui& vvv : invG[vv])
        //            {
        //                if (vvv < i) continue;
        //                onestep_reach[vvv] = false;
        //            }
        //        }
        //    }
        //}
    }

    void save(string& outputFile) {
        int ansSize = 0;
        for (int i = 0; i < 6; i++)
        {
            ansSize += ans[i].size();
        }
        ofstream out(outputFile);
        out << ansSize << '\n';
        for (int i = 0; i < 6; i++)
        {
            for (auto& x : ans[i]) {
                auto path = x.path;
                int sz = path.size();
                out << path[0];
                for (int i = 1; i < sz; i++)
                    out << "," << path[i];
                out << '\n';
            }
        }
    }

#ifndef _WIN64
    void save_mmap(string& outputFile)
    {
        int count = 0;
        for (int i = 0; i < 5; i++)
            count += ans[i].size();
        char* p = new char[(ull)count * 80 / 5 + 11];
        char* pp = (char*)p;
        pp += append_uint_to_str(pp, count, intSize(count));
        pp[-1] = '\n';
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < ans[i].size(); j++) {
                for (ui& k : ans[i][j].path)
                {
                    pp += append_uint_to_str(pp, k, intSize(k));
                }
                pp[-1] = '\n';
            }
        }

        int fd = open(outputFile.c_str(), O_RDWR | O_CREAT, (mode_t)0666);
        size_t textsize = pp - p + 1;
        lseek(fd, textsize - 1, SEEK_SET);
        write(fd, "", 1);
        char* map = (char*)mmap(0, textsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        memcpy(map, p, pp - p);
        msync(map, textsize, MS_SYNC);
        close(fd);
    }
#endif

    //void save(string& outputFile)
    //{
    //    int ansSize = 0;
    //    for (int i = 0; i < 5; i++)
    //    {
    //        ansSize += ans[i].size();
    //    }
    //    ofstream out(outputFile);
    //    out << ansSize << endl;
    //    for (int i = 0; i < 4; i++)
    //    {
    //        for (auto& x : ans[i]) {
    //            auto path = x.path;
    //            int sz = path.size();
    //            out << path[0];
    //            for (int i = 1; i < sz; i++)
    //                out << "," << path[i];
    //            out << endl;
    //        }
    //    }
    //    FILE* fp = fopen(outputFile.c_str(), "a");
    //    char* p = new char[(ull)ansSize * 40 + 11];
    //    char* pp = (char*)p;
    //    for (int j = 0; j < ans[4].size(); j++) {
    //        for (ui& k : ans[4][j].path)
    //        {
    //            pp += append_uint_to_str(pp, k, intSize(k));
    //        }
    //        pp[-1] = '\n';
    //    }
    //    fwrite(p, 1, pp - p, fp);
    //    fclose(fp);
    //}

    //void save(string& outputFile) {
    //    int count = 0;
    //    for (int i = 0; i < 5; i++) 
    //        count += ans[i].size();
    //    FILE* fp = fopen(outputFile.c_str(), "w");
    //    char* p = new char[(ull)count*80 + 11];
    //    char* pp = (char*)p;
    //    pp += append_uint_to_str(pp, count, intSize(count));
    //    pp[-1] = '\n';
    //    //*pp = '\n';
    //    //pp++;
    //    for (int i = 0; i < 5; i++) {
    //        for (int j = 0; j < ans[i].size(); j++) {
    //            for (ui &k:ans[i][j].path)
    //            {
    //                pp += append_uint_to_str(pp, k, intSize(k));
    //            }
    //            pp[-1] = '\n';
    //            //*pp = '\n';
    //            //pp++;
    //        }
    //    }
    //    fwrite(p, 1, pp - p, fp);
    //    //fprintf(fp, );
    //    fclose(fp);
    //}
};


void solve(string testFile, string outputFile) {
    Solution solution;
    solution.parseInput(testFile);
    solution.constructGraph();
    //if (solution.isDenseGraph)
    //    solution.solve();
    //else
    //    solution.solveSCC();
    solution.solve();
    solution.save(outputFile);

    //#ifdef _WIN64
    //    solution.save(outputFile);
    //#else
    //    solution.save_mmap(outputFile);
    //#endif
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
    //string testFile = "../data/official/test_data.txt";
    string testFile = "../data/fusai_final/test_data.txt";
    //string testFile = "../temp_data/2755223/test_data.txt";
    string outputFile = "../data/myresult.txt";
#else
    string testFile = "/data/test_data.txt";
    string outputFile = "/projects/student/result.txt";
#endif // _WIN64
    solve(testFile, outputFile);

    return 0;
}