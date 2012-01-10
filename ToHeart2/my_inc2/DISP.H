
 
  
   
    
     
      

#ifndef	_DISP_H_
#define _DISP_H_

#include "comp_pac.h"
#include "bmp.h"
#include "draw.h"

extern int	DISP_X;
extern int	DISP_Y;
extern int	DISP_DY;


#define BMP_256P	0
#define BMP_HIGH	1
#define BMP_FULL	2
#define BMP_TRUE	3

#define PRM_BMP		0
#define PRM_SPR		1
#define PRM_DGT		2
#define PRM_STR		3

#define PRM_FLAT	4
#define PRM_YGRA	5
#define PRM_GULO	6

#define PRM_BAPC	10	
#define PRM_BAVI	11	

#define POL_RECT	0
#define POL_ZOOM	1
#define POL_POL4	2
#define POL_POL3	3
#define POL_LINE	4
#define POL_BOX		5
#define POL_KAGE	6
#define POL_RPLE	7


#define BPP(B)	((B)/8-1)

#define DMOJI_DIGIT		0
#define DMOJI_TEXT		1

extern void DSP_InitDisp( int grp, int txt, int bmp, int lay );
extern void DSP_TermDisp( void );

extern void DSP_ResetGraphAll( void );
extern void DSP_ResetTextAll( void );
extern void DSP_ReleaseBmpAll( void );
extern void DSP_ReleaseBmpAnimeAll( void );
extern void DSP_ReleaseAviAll( void );
extern void DSP_ReleaseSpriteAll(void );

extern void DSP_ReleaseBmp( int bno );
extern void DSP_CreateBmp( int bno, int bmp_bit, int w, int h );

extern BOOL DSP_ChangeBmp( int bno, int chg_bpp );
extern BOOL DSP_ChangeBmpAll( int chg_bpp );

extern void DSP_SetDispTemp( int bno );
extern void DSP_ResetDispTemp( void );
extern void DSP_SetDrawTemp( int bno, int draw_mode, int frame  );

extern void DSP_GetDispBmp( int bno, int draw_mode );

extern BOOL LoadBmpSet( BMP_SET	*bmp_set, int bmp_bit, char *fname, char *pack=NULL );

extern BOOL DSP_LoadBmp(  int bno, int bmp_bit, char *fname, char *tfname=NULL, int viv=256, char *pack=NULL );
extern BOOL DSP_LoadBmp2( int bno, int bmp_bit, char *fname, char *tfname=NULL, int viv=256, int r=128, int g=128, int b=128, int param=DRW_NML, int rparam=REV_NOT, char *pack=NULL );

extern void DSP_CopyBmp( int db_no, int sb_no );
extern void DSP_CopyBmp2( int db_no, int sb_no, char *tfname=NULL, int viv=256, int r=128, int g=128, int b=128 );
extern void DSP_CopyBmp3( int db_no, BMP_SET bmp_set );

extern BOOL DSP_SetTonecurve( int bno, char *tfname, int viv );

extern int DSP_GetBmpFlag( int bno );
extern int DSP_GetBmpBit( int bno );

extern int DSP_LoadBmpAnime( int bno, int bmp_bit, char *fname );
extern void DSP_ReleaseBmpAnime( int mh );
extern int DSP_GetBmpAnimeCondition( int mh );
extern int DSP_GetBmpAnimeLoop( int mh );
extern void DSP_GetBmpAnimeSize( int mh, int *sx, int *sy );
extern BOOL DSP_GetBmpAnimeSabun( int mh );
extern BOOL DSP_PlayBmpAnime( int mh, int initial, int loop );
extern BOOL DSP_StopBmpAnime( int mh, int initial );

extern BOOL DSP_SetBmpAnimeLoop( int mh, int loop );
extern BOOL DSP_SetBmpAnimeBright( int mh, int r, int g, int b );
extern BOOL DSP_SetBmpAnimeBmpNo( int mh, int bno );



extern BOOL DSP_LoadSpriteCash( char *fname, char *tfname,int viv, int nuki );
extern BOOL DSP_LoadSprite( int sno, char *fname, int nuki=CHK_NO, char *tfname=NULL, int viv=256 );

extern BOOL DSP_CopySprite( int ano1, int ano2, char *tfname=NULL, int viv=256, int r=128, int g=128, int b=128 );

extern void DSP_ReleaseSprite( int sno );
extern void DSP_ResetSpriteCash( void );

