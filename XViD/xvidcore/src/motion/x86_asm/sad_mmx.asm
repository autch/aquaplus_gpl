;/****************************************************************************
; *
; *  XVID MPEG-4 VIDEO CODEC
; *  - K7 optimized SAD operators -
; *
; *  Copyright(C) 2001 Peter Ross <pross@xvid.org>
; *               2002 Pascal Massimino <skal@planet-d.net>
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

%macro SAD_16x16_MMX 0
  movq mm0, [eax]
  movq mm1, [edx]

  movq mm2, [eax+8]
  movq mm3, [edx+8]

  movq mm4, mm0
  psubusb mm0, mm1
  lea eax, [eax+ecx]
  movq mm5, mm2
  psubusb mm2, mm3
  lea edx, [edx+ecx]

  psubusb mm1, mm4
  por mm0, mm1
  psubusb mm3, mm5
  por mm2, mm3

  movq mm1, mm0
  movq mm3, mm2

  punpcklbw mm0,mm7
  punpckhbw mm1,mm7
  punpcklbw mm2,mm7
  punpckhbw mm3,mm7

  paddusw mm0, mm1
  paddusw mm6, mm0
  paddusw mm2, mm3
  paddusw mm6, mm2
%endmacro

%macro SAD_8x8_MMX	0
  movq mm0, [eax]
  movq mm1, [edx]

  movq mm2, [eax+ecx]
  movq mm3, [edx+ecx]

  lea eax,[eax+2*ecx]
  lea edx,[edx+2*ecx]

  movq mm4, mm0
  psubusb mm0, mm1
  movq mm5, mm2
  psubusb mm2, mm3

  psubusb mm1, mm4
  por mm0, mm1
  psubusb mm3, mm5
  por mm2, mm3

  movq mm1,mm0
  movq mm3,mm2

  punpcklbw mm0,mm7
  punpckhbw mm1,mm7
  punpcklbw mm2,mm7
  punpckhbw mm3,mm7

  paddusw mm0,mm1
  paddusw mm6,mm0
  paddusw mm2,mm3
  paddusw mm6,mm2
%endmacro

%macro SADV_16x16_MMX 0
  movq mm0, [eax]
  movq mm1, [edx]

  movq mm2, [eax+8]
  movq mm3, [edx+8]

  movq mm4, mm0
  psubusb mm0, mm1

  psubusb mm1, mm4
  por mm0, mm1
  lea eax,[eax+ecx]

  movq mm4, mm2
  psubusb mm2, mm3

  psubusb mm3, mm4
  por mm2, mm3
  lea edx,[edx+ecx]

  movq mm1,mm0
  movq mm3,mm2

  punpcklbw mm0,mm7
  punpckhbw mm1,mm7
  punpcklbw mm2,mm7
  punpckhbw mm3,mm7

  paddusw mm0,mm1
  paddusw mm2,mm3

  paddusw mm5, mm0
  paddusw mm6, mm2
%endmacro

%macro SADBI_16x16_MMX 2    ; SADBI_16x16_MMX( int_ptr_offset, bool_increment_ptr );

  movq mm0, [edx+%1]
  movq mm2, [ebx+%1]
  movq mm1, mm0
  movq mm3, mm2

%if %2 != 0
  add edx, ecx
%endif

  punpcklbw mm0, mm7
  punpckhbw mm1, mm7
  punpcklbw mm2, mm7
  punpckhbw mm3, mm7

%if %2 != 0
  add ebx, ecx
%endif

  paddusw mm0, mm2              ; mm01 = ref1 + ref2
  paddusw mm1, mm3
  paddusw mm0, [mmx_one]        ; mm01 += 1
  paddusw mm1, [mmx_one]
  psrlw mm0, 1                  ; mm01 >>= 1
  psrlw mm1, 1

  movq mm2, [eax+%1]
  movq mm3, mm2
  punpcklbw mm2, mm7            ; mm23 = src
  punpckhbw mm3, mm7

%if %2 != 0
  add eax, ecx
%endif

  movq mm4, mm0
  movq mm5, mm1
  psubusw mm0, mm2
  psubusw mm1, mm3
  psubusw mm2, mm4
  psubusw mm3, mm5
  por mm0, mm2                  ; mm01 = ABS(mm01 - mm23)
  por mm1, mm3

  paddusw mm6, mm0              ; mm6 += mm01
  paddusw mm6, mm1

%endmacro

%macro MEAN_16x16_MMX 0
  movq mm0, [eax]
  movq mm2, [eax+8]
  lea eax, [eax+ecx]
  movq mm1, mm0
  movq mm3, mm2
  punpcklbw mm0, mm7
  punpcklbw mm2, mm7
  punpckhbw mm1, mm7
  punpckhbw mm3, mm7
  paddw mm5, mm0
  paddw mm6, mm1
  paddw mm5, mm2
  paddw mm6, mm3
%endmacro

%macro ABS_16x16_MMX 0
  movq mm0, [eax]
  movq mm2, [eax+8]
  lea eax, [eax+ecx]
  movq mm1, mm0
  movq mm3, mm2
  punpcklbw mm0, mm7
  punpcklbw mm2, mm7
  punpckhbw mm1, mm7
  punpckhbw mm3, mm7
  movq mm4, mm6
  psubusw mm4, mm0

  psubusw mm0, mm6
  por mm0, mm4
  movq mm4, mm6
  psubusw mm4, mm1
  psubusw mm1, mm6
  por mm1, mm4

  movq mm4, mm6
  psubusw mm4, mm2
  psubusw mm2, mm6
  por mm2, mm4
  movq mm4, mm6
  psubusw mm4, mm3
  psubusw mm3, mm6
  por mm3, mm4

  paddw mm0, mm1
  paddw mm2, mm3
  paddw mm5, mm0
  paddw mm5, mm2
%endmacro

;=============================================================================
; Code
;=============================================================================

SECTION .text

cglobal sad16_mmx
cglobal sad16v_mmx
cglobal sad8_mmx
cglobal sad16bi_mmx
cglobal sad8bi_mmx
cglobal dev16_mmx
cglobal sse8_16bit_mmx
	
;-----------------------------------------------------------------------------
;
; uint32_t sad16_mmx(const uint8_t * const cur,
;					 const uint8_t * const ref,
;					 const uint32_t stride,
;					 const uint32_t best_sad);
;
; (early termination ignore; slows this down)
;
;-----------------------------------------------------------------------------

ALIGN 16
sad16_mmx:

  mov eax, [esp+ 4] ; Src1
  mov edx, [esp+ 8] ; Src2
  mov ecx, [esp+12] ; Stride

  pxor mm6, mm6 ; accum
  pxor mm7, mm7 ; zero

  SAD_16x16_MMX
  SAD_16x16_MMX
  SAD_16x16_MMX
  SAD_16x16_MMX
  SAD_16x16_MMX
  SAD_16x16_MMX
  SAD_16x16_MMX
  SAD_16x16_MMX

  SAD_16x16_MMX
  SAD_16x16_MMX
  SAD_16x16_MMX
  SAD_16x16_MMX
  SAD_16x16_MMX
  SAD_16x16_MMX
  SAD_16x16_MMX
  SAD_16x16_MMX

  pmaddwd mm6, [mmx_one] ; collapse
  movq mm7, mm6
  psrlq mm7, 32
  paddd mm6, mm7

  movd eax, mm6

  ret

;-----------------------------------------------------------------------------
;
; uint32_t sad8_mmx(const uint8_t * const cur,
;					const uint8_t * const ref,
;					const uint32_t stride);
;
;-----------------------------------------------------------------------------

ALIGN 16
sad8_mmx:

  mov eax, [esp+ 4] ; Src1
  mov edx, [esp+ 8] ; Src2
  mov ecx, [esp+12] ; Stride

  pxor mm6, mm6 ; accum
  pxor mm7, mm7 ; zero

  SAD_8x8_MMX
  SAD_8x8_MMX
  SAD_8x8_MMX
  SAD_8x8_MMX

  pmaddwd mm6, [mmx_one] ; collapse
  movq mm7, mm6
  psrlq mm7, 32
  paddd mm6, mm7

  movd eax, mm6

  ret

;-----------------------------------------------------------------------------
;
; uint32_t sad16v_mmx(const uint8_t * const cur,
;				      const uint8_t * const ref,
;					  const uint32_t stride,
;					  int32_t *sad);
;
;-----------------------------------------------------------------------------

ALIGN 16
sad16v_mmx:

  push ebx
  push edi

  mov eax, [esp + 8 + 4] ; Src1
  mov edx, [esp + 8 + 8] ; Src2
  mov ecx, [esp + 8 + 12] ; Stride
  mov ebx, [esp + 8 + 16] ; sad ptr

  pxor mm5, mm5 ; accum
  pxor mm6, mm6 ; accum
  pxor mm7, mm7 ; zero

  SADV_16x16_MMX
  SADV_16x16_MMX
  SADV_16x16_MMX
  SADV_16x16_MMX
  SADV_16x16_MMX
  SADV_16x16_MMX
  SADV_16x16_MMX
  SADV_16x16_MMX

  pmaddwd mm5, [mmx_one] ; collapse
  pmaddwd mm6, [mmx_one] ; collapse

  movq mm2, mm5
  movq mm3, mm6

  psrlq mm2, 32
  psrlq mm3, 32

  paddd mm5, mm2
  paddd mm6, mm3

  movd [ebx], mm5
  movd [ebx + 4], mm6

  paddd mm5, mm6

  movd edi, mm5

  pxor mm5, mm5
  pxor mm6, mm6

  SADV_16x16_MMX
  SADV_16x16_MMX
  SADV_16x16_MMX
  SADV_16x16_MMX
  SADV_16x16_MMX
  SADV_16x16_MMX
  SADV_16x16_MMX
  SADV_16x16_MMX

  pmaddwd mm5, [mmx_one] ; collapse
  pmaddwd mm6, [mmx_one] ; collapse

  movq mm2, mm5
  movq mm3, mm6

  psrlq mm2, 32
  psrlq mm3, 32

  paddd mm5, mm2
  paddd mm6, mm3

  movd [ebx + 8], mm5
  movd [ebx + 12], mm6

  paddd mm5, mm6

  movd eax, mm5

  add eax, edi

  pop edi
  pop ebx

  ret

;-----------------------------------------------------------------------------
;
; uint32_t sad16bi_mmx(const uint8_t * const cur,
; const uint8_t * const ref1,
; const uint8_t * const ref2,
; const uint32_t stride);
;
;-----------------------------------------------------------------------------

ALIGN 16
sad16bi_mmx:
  push ebx
  mov eax, [esp+4+ 4] ; Src
  mov edx, [esp+4+ 8] ; Ref1
  mov ebx, [esp+4+12] ; Ref2
  mov ecx, [esp+4+16] ; Stride

  pxor mm6, mm6 ; accum2
  pxor mm7, mm7
.Loop
  SADBI_16x16_MMX 0, 0
  SADBI_16x16_MMX 8, 1
  SADBI_16x16_MMX 0, 0
  SADBI_16x16_MMX 8, 1
  SADBI_16x16_MMX 0, 0
  SADBI_16x16_MMX 8, 1
  SADBI_16x16_MMX 0, 0
  SADBI_16x16_MMX 8, 1
  SADBI_16x16_MMX 0, 0
  SADBI_16x16_MMX 8, 1
  SADBI_16x16_MMX 0, 0
  SADBI_16x16_MMX 8, 1
  SADBI_16x16_MMX 0, 0
  SADBI_16x16_MMX 8, 1
  SADBI_16x16_MMX 0, 0
  SADBI_16x16_MMX 8, 1

  SADBI_16x16_MMX 0, 0
  SADBI_16x16_MMX 8, 1
  SADBI_16x16_MMX 0, 0
  SADBI_16x16_MMX 8, 1
  SADBI_16x16_MMX 0, 0
  SADBI_16x16_MMX 8, 1
  SADBI_16x16_MMX 0, 0
  SADBI_16x16_MMX 8, 1
  SADBI_16x16_MMX 0, 0
  SADBI_16x16_MMX 8, 1
  SADBI_16x16_MMX 0, 0
  SADBI_16x16_MMX 8, 1
  SADBI_16x16_MMX 0, 0
  SADBI_16x16_MMX 8, 1
  SADBI_16x16_MMX 0, 0
  SADBI_16x16_MMX 8, 1

  pmaddwd mm6, [mmx_one] ; collapse
  movq mm7, mm6
  psrlq mm7, 32
  paddd mm6, mm7

  movd eax, mm6
  pop ebx

  ret

;-----------------------------------------------------------------------------
;
; uint32_t sad8bi_mmx(const uint8_t * const cur,
; const uint8_t * const ref1,
; const uint8_t * const ref2,
; const uint32_t stride);
;
;-----------------------------------------------------------------------------

ALIGN 16
sad8bi_mmx:
  push ebx
  mov eax, [esp+4+ 4] ; Src
  mov edx, [esp+4+ 8] ; Ref1
  mov ebx, [esp+4+12] ; Ref2
  mov ecx, [esp+4+16] ; Stride

  pxor mm6, mm6 ; accum2
  pxor mm7, mm7
.Loop
  SADBI_16x16_MMX 0, 1
  SADBI_16x16_MMX 0, 1
  SADBI_16x16_MMX 0, 1
  SADBI_16x16_MMX 0, 1
  SADBI_16x16_MMX 0, 1
  SADBI_16x16_MMX 0, 1
  SADBI_16x16_MMX 0, 1
  SADBI_16x16_MMX 0, 1

  pmaddwd mm6, [mmx_one] ; collapse
  movq mm7, mm6
  psrlq mm7, 32
  paddd mm6, mm7

  movd eax, mm6
  pop ebx
  ret

;-----------------------------------------------------------------------------
;
; uint32_t dev16_mmx(const uint8_t * const cur,
;					const uint32_t stride);
;
;-----------------------------------------------------------------------------

ALIGN 16
dev16_mmx:
  mov eax, [esp+ 4] ; Src
  mov ecx, [esp+ 8] ; Stride

  pxor mm7, mm7 ; zero
  pxor mm5, mm5 ; accum1
  pxor mm6, mm6 ; accum2

  MEAN_16x16_MMX
  MEAN_16x16_MMX
  MEAN_16x16_MMX
  MEAN_16x16_MMX
  MEAN_16x16_MMX
  MEAN_16x16_MMX
  MEAN_16x16_MMX
  MEAN_16x16_MMX

  MEAN_16x16_MMX
  MEAN_16x16_MMX
  MEAN_16x16_MMX
  MEAN_16x16_MMX
  MEAN_16x16_MMX
  MEAN_16x16_MMX
  MEAN_16x16_MMX
  MEAN_16x16_MMX

  paddusw mm6, mm5
  pmaddwd mm6, [mmx_one]    ; collapse
  movq mm5, mm6
  psrlq mm5, 32
  paddd mm6, mm5

  psllq mm6, 32             ; blank upper dword
  psrlq mm6, 32 + 8         ;    /= (16*16)

  punpckldq mm6, mm6
  packssdw mm6, mm6

    ; mm6 contains the mean
    ; mm5 is the new accum

  pxor mm5, mm5
  mov eax, [esp+ 4]         ; Src

  ABS_16x16_MMX
  ABS_16x16_MMX
  ABS_16x16_MMX
  ABS_16x16_MMX
  ABS_16x16_MMX
  ABS_16x16_MMX
  ABS_16x16_MMX
  ABS_16x16_MMX

  ABS_16x16_MMX
  ABS_16x16_MMX
  ABS_16x16_MMX
  ABS_16x16_MMX
  ABS_16x16_MMX
  ABS_16x16_MMX
  ABS_16x16_MMX
  ABS_16x16_MMX

  pmaddwd mm5, [mmx_one]    ; collapse
  movq mm6, mm5
  psrlq mm6, 32
  paddd mm6, mm5

  movd eax, mm6

  ret

;-----------------------------------------------------------------------------
;
; uint32_t sse8_16bit_mmx(const int16_t *b1,
;                         const int16_t *b2,
;                         const uint32_t stride);
;
;-----------------------------------------------------------------------------

%macro ROW_SSE_MMX 2
  movq mm0, [%1]
  movq mm1, [%1+8]
  psubw mm0, [%2]
  psubw mm1, [%2+8]
  pmaddwd mm0, mm0
  pmaddwd mm1, mm1
  paddd mm2, mm0
  paddd mm2, mm1
%endmacro	
	
sse8_16bit_mmx:
  push esi
  push edi

  ;; Load the function params
  mov esi, [esp+8+4]
  mov edi, [esp+8+8]
  mov edx, [esp+8+12]

  ;; Reset the sse accumulator
  pxor mm2, mm2

  ;; Let's go
  ROW_SSE_MMX esi, edi
  lea esi, [esi+edx]
  lea edi, [edi+edx]
  ROW_SSE_MMX esi, edi
  lea esi, [esi+edx]
  lea edi, [edi+edx]
  ROW_SSE_MMX esi, edi
  lea esi, [esi+edx]
  lea edi, [edi+edx]
  ROW_SSE_MMX esi, edi
  lea esi, [esi+edx]
  lea edi, [edi+edx]
  ROW_SSE_MMX esi, edi
  lea esi, [esi+edx]
  lea edi, [edi+edx]
  ROW_SSE_MMX esi, edi
  lea esi, [esi+edx]
  lea edi, [edi+edx]
  ROW_SSE_MMX esi, edi
  lea esi, [esi+edx]
  lea edi, [edi+edx]
  ROW_SSE_MMX esi, edi
  lea esi, [esi+edx]
  lea edi, [edi+edx]

  ;; Finish adding each dword of the accumulator
  movq mm3, mm2
  psrlq mm2, 32
  paddd mm2, mm3
  movd eax, mm2

  ;; All done
  pop edi
  pop esi
  ret
