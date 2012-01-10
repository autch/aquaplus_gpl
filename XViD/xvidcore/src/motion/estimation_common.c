/*****************************************************************************
 *
 *  XVID MPEG-4 VIDEO CODEC
 *  - Motion Estimation shared functions -
 *
 *  Copyright(C) 2002 Christoph Lampert <gruel@web.de>
 *               2002 Michael Militzer <michael@xvid.org>
 *               2002-2003 Radoslaw Czyz <xvid@syskin.cjb.net>
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

#include "../encoder.h"
#include "../global.h"
#include "../image/interpolate8x8.h"
#include "estimation.h"
#include "motion.h"
#include "sad.h"
#include "motion_inlines.h"


/*****************************************************************************
 * Modified rounding tables
 * Original tables see ISO spec tables 7-6 -> 7-9
 ****************************************************************************/

const uint32_t roundtab[16] =
{0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2 };

/* K = 4 */
const uint32_t roundtab_76[16] =
{ 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1 };

/* K = 2 */
const uint32_t roundtab_78[8] =
{ 0, 0, 1, 1, 0, 0, 0, 1  };

/* K = 1 */
const uint32_t roundtab_79[4] =
{ 0, 1, 0, 0 };

const int xvid_me_lambda_vec16[32] =
	{     0    ,(int)(1.00235 * NEIGH_TEND_16X16 + 0.5),
	(int)(1.15582*NEIGH_TEND_16X16 + 0.5), (int)(1.31976*NEIGH_TEND_16X16 + 0.5),
	(int)(1.49591*NEIGH_TEND_16X16 + 0.5), (int)(1.68601*NEIGH_TEND_16X16 + 0.5),
	(int)(1.89187*NEIGH_TEND_16X16 + 0.5), (int)(2.11542*NEIGH_TEND_16X16 + 0.5),
	(int)(2.35878*NEIGH_TEND_16X16 + 0.5), (int)(2.62429*NEIGH_TEND_16X16 + 0.5),
	(int)(2.91455*NEIGH_TEND_16X16 + 0.5), (int)(3.23253*NEIGH_TEND_16X16 + 0.5),
	(int)(3.58158*NEIGH_TEND_16X16 + 0.5), (int)(3.96555*NEIGH_TEND_16X16 + 0.5),
	(int)(4.38887*NEIGH_TEND_16X16 + 0.5), (int)(4.85673*NEIGH_TEND_16X16 + 0.5),
	(int)(5.37519*NEIGH_TEND_16X16 + 0.5), (int)(5.95144*NEIGH_TEND_16X16 + 0.5),
	(int)(6.59408*NEIGH_TEND_16X16 + 0.5), (int)(7.31349*NEIGH_TEND_16X16 + 0.5),
	(int)(8.12242*NEIGH_TEND_16X16 + 0.5), (int)(9.03669*NEIGH_TEND_16X16 + 0.5),
	(int)(10.0763*NEIGH_TEND_16X16 + 0.5), (int)(11.2669*NEIGH_TEND_16X16 + 0.5),
	(int)(12.6426*NEIGH_TEND_16X16 + 0.5), (int)(14.2493*NEIGH_TEND_16X16 + 0.5),
	(int)(16.1512*NEIGH_TEND_16X16 + 0.5), (int)(18.442*NEIGH_TEND_16X16 + 0.5),
	(int)(21.2656*NEIGH_TEND_16X16 + 0.5), (int)(24.8580*NEIGH_TEND_16X16 + 0.5),
	(int)(29.6436*NEIGH_TEND_16X16 + 0.5), (int)(36.4949*NEIGH_TEND_16X16 + 0.5)
};

/*****************************************************************************
 * Code
 ****************************************************************************/

int32_t
xvid_me_ChromaSAD(const int dx, const int dy, SearchData * const data)
{
	int sad;
	const uint32_t stride = data->iEdgedWidth/2;
	int offset = (dx>>1) + (dy>>1)*stride;
	int next = 1;

	if (dx == data->chromaX && dy == data->chromaY) 
		return data->chromaSAD; /* it has been checked recently */
	data->chromaX = dx; data->chromaY = dy; /* backup */

	switch (((dx & 1) << 1) | (dy & 1))	{
		case 0:
			sad = sad8(data->CurU, data->RefP[4] + offset, stride);
			sad += sad8(data->CurV, data->RefP[5] + offset, stride);
			break;
		case 1:
			next = stride;
		case 2:
			sad = sad8bi(data->CurU, data->RefP[4] + offset, data->RefP[4] + offset + next, stride);
			sad += sad8bi(data->CurV, data->RefP[5] + offset, data->RefP[5] + offset + next, stride);
			break;
		default:
			interpolate8x8_halfpel_hv(data->RefQ, data->RefP[4] + offset, stride, data->rounding);
			sad = sad8(data->CurU, data->RefQ, stride);

			interpolate8x8_halfpel_hv(data->RefQ, data->RefP[5] + offset, stride, data->rounding);
			sad += sad8(data->CurV, data->RefQ, stride);
			break;
	}
	data->chromaSAD = sad; /* backup, part 2 */
	return sad;
}

