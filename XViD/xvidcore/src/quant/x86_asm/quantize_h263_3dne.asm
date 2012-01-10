;/**************************************************************************
; *
; *  XVID MPEG-4 VIDEO CODEC
; *  - 3dne Quantization/Dequantization -
; *
; *  Copyright(C) 2002-2003 Jaan Kalda
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
; *************************************************************************/
;
; these 3dne functions are compatible with iSSE, but are optimized specifically for
; K7 pipelines

; enable dequant saturate [-2048,2047], test purposes only.
%define SATURATE

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
; Local data
;=============================================================================

%ifdef FORMAT_COFF
SECTION .rodata
%else
SECTION .rodata align=16
%endif

align 4
int_div:
	dd 0
%assign i 1
%rep 255
	dd  (1 << 16) / (i) + 1
	%assign i i+1
%endrep

ALIGN 16
plus_one:
	times 8 dw 1

;-----------------------------------------------------------------------------
; subtract by Q/2 table
;-----------------------------------------------------------------------------

ALIGN 16
mmx_sub:
%assign i 1
%rep 31
	times 4 dw i / 2
	%assign i i+1
%endrep


;-----------------------------------------------------------------------------
;
; divide by 2Q table
;
; use a shift of 16 to take full advantage of _pmulhw_
; for q=1, _pmulhw_ will overflow so it is treated seperately
; (3dnow2 provides _pmulhuw_ which wont cause overflow)
;
;-----------------------------------------------------------------------------

ALIGN 16
mmx_div:
%assign i 1
%rep 31
	times 4 dw  (1 << 16) / (i * 2) + 1
	%assign i i+1
%endrep

;-----------------------------------------------------------------------------
; add by (odd(Q) ? Q : Q - 1) table
;-----------------------------------------------------------------------------

ALIGN 16
mmx_add:
%assign i 1
%rep 31
	%if i % 2 != 0
	times 4 dw i
	%else
	times 4 dw i - 1
	%endif
	%assign i i+1
%endrep

;-----------------------------------------------------------------------------
; multiple by 2Q table
;-----------------------------------------------------------------------------

ALIGN 16
mmx_mul:
%assign i 1
%rep 31
	times 4 dw i * 2
	%assign i i+1
%endrep

;-----------------------------------------------------------------------------
; saturation limits
;-----------------------------------------------------------------------------

ALIGN 8
mmx_32768_minus_2048:
	times 4 dw (32768-2048)
mmx_32767_minus_2047:
	times 4 dw (32767-2047)

ALIGN 16
mmx_2047:
	times 4 dw 2047

ALIGN 8
mmzero:
	dd 0, 0
int2047:
	dd 2047
int_2048:
	dd -2048

;=============================================================================
; Code
;=============================================================================

SECTION .text


;-----------------------------------------------------------------------------
;
; uint32_t quant_h263_intra_3dne(int16_t * coeff,
;                                const int16_t const * data,
;                                const uint32_t quant,
;                                const uint32_t dcscalar,
;                                const uint16_t *mpeg_matrices);
;
;-----------------------------------------------------------------------------
;This is Athlon-optimized code (ca 70 clk per call)

%macro quant_intra1  1
  psubw mm1, mm0    ;A3
  psubw mm3, mm2    ;B3
%if (%1)
  psubw mm5, mm4    ;C8
  psubw mm7, mm6    ;D8
%endif

ALIGN 8
  db 0Fh, 6Fh, 64h, 21h, (%1 * 32 +16)  ;movq   mm4, [ecx + %1 * 32 +16+32] ;C1
  pmaxsw mm1, mm0   ;A4
  db 0Fh, 6Fh, 74h, 21h, (%1 * 32 +24)  ;movq   mm6, [ecx + %1 * 32 +24+32] ;D1
  pmaxsw mm3, mm2   ;B4


  psraw mm0, 15     ;A5
  psraw mm2, 15     ;B5
