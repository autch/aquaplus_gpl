
 
  
   
    
     
      

#include "mm_std.h"

#include "bmp.h"
#include "draw.h"
#include "palette.h"

int	AlphaFormat=ALPHA_ALL;


int	DirectDrawFlag=0;


const char MeshTable[16][4] = {
	{ 0,0,0,0 },	
 	{ 0,0,0,1 },	
 	{ 0,0,1,0 },	
 	{ 0,0,1,1 },	
 	{ 0,1,0,0 },	
	{ 0,1,0,1 },	
	{ 0,1,1,0 },	
 	{ 0,1,1,1 },	
	{ 1,0,0,0 },	
 	{ 1,0,0,1 },	
 	{ 1,0,1,0 },	
 	{ 1,0,1,1 },	
 	{ 1,1,0,0 },	
 	{ 1,1,0,1 },	
 	{ 1,1,1,0 },	
 	{ 1,1,1,1 },	
};


WORD MeshFadePatern[17] = {

	0x0000,	
	0x1000,	
	0x1040,	
	0x5040,	

	0x5050,	
	0x5250,	
	0x5258,	
	0x5a58,	

	0x5a5a,	
	0x7a5a,	
	0x7ada,	
	0xfada,	

	0xfafa,	
	0xfbfa,	
	0xfbfe,	
	0xfffe,	

	0xffff,	

};



int	DispWidth  = 640;
int	DispHeight = 448;
void DRW_SetDispWH( int disp_w, int disp_h )
{
	DispWidth  = disp_w;
	DispHeight = disp_h;

	DRW_SetClipRectDefault( 0, 0, disp_w, disp_h );
}





RECT	ClipRectDef={ 0, 0, 640, 480 };	

BOOL DRW_SetClipRectDefault( int l, int t, int r, int b )
{
	if( l >= r ) return FALSE;
	if( t >= b ) return FALSE;
	
	ClipRectDef.left   = l;
	ClipRectDef.top    = t;
	ClipRectDef.right  = r;
	ClipRectDef.bottom = b;

	return TRUE;
}



BOOL ClipRect(	int *dest_px, int *dest_py, int dest_sx, int dest_sy,
				int *src_px,  int *src_py,  int src_sx,  int src_sy,
				int *w, int *h, RECT *clip )
{
	int		cx1,cx2,cy1,cy2;
	if( clip==NULL ) { clip = &ClipRectDef; }
	cx1 = max( 0,       clip->left   );
	cx2 = min( dest_sx, clip->right  );

	cy1 = max( 0,       clip->top    );
	cy2 = min( dest_sy, clip->bottom );


	if( *dest_px < cx1 ){			
		*w      -= cx1-*dest_px;
		*src_px += cx1-*dest_px;
		*dest_px = cx1;
	}
	if( *dest_px+*w > cx2 )	
		*w = cx2 - *dest_px;

	if( *src_px    > src_sx ) return FALSE;	
	if( *src_px+*w < 0      ) return FALSE;	
	
	if( *src_px < 0 ){			
		*w      += *src_px;
		*dest_px += -*src_px;
		*src_px = 0;

	}
	if( *src_px+*w > src_sx ){	
		*w = src_sx - *src_px;

	}

	if( *dest_py < cy1 ){			
		*h      -= cy1-*dest_py;
		*src_py += cy1-*dest_py;
		*dest_py = cy1;
	}
	if( *dest_py+*h > cy2 )	
		*h = cy2 - *dest_py;

	if( *src_py > src_sy )	return FALSE;	
	if( *src_py+*h < 0 )	return FALSE;	
	
	if( *src_py < 0 ){			
		*h      += *src_py;
		*dest_py += -*src_py;
		*src_py = 0;

	}
	if( *src_py+*h > src_sy ){	
		*h = src_sy - *src_py;

	}

	if( *w<=0 ){ *w = 0; return FALSE; }
	if( *h<=0 ){ *h = 0; return FALSE; }
	if( *dest_px<cx1 || *dest_px+*w>cx2    )	DebugPrintf("まぢデスか？(ClipRect)\n");
	if( *dest_py<cy1 || *dest_py+*h>cy2    )	DebugPrintf("まぢデスか？(ClipRect)\n");
	if( *src_px <0   || *src_px +*w>src_sx )	DebugPrintf("まぢデスか？(ClipRect)\n");
	if( *src_py <0   || *src_py +*h>src_sy )	DebugPrintf("まぢデスか？(ClipRect)\n");

	return TRUE;
}



BOOL ClipRectS2(int *dest_px, int *dest_py, int dest_sx, int dest_sy,
				int *src1_px, int *src1_py, int src1_sx, int src1_sy, int *w1, int *h1,
				int *src2_px, int *src2_py, int src2_sx, int src2_sy, int *w2, int *h2, RECT *clip )
{
	int		cx1,cx2,cy1,cy2;

	if( clip==NULL ) { clip = &ClipRectDef; }

	cx1 = max( 0,       clip->left   );
	cx2 = min( dest_sx, clip->right  );
	cy1 = max( 0,       clip->top    );
	cy2 = min( dest_sy, clip->bottom );

	if( *dest_px     > cx2 )	return FALSE;	
	if( *dest_px+*w1 < cx1 )	return FALSE;	
	if( *dest_px+*w2 < cx1 )	return FALSE;	
	
	if( *dest_px < cx1 ){			
		*w1      -= cx1-*dest_px;
		*w2      -= cx1-*dest_px;
		*src1_px += cx1-*dest_px;
		*src2_px += cx1-*dest_px;
		*dest_px  = cx1;
	}
	if( *dest_px+*w1 > cx2 ){	
		*w1 = cx2 - *dest_px;
	}
	if( *dest_px+*w2 > cx2 ){	
		*w2 = cx2 - *dest_px;
	}

	if( *src1_px    > src1_sx ) { DebugPrintf("画像領域指定エラー\n"); return FALSE;}	
	if( *src1_px+*w1 < 0       ) { DebugPrintf("画像領域指定エラー\n"); return FALSE;}	
	if( *src2_px    > src2_sx ) { DebugPrintf("画像領域指定エラー\n"); return FALSE;}	
	if( *src2_px+*w2 < 0       ) { DebugPrintf("画像領域指定エラー\n"); return FALSE;}	
	
	if( *src1_px < 0 ){			
		*w1     += *src1_px;
		*src1_px = 0;
		DebugPrintf("画像領域指定エラー\n");
	}
	if( *src1_px+*w1 > src1_sx ){	
		*w1 = src1_sx - *src1_px;
		DebugPrintf("画像領域指定エラー\n");
	}
	if( *src2_px < 0 ){			
		*w2      += *src2_px;
		*src2_px = 0;
		DebugPrintf("画像領域指定エラー\n");
	}
	if( *src2_px+*w2 > src2_sx ){	
		*w2 = src2_sx - *src2_px;
		DebugPrintf("画像領域指定エラー\n");
	}

	if( *dest_py     > cy2 )	return FALSE;	
	if( *dest_py+*h1 < cy1 )	return FALSE;	
	if( *dest_py+*h2 < cy1 )	return FALSE;	
	
	if( *dest_py < cy1 ){			
		*h1       -= cy1-*dest_py;
		*h2       -= cy1-*dest_py;
		*src1_py += cy1-*dest_py;
		*src2_py += cy1-*dest_py;
		*dest_py  = cy1;
	}
	if( *dest_py+*h1 > cy2 ){	
		*h1 = cy2 - *dest_py;
	}
	if( *dest_py+*h2 > cy2 ){	
		*h2 = cy2 - *dest_py;
	}

	if( *src1_py > src1_sy)	{ DebugPrintf("画像領域指定エラー\n"); return FALSE;}	
	if( *src1_py+*h1 < 0 )	{ DebugPrintf("画像領域指定エラー\n"); return FALSE;}	
	if( *src2_py > src2_sy)	{ DebugPrintf("画像領域指定エラー\n"); return FALSE;}	
	if( *src2_py+*h2 < 0 )	{ DebugPrintf("画像領域指定エラー\n"); return FALSE;}	
	
	if( *src1_py < 0 ){			
		*h1      += *src1_py;
		*src1_py = 0;
		DebugPrintf("画像領域指定エラー\n");
	}
	if( *src1_py+*h1 > src1_sy ){	
		*h1 = src1_sy - *src1_py;
		DebugPrintf("画像領域指定エラー\n");
	}
	if( *src2_py < 0 ){			
		*h2      += *src2_py;
		*src2_py = 0;
		DebugPrintf("画像領域指定エラー\n");
	}
	if( *src2_py+*h2 > src2_sy ){	
		*h2 = src2_sy - *src2_py;
		DebugPrintf("画像領域指定エラー\n");
	}

	if( *w1==0 || *h1==0 || *w2==0 || *h2==0 ) return FALSE;

	if( *dest_px<cx1 || *dest_px+*w1>cx2     )	DebugPrintf("まぢデスか？(ClipRectS2)\n");
	if( *dest_py<cy1 || *dest_py+*h1>cy2     )	DebugPrintf("まぢデスか？(ClipRectS2)\n");
	if( *dest_px<cx1 || *dest_px+*w2>cx2     )	DebugPrintf("まぢデスか？(ClipRectS2)\n");
	if( *dest_py<cy1 || *dest_py+*h2>cy2     )	DebugPrintf("まぢデスか？(ClipRectS2)\n");

	if( *src1_px<0   || *src1_px+*w1>src1_sx )	DebugPrintf("まぢデスか？(ClipRectS2)\n");
	if( *src1_py<0   || *src1_py+*h1>src1_sy )	DebugPrintf("まぢデスか？(ClipRectS2)\n");
	if( *src2_px<0   || *src2_px+*w2>src2_sx )	DebugPrintf("まぢデスか？(ClipRectS2)\n");
	if( *src2_py<0   || *src2_py+*h2>src2_sy )	DebugPrintf("まぢデスか？(ClipRectS2)\n");
	
	return TRUE;
}



