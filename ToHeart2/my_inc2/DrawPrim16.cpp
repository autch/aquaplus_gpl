
 
  
   
    
     
      

#include "mm_std.h"


#include "bmp.h"
#include "draw.h"
#include "drawprim.h"



extern RECT	ClipRectDef;	
extern int	DispWidth;
extern int	DispHeight;














BOOL PRM_DrawRect_H_Std( BMP_H *dest, int dx, int dy, int w, int h,
						int r, int g, int b )
{
	int		x,y;
	int		w2 = w/4;
	int		wa = w%4;
	WORD	*dest_p = dest->buf + dy * dest->sx + dx;
	int		drev = dest->sx-w;
	WORD	rgb = BMP_RGB_FH(r,g,b);
	WORD	rgb_tbl[4] = { rgb, rgb, rgb, rgb };

	for( y=0; y < h ; y++, dest_p+=drev ){
		for( x=0 ; x < w2 ; x++, dest_p+=4 ){
			*(double*)dest_p = *(double*)rgb_tbl;
		}
		for( x=0 ; x < wa ; x++, dest_p++ ){
			*dest_p = rgb;
		}
	}

	return TRUE;
}



BOOL PRM_DrawRect_H_Bld( BMP_H *dest, int dx, int dy, int w, int h,
						int r, int g, int b, int blnd )
{
	int		x,y;
	int		w2 = w/2;
	int		wa = w%2;
	WORD	*dest_p = dest->buf + dy*dest->sx + dx;
	int		drev = dest->sx-w;
	int		brev = 32-blnd/8;
	WORD	rgb = BMP_RGB_FH( (r*blnd)>>8, (g*blnd)>>8, (b*blnd)>>8 );
	WORD	rgb_tbl[4] = { rgb, rgb, rgb, rgb };

	if( blnd==0  ) return TRUE;
	if( blnd==256) return PRM_DrawRect_H_Std( dest, dx, dy, w, h, r,g, b );
	
	for( y=0; y < h ; y++, dest_p+=drev ){
		for( x=0 ; x < w2 ; x++, dest_p+=2 ){
			*(DWORD*)dest_p = ( ((((*(DWORD*)dest_p&rgb16.mask_rbg)>>5)*brev )&rgb16.mask_rbg)
							 |  ((((*(DWORD*)dest_p&rgb16.mask_grb)*brev)>>5 )&rgb16.mask_grb) ) + *(DWORD*)rgb_tbl;
		}
		if(wa){
			*dest_p = (WORD)( ((((*dest_p&rgb16.mask_rb)*brev)>>5 )&rgb16.mask_rb)
							| ((((*dest_p&rgb16.mask_g )>>5)*brev )&rgb16.mask_g ) ) + rgb;
			dest_p++;
		}
	}
	return TRUE;
}



BOOL PRM_DrawRect_H_Add( BMP_H *dest, int dx, int dy, int w, int h,
						int r, int g, int b )
{
	int		x,y;
	WORD	*dest_p = dest->buf + dy * dest->sx + dx;
	int		drev = dest->sx-w;
	WORD	rgb = BMP_RGB_FH(r,g,b);
	DWORD	dcol,scol;
	DWORD	mask1, mask2=0;

	mask2 |= 0x00000001 << (rgb16.pos_r+5);
	mask2 |= 0x00000001 << (rgb16.pos_r);
	mask2 |= 0x00000001 << (rgb16.pos_g);
	mask1 = ~mask2;
	rgb = (WORD)(rgb&mask1);

	
	for( y=0; y < h ; y++, dest_p+=drev ){
		for( x=0 ; x < w ; x++, dest_p++ ){
			dcol = (*dest_p&mask1)+rgb;
			scol = dcol & mask2;
			*dest_p = (WORD)((scol-(scol>>5)) | dcol);
		}
	}
	return TRUE;
}



BOOL PRM_DrawRect_H_Sub( BMP_H *dest, int dx, int dy, int w, int h,
						int r, int g, int b )
{
	int		x,y;
	WORD	*dest_p = dest->buf + dy * dest->sx + dx;
	int		drev = dest->sx-w;
	WORD	rgb = BMP_RGB_FH(r,g,b);
	WORD	wr = rgb&rgb16.mask_r;
	WORD	wg = rgb&rgb16.mask_g;
	WORD	wb = rgb&rgb16.mask_b;
	int		rr,gg,bb;

	for( y=0; y < h ; y++, dest_p+=drev ){
		for( x=0 ; x < w ; x++, dest_p++ ){
			rr = (*dest_p&rgb16.mask_r) - wr;
			gg = (*dest_p&rgb16.mask_g) - wg;
			bb = (*dest_p&rgb16.mask_b) - wb;
			if( rr<0 ) rr = 0;
			if( gg<0 ) gg = 0;
			if( bb<0 ) bb = 0;
			*dest_p = (rr|gg|bb);
		}
	}
	return TRUE;
}



BOOL PRM_DrawRect_H_Mul( BMP_H *dest, int dx, int dy, int w, int h,
						int r, int g, int b )
{
	int		x,y;
	WORD	*dest_p = dest->buf + dy * dest->sx + dx;
	int		drev = dest->sx-w;
	WORD	*rtbl = BrightTable_HR[r];
	WORD	*gtbl = BrightTable_HG[g];
	WORD	*btbl = BrightTable_HB[b];
				
	
	if(w*h>40000){
		WORD	*brt_tbl = BMP_CreateBrightTable_H( r, g, b );
		for( y=0; y < h ; y++, dest_p+=drev ){
			for( x=0 ; x < w ; x++, dest_p++ ){
				*dest_p = brt_tbl[*dest_p];
			}
		}
	}else{
		for( y=0; y < h ; y++, dest_p+=drev ){
			for( x=0 ; x < w ; x++, dest_p++ ){
				*dest_p = rtbl[ BMP_R_HH(*dest_p) ] | gtbl[ BMP_G_HH(*dest_p) ] | btbl[ BMP_B_HH(*dest_p) ];
			}
		}
	}
	return TRUE;
}



