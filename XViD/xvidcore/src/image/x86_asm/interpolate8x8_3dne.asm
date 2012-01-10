;/*****************************************************************************
; *
; *  XVID MPEG-4 VIDEO CODEC
; *  - 3dne pipeline optimized  8x8 block-based halfpel interpolation -
; *
; *  Copyright(C) 2002 Jaan Kalda
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
	times 8 db 1

ALIGN 8
mm_minusone:
	dd -1,-1

;=============================================================================
; Macros
;=============================================================================

%macro nop4 0
DB 08Dh,074h,026h,0
%endmacro

;=============================================================================
; Macros
;=============================================================================

SECTION .text

cglobal interpolate8x8_halfpel_h_3dne
cglobal interpolate8x8_halfpel_v_3dne
cglobal interpolate8x8_halfpel_hv_3dne

;-----------------------------------------------------------------------------
;
; void interpolate8x8_halfpel_h_3dne(uint8_t * const dst,
;                       const uint8_t * const src,
;                       const uint32_t stride,
;                       const uint32_t rounding);
;
;-----------------------------------------------------------------------------

%macro COPY_H_SSE_RND0 1
%if (%1)
  movq mm0, [eax]
%else
  movq mm0, [dword eax]
%endif
  pavgb mm0, [eax+1]
  movq mm1, [eax+edx]
  pavgb mm1, [eax+edx+1]
  lea eax, [eax+2*edx]
  movq [ecx], mm0
  movq [ecx+edx], mm1
%endmacro

%macro COPY_H_SSE_RND1 0
  movq mm0, [eax]
  movq mm1, [eax+edx]
  movq mm4, mm0
  movq mm5, mm1
  movq mm2, [eax+1]
  movq mm3, [eax+edx+1]
  pavgb mm0, mm2
  pxor mm2, mm4
  pavgb mm1, mm3
  lea eax, [eax+2*edx]
  pxor mm3, mm5
  pand mm2, mm7
  pand mm3, mm7
  psubb mm0, mm2
  movq [ecx], mm0
  psubb mm1, mm3
  movq [ecx+edx], mm1
%endmacro

ALIGN 16
interpolate8x8_halfpel_h_3dne:

  mov eax, [esp+ 8] ; Src
  mov edx, [esp+12] ; stride
  dec dword [esp+16]; rounding

  jz .rounding1
  mov ecx, [esp+ 4] ; Dst

  COPY_H_SSE_RND0 0
  lea ecx,[ecx+2*edx]
  COPY_H_SSE_RND0 1
  lea ecx,[ecx+2*edx]
  COPY_H_SSE_RND0 1
  lea ecx,[ecx+2*edx]
  COPY_H_SSE_RND0 1
  ret

.rounding1
 ; we use: (i+j)/2 = ( i+j+1 )/2 - (i^j)&1
  mov ecx, [esp+ 4] ; Dst
  movq mm7, [mmx_one]
  COPY_H_SSE_RND1
  lea ecx, [ecx+2*edx]
  COPY_H_SSE_RND1
  lea ecx,[ecx+2*edx]
  COPY_H_SSE_RND1
  lea ecx,[ecx+2*edx]
  COPY_H_SSE_RND1
  ret

;-----------------------------------------------------------------------------
;
; void interpolate8x8_halfpel_v_3dne(uint8_t * const dst,
;                       const uint8_t * const src,
;                       const uint32_t stride,
;                       const uint32_t rounding);
;
;-----------------------------------------------------------------------------

ALIGN 16
interpolate8x8_halfpel_v_3dne:

  mov eax, [esp+ 8] ; Src
  mov edx, [esp+12] ; stride
  dec dword [esp+16]; rounding

    ; we process 2 line at a time

  jz .rounding1
  pxor mm2,mm2
  movq mm0, [eax]
  movq mm1, [eax+edx]
  por mm2, [eax+2*edx]
  mov ecx, [esp+ 4] ; Dst
  lea eax, [eax+2*edx]
  pxor mm4, mm4
  pavgb mm0, mm1
  pavgb mm1, mm2
  movq [byte ecx], mm0
  movq [ecx+edx], mm1
  pxor mm6, mm6
  add eax, edx
  lea ecx, [ecx+2*edx]
  movq mm3, [byte eax]
  por mm4, [eax+edx]
  lea eax, [eax+2*edx]
  pavgb mm2, mm3
  pavgb mm3, mm4
  movq [ecx], mm2
  movq [ecx+edx], mm3
  lea ecx, [byte ecx+2*edx]
  movq mm5, [byte eax]
  por mm6, [eax+edx]
  lea eax, [eax+2*edx]
  pavgb mm4, mm5
  pavgb mm5, mm6
  movq [ecx], mm4
  movq [ecx+edx], mm5
  lea ecx, [ecx+2*edx]
  movq mm7, [eax]
  movq mm0, [eax+edx]
  pavgb mm6, mm7
  pavgb mm7, mm0
  movq [ecx], mm6
  movq [ecx+edx], mm7
  ret

ALIGN 8
.rounding1
  pcmpeqb mm0, mm0
  psubusb mm0, [eax]
  add eax, edx
  mov ecx, [esp+ 4] ; Dst
  push esi
  pcmpeqb mm1, mm1
  pcmpeqb mm2, mm2
  mov esi, mm_minusone
  psubusb mm1, [byte eax]
  psubusb mm2, [eax+edx]
  lea eax, [eax+2*edx]
  movq mm6, [esi]
  movq mm7, [esi]
  pavgb mm0, mm1
  pavgb mm1, mm2
  psubusb mm6, mm0
  psubusb mm7, mm1
  movq [ecx], mm6
  movq [ecx+edx], mm7
  lea ecx, [ecx+2*edx]
  pcmpeqb mm3, mm3
  pcmpeqb mm4, mm4
  psubusb mm3, [eax]
  psubusb mm4, [eax+edx]
  lea eax, [eax+2*edx]
  pavgb mm2, mm3
  pavgb mm3, mm4
  movq mm0, [esi]
  movq mm1, [esi]
  psubusb mm0, mm2
  psubusb mm1, mm3
  movq [ecx], mm0
  movq [ecx+edx], mm1
  lea ecx,[ecx+2*edx]

  pcmpeqb mm5, mm5
  pcmpeqb mm6, mm6
  psubusb mm5, [eax]
  psubusb mm6, [eax+edx]
  lea eax, [eax+2*edx]
  pavgb mm4, mm5
  pavgb mm5, mm6
  movq mm2, [esi]
  movq mm3, [esi]
  psubusb mm2, mm4
  psubusb mm3, mm5
  movq [ecx], mm2
  movq [ecx+edx], mm3
  lea ecx, [ecx+2*edx]
  pcmpeqb mm7, mm7
  pcmpeqb mm0, mm0
  psubusb mm7, [eax]
  psubusb mm0, [eax+edx]
  pavgb mm6, mm7
  pavgb mm7, mm0
  movq mm4, [esi]
  movq mm5, [esi]
  psubusb mm4, mm6
  pop esi
  psubusb mm5, mm7
  movq [ecx], mm4
  movq [ecx+edx], mm5
  ret

;-----------------------------------------------------------------------------
;
; void interpolate8x8_halfpel_hv_3dne(uint8_t * const dst,
;                       const uint8_t * const src,
;                       const uint32_t stride,
;                       const uint32_t rounding);
;
;
;-----------------------------------------------------------------------------

; The trick is to correct the result of 'pavgb' with some combination of the
; lsb's of the 4 input values i,j,k,l, and their intermediate 'pavgb' (s and t).
; The boolean relations are:
;   (i+j+k+l+3)/4 = (s+t+1)/2 - (ij&kl)&st
;   (i+j+k+l+2)/4 = (s+t+1)/2 - (ij|kl)&st
;   (i+j+k+l+1)/4 = (s+t+1)/2 - (ij&kl)|st
;   (i+j+k+l+0)/4 = (s+t+1)/2 - (ij|kl)|st
; with  s=(i+j+1)/2, t=(k+l+1)/2, ij = i^j, kl = k^l, st = s^t.

; Moreover, we process 2 lines at a times, for better overlapping (~15% faster).

%macro COPY_HV_SSE_RND0 0

  movq mm0, [eax+edx]
  movq mm1, [eax+edx+1]

  movq mm6, mm0
  pavgb mm0, mm1        ; mm0=(j+k+1)/2. preserved for next step
  lea eax, [eax+2*edx]
  pxor mm1, mm6         ; mm1=(j^k).     preserved for next step

  por mm3, mm1          ; ij |= jk
  movq mm6, mm2
  pxor mm6, mm0         ; mm6 = s^t
  pand mm3, mm6         ; (ij|jk) &= st
  pavgb mm2, mm0        ; mm2 = (s+t+1)/2
  movq mm6, [eax]
  pand mm3, mm7         ; mask lsb
  psubb mm2, mm3        ; apply.

  movq [ecx], mm2

  movq mm2, [eax]
  movq mm3, [eax+1]
  pavgb mm2, mm3        ; preserved for next iteration
  pxor mm3, mm6         ; preserved for next iteration

  por mm1, mm3
  movq mm6, mm0
  pxor mm6, mm2
  pand mm1, mm6
  pavgb mm0, mm2

  pand mm1, mm7
  psubb mm0, mm1

  movq [ecx+edx], mm0
%endmacro

%macro COPY_HV_SSE_RND1 0
  movq mm0, [eax+edx]
  movq mm1, [eax+edx+1]

  movq mm6, mm0
  pavgb mm0, mm1        ; mm0=(j+k+1)/2. preserved for next step
  lea eax,[eax+2*edx]
  pxor mm1, mm6         ; mm1=(j^k).     preserved for next step

  pand mm3, mm1
  movq mm6, mm2
  pxor mm6, mm0
  por mm3, mm6
  pavgb mm2, mm0
  movq mm6, [eax]
  pand mm3, mm7
  psubb mm2, mm3

  movq [ecx], mm2

  movq mm2, [eax]
  movq mm3, [eax+1]
  pavgb mm2, mm3        ; preserved for next iteration
  pxor mm3, mm6         ; preserved for next iteration

  pand mm1, mm3
  movq mm6, mm0
  pxor mm6, mm2
  por mm1, mm6
  pavgb mm0, mm2
  pand mm1, mm7
  psubb mm0, mm1
  movq [ecx+edx], mm0
%endmacro

ALIGN 16
interpolate8x8_halfpel_hv_3dne:
  mov eax, [esp+ 8]     ; Src
  mov edx, [esp+12]     ; stride
  dec dword [esp+16]    ; rounding

    ; loop invariants: mm2=(i+j+1)/2  and  mm3= i^j
  movq mm2, [eax]
  movq mm3, [eax+1]
  movq mm6, mm2
  pavgb mm2, mm3
  pxor mm3, mm6         ; mm2/mm3 ready
  mov ecx, [esp+ 4]     ; Dst
  movq mm7, [mmx_one]

  jz near .rounding1
  lea ebp,[byte ebp]
  COPY_HV_SSE_RND0
  lea ecx,[ecx+2*edx]
  COPY_HV_SSE_RND0
  lea ecx,[ecx+2*edx]
  COPY_HV_SSE_RND0
  lea ecx,[ecx+2*edx]
  COPY_HV_SSE_RND0
  ret

ALIGN 16
.rounding1
  COPY_HV_SSE_RND1
  lea ecx,[ecx+2*edx]
  COPY_HV_SSE_RND1
  lea ecx,[ecx+2*edx]
  COPY_HV_SSE_RND1
  lea ecx,[ecx+2*edx]
  COPY_HV_SSE_RND1
  ret
