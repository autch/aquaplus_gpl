;/****************************************************************************
; *
; *  XVID MPEG-4 VIDEO CODEC
; *  - 3DNow sad operators w/o XMM instructions -
; *
; *  Copyright(C) 2002 Peter ross <pross@xvid.org>
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
%macro SADBI_16x16_3DN 0
  movq mm0, [eax] ; src
  movq mm2, [eax+8]

  movq mm1, [edx] ; ref1
  movq mm3, [edx+8]
  pavgusb mm1, [ebx] ; ref2
  lea edx, [edx+ecx]
  pavgusb mm3, [ebx+8]
  lea ebx, [ebx+ecx]

  movq mm4, mm0
  lea eax, [eax+ecx]
  psubusb mm0, mm1
  movq mm5, mm2
  psubusb mm2, mm3

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

  paddusw mm0,mm1
  paddusw mm2,mm3
  paddusw mm6,mm0
  paddusw mm6,mm2
%endmacro

%macro SADBI_8x8_3DN 0
  movq mm0, [eax] ; src
  movq mm2, [eax+ecx]

  movq mm1, [edx] ; ref1
  movq mm3, [edx+ecx]
  pavgusb mm1, [ebx] ; ref2
  lea edx, [edx+2*ecx]
  pavgusb mm3, [ebx+ecx]
  lea ebx, [ebx+2*ecx]

  movq mm4, mm0
  lea eax, [eax+2*ecx]
  psubusb mm0, mm1
  movq mm5, mm2
  psubusb mm2, mm3

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

  paddusw mm0,mm1
  paddusw mm2,mm3
  paddusw mm6,mm0
  paddusw mm6,mm2
%endmacro

;=============================================================================
; Code
;=============================================================================

SECTION .text

cglobal  sad16bi_3dn
cglobal  sad8bi_3dn

;-----------------------------------------------------------------------------
;
; uint32_t sad16bi_3dn(const uint8_t * const cur,
; const uint8_t * const ref1,
; const uint8_t * const ref2,
; const uint32_t stride);
;
;-----------------------------------------------------------------------------

ALIGN 16
sad16bi_3dn:
  push ebx
  mov eax, [esp+4+ 4] ; Src
  mov edx, [esp+4+ 8] ; Ref1
  mov ebx, [esp+4+12] ; Ref2
  mov ecx, [esp+4+16] ; Stride

  pxor mm6, mm6 ; accum2
  pxor mm7, mm7
.Loop
  SADBI_16x16_3DN
  SADBI_16x16_3DN
  SADBI_16x16_3DN
  SADBI_16x16_3DN
  SADBI_16x16_3DN
  SADBI_16x16_3DN
  SADBI_16x16_3DN
  SADBI_16x16_3DN

  SADBI_16x16_3DN
  SADBI_16x16_3DN
  SADBI_16x16_3DN
  SADBI_16x16_3DN
  SADBI_16x16_3DN
  SADBI_16x16_3DN
  SADBI_16x16_3DN
  SADBI_16x16_3DN

  pmaddwd mm6, [mmx_one] ; collapse
  movq mm7, mm6
  psrlq mm7, 32
  paddd mm6, mm7

  movd eax, mm6

  pop ebx

  ret

;-----------------------------------------------------------------------------
;
; uint32_t sad8bi_3dn(const uint8_t * const cur,
; const uint8_t * const ref1,
; const uint8_t * const ref2,
; const uint32_t stride);
;
;-----------------------------------------------------------------------------

ALIGN 16
sad8bi_3dn:
  push ebx
  mov eax, [esp+4+ 4] ; Src
  mov edx, [esp+4+ 8] ; Ref1
  mov ebx, [esp+4+12] ; Ref2
  mov ecx, [esp+4+16] ; Stride

  pxor mm6, mm6 ; accum2
  pxor mm7, mm7
.Loop
  SADBI_8x8_3DN
  SADBI_8x8_3DN
  SADBI_8x8_3DN
  SADBI_8x8_3DN

  pmaddwd mm6, [mmx_one] ; collapse
  movq mm7, mm6
  psrlq mm7, 32
  paddd mm6, mm7

  movd eax, mm6

  pop ebx

  ret
