
 
  
   
    
     
      

#include "mm_std.h"

#include "bmp.h"
#include "draw.h"
#include "drawprim.h"
#include "disp.h"
#include "text.h"
#include "dispSprite.h"
#include "movie.h"
//#include "pcm.h"

#include "comp_pac.h"
#include "comp_BmpPac.h"

int	DISP_X = 800;
int	DISP_Y = 600;
int	DISP_DY = 600;


#define LAYER_MAX	64

typedef struct{
	char	flag;		
	char	disp;		
	char	layer;		
	char	type;		
						
	char	poly;		
						
	char	bno;		

	char	bset;		
	char	bno2;		
	char	bno3;		

	short	ac_no;		

	char	r,g,b;		
	char	r2,g2,b2;	
	char	r3,g3,b3;	
	char	r4,g4,b4;	
	char	brt_flag;	
	DWORD	param;		
	DWORD	param2;		
	DWORD	param3;		
	DWORD	rparam;		

	long	nuki;		

	short	dx,dy;		
	short	dx2,dy2;	
	short	dx3,dy3;	
	short	dx4,dy4;	
	short	dw,dh;		

	short	sx,sy;		
	short	sx2,sy2;	
	short	sx3,sy3;	
	short	sx4,sy4;	
	short	sw,sh;		
	short	sw2,sh2;	

	short	zoom;		
	short	cx,cy;		

	short	target;

	BYTE	*str;		

	RECT	*clip;		
	short	sp_clip;
}GRP_STRUCT;

typedef struct{
	char	flag;		
	char	disp;		
	char	layer;		
	char	font;		

	char	*str;		
	char	color;		
	char	brt_flag;	
	short	alph;		
	char	kage;		
	short	cnt;		

	short	dx,dy;		
	short	ws,hs;		
	short	pw,ph;		

	int		px,py;		
	short	step;		

	short	target;

	RECT	*clip;		
}TEXT_STRUCT;

typedef struct{
	int		flag;
	int		disp;
	int		dx;
	int		dy;
	int		type;
	int		digit;
	char	str[32];
	int		param;
}DMOJI_STRUCT;


#define GRP_MAX		768
#define TXT_MAX		128
#define DMOJI_MAX	32

static GRP_STRUCT	GrpStruct[GRP_MAX];		
static TEXT_STRUCT	TextStruct[TXT_MAX];	
static DMOJI_STRUCT	DmojiStruct[DMOJI_MAX];

#define BMP_MAX		256


static BMP_SET			BmpSet[BMP_MAX];
static BMP_SET			VramBmp;


typedef struct{
	int		flag;
	int		bmp_no;
	int		sx,sy;
	int		bmp_bit;
}MOVE_CONTOROL;

static MOVE_CONTOROL	MoveControl[MOVIE_MAX];


static int		BrightR = BRT_NML;
static int		BrightG = BRT_NML;
static int		BrightB = BRT_NML;

static int		GetBackNo;
static int		GetBackFlag;

static int		DispTempNo = -1;	

static int		GlobalX = 0;
static int		GlobalY = 0;

static BOOL DrawGraphBmp( void *dest, int x, int y, GRP_STRUCT *gs, int draw_mode, int interless );
static BOOL DrawGraphDigit( void *dest, int x, int y, GRP_STRUCT *gs, int draw_mode );
static BOOL DrawGraphStr( void *dest, int x, int y, GRP_STRUCT *gs, int draw_mode );
static BOOL DrawGraphSprite( void *dest, int x, int y, GRP_STRUCT *gs, int draw_mode );
static BOOL DrawGraphPrim( void *dest, int x, int y, GRP_STRUCT *gs, int draw_mode );
static BOOL DrawGraphText( void *dest, int x, int y, TEXT_STRUCT *ts, int draw_mode, int kaigyou_musi=FALSE );




void DSP_InitDisp( int grp, int txt, int bmp, int lay )
{
	if( lay >= LAYER_MAX )	DebugBox(NULL, "レイヤーの数が多すぎます" );
	if( grp >= GRP_MAX )	DebugBox(NULL, "登録グラフィックの数が多すぎます" );
	if( txt >= TXT_MAX )	DebugBox(NULL, "登録テキストの数が多すぎます" );
	if( bmp >= BMP_MAX )	DebugBox(NULL, "登録BMPの数が多すぎます" );
	
	DSP_TermDisp();

	PAC_PackFileMovie_Init();

	DRW_InitRipleMapping(DISP_X,DISP_Y,DISP_X/2,DISP_Y/2);
}




void DSP_TermDisp( void )
{
	DSP_ReleaseAviAll();

	DSP_ResetGraphAll();
	DSP_ResetTextAll();
	DSP_ReleaseBmpAll();
	DSP_ReleaseBmpAnimeAll();
	DSP_ReleaseSpriteAll();
	PAC_PackFileMovie_Term();
	DSP_SetGraphGlobalBright( 128,128,128 );
}



void DSP_ResetGraphAll( void )
{
	int	i;
	for( i=0; i<GRP_MAX ;i++ ){
		DSP_ResetGraph( i );
	}
}



void DSP_ResetTextAll( void )
{
	int	i;
	for( i=0; i<TXT_MAX ;i++ ){
		DSP_ResetText( i );
	}
}



void DSP_ReleaseBmpAll( void )
{
	int	i;
	for( i=0; i<BMP_MAX ;i++ ){
		DSP_ReleaseBmp( i );
	}
}



void DSP_ReleaseBmpAnimeAll( void )
{
	int	i;
	for( i=0; i<MOVIE_MAX ;i++ ){
		DSP_ReleaseBmpAnime( i );
	}
}



void DSP_ReleaseAviAll( void )
{
	int	i;
	for( i=0 ; i<AVI_MAX ; i++ ){
		DSP_ReleaseAvi( i );
	}
}



void DSP_ReleaseSpriteAll(void )
{
	SPR_ReleaseSpriteAll();
}




void DSP_ReleaseBmp( int bno )
{
	BMP_ReleaseBMP_BT( &BmpSet[bno].bmp_b );
	BMP_ReleaseBMP_H(  &BmpSet[bno].bmp_h );
	BMP_ReleaseBMP_F(  &BmpSet[bno].bmp_f );
	BMP_ReleaseBMP_T(  &BmpSet[bno].bmp_t );
	ZeroMemory( &BmpSet[bno], sizeof(BMP_SET) );
}



void DSP_CreateBmp( int bno, int bmp_bit, int w, int h )
{
	DSP_ReleaseBmp( bno );

	BmpSet[bno].flag    = 1;
	BmpSet[bno].bmp_bit = bmp_bit;
	BmpSet[bno].pos.x = 0;
	BmpSet[bno].pos.y = 0;
	BmpSet[bno].size.x = w;
	BmpSet[bno].size.y = h;
	switch( bmp_bit ){
		case BMP_256P:	BMP_CreateBMP_BT(&BmpSet[bno].bmp_b, w, h, 0 );	break;
		case BMP_HIGH:	BMP_CreateBMP_H( &BmpSet[bno].bmp_h, w, h, 0 );	break;
		case BMP_FULL:	BMP_CreateBMP_F( &BmpSet[bno].bmp_f, w, h );	break;
		case BMP_TRUE:	BMP_CreateBMP_T( &BmpSet[bno].bmp_t, w, h );	break;
	}
}



void DSP_GetBmpSize( int bno, int *sx, int *sy )
{
	*sx = BmpSet[bno].size.x;
	*sy = BmpSet[bno].size.y;






}



void DSP_GetBmpSize2( BMP_SET bs, int *sx, int *sy )
{
	*sx = bs.size.x;
	*sy = bs.size.y;






}




BOOL DSP_ChangeBmp( int bno, int chg_bpp )
{
	BOOL	ret=FALSE;
	int		t2f=0;
	int		sx, sy;
	BMP_H	bmp_h;
	BMP_F	bmp_f;
	BMP_T	bmp_t;
	if( !BmpSet[bno].flag ) return FALSE;
	if( chg_bpp!=BMP_HIGH && chg_bpp!=BMP_FULL && chg_bpp!=BMP_TRUE )	return FALSE;
	if( chg_bpp==BmpSet[bno].bmp_bit || BmpSet[bno].bmp_bit==BMP_256P )	return TRUE;

	ZeroMemory( &bmp_h, sizeof(bmp_h) );
	ZeroMemory( &bmp_f, sizeof(bmp_f) );
	ZeroMemory( &bmp_t, sizeof(bmp_t) );

	
	DSP_GetBmpSize( bno, &sx, &sy );
	switch(chg_bpp){
		case BMP_HIGH:	BMP_CreateBMP_H( &bmp_h, sx, sy, BmpSet[bno].alp_flag );	break;
		case BMP_FULL:	BMP_CreateBMP_F( &bmp_f, sx, sy );							break;
		case BMP_TRUE:	BMP_CreateBMP_T( &bmp_t, sx, sy );							break;
	}
	
	switch(BmpSet[bno].bmp_bit){
		case BMP_HIGH:
			if(BmpSet[bno].alp_flag){
				chg_bpp = BMP_TRUE;
				BMP_CreateBMP_T( &bmp_t, sx, sy );
				ret=DRW_ChangeBMP_H2T( &bmp_t, &BmpSet[bno].bmp_h, 1 );
			}else{
				switch(chg_bpp){
					case BMP_FULL:	ret=DRW_ChangeBMP_H2F( &bmp_f, &BmpSet[bno].bmp_h );		break;
					case BMP_TRUE:	ret=DRW_ChangeBMP_H2T( &bmp_t, &BmpSet[bno].bmp_h, 0 );	break;
				}
			}
			break;
		case BMP_FULL:
			switch(chg_bpp){
				case BMP_HIGH:	ret=DRW_ChangeBMP_F2H( &bmp_h, &BmpSet[bno].bmp_f );	break;
				case BMP_TRUE:	ret=DRW_ChangeBMP_F2T( &bmp_t, &BmpSet[bno].bmp_f );	break;
			}
			break;
		case BMP_TRUE:
			if(BmpSet[bno].alp_flag){
				switch(chg_bpp){
					case BMP_HIGH:	ret=DRW_ChangeBMP_T2H( &bmp_h, &BmpSet[bno].bmp_t, 1 );	break;
					case BMP_FULL:	ret=TRUE;	t2f=1;	chg_bpp = BMP_TRUE;	break;
				}
			}else{
				switch(chg_bpp){
					case BMP_HIGH:	ret=DRW_ChangeBMP_T2H( &bmp_h, &BmpSet[bno].bmp_t, 0 );	break;
					case BMP_FULL:	ret=DRW_ChangeBMP_T2F( &bmp_f, &BmpSet[bno].bmp_t );		break;
				}
			}
			break;
	}
	
	if(ret){
		BmpSet[bno].bmp_bit = chg_bpp;
		if( !t2f ){
			BMP_ReleaseBMP_H(  &BmpSet[bno].bmp_h );
			BMP_ReleaseBMP_F(  &BmpSet[bno].bmp_f );
			BMP_ReleaseBMP_T(  &BmpSet[bno].bmp_t );
			BmpSet[bno].bmp_h = bmp_h;
			BmpSet[bno].bmp_f = bmp_f;
			BmpSet[bno].bmp_t = bmp_t;
		}
	}else{
		BMP_ReleaseBMP_H( &bmp_h );
		BMP_ReleaseBMP_F( &bmp_f );
		BMP_ReleaseBMP_T( &bmp_t );
	}
	return ret;
}



BOOL DSP_ChangeBmpAll( int chg_bpp )
{
	int		i;
	BOOL	ret=TRUE;

	for( i=0; i<BMP_MAX ;i++ ){
		if(BmpSet[i].flag){
			if( !DSP_ChangeBmp( i, chg_bpp ) ){
				DebugPrintf("変換失敗[%d]\n",i);
				ret=FALSE;
			}
		}
	}
	if(ret){
		DebugPrintf( "%dBITに変換成功\n", 8*(1+chg_bpp) );
	}
	return ret;
}




void DSP_SetDispTemp( int bno )
{
	DispTempNo = bno;
}
void DSP_ResetDispTemp( void )
{
	DispTempNo = -1;
}




void DSP_SetDrawTemp( int bno, int draw_mode, int frame )
{
	DSP_CreateBmp( bno, BPP(draw_mode), DISP_X, DISP_Y );

	switch(draw_mode){
		case 8:	DSP_DrawGraph( &BmpSet[bno].bmp_b, draw_mode,0, frame,0, 1 );	break;
		case 16:DSP_DrawGraph( &BmpSet[bno].bmp_h, draw_mode,0, frame,0, 1 );	break;
		case 24:DSP_DrawGraph( &BmpSet[bno].bmp_f, draw_mode,0, frame,0, 1 );	break;
		case 32:DSP_DrawGraph( &BmpSet[bno].bmp_t, draw_mode,0, frame,0, 1 );	break;
	}
}




void DSP_GetDispBmp( int bno, int draw_mode )
{
	GetBackNo   = bno;
	GetBackFlag = 1;
	DSP_CreateBmp( bno, BPP(draw_mode), DISP_X, DISP_Y );
}




BOOL LoadBmpSet( BMP_SET	*bmp_set, int bmp_bit, char *fname, char *pack )
{
	BOOL	ret=FALSE;
	bmp_set->flag     = 1;
	bmp_set->bmp_bit  = bmp_bit;
	bmp_set->alp_flag = 0;

	ret = BMP_LoadBMP_X2X( fname, bmp_set, pack );

	return ret;
}



BOOL DSP_LoadBmp( int bno, int bmp_bit, char *fname, char *tfname, int viv, char *pack )
{
	int		x,y,i;
	char	*alp_tbl;

	if( FILE_SPOIT ){
		char	src[MAX_PATH],dst[MAX_PATH];
		if(pack){
			wsprintf( dst, "%s\\buf\\%s", pack, fname );
			wsprintf( src, "%s\\%s", pack, fname );
			CopyFile( src, dst, TRUE);
		}else{
			wsprintf( dst, "grp\\buf\\%s", fname );
			wsprintf( src, "grp\\%s", fname );
			CopyFile( src, dst, TRUE);
		}
		if(tfname){
			wsprintf( dst, "grp\\buf\\%s", tfname );
			wsprintf( src, "grp\\%s", tfname );
			CopyFile( src, dst, TRUE);
		}
	}

	DSP_ReleaseBmp( bno );
	if( !LoadBmpSet( &BmpSet[bno], bmp_bit, fname, pack ) ) return FALSE;
	switch( bmp_bit ){
		case 0:	BMP_SetTonecurve_BT(&BmpSet[bno].bmp_b, tfname, viv );	break;
		case 1: BMP_SetTonecurve_H( &BmpSet[bno].bmp_h, tfname, viv );	break;
		case 2: BMP_SetTonecurve_F( &BmpSet[bno].bmp_f, tfname, viv );	break;
		case 3:
			i=0;
			for(y=0;y<BmpSet[bno].bmp_t.sy;y++){
				for(x=0;x<BmpSet[bno].bmp_t.sx;x++,i++){
					alp_tbl = BlendTable[  BmpSet[bno].bmp_t.buf[i].a ];

					BmpSet[bno].bmp_t.buf[i].b = alp_tbl[BmpSet[bno].bmp_t.buf[i].b];
					BmpSet[bno].bmp_t.buf[i].g = alp_tbl[BmpSet[bno].bmp_t.buf[i].g];
					BmpSet[bno].bmp_t.buf[i].r = alp_tbl[BmpSet[bno].bmp_t.buf[i].r];
				}
			}
			BMP_SetTonecurve_T( &BmpSet[bno].bmp_t, tfname, viv );
			break;
	}
	return TRUE;
}