BOOL ClipRectZ( int *sx, int *sy, int *sw, int *sh, int *dx, int *dy, int *dw, int *dh, int *xx, int *yy,
			   int dest_sx, int dest_sy, int src_sx, int src_sy, RECT *clip )
{
	int a;
	int x1, y1, w1, h1;
	int x2, y2, w2, h2;
	int	clip_r;
	int	clip_u;
	int	clip_l;
	int	clip_d;

	*xx = 0;
	*yy = 0;
	if( clip==NULL ) { clip = &ClipRectDef; }

	clip_l = max( 0,       clip->left );
	clip_r = min( dest_sx, clip->right );

		clip_u = max( 0,       clip->top );
		clip_d = min( dest_sy, clip->bottom );

	x1 = *sx; y1 = *sy;
	w1 = *sw; h1 = *sh;
	x2 = *dx; y2 = *dy;
	w2 = *dw; h2 = *dh;
	
	if( x2>clip_r || y2>clip_d || x2+w2<clip_l || y2+h2<clip_u) return FALSE;

	if( *dx<clip_l) {
		a = *sw * (clip_l-*dx) / *dw;
		*xx = *sw * (clip_l-*dx) % (*dw);

		x1 += a;
		w1 -= a;
		x2 += (clip_l-*dx);
		w2 -= (clip_l-*dx);
	}

	if( *dx+*dw>clip_r) {
		a = *sw * ( (*dx+*dw) - clip_r) / *dw;
		w1 -= a;
		w2 -= (*dx+*dw) - clip_r;
	}

	if( *dy<clip_u) {
		a = *sh * (clip_u-*dy) / *dh;
		*yy = *sh * (clip_u-*dy) % (*dh);

		y1 += a;
		h1 -= a;

		y2 += (clip_u-*dy);
		h2 -= (clip_u-*dy);
	}

	if( *dy+*dh>clip_d) {
		a = *sh * ( (*dy+*dh) - clip_d) / *dh;
		h1 -= a;
		h2 -= (*dy+*dh) - clip_d;
	}

	if( x1<0 ) {
		x2 -= w2*x1/w1;
		w2 = w2*(w1+x1)/w1;
		w1 += x1;
		x1 = 0;
	}
	if( x1+w1>src_sx ){
		w2 -= w2*(x1+w1-src_sx)/w1;
		w1 -= x1+w1 - src_sx;
	}
	if( y1<0 ) {
		y2 -= h2*y1/h1;
		h2 = h2*(h1+y1)/h1;
		h1 += y1;
		y1 = 0;
	}
	if( y1+h1>src_sy ){
		h2 -= h2*(y1+h1-src_sy)/h1;
		h1 -= y1+h1 - src_sy;
	}

	if( !w1 || !h1 || !w2 || !h2) return FALSE;




	*sx = x1; *sy = y1; *sw = w1; *sh = h1;
	*dx = x2; *dy = y2; *dw = w2; *dh = h2;

	if( *dx<clip_l || *dx+*dw>clip_r )	DebugPrintf("まぢデスか？(ClipRectZ)\n");
	if( *dy<clip_u || *dy+*dh>clip_d )	DebugPrintf("まぢデスか？(ClipRectZ)\n");



	return TRUE;
}



BOOL ClipRect2( int *dest_px, int *dest_py, int *w, int *h, int dest_sx, int dest_sy, RECT *clip )
{
	int		cx1,cx2,cy1,cy2;
	if( clip==NULL ) { clip = &ClipRectDef; }

	cx1 = max( 0,       clip->left   );
	cx2 = min( dest_sx, clip->right  );

		cy1 = max( 0,       clip->top    );
		cy2 = min( dest_sy, clip->bottom );


	if( *dest_px    > cx2 )	return FALSE;	
	if( *dest_px+*w < cx1 )	return FALSE;	
	
	if( *dest_px < cx1 ){			
		*w      -= cx1-*dest_px;
		*dest_px = cx1;
	}
	if( *dest_px+*w > cx2 )			
		*w = cx2 - *dest_px;

	if( *dest_py    > cy2 )	return FALSE;	
	if( *dest_py+*h < cy1 )	return FALSE;	
	
	if( *dest_py < cy1 ){			
		*h      -= cy1-*dest_py;
		*dest_py = cy1;
	}
	if( *dest_py+*h > cy2 )			
		*h = cy2 - *dest_py;

	if( *w<=0 || *h<=0 ) return FALSE;

	if( *dest_px<cx1 || *dest_px+*w>cx2    )	DebugPrintf("まぢデスか？(ClipRect2)\n");
	if( *dest_py<cy1 || *dest_py+*h>cy2    )	DebugPrintf("まぢデスか？(ClipRect2)\n");
	
	return TRUE;
}



