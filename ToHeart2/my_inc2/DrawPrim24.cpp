
 
  
   
    
     
      

#include "mm_std.h"


#include "bmp.h"
#include "draw.h"
#include "drawprim.h"



extern RECT	ClipRectDef;	
extern int	DispWidth;
extern int	DispHeight;











void FillMemory3B( void *dest, SIZE_T size, BYTE f1, BYTE f2, BYTE f3 )
{
	char	B3_tbl[24] = {	f1, f2, f3, f1, f2, f3, f1, f2,
							f3, f1, f2, f3, f1, f2, f3, f1,
							f2, f3, f1, f2, f3, f1, f2, f3 };
	char	*src = B3_tbl;
	char	*dst = (char*)dest;
	int		i;
	int		size2 = size/8;

	if( (SSE_FP_Flag || _3DNowExt_Flag) ){
		if(size2){
			_asm {
				mov		esi, src;
				mov		edi, dst;
				mov		ecx, size2;

				movq	mm0, [esi]
				movq	mm1, [esi+8]
				movq	mm2, [esi+16]

				lx_sse2:
					movntq	[edi],   mm0;
					movntq	[edi+8], mm1;
					movntq	[edi+16],mm2;

					lea		edi,[edi+24];
					dec		ecx;
				jnz		lx_sse2;

				mov		src, esi;
				mov		dst, edi;
				emms
			}
		}
	}else{
		for( i=0; i < size2 ; i++, dst+=24 ){
			*(((double*)dst)  ) = *(double*)&B3_tbl[0];
			*(((double*)dst)+1) = *(double*)&B3_tbl[8];
			*(((double*)dst)+2) = *(double*)&B3_tbl[16];
		}
	}

	for( i=0; i < (int)size%8 ; i++, dst+=3 ){
		*(dst+0) = f1;
		*(dst+1) = f2;
		*(dst+2) = f3;
	}
}



BOOL PRM_DrawRect_F_Std( BMP_F *dest, int dx, int dy, int w, int h,
						int r, int g, int b )
{
	int		y;
	RGB24	*dest_p = dest->buf + dy * dest->sx + dx;
	int		drev = dest->sx-w;

	if( w==dest->sx ){
		FillMemory3B( dest_p, w*h, b, g, r );
	}else{
		for( y=0; y < h ; y++, dest_p+=dest->sx ){
			FillMemory3B( dest_p, w, b, g, r );
		}
	}

	return TRUE;
}



BOOL PRM_DrawRect_F_Bld( BMP_F *dest, int dx, int dy, int w, int h,
						int r, int g, int b,
						int blnd )
{
	int		x,y;
	RGB24	*dest_p = dest->buf + dy * dest->sx + dx;
	int		drev = dest->sx-w;
	int		brev = 256-blnd;
	char	*blnd_tbl = BlendTable[blnd];
	char	*brev_tbl = BlendTable[brev];
	char	bb = blnd_tbl[b];
	char	gg = blnd_tbl[g];
	char	rr = blnd_tbl[r];
	char	rgb_tbl[24] = {	bb, gg, rr, bb, gg, rr, bb, gg,
							rr, bb, gg, rr, bb, gg, rr, bb,
							gg, rr, bb, gg, rr, bb, gg, rr };

	if(blnd==BLD_0P )	return TRUE;											
	if(blnd==BLD_100P)	return PRM_DrawRect_F_Std( dest, dx, dy, w, h, r,g,b );	

	if( MMX_Flag ){
		int		w2 = w*3;
		int		w3 = w2/24;
		int		wa = w2%24;
		char	*temp = (char*)rgb_tbl;
		DWORD	alpha2 = brev*0x01010101;
		DWORD	alpha1 = blnd*0x01010101;
		char	*dest_p2;

		for( y=0; y < h ; y++, dest_p+=drev ){
			if(w3){
				_asm {
					mov			ecx, w3
					mov			eax, temp
					mov			ebx, dest_p
					mov			edx,	0

					pxor		mm3, mm3	
					movd		mm5, alpha2	
					punpcklbw	mm5, mm3	

					movq		mm0, [eax]
					movq		mm2, [eax+8]
					movq		mm4, [eax+16]

					mmloopx1:
					
						movq		mm1, [ebx]	
						movq		mm7, mm1	
						punpcklbw	mm1, mm3	
						punpckhbw	mm7, mm3	
						
						pmullw		mm1, mm5	
						pmullw		mm7, mm5	
						psrlw		mm1, 8		
						psrlw		mm7, 8		
						packuswb	mm1, mm7	
						
						paddusb		mm1, mm0	
						movq		[ebx], mm1	

						add			ebx, 8			

						movq		mm1, [ebx]	
						movq		mm7, mm1	
						punpcklbw	mm1, mm3	
						punpckhbw	mm7, mm3	
						
						pmullw		mm1, mm5	
						pmullw		mm7, mm5	
						psrlw		mm1, 8		
						psrlw		mm7, 8		
						packuswb	mm1, mm7	
						
						paddusb		mm1, mm2	
						movq		[ebx], mm1	

						add			ebx, 8			

						movq		mm1, [ebx]	
						movq		mm7, mm1	
						punpcklbw	mm1, mm3	
						punpckhbw	mm7, mm3	
						
						pmullw		mm1, mm5	
						pmullw		mm7, mm5	
						psrlw		mm1, 8		
						psrlw		mm7, 8		
						packuswb	mm1, mm7	
						
						paddusb		mm1, mm4	
						movq		[ebx], mm1	

						add			ebx, 8			

					loop		mmloopx1
					mov			dest_p, ebx
				}
				_asm	emms
			}

			dest_p2 = (char*)dest_p;
			for( x=0; x < wa ; x++, dest_p2++ ){
				*dest_p2 = brev_tbl[ *dest_p2 ] + rgb_tbl[ x ];
			}
			dest_p = (RGB24*)dest_p2;
		}
	}else{
		for( y=0; y < h ; y++, dest_p+=drev ){
			for( x=0 ; x < w ; x++, dest_p++ ){
				dest_p->b = brev_tbl[dest_p->b] + bb;
				dest_p->g = brev_tbl[dest_p->g] + gg;
				dest_p->r = brev_tbl[dest_p->r] + rr;
			}
		}
	}

	return TRUE;
}



BOOL PRM_DrawRect_F_Add( BMP_F *dest, int dx, int dy, int w, int h,
						int r, int g, int b )
{
	int		x,y;
	RGB24	*dest_p = dest->buf + dy * dest->sx + dx;
	int		drev = dest->sx-w;
	char	rgb_tbl[24] = {	b, g, r, b, g, r, b, g,
							r, b, g, r, b, g, r, b,
							g, r, b, g, r, b, g, r };
	char	*temp = (char*)rgb_tbl;
	char	*add_tbl = AddTable;

	if(r==0 && g==0 && b==0)	return TRUE;

	if( MMX_Flag ){
		int	w2 = w*3;
		int	w3 = w2/24;
		int	wa = w2%24;
		char	*dest_p2;

		for( y=0; y < h ; y++, dest_p+=drev ){
			if(w3){
				_asm {
					mov			ecx, w3
					mov			eax, temp
					mov			ebx, dest_p
					mov			edx, 0

					movq		mm1, [eax]
					movq		mm2, [eax+8]
					movq		mm3, [eax+16]

					loopx:
						movq		mm0, [ebx]
						paddusb		mm0, mm1
						movq		[ebx], mm0
						add			ebx, 8

						movq		mm0, [ebx]
						paddusb		mm0, mm2
						movq		[ebx], mm0
						add			ebx, 8

						movq		mm0, [ebx]
						paddusb		mm0, mm3
						movq		[ebx], mm0
						add			ebx, 8
					loop	loopx

					mov			dest_p, ebx
				}
				_asm	emms
			}

			dest_p2 = (char*)dest_p;
			for( x=0; x < wa ; x++, dest_p2++ ){
				*dest_p2 = add_tbl[ *dest_p2 + rgb_tbl[x] ];
			}
			dest_p = (RGB24*)dest_p2;
		}
	}else{
		for( y=0; y < h ; y++, dest_p+=drev ){
			for( x=0 ; x < w ; x++, dest_p++ ){
				dest_p->b = add_tbl[ dest_p->b+b ];
				dest_p->g = add_tbl[ dest_p->g+g ];
				dest_p->r = add_tbl[ dest_p->r+r ];
			}
		}
	}

	return TRUE;
}



BOOL PRM_DrawRect_F_Sub( BMP_F *dest, int dx, int dy, int w, int h,
						int r, int g, int b )
{
	int		x,y;
	RGB24	*dest_p = dest->buf + dy * dest->sx + dx;
	int		drev = dest->sx-w;
	char	rgb_tbl[24] = {	b, g, r, b, g, r, b, g,
							r, b, g, r, b, g, r, b,
							g, r, b, g, r, b, g, r };
	char	*temp = (char*)rgb_tbl;
	char	*sub_tbl = SubTable+255;

	if(r==0 && g==0 && b==0)	return TRUE;

	if( MMX_Flag ){
		int	w2 = w*3;
		int	w3 = w2/24;
		int	wa = w2%24;
		char	*dest_p2;

		for( y=0; y < h ; y++, dest_p+=drev ){
			if(w3){
				_asm {
					mov			ecx, w3
					mov			eax, temp
					mov			ebx, dest_p
					mov			edx, 0

					movq		mm1, [eax]
					movq		mm2, [eax+8]
					movq		mm3, [eax+16]

					loopx:
						movq		mm0, [ebx]
						psubusb		mm0, mm1
						movq		[ebx], mm0
						add			ebx, 8

						movq		mm0, [ebx]
						psubusb		mm0, mm2
						movq		[ebx], mm0
						add			ebx, 8

						movq		mm0, [ebx]
						psubusb		mm0, mm3
						movq		[ebx], mm0
						add			ebx, 8
					loop	loopx

					mov			dest_p, ebx
				}
				_asm	emms
			}

			dest_p2 = (char*)dest_p;
			for( x=0; x < wa ; x++, dest_p2++ ){
				*dest_p2 = sub_tbl[ *dest_p2 - rgb_tbl[x] ];
			}
			dest_p = (RGB24*)dest_p2;
		}
	}else{
		for( y=0; y < h ; y++, dest_p+=drev ){
			for( x=0 ; x < w ; x++, dest_p++ ){
				dest_p->b = sub_tbl[ dest_p->b-b ];
				dest_p->g = sub_tbl[ dest_p->g-g ];
				dest_p->r = sub_tbl[ dest_p->r-r ];
			}
		}
	}

	return TRUE;
}



BOOL PRM_DrawRect_F_Mul( BMP_F *dest, int dx, int dy, int w, int h,
						int r, int g, int b )
{
	int		x,y;
	RGB24	*dest_p = dest->buf + dy * dest->sx + dx;
	int		drev = dest->sx-w;
	char	*blend_tblr = BrightTable_TB[r];
	char	*blend_tblg = BrightTable_TB[g];
	char	*blend_tblb = BrightTable_TB[b];

	for( y=0; y < h ; y++, dest_p+=drev ){
		for( x=0 ; x < w ; x++, dest_p++ ){
			dest_p->b = blend_tblb[ dest_p->b ];
			dest_p->g = blend_tblg[ dest_p->g ];
			dest_p->r = blend_tblr[ dest_p->r ];
		}
	}
	return TRUE;
}



