/*****************************************************************************
 *
 *  XVID MPEG-4 VIDEO CODEC
 *  - Motion Estimation for B-VOPs  -
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


#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>	/* memcpy */

#include "../encoder.h"
#include "../global.h"
#include "../image/interpolate8x8.h"
#include "estimation.h"
#include "motion.h"
#include "sad.h"
#include "motion_inlines.h"

static int32_t
ChromaSAD2(const int fx, const int fy, const int bx, const int by,
			SearchData * const data)
{
	int sad;
	const uint32_t stride = data->iEdgedWidth/2;
	uint8_t *f_refu, *f_refv, *b_refu, *b_refv;

	const INTERPOLATE8X8_PTR interpolate8x8_halfpel[] = {
		NULL,
		interpolate8x8_halfpel_v,
		interpolate8x8_halfpel_h,
		interpolate8x8_halfpel_hv
	};

	int offset = (fx>>1) + (fy>>1)*stride;
	int filter = ((fx & 1) << 1) | (fy & 1);

	if (filter != 0) {
		f_refu = data->RefQ;
		f_refv = data->RefQ + 8;
		interpolate8x8_halfpel[filter](f_refu, data->RefP[4] + offset, stride, data->rounding);
		interpolate8x8_halfpel[filter](f_refv, data->RefP[5] + offset, stride, data->rounding);
	} else {
		f_refu = (uint8_t*)data->RefP[4] + offset;
		f_refv = (uint8_t*)data->RefP[5] + offset;
	}

	offset = (bx>>1) + (by>>1)*stride;
	filter = ((bx & 1) << 1) | (by & 1);

	if (filter != 0) {
		b_refu = data->RefQ + 16;
		b_refv = data->RefQ + 24;
		interpolate8x8_halfpel[filter](b_refu, data->b_RefP[4] + offset, stride, data->rounding);
		interpolate8x8_halfpel[filter](b_refv, data->b_RefP[5] + offset, stride, data->rounding);
	} else {
		b_refu = (uint8_t*)data->b_RefP[4] + offset;
		b_refv = (uint8_t*)data->b_RefP[5] + offset;
	}

	sad = sad8bi(data->CurU, b_refu, f_refu, stride);
	sad += sad8bi(data->CurV, b_refv, f_refv, stride);

	return sad;
}

static void
CheckCandidateInt(const int x, const int y, SearchData * const data, const unsigned int Direction)
{
	int32_t sad, xf, yf, xb, yb, xcf, ycf, xcb, ycb;
	uint32_t t;
	
	const uint8_t *ReferenceF, *ReferenceB;
	VECTOR *current;

	if ((x > data->max_dx) || (x < data->min_dx) ||
		(y > data->max_dy) || (y < data->min_dy))
		return;

	if (Direction == 1) { /* x and y mean forward vector */
		VECTOR backward = data->qpel_precision ? data->currentQMV[1] : data->currentMV[1];
		xb = backward.x;
		yb = backward.y;
		xf = x; yf = y;
	} else { /* x and y mean backward vector */
		VECTOR forward = data->qpel_precision ? data->currentQMV[0] : data->currentMV[0];
		xf = forward.x;
		yf = forward.y;
		xb = x; yb = y;
	}
	
	if (!data->qpel_precision) {
		ReferenceF = GetReference(xf, yf, data);
		ReferenceB = GetReferenceB(xb, yb, 1, data);
		current = data->currentMV + Direction - 1;
		xcf = xf; ycf = yf;
		xcb = xb; ycb = yb;
	} else {
		ReferenceF = xvid_me_interpolate16x16qpel(xf, yf, 0, data);
		current = data->currentQMV + Direction - 1;
		ReferenceB = xvid_me_interpolate16x16qpel(xb, yb, 1, data);
		xcf = xf/2; ycf = yf/2;
		xcb = xb/2; ycb = yb/2;
	}

	t = d_mv_bits(xf, yf, data->predMV, data->iFcode, data->qpel^data->qpel_precision, 0)
		 + d_mv_bits(xb, yb, data->bpredMV, data->iFcode, data->qpel^data->qpel_precision, 0);

	sad = sad16bi(data->Cur, ReferenceF, ReferenceB, data->iEdgedWidth);
	sad += (data->lambda16 * t * sad)>>10;

	if (data->chroma && sad < *data->iMinSAD)
		sad += ChromaSAD2((xcf >> 1) + roundtab_79[xcf & 0x3],
							(ycf >> 1) + roundtab_79[ycf & 0x3],
							(xcb >> 1) + roundtab_79[xcb & 0x3],
							(ycb >> 1) + roundtab_79[ycb & 0x3], data);

	if (sad < *(data->iMinSAD)) {
		*data->iMinSAD = sad;
		current->x = x; current->y = y;
		data->dir = Direction;
	}
}

static void
CheckCandidateInt_qpel(const int x, const int y, SearchData * const data, const unsigned int Direction)
{
	int32_t sad, xf, yf, xb, yb, xcf, ycf, xcb, ycb;
	uint32_t t;
	
	const uint8_t *ReferenceF, *ReferenceB;
	VECTOR *current;

	if ((x > data->max_dx) || (x < data->min_dx) ||
		(y > data->max_dy) || (y < data->min_dy))
		return;

	if (Direction == 1) { /* x and y mean forward vector */
		VECTOR backward = data->qpel_precision ? data->currentQMV[1] : data->currentMV[1];
		xb = backward.x;
		yb = backward.y;
		xf = x; yf = y;
	} else { /* x and y mean backward vector */
		VECTOR forward = data->qpel_precision ? data->currentQMV[0] : data->currentMV[0];
		xf = forward.x;
		yf = forward.y;
		xb = x; yb = y;
	}
	
	ReferenceF = xvid_me_interpolate16x16qpel(xf, yf, 0, data);
	current = data->currentQMV + Direction - 1;
	ReferenceB = xvid_me_interpolate16x16qpel(xb, yb, 1, data);
	xcf = xf/2; ycf = yf/2;
	xcb = xb/2; ycb = yb/2;

	t = d_mv_bits(xf, yf, data->predMV, data->iFcode, data->qpel^data->qpel_precision, 0)
		 + d_mv_bits(xb, yb, data->bpredMV, data->iFcode, data->qpel^data->qpel_precision, 0);

	sad = sad16bi(data->Cur, ReferenceF, ReferenceB, data->iEdgedWidth);
	sad += (data->lambda16 * t * sad)>>10;

	if (data->chroma && sad < *data->iMinSAD)
		sad += ChromaSAD2((xcf >> 1) + roundtab_79[xcf & 0x3],
							(ycf >> 1) + roundtab_79[ycf & 0x3],
							(xcb >> 1) + roundtab_79[xcb & 0x3],
							(ycb >> 1) + roundtab_79[ycb & 0x3], data);

	if (sad < *(data->iMinSAD)) {
		*data->iMinSAD = sad;
		current->x = x; current->y = y;
		data->dir = Direction;
	}

	if (sad < *(data->iMinSAD)) {
		data->iMinSAD2 = *(data->iMinSAD);
		data->currentQMV2.x = current->x;
		data->currentQMV2.y = current->y;

		*data->iMinSAD = sad;
		current->x = x; current->y = y;
	} else if (sad < data->iMinSAD2) {
		data->iMinSAD2 = sad;
		data->currentQMV2.x = x; data->currentQMV2.y = y;
	}
}

