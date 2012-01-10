;/*****************************************************************************
; *
; *  XVID MPEG-4 VIDEO CODEC
; *  - MPEG4 Quantization H263 implementation / MMX optimized -
; *
; *  Copyright(C) 2001-2003 Peter Ross <pross@xvid.org>
; *               2002-2003 Pascal Massimino <skal@planet-d.net>
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
; ****************************************************************************/

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
; Read only Local data
;=============================================================================

%ifdef FORMAT_COFF
SECTION .rodata
%else
SECTION .rodata align=16
%endif

ALIGN 16
plus_one:
	times 8 dw 1

;-----------------------------------------------------------------------------
;
; subtract by Q/2 table
;
;-----------------------------------------------------------------------------

ALIGN 16
mmx_sub:
%assign quant 1
%rep 31
	times 4 dw  quant / 2
	%assign quant quant+1
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
%assign quant 1
%rep 31
	times 4 dw  (1<<16) / (quant*2) + 1
	%assign quant quant+1
%endrep

;-----------------------------------------------------------------------------
;
; add by (odd(Q) ? Q : Q - 1) table
;
;-----------------------------------------------------------------------------

ALIGN 16
mmx_add:
%assign quant 1
%rep 31
	%if quant % 2 != 0
	times 4 dw  quant
	%else
	times 4 dw quant - 1
	%endif
	%assign quant quant+1
%endrep

;-----------------------------------------------------------------------------
;
; multiple by 2Q table
;
;-----------------------------------------------------------------------------

ALIGN 16
mmx_mul:
%assign quant 1
%rep 31
	times 4 dw  quant*2
	%assign quant quant+1
%endrep

;-----------------------------------------------------------------------------
;
; saturation limits
;
;-----------------------------------------------------------------------------

ALIGN 16
sse2_2047:
	times 8 dw 2047

ALIGN 16
mmx_2047:
	times 4 dw 2047

ALIGN 8
mmx_32768_minus_2048:
	times 4 dw (32768-2048)

mmx_32767_minus_2047:
	times 4 dw (32767-2047)


;=============================================================================
; Code
;=============================================================================

SECTION .text

cglobal quant_h263_intra_mmx
cglobal quant_h263_intra_sse2
cglobal quant_h263_inter_mmx
cglobal quant_h263_inter_sse2
cglobal dequant_h263_intra_mmx
cglobal dequant_h263_intra_xmm
cglobal dequant_h263_intra_sse2
cglobal dequant_h263_inter_mmx
cglobal dequant_h263_inter_xmm
cglobal dequant_h263_inter_sse2

;-----------------------------------------------------------------------------
;
; uint32_t quant_h263_intra_mmx(int16_t * coeff,
;                               const int16_t const * data,
;                               const uint32_t quant,
;                               const uint32_t dcscalar,
;                               const uint16_t *mpeg_matrices);
;
;-----------------------------------------------------------------------------

ALIGN 16
quant_h263_intra_mmx:

  push ecx
  push esi
  push edi

  mov edi, [esp + 12 + 4]     ; coeff
  mov esi, [esp + 12 + 8]     ; data
  mov eax, [esp + 12 + 12]    ; quant

  xor ecx, ecx
  cmp al, 1
  jz .q1loop

  movq mm7, [mmx_div + eax * 8 - 8]

ALIGN 16
.loop
  movq mm0, [esi + 8*ecx]           ; mm0 = [1st]
  movq mm3, [esi + 8*ecx + 8]
  pxor mm1, mm1                     ; mm1 = 0
  pxor mm4, mm4                     ;
  pcmpgtw mm1, mm0                  ; mm1 = (0 > mm0)
  pcmpgtw mm4, mm3                  ;
  pxor mm0, mm1                     ; mm0 = |mm0|
  pxor mm3, mm4                     ;
  psubw mm0, mm1                    ; displace
  psubw mm3, mm4                    ;
  pmulhw mm0, mm7                   ; mm0 = (mm0 / 2Q) >> 16
  pmulhw mm3, mm7                   ;
  pxor mm0, mm1                     ; mm0 *= sign(mm0)
  pxor mm3, mm4                     ;
  psubw mm0, mm1                    ; undisplace
  psubw mm3, mm4                    ;
  movq [edi + 8*ecx], mm0
  movq [edi + 8*ecx + 8], mm3

  add ecx, 2
  cmp ecx, 16
  jnz .loop

