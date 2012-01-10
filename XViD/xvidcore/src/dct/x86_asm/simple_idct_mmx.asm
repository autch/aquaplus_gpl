;/*
; * Simple IDCT MMX
; *
; * Copyright (c) 2001, 2002 Michael Niedermayer <michaelni@gmx.at>
; *
; * This library is free software; you can redistribute it and/or
; * modify it under the terms of the GNU Lesser General Public
; * License as published by the Free Software Foundation; either
; * version 2 of the License, or (at your option) any later version.
; *
; * This library is distributed in the hope that it will be useful,
; * but WITHOUT ANY WARRANTY; without even the implied warranty of
; * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
; * Lesser General Public License for more details.
; *
; * You should have received a copy of the GNU Lesser General Public
; * License along with this library; if not, write to the Free Software
; * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
; *
; * Ported to nasm by Peter Ross <pross@xvid.org>
; */

BITS 32

;=============================================================================
; Macros and other preprocessor constants
;=============================================================================

%macro cglobal 1
	%ifdef PREFIX
		global _%1
		%define %1 _%1
	%else
		global %1
	%endif
%endmacro

%define ROW_SHIFT 11
%define COL_SHIFT 20
%define C0 23170	;cos(i*M_PI/16)*sqrt(2)*(1<<14) + 0.5 = 23170.475006
%define C1 22725	;cos(i*M_PI/16)*sqrt(2)*(1<<14) + 0.5 = 22725.260826
%define C2 21407	;cos(i*M_PI/16)*sqrt(2)*(1<<14) + 0.5 = 21406.727617
%define C3 19266	;cos(i*M_PI/16)*sqrt(2)*(1<<14) + 0.5 = 19265.545870
%define C4 16383	;cos(i*M_PI/16)*sqrt(2)*(1<<14) - 0.5 = 16384.000000
%define C5 12873	;cos(i*M_PI/16)*sqrt(2)*(1<<14) + 0.5 = 12872.826198
%define C6 8867		;cos(i*M_PI/16)*sqrt(2)*(1<<14) + 0.5 = 8866.956905
%define C7 4520		;cos(i*M_PI/16)*sqrt(2)*(1<<14) + 0.5 = 4520.335430

;===========================================================================
; Data (Read Only)
;===========================================================================

%ifdef FORMAT_COFF
SECTION .rodata
%else
SECTION .rodata align=16
%endif

;-----------------------------------------------------------------------------
; Trigonometric Tables
;-----------------------------------------------------------------------------

ALIGN 16
wm1010:
	dw 0, 0xffff, 0, 0xffff

ALIGN 16
d40000:
	dd 0x40000, 0

ALIGN 16
coeffs:
  dw 1<<(ROW_SHIFT-1), 0, 1<<(ROW_SHIFT-1), 0,		; 0
  dw 1<<(ROW_SHIFT-1), 1, 1<<(ROW_SHIFT-1), 0,		; 8

  dw C4,  C4,  C4,  C4		; 16
  dw C4, -C4,  C4, -C4		; 24

  dw C2,  C6,  C2,  C6		; 32
  dw C6, -C2,  C6, -C2		; 40

  dw C1,  C3,  C1,  C3		; 48
  dw C5,  C7,  C5,  C7		; 56

  dw C3, -C7,  C3, -C7		; 64
  dw -C1, -C5, -C1, -C5		; 72

  dw C5, -C1,  C5, -C1		; 80
  dw C7,  C3,  C7,  C3		; 88

  dw C7, -C5,  C7, -C5		; 96
  dw C3, -C1,  C3, -C1		; 104


;===========================================================================
; Helper macros
;===========================================================================

;---------------------------------------------------------------------------
; DC_COND_IDCT
;---------------------------------------------------------------------------

%macro	DC_COND_IDCT	8
%define	src0		%1
%define	src4		%2
%define	src1		%3
%define	src5		%4
%define	dst			%5
%define	rounder_op	%6
%define	rounder_arg	%7
%define	shift		%8
  movq mm0,[src0]               ; R4    R0  r4  r0
  movq mm1,[src4]               ; R6    R2  r6  r2
  movq mm2,[src1]               ; R3    R1  r3  r1
  movq mm3,[src5]               ; R7    R5  r7  r5
  movq mm4,[wm1010]
  pand mm4,mm0
  por mm4,mm1
  por mm4,mm2
  por mm4,mm3
  packssdw mm4,mm4
  movd eax,mm4
  or eax,eax
  jz near .skip1
  movq mm4,[coeffs+16]          ; C4    C4  C4  C4
  pmaddwd mm4,mm0               ; C4R4+C4R0 C4r4+C4r0
  movq mm5,[coeffs+24]          ; -C4   C4  -C4 C4
  pmaddwd mm0,mm5               ; -C4R4+C4R0    -C4r4+C4r0
  movq mm5,[coeffs+32]          ; C6    C2  C6  C2
  pmaddwd mm5,mm1               ; C6R6+C2R2 C6r6+C2r2
  movq mm6,[coeffs+40]          ; -C2   C6  -C2 C6
  pmaddwd mm1,mm6               ; -C2R6+C6R2    -C2r6+C6r2
  movq mm7,[coeffs+48]          ; C3    C1  C3  C1
  pmaddwd mm7,mm2               ; C3R3+C1R1 C3r3+C1r1
  rounder_op mm4, rounder_arg
  movq mm6,mm4                  ; C4R4+C4R0 C4r4+C4r0
  paddd mm4,mm5                 ; A0        a0
  psubd mm6,mm5                 ; A3        a3
  movq mm5,[coeffs+56]          ; C7    C5  C7  C5
  pmaddwd mm5,mm3               ; C7R7+C5R5 C7r7+C5r5
  rounder_op mm0, rounder_arg
  paddd mm1,mm0                 ; A1        a1
  paddd mm0,mm0
  psubd mm0,mm1                 ; A2        a2
  pmaddwd mm2,[coeffs+64]       ; -C7R3+C3R1    -C7r3+C3r1
  paddd mm7,mm5                 ; B0        b0
  movq mm5,[coeffs+72]          ; -C5   -C1 -C5 -C1
  pmaddwd mm5,mm3               ; -C5R7-C1R5    -C5r7-C1r5
  paddd mm7,mm4                 ; A0+B0     a0+b0
  paddd mm4,mm4                 ; 2A0       2a0
  psubd mm4,mm7                 ; A0-B0     a0-b0
  paddd mm5,mm2                 ; B1        b1
  psrad mm7,shift
  psrad mm4,shift
  movq mm2,mm1                  ; A1        a1
  paddd mm1,mm5                 ; A1+B1     a1+b1
  psubd mm2,mm5                 ; A1-B1     a1-b1
  psrad mm1,shift
  psrad mm2,shift
  packssdw mm7,mm1              ; A1+B1 a1+b1   A0+B0   a0+b0
  packssdw mm2,mm4              ; A0-B0 a0-b0   A1-B1   a1-b1
  movq [dst],mm7
  movq mm1,[src1]               ; R3    R1  r3  r1
  movq mm4,[coeffs+80]          ;-C1    C5  -C1     C5
  movq [dst + 24],mm2
  pmaddwd mm4,mm1               ; -C1R3+C5R1    -C1r3+C5r1
  movq mm7,[coeffs+88]          ; C3    C7  C3  C7
  pmaddwd mm1,[coeffs+96]       ; -C5R3+C7R1    -C5r3+C7r1
  pmaddwd mm7,mm3               ; C3R7+C7R5 C3r7+C7r5
  movq mm2,mm0                  ; A2        a2
  pmaddwd mm3,[coeffs+104]      ; -C1R7+C3R5    -C1r7+C3r5
  paddd mm4,mm7                 ; B2        b2
  paddd mm2,mm4                 ; A2+B2     a2+b2
  psubd mm0,mm4                 ; a2-B2     a2-b2
  psrad mm2,shift
  psrad mm0,shift
  movq mm4,mm6                  ; A3        a3
  paddd mm3,mm1                 ; B3        b3
  paddd mm6,mm3                 ; A3+B3     a3+b3
  psubd mm4,mm3                 ; a3-B3     a3-b3
  psrad mm6,shift
  packssdw mm2,mm6              ; A3+B3 a3+b3   A2+B2   a2+b2
  movq [ dst + 8],mm2
  psrad mm4,shift
  packssdw mm4,mm0              ; A2-B2 a2-b2   A3-B3   a3-b3
  movq [ dst + 16],mm4
  jmp short .skip2