%if (%1)
  movq [edx + %1 * 32 + 16-32], mm5 ;C9
  movq [edx + %1 * 32 + 24-32], mm7 ;D9
%endif

  psrlw mm1, 1      ;A6
  psrlw mm3, 1      ;B6
  movq mm5, [ebx]   ;C2
  movq mm7, [ebx]   ;D2

  pxor mm1, mm0 ;A7
  pxor mm3, mm2 ;B7

  psubw mm5, mm4    ;C3
  psubw mm7, mm6    ;D3
  psubw mm1, mm0    ;A8
  psubw mm3, mm2    ;B8

%if (%1 == 0)
  push ebp
  movq mm0, [ecx + %1 * 32 +32]
%elif (%1 < 3)
  db 0Fh, 6Fh, 44h, 21h, (%1 * 32 +32)  ;movq   mm0, [ecx + %1 * 32 +32]    ;A1
%endif
  pmaxsw mm5, mm4   ;C4
%if (%1 < 3)
  db 0Fh, 6Fh, 54h, 21h, ( %1 * 32 +8+32)   ;movq   mm2, [ecx + %1 * 32 +8+32]  ;B1
%else
  cmp esp, esp
%endif
  pmaxsw mm7, mm6   ;D4

  psraw mm4, 15     ;C5
  psraw mm6, 15     ;D5
  movq [byte edx + %1 * 32], mm1    ;A9
  movq [edx + %1 * 32+8], mm3       ;B9


  psrlw mm5, 1      ;C6
  psrlw mm7, 1      ;D6
%if (%1 < 3)
  movq mm1, [ebx]   ;A2
  movq mm3, [ebx]   ;B2
%endif
%if (%1 == 3)
  imul eax, [int_div+4*edi]
%endif
  pxor mm5, mm4 ;C7
  pxor mm7, mm6 ;D7
%endm


%macro quant_intra  1
    ; Rules for athlon:
        ; 1) schedule latencies
        ; 2) add/mul and load/store in 2:1 proportion
        ; 3) avoid spliting >3byte instructions over 8byte boundaries

  psubw mm1, mm0    ;A3
  psubw mm3, mm2    ;B3
%if (%1)
  psubw mm5, mm4    ;C8
  psubw mm7, mm6    ;D8
%endif

ALIGN 8
  db 0Fh, 6Fh, 64h, 21h, (%1 * 32 +16)  ;movq   mm4, [ecx + %1 * 32 +16+32] ;C1
  pmaxsw mm1, mm0   ;A4
  db 0Fh, 6Fh, 74h, 21h, (%1 * 32 +24)  ;movq   mm6, [ecx + %1 * 32 +24+32] ;D1
  pmaxsw mm3, mm2   ;B4


  psraw mm0, 15     ;A5
  psraw mm2, 15     ;B5
%if (%1)
  movq [edx + %1 * 32 + 16-32], mm5 ;C9
  movq [edx + %1 * 32 + 24-32], mm7 ;D9
%endif

  pmulhw mm1, [esi] ;A6
  pmulhw mm3, [esi] ;B6
  movq mm5, [ebx]   ;C2
  movq mm7, [ebx]   ;D2

  nop
  nop
  pxor mm1, mm0 ;A7
  pxor mm3, mm2 ;B7

  psubw mm5, mm4    ;C3
  psubw mm7, mm6    ;D3
  psubw mm1, mm0    ;A8
  psubw mm3, mm2    ;B8


%if (%1 < 3)
  db 0Fh, 6Fh, 44h, 21h, (%1 * 32 +32) ;movq    mm0, [ecx + %1 * 32 +32]    ;A1
%endif
  pmaxsw mm5, mm4     ;C4
%if (%1 < 3)
  db 0Fh, 6Fh, 54h, 21h, ( %1 * 32 +8+32) ;movq mm2, [ecx + %1 * 32 +8+32]  ;B1