.done

    ; caclulate  data[0] // (int32_t)dcscalar)
  mov ecx, [esp + 12 + 16]      ; dcscalar
  mov edx, ecx
  movsx eax, word [esi]         ; data[0]
  shr edx, 1                    ; edx = dcscalar /2
  cmp eax, 0
  jg .gtzero

  sub eax, edx
  jmp short .mul

.gtzero
  add eax, edx
.mul
  cdq ; expand eax -> edx:eax
  idiv ecx          ; eax = edx:eax / dcscalar
  mov [edi], ax     ; coeff[0] = ax

  xor eax, eax      ; return(0);
  pop edi
  pop esi
  pop ecx

  ret

ALIGN 16
.q1loop
  movq mm0, [esi + 8*ecx]           ; mm0 = [1st]
  movq mm3, [esi + 8*ecx + 8]
  pxor mm1, mm1                     ; mm1 = 0
  pxor mm4, mm4                     ;
  pcmpgtw mm1, mm0                  ; mm1 = (0 > mm0)
  pcmpgtw mm4, mm3                  ;
  pxor mm0, mm1                     ; mm0 = |mm0|
  pxor mm3, mm4                     ;
  psubw mm0, mm1                    ; displace
  psubw mm3, mm4                    ;
  psrlw mm0, 1                      ; mm0 >>= 1   (/2)
  psrlw mm3, 1                      ;
  pxor mm0, mm1                     ; mm0 *= sign(mm0)
  pxor mm3, mm4
  psubw mm0, mm1                    ; undisplace
  psubw mm3, mm4                    ;
  movq [edi + 8*ecx], mm0
  movq [edi + 8*ecx + 8], mm3

  add ecx, 2
  cmp ecx, 16
  jnz .q1loop
  jmp short .done



;-----------------------------------------------------------------------------
;
; uint32_t quant_h263_intra_sse2(int16_t * coeff,
;                                const int16_t const * data,
;                                const uint32_t quant,
;                                const uint32_t dcscalar,
;                                const uint16_t *mpeg_matrices);
;
;-----------------------------------------------------------------------------

ALIGN 16
quant_h263_intra_sse2:

  push esi
  push edi

  mov edi, [esp + 8 + 4]                ; coeff
  mov esi, [esp + 8 + 8]                ; data
  mov eax, [esp + 8 + 12]               ; quant

  xor ecx, ecx
  cmp al, 1
  jz near .qas2_q1loop

.qas2_not1
  movq mm7, [mmx_div + eax*8 - 8]
  movq2dq xmm7, mm7
  movlhps xmm7, xmm7

ALIGN 16
.qas2_loop
  movdqa xmm0, [esi + ecx*8]                ; xmm0 = [1st]
  movdqa xmm3, [esi + ecx*8 + 16]           ; xmm3 = [2nd]
  pxor xmm1, xmm1
  pxor xmm4, xmm4
  pcmpgtw xmm1, xmm0
  pcmpgtw xmm4, xmm3
  pxor xmm0, xmm1
  pxor xmm3, xmm4
  psubw xmm0, xmm1
  psubw xmm3, xmm4
  pmulhw xmm0, xmm7
  pmulhw xmm3, xmm7
  pxor xmm0, xmm1
  pxor xmm3, xmm4
  psubw xmm0, xmm1
  psubw xmm3, xmm4
  movdqa [edi + ecx*8], xmm0
  movdqa [edi + ecx*8 + 16], xmm3

  add ecx, 4
  cmp ecx, 16
  jnz .qas2_loop

.qas2_done
  mov ecx, [esp + 8 + 16]   ; dcscalar
  mov edx, ecx
  movsx eax, word [esi]
  shr edx, 1
  cmp eax, 0
  jg .qas2_gtzero

  sub eax, edx
  jmp short .qas2_mul

