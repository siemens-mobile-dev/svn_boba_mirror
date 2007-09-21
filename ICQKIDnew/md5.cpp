/***************************************************************************
 *   Copyright (C) 2007 by Alexander S. Salieff                            *
 *   salieff@mail.ru                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <string.h>
#include "md5.h"

/* typedef a 32 bit type */
typedef unsigned long int UINT4;

/* Data structure for MD5 (Message Digest) computation */
typedef struct {
  UINT4 i[2];                   /* number of _bits_ handled mod 2^64 */
  UINT4 buf[4];                                    /* scratch buffer */
  unsigned char in[64];                              /* input buffer */
  unsigned char digest[16];     /* actual digest after MD5Final call */
} MD5_CTX;

static unsigned char PADDING[64] = {
  0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/* F, G and H are basic MD5 functions: selection, majority, parity */
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z))) 

/* ROTATE_LEFT rotates x left n bits */
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

/* FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4 */
/* Rotation is separate from addition to prevent recomputation */

#define FF(a, b, c, d, x, s, ac) {(a) += F ((b), (c), (d)) + (x) + (UINT4)(ac); (a) = ROTATE_LEFT ((a), (s)); (a) += (b); }
#define GG(a, b, c, d, x, s, ac) {(a) += G ((b), (c), (d)) + (x) + (UINT4)(ac); (a) = ROTATE_LEFT ((a), (s)); (a) += (b); }
#define HH(a, b, c, d, x, s, ac) {(a) += H ((b), (c), (d)) + (x) + (UINT4)(ac); (a) = ROTATE_LEFT ((a), (s)); (a) += (b); }
#define II(a, b, c, d, x, s, ac) {(a) += I ((b), (c), (d)) + (x) + (UINT4)(ac); (a) = ROTATE_LEFT ((a), (s)); (a) += (b); }


/* Basic MD5 step. Transform buf based on in.
 */
