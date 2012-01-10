
 
  
   
    
     
      

#include "mm_std.h"

#include "comp_pac.h"

#include "bmp.h"
#include "tga.h"


static char	*PackBmpDir=NULL;
static char	HighColorType = BIT16_555;


void BMP_SetPackDir( char *pac_dir )
{
	PackBmpDir = pac_dir;
}

char *BMP_GetPackDir( void )
{
	return PackBmpDir;
}

static BITMAPINFO3	bmi_8 = {
	{
		sizeof(BITMAPINFOHEADER),	
		640,			
		480,			
		1,				
		8,				
		BI_RGB,			
		640*480,	
		0,				
		0,				
		256,			
		0,				
	},

};

static BITMAPINFO2	bmi_16_4444 = {
	{
		sizeof(BITMAPINFOHEADER),	
		640,			
		480,			
		1,				
		16,				
		BI_BITFIELDS,	
		640*480*2,		
		0,				
		0,				
		0,				
		0,				
	},
	0x0F00,0x00F0,0x000F
};

static BITMAPINFO2	bmi_16_565 = {
	{
		sizeof(BITMAPINFOHEADER),	
		640,			
		480,			
		1,				
		16,				
		BI_BITFIELDS,	
		640*480*2,		
		0,				
		0,				
		0,				
		0,				
	},
	0xf800,0x07E0,0x001F
};
static BITMAPINFO2	bmi_16_555 = {
	{
		sizeof(BITMAPINFOHEADER),	
		640,			
		480,			
		1,				
		16,				
		BI_BITFIELDS,	
		640*480*2,		
		0,				
		0,				
		0,				
		0,				
	},
	0x7C00,0x03E0,0x001F
};

static BITMAPINFO	bmi_24 = {
	{
		sizeof(BITMAPINFOHEADER),	
		640,			
		480,			
		1,				
		24,				
		BI_RGB,			
		0,				
		0,				
		0,				
		0,				
		0,				
	},{
		0xff,0xff,0xff
	}

};

static BITMAPINFO2	bmi_32 = {
	{
		sizeof(BITMAPINFOHEADER),	
		640,			
		480,			
		1,				
		32,				
		BI_RGB,			
		0,				
		0,				
		0,				
		0,				
		0,				
	},
	0xff0000, 0x00ff00, 0x0000ff
};

RGB16	rgb16;


BOOL BMP_CheckFile( char *fname )
{
	return PAC_CheckFile( PackBmpDir, fname );
}








BOOL BMP_PalLoad_act( RGB32 *pal, char *fname )
{
	int		i,size;
	char	*buf=NULL;
	if( !(size=PAC_LoadFile( PackBmpDir, fname, &buf)) ) return FALSE;

	if( size>768 ){
		GFree(buf);
		return FALSE;
	}

	for(i=0; i<size/3 ;i++){
		pal[i].r = buf[i*3+0];
		pal[i].g = buf[i*3+1];
		pal[i].b = buf[i*3+2];
	}

	GFree(buf);
	return TRUE;
}
BOOL BMP_PalLoad_BT( BMP_BT *bmp_b, char *fname )
{
	return BMP_PalLoad_act( bmp_b->pal, fname );
}



BOOL BMP_PalSave( RGB32 *pal, char *fname )
{
	int		i;
	char	buf[256*3];

	for(i=0; i<256 ;i++){
		buf[i*3+0] = pal[i].r;
		buf[i*3+1] = pal[i].g;
		buf[i*3+2] = pal[i].b;
	}
	STD_WriteFile( fname, buf, 256*3 );

	return TRUE;
}

BOOL BMP_PalSave_BT( BMP_BT *bmp_b, char *fname )
{
	return BMP_PalSave( bmp_b->pal, fname );
}

char	*ToneCourve=NULL;

int BMP_LoadTonecurve( char *fname )
{
	int		size;

	GFree(ToneCourve);
	if(fname==NULL) return 0;
	if( !(size=PAC_LoadFile( PackBmpDir, fname, &ToneCourve)) ){
		DebugPrintf( "トーンカーブファイル[%s]がありません", fname );
		return 0;
	}
	if(size!=1280 && size!=768 && size!=256){
		GFree(ToneCourve);
		DebugBox( NULL, "サイズが違います。本当にトーンカーブファイルかチェックしてください[%s]", fname );
		return 0;
	}
	return size;
}

void BMP_ReleaseTonecurve(void)
{
	GFree(ToneCourve);
}

BOOL BMP_SetTonecurve_T( BMP_T *bmp_t, char *fname, int viv )
{
	int		i;
	char	*rgb, *r, *g, *b;
	int		size = BMP_LoadTonecurve( fname );
	char	*alp1_tbl;
	char	*alp2_tbl;
	if(viv!=256){
		char	*vivtabl1 = BlendTable[256-viv];
		char	*vivtabl2 = BlendTable[viv];
		int		glay,size2 = bmp_t->sy*bmp_t->sx;
		if(viv==0){
			for( i=0 ; i<size2 ; i++){
				bmp_t->buf[i].r = bmp_t->buf[i].g = bmp_t->buf[i].b
								= (bmp_t->buf[i].r*77 + bmp_t->buf[i].g*28 + bmp_t->buf[i].b*151)>>8;
			}
		}else{
			for( i=0 ; i<size2 ; i++){
				glay = vivtabl1[(bmp_t->buf[i].r*77 + bmp_t->buf[i].g*28 + bmp_t->buf[i].b*151)>>8];
				bmp_t->buf[i].r = glay + vivtabl2[bmp_t->buf[i].r];
				bmp_t->buf[i].g = glay + vivtabl2[bmp_t->buf[i].g];
				bmp_t->buf[i].b = glay + vivtabl2[bmp_t->buf[i].b];
			}
		}
	}
	
	if( !size )	return FALSE;
	if(size==1280){
		rgb = ToneCourve;
		r = ToneCourve+256;
		g = ToneCourve+512;
		b = ToneCourve+768;

		for( i=0 ; i<bmp_t->sy*bmp_t->sx ; i++){
			if(bmp_t->buf[i].a==255){
				bmp_t->buf[i].r = rgb[r[ bmp_t->buf[i].r ]];
				bmp_t->buf[i].g = rgb[g[ bmp_t->buf[i].g ]];
				bmp_t->buf[i].b = rgb[b[ bmp_t->buf[i].b ]];
			}else{
				if(bmp_t->buf[i].a!=0){
					alp1_tbl = BlendTable[  bmp_t->buf[i].a ];
					alp2_tbl = BlendTable2[ bmp_t->buf[i].a ];

					bmp_t->buf[i].r = alp1_tbl[ rgb[r[ alp2_tbl[bmp_t->buf[i].r] ]] ];
					bmp_t->buf[i].g = alp1_tbl[ rgb[g[ alp2_tbl[bmp_t->buf[i].g] ]] ];
					bmp_t->buf[i].b = alp1_tbl[ rgb[b[ alp2_tbl[bmp_t->buf[i].b] ]] ];




				}
			}
		}
	}else if(size==768){
		r = ToneCourve;
		g = ToneCourve+256;
		b = ToneCourve+512;

		for( i=0 ; i<bmp_t->sy*bmp_t->sx ; i++){
			if(bmp_t->buf[i].a==255){
				bmp_t->buf[i].r = r[ bmp_t->buf[i].r ];
				bmp_t->buf[i].g = g[ bmp_t->buf[i].g ];
				bmp_t->buf[i].b = b[ bmp_t->buf[i].b ];
			}else{
				if(bmp_t->buf[i].a!=0){
					alp1_tbl = BlendTable[  bmp_t->buf[i].a ];
					alp2_tbl = BlendTable2[ bmp_t->buf[i].a ];

					bmp_t->buf[i].r = alp1_tbl[ r[ alp2_tbl[bmp_t->buf[i].r] ] ];
					bmp_t->buf[i].g = alp1_tbl[ g[ alp2_tbl[bmp_t->buf[i].g] ] ];
					bmp_t->buf[i].b = alp1_tbl[ b[ alp2_tbl[bmp_t->buf[i].b] ] ];
				}
			}
		}
	}else if(size==256){
		rgb = ToneCourve;

		for( i=0 ; i<bmp_t->sy*bmp_t->sx ; i++){
			if(bmp_t->buf[i].a==255){
				bmp_t->buf[i].r = rgb[ bmp_t->buf[i].r ];
				bmp_t->buf[i].g = rgb[ bmp_t->buf[i].g ];
				bmp_t->buf[i].b = rgb[ bmp_t->buf[i].b ];
			}else{
				if(bmp_t->buf[i].a!=0){
					alp1_tbl = BlendTable[  bmp_t->buf[i].a ];
					alp2_tbl = BlendTable2[ bmp_t->buf[i].a ];

					bmp_t->buf[i].r = alp1_tbl[ rgb[ alp2_tbl[bmp_t->buf[i].r] ] ];
					bmp_t->buf[i].g = alp1_tbl[ rgb[ alp2_tbl[bmp_t->buf[i].g] ] ];
					bmp_t->buf[i].b = alp1_tbl[ rgb[ alp2_tbl[bmp_t->buf[i].b] ] ];
				}
			}
		}
	}
	
	BMP_ReleaseTonecurve();
	return TRUE;
}

BOOL BMP_SetTonecurve_F( BMP_F *bmp_f, char *fname, int viv )
{
	int		i;
	char	*rgb, *r, *g, *b;
	int		size = BMP_LoadTonecurve( fname );

	if(viv!=256){
		char	*vivtabl1 = BlendTable[256-viv];
		char	*vivtabl2 = BlendTable[viv];
		int		glay,size = bmp_f->sy*bmp_f->sx;
		if(viv==0){
			for( i=0 ; i<size ; i++){
				bmp_f->buf[i].r = bmp_f->buf[i].g = bmp_f->buf[i].b
								= (bmp_f->buf[i].r*77 + bmp_f->buf[i].g*28 + bmp_f->buf[i].b*151)>>8;
			}
		}else{
			for( i=0 ; i<size ; i++){
				glay = vivtabl1[(bmp_f->buf[i].r*77 + bmp_f->buf[i].g*28 + bmp_f->buf[i].b*151)>>8];
				bmp_f->buf[i].r = glay + vivtabl2[bmp_f->buf[i].r];
				bmp_f->buf[i].g = glay + vivtabl2[bmp_f->buf[i].g];
				bmp_f->buf[i].b = glay + vivtabl2[bmp_f->buf[i].b];
			}
		}
	}
	
	if( !size )	return FALSE;
	if(size==1280){
		rgb = ToneCourve;
		r = ToneCourve+256;
		g = ToneCourve+512;
		b = ToneCourve+768;
		for( i=0 ; i<bmp_f->sy*bmp_f->sx ; i++){
			bmp_f->buf[i].r = rgb[r[ bmp_f->buf[i].r ]];
			bmp_f->buf[i].g = rgb[g[ bmp_f->buf[i].g ]];
			bmp_f->buf[i].b = rgb[b[ bmp_f->buf[i].b ]];
		}
	}else if(size==768){
		r = ToneCourve;
		g = ToneCourve+256;
		b = ToneCourve+512;

		for( i=0 ; i<bmp_f->sy*bmp_f->sx ; i++){
			bmp_f->buf[i].r = r[ bmp_f->buf[i].r ];
			bmp_f->buf[i].g = g[ bmp_f->buf[i].g ];
			bmp_f->buf[i].b = b[ bmp_f->buf[i].b ];
		}
	}else if(size==256){
		rgb = ToneCourve;
		for( i=0 ; i<bmp_f->sy*bmp_f->sx ; i++){
			bmp_f->buf[i].r = rgb[ bmp_f->buf[i].r ];
			bmp_f->buf[i].g = rgb[ bmp_f->buf[i].g ];
			bmp_f->buf[i].b = rgb[ bmp_f->buf[i].b ];
		}
	}

	BMP_ReleaseTonecurve();
	return TRUE;
}

BOOL BMP_SetTonecurve_H( BMP_H *bmp_h, char *fname, int viv )
{
	int		i;
	char	*rgb, *r, *g, *b;
	int		size = BMP_LoadTonecurve( fname );
	if( !size ) return FALSE;
	if(size==1280){
		rgb = ToneCourve;
		r = ToneCourve+256;
		g = ToneCourve+512;
		b = ToneCourve+768;

		if(viv==256){
			for( i=0 ; i<bmp_h->sy*bmp_h->sx ; i++){
				bmp_h->buf[i] =
					  (rgb[r[ ((bmp_h->buf[i]&rgb16.mask_r)>>rgb16.pos_r<<rgb16.num_r) ]]>>rgb16.num_r<<rgb16.pos_r)
					| (rgb[g[ ((bmp_h->buf[i]&rgb16.mask_g)>>rgb16.pos_g<<rgb16.num_g) ]]>>rgb16.num_g<<rgb16.pos_g)
					| (rgb[b[ ((bmp_h->buf[i]&rgb16.mask_b)>>rgb16.pos_b<<rgb16.num_b) ]]>>rgb16.num_b<<rgb16.pos_b);
			}
		}else{
			int	rr,gg,bb, glay;
			for( i=0 ; i<bmp_h->sy*bmp_h->sx ; i++){
				rr = ((bmp_h->buf[i]&rgb16.mask_r)>>rgb16.pos_r<<rgb16.num_r);
				gg = ((bmp_h->buf[i]&rgb16.mask_g)>>rgb16.pos_g<<rgb16.num_g);
				bb = ((bmp_h->buf[i]&rgb16.mask_b)>>rgb16.pos_b<<rgb16.num_b);

				glay = ((rr*77 + gg*28 + bb*151)>>8) * (256-viv);

				bmp_h->buf[i] =
					  (rgb[r[ (glay + rr*viv)>>8 ]]>>rgb16.num_r<<rgb16.pos_r)
					| (rgb[g[ (glay + gg*viv)>>8 ]]>>rgb16.num_g<<rgb16.pos_g)
					| (rgb[b[ (glay + bb*viv)>>8 ]]>>rgb16.num_b<<rgb16.pos_b);
			}
		}
	}else if(size==768){
		r = ToneCourve;
		g = ToneCourve+256;
		b = ToneCourve+512;

		if(viv==256){
			for( i=0 ; i<bmp_h->sy*bmp_h->sx ; i++){
				bmp_h->buf[i] =
					  (r[ ((bmp_h->buf[i]&rgb16.mask_r)>>rgb16.pos_r<<rgb16.num_r) ]>>rgb16.num_r<<rgb16.pos_r)
					| (g[ ((bmp_h->buf[i]&rgb16.mask_g)>>rgb16.pos_g<<rgb16.num_g) ]>>rgb16.num_g<<rgb16.pos_g)
					| (b[ ((bmp_h->buf[i]&rgb16.mask_b)>>rgb16.pos_b<<rgb16.num_b) ]>>rgb16.num_b<<rgb16.pos_b);
			}
		}else{
			int	rr,gg,bb, glay;
			for( i=0 ; i<bmp_h->sy*bmp_h->sx ; i++){
				rr = ((bmp_h->buf[i]&rgb16.mask_r)>>rgb16.pos_r<<rgb16.num_r);
				gg = ((bmp_h->buf[i]&rgb16.mask_g)>>rgb16.pos_g<<rgb16.num_g);
				bb = ((bmp_h->buf[i]&rgb16.mask_b)>>rgb16.pos_b<<rgb16.num_b);

				glay = ((rr*77 + gg*28 + bb*151)>>8) * (256-viv);

				bmp_h->buf[i] =
					  (r[ (glay + rr*viv)>>8 ]>>rgb16.num_r<<rgb16.pos_r)
					| (g[ (glay + gg*viv)>>8 ]>>rgb16.num_g<<rgb16.pos_g)
					| (b[ (glay + bb*viv)>>8 ]>>rgb16.num_b<<rgb16.pos_b);
			}
		}
	}else if(size==256){
		rgb = ToneCourve;

		if(viv==256){
			for( i=0 ; i<bmp_h->sy*bmp_h->sx ; i++){
				bmp_h->buf[i] =
					  (rgb[ ((bmp_h->buf[i]&rgb16.mask_r)>>rgb16.pos_r<<rgb16.num_r) ]>>rgb16.num_r<<rgb16.pos_r)
					| (rgb[ ((bmp_h->buf[i]&rgb16.mask_g)>>rgb16.pos_g<<rgb16.num_g) ]>>rgb16.num_g<<rgb16.pos_g)
					| (rgb[ ((bmp_h->buf[i]&rgb16.mask_b)>>rgb16.pos_b<<rgb16.num_b) ]>>rgb16.num_b<<rgb16.pos_b);
			}
		}else{
			int	rr,gg,bb, glay;
			for( i=0 ; i<bmp_h->sy*bmp_h->sx ; i++){
				rr = ((bmp_h->buf[i]&rgb16.mask_r)>>rgb16.pos_r<<rgb16.num_r);
				gg = ((bmp_h->buf[i]&rgb16.mask_g)>>rgb16.pos_g<<rgb16.num_g);
				bb = ((bmp_h->buf[i]&rgb16.mask_b)>>rgb16.pos_b<<rgb16.num_b);

				glay = ((rr*77 + gg*28 + bb*151)>>8) * (256-viv);

				bmp_h->buf[i] =
					  (rgb[ (glay + rr*viv)>>8 ]>>rgb16.num_r<<rgb16.pos_r)
					| (rgb[ (glay + gg*viv)>>8 ]>>rgb16.num_g<<rgb16.pos_g)
					| (rgb[ (glay + bb*viv)>>8 ]>>rgb16.num_b<<rgb16.pos_b);
			}
		}
	}
	BMP_ReleaseTonecurve();
	return TRUE;
}

