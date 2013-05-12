// Burrows Wheeler Transform Encoder/Decoder

#ifdef LINKED
#include "xlink.h"
extern void __cdecl qsort ( void *base, unsigned num, unsigned width, int (__cdecl *comp)(const void *, const void *));
#else
#include <stdlib.h>
#include <fcntl.h>
#endif

#include <memory.h>
#include <string.h>

#ifdef unix
#define __cdecl
#else
#include <io.h>
#endif

//  these two values are stored together
//  to improve processor cache hits

typedef struct {
    unsigned prefix, offset;
} KeyPrefix;

//  link to suffix sort module

extern KeyPrefix *bwtsort(unsigned char *, unsigned);

//  these functions link bit-level I/O

void arc_put1 (int bit);
void arc_put8 (int byte);
int arc_get1 ();
int arc_get8 ();

//  define 1/2 rle zero alphabet bits

#define HUFF_bit0 256
#define HUFF_bit1 257

//  the size of the canonical Huffman alphabet

#define HUFF_size 258

//  store these values together for bwt decoding

typedef struct {
    unsigned code:8;
    unsigned cnt:24;
} Xform;

//  the canonical HuffMan table for each alphabet character

typedef struct {
    unsigned len;
    unsigned code;
} HuffTable;

HuffTable HuffCode[HUFF_size];

//  used to construct the canonical HuffCode table

struct Node {
    struct Node *left, *right;
    unsigned freq;
};

unsigned Freq[HUFF_size], ZeroCnt;  // alphabet counts
unsigned char MtfOrder[256];        // move-to-front

//    enumerate canonical coding tree depths

unsigned enumerate (unsigned *codes, struct Node *node, unsigned depth)
{
unsigned one, two;

    if( !node->right ) {    // leaf node?
        HuffCode[(int)(node->left)].len = depth;
        codes[depth]++;
        return depth;
    }

    one = enumerate (codes, node->left, depth + 1);
    two = enumerate (codes, node->right, depth + 1);

    // return the max depth of the two sub-trees

    return one > two ? one : two;
}

int __cdecl comp_node (const void *left, const void *right)
{
    return ((struct Node *)left)->freq - ((struct Node *)right)->freq;
}

// construct canonical Huffman coding tree lengths

void huff_init ()
{
struct Node tree[2 * HUFF_size], *base = tree, *left, *right;
unsigned codes[32], rank[32], weight, mask, count;
int idx, max;
int size;

    // the node tree is built with all the base symbols
    // then interiour nodes are appended

    memset (HuffCode, 0, sizeof(HuffCode));
    memset (tree, 0, sizeof(tree));

    // sort base symbol nodes by their frequencies

    for( size = 0; size < HUFF_size; size++ ) {
        tree[size].left = (void *)size;
        tree[size].freq = Freq[size];
        tree[size].right = NULL;    // indicates a base node
    }

    qsort (tree, HUFF_size, sizeof(struct Node), comp_node);

    // repeatedly combine & remove two lowest freq nodes,
    // construct an interiour node w/sum of these two freq
    // and insert onto the end of the tree (base + size)

    while( size-- > 1 ) {
        left = base;

        if( weight = (base++)->freq )
            weight += base->freq; 
        else
            continue;    // skip zero freq alphabet chars

        right = base++;
        idx = size;

        // sort new interiour node into place

        while( --idx )
          if( base[idx-1].freq > weight )
            base[idx] = base[idx-1];
          else
            break;

        // construct the new interiour node

        base[idx].freq = weight;
        base[idx].right = right;
        base[idx].left = left;
    }

    // base now points at root of tree (size == 1)
    // construct the canonical Huffman code lengths
    // down from here

    memset (codes, 0, sizeof(codes));
    memset (rank, 0, sizeof(rank));

    // enumerate the left & right subtrees,
    // returns the deepest path to leaves

    max = enumerate (rank, base, 0);

    // use canonical Huffman coding technique

    for( idx = 0; idx <= max; idx++ )
        codes[idx + 1] = (codes[idx] + rank[idx]) << 1, rank[idx] = 0;

    // set the code for each non-zero freq alphabet symbol 

    for( idx = 0; idx < HUFF_size; idx++ ) {
      if( count = HuffCode[idx].len )
        HuffCode[idx].code = codes[HuffCode[idx].len] + rank[HuffCode[idx].len]++;
    // transmit canonical huffman coding tree by
    // sending 5 bits for each symbol's length

      mask = 1 << 5;

      while( mask >>= 1 )
          arc_put1 (count & mask);
    }
}