BOOL PRM_DrawRect_H_Ami( BMP_H *dest, int dx, int dy, int w, int h,
						int r, int g, int b, int mesh )
{
	int		x,y;
	WORD	*dest_p = dest->buf + dy * dest->sx + dx;
	int		drev = dest->sx-w;
	WORD	rgb = BMP_RGB_FH(r,g,b);
	char	*linemask[4], *linemask2;
	WORD	mesh_pt = MeshFadePatern[mesh/16];

	
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






BOOL PRM_DrawRect_H(BMP_H *dest, int dx, int dy, int w, int h,
					RECT *clip, int r, int g, int b,
					DWORD wparam )
{
	BOOL	ret=TRUE;
	int		blnd  = HIWORD(wparam);
	int		param = LOWORD(wparam);

	
	if( dest->buf==NULL ) { DebugPrintf("•`‰æ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ\n"); return FALSE; }



	if( !ClipRect2( &dx, &dy, &w, &h, dest->sx, dest->sy, clip ) ) return ret;

	
	if( param==DRW_AMI2 || param== DRW_BLD2 ){
		if( blnd==BLD_0P   )return 1;			
		if( blnd==BLD_100P )param = DRW_NML;	
	}


	switch(param){
		default:
		case DRW_NML:	PRM_DrawRect_H_Std( dest, dx, dy, w, h, r, g, b );			break;	
		case DRW_BLD2:	PRM_DrawRect_H_Bld( dest, dx, dy, w, h, r, g, b, blnd );	break;	
		case DRW_ADD:	PRM_DrawRect_H_Add( dest, dx, dy, w, h, r, g, b );			break;	
		case DRW_SUB:	PRM_DrawRect_H_Sub( dest, dx, dy, w, h, r, g, b );			break;	
		case DRW_MUL:	PRM_DrawRect_H_Mul( dest, dx, dy, w, h, r, g, b );			break;	
		case DRW_AMI2:	PRM_DrawRect_H_Ami( dest, dx, dy, w, h, r, g, b, blnd );	break;	
		case DRW_NEG:			break;	
		case DRW_VIV2:			break;	
	}
	return ret;
}








BOOL PRM_DrawFxLine_H_Std( WORD *dest_p, MIN_MAX *mi, MIN_MAX *ma, RECT *clip, WORD *rgb_tbl )
{
	int		x, w;
	int		w2, wa;
	int		dx1,dx2;
	
	if( mi->x == DispWidth ) return TRUE;

	dx1	= max( clip->left,  mi->x );
	dx2 = min( clip->right, ma->x );
	dest_p += dx1;
	w  = dx2-dx1;
	w2 = w/4;
	wa = w%4;

	for( x=0 ; x < w2 ; x++, dest_p+=4 ){
		*(double*)dest_p = *(double*)rgb_tbl;
	}
	for( x=0 ; x < wa ; x++, dest_p++ ){
		*dest_p = *rgb_tbl;
	}
	
	return TRUE;
}



BOOL PRM_DrawFxLine_H_Bld( WORD *dest_p, MIN_MAX *mi, MIN_MAX *ma, RECT *clip, WORD *rgb_tbl, int blnd )
{
	int		x,w;
	int		dx1,dx2;
	int		brev = 256-blnd;
	int		blnd3 = blnd>>3, brev3 = 32-blnd3;

	if( mi->x == DispWidth )	return TRUE;	

	dx1	= max( clip->left,  mi->x );
	dx2 = min( clip->right, ma->x );
	w = dx2-dx1;
	dest_p += dx1;

	for( x=0 ; x < w ; x++, dest_p++ ){
		*dest_p = (WORD)( (( ( (*dest_p&rgb16.mask_rb)*brev3)>>5 )&rgb16.mask_rb)
						| (( ( (*dest_p&rgb16.mask_g )*brev3)>>5 )&rgb16.mask_g ) ) + *rgb_tbl;
	}
	return TRUE;
}



BOOL PRM_DrawFxLine_H_Add( WORD *dest_p, MIN_MAX *mi, MIN_MAX *ma, RECT *clip, WORD *rgb_tbl, int param )
{
	int		x, w;
	int		dx1,dx2;
	WORD	rgb;
	DWORD	dcol,scol;
	DWORD	mask1, mask2=0;
	
	if( mi->x == DispWidth ) return TRUE;

	dx1	= max( clip->left,  mi->x );
	dx2 = min( clip->right, ma->x );
	dest_p += dx1;
	w  = dx2-dx1;

	mask2 |= 0x00000001 << (rgb16.pos_r+5);
	mask2 |= 0x00000001 << (rgb16.pos_r);
	mask2 |= 0x00000001 << (rgb16.pos_g);
	mask1 = ~mask2;
	rgb = (WORD)(rgb_tbl[0]&mask1);

	
	for( x=0 ; x < w ; x++, dest_p++ ){
		dcol = (*dest_p&mask1)+rgb;
		scol = dcol & mask2;
		*dest_p = (WORD)((scol-(scol>>5)) | dcol);
	}
	return TRUE;
}



BOOL PRM_DrawFxLine_H_Sub( WORD *dest_p, MIN_MAX *mi, MIN_MAX *ma, RECT *clip, WORD *rgb_tbl )
{
	int		x, w;
	int		dx1,dx2;
	WORD	wr = rgb_tbl[0]&rgb16.mask_r;
	WORD	wg = rgb_tbl[0]&rgb16.mask_g;
	WORD	wb = rgb_tbl[0]&rgb16.mask_b;
	int		rr,gg,bb;
	
	if( mi->x == DispWidth ) return TRUE;

	dx1	= max( clip->left,  mi->x );
	dx2 = min( clip->right, ma->x );
	dest_p += dx1;
	w  = dx2-dx1;

	
	for( x=0 ; x < w ; x++, dest_p++ ){
		rr = (*dest_p&rgb16.mask_r) - wr;
		gg = (*dest_p&rgb16.mask_g) - wg;
		bb = (*dest_p&rgb16.mask_b) - wb;
		if( rr<0 ) rr = 0;
		if( gg<0 ) gg = 0;
		if( bb<0 ) bb = 0;
		*dest_p = (rr|gg|bb);
	}
	return TRUE;
}



BOOL PRM_DrawFxLine_H_Mul( WORD *dest_p, MIN_MAX *mi, MIN_MAX *ma, RECT *clip, int r, int g, int b )
{
	int		x, w;
	int		dx1,dx2;
	DWORD	mr = rgb16.mask_r;
	DWORD	mg = rgb16.mask_g;
	DWORD	mb = rgb16.mask_b;
	int		r2 = r-0x80;
	int		g2 = g-0x80;
	int		b2 = b-0x80;
	int		rr,gg,bb;
	
	if( mi->x == DispWidth ) return TRUE;

	dx1	= max( clip->left,  mi->x );
	dx2 = min( clip->right, ma->x );
	dest_p += dx1;
	w  = dx2-dx1;

	
	for( x=0 ; x < w ; x++, dest_p++ ){
		if( r<=BRT_NML)	rr = ( (     (*dest_p&mr) *r  )>>7              )&mr;
		else			rr = ((( (mr-(*dest_p&mr))*r2 )>>7)+(*dest_p&mr))&mr;
		if( g<=BRT_NML)	gg = ( (     (*dest_p&mg) *g  )>>7              )&mg;
		else			gg = ((( (mg-(*dest_p&mg))*g2 )>>7)+(*dest_p&mg))&mg;
		if( b<=BRT_NML)	bb = ( (     (*dest_p&mb) *b  )>>7              )&mb;
		else			bb = ((( (mb-(*dest_p&mb))*b2 )>>7)+(*dest_p&mb))&mb;
		*dest_p = (rr|gg|bb);
	}
	return TRUE;
}






BOOL PRM_DrawF3_H( BMP_H *dest,	int x1, int y1,
								int x2, int y2,
								int x3, int y3,
								RECT *clip, int r, int g, int b,
								DWORD wparam )
{
	BOOL		ret=TRUE;
	int			blnd  = HIWORD(wparam);
	int			param = LOWORD(wparam);
	WORD		*dest_p = dest->buf;
	int			drev = dest->sx;
	MIN_MAX		*mi = (MIN_MAX*)GAlloc(DispHeight*sizeof(MIN_MAX));
	MIN_MAX		*ma = (MIN_MAX*)GAlloc(DispHeight*sizeof(MIN_MAX));
	int			min_y, max_y;
	int			y;
	WORD		rgb = BMP_RGB_FH(r,g,b);
	WORD		rgb_tbl[4] = { rgb, rgb, rgb, rgb };

	
	if( dest->buf==NULL ) { DebugPrintf("•`‰æ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ\n"); ret=FALSE; goto end; }

	if( clip==NULL ) { clip = &ClipRectDef; }	

	
	if( param==DRW_AMI2 || param== DRW_BLD2 ){
		if( blnd==BLD_0P   )goto end;			
		if( blnd==BLD_100P )param = DRW_NML;	
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

	dest_p = dest->buf + drev*min_y;
	switch(param){
		default:
		case DRW_NML:	
			for( y = min_y ; y <= max_y ; y++, dest_p+=drev )
				PRM_DrawFxLine_H_Std( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl );
			break;
		case DRW_BLD2:	
			rgb = BMP_RGB_FH((r*blnd)>>8,(g*blnd)>>8,(b*blnd)>>8);
			rgb_tbl[0] = rgb_tbl[1] = rgb_tbl[2] = rgb_tbl[3] = rgb;
			for( y = min_y ; y <= max_y ; y++, dest_p+=drev )
				PRM_DrawFxLine_H_Bld( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl, blnd );
			break;
		case DRW_ADD:	
			for( y = min_y ; y <= max_y ; y++, dest_p+=drev )
				PRM_DrawFxLine_H_Add( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl, param );
			break;
		case DRW_SUB:	
			for( y = min_y ; y <= max_y ; y++, dest_p+=drev )
				PRM_DrawFxLine_H_Sub( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl );
			break;
		case DRW_MUL:	
			for( y = min_y ; y <= max_y ; y++, dest_p+=drev )
				PRM_DrawFxLine_H_Mul( dest_p, &mi[y-min_y], &ma[y-min_y], clip, r,g,b );
			break;
	}
end:
	GFree(mi);
	GFree(ma);
	return ret;
}





BOOL PRM_DrawF4_H( BMP_H *dest,	int x1, int y1,
								int x2, int y2,
								int x3, int y3,
								int x4, int y4,
								RECT *clip, int r, int g, int b,
								DWORD wparam )
{
	BOOL		ret=TRUE;
	int			blnd  = HIWORD(wparam);
	int			param = LOWORD(wparam);
	WORD		*dest_p = dest->buf;
	int			drev = dest->sx;
	MIN_MAX		*mi = (MIN_MAX*)GAlloc(DispHeight*sizeof(MIN_MAX));
	MIN_MAX		*ma = (MIN_MAX*)GAlloc(DispHeight*sizeof(MIN_MAX));
	int			min_y, max_y;
	int			y;
	WORD		rgb = BMP_RGB_FH(r,g,b);
	WORD		rgb_tbl[4] = { rgb, rgb, rgb, rgb };

	
	if( dest->buf==NULL ) { DebugPrintf("•`‰æ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ\n"); ret=FALSE; goto end; }

	if( clip==NULL ) { clip = &ClipRectDef; }	

	
	if( param==DRW_AMI2 || param== DRW_BLD2 ){
		if( blnd==BLD_0P   )goto end;	
		if( blnd==BLD_100P )param = DRW_NML;	
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

	dest_p = dest->buf + drev*min_y;
	switch(param){
		default:
		case DRW_NML:	
			for( y = min_y ; y <= max_y ; y++, dest_p+=drev )
				PRM_DrawFxLine_H_Std( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl );
			break;
		case DRW_BLD2:	
			rgb = BMP_RGB_FH((r*blnd)>>8,(g*blnd)>>8,(b*blnd)>>8);
			rgb_tbl[0] = rgb_tbl[1] = rgb_tbl[2] = rgb_tbl[3] = rgb;
			for( y = min_y ; y <= max_y ; y++, dest_p+=drev )
				PRM_DrawFxLine_H_Bld( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl, blnd );
			break;
		case DRW_ADD:	
			for( y = min_y ; y <= max_y ; y++, dest_p+=drev )
				PRM_DrawFxLine_H_Add( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl, param );
			break;
		case DRW_SUB:	
			for( y = min_y ; y <= max_y ; y++, dest_p+=drev )
				PRM_DrawFxLine_H_Sub( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl );
			break;
		case DRW_MUL:	
			for( y = min_y ; y <= max_y ; y++, dest_p+=drev )
				PRM_DrawFxLine_H_Mul( dest_p, &mi[y-min_y], &ma[y-min_y], clip, r,g,b );
			break;
	}
end:
	GFree(mi);
	GFree(ma);
	return ret;
}










BOOL PRM_DrawY3_H( BMP_H *dest,	int x1, int y1,
								int x2, int y2,
								int x3, int y3, RECT *clip,
								int sy, int sr, int sg, int sb,
								int ey, int er, int eg, int eb,
								DWORD wparam )
{
	BOOL		ret=TRUE;
	int			blnd  = HIWORD(wparam);
	int			param = LOWORD(wparam);
	WORD		*dest_p = dest->buf;
	int			drev = dest->sx;
	MIN_MAX		*mi = (MIN_MAX*)GAlloc(DispHeight*sizeof(MIN_MAX));
	MIN_MAX		*ma = (MIN_MAX*)GAlloc(DispHeight*sizeof(MIN_MAX));
	int			min_y, max_y;
	int			y, ay;
	int			r,g,b;
	int			esy = ey-sy;
	int			esr = er-sr;
	int			esg = eg-sg;
	int			esb = eb-sb;
	WORD		rgb_tbl[4];

	if( sy==ey || (sr==er && sg==eg && sb==eb) )
		return PRM_DrawF3_H( dest, x1, y1, x2, y2, x3, y3, clip, sr, sg, sb, wparam );

	
	if( dest->buf==NULL ) { DebugPrintf("•`‰æ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ\n"); ret=FALSE; goto end; }

	if( clip==NULL ) { clip = &ClipRectDef; }	

	
	if( param==DRW_AMI2 || param== DRW_BLD2 ){
		if( blnd==BLD_0P   )goto end;			
		if( blnd==BLD_100P )param = DRW_NML;	
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

	dest_p = dest->buf + drev*min_y;
	for( y = min_y ; y <= max_y ; y++, dest_p+=drev ){
		ay = y-sy;
		r = STD_Limit( esr*ay/esy + sr, sr, er );
		g = STD_Limit( esg*ay/esy + sg, sg, eg );
		b = STD_Limit( esb*ay/esy + sb, sb, eb );
		if( param== DRW_BLD2 ){
			rgb_tbl[0] = rgb_tbl[1] = rgb_tbl[2] = rgb_tbl[3] = BMP_RGB_FH((r*blnd)>>8,(g*blnd)>>8,(b*blnd)>>8);
		}else{
			rgb_tbl[0] = rgb_tbl[1] = rgb_tbl[2] = rgb_tbl[3] = BMP_RGB_FH(r,g,b);
		}
		switch(param){
			default:
			case DRW_NML:	PRM_DrawFxLine_H_Std( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl );			break;	
			case DRW_BLD2:	PRM_DrawFxLine_H_Bld( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl, blnd );	break;	
			case DRW_ADD:	PRM_DrawFxLine_H_Add( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl, param );	break;
			case DRW_SUB:	PRM_DrawFxLine_H_Sub( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl );			break;	
			case DRW_MUL:	PRM_DrawFxLine_H_Mul( dest_p, &mi[y-min_y], &ma[y-min_y], clip, r,g,b );			break;	
		}
	}
end:
	GFree(mi);
	GFree(ma);
	return ret;
}






BOOL PRM_DrawY4_H( BMP_H *dest,	int x1, int y1,
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
	WORD		*dest_p = dest->buf;
	int			drev = dest->sx;
	MIN_MAX		*mi = (MIN_MAX*)GAlloc(DispHeight*sizeof(MIN_MAX));
	MIN_MAX		*ma = (MIN_MAX*)GAlloc(DispHeight*sizeof(MIN_MAX));
	int			min_y, max_y;
	int			y, ay;
	int			r,g,b;
	int			esy = ey-sy;
	int			esr = er-sr;
	int			esg = eg-sg;
	int			esb = eb-sb;
	WORD		rgb_tbl[4];

	if( sy==ey || (sr==er && sg==eg && sb==eb) )
		return PRM_DrawF4_H( dest, x1, y1, x2, y2, x3, y3, x4, y4, clip, sr, sg, sb, wparam );

	
	if( dest->buf==NULL ) { DebugPrintf("•`‰æ—Ìˆæ‚ª‚ ‚è‚Ü‚¹‚ñ\n"); ret=FALSE; goto end; }

	if( clip==NULL ) { clip = &ClipRectDef; }	

	
	if( param==DRW_AMI2 || param== DRW_BLD2 ){
		if( blnd==BLD_0P   )goto end;			
		if( blnd==BLD_100P )param = DRW_NML;	
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

	dest_p = dest->buf + drev*min_y;
	for( y = min_y ; y <= max_y ; y++, dest_p+=drev ){
		ay = y-sy;
		r = STD_Limit( esr*ay/esy + sr, sr, er );
		g = STD_Limit( esg*ay/esy + sg, sg, eg );
		b = STD_Limit( esb*ay/esy + sb, sb, eb );
		if( param== DRW_BLD2 ){
			rgb_tbl[0] = rgb_tbl[1] = rgb_tbl[2] = rgb_tbl[3] = BMP_RGB_FH((r*blnd)>>8,(g*blnd)>>8,(b*blnd)>>8);
		}else{
			rgb_tbl[0] = rgb_tbl[1] = rgb_tbl[2] = rgb_tbl[3] = BMP_RGB_FH(r,g,b);
		}
		switch(param){
			default:
			case DRW_NML:	PRM_DrawFxLine_H_Std( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl );			break;	
			case DRW_BLD2:	PRM_DrawFxLine_H_Bld( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl, blnd );	break;	
			case DRW_ADD:	PRM_DrawFxLine_H_Add( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl, param );	break;
			case DRW_SUB:	PRM_DrawFxLine_H_Sub( dest_p, &mi[y-min_y], &ma[y-min_y], clip, rgb_tbl );			break;	
			case DRW_MUL:	PRM_DrawFxLine_H_Mul( dest_p, &mi[y-min_y], &ma[y-min_y], clip, r,g,b );			break;	
		}
	}
end:
	GFree(mi);
	GFree(ma);
	return ret;
}








BOOL PRM_DrawGxLine_H_Std( WORD *dest_p,
							int r,  int g,  int b,  int pr, int pg, int pb,
							int dr, int dg, int db, int fr, int fg, int fb, int  w, int ww)
{
	int		rr=0,gg=0,bb=0;
	int		x;
	int		wwr,wwg;
	WORD	rgb,prgb;
	int		r2 = 1<<rgb16.num_r;
	int		g2 = 1<<rgb16.num_g;
	int		b2 = 1<<rgb16.num_b;

	db = db + ww*(pb%b2)/b2;
	b  /= b2;	pb /= b2;
	if(db>=ww)	{ pb+=fb; db-=ww; }

	dg = dg + ww*(pg%g2)/g2;
	g  /= g2;	pg /= g2;
	if(dg>=ww)	{ pg+=fg; dg-=ww; }
	g<<=rgb16.pos_g;	pg<<=rgb16.pos_g;	fg<<=rgb16.pos_g;

	dr = dr + ww*(pr%r2)/r2;
	r  /= r2;	pr /= r2;
	if(dr>=ww)	{ pr+=fr; dr-=ww; }
	r<<=rgb16.pos_r;	pr<<=rgb16.pos_r;	fr<<=rgb16.pos_r;

	wwr = ww*r2;	wwg = ww*g2;
	rgb = r|g|b;	prgb = pr|pg|pb;

	for( x=0; x < w ; x++, dest_p++ ){
		*dest_p = rgb;

		rgb +=prgb;
		rr+=dr;	gg+=dg;	bb+=db;
		if( rr >= wwr ){ rr -= wwr; rgb+=fr; }
		if( gg >= wwg ){ gg -= wwg; rgb+=fg; }
		if( bb >= wwr ){ bb -= wwr; rgb+=fb; }
	}
	return TRUE;
}



BOOL PRM_DrawGxLine_H_Bld( WORD *dest_p,
							int r,  int g,  int b,  int pr, int pg, int pb,
							int dr, int dg, int db, int fr, int fg, int fb, int  w, int ww, int blnd )
{
	int		bl32 = blnd/8;
	int		brev = 32-bl32;
	int		rr=0,gg=0,bb=0;
	int		x;
	int		wwr,wwg;
	WORD	rgb,prgb;
	int		r2 = 1<<rgb16.num_r;
	int		g2 = 1<<rgb16.num_g;
	int		b2 = 1<<rgb16.num_b;

	r  = (r*bl32)>>5;
	g  = (g*bl32)>>5;
	b  = (b*bl32)>>5;
	pr = (pr*bl32)>>5;
	pg = (pg*bl32)>>5;
	pb = (pb*bl32)>>5;
	dr = (dr*bl32)>>5;
	dg = (dg*bl32)>>5;
	db = (db*bl32)>>5;

	db = db + ww*(pb%b2)/b2;
	b  /= b2;	pb /= b2;
	if(db>=ww)	{ pb+=fb; db-=ww; }

	dg = dg + ww*(pg%g2)/g2;
	g  /= g2;	pg /= g2;
	if(dg>=ww)	{ pg+=fg; dg-=ww; }
	g<<=rgb16.pos_g;	pg<<=rgb16.pos_g;	fg<<=rgb16.pos_g;

	dr = dr + ww*(pr%r2)/r2;
	r  /= r2;	pr /= r2;
	if(dr>=ww)	{ pr+=fr; dr-=ww; }
	r<<=rgb16.pos_r;	pr<<=rgb16.pos_r;	fr<<=rgb16.pos_r;

	wwr = ww*r2;	wwg = ww*g2;
	rgb = r|g|b;	prgb = pr|pg|pb;

	for( x=0; x < w ; x++, dest_p++ ){
		*dest_p = (WORD)( (((((*dest_p&rgb16.mask_rb)*brev)>>5) )&rgb16.mask_rb)
						| (((((*dest_p&rgb16.mask_g )>>5)*brev) )&rgb16.mask_g ) ) + rgb;

		rgb +=prgb;
		rr+=dr;	gg+=dg;	bb+=db;
		if( rr >= wwr ){ rr -= wwr; rgb+=fr; }
		if( gg >= wwg ){ gg -= wwg; rgb+=fg; }
		if( bb >= wwr ){ bb -= wwr; rgb+=fb; }
	}

	return TRUE;
}



BOOL PRM_DrawGxLine_H_Add( WORD *dest_p,
							int r,  int g,  int b,  int pr, int pg, int pb,
							int dr, int dg, int db, int fr, int fg, int fb, int  w, int ww )
{
	int		rr=0,gg=0,bb=0;
	int		x;
	int		wwr,wwg;
	WORD	rgb,prgb;
	int		r2 = 1<<rgb16.num_r;
	int		g2 = 1<<rgb16.num_g;
	int		b2 = 1<<rgb16.num_b;
	DWORD	dcol,scol;
	DWORD	mask1, mask2=0;

	mask2 |= 0x00000001 << (rgb16.pos_r+5);
	mask2 |= 0x00000001 << (rgb16.pos_r);
	mask2 |= 0x00000001 << (rgb16.pos_g);
	mask1 = ~mask2;


	db = db + ww*(pb%b2)/b2;
	b  /= b2;	pb /= b2;
	if(db>=ww)	{ pb+=fb; db-=ww; }

	dg = dg + ww*(pg%g2)/g2;
	g  /= g2;	pg /= g2;
	if(dg>=ww)	{ pg+=fg; dg-=ww; }
	g<<=rgb16.pos_g;	pg<<=rgb16.pos_g;	fg<<=rgb16.pos_g;

	dr = dr + ww*(pr%r2)/r2;
	r  /= r2;	pr /= r2;
	if(dr>=ww)	{ pr+=fr; dr-=ww; }
	r<<=rgb16.pos_r;	pr<<=rgb16.pos_r;	fr<<=rgb16.pos_r;

	wwr = ww*r2;	wwg = ww*g2;
	rgb = r|g|b;	prgb = pr|pg|pb;

	for( x=0; x < w ; x++, dest_p++ ){
		dcol = (*dest_p&mask1)+(rgb&mask1);
		scol = dcol & mask2;
		*dest_p = (WORD)((scol-(scol>>5)) | dcol);

		rgb +=prgb;
		rr+=dr;	gg+=dg;	bb+=db;
		if( rr >= wwr ){ rr -= wwr; rgb+=fr; }
		if( gg >= wwg ){ gg -= wwg; rgb+=fg; }
		if( bb >= wwr ){ bb -= wwr; rgb+=fb; }
	}

	return TRUE;
}



BOOL PRM_DrawGxLine_H_Sub( WORD *dest_p,
							int r,  int g,  int b,  int pr, int pg, int pb,
							int dr, int dg, int db, int fr, int fg, int fb, int  w, int ww )
{
	int		rr=0,gg=0,bb=0;
	int		x;
	int		wwr,wwg;
	WORD	rgb,prgb;
	int		r2 = 1<<rgb16.num_r;
	int		g2 = 1<<rgb16.num_g;
	int		b2 = 1<<rgb16.num_b;
	int		sr,sg,sb;

	db = db + ww*(pb%b2)/b2;
	b  /= b2;	pb /= b2;
	if(db>=ww)	{ pb+=fb; db-=ww; }

	dg = dg + ww*(pg%g2)/g2;
	g  /= g2;	pg /= g2;
	if(dg>=ww)	{ pg+=fg; dg-=ww; }
	g<<=rgb16.pos_g;	pg<<=rgb16.pos_g;	fg<<=rgb16.pos_g;

	dr = dr + ww*(pr%r2)/r2;
	r  /= r2;	pr /= r2;
	if(dr>=ww)	{ pr+=fr; dr-=ww; }
	r<<=rgb16.pos_r;	pr<<=rgb16.pos_r;	fr<<=rgb16.pos_r;

	wwr = ww*r2;	wwg = ww*g2;
	rgb = r|g|b;	prgb = pr|pg|pb;

	for( x=0; x < w ; x++, dest_p++ ){
		sr = (*dest_p&rgb16.mask_r) - (rgb&rgb16.mask_r);
		if( sr<0 ) sr = 0;
		sg = (*dest_p&rgb16.mask_g) - (rgb&rgb16.mask_g);
		if( sg<0 ) sg = 0;
		sb = (*dest_p&rgb16.mask_b) - (rgb&rgb16.mask_b);
		if( sb<0 ) sb = 0;
		*dest_p = sr|sg|sb;

		rgb +=prgb;
		rr+=dr;	gg+=dg;	bb+=db;
		if( rr >= wwr ){ rr -= wwr; r+=fr; }
		if( gg >= wwg ){ gg -= wwg; g+=fg; }
		if( bb >= wwr ){ bb -= wwr; b+=fb; }
	}

	return TRUE;
}



BOOL PRM_DrawGxLine_H_Mul( WORD *dest_p,
							int r,  int g,  int b,  int pr, int pg, int pb,
							int dr, int dg, int db, int fr, int fg, int fb, int  w, int ww )
{
	int		rr=0,gg=0,bb=0;
	int		x;
	int		wwr,wwg;
	WORD	rgb,prgb;
	int		r2 = 1<<rgb16.num_r;
	int		g2 = 1<<rgb16.num_g;
	int		b2 = 1<<rgb16.num_b;

	db = db + ww*(pb%b2)/b2;
	b  /= b2;	pb /= b2;
	if(db>=ww)	{ pb+=fb; db-=ww; }

	dg = dg + ww*(pg%g2)/g2;
	g  /= g2;	pg /= g2;
	if(dg>=ww)	{ pg+=fg; dg-=ww; }
	g<<=rgb16.pos_g;	pg<<=rgb16.pos_g;	fg<<=rgb16.pos_g;

	dr = dr + ww*(pr%r2)/r2;
	r  /= r2;	pr /= r2;
	if(dr>=ww)	{ pr+=fr; dr-=ww; }
	r<<=rgb16.pos_r;	pr<<=rgb16.pos_r;	fr<<=rgb16.pos_r;

	wwr = ww*r2;	wwg = ww*g2;
	rgb = r|g|b;	prgb = pr|pg|pb;

	for( x=0; x < w ; x++, dest_p++ ){
		*dest_p = rgb;

		rgb +=prgb;
		rr+=dr;	gg+=dg;	bb+=db;
		if( rr >= wwr ){ rr -= wwr; rgb+=fr; }
		if( gg >= wwg ){ gg -= wwg; rgb+=fg; }
		if( bb >= wwr ){ bb -= wwr; rgb+=fb; }
	}

	return TRUE;
}



BOOL PRM_DrawGxLine_H( WORD *dest_p, MIN_MAX *mi, MIN_MAX *ma, RECT *clip, DWORD wparam )
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
	dest_p += dx1;

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
			ret = PRM_DrawGxLine_H_Std( dest_p,
										r*xx/ww + mi->rgb.r, g*xx/ww + mi->rgb.g, b*xx/ww + mi->rgb.b,
										r/ww, g/ww, b/ww, dr, dg, db, fr, fg, fb, w, ww );
			break;
		case DRW_BLD2:	
			ret = PRM_DrawGxLine_H_Bld( dest_p,
										r*xx/ww + mi->rgb.r, g*xx/ww + mi->rgb.g, b*xx/ww + mi->rgb.b,
										r/ww, g/ww, b/ww, dr, dg, db, fr, fg, fb, w, ww, blnd );
			break;
		case DRW_ADD:	
			ret = PRM_DrawGxLine_H_Add( dest_p,
										r*xx/ww + mi->rgb.r, g*xx/ww + mi->rgb.g, b*xx/ww + mi->rgb.b,
										r/ww, g/ww, b/ww, dr, dg, db, fr, fg, fb, w, ww );
			break;
		case DRW_SUB:	
			ret = PRM_DrawGxLine_H_Sub( dest_p,
										r*xx/ww + mi->rgb.r, g*xx/ww + mi->rgb.g, b*xx/ww + mi->rgb.b,
										r/ww, g/ww, b/ww, dr, dg, db, fr, fg, fb, w, ww );
			break;
		case DRW_MUL:	
			ret = PRM_DrawGxLine_H_Mul( dest_p,
										r*xx/ww + mi->rgb.r, g*xx/ww + mi->rgb.g, b*xx/ww + mi->rgb.b,
										r/ww, g/ww, b/ww, dr, dg, db, fr, fg, fb, w, ww );
			break;
		case DRW_AMI2:			break;	
		case DRW_NEG:			break;	
		case DRW_VIV2:			break;	
	}
	return ret;
}






BOOL PRM_DrawG3_H( BMP_H *dest,	int x1, int y1, int r1, int g1, int b1,
								int x2, int y2, int r2, int g2, int b2,
								int x3, int y3, int r3, int g3, int b3,
								RECT *clip, DWORD wparam )
{
	BOOL		ret=TRUE;
	int			blnd  = HIWORD(wparam);
	int			param = LOWORD(wparam);
	WORD		*dest_p = dest->buf;
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
		PRM_DrawGxLine_H( dest_p, &mi[y-min_y], &ma[y-min_y], clip, wparam );
end:
	GFree(mi);
	GFree(ma);
	return ret;
}





BOOL PRM_DrawG4_H( BMP_H *dest,	int x1, int y1, int r1, int g1, int b1,
								int x2, int y2, int r2, int g2, int b2,
								int x3, int y3, int r3, int g3, int b3,
								int x4, int y4, int r4, int g4, int b4,
								RECT *clip, DWORD wparam )
{
	BOOL		ret=TRUE;
	int			blnd  = HIWORD(wparam);
	int			param = LOWORD(wparam);
	WORD		*dest_p = dest->buf;
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
		PRM_DrawGxLine_H( dest_p, &mi[y-min_y], &ma[y-min_y], clip, wparam );

end:
	GFree(mi);
	GFree(ma);
	return ret;
}



BOOL PRM_DrawFLine_H_Std( BMP_H *dest,	int x1, int y1,
										int x2, int y2,
										int r, int g, int b )
{
	WORD	rgb = BMP_RGB_FH(r,g,b);
	WORD	*dest_p = dest->buf + y1*dest->sx + x1;
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



BOOL PRM_DrawFLine_H_Bld( BMP_H *dest,	int x1, int y1,
										int x2, int y2,
										int r, int g, int b, int blnd )
{
	int		bl32 = blnd/8;
	int		brev = 32-bl32;
	int		r2 = (r*bl32)>>5;
	int		g2 = (g*bl32)>>5;
	int		b2 = (b*bl32)>>5;
	WORD	rgb = BMP_RGB_FH(r2,g2,b2);
	WORD	*dest_p = dest->buf + y1*dest->sx + x1;
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
				*dest_p = (WORD)( ((((*dest_p&rgb16.mask_rb)*brev)>>5 )&rgb16.mask_rb)
								| ((((*dest_p&rgb16.mask_g )>>5)*brev )&rgb16.mask_g ) ) + rgb;
				
				hcnt += w;
				dest_p += dh;
			}
			hcnt -= h;
		}
	}else{
		for( y=0; y < h ; y++, dest_p += dh ){
			while( wcnt < w ){
				*dest_p = (WORD)( ((((*dest_p&rgb16.mask_rb)*brev)>>5 )&rgb16.mask_rb)
								| ((((*dest_p&rgb16.mask_g )>>5)*brev )&rgb16.mask_g ) ) + rgb;
				
				wcnt += h;
				dest_p+=fw;
			}
			wcnt -= w;
		}
	}

	return 1;
}



BOOL PRM_DrawFLine_H_Add( BMP_H *dest,	int x1, int y1,
										int x2, int y2,
										int r, int g, int b )
{
	DWORD	dcol,scol;
	DWORD	mask1, mask2=0;
	WORD	rgb = BMP_RGB_FH(r,g,b);
	WORD	*dest_p = dest->buf + y1*dest->sx + x1;
	int		x,y;
	int		w=x2-x1, wcnt=0;
	int		h=y2-y1, hcnt=0;
	int		fw = (w<0)?-1:1;
	int		fh = (h<0)?-1:1;
	int		dh = dest->sx*fh;

	mask2 |= 0x00000001 << (rgb16.pos_r+5);
	mask2 |= 0x00000001 << (rgb16.pos_r);
	mask2 |= 0x00000001 << (rgb16.pos_g);
	mask1 = ~mask2;
	rgb = (WORD)(rgb&mask1);

	w = w*fw+1;
	h = h*fh+1;

	if( w<h ){
		for( x=0; x < w ; x++, dest_p+=fw ){
			while( hcnt < h ){
				dcol = (*dest_p&mask1)+rgb;
				scol = dcol & mask2;
				*dest_p = (WORD)((scol-(scol>>5)) | dcol);
				
				hcnt += w;
				dest_p += dh;
			}
			hcnt -= h;
		}
	}else{
		for( y=0; y < h ; y++, dest_p += dh ){
			while( wcnt < w ){
				dcol = (*dest_p&mask1)+rgb;
				scol = dcol & mask2;
				*dest_p = (WORD)((scol-(scol>>5)) | dcol);
				
				wcnt += h;
				dest_p+=fw;
			}
			wcnt -= w;
		}
	}

	return 1;
}



BOOL PRM_DrawFLine_H_Sub( BMP_H *dest,	int x1, int y1,
										int x2, int y2,
										int r, int g, int b )
{
	WORD	wr = F2H_TblRW[r];
	WORD	wg = F2H_TblGW[g];
	WORD	wb = F2H_TblBW[b];
	int		rr,gg,bb;
	WORD	*dest_p = dest->buf + y1*dest->sx + x1;
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
				rr = (*dest_p&rgb16.mask_r) - wr;
				gg = (*dest_p&rgb16.mask_g) - wg;
				bb = (*dest_p&rgb16.mask_b) - wb;
				if( rr<0 ) rr = 0;
				if( gg<0 ) gg = 0;
				if( bb<0 ) bb = 0;
				*dest_p = (rr|gg|bb);
				
				hcnt += w;
				dest_p += dh;
			}
			hcnt -= h;
		}
	}else{
		for( y=0; y < h ; y++, dest_p += dh ){
			while( wcnt < w ){
				rr = (*dest_p&rgb16.mask_r) - wr;
				gg = (*dest_p&rgb16.mask_g) - wg;
				bb = (*dest_p&rgb16.mask_b) - wb;
				if( rr<0 ) rr = 0;
				if( gg<0 ) gg = 0;
				if( bb<0 ) bb = 0;
				*dest_p = (rr|gg|bb);
				
				wcnt += h;
				dest_p+=fw;
			}
			wcnt -= w;
		}
	}

	return 1;
}