%else
  cmp esp, esp
%endif
  pmaxsw mm7,mm6        ;D4

  psraw mm4, 15     ;C5
  psraw mm6, 15     ;D5
  movq [byte edx + %1 * 32], mm1 ;A9
  movq [edx + %1 * 32+8], mm3     ;B9


  pmulhw mm5, [esi] ;C6
  pmulhw mm7, [esi] ;D6
%if (%1 < 3)
  movq mm1, [ebx]   ;A2
  movq mm3, [ebx]   ;B2
%endif
%if (%1 == 0)
  push ebp
%elif (%1 < 3)
  nop
%endif
  nop
%if (%1 == 3)
  imul eax, [int_div+4*edi]
%endif
  pxor mm5, mm4 ;C7
  pxor mm7, mm6 ;D7
%endmacro


ALIGN 16
cglobal quant_h263_intra_3dne
quant_h263_intra_3dne:

  mov eax, [esp + 12]       ; quant
  mov ecx, [esp + 8]        ; data
  mov edx, [esp + 4]        ; coeff
  cmp al, 1
  pxor mm1, mm1
  pxor mm3, mm3
  movq mm0, [ecx]           ; mm0 = [1st]
  movq mm2, [ecx + 8]
  push esi
  lea esi, [mmx_div + eax*8 - 8]

  push ebx
  mov ebx, mmzero
  push edi
  jz near .q1loop

  quant_intra 0
  mov ebp, [esp + 16 + 16]      ; dcscalar
                                ; NB -- there are 3 pushes in the function preambule and one more
                                ; in "quant_intra 0", thus an added offset of 16 bytes
  movsx eax, word [byte ecx]    ; DC

  quant_intra 1
  mov edi, eax
  sar edi, 31                       ; sign(DC)
  shr ebp, byte 1                   ; ebp = dcscalar/2

  quant_intra 2
  sub eax, edi                      ; DC (+1)
  xor ebp, edi                      ; sign(DC) dcscalar /2  (-1)
  mov edi, [esp + 16 + 16]          ; dscalar
  lea eax, [byte eax + ebp]         ; DC + sign(DC) dcscalar/2
  mov ebp, [byte esp]

  quant_intra 3
  psubw mm5, mm4                    ;C8
  mov esi, [esp + 12]               ; pop back the register value
  mov edi, [esp + 4]                ; pop back the register value
  sar eax, 16
  lea ebx, [byte eax + 1]           ; workaround for eax < 0
  cmovs eax, ebx                    ; conditionnaly move the corrected value
  mov [edx], ax                     ; coeff[0] = ax
  mov ebx, [esp + 8]                ; pop back the register value
  add esp, byte 16                  ; "quant_intra 0" pushed ebp, but we don't restore that one, just correct the stack offset by 16
  psubw mm7, mm6                    ;D8
  movq [edx + 3 * 32 + 16], mm5     ;C9
  movq [edx + 3 * 32 + 24], mm7     ;D9

  xor eax, eax
  ret

ALIGN 16

.q1loop
  quant_intra1 0
  mov ebp, [esp + 16 + 16]          ; dcscalar
  movsx eax, word [byte ecx]        ; DC

  quant_intra1 1
  mov edi, eax
  sar edi, 31                       ; sign(DC)
  shr ebp, byte 1                   ; ebp = dcscalar /2

  quant_intra1 2
  sub eax, edi                      ; DC (+1)
  xor ebp, edi                      ; sign(DC) dcscalar /2  (-1)
  mov edi, [esp + 16 + 16]          ; dcscalar
  lea eax, [byte eax + ebp]         ; DC + sign(DC) dcscalar /2
  mov ebp, [byte esp]

  quant_intra1 3
  psubw mm5, mm4                    ;C8
  mov esi, [dword esp + 12]         ; pop back the register value
  mov edi, [esp + 4]                ; pop back the register value
  sar eax, 16
  lea ebx, [byte eax + 1]           ; workaround for eax < 0
  cmovs eax, ebx                    ; conditionnaly move the corrected value
  mov [edx], ax                     ; coeff[0] = ax
  mov ebx, [esp + 8]                ; pop back the register value
  add esp, byte 16                  ; "quant_intra 0" pushed ebp, but we don't restore that one, just correct the stack offset by 16
  psubw mm7, mm6                    ;D8
  movq [edx + 3 * 32 + 16], mm5     ;C9
  movq [edx + 3 * 32 + 24], mm7     ;D9

  xor eax, eax
  ret