static void
CheckCandidateDirect(const int x, const int y, SearchData * const data, const unsigned int Direction)
{
	int32_t sad = 0, xcf = 0, ycf = 0, xcb = 0, ycb = 0;
	uint32_t k;
	const uint8_t *ReferenceF;
	const uint8_t *ReferenceB;
	VECTOR mvs, b_mvs;

	if (( x > 31) || ( x < -32) || ( y > 31) || (y < -32)) return;

	for (k = 0; k < 4; k++) {
		mvs.x = data->directmvF[k].x + x;
		b_mvs.x = ((x == 0) ?
			data->directmvB[k].x
			: mvs.x - data->referencemv[k].x);

		mvs.y = data->directmvF[k].y + y;
		b_mvs.y = ((y == 0) ?
			data->directmvB[k].y
			: mvs.y - data->referencemv[k].y);

		if ((mvs.x > data->max_dx)   || (mvs.x < data->min_dx)   ||
			(mvs.y > data->max_dy)   || (mvs.y < data->min_dy)   ||
			(b_mvs.x > data->max_dx) || (b_mvs.x < data->min_dx) ||
			(b_mvs.y > data->max_dy) || (b_mvs.y < data->min_dy) )
			return;

		if (data->qpel) {
			xcf += mvs.x/2; ycf += mvs.y/2;
			xcb += b_mvs.x/2; ycb += b_mvs.y/2;
		} else {
			xcf += mvs.x; ycf += mvs.y;
			xcb += b_mvs.x; ycb += b_mvs.y;
			mvs.x *= 2; mvs.y *= 2; /* we move to qpel precision anyway */
			b_mvs.x *= 2; b_mvs.y *= 2;
		}

		ReferenceF = xvid_me_interpolate8x8qpel(mvs.x, mvs.y, k, 0, data);
		ReferenceB = xvid_me_interpolate8x8qpel(b_mvs.x, b_mvs.y, k, 1, data);

		sad += sad8bi(data->Cur + 8*(k&1) + 8*(k>>1)*(data->iEdgedWidth),
						ReferenceF, ReferenceB, data->iEdgedWidth);
		if (sad > *(data->iMinSAD)) return;
	}

	sad += (data->lambda16 * d_mv_bits(x, y, zeroMV, 1, 0, 0) * sad)>>10;

	if (data->chroma && sad < *data->iMinSAD)
		sad += ChromaSAD2((xcf >> 3) + roundtab_76[xcf & 0xf],
							(ycf >> 3) + roundtab_76[ycf & 0xf],
							(xcb >> 3) + roundtab_76[xcb & 0xf],
							(ycb >> 3) + roundtab_76[ycb & 0xf], data);

	if (sad < *(data->iMinSAD)) {
		data->iMinSAD[0] = sad;
		data->currentMV->x = x; data->currentMV->y = y;
		data->dir = Direction;
	}
}

static void
CheckCandidateDirectno4v(const int x, const int y, SearchData * const data, const unsigned int Direction)
{
	int32_t sad, xcf, ycf, xcb, ycb;
	const uint8_t *ReferenceF;
	const uint8_t *ReferenceB;
	VECTOR mvs, b_mvs;

	if (( x > 31) || ( x < -32) || ( y > 31) || (y < -32)) return;

	mvs.x = data->directmvF[0].x + x;
	b_mvs.x = ((x == 0) ?
		data->directmvB[0].x
		: mvs.x - data->referencemv[0].x);

	mvs.y = data->directmvF[0].y + y;
	b_mvs.y = ((y == 0) ?
		data->directmvB[0].y
		: mvs.y - data->referencemv[0].y);

	if ( (mvs.x > data->max_dx) || (mvs.x < data->min_dx)
		|| (mvs.y > data->max_dy) || (mvs.y < data->min_dy)
		|| (b_mvs.x > data->max_dx) || (b_mvs.x < data->min_dx)
		|| (b_mvs.y > data->max_dy) || (b_mvs.y < data->min_dy) ) return;

	if (data->qpel) {
		xcf = 4*(mvs.x/2); ycf = 4*(mvs.y/2);
		xcb = 4*(b_mvs.x/2); ycb = 4*(b_mvs.y/2);
		ReferenceF = xvid_me_interpolate16x16qpel(mvs.x, mvs.y, 0, data);
		ReferenceB = xvid_me_interpolate16x16qpel(b_mvs.x, b_mvs.y, 1, data);
	} else {
		xcf = 4*mvs.x; ycf = 4*mvs.y;
		xcb = 4*b_mvs.x; ycb = 4*b_mvs.y;
		ReferenceF = GetReference(mvs.x, mvs.y, data);
		ReferenceB = GetReferenceB(b_mvs.x, b_mvs.y, 1, data);
	}

	sad = sad16bi(data->Cur, ReferenceF, ReferenceB, data->iEdgedWidth);
	sad += (data->lambda16 * d_mv_bits(x, y, zeroMV, 1, 0, 0) * sad)>>10;

	if (data->chroma && sad < *data->iMinSAD)
		sad += ChromaSAD2((xcf >> 3) + roundtab_76[xcf & 0xf],
							(ycf >> 3) + roundtab_76[ycf & 0xf],
							(xcb >> 3) + roundtab_76[xcb & 0xf],
							(ycb >> 3) + roundtab_76[ycb & 0xf], data);

	if (sad < *(data->iMinSAD)) {
		*(data->iMinSAD) = sad;
		data->currentMV->x = x; data->currentMV->y = y;
		data->dir = Direction;
	}
}

