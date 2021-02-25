#include "prefix.h"

//
prefixnode::prefixnode(set<int>& idx1, int depth1, deque<int> path1, int parent1, int startRecord1) {//初始化节点
	this->idx = idx1;//id编号
	this->depth = depth1;//0
	this->path = path1;//root（-1）
	this->score = 0;
	this->leaf = true;
	this->startRecord = startRecord1;//0
	this->parent = parent1;//-1
	this->npart = 0;
	this->truehis = 0;

	this->avalue = 0;
	this->zvalue = 0;
	this->cvalue = 0;

	this->rvalue = 0;
	this->svalue = 0;
	this->rootsum = 0;
}
//sub[i], f->depth + 1, newpath, idx, startRecord
//idx=0，children，startRecord=number1=用户数*0.8，id即打乱后的用户编号





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
			orgraw[i][j].push_back(26);//在每个字符串的最后加上26，代表&
		}
	}
}







prefix::~prefix() {
}



void prefix::initialRoot(vector<int>& id){//初始化根节点
	this->raw.clear();
	raw = orgraw;
	
	for(int i =0; i < nodes1.size(); i ++){
		delete nodes1[i];
	}
	nodes1.clear();//清空nodes1
	set<int> idx1;//用户编号
	for(int i = 0; i < this->raw.size(); i ++){//插入用户编号
		idx1.insert(i);
	}
	deque<int> root; root.push_back(-1);//双端队列，先添加-1
	prefixnode* x = new prefixnode(idx1, 0, root, -1, 0);
	nodes1.push_back(x);// the root node*/把根节点加入到nodes1中

}


bool compare_vector(pair<int, double> a, pair<int, double> b){
	return a.second > b.second;
}





void prefix::computePST3_delta(int startNode, int endNode, double ep, vector<int>& id, vector<int>& priv, int c, vector<pair<int, int>>& deleteNode, int endn, int groupi){//计算具体的频数
	//假设nodelabel是节点的开始编号，也可能是priv中存放，先不管
	map<int, int> preMap;//<节点编号，前缀编号>
	vector<int> count; //<频数>
	int pre0;
	int d = 1;//计算前缀数量
	for (int i = 0;i < groupi;i++)
		d *= 26;
	for (int i = 0; i < priv.size(); i++) {//计算新节点所对应的前缀编号，0-d
		int pre = 0;//前缀编号
		for (deque<int>::iterator itr = this->nodes1[priv[i]]->path.begin() + 1; itr != this->nodes1[priv[i]]->path.end(); itr++)
			pre = pre * 26 + *itr;
		if (!i) pre0 = pre;
		preMap.insert(make_pair(priv[i], pre-pre0));
		count.push_back(0);
	}
	for (int i = startNode; i < endNode;i++) {
		vector<int> rawPrefix;//存储前缀编号
		vector<int> noisyone;
		for (int j = 0;j < raw[id[i]].size();j++) {//计算此用户的前缀编号
			int pre=0;
			for (int k = 0;k < groupi; k++) {
				if (k >= raw[id[i]][j].size())	break;
				pre = pre * 26 + raw[id[i]][j][k];
			}
			rawPrefix.push_back(pre - pre0);
		}
		Noisy_wheel(ep / 2,rawPrefix, noisyone,d,rawPrefix.size());
		for(int j=0;j<noisyone.size();j++)
			if (noisyone[j]) {
				int m = preMap.find(j + pre0)->second;
				count[m]++;
			}
	}
	/*for(int i = startNode; i < endNode; i ++){	//从这组用户的第一个用户开始，到这组用户的最后一个用户为止
		
		int idx = 0;//字符，从0到字符的最大编号
		vector<int> seq = this->raw[id[i]];	//seq为当前用户的字符串
		for(int seqi = 0; seqi < seq.size(); seqi ++){
			if(nodes1[idx]->leaf == false)//
				idx = nodes1[idx]-> next[seq[seqi]];//idx=该字符串的第i个字符（数字）
			else break;
		}//总之就是遇到子节点，就返回

		if(count.find(idx) != count.end()){//如果count中能找到idx所指的字符，
			count.find(idx)->second ++;//count，第一个数idx所对应的第二个数就+1（频数）
		}
	}*/

	
	int number = endNode - startNode;//这一组的用户数量

	/*vector<double> countvector;
	for(map<int, int>::iterator itr = count.begin(); itr != count.end(); itr ++)//countvector中存储count的第二个数，所有用户中含priv的个数
		countvector.push_back(itr->second);
		*/
	//vector<double> noisyone;
	//noise::NoisyVec_delta(eng, number, countvector, noisyone, ep/(2));//加噪，结果放在noisyone中
	

	map<int, double> noisycount; int i = 0; 
	for(map<int, int>::iterator itr = preMap.begin(); itr != preMap.end(); itr ++, i ++){//itr迭代preMap
		noisycount.insert(make_pair(itr->first, count[i]));//noisycount中存储priv[i]<->加噪后的频数
	}
	
	for(int i = 0; i < priv.size(); i ++){
		int nodelabel = priv[i];//nodelabel是节点编号
		prefixnode* node = this->nodes1[nodelabel];//当前节点是nodel的这个字符所指的节点
		
		node->score += noisycount.find(nodelabel)->second;//频数是这个字符加噪后的频数
		node->npart += number;//npart是这一组的用户数量

		if(! expandable(priv[i])){//如果它是&，则不可再扩大节点，进入下一次循环（跳出）
			continue;
		}
		if(node->score >  c * sqrt(2.0*(double)node->npart )/ep || endNode == endn){//如果这个节点的频数大于某个值或此为最后一个用户（即为最后一层）
			
			if(node->score > c* sqrt(2.0*(double)node->npart )/ep && endNode != endn){//频数超过阈值，且不是最后一层
				deleteNode.push_back(make_pair(i, 1));//deleteNode就存储一个<i,1>
				node->leaf = false;//leaf改为false，还可以再往下找
			}
			else{
				if(endNode == endn){
					deleteNode.push_back(make_pair(i, -1));
					node->leaf = true;//不扩展
				}
			
			}
		}

	}
}








