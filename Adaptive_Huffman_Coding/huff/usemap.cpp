#include <iostream>
#include <string>
using namespace std;



#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#include "tmap.h"            // base class tmap
#include "uvmap.h"           // unsorted vector implementation UVMap
#include "prompt.h"

int main(int argc, char * argv[])
{
    string filename,word;
    if (argc > 1)
    {
	filename = argv[1];
    }
    else
    {
	filename = PromptString("filename: ");
    }
    ifstream input(filename.c_str());

    // make an unsorted vector map and store all words/counts int it
    
    tmap<string,int> * map = new UVMap<string,int>();
    
    while (input >> word)
    {
	if (map->contains(word))
	{
	    map->get(word) += 1;
	}
	else
	{
	    map->insert(word,1);
	}
    }

    Iterator<pair<string,int> > * it = map->makeIterator();
    for(it->Init(); it->HasMore(); it->Next())
    {
	cout << it->Current().second << "\t" << it->Current().first << endl;
    }
	
    return 0;
    
}