BOOL ClipRectLine( int *x1, int *y1, int *x2, int *y2, int dest_sx, int dest_sy, RECT *clip )
{
	int		cp_l, cp_r, cp_t, cp_b;
	int		w = *x2-*x1;
	int		h = *y2-*y1;
	int		x,y;

	if( clip==NULL ) { clip = &ClipRectDef; }

	cp_l = max( 0,       clip->left   );
	cp_r = min( dest_sx, clip->right  );

		cp_t = max( 0,       clip->top    );
		cp_b = min( dest_sy, clip->bottom );

	if( *x1<cp_l   && *x2<cp_l   )	return FALSE;
	if( *x1>cp_r-1 && *x2>cp_r-1 )	return FALSE;
	if( *y1<cp_t   && *y2<cp_t   )	return FALSE;
	if( *y1>cp_b-1 && *y2>cp_b-1 )	return FALSE;

	if( *x1<cp_l ) {
		x = cp_l - *x1;		*x1 += x;
		if(w){
			y = h * x / w;		*y1 += y;
		}else{
			return FALSE;
		}
		w = *x2-*x1;
		h = *y2-*y1;
	}
	if( *x1>cp_r-1 ) {
		x = cp_r-1 - *x1;	*x1 += x;
		if(w){
			y = h * x / w;		*y1 += y;
		}else{
			return FALSE;
		}
		w = *x2-*x1;
		h = *y2-*y1;
	}
	if( *x2<cp_l ) {
		x = cp_l - *x2;		*x2 += x;
		if(w){
			y = h * x / w;		*y2 += y;
		}else{
			return FALSE;
		}
		w = *x2-*x1;
		h = *y2-*y1;
	}
	if( *x2>cp_r-1 ) {
		x = cp_r-1 - *x2;	*x2 += x;
		if(w){
			y = h * x / w;		*y2 += y;
		}else{
			return FALSE;
		}
		w = *x2-*x1;
		h = *y2-*y1;
	}

	if( *y1<cp_t ) {
		y = cp_t - *y1;		*y1 += y;
		if(h){
			x = w * y / h;		*x1 += x;
		}else{
			return FALSE;
		}
		w = *x2-*x1;
		h = *y2-*y1;
	}
	if( *y1>cp_b-1 ) {
		y = cp_b-1 - *y1;	*y1 += y;
		if(h){
			x = w * y / h;		*x1 += x;
		}else{
			return FALSE;
		}
		w = *x2-*x1;
		h = *y2-*y1;
	}
	if( *y2<cp_t ) {
		y = cp_t - *y2;		*y2 += y;
		if(h){
			x = w * y / h;		*x2 += x;
		}else{
			return FALSE;
		}
		w = *x2-*x1;
		h = *y2-*y1;
	}
	if( *y2>cp_b-1 ) {
		y = cp_b-1 - *y2;	*y2 += y;
		if(h){
			x = w * y / h;		*x2 += x;
		}else{
			return FALSE;
		}
		w = *x2-*x1;
		h = *y2-*y1;
	}
	if( *x1<cp_l || *x1>=cp_r || *x2<cp_l || *x2>=cp_r || *y1<cp_t || *y1>=cp_b || *y2<cp_t || *y2>=cp_b ){
		return FALSE;
	}

	return TRUE;
}



BOOL ClipRectLineRGB( int *x1, int *y1, int *r1, int *g1, int *b1,
					  int *x2, int *y2, int *r2, int *g2, int *b2,
					  int dest_sx, int dest_sy, RECT *clip )
{
	int		cp_l, cp_r, cp_t, cp_b;
	int		w  = *x2-*x1;
	int		h  = *y2-*y1;
	int		rr = *r2-*r1;
	int		gg = *g2-*g1;
	int		bb = *b2-*b1;
	int		x,y;
	int		r,g,b;

	if( clip==NULL ) { clip = &ClipRectDef; }

	cp_l = max( 0,       clip->left   );
	cp_r = min( dest_sx, clip->right  );

		cp_t = max( 0,       clip->top    );
		cp_b = min( dest_sy, clip->bottom );

	if( *x1<cp_l   && *x2<cp_l   )	return FALSE;
	if( *x1>cp_r-1 && *x2>cp_r-1 )	return FALSE;
	if( *y1<cp_t   && *y2<cp_t   )	return FALSE;
	if( *y1>cp_b-1 && *y2>cp_b-1 )	return FALSE;

	if( *x1<cp_l ) {
		x = cp_l - *x1;		*x1 += x;
		if(w){
			y = h  * x / w;		*y1 += y;
			r = rr * x / w;		*r1 += r;
			g = gg * x / w;		*g1 += g;
			b = bb * x / w;		*b1 += b;
		}else{
			return FALSE;
		}
		w = *x2-*x1;
		h = *y2-*y1;
	}
	if( *x1>cp_r-1 ) {
		x = cp_r-1 - *x1;	*x1 += x;
		if(w){
			y = h  * x / w;		*y1 += y;
			r = rr * x / w;		*r1 += r;
			g = gg * x / w;		*g1 += g;
			b = bb * x / w;		*b1 += b;
		}else{
			return FALSE;
		}
		w = *x2-*x1;
		h = *y2-*y1;
	}
	if( *x2<cp_l ) {
		x = cp_l - *x2;		*x2 += x;
		if(w){
			y = h  * x / w;		*y2 += y;
			r = rr * x / w;		*r2 += r;
			g = gg * x / w;		*g2 += g;
			b = bb * x / w;		*b2 += b;
		}else{
			return FALSE;
		}
		w = *x2-*x1;
		h = *y2-*y1;
	}
	if( *x2>cp_r-1 ) {
		x = cp_r-1 - *x2;		*x2 += x;
		if(w){
			y = h  * x / w;		*y2 += y;
			r = rr * x / w;		*r2 += r;
			g = gg * x / w;		*g2 += g;
			b = bb * x / w;		*b2 += b;
		}else{
			return FALSE;
		}
		w = *x2-*x1;
		h = *y2-*y1;
	}

	if( *y1<cp_t ) {
		y = cp_t - *y1;		*y1 += y;
		if(h){
			x = w  * y / h;		*x1 += x;
			r = rr * y / h;		*r1 += r;
			g = gg * y / h;		*g1 += g;
			b = bb * y / h;		*b1 += b;
		}else{
			return FALSE;
		}
		w = *x2-*x1;
		h = *y2-*y1;
	}
	if( *y1>cp_b-1 ) {
		y = cp_b-1 - *y1;	*y1 += y;
		if(h){
			x = w  * y / h;		*x1 += x;
			r = rr * y / h;		*r1 += r;
			g = gg * y / h;		*g1 += g;
			b = bb * y / h;		*b1 += b;
		}else{
			return FALSE;
		}
		w = *x2-*x1;
		h = *y2-*y1;
	}
	if( *y2<cp_t ) {
		y = cp_t - *y2;	*y2 += y;
		if(h){
			x = w  * y / h;		*x2 += x;
			r = rr * y / h;		*r2 += r;
			g = gg * y / h;		*g2 += g;
			b = bb * y / h;		*b2 += b;
		}else{
			return FALSE;
		}
		w = *x2-*x1;
		h = *y2-*y1;
	}
	if( *y2>cp_b-1 ) {
		y = cp_b-1 - *y2;		*y2 += y;
		if(h){
			x = w  * y / h;		*x2 += x;
			r = rr * y / h;		*r2 += r;
			g = gg * y / h;		*g2 += g;
			b = bb * y / h;		*b2 += b;
		}else{
			return FALSE;
		}
		w = *x2-*x1;
		h = *y2-*y1;
	}
	if( *x1<cp_l || *x1>=cp_r || *x2<cp_l || *x2>=cp_r || *y1<cp_t || *y1>=cp_b || *y2<cp_t || *y2>=cp_b ){
		return FALSE;
	}

	return TRUE;
}