BOOL DSP_LoadBmp2( int bno, int bmp_bit, char *fname, char *tfname, int viv, int r, int g, int b, int param, int rparam, char *pack )
{
	int		x,y,i;
	char	*alp_tbl;
	BMP_SET		bmp_set;
	DRAW_OBJECT	dobj;
	int			tnc=0;

	if( r==128 && g==128 && b==128 && param==DRW_NML && rparam==REV_NOT ) return DSP_LoadBmp( bno, bmp_bit, fname, tfname, viv, pack );


	if( FILE_SPOIT ){
		char	src[MAX_PATH],dst[MAX_PATH];
		if(pack){
			wsprintf( dst, "%s\\buf\\%s", pack, fname );
			wsprintf( src, "%s\\%s", pack, fname );
			CopyFile( src, dst, TRUE);
		}else{
			wsprintf( dst, "grp\\buf\\%s", fname );
			wsprintf( src, "grp\\%s", fname );
			CopyFile( src, dst, TRUE);
		}
		if(tfname){
			wsprintf( dst, "grp\\buf\\%s", tfname );
			wsprintf( src, "grp\\%s", tfname );
			CopyFile( src, dst, TRUE);
		}
	}

	ZeroMemory( &bmp_set, sizeof(bmp_set) );
	if( !LoadBmpSet( &bmp_set, bmp_bit, fname, pack ) ) return FALSE;

	switch( bmp_bit ){
		case 0:	DSP_CreateBmp( bno, bmp_bit, bmp_set.bmp_b.sx, bmp_set.bmp_b.sy );	break;
		case 1: DSP_CreateBmp( bno, bmp_bit, bmp_set.bmp_h.sx, bmp_set.bmp_h.sy );	break;
		case 2: DSP_CreateBmp( bno, bmp_bit, bmp_set.bmp_f.sx, bmp_set.bmp_f.sy );	break;
		case 3: DSP_CreateBmp( bno, bmp_bit, bmp_set.bmp_t.sx, bmp_set.bmp_t.sy );	break;
	}
	BmpSet[bno].flag     = 1;
	BmpSet[bno].bmp_bit  = bmp_set.bmp_bit;
	BmpSet[bno].alp_flag = bmp_set.alp_flag;
	BmpSet[bno].pos  = bmp_set.pos;
	BmpSet[bno].size = bmp_set.size;

	switch( bmp_bit ){
		case 0:
			DRW_DrawBMP_BB( &BmpSet[bno].bmp_b, 0, 0, &bmp_set.bmp_b, 0, 0, bmp_set.bmp_b.sx, bmp_set.bmp_b.sy, CHK_NO, (RECT*)-1, r, g, b, param, rparam );
			break;
		case 1:
			dobj = DRW_SetDrawObject( BMP_HIGH, &BmpSet[bno].bmp_h, 0, 0, bmp_set.bmp_h.sx, bmp_set.bmp_h.sy,
									  BMP_HIGH, &bmp_set.bmp_h,     0, 0, bmp_set.bmp_h.sx, bmp_set.bmp_h.sy, 0,NULL, 0, 0, 0, 0,
									  r, g, b, CHK_NO, (RECT*)-1, param, 0,0, rparam );
			DRW_DrawBMP_HH(  dobj );
			break;
		case 2:
			dobj = DRW_SetDrawObject( BMP_FULL, &BmpSet[bno].bmp_f, 0, 0, bmp_set.bmp_f.sx, bmp_set.bmp_f.sy,
									  BMP_FULL, &bmp_set.bmp_f,     0, 0, bmp_set.bmp_f.sx, bmp_set.bmp_f.sy, 0,(RECT*)NULL, 0, 0, 0, 0,
									  r, g, b, CHK_NO, (RECT*)-1, param, 0,0, rparam );
			DRW_DrawBMP_FF( dobj );
			break;
		case 3:
			dobj = DRW_SetDrawObject( BMP_TRUE, &BmpSet[bno].bmp_t, 0, 0, bmp_set.bmp_t.sx, bmp_set.bmp_t.sy,
									  BMP_TRUE, &bmp_set.bmp_t,     0, 0, bmp_set.bmp_t.sx, bmp_set.bmp_t.sy, 0,(RECT*)NULL, 0, 0, 0, 0,
									  r, g, b, CHK_NO, (RECT*)-1, param, 0,0, rparam );
			DRW_DrawBMP_TT(  dobj );
			break;
	}
	BMP_ReleaseBMP_BT( &bmp_set.bmp_b );
	BMP_ReleaseBMP_H(  &bmp_set.bmp_h );
	BMP_ReleaseBMP_F(  &bmp_set.bmp_f );
	BMP_ReleaseBMP_T(  &bmp_set.bmp_t );

	switch( bmp_bit ){
		case 0:	BMP_SetTonecurve_BT(&BmpSet[bno].bmp_b, tfname, viv );	break;
		case 1: BMP_SetTonecurve_H( &BmpSet[bno].bmp_h, tfname, viv );	break;
		case 2: BMP_SetTonecurve_F( &BmpSet[bno].bmp_f, tfname, viv );	break;
		case 3:
			i=0;
			for(y=0;y<BmpSet[bno].bmp_t.sy;y++){
				for(x=0;x<BmpSet[bno].bmp_t.sx;x++,i++){
					alp_tbl = BlendTable[  BmpSet[bno].bmp_t.buf[i].a ];

					BmpSet[bno].bmp_t.buf[i].b = alp_tbl[BmpSet[bno].bmp_t.buf[i].b];
					BmpSet[bno].bmp_t.buf[i].g = alp_tbl[BmpSet[bno].bmp_t.buf[i].g];
					BmpSet[bno].bmp_t.buf[i].r = alp_tbl[BmpSet[bno].bmp_t.buf[i].r];
				}
			}
			
			BMP_SetTonecurve_T( &BmpSet[bno].bmp_t, tfname, viv );
			break;
	}
	return TRUE;
}




void DSP_CopyBmp( int db_no, int sb_no )
{
	if( db_no != sb_no ){
		if(sb_no==-1){
			BmpSet[db_no].flag     = 1;
			BmpSet[db_no].bmp_bit  = VramBmp.bmp_bit;
			BmpSet[db_no].alp_flag = VramBmp.alp_flag;
			BmpSet[db_no].pos      = VramBmp.pos;
			BmpSet[db_no].size     = VramBmp.size;
			switch( BmpSet[db_no].bmp_bit ){
				case 0:	BMP_CopyBMP_BT( &BmpSet[db_no].bmp_b, &VramBmp.bmp_b );	break;
				case 1:	BMP_CopyBMP_H(  &BmpSet[db_no].bmp_h, &VramBmp.bmp_h );	break;
				case 2:	BMP_CopyBMP_F(  &BmpSet[db_no].bmp_f, &VramBmp.bmp_f );	break;
				case 3:	BMP_CopyBMP_T(  &BmpSet[db_no].bmp_t, &VramBmp.bmp_t );	break;
			}
		}else{
			if(BmpSet[sb_no].flag){
				BmpSet[db_no].flag     = 1;
				BmpSet[db_no].bmp_bit  = BmpSet[sb_no].bmp_bit;
				BmpSet[db_no].alp_flag = BmpSet[sb_no].alp_flag;
				BmpSet[db_no].pos      = BmpSet[sb_no].pos;
				BmpSet[db_no].size     = BmpSet[sb_no].size;
				switch( BmpSet[db_no].bmp_bit ){
					case 0:	BMP_CopyBMP_BT( &BmpSet[db_no].bmp_b, &BmpSet[sb_no].bmp_b );	break;
					case 1:	BMP_CopyBMP_H(  &BmpSet[db_no].bmp_h, &BmpSet[sb_no].bmp_h );	break;
					case 2:	BMP_CopyBMP_F(  &BmpSet[db_no].bmp_f, &BmpSet[sb_no].bmp_f );	break;
					case 3:	BMP_CopyBMP_T(  &BmpSet[db_no].bmp_t, &BmpSet[sb_no].bmp_t );	break;
				}
			}else{
				DebugPrintf("ソースの無い画像コピーを実行しました");
			}
		}
	}
}
void DSP_CopyBmp2( int db_no, int sb_no, char *tfname, int viv, int r, int g, int b )
{
	DRAW_OBJECT	dobj;
	BMP_SET		bmp_set;

	if( db_no != sb_no ){
		if(sb_no==-1){
			bmp_set = VramBmp;
		}else{
			if(!BmpSet[sb_no].flag) { DebugPrintf("ソースの無い画像コピーを実行しました"); return; }
			bmp_set = BmpSet[sb_no];
		}
		BmpSet[db_no].flag     = 1;
		BmpSet[db_no].bmp_bit  = bmp_set.bmp_bit;
		BmpSet[db_no].alp_flag = bmp_set.alp_flag;
		BmpSet[db_no].pos      = bmp_set.pos;
		BmpSet[db_no].size     = bmp_set.size;
		switch( bmp_set.bmp_bit ){
			case 0:

				break;
			case 1:
				BMP_CreateBMP_H( &BmpSet[db_no].bmp_h, bmp_set.bmp_h.sx, bmp_set.bmp_h.sy, 0 );
				dobj = DRW_SetDrawObject( BMP_HIGH, &BmpSet[db_no].bmp_h, 0, 0, bmp_set.bmp_h.sx, bmp_set.bmp_h.sy,
										  BMP_HIGH, &bmp_set.bmp_h,     0, 0, bmp_set.bmp_h.sx, bmp_set.bmp_h.sy, 0,NULL, 0, 0, 0, 0,
										  r, g, b, CHK_NO, (RECT*)-1, DRW_NML, 0,0, 0 );
				DRW_DrawBMP_HH(  dobj );
				BMP_SetTonecurve_H( &BmpSet[db_no].bmp_h, tfname, viv );
				break;
			case 2:
				BMP_CreateBMP_F( &BmpSet[db_no].bmp_f, bmp_set.bmp_f.sx, bmp_set.bmp_f.sy );
				dobj = DRW_SetDrawObject( BMP_FULL, &BmpSet[db_no].bmp_f, 0, 0, BmpSet[db_no].bmp_f.sx, BmpSet[db_no].bmp_f.sy,
										  BMP_FULL, &bmp_set.bmp_f, 0, 0, bmp_set.bmp_f.sx, bmp_set.bmp_f.sy, 0,(RECT*)NULL, 0, 0, 0, 0,
										  r, g, b, CHK_NO, (RECT*)-1, DRW_NML, 0,0, 0 );
				DRW_DrawBMP_FF( dobj );
				BMP_SetTonecurve_F( &BmpSet[db_no].bmp_f, tfname, viv );
				break;
			case 3:
				BMP_CreateBMP_T( &BmpSet[db_no].bmp_t, bmp_set.bmp_t.sx, bmp_set.bmp_t.sy );
				dobj = DRW_SetDrawObject( BMP_TRUE, &BmpSet[db_no].bmp_t, 0, 0, bmp_set.bmp_t.sx, bmp_set.bmp_t.sy,
										  BMP_TRUE, &bmp_set.bmp_t,     0, 0, bmp_set.bmp_t.sx, bmp_set.bmp_t.sy, 0,(RECT*)NULL, 0, 0, 0, 0,
										  r, g, b, CHK_NO, (RECT*)-1, DRW_NML, 0,0, 0 );
				DRW_DrawBMP_TT(  dobj );
				BMP_SetTonecurve_T( &BmpSet[db_no].bmp_t, tfname, viv );
				break;
		}
	}
}
void DSP_CopyBmp3( int db_no, BMP_SET bmp_set )
{
	DRAW_OBJECT	dobj;
	int	r=128, g=128, b=128;

	BmpSet[db_no].flag     = 1;
	BmpSet[db_no].bmp_bit  = bmp_set.bmp_bit;
	BmpSet[db_no].alp_flag = bmp_set.alp_flag;
	BmpSet[db_no].pos      = bmp_set.pos;
	BmpSet[db_no].size     = bmp_set.size;
	switch( bmp_set.bmp_bit ){
		case 0:

			break;
		case 1:
			BMP_CreateBMP_H( &BmpSet[db_no].bmp_h, bmp_set.bmp_h.sx, bmp_set.bmp_h.sy, 0 );
			dobj = DRW_SetDrawObject( BMP_HIGH, &BmpSet[db_no].bmp_h, 0, 0, bmp_set.bmp_h.sx, bmp_set.bmp_h.sy,
									  BMP_HIGH, &bmp_set.bmp_h,     0, 0, bmp_set.bmp_h.sx, bmp_set.bmp_h.sy, 0,NULL, 0, 0, 0, 0,
									  r, g, b, CHK_NO, (RECT*)-1, DRW_NML, 0,0, 0 );
			DRW_DrawBMP_HH(  dobj );
			break;
		case 2:
			BMP_CreateBMP_F( &BmpSet[db_no].bmp_f, bmp_set.bmp_f.sx, bmp_set.bmp_f.sy );
			dobj = DRW_SetDrawObject( BMP_FULL, &BmpSet[db_no].bmp_f, 0, 0, BmpSet[db_no].bmp_f.sx, BmpSet[db_no].bmp_f.sy,
									  BMP_FULL, &bmp_set.bmp_f, 0, 0, bmp_set.bmp_f.sx, bmp_set.bmp_f.sy, 0,(RECT*)NULL, 0, 0, 0, 0,
									  r, g, b, CHK_NO, (RECT*)-1, DRW_NML, 0,0, 0 );
			DRW_DrawBMP_FF( dobj );
			break;
		case 3:
			BMP_CreateBMP_T( &BmpSet[db_no].bmp_t, bmp_set.bmp_t.sx, bmp_set.bmp_t.sy );
			dobj = DRW_SetDrawObject( BMP_TRUE, &BmpSet[db_no].bmp_t, 0, 0, bmp_set.bmp_t.sx, bmp_set.bmp_t.sy,
									  BMP_TRUE, &bmp_set.bmp_t,     0, 0, bmp_set.bmp_t.sx, bmp_set.bmp_t.sy, 0,(RECT*)NULL, 0, 0, 0, 0,
									  r, g, b, CHK_NO, (RECT*)-1, DRW_NML, 0,0, 0 );
			DRW_DrawBMP_TT(  dobj );
			break;
	}
}



BOOL DSP_SetTonecurve( int bno, char *tfname, int viv )
{
	switch( BmpSet[bno].bmp_bit ){
		case 0:	BMP_SetTonecurve_BT(&BmpSet[bno].bmp_b, tfname, viv );	break;
		case 1: BMP_SetTonecurve_H( &BmpSet[bno].bmp_h, tfname, viv );	break;
		case 2: BMP_SetTonecurve_F( &BmpSet[bno].bmp_f, tfname, viv );	break;
		case 3: BMP_SetTonecurve_T( &BmpSet[bno].bmp_t, tfname, viv );	break;
	}
	return TRUE;
}


int DSP_GetBmpFlag( int bno )
{
	return BmpSet[bno].flag;
}
int DSP_GetBmpBit( int bno )
{
	return BmpSet[bno].bmp_bit;
}




int DSP_LoadBmpAnime( int bno, int bmp_bit, char *fname )
{
	char	fname2[256];
	int		w,h, mh;

	if(FILE_SPOIT){
		char	src[MAX_PATH],dst[MAX_PATH];
		wsprintf( dst, "mov\\buf\\%s", fname );
		wsprintf( src, "mov\\%s", fname );
		CopyFile( src, dst, TRUE);
	}
	wsprintf( fname2, "mov\\%s", fname );
	mh = PAC_PackFileMovie_Open( fname2 );

	if(mh==-1){
		DebugPrintf( "DSP_LoadBmpAnime ハンドルオーバーフロー\nムービーを開きすぎたり閉じ忘れたりしてませんか？　[最大３２個]\n" );
		return -1;
	}
	if(mh==-2){
		DebugBox(NULL,"動画ファイルオープン失敗[%s]",fname2);
		return -1;
	}
	PAC_PackFileMovie_GetSize( mh, &w, &h );
	MoveControl[mh].flag   = 1;
	MoveControl[mh].bmp_no = bno;
	MoveControl[mh].sx     = w;
	MoveControl[mh].sy     = h;
	MoveControl[mh].bmp_bit= bmp_bit;

	if(bno==-1){
		if(w!=DISP_X){
			DebugPrintf( "画面サイズと直展開ムービーの横サイズが合いません、画像が破壊されます\n" );
			return -1;
		}
	}else{
		DSP_CreateBmp( bno, bmp_bit, w, h );
	}
	return mh;
}



