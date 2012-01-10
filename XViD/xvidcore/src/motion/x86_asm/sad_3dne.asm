;/****************************************************************************
; *
; *  XVID MPEG-4 VIDEO CODEC
; *  - K7 optimized SAD operators -
; *
; *  Copyright(C) 2002 Jaan Kalda
; *
; *  This program is free software; you can redistribute it and/or modify it
; *  under the terms of the GNU General Public License as published by
; *  the Free Software Foundation; either version 2 of the License, or
; *  (at your option) any later version.
; *
; *  This program is distributed in the hope that it will be useful,
; *  but WITHOUT ANY WARRANTY; without even the implied warranty of
; *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; *  GNU General Public License for more details.
; *
; *  You should have received a copy of the GNU General Public License
; *  along with this program; if not, write to the Free Software
; *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
; *
; * $Id$
; *
; ***************************************************************************/

; these 3dne functions are compatible with iSSE, but are optimized specifically
; for K7 pipelines

BITS 32

%macro cglobal 1
	%ifdef PREFIX
		global _%1
		%define %1 _%1
	%else
		global %1
	%endif
%endmacro

;=============================================================================
; Read only data
;=============================================================================

%ifdef FORMAT_COFF
SECTION .rodata
%else
SECTION .rodata align=16
%endif

ALIGN 16
mmx_one:
	times 4	dw 1

;=============================================================================
; Helper macros
;=============================================================================

	;; %1 block number (0..4)
%macro SAD_16x16_SSE 1
  movq mm7, [eax]
  movq mm6, [eax+8]
  psadbw mm7, [edx]
  psadbw mm6, [edx+8]
%if (%1)
  paddd mm1, mm5
%endif
  movq mm5, [eax+ecx]
  movq mm4, [eax+ecx+8]
  psadbw mm5, [edx+ecx]
  psadbw mm4, [edx+ecx+8]
  movq mm3, [eax+2*ecx]
  movq mm2, [eax+2*ecx+8]
  psadbw mm3, [edx+2*ecx]
  psadbw mm2, [edx+2*ecx+8]
%if (%1)
  movd [esp+4*(%1-1)], mm1
%else
  sub esp, byte 12
%endif
  movq mm1, [eax+ebx]
  movq mm0, [eax+ebx+8]
  psadbw mm1, [edx+ebx]
  psadbw mm0, [edx+ebx+8]
  lea eax, [eax+4*ecx]
  lea edx, [edx+4*ecx]
  paddd mm7, mm6
  paddd mm5, mm4
  paddd mm3, mm2
  paddd mm1, mm0
  paddd mm5, mm7
  paddd mm1, mm3
%endmacro

%macro SADBI_16x16_SSE0 0
  movq mm2, [edx]
  movq mm3, [edx+8]

  movq mm5, [byte eax]
  movq mm6, [eax+8]
  pavgb mm2, [byte ebx]
  pavgb mm3, [ebx+8]

  add edx, ecx
  psadbw mm5, mm2
  psadbw mm6, mm3

  add eax, ecx
  add ebx, ecx
  movq mm2, [byte edx]

  movq mm3, [edx+8]
  movq mm0, [byte eax]

  movq mm1, [eax+8]
  pavgb mm2, [byte ebx]

  pavgb mm3, [ebx+8]
  add edx, ecx
  add eax, ecx

  add ebx, ecx
  psadbw mm0, mm2
  psadbw mm1, mm3

%endmacro

%macro SADBI_16x16_SSE 0
  movq mm2, [byte edx]
  movq mm3, [edx+8]
  paddusw mm5, mm0
  paddusw mm6, mm1
  movq mm0, [eax]
  movq mm1, [eax+8]
  pavgb mm2, [ebx]
  pavgb mm3, [ebx+8]
  add edx, ecx
  add eax, ecx
  add ebx, ecx
  psadbw mm0, mm2
  psadbw mm1, mm3
%endmacro

%macro SADBI_8x8_3dne 0
  movq mm2, [edx]
  movq mm3, [edx+ecx]
  pavgb mm2, [eax]
  pavgb mm3, [eax+ecx]
  lea edx, [edx+2*ecx]
  lea eax, [eax+2*ecx]
  paddusw mm5, mm0
  paddusw mm6, mm1
  movq mm0, [ebx]
  movq mm1, [ebx+ecx]
  lea ebx, [ebx+2*ecx]
  psadbw mm0, mm2
  psadbw mm1, mm3
%endmacro

