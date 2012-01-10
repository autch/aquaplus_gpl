;/****************************************************************************
; *
; *  XVID MPEG-4 VIDEO CODEC
; *  - 8<->16 bit transfer functions -
; *
; *  Copyright (C) 2002 Jaan Kalda
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

ALIGN 8
mm_zero:
	dd 0,0
;=============================================================================
; Macros
;=============================================================================

%macro nop4 0
	db 08Dh, 074h, 026h, 0
%endmacro

;=============================================================================
; Code
;=============================================================================

SECTION .text

cglobal transfer_8to16copy_3dne
cglobal transfer_16to8copy_3dne
cglobal transfer_8to16sub_3dne
cglobal transfer_8to16subro_3dne
cglobal transfer_8to16sub2_3dne
cglobal transfer_16to8add_3dne
cglobal transfer8x8_copy_3dne

;-----------------------------------------------------------------------------
;
; void transfer_8to16copy_3dne(int16_t * const dst,
;							const uint8_t * const src,
;							uint32_t stride);
;
;-----------------------------------------------------------------------------

ALIGN 16
transfer_8to16copy_3dne:

  mov eax, [esp+ 8] ; Src
  mov edx, [esp+12] ; Stride
  mov ecx, [esp+ 4] ; Dst
  punpcklbw mm0, [byte eax]
  punpcklbw mm1, [eax+4]
  movq mm2, [eax+edx]
  movq mm3, [eax+edx]
  pxor mm7, mm7
  lea eax, [eax+2*edx]
  punpcklbw mm2, mm7
  punpckhbw mm3, mm7
  psrlw mm0, 8
  psrlw mm1, 8
  punpcklbw mm4, [eax]
  punpcklbw mm5, [eax+edx+4]
  movq [byte ecx+0*64], mm0
  movq [ecx+0*64+8], mm1
  punpcklbw mm6, [eax+edx]
  punpcklbw mm7, [eax+4]
  lea eax, [byte eax+2*edx]
  psrlw mm4, 8
  psrlw mm5, 8
  punpcklbw mm0, [eax]
  punpcklbw mm1, [eax+edx+4]
  movq [ecx+0*64+16], mm2
  movq [ecx+0*64+24], mm3
  psrlw mm6, 8
  psrlw mm7, 8
  punpcklbw mm2, [eax+edx]
  punpcklbw mm3, [eax+4]
  lea eax, [byte eax+2*edx]
  movq [byte ecx+0*64+32], mm4
  movq [ecx+0*64+56], mm5
  psrlw mm0, 8
  psrlw mm1, 8
  punpcklbw mm4, [eax]
  punpcklbw mm5, [eax+edx+4]
  movq [byte ecx+0*64+48], mm6
  movq [ecx+0*64+40], mm7
  psrlw mm2, 8
  psrlw mm3, 8
  punpcklbw mm6, [eax+edx]
  punpcklbw mm7, [eax+4]
  movq [byte ecx+1*64], mm0
  movq [ecx+1*64+24], mm1
  psrlw mm4, 8
  psrlw mm5, 8
  movq [ecx+1*64+16], mm2
  movq [ecx+1*64+8], mm3
  psrlw mm6, 8
  psrlw mm7, 8
  movq [byte ecx+1*64+32], mm4
  movq [ecx+1*64+56], mm5
  movq [byte ecx+1*64+48], mm6
  movq [ecx+1*64+40], mm7
  ret



;-----------------------------------------------------------------------------
;
; void transfer_16to8copy_3dne(uint8_t * const dst,
;							const int16_t * const src,
;							uint32_t stride);
;
;-----------------------------------------------------------------------------

ALIGN 16
transfer_16to8copy_3dne:

  mov eax, [esp+ 8] ; Src
  mov ecx, [esp+ 4] ; Dst
  mov edx, [esp+12] ; Stride

  movq mm0, [byte eax+0*32]
  packuswb mm0, [eax+0*32+8]
  movq mm1, [eax+0*32+16]
  packuswb mm1, [eax+0*32+24]
  movq mm5, [eax+2*32+16]
  movq mm2, [eax+1*32]
  packuswb mm2, [eax+1*32+8]
  movq mm3, [eax+1*32+16]
  packuswb mm3, [eax+1*32+24]
  movq mm6, [eax+3*32]
  movq mm4, [eax+2*32]
  packuswb mm4, [eax+2*32+8]
  packuswb mm5, [eax+2*32+24]
  movq mm7, [eax+3*32+16]
  packuswb mm7, [eax+3*32+24]
  packuswb mm6, [eax+3*32+8]
  movq [ecx], mm0
  lea eax, [3*edx]
  add eax, ecx
  movq [ecx+edx], mm1
  movq [ecx+2*edx], mm2
  movq [byte eax], mm3
  movq [ecx+4*edx], mm4
  lea ecx, [byte ecx+4*edx]
  movq [eax+2*edx], mm5
  movq [eax+4*edx], mm7
  movq [ecx+2*edx], mm6
  ret

;-----------------------------------------------------------------------------
;
; void transfer_8to16sub_3dne(int16_t * const dct,
;				uint8_t * const cur,
;				const uint8_t * const ref,
;				const uint32_t stride);
;
;-----------------------------------------------------------------------------

; when second argument == 1, reference (ebx) block is to current (eax)
%macro COPY_8_TO_16_SUB 2
  movq mm1, [eax]      ; cur
  movq mm0, mm1
  movq mm4, [ecx]      ; ref
  movq mm6, mm4
%if %2 == 1
  movq [eax], mm4
%endif
  punpckhbw mm1, mm7
  punpckhbw mm6, mm7
  punpcklbw mm4, mm7
ALIGN 8
  movq mm2, [byte eax+edx]
  punpcklbw mm0, mm7
  movq mm3, [byte eax+edx]
  punpcklbw mm2, mm7
  movq mm5, [byte ecx+edx]  ; ref
  punpckhbw mm3, mm7
%if %2 == 1
  movq [byte eax+edx], mm5
%endif
  psubsw mm1, mm6

  movq mm6, mm5
  psubsw mm0, mm4
%if (%1 < 3)
  lea eax,[eax+2*edx]
  lea ecx,[ecx+2*edx]
%else
  mov ecx,[esp]
  add esp,byte 4
%endif
  movq [edi+%1*32+ 8], mm1
  movq [byte edi+%1*32+ 0], mm0 ; dst
  punpcklbw mm5, mm7
  punpckhbw mm6, mm7
  psubsw mm2, mm5
  psubsw mm3, mm6
  movq [edi+%1*32+16], mm2
  movq [edi+%1*32+24], mm3
%endmacro

ALIGN 16
transfer_8to16sub_3dne:
  mov eax, [esp + 8] ; Cur
  mov ecx, [esp +12] ; Ref
  push edi
  mov edx, [dword esp+4+16] ; Stride
  mov edi, [esp+4+ 4] ; Dst
  pxor mm7, mm7
  nop
ALIGN 4
  COPY_8_TO_16_SUB 0, 1
  COPY_8_TO_16_SUB 1, 1
  COPY_8_TO_16_SUB 2, 1
  COPY_8_TO_16_SUB 3, 1
  mov edi, ecx
  ret

ALIGN 16
transfer_8to16subro_3dne:
  mov eax, [esp + 8] ; Cur
  mov ecx, [esp +12] ; Ref
  push edi
  mov edx, [dword esp+4+16] ; Stride
  mov edi, [esp+4+ 4] ; Dst
  pxor mm7, mm7
  nop
ALIGN 4
  COPY_8_TO_16_SUB 0, 0
  COPY_8_TO_16_SUB 1, 0
  COPY_8_TO_16_SUB 2, 0
  COPY_8_TO_16_SUB 3, 0
  mov edi, ecx
  ret


;-----------------------------------------------------------------------------
;
; void transfer_8to16sub2_3dne(int16_t * const dct,
;				uint8_t * const cur,
;				const uint8_t * ref1,
;				const uint8_t * ref2,
;				const uint32_t stride)
;
;-----------------------------------------------------------------------------

%macro COPY_8_TO_16_SUB2_SSE 1
  db 0Fh, 6Fh, 44h, 20h, 00  ;movq mm0, [byte eax]      ; cur
  punpcklbw mm0, mm7
  movq mm2, [byte eax+edx]
  punpcklbw mm2, mm7
  db 0Fh, 6Fh, 4ch, 20h, 00 ;movq mm1, [byte eax]
  punpckhbw mm1, mm7
  movq mm3, [byte eax+edx]
  punpckhbw mm3, mm7

  movq mm4, [byte ebx]      ; ref1
  pavgb mm4, [byte esi]     ; ref2
  movq [eax], mm4
  movq mm5, [ebx+edx]  ; ref
  pavgb mm5, [esi+edx] ; ref2
  movq [eax+edx], mm5
  movq mm6, mm4
  punpcklbw mm4, mm7
  punpckhbw mm6, mm7
%if (%1 < 3)
  lea esi,[esi+2*edx]
  lea ebx,[byte ebx+2*edx]
  lea eax,[eax+2*edx]
%else
  mov esi,[esp]
  mov ebx,[esp+4]
  add esp,byte 8
%endif
  psubsw mm0, mm4
  psubsw mm1, mm6
  movq mm6, mm5
  punpcklbw mm5, mm7
  punpckhbw mm6, mm7
  psubsw mm2, mm5
  psubsw mm3, mm6
  movq [byte ecx+%1*32+ 0], mm0 ; dst
  movq [ecx+%1*32+ 8], mm1
  movq [ecx+%1*32+16], mm2
  movq [ecx+%1*32+24], mm3
%endmacro

ALIGN 16
transfer_8to16sub2_3dne:
  mov edx, [esp +20] ; Stride
  mov ecx, [esp  + 4] ; Dst
  mov eax, [esp  + 8] ; Cur
  push ebx
  lea ebp,[byte ebp]
  mov ebx, [esp+4+12] ; Ref1
  push esi
  pxor mm7, mm7
  mov esi, [esp+8+16] ; Ref2
  nop4
  COPY_8_TO_16_SUB2_SSE 0
  COPY_8_TO_16_SUB2_SSE 1
  COPY_8_TO_16_SUB2_SSE 2
  COPY_8_TO_16_SUB2_SSE 3

  ret


;-----------------------------------------------------------------------------
;
; void transfer_16to8add_3dne(uint8_t * const dst,
;						const int16_t * const src,
;						uint32_t stride);
;
;-----------------------------------------------------------------------------

%macro COPY_16_TO_8_ADD 1
  db 0Fh, 6Fh, 44h, 21h, 00 ;movq mm0, [byte ecx]
  punpcklbw mm0, mm7
  movq mm2, [byte ecx+edx]
  punpcklbw mm2, mm7
  db 0Fh, 6Fh, 4ch, 21h, 00 ;movq mm1, [byte ecx]
  punpckhbw mm1, mm7
  movq mm3, [byte ecx+edx]
  punpckhbw mm3, mm7
  paddsw mm0, [byte eax+%1*32+ 0]
  paddsw mm1, [eax+%1*32+ 8]
  paddsw mm2, [eax+%1*32+16]
  paddsw mm3, [eax+%1*32+24]
  packuswb mm0, mm1
  packuswb mm2, mm3
  mov esp,esp
  movq [byte ecx], mm0
  movq [ecx+edx], mm2
%endmacro


ALIGN 16
transfer_16to8add_3dne:
  mov ecx, [esp+ 4] ; Dst
  mov edx, [esp+12] ; Stride
  mov eax, [esp+ 8] ; Src
  pxor mm7, mm7
  nop

  COPY_16_TO_8_ADD 0
  lea ecx,[byte ecx+2*edx]
  COPY_16_TO_8_ADD 1
  lea ecx,[byte ecx+2*edx]
  COPY_16_TO_8_ADD 2
  lea ecx,[byte ecx+2*edx]
  COPY_16_TO_8_ADD 3
  ret

;-----------------------------------------------------------------------------
;
; void transfer8x8_copy_3dne(uint8_t * const dst,
;					const uint8_t * const src,
;					const uint32_t stride);
;
;
;-----------------------------------------------------------------------------

%macro COPY_8_TO_8 0
  movq mm0, [byte  eax]
  movq mm1, [eax+edx]
  movq [byte ecx], mm0
  lea eax,[byte eax+2*edx]
  movq [ecx+edx], mm1
%endmacro

ALIGN 16
transfer8x8_copy_3dne:
  mov eax, [esp+ 8] ; Src
  mov edx, [esp+12] ; Stride
  mov ecx, [esp+ 4] ; Dst

  COPY_8_TO_8
  lea ecx,[byte ecx+2*edx]
  COPY_8_TO_8
  lea ecx,[byte ecx+2*edx]
  COPY_8_TO_8
  lea ecx,[byte ecx+2*edx]
  COPY_8_TO_8
  ret