;-----------------------------------------------------------------------------
;
; uint32_t quant_h263_inter_3dne(int16_t * coeff,
;                                const int16_t const * data,
;                                const uint32_t quant,
;                                const uint16_t *mpeg_matrices);
;
;-----------------------------------------------------------------------------
;This is Athlon-optimized code (ca 90 clk per call)
;Optimized by Jaan, 30 Nov 2002


%macro quantinter 1
  movq mm1, [eax]               ;A2
  psraw mm3, 15                 ;B6
%if (%1)
  psubw mm2, mm6                ;C10
%endif
  psubw mm1, mm0                ;A3
  pmulhw mm4, mm7               ;B7
  movq mm6, [ecx + %1*24+16]    ;C1
  pmaxsw mm1, mm0               ;A4
  paddw mm5, mm4                ;B8
%if (%1)
  movq [edx + %1*24+16-24], mm2 ;C11
%endif
  psubusw mm1, [ebx]            ;A5 mm0 -= sub (unsigned, dont go < 0)
  pxor mm4, mm3                 ;B9
  movq mm2, [eax]               ;C2
  psraw mm0, 15                 ;A6
  psubw mm4, mm3                ;B10
  psubw mm2, mm6                ;C3
  pmulhw mm1, mm7               ;A7 mm0 = (mm0 / 2Q) >> 24
  movq mm3, [ecx + %1*24+8] ;B1
  pmaxsw mm2, mm6               ;C4
  paddw mm5, mm1                ;A8 sum += mm0
%if (%1)
  movq [edx + %1*24+8-24], mm4  ;B11
%else
  movq [edx + 120], mm4         ;B11
%endif
  psubusw mm2, [ebx]            ;C5
  pxor mm1, mm0                 ;A9 mm0 *= sign(mm0)
  movq mm4, [eax]               ;B2
  psraw mm6, 15                 ;C6
  psubw mm1, mm0                ;A10 undisplace
  psubw mm4, mm3                ;B3
  pmulhw mm2, mm7               ;C7
  movq mm0, [ecx + %1*24+24]    ;A1 mm0 = [1st]
  pmaxsw mm4, mm3               ;B4
  paddw mm5, mm2                ;C8
  movq [byte edx + %1*24], mm1  ;A11
  psubusw mm4, [ebx]            ;B5
  pxor mm2, mm6                 ;C9
%endmacro

%macro quantinter1 1
  movq mm0, [byte ecx + %1*16]  ;mm0 = [1st]
  movq mm3, [ecx + %1*16+8] ;
  movq mm1, [eax]
  movq mm4, [eax]
  psubw mm1, mm0
  psubw mm4, mm3
  pmaxsw mm1, mm0
  pmaxsw mm4, mm3
  psubusw mm1, mm6              ; mm0 -= sub (unsigned, dont go < 0)
  psubusw mm4, mm6              ;
  psraw mm0, 15
  psraw mm3, 15
  psrlw mm1, 1                  ; mm0 = (mm0 / 2Q) >> 16
  psrlw mm4, 1                  ;
  paddw mm5, mm1                ; sum += mm0
  pxor mm1, mm0                 ; mm0 *= sign(mm0)
  paddw mm5, mm4
  pxor mm4, mm3                 ;
  psubw mm1, mm0                ; undisplace
  psubw mm4, mm3
  cmp esp, esp
  movq [byte edx + %1*16], mm1
  movq [edx + %1*16+8], mm4
