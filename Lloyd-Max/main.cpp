
#include <iostream>
using namespace std;

#include "PDF.h"
#include "LloydMax.h"

class MyPDF : public PDF
{
public:
	double f(double v)
	{
		if(v>=-1 && v<=1)
			return 0.5;
		else
			return 0;
	}
};

int main(int argc, char *argv[])
{
	LloydMax lm(8, new MyPDF, -1, 1, 100, 0.000001);
	//lm.init(false);
	lm.init(true);

	for (int i = 0; i < 1; i++)
	{
		int iterations = 0;
		iterations = lm.doIteration();
		cout << "Iterations : " << iterations << endl << endl;

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

