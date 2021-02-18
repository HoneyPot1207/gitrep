#pragma once
#include<vector>
#include<string>
#include<deque>
#include<map>
#include<set>
#include<sstream>
#include<queue>
#include<functional>
#include<limits.h>


using namespace std;

#include"tree.h"

class prefixnode
{
public:
	prefixnode(set<int>& idx1, int depth1, deque<int> path1, int parent1, int startRecord1);
	~prefixnode();


	int parent;
	
	int truehis;

	int startRecord;
	int npart;

	set<int> idx;
	deque<int> path;
	vector<int> next;
	int depth;
	bool leaf;
	double score;

	double zvalue;
	double avalue; 
	double cvalue;
	int trueflag;

	double rvalue;
	double svalue;
	
	double rootsum; 
};

class prefix : public tree<prefixnode>
{
public:
	prefix(engine&);
	prefix(engine& eng, string filename, vector<int>& id_backup);
	~prefix();
	void load(vector<vector<int>>&, int);

	bool expandable(int);
	

	int count(vector<int>&);
	bool is_prefix(vector<int>&, pair<int, int>&);
	

	

	vector<vector<int>> tokenize(string);
	int translate(string);
	string translate(int);
	//end-of-tools

	void initialRoot(vector<int>& id);
	void computePST3_delta(int startNode, int endNode, double ep, vector<int>& id, vector<int>& priv, int c, vector<pair<int, int>>& deleteNode, int endn);
	vector<pair<vector<int>, double>> computeTruePST(vector<int>&, int number, double ep, int k);
	void split_PFS3(int idx, vector<prefixnode*>& children, int startRecord, vector<int>& id);
	
	void computeweightavg(vector<int>& hitnode, vector<double>& frequency, int number, int usenumber);
	void boost(vector<int>& hitnode, vector<double>& noisyone, int number, int usenumber);


	//vector<vector<int>> computeTruePST(vector<int>&, int number, double ep);
	int size();
	//bool compare_vector(pair<int, double>& a, pair<int, double>& b);

	map<string, int> s2i;					//size of domain(normal symbols[0, fanout - 2]; -1 = $; fanout - 1 = &)
	map<int, string> i2s;


	vector<vector<vector<int>>> orgraw;

	vector<vector<int>> raw;				//original data
	set<vector<int>> dictionary;
	map<deque<int>, vector<double>> truengrams;


};