void DrawMinMax4Table( int x1, int y1, int x2, int y2, RECT *clip, short (*mm)[4] )
{
	int		x,y,i,j,k;
	int		ax, ay, dx, dy, cnt=0;

	x1 = LIM( x1, clip->left, clip->right-1 );
	x2 = LIM( x2, clip->left, clip->right-1 );
	y1 = LIM( y1, clip->top, clip->bottom-1 );
	y2 = LIM( y2, clip->top, clip->bottom-1 );

	dx = x1-x2; dy = y1-y2;
	if( dx<0) { ax = -1; dx *= -1;} else ax = 1;
	if( dy<0) { ay = -1; dy *= -1;} else ay = 1;
	x = x2; y = y2;

		for( i=0; i<=dy; i++ ){
			for( j=0; j<4; j++ ){
				if( mm[y][j] > x ){
					for( k=3; k>j; k--) mm[y][k] = mm[y][k-1];
					mm[y][j] = x;
				}
			}
			y+=ay;
			cnt+=dx;
			if( cnt>=dy) {
				x+=ax;
				cnt-=dy;
			}
		}

}




void DrawMinMaxTable( int x1, int y1, int x2, int y2, RECT *clip, MIN_MAX mi[], MIN_MAX ma[] )
{
	int		x,y,i;
	int		ax, ay, dx, dy, cnt=0;
	int		cy1, cy2;

	cy1 = clip->top;
	cy2 = clip->bottom;

	dx = x1-x2; dy = y1-y2;
	if( dx<0) { ax = -1; dx *= -1;} else ax = 1;
	if( dy<0) { ay = -1; dy *= -1;} else ay = 1;
	x = x2; y = y2;
	if(dx<dy){
		for( i=0; i<dy; i++) {
			if( cy1 <= y && y < cy2 ){
				if( mi[y].x > x ){	mi[y].x = x;	}
				if( ma[y].x < x ){	ma[y].x = x;	}
			}
			y+=ay;
			cnt+=dx;
			if( cnt>=dy) {
				x+=ax;
				cnt-=dy;
			}
		}
	}else{
		for( i=0; i<dx; i++) {
			if( cy1 <= y && y < cy2 ){
				if( mi[y].x > x ){	mi[y].x = x;	}
				if( ma[y].x < x ){	ma[y].x = x;	}
			}
			x+=ax;
			cnt+=dy;
			if( cnt>=dx) {
				y+=ay;
				cnt -= dx;
			}
		}
	}
}



void DrawMinMaxTableRGB( int x1, int y1, int r1, int g1, int b1,
					    int x2, int y2, int r2, int g2, int b2, RECT *clip, MIN_MAX mi[], MIN_MAX ma[] )
{
	int		x,y,i;
	int		ax, ay, dx, dy, cnt=0;
	int		rcnt=0,gcnt=0,bcnt=0;
	int		r,g,b;
	int		ar,ag,ab;
	int		dr,dg,db;
	int		dr1,dg1,db1;
	int		dr2,dg2,db2;
	int		cy1, cy2;

	cy1 = clip->top;
	cy2 = clip->bottom;

	dx = x1-x2;	dy = y1-y2;
	if( dx==0 && dy==0 ) return ;

	if( dx<0) { ax = -1; dx *= -1; } else { ax = 1; }
	if( dy<0) { ay = -1; dy *= -1; } else { ay = 1; }
	x = x2; y = y2;

	dr = r1-r2;		dg = g1-g2;		db = b1-b2;
	r   = r2;		g   = g2;		b   = b2;
	if(dx<dy){
		dr1 = dr/dy;	dg1 = dg/dy;	db1 = db/dy;
		if( dr<0) { ar = -1; dr *= -1; } else { ar = 1; }
		if( dg<0) { ag = -1; dg *= -1; } else { ag = 1; }
		if( db<0) { ab = -1; db *= -1; } else { ab = 1; }
		dr2 = dr%dy;	dg2 = dg%dy;	db2 = db%dy;

		for( i=0; i<dy; i++) {
			if( cy1 <= y && y < cy2 ){
				if( mi[y].x > x ) {
					mi[y].x = x;
					mi[y].rgb.r   = r;
					mi[y].rgb.g = g;
					mi[y].rgb.b  = b;
				}
				if( ma[y].x < x ) {
					ma[y].x = x;
					ma[y].rgb.r   = r;
					ma[y].rgb.g = g;
					ma[y].rgb.b  = b;
				}
			}
			y+=ay;
			
			cnt += dx;	if( cnt >=dy ) {  cnt-=dy; x+=ax; }
			rcnt+= dr2;	if( rcnt>=dy ) { rcnt-=dy; r+=ar; }	r+=dr1;
			gcnt+= dg2;	if( gcnt>=dy ) { gcnt-=dy; g+=ag; }	g+=dg1;
			bcnt+= db2;	if( bcnt>=dy ) { bcnt-=dy; b+=ab; }	b+=db1;
		}
	}else{
		dr1 = dr/dx;	dg1 = dg/dx;	db1 = db/dx;
		if( dr<0) { ar = -1; dr *= -1; } else { ar = 1; }
		if( dg<0) { ag = -1; dg *= -1; } else { ag = 1; }
		if( db<0) { ab = -1; db *= -1; } else { ab = 1; }
		dr2 = dr%dx;	dg2 = dg%dx;	db2 = db%dx;

		for( i=0; i<dx; i++) {
			if( cy1 <= y && y < cy2 ){
				if( mi[y].x > x ) {
					mi[y].x = x;
					mi[y].rgb.r   = r;
					mi[y].rgb.g = g;
					mi[y].rgb.b  = b;
				}
				if( ma[y].x < x ) {
					ma[y].x = x;
					ma[y].rgb.r   = r;
					ma[y].rgb.g = g;
					ma[y].rgb.b  = b;
				}
			}
			x+=ax;

			cnt += dy;	if( cnt >=dx ) { cnt -=dx; y+=ay; }
			rcnt+= dr2;	if( rcnt>=dx ) { rcnt-=dx; r+=ar; }	r+=dr1;
			gcnt+= dg2;	if( gcnt>=dx ) { gcnt-=dx; g+=ag; }	g+=dg1;
			bcnt+= db2;	if( bcnt>=dx ) { bcnt-=dx; b+=ab; }	b+=db1;
		}
	}
}




void DrawMinMaxTableSrc(int x1, int y1, int sx1, int sy1,
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










BOOL DRW_ChangeBMP_B2H( BMP_H *dest, BMP_BT *src )
{
	int		x,y;
	int		w = dest->sx;
	int		h = dest->sy;
	WORD	*dest_p = dest->buf;
	char	*src_p  = src->buf;
	WORD	pal[256];

	if( dest->buf==NULL ) { DebugPrintf("描画領域がありません[DRW_ChangeBMP_B2H]\n"); return FALSE; }
	if( src->buf==NULL  ) { DebugPrintf("画像領域がありません[DRW_ChangeBMP_B2H]\n"); return FALSE; }



	for( y=0; y < h ; y++ ){
		for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
			*dest_p = pal[ *src_p ];
		}
	}

	return TRUE;
}



BOOL DRW_ChangeBMP_B2F( BMP_F *dest, BMP_BT *src )
{
	int		x,y;
	int		w = dest->sx;
	int		h = dest->sy;
	RGB24	*dest_p = dest->buf;
	char	*src_p  = src->buf;

	if( dest->buf==NULL ) { DebugPrintf("描画領域がありません[DRW_ChangeBMP_B2F]\n"); return FALSE; }
	if( src->buf==NULL  ) { DebugPrintf("画像領域がありません[DRW_ChangeBMP_B2F]\n"); return FALSE; }

	for( y=0; y < h ; y++ ){
		for( x=0 ; x < w-1 ; x++, dest_p++, src_p++ ){
			*(RGB32*)dest_p = GlobalPal[ *src_p ];
		}
		*dest_p = *(RGB24*)&GlobalPal[ *src_p ];
		dest_p++, src_p++;
	}
	return TRUE;
}