BOOL PRM_DrawFLine_H_Mul( BMP_H *dest,	int x1, int y1,
										int x2, int y2,
										int r, int g, int b )
{
	WORD	*dest_p = dest->buf + y1*dest->sx + x1;
	int		x,y;
	int		w=x2-x1, wcnt=0;
	int		h=y2-y1, hcnt=0;
	int		fw = (w<0)?-1:1;
	int		fh = (h<0)?-1:1;
	int		dh = dest->sx*fh;
	WORD	*rtbl = BrightTable_HR[r];
	WORD	*gtbl = BrightTable_HG[g];
	WORD	*btbl = BrightTable_HB[b];

	w = w*fw+1;
	h = h*fh+1;

	if( w<h ){
		for( x=0; x < w ; x++, dest_p+=fw ){
			while( hcnt < h ){
				*dest_p = rtbl[ (*dest_p&rgb16.mask_r)>>rgb16.pos_r ]
						| gtbl[ (*dest_p&rgb16.mask_g)>>rgb16.pos_g ]
						| btbl[ (*dest_p&rgb16.mask_b)>>rgb16.pos_b ];
				
				hcnt += w;
				dest_p += dh;
			}
			hcnt -= h;
		}
	}else{
		for( y=0; y < h ; y++, dest_p += dh ){
			while( wcnt < w ){
				*dest_p = rtbl[ (*dest_p&rgb16.mask_r)>>rgb16.pos_r ]
						| gtbl[ (*dest_p&rgb16.mask_g)>>rgb16.pos_g ]
						| btbl[ (*dest_p&rgb16.mask_b)>>rgb16.pos_b ];
				
				wcnt += h;
				dest_p+=fw;
			}
			wcnt -= w;
		}
	}

	return 1;
}






