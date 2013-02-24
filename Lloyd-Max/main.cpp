

#include <iostream>
using namespace std;

#include "PDF.h"
#include "LloydMax.h"

class MyPDF : public PDF
{
public:
	double f(double v)
	{
		if(v>-5 && v<5)
			return 0.1;
		else
			return 0;
	}
};

int main(int argc, char *argv[])
{
	LloydMax lm(256, new MyPDF, -5, 5);

	for (int i = 0; i < 1; i++)
	{
		cout << "Iterations : " << lm.doIteration()  << endl << endl;

		for(int i=1; i <= lm.getM(); i++)
		{
			cout << "a[" << i << "] = " << lm.getA(i) << endl;
		}

		cout<<endl;

		for(int i=0; i <= lm.getM(); i++)
		{
			//cout << "b[" << i << "] = " << lm.getB(i) << endl;
		}
		cout << endl << "----------" << endl << endl;
	}



	cin.get();
	return 0;
}