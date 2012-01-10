#include <windows.h>
#include <mmsystem.h>
#include <winnls32.h>

#define LIM(D,L,H)			(min(max((D),(L)),(H)))

#include "DrawPoly4.h"

static RGB32 BMP_ChangeBright_Full( RGB32 src, int r, int g, int b )
{
	DWORD		wr,wg,wb;
	DWORD		rr=r, gg=g, bb=b;

	wr = src.r;
	wg = src.g;
	wb = src.b;

	if( rr<=0x80 )	wr =  (      wr * rr      )>>7;
	else			wr = (((0xff-wr)*(rr-0x7f))>>7)+wr;

	if( gg<=0x80 )	wg =  (      wg * gg      )>>7;
	else			wg = (((0xff-wg)*(gg-0x7f))>>7)+wg;

	if( bb<=0x80 )	wb =  (      wb * bb      )>>7;
	else			wb = (((0xff-wb)*(bb-0x7f))>>7)+wb;

	src.r   = (BYTE)wr;
	src.g = (BYTE)wg;
	src.b  = (BYTE)wb;
	src.a = 0;

	return src;
}

static void BMP_ChangePalBright_Full( RGB32 *pal, int r, int g, int b )
{
	int		i;

	if( r==0x80 && g==0x80 && b==0x80 ) return ;

	for( i=0; i<256 ;i++ ){
		pal[i] = BMP_ChangeBright_Full( pal[i], r, g, b );
	}
}
typedef struct{
	int		x;
	RGB32	rgb;
	int		sx, sy;
}MIN_MAX;

static int	DispWidth  = 800;
static int 	DispHeight = 600;
static RECT	ClipRectDef={ 0, 0, 800, 600 };

static void DrawMinMaxTableSrc(int x1, int y1, int sx1, int sy1,
						int x2, int y2, int sx2, int sy2, RECT *clip, MIN_MAX mi[], MIN_MAX ma[] )
{
	int		x,y,i;
	int		ax, ay, dx, dy, cnt=0;
	int		sxcnt=0,sycnt=0;
	int		sx,sy;
	int		asx,asy;
	int		dsx,dsy;
	int		dsx1,dsy1;
	int		dsx2,dsy2;
	int		cy1, cy2;

	cy1 = clip->top;
	cy2 = clip->bottom;

	dx = x1-x2;	dy = y1-y2;
	if( dx==0 && dy==0 ) return ;


	if( dx<0) { ax = -1; dx *= -1; } else { ax = 1; }
	if( dy<0) { ay = -1; dy *= -1; } else { ay = 1; }
	x = x2; y = y2;

	dsx = sx1-sx2;	dsy = sy1-sy2;
	sx  = sx2;		sy  = sy2;
	if(dx<dy){
		dsx1 = dsx/dy;	dsy1 = dsy/dy;
		if( dsx<0) { asx = -1; dsx *= -1; } else { asx = 1; }
		if( dsy<0) { asy = -1; dsy *= -1; } else { asy = 1; }
		dsx2 = dsx%dy;	dsy2 = dsy%dy;
		
		for( i=0 ; i<dy ; i++ ){

			if( cy1 <= y && y < cy2 ){
				if( mi[y].x > x ) {
					mi[y].x = x;
					mi[y].sx = sx;
					mi[y].sy = sy;
				}
				if( ma[y].x < x+1 ) {
					ma[y].x = x+1;
					ma[y].sx = sx+1;
					ma[y].sy = sy;
				}
			}
			y+=ay;

			cnt += dx;		if( cnt >=dy  ) {  cnt-=dy;  x+=ax; }
			sxcnt+= dsx2;	if( sxcnt>=dy ) { sxcnt-=dy; sx+=asx; }	sx+=dsx1;
			sycnt+= dsy2;	if( sycnt>=dy ) { sycnt-=dy; sy+=asy; }	sy+=dsy1;
		}
	}else{
		dsx1 = dsx/dx;	dsy1 = dsy/dx;
		if( dsx<0) { asx = -1; dsx *= -1; } else { asx = 1; }
		if( dsy<0) { asy = -1; dsy *= -1; } else { asy = 1; }
		dsx2 = dsx%dx;	dsy2 = dsy%dx;
		
		for( i=0; i<dx; i++) {
			if( cy1 <= y && y < cy2 ){
				if( mi[y].x > x ) {
					mi[y].x = x;
					mi[y].sx = sx;
					mi[y].sy = sy;
				}
				if( ma[y].x < x+1 ) {
					ma[y].x = x+1;
					ma[y].sx = sx+1;
					ma[y].sy = sy;
				}
			}
			x+=ax;

			cnt += dy;		if( cnt >=dx  ) { cnt -=dx;  y+=ay; }
			sxcnt+= dsx2;	if( sxcnt>=dx ) { sxcnt-=dx; sx+=asx; }	sx+=dsx1;
			sycnt+= dsy2;	if( sycnt>=dx ) { sycnt-=dx; sy+=asy; }	sy+=dsy1;
		}
	}
}