void
CheckCandidate16no4v(const int x, const int y, SearchData * const data, const unsigned int Direction)
{
	int32_t sad, xc, yc;
	const uint8_t * Reference;
	uint32_t t;
	VECTOR * current;

	if ( (x > data->max_dx) || ( x < data->min_dx)
		|| (y > data->max_dy) || (y < data->min_dy) ) return;

	if (data->rrv && (!(x&1) && x !=0) | (!(y&1) && y !=0) ) return; /* non-zero even value */

	if (data->qpel_precision) { /* x and y are in 1/4 precision */
		Reference = xvid_me_interpolate16x16qpel(x, y, 0, data);
		current = data->currentQMV;
		xc = x/2; yc = y/2;
	} else {
		Reference = GetReference(x, y, data);
		current = data->currentMV;
		xc = x; yc = y;
	}
	t = d_mv_bits(x, y, data->predMV, data->iFcode,
					data->qpel^data->qpel_precision, data->rrv);

	sad = sad16(data->Cur, Reference, data->iEdgedWidth, 256*4096);
	sad += (data->lambda16 * t * sad)>>10;

	if (data->chroma && sad < *data->iMinSAD)
		sad += xvid_me_ChromaSAD((xc >> 1) + roundtab_79[xc & 0x3],
								(yc >> 1) + roundtab_79[yc & 0x3], data);

	if (sad < *(data->iMinSAD)) {
		*(data->iMinSAD) = sad;
		current->x = x; current->y = y;
		data->dir = Direction;
	}
}

void
CheckCandidate16no4v_qpel(const int x, const int y, SearchData * const data, const unsigned int Direction)
{
	int32_t sad, xc, yc;
	const uint8_t * Reference;
	uint32_t t;

	if ( (x > data->max_dx) || ( x < data->min_dx)
		|| (y > data->max_dy) || (y < data->min_dy) ) return;

	if (data->rrv && (!(x&1) && x !=0) | (!(y&1) && y !=0) ) return; /* non-zero even value */

	Reference = xvid_me_interpolate16x16qpel(x, y, 0, data);

	xc = x/2; yc = y/2;
	t = d_mv_bits(x, y, data->predMV, data->iFcode,
					data->qpel^data->qpel_precision, data->rrv);

	sad = sad16(data->Cur, Reference, data->iEdgedWidth, 256*4096);
	sad += (data->lambda16 * t * sad)>>10;

	if (data->chroma && sad < *data->iMinSAD)
		sad += xvid_me_ChromaSAD((xc >> 1) + roundtab_79[xc & 0x3],
								(yc >> 1) + roundtab_79[yc & 0x3], data);

	if (sad < *(data->iMinSAD)) {
		data->iMinSAD2 = *(data->iMinSAD);
		data->currentQMV2.x = data->currentQMV->x;
		data->currentQMV2.y = data->currentQMV->y;

		data->iMinSAD[0] = sad;
		data->currentQMV[0].x = x; data->currentQMV[0].y = y;
	} else if (sad < data->iMinSAD2) {
		data->iMinSAD2 = sad;
		data->currentQMV2.x = x; data->currentQMV2.y = y;
	}
}

static __inline VECTOR
ChoosePred(const MACROBLOCK * const pMB, const uint32_t mode)
{
/* the stupidiest function ever */
	return (mode == MODE_FORWARD ? pMB->mvs[0] : pMB->b_mvs[0]);
}

static void __inline
PreparePredictionsBF(VECTOR * const pmv, const int x, const int y,
							const uint32_t iWcount,
							const MACROBLOCK * const pMB,
							const uint32_t mode_curr)
{

	/* [0] is prediction */
	pmv[0].x = EVEN(pmv[0].x); pmv[0].y = EVEN(pmv[0].y);

	pmv[1].x = pmv[1].y = 0; /* [1] is zero */

	pmv[2] = ChoosePred(pMB, mode_curr);
	pmv[2].x = EVEN(pmv[2].x); pmv[2].y = EVEN(pmv[2].y);

	if ((y != 0)&&(x != (int)(iWcount+1))) {			/* [3] top-right neighbour */
		pmv[3] = ChoosePred(pMB+1-iWcount, mode_curr);
		pmv[3].x = EVEN(pmv[3].x); pmv[3].y = EVEN(pmv[3].y);
	} else pmv[3].x = pmv[3].y = 0;

	if (y != 0) {
		pmv[4] = ChoosePred(pMB-iWcount, mode_curr);
		pmv[4].x = EVEN(pmv[4].x); pmv[4].y = EVEN(pmv[4].y);
	} else pmv[4].x = pmv[4].y = 0;

	if (x != 0) {
		pmv[5] = ChoosePred(pMB-1, mode_curr);
		pmv[5].x = EVEN(pmv[5].x); pmv[5].y = EVEN(pmv[5].y);
	} else pmv[5].x = pmv[5].y = 0;

	if (x != 0 && y != 0) {
		pmv[6] = ChoosePred(pMB-1-iWcount, mode_curr);
		pmv[6].x = EVEN(pmv[6].x); pmv[6].y = EVEN(pmv[6].y);
	} else pmv[6].x = pmv[6].y = 0;
}