void DSP_ReleaseBmpAnime( int mh )
{
	if(mh>=0){
		if( MoveControl[mh].flag ){
			PAC_PackFileMovie_Close( mh );
			DSP_ReleaseBmp( MoveControl[mh].bmp_no );

			ZeroMemory( &MoveControl[mh], sizeof(MOVE_CONTOROL) );
		}
	}
}



int DSP_GetBmpAnimeCondition( int mh )
{
	return PAC_PackFileMovie_GetCondition( mh );
}



int DSP_GetBmpAnimeLoop( int mh )
{
	return PAC_PackFileMovie_GetLoop( mh );
}




void DSP_GetBmpAnimeSize( int mh, int *sx, int *sy )
{
	*sx = MoveControl[mh].sx;
	*sy = MoveControl[mh].sy;
}



BOOL DSP_GetBmpAnimeSabun( int mh )
{
	return PAC_PackFileMovie_GetSabun( mh );
}



BOOL DSP_PlayBmpAnime( int mh, int initial, int loop )
{
	if(initial)	return PAC_PackFileMovie_Start( mh, MOVE_COND_PLAY, loop );
	else		return PAC_PackFileMovie_Play( mh, loop );
}



BOOL DSP_StopBmpAnime( int mh, int initial )
{
	if(initial)	return PAC_PackFileMovie_Start( mh, MOVE_COND_STOP, 0 );
	else		return PAC_PackFileMovie_Pause( mh );
}



BOOL DSP_SetBmpAnimeLoop( int mh, int loop )
{
	return PAC_PackFileMovie_Loop( mh, loop );
}



BOOL DSP_SetBmpAnimeBright( int mh, int r, int g, int b )
{
	PAC_PackFileMovie_Bright( mh, r, g, b );
	return TRUE;
}



BOOL DSP_SetBmpAnimeBmpNo( int mh, int bno )
{
	if(MoveControl[mh].bmp_no!=-1){
		DSP_ReleaseBmp( MoveControl[mh].bmp_no );
	}
	MoveControl[mh].bmp_no = bno;
	if(bno==-1){
		if(MoveControl[mh].sx!=DISP_X){
			DebugPrintf( "画面サイズと直展開ムービーの横サイズが合いません、画像が破壊されます\n" );
		}
	}else{
		DSP_CreateBmp( bno, MoveControl[mh].bmp_bit, MoveControl[mh].sx, MoveControl[mh].sy );
	}
	return TRUE;
}




BOOL DSP_LoadSpriteCash( char *fname, char *tfname,int viv, int nuki )
{








	return 0;
}




BOOL DSP_LoadSprite( int ano, char *fname, int nuki, char *tfname,int viv )
{
	BOOL	ret;
	char	attr[256];
	STD_strlower( attr, STD_GetAttr(fname) );

	if(!strcmp("bmp",attr))	ret=SPR_LoadBmpSprite( ano, fname, nuki, tfname, viv );
	else					ret=SPR_LoadSprite( ano, fname, tfname, viv );
	return ret;
}
BOOL DSP_CopySprite( int ano1, int ano2, char *tfname, int viv, int r, int g, int b )
{
	return SPR_CopySprite( ano1, ano2, tfname, viv, r, g, b );
}



void DSP_ReleaseSprite( int sno )
{
	SPR_ReleaseSprite( sno );
}



void DSP_ResetSpriteCash( void )
{

}





BOOL DSP_SetSpritePlay( int gno, int ano, int sno, int end, int lnum, int cfade )
{
	return SPR_SetSpritePlay( GrpStruct[gno].ac_no-1, ano, sno, SP_PLAY, end, lnum, cfade );
}



BOOL DSP_SetSpriteNo( int gno, int sno )
{
	return SPR_SetSpriteNo( GrpStruct[gno].ac_no-1, sno );
}



BOOL DSP_SetSpriteCfade( int gno, int flag )
{
	return SPR_SetSpriteCfade( GrpStruct[gno].ac_no-1, flag );
}




BOOL DSP_SetSpriteLoop( int gno, int end, int lnum )
{
	return SPR_SetSpriteLoop( GrpStruct[gno].ac_no-1, end, lnum );
}



BOOL DSP_ThroughSpriteLoop( int gno )
{
	return SPR_SetSpriteLoopThrough( GrpStruct[gno].ac_no-1 );
}



BOOL DSP_SetSpriteRepeatFind( int gno )
{
	return SPR_SetSpriteRepeatFind( GrpStruct[gno].ac_no-1 );
}



int DSP_GetSpriteRepeatFind( int gno )
{
	return SPR_GetSpriteRepeatFind( GrpStruct[gno].ac_no-1 );
}



RECT *DSP_GetSpriteRect( int gno )
{
	if(GrpStruct[gno].flag)
		return SPR_GetSpriteRect( GrpStruct[gno].ac_no-1 );
	else{
		return NULL;
	}
}




int DSP_WaitSpriteCondition( int gno )
{
	return SPR_WaitSpriteCondition( GrpStruct[gno].ac_no-1 );
}



int DSP_GetSpriteEvent( int gno )
{
	return SPR_GetSpriteEvent( GrpStruct[gno].ac_no-1 );
}




void DSP_SetSprite( int gno,  int ano, int sno, int lno, int disp, int end, int lnum, int cfade )
{
	DSP_ResetGraph( gno );

	GrpStruct[gno].flag = 1;	
	GrpStruct[gno].disp = disp;	
	GrpStruct[gno].layer= lno;	
	GrpStruct[gno].type = PRM_SPR;	
	GrpStruct[gno].poly = 0;	

	GrpStruct[gno].bno  = ano;	

	GrpStruct[gno].r    = BRT_NML;	
	GrpStruct[gno].g    = BRT_NML;
	GrpStruct[gno].b    = BRT_NML;
	GrpStruct[gno].brt_flag = 0;	
	GrpStruct[gno].param= DRW_NML;	

	GrpStruct[gno].dx   = 0;	
	GrpStruct[gno].dy   = 0;

	GrpStruct[gno].clip = NULL;
	
	GrpStruct[gno].ac_no = SPR_SetSprite( ano, sno, SP_PLAY, end, lnum, cfade )+1;
}



void DSP_SetGraph( int gno, int bno, int lno,int disp,int nuki )
{
	DSP_ResetGraph( gno );

	GrpStruct[gno].flag = 1;	
	GrpStruct[gno].disp = disp;	
	GrpStruct[gno].layer= lno;	
	GrpStruct[gno].type = PRM_BMP;	
	GrpStruct[gno].poly = 0;	

	GrpStruct[gno].bno  = bno;	
	GrpStruct[gno].r    = BRT_NML;	
	GrpStruct[gno].g    = BRT_NML;
	GrpStruct[gno].b    = BRT_NML;
	GrpStruct[gno].brt_flag = 0;	
	GrpStruct[gno].param= DRW_NML;	
	GrpStruct[gno].nuki = nuki;

	GrpStruct[gno].dx   = 0;	
	GrpStruct[gno].dy   = 0;
	GrpStruct[gno].sx   = 0;	
	GrpStruct[gno].sy   = 0;
	GrpStruct[gno].target = 0;

	GrpStruct[gno].dw = GrpStruct[gno].sw = (short)BmpSet[bno].size.x;
	GrpStruct[gno].dh = GrpStruct[gno].sh = (short)BmpSet[bno].size.y;

	GrpStruct[gno].clip = NULL;
}



void DSP_SetGraphPrim( int gno, int type, int poly, int lno, int disp )
{
	GrpStruct[gno].flag = 1;	
	GrpStruct[gno].disp = disp;	
	GrpStruct[gno].layer= lno;	
	GrpStruct[gno].type = type;	
	GrpStruct[gno].poly = poly;	

	GrpStruct[gno].r    = BRT_NML;	
	GrpStruct[gno].g    = BRT_NML;
	GrpStruct[gno].b    = BRT_NML;
	GrpStruct[gno].brt_flag = 0;	
	GrpStruct[gno].param    = DRW_NML;	
	if(poly==POL_BOX){
		GrpStruct[gno].nuki = 1;	
	}

	GrpStruct[gno].dx = 0;	
	GrpStruct[gno].dy = 0;
	GrpStruct[gno].dw = GrpStruct[gno].sw = 0;
	GrpStruct[gno].dh = GrpStruct[gno].sh = 0;

	GrpStruct[gno].target = 0;

	GrpStruct[gno].clip = NULL;
}



void DSP_SetDigit( int gno, int bno, int lno,int disp,int nuki, int digit, int keta )
{
	DSP_ResetGraph( gno );

	GrpStruct[gno].flag = 1;	
	GrpStruct[gno].disp = disp;	
	GrpStruct[gno].layer= lno;	
	GrpStruct[gno].type = PRM_DGT;	
	GrpStruct[gno].poly = 0;	

	GrpStruct[gno].bno  = bno;	
	GrpStruct[gno].r    = BRT_NML;	
	GrpStruct[gno].g    = BRT_NML;
	GrpStruct[gno].b    = BRT_NML;
	GrpStruct[gno].brt_flag = 0;	
	GrpStruct[gno].param = DRW_NML;	
	GrpStruct[gno].nuki = nuki;

	GrpStruct[gno].dx2 = digit;
	GrpStruct[gno].dy2 = keta;

	GrpStruct[gno].dx   = 0;	
	GrpStruct[gno].dy   = 0;
	GrpStruct[gno].sx   = 0;	
	GrpStruct[gno].sy   = 0;
	GrpStruct[gno].target = 0;

	GrpStruct[gno].dw = GrpStruct[gno].sw = (short)BmpSet[bno].size.x;
	GrpStruct[gno].dh = GrpStruct[gno].sh = (short)BmpSet[bno].size.y;

	GrpStruct[gno].clip = NULL;
}
void DSP_SetDigitNum( int gno, int digit )
{
	GrpStruct[gno].dx2 = digit;
}
void DSP_SetGraphStr( int gno, int bno, int lno,int disp,int nuki, char *str )
{
	DSP_ResetGraph( gno );

	GrpStruct[gno].flag = 1;	
	GrpStruct[gno].disp = disp;	
	GrpStruct[gno].layer= lno;	
	GrpStruct[gno].type = PRM_STR;	
	GrpStruct[gno].poly = 0;	

	GrpStruct[gno].bno  = bno;	
	GrpStruct[gno].r    = BRT_NML;	
	GrpStruct[gno].g    = BRT_NML;
	GrpStruct[gno].b    = BRT_NML;
	GrpStruct[gno].brt_flag = 0;	
	GrpStruct[gno].param = DRW_NML;	
	GrpStruct[gno].nuki = nuki;

	GrpStruct[gno].dx   = 0;	
	GrpStruct[gno].dy   = 0;
	GrpStruct[gno].sx   = 0;	
	GrpStruct[gno].sy   = 0;
	GrpStruct[gno].target = 0;

	GrpStruct[gno].dw = (short)BmpSet[bno].size.x/16;
	GrpStruct[gno].sw = (short)BmpSet[bno].size.x;
	GrpStruct[gno].dh = GrpStruct[gno].sh = (short)BmpSet[bno].size.y/4;

	GrpStruct[gno].clip = NULL;

	GrpStruct[gno].str = (BYTE*)GAlloc( strlen(str)+1 );
	strcpy( (char*)GrpStruct[gno].str, str );
}
void DSP_SetGraphStrDirect( int gno, char *str )
{
	GFree( GrpStruct[gno].str );
	GrpStruct[gno].str = (BYTE*)GAlloc( strlen(str)+1 );
	strcpy( (char*)GrpStruct[gno].str, str );
}



void DSP_ResetGraph( int gno )
{
	if( GrpStruct[gno].ac_no )
		SPR_ResetSprite( GrpStruct[gno].ac_no-1 );
	GFree( GrpStruct[gno].str );

	ZeroMemory( &GrpStruct[gno], sizeof(GRP_STRUCT) );
}



void DSP_SetGraphBSet( int gno, int bno2, int blnd, int vague )
{
	int	sx,sy;
	GrpStruct[gno].bset  = 1;			
	GrpStruct[gno].bno2  = bno2;		
	GrpStruct[gno].param2= DRW_BLD(blnd);
	GrpStruct[gno].sx2   = 0;
	GrpStruct[gno].sy2   = 0;
	DSP_GetBmpSize( bno2, &sx, &sy );
	GrpStruct[gno].sw2   = sx;
	GrpStruct[gno].sh2   = sy;

	DSP_SetGraphPtarnFadeRate( gno, vague );
}



void DSP_ResetGraphBSet( int gno )
{
	GrpStruct[gno].bset = 0;		
	GrpStruct[gno].bno2 = 0;		
	GrpStruct[gno].bno3 = 0;		
	GrpStruct[gno].param2= DRW_NML;
}
void DSP_SetGraphBSet2( int gno, int bno2, int bno3, int blnd )
{
	int	sx,sy;
	GrpStruct[gno].bset  = 2;			
	GrpStruct[gno].bno2  = bno2;		
	GrpStruct[gno].bno3  = bno3;		
	GrpStruct[gno].param2= DRW_BLD(blnd);
	GrpStruct[gno].sx2   = 0;
	GrpStruct[gno].sy2   = 0;
	DSP_GetBmpSize( bno2, &sx, &sy );
	GrpStruct[gno].sw2   = sx;
	GrpStruct[gno].sh2   = sy;
}
void DSP_SetGraphPtarnFadeRate( int gno, int param3 )
{
	GrpStruct[gno].param3 = param3;
}




void DSP_SetBmpParam( int bno, void *buf, int w, int h, int bit, void *pal )
{
	DSP_ReleaseBmp(bno);

	BmpSet[bno].flag = 1;
	BmpSet[bno].pos.x=0;
	BmpSet[bno].pos.y=0;
	BmpSet[bno].size.x=w;
	BmpSet[bno].size.y=h;
	switch(bit){
		default:
		case 8:
			BmpSet[bno].bmp_bit   = BMP_256P;
			BmpSet[bno].bmp_b.buf = (char *)buf;
			BmpSet[bno].bmp_b.pal = (RGB32 *)pal;
			BmpSet[bno].bmp_b.sx  = w;
			BmpSet[bno].bmp_b.sy  = h;
			break;
		case 16:
			BmpSet[bno].bmp_bit   = BMP_HIGH;
			BmpSet[bno].bmp_h.buf = (unsigned short *)buf;
			BmpSet[bno].bmp_h.sx  = w;
			BmpSet[bno].bmp_h.sy  = h;
			break;
		case 24:
			BmpSet[bno].bmp_bit   = BMP_FULL;
			BmpSet[bno].bmp_f.buf = (RGB24 *)buf;
			BmpSet[bno].bmp_f.sx  = w;
			BmpSet[bno].bmp_f.sy  = h;
			break;
		case 32:
			BmpSet[bno].bmp_bit   = BMP_TRUE;
			BmpSet[bno].bmp_t.buf = (RGB32 *)buf;
			BmpSet[bno].bmp_t.sx  = w;
			BmpSet[bno].bmp_t.sy  = h;
			break;
	}
}




void *DSP_GetBmp( int bno )
{
	void *buf=NULL;
	switch(BmpSet[bno].bmp_bit){
		case 0:	buf = &BmpSet[bno].bmp_b;	break;
		case 1:	buf = &BmpSet[bno].bmp_h;	break;
		case 2:	buf = &BmpSet[bno].bmp_f;	break;
		case 3:	buf = &BmpSet[bno].bmp_t;	break;
	}
	return buf;
}



void *DSP_GetBmp2( BMP_SET *bs )
{
	void *buf=NULL;
	switch(bs->bmp_bit){
		case 0:	buf = &bs->bmp_b;	break;
		case 1:	buf = &bs->bmp_h;	break;
		case 2:	buf = &bs->bmp_f;	break;
		case 3:	buf = &bs->bmp_t;	break;
	}
	return buf;
}