BOOL PRM_DrawFLine_H( BMP_H *dest,	int x1, int y1,
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
		case DRW_NML:	ret = PRM_DrawFLine_H_Std( dest, x1, y1, x2, y2, r, g, b );			break;	
		case DRW_BLD2:	ret = PRM_DrawFLine_H_Bld( dest, x1, y1, x2, y2, r, g, b, blnd );	break;	
		case DRW_ADD:	ret = PRM_DrawFLine_H_Add( dest, x1, y1, x2, y2, r, g, b );			break;	
		case DRW_SUB:	ret = PRM_DrawFLine_H_Sub( dest, x1, y1, x2, y2, r, g, b );			break;	
		case DRW_MUL:	ret = PRM_DrawFLine_H_Mul( dest, x1, y1, x2, y2, r, g, b );			break;	
		case DRW_AMI2:			break;	
		case DRW_NEG:			break;	
		case DRW_VIV2:			break;	
	}
	return ret;
}




BOOL PRM_DrawGLine_H_Std( BMP_H *dest,	int x1, int y1, int r1, int g1, int b1,
										int x2, int y2, int r2, int g2, int b2 )
{
	int		x,y;
	WORD	*dest_p = dest->buf + y1*dest->sx + x1;
	int		r = r1, g = g1, b = b1;
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
				*dest_p = BMP_RGB_FH(r,g,b);
			
				r += r4;	r5 += r3;	if( r5>h ) { r5-=h; r +=fr; }
				g += g4;	g5 += g3;	if( g5>h ) { g5-=h; g +=fg; }
				b += b4;	b5 += b3;	if( b5>h ) { b5-=h; b +=fb; }

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
				*dest_p = BMP_RGB_FH(r,g,b);

				r += r4;	r5 += r3;	if( r5>h ) { r5-=h; r +=fr; }
				g += g4;	g5 += g3;	if( g5>h ) { g5-=h; g +=fg; }
				b += b4;	b5 += b3;	if( b5>h ) { b5-=h; b +=fb; }

				wcnt += h;
				dest_p+=fw;
			}
			wcnt -= w;
		}
	}

	return 1;
}




