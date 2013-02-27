
// MuLaw Compresion 
// the purpose of the algorithm is to compress a 16-bit source sample down to
// an 8-bit sample.
// First, the algorithm first stores off the sign. It then adds in a bias value
// which (due to wrapping) will cause high valued samples to lose precision.
// The top five most significant bits are pulled out of the sample (which has
// been previously biased). Then, the bottom three bits of the compressed byte
// are generated using a small look-up table, based on the biased value of the
// source sample. The 8-bit compressed sample is then finally created by
// logically OR'ing together the 5 most important bits, the 3 lower bits, and
// the sign when applicable. The bits are the logically NOT'ed, which I assume
// is for transmission reasons (although you might not transmit your sample.)

const int cBias = 0x84; 
const int cClip = 32635; 

static char MuLawCompressTable[256] = 
{ 
     0,0,1,1,2,2,2,2,3,3,3,3,3,3,3,3, 
     4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4, 
     5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5, 
     5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5, 
     6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6, 
     6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6, 
     6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6, 
     6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6, 
     7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7, 
     7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7, 
     7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7, 
     7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7, 
     7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7, 
     7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7, 
     7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7, 
     7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7 
}; 

unsigned char LinearToMuLawSample(short sample) 
{ 
     int sign = (sample >> 8) & 0x80;	
     if (sign) 
          sample = (short)-sample; 
     if (sample > cClip) 
          sample = cClip; 
     sample = (short)(sample + cBias); 
     int exponent = (int)MuLawCompressTable[(sample>>7) & 0xFF]; 
     int mantissa = (sample >> (exponent+3)) & 0x0F; 
     int compressedByte = ~ (sign | (exponent << 4) | mantissa); 

     return (unsigned char)compressedByte; 
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


static short MuLawDecompressTable[256] = 
{ 
     -32124,-31100,-30076,-29052,-28028,-27004,-25980,-24956, 
     -23932,-22908,-21884,-20860,-19836,-18812,-17788,-16764, 
     -15996,-15484,-14972,-14460,-13948,-13436,-12924,-12412, 
     -11900,-11388,-10876,-10364, -9852, -9340, -8828, -8316, 
      -7932, -7676, -7420, -7164, -6908, -6652, -6396, -6140, 
      -5884, -5628, -5372, -5116, -4860, -4604, -4348, -4092, 
      -3900, -3772, -3644, -3516, -3388, -3260, -3132, -3004, 
      -2876, -2748, -2620, -2492, -2364, -2236, -2108, -1980, 
      -1884, -1820, -1756, -1692, -1628, -1564, -1500, -1436, 
      -1372, -1308, -1244, -1180, -1116, -1052,  -988,  -924, 
       -876,  -844,  -812,  -780,  -748,  -716,  -684,  -652, 
       -620,  -588,  -556,  -524,  -492,  -460,  -428,  -396, 
       -372,  -356,  -340,  -324,  -308,  -292,  -276,  -260, 
       -244,  -228,  -212,  -196,  -180,  -164,  -148,  -132, 
       -120,  -112,  -104,   -96,   -88,   -80,   -72,   -64, 
        -56,   -48,   -40,   -32,   -24,   -16,    -8,     -1, 
      32124, 31100, 30076, 29052, 28028, 27004, 25980, 24956, 
      23932, 22908, 21884, 20860, 19836, 18812, 17788, 16764, 
      15996, 15484, 14972, 14460, 13948, 13436, 12924, 12412, 
      11900, 11388, 10876, 10364,  9852,  9340,  8828,  8316, 
       7932,  7676,  7420,  7164,  6908,  6652,  6396,  6140, 
       5884,  5628,  5372,  5116,  4860,  4604,  4348,  4092, 
       3900,  3772,  3644,  3516,  3388,  3260,  3132,  3004, 
       2876,  2748,  2620,  2492,  2364,  2236,  2108,  1980, 
       1884,  1820,  1756,  1692,  1628,  1564,  1500,  1436, 
       1372,  1308,  1244,  1180,  1116,  1052,   988,   924, 
        876,   844,   812,   780,   748,   716,   684,   652, 
        620,   588,   556,   524,   492,   460,   428,   396, 
        372,   356,   340,   324,   308,   292,   276,   260, 
        244,   228,   212,   196,   180,   164,   148,   132, 
        120,   112,   104,    96,    88,    80,    72,    64, 
         56,    48,    40,    32,    24,    16,     8,     0 
}; 

