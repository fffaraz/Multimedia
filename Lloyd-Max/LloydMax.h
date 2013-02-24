#pragma once

#include <ctime>
#include <random>
#include "PDF.h"

class LloydMax
{
public:
	LloydMax(int m, PDF *pdf, double minv, double maxv, int maxiter=1000, double minmse=0.0001);
	~LloydMax(void);
	void init(bool rand=false);
	int doIteration(int count);
	int doIteration();
	double getA(int i);
	double getB(int i);
	int    getM();

private:
	int m;
	PDF *pdf;
	double *a;
	double *b;
	int total;
	int maxiter;
	double minmse;

	bool updateA();
	bool updateA(int i);
	bool updateB();
	bool updateB(int i);
};