.qas2_gtzero
  add eax, edx

.qas2_mul
  cdq
  idiv ecx

  mov [edi], ax

  xor eax, eax      ; return(0);
  pop edi
  pop esi

  ret

ALIGN 16
.qas2_q1loop
  movdqa xmm0, [esi + ecx*8]         ; xmm0 = [1st]
  movdqa xmm3, [esi + ecx*8 + 16]    ; xmm3 = [2nd]
  pxor xmm1, xmm1
  pxor xmm4, xmm4
  pcmpgtw xmm1, xmm0
  pcmpgtw xmm4, xmm3
  pxor xmm0, xmm1
  pxor xmm3, xmm4
  psubw xmm0, xmm1
  psubw xmm3, xmm4
  psrlw xmm0, 1
  psrlw xmm3, 1
  pxor xmm0, xmm1
  pxor xmm3, xmm4
  psubw xmm0, xmm1
  psubw xmm3, xmm4
  movdqa [edi + ecx*8], xmm0
  movdqa [edi + ecx*8 + 16], xmm3

  add ecx, 4
  cmp ecx, 16
  jnz .qas2_q1loop
  jmp near .qas2_done



;-----------------------------------------------------------------------------
;
; uint32_t quant_h263_inter_mmx(int16_t * coeff,
;                               const int16_t const * data,
;                               const uint32_t quant,
;                               const uint16_t *mpeg_matrices);
;
;-----------------------------------------------------------------------------

ALIGN 16
quant_h263_inter_mmx:

  push ecx
  push esi
  push edi

  mov edi, [esp + 12 + 4]           ; coeff
  mov esi, [esp + 12 + 8]           ; data
  mov eax, [esp + 12 + 12]          ; quant

  xor ecx, ecx

  pxor mm5, mm5                     ; sum
  movq mm6, [mmx_sub + eax * 8 - 8] ; sub

  cmp al, 1
  jz .q1loop

  movq mm7, [mmx_div + eax * 8 - 8] ; divider

ALIGN 8
.loop
  movq mm0, [esi + 8*ecx]           ; mm0 = [1st]
  movq mm3, [esi + 8*ecx + 8]
  pxor mm1, mm1                     ; mm1 = 0
  pxor mm4, mm4                     ;
  pcmpgtw mm1, mm0                  ; mm1 = (0 > mm0)
  pcmpgtw mm4, mm3                  ;
  pxor mm0, mm1                     ; mm0 = |mm0|
  pxor mm3, mm4                     ;
  psubw mm0, mm1                    ; displace
  psubw mm3, mm4                    ;
  psubusw mm0, mm6                  ; mm0 -= sub (unsigned, dont go < 0)
  psubusw mm3, mm6                  ;
  pmulhw mm0, mm7                   ; mm0 = (mm0 / 2Q) >> 16
  pmulhw mm3, mm7                   ;
  paddw mm5, mm0                    ; sum += mm0
  pxor mm0, mm1                     ; mm0 *= sign(mm0)
  paddw mm5, mm3                    ;
  pxor mm3, mm4                     ;
  psubw mm0, mm1                    ; undisplace
  psubw mm3, mm4
  movq [edi + 8*ecx], mm0
  movq [edi + 8*ecx + 8], mm3

  add ecx, 2
  cmp ecx, 16
  jnz .loop

.done
  pmaddwd mm5, [plus_one]
  movq mm0, mm5
  psrlq mm5, 32
  paddd mm0, mm5

  movd eax, mm0     ; return sum
  pop edi
  pop esi
  pop ecx

  ret

