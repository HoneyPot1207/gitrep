#pragma once
#include<vector>
#include<fstream>
#include<algorithm>
#include<iostream>
#include<list>
using namespace std;

#include"noise.h"
#define e 2.718281828
template <typename T>
class tree
{
public:
	tree(engine& eng1) : eng(eng1) {}
	~tree() {}


	
	vector<pair<vector<int>, double>> PST3(double ep, vector<int>& id, double ratio, int k) {	//this->depthgram: 1,Ðè´ó¸Ä
		
		int eta = 4;
		vector<int> priv(1);
		
		int number1 = size() * ratio;

		int step = 8.0/(ep * ep) * 10*10;
		if(step < number1 /1000){
			step = number1/1000;
		}

		int group = number1/step;

		for(int groupi = 0; groupi < group && priv.size() > 0; ){//split the dataset into group to imprive the time efficiency

			int startNode = groupi * step;
			int endNode = (groupi + 1) * step;
			if(groupi == group - 1) endNode = number1;
			
			vector<pair<int, int>> deleteNode;
			
			if(*priv.begin() > 0){
				computePST3_delta(startNode,endNode, ep, id, priv, eta, deleteNode, number1);//compute the contributions to frequencies made by users in this group.This function returns the nodes to be expanded
				groupi ++;
			}
			else{
				deleteNode.push_back(make_pair(0, 1));
				endNode = 0;
			}
			
			if(deleteNode.size() > 0){
				
				for(int deletei = 0; deletei < deleteNode.size(); deletei ++){//expand node
			
					pair<int, int> nodelabel = deleteNode[deletei];
					if(nodelabel.second == 1){
						vector<int> children; 
						expand(priv[nodelabel.first], children, number1, id);
						priv.insert(priv.end(), children.begin(), children.end());
					}
				}
			
				for(int deletei = 0; deletei < deleteNode.size(); deletei ++){//delete the expanded nodes and the nodes consuming all users
					pair<int, int> nodelabel = deleteNode[deletei];
					priv.erase(priv.begin() + (nodelabel.first - deletei));
				}
	
			}
		}

		return computeTruePST(id, number1, ep, k);//filter the candidates
	}
	

	
	void expand(int idx, vector<int>& next, int startRecord, vector<int>& id) {

		if (nodes1[idx]->next.empty()) {
			vector<T*> children;
			split_PFS3(idx, children, startRecord, id);
			for(int x = 0; x < children.size(); x++){
				nodes1[idx]->next.push_back(nodes1.size());
				next.push_back(nodes1.size());
				nodes1.push_back(children[x]);
			}
		}
		nodes1[idx]->leaf = false;
	}
	

	



	
	virtual bool expandable(int) = 0;


	virtual void split_PFS3(int idx, vector<T*>& children, int startRecord, vector<int>& id) =  0;
	virtual void computePST3_delta(int startNode, int endNode, double ep, vector<int>& id, vector<int>& priv, int c, vector<pair<int, int>>& deleteNode, int number) = 0;
	

	virtual vector<pair<vector<int>, double>> computeTruePST(vector<int>&, int number, double ep, int k) = 0;
	//virtual vector<vector<int>> est_topk(int) = 0;
	//virtual vector<vector<int>> getPartResult(set<int>& nodeset, vector<int>& id) = 0;
	virtual int size() = 0;

	vector<T*> nodes1;
	engine& eng;
	int fanout;
	int trunlength;
	int depthgram;
	
};

