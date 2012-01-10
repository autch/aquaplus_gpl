;/*****************************************************************************
; *
; *  XVID MPEG-4 VIDEO CODEC
; *  - mmx 8x8 block-based halfpel interpolation -
; *
; *  Copyright(C) 2001 Peter Ross <pross@xvid.org>
; *               2002 Michael Militzer <isibaar@xvid.org>
; *
; *  This program is free software ; you can redistribute it and/or modify
; *  it under the terms of the GNU General Public License as published by
; *  the Free Software Foundation ; either version 2 of the License, or
; *  (at your option) any later version.
; *
; *  This program is distributed in the hope that it will be useful,
; *  but WITHOUT ANY WARRANTY ; without even the implied warranty of
; *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; *  GNU General Public License for more details.
; *
; *  You should have received a copy of the GNU General Public License
; *  along with this program ; if not, write to the Free Software
; *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
; *
; ****************************************************************************/

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

;-----------------------------------------------------------------------------
; (16 - r) rounding table
;-----------------------------------------------------------------------------

ALIGN 16
rounding_lowpass_mmx:
	times 4 dw 16
	times 4 dw 15

;-----------------------------------------------------------------------------
; (1 - r) rounding table
;-----------------------------------------------------------------------------

rounding1_mmx:
	times 4 dw 1
	times 4 dw 0

;-----------------------------------------------------------------------------
; (2 - r) rounding table
;-----------------------------------------------------------------------------

rounding2_mmx:
	times 4 dw 2
	times 4 dw 1

mmx_one:
	times 8 db 1

mmx_two:
	times 8 db 2

mmx_three:
	times 8 db 3

mmx_five:
	times 4 dw 5

mmx_mask:
	times 8 db 254

mmx_mask2:
	times 8 db 252

;=============================================================================
; Code
;=============================================================================

SECTION .text

cglobal interpolate8x8_halfpel_h_mmx
cglobal interpolate8x8_halfpel_v_mmx
cglobal interpolate8x8_halfpel_hv_mmx
cglobal interpolate8x8_avg4_mmx
cglobal interpolate8x8_avg2_mmx
cglobal interpolate8x8_6tap_lowpass_h_mmx
cglobal interpolate8x8_6tap_lowpass_v_mmx

%macro  CALC_AVG 6
  punpcklbw %3, %6
  punpckhbw %4, %6

  paddusw %1, %3    ; mm01 += mm23
  paddusw %2, %4
  paddusw %1, %5    ; mm01 += rounding
  paddusw %2, %5

  psrlw %1, 1   ; mm01 >>= 1
  psrlw %2, 1
%endmacro


;-----------------------------------------------------------------------------
;
; void interpolate8x8_halfpel_h_mmx(uint8_t * const dst,
;                       const uint8_t * const src,
;                       const uint32_t stride,
;                       const uint32_t rounding);
;
;-----------------------------------------------------------------------------

%macro COPY_H_MMX 0
  movq mm0, [esi]
  movq mm2, [esi + 1]
  movq mm1, mm0
  movq mm3, mm2

  punpcklbw mm0, mm6        ; mm01 = [src]
  punpckhbw mm1, mm6        ; mm23 = [src + 1]

  CALC_AVG mm0, mm1, mm2, mm3, mm7, mm6

  packuswb mm0, mm1
  movq [edi], mm0           ; [dst] = mm01

  add esi, edx              ; src += stride
  add edi, edx              ; dst += stride
%endmacro

ALIGN 16
interpolate8x8_halfpel_h_mmx:

  push esi
  push edi
  mov eax, [esp + 8 + 16]       ; rounding

  movq mm7, [rounding1_mmx + eax * 8]

  mov edi, [esp + 8 + 4]        ; dst
  mov esi, [esp + 8 + 8]        ; src
  mov edx, [esp + 8 + 12]       ; stride

  pxor mm6, mm6                 ; zero

  COPY_H_MMX
  COPY_H_MMX
  COPY_H_MMX
  COPY_H_MMX
  COPY_H_MMX
  COPY_H_MMX
  COPY_H_MMX
  COPY_H_MMX

  pop edi
  pop esi

  ret


