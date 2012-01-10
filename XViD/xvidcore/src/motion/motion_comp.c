/*****************************************************************************
 *
 *  XVID MPEG-4 VIDEO CODEC
 *  - Motion Compensation related code  -
 *
 *  Copyright(C) 2002 Peter Ross <pross@xvid.org>
 *               2003 Christoph Lampert <gruel@web.de>
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

#include <stdio.h>

#include "../encoder.h"
#include "../utils/mbfunctions.h"
#include "../image/interpolate8x8.h"
#include "../image/qpel.h"
#include "../image/reduced.h"
#include "../utils/timer.h"
#include "motion.h"

#ifndef RSHIFT
#define RSHIFT(a,b) ((a) > 0 ? ((a) + (1<<((b)-1)))>>(b) : ((a) + (1<<((b)-1))-1)>>(b))
#endif

/* assume b>0 */
#ifndef RDIV
#define RDIV(a,b) (((a)>0 ? (a) + ((b)>>1) : (a) - ((b)>>1))/(b))
#endif


/* This is borrowed from   bitstream.c  until we find a common solution */

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

/*
 * getref: calculate reference image pointer
 * the decision to use interpolation h/v/hv or the normal image is
 * based on dx & dy.
 */

static __inline const uint8_t *
get_ref(const uint8_t * const refn,
		const uint8_t * const refh,
		const uint8_t * const refv,
		const uint8_t * const refhv,
		const uint32_t x,
		const uint32_t y,
		const uint32_t block,
		const int32_t dx,
		const int32_t dy,
		const int32_t stride)
{
	switch (((dx & 1) << 1) + (dy & 1)) {
	case 0:
		return refn + (int) ((x * block + dx / 2) + (y * block + dy / 2) * stride);
	case 1:
		return refv + (int) ((x * block + dx / 2) + (y * block + (dy - 1) / 2) * stride);
	case 2:
		return refh + (int) ((x * block + (dx - 1) / 2) + (y * block + dy / 2) * stride);
	default:
		return refhv + (int) ((x * block + (dx - 1) / 2) + (y * block + (dy - 1) / 2) * stride);
	}
}

static __inline void
compensate16x16_interpolate(int16_t * const dct_codes,
							uint8_t * const cur,
							const uint8_t * const ref,
							const uint8_t * const refh,
							const uint8_t * const refv,
							const uint8_t * const refhv,
							uint8_t * const tmp,
							uint32_t x,
							uint32_t y,
							const int32_t dx,
							const int32_t dy,
							const int32_t stride,
							const int quarterpel,
							const int reduced_resolution,
							const int32_t rounding)
{
	const uint8_t * ptr;

	if (!reduced_resolution) {

		if(quarterpel) {
			if ((dx&3) | (dy&3)) {
#if defined(ARCH_IS_IA32) /* new_interpolate is only faster on x86 (MMX) machines */
				new_interpolate16x16_quarterpel(tmp - y * stride - x,
											(uint8_t *) ref, tmp + 32,
											tmp + 64, tmp + 96, x, y, dx, dy, stride, rounding);
#else
				interpolate16x16_quarterpel(tmp - y * stride - x,
											(uint8_t *) ref, tmp + 32,
											tmp + 64, tmp + 96, x, y, dx, dy, stride, rounding);
#endif
				ptr = tmp;
			} else ptr =  ref + (y + dy/4)*stride + x + dx/4; /* fullpixel position */

		} else ptr = get_ref(ref, refh, refv, refhv, x, y, 1, dx, dy, stride);

		transfer_8to16sub(dct_codes, cur + y * stride + x,
							ptr, stride);
		transfer_8to16sub(dct_codes+64, cur + y * stride + x + 8,
							ptr + 8, stride);
		transfer_8to16sub(dct_codes+128, cur + y * stride + x + 8*stride,
							ptr + 8*stride, stride);
		transfer_8to16sub(dct_codes+192, cur + y * stride + x + 8*stride+8,
							ptr + 8*stride + 8, stride);

	} else { /* reduced_resolution */

		x *= 2; y *= 2;

		ptr = get_ref(ref, refh, refv, refhv, x, y, 1, dx, dy, stride);

		filter_18x18_to_8x8(dct_codes, cur+y*stride + x, stride);
		filter_diff_18x18_to_8x8(dct_codes, ptr, stride);

		filter_18x18_to_8x8(dct_codes+64, cur+y*stride + x + 16, stride);
		filter_diff_18x18_to_8x8(dct_codes+64, ptr + 16, stride);

		filter_18x18_to_8x8(dct_codes+128, cur+(y+16)*stride + x, stride);
		filter_diff_18x18_to_8x8(dct_codes+128, ptr + 16*stride, stride);

		filter_18x18_to_8x8(dct_codes+192, cur+(y+16)*stride + x + 16, stride);
		filter_diff_18x18_to_8x8(dct_codes+192, ptr + 16*stride + 16, stride);

		transfer32x32_copy(cur + y*stride + x, ptr, stride);
	}
}