BOOL PRM_DrawRect_F_Ami( BMP_F *dest, int dx, int dy, int w, int h,
						int r, int g, int b, int mesh )
{
	int		x,y;
	RGB24	*dest_p = dest->buf + dy * dest->sx + dx;
	int		drev = dest->sx-w;
	char	*linemask[4], *linemask2;
	WORD	mesh_pt = MeshFadePatern[mesh/16];
	RGB24	rgb = { b, g, r };

	
	linemask[0] = (char*)MeshTable[((mesh_pt >> 12) & 0x0f)];
	linemask[1] = (char*)MeshTable[((mesh_pt >>  8) & 0x0f)];
	linemask[2] = (char*)MeshTable[((mesh_pt >>  4) & 0x0f)];
	linemask[3] = (char*)MeshTable[((mesh_pt      ) & 0x0f)];

	for( y=0; y < h ; y++, dest_p+=drev ){
		linemask2 = linemask[y & 3];
		for( x=0 ; x < w ; x++, dest_p++ ){
			if( linemask2[x & 3] ){
				*dest_p = rgb;
			}
		}
	}
	return TRUE;
}



BOOL PRM_DrawRect_F_Nis( BMP_F *dest, int dx, int dy, int w, int h,
						int r, int g, int b, int noise )
{
	int		x,y;
	RGB24	*dest_p = dest->buf + dy * dest->sx + dx;
	int		drev = dest->sx-w;
	RGB24	rgb = { b, g, r };
	RGB24	rgb2 = { 0, 0, 0 };
	int		rate;

	for( y=0; y < h ; y++, dest_p+=drev ){
		for( x=0 ; x < w ; x++, dest_p++ ){
			rate = rand()&0xff;
			if( rate<=noise ){
				*dest_p = rgb2;
			}
		}
	}
	return TRUE;
}

BOOL PRM_DrawRipple_F( BMP_F *dest, int dx, int dy,
					RECT *clip, int r, int g, int b,
					DWORD wparam )
{
	BOOL	ret=TRUE;
	int		blnd  = HIWORD(wparam);
	int		param = LOWORD(wparam);

	
	if( dest->buf==NULL ) { DebugPrintf("•`‰æ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ[PRM_DrawRect_F]\n"); return FALSE; }



	return ret;
}







BOOL PRM_DrawRect_F( BMP_F *dest, int dx, int dy, int w, int h,
					RECT *clip, int r, int g, int b,
					DWORD wparam )
{
	BOOL	ret=TRUE;
	int		blnd  = HIWORD(wparam);
	int		param = LOWORD(wparam);

	
	if( dest->buf==NULL ) { DebugPrintf("•`‰æ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ[PRM_DrawRect_F]\n"); return FALSE; }



	if( !ClipRect2( &dx, &dy, &w, &h, dest->sx, dest->sy, clip ) ) return ret;

	
	if( param==DRW_AMI2 || param== DRW_BLD2 ){
		if( blnd==BLD_0P   )return 1;			
		if( blnd==BLD_100P )param = DRW_NML;	
	}

	switch(param){
		default:
		case DRW_NML:	PRM_DrawRect_F_Std( dest, dx, dy, w, h, r, g, b );			break;	
		case DRW_BLD2:	PRM_DrawRect_F_Bld( dest, dx, dy, w, h, r, g, b, blnd );	break;	
		case DRW_ADD:	PRM_DrawRect_F_Add( dest, dx, dy, w, h, r, g, b );			break;	
		case DRW_SUB:	PRM_DrawRect_F_Sub( dest, dx, dy, w, h, r, g, b );			break;	
		case DRW_MUL:	PRM_DrawRect_F_Mul( dest, dx, dy, w, h, r, g, b );			break;	
		case DRW_AMI2:	PRM_DrawRect_F_Ami( dest, dx, dy, w, h, r, g, b, blnd );	break;	
		case DRW_NIS2:	PRM_DrawRect_F_Nis( dest, dx, dy, w, h, r, g, b, blnd );	break;	
		case DRW_NEG:			break;	
		case DRW_VIV2:			break;	
	}
	return ret;
}







BOOL PRM_DrawFxLine_F_Std( char *dest_p, MIN_MAX *mi, MIN_MAX *ma, RECT *clip, char *rgb_tbl )
{
	int		x,w;
	int		w2,w3,wa;
	int		dx1,dx2;
	
	if( mi->x == DispWidth ) return TRUE;

	dx1	= max( clip->left,  mi->x );
	dx2 = min( clip->right, ma->x );
	w = dx2-dx1;
	dest_p += dx1*3;

	w2 = w*3;

	if( MMX_Flag ){
		w3 = w2/24;
		wa = w2%24;
		if(w3){
			_asm {
				mov		ecx, w3
				mov		esi, rgb_tbl
				mov		edi, dest_p

				movq	mm0,  [esi]
				movq	mm1,  [esi+8]
				movq	mm2,  [esi+16]

				mmloopx:
					movq	[edi],    mm0
					movq	[edi+8],  mm1
					movq	[edi+16], mm2
					add		edi, 24
				loop	mmloopx
			}
			_asm	emms
		}
		dest_p += w2;

		if( wa ){	
			for( x=0; x < wa ; x++ )
				*(dest_p-wa+x) = *(rgb_tbl-wa+x+24);
		}
	}else{
		w3 = w2/12;
		wa = w2%12;
		if(w3){
			_asm {
				mov		ecx, w3
				mov		esi, rgb_tbl
				mov		edi, dest_p

				mov		eax,  [esi]
				mov		ebx,  [esi+4]
				mov		edx,  [esi+8]

				loopx:
					mov		[edi],   eax
					mov		[edi+4], ebx
					mov		[edi+8], edx
					add		edi,12
				loop	loopx
			}
		}
		dest_p += w2;

		if( wa ){	
			for( x=0; x < wa ; x++ )
				*(dest_p-wa+x) = *(rgb_tbl-wa+x+12);
		}
	}
	
	return TRUE;
}



BOOL PRM_DrawFxLine_F_Bld( char *dest_p, MIN_MAX *mi, MIN_MAX *ma, RECT *clip, char *rgb_tbl, int blnd )
{
	int		x,w;
	int		dx1,dx2;
	int		w2, w3, wa;
	DWORD	temp2 = 0;
	int		blnd_rev = 256-blnd;
	DWORD	alpha2 = blnd_rev*0x01010101;
	DWORD	alpha1 = blnd    *0x01010101;
	char	*blnd_tbl = BlendTable[blnd];
	char	*brev_tbl = BlendTable[blnd_rev];

	if(blnd>=256) return PRM_DrawFxLine_F_Std( dest_p, mi, ma, clip, rgb_tbl );
	if(blnd<=0) return TRUE;

	if( mi->x == DispWidth )	return TRUE;	

	dx1	= max( clip->left,  mi->x );
	dx2 = min( clip->right, ma->x );
	w = dx2-dx1;
	dest_p += dx1*3;

	if( MMX_Flag ){
		w2 = w*3;
		w3 = w2/8;
		wa = w2%8;
		if(w3){
			_asm {
				mov			ecx, w3
				mov			eax, rgb_tbl
				mov			ebx, dest_p
				mov			edx,	0

				movd		mm3, temp2	
				movd		mm4, alpha1	
				movd		mm5, alpha2	
				punpcklbw	mm4, mm3	
				punpcklbw	mm5, mm3	
			mmloopx1:
			
				movq		mm0, [eax]	
				movq		mm1, [ebx]	
				
				movq		mm6, mm0	
				movq		mm7, mm1	
				
				punpcklbw	mm0, mm3	
				punpcklbw	mm1, mm3	
				
				punpckhbw	mm6, mm3	
				punpckhbw	mm7, mm3	
				
				pmullw		mm0, mm4	
				pmullw		mm1, mm5	
				paddusw		mm0, mm1	
				pmullw		mm6, mm4	
				pmullw		mm7, mm5	
				psrlw		mm0, 8		
				paddusw		mm6, mm7	
				psrlw		mm6, 8		
				
				packuswb	mm0, mm6	
				movq		[ebx], mm0	

				add			eax,8			
				add			ebx,8			

				inc			edx
				cmp			edx, 3
				jne			next1
				mov			edx, 0
				mov			eax, rgb_tbl
			next1:

				loop		mmloopx1
			}
			_asm	emms
		}
		dest_p += w2;

		if( wa ){	
			for( x=0; x < wa ; x++ )
				*(dest_p-wa+x) = brev_tbl[  *(dest_p-wa+x) ] + blnd_tbl[ *(rgb_tbl-wa+x+24) ];
		}
	}else{
		int		rb = (rgb_tbl[2]*blnd)>>8;
		int		gb = (rgb_tbl[1]*blnd)>>8;
		int		bb = (rgb_tbl[0]*blnd)>>8;

		for( x=0 ; x<w ; x++, dest_p+=3 ){
			*(dest_p  ) = brev_tbl[ *(dest_p  ) ] + bb;
			*(dest_p+1) = brev_tbl[ *(dest_p+1) ] + gb;
			*(dest_p+2) = brev_tbl[ *(dest_p+2) ] + rb;
		}
	}
	return TRUE;
}



BOOL PRM_DrawFxLine_F_Add( char *dest_p, MIN_MAX *mi, MIN_MAX *ma, RECT *clip, char *rgb_tbl, int param )
{
	int		x,w;
	int		dx1,dx2;
	int		w2, w3, wa;
	char	*bld_tbl=AddTable;

	if( mi->x == DispWidth )	return TRUE;	

	dx1	= max( clip->left,  mi->x );
	dx2 = min( clip->right, ma->x );
	w = dx2-dx1;
	dest_p += dx1*3;

	if( MMX_Flag ){
		w2 = w*3;
		w3 = w2/8;
		wa = w2%8;
		if(w3){
			_asm {
				mov			ecx, w3
				mov			eax, rgb_tbl
				mov			ebx, dest_p
				mov			edx, 0
			loopx:
				movq		mm0, [ebx]
				movq		mm1, [eax]
				paddusb		mm0, mm1
				movq		[ebx], mm0
				add			eax, 8
				add			ebx, 8

				inc			edx
				cmp			edx, 3
				jne			next1
				mov			edx, 0
				mov			eax, rgb_tbl
			next1:

				loop	loopx
			}
			_asm	emms

		}
		dest_p += w2;

		if( wa ){	
			for( x=0; x < wa ; x++ ){
				*(dest_p-wa+x) = bld_tbl[ *(dest_p-wa+x) + *(rgb_tbl-wa+x+24) ];
			}
		}
	}else{
		if( param == DRW_ADD ){
			for( x=0 ; x<w ; x++, dest_p+=3 ){
				*(dest_p  ) = bld_tbl[ *(dest_p  ) + rgb_tbl[0] ];
				*(dest_p+1) = bld_tbl[ *(dest_p+1) + rgb_tbl[1] ];
				*(dest_p+2) = bld_tbl[ *(dest_p+2) + rgb_tbl[2] ];
			}
		}else{
			for( x=0 ; x<w ; x++, dest_p+=3 ){
				*(dest_p  ) = *(dest_p  ) + rgb_tbl[0];
				*(dest_p+1) = *(dest_p+1) + rgb_tbl[1];
				*(dest_p+2) = *(dest_p+2) + rgb_tbl[2];
			}
		}
	}
	return TRUE;
}