ALIGN 8
.q1loop
  movq mm0, [esi + 8*ecx]           ; mm0 = [1st]
  movq mm3, [esi + 8*ecx+ 8]        ;
  pxor mm1, mm1                     ; mm1 = 0
  pxor mm4, mm4                     ;
  pcmpgtw mm1, mm0                  ; mm1 = (0 > mm0)
  pcmpgtw mm4, mm3                  ;
  pxor mm0, mm1                     ; mm0 = |mm0|
  pxor mm3, mm4                     ;
  psubw mm0, mm1                    ; displace
  psubw mm3, mm4                    ;
  psubusw mm0, mm6                  ; mm0 -= sub (unsigned, dont go < 0)
  psubusw mm3, mm6                  ;
  psrlw mm0, 1                      ; mm0 >>= 1   (/2)
  psrlw mm3, 1                      ;
  paddw mm5, mm0                    ; sum += mm0
  pxor mm0, mm1                     ; mm0 *= sign(mm0)
  paddw mm5, mm3                    ;
  pxor mm3, mm4                     ;
  psubw mm0, mm1                    ; undisplace
  psubw mm3, mm4
  movq [edi + 8*ecx], mm0
  movq [edi + 8*ecx + 8], mm3

  add ecx, 2
  cmp ecx, 16
  jnz .q1loop

  jmp .done



;-----------------------------------------------------------------------------
;
; uint32_t quant_h263_inter_sse2(int16_t * coeff,
;                                const int16_t const * data,
;                                const uint32_t quant,
;                                const uint16_t *mpeg_matrices);
;
;-----------------------------------------------------------------------------

ALIGN 16
quant_h263_inter_sse2:

  push esi
  push edi

  mov edi, [esp + 8 + 4]      ; coeff
  mov esi, [esp + 8 + 8]      ; data
  mov eax, [esp + 8 + 12]     ; quant

  xor ecx, ecx

  pxor xmm5, xmm5                           ; sum

  movq mm0, [mmx_sub + eax*8 - 8]           ; sub
  movq2dq xmm6, mm0                         ; load into low 8 bytes
  movlhps xmm6, xmm6                        ; duplicate into high 8 bytes

  cmp al, 1
  jz near .qes2_q1loop

.qes2_not1
  movq mm0, [mmx_div + eax*8 - 8]           ; divider
  movq2dq xmm7, mm0
  movlhps xmm7, xmm7

ALIGN 16
.qes2_loop
  movdqa xmm0, [esi + ecx*8]                ; xmm0 = [1st]
  movdqa xmm3, [esi + ecx*8 + 16]           ; xmm3 = [2nd]
  pxor xmm1, xmm1
  pxor xmm4, xmm4
  pcmpgtw xmm1, xmm0
  pcmpgtw xmm4, xmm3
  pxor xmm0, xmm1
  pxor xmm3, xmm4
  psubw xmm0, xmm1
  psubw xmm3, xmm4
  psubusw xmm0, xmm6
  psubusw xmm3, xmm6
  pmulhw xmm0, xmm7
  pmulhw xmm3, xmm7
  paddw xmm5, xmm0
  pxor xmm0, xmm1
  paddw xmm5, xmm3
  pxor xmm3, xmm4
  psubw xmm0, xmm1
  psubw xmm3, xmm4
  movdqa [edi + ecx*8], xmm0
  movdqa [edi + ecx*8 + 16], xmm3

  add ecx, 4
  cmp ecx, 16
  jnz .qes2_loop

.qes2_done
  movdqu xmm6, [plus_one]
  pmaddwd xmm5, xmm6
  movhlps xmm6, xmm5
  paddd xmm5, xmm6
  movdq2q mm0, xmm5

  movq mm5, mm0
  psrlq mm5, 32
  paddd mm0, mm5

  movd eax, mm0         ; return sum

  pop edi
  pop esi

  ret

ALIGN 16
.qes2_q1loop
  movdqa xmm0, [esi + ecx*8]        ; xmm0 = [1st]
  movdqa xmm3, [esi + ecx*8 + 16]   ; xmm3 = [2nd]
  pxor xmm1, xmm1
  pxor xmm4, xmm4
  pcmpgtw xmm1, xmm0
  pcmpgtw xmm4, xmm3
  pxor xmm0, xmm1
  pxor xmm3, xmm4
  psubw xmm0, xmm1
  psubw xmm3, xmm4
  psubusw xmm0, xmm6
  psubusw xmm3, xmm6
  psrlw xmm0, 1
  psrlw xmm3, 1
  paddw xmm5, xmm0
  pxor xmm0, xmm1
  paddw xmm5, xmm3
  pxor xmm3, xmm4
  psubw xmm0, xmm1
  psubw xmm3, xmm4
  movdqa [edi + ecx*8], xmm0
  movdqa [edi + ecx*8 + 16], xmm3

  add ecx, 4
  cmp ecx, 16
  jnz .qes2_q1loop
  jmp .qes2_done