extern BOOL DSP_SetSpritePlay( int gno, int ano, int sno, int end=ON, int lnum=0, int cfade=OFF );
extern BOOL DSP_SetSpriteNo( int gno, int sno );
extern BOOL DSP_SetSpriteCfade( int gno, int flag );
extern BOOL DSP_SetSpriteLoop( int gno, int end, int lnum );
extern BOOL DSP_ThroughSpriteLoop( int gno );
extern BOOL DSP_SetSpriteRepeatFind( int gno );
extern int DSP_GetSpriteRepeatFind( int gno );
extern RECT *DSP_GetSpriteRect( int gno );

extern int DSP_WaitSpriteCondition( int gno );
extern int DSP_GetSpriteEvent( int gno );

extern void DSP_SetSprite( int gno, int ano, int sno, int lno, int disp, int end, int lnum, int cfade );



extern void DSP_SetGraph( int gno, int bno, int lno,int disp,int nuki );
extern void DSP_SetDigit( int gno, int bno, int lno,int disp,int nuki, int digit, int keta );
extern void DSP_SetDigitNum( int gno, int digit );

extern void DSP_SetGraphStr( int gno, int bno, int lno,int disp,int nuki, char *str );
extern void DSP_SetGraphStrDirect( int gno, char *str );

extern void DSP_SetGraphPrim( int gno, int type, int poly, int lno, int disp );
extern void DSP_ResetGraph( int gno );

extern void DSP_SetGraphBSet( int gno, int bno2, int blnd, int vague=128 );
extern void DSP_SetGraphBSet2( int gno, int bno2, int bno3, int blnd );
extern void DSP_ResetGraphBSet( int gno );
extern void DSP_SetGraphPtarnFadeRate( int gno, int param3 );

extern void DSP_SetBmpParam( int bno, void *buf, int w, int h, int bit, void *pal );

extern void *DSP_GetBmp( int bno );
extern void *DSP_GetBmp2( BMP_SET *bs );
extern void *DSP_GetGraphBmp( int gno );
extern int	DSP_GetGraphBmpNo( int gno );

extern void DSP_GetBmpSize( int bno, int *sx, int *sy );
extern void DSP_GetBmpSize2( BMP_SET bs, int *sx, int *sy );
extern void DSP_GetGraphBmpSize( int gno, int *sx,int *sy );

extern int  DSP_GetGraphFlag( int gno );
extern int  DSP_GetGraphTarget( int gno );
extern int  DSP_GetGraphDisp( int gno );
extern int  DSP_GetGraphLayer( int gno );
extern int  DSP_GetGraphNuki( int gno );
extern int  DSP_GetGraphParam( int gno );
extern int  DSP_GetGraphParam2( int gno );
extern int  DSP_GetGraphType( int gno );
extern int  DSP_GetGraphBrightFlag( int gno );
extern void DSP_GetGraphGlobalBright( int *r,int *g,int *b );

extern void DSP_GetGraphBright( int gno, int *r,int *g,int *b );
extern void DSP_GetGraphMove( int gno, int *dx, int *dy );

extern void DSP_SetGraphBmp( int gno, int bno );
extern void DSP_SetGraphTarget( int gno, int target, int draw_mode );
extern void DSP_SetGraphDisp( int gno, int disp );
extern void DSP_SetGraphLayer( int gno, int lno );
extern void DSP_SetGraphNuki( int gno, int nuki );
extern void DSP_SetGraphParam( int gno, int param );
extern void DSP_SetGraphParam2( int gno, int param );
extern void DSP_SetGraphType( int gno, int type );
extern void DSP_SetGraphBrightFlag( int gno, int brt_flag );
extern void DSP_SetGraphBoxPich( int tno, int pich );

extern void DSP_SetGraphGlobalBright( int r,int g,int b );
extern void DSP_SetGraphBright( int gno, int r,int g,int b );
extern void DSP_SetGraphFade( int gno, int br );
extern void DSP_SetGraphBrightPoint( int gno, int point, int r,int g,int b );
extern void DSP_SetGraphBrightYGra( int gno, int point, int y, int r,int g,int b );

extern void DSP_SetGraphGlobalPos( int x,int y );

extern void DSP_SetGraphMoveX( int gno, int dx );
extern void DSP_SetGraphMoveY( int gno, int dy );
extern void DSP_SetGraphMove( int gno, int dx, int dy );
extern void DSP_SetGraphMove2( int gno, int dx, int dy );
extern void DSP_SetGraphWidth( int gno, int w, int h );

extern void DSP_SetGraphSMove( int gno, int sx, int sy );
extern void DSP_SetGraphSPos( int gno, int sx, int sy, int sw, int sh, int poly=-1 );

