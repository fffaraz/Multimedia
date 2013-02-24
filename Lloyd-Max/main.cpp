
#include <iostream>
#include <fstream>
#include <ctime>
#include <random>
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
	ofstream msefile("mse.txt");

	double minv = -1;
	double maxv = +1;

	PDF* pdf = new MyPDF;

	LloydMax lm(8, pdf, minv, maxv, 100, 0.000001); // zero initialisation

	//lm.init(false); // uniform initialisation
	lm.init(true); // random initialisation

	for (int i = 0; i < 20; i++)
	{
		int iterations = 0;

		//iterations = lm.doIteration();
		iterations = lm.doIteration(1);

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

		cout<<endl;

		double mse = lm.calcMSE();
		cout << "MSE : " << mse << endl;
		msefile << mse << endl;

		cout << endl << "----------" << endl << endl;
	}

	msefile.flush();
	msefile.close();

	// Random Number Generator
	typedef std::mt19937 MyRNG;  // the Mersenne Twister with a popular choice of parameters
	MyRNG rng;
	rng.seed(time(NULL));
	std::uniform_real_distribution<double> real_dist(minv, maxv);

	double randvals[1000];
	for (int i = 0; i < 1000; i++)
		randvals[i] = real_dist(rng);

	// calc mse
	for (int j = 1; j < 8; j++)
	{
		double mse=0;
		for (int i = 0; i < 1000; i++)
		{
			if(randvals[i] > lm.getB(j) && randvals[i] < lm.getB(j+1))
			{
				double quantized = lm.quantize(randvals[i]);
				mse += (quantized - randvals[i]) * (quantized - randvals[i]) * pdf->f(randvals[i]);
			}
		}
		cout << "j : " << j << endl;
		cout << "delta^2/12 = " << lm.getB(j+1) - lm.getB(j) << endl;
		cout << "MSE        = " << mse << endl;
	}


	std::cin.ignore(); 
	std::cin.get();
	return 0;
}