static __inline void
compensate8x8_interpolate(	int16_t * const dct_codes,
							uint8_t * const cur,
							const uint8_t * const ref,
							const uint8_t * const refh,
							const uint8_t * const refv,
							const uint8_t * const refhv,
							uint8_t * const tmp,
							uint32_t x,
							uint32_t y,
							const int32_t dx,
							const int32_t dy,
							const int32_t stride,
							const int32_t quarterpel,
							const int reduced_resolution,
							const int32_t rounding)
{
	const uint8_t * ptr;

	if (!reduced_resolution) {

		if(quarterpel) {
			if ((dx&3) | (dy&3)) {
#if defined(ARCH_IS_IA32) /* new_interpolate is only faster on x86 (MMX) machines */
				new_interpolate8x8_quarterpel(tmp - y*stride - x,
										(uint8_t *) ref, tmp + 32,
										tmp + 64, tmp + 96, x, y, dx, dy, stride, rounding);
#else
				interpolate8x8_quarterpel(tmp - y*stride - x,
										(uint8_t *) ref, tmp + 32,
										tmp + 64, tmp + 96, x, y, dx, dy, stride, rounding);
#endif
				ptr = tmp;
			} else ptr = ref + (y + dy/4)*stride + x + dx/4; /* fullpixel position */
		} else ptr = get_ref(ref, refh, refv, refhv, x, y, 1, dx, dy, stride);

			transfer_8to16sub(dct_codes, cur + y * stride + x, ptr, stride);

	} else { /* reduced_resolution */

		x *= 2; y *= 2;

		ptr = get_ref(ref, refh, refv, refhv, x, y, 1, dx, dy, stride);

		filter_18x18_to_8x8(dct_codes, cur+y*stride + x, stride);
		filter_diff_18x18_to_8x8(dct_codes, ptr, stride);

		transfer16x16_copy(cur + y*stride + x, ptr, stride);
	}
}

/* XXX: slow, inelegant... */
static void
interpolate18x18_switch(uint8_t * const cur,
						const uint8_t * const refn,
						const uint32_t x,
						const uint32_t y,
						const int32_t dx,
						const int dy,
						const int32_t stride,
						const int32_t rounding)
{
	interpolate8x8_switch(cur, refn, x-1, y-1, dx, dy, stride, rounding);
	interpolate8x8_switch(cur, refn, x+7, y-1, dx, dy, stride, rounding);
	interpolate8x8_switch(cur, refn, x+9, y-1, dx, dy, stride, rounding);

	interpolate8x8_switch(cur, refn, x-1, y+7, dx, dy, stride, rounding);
	interpolate8x8_switch(cur, refn, x+7, y+7, dx, dy, stride, rounding);
	interpolate8x8_switch(cur, refn, x+9, y+7, dx, dy, stride, rounding);

	interpolate8x8_switch(cur, refn, x-1, y+9, dx, dy, stride, rounding);
	interpolate8x8_switch(cur, refn, x+7, y+9, dx, dy, stride, rounding);
	interpolate8x8_switch(cur, refn, x+9, y+9, dx, dy, stride, rounding);
}