/* search backward or forward */
static void
SearchBF(	const IMAGE * const pRef,
			const uint8_t * const pRefH,
			const uint8_t * const pRefV,
			const uint8_t * const pRefHV,
			const int x, const int y,
			const uint32_t MotionFlags,
			const uint32_t iFcode,
			const MBParam * const pParam,
			MACROBLOCK * const pMB,
			const VECTOR * const predMV,
			int32_t * const best_sad,
			const int32_t mode_current,
			SearchData * const Data)
{

	int i;
	VECTOR pmv[7];
	int threshA = (MotionFlags & XVID_ME_FASTREFINE16) ? 150 : 300;
	*Data->iMinSAD = MV_MAX_ERROR;
	Data->iFcode = iFcode;
	Data->qpel_precision = 0;
	Data->chromaX = Data->chromaY = Data->chromaSAD = 256*4096; /* reset chroma-sad cache */

	Data->RefP[0] = pRef->y + (x + Data->iEdgedWidth*y) * 16;
	Data->RefP[2] = pRefH + (x + Data->iEdgedWidth*y) * 16;
	Data->RefP[1] = pRefV + (x + Data->iEdgedWidth*y) * 16;
	Data->RefP[3] = pRefHV + (x + Data->iEdgedWidth*y) * 16;
	Data->RefP[4] = pRef->u + (x + y * (Data->iEdgedWidth/2)) * 8;
	Data->RefP[5] = pRef->v + (x + y * (Data->iEdgedWidth/2)) * 8;

	Data->predMV = *predMV;

	get_range(&Data->min_dx, &Data->max_dx, &Data->min_dy, &Data->max_dy, x, y, 4,
				pParam->width, pParam->height, iFcode - Data->qpel, 1, 0);

	pmv[0] = Data->predMV;
	if (Data->qpel) { pmv[0].x /= 2; pmv[0].y /= 2; }

	PreparePredictionsBF(pmv, x, y, pParam->mb_width, pMB, mode_current);

	Data->currentMV->x = Data->currentMV->y = 0;

	/* main loop. checking all predictions */
	for (i = 0; i < 7; i++)
		if (!vector_repeats(pmv, i) )
			CheckCandidate16no4v(pmv[i].x, pmv[i].y, Data, i);

	if (*Data->iMinSAD > 512) {
		unsigned int mask = make_mask(pmv, 7, Data->dir);

		MainSearchFunc *MainSearchPtr;
		if (MotionFlags & XVID_ME_USESQUARES16) MainSearchPtr = xvid_me_SquareSearch;
		else if (MotionFlags & XVID_ME_ADVANCEDDIAMOND16) MainSearchPtr = xvid_me_AdvDiamondSearch;
		else MainSearchPtr = xvid_me_DiamondSearch;
		
		MainSearchPtr(Data->currentMV->x, Data->currentMV->y, Data, mask, CheckCandidate16no4v);
	}

	xvid_me_SubpelRefine(Data, CheckCandidate16no4v);

	if (Data->qpel && (*Data->iMinSAD < *best_sad + threshA)) {
		Data->currentQMV->x = 2*Data->currentMV->x;
		Data->currentQMV->y = 2*Data->currentMV->y;
		Data->qpel_precision = 1;
		get_range(&Data->min_dx, &Data->max_dx, &Data->min_dy, &Data->max_dy, x, y, 4,
					pParam->width, pParam->height, iFcode, 2, 0);

		if (MotionFlags & XVID_ME_QUARTERPELREFINE16) { 
			if (MotionFlags & XVID_ME_FASTREFINE16)
				SubpelRefine_Fast(Data, CheckCandidate16no4v_qpel);
			else
				xvid_me_SubpelRefine(Data, CheckCandidate16no4v);
		}
	}

	/* three bits are needed to code backward mode. four for forward */

	if (mode_current == MODE_FORWARD) *Data->iMinSAD += 4 * Data->lambda16;
	else *Data->iMinSAD += 3 * Data->lambda16;

	if (*Data->iMinSAD < *best_sad) {
		*best_sad = *Data->iMinSAD;
		pMB->mode = mode_current;
		if (Data->qpel) {
			pMB->pmvs[0].x = Data->currentQMV->x - predMV->x;
			pMB->pmvs[0].y = Data->currentQMV->y - predMV->y;
			if (mode_current == MODE_FORWARD)
				pMB->qmvs[0] = *Data->currentQMV;
			else
				pMB->b_qmvs[0] = *Data->currentQMV;
		} else {
			pMB->pmvs[0].x = Data->currentMV->x - predMV->x;
			pMB->pmvs[0].y = Data->currentMV->y - predMV->y;
		}
		if (mode_current == MODE_FORWARD) pMB->mvs[0] = *Data->currentMV;
		else pMB->b_mvs[0] = *Data->currentMV;
	}

	if (mode_current == MODE_FORWARD) *(Data->currentMV+2) = *Data->currentMV;
	else *(Data->currentMV+1) = *Data->currentMV; /* we store currmv for interpolate search */
}

static void
SkipDecisionB(const IMAGE * const pCur,
				const IMAGE * const f_Ref,
				const IMAGE * const b_Ref,
				MACROBLOCK * const pMB,
				const uint32_t x, const uint32_t y,
				const SearchData * const Data)
{
	int k;

	if (!Data->chroma) {
		int dx = 0, dy = 0, b_dx = 0, b_dy = 0;
		int32_t sum;
		const uint32_t stride = Data->iEdgedWidth/2;
		/* this is not full chroma compensation, only it's fullpel approximation. should work though */

		for (k = 0; k < 4; k++) {
			dy += Data->directmvF[k].y >> Data->qpel;
			dx += Data->directmvF[k].x >> Data->qpel;
			b_dy += Data->directmvB[k].y >> Data->qpel;
			b_dx += Data->directmvB[k].x >> Data->qpel;
		}

		dy = (dy >> 3) + roundtab_76[dy & 0xf];
		dx = (dx >> 3) + roundtab_76[dx & 0xf];
		b_dy = (b_dy >> 3) + roundtab_76[b_dy & 0xf];
		b_dx = (b_dx >> 3) + roundtab_76[b_dx & 0xf];

		sum = sad8bi(pCur->u + 8 * x + 8 * y * stride,
						f_Ref->u + (y*8 + dy/2) * stride + x*8 + dx/2,
						b_Ref->u + (y*8 + b_dy/2) * stride + x*8 + b_dx/2,
						stride);

		if (sum >= MAX_CHROMA_SAD_FOR_SKIP * (int)Data->iQuant) return; /* no skip */

		sum += sad8bi(pCur->v + 8*x + 8 * y * stride,
						f_Ref->v + (y*8 + dy/2) * stride + x*8 + dx/2,
						b_Ref->v + (y*8 + b_dy/2) * stride + x*8 + b_dx/2,
						stride);
		
		if (sum >= MAX_CHROMA_SAD_FOR_SKIP * (int)Data->iQuant) return; /* no skip */
	}

	/* skip */
	pMB->mode = MODE_DIRECT_NONE_MV; /* skipped */
	for (k = 0; k < 4; k++) {
		pMB->qmvs[k] = pMB->mvs[k] = Data->directmvF[k];
		pMB->b_qmvs[k] = pMB->b_mvs[k] =  Data->directmvB[k];
	}
}

static uint32_t
SearchDirect(const IMAGE * const f_Ref,
				const uint8_t * const f_RefH,
				const uint8_t * const f_RefV,
				const uint8_t * const f_RefHV,
				const IMAGE * const b_Ref,
				const uint8_t * const b_RefH,
				const uint8_t * const b_RefV,
				const uint8_t * const b_RefHV,
				const IMAGE * const pCur,
				const int x, const int y,
				const uint32_t MotionFlags,
				const int32_t TRB, const int32_t TRD,
				const MBParam * const pParam,
				MACROBLOCK * const pMB,
				const MACROBLOCK * const b_mb,
				int32_t * const best_sad,
				SearchData * const Data)

