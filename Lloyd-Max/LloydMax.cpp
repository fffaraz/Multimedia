#include "LloydMax.h"

LloydMax::LloydMax(int m, PDF *pdf, double minv, double maxv)
{
	this->m = m;
	this->pdf = pdf;
	total = 0;

	// a[1], a[2], ..., a[m]
	a = new double[m+1];

	// b[0], b[1], ..., b[m]
	b = new double[m+1];

	for (int i = 0; i <= m; i++)
		a[i] = b[i] = 0;

	b[0] = minv;
	b[m] = maxv;

}

int LloydMax::doIteration(int count)
{
	for (int i = 0; i < count; i++)
	{
		updateA();
		updateB();
	}
	total += count;
	return total;
}

int LloydMax::doIteration()
{
	int counter = 0;
	bool ans = true;

	while (counter < 100000 && ans)
	{
		ans = false;
		ans |= updateA();
		ans |= updateB();
		counter++;
	}

	total += counter;
	return total;
}

bool LloydMax::updateA()
{
	// a[1], ... , a[m]
	bool ans = false;
	for (int i = 1; i <= m; i++)
		ans |= updateA(i);
	return ans;
}

bool LloydMax::updateB()
{
	// b[1], ... , b[m-1]
	bool ans = false;
	for (int i = 1; i < m; i++)
		ans |= updateB(i);
	return ans;
}

bool LloydMax::updateA(int i)
{
	// b[i-1] ... b[i]
	double sum1=0, sum2=0;
	double delta = 0.001;
	for (double d = b[i-1]; d < b[i]; d+=delta)
	{
		double val;

		val   = pdf->f(d) * d;
		val  *= delta;
		sum1 += val;

		val   = pdf->f(d);
		val  *= delta;
		sum2 += val;
	}

	double na;
	if(sum2 == 0 ) 
		na = 0;
	else
		na = sum1/sum2;
	
	bool ans = (na != a[i]);
	a[i] = na;
	return ans;
}

bool LloydMax::updateB(int i)
{
	double nb = (a[i]+a[i+1])/2.0;
	bool ans = (nb != b[i]);
	b[i] = nb;
	return ans;
}

double LloydMax::getA(int i)
{
	return a[i];
}

double LloydMax::getB(int i)
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
