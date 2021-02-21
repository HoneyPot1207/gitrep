#pragma once
#include<vector>
#include<fstream>
#include<algorithm>
#include<iostream>
#include<list>
using namespace std;
vector<pair<vector<int>, double>> PST3(double ep, vector<int>& id, double ratio, int k) {
	int number1 = 18;//预定义l，单词的最大长度
	int userNumber = size();//用户数量
	int group = number1+1;
	int step = group / numberl;
	for (int groupi = 0; groupi < group && priv.size() > 0; ) {//split the dataset into group to imprive the time efficiency		Line2

		int startNode = groupi * step;//开始节点为第n组用户
		int endNode = (groupi + 1) * step;//结束节点为第n+1组用户
		if (groupi == group - 1) endNode = number1;//最后一组不够，则结束节点为最后一个

		vector<pair<int, int>> deleteNode;	//删除节点

		if (*priv.begin() > 0) {
			computePST3_delta(startNode, endNode, ep, id, priv, eta, deleteNode, number1);//compute the contributions to frequencies made by users in this group.This function returns the nodes to be expanded，计算节点的频数Line12
			groupi++;//下一层
		}
		else {
			deleteNode.push_back(make_pair(0, 1));
			endNode = 0;
		}

		if (deleteNode.size() > 0) {

			for (int deletei = 0; deletei < deleteNode.size(); deletei++) {//expand node	line6,7

				pair<int, int> nodelabel = deleteNode[deletei];
				if (nodelabel.second == 1) {
					vector<int> children;
					expand(priv[nodelabel.first], children, number1, id);//line6, 7
					priv.insert(priv.end(), children.begin(), children.end());
				}
			}

			for (int deletei = 0; deletei < deleteNode.size(); deletei++) {//delete the expanded nodes and the nodes consuming all users
				pair<int, int> nodelabel = deleteNode[deletei];
				priv.erase(priv.begin() + (nodelabel.first - deletei));
			}

		}
	}

}