/* Copyright (c) 2013 the authors listed at the following URL, and/or
the authors of referenced articles or incorporated external code:
http://en.literateprograms.org/Huffman_coding_(C_Plus_Plus)?action=history&offset=20090129100015

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Retrieved from: http://en.literateprograms.org/Huffman_coding_(C_Plus_Plus)?oldid=16057
*/

#include "huffman.h"
#include <map>
#include <iostream>
#include <ostream>
#include <algorithm>
#include <iterator>
#include <string>

std::ostream& operator<<(std::ostream& os, std::vector<bool> vec)
{
  std::copy(vec.begin(), vec.end(), std::ostream_iterator<bool>(os, ""));
  return os;
}

int main()
{
  std::map<char, double> frequencies;
  frequencies['e'] = 0.124167;
  frequencies['a'] = 0.0820011;
  frequencies['t'] = 0.0969225;
  frequencies['i'] = 0.0768052;
  frequencies['n'] = 0.0764055;
  frequencies['o'] = 0.0714095;
  frequencies['s'] = 0.0706768;
  frequencies['r'] = 0.0668132;
  frequencies['l'] = 0.0448308;
  frequencies['d'] = 0.0363709;
  frequencies['h'] = 0.0350386;
  frequencies['c'] = 0.0344391;
  frequencies['u'] = 0.028777;
  frequencies['m'] = 0.0281775;
  frequencies['f'] = 0.0235145;
  frequencies['p'] = 0.0203171;
  frequencies['y'] = 0.0189182;
  frequencies['g'] = 0.0181188;
  frequencies['w'] = 0.0135225;
  frequencies['v'] = 0.0124567;
  frequencies['b'] = 0.0106581;
  frequencies['k'] = 0.00393019;
  frequencies['x'] = 0.00219824;
  frequencies['j'] = 0.0019984;
  frequencies['q'] = 0.0009325;
  frequencies['z'] = 0.000599;
  Hufftree<char, double> hufftree(frequencies.begin(), frequencies.end());
  for (char ch = 'a'; ch <= 'z'; ++ch)
  {
    std::cout << ch << ": " << hufftree.encode(ch) << "\n";
  }
  std::string source = "literateprogramming";
  std::cout << source << "\n";

  std::vector<bool> encoded = hufftree.encode(source.begin(), source.end());
  std::cout << encoded << "\n";

  std::string destination;
  hufftree.decode(encoded, std::back_inserter(destination));
  std::cout << destination << "\n";
}