BOOL PRM_DrawGLine_H_Bld( BMP_H *dest,	int x1, int y1, int r1, int g1, int b1,
										int x2, int y2, int r2, int g2, int b2, int blnd )
{
	int		x,y;
	WORD	*dest_p = dest->buf + y1*dest->sx + x1;
	int		r = r1, g = g1, b = b1;
	int		r3 = r2-r1, g3 = g2-g1, b3 = b2-b1;
	int		r4, g4, b4, r5=0, g5=0, b5=0;
	int		fr = (r3<0)?-1:1, fg = (g3<0)?-1:1, fb=(b3<0)?-1:1;
	int		w=x2-x1,wcnt=0;
	int		h=y2-y1,hcnt=0;
	int		fw = (w <0)?-1:1, fh = ( h<0)?-1:1;
	int		dh = dest->sx*fh;
	int		brev = 32-blnd/8;
	w = w*fw+1;
	h = h*fh+1;

	r = r1 = (r1*blnd)>>8;
	g = g1 = (g1*blnd)>>8;
	b = b1 = (b1*blnd)>>8;
	r2 = (r2*blnd)>>8;
	g2 = (g2*blnd)>>8;
	b2 = (b2*blnd)>>8;
	r3 = r2-r1;
	g3 = g2-g1;
	b3 = b2-b1;

	if( w<h ){
		r4 = r3   /h;	g4 = g3   /h;	b4 = b3   /h;
		r3 = r3*fr%h;	g3 = g3*fg%h;	b3 = b3*fb%h;
		for( x=0; x < w ; x++, dest_p+=fw ){
			while( hcnt < h ){
				*dest_p = BMP_BLD_H(*dest_p,brev) + BMP_RGB_FH(r,g,b);
			
				r += r4;	r5 += r3;	if( r5>h ) { r5-=h; r +=fr; }
				g += g4;	g5 += g3;	if( g5>h ) { g5-=h; g +=fg; }
				b += b4;	b5 += b3;	if( b5>h ) { b5-=h; b +=fb; }

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
				*dest_p = BMP_BLD_H(*dest_p,brev) + BMP_RGB_FH(r,g,b);

				r += r4;	r5 += r3;	if( r5>h ) { r5-=h; r +=fr; }
				g += g4;	g5 += g3;	if( g5>h ) { g5-=h; g +=fg; }
				b += b4;	b5 += b3;	if( b5>h ) { b5-=h; b +=fb; }

				wcnt += h;
				dest_p+=fw;
			}
			wcnt -= w;
		}
	}

	return 1;
}