uint8_t *
xvid_me_interpolate8x8qpel(const int x, const int y, const uint32_t block, const uint32_t dir, const SearchData * const data)
{
	/* create or find a qpel-precision reference picture; return pointer to it */
	uint8_t * Reference = data->RefQ + 16*dir;
	const uint32_t iEdgedWidth = data->iEdgedWidth;
	const uint32_t rounding = data->rounding;
	const int halfpel_x = x/2;
	const int halfpel_y = y/2;
	const uint8_t *ref1, *ref2, *ref3, *ref4;

	ref1 = GetReferenceB(halfpel_x, halfpel_y, dir, data);
	ref1 += 8 * (block&1) + 8 * (block>>1) * iEdgedWidth;
	switch( ((x&1)<<1) + (y&1) ) {
	case 3: /* x and y in qpel resolution - the "corners" (top left/right and */
			/* bottom left/right) during qpel refinement */
		ref2 = GetReferenceB(halfpel_x, y - halfpel_y, dir, data);
		ref3 = GetReferenceB(x - halfpel_x, halfpel_y, dir, data);
		ref4 = GetReferenceB(x - halfpel_x, y - halfpel_y, dir, data);
		ref2 += 8 * (block&1) + 8 * (block>>1) * iEdgedWidth;
		ref3 += 8 * (block&1) + 8 * (block>>1) * iEdgedWidth;
		ref4 += 8 * (block&1) + 8 * (block>>1) * iEdgedWidth;
		interpolate8x8_avg4(Reference, ref1, ref2, ref3, ref4, iEdgedWidth, rounding);
		break;

	case 1: /* x halfpel, y qpel - top or bottom during qpel refinement */
		ref2 = GetReferenceB(halfpel_x, y - halfpel_y, dir, data);
		ref2 += 8 * (block&1) + 8 * (block>>1) * iEdgedWidth;
		interpolate8x8_avg2(Reference, ref1, ref2, iEdgedWidth, rounding, 8);
		break;

	case 2: /* x qpel, y halfpel - left or right during qpel refinement */
		ref2 = GetReferenceB(x - halfpel_x, halfpel_y, dir, data);
		ref2 += 8 * (block&1) + 8 * (block>>1) * iEdgedWidth;
		interpolate8x8_avg2(Reference, ref1, ref2, iEdgedWidth, rounding, 8);
		break;

	default: /* pure halfpel position */
		return (uint8_t *) ref1;

	}
	return Reference;
}