{
	int32_t skip_sad;
	int k = (x + Data->iEdgedWidth*y) * 16;
	MainSearchFunc *MainSearchPtr;
	CheckFunc * CheckCandidate;

	*Data->iMinSAD = 256*4096;
	Data->RefP[0] = f_Ref->y + k;
	Data->RefP[2] = f_RefH + k;
	Data->RefP[1] = f_RefV + k;
	Data->RefP[3] = f_RefHV + k;
	Data->b_RefP[0] = b_Ref->y + k;
	Data->b_RefP[2] = b_RefH + k;
	Data->b_RefP[1] = b_RefV + k;
	Data->b_RefP[3] = b_RefHV + k;
	Data->RefP[4] = f_Ref->u + (x + (Data->iEdgedWidth/2) * y) * 8;
	Data->RefP[5] = f_Ref->v + (x + (Data->iEdgedWidth/2) * y) * 8;
	Data->b_RefP[4] = b_Ref->u + (x + (Data->iEdgedWidth/2) * y) * 8;
	Data->b_RefP[5] = b_Ref->v + (x + (Data->iEdgedWidth/2) * y) * 8;

	k = Data->qpel ? 4 : 2;
	Data->max_dx = k * (pParam->width - x * 16);
	Data->max_dy = k * (pParam->height - y * 16);
	Data->min_dx = -k * (16 + x * 16);
	Data->min_dy = -k * (16 + y * 16);

	Data->referencemv = Data->qpel ? b_mb->qmvs : b_mb->mvs;
	Data->qpel_precision = 0;

	for (k = 0; k < 4; k++) {
		pMB->mvs[k].x = Data->directmvF[k].x = ((TRB * Data->referencemv[k].x) / TRD);
		pMB->b_mvs[k].x = Data->directmvB[k].x = ((TRB - TRD) * Data->referencemv[k].x) / TRD;
		pMB->mvs[k].y = Data->directmvF[k].y = ((TRB * Data->referencemv[k].y) / TRD);
		pMB->b_mvs[k].y = Data->directmvB[k].y = ((TRB - TRD) * Data->referencemv[k].y) / TRD;

		if ( (pMB->b_mvs[k].x > Data->max_dx) | (pMB->b_mvs[k].x < Data->min_dx)
			| (pMB->b_mvs[k].y > Data->max_dy) | (pMB->b_mvs[k].y < Data->min_dy) ) {

			*best_sad = 256*4096; /* in that case, we won't use direct mode */
			pMB->mode = MODE_DIRECT; /* just to make sure it doesn't say "MODE_DIRECT_NONE_MV" */
			pMB->b_mvs[0].x = pMB->b_mvs[0].y = 0;
			return 256*4096;
		}
		if (b_mb->mode != MODE_INTER4V) {
			pMB->mvs[1] = pMB->mvs[2] = pMB->mvs[3] = pMB->mvs[0];
			pMB->b_mvs[1] = pMB->b_mvs[2] = pMB->b_mvs[3] = pMB->b_mvs[0];
			Data->directmvF[1] = Data->directmvF[2] = Data->directmvF[3] = Data->directmvF[0];
			Data->directmvB[1] = Data->directmvB[2] = Data->directmvB[3] = Data->directmvB[0];
			break;
		}
	}

	CheckCandidate = b_mb->mode == MODE_INTER4V ? CheckCandidateDirect : CheckCandidateDirectno4v;

	CheckCandidate(0, 0, Data, 255);

	/* initial (fast) skip decision */
	if (*Data->iMinSAD < (int)Data->iQuant * INITIAL_SKIP_THRESH) {
		/* possible skip */
		SkipDecisionB(pCur, f_Ref, b_Ref, pMB, x, y, Data);
		if (pMB->mode == MODE_DIRECT_NONE_MV) return *Data->iMinSAD; /* skipped */
	}

	skip_sad = *Data->iMinSAD;

	if (!(MotionFlags & XVID_ME_SKIP_DELTASEARCH)) {
		if (MotionFlags & XVID_ME_USESQUARES16) MainSearchPtr = xvid_me_SquareSearch;
			else if (MotionFlags & XVID_ME_ADVANCEDDIAMOND16) MainSearchPtr = xvid_me_AdvDiamondSearch;
				else MainSearchPtr = xvid_me_DiamondSearch;

		MainSearchPtr(0, 0, Data, 255, CheckCandidate);

		xvid_me_SubpelRefine(Data, CheckCandidate);
	}

	*Data->iMinSAD += Data->lambda16;
	*best_sad = *Data->iMinSAD;

	if (Data->qpel || b_mb->mode == MODE_INTER4V) pMB->mode = MODE_DIRECT;
	else pMB->mode = MODE_DIRECT_NO4V; /* for faster compensation */

	pMB->pmvs[3] = *Data->currentMV;

	for (k = 0; k < 4; k++) {
		pMB->mvs[k].x = Data->directmvF[k].x + Data->currentMV->x;
		pMB->b_mvs[k].x = (	(Data->currentMV->x == 0)
							? Data->directmvB[k].x
							:pMB->mvs[k].x - Data->referencemv[k].x);
		pMB->mvs[k].y = (Data->directmvF[k].y + Data->currentMV->y);
		pMB->b_mvs[k].y = ((Data->currentMV->y == 0)
							? Data->directmvB[k].y
							: pMB->mvs[k].y - Data->referencemv[k].y);
		if (Data->qpel) {
			pMB->qmvs[k].x = pMB->mvs[k].x; pMB->mvs[k].x /= 2;
			pMB->b_qmvs[k].x = pMB->b_mvs[k].x; pMB->b_mvs[k].x /= 2;
			pMB->qmvs[k].y = pMB->mvs[k].y; pMB->mvs[k].y /= 2;
			pMB->b_qmvs[k].y = pMB->b_mvs[k].y; pMB->b_mvs[k].y /= 2;
		}

		if (b_mb->mode != MODE_INTER4V) {
			pMB->mvs[3] = pMB->mvs[2] = pMB->mvs[1] = pMB->mvs[0];
			pMB->b_mvs[3] = pMB->b_mvs[2] = pMB->b_mvs[1] = pMB->b_mvs[0];
			pMB->qmvs[3] = pMB->qmvs[2] = pMB->qmvs[1] = pMB->qmvs[0];
			pMB->b_qmvs[3] = pMB->b_qmvs[2] = pMB->b_qmvs[1] = pMB->b_qmvs[0];
			break;
		}
	}
	return skip_sad;
}


static void set_range(int * range, SearchData * Data)
{
	Data->min_dx = range[0];
	Data->max_dx = range[1];
	Data->min_dy = range[2];
	Data->max_dy = range[3];
}