.skip1
  pslld mm0,16
  paddd mm0,[d40000]
  psrad mm0,13
  packssdw mm0,mm0
  movq [ dst ],mm0
  movq [ dst + 8],mm0
  movq [ dst + 16],mm0
  movq [ dst + 24],mm0
.skip2
%undef  src0
%undef  src4
%undef  src1
%undef  src5
%undef  dst
%undef  rounder_op
%undef  rounder_arg
%undef  shift
%endmacro

;---------------------------------------------------------------------------
; Z_COND_IDCT
;---------------------------------------------------------------------------

%macro	Z_COND_IDCT	9
%define src0        %1
%define src4        %2
%define src1        %3
%define src5        %4
%define dst         %5
%define rounder_op  %6
%define rounder_arg %7
%define shift       %8
%define bt          %9
  movq mm0,[src0]               ; R4    R0  r4  r0
  movq mm1,[src4]               ; R6    R2  r6  r2
  movq mm2,[src1]               ; R3    R1  r3  r1
  movq mm3,[src5]               ; R7    R5  r7  r5
  movq mm4,mm0
  por mm4,mm1
  por mm4,mm2
  por mm4,mm3
  packssdw mm4,mm4
  movd eax,mm4
  or eax,eax
  jz near bt
  movq mm4,[coeffs+16]          ; C4    C4  C4  C4
  pmaddwd mm4,mm0               ; C4R4+C4R0 C4r4+C4r0
  movq mm5,[coeffs+24]          ; -C4   C4  -C4 C4
  pmaddwd mm0,mm5               ; -C4R4+C4R0    -C4r4+C4r0
  movq mm5,[coeffs+32]          ; C6    C2  C6  C2
  pmaddwd mm5,mm1               ; C6R6+C2R2 C6r6+C2r2
  movq mm6,[coeffs+40]          ; -C2   C6  -C2 C6
  pmaddwd mm1,mm6               ; -C2R6+C6R2    -C2r6+C6r2
  movq mm7,[coeffs+48]          ; C3    C1  C3  C1
  pmaddwd mm7,mm2               ; C3R3+C1R1 C3r3+C1r1
  rounder_op mm4, rounder_arg
  movq mm6,mm4                  ; C4R4+C4R0 C4r4+C4r0
  paddd mm4,mm5                 ; A0        a0
  psubd mm6,mm5                 ; A3        a3
  movq mm5,[coeffs+56]          ; C7    C5  C7  C5
  pmaddwd mm5,mm3               ; C7R7+C5R5 C7r7+C5r5
  rounder_op mm0, rounder_arg
  paddd mm1,mm0                 ; A1        a1
  paddd mm0,mm0
  psubd mm0,mm1                 ; A2        a2
  pmaddwd mm2,[coeffs+64]       ; -C7R3+C3R1    -C7r3+C3r1
  paddd mm7,mm5                 ; B0        b0
  movq mm5,[coeffs+72]          ; -C5   -C1 -C5 -C1
  pmaddwd mm5,mm3               ; -C5R7-C1R5    -C5r7-C1r5
  paddd mm7,mm4                 ; A0+B0     a0+b0
  paddd mm4,mm4                 ; 2A0       2a0
  psubd mm4,mm7                 ; A0-B0     a0-b0
  paddd mm5,mm2                 ; B1        b1
  psrad mm7,shift
  psrad mm4,shift
  movq mm2,mm1                  ; A1        a1
  paddd mm1,mm5                 ; A1+B1     a1+b1
  psubd mm2,mm5                 ; A1-B1     a1-b1
  psrad mm1,shift
  psrad mm2,shift
  packssdw mm7,mm1              ; A1+B1 a1+b1   A0+B0   a0+b0
  packssdw mm2,mm4              ; A0-B0 a0-b0   A1-B1   a1-b1
  movq [ dst ],mm7
  movq mm1,[src1]               ; R3    R1  r3  r1
  movq mm4,[coeffs+80]          ; -C1   C5  -C1     C5
  movq [ dst + 24 ],mm2
  pmaddwd mm4,mm1               ; -C1R3+C5R1    -C1r3+C5r1
  movq mm7,[coeffs+88]          ; C3    C7  C3  C7
  pmaddwd mm1,[coeffs+96]       ; -C5R3+C7R1    -C5r3+C7r1
  pmaddwd mm7,mm3               ; C3R7+C7R5 C3r7+C7r5
  movq mm2,mm0                  ; A2        a2
  pmaddwd mm3,[coeffs+104]      ; -C1R7+C3R5    -C1r7+C3r5
  paddd mm4,mm7                 ; B2        b2
  paddd mm2,mm4                 ; A2+B2     a2+b2
  psubd mm0,mm4                 ; a2-B2     a2-b2
  psrad mm2,shift
  psrad mm0,shift
  movq mm4,mm6                  ; A3        a3
  paddd mm3,mm1                 ; B3        b3
  paddd mm6,mm3                 ; A3+B3     a3+b3
  psubd mm4,mm3                 ; a3-B3     a3-b3
  psrad mm6,shift
  packssdw mm2,mm6              ; A3+B3 a3+b3   A2+B2   a2+b2
  movq [ dst + 8],mm2
  psrad mm4,shift
  packssdw mm4,mm0              ; A2-B2 a2-b2   A3-B3   a3-b3
  movq [dst + 16],mm4
%undef	src0
%undef	src4
%undef	src1
%undef	src5
%undef	dst
%undef	rounder_op
%undef	rounder_arg
%undef	shift
%undef	bt
%endmacro

;---------------------------------------------------------------------------
; IDCT0
;---------------------------------------------------------------------------

