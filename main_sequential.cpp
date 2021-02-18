#include<iostream>
#include<fstream>
#include<ctime>
#include<set>
using namespace std;
#include<time.h>
#include"prefix.h"
#include"noise.h"

//

template <typename T>
double mean_relative_error(vector<double>& a, vector<T>& q, double smooth) {
	double err = 0.0;
	for (int i = 0; i < a.size(); i++)
		err += abs(q[i].ans - a[i]) / max(q[i].ans, smooth);			// smoothed relative error
	return err / a.size();												// mean relative error
}

double total_variation_distance(vector<double>& a, vector<double>& b) {
	double err = 0.0;
	for (int i = 0; i < a.size(); i++) err += abs(a[i] - b[i]);			
	return err / 2.0;												
}


bool compare_vector(pair<vector<int>, double>& a, pair<vector<int>, double>& b){
	return a.second > b.second;
}


double topk_precision(vector<pair<vector<int>, double>>& a, vector<vector<int>>& b, int k) {
	//if(a.size() < k) return -1;
	set<vector<int>> bset;
	for (int i = 0; i < b.size() && i < k; i ++) bset.insert(b[i]);

	int match = 0;
	for (int i = 0; i < a.size() && i < k; i ++){
		if (bset.find(a[i].first) != bset.end()) {
			//for(int x = 0; x < a[i].size(); x++) cout<<a[i][x]<<" ";
			//cout<<endl;
			match++;
		}
	}

	//cout<<(double)match/a.size()<<" "<<(double)match/b.size()<<" "<<a.size()<<" "<<b.size()<<endl;
	//return (double)match / a.size();
	//return (double)match/min((double)k, (double)a.size());

	double pre = (double)match/k; double recall = (double)match/min((double)k, (double)a.size());

	return 2*pre*recall/(pre + recall);
	 
}
//


double top_ndcg(vector<pair<vector<int>, double>>& a, map<vector<int>, double>& b, int k){
	vector<pair<vector<int>, double>> bstatistic;
	bstatistic.insert(bstatistic.end(), b.begin(), b.end());
	sort(bstatistic.begin(), bstatistic.end(), compare_vector);
	map<vector<int>, int> locmap;
	for(int i = 0; i < bstatistic.size(); i ++){
		locmap.insert(make_pair(bstatistic[i].first, i + 1));
	}
	int d = locmap.size();
	double dcg = log((double)d)/log(2.0);
	for(int i = 2; i <= k; i ++)
		dcg += (log((double)d)/log(2.0))/(log((double)i)/log(2.0));

	double idcg = 0;
	
	
	


	map<vector<int>, int>::iterator itr_find = locmap.find(a.begin()->first);
	if(itr_find != locmap.end()) idcg += log( d - fabs(itr_find->second - 1.0))/log(2.0);
	
	for(int i = 2; i <= min((double)a.size(), (double)k); i ++){
		vector<int> key = a[i-1].first;
		map<vector<int>, int>::iterator itr_find = locmap.find(key);
		if(itr_find != locmap.end()) idcg += log( d - fabs(itr_find->second - (double)i))/log((double)i);
	}

	return  idcg/dcg;
}




double topk_re_inc(vector<pair<vector<int>, double>>& a, map<vector<int>, double>& b, int k, int n) {
	
   
	vector<pair<vector<int>, double>> back;
	back.insert(back.end(), b.begin(), b.end());
	sort(back.begin(), back.end(), compare_vector);


	map<vector<int>, double> aback;
	for(int i = 0; i < min((double)a.size(),(double)k); i ++)
		aback.insert(a[i]);

	vector<pair<double, double>> result;
	for(int i = 0; i < k; i ++){
		double trues = back[i].second;
		double noisys = 0;
		if(aback.find(back[i].first) != aback.end()) noisys = aback.find(back[i].first)->second;
		result.push_back(make_pair(trues/n, noisys/n));
	}

	fstream f; f.open("freuqecy_distribution.txt", ios::app);
	f<<endl;
	f<<"true frequency: ";
	for(int i = 0; i < result.size(); i ++){
		f<<result[i].first<<" ";
	}
	f<<endl;
	f<<"noisy frequency: ";
	for(int i = 0; i < result.size(); i ++){
		f<<result[i].second<<" ";
	}
	f<<endl;
	return 0;

}