static BOOL DRW_DrawXLine_FB_Std(	RGB24 *dest_p, BMP_BT *src,
							int sx,  int sy,  int psx, int psy,
							int dsx, int dsy, int fsx, int fsy, int  w, int ww,
							int nuki )
{
	int		ssx=0,ssy=0;
	int		x;
	BYTE	*src_p = src->buf + sy*src->sx + sx;
	RGB32	*spal = src->pal;

	psy = psy*src->sx + psx;
	fsy = fsy*src->sx;

	

	if( nuki==-1 ){
		for( x=0; x < w ; x++, dest_p++ ){
			*dest_p = *(RGB24*)&spal[*src_p];
			src_p += psy;
			ssx+=dsx;		ssy+=dsy;
			if( ssx >= ww ){ ssx -= ww; src_p += fsx; }
			if( ssy >= ww ){ ssy -= ww; src_p += fsy; }
		}
	}else{
		for( x=0; x < w ; x++, dest_p++ ){
			if( nuki != *src_p ){
				*dest_p = *(RGB24*)&spal[*src_p];
			}
			src_p += psy;
			ssx+=dsx;		ssy+=dsy;
			if( ssx >= ww ){ ssx -= ww; src_p += fsx; }
			if( ssy >= ww ){ ssy -= ww; src_p += fsy; }
		}
	}

	return TRUE;
}

static BOOL DRW_DrawXLine_FB( RGB24 *dest_p,int dy, BMP_BT *src, MIN_MAX *mi, MIN_MAX *ma, int nuki, RECT *clip, int param, int blnd )
{
	BOOL	ret = FALSE;
	int		dx1, dx2, w;
	int		xx, ww = ma->x - mi->x;
	int		sx,  sy,  sx2, sy2;
	int		dsx, dsy, fsx, fsy;

	if( mi->x == DispWidth )	return 1;
	if( ww==0 )			return 1;

	dx1 = max( mi->x, clip->left  );
	dx2 = min( ma->x, clip->right );

	w  = dx2-dx1;
	xx = dx1 - mi->x;
	dest_p += dx1;

	sx = ma->sx - mi->sx;
	sy = ma->sy - mi->sy;

	if(sx<0) { dsx = (-sx)%ww; fsx=-1; sx2 = LIM(sx*xx/ww + mi->sx-1,0,src->sx-1); }
	else	 { dsx =   sx %ww; fsx= 1; sx2 = LIM(sx*xx/ww + mi->sx,  0,src->sx-1); }

	if(sy<0) { dsy = (-sy)%ww; fsy=-1; sy2 = LIM(sy*xx/ww + mi->sy-1,0,src->sy-1); }
	else	 { dsy =   sy %ww; fsy= 1; sy2 = LIM(sy*xx/ww + mi->sy,  0,src->sy-1); }

	DRW_DrawXLine_FB_Std( dest_p, src, sx2, sy2, sx/ww, sy/ww, dsx, dsy, fsx, fsy, w, ww, nuki );
	return ret;
}

