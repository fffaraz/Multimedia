
#include <cmath>
#include <iostream>
using namespace std;

#include "PDF.h"
#include "LloydMax.h"

class MyPDF : public PDF
{
public:
	double f(double v)
	{
		double lambda = 0.2;

		if(v>=-100 && v<=100)
			return lambda/2*exp(-lambda*abs(v));
		else
			return 0;
	}
};

int main(int argc, char *argv[])
{

	double minv = -100;
	double maxv = +100;

	PDF* pdf = new MyPDF;

	LloydMax lm(3, pdf, minv, maxv, 100, 0.000001); // zero initialisation

	lm.init(false); // uniform initialisation
	//lm.init(true); // random initialisation

	for (int i = 0; i < 1; i++)
	{
		int iterations = 0;

		iterations = lm.doIteration();
		//iterations = lm.doIteration(1);

		cout << "Iterations : " << iterations << endl << endl;

		for(int i=1; i <= lm.getM(); i++)
		{
			cout << "a[" << i << "] = " << lm.getA(i) << endl;
		}

		cout<<endl;

		for(int i=0; i <= lm.getM(); i++)
		{
			cout << "b[" << i << "] = " << lm.getB(i) << endl;
		}

		cout << endl << "----------" << endl << endl;
	}

	std::cin.ignore(); 
	std::cin.get();
	return 0;
}