vector<pair<vector<int>, double>> prefix::computeTruePST(vector<int>& id, int number, double ep, int k) {

	vector<vector<int>> orgresult;  vector<int> hitnode;

	for (int i = 0; i < this->nodes1.size(); i++) {
		prefixnode* node = this->nodes1[i];

		if (node->path.back() == fanout - 1 && node->trueflag != -1) {//每一个&开始选

			vector<int> oneresult; //一个结果
			oneresult.insert(oneresult.end(), node->path.begin() + 1, node->path.end());//oneresult中插入从开始到结束的所有字符
			orgresult.push_back(oneresult);//orgresult中加入oneresult
			hitnode.push_back(i);//hitnode
		}
		//orgresult存储了所有以&结尾的高频字符串（仅超过阈值），hitnode为&所在节点编号
	}

	vector<double> frequency; frequency.resize(orgresult.size(), 0);//frequency大小为orgresult的大小



	set<int> checkset;

	for (int j = 0; j < hitnode.size(); j++) {	//对于每一个收录的字符串
		for (set<int>::iterator itr = this->nodes1[hitnode[j]]->idx.begin(); itr != this->nodes1[hitnode[j]]->idx.end(); itr++) {//遍历&节点对应的所有用户
			int idx = id[*itr];
			if (raw[idx].size() == orgresult[j].size())//如果这个用户的字符串的大小与节点计算的对应字符串的大小相等，频数+1
				frequency[j] += 1;
		}
	}
	//frequency中存储了用户字符串与计算字符串大小相同的频数

	vector<double> noisyone;
	noise::NoisyVec_delta(eng, this->raw.size() - number, frequency, noisyone, ep / (2));//加噪函数，noiseone为frequency加噪后的结果



	//consistency enforcement  猜测是根据一定的计算，即原先的频数是分组计算所得，对该频数扩大一定的倍数，使得频数与实际频数尽量相符，
	computeweightavg(hitnode, noisyone, this->raw.size(), this->raw.size() - number);//加工量noisyone
	boost(hitnode, noisyone, this->raw.size(), number);

	vector<pair<int, double>> result;
	for (int i = 0; i < noisyone.size(); i++) {
		result.push_back(make_pair(i, noisyone[i]));


	}//把noisyone编号加入result中
	sort(result.begin(), result.end(), compare_vector);//对result进行排序，也就是按照频数高低给编号排序

	vector<pair<vector<int>, double>> lastresult;

	for (int i = 0; i < k && i < result.size(); i++) {
		lastresult.push_back(make_pair(orgresult[result[i].first], result[i].second));

	}
	//根据排好的编号，选出前k个字符串及其频数加入到lastresult中
	return lastresult;
}


//computeweightavg(hitnode, noisyone, this->raw.size(), number, frequency);
void prefix::computeweightavg(vector<int>& hitnode, vector<double>& frequency, int number, int usenumber){//对noisyone进行了加工
	double variance = 1.0/usenumber;
	for(int i = 0; i < frequency.size(); i ++){
		
		frequency[i] = frequency[i] * number/usenumber;
		double freq_node = this->nodes1[hitnode[i]]->score * number/this->nodes1[hitnode[i]]->npart;
	
		double variance_node = 1.0/this->nodes1[hitnode[i]]->npart;
		frequency[i] = variance/(variance + variance_node) * freq_node + variance_node/(variance + variance_node)* frequency[i];
	}

}



void prefix::split_PFS3(int idx, vector<prefixnode*>& children, int startRecord, vector<int>& id) {//插入26个孩子节点，每个字符一个节点，节点中包括了这一层拥有这个字符的用户编号
	//idx=0，children，startRecord=number1=用户数*0.8，id即打乱后的用户编号
	prefixnode* f = nodes1[idx];//f=第idx个节点，编号为idx的节点
	vector<set<int>> sub;

	for(int i = 0; i < fanout; i ++){//在sub中放入26个整数集合
		set<int> x;
		sub.push_back(x);
	}

	for(set<int>::iterator itr =f->idx.begin(); itr != f->idx.end(); itr ++){//遍历此节点所属的id编号
		
		if(*itr < startRecord) continue;//id编号要大于起始编号，才开始操作

		int loc = id[*itr];//定位这个id编号
		
		if(this->raw[loc].size() < f->depth + 1) continue;//如果raw的大小达不到这个深度，则没有考虑的必要
		else{
			
			sub[this->raw[loc][f->depth]].insert(*itr);	//否则 这个用户的这一层字母所对应的集合增加这个id编号
			
		}
		
	}								
	//也就是说，sub中是这一层有哪些字符对应了哪些用户
	
	for (int i = 0; i < fanout; i++) {
		deque<int> newpath = f->path;//
		newpath.push_back(i);					//加入所有的字符								
		prefixnode* mm = new prefixnode(sub[i], f->depth + 1, newpath, idx, startRecord);//生成新节点
		children.push_back(mm);//children中加入了26个节点
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









