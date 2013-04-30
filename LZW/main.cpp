#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "lzw.h"

using namespace std;

void printUsage(char *exec)
{
    cout << "Usage : " << exec << " [-d] inputfile [outputfile]" << endl;
    exit(0);
}

int main(int argc, char *argv[])
{
    // check for minimum args
    if(argc < 2) printUsage(argv[0]);

    // parse args
    vector<string> args(argv, argv + argc);

    // compression/decompression mode
    bool isdec = false;
    if(args[1] == "-d")
    {
        // decompression
        isdec = true;
        if(argc < 3) printUsage(argv[0]);
    }

    // output file name
    string outputname = args[1+isdec];
    if(isdec) outputname+=".dec"; else outputname+=".lzw";
    if(argc > 3) outputname = args[2+isdec];

    // open file streams
    ifstream inputfile(args[1+isdec].c_str(), ios::in  | ios::binary);
    ofstream outputfile(outputname.c_str(),   ios::out | ios::binary);

    // LZW
    LZW lzw;
    if(isdec) lzw.decompress(inputfile, outputfile);
    else      lzw.compress  (inputfile, outputfile);

    // close files
    inputfile.close();
    outputfile.close();

    return 0;
}