static void
CompensateChroma(	int dx, int dy,
					const int i, const int j,
					IMAGE * const Cur,
					const IMAGE * const Ref,
					uint8_t * const temp,
					int16_t * const coeff,
					const int32_t stride,
					const int rounding,
					const int rrv)
{ /* uv-block-based compensation */

	if (!rrv) {
		transfer_8to16sub(coeff, Cur->u + 8 * j * stride + 8 * i,
							interpolate8x8_switch2(temp, Ref->u, 8 * i, 8 * j,
													dx, dy, stride, rounding),
							stride);
		transfer_8to16sub(coeff + 64, Cur->v + 8 * j * stride + 8 * i,
 							interpolate8x8_switch2(temp, Ref->v, 8 * i, 8 * j,
													dx, dy, stride, rounding),
							stride);
	} else {
		uint8_t * current, * reference;

		current = Cur->u + 16*j*stride + 16*i;
		reference = temp - 16*j*stride - 16*i;
		interpolate18x18_switch(reference, Ref->u, 16*i, 16*j, dx, dy, stride, rounding);
		filter_18x18_to_8x8(coeff, current, stride);
		filter_diff_18x18_to_8x8(coeff, temp, stride);
		transfer16x16_copy(current, temp, stride);

		current = Cur->v + 16*j*stride + 16*i;
		interpolate18x18_switch(reference, Ref->v, 16*i, 16*j, dx, dy, stride, rounding);
		filter_18x18_to_8x8(coeff + 64, current, stride);
		filter_diff_18x18_to_8x8(coeff + 64, temp, stride);
		transfer16x16_copy(current, temp, stride);
	}
}