BOOL PRM_DrawGLine_H_Add( BMP_H *dest,	int x1, int y1, int r1, int g1, int b1,
										int x2, int y2, int r2, int g2, int b2 )
{
	int		x,y;
	WORD	*dest_p = dest->buf + y1*dest->sx + x1;
	int		r = r1, g = g1, b = b1;
	int		r3 = r2-r1, g3 = g2-g1, b3 = b2-b1;
	int		r4, g4, b4, r5=0, g5=0, b5=0;
	int		fr = (r3<0)?-1:1, fg = (g3<0)?-1:1, fb=(b3<0)?-1:1;
	int		w=x2-x1,wcnt=0;
	int		h=y2-y1,hcnt=0;
	int		fw = (w <0)?-1:1, fh = ( h<0)?-1:1;
	int		dh = dest->sx*fh;
	DWORD	dcol,scol;
	DWORD	mask1, mask2=0;
	
	mask2 |= 0x00000001 << (rgb16.pos_r+5);
	mask2 |= 0x00000001 << (rgb16.pos_r);
	mask2 |= 0x00000001 << (rgb16.pos_g);
	mask1 = ~mask2;

	
	w = w*fw+1;
	h = h*fh+1;

	if( w<h ){
		r4 = r3   /h;	g4 = g3   /h;	b4 = b3   /h;
		r3 = r3*fr%h;	g3 = g3*fg%h;	b3 = b3*fb%h;
		for( x=0; x < w ; x++, dest_p+=fw ){
			while( hcnt < h ){
				dcol = (*dest_p&mask1) + (BMP_RGB_FH(r,g,b)&mask1);
				scol = dcol & mask2;
				*dest_p = (WORD)((scol-(scol>>5)) | dcol);

				r += r4;	r5 += r3;	if( r5>h ) { r5-=h; r +=fr; }
				g += g4;	g5 += g3;	if( g5>h ) { g5-=h; g +=fg; }
				b += b4;	b5 += b3;	if( b5>h ) { b5-=h; b +=fb; }

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
				dcol = (*dest_p&mask1) + (BMP_RGB_FH(r,g,b)&mask1);
				scol = dcol & mask2;
				*dest_p = (WORD)((scol-(scol>>5)) | dcol);

				r += r4;	r5 += r3;	if( r5>h ) { r5-=h; r +=fr; }
				g += g4;	g5 += g3;	if( g5>h ) { g5-=h; g +=fg; }
				b += b4;	b5 += b3;	if( b5>h ) { b5-=h; b +=fb; }

				wcnt += h;
				dest_p+=fw;
			}
			wcnt -= w;
		}
	}

	return 1;
}