BOOL BMP_SetTonecurve_BT( BMP_BT *bmp_bt, char *fname, int viv )
{
	int		i;
	char	*rgb, *r, *g, *b;
	int		size = BMP_LoadTonecurve( fname );
	if( !size ) return FALSE;
	if(size==1280){
		rgb = ToneCourve;
		r = ToneCourve+256;
		g = ToneCourve+512;
		b = ToneCourve+768;

		if(viv==256){
			for( i=0 ; i<256 ; i++){
				bmp_bt->pal[i].r = rgb[r[ bmp_bt->pal[i].r ]];
				bmp_bt->pal[i].g = rgb[g[ bmp_bt->pal[i].g ]];
				bmp_bt->pal[i].b = rgb[b[ bmp_bt->pal[i].b ]];
			}
		}else{
			int	glay;
			for( i=0 ; i<256 ; i++){
				glay = ((bmp_bt->pal[i].r*77 + bmp_bt->pal[i].g*28 + bmp_bt->pal[i].b*151)>>8) * (256-viv);
				bmp_bt->pal[i].r = rgb[r[ (glay + bmp_bt->pal[i].r*viv)>>8 ]];
				bmp_bt->pal[i].g = rgb[g[ (glay + bmp_bt->pal[i].g*viv)>>8 ]];
				bmp_bt->pal[i].b = rgb[b[ (glay + bmp_bt->pal[i].b*viv)>>8 ]];
			}
		}
	}else if(size==768){
		r = ToneCourve;
		g = ToneCourve+256;
		b = ToneCourve+512;

		if(viv==256){
			for( i=0 ; i<256 ; i++){
				bmp_bt->pal[i].r = r[ bmp_bt->pal[i].r ];
				bmp_bt->pal[i].g = g[ bmp_bt->pal[i].g ];
				bmp_bt->pal[i].b = b[ bmp_bt->pal[i].b ];
			}
		}else{
			int	glay;
			for( i=0 ; i<256 ; i++){
				glay = ((bmp_bt->pal[i].r*77 + bmp_bt->pal[i].g*28 + bmp_bt->pal[i].b*151)>>8) * (256-viv);
				bmp_bt->pal[i].r = r[ (glay + bmp_bt->pal[i].r*viv)>>8 ];
				bmp_bt->pal[i].g = g[ (glay + bmp_bt->pal[i].g*viv)>>8 ];
				bmp_bt->pal[i].b = b[ (glay + bmp_bt->pal[i].b*viv)>>8 ];
			}
		}
	}else if(size==256){
		rgb = ToneCourve;

		if(viv==256){
			for( i=0 ; i<256 ; i++){
				bmp_bt->pal[i].r = rgb[ bmp_bt->pal[i].r ];
				bmp_bt->pal[i].g = rgb[ bmp_bt->pal[i].g ];
				bmp_bt->pal[i].b = rgb[ bmp_bt->pal[i].b ];
			}
		}else{
			int	glay;
			for( i=0 ; i<256 ; i++){
				glay = ((bmp_bt->pal[i].r*77 + bmp_bt->pal[i].g*28 + bmp_bt->pal[i].b*151)>>8) * (256-viv);
				bmp_bt->pal[i].r = rgb[ (glay + bmp_bt->pal[i].r*viv)>>8 ];
				bmp_bt->pal[i].g = rgb[ (glay + bmp_bt->pal[i].g*viv)>>8 ];
				bmp_bt->pal[i].b = rgb[ (glay + bmp_bt->pal[i].b*viv)>>8 ];
			}
		}
	}

	BMP_ReleaseTonecurve();
	return TRUE;
}




BOOL BMP_MakeBMP_T2T( BMP_T *bmp_t, char *ptr, int rev_bmp )
{
	int		y;
	int		w=bmp_t->sx, h=bmp_t->sy;

	
	if( rev_bmp ){
		RGB32	*dst = bmp_t->buf + (h-1)*w;

		for(y=0; y<h ;y++, dst-=w, ptr+=w*4 ){
			CopyMemory( dst, ptr, w * sizeof(RGB32) );
		}
	}else{
		CopyMemory( bmp_t->buf, ptr, w*h * sizeof(RGB32) );
	}

	return TRUE;
}




BOOL BMP_MakeBMP_F2T( BMP_T *bmp_t, char *ptr, int rev_bmp )
{
	int		x,y;
	int		w=bmp_t->sx, h=bmp_t->sy;
	
	
	if( rev_bmp ){
		RGB32	*dst = bmp_t->buf + (h-1)*w;

		for(y=0; y<h ;y++, dst-=w*2, ptr+=w%4 ){
			for(x=0; x<w ;x++, dst++, ptr+=3 ){
				*dst = *(RGB32*)ptr;
			}
		}
	}else{
		RGB32	*dst = bmp_t->buf;

		for(y=0; y<h ;y++, ptr+=w%4 ){
			for(x=0; x<w ;x++, dst++, ptr+=3 ){
				*dst = *(RGB32*)ptr;
			}
		}
	}

	return TRUE;
}




BOOL BMP_MakeBMP_B2T( BMP_T *bmp_t, char *ptr, int rev_bmp, int alp_flag, int palnum )
{
	int		x,y;
	int		w=bmp_t->sx, h=bmp_t->sy;
	RGB32	*pal;
	char	*bmp;
	int		rev = (4-w%4)%4;

	
	pal = (RGB32*)ptr;
	ptr += palnum;
	bmp = ptr;

	if(alp_flag){
		ptr += (w+rev)*h;
		
		if( rev_bmp ){
			RGB32	*dst = bmp_t->buf + (h-1)*w;

			for(y=0; y<h ;y++, dst-=w*2, bmp+=rev, ptr+=rev ){
				for(x=0; x<w ;x++, dst++, bmp++, ptr++ ){
					dst->b = BlendTable[*ptr][ pal[*bmp].b ];
					dst->g = BlendTable[*ptr][ pal[*bmp].g ];
					dst->r = BlendTable[*ptr][ pal[*bmp].r ];
					dst->a = *ptr;
				}
			}
		}else{
			RGB32	*dst = bmp_t->buf;

			for(y=0; y<h ;y++ ){
				for(x=0; x<w ;x++, dst++, bmp++, ptr++ ){
					dst->b = BlendTable[*ptr][ pal[*bmp].b ];
					dst->g = BlendTable[*ptr][ pal[*bmp].g ];
					dst->r = BlendTable[*ptr][ pal[*bmp].r ];

					dst->a = *ptr;
				}
			}
		}
	}else{
		if( rev_bmp ){
			RGB32	*dst = bmp_t->buf + (h-1)*w;

			for(y=0; y<h ;y++, dst-=w*2 ){
				for(x=0; x<w ;x++, dst++, bmp++ ){
					*dst = pal[ *bmp ];
				}
			}
		}else{
			RGB32	*dst = bmp_t->buf;

			for(y=0; y<h ;y++ ){
				for(x=0; x<w ;x++, dst++, bmp++ ){
					*dst = pal[ *bmp ];
				}
			}
		}
	}

	return TRUE;
}






BOOL BMP_MakeBMP_F2F( BMP_F *bmp_f, char *ptr, int rev_bmp )
{
	int		y;
	int		w=bmp_f->sx, h=bmp_f->sy;

	
	if( rev_bmp ){
		RGB24	*dst = bmp_f->buf + (h-1)*w;

		for(y=0; y<h ;y++, dst-=w, ptr+=w*3 + w%4 ){
			CopyMemory( dst, ptr, w*3 );
		}
	}else{
		if(w%4){
			RGB24	*dst = bmp_f->buf;
			for(y=0; y<h ;y++, dst+=w, ptr+=w*3 + w%4 ){
				CopyMemory( dst, ptr, w*3 );
			}
		}else{
			CopyMemory( bmp_f->buf, ptr, w*h*3 );
		}
	}

	return TRUE;
}
BOOL BMP_MakeBMP_F2F_BJR( BMP_F *bmp_f, char *ptr, int rev_bmp, char *fname )
{
	int		x,y;
	int		w=bmp_f->sx, h=bmp_f->sy;
	int		key1,key2,key3;
	int		i,j,k;
	i=0;
	key1=256*256;
	key2=0;
	key3 = 0;
	while(fname[i]){
		key1 -= (BYTE)fname[i];
		key2 += (BYTE)fname[i];
		key3 |= (BYTE)fname[i];
		i++;
	}

	unsigned char	*dst = (unsigned char*)bmp_f->buf;

	y = key3 % h;
	for(j=0; j<h ;j++ ){
		y = (y+7)%h;
		unsigned char	*src = (unsigned char*)ptr + y*(w*3+w%4);
		for(x=0; x<w*3 ;x++, dst++, src++ ){
			if(x%2){
				*dst = (256*256+*src-key2)%256;
			}else{
				*dst = (256*256+(255-*src)-key1)%256;
			}
		}
	}

	return TRUE;
}




BOOL BMP_MakeBMP_B2F( BMP_F *bmp_f, char *ptr, int rev_bmp, int palnum )
{
	int		x,y;
	int		w=bmp_f->sx, h=bmp_f->sy;
	RGB32	*pal;

	
	pal = (RGB32*)ptr;
	ptr += palnum;
	
	if( rev_bmp ){
		RGB24	*dst = bmp_f->buf + (h-1)*w;

		for(y=0; y<h ;y++, dst-=w*2 ){
			for(x=0; x<w ;x++, dst++, ptr++ ){
				*dst = *(RGB24*)&pal[ *ptr ];
			}
		}
	}else{
		RGB24	*dst = bmp_f->buf;

		for(y=0; y<h ;y++ ){
			for(x=0; x<w ;x++, dst++, ptr++ ){
				*dst = *(RGB24*)&pal[ *ptr ];
			}
		}
	}

	return TRUE;
}







BOOL BMP_MakeBMP_T2H( BMP_H *bmp_h, char *ptr, int rev_bmp )
{
	int		x,y;
	int		w=bmp_h->sx, h=bmp_h->sy;

	bmp_h->alp = (char *)GlobalAlloc( GPTR, w*h );
	
	if( rev_bmp ){
		WORD	*dst = bmp_h->buf + (h-1)*w;
		char	*alp = bmp_h->alp + (h-1)*w;

		for(y=0; y<h ;y++, dst-=w*2, alp-=w*2 ){
			for(x=0; x<w ;x++, dst++, alp++, ptr+=4 ){
				*dst = BMP_RGB_FH( *(ptr+2), *(ptr+1), *ptr );
				*alp = *(ptr+3);
			}
		}
	}else{
		WORD	*dst = bmp_h->buf;
		char	*alp = bmp_h->alp;

		for(y=0; y<h ;y++ ){
			for(x=0; x<w ;x++, dst++, alp++, ptr+=4 ){
				*dst = BMP_RGB_FH( *(ptr+2), *(ptr+1), *ptr );
				*alp = *(ptr+3);
			}
		}
	}

	return TRUE;
}




BOOL BMP_MakeBMP_F2H( BMP_H *bmp_h, char *ptr, int rev_bmp )
{
	int		x,y;
	int		w=bmp_h->sx, h=bmp_h->sy;

	
	if( rev_bmp ){
		WORD	*dst = bmp_h->buf + (h-1)*w;

		for(y=0; y<h ;y++, dst-=w*2 ){
			for(x=0; x<w ;x++, dst++, ptr+=3 ){
				*dst = BMP_RGB_FH( *(ptr+2), *(ptr+1), *ptr );
			}
		}
	}else{
		WORD	*dst = bmp_h->buf;
		char	*alp = bmp_h->alp;

		for(y=0; y<h ;y++ ){
			for(x=0; x<w ;x++, dst++, ptr+=3 ){
				*dst = BMP_RGB_FH( *(ptr+2), *(ptr+1), *ptr );
			}
		}
	}
	return TRUE;
}



BOOL BMP_MakeBMP_B2H( BMP_H *bmp_h, char *ptr, int rev_bmp, int alp_flag, int palnum )
{
	int		x,y;
	int		w=bmp_h->sx, h=bmp_h->sy;
	RGB32	*pal;
	char	*bmp;
	WORD	wpal[256];

	
	pal = (RGB32*)ptr;
	ptr += palnum;
	bmp = ptr;
	ptr += w*h;

	for( y=0; y<palnum/4; y++) {
		wpal[y] = BMP_RGB_FH( pal[y].r , pal[y].g, pal[y].b );
	}
	
	if( alp_flag ){
		bmp_h->alp = (char *)GlobalAlloc( GPTR, w*h );

		if( rev_bmp ){
			WORD	*dst = bmp_h->buf + (h-1)*w;
			char	*alp = bmp_h->alp + (h-1)*w;

			for(y=0; y<h ;y++, dst-=w*2, alp-=w, ptr+=w ){
				for(x=0; x<w ;x++, dst++, bmp++ ){
					*dst = wpal[ *bmp ];
				}
				CopyMemory( alp, ptr, w );
			}
		}else{
			WORD	*dst = bmp_h->buf;

			for(y=0; y<h*w ;y++, dst++, ptr++ ){
				*dst = wpal[ *ptr ];
			}
			CopyMemory( bmp_h->alp, ptr, w*h );
		}
	}else{
		if( rev_bmp ){
			WORD	*dst = bmp_h->buf + (h-1)*w;

			for(y=0; y<h ;y++, dst-=w*2 ){
				for(x=0; x<w ;x++, dst++, bmp++ ){
					*dst = wpal[ *bmp ];
				}
			}
		}else{
			WORD	*dst = bmp_h->buf;

			for(y=0; y<h ;y++ ){
				for(x=0; x<w ;x++, dst++, bmp++ ){
					*dst = wpal[ *bmp ];
				}
			}
		}
	}
	return TRUE;
}






BOOL BMP_MakeBMP_B2B( BMP_BT *bmp_bt, char *ptr, int rev_bmp, int alp_flag, int palnum )
{
	int		y;
	int		w=bmp_bt->sx, h=bmp_bt->sy;
	char	*bmp, *alp;
	int		rev = (4-w%4)%4;

	if(alp_flag){
		bmp_bt->alp = (char *)GlobalAlloc( GPTR, bmp_bt->sx*bmp_bt->sy );
	}
	
	CopyMemory( bmp_bt->pal, ptr, palnum );
	ptr += palnum;

	
	bmp = ptr;
	ptr += (w+rev)*h;
	alp = ptr;

	
	if(alp_flag){
		if( rev_bmp ){
			char	*dbmp = bmp_bt->buf + (h-1)*w;
			char	*dalp = bmp_bt->alp + (h-1)*w;

			for( y=0 ; y<h ; y++, dbmp-=w, dalp-=w, bmp+=w+rev, alp+=w+rev ){
				CopyMemory( dbmp, bmp, w );
				CopyMemory( dalp, alp, w );
			}
		}else{
			char	*dbmp = bmp_bt->buf;
			char	*dalp = bmp_bt->alp;

			for(y=0; y<h ;y++, dbmp+=w, dalp+=w, bmp+=w+rev, alp+=w+rev ){
				CopyMemory( dbmp, bmp, w );
				CopyMemory( dalp, alp, w );
			}
		}
	}else{
		if(rev_bmp){
			char	*dbmp = bmp_bt->buf + (h-1)*w;

			for(y=0; y<h ;y++, dbmp-=w, bmp+=w+rev ){
				CopyMemory( dbmp, bmp, w );
			}
		}else{
			char	*dbmp = bmp_bt->buf;

			for(y=0; y<h ;y++, dbmp+=w, bmp+=w+rev ){
				CopyMemory( dbmp, bmp, w );
			}
		}
	}

	return TRUE;
}

