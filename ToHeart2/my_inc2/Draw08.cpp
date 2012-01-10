
 
  
   
    
     
      
#include "mm_std.h"


#include "bmp.h"
#include "draw.h"
#include "palette.h"







int DRW_GetStartPointerSrc( int sx, int sy, int sw, int sh, int w, int rparam  )
{
	int	ret;
	
	if( rparam&REV_H ){
		ret = (sy + sh-1) * w;
	}else{
		ret = sy * w;
	}
	if( rparam&REV_W ){
		ret += sx + sw -1;
	}else{
		ret += sx;
	}

	return ret;
}



BOOL DRW_DrawBMP_BB_Std( BMP_BT *dest, int dest_x, int dest_y,
							BMP_BT *src,  int src_x,  int src_y,
							int w,int h, int nuki, RECT *clip,
							int r, int g, int b, int rparam )
{
	int		x, y;
	int		inc_x=((rparam&REV_W)?-1:1), inc_y=((rparam&REV_H)?-1:1);
	int		drev = dest->sx-w;
	int		srev = src->sx*inc_y - w * inc_x;
	char	*dest_p = dest->buf + dest_y*dest->sx + dest_x;
	char	*src_p  = src->buf  + DRW_GetStartPointerSrc( src_x, src_y, w, h, src->sx, rparam  );
	char	bright_index[256];
	int		i,rr,gg,bb;

	
	if( dest->buf==NULL ) { DebugPrintf("•`‰æ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ\n"); return FALSE; }
	if( src->buf==NULL  ) { DebugPrintf("‰æ‘œ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ\n"); return FALSE; }

	if( r==BRT_NML && g==BRT_NML && b==BRT_NML ){
		
		if( nuki==-1 ){	
			if(!rparam){
				if( w==dest->sx && w==src->sx ){
					CopyMemory( dest_p, src_p, w*h );
				}else{
					for( y=0; y < h ; y++, dest_p+=dest->sx, src_p+=src->sx )
						CopyMemory( dest_p, src_p, w );
				}
			}else{
				for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
					for( x=0 ; x < w ; x++, dest_p++, src_p+=inc_x ){
						*dest_p = *src_p;
					}
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=inc_x ){
					
					if( *src_p != nuki ){
						*dest_p = *src_p;
					}
				}
			}
		}
	}else{
		for( i=0; i<256 ; i++ ){
			if( r<BRT_NML )	rr = (GlobalPal[i].r   * r)>>10;
			else			rr = ((((0xff-GlobalPal[i].r  ) * (r-BRT_NML))>>7)+GlobalPal[i].r  )>>3;
			
			if( g<BRT_NML )	gg = (GlobalPal[i].g * g)>>10;
			else			gg = ((((0xff-GlobalPal[i].g) * (g-BRT_NML))>>7)+GlobalPal[i].g)>>3;

			if( b<BRT_NML )	bb = (GlobalPal[i].b  * b)>>10;
			else			bb = ((((0xff-GlobalPal[i].b ) * (b-BRT_NML))>>7)+GlobalPal[i].b )>>3;

			bright_index[i] = PaletteIndex[(rr<<10)|(gg<<5)|bb];
		}

		
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=inc_x ){
					*dest_p = bright_index[ *src_p ];
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p+=inc_x ){
					
					if( *src_p != nuki ){
						*dest_p = bright_index[ *src_p ];
					}
				}
			}
		}
	}

	return TRUE;
}



BOOL DRW_DrawBMP_BB_Pal(	BMP_BT *dest, int dest_x, int dest_y,
							BMP_BT *src,  int src_x,  int src_y,
							int w,int h, int nuki, RECT *clip,
							int r, int g, int b )
{
	int		x, y;
	int		drev = dest->sx-w;
	int		srev = src->sx -w;
	char	*dest_p = dest->buf + dest_y*dest->sx + dest_x;
	char	*src_p  = src->buf  + src_y *src->sx  + src_x;
	RGB32	*spal  = src->pal;

	
	if( dest->buf==NULL ) { DebugPrintf("•`‰æ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ\n"); return FALSE; }
	if( src->buf==NULL  ) { DebugPrintf("‰æ‘œ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ\n"); return FALSE; }

	if( r==BRT_NML && g==BRT_NML && b==BRT_NML ){
		
		if( nuki==-1 ){	
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
					*dest_p = PaletteIndex[   (spal[ *src_p ].r  >>3<<10)
											| (spal[ *src_p ].g>>3<<5)
											| (spal[ *src_p ].b >>3) ];
				}
			}
		}else{			
			for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
				for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
					*dest_p = PaletteIndex[   (spal[ *src_p ].r  >>3<<10)
											| (spal[ *src_p ].g>>3<<5)
											| (spal[ *src_p ].b >>3) ];
				}
			}
		}
	}else{
	}

	return TRUE;
}