void *DSP_GetGraphBmp( int gno )
{
	void *buf=NULL;
	if(GrpStruct[gno].type==0){
		switch( BmpSet[GrpStruct[gno].bno].bmp_bit ){
			case 0:	buf = &BmpSet[GrpStruct[gno].bno].bmp_b;	break;
			case 1:	buf = &BmpSet[GrpStruct[gno].bno].bmp_h;	break;
			case 2:	buf = &BmpSet[GrpStruct[gno].bno].bmp_f;	break;
			case 3:	buf = &BmpSet[GrpStruct[gno].bno].bmp_t;	break;
		}
	}
	return buf;
}
int	DSP_GetGraphBmpNo( int gno )
{
	return GrpStruct[gno].bno;
}



void DSP_GetGraphBmpSize( int gno, int *sx,int *sy )
{
	DSP_GetBmpSize( GrpStruct[gno].bno, sx, sy );
}



int DSP_GetGraphFlag( int gno )
{
	return GrpStruct[gno].flag;	
}



int DSP_GetGraphDisp( int gno )
{
	return GrpStruct[gno].disp;	
}



int DSP_GetGraphLayer( int gno )
{
	return GrpStruct[gno].layer;	
}



int DSP_GetGraphNuki( int gno )
{
	return GrpStruct[gno].nuki;
}



int DSP_GetGraphParam( int gno )
{
	return GrpStruct[gno].param;
}



int DSP_GetGraphParam2( int gno )
{
	return GrpStruct[gno].param2;
}



int DSP_GetGraphType( int gno )
{
	return GrpStruct[gno].type;
}



int DSP_GetGraphBrightFlag( int gno )
{
	return GrpStruct[gno].brt_flag;
}



void DSP_GetGraphGlobalBright( int *r,int *g,int *b )
{
	*r = BrightR;
	*g = BrightG;
	*b = BrightB;
}



void DSP_GetGraphBright( int gno, int *r,int *g,int *b )
{
	*r = GrpStruct[gno].r;
	*g = GrpStruct[gno].g;
	*b = GrpStruct[gno].b;
}



void DSP_GetGraphMove( int gno, int *dx, int *dy )
{
	*dx = GrpStruct[gno].dx;
	*dy = GrpStruct[gno].dy;
}






void DSP_SetGraphBmp( int gno, int bno )
{

	GrpStruct[gno].type = 0;	
	GrpStruct[gno].bno  = bno;					
}



void DSP_SetGraphTarget( int gno, int target, int draw_mode )
{
	void	*dest_bmp;

	if( GrpStruct[gno].flag ){
		switch( draw_mode ){
			case DISP_256:	dest_bmp = &BmpSet[target].bmp_b;	break;
			case DISP_HIGH:	dest_bmp = &BmpSet[target].bmp_h;	break;
			case DISP_FULL:	dest_bmp = &BmpSet[target].bmp_f;	break;
			case DISP_TRUE:	dest_bmp = &BmpSet[target].bmp_t;	break;
		}
		switch( GrpStruct[gno].type) {
			case PRM_BMP:	DrawGraphBmp( dest_bmp, 0, 0, &GrpStruct[gno], draw_mode, 0 );		break;
			case PRM_SPR:	DrawGraphSprite( dest_bmp, 0, 0, &GrpStruct[gno], draw_mode );	break;
			case PRM_DGT:	DrawGraphDigit( dest_bmp, 0, 0, &GrpStruct[gno], draw_mode );	break;
			case PRM_STR:	DrawGraphStr( dest_bmp, 0, 0, &GrpStruct[gno], draw_mode );	break;
			default:DrawGraphPrim( dest_bmp, 0, 0, &GrpStruct[gno], draw_mode );		break;
		}

		GrpStruct[gno].target = GrpStruct[gno].disp;
		GrpStruct[gno].disp   = OFF;
	}
}



void DSP_SetGraphDisp( int gno, int disp )
{
	if(GrpStruct[gno].flag){
		GrpStruct[gno].disp   = disp;	
		GrpStruct[gno].target = OFF;
		if(GrpStruct[gno].type==1){

		}
	}
}



void DSP_SetGraphLayer( int gno, int lno )
{
	GrpStruct[gno].layer= lno;	
}



void DSP_SetGraphNuki( int gno, int nuki )
{
	GrpStruct[gno].nuki= nuki;	
}



void DSP_SetGraphParam( int gno, int param )
{
	GrpStruct[gno].param    = param;
}



void DSP_SetGraphParam2( int gno, int param )
{
	GrpStruct[gno].param2 = param;
}



void DSP_SetGraphType( int gno, int type )
{
	GrpStruct[gno].type = type;
}



void DSP_SetGraphBrightFlag( int gno, int brt_flag )
{
	GrpStruct[gno].brt_flag = brt_flag;
}



void DSP_SetGraphGlobalBright( int r,int g,int b )
{
	BrightR = r;
	BrightG = g;
	BrightB = b;
}



void DSP_SetGraphBright( int gno, int r, int g, int b )
{
	DSP_SetGraphBrightPoint( gno, 0, r, g, b );
}
void DSP_SetGraphFade( int gno, int br )
{
	DSP_SetGraphBrightPoint( gno, 0, br, br, br );
}




void DSP_SetGraphBoxPich( int gno, int pich )
{
	GrpStruct[gno].nuki = pich;
}




void DSP_SetGraphBrightPoint( int gno, int point, int r,int g,int b )
{
	r=LIM(r,0,255);
	g=LIM(g,0,255);
	b=LIM(b,0,255);

	switch(point){
		default:
		case 0:
			GrpStruct[gno].r  = (char)r;	
			GrpStruct[gno].g  = (char)g;
			GrpStruct[gno].b  = (char)b;
			break;
		case 1:
			GrpStruct[gno].r2 = (char)r;	
			GrpStruct[gno].g2 = (char)g;
			GrpStruct[gno].b2 = (char)b;
			break;
		case 2:
			GrpStruct[gno].r3 = (char)r;	
			GrpStruct[gno].g3 = (char)g;
			GrpStruct[gno].b3 = (char)b;
			break;
		case 3:
			GrpStruct[gno].r4 = (char)r;	
			GrpStruct[gno].g4 = (char)g;
			GrpStruct[gno].b4 = (char)b;
			break;
	}
}



void DSP_SetGraphBrightYGra( int gno, int point, int y, int r,int g,int b )
{
	GrpStruct[gno].type = PRM_YGRA;
	switch(point){
		default:
		case 0:
			GrpStruct[gno].sy = y;
			GrpStruct[gno].r  = (char)r;	
			GrpStruct[gno].g  = (char)g;
			GrpStruct[gno].b  = (char)b;
			break;
		case 1:
			GrpStruct[gno].sy2 = y;
			GrpStruct[gno].r2  = (char)r;	
			GrpStruct[gno].g2  = (char)g;
			GrpStruct[gno].b2  = (char)b;
			break;
	}
}



void DSP_SetGraphGlobalPos( int x,int y )
{
	GlobalX = x;
	GlobalY = y;
}



void DSP_SetGraphMoveX( int gno, int dx )
{
	GrpStruct[gno].dx   = dx;
}
void DSP_SetGraphMoveY( int gno, int dy )
{
	GrpStruct[gno].dy   = dy;
}



void DSP_SetGraphMove( int gno, int dx, int dy )
{
	GrpStruct[gno].dx   = dx;	
	GrpStruct[gno].dy   = dy;
}
void DSP_SetGraphMove2( int gno, int dx, int dy )
{
	GrpStruct[gno].dx   = dx;	
	GrpStruct[gno].dy   = dy;

	GrpStruct[gno].sx   = 0;	
	GrpStruct[gno].sy   = 0;

	GrpStruct[gno].dw   = GrpStruct[gno].sw   = (short)BmpSet[GrpStruct[gno].bno].size.x;
	GrpStruct[gno].dh   = GrpStruct[gno].sh   = (short)BmpSet[GrpStruct[gno].bno].size.y;
}



void DSP_SetGraphWidth( int gno, int w, int h )
{
	GrpStruct[gno].dw   = w;	
	GrpStruct[gno].dh   = h;

	GrpStruct[gno].sw   = w;	
	GrpStruct[gno].sh   = h;
}




void DSP_SetGraphSMove( int gno, int sx, int sy )
{
	GrpStruct[gno].sx   = sx;
	GrpStruct[gno].sy   = sy;
}



void DSP_SetGraphSPos( int gno, int sx, int sy, int sw, int sh, int poly )
{
	GrpStruct[gno].sx   = sx;
	GrpStruct[gno].sy   = sy;
	if(poly!=-1) GrpStruct[gno].poly = poly;
	if(GrpStruct[gno].poly){
		GrpStruct[gno].sw = sw;
		GrpStruct[gno].sh = sh;
	}else{
		GrpStruct[gno].dw = GrpStruct[gno].sw = sw;
		GrpStruct[gno].dh = GrpStruct[gno].sh = sh;
	}
}



void DSP_SetGraphRevParam( int gno, DWORD rparam )
{
	GrpStruct[gno].rparam = rparam;	
}



void DSP_SetGraphPos( int gno, int dx, int dy, int sx, int sy, int w, int h )
{
	GrpStruct[gno].poly = 0;	
	
	GrpStruct[gno].dx   = dx;	
	GrpStruct[gno].dy   = dy;
	GrpStruct[gno].dw   = w;	
	GrpStruct[gno].dh   = h;

	GrpStruct[gno].sx   = sx;	
	GrpStruct[gno].sy   = sy;
	GrpStruct[gno].sw   = w;	
	GrpStruct[gno].sh   = h;
}



void DSP_SetGraphPosZoom( int gno,  int dx, int dy, int dw, int dh,
									int sx, int sy, int sw, int sh )
{
	GrpStruct[gno].poly = 1;	
	
	GrpStruct[gno].dx   = dx;	
	GrpStruct[gno].dy   = dy;
	GrpStruct[gno].dw   = dw;	
	GrpStruct[gno].dh   = dh;

	GrpStruct[gno].sx   = sx;	
	GrpStruct[gno].sy   = sy;
	GrpStruct[gno].sw   = sw;	
	GrpStruct[gno].sh   = sh;

	if(dw==sw && dh==sh ){
		GrpStruct[gno].poly = 0;
	}
}



void DSP_SetGraphZoom( int gno, int dx, int dy, int dw, int dh )
{
	GrpStruct[gno].poly = 1;	
	
	GrpStruct[gno].dx   = dx;	
	GrpStruct[gno].dy   = dy;
	GrpStruct[gno].dw   = dw;	
	GrpStruct[gno].dh   = dh;

	GrpStruct[gno].sx   = 0;	
	GrpStruct[gno].sy   = 0;

	GrpStruct[gno].sw   = (short)BmpSet[GrpStruct[gno].bno].size.x;
	GrpStruct[gno].sh   = (short)BmpSet[GrpStruct[gno].bno].size.y;

	if(GrpStruct[gno].dw==GrpStruct[gno].sw&&GrpStruct[gno].dh==GrpStruct[gno].sh){
		GrpStruct[gno].poly = 0;
	}
}
void DSP_SetGraphZoomBai( int gno, int dx, int dy, int nuki, int dw, int sw )
{
	GrpStruct[gno].poly = 1;	
	GrpStruct[gno].nuki = nuki;	
	
	GrpStruct[gno].dx   = dx;	
	GrpStruct[gno].dy   = dy;

	GrpStruct[gno].sx   = 0;	
	GrpStruct[gno].sy   = 0;

	GrpStruct[gno].sw   = (short)BmpSet[GrpStruct[gno].bno].size.x;
	GrpStruct[gno].sh   = (short)BmpSet[GrpStruct[gno].bno].size.y;

	GrpStruct[gno].dw   = GrpStruct[gno].sw*dw/sw;	
	GrpStruct[gno].dh   = GrpStruct[gno].sh*dw/sw;
}





void DSP_SetGraphZoom2( int gno, int cx, int cy, int zoom )
{
	if(zoom<-256){
		DebugPrintf( "０％以下にズームできません" );
		GrpStruct[gno].poly = 1;	
		GrpStruct[gno].zoom=-256;
		GrpStruct[gno].cx   = cx;
		GrpStruct[gno].cy   = cy;
	}else if(zoom){
		GrpStruct[gno].poly = 1;	
		GrpStruct[gno].zoom = zoom;
		GrpStruct[gno].cx   = cx;
		GrpStruct[gno].cy   = cy;
	}else{
		GrpStruct[gno].poly = 0;	
		GrpStruct[gno].zoom = 0;
	}
}



void DSP_SetGraphZoom3( int gno, int cx, int cy, int zoom )
{
	if(zoom<0){
		DebugPrintf( "０％以下にズームできません" );
	}else if(zoom!=100){
		GrpStruct[gno].poly = 1;	
		GrpStruct[gno].zoom = zoom*256/100-256;
		GrpStruct[gno].cx   = cx;
		GrpStruct[gno].cy   = cy;
	}else{
		GrpStruct[gno].poly = 0;	
		GrpStruct[gno].zoom = 0;
	}
}

void DSP_SetGraphRoll( int gno, int cx, int cy, int zoom, int rate, int sx, int sy, int sw, int sh )
{
	int	sw2, sh2;
	sw2 = sw*(zoom+256)/256/2;
	sh2 = sh*(zoom+256)/256/2;

	DSP_SetGraphPosPoly( gno,
		cx+( COS(rate)*sh2-SIN(rate)*sw2)/4096,   cy+(-COS(rate)*sw2-SIN(rate)*sh2)/4096,
		cx+( COS(rate)*sh2+SIN(rate)*sw2)/4096-1, cy+( COS(rate)*sw2-SIN(rate)*sh2)/4096,
		cx+(-COS(rate)*sh2-SIN(rate)*sw2)/4096,   cy+(-COS(rate)*sw2+SIN(rate)*sh2)/4096-1,
		cx+(-COS(rate)*sh2+SIN(rate)*sw2)/4096-1, cy+( COS(rate)*sw2+SIN(rate)*sh2)/4096-1,
		sx, sy,  sx+sw-1, sy,  sx, sy+sh-1, sx+sw-1, sy+sh-1 );
}




void DSP_SetGraphPosPoly( int gno,  int dx1, int dy1, int dx2, int dy2, int dx3, int dy3, int dx4, int dy4,
									int sx1, int sy1, int sx2, int sy2, int sx3, int sy3, int sx4, int sy4 )
{
	int	sx,sy;
	GrpStruct[gno].poly = 2;	
	
	GrpStruct[gno].dx   = dx1;	
	GrpStruct[gno].dy   = dy1;
	GrpStruct[gno].dx2  = dx2;	
	GrpStruct[gno].dy2  = dy2;
	GrpStruct[gno].dx3  = dx3;	
	GrpStruct[gno].dy3  = dy3;
	GrpStruct[gno].dx4  = dx4;	
	GrpStruct[gno].dy4  = dy4;

	sx = BmpSet[GrpStruct[gno].bno].size.x;
	sy = BmpSet[GrpStruct[gno].bno].size.y;

	GrpStruct[gno].sx   = LIM(sx1,0,sx);	
	GrpStruct[gno].sy   = LIM(sy1,0,sy);
	GrpStruct[gno].sx2  = LIM(sx2,0,sx);	
	GrpStruct[gno].sy2  = LIM(sy2,0,sy);
	GrpStruct[gno].sx3  = LIM(sx3,0,sx);	
	GrpStruct[gno].sy3  = LIM(sy3,0,sy);
	GrpStruct[gno].sx4  = LIM(sx4,0,sx);	
	GrpStruct[gno].sy4  = LIM(sy4,0,sy);
}



void DSP_SetGraphPosRect( int gno, int dx, int dy, int w, int h )
{

	GrpStruct[gno].dx = dx;	
	GrpStruct[gno].dy = dy;
	GrpStruct[gno].dw = w;
	GrpStruct[gno].dh = h;
}



