#ifdef LINKED
#include "xlink.h"
#else
#include <stdlib.h>
#include <memory.h>
#endif

//  these two values are stored together
//  to improve processor cache hits

typedef struct {
    unsigned prefix, offset;
} KeyPrefix;

//  offset/key prefix
//  for qsort to use

KeyPrefix *Keys;
unsigned *Rank;

//  During the first round which qsorts the prefix into
//  order, a groups of equal keys are chained together
//  into work units for the next round, using
//  the first two keys of the group

unsigned WorkChain;

//  set the offset rankings and create
//  new work units for unsorted groups
//  of equal keys

void bwtsetranks (unsigned from, unsigned cnt)
{
unsigned idx = 0;

    // all members of a group get the same rank

    while( idx < cnt )
        Rank[Keys[from+idx++].offset] = from;

    // is this a sortable group?

    if( cnt < 2 )
        return;    // final ranking was set

    // if so, add this group to work chain for next round
    // by using the first two key prefix from the group.

    Keys[from].prefix = WorkChain;
    Keys[from + 1].prefix = cnt;
    WorkChain = from;
}

//  set the sort key (prefix) from the ranking of the offsets
//  for rounds after the initial one.

void bwtkeygroup (unsigned from, unsigned cnt, unsigned offset)
{
unsigned off;

  while( cnt-- ) {
    off = Keys[from].offset + offset;
    Keys[from++].prefix = Rank[off];
  }
}

//  the tri-partite qsort partitioning

//  creates two sets of pivot valued
//  elements from [0:leq] and [heq:size]
//  while partitioning a segment of the Keys

void bwtpartition (unsigned start, unsigned size)
{
KeyPrefix tmp, pvt, *lo;
unsigned loguy, higuy;
unsigned leq, heq;

  while( size > 7 ) {
    // find median-of-three element to use as a pivot
    // and swap it to the beginning of the array
    // to begin the leq group of pivot equals.

    // the larger-of-three element goes to higuy
    // the smallest-of-three element goes to middle

    lo = Keys + start;
    higuy = size - 1;
    leq = loguy = 0;

    //  move larger of lo and hi to tmp,hi

    tmp = lo[higuy];

    if( tmp.prefix < lo->prefix )
        lo[higuy] = *lo, *lo = tmp, tmp = lo[higuy];

    //  move larger of tmp,hi and mid to hi

    if( lo[size >> 1].prefix > tmp.prefix )
        lo[higuy] = lo[size >> 1], lo[size >> 1] = tmp;

    //  move larger of mid and lo to pvt,lo
    //  and the smaller into the middle

    pvt = *lo;

    if( pvt.prefix < lo[size >> 1].prefix )
        *lo = lo[size >> 1], lo[size >> 1] = pvt, pvt = *lo;

    //  start the high group of equals
    //  with a pivot valued element, or not

    if( pvt.prefix == lo[higuy].prefix )
        heq = higuy;
    else
        heq = size;

    for( ; ; ) {
        //  both higuy and loguy are already in position
        //  loguy leaves .le. elements beneath it
        //  and swaps equal to pvt elements to leq

        while( ++loguy < higuy )
          if( pvt.prefix < lo[loguy].prefix )
              break;
          else if( pvt.prefix == lo[loguy].prefix )
           if( ++leq < loguy )
            tmp = lo[loguy], lo[loguy] = lo[leq], lo[leq] = tmp;

        //  higuy leaves .ge. elements above it
        //  and swaps equal to pvt elements to heq

        while( --higuy > loguy )
          if( pvt.prefix > lo[higuy].prefix )
              break;
          else if( pvt.prefix == lo[higuy].prefix )
           if( --heq > higuy )
            tmp = lo[higuy], lo[higuy] = lo[heq], lo[heq] = tmp;

        // quit when they finally meet at the empty middle

        if( higuy <= loguy )
            break;

        // element loguy is .gt. element higuy
        // swap them around (the pivot)

        tmp = lo[higuy];
        lo[higuy] = lo[loguy];
        lo[loguy] = tmp;
    }

    // initialize an empty pivot value group

    higuy = loguy;

    //  swap the group of pivot equals into the middle from
    //  the leq and heq sets. Include original pivot in
    //  the leq set.  higuy will be the lowest pivot
    //  element; loguy will be one past the highest.

    //  the heq set might be empty or completely full.

    if( loguy < heq )
      while( heq < size )
        tmp = lo[loguy], lo[loguy++] = lo[heq], lo[heq++] = tmp;
    else
        loguy = size;  // no high elements, they're all pvt valued

    //  the leq set always has the original pivot, but might
    //  also be completely full of pivot valued elements.

    if( higuy > ++leq )
        while( leq )
          tmp = lo[--higuy], lo[higuy] = lo[--leq], lo[leq] = tmp;
    else
        higuy = 0;    // no low elements, they're all pvt valued

    //  The partitioning around pvt is done.
    //  ranges [0:higuy-1] .lt. pivot and [loguy:size-1] .gt. pivot

    //  set the new group rank of the middle range [higuy:loguy-1]
    //  (the .lt. and .gt. ranges get set during their selection sorts)

    bwtsetranks (start + higuy, loguy - higuy);

    //  pick the smaller group to partition first,
    //  then loop with larger group.

    if( higuy < size - loguy ) {
        bwtpartition (start, higuy);
        size -= loguy;
        start += loguy;
    } else {
        bwtpartition (start + loguy, size - loguy);
        size = higuy;
    }
  }

  //  do a selection sort for small sets by
  //  repeately selecting the smallest key to
  //  start, and pulling any group together
  //  for it at leq

  while( size ) {
    for( leq = loguy = 0; ++loguy < size; )
      if( Keys[start].prefix > Keys[start + loguy].prefix )
        tmp = Keys[start], Keys[start] = Keys[start + loguy], Keys[start + loguy] = tmp, leq = 0;
      else if( Keys[start].prefix == Keys[start + loguy].prefix )
       if( ++leq < loguy )
        tmp = Keys[start + leq], Keys[start + leq] = Keys[start + loguy], Keys[start + loguy] = tmp;

    //  now set the rank for the group of size >= 1

    bwtsetranks (start, ++leq);
    start += leq;
    size -= leq;
   }
}