//    output code bits for one alphabet symbol

unsigned huff_encode (unsigned val)
{
unsigned mask = 1 << HuffCode[val].len;
unsigned code = HuffCode[val].code;

    while( mask >>= 1 )
        arc_put1 (code & mask);

    return code;
}

//  perform run-length-encoding
//  using two new Huffman codes
//  for RLE count bits 0 & 1

// repeated zeroes are first counted,
// this count is transmitted in binary
// using 2 special HUFF alphabet symbols
// HUFF_bit0 and HUFF_bit1, representing
// count values 1 & 2:

// transmit HUFF_bit0 = count of 1
// transmit HUFF_bit1 = count of 2
// transmit HUFF_bit0, HUFF_bit0 = count of 3
// transmit HUFF_bit0, HUFF_bit1 = count of 4
// transmit HUFF_bit1, HUFF_bit0 = count of 5
// transmit HUFF_bit1, HUFF_bit1 = count of 6 ...

// to make decoding simpler, transmit any final
// zero code separately from its RLE count

void rle_encode (unsigned code, int flush)
{
    if( !code && !flush ) {
        ZeroCnt++;         // accumulate RLE count
        return;            // except for trailing code
    }

    while( ZeroCnt )  // transmit any RLE count bits
        huff_encode (HUFF_bit0 + (--ZeroCnt & 0x1)), ZeroCnt >>= 1;

    huff_encode (code);
}

//    Move-to-Front decoder

unsigned mtf_decode (unsigned nxt)
{
unsigned char code;

//  Pull the char

    code = MtfOrder[nxt];

//  Now shuffle the order array

    revcpy (MtfOrder + 1, MtfOrder, nxt);
    return MtfOrder[0] = code;
}

// expand BWT into the supplied buffer

void rle_decode (Xform *xform, unsigned size, unsigned last)
{
unsigned xlate[HUFF_size], length[HUFF_size];
unsigned codes[32], rank[32], base[32], bits;
unsigned nxt, count, lvl, idx, out = 0, zero;
unsigned char prev;

    // construct decode table

    memset (codes, 0, sizeof(codes));
    memset (rank, 0, sizeof(rank));

    // retrieve code lengths, 5 bits each

    for( idx = 0; idx < HUFF_size; idx++ ) {
      for( length[idx] = bits = 0; bits < 5; bits++ )
        length[idx] <<= 1, length[idx] |= arc_get1();
      rank[length[idx]]++;
    }

    // construct canonical Huffman code groups
    // one group range for each bit length

    base[0] = base[1] = 0;

    for( idx = 1; idx < 30; idx++ ) {
        codes[idx + 1] = (codes[idx] + rank[idx]) << 1;
        base[idx + 1] = base[idx] + rank[idx];
        rank[idx] = 0;
    }

    // fill in the translated canonical Huffman codes
    // by filling in ranks for each code group

    for( nxt = idx = 0; idx < HUFF_size; idx++ )
      if( lvl = length[idx] )
        xlate[base[lvl] + rank[lvl]++] = idx;

    zero = prev = count = bits = lvl = 0;

    // fill supplied buffer by reading the input
    // one bit at a time and assembling codes

    while( ++lvl < 32 && out < size ) {
      bits <<= 1, bits |= arc_get1 ();

      if( rank[lvl] )
        if( bits < codes[lvl] + rank[lvl] )
          nxt = xlate[base[lvl] + bits - codes[lvl]];
        else
          continue;  // the code is above the range for this length
      else
        continue;    // no symbols with this code length, get next bit

      // nxt is the recognized symbol
      // reset code accumulator

      bits = lvl = 0;

      // process RLE count code as a 1 or 2

      if( nxt > 255 ) {
        count += ( nxt - 255 ) << zero++;
        continue;
      }

      // expand any previously decoded RLE count

      while( count ) {
        if( out == last )       // not needed since we never look at it 
            xform[out].cnt = 0; // but the EOB must not be counted
        else
            xform[out].cnt = Freq[prev]++;

        xform[out++].code = prev;
        count--;
      }

      zero = 0;
      prev = mtf_decode (nxt);  // translate mtf of the symbol

      if( out == last )         // not needed since we never look at it 
          xform[out].cnt = 0;   // but the EOB must not be counted
      else
          xform[out].cnt = Freq[prev]++;

      xform[out++].code = prev;  // store next symbol
    }
}