%macro	IDCT0		8
%define	src0		%1
%define src4        %2
%define src1        %3
%define src5        %4
%define dst         %5
%define rounder_op  %6
%define rounder_arg %7
%define shift       %8
  movq mm0,[src0]               ; R4    R0  r4  r0
  movq mm1,[src4]               ; R6    R2  r6  r2
  movq mm2,[src1]               ; R3    R1  r3  r1
  movq mm3,[src5]               ; R7    R5  r7  r5
  movq mm4,[coeffs+16]          ; C4    C4  C4  C4
  pmaddwd mm4,mm0               ; C4R4+C4R0 C4r4+C4r0
  movq mm5,[coeffs+24]          ; -C4   C4  -C4 C4
  pmaddwd mm0,mm5               ; -C4R4+C4R0    -C4r4+C4r0
  movq mm5,[coeffs+32]          ; C6    C2  C6  C2
  pmaddwd mm5,mm1               ; C6R6+C2R2 C6r6+C2r2
  movq mm6,[coeffs+40]          ; -C2   C6  -C2 C6
  pmaddwd mm1,mm6               ; -C2R6+C6R2    -C2r6+C6r2
  ; rounder_op mm4, rounder_arg
  movq mm6,mm4                  ; C4R4+C4R0 C4r4+C4r0
  movq mm7,[coeffs+48]          ; C3    C1  C3  C1
  ; rounder_op mm0, rounder_arg
  pmaddwd mm7,mm2               ; C3R3+C1R1 C3r3+C1r1
  paddd mm4,mm5                 ; A0        a0
  psubd mm6,mm5                 ; A3        a3
  movq mm5,mm0                  ; -C4R4+C4R0    -C4r4+C4r0
  paddd mm0,mm1                 ; A1        a1
  psubd mm5,mm1                 ; A2        a2
  movq mm1,[coeffs+56]          ; C7    C5  C7  C5
  pmaddwd mm1,mm3               ; C7R7+C5R5 C7r7+C5r5
  pmaddwd mm2,[coeffs+64]       ; -C7R3+C3R1    -C7r3+C3r1
  paddd mm7,mm1                 ; B0        b0
  movq mm1,[coeffs+72]          ; -C5   -C1 -C5 -C1
  pmaddwd mm1,mm3               ; -C5R7-C1R5    -C5r7-C1r5
  paddd mm7,mm4                 ; A0+B0     a0+b0
  paddd mm4,mm4                 ; 2A0       2a0
  psubd mm4,mm7                 ; A0-B0     a0-b0
  paddd mm1,mm2                 ; B1        b1
  psrad mm7,shift
  psrad mm4,shift
  movq mm2,mm0                  ; A1        a1
  paddd mm0,mm1                 ; A1+B1     a1+b1
  psubd mm2,mm1                 ; A1-B1     a1-b1
  psrad mm0,shift
  psrad mm2,shift
  packssdw mm7,mm7              ; A0+B0 a0+b0
  movd [ dst ],mm7
  packssdw mm0,mm0              ; A1+B1 a1+b1
  movd [ dst + 16],mm0
  packssdw mm2,mm2              ; A1-B1 a1-b1
  movd [ dst + 96 ],mm2
  packssdw mm4,mm4              ; A0-B0 a0-b0
  movd [ dst + 112],mm4
  movq mm0,[src1]               ; R3    R1  r3  r1
  movq mm4,[coeffs+80]          ; -C1   C5  -C1     C5
  pmaddwd mm4,mm0               ; -C1R3+C5R1    -C1r3+C5r1
  movq mm7,[coeffs+88]          ; C3    C7  C3  C7
  pmaddwd mm0,[coeffs+96]       ; -C5R3+C7R1    -C5r3+C7r1
  pmaddwd mm7,mm3               ; C3R7+C7R5 C3r7+C7r5
  movq mm2,mm5                  ; A2        a2
  pmaddwd mm3,[coeffs+104]      ; -C1R7+C3R5    -C1r7+C3r5
  paddd mm4,mm7                 ; B2        b2
  paddd mm2,mm4                 ; A2+B2     a2+b2
  psubd mm5,mm4                 ; a2-B2     a2-b2
  psrad mm2,shift
  psrad mm5,shift
  movq mm4,mm6                  ; A3        a3
  paddd mm3,mm0                 ; B3        b3
  paddd mm6,mm3                 ; A3+B3     a3+b3
  psubd mm4,mm3                 ; a3-B3     a3-b3
  psrad mm6,shift
  psrad mm4,shift
  packssdw mm2,mm2              ; A2+B2 a2+b2
  packssdw mm6,mm6              ; A3+B3 a3+b3
  movd [ dst + 32 ],mm2
  packssdw mm4,mm4              ; A3-B3 a3-b3
  packssdw mm5,mm5              ; A2-B2 a2-b2
  movd [ dst + 48 ],mm6
  movd [ dst + 64 ],mm4
  movd [ dst + 80 ],mm5
%undef	src0
%undef	src4
%undef	src1
%undef	src5
%undef	dst
%undef	rounder_op
%undef	rounder_arg
%undef	shift
%endmacro

;---------------------------------------------------------------------------
; IDCT4
;---------------------------------------------------------------------------

%macro	IDCT4		8
%define	src0		%1
%define	src4		%2
%define	src1		%3
%define	src5		%4
%define	dst			%5
%define	rounder_op	%6
%define	rounder_arg	%7
%define	shift		%8
  movq mm0,[src0]               ; R4    R0  r4  r0
  movq mm1,[src4]               ; R6    R2  r6  r2
  movq mm3,[src5]               ; R7    R5  r7  r5
  movq mm4,[coeffs+16]          ; C4    C4  C4  C4
  pmaddwd mm4,mm0               ; C4R4+C4R0 C4r4+C4r0
  movq mm5,[coeffs+24]          ; -C4   C4  -C4 C4
  pmaddwd mm0,mm5               ; -C4R4+C4R0    -C4r4+C4r0
  movq mm5,[coeffs+32]          ; C6    C2  C6  C2
  pmaddwd mm5,mm1               ; C6R6+C2R2 C6r6+C2r2
  movq mm6,[coeffs+40]          ; -C2   C6  -C2 C6
  pmaddwd mm1,mm6               ; -C2R6+C6R2    -C2r6+C6r2
  ; rounder_op mm4, rounder_arg
  movq mm6,mm4                  ; C4R4+C4R0 C4r4+C4r0
  ; rounder_op mm0, rounder_arg
  paddd mm4,mm5                 ; A0        a0
  psubd mm6,mm5                 ; A3        a3
  movq mm5,mm0                  ; -C4R4+C4R0    -C4r4+C4r0
  paddd mm0,mm1                 ; A1        a1
  psubd mm5,mm1                 ; A2        a2
  movq mm1,[coeffs+56]          ; C7    C5  C7  C5
  pmaddwd mm1,mm3               ; C7R7+C5R5 C7r7+C5r5
  movq mm7,[coeffs+72]          ; -C5   -C1 -C5 -C1
  pmaddwd mm7,mm3               ; -C5R7-C1R5    -C5r7-C1r5
  paddd mm1,mm4                 ; A0+B0     a0+b0
  paddd mm4,mm4                 ; 2A0       2a0
  psubd mm4,mm1                 ; A0-B0     a0-b0
  psrad mm1,shift
  psrad mm4,shift
  movq mm2,mm0                  ; A1        a1
  paddd mm0,mm7                 ; A1+B1     a1+b1
  psubd mm2,mm7                 ; A1-B1     a1-b1
  psrad mm0,shift
  psrad mm2,shift
  packssdw mm1,mm1              ; A0+B0 a0+b0
  movd [ dst ],mm1
  packssdw mm0,mm0              ; A1+B1 a1+b1
  movd [ dst + 16 ],mm0
  packssdw mm2,mm2              ; A1-B1 a1-b1
  movd [ dst + 96 ],mm2
  packssdw mm4,mm4              ; A0-B0 a0-b0
  movd [ dst + 112 ],mm4
  movq mm1,[coeffs+88]          ; C3    C7  C3  C7
  pmaddwd mm1,mm3               ; C3R7+C7R5 C3r7+C7r5
  movq mm2,mm5                  ; A2        a2
  pmaddwd mm3,[coeffs+104]      ; -C1R7+C3R5    -C1r7+C3r5
  paddd mm2,mm1                 ; A2+B2     a2+b2
  psubd mm5,mm1                 ; a2-B2     a2-b2
  psrad mm2,shift
  psrad mm5,shift
  movq mm1,mm6                  ; A3        a3
  paddd mm6,mm3                 ; A3+B3     a3+b3
  psubd mm1,mm3                 ; a3-B3     a3-b3
  psrad mm6,shift
  psrad mm1,shift
  packssdw mm2,mm2              ; A2+B2 a2+b2
  packssdw mm6,mm6              ; A3+B3 a3+b3
  movd [dst + 32],mm2
  packssdw mm1,mm1              ; A3-B3 a3-b3
  packssdw mm5,mm5              ; A2-B2 a2-b2
  movd [dst + 48],mm6
  movd [dst + 64],mm1
  movd [dst + 80],mm5