BOOL PRM_DrawGLine_H_Sub( BMP_H *dest,	int x1, int y1, int r1, int g1, int b1,
										int x2, int y2, int r2, int g2, int b2 )
{
	int		x,y;
	WORD	*dest_p = dest->buf + y1*dest->sx + x1;
	int		rr,gg,bb,r = r1, g = g1, b = b1;
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
				rr = (*dest_p&rgb16.mask_r) - F2H_TblRW[r];
				gg = (*dest_p&rgb16.mask_g) - F2H_TblGW[g];
				bb = (*dest_p&rgb16.mask_b) - F2H_TblBW[b];
				if( rr<0 ) rr = 0;
				if( gg<0 ) gg = 0;
				if( bb<0 ) bb = 0;
				*dest_p = (rr|gg|bb);

				r += r4;	r5 += r3;	if( r5>h ) { r5-=h; r +=fr; }
				g += g4;	g5 += g3;	if( g5>h ) { g5-=h; g +=fg; }
				b += b4;	b5 += b3;	if( b5>h ) { b5-=h; b +=fb; }

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
				rr = (*dest_p&rgb16.mask_r) - F2H_TblRW[r];
				gg = (*dest_p&rgb16.mask_g) - F2H_TblGW[g];
				bb = (*dest_p&rgb16.mask_b) - F2H_TblBW[b];
				if( rr<0 ) rr = 0;
				if( gg<0 ) gg = 0;
				if( bb<0 ) bb = 0;
				*dest_p = (rr|gg|bb);

				r += r4;	r5 += r3;	if( r5>h ) { r5-=h; r +=fr; }
				g += g4;	g5 += g3;	if( g5>h ) { g5-=h; g +=fg; }
				b += b4;	b5 += b3;	if( b5>h ) { b5-=h; b +=fb; }

				wcnt += h;
				dest_p+=fw;
			}
			wcnt -= w;
		}
	}

	return 1;
}






