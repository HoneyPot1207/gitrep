#pragma once
#include<vector>
#include<fstream>
#include<algorithm>
#include<iostream>
#include<list>
using namespace std;
vector<pair<vector<int>, double>> PST3(double ep, vector<int>& id, double ratio, int k) {
	int number1 = 18;//Ԥ����l�����ʵ���󳤶�
	int userNumber = size();//�û�����
	int group = number1+1;
	int step = group / numberl;
	for (int groupi = 0; groupi < group && priv.size() > 0; ) {//split the dataset into group to imprive the time efficiency		Line2

		int startNode = groupi * step;//��ʼ�ڵ�Ϊ��n���û�
		int endNode = (groupi + 1) * step;//�����ڵ�Ϊ��n+1���û�
		if (groupi == group - 1) endNode = number1;//���һ�鲻����������ڵ�Ϊ���һ��

		vector<pair<int, int>> deleteNode;	//ɾ���ڵ�

		if (*priv.begin() > 0) {
			computePST3_delta(startNode, endNode, ep, id, priv, eta, deleteNode, number1);//compute the contributions to frequencies made by users in this group.This function returns the nodes to be expanded������ڵ��Ƶ��Line12
			groupi++;//��һ��
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