#ifdef _WIN64
#define _CRT_SECURE_NO_WARNINGS
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <time.h>
#include <thread>
#include <math.h>
#else
#include <bits/stdc++.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <thread>
#include <math.h>
#endif

using namespace std;

#define thread_cnt 1
#define five (long long)5
#define three (long long)3

// #define TEST

// #define MYTIME

//template <typename T, typename Compare>
//vector<size_t> sort_permutation(
//    const vector<T>& vec,
//    Compare& compare)
//{
//    vector<std::size_t> p(vec.size());
//    iota(p.begin(), p.end(), 0);
//    sort(p.begin(), p.end(),
//        [&](size_t i, size_t j) { return compare(vec[i], vec[j]); });
//    return p;
//}


struct connection {
	uint32_t next;
	long long amount;
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


int sizeTable[10] = { 9, 99, 999, 9999, 99999, 999999, 9999999,
  99999999, 999999999, INT32_MAX };



inline int intSize(unsigned int x)
{
	for (int i = 0; i < 9; ++i)
	{
		if (x <= sizeTable[i]) return i + 1;
	}
	return 10;
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

inline int append_uint_to_str(char* s, unsigned int i)
{
	//if (i == 0)
	//{
	//    *s = '0';
	//    *(s + 1) = ',';
	//    return 2;
	//}
	int size = intSize(i);
	s[size] = ',';
	char* c = &s[size - 1];
	while (i >= 100)
	{
		int pos = i % 100;
		i /= 100;
		*(short*)(c - 1) = *(short*)(digit_pairs + 2 * pos);
		c -= 2;
	}
	while (i > 0)
	{
		*c-- = '0' + (i % 10);
		i /= 10;
	}
	return size + 1;
};

void insertSort(int* a, int N) {
	int i, j;
	for (i = 1; i < N; i++) {
		for (j = i - 1; j >= 0 && a[i] < a[j]; j--) {

		}
		//这里跳出内层循环，a[i]应被插入到a[j]后
		int tmp = a[i];
		for (int k = i; k > j + 1; k--) {
			a[k] = a[k - 1];
		}
		a[j + 1] = tmp;
	}
}

void shellSort(int* a, int* b, int N) {
	int h = 1;
	while (h < N / 3) {
		h = 3 * h + 1; //h的取值序列为1, 4, 13, 40, ...
	}
	while (h >= 1) {
		int n, i, j, k;
		//分割后，产生n个子序列
		for (n = 0; n < h; n++) {
			//分别对每个子序列进行插入排序
			for (i = n + h; i < N; i += h) {
				for (j = i - h; j >= 0 && a[i] < a[j]; j -= h) {

				}
				int atmp = a[i];
				int btmp = b[i];
				for (k = i; k > j + h; k -= h) {
					a[k] = a[k - h];
					b[k] = b[k - h];
				}
				a[j + h] = atmp;
				b[j + h] = btmp;
			}
		}
		h = h / 3;
	}
}

typedef uint32_t ui;

class Solution {
public:
	//maxN=560000
	//maxE=280000 ~avgN=26000
	//vector<int> *G;
	vector<vector<connection>> G;
	//int G_arr_num[280000];
	//int invG_arr_num[280000];
	//int* G_arr;
	//int* invG_arr;
	//int* amount_arr;
	vector<vector<connection>> invG;
	unordered_map<ui, uint32_t> idHash; //sorted id to 0...n
	vector<ui> ids; //0...n to sorted id
	char* idsStr;
	int* idsStrIndex;
	vector<ui> inputs; //u-v pairs
	vector<long long> inputs_amount;
	//vector<int> inDegrees;
	vector<vector<bool>> vis;
	//    vector<vector<Path>> ans_arr;
	char* ans[6][thread_cnt];
	int n_ans[6][thread_cnt];
	ui ans_top[6][thread_cnt];
	int nodeCnt;
	long long* direct_reach;
	bool* onestep_reach;

	void parseInput(string& testFile) {
		ui u, v; long long c;
		short decimals;
		// while(fscanf(file,"%u,%u,%u",&u,&v,&c)!=EOF){
		//     inputs.push_back(u);
		//     inputs.push_back(v);
		//     ++cnt;
		// }
#ifdef _WIN64
		ifstream in(testFile);
		string contents((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
		int length = contents.length();
		char* p = new char[length];
		strcpy(p, contents.c_str());
#else
		int fd = open(testFile.c_str(), O_RDONLY);
		int length = lseek(fd, 0, SEEK_END);
		char* p = (char*)mmap(0, length, PROT_READ, MAP_SHARED, fd, 0);
#endif
		char* pp = p;
		nodeCnt = 0;
		while (pp - p < length)
		{
			u = 0; v = 0; c = 0; decimals = 0;
			while (*pp != ',')
			{
				u = (*pp++ - '0') + u * 10;
			}
			pp++;
			while (*pp != ',')
			{
				v = (*pp++ - '0') + v * 10;
			}
			pp++;
			while (*pp != '\n' && *pp != '\r')
			{
				if (*pp == '.')
				{
					decimals = 1;
					pp++;
				}
				else
				{
					c = (*pp++ - '0') + c * 10;
					if (decimals) decimals++;
				}
			}
			if(*pp=='\r') pp++;
			if (decimals == 0)
				c *= 100;
			else if (decimals == 2)
				c *= 10;
			pp++;
			inputs.push_back(u);
			inputs.push_back(v);
			inputs_amount.push_back(c);
			// if (idHash.find(u) == idHash.end())
			// {
			//     idHash[u] = nodeCnt++;
			// }
			// if (idHash.find(v) == idHash.end())
			// {
			//     idHash[v] = nodeCnt++;
			// }
		}
#ifdef TEST
		printf("%d Records in Total\n", cnt);
#endif
	}

	void constructGraph() {
		auto tmp = inputs;
		sort(tmp.begin(), tmp.end());
		tmp.erase(unique(tmp.begin(), tmp.end()), tmp.end());

		nodeCnt = 0;
		for (ui& x : tmp) {

			idHash[x] = nodeCnt++;
		}
		idsStr = new char[nodeCnt * 11];
		idsStrIndex = new int[nodeCnt + 1];
		idsStrIndex[0] = 0;
		char* p = &idsStr[0];
		int index = 0; int step = 0;
		for (int i = 0; i < nodeCnt; i++)
		{
			step = append_uint_to_str(p, tmp[i]);
			p += step;
			index += step;
			idsStrIndex[i + 1] = index;
		}
#ifdef TEST
		printf("%d Nodes in Total\n", nodeCnt);
#endif
		int sz = inputs.size();
		//G=new vector<int>[nodeCnt];
		G = vector<vector<connection>>(nodeCnt);
		invG = vector<vector<connection>>(nodeCnt);

		//inDegrees = vector<int>(nodeCnt, 0);
		int u, v;
		for (int i = 0; i < sz; i += 2) {
			u = idHash[inputs[i]]; v = idHash[inputs[i + 1]];
			struct connection conn = { v, inputs_amount[i / 2] };
			G[u].push_back(conn);
			struct connection invConn = { u, inputs_amount[i / 2] };
			invG[v].push_back(invConn);
		}
		for (int i = 0; i < nodeCnt; i++)
		{
			sort(G[i].begin(), G[i].end());
		}

	}

	//void printNode()
	//{
	//    string s("nodes.txt");
	//    ofstream f(s.c_str());
	//    for (int i = 0; i < nodeCnt; i++)
	//        f << i << "," << node_to_input[i] << endl;
	//    f.close();

	//}

	void dfs(int head, int cur, int depth, int thread_num, char* path_new, char* path_head, long long startAmount, long long preAmount) {
		vis[thread_num][cur] = true;
		//        int len = idsStrStep[cur];
		int len = idsStrIndex[cur + 1] - idsStrIndex[cur];
		memcpy(path_new, &idsStr[idsStrIndex[cur]], len * sizeof(char));
		path_new += len;

		for (connection& conn : G[cur]) {
			ui v = conn.next;
			long long amount = conn.amount;
			if (depth == 1 && v > head)
			{
				preAmount = amount;
				dfs(head, v, depth + 1, thread_num, path_new, path_head, preAmount, preAmount);
			}
			else if (v == head && depth >= 3
				&& (preAmount <= five * amount && amount <= three * preAmount)
				&& (amount <= five * startAmount && startAmount <= three * amount)
				)
			{
				//memcpy(&ans[depth - 3][n_ans[depth - 3]++ * depth], path_new - depth, depth * sizeof(int));
				memcpy(&ans[depth - 3][thread_num][ans_top[depth - 3][thread_num]], path_head, path_new - path_head);
				ans_top[depth - 3][thread_num] += path_new - path_head;
				n_ans[depth - 3][thread_num]++;
				ans[depth - 3][thread_num][ans_top[depth - 3][thread_num] - 1] = '\n';
			}
			else if (depth < 8 && !vis[thread_num][v] && v > head
				&& (five * amount >= preAmount && amount <= three * preAmount)
				)
			{
				if (depth < 4)
					dfs(head, v, depth + 1, thread_num, path_new, path_head, startAmount, amount);
				else if (depth < 7)
				{
					if (onestep_reach[thread_num * nodeCnt + v])
						dfs(head, v, depth + 1, thread_num, path_new, path_head, startAmount, amount);
				}
				else if (direct_reach[thread_num * nodeCnt + v]) {
					long long amount_final = direct_reach[thread_num * nodeCnt + v];
					if ((five * amount_final >= amount && amount_final <= three * amount) &&
						(five * startAmount >= amount_final && startAmount <= three * amount_final))
					{
						int len8 = idsStrIndex[v + 1] - idsStrIndex[v];
						memcpy(path_new, &idsStr[idsStrIndex[v]], len8 * sizeof(char));
						path_new += len8;
						memcpy(&ans[5][thread_num][ans_top[5][thread_num]], path_head, path_new - path_head);
						ans_top[5][thread_num] += path_new - path_head;
						n_ans[5][thread_num]++;
						ans[5][thread_num][ans_top[5][thread_num] - 1] = '\n';
						path_new -= len8;
					}
				}
			}
		}
		vis[thread_num][cur] = false;
		path_new -= len;
	}


	void start_threaded_solve(int thread_num, char* path_new)
	{
		const double thread_cntd = (double)thread_cnt;
		const double thread_numd = (double)thread_num;
		const double nodeCntd = (double)nodeCnt;
		const int nodeMin = nodeCntd * (1.0 - pow((thread_cntd - thread_numd) / thread_cntd, 1.0 / 5.0));
		const int nodeMax = nodeCntd * (1.0 - pow((thread_cntd - thread_numd - 1.0) / thread_cntd, 1.0 / 5.0));
		char* path_head = path_new;

		for (int i = nodeMin; i < nodeMax; i++)
		{
			for (connection& c : invG[i])
			{
				ui v = c.next;
				long long amount = c.amount;
				if (v < i) continue;
				onestep_reach[v + thread_num * nodeCnt] = true;
				direct_reach[v + thread_num * nodeCnt] = amount;
				for (connection& cc : invG[v])
				{
					ui vv = cc.next;
					if (vv < i) continue;
					onestep_reach[vv + thread_num * nodeCnt] = true;
					for (connection& ccc : invG[vv])
					{
						ui vvv = ccc.next;
						if (vvv < i) continue;
						onestep_reach[vvv + thread_num * nodeCnt] = true;
						for (connection& cccc : invG[vvv])
						{
							ui vvvv = cccc.next;
							if (vvvv < i) continue;
							onestep_reach[vvvv + thread_num * nodeCnt] = true;
						}
					}
				}
			}

			if (!G[i].empty()) {
				dfs(i, i, 1, thread_num, path_new, path_head, 0, 0);
			}

			for (connection& c : invG[i])
			{
				ui v = c.next;
				long long amount = c.amount;
				if (v < i) continue;
				onestep_reach[v + thread_num * nodeCnt] = false;
				direct_reach[v + thread_num * nodeCnt] = 0;
				for (connection& cc : invG[v])
				{
					ui vv = cc.next;
					if (vv < i) continue;
					onestep_reach[vv + thread_num * nodeCnt] = false;
					for (connection& ccc : invG[vv])
					{
						ui vvv = ccc.next;
						if (vvv < i) continue;
						onestep_reach[vvv + thread_num * nodeCnt] = false;
						for (connection& cccc : invG[vvv])
						{
							ui vvvv = cccc.next;
							if (vvvv < i) continue;
							onestep_reach[vvvv + thread_num * nodeCnt] = false;
						}
					}
				}
			}
		}
	}

	//search from 0...n
	//鐢变簬瑕佹眰id鏈€灏忕殑鍦ㄥ墠锛屽洜姝ゆ悳绱㈢殑鍏ㄨ繃绋嬩腑涓嶈€冭檻姣旇捣鐐筰d鏇村皬鐨勮妭鐐�
	void solve() {

		for (int i = 0; i < thread_cnt; i++)
		{
			ans[0][i] = new char[1000000 * 40];
			ans[1][i] = new char[2000000 * 50];
			ans[2][i] = new char[3000000 * 60];
			ans[3][i] = new char[4000000 * 70];
			ans[4][i] = new char[10000000 * 80];
			ans[5][i] = new char[15000000 * 90];
			ans_top[0][i] = 0;
			ans_top[1][i] = 0;
			ans_top[2][i] = 0;
			ans_top[3][i] = 0;
			ans_top[4][i] = 0;
			ans_top[5][i] = 0;
			n_ans[0][i] = 0;
			n_ans[1][i] = 0;
			n_ans[2][i] = 0;
			n_ans[3][i] = 0;
			n_ans[4][i] = 0;
			n_ans[5][i] = 0;
		}

		vis = vector<vector<bool>>(thread_cnt, vector<bool>(nodeCnt, false));

		char* path_new_char = new char[200 * thread_cnt];
		//char* path_head = path_new_char;

		direct_reach = new long long[nodeCnt * thread_cnt];
		//direct_reach_amount = new double[nodeCnt * thread_cnt];
		onestep_reach = new bool[nodeCnt * thread_cnt];
		memset(direct_reach, 0, nodeCnt * thread_cnt);
		memset(onestep_reach, false, nodeCnt * thread_cnt);

		thread th[thread_cnt];
		for (int i = 0; i < thread_cnt; i++)
		{
			th[i] = thread(&Solution::start_threaded_solve, this, i, &path_new_char[i * 200]);
		}
		for (int i = 0; i < thread_cnt; i++)
		{
			th[i].join();
		}
	}

	void save(string& outputFile) {
		int count = 0;
		for (int i = 0; i < 6; i++) {
			for (int j = 0; j < thread_cnt; j++)
				count += n_ans[i][j];
		}
		ofstream out(outputFile);
		out << count << '\n';
		for (int i = 0; i < 6; i++)
		{
			for (int j = 0; j < thread_cnt; j++)
				out.write((char*)ans[i][j], ans_top[i][j]);
		}
		out.close();
	}

	//    void save(string& outputFile) {
	//        int count = 0;
	//        //        for(auto &a:ans_arr){
	//        //            count += a.size();
	//        //        }
	//        for (int i = 0; i < 5; i++) {
	//            for (int j = 0; j < thread_cnt; j++)
	//                count += n_ans[i][j];
	//        }
	//#ifdef TEST
	//        printf("Total Loops %d\n", count);
	//#endif
	//        // int fd = open(outputFile.c_str(),O_RDWR|O_CREAT|O_TRUNC,0666);
	//        FILE* fp = fopen(outputFile.c_str(), "w");
	//        // ofstream out(outputFile);
	//        // char* dst_ptr = (char*)mmap(0,count* 80,PROT_WRITE|PROT_READ,MAP_SHARED,fd,0);
	//        // truncate(outputFile.c_str(),count* 80);
	//        char* p = new char[80];
	//        char* pp = (char*)p;
	//        pp += append_uint_to_str(pp, count);
	//        pp[-1] = '\n';
	//        fwrite(p, 1, pp - p, fp);
	//        //int tmp[7];
	//        //        for(auto &a:ans_arr){
	//        //            int sz=a[0].length;
	//        //            for(auto &x:a){
	//        //                for(int i=0;i<sz;i++)
	//        //                {
	//        //                    pp += append_uint_to_str(pp, x.path[i]);
	//        //                }
	//        //                pp[-1] = '\n';
	//        //            }
	//        //        }
	//        //for (int i = 0; i < 5; i++) {
	//        //    for (int j = 0; j < n_ans[i]; j++) {
	//        //        for (int k = 0; k < i + 3; k++)
	//        //        {
	//        //            pp += append_uint_to_str(pp, ans[i][j * (i + 3) + k]);
	//        //        }
	//        //        pp[-1] = '\n';
	//        //    }
	//        //}
	//        for (int i = 0; i < 5; i++)
	//        {
	//            for (int j=0;j<thread_cnt;j++)
	//            //            memcpy(pp, ans[i], ans_top[i]);
	//                fwrite(ans[i][j], 1, ans_top[i][j], fp);
	//            //            pp += ans_top[i];
	//        }
	//        // memcpy(dst_ptr,p,pp-p);
	//        // truncate(outputFile.c_str(),pp-dst_ptr);
	//        // munmap(dst_ptr,pp-p);
	//        // out.write(p, pp - p);
	////        fwrite(p, 1, pp - p, fp);
	//    }
};

int main()
{
#ifdef _WIN64
	//string testFile = "./temp_data/2755223/test_data.txt";
	string testFile = "./data/fusai_final/test_data.txt";
	clock_t start, finish;
	double totaltime;
	start = clock();
#elif defined TEST
	string testFile = "/root/lhb/chusai/data/77409/test_data.txt";
#else
	string testFile = "/data/test_data.txt";
#endif

#ifdef _WIN64
	string outputFile = "./data/myresults.txt";
#else
	string outputFile = "/projects/student/result.txt";
#endif 

#ifdef TEST
	string answerFile = "answer.txt";
#endif
#ifdef TEST
	auto t = clock();
#endif
	//    for(int i=0;i<100;i++){
	Solution solution;
#ifdef MYTIME
	struct timeval ov_start, ov_end;
	gettimeofday(&ov_start, NULL);
#endif
	solution.parseInput(testFile);
#ifdef MYTIME
	gettimeofday(&ov_end, NULL);
	double timeuse
		= 1000000 * (ov_end.tv_sec - ov_start.tv_sec)
		+ ov_end.tv_usec - ov_start.tv_usec;
	cout << "parseInput: " << timeuse << " us" << endl;
#endif
#ifdef MYTIME
	gettimeofday(&ov_start, NULL);
#endif
	solution.constructGraph();
#ifdef MYTIME
	gettimeofday(&ov_end, NULL);
	timeuse
		= 1000000 * (ov_end.tv_sec - ov_start.tv_sec)
		+ ov_end.tv_usec - ov_start.tv_usec;
	cout << "constructGraph: " << timeuse << " us" << endl;
#endif
	//solution.topoSort();
#ifdef MYTIME
	gettimeofday(&ov_start, NULL);
#endif
	solution.solve();
#ifdef MYTIME
	gettimeofday(&ov_end, NULL);
	timeuse
		= 1000000 * (ov_end.tv_sec - ov_start.tv_sec)
		+ ov_end.tv_usec - ov_start.tv_usec;
	cout << "solve: " << timeuse << " us" << endl;
#endif
#ifdef MYTIME
	gettimeofday(&ov_start, NULL);
#endif
	solution.save(outputFile);
#ifdef MYTIME
	gettimeofday(&ov_end, NULL);
	timeuse
		= 1000000 * (ov_end.tv_sec - ov_start.tv_sec)
		+ ov_end.tv_usec - ov_start.tv_usec;
	cout << "save: " << timeuse << " us" << endl;
#endif
#ifdef TEST
	cout << clock() - t << endl;
#endif

#ifdef _WIN64
	finish = clock();
	totaltime = (double)(finish - start) / CLOCKS_PER_SEC;
	cout << "Run:  " << totaltime << "  Seconds" << endl;
#endif
	return 0;
}