BOOL DRW_ChangeBMP_B2T( BMP_T *dest, BMP_BT *src )
{
	int		x,y;
	int		w = dest->sx;
	int		h = dest->sy;
	RGB32	*dest_p = dest->buf;
	char	*src_p  = src->buf;

	if( dest->buf==NULL ) { DebugPrintf("描画領域がありません[DRW_ChangeBMP_B2T]\n"); return FALSE; }
	if( src->buf==NULL  ) { DebugPrintf("画像領域がありません[DRW_ChangeBMP_B2T]\n"); return FALSE; }

	for( y=0; y < h ; y++ ){
		for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
			*dest_p = GlobalPal[ *src_p ];
		}
	}
	return TRUE;
}




BOOL DRW_ChangeBMP_H2B( BMP_BT *dest, BMP_H *src )
{
	int		x,y;
	int		w = dest->sx;
	int		h = dest->sy;
	char	*dest_p = dest->buf;
	WORD	*src_p  = src->buf;

	if( dest->buf==NULL ) { DebugPrintf("描画領域がありません[DRW_ChangeBMP_H2B]\n"); return FALSE; }
	if( src->buf==NULL  ) { DebugPrintf("画像領域がありません[DRW_ChangeBMP_H2B]\n"); return FALSE; }

	for( y=0; y < h ; y++ ){
		for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
			*dest_p = PaletteIndex[ (*src_p) ];
		}
	}
	return TRUE;
}



BOOL DRW_ChangeBMP_H2F( BMP_F *dest, BMP_H *src )
{
	int		x, y;
	int		w = dest->sx;
	int		h = dest->sy;
	RGB24	*dest_p = dest->buf;
	WORD	*src_p  = src->buf;

	
	if( dest->buf==NULL ) { DebugPrintf("描画領域がありません[DRW_ChangeBMP_H2F]\n"); return FALSE; }
	if( src->buf==NULL  ) { DebugPrintf("画像領域がありません[DRW_ChangeBMP_H2F]\n"); return FALSE; }
	
	for( y=0; y < h ; y++ ){
		for( x=0 ; x < w-1 ; x++, dest_p++, src_p++ ){
			*(RGB32*)dest_p = H2F_Tbl[*src_p];
		}
		*dest_p = *(RGB24*)&H2F_Tbl[*src_p];
		dest_p++, src_p++;
	}
	return TRUE;
}



BOOL DRW_ChangeBMP_H2T( BMP_T *dest, BMP_H *src, int alph )
{
	int		x, y;
	int		w = dest->sx;
	int		h = dest->sy;
	RGB32	*dest_p = dest->buf;
	WORD	*src_p  = src->buf;

	
	if( dest->buf==NULL ) { DebugPrintf("描画領域がありません[DRW_ChangeBMP_H2T]\n"); return FALSE; }
	if( src->buf==NULL  ) { DebugPrintf("画像領域がありません[DRW_ChangeBMP_H2T]\n"); return FALSE; }
	
	if(alph){
		char	*alp_p = src->alp;
		for( y=0; y < h ; y++ ){
			for( x=0 ; x < w ; x++, dest_p++, src_p++, alp_p++ ){
				*dest_p = H2F_Tbl[ *src_p ];
				dest_p->a = *alp_p;
			}
		}
	}else{
		for( y=0; y < h ; y++ ){
			for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
				*dest_p = H2F_Tbl[ *src_p ];
			}
		}
	}
	return TRUE;
}





BOOL DRW_ChangeBMP_F2B( BMP_BT *dest, BMP_F *src )
{
	int		x,y;
	int		w = dest->sx;
	int		h = dest->sy;
	char	*dest_p = dest->buf;
	RGB24	*src_p  = src->buf;

	if( dest->buf==NULL ) { DebugPrintf("描画領域がありません[DRW_ChangeBMP_F2B]\n"); return FALSE; }
	if( src->buf==NULL  ) { DebugPrintf("画像領域がありません[DRW_ChangeBMP_F2B]\n"); return FALSE; }

	for( y=0; y < h ; y++ ){
		for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
			*dest_p = PaletteIndex[ (src_p->r>>3<<10) | (src_p->g>>3<<5) | (src_p->b>>3) ];
		}
	}
	
	return TRUE;
}




BOOL DRW_ChangeBMP_F2H( BMP_H *dest, BMP_F *src )
{
	int		x,y;
	int		w = dest->sx;
	int		h = dest->sy;
	WORD	*dest_p = dest->buf;
	RGB24	*src_p  = src->buf;

	if( dest->buf==NULL ) { DebugPrintf("描画領域がありません[DRW_ChangeBMP_F2H]\n"); return FALSE; }
	if( src->buf==NULL  ) { DebugPrintf("画像領域がありません[DRW_ChangeBMP_F2H]\n"); return FALSE; }

	for( y=0; y < h ; y++ ){
		for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
			*dest_p = F2H_TblBW[src_p->b] | F2H_TblGW[src_p->g] | F2H_TblRW[src_p->r];
		}
	}
	
	return TRUE;
}




BOOL DRW_ChangeBMP_F2T( BMP_T *dest, BMP_F *src )
{
	int		x,y;
	int		w = dest->sx;
	int		h = dest->sy;
	RGB32	*dest_p = dest->buf;
	RGB24	*src_p  = src->buf;

	if( dest->buf==NULL ) { DebugPrintf("描画領域がありません[DRW_ChangeBMP_F2T]\n"); return FALSE; }
	if( src->buf==NULL  ) { DebugPrintf("画像領域がありません[DRW_ChangeBMP_F2T]\n"); return FALSE; }

	for( y=0; y < h ; y++ ){
		for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
			*(DWORD*)dest_p = (*(DWORD*)src_p) & 0x00ffffff;
		}
	}
	return TRUE;
}





BOOL DRW_ChangeBMP_T2B( BMP_BT *dest, BMP_T *src )
{
	int		x,y;
	int		w = dest->sx;
	int		h = dest->sy;
	char	*dest_p = dest->buf;
	RGB32	*src_p  = src->buf;

	if( dest->buf==NULL ) { DebugPrintf("描画領域がありません[DRW_ChangeBMP_T2B]\n"); return FALSE; }
	if( src->buf==NULL  ) { DebugPrintf("画像領域がありません[DRW_ChangeBMP_T2B]\n"); return FALSE; }

	for( y=0; y < h ; y++ ){
		for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
			*dest_p = PaletteIndex[ (src_p->r>>3<<10) | (src_p->g>>3<<5) | (src_p->b>>3) ];
		}
	}
	return TRUE;
}



BOOL DRW_ChangeBMP_T2H( BMP_H *dest, BMP_T *src, int alph )
{
	int		x,y;
	int		w = dest->sx;
	int		h = dest->sy;
	WORD	*dest_p = dest->buf;
	RGB32	*src_p  = src->buf;

	if( dest->buf==NULL ) { DebugPrintf("描画領域がありません[DRW_ChangeBMP_T2H]\n"); return FALSE; }
	if( src->buf ==NULL ) { DebugPrintf("画像領域がありません[DRW_ChangeBMP_T2H]\n"); return FALSE; }

	if(alph){
		char	*alp_p = dest->alp;

		for( y=0; y < h ; y++ ){
			for( x=0 ; x < w ; x++, dest_p++, src_p++, alp_p++ ){
				*dest_p = F2H_TblBW[src_p->b] | F2H_TblGW[src_p->g] | F2H_TblRW[src_p->r];
				*alp_p  = src_p->a;
			}
		}
	}else{
		for( y=0; y < h ; y++ ){
			for( x=0 ; x < w ; x++, dest_p++, src_p++ ){
				*dest_p = F2H_TblBW[src_p->b] | F2H_TblGW[src_p->g] | F2H_TblRW[src_p->r];
			}
		}
	}
	return TRUE;
}