;-----------------------------------------------------------------------------
;
; uint32_t dequant_h263_intra_mmx(int16_t *data,
;                                 const int16_t const *coeff,
;                                 const uint32_t quant,
;                                 const uint32_t dcscalar,
;                                 const uint16_t *mpeg_matrices);
;
;-----------------------------------------------------------------------------

  ; note: we only saturate to +2047 *before* restoring the sign.
  ; Hence, final clamp really is [-2048,2047]

ALIGN 16
dequant_h263_intra_mmx:

  mov edx, [esp+ 4]                 ; data
  mov ecx, [esp+ 8]                 ; coeff
  mov eax, [esp+12]                 ; quant
  movq mm6, [mmx_add + eax*8 - 8]   ; quant or quant-1
  movq mm7, [mmx_mul + eax*8 - 8]   ; 2*quant
  mov eax, -16

ALIGN 16
.loop
  movq mm0, [ecx+8*eax+8*16]        ; c  = coeff[i]
  movq mm3, [ecx+8*eax+8*16 + 8]    ; c' = coeff[i+1]
  pxor mm1, mm1
  pxor mm4, mm4
  pcmpgtw mm1, mm0                  ; sign(c)
  pcmpgtw mm4, mm3                  ; sign(c')
  pxor mm2, mm2
  pxor mm5, mm5
  pcmpeqw mm2, mm0                  ; c is zero
  pcmpeqw mm5, mm3                  ; c' is zero
  pandn mm2, mm6                    ; offset = isZero ? 0 : quant_add
  pandn mm5, mm6
  pxor mm0, mm1                     ; negate if negative
  pxor mm3, mm4                     ; negate if negative
  psubw mm0, mm1
  psubw mm3, mm4
  pmullw mm0, mm7                   ; *= 2Q
  pmullw mm3, mm7                   ; *= 2Q
  paddw mm0, mm2                    ; + offset
  paddw mm3, mm5                    ; + offset
  paddw mm0, mm1                    ; negate back
  paddw mm3, mm4                    ; negate back

  ; saturates to +2047
  movq mm2, [mmx_32767_minus_2047]
  add eax, 2
  paddsw mm0, mm2
  paddsw mm3, mm2
  psubsw mm0, mm2
  psubsw mm3, mm2

  pxor mm0, mm1
  pxor mm3, mm4
  movq [edx + 8*eax + 8*16   - 2*8], mm0
  movq [edx + 8*eax + 8*16+8 - 2*8], mm3
  jnz near .loop

 ; deal with DC
  movd mm0, [ecx]
  pmullw mm0, [esp+16]          ; dcscalar
  movq mm2, [mmx_32767_minus_2047]
  paddsw mm0, mm2
  psubsw mm0, mm2
  movq mm3, [mmx_32768_minus_2048]
  psubsw mm0, mm3
  paddsw mm0, mm3
  movd eax, mm0
  mov [edx], ax

  xor eax, eax              ; return(0);
  ret

;-----------------------------------------------------------------------------
;
; uint32_t dequant_h263_intra_xmm(int16_t *data,
;                                 const int16_t const *coeff,
;                                 const uint32_t quant,
;                                 const uint32_t dcscalar,
;                                 const uint16_t *mpeg_matrices);
;
;-----------------------------------------------------------------------------

  ; this is the same as dequant_inter_mmx, except that we're
  ; saturating using 'pminsw' (saves 2 cycles/loop => ~5% faster)

ALIGN 16
dequant_h263_intra_xmm:

  mov edx, [esp+ 4]                 ; data
  mov ecx, [esp+ 8]                 ; coeff
  mov eax, [esp+12]                 ; quant
  movq mm6, [mmx_add + eax*8 - 8]   ; quant or quant-1
  movq mm7, [mmx_mul + eax*8 - 8]   ; 2*quant
  mov eax, -16

ALIGN 16
.loop
  movq mm0, [ecx+8*eax+8*16]        ; c  = coeff[i]
  movq mm3, [ecx+8*eax+8*16 + 8]    ; c' = coeff[i+1]
  pxor mm1, mm1
  pxor mm4, mm4
  pcmpgtw mm1, mm0                  ; sign(c)
  pcmpgtw mm4, mm3                  ; sign(c')
  pxor mm2, mm2
  pxor mm5, mm5
  pcmpeqw mm2, mm0                  ; c is zero
  pcmpeqw mm5, mm3                  ; c' is zero
  pandn mm2, mm6                    ; offset = isZero ? 0 : quant_add
  pandn mm5, mm6
  pxor mm0, mm1                     ; negate if negative
  pxor mm3, mm4                     ; negate if negative
  psubw mm0, mm1
  psubw mm3, mm4
  pmullw mm0, mm7                   ; *= 2Q
  pmullw mm3, mm7                   ; *= 2Q
  paddw mm0, mm2                    ; + offset
  paddw mm3, mm5                    ; + offset
  paddw mm0, mm1                    ; negate back
  paddw mm3, mm4                    ; negate back

   ; saturates to +2047
  movq mm2, [mmx_2047]
  pminsw mm0, mm2
  add eax, 2
  pminsw mm3, mm2

  pxor mm0, mm1
  pxor mm3, mm4
  movq [edx + 8*eax + 8*16   - 2*8], mm0
  movq [edx + 8*eax + 8*16+8 - 2*8], mm3
  jnz near .loop

    ; deal with DC
  movd mm0, [ecx]
  pmullw mm0, [esp+16]    ; dcscalar
  movq mm2, [mmx_32767_minus_2047]
  paddsw mm0, mm2
  psubsw mm0, mm2
  movq mm2, [mmx_32768_minus_2048]
  psubsw mm0, mm2
  paddsw mm0, mm2
  movd eax, mm0
  mov [edx], ax

  xor eax, eax
  ret


;-----------------------------------------------------------------------------
;
; uint32_t dequant_h263_intra_sse2(int16_t *data,
;                                  const int16_t const *coeff,
;                                  const uint32_t quant,
;                                  const uint32_t dcscalar,
;                                  const uint16_t *mpeg_matrices);
;
;-----------------------------------------------------------------------------

ALIGN 16
dequant_h263_intra_sse2:
  mov edx, [esp+ 4]        ; data
  mov ecx, [esp+ 8]        ; coeff
  mov eax, [esp+12]        ; quant
  movq mm6, [mmx_add + eax * 8 - 8]
  movq mm7, [mmx_mul + eax * 8 - 8]
  movq2dq xmm6, mm6
  movq2dq xmm7, mm7
  movlhps xmm6, xmm6
  movlhps xmm7, xmm7
  mov eax, -16

ALIGN 16
.loop
  movdqa xmm0, [ecx + 8*16 + 8*eax]      ; c  = coeff[i]
  movdqa xmm3, [ecx + 8*16 + 8*eax+ 16]
  pxor xmm1, xmm1
  pxor xmm4, xmm4
  pcmpgtw xmm1, xmm0    ; sign(c)
  pcmpgtw xmm4, xmm3
  pxor xmm2, xmm2
  pxor xmm5, xmm5
  pcmpeqw xmm2, xmm0    ; c is zero
  pcmpeqw xmm5, xmm3
  pandn xmm2, xmm6      ; offset = isZero ? 0 : quant_add
  pandn xmm5, xmm6
  pxor xmm0, xmm1       ; negate if negative
  pxor xmm3, xmm4
  psubw xmm0, xmm1
  psubw xmm3, xmm4
  pmullw xmm0, xmm7     ; *= 2Q
  pmullw xmm3, xmm7
  paddw xmm0, xmm2      ; + offset
  paddw xmm3, xmm5
  paddw xmm0, xmm1      ; negate back
  paddw xmm3, xmm4

    ; saturates to +2047
  movdqa xmm2, [sse2_2047]
  pminsw xmm0, xmm2
  add eax, 4
  pminsw xmm3, xmm2

  pxor xmm0, xmm1
  pxor xmm3, xmm4
  movdqa [edx + 8*16 - 8*4 + 8*eax], xmm0
  movdqa [edx + 8*16 - 8*4 + 8*eax + 16], xmm3
  jnz near .loop

 ; deal with DC
  movd mm0, [ecx]
  pmullw mm0, [esp+16]    ; dcscalar
  movq mm2, [mmx_32767_minus_2047]
  paddsw mm0, mm2
  psubsw mm0, mm2
  movq mm2, [mmx_32768_minus_2048]
  psubsw mm0, mm2
  paddsw mm0, mm2
  movd eax, mm0
  mov [edx], ax

  xor eax, eax
  ret

;-----------------------------------------------------------------------------
;
; uint32t dequant_h263_inter_mmx(int16_t * data,
;                                const int16_t * const coeff,
;                                const uint32_t quant,
;                                const uint16_t *mpeg_matrices);
;
;-----------------------------------------------------------------------------

ALIGN 16
dequant_h263_inter_mmx:

  mov edx, [esp+ 4]        ; data
  mov ecx, [esp+ 8]        ; coeff
  mov eax, [esp+12]        ; quant
  movq mm6, [mmx_add + eax*8 - 8]  ; quant or quant-1
  movq mm7, [mmx_mul + eax*8 - 8]  ; 2*quant
  mov eax, -16

ALIGN 16
.loop
  movq mm0, [ecx+8*eax+8*16]      ; c  = coeff[i]
  movq mm3, [ecx+8*eax+8*16 + 8]  ; c' = coeff[i+1]
  pxor mm1, mm1
  pxor mm4, mm4
  pcmpgtw mm1, mm0  ; sign(c)
  pcmpgtw mm4, mm3  ; sign(c')
  pxor mm2, mm2
  pxor mm5, mm5
  pcmpeqw mm2, mm0  ; c is zero
  pcmpeqw mm5, mm3  ; c' is zero
  pandn mm2, mm6    ; offset = isZero ? 0 : quant_add
  pandn mm5, mm6
  pxor mm0, mm1     ; negate if negative
  pxor mm3, mm4     ; negate if negative
  psubw mm0, mm1
  psubw mm3, mm4
  pmullw mm0, mm7   ; *= 2Q
  pmullw mm3, mm7   ; *= 2Q
  paddw mm0, mm2    ; + offset
  paddw mm3, mm5    ; + offset
  paddw mm0, mm1    ; negate back
  paddw mm3, mm4    ; negate back
  ; saturates to +2047
  movq mm2, [mmx_32767_minus_2047]
  add eax, 2
  paddsw mm0, mm2
  paddsw mm3, mm2
  psubsw mm0, mm2
  psubsw mm3, mm2

  pxor mm0, mm1
  pxor mm3, mm4
  movq [edx + 8*eax + 8*16   - 2*8], mm0
  movq [edx + 8*eax + 8*16+8 - 2*8], mm3
  jnz near .loop

  xor eax, eax
  ret

;-----------------------------------------------------------------------------
;
; uint32_t dequant_h263_inter_xmm(int16_t * data,
;                                 const int16_t * const coeff,
;                                 const uint32_t quant,
;                                 const uint16_t *mpeg_matrices);
;
;-----------------------------------------------------------------------------

  ; this is the same as dequant_inter_mmx,
  ; except that we're saturating using 'pminsw' (saves 2 cycles/loop)

ALIGN 16
dequant_h263_inter_xmm:

  mov edx, [esp+ 4]        ; data
  mov ecx, [esp+ 8]        ; coeff
  mov eax, [esp+12]        ; quant
  movq mm6, [mmx_add + eax*8 - 8]  ; quant or quant-1
  movq mm7, [mmx_mul + eax*8 - 8]  ; 2*quant
  mov eax, -16

ALIGN 16
.loop
  movq mm0, [ecx+8*eax+8*16]      ; c  = coeff[i]
  movq mm3, [ecx+8*eax+8*16 + 8]  ; c' = coeff[i+1]
  pxor mm1, mm1
  pxor mm4, mm4
  pcmpgtw mm1, mm0  ; sign(c)
  pcmpgtw mm4, mm3  ; sign(c')
  pxor mm2, mm2
  pxor mm5, mm5
  pcmpeqw mm2, mm0  ; c is zero
  pcmpeqw mm5, mm3  ; c' is zero
  pandn mm2, mm6    ; offset = isZero ? 0 : quant_add
  pandn mm5, mm6
  pxor mm0, mm1     ; negate if negative
  pxor mm3, mm4     ; negate if negative
  psubw mm0, mm1
  psubw mm3, mm4
  pmullw mm0, mm7   ; *= 2Q
  pmullw mm3, mm7   ; *= 2Q
  paddw mm0, mm2    ; + offset
  paddw mm3, mm5    ; + offset
  paddw mm0, mm1    ; start restoring sign
  paddw mm3, mm4    ; start restoring sign
                            ; saturates to +2047
  movq mm2, [mmx_2047]
  pminsw mm0, mm2
  add eax, 2
  pminsw mm3, mm2

  pxor mm0, mm1 ; finish restoring sign
  pxor mm3, mm4 ; finish restoring sign
  movq [edx + 8*eax + 8*16   - 2*8], mm0
  movq [edx + 8*eax + 8*16+8 - 2*8], mm3
  jnz near .loop

  xor eax, eax
  ret

;-----------------------------------------------------------------------------
;
; uint32_t dequant_h263_inter_sse2(int16_t * data,
;                                  const int16_t * const coeff,
;                                  const uint32_t quant,
;                                  const uint16_t *mpeg_matrices);
;
;-----------------------------------------------------------------------------

ALIGN 16
dequant_h263_inter_sse2:
  mov edx, [esp + 4]    ; data
  mov ecx, [esp + 8]    ; coeff
  mov eax, [esp + 12]   ; quant
  movq mm6, [mmx_add + eax * 8 - 8]
  movq mm7, [mmx_mul + eax * 8 - 8]
  movq2dq xmm6, mm6
  movq2dq xmm7, mm7
  movlhps xmm6, xmm6
  movlhps xmm7, xmm7
  mov eax, -16

ALIGN 16
.loop
  movdqa xmm0, [ecx + 8*16 + 8*eax]  ; c  = coeff[i]
  movdqa xmm3, [ecx + 8*16 + 8*eax + 16]

  pxor xmm1, xmm1
  pxor xmm4, xmm4
  pcmpgtw xmm1, xmm0    ; sign(c)
  pcmpgtw xmm4, xmm3
  pxor xmm2, xmm2
  pxor xmm5, xmm5
  pcmpeqw xmm2, xmm0    ; c is zero
  pcmpeqw xmm5, xmm3
  pandn xmm2, xmm6
  pandn xmm5, xmm6
  pxor xmm0, xmm1       ; negate if negative
  pxor xmm3, xmm4
  psubw xmm0, xmm1
  psubw xmm3, xmm4
  pmullw xmm0, xmm7     ; *= 2Q
  pmullw xmm3, xmm7
  paddw xmm0, xmm2      ; + offset
  paddw xmm3, xmm5

  paddw xmm0, xmm1      ; start restoring sign
  paddw xmm3, xmm4

 ; saturates to +2047
  movdqa xmm2, [sse2_2047]
  pminsw xmm0, xmm2
  add eax, 4
  pminsw xmm3, xmm2

  pxor xmm0, xmm1 ; finish restoring sign
  pxor xmm3, xmm4
  movdqa [edx + 8*16 - 8*4 + 8*eax], xmm0
  movdqa [edx + 8*16 - 8*4 + 8*eax + 16], xmm3
  jnz near .loop

  xor eax, eax
  ret