%undef	src0
%undef	src4
%undef	src1
%undef	src5
%undef	dst
%undef	rounder_op
%undef	rounder_arg
%undef	shift
%endmacro

;---------------------------------------------------------------------------
; IDCT6
;---------------------------------------------------------------------------

%macro	IDCT6		8
%define	src0		%1
%define	src4		%2
%define	src1		%3
%define	src5		%4
%define	dst			%5
%define	rounder_op	%6
%define	rounder_arg	%7
%define	shift		%8
  movq mm0,[src0]               ; R4    R0  r4  r0
  movq mm3,[src5]               ; R7    R5  r7  r5
  movq mm4,[coeffs+16]          ; C4    C4  C4  C4
  pmaddwd mm4,mm0               ; C4R4+C4R0 C4r4+C4r0
  movq mm5,[coeffs+24]          ; -C4   C4  -C4 C4
  pmaddwd mm0,mm5               ; -C4R4+C4R0    -C4r4+C4r0
  ; rounder_op mm4, rounder_arg
  movq mm6,mm4                  ; C4R4+C4R0 C4r4+C4r0
  ; rounder_op mm0, rounder_arg
  movq mm5,mm0                  ; -C4R4+C4R0    -C4r4+C4r0
  movq mm1,[coeffs+56]          ; C7    C5  C7  C5
  pmaddwd mm1,mm3               ; C7R7+C5R5 C7r7+C5r5
  movq mm7,[coeffs+72]          ; -C5   -C1 -C5 -C1
  pmaddwd mm7,mm3               ; -C5R7-C1R5    -C5r7-C1r5
  paddd mm1,mm4                 ; A0+B0     a0+b0
  paddd mm4,mm4                 ; 2A0       2a0
  psubd mm4,mm1                 ; A0-B0     a0-b0
  psrad mm1,shift
  psrad mm4,shift
  movq mm2,mm0                  ; A1        a1
  paddd mm0,mm7                 ; A1+B1     a1+b1
  psubd mm2,mm7                 ; A1-B1     a1-b1
  psrad mm0,shift
  psrad mm2,shift
  packssdw mm1,mm1              ; A0+B0 a0+b0
  movd [ dst ],mm1
  packssdw mm0,mm0              ; A1+B1 a1+b1
  movd [ dst + 16 ],mm0
  packssdw mm2,mm2              ; A1-B1 a1-b1
  movd [ dst + 96 ],mm2
  packssdw mm4,mm4              ; A0-B0 a0-b0
  movd [ dst + 112 ],mm4
  movq mm1,[coeffs+88]          ; C3    C7  C3  C7
  pmaddwd mm1,mm3               ; C3R7+C7R5 C3r7+C7r5
  movq mm2,mm5                  ; A2        a2
  pmaddwd mm3,[coeffs+104]      ; -C1R7+C3R5    -C1r7+C3r5
  paddd mm2,mm1                 ; A2+B2     a2+b2
  psubd mm5,mm1                 ; a2-B2     a2-b2
  psrad mm2,shift
  psrad mm5,shift
  movq mm1,mm6                  ; A3        a3
  paddd mm6,mm3                 ; A3+B3     a3+b3
  psubd mm1,mm3                 ; a3-B3     a3-b3
  psrad mm6,shift
  psrad mm1,shift
  packssdw mm2,mm2              ; A2+B2 a2+b2
  packssdw mm6,mm6              ; A3+B3 a3+b3
  movd [dst + 32],mm2
  packssdw mm1,mm1              ; A3-B3 a3-b3
  packssdw mm5,mm5              ; A2-B2 a2-b2
  movd [dst + 48],mm6
  movd [dst + 64],mm1
  movd [dst + 80],mm5
%undef  src0
%undef  src4
%undef  src1
%undef  src5
%undef  dst
%undef  rounder_op
%undef  rounder_arg
%undef	shift
%endmacro

;---------------------------------------------------------------------------
; IDCT2
;---------------------------------------------------------------------------

%macro	IDCT2		8
%define	src0		%1
%define	src4		%2
%define	src1		%3
%define	src5		%4
%define	dst			%5
%define	rounder_op	%6
%define	rounder_arg	%7
%define shift       %8
  movq mm0,[src0]               ; R4    R0  r4  r0
  movq mm2,[src1]               ; R3    R1  r3  r1
  movq mm3,[src5]               ; R7    R5  r7  r5
  movq mm4,[coeffs+16]          ; C4    C4  C4  C4
  pmaddwd mm4,mm0               ; C4R4+C4R0 C4r4+C4r0
  movq mm5,[coeffs+24]          ; -C4   C4  -C4 C4
  pmaddwd mm0,mm5               ; -C4R4+C4R0    -C4r4+C4r0
  ; rounder_op mm4, rounder_arg
  movq mm6,mm4                  ; C4R4+C4R0 C4r4+C4r0
  movq mm7,[coeffs+48]          ; C3    C1  C3  C1
  ; rounder_op mm0, rounder_arg
  pmaddwd mm7,mm2               ; C3R3+C1R1 C3r3+C1r1
  movq mm5,mm0                  ; -C4R4+C4R0    -C4r4+C4r0
  movq mm1,[coeffs+56]          ; C7    C5  C7  C5
  pmaddwd mm1,mm3               ; C7R7+C5R5 C7r7+C5r5
  pmaddwd mm2,[coeffs+64]       ; -C7R3+C3R1    -C7r3+C3r1
  paddd mm7,mm1                 ; B0        b0
  movq mm1,[coeffs+72]          ; -C5   -C1 -C5 -C1
  pmaddwd mm1,mm3               ; -C5R7-C1R5    -C5r7-C1r5
  paddd mm7,mm4                 ; A0+B0     a0+b0
  paddd mm4,mm4                 ; 2A0       2a0
  psubd mm4,mm7                 ; A0-B0     a0-b0
  paddd mm1,mm2                 ; B1        b1
  psrad mm7,shift
  psrad mm4,shift
  movq mm2,mm0                  ; A1        a1
  paddd mm0,mm1                 ; A1+B1     a1+b1
  psubd mm2,mm1                 ; A1-B1     a1-b1
  psrad mm0,shift
  psrad mm2,shift
  packssdw mm7,mm7              ; A0+B0 a0+b0
  movd [dst],mm7
  packssdw mm0,mm0              ; A1+B1 a1+b1
  movd [dst + 16],mm0
  packssdw mm2,mm2              ; A1-B1 a1-b1
  movd [dst + 96],mm2
  packssdw mm4,mm4              ; A0-B0 a0-b0
  movd [dst + 112],mm4
  movq mm0,[src1]               ; R3    R1  r3  r1
  movq mm4,[coeffs+80]          ; -C1   C5  -C1     C5
  pmaddwd mm4,mm0               ; -C1R3+C5R1    -C1r3+C5r1
  movq mm7,[coeffs+88]          ; C3    C7  C3  C7
  pmaddwd mm0,[coeffs+96]       ; -C5R3+C7R1    -C5r3+C7r1
  pmaddwd mm7,mm3               ; C3R7+C7R5 C3r7+C7r5
  movq mm2,mm5                  ; A2        a2
  pmaddwd mm3,[coeffs+104]      ; -C1R7+C3R5    -C1r7+C3r5
  paddd mm4,mm7                 ; B2        b2
  paddd mm2,mm4                 ; A2+B2     a2+b2
  psubd mm5,mm4                 ; a2-B2     a2-b2
  psrad mm2,shift
  psrad mm5,shift
  movq mm4,mm6                  ; A3        a3
  paddd mm3,mm0                 ; B3        b3
  paddd mm6,mm3                 ; A3+B3     a3+b3
  psubd mm4,mm3                 ; a3-B3     a3-b3
  psrad mm6,shift
  psrad mm4,shift
  packssdw mm2,mm2              ; A2+B2 a2+b2
  packssdw mm6,mm6              ; A3+B3 a3+b3
  movd [dst + 32],mm2
  packssdw mm4,mm4              ; A3-B3 a3-b3
  packssdw mm5,mm5              ; A2-B2 a2-b2
  movd [dst + 48],mm6
  movd [dst + 64],mm4
  movd [dst + 80],mm5