static void Transform (UINT4 *buf, UINT4 *in)
{
  UINT4 a = buf[0], b = buf[1], c = buf[2], d = buf[3];

  /* Round 1 */
#define S11 7
#define S12 12
#define S13 17
#define S14 22
  FF ( a, b, c, d, in[ 0], S11, (UINT4) 3614090360u); /* 1 */
  FF ( d, a, b, c, in[ 1], S12, (UINT4) 3905402710u); /* 2 */
  FF ( c, d, a, b, in[ 2], S13, (UINT4)  606105819u); /* 3 */
  FF ( b, c, d, a, in[ 3], S14, (UINT4) 3250441966u); /* 4 */
  FF ( a, b, c, d, in[ 4], S11, (UINT4) 4118548399u); /* 5 */
  FF ( d, a, b, c, in[ 5], S12, (UINT4) 1200080426u); /* 6 */
  FF ( c, d, a, b, in[ 6], S13, (UINT4) 2821735955u); /* 7 */
  FF ( b, c, d, a, in[ 7], S14, (UINT4) 4249261313u); /* 8 */
  FF ( a, b, c, d, in[ 8], S11, (UINT4) 1770035416u); /* 9 */
  FF ( d, a, b, c, in[ 9], S12, (UINT4) 2336552879u); /* 10 */
  FF ( c, d, a, b, in[10], S13, (UINT4) 4294925233u); /* 11 */
  FF ( b, c, d, a, in[11], S14, (UINT4) 2304563134u); /* 12 */
  FF ( a, b, c, d, in[12], S11, (UINT4) 1804603682u); /* 13 */
  FF ( d, a, b, c, in[13], S12, (UINT4) 4254626195u); /* 14 */
  FF ( c, d, a, b, in[14], S13, (UINT4) 2792965006u); /* 15 */
  FF ( b, c, d, a, in[15], S14, (UINT4) 1236535329u); /* 16 */

  /* Round 2 */
#define S21 5
#define S22 9
#define S23 14
#define S24 20
  GG ( a, b, c, d, in[ 1], S21, (UINT4) 4129170786u); /* 17 */
  GG ( d, a, b, c, in[ 6], S22, (UINT4) 3225465664u); /* 18 */
  GG ( c, d, a, b, in[11], S23, (UINT4)  643717713u); /* 19 */
  GG ( b, c, d, a, in[ 0], S24, (UINT4) 3921069994u); /* 20 */
  GG ( a, b, c, d, in[ 5], S21, (UINT4) 3593408605u); /* 21 */
  GG ( d, a, b, c, in[10], S22, (UINT4)   38016083u); /* 22 */
  GG ( c, d, a, b, in[15], S23, (UINT4) 3634488961u); /* 23 */
  GG ( b, c, d, a, in[ 4], S24, (UINT4) 3889429448u); /* 24 */
  GG ( a, b, c, d, in[ 9], S21, (UINT4)  568446438u); /* 25 */
  GG ( d, a, b, c, in[14], S22, (UINT4) 3275163606u); /* 26 */
  GG ( c, d, a, b, in[ 3], S23, (UINT4) 4107603335u); /* 27 */
  GG ( b, c, d, a, in[ 8], S24, (UINT4) 1163531501u); /* 28 */
  GG ( a, b, c, d, in[13], S21, (UINT4) 2850285829u); /* 29 */
  GG ( d, a, b, c, in[ 2], S22, (UINT4) 4243563512u); /* 30 */
  GG ( c, d, a, b, in[ 7], S23, (UINT4) 1735328473u); /* 31 */
  GG ( b, c, d, a, in[12], S24, (UINT4) 2368359562u); /* 32 */

  /* Round 3 */
#define S31 4
#define S32 11
#define S33 16
#define S34 23
  HH ( a, b, c, d, in[ 5], S31, (UINT4) 4294588738u); /* 33 */
  HH ( d, a, b, c, in[ 8], S32, (UINT4) 2272392833u); /* 34 */
  HH ( c, d, a, b, in[11], S33, (UINT4) 1839030562u); /* 35 */
  HH ( b, c, d, a, in[14], S34, (UINT4) 4259657740u); /* 36 */
  HH ( a, b, c, d, in[ 1], S31, (UINT4) 2763975236u); /* 37 */
  HH ( d, a, b, c, in[ 4], S32, (UINT4) 1272893353u); /* 38 */
  HH ( c, d, a, b, in[ 7], S33, (UINT4) 4139469664u); /* 39 */
  HH ( b, c, d, a, in[10], S34, (UINT4) 3200236656u); /* 40 */
  HH ( a, b, c, d, in[13], S31, (UINT4)  681279174u); /* 41 */
  HH ( d, a, b, c, in[ 0], S32, (UINT4) 3936430074u); /* 42 */
  HH ( c, d, a, b, in[ 3], S33, (UINT4) 3572445317u); /* 43 */
  HH ( b, c, d, a, in[ 6], S34, (UINT4)   76029189u); /* 44 */
  HH ( a, b, c, d, in[ 9], S31, (UINT4) 3654602809u); /* 45 */
  HH ( d, a, b, c, in[12], S32, (UINT4) 3873151461u); /* 46 */
  HH ( c, d, a, b, in[15], S33, (UINT4)  530742520u); /* 47 */
  HH ( b, c, d, a, in[ 2], S34, (UINT4) 3299628645u); /* 48 */

  /* Round 4 */
#define S41 6
#define S42 10
#define S43 15
#define S44 21
  II ( a, b, c, d, in[ 0], S41, (UINT4) 4096336452u); /* 49 */
  II ( d, a, b, c, in[ 7], S42, (UINT4) 1126891415u); /* 50 */
  II ( c, d, a, b, in[14], S43, (UINT4) 2878612391u); /* 51 */
  II ( b, c, d, a, in[ 5], S44, (UINT4) 4237533241u); /* 52 */
  II ( a, b, c, d, in[12], S41, (UINT4) 1700485571u); /* 53 */
  II ( d, a, b, c, in[ 3], S42, (UINT4) 2399980690u); /* 54 */
  II ( c, d, a, b, in[10], S43, (UINT4) 4293915773u); /* 55 */
  II ( b, c, d, a, in[ 1], S44, (UINT4) 2240044497u); /* 56 */
  II ( a, b, c, d, in[ 8], S41, (UINT4) 1873313359u); /* 57 */
  II ( d, a, b, c, in[15], S42, (UINT4) 4264355552u); /* 58 */
  II ( c, d, a, b, in[ 6], S43, (UINT4) 2734768916u); /* 59 */
  II ( b, c, d, a, in[13], S44, (UINT4) 1309151649u); /* 60 */
  II ( a, b, c, d, in[ 4], S41, (UINT4) 4149444226u); /* 61 */
  II ( d, a, b, c, in[11], S42, (UINT4) 3174756917u); /* 62 */
  II ( c, d, a, b, in[ 2], S43, (UINT4)  718787259u); /* 63 */
  II ( b, c, d, a, in[ 9], S44, (UINT4) 3951481745u); /* 64 */

  buf[0] += a;
  buf[1] += b;
  buf[2] += c;
  buf[3] += d;
}