%endmacro

ALIGN 16
cglobal quant_h263_inter_3dne
quant_h263_inter_3dne:
  mov edx, [esp  + 4]               ; coeff
  mov ecx, [esp  + 8]               ; data
  mov eax, [esp  + 12]              ; quant
  push ebx

  pxor mm5, mm5                     ; sum
  nop
  lea ebx,[mmx_sub + eax * 8 - 8]   ; sub
  movq mm7, [mmx_div + eax * 8 - 8] ; divider

  cmp al, 1
  lea eax, [mmzero]
  jz near .q1loop
  cmp esp, esp
ALIGN 8
  movq mm3, [ecx + 120]     ;B1
  pxor mm4, mm4             ;B2
  psubw mm4, mm3            ;B3
  movq mm0, [ecx]           ;A1 mm0 = [1st]
  pmaxsw mm4, mm3           ;B4
  psubusw mm4, [ebx]        ;B5

  quantinter 0
  quantinter 1
  quantinter 2
  quantinter 3
  quantinter 4

  psraw mm3, 15             ;B6
  psubw mm2, mm6            ;C10
  pmulhw mm4, mm7           ;B7
  paddw mm5, mm4            ;B8
  pxor mm4, mm3             ;B9
  psubw mm4, mm3            ;B10
  movq [edx + 4*24+16], mm2 ;C11
  pop ebx
  movq [edx + 4*24+8], mm4  ;B11
  pmaddwd mm5, [plus_one]
  movq mm0, mm5
  punpckhdq mm5, mm5
  paddd mm0, mm5
  movd eax, mm0             ; return sum

  ret

ALIGN 16
.q1loop
  movq mm6, [byte ebx]

  quantinter1 0
  quantinter1 1
  quantinter1 2
  quantinter1 3
  quantinter1 4
  quantinter1 5
  quantinter1 6
  quantinter1 7

  pmaddwd mm5, [plus_one]
  movq mm0, mm5
  psrlq mm5, 32
  paddd mm0, mm5
  movd eax, mm0 ; return sum

  pop ebx

  ret

;-----------------------------------------------------------------------------
;
; uint32_t dequant_h263_intra_3dne(int16_t *data,
;                                  const int16_t const *coeff,
;                                  const uint32_t quant,
;                                  const uint32_t dcscalar,
;                                  const uint16_t *mpeg_matrices);
;
;-----------------------------------------------------------------------------

  ; this is the same as dequant_inter_3dne, except that we're
  ; saturating using 'pminsw' (saves 2 cycles/loop => ~5% faster)

;This is Athlon-optimized code (ca 106 clk per call)

%macro dequant 1
  movq mm1, [ecx+%1*24]         ; c  = coeff[i] ;A2
  psubw mm0, mm1                ;-c     ;A3 (1st dep)
%if (%1)
  paddw mm4, mm6                ;C11 mm6 free (4th+)
%endif
  pmaxsw mm0, mm1               ;|c|        ;A4 (2nd)
%if (%1)
  mov ebp, ebp
  pminsw mm4, [ebx]             ;C12 saturates to +2047 (5th+) later
%endif
  movq mm6, [esi]               ;0      ;A5  mm6 in use
  pandn mm7, [eax]              ;B9 offset = isZero ? 0 : quant_add (2nd)
%if (%1)
  pxor mm5, mm4                 ;C13 (6th+) 1later
%endif
  movq mm4, [esi]               ;C1 ;0
  mov esp, esp
  pcmpeqw mm6, [ecx+%1*24]      ;A6 (c ==0) ? -1 : 0 (1st)
ALIGN 4
  psraw mm1, 15                 ; sign(c)   ;A7 (2nd)
%if (%1)
  movq [edx+%1*24+16-24], mm5   ; C14 (7th) 2later