BOOL PRM_DrawFxLine_F_Sub( char *dest_p, MIN_MAX *mi, MIN_MAX *ma, RECT *clip, char *rgb_tbl )
{
	int		x,w;
	int		dx1,dx2;
	int		w2, w3, wa;
	char	*bld_tbl=SubTable+255;

	if( mi->x == DispWidth )	return TRUE;	

	dx1	= max( clip->left,  mi->x );
	dx2 = min( clip->right, ma->x );
	w = dx2-dx1;
	dest_p += dx1*3;

	if( MMX_Flag ){
		w2 = w*3;
		w3 = w2/8;
		wa = w2%8;
		if(w3){
			_asm {
				mov			ecx, w3
				mov			eax, rgb_tbl
				mov			ebx, dest_p
				mov			edx, 0
			loopx:
				movq		mm0, [ebx]
				movq		mm1, [eax]
				psubusb		mm0, mm1
				movq		[ebx], mm0
				add			eax, 8
				add			ebx, 8

				inc			edx
				cmp			edx, 3
				jne			next1
				mov			edx, 0
				mov			eax, rgb_tbl
			next1:

				loop	loopx
			}
			_asm	emms

		}
		dest_p += w2;

		if( wa ){	
			for( x=0; x < wa ; x++ ){
				*(dest_p-wa+x) = bld_tbl[ *(dest_p-wa+x) + *(rgb_tbl-wa+x+24) ];
			}
		}
	}else{
		for( x=0 ; x<w ; x++, dest_p+=3 ){
			*(dest_p  ) = bld_tbl[ *(dest_p  ) + rgb_tbl[0] ];
			*(dest_p+1) = bld_tbl[ *(dest_p+1) + rgb_tbl[1] ];
			*(dest_p+2) = bld_tbl[ *(dest_p+2) + rgb_tbl[2] ];
		}
	}
	return TRUE;
}



BOOL PRM_DrawFxLine_F_Mul( char *dest_p, MIN_MAX *mi, MIN_MAX *ma, RECT *clip, char *rgb_tbl )
{
	int		x,w;
	int		dx1,dx2;
	char	*blend_tblr = BrightTable_TB[ rgb_tbl[2] ];
	char	*blend_tblg = BrightTable_TB[ rgb_tbl[1] ];
	char	*blend_tblb = BrightTable_TB[ rgb_tbl[0] ];

	if( mi->x == DispWidth )	return TRUE;	

	dx1	= max( clip->left,  mi->x );
	dx2 = min( clip->right, ma->x );
	w = dx2-dx1;
	dest_p += dx1*3;

	for( x=0 ; x < w ; x++, dest_p+=3 ){
		*(dest_p  ) = blend_tblb[ *(dest_p  ) ];
		*(dest_p+1) = blend_tblg[ *(dest_p+1) ];
		*(dest_p+2) = blend_tblr[ *(dest_p+2) ];
	}
	return TRUE;
}






BOOL PRM_DrawF3_F( BMP_F *dest,	int x1, int y1,
								int x2, int y2,
								int x3, int y3,
								RECT *clip, int r, int g, int b,
								DWORD wparam )
{
	BOOL		ret=TRUE;
	int			blnd  = HIWORD(wparam);
	int			param = LOWORD(wparam);
	char		*dest_p = (char*)dest->buf;
	int			drev = dest->sx*3;
	MIN_MAX		*mi = (MIN_MAX*)GAlloc(DispHeight*sizeof(MIN_MAX));
	MIN_MAX		*ma = (MIN_MAX*)GAlloc(DispHeight*sizeof(MIN_MAX));
	int			min_y, max_y;
	int			y;
	char		rgb_tbl[24] = {	b, g, r, b, g, r, b, g,
								r, b, g, r, b, g, r, b,
								g, r, b, g, r, b, g, r };

	if( dest->buf==NULL ) { DebugPrintf("•`‰æ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ\n"); ret=FALSE; goto end; }

	if( clip==NULL ) { clip = &ClipRectDef; }	

	
	if( param==DRW_AMI2 || param== DRW_BLD2 ){
		if( blnd==BLD_0P   )goto end;			
		if( blnd==BLD_100P )wparam = DRW_NML;	
	}
	min_y = y1;
	min_y = min(min_y,y2);
	min_y = min(min_y,y3);

	max_y = y1;
	max_y = max(max_y,y2);
	max_y = max(max_y,y3);

	min_y = max(min_y,clip->top);
	max_y = min(max_y,clip->bottom-1);

	for( y=min_y; y<=max_y; y++ )	{	mi[y-min_y].x = clip->right;	ma[y-min_y].x = 0;	}
	DrawMinMaxTable( x1, y1-min_y, x2, y2-min_y, clip, mi, ma );	
	DrawMinMaxTable( x2, y2-min_y, x3, y3-min_y, clip, mi, ma );	
	DrawMinMaxTable( x3, y3-min_y, x1, y1-min_y, clip, mi, ma );	

	dest_p = dest_p + drev*min_y;
	switch(param){
		default:
		case DRW_NML:	
			for( y = min_y ; y <= max_y ; y++, dest_p+=drev )
				PRM_DrawFxLine_F_Std( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl );
			break;
		case DRW_BLD2:	
			for( y = min_y ; y <= max_y ; y++, dest_p+=drev )
				PRM_DrawFxLine_F_Bld( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl, blnd );
			break;
		case DRW_ADD:	
			for( y = min_y ; y <= max_y ; y++, dest_p+=drev )
				PRM_DrawFxLine_F_Add( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl, param );
			break;
		case DRW_SUB:	
			for( y = min_y ; y <= max_y ; y++, dest_p+=drev )
				PRM_DrawFxLine_F_Sub( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl );
			break;
		case DRW_MUL:	
			for( y = min_y ; y <= max_y ; y++, dest_p+=drev )
				PRM_DrawFxLine_F_Mul( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl );
			break;
		case DRW_AMI2:			break;	
		case DRW_NEG:			break;	
		case DRW_VIV2:			break;	
	}
end:
	GFree(mi);
	GFree(ma);
	return ret;
}





BOOL PRM_DrawF4_F( BMP_F *dest,	int x1, int y1,
								int x2, int y2,
								int x3, int y3,
								int x4, int y4,
								RECT *clip, int r, int g, int b,
								DWORD wparam )
{
	BOOL		ret=TRUE;
	int			blnd  = HIWORD(wparam);
	int			param = LOWORD(wparam);
	char		*dest_p = (char*)dest->buf;
	int			drev = dest->sx*3;
	MIN_MAX		*mi = (MIN_MAX*)GAlloc(DispHeight*sizeof(MIN_MAX));
	MIN_MAX		*ma = (MIN_MAX*)GAlloc(DispHeight*sizeof(MIN_MAX));
	int			min_y, max_y;
	int			y;
	char		rgb_tbl[24] = {	b, g, r, b, g, r, b, g,
								r, b, g, r, b, g, r, b,
								g, r, b, g, r, b, g, r };

	if( dest->buf==NULL ) { DebugPrintf("•`‰æ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ\n"); ret=FALSE; goto end; }

	if( clip==NULL ) { clip = &ClipRectDef; }	

	
	if( param==DRW_AMI2 || param== DRW_BLD2 ){
		if( blnd==BLD_0P   )goto end;			
		if( blnd==BLD_100P )wparam = DRW_NML;	
	}

	min_y = y1;
	min_y = min(min_y,y2);
	min_y = min(min_y,y3);
	min_y = min(min_y,y4);

	max_y = y1;
	max_y = max(max_y,y2);
	max_y = max(max_y,y3);
	max_y = max(max_y,y4);

	min_y = max(min_y,clip->top);
	max_y = min(max_y,clip->bottom-1);

	for( y=min_y; y<=max_y ; y++ )	{	mi[y-min_y].x = DispWidth;	ma[y-min_y].x = 0;	}
	DrawMinMaxTable( x1, y1-min_y, x2, y2-min_y, clip, mi, ma );	
	DrawMinMaxTable( x2, y2-min_y, x4, y4-min_y, clip, mi, ma );	
	DrawMinMaxTable( x4, y4-min_y, x3, y3-min_y, clip, mi, ma );	
	DrawMinMaxTable( x3, y3-min_y, x1, y1-min_y, clip, mi, ma );	

	dest_p = dest_p + drev*min_y;
	switch(param){
		default:
		case DRW_NML:	
			for( y = min_y ; y <= max_y ; y++, dest_p+=drev )
				PRM_DrawFxLine_F_Std( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl );
			break;
		case DRW_BLD2:	
			for( y = min_y ; y <= max_y ; y++, dest_p+=drev )
				PRM_DrawFxLine_F_Bld( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl, blnd );
			break;
		case DRW_ADD:	
			for( y = min_y ; y <= max_y ; y++, dest_p+=drev )
				PRM_DrawFxLine_F_Add( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl, param );
			break;
		case DRW_SUB:	
			for( y = min_y ; y <= max_y ; y++, dest_p+=drev )
				PRM_DrawFxLine_F_Sub( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl );
			break;
		case DRW_MUL:	
			for( y = min_y ; y <= max_y ; y++, dest_p+=drev )
				PRM_DrawFxLine_F_Mul( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl );
			break;
		case DRW_AMI2:			break;	
		case DRW_NEG:			break;	
		case DRW_VIV2:			break;	
	}
end:
	GFree(mi);
	GFree(ma);
	return ret;
}