BOOL DRW_ChangeBMP_T2F( BMP_F *dest, BMP_T *src )
{
	int		x,y;
	int		w = dest->sx;
	int		h = dest->sy;
	RGB24	*dest_p = dest->buf;
	RGB32	*src_p  = src->buf;

	if( dest->buf==NULL ) { DebugPrintf("描画領域がありません[DRW_ChangeBMP_T2F]\n"); return FALSE; }
	if( src->buf==NULL  ) { DebugPrintf("画像領域がありません[DRW_ChangeBMP_T2F]\n"); return FALSE; }

	for( y=0; y < h ; y++ ){
		for( x=0 ; x < w-1 ; x++, dest_p++, src_p++ ){
			*(RGB32*)dest_p = *src_p;
		}
		*dest_p = *(RGB24*)src_p;
		dest_p++, src_p++;
	}
	
	return TRUE;
}








BMP_BT	DigitBmp;
HBITMAP	hDigitBmp;



BOOL DRW_InitDigit( HINSTANCE hinst, char *bitmap )
{
	int		dmy;
	POINT	pos;
	POINT	size;
	
	if(1){
		HBITMAP	hbmp = LoadBitmap( hinst, bitmap );
		HDC		hMem1 = CreateCompatibleDC(NULL);
		HDC		hMem2 = CreateCompatibleDC(NULL);

		SelectObject( hMem1, hbmp );

		DigitBmp.pal = (RGB32 *)GAlloc(4*256);
		DigitBmp.pal[0].r=0;
		DigitBmp.pal[0].g=255;
		DigitBmp.pal[0].b=0;
		DigitBmp.pal[1].r=255;
		DigitBmp.pal[1].g=255;
		DigitBmp.pal[1].b=255;
		BMP_GetImageVram8( NULL, &hMem2, &hDigitBmp, NULL, &DigitBmp, 128, 32 );

		BitBlt( hMem2, 0, 0, 128, 32,  hMem1, 0,0, SRCCOPY );

		DeleteDC(hMem1);
		DeleteDC(hMem2);
		DeleteObject( hbmp );
	}else{
		BMP_LoadBMP_X2B( "src\\bitmap1.bmp", &DigitBmp, &pos, &size, &dmy );
	}
	return 1;
}



BOOL DRW_TermDigit( void )
{
	
	if(1){
		GFree(DigitBmp.pal);
		ZeroMemory( &DigitBmp, sizeof(BMP_BT) );
		DeleteObject( hDigitBmp );
		hDigitBmp = NULL;
	}else{
		BMP_ReleaseBMP_BT( &DigitBmp );
	}

	return 1;
}



static BOOL DrawDigit( void *dest, int x, int y, int num, DWORD wparam, int draw_mode  )
{
	BOOL		ret=0;
	DRAW_OBJECT dobj;

	dobj = DRW_SetDrawObject( draw_mode/8-1, dest, x, y, 8, 8,
							  0, &DigitBmp, num*8, 8, 8, 8, 0,NULL, 0, 0, 0, 0,
							  BRT_NML, BRT_NML, BRT_NML, CHK_256, NULL, wparam, 0,0, 0 );

	switch( draw_mode ){
		default:

		case DISP_HIGH:	ret = DRW_DrawBMP_HB( dobj );	break;
		case DISP_FULL:	ret = DRW_DrawBMP_FB( dobj );	break;
		case DISP_TRUE:	ret = DRW_DrawBMP_TB( dobj );	break;
	}

	return ret;
}



static BOOL DrawDmoji( void *dest, int x, int y, char moji, DWORD wparam, int draw_mode )
{
	BOOL		ret=0;
	int			work,sx,sy;
	DRAW_OBJECT dobj;

	if( 'a' <= moji && moji <= 'z' ) moji = moji-32;
	if( !('!' <= moji && moji <= '_') ) return ret;
	work = moji-'!'+1;
	sx = work%16*8;
	sy = work/16*8;

	dobj = DRW_SetDrawObject( draw_mode/8-1, dest, x, y, 8, 8,
							  0, &DigitBmp, sx, sy, 8, 8, 0,NULL, 0, 0, 0, 0,
							  BRT_NML, BRT_NML, BRT_NML, CHK_256, NULL, wparam, 0,0, 0 );

	switch( draw_mode ){
		default:

		case DISP_HIGH:	ret = DRW_DrawBMP_HB( dobj );	break;
		case DISP_FULL:	ret = DRW_DrawBMP_FB( dobj );	break;
		case DISP_TRUE:	ret = DRW_DrawBMP_TB( dobj );	break;
	}
	return ret;
}




BOOL DRW_DrawDigit( void *dest, int x, int y, int digit, DWORD wparam, int draw_mode )
{
	int			zero = 0;
	int			dg=1, p=0, i;

	if( digit == 0 ){
		DrawDigit( dest, x, y, 0, wparam, draw_mode );
	}else{
		while( digit/dg ){
			dg*=10;
			p++;
		}

		if( digit > 0 ){	
			for( i=0; i<p ;i++ ){
				dg/=10;
				DrawDigit( dest, x+i*8, y, digit/dg%10, wparam, draw_mode );
			}
		}else{				
		}
	}

	return 1;
}




BOOL DRW_DrawDmoji( void *dest, int x, int y, char *str, DWORD wparam, int draw_mode )
{
	int			p=0;

	while( *str ){
		DrawDmoji( dest, x+p*8, y, *str, wparam, draw_mode );
		p++;
		str++;
	}
	return 1;
}

int	InterlessLine=0;




void DRW_DrawBMP( HWND hwnd, HDC MemDC, HBITMAP hbmp, int w, int h, int interless )
{
	HDC		hDC;
	RECT	WindowRect;

	if(hwnd==NULL || hbmp==NULL) return ;
	
	hDC = GetDC(hwnd);
	







	
	GetClientRect( hwnd, &WindowRect );
	WindowRect.right  = WindowRect.right  - WindowRect.left;
	WindowRect.bottom = WindowRect.bottom - WindowRect.top;

	PAL_Select( hDC );
	PAL_Realize( hDC );
	


	

	if(interless){
		int	y;
		interless++;
		for(y=0;y<WindowRect.bottom/interless;y++){
			BitBlt( hDC, 0, y*interless+InterlessLine, WindowRect.right, 1, MemDC, 0, y*interless+InterlessLine, SRCCOPY );
		}
		InterlessLine=(InterlessLine+1)%interless;
	}else{
		InterlessLine=0;

		StretchBlt( hDC, 0, 0, WindowRect.right, WindowRect.bottom, MemDC, 0,0, w, h, SRCCOPY );
	}

	

	ReleaseDC( hwnd, hDC );
}