void DSP_SetGraphPosPoint( int gno, int point, int dx, int dy )
{
	switch(point){
		default:
		case 0:
			GrpStruct[gno].dx = dx;		
			GrpStruct[gno].dy = dy;
			break;
		case 1:
			GrpStruct[gno].dx2 = dx;	
			GrpStruct[gno].dy2 = dy;
			break;
		case 2:
			GrpStruct[gno].dx3 = dx;	
			GrpStruct[gno].dy3 = dy;
			break;
		case 3:
			GrpStruct[gno].dx4 = dx;	
			GrpStruct[gno].dy4 = dy;
			break;
	}
}



void DSP_SetGraphClip( int gno, int dx, int dy, int w, int h )
{
	static RECT	clip[GRP_MAX];

	if( dx<0 ){
		GrpStruct[gno].clip = NULL;
	}else{
		clip[gno].left	=dx;
		clip[gno].top	=dy;
		clip[gno].right	=dx+w;
		clip[gno].bottom=dy+h;
		GrpStruct[gno].clip = &clip[gno];
	}
}
void DSP_SetGraphClipSprit( int gno, int sp_gno )
{
	GrpStruct[gno].sp_clip = sp_gno+1;
}




void DSP_SetText( int tno, int layer, int font, int disp, char *str, ... )
{
	TextStruct[tno].flag  = 1;			
	TextStruct[tno].disp  = disp;		
	TextStruct[tno].layer = layer;		
	TextStruct[tno].font  = font;		

	GFree(TextStruct[tno].str);
	TextStruct[tno].str = (char *)GAlloc( 1024 );
	wvsprintf( TextStruct[tno].str, str, (char *)(&str+1) );

	TextStruct[tno].color    = 0;		
	TextStruct[tno].brt_flag = 0;		
	TextStruct[tno].cnt      = -1;		
	TextStruct[tno].step     = -1;		
	TextStruct[tno].alph     = 256;		
	TextStruct[tno].kage     = 0;		

	TextStruct[tno].dx = 0;
	TextStruct[tno].dy = 0;
	TextStruct[tno].ws = 256;
	TextStruct[tno].hs = 256;

	TextStruct[tno].target = 0;

	TextStruct[tno].clip = NULL;
}



char *DSP_AddText( int tno, char *str )
{
	if(TextStruct[tno].str){
		strcat( TextStruct[tno].str, str );
	}else{
		return NULL;
	}
	return TextStruct[tno].str;
}



void DSP_ResetText( int tno )
{
	GFree(TextStruct[tno].str);
	ZeroMemory( &TextStruct[tno], sizeof(TEXT_STRUCT) );
}




int DSP_GetTextDisp( int tno )
{
	if(TextStruct[tno].flag){
		return TextStruct[tno].disp;		
	}else{
		return OFF;
	}
}



int DSP_GetTextLayer( int tno )
{
	return TextStruct[tno].layer;		
}



int DSP_GetTextBrightFlag( int tno )
{
	return TextStruct[tno].brt_flag;		
}



int DSP_GetTextColor( int tno )
{
	return TextStruct[tno].color;		
}



int DSP_GetTextAlph( int tno )
{
	return TextStruct[tno].alph;		
}



int DSP_GetTextKage( int tno )
{
	return TextStruct[tno].kage;
}



char *DSP_GetTextStr( int tno )
{
	return TextStruct[tno].str;
}



int DSP_GetTextCount( int tno )
{
	return TextStruct[tno].cnt;
}



void DSP_GetTextMove( int tno, int *dx, int *dy )
{
	*dx = TextStruct[tno].dx;
	*dy = TextStruct[tno].dy;
}



int DSP_GetTextDispH( int tno )
{
	TEXT_STRUCT *ts = &TextStruct[tno];
	int			dmy;
	return TXT_DrawText( NULL, 0, ts->dx, ts->dy, ts->ws, ts->hs, ts->pw, ts->ph, &dmy, &dmy, ts->clip,
								ts->font, ts->str, ts->color, -1, -1,128,128,128, ts->alph, 0 );
}



int DSP_GetTextDispStr( int tno, char *str )
{
	TXT_GetTextDispStr( TextStruct[tno].str, str );

	return TRUE;
}



void DSP_GetTextDispPos( int tno, int *px, int *py, int kaigyou_musi )
{
	DrawGraphText( NULL, 0, 0, &TextStruct[tno], 0, kaigyou_musi );
	*px = TextStruct[tno].px;
	*py = TextStruct[tno].py;
}



void DSP_SetTextTarget( int tno, int target, int draw_mode )
{
	void	*dest_bmp;
	switch( draw_mode ){
		case DISP_256:	dest_bmp = &BmpSet[target].bmp_b;	break;
		case DISP_HIGH:	dest_bmp = &BmpSet[target].bmp_h;	break;
		case DISP_FULL:	dest_bmp = &BmpSet[target].bmp_f;	break;
		case DISP_TRUE:	dest_bmp = &BmpSet[target].bmp_t;	break;
	}
	DrawGraphText( dest_bmp, 0, 0, &TextStruct[tno], draw_mode );

	TextStruct[tno].target = TextStruct[tno].disp;
	TextStruct[tno].disp   = OFF;
}



void DSP_SetTextDisp( int tno, int disp )
{
	TextStruct[tno].disp  = disp;		
}



void DSP_SetTextLayer( int tno, int lno )
{
	TextStruct[tno].layer = lno;		
}



void DSP_SetTextBrightFlag( int tno, int brt_flag )
{
	TextStruct[tno].brt_flag = brt_flag;		
}



void DSP_SetTextColor( int tno, int color )
{
	TextStruct[tno].color    = color;		
}



void DSP_SetTextAlph( int tno, int alph )
{
	TextStruct[tno].alph    = alph;		
}



void DSP_SetTextKage( int tno, int kage )
{
	TextStruct[tno].kage = kage;
}




void DSP_SetTextStr( int tno, char *str, ... )
{
	if( TextStruct[tno].str ){
		wvsprintf( TextStruct[tno].str, str, (char *)(&str+1) );

		TextStruct[tno].cnt = -1;
	}
}



void DSP_SetTextCount( int tno, int cnt )
{
	TextStruct[tno].cnt = cnt;
}



void DSP_SetTextStep( int tno, int step )
{
	TextStruct[tno].step = step;
}




void DSP_SetTextPos( int tno, int x, int y, int ws, int hs )
{
	TextStruct[tno].dx = x;
	TextStruct[tno].dy = y;
	TextStruct[tno].ws = ws;
	TextStruct[tno].hs = hs;
}



void DSP_SetTextMove( int tno, int x, int y )
{
	TextStruct[tno].dx = x;
	TextStruct[tno].dy = y;
}




void DSP_SetTextPich( int tno, int pw, int ph )
{
	TextStruct[tno].pw = pw;
	TextStruct[tno].ph = ph;
}



void DSP_SetTextClip( int tno, int dx, int dy, int w, int h )
{
	static RECT	clip[TXT_MAX];

	if( dx<0 ){
		TextStruct[tno].clip = NULL;
	}else{
		clip[tno].left	=dx;
		clip[tno].top	=dy;
		clip[tno].right	=dx+w;
		clip[tno].bottom=dy+h;
		TextStruct[tno].clip = &clip[tno];
	}
}



void DSP_SetDmoji( int dno, int disp, int type, int digit, char *str )
{
	DmojiStruct[dno].flag = 1;
	DmojiStruct[dno].disp = disp;
	DmojiStruct[dno].dx   = 0;
	DmojiStruct[dno].dy   = 0;
	DmojiStruct[dno].type = type;
	DmojiStruct[dno].digit= digit;
	if(str!=NULL)
		strcpy( DmojiStruct[dno].str, str );
	DmojiStruct[dno].param= DRW_NML;
}



void DSP_ResetDmoji( int dno )
{
	ZeroMemory( &DmojiStruct[dno], sizeof(DmojiStruct[dno]) );
}



void DSP_SetDmojiDisp( int dno, int disp )
{
	DmojiStruct[dno].disp = disp;
}



void DSP_SetDmojiPos( int dno, int dx, int dy )
{
	DmojiStruct[dno].dx = dx;
	DmojiStruct[dno].dy = dy;
}



void DSP_SetDmojiDigit( int dno, int digit )
{
	DmojiStruct[dno].type  = DMOJI_DIGIT;
	DmojiStruct[dno].digit = digit;
}




void DSP_SetDmojiStr( int dno, char *str )
{
	DmojiStruct[dno].type  = DMOJI_TEXT;
	if(str!=NULL)
		strcpy( DmojiStruct[dno].str, str );
}




extern void XVID_ResetAvi( int i );
extern int XVID_SetAviStart( char *fname, int *w, int *h, int bpp );
extern int XVID_WaitAviTexture( int i  );
extern int XVID_UpdateAviTexture( int i, void *dest, int draw_mode );
extern int XVID_CreateAviTexture( char *fname );

#include <xvid_dec.h>


#define XVID_MAX_AVI_AMOUNT		8

typedef struct{
	char	flag;
	long	w,h;
	FILE	*fpr;
	DWORD	fpms;
	DWORD	start_time;
	DWORD	frame_cnt;
	DWORD	skip_cnt;

	DWORD	buffer_size;
	DWORD	read_size;
	int		rest;
	DWORD	pitch;

	BYTE	*SrcBuf;
	BYTE	*DstBuf;

	XviDDec		Xvid;
}XVID_AVI;

XVID_AVI		XvidAvi[XVID_MAX_AVI_AMOUNT];

struct Chunk{
	FOURCC	ckID;
	DWORD	size;
	FOURCC	type;
};


#include <stdio.h>
#include <Aviriff.h>

void XVID_ResetAvi( int i )
{
	if(XvidAvi[i].flag){
		if(XvidAvi[i].fpr) fclose(XvidAvi[i].fpr);
		XvidAvi[i].Xvid.Clear_XviD();
		GFree( XvidAvi[i].SrcBuf );
		GFree( XvidAvi[i].DstBuf );
	}

	ZeroMemory( &XvidAvi[i], sizeof(XVID_AVI) );
}



int XVID_SetAviStart( char *fname, int *w, int *h, int bpp )
{
	int	i;
	
	fname = "mov\\toHeart2_OP_400x224_XVID._2M.avi";



	
	for( i=0 ; i <XVID_MAX_AVI_AMOUNT ; i++ ){
		if(!XvidAvi[i].flag){
			break;
		}
	}
	if(i==XVID_MAX_AVI_AMOUNT) return XVID_MAX_AVI_AMOUNT;
	XVID_ResetAvi(i);

	XvidAvi[i].fpr = fopen(fname,"rb");
	if(NULL==XvidAvi[i].fpr)return XVID_MAX_AVI_AMOUNT;

	Chunk		aviFileHead;
	Chunk		aviInfo;
	AVIMAINHEADER aviMainHead;

	fread(&aviFileHead,sizeof(Chunk),1,XvidAvi[i].fpr);	
	if(MAKEFOURCC('R','I','F','F')!=aviFileHead.ckID){
		fclose(XvidAvi[i].fpr);
		XvidAvi[i].fpr = NULL;
		return FALSE;
	}
	fread(&aviInfo,sizeof(Chunk),1,XvidAvi[i].fpr);
	fread(&aviMainHead,sizeof(AVIMAINHEADER),1,XvidAvi[i].fpr);


	XvidAvi[i].flag = TRUE;
	XvidAvi[i].fpms = aviMainHead.dwMicroSecPerFrame;
	XvidAvi[i].w = *w = aviMainHead.dwWidth;
	XvidAvi[i].h = *h = aviMainHead.dwHeight;

	XvidAvi[i].buffer_size = XvidAvi[i].w*XvidAvi[i].h*2;

	XvidAvi[i].SrcBuf = (BYTE*)GAlloc( XvidAvi[i].buffer_size );

	BOOL	dv_ret=0;
	switch(bpp){
		case 24:
			XvidAvi[i].pitch = 3;
			dv_ret=XvidAvi[i].Xvid.Start_XviD( XvidAvi[i].w, XvidAvi[i].h, XVID_CSP_BGR );
			break;
		case 32:
			XvidAvi[i].pitch = 3;
			dv_ret=XvidAvi[i].Xvid.Start_XviD( XvidAvi[i].w, XvidAvi[i].h, XVID_CSP_BGR );


			break;
		case 16:
			XvidAvi[i].pitch = 2;
			dv_ret=XvidAvi[i].Xvid.Start_XviD( XvidAvi[i].w, XvidAvi[i].h, XVID_CSP_RGB565 );
			break;
		case 15:
			XvidAvi[i].pitch = 2;
			dv_ret=XvidAvi[i].Xvid.Start_XviD( XvidAvi[i].w, XvidAvi[i].h, XVID_CSP_RGB555 );
			break;
	}
	if(!dv_ret){
		DebugBox( NULL, "XVIDの初期化に失敗しました" );
	}

	DSP_CreateBmp( 255, BMP_FULL, XvidAvi[i].w, XvidAvi[i].h );

	XvidAvi[i].read_size = fread( XvidAvi[i].SrcBuf, 1, XvidAvi[i].buffer_size, XvidAvi[i].fpr );
	XvidAvi[i].start_time = timeGetTime();
	XvidAvi[i].frame_cnt = 0;
	return i;
}

int XVID_WaitAviTexture( int i  )
{
	return XvidAvi[i].flag;
}

extern BOOL DRW_DrawZOOM2_FF_Std(	BMP_F *dest, int dx, int dy,
							BMP_F *src,  int sx, int sy, int sw, int sh,
							int nuki, int r, int g, int b );

extern BOOL DRW_DrawZOOMX_FF_Std(	BMP_F *dest, int dx, int dy, int dw, int dh,
							BMP_F *src,  int sx, int sy, int sw, int sh,
							int nuki, int r, int g, int b );

int XVID_UpdateAviTexture( int i, void *dest, int draw_mode )
{
	static int aaa=0;
	if(aaa==0){
		aaa=1;

	}
	int		skiped=FALSE;

	if(XvidAvi[i].flag){
		DWORD play_time = DWORD(XvidAvi[i].start_time+XvidAvi[i].frame_cnt*XvidAvi[i].fpms/1000);
		DWORD now_time = timeGetTime();

		if( play_time<=now_time ){
				XvidAvi[i].Xvid.SetSkip(FALSE);
				XvidAvi[i].skip_cnt=0;
				skiped = FALSE;


			XvidAvi[i].frame_cnt++;

			switch( XvidAvi[i].Xvid.DecodeXviD( XvidAvi[i].SrcBuf, XvidAvi[i].read_size, &XvidAvi[i].rest, (BYTE*)BmpSet[255].bmp_f.buf, XvidAvi[i].w*XvidAvi[i].pitch ) ){

				case XviD_DEC_ERR:		
					XVID_ResetAvi( i );
					break;
				case XviD_DEC_END:		
					XVID_ResetAvi( i );
					break;
				case XviD_DEC_CONTINUE:	
					i=i;
					break;
				case XviD_DEC_NEEDMORE:	
					XvidAvi[i].read_size = fread( XvidAvi[i].SrcBuf + XvidAvi[i].rest, 1, XvidAvi[i].buffer_size-XvidAvi[i].rest, XvidAvi[i].fpr );
					break;
			}

			if(XvidAvi[i].flag && skiped==FALSE){
				int		x,y;
				BMP_F	*bmp_f = (BMP_F*)dest;
				BMP_F	bmp_f2 = { (RGB24*)BmpSet[255].bmp_f.buf, XvidAvi[i].w, XvidAvi[i].h };


				switch( draw_mode ){
					case DISP_256:	break;
					case DISP_HIGH:	break;
					case DISP_FULL:



						DSP_SetGraph( 0, 255, 0,ON,-1 );
						DSP_SetGraphZoom( 0, 0, 76, 800, 448 );












						break;
					case DISP_TRUE:	break;
				}


			}
		}
	}
	return skiped;
}


