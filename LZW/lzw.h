#ifndef LZW_H
#define LZW_H

#include <iostream>
#include <vector>
#include <map>

using namespace std;
typedef unsigned short int uint16;

class LZW
{
public:
    LZW();
    int compress  (istream &is, ostream &os);
    int decompress(istream &is, ostream &os);

private:
    void initDictC(map<string, uint16> &dic, uint16 &newCode);
    void initDictD(map<uint16, string> &dic, uint16 &newCode);
};

#endif // LZW_H
