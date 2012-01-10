/*****************************************************************************
 *
 *  XVID MPEG-4 VIDEO CODEC
 *  - GMC interpolation module header -
 *
 *  Copyright(C) 2002-2003 Pascal Massimino <skal@planet-d.net>
 *
 *  This program is free software ; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation ; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY ; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program ; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 * $Id$
 *
 ****************************************************************************/

#include "../portab.h"
#include "../global.h"

/* This is borrowed from	decoder.c   */
static __inline int
gmc_sanitize(int value, int quarterpel, int fcode)
{
	int length = 1 << (fcode+4);

#if 0
	if (quarterpel) value *= 2;
#endif

	if (value < -length)
		return -length;
	else if (value >= length)
		return length-1;
	else return value;
}


/* And this is borrowed from   bitstream.c  until we find a common solution */
static uint32_t __inline
log2bin(uint32_t value)
{
/* Changed by Chenm001 */
#if !defined(_MSC_VER)
  int n = 0;

  while (value) {
	value >>= 1;
	n++;
  }
  return n;
#else
  __asm {
	bsr eax, value
	inc eax
  }
#endif
}
/* 16*sizeof(int) -> 1 or 2 cachelines */
/* table lookup might be faster!  (still to be benchmarked) */

/*
static int log2bin_table[16] =
	{ 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4};
*/
/*	1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 */

#define RDIV(a,b) (((a)>0 ? (a) + ((b)>>1) : (a) - ((b)>>1))/(b))
#define RSHIFT(a,b) ( (a)>0 ? ((a) + (1<<((b)-1)))>>(b) : ((a) + (1<<((b)-1))-1)>>(b))

#define MLT(i)  (((16-(i))<<16) + (i))
static const uint32_t MTab[16] = {
  MLT( 0), MLT( 1), MLT( 2), MLT( 3), MLT( 4), MLT( 5), MLT( 6), MLT( 7),
  MLT( 8), MLT( 9), MLT(10), MLT(11), MLT(12), MLT(13), MLT(14), MLT(15)
};
#undef MLT


/* ************************************************************
 * Pts = 2 or 3
 *
 * Warning! *src is the global frame pointer (that is: adress
 * of pixel 0,0), not the macroblock one.
 * Conversely, *dst is the macroblock top-left adress.
 */

void Predict_16x16_C(const NEW_GMC_DATA * const This,
					 uint8_t *dst, const uint8_t *src,
					 int dststride, int srcstride, int x, int y, int rounding);

void Predict_8x8_C(const NEW_GMC_DATA * const This,
				   uint8_t *uDst, const uint8_t *uSrc,
				   uint8_t *vDst, const uint8_t *vSrc,
				   int dststride, int srcstride, int x, int y, int rounding);

void get_average_mv_C(const NEW_GMC_DATA * const Dsp, VECTOR * const mv,
					  int x, int y, int qpel);

/* ************************************************************
 * simplified version for 1 warp point
 */

void Predict_1pt_16x16_C(const NEW_GMC_DATA * const This,
						 uint8_t *Dst, const uint8_t *Src,
						 int dststride, int srcstride, int x, int y, int rounding);

void Predict_1pt_8x8_C(const NEW_GMC_DATA * const This,
					   uint8_t *uDst, const uint8_t *uSrc,
					   uint8_t *vDst, const uint8_t *vSrc,
					   int dststride, int srcstride, int x, int y, int rounding);

void get_average_mv_1pt_C(const NEW_GMC_DATA * const Dsp, VECTOR * const mv,
						  int x, int y, int qpel);

/* *************************************************************
 * Warning! It's Accuracy being passed, not 'resolution'!
 */

void generate_GMCparameters(int nb_pts, const int accuracy,
							const WARPPOINTS *const pts,
							const int width, const int height,
							NEW_GMC_DATA *const gmc);

/* *******************************************************************
 */

void
generate_GMCimage(	const NEW_GMC_DATA *const gmc_data, /* [input] precalculated data */
					const IMAGE *const pRef,		/* [input] */
					const int mb_width,
					const int mb_height,
					const int stride,
					const int stride2,
					const int fcode, 				/* [input] some parameters... */
  					const int32_t quarterpel,		/* [input] for rounding avgMV */
					const int reduced_resolution,	/* [input] ignored */
					const int32_t rounding,			/* [input] for rounding image data */
					MACROBLOCK *const pMBs, 		/* [output] average motion vectors */
					IMAGE *const pGMC);				/* [output] full warped image */

