#include "prefix.h"

//
prefixnode::prefixnode(set<int>& idx1, int depth1, deque<int> path1, int parent1, int startRecord1) {//初始化节点
	this->idx = idx1;
	this->depth = depth1;
	this->path = path1;
	this->score = 0;
	this->leaf = true;
	this->startRecord = startRecord1;
	this->parent = parent1;
	this->npart = 0;
	this->truehis = 0;

	this->avalue = 0;
	this->zvalue = 0;
	this->cvalue = 0;

	this->rvalue = 0;
	this->svalue = 0;
	this->rootsum = 0;
}
//





prefixnode::~prefixnode() {
}

prefix::prefix(engine& eng1) : tree(eng1) {
}




int prefix::translate(string s) {	//把每个字符编号
	if (s2i.find(s) == s2i.end()) {		//如果没找到
		int tag = s2i.size();		//tag=目前最大的数
		s2i[s] = tag;
		i2s[tag] = s;
	}
	return s2i[s];
}

string prefix::translate(int i) {
	return i2s[i];
}

vector<vector<int>> prefix::tokenize(string s) {						//need test
	
	stringstream ss(s);
	string item;
	vector<vector<int>> result;
	while (getline(ss, item, ':')){		//一个冒号间隔字符串，即一个字符串一个字符串地读入

		stringstream interss(item);
		string interitem;
		vector<int> tokens;
		while(getline(interss, interitem, ' ')){		
			tokens.push_back(translate(interitem));//字符串中每个字符都编号
		}
		result.push_back(tokens);
	}
	return result;//返回每个用户编号结束的n个字符
}



prefix::prefix(engine& eng, string filename, vector<int>& id_backup): tree(eng){
	
	ifstream tfile(filename);
	string s;
	
	int i = 0;
	while (getline(tfile, s)){		//一行一行（一个用户一个用户）读入
		
		this->orgraw.push_back(tokenize(s));								//symbol system: -1=>$ start symbol; [0, fanout-2] normal symbols; fanout-1=>& end symbol  
		id_backup.push_back(i);
		i ++;
	}//orgraw三维数组存放数据
	
	tfile.close();

	
	fanout = 27;

	for(int i = 0; i < orgraw.size(); i ++){		
		
		for(int j = 0; j < orgraw[i].size(); j ++){
			orgraw[i][j].push_back(26);
		}
	}
}







prefix::~prefix() {
}



void prefix::initialRoot(vector<int>& id){//初始化根节点
	this->raw.clear();
	for(int i = 0; i < orgraw.size(); i ++){//把orgraw中的内容随机选取一个加到raw中，使得每个用户只有一个字符串
		if(orgraw[i].size() > 1){//如果用户有多个字符串
			int loc = noise::nextInt(eng, orgraw[i].size());//每个用户的n个字符串中随机选取一个
			this->raw.push_back(orgraw[i][loc]);//向raw中添加该字符串

		}
		else{//如果用户只有一个字符串
			this->raw.push_back(orgraw[i].at(0));//向raw中添加该字符串
		}
	}
	
	
	for(int i =0; i < nodes1.size(); i ++){
		delete nodes1[i];
	}
	nodes1.clear();
	set<int> idx1;
	for(int i = 0; i < this->raw.size(); i ++){
		idx1.insert(i);
	}
	deque<int> root; root.push_back(-1);
	prefixnode* x = new prefixnode(idx1, 0, root, -1, 0);
	nodes1.push_back(x);// the root node*/

}


bool compare_vector(pair<int, double> a, pair<int, double> b){
	return a.second > b.second;
}