BOOL PRM_DrawY3_F( BMP_F *dest,	int x1, int y1,
								int x2, int y2,
								int x3, int y3, RECT *clip,
								int sy, int sr, int sg, int sb,
								int ey, int er, int eg, int eb,
								DWORD wparam )
{
	BOOL		ret=TRUE;
	int			blnd  = HIWORD(wparam);
	int			param = LOWORD(wparam);
	char		*dest_p = (char*)dest->buf;
	int			drev = dest->sx*3;
	MIN_MAX		*mi = (MIN_MAX*)GAlloc(DispHeight*sizeof(MIN_MAX));
	MIN_MAX		*ma = (MIN_MAX*)GAlloc(DispHeight*sizeof(MIN_MAX));
	int			min_y, max_y;
	int			i, y, ay;
	int			esy = ey-sy;
	int			esr = er-sr;
	int			esg = eg-sg;
	int			esb = eb-sb;
	char		rgb_tbl[24];

	if( sy==ey || (sr==er && sg==eg && sb==eb) )
		return PRM_DrawF3_F( dest, x1, y1, x2, y2, x3, y3, clip, sr, sg, sb,wparam );

	
	if( dest->buf==NULL ) { DebugPrintf("•`‰æ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ\n"); ret=FALSE; goto end; }

	if( clip==NULL ) { clip = &ClipRectDef; }	

	
	if( param==DRW_AMI2 || param== DRW_BLD2 ){
		if( blnd==BLD_0P   )goto end;			
		if( blnd==BLD_100P )wparam = DRW_NML;	
	}

	min_y = y1;
	min_y = min(min_y,y2);
	min_y = min(min_y,y3);

	max_y = y1;
	max_y = max(max_y,y2);
	max_y = max(max_y,y3);

	min_y = max(min_y,clip->top);
	max_y = min(max_y,clip->bottom-1);

	for( y=min_y; y<=max_y ; y++ )	{	mi[y-min_y].x = DispWidth;	ma[y-min_y].x = 0;	}
	DrawMinMaxTable( x1, y1-min_y, x2, y2-min_y, clip, mi, ma );	
	DrawMinMaxTable( x2, y2-min_y, x3, y3-min_y, clip, mi, ma );	
	DrawMinMaxTable( x3, y3-min_y, x1, y1-min_y, clip, mi, ma );	

	dest_p = dest_p + drev*min_y;
	for( y = min_y ; y <= max_y ; y++, dest_p+=drev ){
		ay = y-sy;
		for( i = 0 ; i < 8 ; i++ ){
			rgb_tbl[i*3+0] = (char)STD_Limit( esb*ay/esy + sb, sb, eb );	
			rgb_tbl[i*3+1] = (char)STD_Limit( esg*ay/esy + sg, sg, eg );	
			rgb_tbl[i*3+2] = (char)STD_Limit( esr*ay/esy + sr, sr, er );	
		}
		switch(param){
			default:
			case DRW_NML:	PRM_DrawFxLine_F_Std( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl );		break;	
			case DRW_BLD2:	PRM_DrawFxLine_F_Bld( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl, blnd );	break;	
			case DRW_ADD:	PRM_DrawFxLine_F_Add( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl, param );	break;
			case DRW_SUB:	PRM_DrawFxLine_F_Sub( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl );		break;	
			case DRW_MUL:	PRM_DrawFxLine_F_Mul( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl );		break;	
			case DRW_AMI2:			break;	
			case DRW_NEG:			break;	
			case DRW_VIV2:			break;	
		}
	}
end:
	GFree(mi);
	GFree(ma);
	return ret;
}






BOOL PRM_DrawY4_F( BMP_F *dest,	int x1, int y1,
								int x2, int y2,
								int x3, int y3,
								int x4, int y4, RECT *clip,
								int sy, int sr, int sg, int sb,
								int ey, int er, int eg, int eb,
								DWORD wparam )
{
	BOOL		ret=TRUE;
	int			blnd  = HIWORD(wparam);
	int			param = LOWORD(wparam);
	char		*dest_p = (char*)dest->buf;
	int			drev = dest->sx*3;
	MIN_MAX		*mi = (MIN_MAX*)GAlloc(DispHeight*sizeof(MIN_MAX));
	MIN_MAX		*ma = (MIN_MAX*)GAlloc(DispHeight*sizeof(MIN_MAX));
	int			min_y, max_y;
	int			i, y, ay;
	int			esy = ey-sy;
	int			esr = er-sr;
	int			esg = eg-sg;
	int			esb = eb-sb;
	char		rgb_tbl[24];

	if( sy==ey || (sr==er && sg==eg && sb==eb) )
		return PRM_DrawF4_F( dest, x1, y1, x2, y2, x3, y3, x4, y4, clip, sr, sg, sb, wparam );

	
	if( dest->buf==NULL ) { DebugPrintf("•`‰æ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ\n"); ret=FALSE; goto end; }

	if( clip==NULL ) { clip = &ClipRectDef; }	

	
	if( param==DRW_AMI2 || param== DRW_BLD2 ){
		if( blnd==BLD_0P   )goto end;			
		if( blnd==BLD_100P )wparam = DRW_NML;	
	}

	min_y = y1;
	min_y = min(min_y,y2);
	min_y = min(min_y,y3);
	min_y = min(min_y,y4);

	max_y = y1;
	max_y = max(max_y,y2);
	max_y = max(max_y,y3);
	max_y = max(max_y,y4);

	min_y = max(min_y,clip->top);
	max_y = min(max_y,clip->bottom-1);

	for( y=min_y; y<=max_y ; y++ )	{	mi[y-min_y].x = DispWidth;	ma[y-min_y].x = 0;	}
	DrawMinMaxTable( x1, y1-min_y, x2, y2-min_y, clip, mi, ma );	
	DrawMinMaxTable( x2, y2-min_y, x4, y4-min_y, clip, mi, ma );	
	DrawMinMaxTable( x4, y4-min_y, x3, y3-min_y, clip, mi, ma );	
	DrawMinMaxTable( x3, y3-min_y, x1, y1-min_y, clip, mi, ma );	

	dest_p = dest_p + drev*min_y;
	for( y = min_y ; y <= max_y ; y++, dest_p+=drev ){
		ay = y-sy;
		rgb_tbl[0] = (char)STD_Limit( esb*ay/esy + sb, sb, eb );	
		rgb_tbl[1] = (char)STD_Limit( esg*ay/esy + sg, sg, eg );	
		rgb_tbl[2] = (char)STD_Limit( esr*ay/esy + sr, sr, er );	
		for( i = 1 ; i < 8 ; i++ ){
			rgb_tbl[i*3+0] = rgb_tbl[0];	
			rgb_tbl[i*3+1] = rgb_tbl[1];	
			rgb_tbl[i*3+2] = rgb_tbl[2];	
		}
		switch(param){
			default:
			case DRW_NML:	PRM_DrawFxLine_F_Std( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl );			break;	
			case DRW_BLD2:	PRM_DrawFxLine_F_Bld( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl, blnd );	break;	
			case DRW_ADD:	PRM_DrawFxLine_F_Add( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl, param );	break;
			case DRW_SUB:	PRM_DrawFxLine_F_Sub( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl );			break;	
			case DRW_MUL:	PRM_DrawFxLine_F_Mul( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl );			break;	
			case DRW_AMI2:			break;	
			case DRW_NEG:			break;	
			case DRW_VIV2:			break;	
		}
	}
end:
	GFree(mi);
	GFree(ma);
	return ret;
}




BOOL PRM_DrawGxLine_F_Std( char *dest_p, MIN_MAX *mi, MIN_MAX *ma,
							int r,  int g,  int b,  int pr, int pg, int pb,
							int dr, int dg, int db, int fr, int fg, int fb, int  w, int ww)
{
	int		rr=0,gg=0,bb=0;
	int		x;
	int		w2 = w-1;
	WORD	rgb1[4];
	WORD	rgb2[4];
	WORD	rgb3[4];
	DWORD	temp = 0x10101010;

	if( MMX_Flag ){
		if( w2 > 0 ){
			if( fr==-1 ) { temp |= 0x00010000;	temp &= ~0x00100000; }
			if( fg==-1 ) { temp |= 0x00000100;	temp &= ~0x00001000; }
			if( fb==-1 ) { temp |= 0x00000001;	temp &= ~0x00000010; }
			rgb1[0] = b<<8;
			rgb1[1] = g<<8;
			rgb1[2] = r<<8;
			rgb1[3] = 0;

			rgb2[0] = pb*256;
			rgb2[1] = pg*256;
			rgb2[2] = pr*256;
			rgb2[3] = 0;

			rgb3[0] = db*256/ww;
			rgb3[1] = dg*256/ww;
			rgb3[2] = dr*256/ww;
			rgb3[3] = 0;

			_asm {
				mov			ecx, w2
				mov			ebx, dest_p
				
				pxor		mm0, mm0	
				movd		mm6, temp
				punpcklbw	mm6, mm0
				psllw		mm6, 8		

				movq		mm1, [rgb1]	
				movq		mm2, [rgb2]	
				movq		mm3, [rgb3]	
			loopx:
				paddb		mm1, mm2	
				paddw		mm0, mm3	

				movq		mm4, mm0	

				PCMPEQB		mm4, mm6	
				por			mm4, mm0	

				psllw		mm0, 8		
				psrlw		mm0, 8		

				paddb		mm1, mm4	
				movq		mm4, mm1	
				
				psrlw		mm4, 8		
				packuswb	mm4, mm0	
				movd		[ebx], mm4	

				add			ebx, 3		

				loop	loopx
			}
			_asm	emms
		}
		if(w){
			*(dest_p+w2*3  ) = min( 255, b + pb*w2 + db*w2/ww );
			*(dest_p+w2*3+1) = min( 255, g + pg*w2 + dg*w2/ww );
			*(dest_p+w2*3+2) = min( 255, r + pr*w2 + dr*w2/ww );
		}
	}else{
		for( x=0; x < w ; x++, dest_p+=3 ){
			*(dest_p  ) = b;
			*(dest_p+1) = g;
			*(dest_p+2) = r;

			r +=pr;	g +=pg;	b +=pb;
			rr+=dr;	gg+=dg;	bb+=db;
			if( rr >= ww ){ rr -= ww; r+=fr; }
			if( gg >= ww ){ gg -= ww; g+=fg; }
			if( bb >= ww ){ bb -= ww; b+=fb; }
		}
	}
	return TRUE;
}



BOOL PRM_DrawGxLine_F_Bld( char *dest_p, MIN_MAX *mi, MIN_MAX *ma,
							int r,  int g,  int b,  int pr, int pg, int pb,
							int dr, int dg, int db, int fr, int fg, int fb, int  w, int ww, int blnd )
{
	int		brev = 256-blnd;
	int		rr=0,gg=0,bb=0;
	int		x;
	int		w2 = w-1;
	WORD	rgb1[4];
	WORD	rgb2[4];
	WORD	rgb3[4];
	DWORD	temp = 0x10101010;
	DWORD	temp2=0;
	DWORD	alpha2 = brev*0x01010101;
	char	*brev_tbl=BlendTable[brev];

	dr = dr*blnd/256 + pr*blnd%256*ww/256*fr;
	dg = dg*blnd/256 + pg*blnd%256*ww/256*fg;
	db = db*blnd/256 + pb*blnd%256*ww/256*fb;
	rr = r*blnd%256*ww/256;
	gg = g*blnd%256*ww/256;
	bb = b*blnd%256*ww/256;

	r  = r*blnd/256;
	g  = g*blnd/256;
	b  = b*blnd/256;
	pr = pr*blnd/256;
	pg = pg*blnd/256;
	pb = pb*blnd/256;

	if(dr>=ww) { dr-=ww; pr+=fr; }
	if(dg>=ww) { dg-=ww; pg+=fg; }
	if(db>=ww) { db-=ww; pb+=fb; }

	if( MMX_Flag ){
		if( w2 > 0 ){
			if( fr==-1 ) { temp |= 0x00010000;	temp &= ~0x00100000; }
			if( fg==-1 ) { temp |= 0x00000100;	temp &= ~0x00001000; }
			if( fb==-1 ) { temp |= 0x00000001;	temp &= ~0x00000010; }
			rgb1[0] = b<<8;
			rgb1[1] = g<<8;
			rgb1[2] = r<<8;
			rgb1[3] = 0;

			rgb2[0] = pb*256;
			rgb2[1] = pg*256;
			rgb2[2] = pr*256;
			rgb2[3] = 0;

			rgb3[0] = db*256/ww;
			rgb3[1] = dg*256/ww;
			rgb3[2] = dr*256/ww;
			rgb3[3] = 0;

			_asm {
				mov			ecx, w2
				mov			ebx, dest_p
				
				pxor		mm0, mm0	
				movd		mm6, temp
				punpcklbw	mm6, mm0
				psllw		mm6, 8		

				movd		mm3, temp2	
				movd		mm5, alpha2	
				punpcklbw	mm5, mm3	

				movq		mm1, [rgb1]	
				movq		mm2, [rgb2]	
				movq		mm3, [rgb3]	
			loopx:
				paddb		mm1, mm2	
				paddw		mm0, mm3	

				movd		mm4, temp2	
				movd		mm7, [ebx]
				punpcklbw	mm7, mm4
				pmullw		mm7, mm5	

				movq		mm4, mm0	

				PCMPEQB		mm4, mm6	
				por			mm4, mm0	

				psllw		mm0, 8		
				psrlw		mm0, 8		

				paddb		mm1, mm4	
				movq		mm4, mm1	
				
				paddusw		mm4, mm7	

				psrlw		mm4, 8		
				packuswb	mm4, mm0	
				movd		[ebx], mm4	

				add			ebx, 3		

				loop	loopx
			}
			_asm	emms
		}
		if(w){
			*(dest_p+w2*3  ) = brev_tbl[ *(dest_p  ) ] + (b + pb*w2 + db*w2/ww*fb);
			*(dest_p+w2*3+1) = brev_tbl[ *(dest_p+1) ] + (g + pg*w2 + dg*w2/ww*fg);
			*(dest_p+w2*3+2) = brev_tbl[ *(dest_p+2) ] + (r + pr*w2 + dr*w2/ww*fr);
		}
	}else{
		for( x=0; x < w ; x++, dest_p+=3 ){
	
	
	
			*(dest_p  ) = brev_tbl[ *(dest_p  ) ] + b;
			*(dest_p+1) = brev_tbl[ *(dest_p+1) ] + g;
			*(dest_p+2) = brev_tbl[ *(dest_p+2) ] + r;

			r +=pr;	g +=pg;	b +=pb;
			rr+=dr;	gg+=dg;	bb+=db;
			if( rr >= ww ){ rr -= ww; r+=fr; }
			if( gg >= ww ){ gg -= ww; g+=fg; }
			if( bb >= ww ){ bb -= ww; b+=fb; }
		}
	}
	return TRUE;
}



