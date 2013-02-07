#pragma once

#include "PDF.h"

class LloydMax
{
public:
	LloydMax(int m, PDF *pdf);
	~LloydMax(void);
	void doIteration(int count);
	float getA(int i);
	float getB(int i);
	int   getM();

private:
	int m;
	PDF *pdf;
	float *a;
	float *b;

	void updateA();
	void updateA(int i);
	void updateB();
	void updateB(int i);
};