void prefix::computePST3_delta(int startNode, int endNode, double ep, vector<int>& id, vector<int>& priv, int c, vector<pair<int, int>>& deleteNode, int endn){//计算具体的频数

	map<int, int> count; 
	for(int i = 0; i < priv.size(); i ++)
		count.insert(make_pair(priv[i], 0));

	for(int i = startNode; i < endNode; i ++){
		
		int idx = 0;
		vector<int> seq = this->raw[id[i]];
		for(int seqi = 0; seqi < seq.size(); seqi ++){
			if(nodes1[idx]->leaf == false)
				idx = nodes1[idx]->next[seq[seqi]];
			else break;
		}

		if(count.find(idx) != count.end()){
			count.find(idx)->second ++;
		}
	}

	
	int number = endNode - startNode;

	vector<double> countvector;
	for(map<int, int>::iterator itr = count.begin(); itr != count.end(); itr ++)
		countvector.push_back(itr->second);

	vector<double> noisyone;
	noise::NoisyVec_delta(eng, number, countvector, noisyone, ep/(2));
	

	map<int, double> noisycount; int i = 0; 
	for(map<int, int>::iterator itr = count.begin(); itr != count.end(); itr ++, i ++){
		noisycount.insert(make_pair(itr->first, noisyone[i]));
	}
	
	for(int i = 0; i < priv.size(); i ++){
		int nodelabel = priv[i];
		prefixnode* node = this->nodes1[nodelabel];
		
		node->score += noisycount.find(nodelabel)->second;
		node->npart += number;

		if(! expandable(priv[i])){
			continue;
		}
		if(node->score >  c * sqrt(2.0*(double)node->npart )/ep || endNode == endn){
			
			if(node->score > c* sqrt(2.0*(double)node->npart )/ep && endNode != endn){
				deleteNode.push_back(make_pair(i, 1));
				node->leaf = false;
			}
			else{
				if(endNode == endn){
					deleteNode.push_back(make_pair(i, -1));
					node->leaf = true;
				}
			
			}
		}

	}
}








vector<pair<vector<int>, double>> prefix::computeTruePST(vector<int>& id, int number, double ep, int k){

	vector<vector<int>> orgresult;  vector<int> hitnode;

	for(int i = 0; i < this->nodes1.size(); i ++){
		prefixnode* node = this->nodes1[i];
	
		if(node->path.back() == fanout - 1 && node->trueflag != -1){
			
			vector<int> oneresult; 
			oneresult.insert(oneresult.end(), node->path.begin() + 1, node->path.end());
			orgresult.push_back(oneresult);
			hitnode.push_back(i);
		}
		
	}
	
	vector<double> frequency; frequency.resize(orgresult.size(), 0);

	

	set<int> checkset;

	for(int j = 0; j < hitnode.size(); j ++){	
		for(set<int>::iterator itr = this->nodes1[hitnode[j]]->idx.begin(); itr != this->nodes1[hitnode[j]]->idx.end(); itr ++){
			int idx = id[*itr];
			if(raw[idx].size() == orgresult[j].size())
				frequency[j] += 1;
		}
	}


	vector<double> noisyone;
	noise::NoisyVec_delta(eng, this->raw.size() - number, frequency, noisyone, ep/(2));

	
	//consistency enforcement
	computeweightavg(hitnode, noisyone, this->raw.size(), this->raw.size()- number);
	boost(hitnode, noisyone, this->raw.size(), number);

	vector<pair<int, double>> result;
	for(int i = 0; i < noisyone.size(); i ++){
		result.push_back(make_pair(i, noisyone[i]));


	}
	sort(result.begin(), result.end(), compare_vector);

	vector<pair<vector<int>, double>> lastresult;

	for(int i = 0; i < k && i < result.size(); i ++){
		lastresult.push_back(make_pair(orgresult[result[i].first], result[i].second));

	}

	return lastresult;
}


//computeweightavg(hitnode, noisyone, this->raw.size(), number, frequency);
void prefix::computeweightavg(vector<int>& hitnode, vector<double>& frequency, int number, int usenumber){
	double variance = 1.0/usenumber;
	for(int i = 0; i < frequency.size(); i ++){
		
		frequency[i] = frequency[i] * number/usenumber;
		double freq_node = this->nodes1[hitnode[i]]->score * number/this->nodes1[hitnode[i]]->npart;
	
		double variance_node = 1.0/this->nodes1[hitnode[i]]->npart;
		frequency[i] = variance/(variance + variance_node) * freq_node + variance_node/(variance + variance_node)* frequency[i];
	}

}