int XVID_CreateAviTexture( char *fname )
{
	int	w,h;
	int	i = XVID_SetAviStart( fname, &w, &h, 24 );
	if(i==XVID_MAX_AVI_AMOUNT) return FALSE;

	return TRUE;
}




typedef struct{
	BOOL	flag;
	short	havi;
	short	bno;
	short	bit;
}AVI_INFO;

AVI_INFO	AviInfo[AVI_MAX];

static char	*PackMovDir=NULL;

void DSP_SetPackDirMov( char *pac_dir )
{
	PackMovDir = pac_dir;
}




int DSP_LoadAvi( int ano, int bno, int w, int h, int bit, char *fname )
{
	int		havi;
	char	fname2[1024];
	char	*buf;

	if(PackMovDir)	wsprintf( fname2, "%s\\%s", PackMovDir, fname );
	else			wsprintf( fname2, fname );

	havi = MOV_OpenFile( fname2, w, h, bit );
	if( havi==-1 ) return FALSE;
	MOV_Play( havi );
	AviInfo[ ano ].flag = 1;
	AviInfo[ ano ].havi = havi;
	AviInfo[ ano ].bno  = bno;
	AviInfo[ ano ].bit  = bit;

	MOV_GetVideoBuf( AviInfo[ano].havi, (void **)&buf, &w, &h, &bit );
	DSP_SetBmpParam( bno, buf, w, h, bit, NULL );
	return TRUE;
}



int DSP_ReleaseAvi( int ano )
{
	if(AviInfo[ ano ].flag){
		MOV_CloseFile( AviInfo[ ano ].havi );
		ZeroMemory( &BmpSet[AviInfo[ ano ].bno], sizeof(BMP_SET) );
	}
	ZeroMemory( &AviInfo[ ano ], sizeof(AVI_INFO) );
	return TRUE;
}



int DSP_GetAviParam( int ano )
{
	return MOV_GetMode( AviInfo[ ano ].havi );
}



int DSP_GetAviNowFrameAudio( int ano )
{
	return MOV_GetNowFrameAudio( AviInfo[ ano ].havi );
}



int DSP_GetAviNowFrameVideo( int ano )
{
	return MOV_GetNowFrameVideo( AviInfo[ ano ].havi );
}




int DSP_GetAviMaxFrameAudio( int ano )
{
	return MOV_GetMaxFrameAudio( AviInfo[ ano ].havi );
}



int DSP_GetAviMaxFrameVideo( int ano )
{
	return MOV_GetMaxFrameVideo( AviInfo[ ano ].havi );
}




DWORD DSP_GetAviWavVolume( int ano )
{

	return 0;
}



BOOL DSP_SetAviWavVolume( int ano, DWORD volume )
{

	return 0;
}





void DSP_RenewMovie( int draw_mode )
{
	int		i;
	void	*buf;
	int		w,h,bit;
	
	for( i=0 ; i<AVI_MAX ; i++ ){
		if( AviInfo[i].flag ){
			if( AviInfo[i].bit != 8+8*BmpSet[AviInfo[i].bno].bmp_bit ){
				AviInfo[i].bit = 8+8*BmpSet[AviInfo[i].bno].bmp_bit;
				MOV_SetVideoBufFormat( AviInfo[i].havi, 0, 0, AviInfo[i].bit, NULL );
			}
			if( MOV_RenewMovie(i) ){
				ZeroMemory( &BmpSet[ AviInfo[i].bno ], sizeof(BMP_SET) );
				MOV_GetVideoBuf( AviInfo[i].havi, &buf, &w, &h, &bit );
				DSP_SetBmpParam( AviInfo[i].bno, buf, w, h, bit, NULL );
			}
		}
	}
}





BOOL DrawGraphSprite( void *dest, int x, int y, GRP_STRUCT *gs, int draw_mode )
{
	char	r,g,b;
	int		dx  = gs->dx +x;
	int		dy  = gs->dy +y;
	RECT	clip;

	if(gs->clip){
		clip.left   = gs->clip->left  + x;
		clip.top    = gs->clip->top   - y;
		clip.right  = gs->clip->right + x;
		clip.bottom = gs->clip->bottom- y;
	}else{
		clip.left   = 0;
		clip.top    = 0;
		clip.right  = DISP_X;
		clip.bottom = DISP_Y;
	}

	if(!gs->brt_flag){
		if( BrightR<=BRT_NML)	r =       gs->r * BrightR          / BRT_NML;
		else					r = (0xff-gs->r)*(BrightR-BRT_NML) / BRT_NML + gs->r;
		if( BrightG<=BRT_NML)	g =       gs->g * BrightG          / BRT_NML;
		else					g = (0xff-gs->g)*(BrightG-BRT_NML) / BRT_NML + gs->g;
		if( BrightB<=BRT_NML)	b =       gs->b * BrightB          / BRT_NML;
		else					b = (0xff-gs->b)*(BrightB-BRT_NML) / BRT_NML + gs->b;
	}else{
		r = gs->r;
		g = gs->g;
		b = gs->b;
	}

	SPR_DrawSprite( dest, draw_mode, gs->ac_no-1, dx, dy, r,g,b, &clip, gs->param, gs->rparam, gs->zoom );
	return TRUE;
}




BOOL DrawGraphDigit( void *dest, int x, int y, GRP_STRUCT *gs, int draw_mode )
{
	if( gs->bno<0 || gs->bno>=BMP_MAX ) return FALSE;
	char r,g,b;
	int	dx = gs->dx + x;
	int	dy = gs->dy + y;
	int	dw = gs->dw;
	int	dh = gs->dh;
	int	sx = gs->sx - BmpSet[gs->bno].pos.x;
	int	sy = gs->sy - BmpSet[gs->bno].pos.y;

	RECT	clip;
	int		draw_bpp;
	DRAW_OBJECT		dobj;

	if(gs->clip){
		clip.left   = gs->clip->left  + x;
		clip.top    = gs->clip->top   + y;
		clip.right  = gs->clip->right + x;
		clip.bottom = gs->clip->bottom+ y;
	}else{
		clip.left   = 0;
		clip.top    = 0;
		clip.right  = DISP_X;
		clip.bottom = DISP_Y;
	}

	if(!gs->brt_flag){
		if( BrightR<=BRT_NML)	r =       gs->r * BrightR          / BRT_NML;
		else					r = (0xff-gs->r)*(BrightR-BRT_NML) / BRT_NML + gs->r;
		if( BrightG<=BRT_NML)	g =       gs->g * BrightG          / BRT_NML;
		else					g = (0xff-gs->g)*(BrightG-BRT_NML) / BRT_NML + gs->g;
		if( BrightB<=BRT_NML)	b =       gs->b * BrightB          / BRT_NML;
		else					b = (0xff-gs->b)*(BrightB-BRT_NML) / BRT_NML + gs->b;
	}else{
		r = gs->r;
		g = gs->g;
		b = gs->b;
	}

	dobj.mask = NULL;
	draw_bpp = BmpSet[gs->bno].bmp_bit;
	int	zero = zero = (gs->dy2<0)? 1: 0;
	int keta = abs(gs->dy2);
	int	digit = gs->dx2;
	int	i,j;

	i=0;
	j=digit;
	while(j){ j/=10; i++; }
	keta = max(1,max(i,keta));

	j=digit;
	for(i=0;i<keta;i++){
		if(zero || j || i==0){
			dobj = DRW_SetDrawObject( BPP(draw_mode), dest, dx+(dw/10)*(keta-i-1), dy, dw/10, dh,
									BmpSet[gs->bno].bmp_bit,  DSP_GetBmp(gs->bno),  sx+(gs->sw/10)*(j%10), sy,   gs->sw/10,  gs->sh,
									0, NULL, 0, 0, 0, 0,
									r, g, b, gs->nuki, &clip, gs->param, gs->param2, gs->param3, gs->rparam );
			switch( draw_mode ){
			case DISP_256:
				break;
			case DISP_HIGH:
				switch( draw_bpp ){
					default:DebugPrintf( "画像と画面モードが合いません。" );	break;
					case 0:	DRW_DrawBMP_HB(dobj);	break;	
					case 1:	DRW_DrawBMP_HH(dobj);	break;	
				}
				break;
			case DISP_FULL:
				switch( draw_bpp ){
					default:DebugPrintf( "画像と画面モードが合いません。" );	break;
					case 0:	DRW_DrawBMP_FB(dobj,0);	break;	
					case 2:	DRW_DrawBMP_FF(dobj,0);	break;	
					case 3:	DRW_DrawBMP_FT(dobj,0);	break;	
				}
				break;
			case DISP_TRUE:
				switch( draw_bpp ){
					default:DebugPrintf( "画像と画面モードが合いません。" );	break;
					case 0:	DRW_DrawBMP_TB(dobj);	break;	
					case 3:	DRW_DrawBMP_TT(dobj);	break;	
				}
				break;
			}
		}
		j/=10;
	}
	return TRUE;
}



BOOL DrawGraphStr( void *dest, int x, int y, GRP_STRUCT *gs, int draw_mode )
{
	if( gs->bno<0 || gs->bno>=BMP_MAX ) return FALSE;
	char r,g,b;
	int	dx = gs->dx + x;
	int	dy = gs->dy + y;
	int	dw = gs->dw;
	int	dh = gs->dh;
	int	sx = gs->sx - BmpSet[gs->bno].pos.x;
	int	sy = gs->sy - BmpSet[gs->bno].pos.y;

	RECT	clip;
	int		draw_bpp;
	DRAW_OBJECT		dobj;

	if(gs->clip){
		clip.left   = gs->clip->left  + x;
		clip.top    = gs->clip->top   + y;
		clip.right  = gs->clip->right + x;
		clip.bottom = gs->clip->bottom+ y;
	}else{
		clip.left   = 0;
		clip.top    = 0;
		clip.right  = DISP_X;
		clip.bottom = DISP_Y;
	}

	if(!gs->brt_flag){
		if( BrightR<=BRT_NML)	r =       gs->r * BrightR          / BRT_NML;
		else					r = (0xff-gs->r)*(BrightR-BRT_NML) / BRT_NML + gs->r;
		if( BrightG<=BRT_NML)	g =       gs->g * BrightG          / BRT_NML;
		else					g = (0xff-gs->g)*(BrightG-BRT_NML) / BRT_NML + gs->g;
		if( BrightB<=BRT_NML)	b =       gs->b * BrightB          / BRT_NML;
		else					b = (0xff-gs->b)*(BrightB-BRT_NML) / BRT_NML + gs->b;
	}else{
		r = gs->r;
		g = gs->g;
		b = gs->b;
	}

	dobj.mask = NULL;
	draw_bpp = BmpSet[gs->bno].bmp_bit;
	int	i,j;


	i=0;
	while( gs->str[i] ){
		if( '!' <= gs->str[i] && gs->str[i] <= '_' ){
			j = gs->str[i]-('!'-1);

			dobj = DRW_SetDrawObject( BPP(draw_mode), dest, dx+dw*i, dy, dw, dh,
									BmpSet[gs->bno].bmp_bit,  DSP_GetBmp(gs->bno),  sx+(gs->sw/16)*(j%16), sy+(gs->sh)*(j/16),   gs->sw/16,  gs->sh,
									0, NULL, 0, 0, 0, 0,
									r, g, b, gs->nuki, &clip, gs->param, gs->param2, gs->param3, gs->rparam );
			switch( draw_mode ){
			case DISP_256:
				break;
			case DISP_HIGH:
				switch( draw_bpp ){
					default:DebugPrintf( "画像と画面モードが合いません。" );	break;
					case 0:	DRW_DrawBMP_HB(dobj);	break;	
					case 1:	DRW_DrawBMP_HH(dobj);	break;	
				}
				break;
			case DISP_FULL:
				switch( draw_bpp ){
					default:DebugPrintf( "画像と画面モードが合いません。" );	break;
					case 0:	DRW_DrawBMP_FB(dobj,0);	break;	
					case 2:	DRW_DrawBMP_FF(dobj,0);	break;	
					case 3:	DRW_DrawBMP_FT(dobj,0);	break;	
				}
				break;
			case DISP_TRUE:
				switch( draw_bpp ){
					default:DebugPrintf( "画像と画面モードが合いません。" );	break;
					case 0:	DRW_DrawBMP_TB(dobj);	break;	
					case 3:	DRW_DrawBMP_TT(dobj);	break;	
				}
				break;
			}
		}
		i++;
	}
	return TRUE;
}




