#include "LloydMax.h"


LloydMax::LloydMax(int m, PDF *pdf)
{
	this->m = m;
	this->pdf = pdf;
	a = new float[m+1];
	b = new float[m+1];
	for (int i = 0; i < m+1; i++)
	{
		a[i] = 0;
		b[i] = 0;
	}
}

void LloydMax::doIteration(int count)
{
	for (int i = 0; i < count; i++)
	{
		updateA();
		updateB();
	}
}

void LloydMax::updateA()
{
	for (int i = 0; i < m; i++)
	{
		updateA(i);
	}
}

void LloydMax::updateA(int i)
{
	// TODO
}

void LloydMax::updateB()
{
	for (int i = 0; i < m; i++)
	{
		updateB(i);
	}
}

void LloydMax::updateB(int i)
{
	b[i] = (a[i]+a[i+1])/2.0;
}

float LloydMax::getA(int i)
{
	return a[i];
}

float LloydMax::getB(int i)
{
	return b[i];
}

int LloydMax::getM()
{
	return m;
}


LloydMax::~LloydMax(void)
{
	delete[] a;
	delete[] b;
}