%endif
  paddw mm7, mm3                ;B10  offset +negate back (3rd)
  pmullw mm0, [edi]             ;*= 2Q  ;A8 (3rd+)
  paddw mm2, mm7                ;B11 mm7 free (4th+)
  lea ebp, [byte ebp]
  movq mm5, [ecx+%1*24+16]      ;C2 ; c  = coeff[i]
  psubw mm4, mm5                ;-c         ;C3 (1st dep)
  pandn mm6, [eax]              ;A9 offset = isZero ? 0 : quant_add (2nd)
  pminsw mm2, [ebx]             ;B12 saturates to +2047 (5th+)
  pxor mm3, mm2                 ;B13 (6th+)
  movq mm2, [byte esi]          ;B1 ;0
%if (%1)
  movq [edx+%1*24+8-24], mm3    ;B14 (7th)
%else
  movq [edx+120], mm3
%endif
  pmaxsw mm4, mm5               ;|c|        ;C4 (2nd)
  paddw mm6, mm1                ;A10  offset +negate back (3rd)
  movq mm3, [ecx+%1*24 + 8]     ;B2 ; c  = coeff[i]
  psubw mm2, mm3                ;-c     ;B3 (1st dep)
  paddw mm0, mm6                ;A11 mm6 free (4th+)
  movq mm6, [byte esi]          ;0          ;C5  mm6 in use
  pcmpeqw mm6, [ecx+%1*24+16]   ;C6 (c ==0) ? -1 : 0 (1st)
  pminsw mm0, [ebx]             ;A12 saturates to +2047 (5th+)
  pmaxsw mm2, mm3               ;|c|        ;B4 (2nd)
  pxor mm1, mm0                 ;A13 (6th+)
  pmullw mm4, [edi]             ;*= 2Q  ;C8 (3rd+)
  psraw mm5, 15                 ; sign(c)   ;C7 (2nd)
  movq mm7, [byte esi]          ;0          ;B5 mm7 in use
  pcmpeqw mm7, [ecx+%1*24 + 8]  ;B6 (c ==0) ? -1 : 0 (1st)
%if (%1 < 4)
  movq mm0, [byte esi]          ;A1 ;0
%endif
  pandn mm6, [byte eax]         ;C9 offset = isZero ? 0 : quant_add (2nd)
  psraw mm3, 15                 ;sign(c)    ;B7 (2nd)
  movq [byte edx+%1*24], mm1    ;A14 (7th)
  paddw mm6, mm5                ;C10  offset +negate back (3rd)
  pmullw mm2, [edi]             ;*= 2Q  ;B8 (3rd+)
  mov esp, esp
%endmacro


ALIGN 16
cglobal dequant_h263_intra_3dne
dequant_h263_intra_3dne:
  mov ecx, [esp+ 8]                 ; coeff
  mov eax, [esp+12]                 ; quant
  pxor mm0, mm0
  pxor mm2, mm2
  push edi
  push ebx
  lea edi, [mmx_mul + eax*8 - 8]    ; 2*quant
  push ebp
  mov ebx, mmx_2047
  movsx ebp, word [ecx]
  lea eax, [mmx_add + eax*8 - 8]    ; quant or quant-1
  push esi
  mov esi, mmzero
  pxor mm7, mm7
  movq mm3, [ecx+120]               ;B2 ; c  = coeff[i]
  pcmpeqw mm7, [ecx+120]            ;B6 (c ==0) ? -1 : 0 (1st)

  imul ebp, [esp+16+16]             ; dcscalar
  psubw mm2, mm3                    ;-c         ;B3 (1st dep)
  pmaxsw mm2, mm3                   ;|c|        ;B4 (2nd)
  pmullw mm2, [edi]                 ;*= 2Q  ;B8 (3rd+)
  psraw mm3, 15                     ; sign(c)   ;B7 (2nd)
  mov edx, [esp+ 4+16]              ; data