uint8_t *
xvid_me_interpolate16x16qpel(const int x, const int y, const uint32_t dir, const SearchData * const data)
{
	/* create or find a qpel-precision reference picture; return pointer to it */
	uint8_t * Reference = data->RefQ + 16*dir;
	const uint32_t iEdgedWidth = data->iEdgedWidth;
	const uint32_t rounding = data->rounding;
	const int halfpel_x = x/2;
	const int halfpel_y = y/2;
	const uint8_t *ref1, *ref2, *ref3, *ref4;

	ref1 = GetReferenceB(halfpel_x, halfpel_y, dir, data);
	switch( ((x&1)<<1) + (y&1) ) {
	case 3:
		/*
		 * x and y in qpel resolution - the "corners" (top left/right and
		 * bottom left/right) during qpel refinement
		 */
		ref2 = GetReferenceB(halfpel_x, y - halfpel_y, dir, data);
		ref3 = GetReferenceB(x - halfpel_x, halfpel_y, dir, data);
		ref4 = GetReferenceB(x - halfpel_x, y - halfpel_y, dir, data);
		interpolate8x8_avg4(Reference, ref1, ref2, ref3, ref4, iEdgedWidth, rounding);
		interpolate8x8_avg4(Reference+8, ref1+8, ref2+8, ref3+8, ref4+8, iEdgedWidth, rounding);
		interpolate8x8_avg4(Reference+8*iEdgedWidth, ref1+8*iEdgedWidth, ref2+8*iEdgedWidth, ref3+8*iEdgedWidth, ref4+8*iEdgedWidth, iEdgedWidth, rounding);
		interpolate8x8_avg4(Reference+8*iEdgedWidth+8, ref1+8*iEdgedWidth+8, ref2+8*iEdgedWidth+8, ref3+8*iEdgedWidth+8, ref4+8*iEdgedWidth+8, iEdgedWidth, rounding);
		break;

	case 1: /* x halfpel, y qpel - top or bottom during qpel refinement */
		ref2 = GetReferenceB(halfpel_x, y - halfpel_y, dir, data);
		interpolate8x8_avg2(Reference, ref1, ref2, iEdgedWidth, rounding, 8);
		interpolate8x8_avg2(Reference+8, ref1+8, ref2+8, iEdgedWidth, rounding, 8);
		interpolate8x8_avg2(Reference+8*iEdgedWidth, ref1+8*iEdgedWidth, ref2+8*iEdgedWidth, iEdgedWidth, rounding, 8);
		interpolate8x8_avg2(Reference+8*iEdgedWidth+8, ref1+8*iEdgedWidth+8, ref2+8*iEdgedWidth+8, iEdgedWidth, rounding, 8);
		break;

	case 2: /* x qpel, y halfpel - left or right during qpel refinement */
		ref2 = GetReferenceB(x - halfpel_x, halfpel_y, dir, data);
		interpolate8x8_avg2(Reference, ref1, ref2, iEdgedWidth, rounding, 8);
		interpolate8x8_avg2(Reference+8, ref1+8, ref2+8, iEdgedWidth, rounding, 8);
		interpolate8x8_avg2(Reference+8*iEdgedWidth, ref1+8*iEdgedWidth, ref2+8*iEdgedWidth, iEdgedWidth, rounding, 8);
		interpolate8x8_avg2(Reference+8*iEdgedWidth+8, ref1+8*iEdgedWidth+8, ref2+8*iEdgedWidth+8, iEdgedWidth, rounding, 8);
		break;


	default: /* pure halfpel position */
		return (uint8_t *) ref1;
	}
	return Reference;
}

void
xvid_me_AdvDiamondSearch(int x, int y, SearchData * const data,
						 int bDirection, CheckFunc * const CheckCandidate)
{

/* directions: 1 - left (x-1); 2 - right (x+1), 4 - up (y-1); 8 - down (y+1) */

	unsigned int * const iDirection = &data->dir;

	for(;;) { /* forever */
		*iDirection = 0;
		if (bDirection & 1) CHECK_CANDIDATE(x - iDiamondSize, y, 1);
		if (bDirection & 2) CHECK_CANDIDATE(x + iDiamondSize, y, 2);
		if (bDirection & 4) CHECK_CANDIDATE(x, y - iDiamondSize, 4);
		if (bDirection & 8) CHECK_CANDIDATE(x, y + iDiamondSize, 8);

		/* now we're doing diagonal checks near our candidate */

		if (*iDirection) {		/* if anything found */
			bDirection = *iDirection;
			*iDirection = 0;
			x = data->currentMV->x; y = data->currentMV->y;
			if (bDirection & 3) {	/* our candidate is left or right */
				CHECK_CANDIDATE(x, y + iDiamondSize, 8);
				CHECK_CANDIDATE(x, y - iDiamondSize, 4);
			} else {			/* what remains here is up or down */
				CHECK_CANDIDATE(x + iDiamondSize, y, 2);
				CHECK_CANDIDATE(x - iDiamondSize, y, 1);
			}

			if (*iDirection) {
				bDirection += *iDirection;
				x = data->currentMV->x; y = data->currentMV->y;
			}
		} else {				/* about to quit, eh? not so fast.... */
			switch (bDirection) {
			case 2:
				CHECK_CANDIDATE(x + iDiamondSize, y - iDiamondSize, 2 + 4);
				CHECK_CANDIDATE(x + iDiamondSize, y + iDiamondSize, 2 + 8);
				break;
			case 1:
				CHECK_CANDIDATE(x - iDiamondSize, y - iDiamondSize, 1 + 4);
				CHECK_CANDIDATE(x - iDiamondSize, y + iDiamondSize, 1 + 8);
				break;
			case 2 + 4:
				CHECK_CANDIDATE(x - iDiamondSize, y - iDiamondSize, 1 + 4);
				CHECK_CANDIDATE(x + iDiamondSize, y - iDiamondSize, 2 + 4);
				CHECK_CANDIDATE(x + iDiamondSize, y + iDiamondSize, 2 + 8);
				break;
			case 4:
				CHECK_CANDIDATE(x + iDiamondSize, y - iDiamondSize, 2 + 4);
				CHECK_CANDIDATE(x - iDiamondSize, y - iDiamondSize, 1 + 4);
				break;
			case 8:
				CHECK_CANDIDATE(x + iDiamondSize, y + iDiamondSize, 2 + 8);
				CHECK_CANDIDATE(x - iDiamondSize, y + iDiamondSize, 1 + 8);
				break;
			case 1 + 4:
				CHECK_CANDIDATE(x - iDiamondSize, y + iDiamondSize, 1 + 8);
				CHECK_CANDIDATE(x - iDiamondSize, y - iDiamondSize, 1 + 4);
				CHECK_CANDIDATE(x + iDiamondSize, y - iDiamondSize, 2 + 4);
				break;
			case 2 + 8:
				CHECK_CANDIDATE(x - iDiamondSize, y - iDiamondSize, 1 + 4);
				CHECK_CANDIDATE(x - iDiamondSize, y + iDiamondSize, 1 + 8);
				CHECK_CANDIDATE(x + iDiamondSize, y + iDiamondSize, 2 + 8);
				break;
			case 1 + 8:
				CHECK_CANDIDATE(x + iDiamondSize, y - iDiamondSize, 2 + 4);
				CHECK_CANDIDATE(x + iDiamondSize, y + iDiamondSize, 2 + 8);
				CHECK_CANDIDATE(x - iDiamondSize, y + iDiamondSize, 1 + 8);
				break;
			default:		/* 1+2+4+8 == we didn't find anything at all */
				CHECK_CANDIDATE(x - iDiamondSize, y - iDiamondSize, 1 + 4);
				CHECK_CANDIDATE(x - iDiamondSize, y + iDiamondSize, 1 + 8);
				CHECK_CANDIDATE(x + iDiamondSize, y - iDiamondSize, 2 + 4);
				CHECK_CANDIDATE(x + iDiamondSize, y + iDiamondSize, 2 + 8);
				break;
			}
			if (!*iDirection) break;		/* ok, the end. really */
			bDirection = *iDirection;
			x = data->currentMV->x; y = data->currentMV->y;
		}
	}
}

