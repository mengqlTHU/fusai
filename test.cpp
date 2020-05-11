#ifndef CIRCUITFINDER_H
#define CIRCUITFINDER_H

#define _CRT_SECURE_NO_WARNINGS

#include <algorithm>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <set>
#include <chrono>
#include <assert.h>
#include <string.h>

using namespace std;
using namespace chrono;

// #define TIMERS

#ifdef MYTIME
#include <sys/time.h>
#endif

#ifdef TIMERS
#include "Timers.hpp"
#endif

int sizeTable[10] = { 9, 99, 999, 9999, 99999, 999999, 9999999,
  99999999, 999999999, INT32_MAX };

int intSize(int x)
{
	for (int i = 0; i < 10; ++i)
	{
		if (x <= sizeTable[i]) return i + 1;
	}
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

struct connection {
	int next;
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

class CircuitFinder
{
	vector<vector<connection>> AK;
	vector<vector<int>> inAK;
	//vector<NodeList> subAK;
	vector<int> Stack;
	//std::vector<bool> Blocked;
	bool* Blocked;
	bool* hasInEdge;
	bool* hasOneStepEdge;
	//std::vector<bool> falseBlocked;
	//std::vector<vector<int>> B;
	int* B;
	int* sizeB;
	//   map<int, int> m;
	vector<int> nodes;
	vector<vector<vector<int>>> resVect;
	int N;
	int circuitCount;
	int S;
	bool isDenseGraph;
	time_point<system_clock, nanoseconds> start;

	void unblock(int U);
	bool circuit(int V, long long preAmount, long long startAmount);
	//bool circuitSubGraph(int V);
	//vector<NodeList> getSubGraph(set<int> s);

	//void circuitIterate(int V);
	void output();
	int findMin();
	static bool compareVector(vector<int> v1, vector<int> v2);
	void outputTime(string info);
	void sortVector();
	void removeNode(int V);
	void printVector(string filename);
	void printMap();
	void strongComponent();
	void runInSubGraph(set<int> s);
	void runInSubGraph(int* s, int len);

public:
	CircuitFinder()
		:resVect(5) //3,4,5,6,7
	{
		N = 0;
		circuitCount = 0;
		start = system_clock::now();
	}

	void run();
	void loadTestData(string filename);
};

//Tarjan??ùùùùùù?ùù?ùùùù
//https://en.wikipedia.org/wiki/Tarjan%27s_strongly_connected_components_algorithm
void CircuitFinder::strongComponent()
{
	//    unordered_map<int, int> preOrder;
	//    unordered_map<int, int> lowLink;
	//    set<int> sccFound;
	//    vector<int> sccQueue;
#ifdef _WIN64
	//const int n = 1000000;
	int* preOrder_arr = new int[N];
	int* sccFound_arr = new int[N];
	int* lowLink_arr = new int[N];
	int* q_arr = new int[N];
	int q_n = 0;
	int* sccQueue_arr = new int[N];
	int sccQueue_n = 0;
	int* scc_arr = new int[N];;
#else
	int* preOrder_arr = new int[N];
	int* sccFound_arr = new int[N];
	int* lowLink_arr = new int[N];
	int* q_arr = new int[N];
	int q_n = 0;
	int* sccQueue_arr = new int[N];
	int sccQueue_n = 0;
	int* scc_arr = new int[N];
#endif
	int v;
	bool done;
	int i = 0;
	for (int node = 0; node < N; node++)
	{
		preOrder_arr[node] = -1;
		sccFound_arr[node] = -1;
		scc_arr[node] = -1;
	}
	for (int node = 0; node < N; node++)
	{
		//        vector<int> q;
		//        if (sccFound.find(node)==sccFound.end())
		if (sccFound_arr[node] == -1)
		{
			//            q.push_back(node);
			q_arr[q_n++] = node;
		}

		while (q_n > 0)
		{
			//            v = q.back();
			v = q_arr[q_n - 1];
			if (preOrder_arr[v] == -1)
			{
				i++;
				//                preOrder[v] = i;
				preOrder_arr[v] = i;
			} //else assert(preOrder[v]==preOrder_arr[v]);
			done = true;
			for (connection conn : AK[v])
			{
				int w = conn.next;
				if (preOrder_arr[w] == -1)
				{
					//                    q.push_back(w);
					q_arr[q_n++] = w;
					done = false;
					break;
				}
			}
			if (done)
			{
				//                lowLink[v] = preOrder_arr[v];
				lowLink_arr[v] = preOrder_arr[v];
				for (connection conn : AK[v])
				{
					int w = conn.next;
					if (sccFound_arr[w] == -1)
					{
						if (preOrder_arr[w] > preOrder_arr[v])
							//                            lowLink[v] = lowLink[v] < lowLink[w] ? lowLink[v] : lowLink[w];
							lowLink_arr[v] = min(lowLink_arr[v], lowLink_arr[w]);
						else
							//                            lowLink[v] = lowLink[v] < preOrder_arr[w] ? lowLink[v] : preOrder_arr[w];
							lowLink_arr[v] = min(lowLink_arr[v], preOrder_arr[w]);
					}
				}
				//                q.pop_back();
				q_n--;
				if (lowLink_arr[v] == preOrder_arr[v])
				{
					set<int> scc;
					// int scc_val[sccQueue_n+1];
					// int scc_i = 0;
					// scc_val[scc_i++] = v;
					scc.insert(v);
					// scc_arr[v] = 1;
		//                    while (!sccQueue.empty() && preOrder_arr[sccQueue.back()] > preOrder_arr[v])
					while (sccQueue_n > 0 && preOrder_arr[sccQueue_arr[sccQueue_n - 1]] > preOrder_arr[v])
					{
						// int k = sccQueue.back();
						int k = sccQueue_arr[--sccQueue_n];
						//                        sccQueue.pop_back();
						scc.insert(k);
						// scc_arr[k] = 1;
						// scc_val[scc_i++]=k;
					}

					//vector<NodeList> subAK = getSubGraph(scc);
   //                  for (int ii=0;ii<scc_i;ii++)
		 //    {
			// int tmp = scc_val[ii];
   //                      for (vector<int>::iterator iter = AK[tmp].begin(); iter != AK[tmp].end(); )
			// { 
   //                          if (scc_arr[*iter] == -1)
   //                              iter = AK[tmp].erase(iter); // advances iter
   //                          else
   //                              ++iter; // don't remove
   //                      }
			// sccFound_arr[tmp] = 1;
   //                  }

					for (int W : scc)
					{
						for (vector<connection>::iterator iter = AK[W].begin(); iter != AK[W].end(); )
						{
							// if (scc_arr[*iter] == -1)
							if (scc.find(iter->next) == scc.end())
								iter = AK[W].erase(iter); // advances iter
							else
								++iter; // don't remove
						}
						sccFound_arr[W] = 1;
					}

					if (scc.size() > 2)
						runInSubGraph(scc);
					// runInSubGraph(scc_val, scc_i);

//                    sccFound.insert(scc.begin(), scc.end());
				}
				else
				{
					//                    sccQueue.push_back(v);
					sccQueue_arr[sccQueue_n++] = v;
				}
			}
		}
	}

}

//unblock?ùù?
//ùùJohnsonùùùù?ùùùùùù???ù£ùù?Johnson?ùlist?ùùùùùùùùùù??ùvector?ùùùùùù
void CircuitFinder::unblock(int U)
{
	Blocked[U] = false;

	//while (!B[U].empty()) {
	//  int W = B[U].back();
	//  B[U].pop_back();

	//  if (Blocked[W]) {
	//    unblock(W);
	//  }
	//}
	while (sizeB[U]) {
		int W = B[U * 10 + sizeB[U]];
		sizeB[U]--;

		if (Blocked[W]) {
			unblock(W);
		}
	}

}

//ùùùùùù?ùù
//ùùùù?ùùùùùùùù?ùùùù?ùù????ùù?ùù?ùùnodesùùùùùù
//AK???ùùùùùvector<vector<int>>?ùù
void CircuitFinder::loadTestData(string filename)
{
#ifdef _WIN64
	unordered_map<int, int> intHash;
#else
	unordered_map<int, int> intHash(20000);
#endif
	ifstream indata;
	indata.open(filename);
	string line;
	int vertexIndex = 0;
	int recordIndex = 0;
	//int* tempAK = (int*)malloc(sizeof(int) * INT32_MAX * 50);
	//int* sizeAK = (int*)malloc(sizeof(int) * INT32_MAX);
	int* outArr = (int*)malloc(sizeof(int) * 2000000);
	int* inArr = (int*)malloc(sizeof(int) * 2000000);
	int* amountArr = (int*)malloc(sizeof(int) * 2000000);
	set<int> tempNodes;
	unordered_map<int, int> reverseNodes;

	while (getline(indata, line)) {
		char* s = &line[0];

		int tmp = 0;
		int accountOut = 0, accountIn = 0, amount = 0;
		while (*s != ',')
		{
			accountOut = (*s++ - '0') + accountOut * 10;
		}
		s++;
		while (*s != ',')
		{
			accountIn = (*s++ - '0') + accountIn * 10;
		}
		s++;
		while (*s)
		{
			amount = (*s++ - '0') + amount * 10;
		}
		s++;

		outArr[recordIndex] = accountOut;
		inArr[recordIndex] = accountIn;
		amountArr[recordIndex] = amount;
		recordIndex++;

		if (intHash.find(accountOut) == intHash.end())
		{
			intHash[accountOut] = vertexIndex++;
			//nodes.push_back(accountOut);
			tempNodes.insert(accountOut);
			//sizeAK[accountOut] = 0;
			//AK.push_back(vector<int>());
			//Blocked.push_back(false);
			//B.push_back(vector<int>());
		}

		if (intHash.find(accountIn) == intHash.end())
		{
			intHash[accountIn] = vertexIndex++;
			tempNodes.insert(accountIn);
			//sizeAK[accountIn] = 0;
			//AK.push_back(vector<int>());
			//Blocked.push_back(false);
			//B.push_back(vector<int>());
		}

		//AK[intHash[accountOut]].push_back(intHash[accountIn]); // 400us
		//tempAK[accountOut * 50 + sizeAK[accountOut]] = accountIn;
		//sizeAK[accountOut]++;
	}
	N = vertexIndex;

	B = (int*)malloc(sizeof(int) * N * 10);
	sizeB = (int*)malloc(sizeof(int) * N);
	Blocked = (bool*)malloc(sizeof(bool) * N);
	hasInEdge = (bool*)malloc(sizeof(bool) * N);
	hasOneStepEdge = (bool*)malloc(sizeof(bool) * N);

	nodes.resize(tempNodes.size());
	copy(tempNodes.begin(), tempNodes.end(), nodes.begin());

	for (int i = 0; i < N; i++)
	{
		Blocked[i] = false;
		hasInEdge[i] = false;
		hasOneStepEdge[i] = false;
		sizeB[i] = 0;
		reverseNodes[nodes[i]] = i;
	}

	AK.resize(N);
	inAK.resize(N);

	for (int i = 0; i < recordIndex; i++)
	{
		struct connection conn = { reverseNodes[inArr[i]], amountArr[i] };
		AK[reverseNodes[outArr[i]]].push_back(conn);
		//if (reverseNodes[outArr[i]]> reverseNodes[inArr[i]])
		inAK[reverseNodes[inArr[i]]].push_back(reverseNodes[outArr[i]]);
	}

	for (int i = 0; i < N; i++)
	{
		sort(AK[i].begin(), AK[i].end());
	}

	if (((double)recordIndex / (double)N / (double)N) > 0.00005)
		isDenseGraph = true;
	else
		isDenseGraph = false;

	//for (int i = 0; i < N; i++)
	//{
	//	for (int j = 0; j < sizeAK[nodes[i]]; j++)
	//	{
	//		AK[i].push_back(reverseNodes[tempAK[nodes[i] * 50 + j]]);
	//	}
	//	sort(AK[i].begin(), AK[i].end());
	//}

	free(outArr);
	free(inArr);
	//falseBlocked = Blocked;
#ifdef mydebug
	outputTime("Load Data");
	printMap();
#endif
}


//?ùùùù?ùùùù?
//ùùJohnsonùùùù?ù£ùù?ùùùùùùùùùùùù?ùù???
bool CircuitFinder::circuit(int V, long long preAmount, long long startAmount)
{
	bool F = false;

	Stack.push_back(V);
	Blocked[V] = true;

	auto circuitLen = Stack.size();
	if (circuitLen < 5 || circuitLen==6)
	{
		for (connection &conn : AK[V]) {
			const int W = conn.next;
			if (circuitLen == 1)
			{
				startAmount = preAmount = conn.amount;
			}
			long long amount = conn.amount;
			if (!Blocked[W]
				&& (5 * amount >= preAmount && amount <= 3 * preAmount))
				F = circuit(W, amount, startAmount) || F;
			else if (W == S
				&& (preAmount <= 5 * amount && amount <= 3 * preAmount)
				&& (amount <= 5 * startAmount && startAmount <= 3 * amount)
			) {
				output();
				F = true;
			}
		}
	}
	else if (circuitLen == 5)
	{
		for (connection& conn : AK[V]) {
			const int W = conn.next;
			long long amount = conn.amount;
			if (!Blocked[W] 
				&& hasInEdge[W]
				&& (5 * amount >= preAmount && amount <= 3 * preAmount)
			)
				circuit(W, amount, startAmount);
			else if (W == S
				&& (preAmount <= 5 * amount && amount <= 3 * preAmount)
				&& (amount <= 5 * startAmount && startAmount <= 3 * amount)
			) {
				output();
			}
		}
		F = true;
	}
	else if (circuitLen == 7)
	{
		for (connection& conn : AK[V]) {
			const int W = conn.next;
			long long amount = conn.amount;
			if (W == S
				&& (preAmount <= 5 * amount && amount <= 3 * preAmount)
				&& (amount <= 5 * startAmount && startAmount <= 3 * amount)
				) {
				output();
				F = true;
			}
		}
	}

	//else if (circuitLen == 6 || circuitLen == 8)
	//{
	//	for (connection& conn : AK[V]) {
	//		const int W = conn.next;
	//		if (W == S) {
	//			output();
	//		}
	//		else if (!Blocked[W] && hasOneStepEdge[W])
	//		{
	//			Stack.push_back(W);
	//			output();
	//			Stack.pop_back();
	//			unblock(W);
	//		}
	//	}
	//	F = true;
	//}
	else
		F = true;

	if (F) {
		unblock(V);
	}
	else {
		for (connection& conn : AK[V]) {
			//auto IT = std::find(B[W].begin(), B[W].end(), V);
			//if (IT == B[W].end()) {
			//    B[W].push_back(V);
			//}
			const int W = conn.next;
			bool discovered = false;
			for (int i = 0; i < sizeB[W]; i++)
			{
				if (B[W * 10 + i] == V)
				{
					discovered = true;
					break;
				}
			}
			if (!discovered)
			{
				sizeB[W]++;
				B[W * 10 + sizeB[W]] = V;
			}
		}
	}

	Stack.pop_back();
	return F;
}

//??ùùùù????ùùùù?ùùùùùù?ùù?ùù
int CircuitFinder::findMin()
{
	int min = nodes[*Stack.begin()];
	int idOfMin = 0;
	int id = 0;
	for (auto I = Stack.begin() + 1, E = Stack.end(); I != E; ++I)
	{
		id++;
		if (nodes[*I] < min)
		{
			min = nodes[*I];
			idOfMin = id;
		}
	}
	return idOfMin;
}

//?ùùùùùùùù?ùùùùùùùù?ùùùù?
bool CircuitFinder::compareVector(vector<int> v1, vector<int> v2)
{
	for (int i = 0; i < v1.size(); i++)
	{
		if (v1[i] != v2[i])
			return v1[i] < v2[i];
	}
	return true;
}

//ùù??ùùùùùùùù
void CircuitFinder::outputTime(string info)
{
	auto duration = duration_cast<microseconds>(system_clock::now() - start);
	cout << info
		<< double(duration.count()) * microseconds::period::num / microseconds::period::den
		<< "Seconds" << endl;
}


//Circuitùùùù??ùùùùùù??ùoutput,ùù?ùùresVect
void CircuitFinder::output()
{
	auto circuitLen = Stack.size();
	if (circuitLen > 2)
	{
		resVect[circuitLen - 3].push_back(vector<int>());
		//int idOfMin = findMin();
		int idOfMin = 0;
		for (int i = idOfMin; i < circuitLen + idOfMin; i++)
		{
			//auto I = Stack.begin() + (i % circuitLen);
			auto I = Stack.begin() + i;
			resVect[circuitLen - 3].back().push_back(nodes[*I]);
		}
		circuitCount += 1;
	}
}


//ùù???ùù??ùùùùùùùùùù?
void CircuitFinder::sortVector()
{
	for (int i = 0; i < 5; i++)
	{
		if (resVect[i].size() > 0)
		{
			sort(resVect[i].begin(), resVect[i].end(), compareVector);
		}
	}
}

//?ùùùùùùùù
void CircuitFinder::printVector(string filename)
{
	ofstream fout(filename);
	fout << circuitCount << endl;
	 //for (int i=0;i<5;i++)
	 //{
	 //    for (int j=0;j<resVect[i].size();j++)
	 //    {
	 //        fout << resVect[i][j][0];
	 //        for (int k=1;k<i+3;k++)
	 //            fout << "," << resVect[i][j][k];
	 //        fout << endl;
	 //    }
	 //}
	int n_entry = 0;
	for (int i = 0; i < 5; i++) n_entry += resVect[i].size();
	char* p = new char[n_entry * 80];
	char* pp = p;
	// ???3
	int idx = 0;
	for (int j = 0; j < resVect[idx].size(); j++)
	{
		sprintf(pp, "%d,%d,%d\n", resVect[idx][j][idx], resVect[idx][j][1], resVect[idx][j][2]);
		pp += intSize(resVect[idx][j][0]) + intSize(resVect[idx][j][1]) + intSize(resVect[idx][j][2]) + 3;
	}
	// ???4
	idx = 1;
	for (int j = 0; j < resVect[idx].size(); j++)
	{
		// int tmp[4] = {intSize(resVect[idx][j][0]), intSize(resVect[idx][j][1]), intSize(resVect[idx][j][2]), intSize(resVect[idx][j][3])};
		// pp += append_uint_to_str(pp, resVect[idx][j][0], tmp[0]);
		// pp += append_uint_to_str(pp, resVect[idx][j][1], tmp[1]);
		// pp += append_uint_to_str(pp, resVect[idx][j][2], tmp[2]);
		// pp += append_uint_to_str(pp, resVect[idx][j][3], tmp[3]);
		// pp[-1] = '\n';
		sprintf(pp, "%d,%d,%d,%d\n", resVect[idx][j][0], resVect[idx][j][1], resVect[idx][j][2], resVect[idx][j][3]);
		pp += intSize(resVect[idx][j][0]) + intSize(resVect[idx][j][1]) + intSize(resVect[idx][j][2]) + intSize(resVect[idx][j][3]) + 4;
	}
	// ???5
	idx = 2;
	for (int j = 0; j < resVect[idx].size(); j++)
	{
		int tmp[5] = { intSize(resVect[idx][j][0]), intSize(resVect[idx][j][1]), intSize(resVect[idx][j][2]), intSize(resVect[idx][j][3]), intSize(resVect[idx][j][4]) };
		pp += append_uint_to_str(pp, resVect[idx][j][0], tmp[0]);
		pp += append_uint_to_str(pp, resVect[idx][j][1], tmp[1]);
		pp += append_uint_to_str(pp, resVect[idx][j][2], tmp[2]);
		pp += append_uint_to_str(pp, resVect[idx][j][3], tmp[3]);
		pp += append_uint_to_str(pp, resVect[idx][j][4], tmp[4]);
		pp[-1] = '\n';
		// sprintf(pp,"%d,%d,%d,%d,%d\n",resVect[idx][j][0],resVect[idx][j][1],resVect[idx][j][2],resVect[idx][j][3],resVect[idx][j][4]);
		// pp += intSize(resVect[idx][j][0])+intSize(resVect[idx][j][1])+intSize(resVect[idx][j][2])+intSize(resVect[idx][j][3])+intSize(resVect[idx][j][4])+5;
	}
	// // ???6
	idx = 3;
	for (int j = 0; j < resVect[idx].size(); j++)
	{
		int tmp[6] = { intSize(resVect[idx][j][0]), intSize(resVect[idx][j][1]), intSize(resVect[idx][j][2]), intSize(resVect[idx][j][3]), intSize(resVect[idx][j][4]),
			  intSize(resVect[idx][j][5]) };
		pp += append_uint_to_str(pp, resVect[idx][j][0], tmp[0]);
		pp += append_uint_to_str(pp, resVect[idx][j][1], tmp[1]);
		pp += append_uint_to_str(pp, resVect[idx][j][2], tmp[2]);
		pp += append_uint_to_str(pp, resVect[idx][j][3], tmp[3]);
		pp += append_uint_to_str(pp, resVect[idx][j][4], tmp[4]);
		pp += append_uint_to_str(pp, resVect[idx][j][5], tmp[5]);
		pp[-1] = '\n';
		// sprintf(pp,"%d,%d,%d,%d,%d,%d\n",resVect[idx][j][0],resVect[idx][j][1],resVect[idx][j][2],resVect[idx][j][3],resVect[idx][j][4],resVect[idx][j][5]);
		// pp += intSize(resVect[idx][j][0])+intSize(resVect[idx][j][1])+intSize(resVect[idx][j][2])+intSize(resVect[idx][j][3])+intSize(resVect[idx][j][4])+intSize(resVect[idx][j][5])+6;
	}
	// // ???7
	idx = 4;
	// printf("%d,%d\n", pp-p,resVect[idx].size());
	for (int j = 0; j < resVect[idx].size(); j++)
	{
		int tmp[7] = { intSize(resVect[idx][j][0]), intSize(resVect[idx][j][1]), intSize(resVect[idx][j][2]), intSize(resVect[idx][j][3]), intSize(resVect[idx][j][4]),
			  intSize(resVect[idx][j][5]),intSize(resVect[idx][j][6]) };
		pp += append_uint_to_str(pp, resVect[idx][j][0], tmp[0]);
		pp += append_uint_to_str(pp, resVect[idx][j][1], tmp[1]);
		pp += append_uint_to_str(pp, resVect[idx][j][2], tmp[2]);
		pp += append_uint_to_str(pp, resVect[idx][j][3], tmp[3]);
		pp += append_uint_to_str(pp, resVect[idx][j][4], tmp[4]);
		pp += append_uint_to_str(pp, resVect[idx][j][5], tmp[5]);
		pp += append_uint_to_str(pp, resVect[idx][j][6], tmp[6]);
		pp[-1] = '\n';
		// sprintf(pp,"%d,%d,%d,%d,%d,%d,%d\n",resVect[idx][j][0],resVect[idx][j][1],resVect[idx][j][2],resVect[idx][j][3],resVect[idx][j][4],resVect[idx][j][5],resVect[idx][j][6]);
		// pp += tmp[0]+tmp[1]+tmp[2]+tmp[3]+tmp[4]+tmp[5]+tmp[6]+7;
	}
	// printf("%d,%d\n", pp-p,n_entry);
	fout.write(p, pp - p);
	fout.close();
}

//ùùùnodesùùùùùù?ùùùùùù?
void CircuitFinder::printMap()
{
	ofstream fout("../data/map.txt");
	for (int i = 0; i < N; i++)
		fout << nodes[i] << endl;
	fout.close();
}

//ùù?ùù?ùù?ùùùù?ùùùùùù????ùùùùcircuit?ùùùù?
void CircuitFinder::runInSubGraph(set<int> s)
{
	vector<int> scc(s.size());
	copy(s.begin(), s.end(), scc.begin());
	for (vector<int>::iterator iter = scc.begin(); iter != scc.end() - 2; iter++)
	{
		S = *iter;
		//for (vector<int>::iterator inner_iter = iter; inner_iter != scc.end(); inner_iter++) {
		//	sizeB[*(inner_iter)] = 0;
		//	Blocked[*(inner_iter)] = false;
		//}
		memset(Blocked + S, false, (N - S) * sizeof(*Blocked));
		memset(sizeB + S, 0, (N - S) * sizeof(*sizeB));

		if (inAK[S].size() == 0)
			continue;

		for (int W : inAK[S])
		{
			if (W > S)
			{
				hasInEdge[W] = true;
				hasOneStepEdge[W] = true;
				for (int Y : inAK[W])
					if (Y > S)
						hasInEdge[Y] = true;
			}
		}

		circuit(S,0,0);
		removeNode(S);

		for (int W : inAK[S])
		{
			if (W > S)
			{
				hasInEdge[W] = false;
				hasOneStepEdge[W] = false;
				for (int Y : inAK[W])
					if (Y > S)
						hasInEdge[Y] = false;
			}
		}

#ifdef mydebug
		outputTime("A S cycle");
		cout << S << endl;
#endif

	}
}

void CircuitFinder::runInSubGraph(int* s, int len)
{
	for (int i = 0; i < len; i++)
	{
		S = s[i];
		for (int j = i; j < len; j++)
		{
			//        for (set<int>::iterator inner_iter = iter; inner_iter != s.end(); inner_iter++) {
			sizeB[s[j]] = 0;
			Blocked[s[j]] = false;
		}
		circuit(S, 0, 0);

#ifdef mydebug
		outputTime("A S cycle");
		cout << S << endl;
#endif

	}
}

void CircuitFinder::removeNode(int V)
{
	for (int in : inAK[V])
	{
		connection conn = { V,0 };
		AK[in].erase(find(AK[in].begin(), AK[in].end(), conn));
	}
}

//ùùùùùù?
void CircuitFinder::run()
{
	Stack.clear();
	S = 0;
#ifdef MYTIME
	struct timeval ov_start, ov_end;
	gettimeofday(&ov_start, NULL);
#endif
	if (isDenseGraph)
	{
		while (S < N) {
			if (inAK[S].size() == 0)
			{
				S++;
				continue;
			}
			//for (int I = S; I < N; ++I) {
			//	Blocked[I] = false;
			//	sizeB[I] = 0;
			//}
			memset(Blocked+S, false, (N-S) * sizeof(*Blocked));
			memset(sizeB+S, 0, (N-S) * sizeof(*sizeB));
			for (int W : inAK[S])
			{
				if (W > S)
				{
					hasInEdge[W] = true;
					hasOneStepEdge[W] = true;
					for (int Y : inAK[W])
						if (Y > S)
							hasInEdge[Y] = true;
				}
			}
			circuit(S, 0, 0);
			removeNode(S);
			for (int W : inAK[S])
			{
				if (W > S)
				{
					hasInEdge[W] = false;
					hasOneStepEdge[W] = false;
					for (int Y : inAK[W])
						if (Y > S)
							hasInEdge[Y] = false;
				}
			}
			++S;
#ifdef mydebug
			outputTime("A S cycle");
			cout << S << endl;
#endif
		}
	}
	else
	{
		strongComponent();
		sortVector();
	}
#ifdef MYTIME
	gettimeofday(&ov_end, NULL);
	double timeuse
		= 1000000 * (ov_end.tv_sec - ov_start.tv_sec)
		+ ov_end.tv_usec - ov_start.tv_usec;
	cout << "run_circuit: " << timeuse << " us" << endl;
#endif

#ifdef mydebug
	printVector("../data/myresult_unsorted.txt");
#endif

#ifdef MYTIME
	gettimeofday(&ov_start, NULL);
#endif
#ifdef MYTIME
	gettimeofday(&ov_end, NULL);
	timeuse
		= 1000000 * (ov_end.tv_sec - ov_start.tv_sec)
		+ ov_end.tv_usec - ov_start.tv_usec;
	cout << "sortVector: " << timeuse << " us" << endl;
#endif
#ifdef MYTIME
	gettimeofday(&ov_start, NULL);
#endif

#ifdef _WIN64
	printVector("../data/myresult.txt");
#else
	printVector("/projects/student/result.txt");
#endif

#ifdef MYTIME
	gettimeofday(&ov_end, NULL);
	timeuse
		= 1000000 * (ov_end.tv_sec - ov_start.tv_sec)
		+ ov_end.tv_usec - ov_start.tv_usec;
	cout << "printVector: " << timeuse << " us" << endl;
#endif
	outputTime("Finished");
}

#endif // CIRCUITFINDER_H


int main()
{
#ifdef TIMERS
	MPI_Init(NULL, NULL);
Timer:startTimer("overall");
#endif
	CircuitFinder cf;
#ifdef MYTIME
	struct timeval ov_start, ov_end;
	gettimeofday(&ov_start, NULL);
#endif

#ifdef _WIN64
	cf.loadTestData("../data/official/test_data.txt");
	//cf.loadTestData("./test_data.txt");
#elif defined TEST
	cf.loadTestData("./data/38252/test_data.txt");
#else
	cf.loadTestData("/data/test_data.txt");
#endif

#ifdef MYTIME
	gettimeofday(&ov_end, NULL);
	double timeuse
		= 1000000 * (ov_end.tv_sec - ov_start.tv_sec)
		+ ov_end.tv_usec - ov_start.tv_usec;
	cout << "loadTestData: " << timeuse << " us" << endl;
#endif
#ifdef MYTIME
	gettimeofday(&ov_start, NULL);
#endif
	cf.run();
#ifdef MYTIME
	gettimeofday(&ov_end, NULL);
	timeuse
		= 1000000 * (ov_end.tv_sec - ov_start.tv_sec)
		+ ov_end.tv_usec - ov_start.tv_usec;
	cout << "run: " << timeuse << " us" << endl;
#endif
	return 0;
}
