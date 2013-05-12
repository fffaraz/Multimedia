#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#include "tpq.h"


/**
 * illustrates how to use tpqueue class
 * with defined Comparator object and pointers
 * this just sorts strings
 *
 * author: Owen Astrachan
 */

struct Thing
{
    string info;
    Thing (const string& s)
	: info(s)
    {
	    
    }
    // other fields could come here
};


/**
 * This class is used to compare Thing pointers.
 * A class is needed because comparing pointers by themselves
 * will use the memory address to compare when what's needed
 * is what the pointers point to for comparing
 *
 */

class ThingCompare : public Comparer<Thing *>
{
   public:

    ThingCompare()
    { }
    
    typedef Thing * Thingptr;   // use Thingptr as alias for Thing *
    
    int compare(const Thingptr & lhs, const Thingptr & rhs) const
    // post:	returns -1 if lhs < rhs, +1 if lhs > rhs, 0 if lhs == rhs
    {
	if (lhs->info < rhs->info)      return -1;
	else if (lhs->info > rhs->info) return +1;
	return 0;   // Thing/strings are equal
    }
};

int main(int argc, char * argv[])
{
    if (argc <= 1)
    {
	cerr << "usage " << argv[0] << " filename" << endl;
	exit(1);
    }

    string filename = argv[1];
    ifstream input(filename.c_str());
    string str;
    ThingCompare comp;          // compares Things in pq
    
    tpqueue<Thing *> pq(comp);
    
    while (input >> str)
    {
	pq.insert(new Thing(str));
    }

    // now print in sorted order
    
    Thing * t;
    while (pq.size() > 0)
    {
	pq.deletemin(t);
	cout << t->info << endl;
    }
    
    return 0;
}