%undef  src0
%undef  src4
%undef  src1
%undef  src5
%undef  dst
%undef  rounder_op
%undef  rounder_arg
%undef  shift
%endmacro

;---------------------------------------------------------------------------
; IDCT3
;---------------------------------------------------------------------------

%macro  IDCT3       8
%define src0        %1
%define src4        %2
%define src1        %3
%define src5        %4
%define dst         %5
%define rounder_op  %6
%define rounder_arg %7
%define shift       %8
  movq mm0,[src0]               ; R4    R0  r4  r0
  movq mm2,[src1]               ; R3    R1  r3  r1
  movq mm4,[coeffs+16]          ; C4    C4  C4  C4
  pmaddwd mm4,mm0               ; C4R4+C4R0 C4r4+C4r0
  movq mm5,[coeffs+24]          ; -C4   C4  -C4 C4
  pmaddwd mm0,mm5               ; -C4R4+C4R0    -C4r4+C4r0
  ; rounder_op mm4, rounder_arg
  movq mm6,mm4                  ; C4R4+C4R0 C4r4+C4r0
  movq mm7,[coeffs+48]          ; C3    C1  C3  C1
  ; rounder_op mm0, rounder_arg
  pmaddwd mm7,mm2               ; C3R3+C1R1 C3r3+C1r1
  movq mm5,mm0                  ; -C4R4+C4R0    -C4r4+C4r0
  movq mm3,[coeffs+64]
  pmaddwd mm3,mm2               ; -C7R3+C3R1    -C7r3+C3r1
  paddd mm7,mm4                 ; A0+B0     a0+b0
  paddd mm4,mm4                 ; 2A0       2a0
  psubd mm4,mm7                 ; A0-B0     a0-b0
  psrad mm7,shift
  psrad mm4,shift
  movq mm1,mm0                  ; A1        a1
  paddd mm0,mm3                 ; A1+B1     a1+b1
  psubd mm1,mm3                 ; A1-B1     a1-b1
  psrad mm0,shift
  psrad mm1,shift
  packssdw mm7,mm7              ; A0+B0 a0+b0
  movd [dst],mm7
  packssdw mm0,mm0              ; A1+B1 a1+b1
  movd [dst + 16],mm0
  packssdw mm1,mm1              ; A1-B1 a1-b1
  movd [dst + 96],mm1
  packssdw mm4,mm4              ; A0-B0 a0-b0
  movd [dst + 112],mm4
  movq mm4,[coeffs+80]          ; -C1   C5  -C1     C5
  pmaddwd mm4,mm2               ; -C1R3+C5R1    -C1r3+C5r1
  pmaddwd mm2,[coeffs+96]       ; -C5R3+C7R1    -C5r3+C7r1
  movq mm1,mm5                  ; A2        a2
  paddd mm1,mm4                 ; A2+B2     a2+b2
  psubd mm5,mm4                 ; a2-B2     a2-b2
  psrad mm1,shift
  psrad mm5,shift
  movq mm4,mm6                  ; A3        a3
  paddd mm6,mm2                 ; A3+B3     a3+b3
  psubd mm4,mm2                 ; a3-B3     a3-b3
  psrad mm6,shift
  psrad mm4,shift
  packssdw mm1,mm1              ; A2+B2 a2+b2
  packssdw mm6,mm6              ; A3+B3 a3+b3
  movd [dst + 32],mm1
  packssdw mm4,mm4              ; A3-B3 a3-b3
  packssdw mm5,mm5              ; A2-B2 a2-b2
  movd [dst + 48],mm6
  movd [dst + 64],mm4
  movd [dst + 80],mm5
%undef  src0
%undef	src4
%undef	src1
%undef	src5
%undef	dst
%undef	rounder_op
%undef	rounder_arg
%undef	shift
%endmacro

;---------------------------------------------------------------------------
; IDCT5
;---------------------------------------------------------------------------

%macro	IDCT5		8
%define	src0		%1
%define	src4		%2
%define	src1		%3
%define	src5		%4
%define	dst			%5
%define	rounder_op	%6
%define	rounder_arg	%7
%define	shift		%8
  movq mm0,[src0]               ; R4    R0  r4  r0
  movq mm1,[src4]               ; R6    R2  r6  r2
  movq mm4,[coeffs+16]          ; C4    C4  C4  C4
  pmaddwd mm4,mm0               ; C4R4+C4R0 C4r4+C4r0
  movq mm5,[coeffs+24]          ; -C4   C4  -C4 C4
  pmaddwd mm0,mm5               ; -C4R4+C4R0    -C4r4+C4r0
  movq mm5,[coeffs+32]          ; C6    C2  C6  C2
  pmaddwd mm5,mm1               ; C6R6+C2R2 C6r6+C2r2
  movq mm6,[coeffs+40]          ; -C2   C6  -C2 C6
  pmaddwd mm1,mm6               ; -C2R6+C6R2    -C2r6+C6r2
  ; rounder_op mm4, rounder_arg
  movq mm6,mm4                  ; C4R4+C4R0 C4r4+C4r0
  paddd mm4,mm5                 ; A0        a0
  ; rounder_op mm0, rounder_arg
  psubd mm6,mm5                 ; A3        a3
  movq mm5,mm0                  ; -C4R4+C4R0    -C4r4+C4r0
  paddd mm0,mm1                 ; A1        a1
  psubd mm5,mm1                 ; A2        a2
  movq mm2,[src0 + 8]           ; R4    R0  r4  r0
  movq mm3,[src4 + 8]           ; R6    R2  r6  r2
  movq mm1,[coeffs+16]          ; C4    C4  C4  C4
  pmaddwd mm1,mm2               ; C4R4+C4R0 C4r4+C4r0
  movq mm7,[coeffs+24]          ; -C4   C4  -C4 C4
  pmaddwd mm2,mm7               ; -C4R4+C4R0    -C4r4+C4r0
  movq mm7,[coeffs+32]          ; C6    C2  C6  C2
  pmaddwd mm7,mm3               ; C6R6+C2R2 C6r6+C2r2
  pmaddwd mm3,[coeffs+40]       ; -C2R6+C6R2    -C2r6+C6r2
  ; rounder_op mm1, rounder_arg
  paddd mm7,mm1                 ; A0        a0
  paddd mm1,mm1                 ; 2C0       2c0
  ; rounder_op mm2, rounder_arg
  psubd mm1,mm7                 ; A3        a3
  paddd mm3,mm2                 ; A1        a1
  paddd mm2,mm2                 ; 2C1       2c1
  psubd mm2,mm3                 ; A2        a2
  psrad mm4,shift
  psrad mm7,shift
  psrad mm3,shift
  packssdw mm4,mm7              ; A0    a0
  movq [dst],mm4
  psrad mm0,shift
  packssdw mm0,mm3              ; A1    a1
  movq [dst + 16],mm0
  movq [dst + 96],mm0
  movq [dst + 112],mm4
  psrad mm5,shift
  psrad mm6,shift
  psrad mm2,shift
  packssdw mm5,mm2              ; A2-B2 a2-b2
  movq [dst + 32],mm5
  psrad mm1,shift
  packssdw mm6,mm1              ; A3+B3 a3+b3
  movq [dst + 48],mm6
  movq [dst + 64],mm6
  movq [dst + 80],mm5
