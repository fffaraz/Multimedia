#ifndef _BITOPS_H
#define _BITOPS_H

//
// Author: Owen Astrachan
//         Based on code written by Steve Tate
//
// facilitates reading/writing in a "bits-at-a-time" fashion
// 
// ported to C++ on 3/27/94
// this version standard C++, 8/23/98

#include <fstream>
#include <string>

using namespace std;


extern int kthbit(int,int);

// operations on obstream
//
//  obstream()
//         constructs output bit-stream (in unusable form)
//         use name in constructor or use open method to bind to a file
//    
//  obstream(const string &)
//         constructs output bit-stream bound to file specified by parameter
//    
//  bool  open(const string &)
//         sets output bit-stream to file specified by parameter
//         (previously open bit-stream closed)
//         returns true if open successful, else false
//
// void writebits(int numbits, int value)
//         writes rightmost # bits specified by first parameter
//         taken from the second parameter
//         (bits may be "buffered" for the next write)
//
//  void flushbits()
//         writes any "left-over" (buffered) bits
//
//  void close()
//         closes stream properly (flushes too).




// operations on ibstream
//
// ibstream()
//         constructs input bit-stream that is not usable
//         use name in constructor or open method to bind to a file
//
// ibstream(const string &)
//         construct input bit-stream bound to specified file
//
// bool open(const string &)
//         bind input bit-stream to file specified
//         (previously open bit-stream closed)
//         returns true if open successful, else false
//
// bool readbits(int numbits,int & value)
//         reads number of bits specified by parameter and returns via
//         ref parameter value these bits (stored in an int)
//         returns true if numbits were read, otherwise returns false
//         EXCEPTION: 
//                    maximum number of bits that can be read is 32
//
//  void resetbits()
//         effectively flushes bits buffered during read operations
//         so that subsequent reads start "from scratch"
//
//  void rewind()
//         reset input bit-stream to beginning, clear it
//
//  void close()
//         closes stream properly (flushes too).
//
// stand-alone function
//    
//   int kthbit(intx, int k)
//          returns the kth bit of the int x: rightmost bit is bit #1
//          EXCEPTION: error if k < 1 or k > 32
//
// specification: bits are read/written in the "same" order
// i.e., if 15 bits are written, then these bits will be read
// in the same order as written if 15 1-bit reads are issued.


class obstream 
{
  public:
     obstream();                      // default constructor
     obstream(const string &);        // bind to file specified by param
     ~obstream();

     bool open(const string &);        // set bit-stream to file
     void writebits(int,int);          // value and # of bits to write
     void flushbits();                 // flush bit-stream
     void close();                     // wrap things up
	 
  private:
    ofstream * myOutStream;           // stream for writing bits
    int myOutBuff;                    // buffer bits for output
    int myBitsToGo;                   // # of 'left-over' bits for output	 
};


class ibstream
{
  public:
     ibstream();                      // default constructor
     ibstream(const string &);        // bind to file specified by param
     virtual ~ibstream();

     bool open(const string &);       // set bit-stream to specified file
     bool readbits(int, int &);       // # of bits to read
     void resetbits();                // reset bits
     void rewind();                   // rewind to beginning of input
     void close();                    // wrap things up nicely
  private:
	 
     ifstream * myInputStream;        // stream for reading bits
     int myInBuff;                    // buffer bits for input
     int myInbbits;                   // used for buffering	 
};


#endif