static void
SubpelRefine_dir(SearchData * const data, CheckFunc * const CheckCandidate, const int dir)
{
/* Do a half-pel or q-pel refinement */
	const VECTOR centerMV = data->qpel_precision ? 
		data->currentQMV[dir-1] : data->currentMV[dir-1];

	CHECK_CANDIDATE(centerMV.x, centerMV.y - 1, dir);
	CHECK_CANDIDATE(centerMV.x + 1, centerMV.y - 1, dir);
	CHECK_CANDIDATE(centerMV.x + 1, centerMV.y, dir);
	CHECK_CANDIDATE(centerMV.x + 1, centerMV.y + 1, dir);
	CHECK_CANDIDATE(centerMV.x, centerMV.y + 1, dir);
	CHECK_CANDIDATE(centerMV.x - 1, centerMV.y + 1, dir);
	CHECK_CANDIDATE(centerMV.x - 1, centerMV.y, dir);
	CHECK_CANDIDATE(centerMV.x - 1, centerMV.y - 1, dir);
}

/* Pretty much redundant code, just as SubpelRefine_dir above too
 *
 * TODO: Get rid off all the redundancy (SubpelRefine_Fast_dir, 
 *       CheckCandidate16no4v_qpel etc.)                          */

void
SubpelRefine_Fast_dir(SearchData * data, CheckFunc * CheckCandidate, const int dir)
{
/* Do a fast q-pel refinement */
	VECTOR centerMV;
	VECTOR second_best;
	int best_sad = *data->iMinSAD;
	int xo, yo, xo2, yo2;
	int size = 2;
	data->iMinSAD2 = 0;

	/* check all halfpixel positions near our best halfpel position */
	centerMV = data->currentQMV[dir-1];
	*data->iMinSAD = 256 * 4096;

	CHECK_CANDIDATE(centerMV.x, centerMV.y - size, dir);
	CHECK_CANDIDATE(centerMV.x + size, centerMV.y - size, dir);
	CHECK_CANDIDATE(centerMV.x + size, centerMV.y, dir);
	CHECK_CANDIDATE(centerMV.x + size, centerMV.y + size, dir);

	CHECK_CANDIDATE(centerMV.x, centerMV.y + size, dir);
	CHECK_CANDIDATE(centerMV.x - size, centerMV.y + size, dir);
	CHECK_CANDIDATE(centerMV.x - size, centerMV.y, dir);
	CHECK_CANDIDATE(centerMV.x - size, centerMV.y - size, dir);

	second_best = data->currentQMV[dir-1];

	/* after second_best has been found, go back to the vector we began with */

	data->currentQMV[dir-1] = centerMV;
	*data->iMinSAD = best_sad;

	xo = centerMV.x;
	yo = centerMV.y;
	xo2 = second_best.x;
	yo2 = second_best.y;

	data->iMinSAD2 = 256 * 4096;

	if (yo == yo2) {
		CHECK_CANDIDATE((xo+xo2)>>1, yo, dir);
		CHECK_CANDIDATE(xo, yo-1, dir);
		CHECK_CANDIDATE(xo, yo+1, dir);

		if(best_sad <= data->iMinSAD2) return;

		if(data->currentQMV[dir-1].x == data->currentQMV2.x) {
			CHECK_CANDIDATE((xo+xo2)>>1, yo-1, dir);
			CHECK_CANDIDATE((xo+xo2)>>1, yo+1, dir);
		} else {
			CHECK_CANDIDATE((xo+xo2)>>1,
				(data->currentQMV[dir-1].x == xo) ? data->currentQMV[dir-1].y : data->currentQMV2.y, dir);
		}
		return;
	}

	if (xo == xo2) {
		CHECK_CANDIDATE(xo, (yo+yo2)>>1, dir);
		CHECK_CANDIDATE(xo-1, yo, dir);
		CHECK_CANDIDATE(xo+1, yo, dir);

		if(best_sad < data->iMinSAD2) return;

		if(data->currentQMV[dir-1].y == data->currentQMV2.y) {
			CHECK_CANDIDATE(xo-1, (yo+yo2)>>1, dir);
			CHECK_CANDIDATE(xo+1, (yo+yo2)>>1, dir);
		} else {
			CHECK_CANDIDATE((data->currentQMV[dir-1].y == yo) ? data->currentQMV[dir-1].x : data->currentQMV2.x, (yo+yo2)>>1, dir);
		}
		return;
	}

	CHECK_CANDIDATE(xo, (yo+yo2)>>1, dir);
	CHECK_CANDIDATE((xo+xo2)>>1, yo, dir);

	if(best_sad <= data->iMinSAD2) return;

	CHECK_CANDIDATE((xo+xo2)>>1, (yo+yo2)>>1, dir);
}

static void
SearchInterpolate(const IMAGE * const f_Ref,
				const uint8_t * const f_RefH,
				const uint8_t * const f_RefV,
				const uint8_t * const f_RefHV,
				const IMAGE * const b_Ref,
				const uint8_t * const b_RefH,
				const uint8_t * const b_RefV,
				const uint8_t * const b_RefHV,
				const int x, const int y,
				const uint32_t fcode,
				const uint32_t bcode,
				const uint32_t MotionFlags,
				const MBParam * const pParam,
				const VECTOR * const f_predMV,
				const VECTOR * const b_predMV,
				MACROBLOCK * const pMB,
				int32_t * const best_sad,
				SearchData * const Data)