void
MBMotionCompensation(MACROBLOCK * const mb,
					const uint32_t i,
					const uint32_t j,
					const IMAGE * const ref,
					const IMAGE * const refh,
					const IMAGE * const refv,
					const IMAGE * const refhv,
					const IMAGE * const refGMC,
					IMAGE * const cur,
					int16_t * dct_codes,
					const uint32_t width,
					const uint32_t height,
					const uint32_t edged_width,
					const int32_t quarterpel,
					const int reduced_resolution,
					const int32_t rounding)
{
	int32_t dx;
	int32_t dy;

	uint8_t * const tmp = refv->u;

	if ( (!reduced_resolution) && (mb->mode == MODE_NOT_CODED) ) {	/* quick copy for early SKIP */
/* early SKIP is only activated in P-VOPs, not in S-VOPs, so mcsel can never be 1 */

		transfer16x16_copy(cur->y + 16 * (i + j * edged_width),
						  ref->y + 16 * (i + j * edged_width),
						  edged_width);

		transfer8x8_copy(cur->u + 8 * (i + j * edged_width/2),
							ref->u + 8 * (i + j * edged_width/2),
							edged_width / 2);
		transfer8x8_copy(cur->v + 8 * (i + j * edged_width/2),
							ref->v + 8 * (i + j * edged_width/2),
							edged_width / 2);
		return;
	}

	if ((mb->mode == MODE_NOT_CODED || mb->mode == MODE_INTER
				|| mb->mode == MODE_INTER_Q)) {

	/* reduced resolution + GMC:  not possible */

		if (mb->mcsel) {

			/* call normal routine once, easier than "if (mcsel)"ing all the time */

			transfer_8to16sub(&dct_codes[0*64], cur->y + 16*j*edged_width + 16*i,
								 			refGMC->y + 16*j*edged_width + 16*i, edged_width);
			transfer_8to16sub(&dct_codes[1*64], cur->y + 16*j*edged_width + 16*i+8,
											refGMC->y + 16*j*edged_width + 16*i+8, edged_width);
			transfer_8to16sub(&dct_codes[2*64], cur->y + (16*j+8)*edged_width + 16*i,
											refGMC->y + (16*j+8)*edged_width + 16*i, edged_width);
			transfer_8to16sub(&dct_codes[3*64], cur->y + (16*j+8)*edged_width + 16*i+8,
											refGMC->y + (16*j+8)*edged_width + 16*i+8, edged_width);

/* lumi is needed earlier for mode decision, but chroma should be done block-based, but it isn't, yet. */

			transfer_8to16sub(&dct_codes[4 * 64], cur->u + 8 *j*edged_width/2 + 8*i,
								refGMC->u + 8 *j*edged_width/2 + 8*i, edged_width/2);

			transfer_8to16sub(&dct_codes[5 * 64], cur->v + 8*j* edged_width/2 + 8*i,
								refGMC->v + 8*j* edged_width/2 + 8*i, edged_width/2);

			return;
		}

		/* ordinary compensation */

		dx = (quarterpel ? mb->qmvs[0].x : mb->mvs[0].x);
		dy = (quarterpel ? mb->qmvs[0].y : mb->mvs[0].y);

		if (reduced_resolution) {
			dx = RRV_MV_SCALEUP(dx);
			dy = RRV_MV_SCALEUP(dy);
		}

		compensate16x16_interpolate(&dct_codes[0 * 64], cur->y, ref->y, refh->y,
							refv->y, refhv->y, tmp, 16 * i, 16 * j, dx, dy,
							edged_width, quarterpel, reduced_resolution, rounding);

		if (quarterpel) { dx /= 2; dy /= 2; }

		dx = (dx >> 1) + roundtab_79[dx & 0x3];
		dy = (dy >> 1) + roundtab_79[dy & 0x3];

	} else {					/* mode == MODE_INTER4V */
		int k, sumx = 0, sumy = 0;
		const VECTOR * const mvs = (quarterpel ? mb->qmvs : mb->mvs);

		for (k = 0; k < 4; k++) {
			dx = mvs[k].x;
			dy = mvs[k].y;
			sumx += quarterpel ? dx/2 : dx;
			sumy += quarterpel ? dy/2 : dy;

			if (reduced_resolution){
				dx = RRV_MV_SCALEUP(dx);
				dy = RRV_MV_SCALEUP(dy);
			}

			compensate8x8_interpolate(&dct_codes[k * 64], cur->y, ref->y, refh->y,
									refv->y, refhv->y, tmp, 16 * i + 8*(k&1), 16 * j + 8*(k>>1), dx,
									dy, edged_width, quarterpel, reduced_resolution, rounding);
		}
		dx = (sumx >> 3) + roundtab_76[sumx & 0xf];
		dy = (sumy >> 3) + roundtab_76[sumy & 0xf];
	}

	CompensateChroma(dx, dy, i, j, cur, ref, tmp,
					&dct_codes[4 * 64], edged_width / 2, rounding, reduced_resolution);
}


