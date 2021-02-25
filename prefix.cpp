#include "prefix.h"

//
prefixnode::prefixnode(set<int>& idx1, int depth1, deque<int> path1, int parent1, int startRecord1) {//��ʼ���ڵ�
	this->idx = idx1;//id���
	this->depth = depth1;//0
	this->path = path1;//root��-1��
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
//idx=0��children��startRecord=number1=�û���*0.8��id�����Һ���û����





prefixnode::~prefixnode() {
}

prefix::prefix(engine& eng1) : tree(eng1) {
}




int prefix::translate(string s) {	//��ÿ���ַ����
	if (s2i.find(s) == s2i.end()) {		//���û�ҵ�
		int tag = s2i.size();		//tag=Ŀǰ������
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
	while (getline(ss, item, ':')){		//һ��ð�ż���ַ�������һ���ַ���һ���ַ����ض���

		stringstream interss(item);
		string interitem;
		vector<int> tokens;
		while(getline(interss, interitem, ' ')){		
			tokens.push_back(translate(interitem));//�ַ�����ÿ���ַ������
		}
		result.push_back(tokens);
	}
	return result;//����ÿ���û���Ž�����n���ַ�
}



prefix::prefix(engine& eng, string filename, vector<int>& id_backup): tree(eng){
	
	ifstream tfile(filename);
	string s;
	
	int i = 0;
	while (getline(tfile, s)){		//һ��һ�У�һ���û�һ���û�������
		
		this->orgraw.push_back(tokenize(s));								//symbol system: -1=>$ start symbol; [0, fanout-2] normal symbols; fanout-1=>& end symbol  
		id_backup.push_back(i);
		i ++;
	}//orgraw��ά����������
	
	tfile.close();

	
	fanout = 27;

	for(int i = 0; i < orgraw.size(); i ++){		
		
		for(int j = 0; j < orgraw[i].size(); j ++){
			orgraw[i][j].push_back(26);//��ÿ���ַ�����������26������&
		}
	}
}







prefix::~prefix() {
}



void prefix::initialRoot(vector<int>& id){//��ʼ�����ڵ�
	this->raw.clear();
	raw = orgraw;
	
	for(int i =0; i < nodes1.size(); i ++){
		delete nodes1[i];
	}
	nodes1.clear();//���nodes1
	set<int> idx1;//�û����
	for(int i = 0; i < this->raw.size(); i ++){//�����û����
		idx1.insert(i);
	}
	deque<int> root; root.push_back(-1);//˫�˶��У������-1
	prefixnode* x = new prefixnode(idx1, 0, root, -1, 0);
	nodes1.push_back(x);// the root node*/�Ѹ��ڵ���뵽nodes1��

}


bool compare_vector(pair<int, double> a, pair<int, double> b){
	return a.second > b.second;
}





void prefix::computePST3_delta(int startNode, int endNode, double ep, vector<int>& id, vector<int>& priv, int c, vector<pair<int, int>>& deleteNode, int endn, int groupi){//��������Ƶ��
	//����nodelabel�ǽڵ�Ŀ�ʼ��ţ�Ҳ������priv�д�ţ��Ȳ���
	map<int, int> preMap;//<�ڵ��ţ�ǰ׺���>
	vector<int> count; //<Ƶ��>
	int pre0;
	int d = 1;//����ǰ׺����
	for (int i = 0;i < groupi;i++)
		d *= 26;
	for (int i = 0; i < priv.size(); i++) {//�����½ڵ�����Ӧ��ǰ׺��ţ�0-d
		int pre = 0;//ǰ׺���
		for (deque<int>::iterator itr = this->nodes1[priv[i]]->path.begin() + 1; itr != this->nodes1[priv[i]]->path.end(); itr++)
			pre = pre * 26 + *itr;
		if (!i) pre0 = pre;
		preMap.insert(make_pair(priv[i], pre-pre0));
		count.push_back(0);
	}
	for (int i = startNode; i < endNode;i++) {
		vector<int> rawPrefix;//�洢ǰ׺���
		vector<int> noisyone;
		for (int j = 0;j < raw[id[i]].size();j++) {//������û���ǰ׺���
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
	/*for(int i = startNode; i < endNode; i ++){	//�������û��ĵ�һ���û���ʼ���������û������һ���û�Ϊֹ
		
		int idx = 0;//�ַ�����0���ַ��������
		vector<int> seq = this->raw[id[i]];	//seqΪ��ǰ�û����ַ���
		for(int seqi = 0; seqi < seq.size(); seqi ++){
			if(nodes1[idx]->leaf == false)//
				idx = nodes1[idx]-> next[seq[seqi]];//idx=���ַ����ĵ�i���ַ������֣�
			else break;
		}//��֮���������ӽڵ㣬�ͷ���

		if(count.find(idx) != count.end()){//���count�����ҵ�idx��ָ���ַ���
			count.find(idx)->second ++;//count����һ����idx����Ӧ�ĵڶ�������+1��Ƶ����
		}
	}*/

	
	int number = endNode - startNode;//��һ����û�����

	/*vector<double> countvector;
	for(map<int, int>::iterator itr = count.begin(); itr != count.end(); itr ++)//countvector�д洢count�ĵڶ������������û��к�priv�ĸ���
		countvector.push_back(itr->second);
		*/
	//vector<double> noisyone;
	//noise::NoisyVec_delta(eng, number, countvector, noisyone, ep/(2));//���룬�������noisyone��
	

	map<int, double> noisycount; int i = 0; 
	for(map<int, int>::iterator itr = preMap.begin(); itr != preMap.end(); itr ++, i ++){//itr����preMap
		noisycount.insert(make_pair(itr->first, count[i]));//noisycount�д洢priv[i]<->������Ƶ��
	}
	
	for(int i = 0; i < priv.size(); i ++){
		int nodelabel = priv[i];//nodelabel�ǽڵ���
		prefixnode* node = this->nodes1[nodelabel];//��ǰ�ڵ���nodel������ַ���ָ�Ľڵ�
		
		node->score += noisycount.find(nodelabel)->second;//Ƶ��������ַ�������Ƶ��
		node->npart += number;//npart����һ����û�����

		if(! expandable(priv[i])){//�������&���򲻿�������ڵ㣬������һ��ѭ����������
			continue;
		}
		if(node->score >  c * sqrt(2.0*(double)node->npart )/ep || endNode == endn){//�������ڵ��Ƶ������ĳ��ֵ���Ϊ���һ���û�����Ϊ���һ�㣩
			
			if(node->score > c* sqrt(2.0*(double)node->npart )/ep && endNode != endn){//Ƶ��������ֵ���Ҳ������һ��
				deleteNode.push_back(make_pair(i, 1));//deleteNode�ʹ洢һ��<i,1>
				node->leaf = false;//leaf��Ϊfalse����������������
			}
			else{
				if(endNode == endn){
					deleteNode.push_back(make_pair(i, -1));
					node->leaf = true;//����չ
				}
			
			}
		}

	}
}








vector<pair<vector<int>, double>> prefix::computeTruePST(vector<int>& id, int number, double ep, int k) {

	vector<vector<int>> orgresult;  vector<int> hitnode;

	for (int i = 0; i < this->nodes1.size(); i++) {
		prefixnode* node = this->nodes1[i];

		if (node->path.back() == fanout - 1 && node->trueflag != -1) {//ÿһ��&��ʼѡ

			vector<int> oneresult; //һ�����
			oneresult.insert(oneresult.end(), node->path.begin() + 1, node->path.end());//oneresult�в���ӿ�ʼ�������������ַ�
			orgresult.push_back(oneresult);//orgresult�м���oneresult
			hitnode.push_back(i);//hitnode
		}
		//orgresult�洢��������&��β�ĸ�Ƶ�ַ�������������ֵ����hitnodeΪ&���ڽڵ���
	}

	vector<double> frequency; frequency.resize(orgresult.size(), 0);//frequency��СΪorgresult�Ĵ�С



	set<int> checkset;

	for (int j = 0; j < hitnode.size(); j++) {	//����ÿһ����¼���ַ���
		for (set<int>::iterator itr = this->nodes1[hitnode[j]]->idx.begin(); itr != this->nodes1[hitnode[j]]->idx.end(); itr++) {//����&�ڵ��Ӧ�������û�
			int idx = id[*itr];
			if (raw[idx].size() == orgresult[j].size())//�������û����ַ����Ĵ�С��ڵ����Ķ�Ӧ�ַ����Ĵ�С��ȣ�Ƶ��+1
				frequency[j] += 1;
		}
	}
	//frequency�д洢���û��ַ���������ַ�����С��ͬ��Ƶ��

	vector<double> noisyone;
	noise::NoisyVec_delta(eng, this->raw.size() - number, frequency, noisyone, ep / (2));//���뺯����noiseoneΪfrequency�����Ľ��



	//consistency enforcement  �²��Ǹ���һ���ļ��㣬��ԭ�ȵ�Ƶ���Ƿ���������ã��Ը�Ƶ������һ���ı�����ʹ��Ƶ����ʵ��Ƶ�����������
	computeweightavg(hitnode, noisyone, this->raw.size(), this->raw.size() - number);//�ӹ���noisyone
	boost(hitnode, noisyone, this->raw.size(), number);

	vector<pair<int, double>> result;
	for (int i = 0; i < noisyone.size(); i++) {
		result.push_back(make_pair(i, noisyone[i]));


	}//��noisyone��ż���result��
	sort(result.begin(), result.end(), compare_vector);//��result��������Ҳ���ǰ���Ƶ���ߵ͸��������

	vector<pair<vector<int>, double>> lastresult;

	for (int i = 0; i < k && i < result.size(); i++) {
		lastresult.push_back(make_pair(orgresult[result[i].first], result[i].second));

	}
	//�����źõı�ţ�ѡ��ǰk���ַ�������Ƶ�����뵽lastresult��
	return lastresult;
}


//computeweightavg(hitnode, noisyone, this->raw.size(), number, frequency);
void prefix::computeweightavg(vector<int>& hitnode, vector<double>& frequency, int number, int usenumber){//��noisyone�����˼ӹ�
	double variance = 1.0/usenumber;
	for(int i = 0; i < frequency.size(); i ++){
		
		frequency[i] = frequency[i] * number/usenumber;
		double freq_node = this->nodes1[hitnode[i]]->score * number/this->nodes1[hitnode[i]]->npart;
	
		double variance_node = 1.0/this->nodes1[hitnode[i]]->npart;
		frequency[i] = variance/(variance + variance_node) * freq_node + variance_node/(variance + variance_node)* frequency[i];
	}

}



void prefix::split_PFS3(int idx, vector<prefixnode*>& children, int startRecord, vector<int>& id) {//����26�����ӽڵ㣬ÿ���ַ�һ���ڵ㣬�ڵ��а�������һ��ӵ������ַ����û����
	//idx=0��children��startRecord=number1=�û���*0.8��id�����Һ���û����
	prefixnode* f = nodes1[idx];//f=��idx���ڵ㣬���Ϊidx�Ľڵ�
	vector<set<int>> sub;

	for(int i = 0; i < fanout; i ++){//��sub�з���26����������
		set<int> x;
		sub.push_back(x);
	}

	for(set<int>::iterator itr =f->idx.begin(); itr != f->idx.end(); itr ++){//�����˽ڵ�������id���
		
		if(*itr < startRecord) continue;//id���Ҫ������ʼ��ţ��ſ�ʼ����

		int loc = id[*itr];//��λ���id���
		
		if(this->raw[loc].size() < f->depth + 1) continue;//���raw�Ĵ�С�ﲻ�������ȣ���û�п��ǵı�Ҫ
		else{
			
			sub[this->raw[loc][f->depth]].insert(*itr);	//���� ����û�����һ����ĸ����Ӧ�ļ����������id���
			
		}
		
	}								
	//Ҳ����˵��sub������һ������Щ�ַ���Ӧ����Щ�û�
	
	for (int i = 0; i < fanout; i++) {
		deque<int> newpath = f->path;//
		newpath.push_back(i);					//�������е��ַ�								
		prefixnode* mm = new prefixnode(sub[i], f->depth + 1, newpath, idx, startRecord);//�����½ڵ�
		children.push_back(mm);//children�м�����26���ڵ�
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