{
	int i, j;
	int b_range[4], f_range[4];
	int threshA = (MotionFlags & XVID_ME_FAST_MODEINTERPOLATE) ? 250 : 500;
	int threshB = (MotionFlags & XVID_ME_FAST_MODEINTERPOLATE) ? 150 : 300;

	Data->qpel_precision = 0;
	*Data->iMinSAD = 4096*256;
	Data->iFcode = fcode; Data->bFcode = bcode;

	i = (x + y * Data->iEdgedWidth) * 16;

	Data->RefP[0] = f_Ref->y + i;
	Data->RefP[2] = f_RefH + i;
	Data->RefP[1] = f_RefV + i;
	Data->RefP[3] = f_RefHV + i;
	Data->b_RefP[0] = b_Ref->y + i;
	Data->b_RefP[2] = b_RefH + i;
	Data->b_RefP[1] = b_RefV + i;
	Data->b_RefP[3] = b_RefHV + i;
	Data->RefP[4] = f_Ref->u + (x + (Data->iEdgedWidth/2) * y) * 8;
	Data->RefP[5] = f_Ref->v + (x + (Data->iEdgedWidth/2) * y) * 8;
	Data->b_RefP[4] = b_Ref->u + (x + (Data->iEdgedWidth/2) * y) * 8;
	Data->b_RefP[5] = b_Ref->v + (x + (Data->iEdgedWidth/2) * y) * 8;

	Data->predMV = *f_predMV;
	Data->bpredMV = *b_predMV;

	Data->currentMV[0] = Data->currentMV[2]; /* forward search left its vector here */

	get_range(f_range, f_range+1, f_range+2, f_range+3, x, y, 4, pParam->width, pParam->height, fcode - Data->qpel, 1, 0);
	get_range(b_range, b_range+1, b_range+2, b_range+3, x, y, 4, pParam->width, pParam->height, bcode - Data->qpel, 1, 0);

	if (Data->currentMV[0].x > f_range[1]) Data->currentMV[0].x = f_range[1];
	if (Data->currentMV[0].x < f_range[0]) Data->currentMV[0].x = f_range[0];
	if (Data->currentMV[0].y > f_range[3]) Data->currentMV[0].y = f_range[3];
	if (Data->currentMV[0].y < f_range[2]) Data->currentMV[0].y = f_range[2];

	if (Data->currentMV[1].x > b_range[1]) Data->currentMV[1].x = b_range[1];
	if (Data->currentMV[1].x < b_range[0]) Data->currentMV[1].x = b_range[0];
	if (Data->currentMV[1].y > b_range[3]) Data->currentMV[1].y = b_range[3];
	if (Data->currentMV[1].y < b_range[2]) Data->currentMV[1].y = b_range[2];

	set_range(f_range, Data);

	CheckCandidateInt(Data->currentMV[0].x, Data->currentMV[0].y, Data, 1);

	/* diamond */
	do {
		Data->dir = 0;
		/* forward MV moves */
		i = Data->currentMV[0].x; j = Data->currentMV[0].y;

		CheckCandidateInt(i + 1, j, Data, 1);
		CheckCandidateInt(i, j + 1, Data, 1);
		CheckCandidateInt(i - 1, j, Data, 1);
		CheckCandidateInt(i, j - 1, Data, 1);

		/* backward MV moves */
		set_range(b_range, Data);
		i = Data->currentMV[1].x; j = Data->currentMV[1].y;

		CheckCandidateInt(i + 1, j, Data, 2);
		CheckCandidateInt(i, j + 1, Data, 2);
		CheckCandidateInt(i - 1, j, Data, 2);
		CheckCandidateInt(i, j - 1, Data, 2);

		set_range(f_range, Data);

	} while (Data->dir != 0);

	/* qpel refinement */
	if (Data->qpel) {
		if (*Data->iMinSAD > *best_sad + threshA) return;
		Data->qpel_precision = 1;
		get_range(&Data->min_dx, &Data->max_dx, &Data->min_dy, &Data->max_dy, x, y, 4, pParam->width, pParam->height, fcode, 2, 0);
		
		Data->currentQMV[0].x = 2 * Data->currentMV[0].x;
		Data->currentQMV[0].y = 2 * Data->currentMV[0].y;
		Data->currentQMV[1].x = 2 * Data->currentMV[1].x;
		Data->currentQMV[1].y = 2 * Data->currentMV[1].y;

		if (MotionFlags & XVID_ME_QUARTERPELREFINE16) { 
			if (MotionFlags & XVID_ME_FASTREFINE16)
				SubpelRefine_Fast_dir(Data, CheckCandidateInt_qpel, 1);
			else
				SubpelRefine_dir(Data, CheckCandidateInt, 1);
		}

		if (*Data->iMinSAD > *best_sad + threshB) return;
		get_range(&Data->min_dx, &Data->max_dx, &Data->min_dy, &Data->max_dy, x, y, 4, pParam->width, pParam->height, bcode, 2, 0);

		if (MotionFlags & XVID_ME_QUARTERPELREFINE16) { 
			if (MotionFlags & XVID_ME_FASTREFINE16)
				SubpelRefine_Fast_dir(Data, CheckCandidateInt_qpel, 2);
			else
				SubpelRefine_dir(Data, CheckCandidateInt, 2);
		}
	}

	*Data->iMinSAD += 2 * Data->lambda16; /* two bits are needed to code interpolate mode. */

	if (*Data->iMinSAD < *best_sad) {
		*best_sad = *Data->iMinSAD;
		pMB->mvs[0] = Data->currentMV[0];
		pMB->b_mvs[0] = Data->currentMV[1];
		pMB->mode = MODE_INTERPOLATE;
		if (Data->qpel) {
			pMB->qmvs[0] = Data->currentQMV[0];
			pMB->b_qmvs[0] = Data->currentQMV[1];
			pMB->pmvs[1].x = pMB->qmvs[0].x - f_predMV->x;
			pMB->pmvs[1].y = pMB->qmvs[0].y - f_predMV->y;
			pMB->pmvs[0].x = pMB->b_qmvs[0].x - b_predMV->x;
			pMB->pmvs[0].y = pMB->b_qmvs[0].y - b_predMV->y;
		} else {
			pMB->pmvs[1].x = pMB->mvs[0].x - f_predMV->x;
			pMB->pmvs[1].y = pMB->mvs[0].y - f_predMV->y;
			pMB->pmvs[0].x = pMB->b_mvs[0].x - b_predMV->x;
			pMB->pmvs[0].y = pMB->b_mvs[0].y - b_predMV->y;
		}
	}
}