BOOL PRM_DrawGLine_H_Mul( BMP_H *dest,	int x1, int y1, int r1, int g1, int b1,
										int x2, int y2, int r2, int g2, int b2 )
{
	int		x,y;
	WORD	*dest_p = dest->buf + y1*dest->sx + x1;
	int		r = r1, g = g1, b = b1;
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
				*dest_p = BrightTable_HR[r][ (*dest_p&rgb16.mask_r)>>rgb16.pos_r ]
						| BrightTable_HG[g][ (*dest_p&rgb16.mask_g)>>rgb16.pos_g ]
						| BrightTable_HB[b][ (*dest_p&rgb16.mask_b)>>rgb16.pos_b ];

				r += r4;	r5 += r3;	if( r5>h ) { r5-=h; r +=fr; }
				g += g4;	g5 += g3;	if( g5>h ) { g5-=h; g +=fg; }
				b += b4;	b5 += b3;	if( b5>h ) { b5-=h; b +=fb; }

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
				*dest_p = BrightTable_HR[r][ (*dest_p&rgb16.mask_r)>>rgb16.pos_r ]
						| BrightTable_HG[g][ (*dest_p&rgb16.mask_g)>>rgb16.pos_g ]
						| BrightTable_HB[b][ (*dest_p&rgb16.mask_b)>>rgb16.pos_b ];

				r += r4;	r5 += r3;	if( r5>h ) { r5-=h; r +=fr; }
				g += g4;	g5 += g3;	if( g5>h ) { g5-=h; g +=fg; }
				b += b4;	b5 += b3;	if( b5>h ) { b5-=h; b +=fb; }

				wcnt += h;
				dest_p+=fw;
			}
			wcnt -= w;
		}
	}

	return 1;
}





BOOL PRM_DrawGLine_H( BMP_H *dest,	int x1, int y1, int r1, int g1, int b1,
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
		case DRW_NML:	ret = PRM_DrawGLine_H_Std( dest, x1, y1, r1, g1, b1, x2, y2, r2, g2, b2 );			break;	
		case DRW_BLD2:	ret = PRM_DrawGLine_H_Bld( dest, x1, y1, r1, g1, b1, x2, y2, r2, g2, b2, blnd );	break;	
		case DRW_ADD:	ret = PRM_DrawGLine_H_Add( dest, x1, y1, r1, g1, b1, x2, y2, r2, g2, b2 );			break;	
		case DRW_SUB:	ret = PRM_DrawGLine_H_Sub( dest, x1, y1, r1, g1, b1, x2, y2, r2, g2, b2 );			break;	
		case DRW_MUL:	ret = PRM_DrawGLine_H_Mul( dest, x1, y1, r1, g1, b1, x2, y2, r2, g2, b2 );			break;	
		case DRW_AMI2:			break;	
		case DRW_NEG:			break;	
		case DRW_VIV2:			break;	
	}

	return ret;
}