;-----------------------------------------------------------------------------
;
; void interpolate8x8_halfpel_v_mmx(uint8_t * const dst,
;                       const uint8_t * const src,
;                       const uint32_t stride,
;                       const uint32_t rounding);
;
;-----------------------------------------------------------------------------

%macro COPY_V_MMX 0
  movq mm0, [esi]
  movq mm2, [esi + edx]
  movq mm1, mm0
  movq mm3, mm2

  punpcklbw mm0, mm6    ; mm01 = [src]
  punpckhbw mm1, mm6    ; mm23 = [src + 1]

  CALC_AVG mm0, mm1, mm2, mm3, mm7, mm6

  packuswb mm0, mm1
  movq [edi], mm0       ; [dst] = mm01

  add esi, edx          ; src += stride
  add edi, edx          ; dst += stride
%endmacro

ALIGN 16
interpolate8x8_halfpel_v_mmx:

  push esi
  push edi

  mov eax, [esp + 8 + 16]       ; rounding

  movq mm7, [rounding1_mmx + eax * 8]

  mov edi, [esp + 8 + 4]        ; dst
  mov esi, [esp + 8 + 8]        ; src
  mov edx, [esp + 8 + 12]       ; stride

  pxor mm6, mm6                 ; zero


  COPY_V_MMX
  COPY_V_MMX
  COPY_V_MMX
  COPY_V_MMX
  COPY_V_MMX
  COPY_V_MMX
  COPY_V_MMX
  COPY_V_MMX

  pop edi
  pop esi

  ret


;-----------------------------------------------------------------------------
;
; void interpolate8x8_halfpel_hv_mmx(uint8_t * const dst,
;                       const uint8_t * const src,
;                       const uint32_t stride,
;                       const uint32_t rounding);
;
;
;-----------------------------------------------------------------------------

%macro COPY_HV_MMX 0
    ; current row
  movq mm0, [esi]
  movq mm2, [esi + 1]

  movq mm1, mm0
  movq mm3, mm2

  punpcklbw mm0, mm6        ; mm01 = [src]
  punpcklbw mm2, mm6        ; mm23 = [src + 1]
  punpckhbw mm1, mm6
  punpckhbw mm3, mm6

  paddusw mm0, mm2          ; mm01 += mm23
  paddusw mm1, mm3

    ; next row
  movq mm4, [esi + edx]
  movq mm2, [esi + edx + 1]

  movq mm5, mm4
  movq mm3, mm2

  punpcklbw mm4, mm6        ; mm45 = [src + stride]
  punpcklbw mm2, mm6        ; mm23 = [src + stride + 1]
  punpckhbw mm5, mm6
  punpckhbw mm3, mm6

  paddusw mm4, mm2          ; mm45 += mm23
  paddusw mm5, mm3

    ; add current + next row
  paddusw mm0, mm4          ; mm01 += mm45
  paddusw mm1, mm5
  paddusw mm0, mm7          ; mm01 += rounding2
  paddusw mm1, mm7

  psrlw mm0, 2              ; mm01 >>= 2
  psrlw mm1, 2

  packuswb mm0, mm1
  movq [edi], mm0           ; [dst] = mm01

  add esi, edx              ; src += stride
  add edi, edx              ; dst += stride
%endmacro

ALIGN 16
interpolate8x8_halfpel_hv_mmx:

  push esi
  push edi

  mov eax, [esp + 8 + 16]   ; rounding

  movq mm7, [rounding2_mmx + eax * 8]

  mov edi, [esp + 8 + 4]    ; dst
  mov esi, [esp + 8 + 8]    ; src

  mov eax, 8

  pxor mm6, mm6             ; zero

  mov edx, [esp + 8 + 12]   ; stride

  COPY_HV_MMX
  COPY_HV_MMX
  COPY_HV_MMX
  COPY_HV_MMX
  COPY_HV_MMX
  COPY_HV_MMX
  COPY_HV_MMX
  COPY_HV_MMX

  pop edi
  pop esi

  ret

