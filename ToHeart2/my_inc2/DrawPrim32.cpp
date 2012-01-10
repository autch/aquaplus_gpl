
 
  
   
    
     
      

#include "mm_std.h"


#include "bmp.h"
#include "draw.h"
#include "drawprim.h"



extern RECT	ClipRectDef;	
extern int	DispWidth;
extern int	DispHeight;














BOOL PRM_DrawRect_T_Std( BMP_T *dest, int dx, int dy, int w, int h,
						int r, int g, int b )
{
	int		x,y;
	RGB32	*dest_p = dest->buf + dy * dest->sx + dx;
	int		drev = dest->sx-w;
	int		w2 = w/2;
	int		wa = w%2;
	RGB32	rgb_tbl[2] = {{ b, g, r, 0 },{ b, g, r, 0 }};

	if(r==g && g==b){
		if( w==dest->sx ){
			FillMemory( dest_p, w*h*4, r );
		}else{
			for( y=0; y < h ; y++, dest_p+=dest->sx )
				FillMemory( dest_p, w*4, r );
		}

	}else{
		for( y=0; y < h ; y++, dest_p+=drev ){
			for( x=0 ; x < w2 ; x++, dest_p+=2 ){
				*(double*)dest_p = *(double*)rgb_tbl;
			}
			if(wa){
				*dest_p = rgb_tbl[0];
				dest_p++;
			}
		}
	}
	return TRUE;
}



BOOL PRM_DrawRect_T_Bld( BMP_T *dest, int dx, int dy, int w, int h,
						int r, int g, int b,
						int blnd )
{
	int		x,y;
	RGB32	*dest_p = dest->buf + dy*dest->sx + dx;
	int		drev = dest->sx-w;
	int		w2 = w/2;
	int		wa = w%2;
	int		w3 = w2*2;
	RGB32	rgb_tbl[2] = { { b, g, r, 0 } , { b, g, r, 0 } };
	RGB32	*temp = rgb_tbl;
	DWORD	alpha2 = (256-blnd)*0x01010101;
	DWORD	alpha1 = blnd      *0x01010101;
	char	*blnd_tbl = BlendTable[blnd];
	char	*brev_tbl = BlendTable[256-blnd];

	if( blnd==BLD_0P )	return TRUE;	
	if( blnd==BLD_100P)	return PRM_DrawRect_T_Std( dest, dx, dy, w, h, r, g, b );	

	if( MMX_Flag ){
		for( y=0; y < h ; y++, dest_p+=drev ){
			if(w2){
				_asm {
					mov			ecx, w2
					mov			ebx, dest_p
					mov			esi, temp
					movq		mm2, [esi]

					pxor		mm3, mm3	
					movd		mm4, alpha1	
					movd		mm5, alpha2	
					punpcklbw	mm4, mm3	
					punpcklbw	mm5, mm3	

					
					movq		mm0, mm2 	
					movq		mm6, mm0	
					punpcklbw	mm0, mm3	
					punpckhbw	mm6, mm3	
					pmullw		mm0, mm4	
					pmullw		mm6, mm4	
					psrlw		mm0, 8		
					psrlw		mm6, 8		
					packuswb	mm0, mm6	

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
					loop		mmloopx1

					mov		dest_p, ebx
				}
				_asm	emms
			}
			if( wa ){
				dest_p->r = brev_tbl[ dest_p->r ] + blnd_tbl[ temp->r ];
				dest_p->g = brev_tbl[ dest_p->g ] + blnd_tbl[ temp->g ];
				dest_p->b = brev_tbl[ dest_p->b ] + blnd_tbl[ temp->b ];
				dest_p++;
			}
		}
	}else{
		int		i;
		char	btbl[256];
		char	gtbl[256];
		char	rtbl[256];

		for(i=0;i<256;i++){
			btbl[i] = brev_tbl[i] + blnd_tbl[b];
			gtbl[i] = brev_tbl[i] + blnd_tbl[g];
			rtbl[i] = brev_tbl[i] + blnd_tbl[r];
		}
		for( y=0; y < h ; y++, dest_p+=drev ){
			for( x=0 ; x < w ; x++, dest_p++ ){
				dest_p->b  = btbl[dest_p->b ];
				dest_p->g = gtbl[dest_p->g];
				dest_p->r   = rtbl[dest_p->r  ];
			}
		}
	}

	return TRUE;
}



BOOL PRM_DrawRect_T_Add( BMP_T *dest, int dx, int dy, int w, int h,
						int r, int g, int b )
{
	int		x,y;
	RGB32	*dest_p = dest->buf + dy*dest->sx + dx;
	int		drev = dest->sx-w;
	int		w2 = w/2;
	int		wa = w%2;
	int		w3 = w2*2;
	RGB32	rgb_tbl[2] = { { b, g, r, 0 } , { b, g, r, 0 } };
	RGB32	*temp = rgb_tbl;
	char	*add_tbl = AddTable;

	if( MMX_Flag ){
		for( y=0; y < h ; y++, dest_p+=dest->sx ){
			if( w2 ){
				_asm {
					mov			ecx, w2
					mov			edi, dest_p
					mov			esi, temp
					movq		mm0, [esi]
					
					loopx:
						movq		mm1, [edi]
						paddusb		mm1, mm0
						movq		[edi], mm1
						add			edi, 8
					loop	loopx
				}
				_asm	emms

			}
			if( wa ){
				(dest_p+w3)->b  = add_tbl[ (dest_p+w3)->b + b ];
				(dest_p+w3)->g = add_tbl[ (dest_p+w3)->g+ g ];
				(dest_p+w3)->r   = add_tbl[ (dest_p+w3)->r  + r ];
			}
		}
	}else{
		for( y=0; y < h ; y++, dest_p+=drev ){
			for( x=0 ; x < w ; x++, dest_p++ ){
				dest_p->b  = add_tbl[ dest_p->b +b ];
				dest_p->g = add_tbl[ dest_p->g+g ];
				dest_p->r   = add_tbl[ dest_p->r  +r ];
			}
		}
	}

	return TRUE;
}



BOOL PRM_DrawRect_T_Sub( BMP_T *dest, int dx, int dy, int w, int h,
						int r, int g, int b )
{
	int		x,y;
	RGB32	*dest_p = dest->buf + dy*dest->sx + dx;
	int		drev = dest->sx-w;
	int		w2 = w/2;
	int		wa = w%2;
	int		w3 = w2*2;
	RGB32	rgb_tbl[2] = { { b, g, r, 0 } , { b, g, r, 0 } };
	RGB32	*temp = rgb_tbl;
	char	*sub_tbl = SubTable+255;

	if( MMX_Flag ){
		for( y=0; y < h ; y++, dest_p+=dest->sx ){
			if( w2 ){
				_asm {
					mov			ecx, w2
					mov			edi, dest_p
					mov			esi, temp
					movq		mm0, [esi]

					loopx:
						movq		mm1, [edi]
						psubusb		mm1, mm0
						movq		[edi], mm1
						add			edi, 8
					loop	loopx
				}
				_asm	emms

			}
			if( wa ){
				(dest_p+w3)->r   = sub_tbl[ (dest_p+w3)->r   - r ];
				(dest_p+w3)->g = sub_tbl[ (dest_p+w3)->g - g ];
				(dest_p+w3)->b  = sub_tbl[ (dest_p+w3)->b  - b ];
			}
		}
	}else{
		for( y=0; y < h ; y++, dest_p+=drev ){
			for( x=0 ; x < w ; x++, dest_p++ ){
				dest_p->b  = sub_tbl[ dest_p->b -b ];
				dest_p->g = sub_tbl[ dest_p->g-g ];
				dest_p->r   = sub_tbl[ dest_p->r  -r ];
			}
		}
	}

	return TRUE;
}