BOOL BMP_LoadBMP_X2X( char *fname, BMP_SET *bmp_set, char *pack )
{
	BITMAPFILEHEADER    *BMPFileHead;
    BITMAPINFOHEADER    *BMPFileInfo;
	char				*buf, *ptr;
	int					rev_bmp=1,y,i;
	int					palnum;
	BOOL				ret=FALSE;

	if( !bmp_set ) return FALSE;
	if(bmp_set->flag){
		switch(bmp_set->bmp_bit){
			case 0:	BMP_ReleaseBMP_BT( &bmp_set->bmp_b );	break;
			case 1:	BMP_ReleaseBMP_H( &bmp_set->bmp_h );	break;
			case 2:	BMP_ReleaseBMP_F( &bmp_set->bmp_f );	break;
			case 3:	BMP_ReleaseBMP_T( &bmp_set->bmp_t );	break;
		}
		ZeroMemory( bmp_set, sizeof(BMP_SET) );
	}

	
	if(pack==NULL) pack = PackBmpDir;
	if( !PAC_LoadFile( pack, fname, &buf) ) return FALSE;
	ptr = buf;

	if( _mbsicmp( (BYTE *)STD_GetAttr(fname), (BYTE *)"bbm")==0 ){

	}else if( _mbsicmp( (BYTE *)STD_GetAttr(fname), (BYTE *)"tga")==0 ){
		int		bpp, srcw, srch, palnum, alpha,hrev;
		BYTE	*bmp, *pal;
		if( !LoadTgaInMemory( (BYTE*)ptr, &bmp, &pal, &palnum, &srcw, &srch, &bpp, &alpha, &hrev ) ){
			DebugBox(NULL,"%sの読み込みに失敗しました",fname);
			GlobalFree( buf );
			return 0;
		}
		
		bmp_set->flag = TRUE;
		bmp_set->bmp_bit = bpp/8-1;
		bmp_set->alp_flag= alpha;
		bmp_set->pos.x=0;
		bmp_set->pos.y=0;
		bmp_set->size.x=srcw;
		bmp_set->size.y=srch;

		switch(bpp){
			case 8:
				bmp_set->bmp_b.sx = srcw;
				bmp_set->bmp_b.sy = srch;
				bmp_set->bmp_b.buf = (char *)GlobalAlloc( GPTR, bmp_set->bmp_b.sx*bmp_set->bmp_b.sy * sizeof(BYTE) );
				bmp_set->bmp_b.pal = (RGB32 *)GlobalAlloc( GPTR, sizeof(RGB32)*256 );

				if(hrev){
					CopyMemory( bmp_set->bmp_b.buf, bmp, bmp_set->bmp_b.sx*bmp_set->bmp_b.sy * sizeof(BYTE) );
				}else{
					for( y=0 ; y<bmp_set->bmp_b.sy ; y++ ){
						CopyMemory( bmp_set->bmp_b.buf + bmp_set->bmp_b.sx*(bmp_set->bmp_b.sy-y-1),
								bmp + bmp_set->bmp_b.sx*y * sizeof(BYTE), bmp_set->bmp_b.sx * sizeof(BYTE) );
					}
				}
				for(i=0;i<palnum;i++){
					bmp_set->bmp_b.pal[i].r = pal[i*4+0];
					bmp_set->bmp_b.pal[i].g = pal[i*4+1];
					bmp_set->bmp_b.pal[i].b = pal[i*4+2];
					bmp_set->bmp_b.pal[i].a = pal[i*4+3];
				}
				break;
			case 24:
				bmp_set->bmp_f.sx = srcw;
				bmp_set->bmp_f.sy = srch;
				bmp_set->bmp_f.buf = (RGB24 *)GlobalAlloc( GPTR, bmp_set->bmp_f.sx*bmp_set->bmp_f.sy * sizeof(RGB24) );
				if(hrev){
					CopyMemory( bmp_set->bmp_f.buf, bmp, bmp_set->bmp_f.sx*bmp_set->bmp_f.sy * sizeof(RGB24) );
				}else{
					for( y=0 ; y<bmp_set->bmp_f.sy ; y++ ){
						CopyMemory( bmp_set->bmp_f.buf + bmp_set->bmp_f.sx * y,
								bmp + bmp_set->bmp_f.sx * sizeof(RGB24) * (bmp_set->bmp_f.sy-y-1) , bmp_set->bmp_f.sx * sizeof(RGB24) );
					}
				}
				break;
			case 32:
				bmp_set->bmp_t.sx = srcw;
				bmp_set->bmp_t.sy = srch;
				bmp_set->bmp_t.buf = (RGB32 *)GlobalAlloc( GPTR, bmp_set->bmp_t.sx*bmp_set->bmp_t.sy * sizeof(RGB32) );
				if(hrev){
					CopyMemory( bmp_set->bmp_t.buf, bmp, bmp_set->bmp_t.sx*bmp_set->bmp_t.sy * sizeof(RGB32) );
				}else{
					for( y=0 ; y<bmp_set->bmp_t.sy ; y++ ){
						CopyMemory( bmp_set->bmp_t.buf + bmp_set->bmp_t.sx * y,
									bmp + bmp_set->bmp_t.sx * sizeof(RGB32) * (bmp_set->bmp_t.sy-y-1), bmp_set->bmp_t.sx * sizeof(RGB32) );
					}
				}
				break;
		}
		ret=TRUE;
	}else{
		
		BMPFileHead = (BITMAPFILEHEADER*)ptr;	ptr += sizeof( BITMAPFILEHEADER);	
		BMPFileInfo = (BITMAPINFOHEADER*)ptr;	ptr += sizeof( BITMAPINFOHEADER);	

		
		if( 0>BMPFileInfo->biHeight ){	BMPFileInfo->biHeight*=-1;	rev_bmp=0;	}
		if( BMPFileHead->bfReserved1 ){
			bmp_set->alp_flag = 1;
			bmp_set->pos.x = LOWORD(BMPFileInfo->biXPelsPerMeter);
			bmp_set->pos.y = LOWORD(BMPFileInfo->biYPelsPerMeter);
			bmp_set->size.x = HIWORD(BMPFileInfo->biXPelsPerMeter);
			bmp_set->size.y = HIWORD(BMPFileInfo->biYPelsPerMeter);
		}else{
			bmp_set->alp_flag = 0;
			bmp_set->pos.x = 0;
			bmp_set->pos.y = 0;
			bmp_set->size.x = BMPFileInfo->biWidth;
			bmp_set->size.y = BMPFileInfo->biHeight;
		}
		
		palnum = BMPFileHead->bfOffBits-(sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER));
		
		switch(BMPFileInfo->biBitCount){
			default:	DebugBox( NULL, "BMPデータのBIT数が異常です[%s](%dBIT?)", fname, BMPFileInfo->biBitCount );	break;
		case 8:
			bmp_set->bmp_b.sx  = BMPFileInfo->biWidth;
			bmp_set->bmp_b.sy  = BMPFileInfo->biHeight;
			bmp_set->bmp_b.buf = (char *)GlobalAlloc( GPTR, bmp_set->bmp_b.sx*bmp_set->bmp_b.sy * sizeof(BYTE) );
			bmp_set->bmp_b.pal = (RGB32 *)GlobalAlloc( GPTR, sizeof(RGB32)*256 );
			ret = BMP_MakeBMP_B2B( &bmp_set->bmp_b, ptr, rev_bmp, bmp_set->alp_flag, palnum );
			bmp_set->bmp_bit  = 0;
			break;
		case 16:
			DebugBox(NULL,"16BITハイカラーの読み込みは非対応です");
			break;
		case 24:
			bmp_set->bmp_f.sx  = BMPFileInfo->biWidth;
			bmp_set->bmp_f.sy  = BMPFileInfo->biHeight;
			bmp_set->bmp_f.buf = (RGB24 *)GlobalAlloc( GPTR, bmp_set->bmp_f.sx*bmp_set->bmp_f.sy * sizeof(RGB24) );
			ptr += palnum;

			if( _mbsicmp( (BYTE *)STD_GetAttr(fname), (BYTE *)"BJR")==0 ){
				ret=BMP_MakeBMP_F2F_BJR( &bmp_set->bmp_f, ptr, rev_bmp, fname );
			}else{
				ret = BMP_MakeBMP_F2F( &bmp_set->bmp_f, ptr, rev_bmp );
			}
			bmp_set->bmp_bit  = 2;
			break;
		case 32:
			bmp_set->bmp_t.sx  = BMPFileInfo->biWidth;
			bmp_set->bmp_t.sy  = BMPFileInfo->biHeight;
			bmp_set->bmp_t.buf = (RGB32 *)GlobalAlloc( GPTR, bmp_set->bmp_t.sx*bmp_set->bmp_t.sy * sizeof(RGB32) );
			ptr += palnum;
			ret = BMP_MakeBMP_T2T( &bmp_set->bmp_t, ptr, rev_bmp );
			bmp_set->alp_flag = 1;
			bmp_set->bmp_bit  = 3;
			break;
		}
		bmp_set->flag     = TRUE;
	}

	GlobalFree( buf );

	return ret;
}



BOOL BMP_LoadBMP_X2T( char *fname, BMP_T *bmp_t, POINT *pos, POINT *size, int *alp_flag, char *pack )
{
	BITMAPFILEHEADER    *BMPFileHead;
    BITMAPINFOHEADER    *BMPFileInfo;
	char				*buf, *ptr;
	int					rev_bmp=1;
	int					palnum;
	BOOL				ret=FALSE;

	BMP_ReleaseBMP_T( bmp_t );

	
	if(pack==NULL) pack = PackBmpDir;
	if( !PAC_LoadFile( pack, fname, &buf) ) return FALSE;
	ptr = buf;
	
	BMPFileHead = (BITMAPFILEHEADER*)ptr;	ptr += sizeof( BITMAPFILEHEADER);	
	BMPFileInfo = (BITMAPINFOHEADER*)ptr;	ptr += sizeof( BITMAPINFOHEADER);	

	
	if( 0>BMPFileInfo->biHeight ){	BMPFileInfo->biHeight*=-1;	rev_bmp=0;	}
	if( BMPFileHead->bfReserved1 ){
		*alp_flag = ON;
		if(BMPFileInfo->biXPelsPerMeter==2834 || BMPFileInfo->biXPelsPerMeter==0){
			pos->x = 0;
			pos->y = 0;
			size->x = BMPFileInfo->biWidth;
			size->y = BMPFileInfo->biHeight;
		}else{
			pos->x = LOWORD(BMPFileInfo->biXPelsPerMeter);
			pos->y = LOWORD(BMPFileInfo->biYPelsPerMeter);
			size->x = HIWORD(BMPFileInfo->biXPelsPerMeter);
			size->y = HIWORD(BMPFileInfo->biYPelsPerMeter);
		}
	}else{
		pos->x = 0;
		pos->y = 0;
		size->x = BMPFileInfo->biWidth;
		size->y = BMPFileInfo->biHeight;
		*alp_flag = OFF;
	}

	
	bmp_t->sx  = BMPFileInfo->biWidth;
	bmp_t->sy  = BMPFileInfo->biHeight;
	bmp_t->buf = (RGB32 *)GlobalAlloc( GPTR, bmp_t->sx*bmp_t->sy * sizeof(RGB32) );
	
	
	palnum = BMPFileHead->bfOffBits-(sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER));

	
	switch( BMPFileInfo->biBitCount ){
		default:	DebugBox( NULL, "BMPデータのBIT数が異常です[%s](%dBIT?)", fname, BMPFileInfo->biBitCount );	ret=FALSE;	break;
		case 8:		ret=BMP_MakeBMP_B2T( bmp_t, ptr, rev_bmp, *alp_flag, palnum );	break;

		case 24:	ptr += palnum;	ret=BMP_MakeBMP_F2T( bmp_t, ptr, rev_bmp );						break;
		case 32:	ptr += palnum;	ret=BMP_MakeBMP_T2T( bmp_t, ptr, rev_bmp );	*alp_flag=1;		break;
	}

	GlobalFree( buf );

	return ret;
}
BOOL BMP_LoadBMP_X2F( char *fname, BMP_F *bmp_f, POINT *pos, POINT *size, char *pack )
{
	BITMAPFILEHEADER    *BMPFileHead;
    BITMAPINFOHEADER    *BMPFileInfo;
	char				*buf=NULL, *ptr=NULL;
	int					rev_bmp=1;
	int					palnum;
	BOOL				ret=FALSE;

	BMP_ReleaseBMP_F( bmp_f );

	
	if(pack==NULL) pack = PackBmpDir;
	if( !PAC_LoadFile( pack, fname, &buf) ) return FALSE;
	ptr = buf;
	
	BMPFileHead = (BITMAPFILEHEADER*)ptr;	ptr += sizeof( BITMAPFILEHEADER);	
	BMPFileInfo = (BITMAPINFOHEADER*)ptr;	ptr += sizeof( BITMAPINFOHEADER);	

	
	if( 0>BMPFileInfo->biHeight ){	BMPFileInfo->biHeight*=-1;	rev_bmp=0;	}
	if( BMPFileHead->bfReserved1 ){
		pos->x = LOWORD(BMPFileInfo->biXPelsPerMeter);
		pos->y = LOWORD(BMPFileInfo->biYPelsPerMeter);
		size->x = HIWORD(BMPFileInfo->biXPelsPerMeter);
		size->y = HIWORD(BMPFileInfo->biYPelsPerMeter);
	}else{
		pos->x = 0;
		pos->y = 0;
		size->x = BMPFileInfo->biWidth;
		size->y = BMPFileInfo->biHeight;
	}

	
	bmp_f->sx  = BMPFileInfo->biWidth;
	bmp_f->sy  = BMPFileInfo->biHeight;
	bmp_f->buf = (RGB24 *)GlobalAlloc( GPTR, bmp_f->sx*bmp_f->sy * 3 );

	
	palnum = BMPFileHead->bfOffBits-(sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER));
	

	if( !stricmp(STD_GetAttr(fname),"BJR") ){
		ptr += palnum;
		ret=BMP_MakeBMP_F2F_BJR( bmp_f, ptr, rev_bmp, fname );
	}else{
		
		switch( BMPFileInfo->biBitCount ){
			default:	DebugBox( NULL, "BMPデータのBIT数が異常です[%s](%dBIT?)", fname, BMPFileInfo->biBitCount );	ret=FALSE;	break;
			case 8:		ret=BMP_MakeBMP_B2F( bmp_f, ptr, rev_bmp, palnum );	break;
	
			case 24:	ptr += palnum;	ret=BMP_MakeBMP_F2F( bmp_f, ptr, rev_bmp );			break;
	
		}
	}

	GlobalFree( buf );

	return ret;
}
BOOL BMP_LoadBMP_X2H( char *fname, BMP_H *bmp_h, POINT *pos, POINT *size, int *alp_flag, char *pack )
{
	BITMAPFILEHEADER    *BMPFileHead;
    BITMAPINFOHEADER    *BMPFileInfo;
	char				*buf, *ptr;
	int					rev_bmp=1;
	int					palnum;
	BOOL				ret=FALSE;

	BMP_ReleaseBMP_H( bmp_h );

	
	if(pack==NULL) pack = PackBmpDir;
	if( !PAC_LoadFile( pack, fname, &buf) ) return FALSE;
	ptr = buf;
	
	BMPFileHead = (BITMAPFILEHEADER*)ptr;	ptr += sizeof( BITMAPFILEHEADER);	
	BMPFileInfo = (BITMAPINFOHEADER*)ptr;	ptr += sizeof( BITMAPINFOHEADER);	

	
	if( 0>BMPFileInfo->biHeight ){	BMPFileInfo->biHeight*=-1;	rev_bmp=0;	}
	if( BMPFileHead->bfReserved1 ){
		*alp_flag = ON;
		pos->x = LOWORD(BMPFileInfo->biXPelsPerMeter);
		pos->y = LOWORD(BMPFileInfo->biYPelsPerMeter);
		size->x = HIWORD(BMPFileInfo->biXPelsPerMeter);
		size->y = HIWORD(BMPFileInfo->biYPelsPerMeter);
	}else{
		pos->x = 0;
		pos->y = 0;
		size->x = BMPFileInfo->biWidth;
		size->y = BMPFileInfo->biHeight;
		*alp_flag = OFF;
	}

	
	bmp_h->sx  = BMPFileInfo->biWidth;
	bmp_h->sy  = BMPFileInfo->biHeight;
	bmp_h->buf = (WORD *)GlobalAlloc( GPTR, bmp_h->sx*bmp_h->sy * sizeof(WORD) );

	
	*alp_flag = BMPFileHead->bfReserved1;
	
	palnum = BMPFileHead->bfOffBits-(sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER));

	
	switch( BMPFileInfo->biBitCount ){
		default:	DebugBox( NULL, "BMPデータのBIT数が異常です[%s](%dBIT?)", fname, BMPFileInfo->biBitCount );	ret=FALSE;	break;
		case 8:		ret=BMP_MakeBMP_B2H( bmp_h, ptr, rev_bmp, *alp_flag, palnum );	break;

		case 24:	ptr += palnum;	ret=BMP_MakeBMP_F2H( bmp_h, ptr, rev_bmp );						break;
		case 32:	ptr += palnum;	ret=BMP_MakeBMP_T2H( bmp_h, ptr, rev_bmp );	*alp_flag=1;		break;
	}

	GlobalFree( buf );

	return ret;
}
BOOL BMP_LoadBMP_X2B( char *fname, BMP_BT *bmp_bt, POINT *pos, POINT *size, int *alp_flag, char *pack )
{
	BITMAPFILEHEADER    *BMPFileHead;
    BITMAPINFOHEADER    *BMPFileInfo;
	char				*buf, *ptr;
	int					rev_bmp=1;
	int					palnum;
	BOOL				ret=FALSE;

	BMP_ReleaseBMP_BT( bmp_bt );

	
	if(pack==NULL) pack = PackBmpDir;
	if( !PAC_LoadFile( pack, fname, &buf) ) return FALSE;
	ptr = buf;
	
	BMPFileHead = (BITMAPFILEHEADER*)ptr;	ptr += sizeof( BITMAPFILEHEADER);	
	BMPFileInfo = (BITMAPINFOHEADER*)ptr;	ptr += sizeof( BITMAPINFOHEADER);	

	
	if( 0>BMPFileInfo->biHeight ){	BMPFileInfo->biHeight*=-1;	rev_bmp=0;	}
	if( BMPFileHead->bfReserved1 ){
		*alp_flag = ON;
		if(BMPFileInfo->biXPelsPerMeter==2834 || BMPFileInfo->biXPelsPerMeter==0){
			pos->x = 0;
			pos->y = 0;
			size->x = BMPFileInfo->biWidth;
			size->y = BMPFileInfo->biHeight;
		}else{
			pos->x = LOWORD(BMPFileInfo->biXPelsPerMeter);
			pos->y = LOWORD(BMPFileInfo->biYPelsPerMeter);
			size->x = HIWORD(BMPFileInfo->biXPelsPerMeter);
			size->y = HIWORD(BMPFileInfo->biYPelsPerMeter);
		}




	}else{
		pos->x = 0;
		pos->y = 0;
		size->x = BMPFileInfo->biWidth;
		size->y = BMPFileInfo->biHeight;
		*alp_flag = OFF;
	}

	
	bmp_bt->sx  = BMPFileInfo->biWidth;
	bmp_bt->sy  = BMPFileInfo->biHeight;
	bmp_bt->buf = (char *)GlobalAlloc( GPTR, bmp_bt->sx*bmp_bt->sy );
	bmp_bt->pal = (RGB32 *)GlobalAlloc( GPTR, sizeof(RGB32)*256 );

	
	palnum = BMPFileHead->bfOffBits-(sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER));

	
	switch( BMPFileInfo->biBitCount ){
		default:	DebugBox( NULL, "BMPデータのBIT数が異常です[%s](%dBIT?)", fname, BMPFileInfo->biBitCount );	ret=FALSE;	break;
		case 8:		ret=BMP_MakeBMP_B2B( bmp_bt, ptr, rev_bmp, *alp_flag, palnum );	break;



	}

	GlobalFree( buf );

	return ret;
}