BOOL DRW_DrawBMP_BB_Bld( BMP_BT *dest, int dest_x, int dest_y,
						 BMP_BT *src,  int src_x,  int src_y,
						 int w,int h, int nuki, RECT *clip,
						 int r, int g, int b, int blnd )
{
	int		x, y;
	int		w2 = w/4;
	int		drev = dest->sx-w;
	int		srev = src->sx -w;
	char	*dest_p = dest->buf + dest_y*dest->sx + dest_x;
	char	*src_p  = src->buf  + src_y *src->sx  + src_x;
	char	(*bl_index)[256] = &BlendIndex[blnd/8][0];

	
	
	if( blnd==BLD_0P )		return 1;
	
	if( blnd==BLD_100P )	return DRW_DrawBMP_BB_Std( dest, dest_x, dest_y, src, src_x, src_y, w, h, nuki, clip, r,g,b,0 );
	
	
	if( dest->buf==NULL ) { DebugPrintf("•`‰æ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ\n"); return FALSE; }
	if( src->buf==NULL  ) { DebugPrintf("‰æ‘œ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ\n"); return FALSE; }

	
	if( nuki==-1 ){	
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			for( x=0 ; x < w2 ; x++, dest_p+=4, src_p+=4 ){
				*(DWORD*)dest_p = (bl_index[ *(dest_p  ) ][ *(src_p  ) ])
								| (bl_index[ *(dest_p+1) ][ *(src_p+1) ]<<8)
								| (bl_index[ *(dest_p+2) ][ *(src_p+2) ]<<16)
								| (bl_index[ *(dest_p+3) ][ *(src_p+3) ]<<24);
			}
		}
	}else{			
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
				
				if( *src_p != nuki ){
					*dest_p = bl_index[ *dest_p ][ *src_p ];
				}
			}
		}
	}
	return TRUE;
}



BOOL DRW_DrawBMP_BB_Add( BMP_BT *dest, int dest_x, int dest_y,
						 BMP_BT *src,  int src_x,  int src_y,
						 int w,int h, int nuki, RECT *clip,
						 int r, int g, int b )
{
	int		x, y;
	int		w2 = w/4;
	int		drev = dest->sx-w;
	int		srev = src->sx -w;
	char	*dest_p = dest->buf + dest_y*dest->sx + dest_x;
	char	*src_p  = src->buf  + src_y *src->sx  + src_x;
	char	(*add_index)[256] = AddIndex;

	
	if( dest->buf==NULL ) { DebugPrintf("•`‰æ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ\n"); return FALSE; }
	if( src->buf==NULL  ) { DebugPrintf("‰æ‘œ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ\n"); return FALSE; }
	
	
	if( nuki==-1 ){	
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			for( x=0 ; x < w2 ; x++, dest_p+=4, src_p+=4 ){
				*(DWORD*)dest_p = (add_index[ *(dest_p  ) ][ *(src_p  ) ])
								| (add_index[ *(dest_p+1) ][ *(src_p+1) ]<<8)
								| (add_index[ *(dest_p+2) ][ *(src_p+2) ]<<16)
								| (add_index[ *(dest_p+3) ][ *(src_p+3) ]<<24);
			}
		}
	}else{			
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
				
				if( *src_p != nuki ){
					*dest_p = add_index[ *dest_p ][ *src_p ];
				}
			}
		}
	}
	return TRUE;
}