%undef  src0
%undef  src4
%undef  src1
%undef  src5
%undef  dst
%undef  rounder_op
%undef	rounder_arg
%undef	shift
%endmacro

;---------------------------------------------------------------------------
; IDCT1
;---------------------------------------------------------------------------

%macro	IDCT1		8
%define	src0		%1
%define	src4		%2
%define	src1		%3
%define	src5		%4
%define dst         %5
%define rounder_op  %6
%define rounder_arg %7
%define shift       %8
  movq mm0,[src0]               ; R4    R0  r4  r0
  movq mm1,[src4]               ; R6    R2  r6  r2
  movq mm2,[src1]               ; R3    R1  r3  r1
  movq mm4,[coeffs+16]          ; C4    C4  C4  C4
  pmaddwd mm4,mm0               ; C4R4+C4R0 C4r4+C4r0
  movq mm5,[coeffs+24]          ; -C4   C4  -C4 C4
  pmaddwd mm0,mm5               ; -C4R4+C4R0    -C4r4+C4r0
  movq mm5,[coeffs+32]          ; C6    C2  C6  C2
  pmaddwd mm5,mm1               ; C6R6+C2R2 C6r6+C2r2
  movq mm6,[coeffs+40]          ; -C2   C6  -C2 C6
  pmaddwd mm1,mm6               ; -C2R6+C6R2    -C2r6+C6r2
  ; rounder_op mm4, rounder_arg
  movq mm6,mm4                  ; C4R4+C4R0 C4r4+C4r0
  movq mm7,[coeffs+48]          ; C3    C1  C3  C1
  ; rounder_op mm0, rounder_arg
  pmaddwd mm7,mm2               ; C3R3+C1R1 C3r3+C1r1
  paddd mm4,mm5                 ; A0        a0
  psubd mm6,mm5                 ; A3        a3
  movq mm5,mm0                  ; -C4R4+C4R0    -C4r4+C4r0
  paddd mm0,mm1                 ; A1        a1
  psubd mm5,mm1                 ; A2        a2
  movq mm1,[coeffs+64]
  pmaddwd mm1,mm2               ; -C7R3+C3R1    -C7r3+C3r1
  paddd mm7,mm4                 ; A0+B0     a0+b0
  paddd mm4,mm4                 ; 2A0       2a0
  psubd mm4,mm7                 ; A0-B0     a0-b0
  psrad mm7,shift
  psrad mm4,shift
  movq mm3,mm0                  ; A1        a1
  paddd mm0,mm1                 ; A1+B1     a1+b1
  psubd mm3,mm1                 ; A1-B1     a1-b1
  psrad mm0,shift
  psrad mm3,shift
  packssdw mm7,mm7              ; A0+B0 a0+b0
  movd [dst],mm7
  packssdw mm0,mm0              ; A1+B1 a1+b1
  movd [dst + 16],mm0
  packssdw mm3,mm3              ; A1-B1 a1-b1
  movd [dst + 96],mm3
  packssdw mm4,mm4              ; A0-B0 a0-b0
  movd [dst + 112],mm4
  movq mm4,[coeffs+80]          ; -C1   C5  -C1     C5
  pmaddwd mm4,mm2               ; -C1R3+C5R1    -C1r3+C5r1
  pmaddwd mm2,[coeffs+96]       ; -C5R3+C7R1    -C5r3+C7r1
  movq mm3,mm5                  ; A2        a2
  paddd mm3,mm4                 ; A2+B2     a2+b2
  psubd mm5,mm4                 ; a2-B2     a2-b2
  psrad mm3,shift
  psrad mm5,shift
  movq mm4,mm6                  ; A3        a3
  paddd mm6,mm2                 ; A3+B3     a3+b3
  psubd mm4,mm2                 ; a3-B3     a3-b3
  psrad mm6,shift
  packssdw mm3,mm3              ; A2+B2 a2+b2
  movd [dst + 32],mm3
  psrad mm4,shift
  packssdw mm6,mm6              ; A3+B3 a3+b3
  movd [dst + 48],mm6
  packssdw mm4,mm4              ; A3-B3 a3-b3
  packssdw mm5,mm5              ; A2-B2 a2-b2
  movd [dst + 64],mm4
  movd [dst + 80],mm5
%undef  src0
%undef  src4
%undef  src1
%undef  src5
%undef  dst
%undef  rounder_op
%undef  rounder_arg
%undef  shift
%endmacro

;---------------------------------------------------------------------------
; IDCT7
;---------------------------------------------------------------------------

%macro	IDCT7		8
%define	src0		%1
%define	src4		%2
%define	src1		%3
%define	src5		%4
%define	dst			%5
%define	rounder_op	%6
%define	rounder_arg	%7
%define	shift		%8
  movq mm0,[src0]               ; R4    R0  r4  r0
  movq mm4,[coeffs+16]          ; C4    C4  C4  C4
  pmaddwd mm4,mm0               ; C4R4+C4R0 C4r4+C4r0
  movq mm5,[coeffs+24]          ; -C4   C4  -C4 C4
  pmaddwd mm0,mm5               ; -C4R4+C4R0    -C4r4+C4r0
  ; rounder_op mm4, rounder_arg
  ; rounder_op mm0, rounder_arg
  psrad mm4,shift
  psrad mm0,shift
  movq mm2,[src0 + 8]           ; R4    R0  r4  r0
  movq mm1,[coeffs+16]          ; C4    C4  C4  C4
  pmaddwd mm1,mm2               ; C4R4+C4R0 C4r4+C4r0
  movq mm7,[coeffs+24]          ; -C4   C4  -C4 C4
  pmaddwd mm2,mm7               ; -C4R4+C4R0    -C4r4+C4r0
  movq mm7,[coeffs+32]          ; C6    C2  C6  C2
  ; rounder_op mm1, rounder_arg
  ; rounder_op mm2, rounder_arg
  psrad mm1,shift
  packssdw mm4,mm1              ; A0    a0
  movq [dst],mm4
  psrad mm2,shift
  packssdw mm0,mm2              ; A1    a1
  movq [dst + 16],mm0
  movq [dst + 96],mm0
  movq [dst + 112],mm4
  movq [dst + 32],mm0
  movq [dst + 48],mm4
  movq [dst + 64],mm4
  movq [dst + 80],mm0