;-----------------------------------------------------------------------------
;
; void interpolate8x8_avg2_mmx(uint8_t const *dst,
;                              const uint8_t * const src1,
;                              const uint8_t * const src2,
;                              const uint32_t stride,
;                              const uint32_t rounding,
;                              const uint32_t height);
;
;-----------------------------------------------------------------------------

%macro AVG2_MMX_RND0 0
  movq mm0, [eax]           ; src1 -> mm0
  movq mm1, [ebx]           ; src2 -> mm1

  movq mm4, [eax+edx]
  movq mm5, [ebx+edx]

  movq mm2, mm0             ; src1 -> mm2
  movq mm3, mm1             ; src2 -> mm3

  pand mm2, mm7             ; isolate the lsb
  pand mm3, mm7             ; isolate the lsb

  por mm2, mm3              ; ODD(src1) OR ODD(src2) -> mm2

  movq mm3, mm4
  movq mm6, mm5

  pand mm3, mm7
  pand mm6, mm7

  por mm3, mm6

  pand mm0, [mmx_mask]
  pand mm1, [mmx_mask]
  pand mm4, [mmx_mask]
  pand mm5, [mmx_mask]

  psrlq mm0, 1              ; src1 / 2
  psrlq mm1, 1              ; src2 / 2

  psrlq mm4, 1
  psrlq mm5, 1

  paddb mm0, mm1            ; src1/2 + src2/2 -> mm0
  paddb mm0, mm2            ; correct rounding error

  paddb mm4, mm5
  paddb mm4, mm3

  lea eax, [eax+2*edx]
  lea ebx, [ebx+2*edx]

  movq [ecx], mm0           ; (src1 + src2 + 1) / 2 -> dst
  movq [ecx+edx], mm4
%endmacro

%macro AVG2_MMX_RND1 0
  movq mm0, [eax]           ; src1 -> mm0
  movq mm1, [ebx]           ; src2 -> mm1

  movq mm4, [eax+edx]
  movq mm5, [ebx+edx]

  movq mm2, mm0             ; src1 -> mm2
  movq mm3, mm1             ; src2 -> mm3

  pand mm2, mm7             ; isolate the lsb
  pand mm3, mm7             ; isolate the lsb

  pand mm2, mm3             ; ODD(src1) AND ODD(src2) -> mm2

  movq mm3, mm4
  movq mm6, mm5

  pand mm3, mm7
  pand mm6, mm7

  pand mm3, mm6

  pand mm0, [mmx_mask]
  pand mm1, [mmx_mask]
  pand mm4, [mmx_mask]
  pand mm5, [mmx_mask]

  psrlq mm0, 1              ; src1 / 2
  psrlq mm1, 1              ; src2 / 2

  psrlq mm4, 1
  psrlq mm5, 1

  paddb mm0, mm1            ; src1/2 + src2/2 -> mm0
  paddb mm0, mm2            ; correct rounding error

  paddb mm4, mm5
  paddb mm4, mm3

  lea eax, [eax+2*edx]
  lea ebx, [ebx+2*edx]

  movq [ecx], mm0           ; (src1 + src2 + 1) / 2 -> dst
  movq [ecx+edx], mm4
%endmacro

ALIGN 16
interpolate8x8_avg2_mmx:

  push ebx

  mov eax, [esp + 4 + 20]   ; rounding
  test eax, eax

  jnz near .rounding1

  mov eax, [esp + 4 + 24]   ; height -> eax
  sub eax, 8
  test eax, eax

  mov ecx, [esp + 4 + 4]    ; dst -> edi
  mov eax, [esp + 4 + 8]    ; src1 -> esi
  mov ebx, [esp + 4 + 12]   ; src2 -> eax
  mov edx, [esp + 4 + 16]   ; stride -> edx

  movq mm7, [mmx_one]

  jz near .start0

  AVG2_MMX_RND0
  lea ecx, [ecx+2*edx]

.start0

  AVG2_MMX_RND0
  lea ecx, [ecx+2*edx]
  AVG2_MMX_RND0
  lea ecx, [ecx+2*edx]
  AVG2_MMX_RND0
  lea ecx, [ecx+2*edx]
  AVG2_MMX_RND0

  pop ebx
  ret

