/*****************************************************************************
 *
 *  XVID MPEG-4 VIDEO CODEC
 *  - QPel interpolation -
 *
 *  Copyright(C) 2003 Pascal Massimino <skal@planet-d.net>
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

#ifndef XVID_AUTO_INCLUDE

#include "../portab.h"
#include "qpel.h"

/* Quarterpel FIR definition
 ****************************************************************************/

static const int32_t FIR_Tab_8[9][8] = {
	{ 14, -3,  2, -1,  0,  0,  0,  0 },
	{ 23, 19, -6,  3, -1,  0,  0,  0 },
	{ -7, 20, 20, -6,  3, -1,  0,  0 },
	{  3, -6, 20, 20, -6,  3, -1,  0 },
	{ -1,  3, -6, 20, 20, -6,  3, -1 },
	{  0, -1,  3, -6, 20, 20, -6,  3 },
	{  0,  0, -1,  3, -6, 20, 20, -7 },
	{  0,  0,  0, -1,  3, -6, 19, 23 },
	{  0,  0,  0,  0, -1,  2, -3, 14 }
};

static const int32_t FIR_Tab_16[17][16] = {
	{ 14, -3,  2, -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
	{ 23, 19, -6,  3, -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
	{ -7, 20, 20, -6,  3, -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
	{  3, -6, 20, 20, -6,  3, -1,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
	{ -1,  3, -6, 20, 20, -6,  3, -1,  0,  0,  0,  0,  0,  0,  0,  0 },
	{  0, -1,  3, -6, 20, 20, -6,  3, -1,  0,  0,  0,  0,  0,  0,  0 },
	{  0,  0, -1,  3, -6, 20, 20, -6,  3, -1,  0,  0,  0,  0,  0,  0 },
	{  0,  0,  0, -1,  3, -6, 20, 20, -6,  3, -1,  0,  0,  0,  0,  0 },
	{  0,  0,  0,  0, -1,  3, -6, 20, 20, -6,  3, -1,  0,  0,  0,  0 },
	{  0,  0,  0,  0,  0, -1,  3, -6, 20, 20, -6,  3, -1,  0,  0,  0 },
	{  0,  0,  0,  0,  0,  0, -1,  3, -6, 20, 20, -6,  3, -1,  0,  0 },
	{  0,  0,  0,  0,  0,  0,  0, -1,  3, -6, 20, 20, -6,  3, -1,  0 },
	{  0,  0,  0,  0,  0,  0,  0,  0, -1,  3, -6, 20, 20, -6,  3, -1 },
	{  0,  0,  0,  0,  0,  0,  0,  0,  0, -1,  3, -6, 20, 20, -6,  3 },
	{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1,  3, -6, 20, 20, -7 },
	{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1,  3, -6, 19, 23 },
	{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1,  2, -3, 14 }
};

/* Implementation
 ****************************************************************************/

#define XVID_AUTO_INCLUDE

/* 16x? filters */

#define SIZE  16
#define TABLE FIR_Tab_16

#define STORE(d,s)  (d) = (s)
#define FUNC_H      H_Pass_16_C
#define FUNC_V      V_Pass_16_C
#define FUNC_HA     H_Pass_Avrg_16_C
#define FUNC_VA     V_Pass_Avrg_16_C
#define FUNC_HA_UP  H_Pass_Avrg_Up_16_C
#define FUNC_VA_UP  V_Pass_Avrg_Up_16_C

#include "qpel.c"   /* self-include ourself */

/* note: B-frame always uses Rnd=0... */
#define STORE(d,s)  (d) = ( (s)+(d)+1 ) >> 1
#define FUNC_H      H_Pass_16_Add_C
#define FUNC_V      V_Pass_16_Add_C
#define FUNC_HA     H_Pass_Avrg_16_Add_C
#define FUNC_VA     V_Pass_Avrg_16_Add_C
#define FUNC_HA_UP  H_Pass_Avrg_Up_16_Add_C
#define FUNC_VA_UP  V_Pass_Avrg_Up_16_Add_C

#include "qpel.c"   /* self-include ourself */

#undef SIZE
#undef TABLE

/* 8x? filters */

#define SIZE  8
#define TABLE FIR_Tab_8

#define STORE(d,s)  (d) = (s)
#define FUNC_H      H_Pass_8_C
#define FUNC_V      V_Pass_8_C
#define FUNC_HA     H_Pass_Avrg_8_C
#define FUNC_VA     V_Pass_Avrg_8_C
#define FUNC_HA_UP  H_Pass_Avrg_Up_8_C
#define FUNC_VA_UP  V_Pass_Avrg_Up_8_C

#include "qpel.c"   /* self-include ourself */

/* note: B-frame always uses Rnd=0... */
#define STORE(d,s)  (d) = ( (s)+(d)+1 ) >> 1
#define FUNC_H      H_Pass_8_Add_C
#define FUNC_V      V_Pass_8_Add_C
#define FUNC_HA     H_Pass_Avrg_8_Add_C
#define FUNC_VA     V_Pass_Avrg_8_Add_C
#define FUNC_HA_UP  H_Pass_Avrg_Up_8_Add_C
#define FUNC_VA_UP  V_Pass_Avrg_Up_8_Add_C

#include "qpel.c"   /* self-include ourself */

#undef SIZE
#undef TABLE

#undef XVID_AUTO_INCLUDE

/* general-purpose hooks
 * TODO: embed in enc/dec structure?
 ****************************************************************************/

XVID_QP_FUNCS *xvid_QP_Funcs = 0;
XVID_QP_FUNCS *xvid_QP_Add_Funcs = 0;

/* plain C impl. declaration
 * TODO: should be declared elsewhere?
 ****************************************************************************/

XVID_QP_FUNCS xvid_QP_Funcs_C = {
	H_Pass_16_C, H_Pass_Avrg_16_C, H_Pass_Avrg_Up_16_C,
	V_Pass_16_C, V_Pass_Avrg_16_C, V_Pass_Avrg_Up_16_C,

	H_Pass_8_C, H_Pass_Avrg_8_C, H_Pass_Avrg_Up_8_C,
	V_Pass_8_C, V_Pass_Avrg_8_C, V_Pass_Avrg_Up_8_C
};

XVID_QP_FUNCS xvid_QP_Add_Funcs_C = {
	H_Pass_16_Add_C, H_Pass_Avrg_16_Add_C, H_Pass_Avrg_Up_16_Add_C,
	V_Pass_16_Add_C, V_Pass_Avrg_16_Add_C, V_Pass_Avrg_Up_16_Add_C,

	H_Pass_8_Add_C, H_Pass_Avrg_8_Add_C, H_Pass_Avrg_Up_8_Add_C,
	V_Pass_8_Add_C, V_Pass_Avrg_8_Add_C, V_Pass_Avrg_Up_8_Add_C
};

/* mmx impl. declaration (see. qpel_mmx.asm
 * TODO: should be declared elsewhere?
 ****************************************************************************/

#ifdef ARCH_IS_IA32
extern XVID_QP_PASS_SIGNATURE(xvid_H_Pass_16_mmx);
extern XVID_QP_PASS_SIGNATURE(xvid_H_Pass_Avrg_16_mmx);
extern XVID_QP_PASS_SIGNATURE(xvid_H_Pass_Avrg_Up_16_mmx);
extern XVID_QP_PASS_SIGNATURE(xvid_V_Pass_16_mmx);
extern XVID_QP_PASS_SIGNATURE(xvid_V_Pass_Avrg_16_mmx);
extern XVID_QP_PASS_SIGNATURE(xvid_V_Pass_Avrg_Up_16_mmx);

extern XVID_QP_PASS_SIGNATURE(xvid_H_Pass_8_mmx);
extern XVID_QP_PASS_SIGNATURE(xvid_H_Pass_Avrg_8_mmx);
extern XVID_QP_PASS_SIGNATURE(xvid_H_Pass_Avrg_Up_8_mmx);
extern XVID_QP_PASS_SIGNATURE(xvid_V_Pass_8_mmx);
extern XVID_QP_PASS_SIGNATURE(xvid_V_Pass_Avrg_8_mmx);
extern XVID_QP_PASS_SIGNATURE(xvid_V_Pass_Avrg_Up_8_mmx);

XVID_QP_FUNCS xvid_QP_Funcs_mmx = {
	xvid_H_Pass_16_mmx, xvid_H_Pass_Avrg_16_mmx, xvid_H_Pass_Avrg_Up_16_mmx,
	xvid_V_Pass_16_mmx, xvid_V_Pass_Avrg_16_mmx, xvid_V_Pass_Avrg_Up_16_mmx,

	xvid_H_Pass_8_mmx, xvid_H_Pass_Avrg_8_mmx, xvid_H_Pass_Avrg_Up_8_mmx,
	xvid_V_Pass_8_mmx, xvid_V_Pass_Avrg_8_mmx, xvid_V_Pass_Avrg_Up_8_mmx
};

extern XVID_QP_PASS_SIGNATURE(xvid_H_Pass_Add_16_mmx);
extern XVID_QP_PASS_SIGNATURE(xvid_H_Pass_Avrg_Add_16_mmx);
extern XVID_QP_PASS_SIGNATURE(xvid_H_Pass_Avrg_Up_Add_16_mmx);
extern XVID_QP_PASS_SIGNATURE(xvid_V_Pass_Add_16_mmx);
extern XVID_QP_PASS_SIGNATURE(xvid_V_Pass_Avrg_Add_16_mmx);
extern XVID_QP_PASS_SIGNATURE(xvid_V_Pass_Avrg_Up_Add_16_mmx);

extern XVID_QP_PASS_SIGNATURE(xvid_H_Pass_8_Add_mmx);
extern XVID_QP_PASS_SIGNATURE(xvid_H_Pass_Avrg_8_Add_mmx);
extern XVID_QP_PASS_SIGNATURE(xvid_H_Pass_Avrg_Up_8_Add_mmx);
extern XVID_QP_PASS_SIGNATURE(xvid_V_Pass_8_Add_mmx);
extern XVID_QP_PASS_SIGNATURE(xvid_V_Pass_Avrg_8_Add_mmx);
extern XVID_QP_PASS_SIGNATURE(xvid_V_Pass_Avrg_Up_8_Add_mmx);

XVID_QP_FUNCS xvid_QP_Add_Funcs_mmx = {
	xvid_H_Pass_Add_16_mmx, xvid_H_Pass_Avrg_Add_16_mmx, xvid_H_Pass_Avrg_Up_Add_16_mmx,
	xvid_V_Pass_Add_16_mmx, xvid_V_Pass_Avrg_Add_16_mmx, xvid_V_Pass_Avrg_Up_Add_16_mmx,

	xvid_H_Pass_8_Add_mmx, xvid_H_Pass_Avrg_8_Add_mmx, xvid_H_Pass_Avrg_Up_8_Add_mmx,
	xvid_V_Pass_8_Add_mmx, xvid_V_Pass_Avrg_8_Add_mmx, xvid_V_Pass_Avrg_Up_8_Add_mmx,
};
#endif /* ARCH_IS_IA32 */

/* tables for ASM
 ****************************************************************************/

#ifdef ARCH_IS_IA32
uint16_t xvid_Expand_mmx[256][4]; /* 8b -> 64b expansion table */
#endif

/* Alternate way of filtering (cf. USE_TABLES flag in qpel_mmx.asm)
 *
 * 17 tables, 2K each => 34K
 * Mirroring can be acheived composing 11 basic tables
 * (for instance: (23,19,-6,3)=(20,20,-6,3)+(3,-1,0,0)
 * Using Symmetries (and bswap) could reduce further
 * the memory to 7 tables (->14K). */

int16_t xvid_FIR_1_0_0_0[256][4];
int16_t xvid_FIR_3_1_0_0[256][4];
int16_t xvid_FIR_6_3_1_0[256][4];
int16_t xvid_FIR_14_3_2_1[256][4];
int16_t xvid_FIR_20_6_3_1[256][4];
int16_t xvid_FIR_20_20_6_3[256][4];
int16_t xvid_FIR_23_19_6_3[256][4];
int16_t xvid_FIR_7_20_20_6[256][4];
int16_t xvid_FIR_6_20_20_6[256][4];
int16_t xvid_FIR_6_20_20_7[256][4];
int16_t xvid_FIR_3_6_20_20[256][4];
int16_t xvid_FIR_3_6_19_23[256][4];
int16_t xvid_FIR_1_3_6_20[256][4];
int16_t xvid_FIR_1_2_3_14[256][4];
int16_t xvid_FIR_0_1_3_6[256][4];
int16_t xvid_FIR_0_0_1_3[256][4];
int16_t xvid_FIR_0_0_0_1[256][4];

static void Init_FIR_Table(int16_t Tab[][4],
                           int A, int B, int C, int D)
{
	int i;
	for(i=0; i<256; ++i) {
		Tab[i][0] = i*A;
		Tab[i][1] = i*B;
		Tab[i][2] = i*C;
		Tab[i][3] = i*D;
	}
}


void xvid_Init_QP()
{
#ifdef ARCH_IS_IA32
	int i;

	for(i=0; i<256; ++i) {
		xvid_Expand_mmx[i][0] = i;
		xvid_Expand_mmx[i][1] = i;
		xvid_Expand_mmx[i][2] = i;
		xvid_Expand_mmx[i][3] = i;
	}
#endif

	/* Alternate way of filtering (cf. USE_TABLES flag in qpel_mmx.asm) */

	Init_FIR_Table(xvid_FIR_1_0_0_0,   -1,  0,  0,  0);
	Init_FIR_Table(xvid_FIR_3_1_0_0,    3, -1,  0,  0);
	Init_FIR_Table(xvid_FIR_6_3_1_0,   -6,  3, -1,  0);
	Init_FIR_Table(xvid_FIR_14_3_2_1,  14, -3,  2, -1);
	Init_FIR_Table(xvid_FIR_20_6_3_1,  20, -6,  3, -1);
	Init_FIR_Table(xvid_FIR_20_20_6_3, 20, 20, -6,  3);
	Init_FIR_Table(xvid_FIR_23_19_6_3, 23, 19, -6,  3);
	Init_FIR_Table(xvid_FIR_7_20_20_6, -7, 20, 20, -6);
	Init_FIR_Table(xvid_FIR_6_20_20_6, -6, 20, 20, -6);
	Init_FIR_Table(xvid_FIR_6_20_20_7, -6, 20, 20, -7);
	Init_FIR_Table(xvid_FIR_3_6_20_20,  3, -6, 20, 20);
	Init_FIR_Table(xvid_FIR_3_6_19_23,  3, -6, 19, 23);
	Init_FIR_Table(xvid_FIR_1_3_6_20,  -1,  3, -6, 20);
	Init_FIR_Table(xvid_FIR_1_2_3_14,  -1,  2, -3, 14);
	Init_FIR_Table(xvid_FIR_0_1_3_6,    0, -1,  3, -6);
	Init_FIR_Table(xvid_FIR_0_0_1_3,    0,  0, -1,  3);
	Init_FIR_Table(xvid_FIR_0_0_0_1,    0,  0,  0, -1);

}

#endif /* !XVID_AUTO_INCLUDE */

/*****************************************************************************
 * "reference" filters impl. in plain C
 ****************************************************************************/

#ifdef XVID_AUTO_INCLUDE

static
void FUNC_H(uint8_t *Dst, const uint8_t *Src, int32_t H, int32_t BpS, int32_t Rnd)
{
	while(H-->0) {
		int32_t i, k;
		int32_t Sums[SIZE] = { 0 };
		for(i=0; i<=SIZE; ++i)
			for(k=0; k<SIZE; ++k)
				Sums[k] += TABLE[i][k] * Src[i];

		for(i=0; i<SIZE; ++i) {
			int32_t C = ( Sums[i] + 16-Rnd ) >> 5;
			if (C<0) C = 0; else if (C>255) C = 255;
			STORE(Dst[i], C);
		}
		Src += BpS;
		Dst += BpS;
	}
}

static
void FUNC_V(uint8_t *Dst, const uint8_t *Src, int32_t W, int32_t BpS, int32_t Rnd)
{
	while(W-->0) {
		int32_t i, k;
		int32_t Sums[SIZE] = { 0 };
		const uint8_t *S = Src++;
		uint8_t *D = Dst++;
		for(i=0; i<=SIZE; ++i) {
			for(k=0; k<SIZE; ++k)
				Sums[k] += TABLE[i][k] * S[0];
			S += BpS;
		}

		for(i=0; i<SIZE; ++i) {
			int32_t C = ( Sums[i] + 16-Rnd )>>5;
			if (C<0) C = 0; else if (C>255) C = 255;
			STORE(D[0], C);
			D += BpS;
		}
	}
}

static
void FUNC_HA(uint8_t *Dst, const uint8_t *Src, int32_t H, int32_t BpS, int32_t Rnd)
{
	while(H-->0) {
		int32_t i, k;
		int32_t Sums[SIZE] = { 0 };
		for(i=0; i<=SIZE; ++i)
			for(k=0; k<SIZE; ++k)
				Sums[k] += TABLE[i][k] * Src[i];

		for(i=0; i<SIZE; ++i) {
			int32_t C = ( Sums[i] + 16-Rnd ) >> 5;
			if (C<0) C = 0; else if (C>255) C = 255;
			C = (C+Src[i]+1-Rnd) >> 1;
			STORE(Dst[i], C);
		}
		Src += BpS;
		Dst += BpS;
	}
}

static
void FUNC_HA_UP(uint8_t *Dst, const uint8_t *Src, int32_t H, int32_t BpS, int32_t Rnd)
{
	while(H-->0) {
		int32_t i, k;
		int32_t Sums[SIZE] = { 0 };
		for(i=0; i<=SIZE; ++i)
			for(k=0; k<SIZE; ++k)
				Sums[k] += TABLE[i][k] * Src[i];

		for(i=0; i<SIZE; ++i) {
			int32_t C = ( Sums[i] + 16-Rnd ) >> 5;
			if (C<0) C = 0; else if (C>255) C = 255;
			C = (C+Src[i+1]+1-Rnd) >> 1;
			STORE(Dst[i], C);
		}
		Src += BpS;
		Dst += BpS;
	}
}

static
void FUNC_VA(uint8_t *Dst, const uint8_t *Src, int32_t W, int32_t BpS, int32_t Rnd)
{
	while(W-->0) {
		int32_t i, k;
		int32_t Sums[SIZE] = { 0 };
		const uint8_t *S = Src;
		uint8_t *D = Dst;

		for(i=0; i<=SIZE; ++i) {
			for(k=0; k<SIZE; ++k)
				Sums[k] += TABLE[i][k] * S[0];
			S += BpS;
		}

		S = Src;
		for(i=0; i<SIZE; ++i) {
			int32_t C = ( Sums[i] + 16-Rnd )>>5;
			if (C<0) C = 0; else if (C>255) C = 255;
			C = ( C+S[0]+1-Rnd ) >> 1;
			STORE(D[0], C);
			D += BpS;
			S += BpS;
		}
		Src++;
		Dst++;
	}
}

static
void FUNC_VA_UP(uint8_t *Dst, const uint8_t *Src, int32_t W, int32_t BpS, int32_t Rnd)
{
	while(W-->0) {
		int32_t i, k;
		int32_t Sums[SIZE] = { 0 };
		const uint8_t *S = Src;
		uint8_t *D = Dst;

		for(i=0; i<=SIZE; ++i) {
			for(k=0; k<SIZE; ++k)
				Sums[k] += TABLE[i][k] * S[0];
			S += BpS;
		}

		S = Src + BpS;
		for(i=0; i<SIZE; ++i) {
			int32_t C = ( Sums[i] + 16-Rnd )>>5;
			if (C<0) C = 0; else if (C>255) C = 255;
			C = ( C+S[0]+1-Rnd ) >> 1;
			STORE(D[0], C);
			D += BpS;
			S += BpS;
		}
		Dst++;
		Src++;
	}
}

#undef STORE
#undef FUNC_H
#undef FUNC_V
#undef FUNC_HA
#undef FUNC_VA
#undef FUNC_HA_UP
#undef FUNC_VA_UP

#endif /* XVID_AUTO_INCLUDE */