%undef	src0
%undef	src4
%undef	src1
%undef	src5
%undef	dst
%undef	rounder_op
%undef	rounder_arg
%undef	shift
%endmacro

;---------------------------------------------------------------------------
; Permutation helpers
;---------------------------------------------------------------------------

%macro XLODA 2
  mov bx, [srcP+2*%2]  	; get src contents
  mov ax, [srcP+2*%1]  	; get dest contents
  mov [srcP+2*%1], bx     ; store new dest val
%endmacro

%macro XCHGA 2
  mov ax, [srcP+2*%1]  	; get dest contents
  mov [srcP+2*%1], bx     ; store new dest val
%endmacro

%macro XCHGB 2
  mov bx, [srcP+2*%1]	    ; get dest contents
  mov [srcP+2*%1], ax     ; store new dest val
%endmacro

%macro XSTRA 2
  mov [srcP+2*%1], bx     ; store dest val
%endmacro

%macro XSTRB 2
  mov [srcP+2*%1], ax     ; store dest val
%endmacro

;---------------------------------------------------------------------------
; Permutation macro
;---------------------------------------------------------------------------

%macro PERMUTEP 1
%define	srcP		%1
  push ebx

;	XCHGA  0x00, 0x00      ; nothing to do

  XLODA 0x08, 0x01
  XCHGB 0x10, 0x08
  XCHGA 0x20, 0x10
  XCHGB 0x02, 0x20
  XCHGA 0x04, 0x02
  XSTRB 0x01, 0x04

  XLODA 0x09, 0x03
  XCHGB 0x18, 0x09
  XCHGA 0x12, 0x18
  XCHGB 0x24, 0x12
  XSTRA 0x03, 0x24

  XLODA 0x0C, 0x05
  XCHGB 0x11, 0x0C
  XCHGA 0x28, 0x11
  XCHGB 0x30, 0x28
  XCHGA 0x22, 0x30
  XCHGB 0x06, 0x22
  XSTRA 0x05, 0x06

  XLODA 0x0D, 0x07
  XCHGB 0x1C, 0x0D
  XCHGA 0x13, 0x1C
  XCHGB 0x29, 0x13
  XCHGA 0x38, 0x29
  XCHGB 0x32, 0x38
  XCHGA 0x26, 0x32
  XSTRB 0x07, 0x26

  XLODA 0x14, 0x0A
  XCHGB 0x21, 0x14
  XSTRA 0x0A, 0x21

  XLODA 0x19, 0x0B
  XCHGB 0x1A, 0x19
  XCHGA 0x16, 0x1A
  XCHGB 0x25, 0x16
  XCHGA 0x0E, 0x25
  XCHGB 0x15, 0x0E
  XCHGA 0x2C, 0x15
  XCHGB 0x31, 0x2C
  XCHGA 0x2A, 0x31
  XCHGB 0x34, 0x2A
  XCHGA 0x23, 0x34
  XSTRB 0x0B, 0x23

  XLODA 0x1D, 0x0F
  XCHGB 0x1E, 0x1D
  XCHGA 0x17, 0x1E
  XCHGB 0x2D, 0x17
  XCHGA 0x3C, 0x2D
  XCHGB 0x33, 0x3C
  XCHGA 0x2B, 0x33
  XCHGB 0x39, 0x2B
  XCHGA 0x3A, 0x39
  XCHGB 0x36, 0x3A
  XCHGA 0x27, 0x36
  XSTRB 0x0F, 0x27

;	XCHGA  0x1B, 0x1B

;	XCHGA  0x1F, 0x1F

  XLODA 0x35, 0x2E
  XSTRB 0x2E, 0x35

  XLODA 0x3D, 0x2F
  XCHGB 0x3E, 0x3D
  XCHGA 0x37, 0x3E
  XSTRB 0x2F, 0x37

;	XCHGA  0x3B, 0x3B

;	XCHGA  0x3F, 0x3F
  pop ebx
%undef	srcP
%endmacro

;=============================================================================
;  Code
;=============================================================================

SECTION .text

cglobal simple_idct_mmx_P
cglobal simple_idct_mmx

;-----------------------------------------------------------------------------
; void simple_idct_mmx_P(int16_t * const block)
; expects input data to be permutated
;-----------------------------------------------------------------------------

ALIGN 16
simple_idct_mmx_P:
  sub esp, 128
  mov edx, [esp+128+4]

;               src0,   src4,   src1,   src5,   dst,    rndop,  rndarg,     shift,  bt
  DC_COND_IDCT  edx+0,  edx+8,  edx+16, edx+24, esp,    paddd,  [coeffs+8], 11
  Z_COND_IDCT   edx+32, edx+40, edx+48, edx+56, esp+32, paddd,  [coeffs],   11,     .four
  Z_COND_IDCT   edx+64, edx+72, edx+80, edx+88, esp+64, paddd,  [coeffs],   11,     .two
  Z_COND_IDCT   edx+96, edx+104,edx+112,edx+120,esp+96, paddd,  [coeffs],   11,     .one
  IDCT0         esp,    esp+64, esp+32, esp+96, edx,    nop,    0,          20
  IDCT0         esp+8,  esp+72, esp+40, esp+104,edx+4,  nop,    0,          20
  IDCT0         esp+16, esp+80, esp+48, esp+112,edx+8,  nop,    0,          20
  IDCT0         esp+24, esp+88, esp+56, esp+120,edx+12, nop,    0,          20
  jmp .ret

ALIGN 16
.four
  Z_COND_IDCT   edx+64, edx+72, edx+80, edx+88, esp+64, paddd,  [coeffs],   11,     .six
  Z_COND_IDCT   edx+96, edx+104,edx+112,edx+120,esp+96, paddd,  [coeffs],   11,     .five
  IDCT4         esp,    esp+64, esp+32, esp+96, edx,    nop,    0,          20
  IDCT4         esp+8,  esp+72, esp+40, esp+104,edx+4,  nop,    0,          20
  IDCT4         esp+16, esp+80, esp+48, esp+112,edx+8,  nop,    0,          20
  IDCT4         esp+24, esp+88, esp+56, esp+120,edx+12, nop,    0,          20
  jmp .ret

ALIGN 16
.six
  Z_COND_IDCT   edx+96, edx+104,edx+112,edx+120,esp+96, paddd,  [coeffs],   11,     .seven
  IDCT6         esp,    esp+64, esp+32, esp+96, edx,    nop,    0,          20
  IDCT6         esp+8,  esp+72, esp+40, esp+104,edx+4,  nop,    0,          20
  IDCT6         esp+16, esp+80, esp+48, esp+112,edx+8,  nop,    0,          20
  IDCT6         esp+24, esp+88, esp+56, esp+120,edx+12, nop,    0,          20
  jmp .ret

ALIGN 16
.two
  Z_COND_IDCT   edx+96, edx+104,edx+112,edx+120,esp+96, paddd,  [coeffs],   11,     .three
  IDCT2         esp,    esp+64, esp+32, esp+96, edx,    nop,    0,          20
  IDCT2         esp+8,  esp+72, esp+40, esp+104,edx+4,  nop,    0,          20
  IDCT2         esp+16, esp+80, esp+48, esp+112,edx+8,  nop,    0,          20
  IDCT2         esp+24, esp+88, esp+56, esp+120,edx+12, nop,    0,          20
  jmp .ret