%macro ABS_16x16_SSE 1
%if (%1 == 0)
  movq mm7, [eax]
  psadbw mm7, mm4
  mov esi, esi
  movq mm6, [eax+8]
  movq mm5, [eax+ecx]
  movq mm3, [eax+ecx+8]
  psadbw mm6, mm4

  movq mm2, [byte eax+2*ecx]
  psadbw mm5, mm4
  movq mm1, [eax+2*ecx+8]
  psadbw mm3, mm4

  movq mm0, [dword eax+edx]
  psadbw mm2, mm4
  add eax, edx
  psadbw mm1, mm4
%endif
%if (%1 == 1)
  psadbw mm0, mm4
  paddd mm7, mm0
  movq mm0, [eax+8]
  psadbw mm0, mm4
  paddd mm6, mm0

  movq mm0, [byte eax+ecx]
  psadbw mm0, mm4

  paddd mm5, mm0
  movq mm0, [eax+ecx+8]

  psadbw mm0, mm4
  paddd mm3, mm0
  movq mm0, [eax+2*ecx]
  psadbw mm0, mm4
  paddd mm2, mm0

  movq mm0, [eax+2*ecx+8]
  add eax, edx
  psadbw mm0, mm4
  paddd mm1, mm0
  movq mm0, [eax]
%endif
%if (%1 == 2)
  psadbw mm0, mm4
  paddd mm7, mm0
  movq mm0, [eax+8]
  psadbw mm0, mm4
  paddd mm6, mm0
%endif
%endmacro

;=============================================================================
; Code
;=============================================================================

SECTION .text

cglobal sad16_3dne
cglobal sad8_3dne
cglobal sad16bi_3dne
cglobal sad8bi_3dne
cglobal dev16_3dne

;-----------------------------------------------------------------------------
;
; uint32_t sad16_3dne(const uint8_t * const cur,
;                     const uint8_t * const ref,
;                     const uint32_t stride,
;                     const uint32_t best_sad);
;
;-----------------------------------------------------------------------------

; optimization: 21% faster

ALIGN 16
sad16_3dne:
  mov eax, [esp+ 4] ; Src1
  mov edx, [esp+ 8] ; Src2
  mov ecx, [esp+12] ; Stride
  push ebx
  lea ebx, [2*ecx+ecx]

  SAD_16x16_SSE 0
  SAD_16x16_SSE 1
  SAD_16x16_SSE 2
  SAD_16x16_SSE 3

  mov ecx, [esp]
  add ecx, [esp+4]
  add ecx, [esp+8]
  paddd mm1, mm5
  mov ebx, [esp+12]
  add esp, byte 4+12
  movd eax, mm1
  add eax, ecx

  ret


;-----------------------------------------------------------------------------
;
; uint32_t sad8_3dne(const uint8_t * const cur,
;					const uint8_t * const ref,
;					const uint32_t stride);
;
;-----------------------------------------------------------------------------

ALIGN 16
sad8_3dne:

  mov eax, [esp+ 4] ; Src1
  mov ecx, [esp+12] ; Stride
  mov edx, [esp+ 8] ; Src2
  push ebx
  lea ebx, [ecx+2*ecx]

  movq mm0, [byte eax]      ;0
  psadbw mm0, [byte edx]
  movq mm1, [eax+ecx]       ;1
  psadbw mm1, [edx+ecx]

  movq mm2, [eax+2*ecx]     ;2
  psadbw mm2, [edx+2*ecx]
  movq mm3, [eax+ebx]       ;3
  psadbw mm3, [edx+ebx]

  paddd mm0, mm1

  movq mm4, [byte eax+4*ecx];4
  psadbw mm4, [edx+4*ecx]
  movq mm5, [eax+2*ebx]     ;6
  psadbw mm5, [edx+2*ebx]

  paddd mm2, mm3
  paddd mm0, mm2

  lea ebx, [ebx+4*ecx]      ;3+4=7
  lea ecx, [ecx+4*ecx]      ; 5
  movq mm6, [eax+ecx]       ;5
  psadbw mm6, [edx+ecx]
  movq mm7, [eax+ebx]       ;7
  psadbw mm7, [edx+ebx]
  paddd mm4, mm5
  paddd mm6, mm7
  paddd mm0, mm4
  mov ebx, [esp]
  add esp, byte 4
  paddd mm0, mm6
  movd eax, mm0

 ret


;-----------------------------------------------------------------------------
;
; uint32_t sad16bi_3dne(const uint8_t * const cur,
;					const uint8_t * const ref1,
;					const uint8_t * const ref2,
;					const uint32_t stride);
;
;-----------------------------------------------------------------------------
;optimization: 14% faster