BOOL PRM_DrawRect_T_Mul( BMP_T *dest, int dx, int dy, int w, int h,
						int r, int g, int b )
{
	int		x,y;
	RGB32	*dest_p = dest->buf + dy * dest->sx + dx;
	int		drev = dest->sx-w;
	char	*blend_tblr = BrightTable_TB[r];
	char	*blend_tblg = BrightTable_TB[g];
	char	*blend_tblb = BrightTable_TB[b];

	for( y=0; y < h ; y++, dest_p+=drev ){
		for( x=0 ; x < w ; x++, dest_p++ ){
			dest_p->b  = blend_tblb[ dest_p->b  ];
			dest_p->g = blend_tblg[ dest_p->g ];
			dest_p->r   = blend_tblr[ dest_p->r   ];
		}
	}
	return TRUE;
}



BOOL PRM_DrawRect_T_Ami( BMP_T *dest, int dx, int dy, int w, int h,
						int r, int g, int b, int mesh )
{
	int		x,y;
	RGB32	*dest_p = dest->buf + dy * dest->sx + dx;
	int		drev = dest->sx-w;
	RGB32	rgb_tbl = { b, g, r, 0 };
	char	*linemask[4], *linemask2;
	WORD	mesh_pt = MeshFadePatern[mesh/16];
	WORD	bg = b|(g<<8);

	
	linemask[0] = (char*)MeshTable[((mesh_pt >> 12) & 0x0f)];
	linemask[1] = (char*)MeshTable[((mesh_pt >>  8) & 0x0f)];
	linemask[2] = (char*)MeshTable[((mesh_pt >>  4) & 0x0f)];
	linemask[3] = (char*)MeshTable[((mesh_pt      ) & 0x0f)];

	for( y=0; y < h ; y++, dest_p+=drev ){
		linemask2 = linemask[y & 3];
		for( x=0 ; x < w ; x++, dest_p++ ){
			if( linemask2[x & 3] ){
				*dest_p  = rgb_tbl;
			}
		}
	}
	return TRUE;
}






BOOL PRM_DrawRect_T( BMP_T *dest, int dx, int dy, int w, int h,
					RECT *clip, int r, int g, int b,
					DWORD wparam )
{
	BOOL	ret=TRUE;
	int		blnd  = HIWORD(wparam);
	int		param = LOWORD(wparam);

	if( dest->buf==NULL ) { DebugPrintf("•`‰æ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ\n"); return FALSE; }



	if( !ClipRect2( &dx, &dy, &w, &h, dest->sx, dest->sy, clip ) ) return 1;

	
	if( param==DRW_AMI2 || param== DRW_BLD2 ){
		if( blnd==BLD_0P   )return 1;			
		if( blnd==BLD_100P )param = DRW_NML;	
	}

	switch(param){
		default:
		case DRW_NML:	PRM_DrawRect_T_Std( dest, dx, dy, w, h, r, g, b );			break;	
		case DRW_BLD2:	PRM_DrawRect_T_Bld( dest, dx, dy, w, h, r, g, b, blnd );	break;	
		case DRW_ADD:	PRM_DrawRect_T_Add( dest, dx, dy, w, h, r, g, b );			break;	
		case DRW_SUB:	PRM_DrawRect_T_Sub( dest, dx, dy, w, h, r, g, b );			break;	
		case DRW_MUL:	PRM_DrawRect_T_Mul( dest, dx, dy, w, h, r, g, b );			break;	
		case DRW_AMI2:	PRM_DrawRect_T_Ami( dest, dx, dy, w, h, r, g, b, blnd );	break;	
		case DRW_NEG:			break;	
		case DRW_VIV2:			break;	
	}
	return ret;
}








BOOL PRM_DrawFxLine_T_Std( RGB32 *dest_p, MIN_MAX *mi, MIN_MAX *ma, RECT *clip, RGB32 *rgb_tbl )
{
	int		x, w;
	int		w2, w3, wa;
	int		dx1,dx2;
	
	if( mi->x == DispWidth ) return TRUE;

	dx1	= max( clip->left,  mi->x );
	dx2 = min( clip->right, ma->x );
	dest_p += dx1;
	w  = dx2-dx1;
	w2 = w/2;
	wa = w%2;
	w3 = w2*2;

	if( MMX_Flag ){
		if(w2){
			_asm {
				mov		ecx, w2
				mov		edi, dest_p
				mov		esi, rgb_tbl
				movq	mm0, [esi]

				mmloopx:
					movq	[edi], mm0
					add		edi,8
				loop	mmloopx
			}
			_asm	emms
		}
		dest_p += w3;
		if( wa ){
			*dest_p = *rgb_tbl;
		}
	}else{
		for( x=0 ; x < w2 ; x++, dest_p+=2 ){
			*(double*)dest_p = *(double*)rgb_tbl;
		}
		if( wa ){ *dest_p = *rgb_tbl; }
	}
	return TRUE;
}



BOOL PRM_DrawFxLine_T_Bld( RGB32 *dest_p, MIN_MAX *mi, MIN_MAX *ma, RECT *clip, RGB32 *rgb_tbl, int blnd )
{
	int		x,w;
	int		dx1,dx2;
	int		w2, w3, wa;
	int		blnd_rev = 256-blnd;
	DWORD	alpha1 = blnd    *0x01010101;
	DWORD	alpha2 = blnd_rev*0x01010101;
	char	*blnd_tbl = BlendTable[blnd];
	char	*brev_tbl = BlendTable[blnd_rev];
	int		rb = blnd_tbl[ rgb_tbl->r   ];
	int		gb = blnd_tbl[ rgb_tbl->g ];
	int		bb = blnd_tbl[ rgb_tbl->b  ];

	if( blnd==BLD_0P )	return TRUE;	
	if( blnd==BLD_100P)	return PRM_DrawFxLine_T_Std( dest_p, mi, ma, clip, rgb_tbl );	
	if( mi->x == DispWidth )	return TRUE;	

	dx1	= max( clip->left,  mi->x );
	dx2 = min( clip->right, ma->x );
	w = dx2-dx1;
	w2 = w/2;
	wa = w%2;
	w3 = w2*2;
	dest_p += dx1;

	if( MMX_Flag ){
		if(w2){
			_asm {
				mov			ecx, w2
				mov			esi, rgb_tbl
				movq		mm2, [esi]
				mov			edi, dest_p

				pxor		mm3, mm3	
				movd		mm4, alpha1	
				movd		mm5, alpha2	
				punpcklbw	mm4, mm3	
				punpcklbw	mm5, mm3	

				mmloopx1:
					
					movq		mm0, mm2	
					movq		mm1, [edi]	
					
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
					movq		[edi], mm0	

					add			edi,8			
				loop	mmloopx1
			}
			_asm	emms
		}
		dest_p += w3;
		if( wa ){
			dest_p->r   = brev_tbl[ dest_p->r   ] + rb;
			dest_p->g = brev_tbl[ dest_p->g ] + gb;
			dest_p->b  = brev_tbl[ dest_p->b  ] + bb;
		}
	}else{
		for( x=0 ; x<w ; x++, dest_p++ ){
			dest_p->r   = brev_tbl[ dest_p->r   ] + rb;
			dest_p->g = brev_tbl[ dest_p->g ] + gb;
			dest_p->b  = brev_tbl[ dest_p->b  ] + bb;
		}
	}
	return TRUE;
}