BOOL DRW_DrawPOLY4_FB( DRAW_OBJECT dobj )
{
	BOOL		ret=TRUE;
	BMP_F		*dest = (BMP_F *)dobj.dest;
	BMP_BT		*src  = (BMP_BT *)dobj.src1;
	RGB24		*dest_p = dest->buf;
	MIN_MAX		*mi = (MIN_MAX*)GlobalAlloc(GPTR,DispHeight*sizeof(MIN_MAX));
	MIN_MAX		*ma = (MIN_MAX*)GlobalAlloc(GPTR,DispHeight*sizeof(MIN_MAX));
	int			y;
	RGB32		pal[256];
	BMP_BT		src2={ src->buf, src->alp, pal, src->sx, src->sy };

	if( dest->buf==NULL ) {	ret=FALSE; goto end; }
	if( src->buf==NULL  ) {	ret=FALSE; goto end; }

	if( dobj.clip==NULL ) {
		dobj.clip = &ClipRectDef;
	}else if( dobj.clip==(void*)-1 ){
		dobj.clip = &ClipRectDef;
	}else{
		dobj.clip = &ClipRectDef;
	}

	for( y=0; y<DispHeight; y++ )	{	mi[y].x = DispWidth;	ma[y].x = 0;	}
	DrawMinMaxTableSrc( dobj.dx,  dobj.dy,  dobj.s1x,  dobj.s1y,	dobj.dx2, dobj.dy2, dobj.s1x2, dobj.s1y2, dobj.clip, mi, ma );
	DrawMinMaxTableSrc( dobj.dx2, dobj.dy2, dobj.s1x2, dobj.s1y2,	dobj.dx4, dobj.dy4, dobj.s1x4, dobj.s1y4, dobj.clip, mi, ma );
	DrawMinMaxTableSrc( dobj.dx4, dobj.dy4, dobj.s1x4, dobj.s1y4,	dobj.dx3, dobj.dy3, dobj.s1x3, dobj.s1y3, dobj.clip, mi, ma );
	DrawMinMaxTableSrc( dobj.dx3, dobj.dy3, dobj.s1x3, dobj.s1y3,	dobj.dx,  dobj.dy,  dobj.s1x,  dobj.s1y,  dobj.clip, mi, ma );

	if( dobj.r==128 && dobj.g==128 && dobj.b==128 ){
		src2.pal = src->pal;
	}else{
		memcpy( pal, src->pal, 256*4 );
		BMP_ChangePalBright_Full( pal, dobj.r, dobj.g, dobj.b );
	}

	for( y = dobj.clip->top ; y < dobj.clip->bottom ; y++, dest_p+=dest->sx ){
		DRW_DrawXLine_FB( dest_p, y, &src2, &mi[y], &ma[y], dobj.nuki, dobj.clip, dobj.dparam, dobj.dnum );
	}

end:
	if(mi){GlobalFree(mi); mi=NULL;}
	if(ma){GlobalFree(ma); ma=NULL;}

	return ret;
}


BOOL DRW_DrawPOLY4_FB_Simple( BMP_F *dest, BMP_BT *src,
							 int dx1, int dy1, int dx2, int dy2,
							 int dx3, int dy3, int dx4, int dy4, int r, int g, int b )
{
	DRAW_OBJECT	dobj;

	memset( &dobj, 0, sizeof(dobj) );
	dobj.dest = dest;
	dobj.dx  = dx1;		dobj.dy  = dy1;
	dobj.dx2 = dx2;		dobj.dy2 = dy2;
	dobj.dx3 = dx3;		dobj.dy3 = dy3;
	dobj.dx4 = dx4;		dobj.dy4 = dy4;


	dobj.src1  = src;
	dobj.s1x  = 0;			dobj.s1y  = 0;
	dobj.s1x2 = src->sx-1;	dobj.s1y2 = 0;
	dobj.s1x3 = 0;			dobj.s1y3 = src->sy-1;
	dobj.s1x4 = src->sx-1;	dobj.s1y4 = src->sy-1;

	dobj.r = r;
	dobj.g = g;
	dobj.b = b;

	dobj.nuki = -1;

	return DRW_DrawPOLY4_FB( dobj );
}
