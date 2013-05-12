#include <iostream>
#include <string>
using namespace std;

int main(int argc, char * argv[])
{
    if (argc == 1)
    {
	cout << "program " << argv[0]
	     << " has no command-line args" << endl;
    }
    else
    {
	int k;
	string arg;
	cout << argv[0] << " has " << argc << " command-line args:" << endl;

	for(k=1; k < argc; k++)
	{
	    arg = argv[k];    // always use string to access argv[k]
	    
	    cout << k << "-th arg is: " << arg << endl;
	}
    }
}