BOOL DRW_DrawBMP_BB_Sub( BMP_BT *dest, int dest_x, int dest_y,
						 BMP_BT *src,  int src_x,  int src_y,
						 int w,int h, int nuki, RECT *clip,
						 int r, int g, int b )
{
	int		x, y;
	int		w2 = w/4;
	int		drev = dest->sx-w;
	int		srev = src->sx -w;
	char	*dest_p = dest->buf + dest_y*dest->sx + dest_x;
	char	*src_p  = src->buf  + src_y *src->sx  + src_x;
	char	(*sub_index)[256] = SubIndex;


	
	if( dest->buf==NULL ) { DebugPrintf("•`‰æ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ\n"); return FALSE; }
	if( src->buf==NULL  ) { DebugPrintf("‰æ‘œ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ\n"); return FALSE; }
	
	
	if( nuki==-1 ){	
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			for( x=0 ; x < w2 ; x++, dest_p+=4, src_p+=4 ){
				*(DWORD*)dest_p = (sub_index[ *(dest_p  ) ][ *(src_p  ) ])
								| (sub_index[ *(dest_p+1) ][ *(src_p+1) ]<<8)
								| (sub_index[ *(dest_p+2) ][ *(src_p+2) ]<<16)
								| (sub_index[ *(dest_p+3) ][ *(src_p+3) ]<<24);
			}
		}
	}else{			
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
				
				if( *src_p != nuki ){
					*dest_p = sub_index[ *dest_p ][ *src_p ];
				}
			}
		}
	}
	return TRUE;
}



BOOL DRW_DrawBMP_BB_Mul( BMP_BT *dest, int dest_x, int dest_y,
						 BMP_BT *src,  int src_x,  int src_y,
						 int w,int h, int nuki, RECT *clip,
						 int r, int g, int b )
{
	int		x, y;
	int		w2 = w/4;
	int		drev = dest->sx-w;
	int		srev = src->sx -w;
	char	*dest_p = dest->buf + dest_y*dest->sx + dest_x;
	char	*src_p  = src->buf  + src_y *src->sx  + src_x;
	char	(*mul_index)[256] = MulIndex;

	
	if( dest->buf==NULL ) { DebugPrintf("•`‰æ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ\n"); return FALSE; }
	if( src->buf==NULL  ) { DebugPrintf("‰æ‘œ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ\n"); return FALSE; }
	
	
	if( nuki==-1 ){	
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			for( x=0 ; x < w2 ; x++, dest_p+=4, src_p+=4 ){
				*(DWORD*)dest_p = (mul_index[ *(dest_p  ) ][ *(src_p  ) ])
								| (mul_index[ *(dest_p+1) ][ *(src_p+1) ]<<8)
								| (mul_index[ *(dest_p+2) ][ *(src_p+2) ]<<16)
								| (mul_index[ *(dest_p+3) ][ *(src_p+3) ]<<24);
			}
		}
	}else{			
		for( y=0; y < h ; y++, dest_p+=drev, src_p+=srev ){
			for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
				
				if( *src_p != nuki ){
					*dest_p = mul_index[ *dest_p ][ *src_p ];
				}
			}
		}
	}
	return TRUE;
}





BOOL DRW_DrawBMP_BB( BMP_BT *dest, int dest_x, int dest_y,
					 BMP_BT *src,  int src_x,  int src_y,
					 int w,int h, int nuki, RECT *clip,
					 int r, int g, int b, DWORD wparam, DWORD rparam )
{
	BOOL	ret = TRUE;
	int		blnd  = HIWORD(wparam);
	int		param = LOWORD(wparam);

	
	if( clip!=(void*)-1 ){
		if( !ClipRect(  &dest_x, &dest_y, dest->sx, dest->sy,
						&src_x,  &src_y,  src->sx,  src->sy,  &w, &h, clip ) ) return 0;
	}
	for(int i=0;i<256;i++){
		dest->pal[i] = src->pal[i];
	}

	switch(param){
		default:
		case DRW_NML:	DRW_DrawBMP_BB_Std( dest, dest_x, dest_y, src, src_x, src_y, w, h, nuki, clip, r,g,b, rparam );			break;	
		case DRW_PAL:	DRW_DrawBMP_BB_Pal( dest, dest_x, dest_y, src, src_x, src_y, w, h, nuki, clip, r,g,b );			break;	
		case DRW_BLD2:	DRW_DrawBMP_BB_Bld( dest, dest_x, dest_y, src, src_x, src_y, w, h, nuki, clip, r,g,b, blnd );	break;	
		case DRW_ADD:	DRW_DrawBMP_BB_Add( dest, dest_x, dest_y, src, src_x, src_y, w, h, nuki, clip, r,g,b );			break;	
		case DRW_SUB:	DRW_DrawBMP_BB_Sub( dest, dest_x, dest_y, src, src_x, src_y, w, h, nuki, clip, r,g,b );			break;	
		case DRW_MUL:	DRW_DrawBMP_BB_Mul( dest, dest_x, dest_y, src, src_x, src_y, w, h, nuki, clip, r,g,b );			break;	
	}

	return TRUE;
}



