#include "noise.h"
#include "include/Python.h"
noise::noise() {
}

noise::~noise() {
}


// gaussian
double noise::nextGaussian(engine& eng, double std) {
	normal_distribution<double> normal(0.0, std);
	return normal(eng);
}


// uniform
double noise::nextDouble(engine& eng, double range) {
	uniform_real_distribution<double> unif(0.0, range);
	return unif(eng);
}

double noise::nextDouble(engine& eng, double left, double right) {
	uniform_real_distribution<double> unif(left, right);
	return unif(eng);
}

int noise::nextInt(engine& eng, int range) {		// right-exclusive
	uniform_int_distribution<int> unif(0, range - 1);
	return unif(eng);
}

int noise::nextSign(engine& eng) {
	return 2 * nextInt(eng) - 1;
}


// exponential
double noise::nextExponential(engine& eng, double lambda) {
	exponential_distribution<double> expo(lambda);
	return expo(eng);
}


// laplace
double noise::nextLaplace(engine& eng, double scale) {
	return nextExponential(eng, 1.0 / scale) - nextExponential(eng, 1.0 / scale);
}





int noise::nextDiscreteLaplace(engine& eng, double scale) {
	double alpha = exp(-1.0 / scale);
	if (nextDouble(eng) < (1 - alpha) / (1 + alpha)) return 0;
	else return nextSign(eng) * (nextGeometric(eng, 1 - alpha) + 1);
}


// geometric
int noise::nextGeometric(engine& eng, double p) {
	geometric_distribution<int> geometric(p);
	return geometric(eng);
}


// cauchy
double noise::nextCauchy(engine& eng, double scale) {
	cauchy_distribution<double> cauchy(0.0, scale);
	return cauchy(eng);
}






// exponential mechanism
int noise::EM(engine& eng, const vector<double>& quality, double ep, double sens) {
	ep = ep / sens;
	double maxq = *max_element(quality.begin(), quality.end());		// compute maxq for the precision issue: when quality scores for ALL outputs are extremely small, exp(q) is ALWAYS zero

	vector<double> weights;
	//for (const double& q : quality)
		//weights.push_back(exp(ep / 2 * (q - maxq)));				// this adjustment will not affect the differences among scores & probabilities after normalization
	
	for(int i = 0; i < quality.size(); i ++){
		double q = quality[i];
		weights.push_back(exp(ep / 2 * (q - maxq)));				// this adjustment will not affect the differences among scores & probabilities after normalization
	}
	
	return sample(eng, weights);
}


void noise::NoisyVec(engine& eng, double sum, vector<int>& hist, int d, int maxlength, double eps, vector<double>& noisyhis){// NoisyVec function 

	double value1 = exp(eps)/(exp(eps) - 1); double value0 = (1 - value1);
	value1 = maxlength * value1; value0 = maxlength * value0;
	
	//double value1 = exp(eps)/(exp(eps) - 1); double value0 = (- value1);
	//value1 = maxlength * value1; value0 = maxlength * value0;


	for(int i = 0; i <= d; i ++){
		double p = nextDouble(eng, 1);
		if(i < d){
			double samplep = ((double)hist[i]/maxlength *(exp(eps) - 1) + 1)/(exp(eps) + 1);
			if(p < samplep){//==1
				noisyhis.push_back(value1);
			}
			else  noisyhis.push_back(value0);
		}
		else{
			double samplep = ((double)sum/maxlength *(exp(eps) - 1) + 1)/(exp(eps) + 1);
			if(p < samplep){//==1
				noisyhis.push_back(value1);
			}
			else  noisyhis.push_back(value0);
		}

	}
	/*for(int i = 0; i <= d; i ++){
		double p = nextDouble(eng, 1);
		if(i < d){
			//double samplep = ((double)hist[i]/maxlength *(exp(eps) - 1) + 1)/(exp(eps) + 1);
			double samplep = ((double)hist[i]/maxlength *(exp(eps) - 1) + exp(eps))/(2.0*exp(eps));
			if(p < samplep){//==1
				noisyhis.push_back(value1);
			}
			else  noisyhis.push_back(value0);
		}
		else{
			//double samplep = ((double)sum/maxlength *(exp(eps) - 1) + 1)/(exp(eps) + 1);
			double samplep = ((double)sum/maxlength *(exp(eps) - 1) + exp(eps))/(2.0*exp(eps));
			if(p < samplep){//==1
				noisyhis.push_back(value1);
			}
			else  noisyhis.push_back(value0);
		}

	}*/

}