.rounding1
  mov eax, [esp + 4 + 24]       ; height -> eax
  sub eax, 8
  test eax, eax

  mov ecx, [esp + 4 + 4]        ; dst -> edi
  mov eax, [esp + 4 + 8]        ; src1 -> esi
  mov ebx, [esp + 4 + 12]       ; src2 -> eax
  mov edx, [esp + 4 + 16]       ; stride -> edx

  movq mm7, [mmx_one]

  jz near .start1

  AVG2_MMX_RND1
  lea ecx, [ecx+2*edx]

.start1

  AVG2_MMX_RND1
  lea ecx, [ecx+2*edx]
  AVG2_MMX_RND1
  lea ecx, [ecx+2*edx]
  AVG2_MMX_RND1
  lea ecx, [ecx+2*edx]
  AVG2_MMX_RND1

  pop ebx
  ret


;-----------------------------------------------------------------------------
;
; void interpolate8x8_avg4_mmx(uint8_t const *dst,
;                              const uint8_t * const src1,
;                              const uint8_t * const src2,
;                              const uint8_t * const src3,
;                              const uint8_t * const src4,
;                              const uint32_t stride,
;                              const uint32_t rounding);
;
;-----------------------------------------------------------------------------

%macro AVG4_MMX_RND0 0
  movq mm0, [eax]           ; src1 -> mm0
  movq mm1, [ebx]           ; src2 -> mm1

  movq mm2, mm0
  movq mm3, mm1

  pand mm2, [mmx_three]
  pand mm3, [mmx_three]

  pand mm0, [mmx_mask2]
  pand mm1, [mmx_mask2]

  psrlq mm0, 2
  psrlq mm1, 2

  lea eax, [eax+edx]
  lea ebx, [ebx+edx]

  paddb mm0, mm1
  paddb mm2, mm3

  movq mm4, [esi]           ; src3 -> mm0
  movq mm5, [edi]           ; src4 -> mm1

  movq mm1, mm4
  movq mm3, mm5

  pand mm1, [mmx_three]
  pand mm3, [mmx_three]

  pand mm4, [mmx_mask2]
  pand mm5, [mmx_mask2]

  psrlq mm4, 2
  psrlq mm5, 2

  paddb mm4, mm5
  paddb mm0, mm4

  paddb mm1, mm3
  paddb mm2, mm1

  paddb mm2, [mmx_two]
  pand mm2, [mmx_mask2]

  psrlq mm2, 2
  paddb mm0, mm2

  lea esi, [esi+edx]
  lea edi, [edi+edx]

  movq [ecx], mm0           ; (src1 + src2 + src3 + src4 + 2) / 4 -> dst
%endmacro

%macro AVG4_MMX_RND1 0
  movq mm0, [eax]           ; src1 -> mm0
  movq mm1, [ebx]           ; src2 -> mm1

  movq mm2, mm0
  movq mm3, mm1

  pand mm2, [mmx_three]
  pand mm3, [mmx_three]

  pand mm0, [mmx_mask2]
  pand mm1, [mmx_mask2]

  psrlq mm0, 2
  psrlq mm1, 2

  lea eax,[eax+edx]
  lea ebx,[ebx+edx]

  paddb mm0, mm1
  paddb mm2, mm3

  movq mm4, [esi]           ; src3 -> mm0
  movq mm5, [edi]           ; src4 -> mm1

  movq mm1, mm4
  movq mm3, mm5

  pand mm1, [mmx_three]
  pand mm3, [mmx_three]

  pand mm4, [mmx_mask2]
  pand mm5, [mmx_mask2]

  psrlq mm4, 2
  psrlq mm5, 2

  paddb mm4, mm5
  paddb mm0, mm4

  paddb mm1, mm3
  paddb mm2, mm1

  paddb mm2, [mmx_one]
  pand mm2, [mmx_mask2]

  psrlq mm2, 2
  paddb mm0, mm2

  lea esi,[esi+edx]
  lea edi,[edi+edx]

  movq [ecx], mm0           ; (src1 + src2 + src3 + src4 + 2) / 4 -> dst