BOOL PRM_DrawGxLine_F_Add( char *dest_p, MIN_MAX *mi, MIN_MAX *ma,
							int r,  int g,  int b,  int pr, int pg, int pb,
							int dr, int dg, int db, int fr, int fg, int fb, int  w, int ww )
{
	int		rr=0,gg=0,bb=0;
	int		x;
	char	*add_tbl=AddTable;

	for( x=0; x < w ; x++, dest_p+=3 ){
		*(dest_p  ) = add_tbl[ *(dest_p  ) + b ];
		*(dest_p+1) = add_tbl[ *(dest_p+1) + g ];
		*(dest_p+2) = add_tbl[ *(dest_p+2) + r ];

		r +=pr;	g +=pg;	b +=pb;
		rr+=dr;	gg+=dg;	bb+=db;
		if( rr >= ww ){ rr -= ww; r+=fr; }
		if( gg >= ww ){ gg -= ww; g+=fg; }
		if( bb >= ww ){ bb -= ww; b+=fb; }
	}
	return TRUE;
}



BOOL PRM_DrawGxLine_F_Sub( char *dest_p, MIN_MAX *mi, MIN_MAX *ma,
							int r,  int g,  int b,  int pr, int pg, int pb,
							int dr, int dg, int db, int fr, int fg, int fb, int  w, int ww )
{
	int		rr=0,gg=0,bb=0;
	int		x;
	char	*sub_tbl=SubTable+255;

	for( x=0; x < w ; x++, dest_p+=3 ){
		*(dest_p  ) = sub_tbl[ *(dest_p  ) - b ];
		*(dest_p+1) = sub_tbl[ *(dest_p+1) - g ];
		*(dest_p+2) = sub_tbl[ *(dest_p+2) - r ];

		r +=pr;	g +=pg;	b +=pb;
		rr+=dr;	gg+=dg;	bb+=db;
		if( rr >= ww ){ rr -= ww; r+=fr; }
		if( gg >= ww ){ gg -= ww; g+=fg; }
		if( bb >= ww ){ bb -= ww; b+=fb; }
	}
	return TRUE;
}



BOOL PRM_DrawGxLine_F_Mul( char *dest_p, MIN_MAX *mi, MIN_MAX *ma,
							int r,  int g,  int b,  int pr, int pg, int pb,
							int dr, int dg, int db, int fr, int fg, int fb, int  w, int ww )
{
	int		rr=0,gg=0,bb=0;
	int		x;

	for( x=0; x < w ; x++, dest_p+=3 ){
		*(dest_p  ) = BrightTable_TB[ b ][ *(dest_p  ) ];
		*(dest_p+1) = BrightTable_TB[ g ][ *(dest_p+1) ];
		*(dest_p+2) = BrightTable_TB[ r ][ *(dest_p+2) ];

		r +=pr;	g +=pg;	b +=pb;
		rr+=dr;	gg+=dg;	bb+=db;
		if( rr >= ww ){ rr -= ww; r+=fr; }
		if( gg >= ww ){ gg -= ww; g+=fg; }
		if( bb >= ww ){ bb -= ww; b+=fb; }
	}
	return TRUE;
}




BOOL PRM_DrawGxLine_F_Ola( char *dest_p, MIN_MAX *mi, MIN_MAX *ma,
							int r,  int g,  int b,  int pr, int pg, int pb,
							int dr, int dg, int db, int fr, int fg, int fb, int  w, int ww )
{
	int		rr=0,gg=0,bb=0;
	int		x;

	for( x=0; x < w ; x++, dest_p+=3 ){
		*(dest_p  ) = *(dest_p  ) + b;
		*(dest_p+1) = *(dest_p+1) + g;
		*(dest_p+2) = *(dest_p+2) + r;

		r +=pr;	g +=pg;	b +=pb;
		rr+=dr;	gg+=dg;	bb+=db;
		if( rr >= ww ){ rr -= ww; r+=fr; }
		if( gg >= ww ){ gg -= ww; g+=fg; }
		if( bb >= ww ){ bb -= ww; b+=fb; }
	}
	return TRUE;
}



BOOL PRM_DrawGxLine_F( char *dest_p, MIN_MAX *mi, MIN_MAX *ma, RECT *clip, DWORD wparam)
{
	BOOL	ret=FALSE;
	int		blnd  = HIWORD(wparam);
	int		param = LOWORD(wparam);
	int		dx1,dx2,w;
	int		xx, ww = ma->x - mi->x;
	int		r=mi->rgb.r, g=mi->rgb.g, b=mi->rgb.b;
	int		dr,dg,db;
	int		fr,fg,fb;

	if( mi->x == DispWidth )	return 1;
	if( ww==0 )			return 1;

	dx1 = max( mi->x, clip->left  );
	dx2 = min( ma->x, clip->right );

	w  = dx2-dx1;
	xx = dx1 - mi->x;
	dest_p += dx1*3;

	r = ma->rgb.r   - mi->rgb.r;
	g = ma->rgb.g - mi->rgb.g;
	b = ma->rgb.b  - mi->rgb.b;

	if(r<0)	{	dr = (-r)%ww;	fr=-1;	}
	else	{	dr =   r %ww;	fr= 1;	}
	if(g<0)	{	dg = (-g)%ww;	fg=-1;	}
	else	{	dg =   g %ww;	fg= 1;	}
	if(b<0)	{	db = (-b)%ww;	fb=-1;	}
	else	{	db =   b %ww;	fb= 1;	}

	switch(param){
		default:
		case DRW_NML:	
			ret = PRM_DrawGxLine_F_Std( dest_p, mi, ma,
										r*xx/ww + mi->rgb.r, g*xx/ww + mi->rgb.g, b*xx/ww + mi->rgb.b,
										r/ww, g/ww, b/ww, dr, dg, db, fr, fg, fb, w, ww );
			break;
		case DRW_BLD2:	
			ret = PRM_DrawGxLine_F_Bld( dest_p, mi, ma,
										r*xx/ww + mi->rgb.r, g*xx/ww + mi->rgb.g, b*xx/ww + mi->rgb.b,
										r/ww, g/ww, b/ww, dr, dg, db, fr, fg, fb, w, ww, blnd );
			break;
		case DRW_ADD:	
			ret = PRM_DrawGxLine_F_Add( dest_p, mi, ma,
										r*xx/ww + mi->rgb.r, g*xx/ww + mi->rgb.g, b*xx/ww + mi->rgb.b,
										r/ww, g/ww, b/ww, dr, dg, db, fr, fg, fb, w, ww );
			break;
		case DRW_SUB:	
			ret = PRM_DrawGxLine_F_Sub( dest_p, mi, ma,
										r*xx/ww + mi->rgb.r, g*xx/ww + mi->rgb.g, b*xx/ww + mi->rgb.b,
										r/ww, g/ww, b/ww, dr, dg, db, fr, fg, fb, w, ww );
			break;
		case DRW_MUL:	
			ret = PRM_DrawGxLine_F_Mul( dest_p, mi, ma,
										r*xx/ww + mi->rgb.r, g*xx/ww + mi->rgb.g, b*xx/ww + mi->rgb.b,
										r/ww, g/ww, b/ww, dr, dg, db, fr, fg, fb, w, ww );
			break;
		case DRW_AMI2:			break;	
		case DRW_NEG:			break;	
		case DRW_VIV2:			break;	
	}
	return ret;
}






BOOL PRM_DrawG3_F( BMP_F *dest,	int x1, int y1, int r1, int g1, int b1,
								int x2, int y2, int r2, int g2, int b2,
								int x3, int y3, int r3, int g3, int b3,
								RECT *clip, DWORD wparam )
{
	BOOL		ret=TRUE;
	int			blnd  = HIWORD(wparam);
	int			param = LOWORD(wparam);
	char		*dest_p = (char*)dest->buf;
	int			drev = dest->sx*3;
	MIN_MAX		*mi = (MIN_MAX*)GAlloc(DispHeight*sizeof(MIN_MAX));
	MIN_MAX		*ma = (MIN_MAX*)GAlloc(DispHeight*sizeof(MIN_MAX));
	int			min_y, max_y;
	int			y;

	if( dest->buf==NULL ) { DebugPrintf("•`‰æ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ\n"); ret=FALSE; goto end; }

	if( clip==NULL ) { clip = &ClipRectDef; }	

	
	if( param==DRW_AMI2 || param== DRW_BLD2 ){
		if( blnd==BLD_0P   )goto end;			
		if( blnd==BLD_100P )wparam = DRW_NML;	
	}
	min_y = y1;
	min_y = min(min_y,y2);
	min_y = min(min_y,y3);

	max_y = y1;
	max_y = max(max_y,y2);
	max_y = max(max_y,y3);

	min_y = max(min_y,clip->top);
	max_y = min(max_y,clip->bottom-1);

	for( y=min_y; y<=max_y; y++ )	{	mi[y-min_y].x = DispWidth;	ma[y-min_y].x = 0;	}
	DrawMinMaxTableRGB( x1, y1-min_y, r1, g1, b1, x2, y2-min_y, r2, g2, b2, clip, mi, ma );	
	DrawMinMaxTableRGB( x2, y2-min_y, r2, g2, b2, x3, y3-min_y, r3, g3, b3, clip, mi, ma );	
	DrawMinMaxTableRGB( x3, y3-min_y, r3, g3, b3, x1, y1-min_y, r1, g1, b1, clip, mi, ma );	

	dest_p = dest_p + drev*min_y;
	for( y = min_y ; y <= max_y ; y++, dest_p+=drev )
		PRM_DrawGxLine_F( dest_p, &mi[y-min_y], &ma[y-min_y], clip, wparam );
end:
	GFree(mi);
	GFree(ma);
	return ret;
}