extern void DSP_SetGraphRevParam( int gno, DWORD rparam );
extern void DSP_SetGraphPos( int gno, int dx, int dy, int sx, int sy, int w, int h );
extern void DSP_SetGraphPosZoom( int gno,  int dx, int dy, int dw, int dh, int sx, int sy, int sw, int sh );
extern void DSP_SetGraphZoom( int gno, int dx, int dy, int dw, int dh );
extern void DSP_SetGraphZoomBai( int gno, int dx, int dy, int nuki, int dw, int sw );
extern void DSP_SetGraphZoom2( int gno, int cx, int cy, int zoom );
extern void DSP_SetGraphZoom3( int gno, int cx, int cy, int zoom );
extern void DSP_SetGraphRoll( int gno, int cx, int cy, int zoom, int rate, int sx, int sy, int sw, int sh );
extern void DSP_SetGraphPosPoly( int gno, int dx1, int dy1, int dx2, int dy2, int dx3, int dy3, int dx4, int dy4,
										  int sx1, int sy1, int sx2, int sy2, int sx3, int sy3, int sx4, int sy4 );
extern void DSP_SetGraphPosRect( int gno, int dx, int dy, int w, int h );
extern void DSP_SetGraphPosPoint( int gno, int point, int dx, int dy );
extern void DSP_SetGraphClip( int gno, int dx, int dy, int w, int h );
extern void DSP_SetGraphClipSprit( int gno, int sp_gno );

extern void DSP_SetText( int tno, int layer, int font, int disp, char *str, ... );
extern char *DSP_AddText( int tno, char *str );
extern void DSP_ResetText( int tno );

extern int  DSP_GetTextTarget( int tno );
extern int  DSP_GetTextDisp( int tno );
extern int  DSP_GetTextLayer( int tno );
extern int  DSP_GetTextBrightFlag( int tno );
extern int  DSP_GetTextAlph( int tno );
extern int  DSP_GetTextKage( int tno );
extern int  DSP_GetTextColor( int tno );
extern char *DSP_GetTextStr( int tno );
extern int  DSP_GetTextCount( int tno );
extern void DSP_GetTextMove( int tno, int *x, int *y );
extern int DSP_GetTextDispH( int tno );
extern int DSP_GetTextDispStr( int tno, char *str );
extern void DSP_GetTextDispPos( int tno, int *px, int *py, int kaigyou_musi=FALSE );


extern void DSP_SetTextTarget( int tno, int target, int draw_mode );
extern void DSP_SetTextDisp( int tno, int disp );
extern void DSP_SetTextLayer( int tno, int lno );
extern void DSP_SetTextBrightFlag( int tno, int brt_flag );
extern void DSP_SetTextColor( int tno, int color );
extern void DSP_SetTextAlph( int tno, int alph );
extern void DSP_SetTextKage( int tno, int kage );
extern void DSP_SetTextStr( int tno, char *str, ... );
extern void DSP_SetTextCount( int tno, int cnt );
extern void DSP_SetTextStep( int tno, int step );
extern void DSP_SetTextPos( int tno, int x, int y, int ws, int hs );
extern void DSP_SetTextMove( int tno, int x, int y );
extern void DSP_SetTextPich( int tno, int pw, int ph );
extern void DSP_SetTextClip( int tno, int dx, int dy, int w, int h );

extern void DSP_SetDmoji( int dno, int disp, int type, int digit, char *str );
extern void DSP_ResetDmoji( int dno );
extern void DSP_SetDmojiDisp( int dno, int disp );
extern void DSP_SetDmojiPos( int dno, int dx, int dy );
extern void DSP_SetDmojiDigit( int dno, int digit );
extern void DSP_SetDmojiStr( int dno, char *str );

extern void DSP_SetPackDirMov( char *pac_dir );
extern int DSP_LoadAvi( int ano, int bno, int w, int h, int bit, char *fname );
extern int DSP_ReleaseAvi( int ano );

extern int DSP_GetAviParam( int ano );
extern int DSP_GetAviNowFrameAudio( int ano );
extern int DSP_GetAviNowFrameVideo( int ano );
extern int DSP_GetAviMaxFrameAudio( int ano );
extern int DSP_GetAviMaxFrameVideo( int ano );
extern DWORD DSP_GetAviWavVolume( int ano );
extern BOOL DSP_SetAviWavVolume( int ano, DWORD volume );

extern void DSP_DrawGraph( void *dest, int draw_mode, int save_disp, int frame, int interless, int anime_skip );

#endif	