BOOL BMP_CheckBMPBpp( char *fname )
{
	BITMAPFILEHEADER    *BMPFileHead;
    BITMAPINFOHEADER    *BMPFileInfo;
	char				*buf, *ptr;
	int					ret=0;

	
	if( !PAC_LoadFile( PackBmpDir, fname, &buf ) ) return ret;
	ptr = buf;
	
	BMPFileHead = (BITMAPFILEHEADER*)ptr;	ptr += sizeof( BITMAPFILEHEADER);	
	BMPFileInfo = (BITMAPINFOHEADER*)ptr;	ptr += sizeof( BITMAPINFOHEADER);	
	ret = BMPFileInfo->biBitCount;

	GlobalFree( buf );

	return ret;
}

BOOL BMP_CheckBMPBpp2( char *fname )
{
	BITMAPFILEHEADER    *BMPFileHead;
    BITMAPINFOHEADER    *BMPFileInfo;
	char				*buf, *ptr;
	int					ret=0;

	
	if( !STD_ReadFileMax( fname, &buf ) ) return ret;
	ptr = buf;
	
	BMPFileHead = (BITMAPFILEHEADER*)ptr;	ptr += sizeof( BITMAPFILEHEADER);	
	BMPFileInfo = (BITMAPINFOHEADER*)ptr;	ptr += sizeof( BITMAPINFOHEADER);	
	ret = BMPFileInfo->biBitCount;

	GlobalFree( buf );

	return ret;
}




BOOL BMP_LoadBMP_T2T( char *fname, BMP_T *bmp_t )
{
	int					y,w,h;
	BITMAPFILEHEADER    *BMPFileHead;
    BITMAPINFOHEADER    *BMPFileInfo;
	char				*buf, *ptr;
	int					rev_bmp=1;

	BMP_ReleaseBMP_T( bmp_t );

	
	STD_ReadFileMax( fname, &buf );
	ptr = buf;

	
	BMPFileHead = (BITMAPFILEHEADER*)ptr;	ptr += sizeof( BITMAPFILEHEADER);	
	BMPFileInfo = (BITMAPINFOHEADER*)ptr;	ptr += sizeof( BITMAPINFOHEADER);	

	
	if( BMPFileInfo->biBitCount!=32 ){
		GlobalFree(buf);
		DebugBox( NULL, "BMPデータが３２ビットではありません[%s]", fname );
		return FALSE;
	}

	
	if( 0>BMPFileInfo->biHeight ){	BMPFileInfo->biHeight*=-1;	rev_bmp=0;	}

	
	bmp_t->sx  = w = BMPFileInfo->biWidth;
	bmp_t->sy  = h = BMPFileInfo->biHeight;
	bmp_t->buf = (RGB32 *)GlobalAlloc( GPTR, w*h * sizeof(RGB32) );

	
	if( rev_bmp ){
		RGB32	*dst = bmp_t->buf + (h-1)*w;

		for(y=0; y<h ;y++, dst-=w, ptr+=w*4 ){
			CopyMemory( dst, ptr, w * sizeof(RGB32) );
		}
	}else{
		CopyMemory( bmp_t->buf, ptr, w*h * sizeof(RGB32) );
	}
	
	GlobalFree( buf );

	return TRUE;
}




BOOL BMP_LoadBMP_F2T( char *fname, BMP_T *bmp_t )
{
	int					x,y,w,h;
	BITMAPFILEHEADER    *BMPFileHead;
    BITMAPINFOHEADER    *BMPFileInfo;
	char				*buf, *ptr;
	int					rev_bmp=1;

	BMP_ReleaseBMP_T( bmp_t );

	
	STD_ReadFileMax( fname, &buf );
	ptr = buf;
	
	BMPFileHead = (BITMAPFILEHEADER*)ptr;	ptr += sizeof( BITMAPFILEHEADER);	
	BMPFileInfo = (BITMAPINFOHEADER*)ptr;	ptr += sizeof( BITMAPINFOHEADER);	

	
	if( BMPFileInfo->biBitCount!=24 ){
		GlobalFree(buf);
		DebugBox( NULL, "BMPデータが２４ビットではありません[%s]", fname );
		return FALSE;
	}

	
	if( 0>BMPFileInfo->biHeight ){	BMPFileInfo->biHeight*=-1;	rev_bmp=0;	}

	
	bmp_t->sx  = w = BMPFileInfo->biWidth;
	bmp_t->sy  = h = BMPFileInfo->biHeight;
	bmp_t->buf = (RGB32 *)GlobalAlloc( GPTR, w*h * sizeof(RGB32) );
	
	
	if( rev_bmp ){
		DWORD	*dst = (DWORD*)bmp_t->buf + (h-1)*w;

		for(y=0; y<h ;y++, dst-=w*2, ptr+=w%4 ){
			for(x=0; x<w ;x++, dst++, ptr+=3 ){
				*dst = *(DWORD*)ptr;
			}
		}
	}else{
		DWORD	*dst = (DWORD*)bmp_t->buf;

		for(y=0; y<h ;y++, ptr+=w%4 ){
			for(x=0; x<w ;x++, dst++, ptr+=3 ){
				*dst = *(DWORD*)ptr;
			}
		}
	}
	
	GlobalFree( buf );

	return TRUE;
}




BOOL BMP_LoadBMP_B2T( char *fname, BMP_T *bmp_t )
{
	int					x,y,w,h;
	BITMAPFILEHEADER    *BMPFileHead;
    BITMAPINFOHEADER    *BMPFileInfo;
	RGB32             *pal;
	int					palnum;
	char				*buf, *ptr;
	int					rev_bmp=1;

	BMP_ReleaseBMP_T( bmp_t );

	
	STD_ReadFileMax( fname, &buf );
	ptr = buf;
	
	BMPFileHead = (BITMAPFILEHEADER*)ptr;	ptr += sizeof( BITMAPFILEHEADER);	
	BMPFileInfo = (BITMAPINFOHEADER*)ptr;	ptr += sizeof( BITMAPINFOHEADER);	

	
	if( BMPFileInfo->biBitCount!=8 ){
		GlobalFree(buf);
		DebugBox( NULL, "BMPデータが２４ビットではありません[%s]", fname );
		return FALSE;
	}

	
	if( 0>BMPFileInfo->biHeight ){	BMPFileInfo->biHeight*=-1;	rev_bmp=0;	}

	
	bmp_t->sx  = w = BMPFileInfo->biWidth;
	bmp_t->sy  = h = BMPFileInfo->biHeight;
	bmp_t->buf = (RGB32 *)GlobalAlloc( GPTR, w*h * sizeof(RGB32) );

	
	palnum = BMPFileHead->bfOffBits-(sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER));
	pal = (RGB32*)ptr;
	ptr += palnum;
	
	if( rev_bmp ){
		RGB32	*dst = bmp_t->buf + (h-1)*w;

		for(y=0; y<h ;y++, dst-=w*2 ){
			for(x=0; x<w ;x++, dst++, ptr++ ){
				*dst = pal[ *ptr ];
			}
		}
	}else{
		RGB32	*dst = bmp_t->buf;

		for(y=0; y<h ;y++ ){
			for(x=0; x<w ;x++, dst++, ptr++ ){
				*dst = pal[ *ptr ];
			}
		}
	}
	
	GlobalFree( buf );

	return TRUE;
}






BOOL BMP_LoadBMP_F2F( char *fname, BMP_F *bmp_f )
{
	BITMAPFILEHEADER    *BMPFileHead;
    BITMAPINFOHEADER    *BMPFileInfo;
	char				*buf, *ptr;
	int					rev_bmp=1;

	BMP_ReleaseBMP_F( bmp_f );

	
	if( !STD_ReadFileMax( fname, &buf ) ){
		DebugBox( NULL, "ファイルが無いでぇ" );
		return 0;
	}
	ptr = buf;
	
	BMPFileHead = (BITMAPFILEHEADER*)ptr;	ptr += sizeof( BITMAPFILEHEADER);	
	BMPFileInfo = (BITMAPINFOHEADER*)ptr;	ptr += sizeof( BITMAPINFOHEADER);	

	
	if( BMPFileInfo->biBitCount!=24 ){
		GlobalFree(buf);
		DebugBox( NULL, "BMPデータが２４ビットではありません[%s]", fname );
		return FALSE;
	}

	
	if( 0>BMPFileInfo->biHeight ){	BMPFileInfo->biHeight*=-1;	rev_bmp=0;	}

	
	bmp_f->sx  = BMPFileInfo->biWidth;
	bmp_f->sy  = BMPFileInfo->biHeight;
	bmp_f->buf = (RGB24 *)GlobalAlloc( GPTR, bmp_f->sx*bmp_f->sy * 3 );

	
	BMP_MakeBMP_F2F( bmp_f, ptr, rev_bmp );

	
	GlobalFree( buf );

	return TRUE;
}





BOOL BMP_LoadBMP_B2F( char *fname, BMP_F *bmp_f )
{

	BITMAPFILEHEADER    *BMPFileHead;
    BITMAPINFOHEADER    *BMPFileInfo;

	int					palnum;
	char				*buf, *ptr;
	int					rev_bmp=1;

	BMP_ReleaseBMP_F( bmp_f );

	
	STD_ReadFileMax( fname, &buf );
	ptr = buf;
	
	BMPFileHead = (BITMAPFILEHEADER*)ptr;	ptr += sizeof( BITMAPFILEHEADER);	
	BMPFileInfo = (BITMAPINFOHEADER*)ptr;	ptr += sizeof( BITMAPINFOHEADER);	

	
	if( BMPFileInfo->biBitCount!=8 ){
		GlobalFree(buf);
		DebugBox( NULL, "BMPデータが２４ビットではありません[%s]", fname );
		return FALSE;
	}

	
	if( 0>BMPFileInfo->biHeight ){	BMPFileInfo->biHeight*=-1;	rev_bmp=0;	}

	
	bmp_f->sx  = BMPFileInfo->biWidth;
	bmp_f->sy  = BMPFileInfo->biHeight;
	bmp_f->buf = (RGB24 *)GlobalAlloc( GPTR, bmp_f->sx*bmp_f->sy * 3 );

	
	palnum = BMPFileHead->bfOffBits-(sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER));


	
	BMP_MakeBMP_B2F( bmp_f, ptr, rev_bmp, palnum );

	
	GlobalFree( buf );

	return TRUE;
}







BOOL BMP_LoadBMP_T2H( char *fname, BMP_H *bmp_h )
{
	int					x,y,w,h;
	BITMAPFILEHEADER    *BMPFileHead;
    BITMAPINFOHEADER    *BMPFileInfo;
	char				*buf, *ptr;
	int					rev_bmp=1;

	BMP_ReleaseBMP_H( bmp_h );

	
	STD_ReadFileMax( fname, &buf );
	ptr = buf;

	
	BMPFileHead = (BITMAPFILEHEADER*)ptr;	ptr += sizeof( BITMAPFILEHEADER);	
	BMPFileInfo = (BITMAPINFOHEADER*)ptr;	ptr += sizeof( BITMAPINFOHEADER);	

	
	if( BMPFileInfo->biBitCount!=32 ){
		GlobalFree(buf);
		DebugBox( NULL, "BMPデータが３２ビットではありません[%s]", fname );
		return FALSE;
	}

	
	if( 0>BMPFileInfo->biHeight ){	BMPFileInfo->biHeight*=-1;	rev_bmp=0;	}

	
	bmp_h->sx  = w = BMPFileInfo->biWidth;
	bmp_h->sy  = h = BMPFileInfo->biHeight;
	bmp_h->buf = (WORD *)GlobalAlloc( GPTR, w*h * sizeof(WORD) );
	bmp_h->alp = (char *)GlobalAlloc( GPTR, w*h );

	
	if( rev_bmp ){
		WORD	*dst = bmp_h->buf + (h-1)*w;
		char	*alp = bmp_h->alp + (h-1)*w;

		for(y=0; y<h ;y++, dst-=w*2, alp-=w*2 ){
			for(x=0; x<w ;x++, dst++, alp++, ptr+=4 ){
				*dst = BMP_RGB_FH( *(ptr+2), *(ptr+1), *ptr );
				*alp = *(ptr+3);
			}
		}
	}else{
		WORD	*dst = bmp_h->buf;
		char	*alp = bmp_h->alp;

		for(y=0; y<h ;y++ ){
			for(x=0; x<w ;x++, dst++, alp++, ptr+=4 ){
				*dst = BMP_RGB_FH( *(ptr+2), *(ptr+1), *ptr );
				*alp = *(ptr+3);
			}
		}
	}
	
	GlobalFree( buf );

	return TRUE;
}