BOOL PRM_DrawFxLine_T_Add( RGB32 *dest_p, MIN_MAX *mi, MIN_MAX *ma, RECT *clip, RGB32 *rgb_tbl, int param )
{
	int		x,w;
	int		dx1,dx2;
	int		w2, w3, wa;
	char	*bld_tbl=AddTable;

	if( mi->x == DispWidth )	return TRUE;	

	dx1	= max( clip->left,  mi->x );
	dx2 = min( clip->right, ma->x );
	w = dx2-dx1;
	w2 = w/2;
	wa = w%2;
	w3 = w2*2;

	dest_p += dx1;

	if( MMX_Flag ){
		if(w2){
			_asm {
				mov			ecx, w2
				mov			ebx, dest_p
				mov			esi, rgb_tbl
				movq		mm0, [esi]

				loopx:
					movq		mm1, [ebx]
					paddusb		mm1, mm0
					movq		[ebx], mm1
					add			ebx, 8
				loop	loopx
			}
		}
		dest_p += w3;
		if( wa ){
			_asm {
				mov			esi, rgb_tbl
				movq		mm0, [esi]
				mov			edi, dest_p
				movq		mm1, [edi]
				paddusb		mm1, mm0
				movd		[edi], mm1
			}
		}
		_asm	emms
	}else{
		if( param == DRW_ADD ){
			for( x=0 ; x<w ; x++, dest_p++ ){
				dest_p->r   = bld_tbl[ dest_p->r   + rgb_tbl->r  ];
				dest_p->g = bld_tbl[ dest_p->g + rgb_tbl->g];
				dest_p->b  = bld_tbl[ dest_p->b  + rgb_tbl->b ];
			}
		}else{
			for( x=0 ; x<w ; x++, dest_p++ ){
				dest_p->r   = dest_p->r   + rgb_tbl->r;
				dest_p->g = dest_p->g + rgb_tbl->g;
				dest_p->b  = dest_p->b  + rgb_tbl->b;
			}
		}
	}
	return TRUE;
}



BOOL PRM_DrawFxLine_T_Sub( RGB32 *dest_p, MIN_MAX *mi, MIN_MAX *ma, RECT *clip, RGB32 *rgb_tbl )
{
	int		x,w;
	int		dx1,dx2;
	int		w2, w3, wa;
	char	*bld_tbl=SubTable+255;

	if( mi->x == DispWidth )	return TRUE;	

	dx1	= max( clip->left,  mi->x );
	dx2 = min( clip->right, ma->x );
	w = dx2-dx1;
	w2 = w/2;
	wa = w%2;
	w3 = w2*2;

	dest_p += dx1;

	if( MMX_Flag ){
		if(w2){
			_asm {
				mov			ecx, w2
				mov			ebx, dest_p
				mov			esi, rgb_tbl
				movq		mm0, [esi]

				loopx:
					movq		mm1, [ebx]
					psubusb		mm1, mm0
					movq		[ebx], mm1
					add			ebx, 8
				loop	loopx
			}
		}
		dest_p += w3;
		if( wa ){
			_asm {
				mov			esi, rgb_tbl
				movd		mm0, [esi]
				mov			edi, dest_p
				movd		mm1, [edi]
				psubusb		mm1, mm0
				movd		[edi], mm1
			}
		}
		_asm	emms
	}else{
		for( x=0 ; x<w ; x++, dest_p++ ){
			dest_p->r   = bld_tbl[ dest_p->r   - rgb_tbl->r  ];
			dest_p->g = bld_tbl[ dest_p->g - rgb_tbl->g];
			dest_p->b  = bld_tbl[ dest_p->b  - rgb_tbl->b ];
		}
	}	return TRUE;
}



BOOL PRM_DrawFxLine_T_Mul( RGB32 *dest_p, MIN_MAX *mi, MIN_MAX *ma, RECT *clip, RGB32 *rgb_tbl )
{
	int		x,w;
	int		dx1,dx2;
	char	*blend_tblr = BrightTable_TB[ rgb_tbl->r   ];
	char	*blend_tblg = BrightTable_TB[ rgb_tbl->g ];
	char	*blend_tblb = BrightTable_TB[ rgb_tbl->b  ];

	if( mi->x == DispWidth )	return TRUE;	

	dx1	= max( clip->left,  mi->x );
	dx2 = min( clip->right, ma->x );
	w = dx2-dx1;
	dest_p += dx1;

	for( x=0 ; x < w ; x++, dest_p++ ){
		dest_p->b  = blend_tblb[ dest_p->b  ];
		dest_p->g = blend_tblg[ dest_p->g ];
		dest_p->r   = blend_tblr[ dest_p->r   ];
	}
	return TRUE;
}