BOOL PRM_DrawG4_F( BMP_F *dest,	int x1, int y1, int r1, int g1, int b1,
								int x2, int y2, int r2, int g2, int b2,
								int x3, int y3, int r3, int g3, int b3,
								int x4, int y4, int r4, int g4, int b4,
								RECT *clip, DWORD wparam )
{
	BOOL		ret=TRUE;
	int			blnd  = HIWORD(wparam);
	int			param = LOWORD(wparam);
	char		*dest_p = (char*)dest->buf;
	int			drev = dest->sx*3;
	MIN_MAX		*mi = (MIN_MAX*)GAlloc(DispHeight*sizeof(MIN_MAX));
	MIN_MAX		*ma = (MIN_MAX*)GAlloc(DispHeight*sizeof(MIN_MAX));
	int			min_y, max_y;
	int			y;

	if( dest->buf==NULL ) { DebugPrintf("•`‰æ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ\n"); ret=FALSE; goto end; }

	if( clip==NULL ) { clip = &ClipRectDef; }	

	
	if( param==DRW_AMI2 || param== DRW_BLD2 ){
		if( blnd==BLD_0P   )goto end;			
		if( blnd==BLD_100P )wparam = DRW_NML;	
	}

	min_y = y1;
	min_y = min(min_y,y2);
	min_y = min(min_y,y3);
	min_y = min(min_y,y4);

	max_y = y1;
	max_y = max(max_y,y2);
	max_y = max(max_y,y3);
	max_y = max(max_y,y4);

	min_y = max(min_y,clip->top);
	max_y = min(max_y,clip->bottom-1);

	for( y=min_y; y<=max_y; y++ )	{	mi[y-min_y].x = DispWidth;	ma[y-min_y].x = 0;	}
	DrawMinMaxTableRGB( x1, y1-min_y, r1, g1, b1, x2, y2-min_y, r2, g2, b2, clip, mi, ma );	
	DrawMinMaxTableRGB( x2, y2-min_y, r2, g2, b2, x4, y4-min_y, r4, g4, b4, clip, mi, ma );	
	DrawMinMaxTableRGB( x4, y4-min_y, r4, g4, b4, x3, y3-min_y, r3, g3, b3, clip, mi, ma );	
	DrawMinMaxTableRGB( x3, y3-min_y, r3, g3, b3, x1, y1-min_y, r1, g1, b1, clip, mi, ma );	

	dest_p = dest_p + drev*min_y;
	for( y = min_y ; y <= max_y ; y++, dest_p+=drev )
		PRM_DrawGxLine_F( dest_p, &mi[y-min_y], &ma[y-min_y], clip, wparam );

end:
	GFree(mi);
	GFree(ma);
	return ret;
}




BOOL PRM_DrawFLine_F_Std( BMP_F *dest,	int x1, int y1,
										int x2, int y2,
										int r, int g, int b )
{
	RGB32	rgb = { b, g, r, 0 };
	char	*dest_p = (char*)(dest->buf + y1*dest->sx + x1);
	int		x,y;
	int		w=x2-x1, wcnt=0;
	int		h=y2-y1, hcnt=0;
	int		fw = (w<0)?-1:1;
	int		fh = (h<0)?-1:1;
	int		dh = dest->sx*fh*3;
	w = w*fw+1;
	h = h*fh+1;

	fw *= 3;
	if( w<h ){
		for( x=0; x < w ; x++, dest_p+=fw ){
			while( hcnt < h ){
				*(WORD*)dest_p = *(WORD*)&rgb;
				*(dest_p+2)    = rgb.r;
				
				hcnt += w;
				dest_p += dh;
			}
			hcnt -= h;
		}
	}else{
		for( y=0; y < h ; y++, dest_p += dh ){
			while( wcnt < w ){
				*(WORD*)dest_p = *(WORD*)&rgb;
				*(dest_p+2)    = rgb.r;
				
				wcnt += h;
				dest_p+=fw;
			}
			wcnt -= w;
		}
	}

	return 1;
}



BOOL PRM_DrawFLine_F_Bld( BMP_F *dest,	int x1, int y1,
										int x2, int y2,
										int r, int g, int b, int blnd )
{
	char	*dest_p = (char*)(dest->buf + y1*dest->sx + x1);
	int		x,y;
	int		w=x2-x1, wcnt=0;
	int		h=y2-y1, hcnt=0;
	int		fw = (w<0)?-1:1;
	int		fh = (h<0)?-1:1;
	int		dh = dest->sx*fh*3;
	char	*brev_tbl = BlendTable[256-blnd];
	int		r2 = (r*blnd)>>8, g2 = (g*blnd)>>8, b2 = (b*blnd)>>8;

	w = w*fw+1;
	h = h*fh+1;
	fw *= 3;
	if( w<h ){
		for( x=0; x < w ; x++, dest_p+=fw ){
			while( hcnt < h ){
				*(dest_p  ) = brev_tbl[ *(dest_p  ) ] + r2;
				*(dest_p+1) = brev_tbl[ *(dest_p+1) ] + g2;
				*(dest_p+2) = brev_tbl[ *(dest_p+2) ] + b2;
				
				hcnt += w;
				dest_p += dh;
			}
			hcnt -= h;
		}
	}else{
		for( y=0; y < h ; y++, dest_p += dh ){
			while( wcnt < w ){
				*(dest_p  ) = brev_tbl[ *(dest_p  ) ] + r2;
				*(dest_p+1) = brev_tbl[ *(dest_p+1) ] + g2;
				*(dest_p+2) = brev_tbl[ *(dest_p+2) ] + b2;
				
				wcnt += h;
				dest_p+=fw;
			}
			wcnt -= w;
		}
	}

	return 1;
}



BOOL PRM_DrawFLine_F_Add( BMP_F *dest,	int x1, int y1,
										int x2, int y2,
										int r, int g, int b )
{
	char	*dest_p = (char*)(dest->buf + y1*dest->sx + x1);
	int		x,y;
	int		w=x2-x1, wcnt=0;
	int		h=y2-y1, hcnt=0;
	int		fw = (w<0)?-1:1;
	int		fh = (h<0)?-1:1;
	int		dh = dest->sx*fh*3;
	char	*add_tbl = AddTable;

	if( r==128 && g==128 && b==128)	return 1;

	w = w*fw+1;
	h = h*fh+1;
	fw *= 3;
	if( w<h ){
		for( x=0; x < w ; x++, dest_p+=fw ){
			while( hcnt < h ){
				*(dest_p  ) = add_tbl[ *(dest_p  ) + r ];
				*(dest_p+1) = add_tbl[ *(dest_p+1) + g ];
				*(dest_p+2) = add_tbl[ *(dest_p+2) + b ];
				
				hcnt += w;
				dest_p += dh;
			}
			hcnt -= h;
		}
	}else{
		for( y=0; y < h ; y++, dest_p += dh ){
			while( wcnt < w ){
				*(dest_p  ) = add_tbl[ *(dest_p  ) + r ];
				*(dest_p+1) = add_tbl[ *(dest_p+1) + g ];
				*(dest_p+2) = add_tbl[ *(dest_p+2) + b ];
				
				wcnt += h;
				dest_p+=fw;
			}
			wcnt -= w;
		}
	}

	return 1;
}



BOOL PRM_DrawFLine_F_Sub( BMP_F *dest,	int x1, int y1,
										int x2, int y2,
										int r, int g, int b )
{
	char	*dest_p = (char*)(dest->buf + y1*dest->sx + x1);
	int		x,y;
	int		w=x2-x1, wcnt=0;
	int		h=y2-y1, hcnt=0;
	int		fw = (w<0)?-1:1;
	int		fh = (h<0)?-1:1;
	int		dh = dest->sx*fh*3;
	char	*sub_tbl = SubTable+255;

	if( r==128 && g==128 && b==128)	return 1;

	w = w*fw+1;
	h = h*fh+1;
	fw *= 3;
	if( w<h ){
		for( x=0; x < w ; x++, dest_p+=fw ){
			while( hcnt < h ){
				*(dest_p  ) = sub_tbl[ *(dest_p  ) - r ];
				*(dest_p+1) = sub_tbl[ *(dest_p+1) - g ];
				*(dest_p+2) = sub_tbl[ *(dest_p+2) - b ];
				
				hcnt += w;
				dest_p += dh;
			}
			hcnt -= h;
		}
	}else{
		for( y=0; y < h ; y++, dest_p += dh ){
			while( wcnt < w ){
				*(dest_p  ) = sub_tbl[ *(dest_p  ) - r ];
				*(dest_p+1) = sub_tbl[ *(dest_p+1) - g ];
				*(dest_p+2) = sub_tbl[ *(dest_p+2) - b ];
				
				wcnt += h;
				dest_p+=fw;
			}
			wcnt -= w;
		}
	}

	return 1;
}



BOOL PRM_DrawFLine_F_Mul( BMP_F *dest,	int x1, int y1,
										int x2, int y2,
										int r, int g, int b )
{
	char	*dest_p = (char*)(dest->buf + y1*dest->sx + x1);
	int		x,y;
	int		w=x2-x1, wcnt=0;
	int		h=y2-y1, hcnt=0;
	int		fw = (w<0)?-1:1;
	int		fh = (h<0)?-1:1;
	int		dh = dest->sx*fh*3;

	if( r==128 && g==128 && b==128)	return 1;

	w = w*fw+1;
	h = h*fh+1;
	fw *= 3;
	if( w<h ){
		for( x=0; x < w ; x++, dest_p+=fw ){
			while( hcnt < h ){
				*(dest_p  ) = BrightTable_TB[ r ][ *(dest_p  ) ];
				*(dest_p+1) = BrightTable_TB[ g ][ *(dest_p+1) ];
				*(dest_p+2) = BrightTable_TB[ b ][ *(dest_p+2) ];
				
				hcnt += w;
				dest_p += dh;
			}
			hcnt -= h;
		}
	}else{
		for( y=0; y < h ; y++, dest_p += dh ){
			while( wcnt < w ){
				*(dest_p  ) = BrightTable_TB[ r ][ *(dest_p  ) ];
				*(dest_p+1) = BrightTable_TB[ g ][ *(dest_p+1) ];
				*(dest_p+2) = BrightTable_TB[ b ][ *(dest_p+2) ];
				
				wcnt += h;
				dest_p+=fw;
			}
			wcnt -= w;
		}
	}

	return 1;
}






