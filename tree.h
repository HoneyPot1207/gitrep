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


	
	vector<pair<vector<int>, double>> PST3(double ep, vector<int>& id, double ratio, int k) {	//this->depthgram: 1,每次循环都计算上次扩展的节点中哪些节点超过阈值，并扩展这些节点
		
		int eta = 4;
		vector<int> priv(1);//初始化了一个默认为0的元素
		
		int number1 = size() * ratio;	//size()用户的数量

		int step = 8.0/(ep * ep) * 10*10;//每一组的人数
		if(step < number1 /1000){
			step = number1/1000;
		}

		int group = number1/step;	

		for(int groupi = 0; groupi < group && priv.size() > 0; ){//split the dataset into group to imprive the time efficiency		Line2

			int startNode = groupi * step;//开始节点为第n组用户
			int endNode = (groupi + 1) * step;//结束节点为第n+1组用户
			if(groupi == group - 1) endNode = number1;//最后一组不够，则结束节点为最后一个
			
			vector<pair<int, int>> deleteNode;	//存储数对
			if(*priv.begin() > 0){
				computePST3_delta(startNode,endNode, ep, id, priv, eta, deleteNode, number1);//compute the contributions to frequencies made by users in this group.This function returns the nodes to be expanded，计算节点的频数，扩张节点
				groupi ++;//下一层
			}
			else{
				deleteNode.push_back(make_pair(0, 1));//先在deleteNode中加一个<0,1>
				endNode = 0;
			}
			
			if(deleteNode.size() > 0){
				
				for(int deletei = 0; deletei < deleteNode.size(); deletei ++){//expand node	line6,7
			
					pair<int, int> nodelabel = deleteNode[deletei];//nodelabel先=<0,1>
					if(nodelabel.second == 1){//扩展节点
						vector<int> children; 
						expand(priv[nodelabel.first], children, number1, id);//line6, 7，扩展这个节点
						priv.insert(priv.end(), children.begin(), children.end());//在最后插入children的所有元素
					}
				}
			
				for(int deletei = 0; deletei < deleteNode.size(); deletei ++){//delete the expanded nodes and the nodes consuming all users删除priv中的内容
					pair<int, int> nodelabel = deleteNode[deletei];
					priv.erase(priv.begin() + (nodelabel.first - deletei));
				}
	
			}
		}

		return computeTruePST(id, number1, ep, k);//filter the candidates
	}
	

	
	void expand(int idx, vector<int>& next, int startRecord, vector<int>& id) {//当前节点加入所有字符子节点

		if (nodes1[idx]->next.empty()) {//这个节点的子节点是空的，未被扩展
			vector<T*> children;
			split_PFS3(idx, children, startRecord, id);//扩展了孩子节点
			for(int x = 0; x < children.size(); x++){
				nodes1[idx]->next.push_back(nodes1.size());//这一层节点的next加入1，2，3，4····27（如第一层，类似于编号）
				next.push_back(nodes1.size());
				nodes1.push_back(children[x]);//nodesl加入这些子节点
			}
		}
		nodes1[idx]->leaf = false;//这一层leaf属性变为false
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