//    Move-to-Front encoder, and
//    accumulate frequency counts
//    using RLE coding (not for flush)

unsigned char mtf_encode (unsigned char val, int flush)
{
unsigned code;

    code = (unsigned char *)memchr (MtfOrder, val, 256) - MtfOrder;
    revcpy (MtfOrder + 1, MtfOrder, code);
    MtfOrder[0] = val;
    
    if( !flush && !code )
        return ZeroCnt++, code;

    //  accumulate the frequency counts for the
    //  new code and the previous zero run

    Freq[code]++;

    while( ZeroCnt )
        Freq[HUFF_bit0 + (--ZeroCnt & 0x1)]++, ZeroCnt >>= 1;

    return code;
}

//    initialize Move-to-Front symbols

void mtf_init ()
{
unsigned idx;

    for( idx = 0 ; idx < 256 ; idx++ )
        MtfOrder[idx] = (unsigned char)idx;
}

// unpack next bwt segment from current stream into buffer

void bwt_decode (unsigned char *outbuff, unsigned buflen)
{
unsigned last, idx = 0;
Xform *xform;
unsigned ch;

    mtf_init ();
    xform = malloc ((buflen + 1 ) * sizeof(Xform));

    // retrieve last row number

    last = arc_get8 () << 16;
    last |= arc_get8 () << 8;
    last |= arc_get8 ();

// To determine a character's position in the output string given
// its position in the input string, we can use the knowledge about
// the fact that the output string is sorted.  Each character 'c' will
// show up in the output stream in in position i, where i is the sum
// total of all characters in the input buffer that precede c in the
// alphabet (kept in the count array), plus the count of all
// occurences of 'c' previously in the block (kept in xform.cnt)

// The first part of this code calculates the running totals for all
// the characters in the alphabet.  That satisfies the first part of the
// equation needed to determine where each 'c' will go in the output
// stream. Remember that the character pointed to by 'last' is a special
// end-of-buffer character that needs to be larger than any other char
// so we just skip over it while tallying counts

    memset (Freq, 0, sizeof(Freq));
    rle_decode (xform, buflen + 1, last);

    for( idx = 1 ; idx < 256 ; idx++ )
        Freq[idx] += Freq[idx-1];

// Once the transformation vector is in place, writing the
// output is just a matter of computing the indices.  Note
// that we ignore the EOB from the end of data first, and
// process the array backwards from there

    last = idx = buflen;

    while( idx-- ) {
        ch = outbuff[idx] = xform[last].code;
        last = xform[last].cnt;

        if( ch-- )
            last += Freq[ch];
    }

    free (xform);
}

// pack next bwt segment into current stream