ALIGN 16
sad16bi_3dne:
  mov eax, [esp+ 4] ; Src
  mov edx, [esp+ 8] ; Ref1
  mov ecx, [esp+16] ; Stride
  push ebx
  mov ebx, [esp+4+12] ; Ref2

  SADBI_16x16_SSE0
  SADBI_16x16_SSE
  SADBI_16x16_SSE
  SADBI_16x16_SSE
  SADBI_16x16_SSE
  SADBI_16x16_SSE
  SADBI_16x16_SSE

  SADBI_16x16_SSE
  SADBI_16x16_SSE
  SADBI_16x16_SSE
  SADBI_16x16_SSE
  SADBI_16x16_SSE
  SADBI_16x16_SSE
  SADBI_16x16_SSE
  SADBI_16x16_SSE
  paddusw mm5,mm0
  paddusw mm6,mm1

  pop ebx
  paddusw mm6,mm5
  movd eax, mm6

  ret

;-----------------------------------------------------------------------------
;
; uint32_t sad8bi_3dne(const uint8_t * const cur,
; const uint8_t * const ref1,
; const uint8_t * const ref2,
; const uint32_t stride);
;
;-----------------------------------------------------------------------------

ALIGN 16
sad8bi_3dne:
  mov eax, [esp+12] ; Ref2
  mov edx, [esp+ 8] ; Ref1
  mov ecx, [esp+16] ; Stride
  push ebx
  mov ebx, [esp+4+ 4] ; Src

  movq mm2, [edx]
  movq mm3, [edx+ecx]
  pavgb mm2, [eax]
  pavgb mm3, [eax+ecx]
  lea edx, [edx+2*ecx]
  lea eax, [eax+2*ecx]
  movq mm5, [ebx]
  movq mm6, [ebx+ecx]
  lea ebx, [ebx+2*ecx]
  psadbw mm5, mm2
  psadbw mm6, mm3

  movq mm2, [edx]
  movq mm3, [edx+ecx]
  pavgb mm2, [eax]
  pavgb mm3, [eax+ecx]
  lea edx, [edx+2*ecx]
  lea eax, [eax+2*ecx]
  movq mm0, [ebx]
  movq mm1, [ebx+ecx]
  lea ebx, [ebx+2*ecx]
  psadbw mm0, mm2
  psadbw mm1, mm3

  movq mm2, [edx]
  movq mm3, [edx+ecx]
  pavgb mm2, [eax]
  pavgb mm3, [eax+ecx]
  lea edx, [edx+2*ecx]
  lea eax, [eax+2*ecx]
  paddusw mm5,mm0
  paddusw mm6,mm1
  movq mm0, [ebx]
  movq mm1, [ebx+ecx]
  lea ebx, [ebx+2*ecx]
  psadbw mm0, mm2
  psadbw mm1, mm3

  movq mm2, [edx]
  movq mm3, [edx+ecx]
  pavgb mm2, [eax]
  pavgb mm3, [eax+ecx]
  paddusw mm5,mm0
  paddusw mm6,mm1
  movq mm0, [ebx]
  movq mm1, [ebx+ecx]
  psadbw mm0, mm2
  psadbw mm1, mm3
  paddusw mm5,mm0
  paddusw mm6,mm1

  paddusw mm6,mm5
  mov ebx,[esp]
  add esp,byte 4
  movd eax, mm6

 ret


;===========================================================================
;
; uint32_t dev16_3dne(const uint8_t * const cur,
;					const uint32_t stride);
;
;===========================================================================
; optimization: 25 % faster

ALIGN 16
dev16_3dne:

  mov eax, [esp+ 4] ; Src
  mov ecx, [esp+ 8] ; Stride
  lea edx, [ecx+2*ecx]

  pxor mm4, mm4

ALIGN 8
  ABS_16x16_SSE 0
  ABS_16x16_SSE 1
  ABS_16x16_SSE 1
  ABS_16x16_SSE 1
  ABS_16x16_SSE 1

  paddd mm1, mm2
  paddd mm3, mm5

  ABS_16x16_SSE 2

  paddd mm7, mm6
  paddd mm1, mm3
  mov eax, [esp+ 4]         ; Src
  paddd mm7, mm1
  punpcklbw mm7, mm7        ;xxyyaazz
  pshufw mm4, mm7, 055h     ; mm4 contains the mean


  pxor mm1, mm1

  ABS_16x16_SSE 0
  ABS_16x16_SSE 1
  ABS_16x16_SSE 1
  ABS_16x16_SSE 1
  ABS_16x16_SSE 1

  paddd mm1, mm2
  paddd mm3, mm5

  ABS_16x16_SSE 2

  paddd mm7, mm6
  paddd mm1, mm3
  paddd mm7, mm1
  movd eax, mm7

  ret