BOOL BMP_LoadBMP_F2H( char *fname, BMP_H *bmp_h )
{
	int					x,y,w,h;
	BITMAPFILEHEADER    *BMPFileHead;
    BITMAPINFOHEADER    *BMPFileInfo;
	char				*buf, *ptr;
	int					rev_bmp=1;

	BMP_ReleaseBMP_H( bmp_h );

	
	STD_ReadFileMax( fname, &buf );
	ptr = buf;

	
	BMPFileHead = (BITMAPFILEHEADER*)ptr;	ptr += sizeof( BITMAPFILEHEADER);	
	BMPFileInfo = (BITMAPINFOHEADER*)ptr;	ptr += sizeof( BITMAPINFOHEADER);	

	
	if( BMPFileInfo->biBitCount!=24 ){
		GlobalFree(buf);
		DebugBox( NULL, "BMPデータが２４ビットではありません[%s]", fname );
		return FALSE;
	}

	
	if( 0>BMPFileInfo->biHeight ){	BMPFileInfo->biHeight*=-1;	rev_bmp=0;	}

	
	bmp_h->sx  = w = BMPFileInfo->biWidth;
	bmp_h->sy  = h = BMPFileInfo->biHeight;
	bmp_h->buf = (WORD *)GlobalAlloc( GPTR, w*h * sizeof(WORD) );

	
	if( rev_bmp ){
		WORD	*dst = bmp_h->buf + (h-1)*w;

		for(y=0; y<h ;y++, dst-=w*2 ){
			for(x=0; x<w ;x++, dst++, ptr+=3 ){
				*dst = BMP_RGB_FH( *(ptr+2), *(ptr+1), *ptr );
			}
		}
	}else{
		WORD	*dst = bmp_h->buf;

		for(y=0; y<h ;y++ ){
			for(x=0; x<w ;x++, dst++, ptr+=3 ){
				*dst = BMP_RGB_FH( *(ptr+2), *(ptr+1), *ptr );
			}
		}
	}
	
	GlobalFree( buf );
	return TRUE;
}



BOOL BMP_LoadBMP_H2H( char *fname, BMP_H *bmp_h )
{
	int					x,y,w,h;
	BITMAPFILEHEADER    *BMPFileHead;
    BITMAPINFOHEADER    *BMPFileInfo;
	char				*buf, *ptr;
	int					rev_bmp=1;

	BMP_ReleaseBMP_H( bmp_h );

	
	STD_ReadFileMax( fname, &buf );
	ptr = buf;

	
	BMPFileHead = (BITMAPFILEHEADER*)ptr;	ptr += sizeof( BITMAPFILEHEADER);	
	BMPFileInfo = (BITMAPINFOHEADER*)ptr;	ptr += sizeof( BITMAPINFOHEADER);	

	
	if( BMPFileInfo->biBitCount!=16 ){
		GlobalFree(buf);
		DebugBox( NULL, "BMPデータが１６ビットではありません[%s]", fname );
		return FALSE;
	}
	
	if( 0>BMPFileInfo->biHeight ){	BMPFileInfo->biHeight*=-1;	rev_bmp=0;	}

	
	bmp_h->sx  = w = BMPFileInfo->biWidth;
	bmp_h->sy  = h = BMPFileInfo->biHeight;
	bmp_h->buf = (WORD *)GlobalAlloc( GPTR, w*h * sizeof(WORD) );

	
	if( rev_bmp ){
		WORD	*dst = bmp_h->buf + (h-1)*w;

		for(y=0; y<h ;y++, dst-=w*2 ){
			for(x=0; x<w ;x++, dst++, ptr+=2 ){
				*dst = *(WORD*)ptr;
			}
		}
	}else{
		WORD	*dst = bmp_h->buf;

		for(y=0; y<h ;y++, dst+=w, ptr+=w*2 ){
			CopyMemory( dst, ptr, w*sizeof(WORD) );
		}
	}
	
	GlobalFree( buf );
	return TRUE;
}



BOOL BMP_LoadBMP_B2H( char *fname, BMP_H *bmp_h )
{
	int					x,y,w,h,i;
	BITMAPFILEHEADER    *BMPFileHead;
    BITMAPINFOHEADER    *BMPFileInfo;
	RGB32             *pal;
	WORD				wpal[256];
	int					palnum;
	char				*buf, *ptr;
	int					rev_bmp=1;

	BMP_ReleaseBMP_H( bmp_h );

	
	STD_ReadFileMax( fname, &buf );
	ptr = buf;
	
	BMPFileHead = (BITMAPFILEHEADER*)ptr;	ptr += sizeof( BITMAPFILEHEADER);	
	BMPFileInfo = (BITMAPINFOHEADER*)ptr;	ptr += sizeof( BITMAPINFOHEADER);	

	
	if( BMPFileInfo->biBitCount!=8 ){
		GlobalFree(buf);
		DebugBox( NULL, "BMPデータが２４ビットではありません[%s]", fname );
		return FALSE;
	}

	
	if( 0>BMPFileInfo->biHeight ){	BMPFileInfo->biHeight*=-1;	rev_bmp=0;	}

	
	bmp_h->sx  = w = BMPFileInfo->biWidth;
	bmp_h->sy  = h = BMPFileInfo->biHeight;
	bmp_h->buf = (WORD *)GlobalAlloc( GPTR, w*h * sizeof(WORD) );

	
	palnum = BMPFileHead->bfOffBits-(sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER));
	pal = (RGB32*)ptr;
	ptr += palnum;

	for( i=0; i<palnum; i++) {
		wpal[i] = BMP_RGB_FH( pal[i].r , pal[i].g, pal[i].b );
	}

	
	if( rev_bmp ){
		WORD	*dst = bmp_h->buf + (h-1)*w;

		for(y=0; y<h ;y++, dst-=w*2 ){
			for(x=0; x<w ;x++, dst++, ptr++ ){
				*dst = wpal[ *ptr ];
			}
		}
	}else{
		WORD	*dst = bmp_h->buf;

		for(y=0; y<h ;y++ ){
			for(x=0; x<w ;x++, dst++, ptr++ ){
				*dst = wpal[ *ptr ];
			}
		}
	}
	
	GlobalFree( buf );

	return TRUE;
}






BOOL BMP_LoadBMP_B2B( char *fname, BMP_BT *bmp_bt  )
{
	return BMP_LoadBMP_B2B_Key( fname, bmp_bt, NULL );
}
BOOL BMP_LoadBMP_B2B_Key( char *fname, BMP_BT *bmp_bt, int *key )
{
	BITMAPFILEHEADER    *BMPFileHead;
    BITMAPINFOHEADER    *BMPFileInfo;
    RGB32             *pal;
	char				*buf, *ptr, *bmp, *alp;
	int					i,j;
	int					y,w,h;
    int					palnum;
	int					alp_flag=0;
	int					rev_bmp=1;

	BMP_ReleaseBMP_BT( bmp_bt );

	
	STD_ReadFileMax( fname, &buf );
	ptr = buf;
	
	BMPFileHead = (BITMAPFILEHEADER*)ptr;	ptr += sizeof( BITMAPFILEHEADER);	
	BMPFileInfo = (BITMAPINFOHEADER*)ptr;	ptr += sizeof( BITMAPINFOHEADER);	

	
	if( BMPFileInfo->biBitCount != 8){
		GlobalFree(buf);
		DebugBox( NULL, "BMPデータが８ビットではありません[%s]",fname );
		return FALSE;
	}
	if( BMPFileHead->bfReserved1 ){
		alp_flag=1;
	}

	
	if( 0>BMPFileInfo->biHeight ){	BMPFileInfo->biHeight*=-1;	rev_bmp=0;	}

	
	bmp_bt->sx  = w = BMPFileInfo->biWidth;
	bmp_bt->sy  = h = BMPFileInfo->biHeight;
	bmp_bt->pal = (RGB32 *)GlobalAlloc( GPTR, sizeof(RGB32)*256 );
	bmp_bt->buf = (char *)GlobalAlloc( GPTR, w*h );
	if(alp_flag)
		bmp_bt->alp = (char *)GlobalAlloc( GPTR, w*h );

	
	palnum = BMPFileHead->bfOffBits-(sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER));
	pal = (RGB32*)ptr;
	ptr += palnum;
	if(key){
		if(palnum==256) { *key = 1; }
		else			{ *key = 0;	}
	}

	
	CopyMemory( bmp_bt->pal, pal, palnum );

	bmp = ptr;
	ptr += w*h;
	alp = ptr;
	
	if( rev_bmp ){
		if(alp_flag){
			for(y=0; y<h ;y++ ){
				i = y*w;
				j = (h-y-1)*w;
				CopyMemory( &bmp_bt->buf[i], &bmp[j], w );
				CopyMemory( &bmp_bt->alp[i], &alp[j], w );
			}
		}else{
			for(y=0; y<h ;y++){
				i = y*w;
				j = (h-y-1)*w;
				CopyMemory( &bmp_bt->buf[i], &bmp[j], w );
			}
		}
	}else{
		if(alp_flag){
			CopyMemory( bmp_bt->buf, bmp, w*h );
			CopyMemory( bmp_bt->alp, alp, w*h );
		}else{
			CopyMemory( bmp_bt->buf, bmp, w*h );
		}
	}
	
	GlobalFree( buf );
	return TRUE;
}








void BMP_CopyBMP_T( BMP_T *dest, BMP_T *src )
{
	BMP_CreateBMP_T( dest, src->sx, src->sy );
	CopyMemory( dest->buf, src->buf, src->sx*src->sy*4 );
}



void BMP_CopyBMP_F( BMP_F *dest, BMP_F *src )
{
	BMP_CreateBMP_F( dest, src->sx, src->sy );
	CopyMemory( dest->buf, src->buf, src->sx*src->sy*3 );
}



void BMP_CopyBMP_H( BMP_H *dest, BMP_H *src )
{
	BMP_CreateBMP_H( dest, src->sx, src->sy, (int)src->alp );
	CopyMemory( dest->buf, src->buf, src->sx*src->sy*2 );
	if(src->alp)
		CopyMemory( dest->alp, src->alp, src->sx*src->sy );
}



void BMP_CopyBMP_BT( BMP_BT *dest, BMP_BT *src )
{
	BMP_CreateBMP_BT( dest, src->sx, src->sy, (int)src->alp );
	CopyMemory( dest->buf, src->buf, src->sx*src->sy );
	CopyMemory( dest->pal, src->pal, 256*4 );
	if(dest->alp)
		CopyMemory( dest->alp, src->alp, src->sx*src->sy );
}






void BMP_CreateBMP_T( BMP_T *bmp_t, int sx, int sy )
{
	BMP_ReleaseBMP_T( bmp_t );

	bmp_t->sx = sx;
	bmp_t->sy = sy;
	bmp_t->buf = (RGB32 *)GlobalAlloc( GPTR, sx*sy*sizeof(RGB32) );
}



void BMP_CreateBMP_F( BMP_F *bmp_f, int sx, int sy )
{
	BMP_ReleaseBMP_F( bmp_f );

	bmp_f->sx = sx;
	bmp_f->sy = sy;
	bmp_f->buf = (RGB24 *)GlobalAlloc( GPTR, sx*sy*3 );
}



void BMP_CreateBMP_H( BMP_H *bmp_h, int sx, int sy,int alph )
{
	BMP_ReleaseBMP_H( bmp_h );

	bmp_h->sx = sx;
	bmp_h->sy = sy;
	bmp_h->buf = (WORD *)GlobalAlloc( GPTR, sx*sy*2 );
	if(alph)
		bmp_h->alp = (char*)GlobalAlloc( GPTR, sx*sy );
}



void BMP_CreateBMP_BT( BMP_BT *bmp_bt, int sx, int sy, int alph )
{
	BMP_ReleaseBMP_BT( bmp_bt );

	bmp_bt->sx = sx;
	bmp_bt->sy = sy;
	bmp_bt->buf = (char *)GlobalAlloc( GPTR, sx*sy );
	bmp_bt->pal = (RGB32 *)GlobalAlloc( GPTR, 256*sizeof(RGB32) );
	if(alph)
		bmp_bt->alp = (char *)GlobalAlloc( GPTR, sx*sy );
}






void BMP_ReleaseBMP_T( BMP_T *bmp_t )
{
	if( bmp_t->buf ) bmp_t->buf = (RGB32 *)GlobalFree( bmp_t->buf );	
	bmp_t->sx = 0;
	bmp_t->sy = 0;
}



void BMP_ReleaseBMP_F( BMP_F *bmp_f )
{
	if( bmp_f->buf ) bmp_f->buf = (RGB24 *)GlobalFree( bmp_f->buf );	
	bmp_f->sx = 0;
	bmp_f->sy = 0;
}



void BMP_ReleaseBMP_H( BMP_H *bmp_h )
{
	if( bmp_h->buf ) bmp_h->buf = (WORD *)GlobalFree( bmp_h->buf );	
	if( bmp_h->alp ) bmp_h->alp = (char *)GlobalFree( bmp_h->alp );	
	bmp_h->sx = 0;
	bmp_h->sy = 0;
}



void BMP_ReleaseBMP_BT( BMP_BT *bmp_bt )
{
	if( bmp_bt->buf )
		bmp_bt->buf = (char *)GlobalFree( bmp_bt->buf );	
	if( bmp_bt->pal )
		bmp_bt->pal = (RGB32 *)GlobalFree( bmp_bt->pal );	
	if( bmp_bt->alp )
		bmp_bt->alp = (char *)GlobalFree( bmp_bt->alp );	
	bmp_bt->sx = 0;
	bmp_bt->sy = 0;
}






void BMP_GetVramWH( HWND hwnd, int *sx, int *sy )
{
	RECT	WindowRect;

	
	GetClientRect( hwnd, &WindowRect );
	
	*sx = WindowRect.right  - WindowRect.left;
	*sy = WindowRect.bottom - WindowRect.top;
}



void BMP_GetImageVram8( HWND hwnd, HDC *MemDC, HBITMAP *hbmp, HBITMAP *hold, BMP_BT *vram, int w, int h )
{
	HDC		hDC;
	int		i;
	
	bmi_8.bmiHeader.biWidth  = vram->sx = w;
	bmi_8.bmiHeader.biHeight = vram->sy = h;
	bmi_8.bmiHeader.biHeight*= -1;

	
	if( vram->pal ){
		for( i=0 ; i<256 ; i++ ){
			bmi_8.bmiColors[i] = vram->pal[i];
		}
	}else{
		for( i=0 ; i<256 ; i++ ){
			bmi_8.bmiColors[i].r = i;
			bmi_8.bmiColors[i].g = i;
			bmi_8.bmiColors[i].b = i;
		}
		vram->pal = bmi_8.bmiColors;
	}
	
	hDC = GetDC(hwnd);
	if(*MemDC==NULL)	*MemDC = CreateCompatibleDC(hDC);
	if(*hbmp){	DeleteObject( *hbmp );	*hbmp = NULL;	}

	*hbmp = CreateDIBSection( hDC, (BITMAPINFO*)&bmi_8, DIB_RGB_COLORS, (void **)&vram->buf, NULL, 0 );
	if(hold) *hold = (HBITMAP)SelectObject( *MemDC, *hbmp );
	else	 SelectObject( *MemDC, *hbmp );
	
	ReleaseDC( hwnd, hDC );
}



void BMP_GetImageVram16( HWND hwnd, HBITMAP *hbmp, BMP_H *vram, int w, int h )
{
	HDC		hDC;

	
	hDC = GetDC(hwnd);

	
	if(*hbmp){	DeleteObject( *hbmp );	*hbmp = NULL;	}
	switch( HighColorType ){
		case BIT16_555:
			bmi_16_555.bmiHeader.biWidth  = vram->sx = w;
			bmi_16_555.bmiHeader.biHeight = vram->sy = h;
			bmi_16_555.bmiHeader.biHeight*=-1;
			*hbmp = CreateDIBSection( hDC, (BITMAPINFO*)&bmi_16_555, DIB_RGB_COLORS, (void **)&vram->buf, NULL, 0 );
			break;
		case BIT16_565:
			bmi_16_565.bmiHeader.biWidth  = vram->sx = w;
			bmi_16_565.bmiHeader.biHeight = vram->sy = h;
			bmi_16_565.bmiHeader.biHeight*=-1;
			*hbmp = CreateDIBSection( hDC, (BITMAPINFO*)&bmi_16_565, DIB_RGB_COLORS, (void **)&vram->buf, NULL, 0 );
			break;
		case BIT16_4444:
			bmi_16_4444.bmiHeader.biWidth  = vram->sx = w;
			bmi_16_4444.bmiHeader.biHeight = vram->sy = h;
			bmi_16_4444.bmiHeader.biHeight*=-1;
			*hbmp = CreateDIBSection( hDC, (BITMAPINFO*)&bmi_16_4444, DIB_RGB_COLORS, (void **)&vram->buf, NULL, 0 );
			break;
	}
	
	ReleaseDC( hwnd, hDC );
}