void bwt_encode (unsigned char *buff, unsigned max)
{
unsigned idx, off;
KeyPrefix *keys;

    // zero freq counts

    mtf_init ();

    memset (Freq, 0, sizeof(Freq));
    ZeroCnt = 0;

    keys = bwtsort (buff, max);

    // transmit where the EOB is located

    arc_put8 ((unsigned char)(keys->prefix >> 16));
    arc_put8 ((unsigned char)(keys->prefix >> 8));
    arc_put8 ((unsigned char)(keys->prefix));

    //  Write out column L.  Column L consists of all
    // the prefix characters to the sorted strings, in order.
    // It's easy to get the prefix character, but offset 0
    // is handled with care, since its prefix character 
    // is the imaginary end-of-buffer character.

    for( idx = 0; idx < max; idx++ )
      if( off = keys[idx].offset )
          keys[idx].offset = mtf_encode (buff[--off], 0);
      else
          keys[idx].offset = mtf_encode (MtfOrder[0], 0);

    keys[idx].offset = mtf_encode (buff[max - 1], 1);

    // construct huff coding tree and transmit code-lengths

    huff_init ();

    // encode and transmit output

    for( idx = 0; idx < max; idx++ )
      rle_encode (keys[idx].offset, 0);

    rle_encode (keys[max].offset, 1);
    free (keys);
}

#ifdef CODERSTANDALONE

#include <stdio.h>

unsigned char ArcBit = 0, ArcChar = 0;
FILE *In = stdin, *Out = stdout;

int main (int argc, char **argv)
{
int mode, max, size, nxt;
unsigned char *buff;

    if( argc > 1 )
        mode = argv[1][0];
    else {
        printf ("Usage: %s [cd] infile outfile\nnn -- alphabet size\ninfile -- source file\noutfile -- output file", argv[0]);
        return 1;
    }

    if( argc > 3 )
      if( !(Out = fopen (argv[3], "w")) )
        return 1;

#ifndef unix
    _setmode (_fileno (Out), _O_BINARY);
#endif

    //  literal text

    if( mode == 'l' ) {
        max = strlen (argv[2]);
        putc ((unsigned char)(max >> 16), Out);
        putc ((unsigned char)(max >> 8), Out);
        putc ((unsigned char)(max), Out);

        if( max )
            bwt_encode ((unsigned char *)argv[2], max);

        while( ArcBit )  // flush last few bits
           arc_put1 (0);

        return 0;
    }

    if( argc > 2 )
      if( !(In = fopen (argv[2], "r")) )
        return 1;

#ifndef unix
    _setmode (_fileno (In), _O_BINARY);
#endif

    //  decompression

    while( mode == 'd' ) {
        size = getc (In);

        if( size < 0 )
            return 0;

        for( nxt = 0; nxt < 2; nxt++ )
            size <<= 8, size |= getc (In);

        ArcBit = 0;

        if( size ) {
            buff = malloc (size);
            bwt_decode (buff, size);
        }

        for( nxt = 0; nxt < size; nxt++ )
            putc (buff[nxt], Out);

        if( size )
            free (buff);
    }

    // compression

    fseek(In, 0, 2);
    size = ftell(In);
    fseek (In, 0, 0);

    do {
        if( max = size > 900000 ? 900000 : size )
            buff = malloc (max + 128);

        putc ((unsigned char)(max >> 16), Out);
        putc ((unsigned char)(max >> 8), Out);
        putc ((unsigned char)(max), Out);

        for( nxt = 0; nxt < max; nxt++ )
            buff[nxt] = getc(In);

        if( max )
            bwt_encode (buff, max), free (buff);

        while( ArcBit )  // flush last few bits
           arc_put1 (0);

    } while( size -= max );

    return 0;
}

void arc_put1 (int bit)
{
    ArcChar <<= 1;

    if( bit )
        ArcChar |= 1;

    if( ++ArcBit < 8 )
        return;

    putc (ArcChar, Out);
    ArcChar = ArcBit = 0;
}

void arc_put8 (int ch)
{
int idx = 8;

    while( idx-- )
        arc_put1 (ch & 1 << idx);
}

int arc_get1 ()
{
    if( !ArcBit )
        ArcChar = getc (In), ArcBit = 8;

    return ArcChar >> --ArcBit & 1;
}

int arc_get8 ()
{
int idx, result = 0;

    for( idx = 0; idx < 8; idx++ )
        result <<= 1, result |= arc_get1();

    return result;
}
#endif