void prefix::split_PFS3(int idx, vector<prefixnode*>& children, int startRecord, vector<int>& id) {

	prefixnode* f = nodes1[idx];
	vector<set<int>> sub;

	for(int i = 0; i < fanout; i ++){
		set<int> x;
		sub.push_back(x);
	}

	for(set<int>::iterator itr =f->idx.begin(); itr != f->idx.end(); itr ++){
		
		if(*itr < startRecord) continue;

		int loc = id[*itr];
		
		if(this->raw[loc].size() < f->depth + 1) continue;
		else{
			
			sub[this->raw[loc][f->depth]].insert(*itr);	
			
		}
		
	}								
	
	
	for (int i = 0; i < fanout; i++) {
		deque<int> newpath = f->path;
		newpath.push_back(i);													
		prefixnode* mm = new prefixnode(sub[i], f->depth + 1, newpath, idx, startRecord);
		children.push_back(mm);
	}
}






int prefix::size(){
	return this->raw.size();
}


bool prefix::expandable(int idx) {
	if(this->nodes1[idx]->path.back() == fanout - 1){return false;}
	else return true;
}







void prefix::boost(vector<int>& hitnode, vector<double>& noisyone, int number, int usenumber){

	map<int, vector<int>> PSTlevel;

	for(int i = 0; i < this->nodes1.size(); i ++){
		this->nodes1[i]->score = this->nodes1[i]->score * number/this->nodes1[i]->npart;
		this->nodes1[i]->npart = this->nodes1[i]->npart;
	}

	for(int i = 0; i < hitnode.size(); i ++){
		this->nodes1[hitnode[i]]->score = noisyone[i];
		this->nodes1[hitnode[i]]->npart = this->nodes1[hitnode[i]]->npart + usenumber;
	}


	for(int i = 0; i < this->nodes1.size(); i ++){
		int depth = nodes1[i]->depth;
		if(PSTlevel.find(depth) == PSTlevel.end()){
			vector<int> temp; temp.push_back(i);
			PSTlevel.insert(make_pair(depth, temp));
		}
		else{
			PSTlevel.find(depth)->second.push_back(i);
		}
		

		if(this->nodes1[i]->next.empty()){
			this->nodes1[i]->avalue = 0;
			int currentnode = i;
			while(currentnode != 0){

				this->nodes1[i]->zvalue += this->nodes1[currentnode]->npart * this->nodes1[currentnode]->score;
				currentnode = this->nodes1[currentnode]->parent;
			}
			
			this->nodes1[i]->rvalue = 1.0/this->nodes1[i]->npart;
			this->nodes1[i]->svalue = this->nodes1[i]->rvalue;
		}
	}

	int bottom = PSTlevel.size() - 1;

	for(int i = bottom ; i > 0; i --){
		
		vector<int> currentNodes = PSTlevel.find(i)->second;
		for(int nodej = 0; nodej < currentNodes.size(); nodej ++){
			prefixnode* node = this->nodes1[currentNodes[nodej]];
			
			if(node->next.empty()) continue;

			double sumS = 0;
			double sumZ = 0;
			for(int nexti = 0; nexti < node->next.size(); nexti ++){
				prefixnode* childnode = this->nodes1[node->next[nexti]];
				sumS += childnode->svalue;
				sumZ += childnode->rvalue * childnode->zvalue;
			}
			node->rvalue = 1.0/(sumS * node->npart + 1);
			node->svalue = node->rvalue * sumS;
			node->zvalue = sumZ;
		}
	}

	for(int i = 1; i <= bottom; i ++){
		vector<int> currentNodes = PSTlevel.find(i)->second;
			
		for(int nodej = 0; nodej < currentNodes.size(); nodej ++){
			prefixnode* node = this->nodes1[currentNodes[nodej]];

			if(node->parent == 0){
				node->cvalue = node->rvalue * node->zvalue;
			}
			else{
				node->cvalue = node->rvalue * node->zvalue - node->svalue * node->rootsum;
			}
			
			if(! node->next.empty()){
				for(int nexti = 0; nexti < fanout; nexti ++){
					this->nodes1[node->next[nexti]]->rootsum += node->rootsum +node->cvalue * node->npart;
				}
			}
		}
	}


	vector<double> noisyoneback = noisyone;
	noisyone.clear();
	for(int i = 0; i < hitnode.size(); i ++){
		int idx = hitnode[i];
		noisyone.push_back(this->nodes1[idx]->cvalue);
	}

}









