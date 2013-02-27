
// A-Law Compression 
// A-Law compression is extremely similar to Mu-Law compression. As you will
// see, they differ primarily in the way that they keep precision. The following
// is a short synopsis of the encoding algorithm, and the code example follows
// the written explanation. First, the sign is stored off. Then the code
// branches. If the absolute value of the source sample is less than 256, the
// 16-bit sample is simply shifted down 4 bits and converted to an 8-bit value,
// thus losing the top 4 bits in the process. However, if it is more than 256,
// a logarithmic algorithm is applied to the sample to determine the precision
// to keep. In that case, the sample is shifted down to access the seven most
// significant bits of the sample. Those seven bits are then used to determine
// the precision of the bottom 4 bits. Finally, the top seven bits are shifted
// back up four bits to make room for the bottom 4 bits. The two are then
// logically OR'd together to create the eight bit compressed sample. The sign
// is then applied, and the entire compressed sample is logically XOR'd, again,
// I assume for transmission reasons. 


static char ALawCompressTable[128] = 
{ 
     1,1,2,2,3,3,3,3, 
     4,4,4,4,4,4,4,4, 
     5,5,5,5,5,5,5,5, 
     5,5,5,5,5,5,5,5, 
     6,6,6,6,6,6,6,6, 
     6,6,6,6,6,6,6,6, 
     6,6,6,6,6,6,6,6, 
     6,6,6,6,6,6,6,6, 
     7,7,7,7,7,7,7,7, 
     7,7,7,7,7,7,7,7, 
     7,7,7,7,7,7,7,7, 
     7,7,7,7,7,7,7,7, 
     7,7,7,7,7,7,7,7, 
     7,7,7,7,7,7,7,7, 
     7,7,7,7,7,7,7,7, 
     7,7,7,7,7,7,7,7 
}; 

unsigned char LinearToALawSample(short sample) 
{ 
     int sign; 
     int exponent; 
     int mantissa; 
     unsigned char compressedByte; 

     sign = ((~sample) >> 8) & 0x80; 
     if (!sign) 
          sample = (short)-sample; 
     if (sample > cClip) 
          sample = cClip; 
     if (sample >= 256) 
     { 
          exponent = (int)ALawCompressTable[(sample >> 8) & 0x7F]; 
          mantissa = (sample >> (exponent + 3) ) & 0x0F; 
          compressedByte = ((exponent << 4) | mantissa); 
     } 
     else 
     { 
          compressedByte = (unsigned char)(sample >> 4); 
     } 
     compressedByte ^= (sign ^ 0x55); 
     return compressedByte; 
} 



// Decompression:
// Now, the most obvious way to decompress a compressed Mu-Law or A-Law sample
// would be to reverse the algorithm. But a more efficient method exists.
// Consider for a moment the fact that A-Law and Mu-Law both take a 16-bit value
// and crunch it down to an 8-bit value. The reverse of that is to take an 8-bit
// value and turn it into a sixteen bit value. In the graphics world, it is
// extremely common to represent 32 and 24 bit values with an eight bit index
// into a palette table. So, why not take a page from the world of graphics and
// use palettes for the Mu-Law and A-Law compression look up? Sounds good to me.
// In fact, these palettes will be smaller than their 24 and 32 bit cousins
// because we only need to represent 16 bit values, not 24 and 32. In a
// nutshell, we will create static lookup tables to do the reverse conversion
// from A-Law and Mu-Law. The two differing tables are presented below. To
// convert from your compressed sample back to the raw 16-bit sample, just use
// your compressed sample as the index into the table, and the corresponding
// value in the table is your decompressed 16-bit sample. Obviously, the
// downside is that this method requires the memory overhead for the tables,
// but each table is only 512 bytes. In this day and age, that's downright
// cheap for the absolute fastest decompression! 


static short ALawDecompressTable[256] = 
{ 
     -5504, -5248, -6016, -5760, -4480, -4224, -4992, -4736, 
     -7552, -7296, -8064, -7808, -6528, -6272, -7040, -6784, 
     -2752, -2624, -3008, -2880, -2240, -2112, -2496, -2368, 
     -3776, -3648, -4032, -3904, -3264, -3136, -3520, -3392, 
     -22016,-20992,-24064,-23040,-17920,-16896,-19968,-18944, 
     -30208,-29184,-32256,-31232,-26112,-25088,-28160,-27136, 
     -11008,-10496,-12032,-11520,-8960, -8448, -9984, -9472, 
     -15104,-14592,-16128,-15616,-13056,-12544,-14080,-13568, 
     -344,  -328,  -376,  -360,  -280,  -264,  -312,  -296, 
     -472,  -456,  -504,  -488,  -408,  -392,  -440,  -424, 
     -88,   -72,   -120,  -104,  -24,   -8,    -56,   -40, 
     -216,  -200,  -248,  -232,  -152,  -136,  -184,  -168, 
     -1376, -1312, -1504, -1440, -1120, -1056, -1248, -1184, 
     -1888, -1824, -2016, -1952, -1632, -1568, -1760, -1696, 
     -688,  -656,  -752,  -720,  -560,  -528,  -624,  -592, 
     -944,  -912,  -1008, -976,  -816,  -784,  -880,  -848, 
      5504,  5248,  6016,  5760,  4480,  4224,  4992,  4736, 
      7552,  7296,  8064,  7808,  6528,  6272,  7040,  6784, 
      2752,  2624,  3008,  2880,  2240,  2112,  2496,  2368, 
      3776,  3648,  4032,  3904,  3264,  3136,  3520,  3392, 
      22016, 20992, 24064, 23040, 17920, 16896, 19968, 18944, 
      30208, 29184, 32256, 31232, 26112, 25088, 28160, 27136, 
      11008, 10496, 12032, 11520, 8960,  8448,  9984,  9472, 
      15104, 14592, 16128, 15616, 13056, 12544, 14080, 13568, 
      344,   328,   376,   360,   280,   264,   312,   296, 
      472,   456,   504,   488,   408,   392,   440,   424, 
      88,    72,   120,   104,    24,     8,    56,    40, 
      216,   200,   248,   232,   152,   136,   184,   168, 
      1376,  1312,  1504,  1440,  1120,  1056,  1248,  1184, 
      1888,  1824,  2016,  1952,  1632,  1568,  1760,  1696, 
      688,   656,   752,   720,   560,   528,   624,   592, 
      944,   912,  1008,   976,   816,   784,   880,   848 
}; 