BOOL PRM_DrawF3_T( BMP_T *dest,	int x1, int y1,
								int x2, int y2,
								int x3, int y3,
								RECT *clip, int r, int g, int b,
								DWORD wparam )
{
	BOOL		ret=TRUE;
	int			blnd  = HIWORD(wparam);
	int			param = LOWORD(wparam);
	RGB32		*dest_p = dest->buf;
	int			drev = dest->sx;
	MIN_MAX		*mi = (MIN_MAX*)GAlloc(DispHeight*sizeof(MIN_MAX));
	MIN_MAX		*ma = (MIN_MAX*)GAlloc(DispHeight*sizeof(MIN_MAX));
	int			min_y, max_y;
	int			y;
	RGB32		rgb_tbl[2] = { { b, g, r, 0 },{ b, g, r, 0 } };

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
	DrawMinMaxTable( x1, y1-min_y, x2, y2-min_y, clip, mi, ma );	
	DrawMinMaxTable( x2, y2-min_y, x3, y3-min_y, clip, mi, ma );	
	DrawMinMaxTable( x3, y3-min_y, x1, y1-min_y, clip, mi, ma );	

	dest_p = dest->buf + drev*min_y;
	switch(param){
		default:
		case DRW_NML:	
			for( y = min_y ; y <= max_y ; y++, dest_p+=drev )
				PRM_DrawFxLine_T_Std( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl );
			break;
		case DRW_BLD2:	
			for( y = min_y ; y <= max_y ; y++, dest_p+=drev )
				PRM_DrawFxLine_T_Bld( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl, blnd );
			break;
		case DRW_ADD:	
			for( y = min_y ; y <= max_y ; y++, dest_p+=drev )
				PRM_DrawFxLine_T_Add( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl, param );
			break;
		case DRW_SUB:	
			for( y = min_y ; y <= max_y ; y++, dest_p+=drev )
				PRM_DrawFxLine_T_Sub( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl );
			break;
		case DRW_MUL:	
			for( y = min_y ; y <= max_y ; y++, dest_p+=drev )
				PRM_DrawFxLine_T_Mul( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl );
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





BOOL PRM_DrawF4_T( BMP_T *dest,	int x1, int y1,
								int x2, int y2,
								int x3, int y3,
								int x4, int y4,
								RECT *clip, int r, int g, int b,
								DWORD wparam )
{
	BOOL		ret=TRUE;
	int			blnd  = HIWORD(wparam);
	int			param = LOWORD(wparam);
	RGB32		*dest_p = dest->buf;
	int			drev = dest->sx;
	MIN_MAX		*mi = (MIN_MAX*)GAlloc(DispHeight*sizeof(MIN_MAX));
	MIN_MAX		*ma = (MIN_MAX*)GAlloc(DispHeight*sizeof(MIN_MAX));
	int			min_y, max_y;
	int			y;
	RGB32		rgb_tbl[2] = { { b, g, r, 0 },{ b, g, r, 0 } };

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
	DrawMinMaxTable( x1, y1-min_y, x2, y2-min_y, clip, mi, ma );	
	DrawMinMaxTable( x2, y2-min_y, x4, y4-min_y, clip, mi, ma );	
	DrawMinMaxTable( x4, y4-min_y, x3, y3-min_y, clip, mi, ma );	
	DrawMinMaxTable( x3, y3-min_y, x1, y1-min_y, clip, mi, ma );	

	dest_p = dest->buf + drev*min_y;
	switch(param){
		default:
		case DRW_NML:	
			for( y = min_y ; y <= max_y ; y++, dest_p+=drev )
				PRM_DrawFxLine_T_Std( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl );
			break;
		case DRW_BLD2:	
			for( y = min_y ; y <= max_y ; y++, dest_p+=drev )
				PRM_DrawFxLine_T_Bld( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl, blnd );
			break;
		case DRW_ADD:	
			for( y = min_y ; y <= max_y ; y++, dest_p+=drev )
				PRM_DrawFxLine_T_Add( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl, param );
			break;
		case DRW_SUB:	
			for( y = min_y ; y <= max_y ; y++, dest_p+=drev )
				PRM_DrawFxLine_T_Sub( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl );
			break;
		case DRW_MUL:	
			for( y = min_y ; y <= max_y ; y++, dest_p+=drev )
				PRM_DrawFxLine_T_Mul( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl );
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










BOOL PRM_DrawY3_T( BMP_T *dest,	int x1, int y1,
								int x2, int y2,
								int x3, int y3, RECT *clip,
								int sy, int sr, int sg, int sb,
								int ey, int er, int eg, int eb,
								DWORD wparam )
{
	BOOL		ret=TRUE;
	int			blnd  = HIWORD(wparam);
	int			param = LOWORD(wparam);
	RGB32		*dest_p = dest->buf;
	int			drev = dest->sx;
	MIN_MAX		*mi = (MIN_MAX*)GAlloc(DispHeight*sizeof(MIN_MAX));
	MIN_MAX		*ma = (MIN_MAX*)GAlloc(DispHeight*sizeof(MIN_MAX));
	int			min_y, max_y;
	int			y, ay;
	int			esy = ey-sy;
	int			esr = er-sr;
	int			esg = eg-sg;
	int			esb = eb-sb;
	RGB32		rgb_tbl[2];

	if( sy==ey || (sr==er && sg==eg && sb==eb) )
		return PRM_DrawF3_T( dest, x1, y1, x2, y2, x3, y3, clip, sr, sg, sb, wparam );

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
	DrawMinMaxTable( x1, y1, x2, y2, clip, mi, ma );	
	DrawMinMaxTable( x2, y2, x3, y3, clip, mi, ma );	
	DrawMinMaxTable( x3, y3, x1, y1, clip, mi, ma );	

	dest_p = dest->buf + drev*min_y;
	for( y = clip->top ; y < clip->bottom ; y++, dest_p+=drev ){
		ay = y-sy;
		rgb_tbl[0].r = rgb_tbl[1].r = (char)STD_Limit( esr*ay/esy + sr, sr, er );	
		rgb_tbl[0].g = rgb_tbl[1].g = (char)STD_Limit( esg*ay/esy + sg, sg, eg );	
		rgb_tbl[0].b = rgb_tbl[1].b = (char)STD_Limit( esb*ay/esy + sb, sb, eb );	
		switch(param){
			default:
			case DRW_NML:	PRM_DrawFxLine_T_Std( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl );			break;	
			case DRW_BLD2:	PRM_DrawFxLine_T_Bld( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl, blnd );	break;	
			case DRW_ADD:	PRM_DrawFxLine_T_Add( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl, param );	break;
			case DRW_SUB:	PRM_DrawFxLine_T_Sub( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl );			break;	
			case DRW_MUL:	PRM_DrawFxLine_T_Mul( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl );			break;	
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






BOOL PRM_DrawY4_T( BMP_T *dest,	int x1, int y1,
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
	RGB32		*dest_p = dest->buf;
	int			drev = dest->sx;
	MIN_MAX		*mi = (MIN_MAX*)GAlloc(DispHeight*sizeof(MIN_MAX));
	MIN_MAX		*ma = (MIN_MAX*)GAlloc(DispHeight*sizeof(MIN_MAX));
	int			min_y, max_y;
	int			y, ay;
	int			esy = ey-sy;
	int			esr = er-sr;
	int			esg = eg-sg;
	int			esb = eb-sb;
	RGB32		rgb_tbl[2];

	if( sy==ey || (sr==er && sg==eg && sb==eb) )
		return PRM_DrawF4_T( dest, x1, y1, x2, y2, x3, y3, x4, y4, clip, sr, sg, sb, wparam );

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
	DrawMinMaxTable( x1, y1-min_y, x2, y2-min_y, clip, mi, ma );	
	DrawMinMaxTable( x2, y2-min_y, x4, y4-min_y, clip, mi, ma );	
	DrawMinMaxTable( x4, y4-min_y, x3, y3-min_y, clip, mi, ma );	
	DrawMinMaxTable( x3, y3-min_y, x1, y1-min_y, clip, mi, ma );	

	dest_p = dest->buf + drev*min_y;
	for( y = min_y ; y <= max_y ; y++, dest_p+=drev ){
		ay = y-sy;
		rgb_tbl[0].r   = rgb_tbl[1].r   = (char)STD_Limit( esr*ay/esy + sr, sr, er );	
		rgb_tbl[0].g = rgb_tbl[1].g = (char)STD_Limit( esg*ay/esy + sg, sg, eg );	
		rgb_tbl[0].b  = rgb_tbl[1].b  = (char)STD_Limit( esb*ay/esy + sb, sb, eb );	
		switch(param){
			default:
			case DRW_NML:	PRM_DrawFxLine_T_Std( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl );			break;	
			case DRW_BLD2:	PRM_DrawFxLine_T_Bld( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl, blnd );	break;	
			case DRW_ADD:	PRM_DrawFxLine_T_Add( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl, param );	break;
			case DRW_SUB:	PRM_DrawFxLine_T_Sub( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl );			break;	
			case DRW_MUL:	PRM_DrawFxLine_T_Mul( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl );			break;	
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





BOOL PRM_DrawGxLine_T_Std( RGB32 *dest_p,
							int r,  int g,  int b,  int pr, int pg, int pb,
							int dr, int dg, int db, int fr, int fg, int fb, int  w, int ww)
{
	int		rr=0,gg=0,bb=0;
	int		x;
	WORD	rgb1[4];
	WORD	rgb2[4];
	WORD	rgb3[4];
	DWORD	temp = 0x10101010;

	if( MMX_Flag ){
		if( w ){
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
				mov			ecx, w
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

					add			ebx, 4		
				loop	loopx
			}
			_asm	emms
		}
	}else{
		RGB32	rgb = {b,g,r,0};
		for( x=0; x < w ; x++, dest_p++ ){
			*dest_p  = rgb;

			rgb.r   +=pr;
			rgb.g +=pg;
			rgb.b  +=pb;
			rr+=dr;	gg+=dg;	bb+=db;
			if( rr >= ww ){ rr -= ww; rgb.r  +=fr; }
			if( gg >= ww ){ gg -= ww; rgb.g+=fg; }
			if( bb >= ww ){ bb -= ww; rgb.b +=fb; }
		}
	}
	return TRUE;
}



BOOL PRM_DrawGxLine_T_Bld( RGB32 *dest_p,
							int r,  int g,  int b,  int pr, int pg, int pb,
							int dr, int dg, int db, int fr, int fg, int fb, int  w, int ww, int blnd )
{
	WORD	rgb1[4];
	WORD	rgb2[4];
	WORD	rgb3[4];
	DWORD	temp = 0x10101010;
	int		brev = 256-blnd;
	DWORD	alpha1 = brev * 0x01010101;

	if( MMX_Flag ){
		if( w ){
			if( fr==-1 ) { temp |= 0x00010000;	temp &= ~0x00100000; }
			if( fg==-1 ) { temp |= 0x00000100;	temp &= ~0x00001000; }
			if( fb==-1 ) { temp |= 0x00000001;	temp &= ~0x00000010; }

			rgb1[0] = b*256;
			rgb1[1] = g*256;
			rgb1[2] = r*256;
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
				mov			ecx, w
				mov			ebx, dest_p
				
				pxor		mm0, mm0	
				movq		mm5, mm0	
				movd		mm6, temp
				punpcklbw	mm6, mm0
				psllw		mm6, 8		

				movq		mm1, [rgb1]	
				movq		mm2, [rgb2]	
				movq		mm3, [rgb3]	

				movd		mm7, alpha1	
				punpcklbw	mm7, mm0	

				loopx:
					paddb		mm1, mm2	
					paddw		mm0, mm3	

					movq		mm4, mm0	

					PCMPEQB		mm4, mm6	
					por			mm4, mm0	

					psllw		mm0, 8		
					psrlw		mm0, 8		

					paddb		mm1, mm4	

					movd		mm4, [ebx]	
					punpcklbw	mm4, mm5	
					pmullw		mm4, mm7	
					paddusw		mm4, mm1	
					psrlw		mm4, 8		

					packuswb	mm4, mm0	
					movd		[ebx], mm4	

					add			ebx, 4		
				loop	loopx
			}
			_asm	emms
		}
	}else{
		int		rr=0,gg=0,bb=0;
		int		x;
		char	*blnd_tbl = BlendTable[blnd];
		char	*brev_tbl = BlendTable[brev];

		for( x=0; x < w ; x++, dest_p++ ){
			dest_p->b  = brev_tbl[ dest_p->b  ] + blnd_tbl[ b ];
			dest_p->g = brev_tbl[ dest_p->g ] + blnd_tbl[ g ];
			dest_p->r   = brev_tbl[ dest_p->r   ] + blnd_tbl[ r ];
			
			r +=pr;	g +=pg;	b +=pb;
			rr+=dr;	gg+=dg;	bb+=db;
			if( rr >= ww ){ rr -= ww; r+=fr; }
			if( gg >= ww ){ gg -= ww; g+=fg; }
			if( bb >= ww ){ bb -= ww; b+=fb; }
		}
	}
	return TRUE;
}



BOOL PRM_DrawGxLine_T_Add( RGB32 *dest_p,
							int r,  int g,  int b,  int pr, int pg, int pb,
							int dr, int dg, int db, int fr, int fg, int fb, int  w, int ww )
{
	WORD	rgb1[4];
	WORD	rgb2[4];
	WORD	rgb3[4];
	DWORD	temp = 0x10101010;


	if( MMX_Flag ){
		if( w ){
			if( fr==-1 ) { temp |= 0x00010000;	temp &= ~0x00100000; }
			if( fg==-1 ) { temp |= 0x00000100;	temp &= ~0x00001000; }
			if( fb==-1 ) { temp |= 0x00000001;	temp &= ~0x00000010; }
			rgb1[0] = b*256;
			rgb1[1] = g*256;
			rgb1[2] = r*256;
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
				mov			ecx, w
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

					movd		mm5, [ebx]	
					paddusb		mm5, mm4
					movd		[ebx], mm5	

					add			ebx, 4		
				loop	loopx
			}
			_asm	emms
		}
	}else{
		int		rr=0,gg=0,bb=0;
		int		x;
		char	*add_tbl = AddTable;

		for( x=0; x < w ; x++, dest_p++ ){
			dest_p->b  = add_tbl[ dest_p->b  + b ];
			dest_p->g = add_tbl[ dest_p->g + g ];
			dest_p->r   = add_tbl[ dest_p->r   + r ];

			r +=pr;	g +=pg;	b +=pb;
			rr+=dr;	gg+=dg;	bb+=db;
			if( rr >= ww ){ rr -= ww; r+=fr; }
			if( gg >= ww ){ gg -= ww; g+=fg; }
			if( bb >= ww ){ bb -= ww; b+=fb; }
		}
	}
	return TRUE;
}



BOOL PRM_DrawGxLine_T_Sub( RGB32 *dest_p,
							int r,  int g,  int b,  int pr, int pg, int pb,
							int dr, int dg, int db, int fr, int fg, int fb, int  w, int ww )
{
	WORD	rgb1[4];
	WORD	rgb2[4];
	WORD	rgb3[4];
	DWORD	temp = 0x10101010;

	if( MMX_Flag ){
		if( w ){
			if( fr==-1 ) { temp |= 0x00010000;	temp &= ~0x00100000; }
			if( fg==-1 ) { temp |= 0x00000100;	temp &= ~0x00001000; }
			if( fb==-1 ) { temp |= 0x00000001;	temp &= ~0x00000010; }
			rgb1[0] = b*256;
			rgb1[1] = g*256;
			rgb1[2] = r*256;
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
				mov			ecx, w
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

					movd		mm5, [ebx]	
					psubusb		mm5, mm4
					movd		[ebx], mm5	

					add			ebx, 4		
				loop	loopx
			}
			_asm	emms
		}
	}else{
		int		rr=0,gg=0,bb=0;
		int		x;
		char	*sub_tbl = SubTable+255;

		for( x=0; x < w ; x++, dest_p++ ){
			dest_p->b  = sub_tbl[ dest_p->b  - b ];
			dest_p->g = sub_tbl[ dest_p->g - g ];
			dest_p->r   = sub_tbl[ dest_p->r   - r ];

			r +=pr;	g +=pg;	b +=pb;
			rr+=dr;	gg+=dg;	bb+=db;
			if( rr >= ww ){ rr -= ww; r+=fr; }
			if( gg >= ww ){ gg -= ww; g+=fg; }
			if( bb >= ww ){ bb -= ww; b+=fb; }
		}
	}
	return TRUE;
}



BOOL PRM_DrawGxLine_T_Mul( RGB32 *dest_p,
							int r,  int g,  int b,  int pr, int pg, int pb,
							int dr, int dg, int db, int fr, int fg, int fb, int  w, int ww )
{
	int		rr=0,gg=0,bb=0;
	int		x;

	for( x=0; x < w ; x++, dest_p++ ){
		dest_p->b  = BrightTable_TB[ b ][ dest_p->b  ];
		dest_p->g = BrightTable_TB[ g ][ dest_p->g ];
		dest_p->r   = BrightTable_TB[ r ][ dest_p->r   ];

		r +=pr;	g +=pg;	b +=pb;
		rr+=dr;	gg+=dg;	bb+=db;
		if( rr >= ww ){ rr -= ww; r+=fr; }
		if( gg >= ww ){ gg -= ww; g+=fg; }
		if( bb >= ww ){ bb -= ww; b+=fb; }
	}
	return TRUE;
}



BOOL PRM_DrawGxLine_T( RGB32 *dest_p, MIN_MAX *mi, MIN_MAX *ma, RECT *clip, DWORD wparam )
{
	BOOL	ret=FALSE;
	int		blnd  = HIWORD(wparam);
	int		param = LOWORD(wparam);
	int		dx1,dx2,w;
	int		xx, ww = ma->x - mi->x;
	int		r, g, b;
	int		dr,dg,db;
	int		fr,fg,fb;
	int		pr,pg,pb;
	int		mir,mig,mib;
	int		mar,mag,mab;

	if( mi->x == DispWidth )	return 1;
	if( ww==0 )			return 1;

	if( MMX_Flag && param==DRW_BLD2 ){
		mir = mi->rgb.r*blnd/256;	mig = mi->rgb.g*blnd/256;	mib = mi->rgb.b*blnd/256;
		mar = ma->rgb.r*blnd/256;	mag = ma->rgb.g*blnd/256;	mab = ma->rgb.b*blnd/256;
	}else{
		mir = mi->rgb.r;	mig = mi->rgb.g;	mib = mi->rgb.b;
		mar = ma->rgb.r;	mag = ma->rgb.g;	mab = ma->rgb.b;
	}
	dx1 = max( mi->x, clip->left  );
	dx2 = min( ma->x, clip->right );

	w  = dx2-dx1;
	xx = dx1 - mi->x;
	dest_p += dx1;

	r = mar - mir;
	g = mag - mig;
	b = mab - mib;

	if(r<0)	{	dr = (-r)%ww;	fr=-1;	}
	else	{	dr =   r %ww;	fr= 1;	}
	if(g<0)	{	dg = (-g)%ww;	fg=-1;	}
	else	{	dg =   g %ww;	fg= 1;	}
	if(b<0)	{	db = (-b)%ww;	fb=-1;	}
	else	{	db =   b %ww;	fb= 1;	}

	pr = r*xx/ww + mir;
	pg = g*xx/ww + mig;
	pb = b*xx/ww + mib;

	r = r/ww;
	g = g/ww;
	b = b/ww;
	switch(param){
		default:
		case DRW_NML:	ret = PRM_DrawGxLine_T_Std( dest_p, pr,pg,pb, r, g, b, dr, dg, db, fr, fg, fb, w, ww );			break;	
		case DRW_BLD2:	ret = PRM_DrawGxLine_T_Bld( dest_p, pr,pg,pb, r, g, b, dr, dg, db, fr, fg, fb, w, ww, blnd );	break;	
		case DRW_ADD:	ret = PRM_DrawGxLine_T_Add( dest_p, pr,pg,pb, r, g, b, dr, dg, db, fr, fg, fb, w, ww );			break;	
		case DRW_SUB:	ret = PRM_DrawGxLine_T_Sub( dest_p, pr,pg,pb, r, g, b, dr, dg, db, fr, fg, fb, w, ww );			break;	
		case DRW_MUL:	ret = PRM_DrawGxLine_T_Mul( dest_p, pr,pg,pb, r, g, b, dr, dg, db, fr, fg, fb, w, ww );			break;	
		case DRW_AMI2:			break;	
		case DRW_NEG:			break;	
		case DRW_VIV2:			break;	
	}
	return ret;
}





BOOL PRM_DrawG3_T( BMP_T *dest,	int x1, int y1, int r1, int g1, int b1,
								int x2, int y2, int r2, int g2, int b2,
								int x3, int y3, int r3, int g3, int b3,
								RECT *clip, DWORD wparam )
{
	BOOL		ret=TRUE;
	int			blnd  = HIWORD(wparam);
	int			param = LOWORD(wparam);
	RGB32		*dest_p = dest->buf;
	int			drev = dest->sx;
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

	dest_p = dest->buf + drev*min_y;
	for( y = min_y ; y <= max_y ; y++, dest_p+=drev )
		PRM_DrawGxLine_T( dest_p, &mi[y-min_y], &ma[y-min_y], clip, wparam );
end:
	GFree(mi);
	GFree(ma);
	return ret;
}





BOOL PRM_DrawG4_T( BMP_T *dest,	int x1, int y1, int r1, int g1, int b1,
								int x2, int y2, int r2, int g2, int b2,
								int x3, int y3, int r3, int g3, int b3,
								int x4, int y4, int r4, int g4, int b4,
								RECT *clip, DWORD wparam )
{
	BOOL		ret=TRUE;
	int			blnd  = HIWORD(wparam);
	int			param = LOWORD(wparam);
	RGB32		*dest_p = dest->buf;
	int			drev = dest->sx;
	MIN_MAX		*mi = (MIN_MAX*)GAlloc(DispHeight*sizeof(MIN_MAX));
	MIN_MAX		*ma = (MIN_MAX*)GAlloc(DispHeight*sizeof(MIN_MAX));
	int			min_y, max_y;
	int			blnd_rev = 256-blnd;
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

	dest_p = dest->buf + drev*min_y;
	for( y = min_y ; y <= max_y ; y++, dest_p+=drev )
		PRM_DrawGxLine_T( dest_p, &mi[y-min_y], &ma[y-min_y], clip, wparam );

end:
	GFree(mi);
	GFree(ma);
	return ret;
}



BOOL PRM_DrawFLine_T_Std( BMP_T *dest,	int x1, int y1,
										int x2, int y2,
										int r, int g, int b )
{
	RGB32	rgb = { b, g, r, 0 };
	RGB32	*dest_p = dest->buf + y1*dest->sx + x1;
	int		x,y;
	int		w=x2-x1, wcnt=0;
	int		h=y2-y1, hcnt=0;
	int		fw = (w<0)?-1:1;
	int		fh = (h<0)?-1:1;
	int		dh = dest->sx*fh;
	w = w*fw+1;
	h = h*fh+1;

	if( w==1 ){
		for( y=0; y < h ; y++, dest_p+=dh ) *dest_p = rgb;
	}else if( h==1 ){
		for( x=0; x < w ; x++, dest_p+=fw ) *dest_p = rgb;
	}else if( w<h ){
		for( x=0; x < w ; x++, dest_p+=fw ){
			while( hcnt < h ){
				*dest_p = rgb;
				hcnt += w;
				dest_p += dh;
			}
			hcnt -= h;
		}
	}else{
		for( y=0; y < h ; y++, dest_p += dh ){
			while( wcnt < w ){
				*dest_p = rgb;
				wcnt += h;
				dest_p+=fw;
			}
			wcnt -= w;
		}
	}

	return 1;
}



BOOL PRM_DrawFLine_T_Bld( BMP_T *dest,	int x1, int y1,
										int x2, int y2,
										int r, int g, int b, int blnd )
{
	int		blnd_rev = 256-blnd;
	RGB32	*dest_p = dest->buf + y1*dest->sx + x1;
	int		x,y;
	int		w=x2-x1, wcnt=0;
	int		h=y2-y1, hcnt=0;
	int		fw = (w<0)?-1:1;
	int		fh = (h<0)?-1:1;
	int		dh = dest->sx*fh;
	char	*brev_tbl = BlendTable[256-blnd];
	int		r2 = (r*blnd)>>8, g2 = (g*blnd)>>8, b2 = (b*blnd)>>8;

	w = w*fw+1;
	h = h*fh+1;

	if( w<h ){
		for( x=0; x < w ; x++, dest_p+=fw ){
			while( hcnt < h ){
				dest_p->r  = brev_tbl[ dest_p->r   ] + r;
				dest_p->g= brev_tbl[ dest_p->g ] + g;
				dest_p->b = brev_tbl[ dest_p->b  ] + b;

				hcnt += w;
				dest_p += dh;
			}
			hcnt -= h;
		}
	}else{
		for( y=0; y < h ; y++, dest_p += dh ){
			while( wcnt < w ){
				dest_p->r  = brev_tbl[ dest_p->r   ] + r;
				dest_p->g= brev_tbl[ dest_p->g ] + g;
				dest_p->b = brev_tbl[ dest_p->b  ] + b;

				wcnt += h;
				dest_p+=fw;
			}
			wcnt -= w;
		}
	}
	return 1;
}





BOOL PRM_DrawFLine_T_Add( BMP_T *dest,	int x1, int y1,
										int x2, int y2,
										int r, int g, int b )
{
	int		x,y;
	RGB32	*dest_p = dest->buf + y1*dest->sx + x1;
	int		w=x2-x1, wcnt=0;
	int		h=y2-y1, hcnt=0;
	int		fw = (w<0)?-1:1;
	int		fh = (h<0)?-1:1;
	int		dh = dest->sx*fh;
	char	rgb[4]={b,g,r,0};
	char	*add_tbl = AddTable;

	w = w*fw+1;
	h = h*fh+1;

	if( w<h ){
		for( x=0; x < w ; x++, dest_p+=fw ){
			while( hcnt < h ){
				dest_p->r  = add_tbl[ dest_p->r   + r ];
				dest_p->g= add_tbl[ dest_p->g + g ];
				dest_p->b = add_tbl[ dest_p->b  + b ];

				hcnt += w;
				dest_p += dh;
			}
			hcnt -= h;
		}
	}else{
		for( y=0; y < h ; y++, dest_p += dh ){
			while( wcnt < w ){
				dest_p->r  = add_tbl[ dest_p->r   + r ];
				dest_p->g= add_tbl[ dest_p->g + g ];
				dest_p->b = add_tbl[ dest_p->b  + b ];

				wcnt += h;
				dest_p+=fw;
			}
			wcnt -= w;
		}
	}
	return 1;
}



BOOL PRM_DrawFLine_T_Sub( BMP_T *dest,	int x1, int y1,
										int x2, int y2,
										int r, int g, int b )
{
	int		x,y;
	RGB32	*dest_p = dest->buf + y1*dest->sx + x1;
	int		w=x2-x1, wcnt=0;
	int		h=y2-y1, hcnt=0;
	int		fw = (w<0)?-1:1;
	int		fh = (h<0)?-1:1;
	int		dh = dest->sx*fh;
	char	rgb[4]={b,g,r,0};
	char	*sub_tbl = SubTable+255;

	w = w*fw+1;
	h = h*fh+1;

	if( w<h ){
		for( x=0; x < w ; x++, dest_p+=fw ){
			while( hcnt < h ){
				dest_p->r  = sub_tbl[ dest_p->r   - r ];
				dest_p->g= sub_tbl[ dest_p->g - g ];
				dest_p->b = sub_tbl[ dest_p->b  - b ];

				hcnt += w;
				dest_p += dh;
			}
			hcnt -= h;
		}
	}else{
		for( y=0; y < h ; y++, dest_p += dh ){
			while( wcnt < w ){
				dest_p->r  = sub_tbl[ dest_p->r   - r ];
				dest_p->g= sub_tbl[ dest_p->g - g ];
				dest_p->b = sub_tbl[ dest_p->b  - b ];

				wcnt += h;
				dest_p+=fw;
			}
			wcnt -= w;
		}
	}
	return 1;
}



BOOL PRM_DrawFLine_T_Mul( BMP_T *dest,	int x1, int y1,
										int x2, int y2,
										int r, int g, int b )
{
	RGB32	*dest_p = dest->buf + y1*dest->sx + x1;
	int		r2 = r-0x80;
	int		g2 = g-0x80;
	int		b2 = b-0x80;
	int		x,y;
	int		w=x2-x1, wcnt=0;
	int		h=y2-y1, hcnt=0;
	int		fw = (w<0)?-1:1;
	int		fh = (h<0)?-1:1;
	int		dh = dest->sx*fh;
	w = w*fw+1;
	h = h*fh+1;

	if( w<h ){
		for( x=0; x < w ; x++, dest_p+=fw ){
			while( hcnt < h ){
				dest_p->r   = BrightTable_TB[ r ][ dest_p->r   ];
				dest_p->g = BrightTable_TB[ g ][ dest_p->g ];
				dest_p->b  = BrightTable_TB[ b ][ dest_p->b  ];

				hcnt += w;
				dest_p += dh;
			}
			hcnt -= h;
		}
	}else{
		for( y=0; y < h ; y++, dest_p += dh ){
			while( wcnt < w ){
				dest_p->r   = BrightTable_TB[ r ][ dest_p->r   ];
				dest_p->g = BrightTable_TB[ g ][ dest_p->g ];
				dest_p->b  = BrightTable_TB[ b ][ dest_p->b  ];
				wcnt += h;
				dest_p+=fw;
			}
			wcnt -= w;
		}
	}

	return 1;
}





BOOL PRM_DrawFLine_T( BMP_T *dest,	int x1, int y1,
									int x2, int y2,
									RECT *clip, int r, int g, int b,
									DWORD wparam )
{
	BOOL		ret=TRUE;
	int			blnd  = HIWORD(wparam);
	int			param = LOWORD(wparam);

	if( dest->buf==NULL ) { DebugPrintf("•`‰æ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ\n"); return FALSE; }


	if( !ClipRectLine( &x1, &y1, &x2, &y2, dest->sx, dest->sy, clip ) ) return 1;

	
	if( param==DRW_AMI2 || param== DRW_BLD2 ){
		if( blnd==BLD_0P   )return 1;			
		if( blnd==BLD_100P )param = DRW_NML;	
	}
	
	switch(param){
		default:
		case DRW_NML:	ret = PRM_DrawFLine_T_Std( dest, x1, y1, x2, y2, r, g, b );			break;	
		case DRW_BLD2:	ret = PRM_DrawFLine_T_Bld( dest, x1, y1, x2, y2, r, g, b, blnd );	break;	
		case DRW_ADD:	ret = PRM_DrawFLine_T_Add( dest, x1, y1, x2, y2, r, g, b );			break;	
		case DRW_SUB:	ret = PRM_DrawFLine_T_Sub( dest, x1, y1, x2, y2, r, g, b );			break;	
		case DRW_MUL:	ret = PRM_DrawFLine_T_Mul( dest, x1, y1, x2, y2, r, g, b );			break;	
		case DRW_AMI2:			break;	
		case DRW_NEG:			break;	
		case DRW_VIV2:			break;	
	}
	return ret;
}




BOOL PRM_DrawGLine_T_Std( BMP_T *dest,	int x1, int y1, int r1, int g1, int b1,
										int x2, int y2, int r2, int g2, int b2 )
{
	int		x,y;
	RGB32	*dest_p = dest->buf + y1*dest->sx + x1;
	RGB32	rgb = { b1, g1, r1, 0 };
	int		r3 = r2-r1, g3 = g2-g1, b3 = b2-b1;
	int		r4, g4, b4, r5=0, g5=0, b5=0;
	int		fr = (r3<0)?-1:1, fg = (g3<0)?-1:1, fb=(b3<0)?-1:1;
	int		w=x2-x1,wcnt=0;
	int		h=y2-y1,hcnt=0;
	int		fw = (w <0)?-1:1, fh = ( h<0)?-1:1;
	int		dh = dest->sx*fh;
	w = w*fw+1;
	h = h*fh+1;

	if( w<h ){
		r4 = r3   /h;	g4 = g3   /h;	b4 = b3   /h;
		r3 = r3*fr%h;	g3 = g3*fg%h;	b3 = b3*fb%h;
		for( x=0; x < w ; x++, dest_p+=fw ){
			while( hcnt < h ){
				*dest_p = rgb;
				
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
				*dest_p = rgb;

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



BOOL PRM_DrawGLine_T_Bld( BMP_T *dest,	int x1, int y1, int r1, int g1, int b1,
										int x2, int y2, int r2, int g2, int b2, int blnd )
{
	int		x,y;
	int		blnd_rev = 256-blnd;
	RGB32	*dest_p = dest->buf + y1*dest->sx + x1;
	RGB32	rgb = { b1, g1, r1, 0 };
	int		r3 = r2-r1, g3 = g2-g1, b3 = b2-b1;
	int		r4, g4, b4, r5=0, g5=0, b5=0;
	int		fr = (r3<0)?-1:1, fg = (g3<0)?-1:1, fb=(b3<0)?-1:1;
	int		w=x2-x1,wcnt=0;
	int		h=y2-y1,hcnt=0;
	int		fw = (w <0)?-1:1, fh = ( h<0)?-1:1;
	int		dh = dest->sx*fh;
	char	*brev_tbl = BlendTable[256-blnd];
	char	*blnd_tbl = BlendTable[blnd];

	w = w*fw+1;
	h = h*fh+1;

	if( w<h ){
		r4 = r3   /h;	g4 = g3   /h;	b4 = b3   /h;
		r3 = r3*fr%h;	g3 = g3*fg%h;	b3 = b3*fb%h;
		for( x=0; x < w ; x++, dest_p+=fw ){
			while( hcnt < h ){
				dest_p->r  = brev_tbl[ dest_p->r   ] + blnd_tbl[ rgb.r   ];
				dest_p->g= brev_tbl[ dest_p->g ] + blnd_tbl[ rgb.g ];
				dest_p->b = brev_tbl[ dest_p->b  ] + blnd_tbl[ rgb.b  ];

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
				dest_p->r  = brev_tbl[ dest_p->r   ] + blnd_tbl[ rgb.r   ];
				dest_p->g= brev_tbl[ dest_p->g ] + blnd_tbl[ rgb.g ];
				dest_p->b = brev_tbl[ dest_p->b  ] + blnd_tbl[ rgb.b  ];

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




BOOL PRM_DrawGLine_T_Add( BMP_T *dest,	int x1, int y1, int r1, int g1, int b1,
										int x2, int y2, int r2, int g2, int b2 )
{
	int		x,y;
	RGB32	*dest_p = dest->buf + y1*dest->sx + x1;
	RGB32	rgb = { b1, g1, r1, 0 };
	int		r3 = r2-r1, g3 = g2-g1, b3 = b2-b1;
	int		r4, g4, b4, r5=0, g5=0, b5=0;
	int		fr = (r3<0)?-1:1, fg = (g3<0)?-1:1, fb=(b3<0)?-1:1;
	int		w=x2-x1,wcnt=0;
	int		h=y2-y1,hcnt=0;
	int		fw = (w <0)?-1:1, fh = ( h<0)?-1:1;
	int		dh = dest->sx*fh;
	char	*add_tbl = AddTable;

	w = w*fw+1;
	h = h*fh+1;

	if( w<h ){
		r4 = r3   /h;	g4 = g3   /h;	b4 = b3   /h;
		r3 = r3*fr%h;	g3 = g3*fg%h;	b3 = b3*fb%h;
		for( x=0; x < w ; x++, dest_p+=fw ){
			while( hcnt < h ){
				dest_p->b  = add_tbl[ dest_p->b  + rgb.b  ];
				dest_p->g = add_tbl[ dest_p->g + rgb.g ];
				dest_p->r   = add_tbl[ dest_p->r   + rgb.r   ];
			
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
				dest_p->b  = add_tbl[ dest_p->b  + rgb.b  ];
				dest_p->g = add_tbl[ dest_p->g + rgb.g ];
				dest_p->r   = add_tbl[ dest_p->r   + rgb.r   ];

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




BOOL PRM_DrawGLine_T_Sub( BMP_T *dest,	int x1, int y1, int r1, int g1, int b1,
										int x2, int y2, int r2, int g2, int b2 )
{
	int		x,y;
	RGB32	*dest_p = dest->buf + y1*dest->sx + x1;
	RGB32	rgb = { b1, g1, r1, 0 };
	int		r3 = r2-r1, g3 = g2-g1, b3 = b2-b1;
	int		r4, g4, b4, r5=0, g5=0, b5=0;
	int		fr = (r3<0)?-1:1, fg = (g3<0)?-1:1, fb=(b3<0)?-1:1;
	int		w=x2-x1,wcnt=0;
	int		h=y2-y1,hcnt=0;
	int		fw = (w <0)?-1:1, fh = ( h<0)?-1:1;
	int		dh = dest->sx*fh;
	char	*sub_tbl = SubTable+255;

	w = w*fw+1;
	h = h*fh+1;

	if( w<h ){
		r4 = r3   /h;	g4 = g3   /h;	b4 = b3   /h;
		r3 = r3*fr%h;	g3 = g3*fg%h;	b3 = b3*fb%h;
		for( x=0; x < w ; x++, dest_p+=fw ){
			while( hcnt < h ){
				dest_p->b  = sub_tbl[ dest_p->b  - rgb.b  ];
				dest_p->g = sub_tbl[ dest_p->g - rgb.g ];
				dest_p->r   = sub_tbl[ dest_p->r   - rgb.r   ];
			
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
				dest_p->b  = sub_tbl[ dest_p->b  - rgb.b  ];
				dest_p->g = sub_tbl[ dest_p->g - rgb.g ];
				dest_p->r   = sub_tbl[ dest_p->r   - rgb.r   ];

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





BOOL PRM_DrawGLine_T_Mul( BMP_T *dest,	int x1, int y1, int r1, int g1, int b1,
										int x2, int y2, int r2, int g2, int b2 )
{
	int		x,y;
	RGB32	*dest_p = dest->buf + y1*dest->sx + x1;
	RGB32	rgb = { b1, g1, r1, 0 };
	int		r3 = r2-r1, g3 = g2-g1, b3 = b2-b1;
	int		r4, g4, b4, r5=0, g5=0, b5=0;
	int		fr = (r3<0)?-1:1, fg = (g3<0)?-1:1, fb=(b3<0)?-1:1;
	int		w=x2-x1,wcnt=0;
	int		h=y2-y1,hcnt=0;
	int		fw = (w <0)?-1:1, fh = ( h<0)?-1:1;
	int		dh = dest->sx*fh;
	w = w*fw+1;
	h = h*fh+1;

	if( w<h ){
		r4 = r3   /h;	g4 = g3   /h;	b4 = b3   /h;
		r3 = r3*fr%h;	g3 = g3*fg%h;	b3 = b3*fb%h;
		for( x=0; x < w ; x++, dest_p+=fw ){
			while( hcnt < h ){
				dest_p->r   = BrightTable_TB[rgb.r  ][dest_p->r  ];
				dest_p->g = BrightTable_TB[rgb.g][dest_p->g];
				dest_p->b  = BrightTable_TB[rgb.b ][dest_p->b ];
			
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
				dest_p->r   = BrightTable_TB[rgb.r  ][dest_p->r  ];
				dest_p->g = BrightTable_TB[rgb.g][dest_p->g];
				dest_p->b  = BrightTable_TB[rgb.b ][dest_p->b ];

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






BOOL PRM_DrawGLine_T( BMP_T *dest,	int x1, int y1, int r1, int g1, int b1,
									int x2, int y2, int r2, int g2, int b2,
									RECT *clip, DWORD wparam )
{
	BOOL		ret=TRUE;
	int			blnd  = HIWORD(wparam);
	int			param = LOWORD(wparam);

	if( dest->buf==NULL ) { DebugPrintf("•`‰æ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ\n"); return FALSE; }


	if( !ClipRectLineRGB( &x1, &y1, &r1, &g1, &b1, &x2, &y2, &r2, &g2, &b2, dest->sx, dest->sy, clip ) ) return 1;

	
	if( param==DRW_AMI2 || param== DRW_BLD2 ){
		if( blnd==BLD_0P   )return 1;			
		if( blnd==BLD_100P )param = DRW_NML;	
	}

	switch(param){
		default:
		case DRW_NML:	ret = PRM_DrawGLine_T_Std( dest, x1, y1, r1, g1, b1, x2, y2, r2, g2, b2 );			break;	
		case DRW_BLD2:	ret = PRM_DrawGLine_T_Bld( dest, x1, y1, r1, g1, b1, x2, y2, r2, g2, b2, blnd );	break;	
		case DRW_ADD:	ret = PRM_DrawGLine_T_Add( dest, x1, y1, r1, g1, b1, x2, y2, r2, g2, b2 );			break;	
		case DRW_SUB:	ret = PRM_DrawGLine_T_Sub( dest, x1, y1, r1, g1, b1, x2, y2, r2, g2, b2 );			break;	
		case DRW_MUL:	ret = PRM_DrawGLine_T_Mul( dest, x1, y1, r1, g1, b1, x2, y2, r2, g2, b2 );			break;	
		case DRW_AMI2:			break;	
		case DRW_NEG:			break;	
		case DRW_VIV2:			break;	
	}

	return ret;
}