void DRW_DrawBMP2( HWND hwnd, HBITMAP hbmp, HDC hMem, int dx, int dy, int sx, int sy )
{
	HDC		hDC;
	RECT	WindowRect;
	int		w,h;

	if(hwnd==NULL || hbmp==NULL) return ;
	
	hDC = GetDC(hwnd);
	


	
	GetClientRect( hwnd, &WindowRect );
	w = WindowRect.right  = WindowRect.right  - WindowRect.left;
	h = WindowRect.bottom = WindowRect.bottom - WindowRect.top;




	
	BitBlt( hDC,    0,    0,     dx,  h, NULL, 0, 0, BLACKNESS );
	BitBlt( hDC, w-dx,    0,     dx,  h, NULL, 0, 0, BLACKNESS );
	BitBlt( hDC,   dx,    0, w-dx*2, dy, NULL, 0, 0, BLACKNESS );
	BitBlt( hDC,   dx, h-dy, w-dx*2, dy, NULL, 0, 0, BLACKNESS );

	BitBlt( hDC, dx, dy, WindowRect.right, WindowRect.bottom,  hMem, sx, sy, SRCCOPY );


	

	ReleaseDC( hwnd, hDC );
}



void DRW_DrawBMP3( HWND hwnd, HBITMAP hbmp, int dx, int dy, int dw, int dh, int sx, int sy, int sw, int sh )
{
	HDC		hDC, hMem;

	if(hwnd==NULL || hbmp==NULL) return ;
	
	hDC = GetDC(hwnd);
	
	hMem = CreateCompatibleDC(hDC);
	SelectObject( hMem, hbmp );

	
	if(dw==sw && dh==sh )
		BitBlt( hDC, dx, dy, dw, dh,  hMem, sx, sy, SRCCOPY );
	else
		StretchBlt( hDC, dx, dy, dw, dh, hMem, sx, sy, sw, sh, SRCCOPY );

	
	DeleteDC(hMem);
	ReleaseDC( hwnd, hDC );
}
DRAW_OBJECT DRW_SetDrawObjectLocal(	int bppd, void *dest, int dx,  int dy,  int dw,  int dh,
									int bpps1, void *src1, int sx1, int sy1, int sw1, int sh1,
									int r, int g, int b, int nuki, RECT *clip, int param1, int rparam )
{
	return DRW_SetDrawObject(	bppd,  dest, dx,  dy,  dw,  dh,
								bpps1, src1, sx1, sy1, sw1, sh1,
								0, NULL, 0, 0, 0, 0,
								r, g, b, nuki, clip, param1, 0,0, rparam );
}
DRAW_OBJECT DRW_SetDrawObject(	int bppd, void *dest, int dx,  int dy,  int dw,  int dh,
								int bpps1, void *src1, int sx1, int sy1, int sw1, int sh1,
								int bpps2, void *src2, int sx2, int sy2, int sw2, int sh2,
								int r, int g, int b, int nuki, RECT *clip, int param1, int param2, int param3, int rparam )
{
	DRAW_OBJECT		dobj;

	dobj.bppd = bppd;
	dobj.dest = dest;
	dobj.dx   = dx;
	dobj.dy   = dy;
	dobj.dw   = dw;
	dobj.dh   = dh;

	dobj.bpps1 = bpps1;
	dobj.src1 = src1;
	dobj.s1x  = sx1;
	dobj.s1y  = sy1;
	dobj.s1w  = sw1;
	dobj.s1h  = sh1;

	dobj.bpps2 = bpps2;
	dobj.src2 = src2;
	dobj.s2x  = sx2;
	dobj.s2y  = sy2;
	dobj.s2w  = sw2;
	dobj.s2h  = sh2;

	dobj.r    = r;
	dobj.g    = g;
	dobj.b    = b;
	dobj.nuki = nuki;
	dobj.clip = clip;
	dobj.dparam  = LOWORD(param1);
	dobj.dnum    = HIWORD(param1);
	dobj.dparam2 = LOWORD(param2);
	dobj.dnum2   = HIWORD(param2);

	dobj.dnum3   = param3;

	dobj.rparam  = rparam;

	return dobj;
}
DRAW_OBJECT DRW_SetDrawObjectPrim(	void *dest,
									int dx1,  int dy1, int r1, int g1, int b1,
									int dx2,  int dy2, int r2, int g2, int b2,
									int dx3,  int dy3, int r3, int g3, int b3,
									int dx4,  int dy4, int r4, int g4, int b4,
									int dw, int dh,
									RECT *clip, int param1 )
{
	DRAW_OBJECT		dobj;

	dobj.dest = dest;

	dobj.dx  = dx1;
	dobj.dy  = dy1;
	dobj.r   = r1;
	dobj.g   = g1;
	dobj.b   = b1;

	dobj.dx2 = dx2;
	dobj.dy2 = dy2;
	dobj.r2  = r2;
	dobj.g2  = g2;
	dobj.b2  = b2;

	dobj.dx3 = dx3;
	dobj.dy3 = dy3;
	dobj.r3  = r3;
	dobj.g3  = g3;
	dobj.b3  = b3;

	dobj.dx4 = dx4;
	dobj.dy4 = dy4;
	dobj.r4  = r4;
	dobj.g4  = g4;
	dobj.b4  = b4;

	dobj.dw  = dw;
	dobj.dh  = dh;

	dobj.clip = clip;
	dobj.dparam = LOWORD(param1);
	dobj.dnum   = HIWORD(param1);

	return dobj;
}

void DRW_SetDrawObjectPoly( DRAW_OBJECT *dobj,
							int dx,  int dy,  int dx2,  int dy2,  int dx3,  int dy3,  int dx4,  int dy4,
							int sx1, int sy1, int sx12, int sy12, int sx13, int sy13, int sx14, int sy14 )
{
	dobj->dx   = dx;
	dobj->dx2  = dx2;
	dobj->dx3  = dx3;
	dobj->dx4  = dx4;

	dobj->dy   = dy;
	dobj->dy2  = dy2;
	dobj->dy3  = dy3;
	dobj->dy4  = dy4;

	dobj->s1x  = sx1;
	dobj->s1x2 = sx12;
	dobj->s1x3 = sx13;
	dobj->s1x4 = sx14;

	dobj->s1y  = sy1;
	dobj->s1y2 = sy12;
	dobj->s1y3 = sy13;
	dobj->s1y4 = sy14;
}

int DRW_GetStartPointerSrc1( DRAW_OBJECT *dobj )
{
	int	ret;
	int	sx=0,sy=0;
	switch(dobj->bpps1){
		default:
		case 0:	sx = ((BMP_BT*)dobj->src1)->sx;	sy = ((BMP_BT*)dobj->src1)->sy;	break;
		case 1:	sx = ((BMP_H*)dobj->src1)->sx;	sy = ((BMP_H*)dobj->src1)->sy;	break;
		case 2:	sx = ((BMP_F*)dobj->src1)->sx;	sy = ((BMP_F*)dobj->src1)->sy;	break;
		case 3:	sx = ((BMP_T*)dobj->src1)->sx;	sy = ((BMP_T*)dobj->src1)->sy;	break;
	}
	
	if( dobj->rparam&REV_H ){
		ret = (dobj->s1y + dobj->s1h-1) * sx;
	}else{
		ret = dobj->s1y * sx;
	}
	if( dobj->rparam&REV_W ){
		ret += dobj->s1x + dobj->s1w -1;
	}else{
		ret += dobj->s1x;
	}

	return ret;
}
int DRW_GetStartPointerEx( DRAW_OBJECT *dobj, int dx, int dy )
{
	int	ret;
	int	sx=0,sy=0;
	switch(dobj->bpps1){
		default:
		case 0:	sx = ((BMP_BT*)dobj->src1)->sx;	sy = ((BMP_BT*)dobj->src1)->sy;	break;
		case 1:	sx = ((BMP_H*)dobj->src1)->sx;	sy = ((BMP_H*)dobj->src1)->sy;	break;
		case 2:	sx = ((BMP_F*)dobj->src1)->sx;	sy = ((BMP_F*)dobj->src1)->sy;	break;
		case 3:	sx = ((BMP_T*)dobj->src1)->sx;	sy = ((BMP_T*)dobj->src1)->sy;	break;
	}
	
	if( dobj->rparam&REV_H ){
		ret = (sy - dobj->s1y - dy - 1) * sx;
	}else{
		ret = (dobj->s1y + dy) * sx;
	}
	if( dobj->rparam&REV_W ){
		ret += sx - dobj->s1x - dx - 1;
	}else{
		ret += dobj->s1x + dx;
	}

	return ret;
}