%endmacro

ALIGN 16
interpolate8x8_avg4_mmx:

  push ebx
  push edi
  push esi

  mov eax, [esp + 12 + 28]      ; rounding

  test eax, eax

  mov ecx, [esp + 12 + 4]       ; dst -> edi
  mov eax, [esp + 12 + 8]       ; src1 -> esi
  mov ebx, [esp + 12 + 12]      ; src2 -> eax
  mov esi, [esp + 12 + 16]      ; src3 -> esi
  mov edi, [esp + 12 + 20]      ; src4 -> edi
  mov edx, [esp + 12 + 24]      ; stride -> edx

  movq mm7, [mmx_one]

  jnz near .rounding1

  AVG4_MMX_RND0
  lea ecx, [ecx+edx]
  AVG4_MMX_RND0
  lea ecx, [ecx+edx]
  AVG4_MMX_RND0
  lea ecx, [ecx+edx]
  AVG4_MMX_RND0
  lea ecx, [ecx+edx]
  AVG4_MMX_RND0
  lea ecx, [ecx+edx]
  AVG4_MMX_RND0
  lea ecx, [ecx+edx]
  AVG4_MMX_RND0
  lea ecx, [ecx+edx]
  AVG4_MMX_RND0

  pop esi
  pop edi
  pop ebx
  ret

.rounding1
  AVG4_MMX_RND1
  lea ecx, [ecx+edx]
  AVG4_MMX_RND1
  lea ecx, [ecx+edx]
  AVG4_MMX_RND1
  lea ecx, [ecx+edx]
  AVG4_MMX_RND1
  lea ecx, [ecx+edx]
  AVG4_MMX_RND1
  lea ecx, [ecx+edx]
  AVG4_MMX_RND1
  lea ecx, [ecx+edx]
  AVG4_MMX_RND1
  lea ecx, [ecx+edx]
  AVG4_MMX_RND1

  pop esi
  pop edi
  pop ebx
  ret


;-----------------------------------------------------------------------------
;
; void interpolate8x8_6tap_lowpass_h_mmx(uint8_t const *dst,
;                                        const uint8_t * const src,
;                                        const uint32_t stride,
;                                        const uint32_t rounding);
;
;-----------------------------------------------------------------------------

%macro LOWPASS_6TAP_H_MMX 0
  movq mm0, [eax]
  movq mm2, [eax+1]

  movq mm1, mm0
  movq mm3, mm2

  punpcklbw mm0, mm7
  punpcklbw mm2, mm7

  punpckhbw mm1, mm7
  punpckhbw mm3, mm7

  paddw mm0, mm2
  paddw mm1, mm3

  psllw mm0, 2
  psllw mm1, 2

  movq mm2, [eax-1]
  movq mm4, [eax+2]

  movq mm3, mm2
  movq mm5, mm4

  punpcklbw mm2, mm7
  punpcklbw mm4, mm7

  punpckhbw mm3, mm7
  punpckhbw mm5, mm7

  paddw mm2, mm4
  paddw mm3, mm5

  psubsw mm0, mm2
  psubsw mm1, mm3

  pmullw mm0, [mmx_five]
  pmullw mm1, [mmx_five]

  movq mm2, [eax-2]
  movq mm4, [eax+3]

  movq mm3, mm2
  movq mm5, mm4

  punpcklbw mm2, mm7
  punpcklbw mm4, mm7

  punpckhbw mm3, mm7
  punpckhbw mm5, mm7

  paddw mm2, mm4
  paddw mm3, mm5

  paddsw mm0, mm2
  paddsw mm1, mm3

  paddsw mm0, mm6
  paddsw mm1, mm6

  psraw mm0, 5
  psraw mm1, 5

  lea eax, [eax+edx]
  packuswb mm0, mm1
  movq [ecx], mm0
%endmacro