ALIGN 8
  dequant 0

  cmp ebp, -2048
  mov esp, esp

  dequant 1

  cmovl ebp, [int_2048]
  nop

  dequant 2

  cmp ebp, 2047
  mov esp, esp

  dequant 3

  cmovg ebp, [int2047]
  nop

  dequant 4

  paddw mm4, mm6            ;C11 mm6 free (4th+)
  pminsw mm4, [ebx]         ;C12 saturates to +2047 (5th+)
  pandn mm7, [eax]          ;B9 offset = isZero ? 0 : quant_add (2nd)
  mov eax, ebp
  mov esi, [esp]
  mov ebp, [esp+4]
  pxor mm5, mm4             ;C13 (6th+)
  paddw mm7, mm3            ;B10  offset +negate back (3rd)
  movq [edx+4*24+16], mm5   ;C14 (7th)
  paddw mm2, mm7            ;B11 mm7 free (4th+)
  pminsw mm2, [ebx]         ;B12 saturates to +2047 (5th+)
  mov ebx, [esp+8]
  mov edi, [esp+12]
  add esp, byte 16
  pxor mm3, mm2             ;B13 (6th+)
  movq [edx+4*24+8], mm3    ;B14 (7th)
  mov [edx], ax

  xor eax, eax
  ret

;-----------------------------------------------------------------------------
;
; uint32_t dequant_h263_inter_3dne(int16_t * data,
;                                  const int16_t * const coeff,
;                                  const uint32_t quant,
;                                  const uint16_t *mpeg_matrices);
;
;-----------------------------------------------------------------------------

; this is the same as dequant_inter_3dne,
; except that we're saturating using 'pminsw' (saves 2 cycles/loop)
; This is Athlon-optimized code (ca 100 clk per call)

ALIGN 16
cglobal dequant_h263_inter_3dne
dequant_h263_inter_3dne:
  mov ecx, [esp+ 8]         ; coeff
  mov eax, [esp+12]         ; quant
  pxor mm0, mm0
  pxor mm2, mm2
  push edi
  push ebx
  push esi
  lea edi, [mmx_mul + eax*8 - 8]    ; 2*quant
  mov ebx, mmx_2047
  pxor mm7, mm7
  movq mm3, [ecx+120]               ;B2 ; c  = coeff[i]
  pcmpeqw mm7, [ecx+120]            ;B6 (c ==0) ? -1 : 0 (1st)
  lea eax, [mmx_add + eax*8 - 8]    ; quant or quant-1
  psubw mm2, mm3                    ;-c ;B3 (1st dep)
  mov esi, mmzero
  pmaxsw mm2, mm3                   ;|c|        ;B4 (2nd)
  pmullw mm2, [edi]                 ;*= 2Q      ;B8 (3rd+)
  psraw mm3, 15                     ; sign(c)   ;B7 (2nd)
  mov edx, [dword esp+ 4+12]        ; data

ALIGN 8

  dequant 0
  dequant 1
  dequant 2
  dequant 3
  dequant 4

  paddw mm4, mm6            ;C11 mm6 free (4th+)
  pminsw mm4, [ebx]         ;C12 saturates to +2047 (5th+)
  pandn mm7, [eax]          ;B9 offset = isZero ? 0 : quant_add (2nd)
  mov esi, [esp]
  pxor mm5, mm4             ;C13 (6th+)
  paddw mm7, mm3            ;B10  offset +negate back (3rd)
  movq [edx+4*24+16], mm5   ;C14 (7th)
  paddw mm2, mm7            ;B11 mm7 free (4th+)
  pminsw mm2, [ebx]         ;B12 saturates to +2047 (5th+)
  mov ebx, [esp+4]
  mov edi, [esp+8]
  add esp, byte 12
  pxor mm3, mm2             ;B13 (6th+)
  movq [edx+4*24+8], mm3    ;B14 (7th)

  xor eax, eax
  ret