BOOL DrawGraphBmp( void *dest, int x, int y, GRP_STRUCT *gs, int draw_mode, int interless )
{
	if( gs->bno<0 || gs->bno>=BMP_MAX ) return FALSE;
	char r,g,b;

	int	dx  = gs->dx +x;
	int	dx2 = gs->dx2+x;
	int	dx3 = gs->dx3+x;
	int	dx4 = gs->dx4+x;
	int	dy  = gs->dy +y;
	int	dy2 = gs->dy2+y;
	int	dy3 = gs->dy3+y;
	int	dy4 = gs->dy4+y;

	int	dw,dh;
	int	sx  = gs->sx  - BmpSet[gs->bno].pos.x;
	int	sx2 = gs->sx2 - BmpSet[gs->bno].pos.x;
	int	sx3 = gs->sx3 - BmpSet[gs->bno].pos.x;
	int	sx4 = gs->sx4 - BmpSet[gs->bno].pos.x;
	int	sy  = gs->sy  - BmpSet[gs->bno].pos.y;
	int	sy2 = gs->sy2 - BmpSet[gs->bno].pos.y;
	int	sy3 = gs->sy3 - BmpSet[gs->bno].pos.y;
	int	sy4 = gs->sy4 - BmpSet[gs->bno].pos.y;

	int	sx22  = gs->sx2 - BmpSet[gs->bno2].pos.x;
	int	sy22  = gs->sy2 - BmpSet[gs->bno2].pos.y;

	RECT	clip;
	int		draw_bpp;
	DRAW_OBJECT		dobj;

	switch(BmpSet[gs->bno].bmp_bit){
		case 0:
			if( BmpSet[gs->bno].bmp_b.buf==NULL ){
				DebugPrintf("moke[%d] ",gs->bno);
			}
			break;
		case 2:
			if( BmpSet[gs->bno].bmp_f.buf==NULL ){
				DebugPrintf("moke[%d] ",gs->bno);
			}
			break;
		case 3:
			if( BmpSet[gs->bno].bmp_t.buf==NULL ){
				DebugPrintf("moke[%d] ",gs->bno);
			}
			break;
	}
	if(gs->zoom){
		dx = gs->cx+(((gs->dx        - gs->cx)*(gs->zoom+256))>>8);
		dy = gs->cy+(((gs->dy        - gs->cy)*(gs->zoom+256))>>8);
		dw = gs->cx+(((gs->dx+gs->dw - gs->cx)*(gs->zoom+256))>>8)-dx;
		dh = gs->cy+(((gs->dy+gs->dh - gs->cy)*(gs->zoom+256))>>8)-dy;
		dx += x;
		dy += y;
	}else{
		dw = gs->dw;
		dh = gs->dh;
	}

	if(gs->clip){
		clip.left   = gs->clip->left  + x;
		clip.top    = gs->clip->top   + y;
		clip.right  = gs->clip->right + x;
		clip.bottom = gs->clip->bottom+ y;
	}else{
		clip.left   = 0;
		clip.top    = 0;
		clip.right  = DISP_X;
		clip.bottom = DISP_Y;
	}

	if(!gs->brt_flag){
		if( BrightR<=BRT_NML)	r =       gs->r * BrightR          / BRT_NML;
		else					r = (0xff-gs->r)*(BrightR-BRT_NML) / BRT_NML + gs->r;
		if( BrightG<=BRT_NML)	g =       gs->g * BrightG          / BRT_NML;
		else					g = (0xff-gs->g)*(BrightG-BRT_NML) / BRT_NML + gs->g;
		if( BrightB<=BRT_NML)	b =       gs->b * BrightB          / BRT_NML;
		else					b = (0xff-gs->b)*(BrightB-BRT_NML) / BRT_NML + gs->b;
	}else{
		r = gs->r;
		g = gs->g;
		b = gs->b;
	}
	dobj = DRW_SetDrawObject( BPP(draw_mode), dest, dx, dy, dw, dh,
							BmpSet[gs->bno].bmp_bit,  DSP_GetBmp(gs->bno),  sx,   sy,   gs->sw,  gs->sh,
							BmpSet[gs->bno2].bmp_bit, DSP_GetBmp(gs->bno2), sx22, sy22, gs->sw2, gs->sh2,
							r, g, b, gs->nuki, &clip, gs->param, gs->param2, gs->param3, gs->rparam );
	dobj.mask = DSP_GetBmp(gs->bno3);

	DRW_SetDrawObjectPoly( &dobj, dx, dy, dx2, dy2, dx3, dy3, dx4, dy4,
								  sx, sy, sx2, sy2, sx3, sy3, sx4, sy4 );
	draw_bpp = BmpSet[gs->bno].bmp_bit;
	switch( draw_mode ){
	case DISP_256:
		switch( gs->poly ){
			case POL_RECT:
				switch( draw_bpp ){
					default:DebugPrintf( "画像と画面モードが合いません。" );	break;
					case 0:	
						DRW_DrawBMP_BB( (BMP_BT*)dest, dx, dy, &BmpSet[gs->bno].bmp_b, gs->sx, gs->sy, gs->dw, gs->dh,
										gs->nuki, &clip, r, g, b, gs->param, gs->rparam );
						break;
				}
				break;

			case POL_ZOOM:
				switch( draw_bpp ){
					default:DebugPrintf( "画像と画面モードが合いません。" );	break;
					case 0:	
						break;
				}
				break;

			case POL_POL4:
				switch( draw_bpp ){
					default:DebugPrintf( "画像と画面モードが合いません。" );	break;
					case 0:	
						break;
				}
				break;
		}
		break;
	case DISP_HIGH:
		switch( gs->poly ){
			case POL_RECT:
				switch( draw_bpp ){
					default:DebugPrintf( "画像と画面モードが合いません。" );	break;
					case 0:	DRW_DrawBMP_HB( dobj );	break;	
					case 1:	
						if(!gs->bset) { DRW_DrawBMP_HH( dobj ); }
						else          { DRW_DrawBMP_HHH( dobj );}
						break;
				}
				break;

			case POL_ZOOM:
				switch( draw_bpp ){
					default:DebugPrintf( "画像と画面モードが合いません。" );	break;
					case 0:	DRW_DrawZOOM_HB( dobj );	break;	
					case 1:	DRW_DrawZOOM_HH( dobj );	break;	
				}
				break;

			case POL_POL4:
				switch( draw_bpp ){
					default:DebugPrintf( "画像と画面モードが合いません。" );	break;
					case 0:	DRW_DrawPOLY4_HB( dobj );	break;	
					case 1:	DRW_DrawPOLY4_HH( dobj );	break;	
				}
				break;
		}
		break;
	case DISP_FULL:
		switch( gs->poly ){
			case POL_RECT:
				switch( draw_bpp ){
					default:DebugPrintf( "画像と画面モードが合いません。" );	break;
					case 0:	DRW_DrawBMP_FB( dobj, interless );	break;	
					case 2:	
						switch( gs->bset) {
							case 0:	DRW_DrawBMP_FF(dobj, interless);	break;
							case 1:
								if(BmpSet[gs->bno2].bmp_bit==2)	DRW_DrawBMP_FFF(dobj, interless);
								else							DRW_DrawBMP_FFB(dobj, interless);
								break;
							case 2:	DRW_DrawBMP_FFBM(dobj, interless);	break;
						}
						break;
					case 3:	
						if( !gs->bset )	DRW_DrawBMP_FT( dobj, interless );
						else			DRW_DrawBMP_FTT( dobj, interless );
						break;
				}
				break;

			case POL_ZOOM:
				switch( draw_bpp ){
					default:DebugPrintf( "画像と画面モードが合いません。" );	break;
					case 0:	DRW_DrawZOOM_FB( dobj, interless );	break;	
					case 2:	DRW_DrawZOOM_FF( dobj, interless );	break;	
					case 3:	DRW_DrawZOOM_FT( dobj, interless );	break;	
				}
				break;
			case POL_POL4:
				switch( draw_bpp ){
					default:DebugPrintf( "画像と画面モードが合いません。" );	break;
					case 0:	DRW_DrawPOLY4_FB( dobj, interless );	break;	
					case 2:	DRW_DrawPOLY4_FF( dobj, interless );	break;	

				}
				break;
		}
		break;
	case DISP_TRUE:
		switch( gs->poly ){
			case POL_RECT:
				switch( draw_bpp ){
					default:DebugPrintf( "画像と画面モードが合いません。" );	break;
					case 0:	DRW_DrawBMP_TB( dobj );	break;	
					case 3:	
						if(!gs->bset) { DRW_DrawBMP_TT(dobj);  }
						else          { DRW_DrawBMP_TTT(dobj); }
						break;
				}
				break;

			case POL_ZOOM:
				switch( draw_bpp ){
					default:DebugPrintf( "画像と画面モードが合いません。" );	break;
					case 0:	DRW_DrawZOOM_TB( dobj );	break;	
					case 3:	DRW_DrawZOOM_TT( dobj );	break;	
				}
				break;

			case POL_POL4:
				switch( draw_bpp ){
					default:DebugPrintf( "画像と画面モードが合いません。" );	break;
					case 0:	DRW_DrawPOLY4_TB( dobj );	break;	
					case 3:	DRW_DrawPOLY4_TT( dobj );	break;	
				}
				break;
		}
		break;
	}
	return TRUE;
}



BOOL DrawGraphPrim( void *dest, int x, int y, GRP_STRUCT *gs, int draw_mode )
{

	char r,g,b;
	char r2,g2,b2;
	char r3,g3,b3;
	char r4,g4,b4;
	int	dx  = gs->dx +x;
	int	dx2 = gs->dx2+x;
	int	dx3 = gs->dx3+x;
	int	dx4 = gs->dx4+x;
	int	dy  = gs->dy +y;
	int	dy2 = gs->dy2+y;
	int	dy3 = gs->dy3+y;
	int	dy4 = gs->dy4+y;
	int	gy1 = gs->sy+y;
	int	gy2 = gs->sy2+y;

	RECT	clip;

	if(gs->clip){
		clip.left   = gs->clip->left  + x;
		clip.top    = gs->clip->top   - y;
		clip.right  = gs->clip->right + x;
		clip.bottom = gs->clip->bottom- y;
	}else{
		clip.left   = 0;
		clip.top    = 0;
		clip.right  = DISP_X;
		clip.bottom = DISP_Y;
	}

	
	if(!gs->brt_flag){
		if( BrightR<=BRT_NML)	r =       gs->r * BrightR          / BRT_NML;
		else					r = (0xff-gs->r)*(BrightR-BRT_NML) / BRT_NML + gs->r;
		if( BrightG<=BRT_NML)	g =       gs->g * BrightG          / BRT_NML;
		else					g = (0xff-gs->g)*(BrightG-BRT_NML) / BRT_NML + gs->g;
		if( BrightB<=BRT_NML)	b =       gs->b * BrightB          / BRT_NML;
		else					b = (0xff-gs->b)*(BrightB-BRT_NML) / BRT_NML + gs->b;

		if( BrightR<=BRT_NML)	r2 =       gs->r2 * BrightR          / BRT_NML;
		else					r2 = (0xff-gs->r2)*(BrightR-BRT_NML) / BRT_NML + gs->r2;
		if( BrightG<=BRT_NML)	g2 =       gs->g2 * BrightG          / BRT_NML;
		else					g2 = (0xff-gs->g2)*(BrightG-BRT_NML) / BRT_NML + gs->g2;
		if( BrightB<=BRT_NML)	b2 =       gs->b2 * BrightB          / BRT_NML;
		else					b2 = (0xff-gs->b2)*(BrightB-BRT_NML) / BRT_NML + gs->b2;

		if( BrightR<=BRT_NML)	r3 =       gs->r3 * BrightR          / BRT_NML;
		else					r3 = (0xff-gs->r3)*(BrightR-BRT_NML) / BRT_NML + gs->r3;
		if( BrightG<=BRT_NML)	g3 =       gs->g3 * BrightG          / BRT_NML;
		else					g3 = (0xff-gs->g3)*(BrightG-BRT_NML) / BRT_NML + gs->g3;
		if( BrightB<=BRT_NML)	b3 =       gs->b3 * BrightB          / BRT_NML;
		else					b3 = (0xff-gs->b3)*(BrightB-BRT_NML) / BRT_NML + gs->b3;

		if( BrightR<=BRT_NML)	r4 =       gs->r4 * BrightR          / BRT_NML;
		else					r4 = (0xff-gs->r4)*(BrightR-BRT_NML) / BRT_NML + gs->r4;
		if( BrightG<=BRT_NML)	g4 =       gs->g4 * BrightG          / BRT_NML;
		else					g4 = (0xff-gs->g4)*(BrightG-BRT_NML) / BRT_NML + gs->g4;
		if( BrightB<=BRT_NML)	b4 =       gs->b4 * BrightB          / BRT_NML;
		else					b4 = (0xff-gs->b4)*(BrightB-BRT_NML) / BRT_NML + gs->b4;
	}else{
		r = gs->r;		g = gs->g;		b = gs->b;
		r2 = gs->r2;	g2 = gs->g2;	b2 = gs->b2;
		r3 = gs->r3;	g3 = gs->g3;	b3 = gs->b3;
		r4 = gs->r4;	g4 = gs->g4;	b4 = gs->b4;
	}

	switch( draw_mode ){
	case DISP_256:
		break;
	case DISP_HIGH:
		switch( gs->poly ){
			case POL_BOX:
				
				PRM_DrawRect_H( (BMP_H *)dest, dx, dy,                 gs->dw, gs->nuki, &clip, r, g, b, gs->param );
				PRM_DrawRect_H( (BMP_H *)dest, dx, dy+gs->dh-gs->nuki, gs->dw, gs->nuki, &clip, r, g, b, gs->param );
				PRM_DrawRect_H( (BMP_H *)dest, dx, dy,                 gs->nuki, gs->dh, &clip, r, g, b, gs->param );
				PRM_DrawRect_H( (BMP_H *)dest, dx+gs->dw-gs->nuki, dy, gs->nuki, gs->dh, &clip, r, g, b, gs->param );
				break;
			case POL_RECT:
			case POL_ZOOM:
				switch( gs->type ){
					default:DebugPrintf( "画像と画面モードが合いません。" );	break;
					case 4:	
						PRM_DrawRect_H( (BMP_H *)dest, dx, dy, gs->dw, gs->dh, &clip, r, g, b, gs->param );
						break;
					case 5:	
						PRM_DrawY4_H(  (BMP_H *)dest, dx,        dy,
												dx+gs->dw, dy,
												dx,        dy+gs->dh-1,
												dx+gs->dw, dy+gs->dh-1, &clip,
												gy1, r,  g,	 b,
												gy2, r2, g2, b2, gs->param );
						break;
					case 6:	
						PRM_DrawG4_H(  (BMP_H *)dest, dx,			dy,		r,  g,  b, 
												dx+gs->dw,	dy,		r2, g2, b2,
												dx,			dy+gs->dh-1,	r3, g3, b3,
												dx+gs->dw,	dy+gs->dh-1,	r4, g4, b4, &clip, gs->param );
						break;
				}
				break;
			case POL_POL4:
				switch( gs->type ){
					default:DebugPrintf( "画像と画面モードが合いません。" );	break;
					case 4:	
						PRM_DrawF4_H( (BMP_H *)dest, dx, dy, dx2, dy2, dx3, dy3, dx4, dy4, &clip, r, g, b, gs->param );
						break;
					case 5:	
						PRM_DrawY4_H( (BMP_H *)dest, dx, dy, dx2, dy2, dx3, dy3, dx4, dy4, &clip,
												gy1,  r,  g, b,
												gy2, r2, g2, b2, gs->param );
						break;
					case 6:	
						PRM_DrawG4_H(  (BMP_H *)dest, dx,  dy,  r,  g,  b,
												dx2, dy2, r2, g2, b2,
												dx3, dy3, r3, g3, b3,
												dx4, dy4, r4, g4, b4, &clip, gs->param );
						break;
				}
				break;

			case POL_POL3:
				switch( gs->type ){
					default:DebugPrintf( "画像と画面モードが合いません。" );	break;
					case 4:	
						PRM_DrawF3_H( (BMP_H *)dest, dx, dy, dx2, dy2, dx3, dy3, &clip, r, g, b, gs->param );
						break;
					case 5:	
						PRM_DrawY3_H( (BMP_H *)dest, dx, dy, dx2, dy2, dx3, dy3, &clip,
												gy1, r,  g,  b,
												gy2, r2, g2, b2, gs->param );
						break;
					case 6:	
						PRM_DrawG3_H( (BMP_H *)dest, dx,  dy,  r,  g,  b,
												dx2, dy2, r2, g2, b2,
												dx3, dy3, r3, g3, b3, &clip, gs->param );
						break;
				}
				break;
			case POL_LINE:
				switch( gs->type ){
					default:DebugPrintf( "画像と画面モードが合いません。" );	break;
					case 4:	
						PRM_DrawFLine_H( (BMP_H *)dest, dx, dy, dx2, dy2, &clip, r, g, b, gs->param );
						break;
					case 6:	
						PRM_DrawGLine_H( (BMP_H *)dest,  dx,  dy,  r,  g,  b,
													dx2, dy2, r2, g2, b2, &clip, gs->param );
						break;
				}
				break;
			case POL_KAGE:

				break;
		}
		break;
	case DISP_FULL:
		switch( gs->poly ){
			case POL_BOX:
				
				PRM_DrawRect_F( (BMP_F *)dest, dx, dy,                 gs->dw, gs->nuki, &clip, r, g, b, gs->param );
				PRM_DrawRect_F( (BMP_F *)dest, dx, dy+gs->dh-gs->nuki, gs->dw, gs->nuki, &clip, r, g, b, gs->param );
				PRM_DrawRect_F( (BMP_F *)dest, dx, dy,                 gs->nuki, gs->dh, &clip, r, g, b, gs->param );
				PRM_DrawRect_F( (BMP_F *)dest, dx+gs->dw-gs->nuki, dy, gs->nuki, gs->dh, &clip, r, g, b, gs->param );
				break;
			case POL_RECT:
			case POL_ZOOM:
				switch( gs->type ){
					default:DebugPrintf( "画像と画面モードが合いません。" );	break;
					case 4:	
						PRM_DrawRect_F( (BMP_F *)dest, dx, dy, gs->dw, gs->dh, &clip, r, g, b, gs->param );
						break;
					case 5:	
						PRM_DrawY4_F( (BMP_F *)dest, dx,        dy,
											dx+gs->dw, dy,
											dx,        dy+gs->dh-1,
											dx+gs->dw, dy+gs->dh-1,	&clip,
											gy1, r,  g,  b,
											gy2, r2, g2, b2, gs->param );
						break;
					case 6:	
						PRM_DrawG4_F( (BMP_F *)dest, dx,        dy,		r,  g,  b, 
											dx+gs->dw, dy,		r2, g2, b2,
											dx,        dy+gs->dh-1,	r3, g3, b3,
											dx+gs->dw, dy+gs->dh-1,	r4, g4, b4, &clip, gs->param );
						break;
				}
				break;

			case POL_POL4:
				switch( gs->type ){
					default:DebugPrintf( "画像と画面モードが合いません。" );	break;
					case 4:	
						PRM_DrawF4_F( (BMP_F *)dest, dx, dy, dx2, dy2, dx3, dy3, dx4, dy4, &clip, r, g, b, gs->param );
						break;
					case 5:	
						PRM_DrawY4_F( (BMP_F *)dest, dx, dy, dx2, dy2, dx3, dy3, dx4, dy4, &clip,
											gy1,  r,  g, b,
											gy2, r2, g2, b2, gs->param );
						break;
					case 6:	
						PRM_DrawG4_F( (BMP_F *)dest, dx,  dy,  r,  g,  b,
											dx2, dy2, r2, g2, b2,
											dx3, dy3, r3, g3, b3,
											dx4, dy4, r4, g4, b4, &clip, gs->param );
						break;
				}
				break;
			case POL_POL3:
				switch( gs->type ){
					default:DebugPrintf( "画像と画面モードが合いません。" );	break;
					case 4:	
						PRM_DrawF3_F( (BMP_F *)dest, dx, dy, dx2, dy2, dx3, dy3, &clip, r, g, b, gs->param );
						break;
					case 5:	
						PRM_DrawY3_F( (BMP_F *)dest, dx, dy, dx2, dy2, dx3, dy3, &clip,
											gy1,  r,  g, b,
											gy2, r2, g2, b2, gs->param );
						break;
					case 6:	
						PRM_DrawG3_F( (BMP_F *)dest, dx,  dy,  r,  g,  b,
											dx2, dy2, r2, g2, b2,
											dx3, dy3, r3, g3, b3, &clip, gs->param );
						break;
				}
				break;
			case POL_LINE:
				switch( gs->type ){
					default:DebugPrintf( "画像と画面モードが合いません。" );	break;
					case 4:	
						PRM_DrawFLine_F( (BMP_F *)dest, dx, dy, dx2, dy2, &clip, r, g, b, gs->param );
						break;
					case 6:	
						PRM_DrawGLine_F( (BMP_F *)dest,  dx,  dy,  r,  g,  b,
												dx2, dy2, r2, g2, b2, &clip, gs->param );
						break;
				}
				break;
			case POL_KAGE:

				break;
			case POL_RPLE:
				PRM_DrawRipple_F( (BMP_F *)dest, dx, dy, &clip, r,  g,  b, gs->param );
				break;
		}
		break;
	case DISP_TRUE:

		switch( gs->poly ){
			case POL_BOX:
				
				PRM_DrawRect_T( (BMP_T *)dest, dx, dy,                 gs->dw, gs->nuki, &clip, r, g, b, gs->param );
				PRM_DrawRect_T( (BMP_T *)dest, dx, dy+gs->dh-gs->nuki, gs->dw, gs->nuki, &clip, r, g, b, gs->param );
				PRM_DrawRect_T( (BMP_T *)dest, dx, dy,                 gs->nuki, gs->dh, &clip, r, g, b, gs->param );
				PRM_DrawRect_T( (BMP_T *)dest, dx+gs->dw-gs->nuki, dy, gs->nuki, gs->dh, &clip, r, g, b, gs->param );
				break;
			case POL_RECT:
			case POL_ZOOM:
				switch( gs->type ){
					default:DebugPrintf( "画像と画面モードが合いません。" );	break;
					case 4:	
						PRM_DrawRect_T( (BMP_T *)dest, dx, dy, gs->dw, gs->dh, &clip, r, g, b, gs->param );
						break;
					case 5:	
						PRM_DrawY4_T( (BMP_T *)dest, dx,        dy,
											dx+gs->dw, dy,
											dx,        dy+gs->dh-1,
											dx+gs->dw, dy+gs->dh-1,	&clip,
											gy1, r,  g,  b,
											gy2, r2, g2, b2, gs->param );
						break;
					case 6:	
						PRM_DrawG4_T( (BMP_T *)dest, dx,			dy,		r,  g,  b, 
											dx+gs->dw,	dy,		r2, g2, b2,
											dx,			dy+gs->dh-1,	r3, g3, b3,
											dx+gs->dw,	dy+gs->dh-1,	r4, g4, b4, &clip, gs->param );
						break;
				}
				break;

			case POL_POL4:
				switch( gs->type ){
					default:DebugPrintf( "画像と画面モードが合いません。" );	break;
					case 4:	
						PRM_DrawF4_T( (BMP_T *)dest, dx, dy, dx2, dy2, dx3, dy3, dx4, dy4, &clip, r, g, b, gs->param );
						break;
					case 5:	
						PRM_DrawY4_T( (BMP_T *)dest, dx, dy, dx2, dy2, dx3, dy3, dx4, dy4, &clip,
											gy1,  r,  g, b,
											gy2, r2, g2, b2, gs->param );
						break;
					case 6:	
						PRM_DrawG4_T( (BMP_T *)dest, dx,  dy,  r,  g,  b,
											dx2, dy2, r2, g2, b2,
											dx3, dy3, r3, g3, b3,
											dx4, dy4, r4, g4, b4, &clip, gs->param );
						break;
				}
				break;

			case POL_POL3:
				switch( gs->type ){
					default:DebugPrintf( "画像と画面モードが合いません。" );	break;
					case 4:	
						PRM_DrawF3_T( (BMP_T *)dest, dx, dy, dx2, dy2, dx3, dy3, &clip, r, g, b, gs->param );
						break;
					case 5:	
						PRM_DrawY3_T( (BMP_T *)dest, dx, dy, dx2, dy2, dx3, dy3, &clip,
											gy1,  r,  g, b,
											gy2, r2, g2, b2, gs->param );
						break;
					case 6:	
						PRM_DrawG3_T( (BMP_T *)dest, dx,  dy,  r,  g,  b,
											dx2, dy2, r2, g2, b2,
											dx3, dy3, r3, g3, b3, &clip, gs->param );
						break;
				}
				break;
			case POL_LINE:
				switch( gs->type ){
					default:DebugPrintf( "画像と画面モードが合いません。" );	break;
					case 4:	
						PRM_DrawFLine_T( (BMP_T *)dest, dx, dy, dx2, dy2, &clip, r, g, b, gs->param );
						break;
					case 6:	
						PRM_DrawGLine_T( (BMP_T *)dest,  dx,  dy,  r,  g,  b,
												dx2, dy2, r2, g2, b2, &clip, gs->param );
						break;
				}
				break;
			case POL_KAGE:

				break;
		}
		break;
	}
	return TRUE;
}