void
MotionEstimationBVOP(MBParam * const pParam,
					 FRAMEINFO * const frame,
					 const int32_t time_bp,
					 const int32_t time_pp,
					 /* forward (past) reference */
					 const MACROBLOCK * const f_mbs,
					 const IMAGE * const f_ref,
					 const IMAGE * const f_refH,
					 const IMAGE * const f_refV,
					 const IMAGE * const f_refHV,
					 /* backward (future) reference */
					 const FRAMEINFO * const b_reference,
					 const IMAGE * const b_ref,
					 const IMAGE * const b_refH,
					 const IMAGE * const b_refV,
					 const IMAGE * const b_refHV)
{
	uint32_t i, j;
	int32_t best_sad;
	uint32_t skip_sad;

	const MACROBLOCK * const b_mbs = b_reference->mbs;
	MACROBLOCK *const pMBs = frame->mbs;

	VECTOR f_predMV, b_predMV;

	const int32_t TRB = time_pp - time_bp;
	const int32_t TRD = time_pp;

	/* some pre-inintialized data for the rest of the search */

	SearchData Data;
	memset(&Data, 0, sizeof(SearchData));

	Data.iEdgedWidth = pParam->edged_width;
	Data.qpel = pParam->vol_flags & XVID_VOL_QUARTERPEL ? 1 : 0;
	Data.rounding = 0;
	Data.chroma = frame->motion_flags & XVID_ME_CHROMA_BVOP;
	Data.iQuant = frame->quant;

	Data.RefQ = f_refV->u; /* a good place, also used in MC (for similar purpose) */

	/* note: i==horizontal, j==vertical */
	for (j = 0; j < pParam->mb_height; j++) {

		f_predMV = b_predMV = zeroMV;	/* prediction is reset at left boundary */

		for (i = 0; i < pParam->mb_width; i++) {
			MACROBLOCK * const pMB = frame->mbs + i + j * pParam->mb_width;
			const MACROBLOCK * const b_mb = b_mbs + i + j * pParam->mb_width;
			int interpol_search = 0;
			int bf_search = 0;
			int bf_thresh = 0;

/* special case, if collocated block is SKIPed in P-VOP: encoding is forward (0,0), cpb=0 without further ado */
			if (b_reference->coding_type != S_VOP)
				if (b_mb->mode == MODE_NOT_CODED) {
					pMB->mode = MODE_NOT_CODED;
					pMB->mvs[0] = pMB->b_mvs[0] = zeroMV;
					pMB->sad16 = 0;
					continue;
				}
			
			Data.lambda16 = xvid_me_lambda_vec16[b_mb->quant];

			Data.Cur = frame->image.y + (j * Data.iEdgedWidth + i) * 16;
			Data.CurU = frame->image.u + (j * Data.iEdgedWidth/2 + i) * 8;
			Data.CurV = frame->image.v + (j * Data.iEdgedWidth/2 + i) * 8;

/* direct search comes first, because it (1) checks for SKIP-mode
	and (2) sets very good predictions for forward and backward search */
			skip_sad = SearchDirect(f_ref, f_refH->y, f_refV->y, f_refHV->y,
									b_ref, b_refH->y, b_refV->y, b_refHV->y,
									&frame->image,
									i, j,
									frame->motion_flags,
									TRB, TRD,
									pParam,
									pMB, b_mb,
									&best_sad,
									&Data);

			if (pMB->mode == MODE_DIRECT_NONE_MV) {
				pMB->sad16 = best_sad;
				continue;
			}

			if (frame->motion_flags & XVID_ME_BFRAME_EARLYSTOP) {
				if(i > 0 && j > 0 && i < pParam->mb_width) {
					bf_thresh = MIN((&pMBs[(i-1) + j * pParam->mb_width])->sad16, 
								     MIN((&pMBs[i + (j-1) * pParam->mb_width])->sad16,
								     (&pMBs[(i+1) + (j-1) * pParam->mb_width])->sad16));

					if (((&pMBs[(i-1) + j * pParam->mb_width])->mode != MODE_FORWARD) &&
						((&pMBs[(i-1) + j * pParam->mb_width])->mode != MODE_BACKWARD) &&
						((&pMBs[(i-1) + j * pParam->mb_width])->mode != MODE_INTERPOLATE))
							bf_search++;

					if (((&pMBs[i + (j - 1) * pParam->mb_width])->mode != MODE_FORWARD) &&
						((&pMBs[i + (j - 1) * pParam->mb_width])->mode != MODE_BACKWARD) &&
						((&pMBs[i + (j - 1) * pParam->mb_width])->mode != MODE_INTERPOLATE))
							bf_search++;

					if (((&pMBs[(i + 1) + (j - 1) * pParam->mb_width])->mode != MODE_FORWARD) &&
						((&pMBs[(i + 1) + (j - 1) * pParam->mb_width])->mode != MODE_BACKWARD) &&
						((&pMBs[(i + 1) + (j - 1) * pParam->mb_width])->mode != MODE_INTERPOLATE))
						bf_search++;
				}

				if ((best_sad < bf_thresh) && (bf_search == 3))
					continue;
			}

			/* forward search */
			SearchBF(f_ref, f_refH->y, f_refV->y, f_refHV->y,
						i, j,
						frame->motion_flags,
						frame->fcode, pParam,
						pMB, &f_predMV, &best_sad,
						MODE_FORWARD, &Data);

			/* backward search */
			SearchBF(b_ref, b_refH->y, b_refV->y, b_refHV->y,
						i, j,
						frame->motion_flags,
						frame->bcode, pParam,
						pMB, &b_predMV, &best_sad,
						MODE_BACKWARD, &Data);

			/* interpolate search comes last, because it uses data from forward and backward as prediction */
			if (frame->motion_flags & XVID_ME_FAST_MODEINTERPOLATE) {

				if(i > 0 && j > 0 && i < pParam->mb_width) {
					if ((&pMBs[(i-1) + j * pParam->mb_width])->mode == MODE_INTERPOLATE)
						interpol_search++;
					if ((&pMBs[i + (j - 1) * pParam->mb_width])->mode == MODE_INTERPOLATE)
						interpol_search++;
					if ((&pMBs[(i + 1) + (j - 1) * pParam->mb_width])->mode == MODE_INTERPOLATE)
						interpol_search++;
				}
				else
					interpol_search = 1;

				interpol_search |= !(best_sad < 3 * Data.iQuant * MAX_SAD00_FOR_SKIP * (Data.chroma ? 3:2));
			}
			else
				interpol_search = 1;

			if (interpol_search) {
				SearchInterpolate(f_ref, f_refH->y, f_refV->y, f_refHV->y,
							   	  b_ref, b_refH->y, b_refV->y, b_refHV->y,
								  i, j,
							  	  frame->fcode, frame->bcode,
								  frame->motion_flags,
								  pParam,
								  &f_predMV, &b_predMV,
								  pMB, &best_sad,
								  &Data);
			}

			/* final skip decision */
			if ( (skip_sad < Data.iQuant * MAX_SAD00_FOR_SKIP * (Data.chroma ? 3:2) )
				&& ((100*best_sad)/(skip_sad+1) > FINAL_SKIP_THRESH) )

				SkipDecisionB(&frame->image, f_ref, b_ref, pMB, i, j, &Data);

			switch (pMB->mode) {
				case MODE_FORWARD:
					f_predMV = Data.qpel ? pMB->qmvs[0] : pMB->mvs[0];
					pMB->sad16 = best_sad;
					break;
				case MODE_BACKWARD:
					b_predMV = Data.qpel ? pMB->b_qmvs[0] : pMB->b_mvs[0];
					pMB->sad16 = best_sad;
					break;
				case MODE_INTERPOLATE:
					f_predMV = Data.qpel ? pMB->qmvs[0] : pMB->mvs[0];
					b_predMV = Data.qpel ? pMB->b_qmvs[0] : pMB->b_mvs[0];
					pMB->sad16 = best_sad;
					break;
				default:
					pMB->sad16 = best_sad;
					break;
			}
		}
	}
}
