#ifdef _WIN64
#define _CRT_SECURE_NO_WARNINGS
#include<vector>
#include<unordered_map>
#include<stack>
#include<algorithm>
#include<iostream>
#include<thread>
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
#include <thread>
#include <string.h>
#endif // 

#define NIL -1 
#define thread_cnt 4

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

//int sizeTable[9] = { 9, 99, 999, 9999, 99999, 999999, 9999999,
//  99999999, 999999999 };
//
//int intSize(int x)
//{
//    for (int i = 0; i < 9; ++i)
//    {
//        if (x <= sizeTable[i]) return i + 1;
//    }
//    return 10;
//};
//
//int append_uint_to_str(char* s, unsigned int i, int size)
//{
//    s[size] = ',';
//    int tmp = size;
//    while (i > 0)
//    {
//        s[--size] = '0' + i % 10;
//        i /= 10;
//    }
//    return tmp + 1;
//};
//
//const char digit_pairs[201] = {
//  "00010203040506070809"
//  "10111213141516171819"
//  "20212223242526272829"
//  "30313233343536373839"
//  "40414243444546474849"
//  "50515253545556575859"
//  "60616263646566676869"
//  "70717273747576777879"
//  "80818283848586878889"
//  "90919293949596979899"
//};

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
    vector<vector<bool>> vis;
    vector<vector<bool>> onestep_reach;
    vector<vector<ull>> direct_reach;
    vector<vector<vector<Path>>> ans;
    int nodeCnt;
    bool isDenseGraph;

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

    void start_threaded_solve(int thread_num)
    {
        const double thread_cntd = (double)thread_cnt;
        const double thread_numd = (double)thread_num;
        const double nodeCntd = (double)nodeCnt;
        const int nodeMin = nodeCntd * (1.0 - pow((thread_cntd - thread_numd) / thread_cntd, 1.0 / 5.0));
        const int nodeMax = nodeCntd * (1.0 - pow((thread_cntd - thread_numd - 1.0) / thread_cntd, 1.0 / 5.0));

        vector<int> path;

        for (int i = nodeMin; i < nodeMax; i++) {
            //if (i % 100 == 0)
            //    cout << i << "/" << nodeCnt << endl;
            for (connection& c : invG[i])
            {
                ui v = c.next;
                ull amount = c.amount;
                if (v < i) continue;
                onestep_reach[thread_num][v] = true;
                direct_reach[thread_num][v] = amount;
                for (connection& cc : invG[v])
                {
                    ui vv = cc.next;
                    if (vv < i) continue;
                    onestep_reach[thread_num][vv] = true;
                    for (connection& ccc : invG[vv])
                    {
                        ui vvv = ccc.next;
                        if (vvv < i) continue;
                        onestep_reach[thread_num][vvv] = true;
                    }
                }
            }

            if (!G[i].empty()) {
                dfs(i, i, i, 1, path, 0, 0, thread_num);
            }

            for (connection& c : invG[i])
            {
                ui v = c.next;
                ull amount = c.amount;
                if (v < i) continue;
                onestep_reach[thread_num][v] = false;
                direct_reach[thread_num][v] = 0;
                for (connection& cc : invG[v])
                {
                    ui vv = cc.next;
                    if (vv < i) continue;
                    onestep_reach[thread_num][vv] = false;
                    for (connection& ccc : invG[vv])
                    {
                        ui vvv = ccc.next;
                        if (vvv < i) continue;
                        onestep_reach[thread_num][vvv] = false;
                    }
                }
            }
        }

    }

    void dfs(int head, int pre, int cur, int depth, vector<int>& path, ull startAmount, ull preAmount, int thread_num) {
        vis[thread_num][cur] = true;
        path.push_back(cur);
        for (connection& conn : G[cur]) {
            ui v = conn.next;
            ull amount = conn.amount;
            if (depth == 1 && v > head)
            {
                startAmount = amount;
                dfs(head, cur, v, depth + 1, path, startAmount, startAmount, thread_num);
            }
            else if (v == head && depth >= 3) {
                vector<ui> tmp;
                for (int& x : path)
                    tmp.push_back(ids[x]);
                //if (checkAns(tmp, depth))
                if (check(preAmount, amount) && check(amount, startAmount))
                    ans[thread_num][depth - 3].emplace_back(Path(depth, tmp));
            }
            else if (depth < 8 && !vis[thread_num][v] && v > head) {
                //if (check(migic[(ull)ids[pre] << 32 | ids[cur]], migic[(ull)ids[cur] << 32 | ids[v]]))
                if (check(preAmount, amount))
                {
                    if (depth < 5)
                        dfs(head, cur, v, depth + 1, path, startAmount, amount, thread_num);
                    else if (depth < 7)
                    {
                        if (onestep_reach[thread_num][v])
                            dfs(head, cur, v, depth + 1, path, startAmount, amount, thread_num);
                    }
                    else
                    {
                        if (direct_reach[thread_num][v])
                        {
                            if (check(amount, direct_reach[thread_num][v]) && check(direct_reach[thread_num][v], startAmount))
                            {
                                vector<ui> tmp;
                                path.push_back(v);
                                for (int& x : path)
                                    tmp.push_back(ids[x]);
                                //if (checkAns(tmp, 7))
                                ans[thread_num][5].emplace_back(Path(8, tmp));
                                path.pop_back();
                            }
                        }
                    }
                }
            }
        }
        vis[thread_num][cur] = false;
        path.pop_back();
    }

    //search from 0...n
    //????id?????????????????????id?????
    void solve() {
        //vis = vector<bool>(nodeCnt, false);
        vis = vector<vector<bool>>(thread_cnt, vector<bool>(nodeCnt, false));
        onestep_reach = vector<vector<bool>>(thread_cnt, vector<bool>(nodeCnt, false));
        direct_reach = vector<vector<ull>>(thread_cnt, vector<ull>(nodeCnt, 0));
        ans = vector<vector<vector<Path>>>(thread_cnt, vector<vector<Path>>(6));

        thread th[thread_cnt];
        for (int i = 0; i < thread_cnt; i++)
        {
            th[i] = thread(&Solution::start_threaded_solve, this, i);
        }
        for (int i = 0; i < thread_cnt; i++)
        {
            th[i].join();
        }

        //for (int i = 0; i < nodeCnt; i++) {
        //    //if (i % 100 == 0)
        //    //    cout << i << "/" << nodeCnt << endl;
        //    for (connection& c : invG[i])
        //    {
        //        ui v = c.next;
        //        ull amount = c.amount;
        //        if (v < i) continue;
        //        onestep_reach[v] = true;
        //        direct_reach[v] = amount;
        //        for (connection& cc : invG[v])
        //        {
        //            ui vv = cc.next;
        //            if (vv < i) continue;
        //            onestep_reach[vv] = true;
        //            for (connection& ccc : invG[vv])
        //            {
        //                ui vvv = ccc.next;
        //                if (vvv < i) continue;
        //                onestep_reach[vvv] = true;
        //            }
        //        }
        //    }

        //    if (!G[i].empty()) {
        //        dfs(i, i, i, 1, path, 0, 0);
        //    }

        //    for (connection& c : invG[i])
        //    {
        //        ui v = c.next;
        //        ull amount = c.amount;
        //        if (v < i) continue;
        //        onestep_reach[v] = false;
        //        direct_reach[v] = 0;
        //        for (connection& cc : invG[v])
        //        {
        //            ui vv = cc.next;
        //            if (vv < i) continue;
        //            onestep_reach[vv] = false;
        //            for (connection& ccc : invG[vv])
        //            {
        //                ui vvv = ccc.next;
        //                if (vvv < i) continue;
        //                onestep_reach[vvv] = false;
        //            }
        //        }
        //    }
        //}
        //sort(ans.begin(), ans.end());
    }


    void save(string& outputFile) {
        int ansSize = 0;
        for (int i = 0; i < 6; i++)
        {
            for (int j = 0; j < thread_cnt; j++)
            {
                ansSize += ans[j][i].size();
            }
        }
        ofstream out(outputFile);
        out << ansSize << '\n';
        for (int i = 0; i < 6; i++)
        {
            for (int j = 0; j < thread_cnt; j++)
            {
                for (auto& x : ans[j][i]) {
                    auto path = x.path;
                    int sz = path.size();
                    out << path[0];
                    for (int i = 1; i < sz; i++)
                        out << "," << path[i];
                    out << '\n';
                }
            }
        }
    }

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