void BMP_GetImageVram24( HWND hwnd, HDC *MemDC, HBITMAP *hbmp, HBITMAP *hold, BMP_F *vram, int w, int h )
{
	HDC		hDC;

	
	bmi_24.bmiHeader.biWidth  = vram->sx = w;
	bmi_24.bmiHeader.biHeight = vram->sy = h;
	bmi_24.bmiHeader.biHeight*=-1;
	
	hDC = GetDC(hwnd);
	if(*MemDC==NULL)	*MemDC = CreateCompatibleDC(hDC);
	if(*hbmp){	DeleteObject( *hbmp );	*hbmp = NULL;	}
	*hbmp = CreateDIBSection( hDC, (BITMAPINFO*)&bmi_24, DIB_RGB_COLORS	, (void **)&vram->buf, NULL, 0 );

	*hold = (HBITMAP)SelectObject( *MemDC, *hbmp );

	
	ReleaseDC( hwnd, hDC );
}



void BMP_GetImageVram32( HWND hwnd, HDC *MemDC, HBITMAP *hbmp, HBITMAP *hold, BMP_T *vram, int w, int h )
{
	HDC		hDC;
	
	
	bmi_32.bmiHeader.biWidth  = vram->sx = w;
	bmi_32.bmiHeader.biHeight = vram->sy = h;
	bmi_32.bmiHeader.biHeight*=-1;
	
	hDC = GetDC(hwnd);
	if(*MemDC==NULL)	*MemDC = CreateCompatibleDC(hDC);
	if(*hbmp){	DeleteObject( *hbmp );	*hbmp = NULL;	}
	*hbmp = CreateDIBSection( hDC, (BITMAPINFO*)&bmi_32, DIB_RGB_COLORS	, (void **)&vram->buf, NULL, 0 );

	*hold = (HBITMAP)SelectObject( *MemDC, *hbmp );

	
	ReleaseDC( hwnd, hDC );
}



BOOL BMP_SetPixelFormat16( int type )
{

	
    
	HighColorType = type;
	switch(HighColorType){
	case BIT16_555:
		rgb16.dep_r = 31;	
		rgb16.num_r = 3;	
		rgb16.pos_r = 10;	

		
		rgb16.dep_g = 31;
		rgb16.num_g = 3;
		rgb16.pos_g = 5;

		
		rgb16.dep_b = 31;
		rgb16.num_b = 3;
		rgb16.pos_b = 0;

		
		rgb16.mask_r = 0x7c00;
		rgb16.mask_g = 0x03e0;
		rgb16.mask_b = 0x001f;
		break;
	case BIT16_565:
		rgb16.dep_r = 31;	
		rgb16.num_r = 3;	
		rgb16.pos_r = 11;	

		
		rgb16.dep_g = 63;
		rgb16.num_g = 2;
		rgb16.pos_g = 5;

		
		rgb16.dep_b = 31;
		rgb16.num_b = 3;
		rgb16.pos_b = 0;

		
		rgb16.mask_r = 0xf800;
		rgb16.mask_g = 0x07e0;
		rgb16.mask_b = 0x001f;
		break;
	case BIT16_4444:
		rgb16.dep_r = 15;	
		rgb16.num_r = 4;	
		rgb16.pos_r = 8;	

		
		rgb16.dep_g = 15;
		rgb16.num_g = 4;
		rgb16.pos_g = 4;

		
		rgb16.dep_b = 15;
		rgb16.num_b = 4;
		rgb16.pos_b = 0;

		
		rgb16.mask_r = 0x0F00;
		rgb16.mask_g = 0x00F0;
		rgb16.mask_b = 0x000F;
		break;
	}
	
	rgb16.mask_rr = ((rgb16.mask_r<<16) | rgb16.mask_r);
	rgb16.mask_gg = ((rgb16.mask_g<<16) | rgb16.mask_g);
	rgb16.mask_bb = ((rgb16.mask_b<<16) | rgb16.mask_b);

	
	rgb16.mask_rb = (rgb16.mask_r | rgb16.mask_b);

	
	rgb16.mask_rbg = ((rgb16.mask_rb<<16) | rgb16.mask_g);
	
	rgb16.mask_grb = ((rgb16.mask_g <<16) | rgb16.mask_rb);

	return TRUE;
}






int BMP_SaveBmp_T( char *fname, BMP_T *bmp_t )
{
	DWORD               actualRead;
	HANDLE              hfile;
	BITMAPFILEHEADER    BMPFileHead;
	BITMAPINFOHEADER    BMPFileInfo;
	long				BmpAmount;
	if(bmp_t==NULL) return FALSE;

	BmpAmount=bmp_t->sx*bmp_t->sy;

	BMPFileHead.bfSize      = BmpAmount*4 + sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
	BMPFileHead.bfOffBits   = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	BMPFileHead.bfType      = 0x4d42;
	BMPFileHead.bfReserved1 = 0;
	BMPFileHead.bfReserved2 = 0;

	BMPFileInfo.biSize			= sizeof( BITMAPINFOHEADER);
	BMPFileInfo.biWidth			= bmp_t->sx;
	BMPFileInfo.biHeight		= -bmp_t->sy;
	BMPFileInfo.biPlanes		= 1;
	BMPFileInfo.biBitCount		= 32;
	BMPFileInfo.biCompression	= BI_RGB;
	BMPFileInfo.biSizeImage		= BmpAmount*4;
	BMPFileInfo.biXPelsPerMeter	= 0;
	BMPFileInfo.biYPelsPerMeter	= 0;
	BMPFileInfo.biClrUsed		= 0;
	BMPFileInfo.biClrImportant	= 0;

	
	hfile = CREATE_WRITE_FILE( fname );
	if( hfile == INVALID_HANDLE_VALUE ){
		return FALSE;
	}
	
	if( !WriteFile( hfile, &BMPFileHead, sizeof(BITMAPFILEHEADER), &actualRead, NULL)) return FALSE;
	if( !WriteFile( hfile, &BMPFileInfo, sizeof(BITMAPINFOHEADER), &actualRead, NULL)) return FALSE;
	
    if( !WriteFile( hfile, bmp_t->buf, BmpAmount*4, &actualRead, NULL) ) return FALSE;
	
	CloseHandle(hfile);
		
	return TRUE;
}




int BMP_SaveBmp_F( char *fname, BMP_F *bmp_f )
{
	DWORD               actualRead;
	HANDLE              hfile;
	BITMAPFILEHEADER    BMPFileHead;
	BITMAPINFOHEADER    BMPFileInfo;
	long				BmpAmount=bmp_f->sx*bmp_f->sy;

	BMPFileHead.bfType      = 0x4d42;
	BMPFileHead.bfSize      = BmpAmount*3 + sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
	BMPFileHead.bfReserved1 = 0;
	BMPFileHead.bfReserved2 = 0;
	BMPFileHead.bfOffBits   = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	BMPFileInfo.biSize			= sizeof( BITMAPINFOHEADER);
	BMPFileInfo.biWidth			= bmp_f->sx;
	BMPFileInfo.biHeight		= bmp_f->sy;
	BMPFileInfo.biPlanes		= 1;
	BMPFileInfo.biBitCount		= 24;
	BMPFileInfo.biCompression	= BI_RGB;
	BMPFileInfo.biSizeImage		= BmpAmount*3;
	BMPFileInfo.biXPelsPerMeter	= 2880;
	BMPFileInfo.biYPelsPerMeter	= 2880;
	BMPFileInfo.biClrUsed		= 0;
	BMPFileInfo.biClrImportant	= 0;

	
	hfile = CREATE_WRITE_FILE( fname );
	if( hfile == INVALID_HANDLE_VALUE ){
		return FALSE;
	}
	
	if( !WriteFile( hfile, &BMPFileHead, sizeof(BITMAPFILEHEADER), &actualRead, NULL)) { CloseHandle(hfile); return FALSE; }
	if( !WriteFile( hfile, &BMPFileInfo, sizeof(BITMAPINFOHEADER), &actualRead, NULL)) { CloseHandle(hfile); return FALSE; }
	
	{
		int		i;
		for(i=0;i<bmp_f->sy;i++){
			if( !WriteFile( hfile, bmp_f->buf+(bmp_f->sy-i-1)*bmp_f->sx, bmp_f->sx*3, &actualRead, NULL) ){
				CloseHandle(hfile);
				return FALSE;
			}
		}
	}
	
	CloseHandle(hfile);
		
	return TRUE;
}




int BMP_SaveBmp_B( char *fname, BMP_BT *bmp_bt )
{
	DWORD               actualRead;
	HANDLE              hfile;
	BITMAPFILEHEADER    BMPFileHead;
	BITMAPINFOHEADER    BMPFileInfo;
	long				BmpAmount = bmp_bt->sx*bmp_bt->sy;

	BMPFileHead.bfType		=0x4d42;
	BMPFileHead.bfSize		=BmpAmount + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	BMPFileHead.bfReserved1	=0;
	BMPFileHead.bfReserved2	=0;
	BMPFileHead.bfOffBits	=sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 256*sizeof(RGB32);


	BMPFileInfo.biSize			= sizeof( BITMAPINFOHEADER);
	BMPFileInfo.biWidth			= bmp_bt->sx;
	BMPFileInfo.biHeight		= -bmp_bt->sy;
	BMPFileInfo.biPlanes		= 1;
	BMPFileInfo.biBitCount		= 8;
	BMPFileInfo.biCompression	= BI_RGB;
	BMPFileInfo.biSizeImage		= BmpAmount;
	BMPFileInfo.biXPelsPerMeter	= 0;
	BMPFileInfo.biYPelsPerMeter	= 0;
	BMPFileInfo.biClrUsed		= 256;
	BMPFileInfo.biClrImportant	= 0;

	
	hfile = CREATE_WRITE_FILE( fname );
	if( hfile == INVALID_HANDLE_VALUE ){
		return FALSE;
	}
	
	if( !WriteFile( hfile, &BMPFileHead, sizeof(BITMAPFILEHEADER), &actualRead, NULL)) return FALSE;
	if( !WriteFile( hfile, &BMPFileInfo, sizeof(BITMAPINFOHEADER), &actualRead, NULL)) return FALSE;
	
	if( !WriteFile( hfile, bmp_bt->pal, 256*sizeof(RGB32), &actualRead, NULL)) return FALSE;
	
    if( !WriteFile( hfile, bmp_bt->buf, BmpAmount, &actualRead, NULL) ) return FALSE;
	
	CloseHandle(hfile);

	return TRUE;
}




int BMP_MakeAlphaFF2T( char *bfname, char *afname, char *tfname, int alp_flag, int alp_end, int alp250, int CutSave )
{
	int					i,j,x,y, rev_flag=0;
	DWORD               actualRead;
    HANDLE              hfile;
    BITMAPFILEHEADER    BMPFileHead;
    BITMAPINFOHEADER    BMPFileInfo;

	char				*buf;
	BMP_T				bmp_t;

	RECT				CutRect;
	int					revRGB, sizeRGB, sizeARGB;

	
	hfile = CREATE_READ_FILE( afname );
	if( hfile == INVALID_HANDLE_VALUE ){
		return 1;
	}
	
	if( !ReadFile( hfile, &BMPFileHead, sizeof( BITMAPFILEHEADER), &actualRead, NULL)) return 2;
	if( !ReadFile( hfile, &BMPFileInfo, sizeof( BITMAPINFOHEADER), &actualRead, NULL)) return 2;
	
	if( BMPFileInfo.biBitCount != 24){
		CloseHandle(hfile);
		return 3;
	}
	if(BMPFileInfo.biHeight<0) { rev_flag=1; BMPFileInfo.biHeight = -BMPFileInfo.biHeight; }
	CutRect.top    = BMPFileInfo.biHeight;
	CutRect.bottom = 0;
	CutRect.left   = BMPFileInfo.biWidth;
	CutRect.right  = 0;
	
	revRGB  = BMPFileInfo.biWidth%4;
	sizeRGB = (BMPFileInfo.biWidth*3 + revRGB) * BMPFileInfo.biHeight;
	sizeARGB = BMPFileInfo.biWidth*4 * BMPFileInfo.biHeight;
	
	bmp_t.buf = (RGB32 *)GlobalAlloc( GPTR, sizeARGB );
	bmp_t.sx = BMPFileInfo.biWidth;
	bmp_t.sy = BMPFileInfo.biHeight;
	
	buf = (char *)GlobalAlloc( GPTR, sizeRGB );
	
    if( !ReadFile( hfile, buf, sizeRGB, &actualRead, NULL) ) return 2;
	
	CloseHandle(hfile);
	
	i=0;j=0;
	if(alp250){
		for(y=0; y<BMPFileInfo.biHeight ;y++, j+=revRGB ){
			for(x=0; x<BMPFileInfo.biWidth ;x++, i++, j+=3 ){
				if(buf[j]>248){
					bmp_t.buf[i].a = 255;
					if(CutRect.top>y)	CutRect.top    = y;
					if(CutRect.bottom<y)CutRect.bottom = y;
					if(CutRect.left>x)	CutRect.left   = x;
					if(CutRect.right<x)	CutRect.right  = x;
				}else{
					if(buf[j]<8) bmp_t.buf[i].a = 0;
					else{
						bmp_t.buf[i].a = buf[j];
						if(CutRect.top>y)	CutRect.top    = y;
						if(CutRect.bottom<y)CutRect.bottom = y;
						if(CutRect.left>x)	CutRect.left   = x;
						if(CutRect.right<x)	CutRect.right  = x;
					}
				}
			}
		}
	}else{
		for(y=0; y<BMPFileInfo.biHeight ;y++, j+=revRGB ){
			for(x=0; x<BMPFileInfo.biWidth ;x++, i++,j+=3){
				bmp_t.buf[i].a = buf[j];
				if( buf[j] ){
					if(CutRect.top>y)	CutRect.top    = y;
					if(CutRect.bottom<y)CutRect.bottom = y;
					if(CutRect.left>x)	CutRect.left   = x;
					if(CutRect.right<x)	CutRect.right  = x;
				}
			}
		}
	}
	
	GlobalFree( buf );

	
	hfile = CREATE_READ_FILE( bfname );
	if( hfile == INVALID_HANDLE_VALUE ){
		return 5;
	}
	
	if( !ReadFile( hfile, &BMPFileHead, sizeof( BITMAPFILEHEADER), &actualRead, NULL)) return 6;
	if( !ReadFile( hfile, &BMPFileInfo, sizeof( BITMAPINFOHEADER), &actualRead, NULL)) return 6;

	if( BMPFileInfo.biBitCount != 24){
		CloseHandle(hfile);
		return 7;
	}
	if( bmp_t.sx != BMPFileInfo.biWidth ){
		CloseHandle(hfile);
		return 7;
	}
	if(BMPFileInfo.biHeight<0) {
		if(rev_flag) { BMPFileInfo.biHeight = -BMPFileInfo.biHeight; }
		else		 { CloseHandle(hfile); return 7; }
	}

	
	buf = (char *)GlobalAlloc( GPTR, sizeRGB );
	
    if( !ReadFile( hfile, buf, sizeRGB, &actualRead, NULL) ) return 6;
	
	CloseHandle(hfile);
	
	i=0;j=0;
	if( !alp_flag || alp_end ){
		for(y=0; y<BMPFileInfo.biHeight ; y++, j+=revRGB ){
			for(x=0; x<BMPFileInfo.biWidth ; x++, i++, j+=3 ){
				if( bmp_t.buf[i].a ){
					bmp_t.buf[i].b = buf[j+0];	
					bmp_t.buf[i].g = buf[j+1];	
					bmp_t.buf[i].r = buf[j+2];	
				}else{
					bmp_t.buf[i].b = 0;	
					bmp_t.buf[i].g = 0;	
					bmp_t.buf[i].r = 0;	
				}
			}
		}
	}else{
		for(y=0; y<BMPFileInfo.biHeight ; y++, j+=revRGB ){
			for(x=0; x<BMPFileInfo.biWidth ; x++, i++, j+=3 ){
				if(bmp_t.buf[i].a==255){
					bmp_t.buf[i].b = buf[j+0];	
					bmp_t.buf[i].g = buf[j+1];	
					bmp_t.buf[i].r = buf[j+2];	
				}else{

					bmp_t.buf[i].b = buf[j+0] * bmp_t.buf[i].a/256;	
					bmp_t.buf[i].g = buf[j+1] * bmp_t.buf[i].a/256;	
					bmp_t.buf[i].r = buf[j+2] * bmp_t.buf[i].a/256;	
				}
			}
		}
	}
	
	GlobalFree( buf );



	BMPFileHead.bfReserved1 = 1;
	BMPFileInfo.biBitCount = 32;
	if(!CutSave){
		
		hfile = CREATE_WRITE_FILE( tfname );
		if( hfile == INVALID_HANDLE_VALUE ){
			return 10;
		}
		
		BMPFileHead.bfSize      = sizeARGB + BMPFileHead.bfOffBits;
		BMPFileInfo.biSizeImage = sizeARGB;
		if( !WriteFile( hfile, &BMPFileHead, sizeof( BITMAPFILEHEADER), &actualRead, NULL)) return 11;
		if( !WriteFile( hfile, &BMPFileInfo, sizeof( BITMAPINFOHEADER), &actualRead, NULL)) return 11;
		
		if( !WriteFile( hfile, bmp_t.buf, sizeARGB, &actualRead, NULL) ) return 11;
		
		CloseHandle(hfile);
		GlobalFree( bmp_t.buf );
	}else{
		
		hfile = CREATE_WRITE_FILE( tfname );
		if( hfile == INVALID_HANDLE_VALUE ){
			return 10;
		}
		
		if(rev_flag){
			BMPFileInfo.biYPelsPerMeter = CutRect.top | (BMPFileInfo.biHeight<<16);
			BMPFileInfo.biHeight = -(CutRect.bottom-CutRect.top+1);
		}else{
			BMPFileInfo.biYPelsPerMeter = (BMPFileInfo.biHeight-CutRect.bottom-1) | (BMPFileInfo.biHeight<<16);
			BMPFileInfo.biHeight = CutRect.bottom-CutRect.top+1;
		}
		BMPFileInfo.biXPelsPerMeter = CutRect.left | (BMPFileInfo.biWidth<<16);
		BMPFileInfo.biWidth  = CutRect.right-CutRect.left+1;
		sizeARGB = BMPFileInfo.biWidth*4 * BMPFileInfo.biHeight;
		BMPFileHead.bfSize      = sizeARGB + BMPFileHead.bfOffBits;
		BMPFileInfo.biSizeImage = sizeARGB;

		if( !WriteFile( hfile, &BMPFileHead, sizeof( BITMAPFILEHEADER), &actualRead, NULL)) return 11;
		if( !WriteFile( hfile, &BMPFileInfo, sizeof( BITMAPINFOHEADER), &actualRead, NULL)) return 11;
		
	
		for( y=CutRect.top ; y<=CutRect.bottom ; y++ ){
			i = y*bmp_t.sx + CutRect.left;
			WriteFile( hfile, &bmp_t.buf[i], BMPFileInfo.biWidth * sizeof(RGB32), &actualRead, NULL);
		}
		
		CloseHandle(hfile);
		GlobalFree( bmp_t.buf );


	}


	return 0;
}