ALIGN 16
.three
  IDCT3 		esp,	esp+64,	esp+32,	esp+96,	edx,	nop,	0,			20
  IDCT3 		esp+8,	esp+72,	esp+40,	esp+104,edx+4,	nop,	0,			20
  IDCT3 		esp+16,	esp+80,	esp+48,	esp+112,edx+8,	nop,	0,			20
  IDCT3 		esp+24,	esp+88,	esp+56,	esp+120,edx+12,	nop,	0,			20
  jmp .ret

ALIGN 16
.five
  IDCT5 esp,    esp+64, esp+32, esp+96, edx,    nop,    0,          20
  ; IDCT5       esp+8,  esp+72, esp+40, esp+104,edx+4,  nop,    0,          20
  IDCT5 esp+16, esp+80, esp+48, esp+112,edx+8,  nop,    0,          20
  ; IDCT5       esp+24, esp+88, esp+56, esp+120,edx+12, nop,    0,          20
  jmp .ret

ALIGN 16
.one
  IDCT1         esp,    esp+64, esp+32, esp+96, edx,    nop,    0,          20
  IDCT1         esp+8,  esp+72, esp+40, esp+104,edx+4,  nop,    0,          20
  IDCT1         esp+16, esp+80, esp+48, esp+112,edx+8,  nop,    0,          20
  IDCT1         esp+24, esp+88, esp+56, esp+120,edx+12, nop,    0,          20
  jmp .ret

ALIGN 16
.seven
  IDCT7 esp,    esp+64, esp+32, esp+96, edx,    nop,    0,          20
  ; IDCT7       esp+8,  esp+72, esp+40, esp+104,edx+4,  nop,    0,          20
  IDCT7 esp+16, esp+80, esp+48, esp+112,edx+8,  nop,    0,          20
  ; IDCT7       esp+24, esp+88, esp+56, esp+120,edx+12, nop,    0,          20

.ret
  add esp, 128

  ret


;-----------------------------------------------------------------------------
; void simple_idct_mmx(int16_t * const block)
;
; simple_idct_mmx is the same function as simple_idct_mmx_P above except that
; on entry it will do a fast in-line and in-place permutation on the iDCT parm
; list.  This means that same parm list will also not have to be copied on the
; way out. - trbarry 6/2003
;-----------------------------------------------------------------------------

ALIGN 16
simple_idct_mmx:
  sub esp, 128
  mov edx, [esp+128+4]
  PERMUTEP edx			; permute parm list in place

;               src0,   src4,   src1,   src5,   dst,    rndop,  rndarg,     shift,  bt
  DC_COND_IDCT  edx+0,  edx+8,  edx+16, edx+24, esp,    paddd,  [coeffs+8], 11
  Z_COND_IDCT   edx+32, edx+40, edx+48, edx+56, esp+32, paddd,  [coeffs],   11,     .fourP
  Z_COND_IDCT   edx+64, edx+72, edx+80, edx+88, esp+64, paddd,  [coeffs],   11,     .twoP
  Z_COND_IDCT   edx+96, edx+104,edx+112,edx+120,esp+96, paddd,  [coeffs],   11,     .oneP
  IDCT0         esp,    esp+64, esp+32, esp+96, edx,    nop,    0,          20
  IDCT0         esp+8,  esp+72, esp+40, esp+104,edx+4,  nop,    0,          20
  IDCT0         esp+16, esp+80, esp+48, esp+112,edx+8,  nop,    0,          20
  IDCT0         esp+24, esp+88, esp+56, esp+120,edx+12, nop,    0,          20
  jmp .retP

ALIGN 16
.fourP
  Z_COND_IDCT   edx+64, edx+72, edx+80, edx+88, esp+64, paddd,  [coeffs],   11,     .sixP
  Z_COND_IDCT   edx+96, edx+104,edx+112,edx+120,esp+96, paddd,  [coeffs],   11,     .fiveP
  IDCT4         esp,    esp+64, esp+32, esp+96, edx,    nop,    0,          20
  IDCT4         esp+8,  esp+72, esp+40, esp+104,edx+4,  nop,    0,          20
  IDCT4         esp+16, esp+80, esp+48, esp+112,edx+8,  nop,    0,          20
  IDCT4         esp+24, esp+88, esp+56, esp+120,edx+12, nop,    0,          20
  jmp .retP

ALIGN 16
.sixP
  Z_COND_IDCT   edx+96, edx+104,edx+112,edx+120,esp+96, paddd,  [coeffs],   11,     .sevenP
  IDCT6         esp,    esp+64, esp+32, esp+96, edx,    nop,    0,          20
  IDCT6         esp+8,  esp+72, esp+40, esp+104,edx+4,  nop,    0,          20
  IDCT6         esp+16, esp+80, esp+48, esp+112,edx+8,  nop,    0,          20
  IDCT6         esp+24, esp+88, esp+56, esp+120,edx+12, nop,    0,          20
  jmp .retP

ALIGN 16
.twoP
  Z_COND_IDCT   edx+96, edx+104,edx+112,edx+120,esp+96, paddd,  [coeffs],   11,     .threeP
  IDCT2         esp,    esp+64, esp+32, esp+96, edx,    nop,    0,          20
  IDCT2         esp+8,  esp+72, esp+40, esp+104,edx+4,  nop,    0,          20
  IDCT2         esp+16, esp+80, esp+48, esp+112,edx+8,  nop,    0,          20
  IDCT2         esp+24, esp+88, esp+56, esp+120,edx+12, nop,    0,          20
  jmp .retP

ALIGN 16
.threeP
  IDCT3         esp,    esp+64, esp+32, esp+96, edx,    nop,    0,          20
  IDCT3         esp+8,  esp+72, esp+40, esp+104,edx+4,  nop,    0,          20
  IDCT3         esp+16, esp+80, esp+48, esp+112,edx+8,  nop,    0,          20
  IDCT3         esp+24, esp+88, esp+56, esp+120,edx+12, nop,    0,          20
  jmp .retP

ALIGN 16
.fiveP
  IDCT5         esp,    esp+64, esp+32, esp+96, edx,    nop,    0,          20
  ; IDCT5       esp+8,  esp+72, esp+40, esp+104,edx+4,  nop,    0,          20
  IDCT5         esp+16, esp+80, esp+48, esp+112,edx+8,  nop,    0,          20
  ; IDCT5       esp+24, esp+88, esp+56, esp+120,edx+12, nop,    0,          20
  jmp .retP

ALIGN 16
.oneP
  IDCT1         esp,    esp+64, esp+32, esp+96, edx,    nop,    0,          20
  IDCT1         esp+8,  esp+72, esp+40, esp+104,edx+4,  nop,    0,          20
  IDCT1         esp+16, esp+80, esp+48, esp+112,edx+8,  nop,    0,          20
  IDCT1         esp+24, esp+88, esp+56, esp+120,edx+12, nop,    0,          20
  jmp .retP

ALIGN 16
.sevenP
  IDCT7         esp,    esp+64, esp+32, esp+96, edx,    nop,    0,          20
  ; IDCT7       esp+8,  esp+72, esp+40, esp+104,edx+4,  nop,    0,          20
  IDCT7         esp+16, esp+80, esp+48, esp+112,edx+8,  nop,    0,          20
  ; IDCT7       esp+24, esp+88, esp+56, esp+120,edx+12, nop,    0,          20

.retP
  add esp, 128

  ret