void noise::NoisyVec_delta(engine& eng, int number, vector<double>& count, vector<double>& noisyone, double eps){// NoisyVec function 

	double value1 = exp(eps)/(exp(eps) - 1); double value0 = (1 - value1);

	double p1 =  ((double)1 *(exp(eps) - 1) + 1)/(exp(eps) + 1); 
	double p0 = ((double)0 *(exp(eps) - 1) + 1)/(exp(eps) + 1); 
	//cout<<"p1: "<<p1<<" "<<p0<<endl;
	
	for(int i = 0; i < count.size(); i ++){
		int number1 = count[i];
		int number0 = number - number1;

		
		/*double value = 0;
		for(int j = 0; j < count[i]; j ++){
			double p = nextDouble(eng, 1);
			double samplep = p1;
			if(p < samplep){//==1
				value += value1;
			}
			else  value += value0;
		}

		for(int j = 0; j < number0; j ++){
			double p = nextDouble(eng, 1);
			double samplep = p0;
			if(p < samplep){//==1
				value += value1;
			}
			else  value += value0;
		}

		noisyone.push_back(value);*/
		
		int k;
		binomial_distribution<int> distribution(number1, p1);
	
		
		if(p1 == 1) k = number1;
		else k = distribution(eng);
		double x1 = k * value1 + (number1 - k) * value0;
		
		binomial_distribution<int> distribution0(number0, p0);
		if(p0 == 0) k = 0;
		else k = distribution0(eng);
		
		double x2 = k * value1 + (number0 - k) * value0;
		noisyone.push_back(x1 + x2);
		
	}
	//cout<<endl;
}




void noise::NoisyVec_delta_kdd(engine& eng, int number, vector<map<double, int>>& count, vector<double>& noisyone, double eps){// NoisyVec function 

	

	double value1 = (exp(eps)+1)/(exp(eps) - 1); double value0 = - (exp(eps)+1)/(exp(eps) - 1);
	double value1x = (exp(10.0)+1)/(exp(10.0) - 1); double value0x = - (exp(10.0)+1)/(exp(10.0) - 1);
	 
	
	double eeps = exp(eps);
	for(int i = 0; i < count.size(); i ++){
		map<double, int> bitvalue = count[i];
		double x = 0; int remain0 = 0;
		for(map<double, int>::iterator itr = bitvalue.begin(); itr != bitvalue.end(); itr ++){
			double key = itr->first; int value = itr->second;
			double p1 = (key * (eeps - 1) + eeps + 1)/(2*eeps + 2);
			int k;
			binomial_distribution<int> distribution(value, p1);
			k = distribution(eng);
			
			x += k * value1 + (value - k) * value0;
			//x += key * value;
			remain0 += value;
		}
		//cout<<endl;
		double p1 = (0 * (eeps - 1) + eeps + 1)/(2*eeps + 2);
		binomial_distribution<int> distribution(number - remain0, p1);
		int k = distribution(eng);
		x += k * value1x + (number -remain0 - k)*value0x;
		cout<<"k: "<<k<<" "<<number-remain0<<" "<<k * value1x + (number -remain0 - k)*value0x;

		noisyone.push_back(x);

	}
	cout<<endl;
}




double noise::NoisyVec_delta(engine& eng, int number, int count, double eps){// NoisyVec function 

	double value1 = exp(eps)/(exp(eps) - 1); double value0 = (1 - value1);

	double p1 =  ((double)1 *(exp(eps) - 1) + 1)/(exp(eps) + 1); 
	double p0 = ((double)0 *(exp(eps) - 1) + 1)/(exp(eps) + 1); 
	
	
	int number1 = count;
	int number0 = number - number1;
		
		
	int k;
	binomial_distribution<int> distribution(number1, p1);
		
	if(p1 == 1) k = number1;
	else k = distribution(eng);
	double x1 = k * value1 + (number1 - k) * value0;

	binomial_distribution<int> distribution0(number0, p0);
	if(p0 == 0) k = 0;
	else k = distribution0(eng);
	double x2 = k * value1 + (number0 - k) * value0;
	return (x1 + x2);

	
}





void noise::NoisyVec_new(engine& eng, double sum, vector<int>& hist, int d, int maxlength, double eps, vector<double>& noisyhis){// NoisyVec function 

	
	double value1 = exp(eps)/(exp(eps) - 1); double value0 = (- value1);
	value1 = maxlength * value1; value0 = maxlength * value0;

	for(int i = 0; i <= d; i ++){
		double p = nextDouble(eng, 1);
		if(i < d){
			double samplep = ((double)hist[i]/maxlength *(exp(eps) - 1) + exp(eps))/(2.0*exp(eps));
			if(p < samplep){//==1
				noisyhis.push_back(value1);
			}
			else  noisyhis.push_back(value0);
		}
		else{
			double samplep = ((double)sum/maxlength *(exp(eps) - 1) + exp(eps))/(2.0*exp(eps));
			if(p < samplep){//==1
				noisyhis.push_back(value1);
			}
			else  noisyhis.push_back(value0);
		}

	}

}