int DRW_GetStartPointerSrc2( DRAW_OBJECT *dobj )
{
	int	ret;
	int	sx=0,sy=0;
	switch(dobj->bpps2){
		default:
		case 0:	sx = ((BMP_BT*)dobj->src2)->sx;	sy = ((BMP_BT*)dobj->src2)->sy;	break;
		case 1:	sx = ((BMP_H*)dobj->src2)->sx;	sy = ((BMP_H*)dobj->src2)->sy;	break;
		case 2:	sx = ((BMP_F*)dobj->src2)->sx;	sy = ((BMP_F*)dobj->src2)->sy;	break;
		case 3:	sx = ((BMP_T*)dobj->src2)->sx;	sy = ((BMP_T*)dobj->src2)->sy;	break;
	}
	
	if( dobj->rparam&REV_H ){
		ret = (sy - dobj->s2y -1) * sx;
	}else{
		ret = dobj->s2y * sx;
	}
	if( dobj->rparam&REV_W ){
		ret += sx - dobj->s2x -1;
	}else{
		ret += dobj->s2x;
	}

	return ret;
}

void DRW_SetDrawObject2XXX( DRAW_OBJECT *dobj, DRAW_OBJECT *dobj2, DRAW_OBJECT *dobj3, DRAW_OBJECT *dobj4, DRAW_OBJECT *dobj5 )
{
	int	work;
	DRAW_OBJECT work1 = *dobj;
	DRAW_OBJECT work2 = *dobj;
	int			sw1,sh1,sw2,sh2;

	switch(dobj->bpps1){
		case 0:	sw1 = ((BMP_BT*)dobj->src1)->sx;sh1 = ((BMP_BT*)dobj->src1)->sy;break;
		case 1:	sw1 = ((BMP_H*)dobj->src1)->sx;	sh1 = ((BMP_H*)dobj->src1)->sy;	break;
		case 2:	sw1 = ((BMP_F*)dobj->src1)->sx;	sh1 = ((BMP_F*)dobj->src1)->sy;	break;
		case 3:	sw1 = ((BMP_T*)dobj->src1)->sx;	sh1 = ((BMP_T*)dobj->src1)->sy;	break;
	}
	switch(dobj->bpps2){
		case 0:	sw2 = ((BMP_BT*)dobj->src2)->sx;sh2 = ((BMP_BT*)dobj->src2)->sy;break;
		case 1:	sw2 = ((BMP_H*)dobj->src2)->sx;	sh2 = ((BMP_H*)dobj->src2)->sy;	break;
		case 2:	sw2 = ((BMP_F*)dobj->src2)->sx;	sh2 = ((BMP_F*)dobj->src2)->sy;	break;
		case 3:	sw2 = ((BMP_T*)dobj->src2)->sx;	sh2 = ((BMP_T*)dobj->src2)->sy;	break;
	}
	
	work1.dnum   = work1.dnum2;
	work1.dparam = work1.dparam2;
	if(work1.s1y < 0){
		work1.dy  -= work1.s1y;
		work1.dh  = work1.s1h += work1.s1y;
		work1.s1y = 0;
	}
	if(work1.s1y+work1.s1h > sh1){
		work1.dh = work1.s1h = sh1-work1.s1y;
	}
	if(work1.s1x < 0){
		work1.dx  -= work1.s1x;
		work1.dw = work1.s1w += work1.s1x;
		work1.s1x = 0;
	}
	if(work1.s1x+work1.s1w > sw1){
		work1.dw = work1.s1w = sw1-work1.s1x;
	}

	work2.s1x  = work2.s2x;
	work2.s1y  = work2.s2y;
	work2.s1w  = work2.s2w;
	work2.s1h  = work2.s2h;
	work2.src1 = work2.src2;
	work2.dnum   = 256-work1.dnum2;
	work2.dparam = work2.dparam2;
	if(work2.s1y < 0){
		work2.dy  -= work2.s1y;
		work2.dh  = work2.s1h += work2.s1y;
		work2.s1y = 0;
	}
	if(work2.s1y+work2.s1h > sh2){
		work2.dh = work2.s1h = sh2-work2.s1y;
	}
	if(work2.s1x < 0){
		work2.dx  -= work2.s1x;
		work2.dw = work2.s1w += work2.s1x;
		work2.s1x = 0;
	}
	if(work2.s1x+work2.s1w > sw2){
		work2.dw = work2.s1w = sw2-work2.s1x;
	}

	if( work1.dy > work2.dy ){
		*dobj2 = work2;
		work = work1.dy - work2.dy;
	}else{
		*dobj2 = work1;
		work = work2.dy - work1.dy;
	}
	dobj2->s1h = dobj2->dh = work;

	if( work1.dy+work1.dh > work2.dy+work2.dh ){
		*dobj3 = work1;
		work = work1.dy+work1.dh - (work2.dy+work2.dh);
	}else{
		*dobj3 = work2;
		work = work2.dy+work2.dh - (work1.dy+work1.dh);
	}
	dobj3->dy  += dobj3->dh-work;
	dobj3->s1y += dobj3->dh-work;
	dobj3->s1h = dobj3->dh = work;

	if( work1.dx > work2.dx ){
		*dobj4 = work2;
		work = work1.dx - work2.dx;
	}else{
		*dobj4 = work1;
		work = work2.dx - work1.dx;
	}
	dobj4->s1w = dobj4->dw = work;
	dobj4->s1y += dobj2->dy+dobj2->dh - dobj4->dy;
	dobj4->dy  += dobj2->dy+dobj2->dh - dobj4->dy;
	dobj4->s1h = dobj4->dh = dobj3->dy - (dobj2->dy+dobj2->dh);

	if( work1.dx+work1.dw > work2.dx+work2.dw ){
		*dobj5 = work1;
		work = work1.dx+work1.dw - (work2.dx+work2.dw);
	}else{						
		*dobj5 = work2;
		work = work2.dx+work2.dw - (work1.dx+work1.dw);
	}
	dobj5->s1x += dobj5->dw - work;
	dobj5->dx  += dobj5->dw - work;
	dobj5->s1w = dobj5->dw = work;

	dobj5->s1y += dobj4->dy-dobj5->dy;
	dobj5->dy   = dobj4->dy;
	dobj5->s1h = dobj5->dh = dobj4->dh;

	work = max(work1.dx,work2.dx);
	dobj->s1x += work-dobj->dx;
	dobj->s2x += work-dobj->dx;
	dobj->dx = work;
	work = max(work1.dy,work2.dy);
	dobj->s1y += work-dobj->dy;
	dobj->s2y += work-dobj->dy;
	dobj->dy = work;
	work = min(work1.dx+work1.dw,work2.dx+work2.dw);
	dobj->s1w = dobj->s2w = dobj->dw = work-dobj->dx;
	work = min(work1.dy+work1.dh,work2.dy+work2.dh);
	dobj->s1h = dobj->s2h = dobj->dh = work-dobj->dy;
}