BOOL DrawGraphText( void *dest, int x, int y, TEXT_STRUCT *ts, int draw_mode, int kaigyou_musi )
{
	RECT	clip;

	if(ts->clip){
		clip.left   = ts->clip->left  + x;
		clip.top    = ts->clip->top   - y;
		clip.right  = ts->clip->right + x;
		clip.bottom = ts->clip->bottom- y;
	}else{
		clip.left   = 0;
		clip.top    = 0;
		clip.right  = DISP_X;
		clip.bottom = DISP_Y;
	}

	if(ts->brt_flag)TXT_DrawText( dest, draw_mode, ts->dx+x, ts->dy+y, ts->ws, ts->hs, ts->pw, ts->ph, &ts->px, &ts->py, &clip, ts->font, ts->str, ts->color, ts->cnt, ts->step, 128, 128, 128, ts->alph, ts->kage, kaigyou_musi );
	else			TXT_DrawText( dest, draw_mode, ts->dx+x, ts->dy+y, ts->ws, ts->hs, ts->pw, ts->ph, &ts->px, &ts->py, &clip, ts->font, ts->str, ts->color, ts->cnt, ts->step, BrightR, BrightG, BrightB, ts->alph, ts->kage, kaigyou_musi );
	return TRUE;
}



BOOL DrawGraphDmoji( void *dest, int x, int y, DMOJI_STRUCT *ds, int draw_mode )
{

	if(ds->type)DRW_DrawDmoji( dest, ds->dx+x, ds->dy+y, ds->str,   ds->param, draw_mode );
	else		DRW_DrawDigit( dest, ds->dx+x, ds->dy+y, ds->digit, ds->param, draw_mode );
		
	return TRUE;
}

BOOL GetGraph( void *dest, int draw_mode )
{
	DRAW_OBJECT	dobj;
	BOOL	ret=FALSE;
	void	*bmp[4] = { &BmpSet[GetBackNo].bmp_b,
						&BmpSet[GetBackNo].bmp_h,
						&BmpSet[GetBackNo].bmp_f,
						&BmpSet[GetBackNo].bmp_t };
	int		bpp = BPP(draw_mode);

	if( GetBackFlag ){
		GetBackFlag = 0;
		dobj = DRW_SetDrawObject( bpp, bmp[bpp], 0, 0, DISP_X, DISP_Y,
								  bpp, dest,     0, 0, DISP_X, DISP_Y, 0, NULL, 0, 0, 0, 0,
								  128, 128, 128, CHK_NO, NULL, DRW_NML, 0,0, 0 );

		switch( draw_mode ){

			case DISP_HIGH:	ret = DRW_DrawBMP_HH( dobj );	break;
			case DISP_FULL:	ret = DRW_DrawBMP_FF( dobj );	break;
			case DISP_TRUE:	ret = DRW_DrawBMP_TT( dobj );	break;
		}
	}
	return ret;
}










void DSP_DrawGraph( void *dest, int draw_mode, int save_disp, int frame, int interless, int anime_skip )
{
	int		i,lno;
	int		x = GlobalX;
	int		y = GlobalY;


	DSP_RenewMovie( draw_mode );

	VramBmp.alp_flag= 0;
	VramBmp.flag    = 1;
	VramBmp.bmp_bit = BPP(draw_mode);
	VramBmp.pos.x   = 0;
	VramBmp.pos.y   = 0;
	VramBmp.size.x  = DISP_X;
	VramBmp.size.y  = DISP_Y;

	switch( draw_mode ){
		case DISP_256:	VramBmp.bmp_b = *(BMP_BT*)dest;	break;
		case DISP_HIGH:	VramBmp.bmp_h = *(BMP_H*)dest;	break;
		case DISP_FULL:	VramBmp.bmp_f = *(BMP_F*)dest;	break;
		case DISP_TRUE:	VramBmp.bmp_t = *(BMP_T*)dest;	break;
	}




	GetGraph( dest, draw_mode );

	if( DispTempNo!=-1 ){
		switch( draw_mode ){
		case DISP_256:	dest = &BmpSet[DispTempNo].bmp_b;	break;
		case DISP_HIGH:	dest = &BmpSet[DispTempNo].bmp_h;	break;
		case DISP_FULL:	dest = &BmpSet[DispTempNo].bmp_f;	break;
		case DISP_TRUE:	dest = &BmpSet[DispTempNo].bmp_t;	break;
		}
	}

	for(i=0; i<MOVIE_MAX; i++){
		if(MoveControl[i].flag){
			if(MoveControl[i].bmp_no==-1){
				int	sx,sy;
				PAC_PackFileMovie_GetSize( i, &sx, &sy );
				if(sx==DISP_X  )PAC_PackFileMovie_Read( i, dest, draw_mode, OFF, ON, anime_skip );
				if(sx==DISP_X/2)PAC_PackFileMovie_Read( i, dest, draw_mode, ON,  ON, anime_skip );
			}else{
				BMP_SET	*bs = &BmpSet[ MoveControl[i].bmp_no ];
				switch( bs->bmp_bit ){
					case BMP_256P:	PAC_PackFileMovie_Read( i, &bs->bmp_b, DISP_256,  OFF, OFF, anime_skip );	break;
					case BMP_HIGH:	PAC_PackFileMovie_Read( i, &bs->bmp_h, DISP_HIGH, OFF, OFF, anime_skip );	break;
					case BMP_FULL:	PAC_PackFileMovie_Read( i, &bs->bmp_f, DISP_FULL, OFF, OFF, anime_skip );	break;
					case BMP_TRUE:	PAC_PackFileMovie_Read( i, &bs->bmp_t, DISP_TRUE, OFF, OFF, anime_skip );	break;
				}
			}
		}
	}
	XVID_UpdateAviTexture( 0, dest, draw_mode );

	for(lno=0; lno<LAYER_MAX; lno++){
		for(i=0; i<GRP_MAX ; i++){
			if( !GrpStruct[i].flag )		continue;
			if( !GrpStruct[i].disp )		continue;
			if( GrpStruct[i].layer != lno )	continue;
			
			if(GrpStruct[i].sp_clip){
				GrpStruct[i].clip = DSP_GetSpriteRect( GrpStruct[i].sp_clip-1 );
			}
			switch( GrpStruct[i].type) {
				case PRM_BMP:	DrawGraphBmp( dest, x, y, &GrpStruct[i], draw_mode, interless );		break;
				case PRM_SPR:	DrawGraphSprite( dest, x, y, &GrpStruct[i], draw_mode );	break;
				case PRM_DGT:	DrawGraphDigit( dest, x, y, &GrpStruct[i], draw_mode );	break;
				case PRM_STR:	DrawGraphStr( dest, x, y, &GrpStruct[i], draw_mode );	break;
				default:DrawGraphPrim( dest, x, y, &GrpStruct[i], draw_mode );		break;
			}
		}
		for(i=0; i<TXT_MAX ; i++){
			if( !TextStruct[i].flag )		continue;
			if( !TextStruct[i].disp )		continue;
			if( TextStruct[i].layer != lno)	continue;
			
			DrawGraphText( dest, x, y, &TextStruct[i], draw_mode );
		}
	}

	for(i=0; i<DMOJI_MAX ; i++){
		if( !DmojiStruct[i].flag )	continue;
		if( !DmojiStruct[i].disp )	continue;
		DrawGraphDmoji( dest, x, y, &DmojiStruct[i], draw_mode );
	}

	for(i=0; i<GRP_MAX ; i++){
		if( GrpStruct[i].target ){
			GrpStruct[i].target = OFF;
			GrpStruct[i].disp   = ON;
		}
	}
	for(i=0; i<TXT_MAX ; i++){
		if( TextStruct[i].target ){
			TextStruct[i].target = OFF;
			TextStruct[i].disp   = ON;
		}
	}

	switch( draw_mode ){
		case DISP_256:
			break;
		case DISP_HIGH:
			PRM_DrawRect_H( (BMP_H *)dest, 0, 0,   x, DISP_Y, NULL, 0, 0, 0, DRW_NML );
			PRM_DrawRect_H( (BMP_H *)dest, x, 0, DISP_X-x, y, NULL, 0, 0, 0, DRW_NML );
			PRM_DrawRect_H( (BMP_H *)dest, DISP_X+x, y, -x, DISP_Y-y, NULL, 0, 0, 0, DRW_NML );
			PRM_DrawRect_H( (BMP_H *)dest, x, DISP_Y+y, DISP_X-x, -y, NULL, 0, 0, 0, DRW_NML );
			break;
		case DISP_FULL:
			PRM_DrawRect_F( (BMP_F *)dest, 0, 0,   x, DISP_Y, NULL, 0, 0, 0, DRW_NML );
			PRM_DrawRect_F( (BMP_F *)dest, x, 0, DISP_X-x, y, NULL, 0, 0, 0, DRW_NML );
			PRM_DrawRect_F( (BMP_F *)dest, DISP_X+x, y, -x, DISP_Y-y, NULL, 0, 0, 0, DRW_NML );
			PRM_DrawRect_F( (BMP_F *)dest, x, DISP_Y+y, DISP_X-x, -y, NULL, 0, 0, 0, DRW_NML );
			break;
		case DISP_TRUE:
			PRM_DrawRect_T( (BMP_T *)dest, 0, 0,   x, DISP_Y, NULL, 0, 0, 0, DRW_NML );
			PRM_DrawRect_T( (BMP_T *)dest, x, 0, DISP_X-x, y, NULL, 0, 0, 0, DRW_NML );
			PRM_DrawRect_T( (BMP_T *)dest, DISP_X+x, y, -x, DISP_Y-y, NULL, 0, 0, 0, DRW_NML );
			PRM_DrawRect_T( (BMP_T *)dest, x, DISP_Y+y, DISP_X-x, -y, NULL, 0, 0, 0, DRW_NML );
			break;
	}







	if(save_disp){
		static int	bmp_count=0;
		char		buf[256];
		wsprintf( buf, "buf\\%06d.bmp", bmp_count );
		BMP_SaveBmp_F( buf, (BMP_F *)dest );
		bmp_count++;
	}


}