void noise::NoisyVecZero(engine& eng, double sum, int d, int maxlength, int times, double eps, vector<double>& noisyhis){// NoisyVec function 

	double p = 1.0/(exp(eps) + 1);
	double value1 = exp(eps)/(exp(eps) - 1); double value0 = (1 - value1);
	//double value1 = exp(eps)/(exp(eps) - 1); double value0 = (- value1);
	value1 = maxlength * value1; value0 = maxlength * value0;





	binomial_distribution<int> distribution(times, p);
	

	for(int i = 0; i <= d; i ++){
		int k = distribution(eng);
		noisyhis.push_back(k * value1 + (times - k) * value0);
	}
	/*noisyhis.resize(d+1, 0);
	for(int i =0; i < times; i ++){
		vector<int> his; his.resize(d, 0); vector<double> noisyhisx; 
		NoisyVec(eng, 0, his, d, maxlength, eps, noisyhisx);
		for(int j = 0; j < d+1;j ++){
			noisyhis[j] += noisyhisx[j];
		}
	}*/

}



void noise::NoisyVecZero_new(engine& eng, double sum, int d, int maxlength, int times, double eps, vector<double>& noisyhis){// NoisyVec function 
	
	double p = 0.5;
	binomial_distribution<int> distribution(times, p);
	
	double value1 = exp(eps)/(exp(eps) - 1); double value0 = (- value1);
	value1 = maxlength * value1; value0 = maxlength * value0;

	for(int i = 0; i <= d; i ++){
		int k = distribution(eng);
		noisyhis.push_back(k * value1 + (times - k) * value0);
	}
}


void noise::Noisy_wheel(double eps, vector<int>& count, vector<int>& noisycount, int d, int m) {
	//初始化Python环境  
	Py_Initialize();
	// 检查初始化是否成功  
	if (!Py_IsInitialized()) {
		cout << "Initialized" << endl;
		exit(0);
	}
	PyRun_SimpleString("import sys");
	//添加Insert模块路径  
	//PyRun_SimpleString(chdir_cmd.c_str());
	PyRun_SimpleString("sys.path.append('./')");
	PyObject * pModule, * pDict, * pFunc, * pArgs, *pClass, * pInstance;
	//导入模块  
	pModule = PyImport_ImportModule("wheel");
	//验证模块是否导入成功
	if (!pModule)
	{
		cout << "Python get module failed." << endl;
		exit(0);
	}
	cout << "Python get module succeed." << endl;
	//完成了py文件的导入

	//导入方法字典（我也不知道是个什么玩意，先导着吧）
	pDict = PyModule_GetDict(pModule);
	//通过字典属性获取模块中的类
	pClass = PyDict_GetItemString(pDict, "Wheel");
	//实例化获取的类
	PyObject* pConstruct = PyInstanceMethod_New(pClass);
	//参数设置参数这里要再考虑一下
	pArgs = Py_BuildValue("(i,i,d)", d,m,eps);
	PyObject* pInstance = PyObject_CallObject(pConstruct, pArgs);//构造函数要传参
	//调用函数
	PyObject* tuple = PyTuple_New(count.size());
	for (int i = 0;i < count.size();i++) 
		PyTuple_SET_ITEM(tuple, i, Py_BuildValue("i", count[i]));
	PyObject* pArg = Py_BuildValue("O", tuple);
	PyObject* result = PyObject_CallMethod(pInstance, "randomizer", "O",pArg);
	noisycount = tupletoVector_Int(result,count.size());
}
vector<int> tupletoVector_Int(PyObject* object,int size) {
	
}



// random sampling
int noise::sample(engine& eng, const vector<double>& weights) {
	double cum = 0.0;
	vector<double> cdf;
	//for (const double& w : weights) {
	for(int i = 0; i < weights.size(); i ++){
		double w = weights[i];
		cum += max(w, 0.0);											// in case of noise
		cdf.push_back(cum);
	}
	if (cum == 0.0) return nextInt(eng, weights.size());			// no positive weight -> uniform sampling
	return lower_bound(cdf.begin(), cdf.end(), nextDouble(eng, cum)) - cdf.begin();
}