BOOL PRM_DrawFLine_F( BMP_F *dest,	int x1, int y1,
									int x2, int y2,
									RECT *clip, int r, int g, int b,
									DWORD wparam )
{
	BOOL		ret=TRUE;
	int			blnd  = HIWORD(wparam);
	int			param = LOWORD(wparam);

	if( dest->buf==NULL ) { DebugPrintf("•`‰æ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ[PRM_DrawFLine_F]\n"); return FALSE; }


	if( !ClipRectLine( &x1, &y1, &x2, &y2, dest->sx, dest->sy, clip ) ) return 1;

	
	if( param==DRW_AMI2 || param== DRW_BLD2 ){
		if( blnd==BLD_0P   )return 1;			
		if( blnd==BLD_100P )param = DRW_NML;	
	}

	switch(param){
		default:
		case DRW_NML:	ret = PRM_DrawFLine_F_Std( dest, x1, y1, x2, y2, r, g, b );			break;	
		case DRW_BLD2:	ret = PRM_DrawFLine_F_Bld( dest, x1, y1, x2, y2, r, g, b, blnd );	break;	
		case DRW_ADD:	ret = PRM_DrawFLine_F_Add( dest, x1, y1, x2, y2, r, g, b );			break;	
		case DRW_SUB:	ret = PRM_DrawFLine_F_Sub( dest, x1, y1, x2, y2, r, g, b );			break;	
		case DRW_MUL:	ret = PRM_DrawFLine_F_Mul( dest, x1, y1, x2, y2, r, g, b );			break;	
		case DRW_AMI2:			break;	
		case DRW_NEG:			break;	
		case DRW_VIV2:			break;	
	}
	return ret;
}





BOOL PRM_DrawGLine_F_Std( BMP_F *dest,	int x1, int y1, int r1, int g1, int b1,
										int x2, int y2, int r2, int g2, int b2 )
{
	int		x,y;
	char	*dest_p = (char*)(dest->buf + y1*dest->sx + x1);
	RGB32	rgb = { b1, g1, r1, 0 };
	int		r3 = r2-r1, g3 = g2-g1, b3 = b2-b1;
	int		r4, g4, b4, r5=0, g5=0, b5=0;
	int		fr = (r3<0)?-1:1, fg = (g3<0)?-1:1, fb=(b3<0)?-1:1;
	int		w=x2-x1,wcnt=0;
	int		h=y2-y1,hcnt=0;
	int		fw = (w <0)?-1:1, fh = ( h<0)?-1:1;
	int		dh = dest->sx*fh*3;
	w = w*fw+1;
	h = h*fh+1;

	fw *= 3;
	if( w<h ){
		r4 = r3   /h;	g4 = g3   /h;	b4 = b3   /h;
		r3 = r3*fr%h;	g3 = g3*fg%h;	b3 = b3*fb%h;
		for( x=0; x < w ; x++, dest_p+=fw ){
			while( hcnt < h ){
				*(WORD*)dest_p = *(WORD*)&rgb;
				*(dest_p+2)    = rgb.r;
				
				rgb.r  += r4;	r5 += r3;	if( r5>h ) { r5-=h; rgb.r  +=fr; }
				rgb.g+= g4;	g5 += g3;	if( g5>h ) { g5-=h; rgb.g+=fg; }
				rgb.b += b4;	b5 += b3;	if( b5>h ) { b5-=h; rgb.b +=fb; }

				hcnt += w;
				dest_p += dh;
			}
			hcnt -= h;
		}
	}else{
		r4 = r3/w;		g4 = g3/w;		b4 = b3/w;
		r3 = r3*fr%w;	g3 = g3*fg%w;	b3 = b3*fb%w;
		for( y=0; y < h ; y++, dest_p += dh ){
			while( wcnt < w ){
				*(WORD*)dest_p = *(WORD*)&rgb;
				*(dest_p+2)    = rgb.r;

				rgb.r  += r4;	r5 += r3;	if( r5>w ) { r5-=w; rgb.r  +=fr; }
				rgb.g+= g4;	g5 += g3;	if( g5>w ) { g5-=w; rgb.g+=fg; }
				rgb.b += b4;	b5 += b3;	if( b5>w ) { b5-=w; rgb.b +=fb; }

				wcnt += h;
				dest_p+=fw;
			}
			wcnt -= w;
		}
	}
	return 1;
}



BOOL PRM_DrawGLine_F_Bld( BMP_F *dest,	int x1, int y1, int r1, int g1, int b1,
										int x2, int y2, int r2, int g2, int b2, int blnd )
{
	int		x,y;
	char	*dest_p = (char*)(dest->buf + y1*dest->sx + x1);
	RGB32	rgb = { b1, g1, r1, 0 };
	int		r3 = r2-r1, g3 = g2-g1, b3 = b2-b1;
	int		r4, g4, b4, r5=0, g5=0, b5=0;
	int		fr = (r3<0)?-1:1, fg = (g3<0)?-1:1, fb=(b3<0)?-1:1;
	int		w=x2-x1,wcnt=0;
	int		h=y2-y1,hcnt=0;
	int		fw = (w <0)?-1:1, fh = ( h<0)?-1:1;
	int		dh = dest->sx*fh*3;
	char	*brev_tbl = BlendTable[256-blnd];
	char	*blnd_tbl = BlendTable[blnd];
	w = w*fw+1;
	h = h*fh+1;
	fw *= 3;

	if( w<h ){
		r4 = r3   /h;	g4 = g3   /h;	b4 = b3   /h;
		r3 = r3*fr%h;	g3 = g3*fg%h;	b3 = b3*fb%h;
		for( x=0; x < w ; x++, dest_p+=fw ){
			while( hcnt < h ){
				*(dest_p  ) = brev_tbl[ *(dest_p  ) ] + blnd_tbl[ rgb.b ];
				*(dest_p+1) = brev_tbl[ *(dest_p+1) ] + blnd_tbl[ rgb.g ];
				*(dest_p+2) = brev_tbl[ *(dest_p+2) ] + blnd_tbl[ rgb.r ];
				
				rgb.r+= r4;	r5 += r3;	if( r5>h ) { r5-=h; rgb.r+=fr; }
				rgb.g+= g4;	g5 += g3;	if( g5>h ) { g5-=h; rgb.g+=fg; }
				rgb.b+= b4;	b5 += b3;	if( b5>h ) { b5-=h; rgb.b+=fb; }

				hcnt += w;
				dest_p += dh;
			}
			hcnt -= h;
		}
	}else{
		r4 = r3/w;		g4 = g3/w;		b4 = b3/w;
		r3 = r3*fr%w;	g3 = g3*fg%w;	b3 = b3*fb%w;
		for( y=0; y < h ; y++, dest_p += dh ){
			while( wcnt < w ){
				*(dest_p  ) = brev_tbl[ *(dest_p  ) ] + blnd_tbl[ rgb.b ];
				*(dest_p+1) = brev_tbl[ *(dest_p+1) ] + blnd_tbl[ rgb.g ];
				*(dest_p+2) = brev_tbl[ *(dest_p+2) ] + blnd_tbl[ rgb.r ];

				rgb.r+= r4;	r5 += r3;	if( r5>w ) { r5-=w; rgb.r+=fr; }
				rgb.g+= g4;	g5 += g3;	if( g5>w ) { g5-=w; rgb.g+=fg; }
				rgb.b+= b4;	b5 += b3;	if( b5>w ) { b5-=w; rgb.b+=fb; }

				wcnt += h;
				dest_p+=fw;
			}
			wcnt -= w;
		}
	}
	return 1;
}



BOOL PRM_DrawGLine_F_Add( BMP_F *dest,	int x1, int y1, int r1, int g1, int b1,
										int x2, int y2, int r2, int g2, int b2 )
{
	int		x,y;
	char	*dest_p = (char*)(dest->buf + y1*dest->sx + x1);
	RGB32	rgb = { b1, g1, r1, 0 };
	int		r3 = r2-r1, g3 = g2-g1, b3 = b2-b1;
	int		r4, g4, b4, r5=0, g5=0, b5=0;
	int		fr = (r3<0)?-1:1, fg = (g3<0)?-1:1, fb=(b3<0)?-1:1;
	int		w=x2-x1,wcnt=0;
	int		h=y2-y1,hcnt=0;
	int		fw = (w <0)?-1:1, fh = ( h<0)?-1:1;
	int		dh = dest->sx*fh*3;
	char	*add_tbl = AddTable;
	w = w*fw+1;
	h = h*fh+1;
	fw *= 3;

	if( w<h ){
		r4 = r3   /h;	g4 = g3   /h;	b4 = b3   /h;
		r3 = r3*fr%h;	g3 = g3*fg%h;	b3 = b3*fb%h;
		for( x=0; x < w ; x++, dest_p+=fw ){
			while( hcnt < h ){
				*(dest_p  ) = add_tbl[ *(dest_p  ) + rgb.b ];
				*(dest_p+1) = add_tbl[ *(dest_p+1) + rgb.g ];
				*(dest_p+2) = add_tbl[ *(dest_p+2) + rgb.r ];
				
				rgb.r+= r4;	r5 += r3;	if( r5>h ) { r5-=h; rgb.r+=fr; }
				rgb.g+= g4;	g5 += g3;	if( g5>h ) { g5-=h; rgb.g+=fg; }
				rgb.b+= b4;	b5 += b3;	if( b5>h ) { b5-=h; rgb.b+=fb; }

				hcnt += w;
				dest_p += dh;
			}
			hcnt -= h;
		}
	}else{
		r4 = r3/w;		g4 = g3/w;		b4 = b3/w;
		r3 = r3*fr%w;	g3 = g3*fg%w;	b3 = b3*fb%w;
		for( y=0; y < h ; y++, dest_p += dh ){
			while( wcnt < w ){
				if(rgb.b>200){
					*(dest_p  ) = add_tbl[ *(dest_p  ) + rgb.b ];
				}
				*(dest_p  ) = add_tbl[ *(dest_p  ) + rgb.b ];
				*(dest_p+1) = add_tbl[ *(dest_p+1) + rgb.g ];
				*(dest_p+2) = add_tbl[ *(dest_p+2) + rgb.r ];

				rgb.r+= r4;	r5 += r3;	if( r5>w ) { r5-=w; rgb.r+=fr; }
				rgb.g+= g4;	g5 += g3;	if( g5>w ) { g5-=w; rgb.g+=fg; }
				rgb.b+= b4;	b5 += b3;	if( b5>w ) { b5-=w; rgb.b+=fb; }

				wcnt += h;
				dest_p+=fw;
			}
			wcnt -= w;
		}
	}
	return 1;
}



