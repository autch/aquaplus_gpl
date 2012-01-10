;/****************************************************************************
; *
; *  XVID MPEG-4 VIDEO CODEC
; *  - MMX CBP computation -
; *
; *  Copyright (C) 2001-2003 Peter Ross <pross@xvid.org>
; *                2002-2003 Pascal Massimino <skal@planet-d.net>
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
; * $Id$
; *
; ***************************************************************************/

BITS 32

;=============================================================================
; Macros
;=============================================================================

%macro cglobal 1
	%ifdef PREFIX
		global _%1
		%define %1 _%1
	%else
		global %1
	%endif
%endmacro

;=============================================================================
; Local data
;=============================================================================

%ifdef FORMAT_COFF
SECTION .rodata
%else
SECTION .rodata align=16
%endif

ALIGN 16

ignore_dc:
	dw 0, -1, -1, -1, -1, -1, -1, -1

;=============================================================================
; Code
;=============================================================================

SECTION .text

cglobal calc_cbp_mmx

;-----------------------------------------------------------------------------
; uint32_t calc_cbp_mmx(const int16_t coeff[6][64]);
;-----------------------------------------------------------------------------

ALIGN 16
calc_cbp_mmx:
  push ebx
  push esi

  mov esi, [esp + 8 + 4]	; coeff
  xor eax, eax			; cbp = 0
  mov edx, (1 << 5)

  movq mm7, [ignore_dc]

.loop
  movq mm0, [esi]
  movq mm1, [esi+8]
  pand mm0, mm7

  por mm0, [esi+16]
  por mm1, [esi+24]

  por mm0, [esi+32]
  por mm1, [esi+40]

  por mm0, [esi+48]
  por mm1, [esi+56]

  por mm0, [esi+64]
  por mm1, [esi+72]

  por mm0, [esi+80]
  por mm1, [esi+88]

  por mm0, [esi+96]
  por mm1, [esi+104]

  por mm0, [esi+112]
  por mm1, [esi+120]

  por mm0, mm1
  movq mm1, mm0
  psrlq mm1, 32
  lea esi, [esi + 128]

  por mm0, mm1
  movd ebx, mm0

  test ebx, ebx
  jz .next
  or eax, edx     ; cbp |= 1 << (5-i)

.next
  shr edx,1
  jnc .loop

  pop esi
  pop ebx

  ret
