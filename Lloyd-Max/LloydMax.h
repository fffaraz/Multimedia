#pragma once

#include "PDF.h"

class LloydMax
{
public:
	LloydMax(int m, PDF *pdf, double minv, double maxv);
	~LloydMax(void);
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

	bool updateA();
	bool updateA(int i);
	bool updateB();
	bool updateB(int i);
};