// the main entry point

KeyPrefix* bwtsort (unsigned char *buff, unsigned size)
{
unsigned start, cnt, chain;
unsigned offset = 0, off;
unsigned prefix[1];

  //  the Key and Rank arrays include stopper elements

  Keys = malloc ((size + 1 ) * sizeof(KeyPrefix));
  memset (prefix, 0xff, sizeof(prefix));

  // construct the suffix sorting key for each offset

  for( off = size; off--; ) {
    *prefix >>= 8;
    *prefix |= buff[off] << (sizeof(prefix) * 8 - 8);
    Keys[off].prefix = *prefix;
    Keys[off].offset = off;
  }

  // the ranking of each suffix offset,
  // plus extra ranks for the stopper elements

  Rank = malloc ((size + sizeof(prefix)) * sizeof(unsigned));

  // fill in the extra stopper ranks

  for( off = 0; off < sizeof(prefix); off++ )
    Rank[size + off] = size + off;

  // perform the initial qsort based on the key prefix constructed
  // above.  Inialize the work unit chain terminator.

  WorkChain = size;
  bwtpartition (0, size);

  // the first pass used prefix keys constructed above,
  // subsequent passes use the offset rankings as keys

  offset = sizeof(prefix); 

  // continue doubling the key offset until there are no
  // undifferentiated suffix groups created during a run

  while( WorkChain < size ) {
    chain = WorkChain;
    WorkChain = size;

    // consume the work units created last round
    // and preparing new work units for next pass
    // (work is created in bwtsetranks)

    do {
      start = chain;
      chain = Keys[start].prefix;
      cnt = Keys[start + 1].prefix;
      bwtkeygroup (start, cnt, offset);
      bwtpartition (start, cnt);
    } while( chain < size );

    //  each pass doubles the range of suffix considered,
    //  achieving Order(n * log(n)) comparisons

    offset <<= 1;
  }

  //  return the rank of offset zero in the first key

  Keys->prefix = Rank[0];
  free (Rank);
  return Keys;
}

#ifdef SORTSTANDALONE
#include <stdio.h>

int main (int argc, char **argv)
{
unsigned size, nxt;
unsigned char *buff;
KeyPrefix *keys;
FILE *in;

    in = fopen(argv[1], "rb");

    fseek(in, 0, 2);
    size = ftell(in);
    fseek (in, 0, 0);
    buff = malloc (size);

    for( nxt = 0; nxt < size; nxt++ )
        buff[nxt] = getc(in);

    keys = bwtsort (buff, size);

    for( nxt = 0; nxt < size; nxt++ )
        putc(buff[keys[nxt].offset], stdout);
}
#endif