int BMP_MakeAlphaBB2BA( char *bfname, char *afname, char *tfname, int alp250, int CutSave )
{
    BITMAPFILEHEADER    BMPFileHead;
    BITMAPINFOHEADER    BMPFileInfo;
    RGB32               pal[256];
	char				*buf;
	int					i,j,x,y, rev_flag=0;
    int					palnum;
	DWORD               actualRead;
    HANDLE              hfile;
	BMP_BT				bmp_bt;
	RECT				CutRect;

	int	revBYTE;
	int	revRGB;
	int	sizeBYTE;
	int	sizeRGB;
	

	
	hfile = CREATE_READ_FILE( afname );
	if( hfile == INVALID_HANDLE_VALUE ){
		return 1;
	}
	
	if( !ReadFile( hfile, &BMPFileHead, sizeof( BITMAPFILEHEADER), &actualRead, NULL)) return 1;
	if( !ReadFile( hfile, &BMPFileInfo, sizeof( BITMAPINFOHEADER), &actualRead, NULL)) return 1;
	
	if( BMPFileInfo.biBitCount != 24){
		CloseHandle(hfile);
		return 3;
	}
	if(BMPFileInfo.biHeight<0) { rev_flag=1; BMPFileInfo.biHeight = -BMPFileInfo.biHeight; }
	CutRect.top    = BMPFileInfo.biHeight;
	CutRect.bottom = 0;
	CutRect.left   = BMPFileInfo.biWidth;
	CutRect.right  = 0;

	revBYTE = (4-BMPFileInfo.biWidth%4)%4;
	revRGB  = BMPFileInfo.biWidth%4;

	
	sizeBYTE = (BMPFileInfo.biWidth  + revBYTE) * BMPFileInfo.biHeight;
	sizeRGB  = (BMPFileInfo.biWidth*3 + revRGB) * BMPFileInfo.biHeight;

	
	buf = (char *)GlobalAlloc( GPTR, sizeRGB );
	
	bmp_bt.alp = (char *)GlobalAlloc( GPTR, sizeBYTE );
	
    if( !ReadFile( hfile, buf, sizeRGB, &actualRead, NULL) ) return 1;
	
	CloseHandle(hfile);
	
	i=0;j=0;
	if(alp250){
		for(y=0; y<BMPFileInfo.biHeight ;y++, i+=revBYTE, j+=revRGB ){
			for(x=0; x<BMPFileInfo.biWidth ;x++, i++, j+=3 ){
				if(buf[j]>248){
					bmp_bt.alp[i] = (char)255;
					if(CutRect.top>y)	CutRect.top    = y;
					if(CutRect.bottom<y)CutRect.bottom = y;
					if(CutRect.left>x)	CutRect.left   = x;
					if(CutRect.right<x)	CutRect.right  = x;
				}else{
					if(buf[j]<8)bmp_bt.alp[i] = 0;
					else{
						bmp_bt.alp[i] = buf[j];
						if(CutRect.top>y)	CutRect.top    = y;
						if(CutRect.bottom<y)CutRect.bottom = y;
						if(CutRect.left>x)	CutRect.left   = x;
						if(CutRect.right<x)	CutRect.right  = x;
					}
				}
			}
		}
	}else{
		for(y=0; y<BMPFileInfo.biHeight ;y++, i+=revBYTE, j+=revRGB ){
			for(x=0; x<BMPFileInfo.biWidth ;x++, i++, j+=3 ){
				bmp_bt.alp[i] = buf[j];
				if(buf[j]){
					if(CutRect.top>y)	CutRect.top    = y;
					if(CutRect.bottom<y)CutRect.bottom = y;
					if(CutRect.left>x)	CutRect.left   = x;
					if(CutRect.right<x)	CutRect.right  = x;
				}
			}
		}
	}
	
	GlobalFree( buf );

	
	hfile = CREATE_READ_FILE( bfname );
	if( hfile == INVALID_HANDLE_VALUE ){
		return 5;
	}
	
	if( !ReadFile( hfile, &BMPFileHead, sizeof( BITMAPFILEHEADER), &actualRead, NULL)) return 6;
	if( !ReadFile( hfile, &BMPFileInfo, sizeof( BITMAPINFOHEADER), &actualRead, NULL)) return 6;
	if( BMPFileInfo.biBitCount != 8){
		CloseHandle(hfile);
		return 7;
	}
	if(BMPFileInfo.biHeight<0) {
		if(rev_flag) { BMPFileInfo.biHeight = -BMPFileInfo.biHeight; }
		else		 { CloseHandle(hfile); return 7; }
	}
	
	palnum = (BMPFileHead.bfOffBits-(sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)))/sizeof(RGB32);
	if( !ReadFile( hfile, pal, sizeof(RGB32)*palnum, &actualRead, NULL)) return 6;
	
	sizeBYTE = (BMPFileInfo.biWidth  + revBYTE) * BMPFileInfo.biHeight;
	
	bmp_bt.buf = (char *)GlobalAlloc( GPTR, sizeof(char)*sizeBYTE );
	buf        = (char *)GlobalAlloc( GPTR, sizeof(char)*sizeBYTE );
	
	if( !ReadFile( hfile, buf, sizeBYTE, &actualRead, NULL) ) return 6;
	
	CloseHandle(hfile);
	i=0;j=0;
	if(alp250){
		for(y=0; y<BMPFileInfo.biHeight ;y++, i+=revBYTE ){
			for(x=0; x<BMPFileInfo.biWidth ;x++, i++ ){
				if( bmp_bt.alp[i] )	bmp_bt.buf[i] = buf[i];
				else				bmp_bt.buf[i] = 0x00;
			}
		}
	}else{
		for(y=0; y<BMPFileInfo.biHeight ;y++, i+=revBYTE ){
			for(x=0; x<BMPFileInfo.biWidth ;x++, i++ ){
				bmp_bt.buf[i] = buf[i];
			}
		}
	}
	
	bmp_bt.sx = BMPFileInfo.biWidth;
	bmp_bt.sy = BMPFileInfo.biHeight;
	
	GlobalFree( buf );


	
	hfile = CREATE_WRITE_FILE( tfname );
	if( hfile == INVALID_HANDLE_VALUE ){
		return 10;
	}
	
	BMPFileHead.bfReserved1 = 1;
	BMPFileInfo.biBitCount = 8;
	if(!CutSave){
		BMPFileHead.bfSize      = sizeBYTE*2 + BMPFileHead.bfOffBits;
		BMPFileInfo.biSizeImage = sizeBYTE*2;
		if( !WriteFile( hfile, &BMPFileHead, sizeof(BITMAPFILEHEADER), &actualRead, NULL)) return 11;
		if( !WriteFile( hfile, &BMPFileInfo, sizeof(BITMAPINFOHEADER), &actualRead, NULL)) return 11;
		
		if( !WriteFile( hfile, pal, sizeof(RGB32)*palnum, &actualRead, NULL) ) return 11;
		
	    if( !WriteFile( hfile, bmp_bt.buf, sizeBYTE, &actualRead, NULL) ) return 11;
		
	    if( !WriteFile( hfile, bmp_bt.alp, sizeBYTE, &actualRead, NULL) ) return 11;
	}else{
		if(rev_flag){
			BMPFileInfo.biYPelsPerMeter = CutRect.top | (BMPFileInfo.biHeight<<16);
			BMPFileInfo.biHeight = -(CutRect.bottom-CutRect.top+1);
		}else{
			BMPFileInfo.biYPelsPerMeter = (BMPFileInfo.biHeight-CutRect.bottom-1) | (BMPFileInfo.biHeight<<16);
			BMPFileInfo.biHeight = CutRect.bottom-CutRect.top+1;
		}
		BMPFileInfo.biXPelsPerMeter = CutRect.left | (BMPFileInfo.biWidth<<16);
		BMPFileInfo.biWidth  = CutRect.right-CutRect.left+1;
		revBYTE  = (4-BMPFileInfo.biWidth%4)%4;
		sizeBYTE = (BMPFileInfo.biWidth  + revBYTE) * BMPFileInfo.biHeight;
		BMPFileHead.bfSize      = sizeBYTE*2 + BMPFileHead.bfOffBits;
		BMPFileInfo.biSizeImage = sizeBYTE*2;
		if( !WriteFile( hfile, &BMPFileHead, sizeof(BITMAPFILEHEADER), &actualRead, NULL)) return 11;
		if( !WriteFile( hfile, &BMPFileInfo, sizeof(BITMAPINFOHEADER), &actualRead, NULL)) return 11;
		
		if( !WriteFile( hfile, pal, sizeof(RGB32)*palnum, &actualRead, NULL) ) return 11;
		
	
		
	
		int	revw1 = bmp_bt.sx+(4-bmp_bt.sx%4)%4;
		int	revw2 = BMPFileInfo.biWidth + revBYTE;
		for( y=CutRect.top ; y<=CutRect.bottom ; y++ ){
			i = y*revw1 + CutRect.left;
			WriteFile( hfile, &bmp_bt.buf[i], revw2, &actualRead, NULL);
		}
		for( y=CutRect.top ; y<=CutRect.bottom ; y++ ){
			i = y*revw1 + CutRect.left;
			WriteFile( hfile, &bmp_bt.alp[i], revw2, &actualRead, NULL);
		}

	}
	
	CloseHandle(hfile);

	GlobalFree( bmp_bt.buf );
	GlobalFree( bmp_bt.alp );
		
	return 0;
}














RGB32 BMP_ChangeNeg_Full( RGB32 src )
{
	*(DWORD*)&src = ~*(DWORD*)&src;
	return src;
}



void BMP_ChangePalNega_Full( RGB32 *pal )
{
	int		i;
	for( i=0; i<256 ;i++ ){
		*(DWORD*)(pal+i) = ~*(DWORD*)(pal+i);
	}
}




RGB32 BMP_ChangeFine_Full( RGB32 src, int rate )
{
	DWORD		wr,wg,wb,rgb;

	wr = src.r;
	wg = src.g;
	wb = src.b;

	rgb= (wr*77 + wg*28 + wb*151)>>8;
	wr = (rgb*rate + wr*(0xff-rate))/0xff;
	wg = (rgb*rate + wg*(0xff-rate))/0xff;
	wb = (rgb*rate + wb*(0xff-rate))/0xff;

	src.r		= (char)wr;
	src.g	= (char)wg;
	src.b		= (char)wb;

	return src;
}



void BMP_ChangePalFine_Full( RGB32 *pal, int rate )
{
	int		i;
	for( i=0; i<256 ;i++ ){
		pal[i] = BMP_ChangeFine_Full( pal[i], rate );
	}
}



RGB32 BMP_ChangeBright_Full( RGB32 src, int r, int g, int b )
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

	src.r   = (char)wr;
	src.g = (char)wg;
	src.b  = (char)wb;


	return src;
}



void BMP_ChangePalBright_Full( RGB32 *pal, int r, int g, int b )
{
	int		i;

	if( r==0x80 && g==0x80 && b==0x80 ) return ;

	for( i=0; i<256 ;i++ ){
		pal[i] = BMP_ChangeBright_Full( pal[i], r, g, b );
	}
}








WORD BMP_ChangeNeg_High( WORD src )
{
	return ~src;
}



void BMP_ChangePalNega_High( WORD *pal )
{
	int		i;
	for( i=0; i<256 ;i++ ){
		*(pal+i) = ~*(pal+i);
	}
}




WORD BMP_ChangeFine_High( WORD src, int rate )
{
	DWORD		wr,wg,wb,rgb;
	DWORD		rr,gg,bb;

	wr = (src&rgb16.mask_r)>>rgb16.pos_r;
	wg = (src&rgb16.mask_g)>>rgb16.pos_g;
	wb =  src&rgb16.mask_b;

	rgb= (wr+wg+wb)/3;
	rr = (rgb*rate + wr*(0xff-rate))/0xff;
	gg = (rgb*rate + wg*(0xff-rate))/0xff;
	bb = (rgb*rate + wb*(0xff-rate))/0xff;

	return (WORD)((rr<<rgb16.pos_r)|(gg<<rgb16.pos_g)|bb);
}




void BMP_ChangePalFine_High( WORD *pal, int rate )
{
	int		i;
	for( i=0; i<256 ;i++ ){
		pal[i] = BMP_ChangeFine_High( pal[i], rate );
	}
}



WORD BMP_ChangeBright_High( WORD src, int r, int g, int b )
{
	DWORD		wr,wg,wb;
	DWORD		rr,rg,rb;

	wr = src&rgb16.mask_r;
	wg = src&rgb16.mask_g;
	wb = src&rgb16.mask_b;

	rr = rgb16.mask_r - wr;
	rg = rgb16.mask_g - wg;
	rb = rgb16.mask_b - wb;

	if( r<=0x80 )wr = ( ( wr *  r         )>>7    )&rgb16.mask_r;
	else			wr = ((( rr * (r-0x80))>>7)+wr)&rgb16.mask_r;

	if( g<=0x80 )wg = ( ( wg *  g         )>>7    )&rgb16.mask_g;
	else			wg = ((( rg * (g-0x80))>>7)+wg)&rgb16.mask_g;

	if( b<=0x80 )wb = ( ( wb *  b         )>>7    )&rgb16.mask_b;
	else			wb = ((( rb * (b-0x80))>>7)+wb)&rgb16.mask_b;

	return (WORD)(wr|wg|wb);
}



void BMP_ChangePalBright_High( WORD *pal, int r, int g, int b )
{
	int		i;
	for( i=0; i<256 ;i++ ){
		pal[i] = BMP_ChangeBright_High( pal[i], r, g, b );
	}
}



