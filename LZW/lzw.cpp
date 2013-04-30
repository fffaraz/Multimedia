#include "lzw.h"

LZW::LZW()
{
}

int LZW::compress(istream &is, ostream &os)
{
    // init dictionary
    map<string, uint16> dic;
    uint16 newCode = 0;
    initDictC(dic, newCode);

    // compression
    string s;
    char c;
    while((c = is.get()))
    {
        if(is.eof()) break;
        cout << "Get " << c << endl;

        if(dic.find(s+c) != dic.end()) // if s + c exists in the dictionary
        {
            cout << "Found " << s+c << endl;
            s = s + c;
        }
        else
        {
            // output the code for s
            map<string, uint16>::iterator iter = dic.find(s);
            uint16 code = iter->second;
            //os.write((char*)&code, 2);
            os.put(code);
            cout << "Write code for " << s << " : " << code << endl;

            // add string s + c to the dictionary with a new code
            dic[s+c] = newCode++;
            cout << "Add " << s+c << " to dictionary with code " << newCode-1 << endl;

            // reset dic
            //if(newCode == 0) initDictC(dic, newCode);

            s = c;
        }
    }

    // output the code for s
    map<string, uint16>::iterator iter = dic.find(s);
    uint16 code = iter->second;
    //os.write((char*)&code, 2);
    os.put(code);
    cout << "Write code for " << s << " : " << code << endl;

    return 0;
}

int LZW::decompress(istream &is, ostream &os)
{
    // init dictionary
    map<uint16, string> dic;
    uint16 newCode = 0;
    initDictD(dic, newCode);

    // decompression
    string s;
    uint16 k;
    while((k = is.get()))
    {
        if(is.eof()) break;

        // entry = dictionary entry for k
        string entry = dic[k];

        // output entry
        os.write(entry.c_str(), entry.length());
        cout << "Write " << entry << " Decoded with " << k << endl;

        // add string s + entry[0] to dictionary with a new code
        if(s.length() != 0)
        {
            dic[newCode++] = s + entry[0];
            cout << "Add " << s + entry[0] << " to dictionary with code " << newCode-1 << endl;
        }

        s = entry;
    }

    return 0;
}

void LZW::initDictC(map<string, uint16> &dic, uint16 &newCode)
{
    dic.empty();
    newCode = 1;
    for(char c='A'; c<'D'; c++)
    {
        string str;
        str = (char)c;
        dic[str] = newCode;
        newCode++;
    }
}

void LZW::initDictD(map<uint16, string> &dic, uint16 &newCode)
{
    dic.empty();
    newCode = 1;
    for(char c='A'; c<'D'; c++)
    {
        string str;
        str = (char)c;
        dic[newCode] = str;
        newCode++;
    }
}