void
xvid_me_SquareSearch(int x, int y, SearchData * const data,
					 int bDirection, CheckFunc * const CheckCandidate)
{
	unsigned int * const iDirection = &data->dir;

	do {
		*iDirection = 0;
		if (bDirection & 1) CHECK_CANDIDATE(x - iDiamondSize, y, 1+16+64);
		if (bDirection & 2) CHECK_CANDIDATE(x + iDiamondSize, y, 2+32+128);
		if (bDirection & 4) CHECK_CANDIDATE(x, y - iDiamondSize, 4+16+32);
		if (bDirection & 8) CHECK_CANDIDATE(x, y + iDiamondSize, 8+64+128);
		if (bDirection & 16) CHECK_CANDIDATE(x - iDiamondSize, y - iDiamondSize, 1+4+16+32+64);
		if (bDirection & 32) CHECK_CANDIDATE(x + iDiamondSize, y - iDiamondSize, 2+4+16+32+128);
		if (bDirection & 64) CHECK_CANDIDATE(x - iDiamondSize, y + iDiamondSize, 1+8+16+64+128);
		if (bDirection & 128) CHECK_CANDIDATE(x + iDiamondSize, y + iDiamondSize, 2+8+32+64+128);

		bDirection = *iDirection;
		x = data->currentMV->x; y = data->currentMV->y;
	} while (*iDirection);
}

void
xvid_me_DiamondSearch(int x, int y, SearchData * const data,
					  int bDirection, CheckFunc * const CheckCandidate)
{

/* directions: 1 - left (x-1); 2 - right (x+1), 4 - up (y-1); 8 - down (y+1) */

	unsigned int * const iDirection = &data->dir;

	do {
		*iDirection = 0;
		if (bDirection & 1) CHECK_CANDIDATE(x - iDiamondSize, y, 1);
		if (bDirection & 2) CHECK_CANDIDATE(x + iDiamondSize, y, 2);
		if (bDirection & 4) CHECK_CANDIDATE(x, y - iDiamondSize, 4);
		if (bDirection & 8) CHECK_CANDIDATE(x, y + iDiamondSize, 8);

		/* now we're doing diagonal checks near our candidate */

		if (*iDirection) {		/* checking if anything found */
			bDirection = *iDirection;
			*iDirection = 0;
			x = data->currentMV->x; y = data->currentMV->y;
			if (bDirection & 3) {	/* our candidate is left or right */
				CHECK_CANDIDATE(x, y + iDiamondSize, 8);
				CHECK_CANDIDATE(x, y - iDiamondSize, 4);
			} else {			/* what remains here is up or down */
				CHECK_CANDIDATE(x + iDiamondSize, y, 2);
				CHECK_CANDIDATE(x - iDiamondSize, y, 1);
			}
			bDirection += *iDirection;
			x = data->currentMV->x; y = data->currentMV->y;
		}
	}
	while (*iDirection);
}