BOOL PRM_DrawGLine_F_Sub( BMP_F *dest,	int x1, int y1, int r1, int g1, int b1,
										int x2, int y2, int r2, int g2, int b2 )
{
	int		x,y;
	char	*dest_p = (char*)(dest->buf + y1*dest->sx + x1);
	RGB32	rgb = { b1, g1, r1, 0 };
	int		r3 = r2-r1, g3 = g2-g1, b3 = b2-b1;
	int		r4, g4, b4, r5=0, g5=0, b5=0;
	int		fr = (r3<0)?-1:1, fg = (g3<0)?-1:1, fb=(b3<0)?-1:1;
	int		w=x2-x1,wcnt=0;
	int		h=y2-y1,hcnt=0;
	int		fw = (w <0)?-1:1, fh = ( h<0)?-1:1;
	int		dh = dest->sx*fh*3;
	char	*sub_tbl = SubTable+255;

	w = w*fw+1;
	h = h*fh+1;

	fw *= 3;

	if( w<h ){
		r4 = r3   /h;	g4 = g3   /h;	b4 = b3   /h;
		r3 = r3*fr%h;	g3 = g3*fg%h;	b3 = b3*fb%h;
		for( x=0; x < w ; x++, dest_p+=fw ){
			while( hcnt < h ){
				*(dest_p  ) = sub_tbl[ *(dest_p  ) - rgb.b ];
				*(dest_p+1) = sub_tbl[ *(dest_p+1) - rgb.g ];
				*(dest_p+2) = sub_tbl[ *(dest_p+2) - rgb.r ];
				
				rgb.r+= r4;	r5 += r3;	if( r5>h ) { r5-=h; rgb.r+=fr; }
				rgb.g+= g4;	g5 += g3;	if( g5>h ) { g5-=h; rgb.g+=fg; }
				rgb.b+= b4;	b5 += b3;	if( b5>h ) { b5-=h; rgb.b+=fb; }

				hcnt += w;
				dest_p += dh;
			}
			hcnt -= h;
		}
	}else{
		r4 = r3/w;		g4 = g3/w;		b4 = b3/w;
		r3 = r3*fr%w;	g3 = g3*fg%w;	b3 = b3*fb%w;
		for( y=0; y < h ; y++, dest_p += dh ){
			while( wcnt < w ){
				*(dest_p  ) = sub_tbl[ *(dest_p  ) - rgb.b ];
				*(dest_p+1) = sub_tbl[ *(dest_p+1) - rgb.g ];
				*(dest_p+2) = sub_tbl[ *(dest_p+2) - rgb.r ];

				rgb.r+= r4;	r5 += r3;	if( r5>w ) { r5-=w; rgb.r+=fr; }
				rgb.g+= g4;	g5 += g3;	if( g5>w ) { g5-=w; rgb.g+=fg; }
				rgb.b+= b4;	b5 += b3;	if( b5>w ) { b5-=w; rgb.b+=fb; }

				wcnt += h;
				dest_p+=fw;
			}
			wcnt -= w;
		}
	}
	return 1;
}



BOOL PRM_DrawGLine_F_Mul( BMP_F *dest,	int x1, int y1, int r1, int g1, int b1,
										int x2, int y2, int r2, int g2, int b2 )
{
	int		x,y;
	char	*dest_p = (char*)(dest->buf + y1*dest->sx + x1);
	RGB32	rgb = { b1, g1, r1, 0 };
	int		r3 = r2-r1, g3 = g2-g1, b3 = b2-b1;
	int		r4, g4, b4, r5=0, g5=0, b5=0;
	int		fr = (r3<0)?-1:1, fg = (g3<0)?-1:1, fb=(b3<0)?-1:1;
	int		w=x2-x1,wcnt=0;
	int		h=y2-y1,hcnt=0;
	int		fw = (w <0)?-1:1, fh = ( h<0)?-1:1;
	int		dh = dest->sx*fh*3;
	w = w*fw+1;
	h = h*fh+1;

	fw *= 3;
	if( w<h ){
		r4 = r3   /h;	g4 = g3   /h;	b4 = b3   /h;
		r3 = r3*fr%h;	g3 = g3*fg%h;	b3 = b3*fb%h;
		for( x=0; x < w ; x++, dest_p+=fw ){
			while( hcnt < h ){
				*(dest_p  ) = BrightTable_TB[ rgb.b ][ *(dest_p  ) ];
				*(dest_p+1) = BrightTable_TB[ rgb.g ][ *(dest_p+1) ];
				*(dest_p+2) = BrightTable_TB[ rgb.r ][ *(dest_p+2) ];
				
				rgb.r+= r4;	r5 += r3;	if( r5>h ) { r5-=h; rgb.r+=fr; }
				rgb.g+= g4;	g5 += g3;	if( g5>h ) { g5-=h; rgb.g+=fg; }
				rgb.b+= b4;	b5 += b3;	if( b5>h ) { b5-=h; rgb.b+=fb; }

				hcnt += w;
				dest_p += dh;
			}
			hcnt -= h;
		}
	}else{
		r4 = r3/w;		g4 = g3/w;		b4 = b3/w;
		r3 = r3*fr%w;	g3 = g3*fg%w;	b3 = b3*fb%w;
		for( y=0; y < h ; y++, dest_p += dh ){
			while( wcnt < w ){
				*(dest_p  ) = BrightTable_TB[ rgb.b ][ *(dest_p  ) ];
				*(dest_p+1) = BrightTable_TB[ rgb.g ][ *(dest_p+1) ];
				*(dest_p+2) = BrightTable_TB[ rgb.r ][ *(dest_p+2) ];

				rgb.r+= r4;	r5 += r3;	if( r5>w ) { r5-=w; rgb.r+=fr; }
				rgb.g+= g4;	g5 += g3;	if( g5>w ) { g5-=w; rgb.g+=fg; }
				rgb.b+= b4;	b5 += b3;	if( b5>w ) { b5-=w; rgb.b+=fb; }

				wcnt += h;
				dest_p+=fw;
			}
			wcnt -= w;
		}
	}
	return 1;
}






BOOL PRM_DrawGLine_F( BMP_F *dest,	int x1, int y1, int r1, int g1, int b1,
									int x2, int y2, int r2, int g2, int b2,
									RECT *clip, DWORD wparam )
{
	BOOL		ret=TRUE;
	int			blnd  = HIWORD(wparam);
	int			param = LOWORD(wparam);

	if( dest->buf==NULL ) { DebugPrintf("•`‰æ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ[PRM_DrawGLine_F]\n"); return FALSE; }

	if( !ClipRectLineRGB( &x1, &y1, &r1, &g1, &b1, &x2, &y2, &r2, &g2, &b2, dest->sx, dest->sy, clip ) ) return 1;

	r1=LIM(r1,0,255);
	g1=LIM(g1,0,255);
	b1=LIM(b1,0,255);
	r2=LIM(r2,0,255);
	g2=LIM(g2,0,255);
	b2=LIM(b2,0,255);

	
	if( param==DRW_AMI2 || param== DRW_BLD2 ){
		if( blnd==BLD_0P   )return 1;			
		if( blnd==BLD_100P )param = DRW_NML;	
	}

	switch(param){
		default:
		case DRW_NML:	ret = PRM_DrawGLine_F_Std( dest, x1, y1, r1, g1, b1, x2, y2, r2, g2, b2 );			break;	
		case DRW_BLD2:	ret = PRM_DrawGLine_F_Bld( dest, x1, y1, r1, g1, b1, x2, y2, r2, g2, b2, blnd );	break;	
		case DRW_ADD:	ret = PRM_DrawGLine_F_Add( dest, x1, y1, r1, g1, b1, x2, y2, r2, g2, b2 );			break;	
		case DRW_SUB:	ret = PRM_DrawGLine_F_Sub( dest, x1, y1, r1, g1, b1, x2, y2, r2, g2, b2 );			break;	
		case DRW_MUL:	ret = PRM_DrawGLine_F_Mul( dest, x1, y1, r1, g1, b1, x2, y2, r2, g2, b2 );			break;	
		case DRW_AMI2:			break;	
		case DRW_NEG:			break;	
		case DRW_VIV2:			break;	
	}

	return ret;
}








BOOL PRM_DrawHanabi_F( BMP_F *dest,	int cx, int cy, int rate, RECT *clip )
{
	BOOL	ret=TRUE;

	if( dest->buf==NULL ) { DebugPrintf("•`‰æ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ[PRM_DrawHanabi_F]\n"); return FALSE; }
	if( rate==0 ) return 1;

	{
		RGB24	*dest_p;
		short	*bom_tbl;
		char	count=0;
		int		x,y,x2,y2;
		int		br = 255-max(rate-128,0)*2;

		
		
		bom_tbl = BomdTabl[rate/12];
		for( y=0; y < 32 ; y++ ){
			for( x=0 ; x < 32 ; x++, count+=x*y+1 ){
				x2 = bom_tbl[ count++ ]+cx+x;
				y2 = bom_tbl[ count   ]+cy+y;

				if( x2>=0 && y2>=0 && x2<dest->sx && y2<dest->sy ){
					dest_p = dest->buf + y2 * dest->sx + x2;
					dest_p->b = br;
				}
			}
		}
		
		bom_tbl = BomdTabl[rate*2/12];
		for( y=0; y < 32 ; y++ ){
			for( x=0 ; x < 32 ; x++, count+=x*y*2+2 ){
				x2 = bom_tbl[ count++ ]+cx+x;
				y2 = bom_tbl[ count   ]+cy+y;

				if( x2>=0 && y2>=0 && x2<dest->sx && y2<dest->sy ){
					dest_p = dest->buf + y2 * dest->sx + x2;
					dest_p->r = br;
				}
			}
		}
		
		bom_tbl = BomdTabl[rate*3/12];
		for( y=0; y < 32 ; y++ ){
			for( x=0 ; x < 32 ; x++, count+=x*y*3+3 ){
				x2 = bom_tbl[ count++ ]+cx+x;
				y2 = bom_tbl[ count   ]+cy+y;

				if( x2>=0 && y2>=0 && x2<dest->sx && y2<dest->sy ){
					dest_p = dest->buf + y2 * dest->sx + x2;
					dest_p->r = br;
					dest_p->b = br;
				}
			}
		}
		
		bom_tbl = BomdTabl[rate*4/12];
		for( y=0; y < 32 ; y++ ){
			for( x=0 ; x < 32 ; x++, count+=x*y*5+5 ){
				x2 = bom_tbl[ count++ ]+cx+x;
				y2 = bom_tbl[ count   ]+cy+y;

				if( x2>=0 && y2>=0 && x2<dest->sx && y2<dest->sy ){
					dest_p = dest->buf + y2 * dest->sx + x2;
					dest_p->g = br;
				}
			}
		}
		
		bom_tbl = BomdTabl[rate*5/12];
		for( y=0; y < 32 ; y++ ){
			for( x=0 ; x < 32 ; x++, count+=x*y*7+7 ){


				x2 = COS(count)*bom_tbl[count  ]/4096 + cx+x;
				y2 = SIN(count)*bom_tbl[count++]/4096 + cy+y;

				if( x2>=0 && y2>=0 && x2<dest->sx && y2<dest->sy ){
					dest_p = dest->buf + y2 * dest->sx + x2;
					dest_p->b = br;
					dest_p->g = br;
				}
			}
		}

		
		bom_tbl = BomdTabl[rate*6/12];
		for( y=0; y < 32 ; y++ ){
			for( x=0; x < 32 ; x++, count+=x*y*11+11 ){


				x2 = COS(count)*bom_tbl[count  ]/4096 + cx+x;
				y2 = SIN(count)*bom_tbl[count++]/4096 + cy+y;

				if( x2>=0 && y2>=0 && x2<dest->sx && y2<dest->sy ){
					dest_p = dest->buf + y2 * dest->sx + x2;
					dest_p->r = br;
					dest_p->g = br;
				}
			}
		}
	}
	return ret;
}