BOOL DRW_DrawZOOM_BB_Std(	BMP_BT *dest, int dx, int dy, int dw, int dh, int dw2, int dh2,
							BMP_BT *src,  int sx, int sy, int sw, int sh, int sw2, int sh2,
							int	xxx, int yyy,
							int nuki, int r, int g, int b )
{
	int			x, y;
	char		*src_p  = src->buf  + sy*src->sx  + sx, *sss;
	char		*dest_p = dest->buf + dy*dest->sx + dx;
	int			drev = dest->sx-dw;
	int			px,py,wx,wy,xx,yy;

	if( sw*2==dw && sh*2==dh ){	
		if( nuki==-1 || nuki==-3 ){			

		}
	}
	px = sw2/dw2;
	wx = sw2%dw2;
	py = sh2/dh2 * src->sx;
	wy = sh2%dh2;

	if( r==0x80 && g==0x80 && b==0x80 ){
		if( nuki==-1 ){
			yy=yyy;
			for( y=0; y<dh; y++, dest_p+=drev ){
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ){
					*dest_p = *sss;

					sss+=px;
					xx += wx;
					if( xx>=dw2 ) { xx -= dw2; sss++; }
				}
				src_p+=py;
				yy += wy;
				if( yy>=dh2 ) { yy -= dh2; src_p+=src->sx; }
			}
		}else{
			yy=yyy;
			for( y=0; y<dh; y++, dest_p+=drev ) {
				sss = src_p;
				xx=xxx;
				for( x=0; x<dw; x++, dest_p++ ) {
					if( ((*(long*)sss)&0xffffff)!=nuki ){
						*(WORD*)dest_p = *(WORD*)sss;
						*(dest_p+2)    = *(sss+2);
					}
					sss+=px;
					xx += wx;
					if( xx>=dw2 ) { xx -= dw2; sss++; }
				}

				src_p+=py;
				yy += wy;
				if( yy>=dh2 ) { yy -= dh2; src_p+=src->sx; }
			}
		}
	}else{
	}
	return TRUE;
}







BOOL DRW_DrawZOOM_BB(	BMP_BT *dest, int dx, int dy, int dw, int dh,
						BMP_BT *src,  int sx, int sy, int sw, int sh,
						int nuki, RECT *clip,
						int r, int g, int b, DWORD wparam )
{
	BOOL	ret = TRUE;
	int		blnd  = HIWORD(wparam);
	int		param = LOWORD(wparam);
	int		xx, yy, dw2=dw, dh2=dh, sw2=sw, sh2=sh;

	
	if( dest->buf==NULL ) { DebugPrintf("•`‰æ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ\n"); return FALSE; }
	if( src->buf==NULL  ) {	DebugPrintf("‰æ‘œ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ\n"); return FALSE; }

	
	if( clip!=(void*)-1 ){
		if( !ClipRectZ( &sx, &sy, &sw, &sh,
						&dx, &dy, &dw, &dh, &xx, &yy, dest->sx, dest->sy, src->sx, src->sy, clip ) )	return 1;
	}
	
	if( param==DRW_AMI2 || param==DRW_BLD2 ){
		if( blnd==BLD_0P   ) return 1;	
		if( blnd==BLD_100P ){
			param = DRW_NML;	
		}
	}
	switch(param){
		default:
		case DRW_NML:	DRW_DrawZOOM_BB_Std( dest, dx, dy, dw, dh, dw2, dh2, src, sx, sy, sw, sh, sw2, sh2, xx, yy, nuki, r, g, b );		break;	





	}
	return TRUE;
}
