#pragma once
#include <algorithm>
#include <random>
#include<map>
#include<iostream>
using namespace std;

typedef default_random_engine engine;

class noise
{
public:
	noise();
	~noise();


	// gaussian
	static double nextGaussian(engine&, double = 1.0);

	// uniform
	static double nextDouble(engine&, double = 1.0);
	static double nextDouble(engine&, double, double);
	static int nextInt(engine&, int = 2);
	static int nextSign(engine&);

	// exponential
	static double nextExponential(engine&, double = 1.0);

	// laplace
	static double nextLaplace(engine&, double = 1.0);
	static int nextDiscreteLaplace(engine&, double = 1.0);

	// geometric
	static int nextGeometric(engine&, double = 0.5);

	// cauchy
	static double nextCauchy(engine&, double = 1.0);

	// exponential mechanism
	static int EM(engine&, const vector<double>&, double, double = 1.0);

	// random sampling
	static int sample(engine&, const vector<double>&);


	static void NoisyVec(engine& eng, double sum, vector<int>& hist, int d, int maxlength, double eps, vector<double>& noisyhis);
	static void NoisyVecZero(engine& eng, double sum, int d, int maxlength, int times, double eps, vector<double>& noisyhis);

	static double NoisyVec_delta(engine& eng, int number, int count, double eps);
	static void NoisyVec_delta(engine& eng, int flag, vector<double>& count, vector<double>& noisyone, double eps);
	static void NoisyVecZero_delta(engine& eng, double sum, int d, int maxlength, int times, double eps, vector<double>& noisyhis);
	
		
	static void NoisyVec_new(engine& eng, double sum, vector<int>& hist, int d, int maxlength, double eps, vector<double>& noisyhis);
	static void NoisyVecZero_new(engine& eng, double sum, int d, int maxlength, int times, double eps, vector<double>& noisyhis);
	static void noise::NoisyVec_delta_kdd(engine& eng, int number, vector<map<double, int>>& count, vector<double>& noisyone, double eps);	
	
};