map<vector<int>, double> getAccurateResult(int ks, vector<vector<vector<int>>>& orgraw, vector<vector<int>>& result){
	
	map<vector<int>, double> statistic;
	for(vector<vector<vector<int>>>::iterator itr = orgraw.begin(); itr != orgraw.end(); itr ++){
		for(int j = 0; j < itr->size(); j ++){
			map<vector<int>, double>::iterator itr_find = statistic.find(itr->at(j));
			if(itr_find != statistic.end()){
				itr_find->second += 1.0/itr->size();
			}
			else{
				statistic.insert(make_pair(itr->at(j), 1.0/itr->size()));
			}
		}
	}
	vector<pair<vector<int>, double>> statisticVector;
	statisticVector.insert(statisticVector.end(), statistic.begin(), statistic.end());
	sort(statisticVector.begin(), statisticVector.end(), compare_vector);

	for(int j = 0; j < statisticVector.size(); j ++){
		if(result.size() < ks){
			result.push_back(statisticVector[j].first);
		}
	}
	return statistic;
}









int main(int argc, char *argv[]) {
	
	clock_t time1 = clock();

	//dataset
	string filename  =  "twitter_number.txt";		
	//string filename  = "reddit_number.txt";

	int rep = 10;// the times of running algorithm

	vector<double> epsvector; //epsilon	有四个值
	epsvector.push_back(4); 
	epsvector.push_back(3); 
	epsvector.push_back(2);
	epsvector.push_back(1);
	
	
	vector<int> kvector; //top-"k" frequent new words 
	kvector.push_back(10); kvector.push_back(20); kvector.push_back(30); kvector.push_back(40); kvector.push_back(50); 
	kvector.push_back(60); kvector.push_back(70); kvector.push_back(80); kvector.push_back(90); kvector.push_back(100);


	random_device rd;						//non-deterministic random engine  生成随机数
	engine eng(rd());						//deterministic engine with a random seed
 
	vector<int> id_backup;
	
	prefix prefix(eng, filename, id_backup);//read data 把数据读入orgraw中

	

	for(int x = 0; x < epsvector.size(); x ++){
		double epsilon = epsvector[x];

		
		fstream xxf; xxf.open("freuqecy_distribution.txt", ios::app);//"frequency_distribution.txt" stores the frequency distribution  频率分布文件
		xxf<<"epsilon= "<<epsilon<<" "<<endl;
		xxf.close();

		vector<double> temp; temp.resize(rep, 0);	//temp大小是10（重复次数），内容为0
		vector<vector<double>> lerr_fscore;		//大小为10，存储内容temp
		lerr_fscore.resize(kvector.size(), temp);
		
		

		for (int i = 0; i < rep; i++) {		
			cout<<"epsilon: "<<epsilon<<" rep: "<<i<<endl;
			vector<int> id = id_backup;
			srand((unsigned)time(NULL));
			std::random_shuffle ( id.begin(),id.end() );//shuffle data 打乱数据

			prefix.initialRoot(id);		//初始化根节点
		
			vector<vector<int>> topks;	
			map<vector<int>, double> statistic =	getAccurateResult(kvector.back(), prefix.orgraw, topks);//获取精确结果被

			vector<pair<vector<int>, double>> result = prefix.PST3(epsilon, id, 0.8, kvector.back());//privTrie


			for(int ki = 0; ki < kvector.size(); ki++){
				lerr_fscore[ki][i] = topk_precision(result, topks, kvector[ki]);	//精确度
				
			}
			topk_re_inc(result, statistic, kvector.back(), prefix.orgraw.size());

		}
		fstream outf;
		outf.open("fscore.txt", ios::app);
		outf<<"epsilon: "<<epsilon<<" "<<endl;
		outf<<"fscore: "<<endl;
		for(int ki = 0; ki < kvector.size(); ki ++){
			outf<<"k="<<kvector[ki]<<" ";
			double sumerror = 0;
			for(int repi = 0; repi < rep; repi ++){
				sumerror += lerr_fscore[ki][repi];
			}
			outf<<sumerror/rep<<endl;
		}

		outf.close();
	}
}