ALIGN 16
interpolate8x8_6tap_lowpass_h_mmx:

  mov eax, [esp + 16]           ; rounding

  movq mm6, [rounding_lowpass_mmx + eax * 8]

  mov ecx, [esp + 4]            ; dst -> edi
  mov eax, [esp + 8]            ; src -> esi
  mov edx, [esp + 12]           ; stride -> edx

  pxor mm7, mm7

  LOWPASS_6TAP_H_MMX
  lea ecx, [ecx+edx]
  LOWPASS_6TAP_H_MMX
  lea ecx, [ecx+edx]
  LOWPASS_6TAP_H_MMX
  lea ecx, [ecx+edx]
  LOWPASS_6TAP_H_MMX
  lea ecx, [ecx+edx]
  LOWPASS_6TAP_H_MMX
  lea ecx, [ecx+edx]
  LOWPASS_6TAP_H_MMX
  lea ecx, [ecx+edx]
  LOWPASS_6TAP_H_MMX
  lea ecx, [ecx+edx]
  LOWPASS_6TAP_H_MMX

  ret

;-----------------------------------------------------------------------------
;
; void interpolate8x8_6tap_lowpass_v_mmx(uint8_t const *dst,
;                                        const uint8_t * const src,
;                                        const uint32_t stride,
;                                        const uint32_t rounding);
;
;-----------------------------------------------------------------------------

%macro LOWPASS_6TAP_V_MMX 0
  movq mm0, [eax]
  movq mm2, [eax+edx]

  movq mm1, mm0
  movq mm3, mm2

  punpcklbw mm0, mm7
  punpcklbw mm2, mm7

  punpckhbw mm1, mm7
  punpckhbw mm3, mm7

  paddw mm0, mm2
  paddw mm1, mm3

  psllw mm0, 2
  psllw mm1, 2

  movq mm4, [eax+2*edx]
  sub eax, ebx
  movq mm2, [eax+2*edx]

  movq mm3, mm2
  movq mm5, mm4

  punpcklbw mm2, mm7
  punpcklbw mm4, mm7

  punpckhbw mm3, mm7
  punpckhbw mm5, mm7

  paddw mm2, mm4
  paddw mm3, mm5

  psubsw mm0, mm2
  psubsw mm1, mm3

  pmullw mm0, [mmx_five]
  pmullw mm1, [mmx_five]

  movq mm2, [eax+edx]
  movq mm4, [eax+2*ebx]

  movq mm3, mm2
  movq mm5, mm4

  punpcklbw mm2, mm7
  punpcklbw mm4, mm7

  punpckhbw mm3, mm7
  punpckhbw mm5, mm7

  paddw mm2, mm4
  paddw mm3, mm5

  paddsw mm0, mm2
  paddsw mm1, mm3

  paddsw mm0, mm6
  paddsw mm1, mm6

  psraw mm0, 5
  psraw mm1, 5

  lea eax, [eax+4*edx]
  packuswb mm0, mm1
  movq [ecx], mm0
%endmacro

ALIGN 16
interpolate8x8_6tap_lowpass_v_mmx:

  push ebx

  mov eax, [esp + 4 + 16]           ; rounding

  movq mm6, [rounding_lowpass_mmx + eax * 8]

  mov ecx, [esp + 4 + 4]            ; dst -> edi
  mov eax, [esp + 4 + 8]            ; src -> esi
  mov edx, [esp + 4 + 12]           ; stride -> edx

  mov ebx, edx
  shl ebx, 1
  add ebx, edx

  pxor mm7, mm7

  LOWPASS_6TAP_V_MMX
  lea ecx, [ecx+edx]
  LOWPASS_6TAP_V_MMX
  lea ecx, [ecx+edx]
  LOWPASS_6TAP_V_MMX
  lea ecx, [ecx+edx]
  LOWPASS_6TAP_V_MMX
  lea ecx, [ecx+edx]
  LOWPASS_6TAP_V_MMX
  lea ecx, [ecx+edx]
  LOWPASS_6TAP_V_MMX
  lea ecx, [ecx+edx]
  LOWPASS_6TAP_V_MMX
  lea ecx, [ecx+edx]
  LOWPASS_6TAP_V_MMX

  pop ebx
  ret
