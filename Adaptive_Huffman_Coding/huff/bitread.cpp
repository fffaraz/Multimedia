#include <iostream>
#include <string>
using namespace std;

#include "prompt.h"
#include "bitops.h"

// demonstrates some use of bitstream operations

void echo(ibstream & input)
// post: echoes input 1 bit-at-a-time to file whose name
//       is entered by the user    
{
   string name = PromptString("enter name of output file: ");
   obstream output;
   int inbits;

   output.open(name);

   input.rewind();
   while (input.readbits(1, inbits))
   {
      output.writebits(1, inbits);
   }

   output.close();
}


int main(int argc, char * argv[])
{
    string filename;

    
   if (argc > 1)
   {
       filename = argv[1];
   }
   else
   {
       filename = PromptString("enter filename: ");
   }
   
   ibstream infile(filename.c_str());
   echo(infile);
   cout << "one more time around" << endl;
   echo(infile);
   
   return 0;
}
