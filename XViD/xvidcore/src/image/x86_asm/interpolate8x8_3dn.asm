;/*****************************************************************************
; *
; *  XVID MPEG-4 VIDEO CODEC
; *  - 3dnow 8x8 block-based halfpel interpolation -
; *
; *  Copyright(C) 2001 Peter Ross <pross@xvid.org>
; *               2002 Michael Militzer <isibaar@xvid.org>
; *               2002 Pascal Massimino <skal@planet-d.net>
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
; Read Only data
;=============================================================================

%ifdef FORMAT_COFF
SECTION .rodata
%else
SECTION .rodata align=16
%endif

ALIGN 16
mmx_one:
	times 8 db 1

;=============================================================================
; Code
;=============================================================================

SECTION .text

cglobal interpolate8x8_halfpel_h_3dn
cglobal interpolate8x8_halfpel_v_3dn
cglobal interpolate8x8_halfpel_hv_3dn

;-----------------------------------------------------------------------------
;
; void interpolate8x8_halfpel_h_3dn(uint8_t * const dst,
;                       const uint8_t * const src,
;                       const uint32_t stride,
;                       const uint32_t rounding);
;
;-----------------------------------------------------------------------------

%macro COPY_H_3DN_RND0 0
  movq mm0, [eax]
  pavgusb mm0, [eax+1]
  movq mm1, [eax+edx]
  pavgusb mm1, [eax+edx+1]
  lea eax, [eax+2*edx]
  movq [ecx], mm0
  movq [ecx+edx], mm1
%endmacro

%macro COPY_H_3DN_RND1 0
  movq mm0, [eax]
  movq mm1, [eax+edx]
  movq mm4, mm0
  movq mm5, mm1
  movq mm2, [eax+1]
  movq mm3, [eax+edx+1]
  pavgusb mm0, mm2
  pxor mm2, mm4
  pavgusb mm1, mm3
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
interpolate8x8_halfpel_h_3dn:

  mov eax, [esp+16] ; rounding
  mov ecx, [esp+ 4] ; Dst
  test eax, eax
  mov eax, [esp+ 8] ; Src
  mov edx, [esp+12] ; stride

  jnz near .rounding1

  COPY_H_3DN_RND0
  lea ecx, [ecx+2*edx]
  COPY_H_3DN_RND0
  lea ecx, [ecx+2*edx]
  COPY_H_3DN_RND0
  lea ecx, [ecx+2*edx]
  COPY_H_3DN_RND0
  ret

.rounding1
  ; we use: (i+j)/2 = ( i+j+1 )/2 - (i^j)&1
  movq mm7, [mmx_one]
  COPY_H_3DN_RND1
  lea ecx, [ecx+2*edx]
  COPY_H_3DN_RND1
  lea ecx, [ecx+2*edx]
  COPY_H_3DN_RND1
  lea ecx, [ecx+2*edx]
  COPY_H_3DN_RND1
  ret


;-----------------------------------------------------------------------------
;
; void interpolate8x8_halfpel_v_3dn(uint8_t * const dst,
;                       const uint8_t * const src,
;                       const uint32_t stride,
;                       const uint32_t rounding);
;
;-----------------------------------------------------------------------------

%macro COPY_V_3DN_RND0 0
  movq mm0, [eax]
  movq mm1, [eax+edx]
  pavgusb mm0, mm1
  pavgusb mm1, [eax+2*edx]
  lea eax, [eax+2*edx]
  movq [ecx], mm0
  movq [ecx+edx], mm1
%endmacro

%macro COPY_V_3DN_RND1 0
  movq mm0, mm2
  movq mm1, [eax]
  movq mm2, [eax+edx]
  lea eax, [eax+2*edx]
  movq mm4, mm0
  movq mm5, mm1
  pavgusb mm0, mm1
  pxor mm4, mm1
  pavgusb mm1, mm2
  pxor mm5, mm2
  pand mm4, mm7         ; lsb's of (i^j)...
  pand mm5, mm7         ; lsb's of (i^j)...
  psubb mm0, mm4        ; ...are substracted from result of pavgusb
  movq [ecx], mm0
  psubb mm1, mm5        ; ...are substracted from result of pavgusb
  movq [ecx+edx], mm1
%endmacro

ALIGN 16
interpolate8x8_halfpel_v_3dn:

  mov eax, [esp+16] ; rounding
  mov ecx, [esp+ 4] ; Dst
  test eax,eax
  mov eax, [esp+ 8] ; Src
  mov edx, [esp+12] ; stride

    ; we process 2 line at a time

  jnz near .rounding1

  COPY_V_3DN_RND0
  lea ecx, [ecx+2*edx]
  COPY_V_3DN_RND0
  lea ecx, [ecx+2*edx]
  COPY_V_3DN_RND0
  lea ecx, [ecx+2*edx]
  COPY_V_3DN_RND0
  ret

.rounding1
 ; we use: (i+j)/2 = ( i+j+1 )/2 - (i^j)&1
  movq mm7, [mmx_one]
  movq mm2, [eax]       ; loop invariant
  add eax, edx

  COPY_V_3DN_RND1
  lea ecx, [ecx+2*edx]
  COPY_V_3DN_RND1
  lea ecx, [ecx+2*edx]
  COPY_V_3DN_RND1
  lea ecx, [ecx+2*edx]
  COPY_V_3DN_RND1
  ret


;-----------------------------------------------------------------------------
;
; void interpolate8x8_halfpel_hv_3dn(uint8_t * const dst,
;                       const uint8_t * const src,
;                       const uint32_t stride,
;                       const uint32_t rounding);
;
;
;-----------------------------------------------------------------------------

; The trick is to correct the result of 'pavgusb' with some combination of the
; lsb's of the 4 input values i,j,k,l, and their intermediate 'pavgusb' (s and t).
; The boolean relations are:
;   (i+j+k+l+3)/4 = (s+t+1)/2 - (ij&kl)&st
;   (i+j+k+l+2)/4 = (s+t+1)/2 - (ij|kl)&st
;   (i+j+k+l+1)/4 = (s+t+1)/2 - (ij&kl)|st
;   (i+j+k+l+0)/4 = (s+t+1)/2 - (ij|kl)|st
; with  s=(i+j+1)/2, t=(k+l+1)/2, ij = i^j, kl = k^l, st = s^t.

; Moreover, we process 2 lines at a times, for better overlapping (~15% faster).

%macro COPY_HV_3DN_RND0 0
  lea eax, [eax+edx]

  movq mm0, [eax]
  movq mm1, [eax+1]

  movq mm6, mm0
  pavgusb mm0, mm1      ; mm0=(j+k+1)/2. preserved for next step
  lea eax, [eax+edx]
  pxor mm1, mm6         ; mm1=(j^k).     preserved for next step

  por mm3, mm1          ; ij |= jk
  movq mm6, mm2
  pxor mm6, mm0         ; mm6 = s^t
  pand mm3, mm6         ; (ij|jk) &= st
  pavgusb mm2, mm0      ; mm2 = (s+t+1)/2
  pand mm3, mm7         ; mask lsb
  psubb mm2, mm3        ; apply.

  movq [ecx], mm2

  movq mm2, [eax]
  movq mm3, [eax+1]
  movq mm6, mm2
  pavgusb mm2, mm3      ; preserved for next iteration
  lea ecx, [ecx+edx]
  pxor mm3, mm6         ; preserved for next iteration

  por mm1, mm3
  movq mm6, mm0
  pxor mm6, mm2
  pand mm1, mm6
  pavgusb mm0, mm2

  pand mm1, mm7
  psubb mm0, mm1

  movq [ecx], mm0
%endmacro

%macro COPY_HV_3DN_RND1 0
  lea eax,[eax+edx]

  movq mm0, [eax]
  movq mm1, [eax+1]

  movq mm6, mm0
  pavgusb mm0, mm1      ; mm0=(j+k+1)/2. preserved for next step
  lea eax, [eax+edx]
  pxor mm1, mm6         ; mm1=(j^k).     preserved for next step

  pand mm3, mm1
  movq mm6, mm2
  pxor mm6, mm0
  por mm3, mm6
  pavgusb mm2, mm0
  pand mm3, mm7
  psubb mm2, mm3

  movq [ecx], mm2

  movq mm2, [eax]
  movq mm3, [eax+1]
  movq mm6, mm2
  pavgusb mm2, mm3      ; preserved for next iteration
  lea ecx, [ecx+edx]
  pxor mm3, mm6         ; preserved for next iteration

  pand mm1, mm3
  movq mm6, mm0
  pxor mm6, mm2
  por mm1, mm6
  pavgusb mm0, mm2
  pand mm1, mm7
  psubb mm0, mm1

  movq [ecx], mm0
%endmacro

ALIGN 16
interpolate8x8_halfpel_hv_3dn
  mov eax, [esp+16] ; rounding
  mov ecx, [esp+ 4] ; Dst
  test eax, eax
  mov eax, [esp+ 8] ; Src
  mov edx, [esp+12] ; stride

  movq mm7, [mmx_one]

    ; loop invariants: mm2=(i+j+1)/2  and  mm3= i^j
  movq mm2, [eax]
  movq mm3, [eax+1]
  movq mm6, mm2
  pavgusb mm2, mm3
  pxor mm3, mm6     ; mm2/mm3 ready

  jnz near .rounding1

  COPY_HV_3DN_RND0
  add ecx, edx
  COPY_HV_3DN_RND0
  add ecx, edx
  COPY_HV_3DN_RND0
  add ecx, edx
  COPY_HV_3DN_RND0
  ret

.rounding1
  COPY_HV_3DN_RND1
  add ecx, edx
  COPY_HV_3DN_RND1
  add ecx, edx
  COPY_HV_3DN_RND1
  add ecx, edx
  COPY_HV_3DN_RND1
  ret