WORD BMP_ChangeBright_TH( RGB32 src, int r, int g, int b )
{
	DWORD		wr,wg,wb;
	int			sr,sg,sb;
									
	sr =  rgb16.pos_r-rgb16.num_r;	
	sg =  rgb16.pos_g-rgb16.num_g;	
	sb =-(rgb16.pos_b-rgb16.num_b);	

	wr = src.r;
	wg = src.g;
	wb = src.b;

	if( r<=0x80 )wr = (( (      wr * r         )>>7    )<<sr)&rgb16.mask_r;
	else			wr = (((((0xff-wr)*(r-0x80))>>7)+wr)<<sr)&rgb16.mask_r;

	if( g<=0x80 )wg = (( (      wg * g         )>>7    )<<sg)&rgb16.mask_g;
	else			wg = (((((0xff-wg)*(g-0x80))>>7)+wg)<<sg)&rgb16.mask_g;

	if( b<=0x80 )wb = (( (      wb * b         )>>7    )>>sb)&rgb16.mask_b;
	else			wb = (((((0xff-wb)*(b-0x80))>>7)+wb)>>sb)&rgb16.mask_b;

	return (WORD)(wr|wg|wb);
}



void BMP_ChangePalBright_TH( WORD *dpal,RGB32 *spal, int r, int g, int b )
{
	int		i;

	if( r==128 && g==128 && b==128 ) {
		BMP_ChangePal_TH( dpal, spal );
	}else{
		for( i=0; i<256 ;i++ ){
			dpal[i] = BMP_ChangeBright_TH( spal[i], r, g, b );
		}
	}
}



void BMP_ChangePal_TH( WORD *dpal,RGB32 *spal )
{
	int		i;
	for( i=0; i<256 ;i++ ){
		dpal[i] = BMP_RGB_FH( spal[i].r, spal[i].g, spal[i].b );
	}
}











RGB32		H2F_Tbl[0x10000];	

WORD		F2H_TblRW[256];	
WORD		F2H_TblBW[256];
WORD		F2H_TblGW[256];
DWORD		F2H_TblRD[256];
DWORD		F2H_TblBD[256];
DWORD		F2H_TblGD[256];

static void BMP_CreateTableH2F( void )
{
	int		r,g,b,addr=0;

	if(rgb16.dep_g==63){
		for( r=0; r<256 ;r+=8 ){
			for( g=0; g<256 ;g+=4 ){
				for( b=0; b<256 ;b+=8, addr++ ){
					H2F_Tbl[addr].r   = r+r*8/256;
					H2F_Tbl[addr].g = g+g*4/256;
					H2F_Tbl[addr].b  = b+b*8/256;
				}
			}
		}
	}else{
		for( r=0; r<256 ;r+=8 ){
			for( g=0; g<256 ;g+=8 ){
				for( b=0; b<256 ;b+=8, addr++ ){
					H2F_Tbl[addr].r   = r+r*8/256;
					H2F_Tbl[addr].g = g+g*8/256;
					H2F_Tbl[addr].b  = b+b*8/256;
				}
			}
		}
	}
}

static void BMP_CreateTableF2H( void )
{
	int	i;
	int	r =  rgb16.pos_r - rgb16.num_r;
	int	g =  rgb16.pos_g - rgb16.num_g;
	int	b =-(rgb16.pos_b - rgb16.num_b);

	for( i=0 ; i<256 ; i++ ){
		F2H_TblBW[i] = (i>>b)&rgb16.mask_b;
		F2H_TblGW[i] = (i<<g)&rgb16.mask_g;
		F2H_TblRW[i] = (i<<r)&rgb16.mask_r;

		F2H_TblBD[i] = F2H_TblBW[i]<<16;
		F2H_TblGD[i] = F2H_TblGW[i]<<16;
		F2H_TblRD[i] = F2H_TblRW[i]<<16;
	}
}




DWORD	BrightTable_TF[256][256];
DWORD	BrightTable_TR[256][256];
WORD	BrightTable_TG[256][256];
char	BrightTable_TB[256][256];


char	BlendTable[257][257];	
char	BlendTable2[257][257];	
char	BlendTable16[16][257];


char	DimTable[256][256];


char	AddTable[512];			


char	SubTable[512];			


char	OoiTable[256][256];			


char	PtnTable[768];			


static void BMP_CreateTableBright_T( void )
{
	int	i,j;

	
	for( i=0 ; i<256 ; i++ ){
		for( j=0 ; j<128 ; j++ ){
			BrightTable_TF[j][i] = ( (i*j)>>7 )<<24;
			BrightTable_TR[j][i] = ( (i*j)>>7 )<<16;
			BrightTable_TG[j][i] = ( (i*j)>>7 )<<8;
			BrightTable_TB[j][i] = ( (i*j)>>7 );
		}
		for( j=128 ; j<256 ; j++ ){
			BrightTable_TF[j][i] = ( ( ( (0xff-i)*(j-128) )>>7 ) + i)<<24;
			BrightTable_TR[j][i] = ( ( ( (0xff-i)*(j-128) )>>7 ) + i)<<16;
			BrightTable_TG[j][i] = ( ( ( (0xff-i)*(j-128) )>>7 ) + i)<<8;
			BrightTable_TB[j][i] = ( ( ( (0xff-i)*(j-128) )>>7 ) + i);
		}
	}
	
	for( i=0 ; i<=256 ; i++ ){
		for( j=0 ; j<=256 ; j++ ){
			BlendTable[i][j]  = LIM( (i*j)>>8,     0, 255 );
			BlendTable2[i][j] = LIM( (j<<8)/(i+1), 0, 255 );
		}
	}
	for( i=0 ; i<16 ; i++ ){
		for( j=0 ; j<257; j++ ){
			BlendTable16[i][j] = i*j/15;
		}
	}
	for( i=0 ; i<256 ; i++ ){
		for( j=0 ; j<256 ; j++ ){
			DimTable[i][j] = ( i*min(i,254) + j*(255-min(i,254)) )/255;
		}
	}
	for( i=0 ; i<512 ; i++ )AddTable[i] = min(i,255);	
	for( i=0 ; i<512 ; i++ )SubTable[i] = max(i-255,0);	
	for( i=0 ; i<256 ; i++ ){
		for( j=0 ; j<256 ; j++ ){
			OoiTable[i][j] = min(255, i*256/(256-j) );
		}
	}



	for( i=0 ; i<256+128+256 ; i++ ){
		PtnTable[i] = LIM( (i-256)*2 ,0,255);
	}

}




static WORD	BrightTable_H[0x10000];
static int	BrightTable_RGB[3] = { -1, -1, -1 };

WORD	BrightTable_HR[256][32];	
WORD	BrightTable_HG[256][64];	
WORD	BrightTable_HB[256][32];	

static void BMP_CreateTableBright_H( void )
{
	int	i,j;
	
	for( i=0 ; i<32 ; i++ ){
		for( j=0 ; j<128 ; j++ ){
			BrightTable_HR[j][i] = (((i*j)>>7)<<rgb16.pos_r)&rgb16.mask_r;
			BrightTable_HB[j][i] = (((i*j)>>7)<<rgb16.pos_b)&rgb16.mask_b;
		}
		for( j=128 ; j<256 ; j++ ){
			BrightTable_HR[j][i] = (((((rgb16.dep_r-i)*(j-0x80))>>7)+i)<<rgb16.pos_r)&rgb16.mask_r;
			BrightTable_HB[j][i] = (((((rgb16.dep_b-i)*(j-0x80))>>7)+i)<<rgb16.pos_b)&rgb16.mask_b;
		}
	}
	for( i=0 ; i<rgb16.dep_g+1 ; i++ ){
		for( j=0 ; j<128 ; j++ ){
			BrightTable_HG[j][i] = (((i*j)>>7)<<rgb16.pos_g)&rgb16.mask_g;
		}
		for( j=128 ; j<256 ; j++ ){
			BrightTable_HG[j][i] = (((((rgb16.dep_g-i)*(j-0x80))>>7)+i)<<rgb16.pos_g)&rgb16.mask_g;
		}
	}
}




static void CreateHighColorBrightTable( int r,int g, int b, WORD *high_tbl )
{
	int		adr=0;
	int		rrr;
	int		rr,gg,bb;
	int		r2 = r-0x80, g2 = g-0x80, b2 = b-0x80;
	WORD	btbl[32], gtbl[64];
	int		i;
	int		dr = rgb16.dep_r+1;
	int		dg = rgb16.dep_g+1;
	int		db = rgb16.dep_b+1;

	if(b<=0x80) { for(i=0; i<db ;i++)	btbl[i] = (i*b)>>7;										}
	else		{ for(i=0; i<db ;i++)	btbl[i] = (((rgb16.dep_b-i)*b2)>>7)+i;					}
	if(g<=0x80) { for(i=0; i<dg ;i++)	gtbl[i] = ((i*g)>>7)<<rgb16.pos_g;						}
	else		{ for(i=0; i<dg ;i++)	gtbl[i] = ((((rgb16.dep_g-i)*g2)>>7)+i)<<rgb16.pos_g;	}

	if(r<=0x80){
		for( rr=0 ; rr < dr ; rr++ ){
			rrr = ( (    rr *r )>>7    )<<rgb16.pos_r;
			for( gg=0 ; gg < dg ; gg++ ){
				for( bb= 0 ; bb < db ; bb++, adr++){
					high_tbl[adr] = (WORD)( rrr | gtbl[gg] | btbl[bb] );
				}
			}
		}
	}else{
		for( rr=0 ; rr < dr ; rr++ ){
			rrr = ((((rgb16.dep_r-rr)*r2)>>7)+rr)<<rgb16.pos_r;
			for( gg=0 ; gg < dg ; gg++ ){
				for( bb= 0 ; bb < db ; bb++, adr++){
					high_tbl[adr] = (WORD)( rrr | gtbl[gg] | btbl[bb] );
				}
			}
		}
	}
}



WORD *BMP_CreateBrightTable_H( int r, int g, int b )
{
	if( !(BrightTable_RGB[0]==r && BrightTable_RGB[1]==g && BrightTable_RGB[2]==b) ){
		BrightTable_RGB[0] = r;
		BrightTable_RGB[1] = g;
		BrightTable_RGB[2] = b;
		CreateHighColorBrightTable( r, g, b, BrightTable_H);
	}
	return BrightTable_H;
}



char	MeshFadePatern64[64][64][64];




void BMP_CreateRandMeshTable(void)
{
	int	i,j,k;
	
	for( i=0 ; i<64 ; i++ ){
		for( j=0 ; j<64 ; j++ ){
			for( k=0 ; k<64 ; k++ ){
				MeshFadePatern64[i][j][k] = (rand()&63) < i;
			}
		}
	}

}

short BomdTabl[256][256];		



void BMP_CreatePixelBombTable(void)
{
	int		i,j;
	for( i=0; i<256 ;i++ ){
		BomdTabl[255][i] = rand()%1280-640;

		for( j=0; j<255 ;j++ ){
			BomdTabl[j][i] = BomdTabl[255][i]*j/255;
		}
	}
}







void BMP_InitBmpinfo( int disp_w, int disp_h )
{
	bmi_8.bmiHeader.biWidth  = disp_w;
	bmi_8.bmiHeader.biHeight = disp_h;
	bmi_8.bmiHeader.biSizeImage = disp_w*disp_h;

	bmi_16_4444.bmiHeader.biWidth  = disp_w;
	bmi_16_4444.bmiHeader.biHeight = disp_h;
	bmi_16_4444.bmiHeader.biSizeImage = disp_w*disp_h*2;

	bmi_16_565.bmiHeader.biWidth  = disp_w;
	bmi_16_565.bmiHeader.biHeight = disp_h;
	bmi_16_565.bmiHeader.biSizeImage = disp_w*disp_h*2;

	bmi_16_555.bmiHeader.biWidth  = disp_w;
	bmi_16_555.bmiHeader.biHeight = disp_h;
	bmi_16_555.bmiHeader.biSizeImage = disp_w*disp_h*2;

	bmi_24.bmiHeader.biWidth  = disp_w;
	bmi_24.bmiHeader.biHeight = disp_h;
	bmi_24.bmiHeader.biSizeImage = disp_w*disp_h*3;

	bmi_32.bmiHeader.biWidth  = disp_w;
	bmi_32.bmiHeader.biHeight = disp_h;
	bmi_32.bmiHeader.biSizeImage = disp_w*disp_h*4;
}

void BMP_InitBmp( int disp_w, int disp_h )
{
	BMP_InitBmpinfo( disp_w, disp_h );

	BMP_SetPixelFormat16( BIT16_555 );
	
	BMP_CreateTableH2F();
	BMP_CreateTableF2H();

	BMP_CreateTableBright_H();
	BMP_CreateTableBright_T();

	BMP_CreateRandMeshTable();
	BMP_CreatePixelBombTable();
}







int BMP_EncodeVisual( char *dst_fname, char *src_fname, char *dir )
{
	int					i,j,k,x,y,rev=0;
	DWORD               actualRead;
    HANDLE              hfile;
    BITMAPFILEHEADER    BMPFileHead;
    BITMAPINFOHEADER    BMPFileInfo;
	int					size;
	unsigned char		*buf;
	unsigned char		*buf2;
	char				src_path[1024];
	char				dst_path[1024];

	wsprintf( src_path, "%s\\%s", dir, src_fname );
	wsprintf( dst_path, "%s\\%s", dir, dst_fname );

	
	hfile = CREATE_READ_FILE( src_path );
	if( hfile == INVALID_HANDLE_VALUE )	return 1;
	
	if( !ReadFile( hfile, &BMPFileHead, sizeof( BITMAPFILEHEADER), &actualRead, NULL)) { CloseHandle(hfile); return 2; }
	if( !ReadFile( hfile, &BMPFileInfo, sizeof( BITMAPINFOHEADER), &actualRead, NULL)) { CloseHandle(hfile); return 2; }
	
	if( BMPFileInfo.biBitCount != 24) { CloseHandle(hfile); return 3; }
	if( BMPFileInfo.biHeight>0 ) { BMPFileInfo.biHeight = -BMPFileInfo.biHeight; rev=1; }

	size = BMPFileHead.bfSize-BMPFileHead.bfOffBits;
	int palnum = BMPFileHead.bfOffBits-(sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER));
	
	
	buf = (unsigned char *)GlobalAlloc( GPTR, size );
	buf2 = (unsigned char *)GlobalAlloc( GPTR, size );


	
    if(palnum){
		if( !ReadFile( hfile, buf, palnum, &actualRead, NULL) ) { CloseHandle(hfile); return 2; }
	}
	
    if( !ReadFile( hfile, buf, size, &actualRead, NULL) ) { CloseHandle(hfile); return 2; }
	
	CloseHandle(hfile);

	int	key1,key2,key3;
	i=0;
	key1=256*256;
	while(dst_fname[i]){
		key1 -= (BYTE)dst_fname[i];
		i++;
	}
	i=0;
	key2=0;
	while(dst_fname[i]){
		key2 += (BYTE)dst_fname[i];
		i++;
	}
	i=0;
	key3 = 0;
	while(dst_fname[i]){
		key3 |= (BYTE)dst_fname[i];
		i++;
	}
	
	int	h = abs(BMPFileInfo.biHeight);
	y = key3 % h;
	for(j=0; j<h ;j++ ){
		y = (y+7)%h;
		for(x=0; x<BMPFileInfo.biWidth*3 ;x++ ){
			i = x+y*BMPFileInfo.biWidth*3;
			if(rev)	k = x+(h-1-j)*BMPFileInfo.biWidth*3;
			else	k = x+j*BMPFileInfo.biWidth*3;
			if(x%2){
				buf2[i] = (buf[k]+key2)%256;
			}else{
				buf2[i] = 255-(buf[k]+key1)%256;
			}
		}
	}


	
	hfile = CREATE_WRITE_FILE( dst_path );
	if( hfile == INVALID_HANDLE_VALUE )	return 11;
	
	BMPFileHead.bfSize = BMPFileInfo.biWidth*abs(BMPFileInfo.biHeight)*3+sizeof( BITMAPFILEHEADER)+sizeof( BITMAPINFOHEADER);
	BMPFileHead.bfOffBits = (sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER));
	
	
	if( !WriteFile( hfile, &BMPFileHead, sizeof( BITMAPFILEHEADER), &actualRead, NULL)) { CloseHandle(hfile); return 12; }
	if( !WriteFile( hfile, &BMPFileInfo, sizeof( BITMAPINFOHEADER), &actualRead, NULL)) { CloseHandle(hfile); return 12; }
	
	if( !WriteFile( hfile, buf2, size, &actualRead, NULL) ) return 12;
	
	CloseHandle(hfile);

	
	GlobalFree( buf );
	return 0;
}