void
MBMotionCompensationBVOP(MBParam * pParam,
						MACROBLOCK * const mb,
						const uint32_t i,
						const uint32_t j,
						IMAGE * const cur,
						const IMAGE * const f_ref,
						const IMAGE * const f_refh,
						const IMAGE * const f_refv,
						const IMAGE * const f_refhv,
						const IMAGE * const b_ref,
						const IMAGE * const b_refh,
						const IMAGE * const b_refv,
						const IMAGE * const b_refhv,
						int16_t * dct_codes)
{
	const uint32_t edged_width = pParam->edged_width;
	int32_t dx, dy, b_dx, b_dy, sumx, sumy, b_sumx, b_sumy;
	int k;
	const int quarterpel = pParam->vol_flags & XVID_VOL_QUARTERPEL;
	const uint8_t * ptr1, * ptr2;
	uint8_t * const tmp = f_refv->u;
	const VECTOR * const fmvs = (quarterpel ? mb->qmvs : mb->mvs);
	const VECTOR * const bmvs = (quarterpel ? mb->b_qmvs : mb->b_mvs);

	switch (mb->mode) {
	case MODE_FORWARD:
		dx = fmvs->x; dy = fmvs->y;

		compensate16x16_interpolate(&dct_codes[0 * 64], cur->y, f_ref->y, f_refh->y,
							f_refv->y, f_refhv->y, tmp, 16 * i, 16 * j, dx,
							dy, edged_width, quarterpel, 0, 0);

		if (quarterpel) { dx /= 2; dy /= 2; }

		CompensateChroma(	(dx >> 1) + roundtab_79[dx & 0x3],
							(dy >> 1) + roundtab_79[dy & 0x3],
							i, j, cur, f_ref, tmp,
							&dct_codes[4 * 64], edged_width / 2, 0, 0);

		return;

	case MODE_BACKWARD:
		b_dx = bmvs->x; b_dy = bmvs->y;

		compensate16x16_interpolate(&dct_codes[0 * 64], cur->y, b_ref->y, b_refh->y,
										b_refv->y, b_refhv->y, tmp, 16 * i, 16 * j, b_dx,
										b_dy, edged_width, quarterpel, 0, 0);

		if (quarterpel) { b_dx /= 2; b_dy /= 2; }

		CompensateChroma(	(b_dx >> 1) + roundtab_79[b_dx & 0x3],
							(b_dy >> 1) + roundtab_79[b_dy & 0x3],
							i, j, cur, b_ref, tmp,
							&dct_codes[4 * 64], edged_width / 2, 0, 0);

		return;

	case MODE_INTERPOLATE: /* _could_ use DIRECT, but would be overkill (no 4MV there) */
	case MODE_DIRECT_NO4V:
		dx = fmvs->x; dy = fmvs->y;
		b_dx = bmvs->x; b_dy = bmvs->y;

		if (quarterpel) {

			if ((dx&3) | (dy&3)) {
				interpolate16x16_quarterpel(tmp - i * 16 - j * 16 * edged_width,
					(uint8_t *) f_ref->y, tmp + 32,
					tmp + 64, tmp + 96, 16*i, 16*j, dx, dy, edged_width, 0);
				ptr1 = tmp;
			} else ptr1 = f_ref->y + (16*j + dy/4)*edged_width + 16*i + dx/4; /* fullpixel position */

			if ((b_dx&3) | (b_dy&3)) {
				interpolate16x16_quarterpel(tmp - i * 16 - j * 16 * edged_width + 16,
					(uint8_t *) b_ref->y, tmp + 32,
					tmp + 64, tmp + 96, 16*i, 16*j, b_dx, b_dy, edged_width, 0);
				ptr2 = tmp + 16;
			} else ptr2 = b_ref->y + (16*j + b_dy/4)*edged_width + 16*i + b_dx/4; /* fullpixel position */

			b_dx /= 2;
			b_dy /= 2;
			dx /= 2;
			dy /= 2;

		} else {
			ptr1 = get_ref(f_ref->y, f_refh->y, f_refv->y, f_refhv->y,
							i, j, 16, dx, dy, edged_width);

			ptr2 = get_ref(b_ref->y, b_refh->y, b_refv->y, b_refhv->y,
							i, j, 16, b_dx, b_dy, edged_width);
		}
		for (k = 0; k < 4; k++)
				transfer_8to16sub2(&dct_codes[k * 64],
									cur->y + (i * 16+(k&1)*8) + (j * 16+((k>>1)*8)) * edged_width,
									ptr1 + (k&1)*8 + (k>>1)*8*edged_width,
									ptr2 + (k&1)*8 + (k>>1)*8*edged_width, edged_width);


		dx = (dx >> 1) + roundtab_79[dx & 0x3];
		dy = (dy >> 1) + roundtab_79[dy & 0x3];

		b_dx = (b_dx >> 1) + roundtab_79[b_dx & 0x3];
		b_dy = (b_dy >> 1) + roundtab_79[b_dy & 0x3];

		break;

	default: /* MODE_DIRECT (or MODE_DIRECT_NONE_MV in case of bframes decoding) */
		sumx = sumy = b_sumx = b_sumy = 0;

		for (k = 0; k < 4; k++) {

			dx = fmvs[k].x; dy = fmvs[k].y;
			b_dx = bmvs[k].x; b_dy = bmvs[k].y;

			if (quarterpel) {
				sumx += dx/2; sumy += dy/2;
				b_sumx += b_dx/2; b_sumy += b_dy/2;

				if ((dx&3) | (dy&3)) {
					interpolate8x8_quarterpel(tmp - (i * 16+(k&1)*8) - (j * 16+((k>>1)*8)) * edged_width,
						(uint8_t *) f_ref->y,
						tmp + 32, tmp + 64, tmp + 96,
						16*i + (k&1)*8, 16*j + (k>>1)*8, dx, dy, edged_width, 0);
					ptr1 = tmp;
				} else ptr1 = f_ref->y + (16*j + (k>>1)*8 + dy/4)*edged_width + 16*i + (k&1)*8 + dx/4;

				if ((b_dx&3) | (b_dy&3)) {
					interpolate8x8_quarterpel(tmp - (i * 16+(k&1)*8) - (j * 16+((k>>1)*8)) * edged_width + 16,
						(uint8_t *) b_ref->y,
						tmp + 16, tmp + 32, tmp + 48,
						16*i + (k&1)*8, 16*j + (k>>1)*8, b_dx, b_dy, edged_width, 0);
					ptr2 = tmp + 16;
				} else ptr2 = b_ref->y + (16*j + (k>>1)*8 + b_dy/4)*edged_width + 16*i + (k&1)*8 + b_dx/4;
			} else {
				sumx += dx; sumy += dy;
				b_sumx += b_dx; b_sumy += b_dy;

				ptr1 = get_ref(f_ref->y, f_refh->y, f_refv->y, f_refhv->y,
								2*i + (k&1), 2*j + (k>>1), 8, dx, dy, edged_width);
				ptr2 = get_ref(b_ref->y, b_refh->y, b_refv->y, b_refhv->y,
								2*i + (k&1), 2*j + (k>>1), 8, b_dx, b_dy,  edged_width);
			}
			transfer_8to16sub2(&dct_codes[k * 64],
								cur->y + (i * 16+(k&1)*8) + (j * 16+((k>>1)*8)) * edged_width,
								ptr1, ptr2,	edged_width);

		}

		dx = (sumx >> 3) + roundtab_76[sumx & 0xf];
		dy = (sumy >> 3) + roundtab_76[sumy & 0xf];
		b_dx = (b_sumx >> 3) + roundtab_76[b_sumx & 0xf];
		b_dy = (b_sumy >> 3) + roundtab_76[b_sumy & 0xf];

		break;
	}

	/* v block-based chroma interpolation for direct and interpolate modes */
	transfer_8to16sub2(&dct_codes[4 * 64],
						cur->u + (j * 8) * edged_width / 2 + (i * 8),
						interpolate8x8_switch2(tmp, b_ref->u, 8 * i, 8 * j,
												b_dx, b_dy, edged_width / 2, 0),
						interpolate8x8_switch2(tmp + 8, f_ref->u, 8 * i, 8 * j,
												dx, dy, edged_width / 2, 0),
						edged_width / 2);

	transfer_8to16sub2(&dct_codes[5 * 64],
						cur->v + (j * 8) * edged_width / 2 + (i * 8),
						interpolate8x8_switch2(tmp, b_ref->v, 8 * i, 8 * j,
												b_dx, b_dy, edged_width / 2, 0),
						interpolate8x8_switch2(tmp + 8, f_ref->v, 8 * i, 8 * j,
												dx, dy, edged_width / 2, 0),
						edged_width / 2);
}