void
xvid_me_SubpelRefine(SearchData * const data, CheckFunc * const CheckCandidate)
{
/* Do a half-pel or q-pel refinement */
	const VECTOR centerMV = data->qpel_precision ? *data->currentQMV : *data->currentMV;

	CHECK_CANDIDATE(centerMV.x, centerMV.y - 1, 0);
	CHECK_CANDIDATE(centerMV.x + 1, centerMV.y - 1, 0);
	CHECK_CANDIDATE(centerMV.x + 1, centerMV.y, 0);
	CHECK_CANDIDATE(centerMV.x + 1, centerMV.y + 1, 0);
	CHECK_CANDIDATE(centerMV.x, centerMV.y + 1, 0);
	CHECK_CANDIDATE(centerMV.x - 1, centerMV.y + 1, 0);
	CHECK_CANDIDATE(centerMV.x - 1, centerMV.y, 0);
	CHECK_CANDIDATE(centerMV.x - 1, centerMV.y - 1, 0);
}

void
SubpelRefine_Fast(SearchData * data, CheckFunc * CheckCandidate)
{
/* Do a fast q-pel refinement */
	VECTOR centerMV;
	VECTOR second_best;
	int best_sad = *data->iMinSAD;
	int xo, yo, xo2, yo2;
	int size = 2;
	data->iMinSAD2 = 0;

	/* check all halfpixel positions near our best halfpel position */
	centerMV = *data->currentQMV;
	*data->iMinSAD = 256 * 4096;

	CHECK_CANDIDATE(centerMV.x, centerMV.y - size, 0);
	CHECK_CANDIDATE(centerMV.x + size, centerMV.y - size, 0);
	CHECK_CANDIDATE(centerMV.x + size, centerMV.y, 0);
	CHECK_CANDIDATE(centerMV.x + size, centerMV.y + size, 0);

	CHECK_CANDIDATE(centerMV.x, centerMV.y + size, 0);
	CHECK_CANDIDATE(centerMV.x - size, centerMV.y + size, 0);
	CHECK_CANDIDATE(centerMV.x - size, centerMV.y, 0);
	CHECK_CANDIDATE(centerMV.x - size, centerMV.y - size, 0);

	second_best = *data->currentQMV;

	/* after second_best has been found, go back to the vector we began with */

	data->currentQMV[0] = centerMV;
	*data->iMinSAD = best_sad;

	xo = centerMV.x;
	yo = centerMV.y;
	xo2 = second_best.x;
	yo2 = second_best.y;

	data->iMinSAD2 = 256 * 4096;

	if (yo == yo2) {
		CHECK_CANDIDATE((xo+xo2)>>1, yo, 0);
		CHECK_CANDIDATE(xo, yo-1, 0);
		CHECK_CANDIDATE(xo, yo+1, 0);

		if(best_sad <= data->iMinSAD2) return;

		if(data->currentQMV[0].x == data->currentQMV2.x) {
			CHECK_CANDIDATE((xo+xo2)>>1, yo-1, 0);
			CHECK_CANDIDATE((xo+xo2)>>1, yo+1, 0);
		} else {
			CHECK_CANDIDATE((xo+xo2)>>1,
				(data->currentQMV[0].x == xo) ? data->currentQMV[0].y : data->currentQMV2.y, 0);
		}
		return;
	}

	if (xo == xo2) {
		CHECK_CANDIDATE(xo, (yo+yo2)>>1, 0);
		CHECK_CANDIDATE(xo-1, yo, 0);
		CHECK_CANDIDATE(xo+1, yo, 0);

		if(best_sad <= data->iMinSAD2) return;

		if(data->currentQMV[0].y == data->currentQMV2.y) {
			CHECK_CANDIDATE(xo-1, (yo+yo2)>>1, 0);
			CHECK_CANDIDATE(xo+1, (yo+yo2)>>1, 0);
		} else {
			CHECK_CANDIDATE((data->currentQMV[0].y == yo) ? data->currentQMV[0].x : data->currentQMV2.x, (yo+yo2)>>1, 0);
		}
		return;
	}

	CHECK_CANDIDATE(xo, (yo+yo2)>>1, 0);
	CHECK_CANDIDATE((xo+xo2)>>1, yo, 0);

	if(best_sad <= data->iMinSAD2) return;

	CHECK_CANDIDATE((xo+xo2)>>1, (yo+yo2)>>1, 0);
}