static void MD5Init (MD5_CTX *mdContext)
{
  mdContext->i[0] = mdContext->i[1] = (UINT4)0;

  /* Load magic initialization constants.
   */
  mdContext->buf[0] = (UINT4)0x67452301;
  mdContext->buf[1] = (UINT4)0xefcdab89;
  mdContext->buf[2] = (UINT4)0x98badcfe;
  mdContext->buf[3] = (UINT4)0x10325476;
}

static void MD5Update (MD5_CTX *mdContext, unsigned char *inBuf, unsigned int inLen)
{
  UINT4 in[16];
  int mdi;
  unsigned int i, ii;

  /* compute number of bytes mod 64 */
  mdi = (int)((mdContext->i[0] >> 3) & 0x3F);

  /* update number of bits */
  if ((mdContext->i[0] + ((UINT4)inLen << 3)) < mdContext->i[0])
    mdContext->i[1]++;
  mdContext->i[0] += ((UINT4)inLen << 3);
  mdContext->i[1] += ((UINT4)inLen >> 29);

  while (inLen--) {
    /* add new character to buffer, increment mdi */
    mdContext->in[mdi++] = *inBuf++;

    /* transform if necessary */
    if (mdi == 0x40) {
      for (i = 0, ii = 0; i < 16; i++, ii += 4)
        in[i] = (((UINT4)mdContext->in[ii+3]) << 24) |
                (((UINT4)mdContext->in[ii+2]) << 16) |
                (((UINT4)mdContext->in[ii+1]) << 8) |
                ((UINT4)mdContext->in[ii]);
      Transform (mdContext->buf, in);
      mdi = 0;
    }
  }
}

static void MD5Final (MD5_CTX *mdContext)
{
  UINT4 in[16];
  int mdi;
  unsigned int i, ii;
  unsigned int padLen;

  /* save number of bits */
  in[14] = mdContext->i[0];
  in[15] = mdContext->i[1];

  /* compute number of bytes mod 64 */
  mdi = (int)((mdContext->i[0] >> 3) & 0x3F);

  /* pad out to 56 mod 64 */
  padLen = (mdi < 56) ? (56 - mdi) : (120 - mdi);
  MD5Update (mdContext, PADDING, padLen);

  /* append length in bits and transform */
  for (i = 0, ii = 0; i < 14; i++, ii += 4)
    in[i] = (((UINT4)mdContext->in[ii+3]) << 24) |
            (((UINT4)mdContext->in[ii+2]) << 16) |
            (((UINT4)mdContext->in[ii+1]) << 8) |
            ((UINT4)mdContext->in[ii]);
  Transform (mdContext->buf, in);

  /* store buffer in digest */
  for (i = 0, ii = 0; i < 4; i++, ii += 4) {
    mdContext->digest[ii] = (unsigned char)(mdContext->buf[i] & 0xFF);
    mdContext->digest[ii+1] =
      (unsigned char)((mdContext->buf[i] >> 8) & 0xFF);
    mdContext->digest[ii+2] =
      (unsigned char)((mdContext->buf[i] >> 16) & 0xFF);
    mdContext->digest[ii+3] =
      (unsigned char)((mdContext->buf[i] >> 24) & 0xFF);
  }
}

/***********************************************************************
 ** End of md5.c                                                      **
 **********************************************************************/


void calculate_md5(const char * src, unsigned int src_size, char * md5buf)
{
	MD5_CTX mdContext;
	MD5Init(&mdContext);
	MD5Update(&mdContext, (unsigned char *)src, src_size);
	MD5Final (&mdContext);
        memcpy(md5buf, mdContext.digest, 16);
}
