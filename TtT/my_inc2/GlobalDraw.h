#ifndef	_GLOBAL_DRAW_H_
#define _GLOBAL_DRAW_H_

#include "d3d_main.h"
#include "d3d_draw.h"


#define GDI_DRAW	0
#define D3D_DRAW	1

#define	FULLSCREEN_MODE		FALSE
#define	WINDOW_MODE			TRUE


extern void GD_ResetAll( void );



extern void GD_SetPackDir( char *pac_dir );
extern void GD_SetWindowStyle( DWORD win, DWORD full );
extern int	GD_InitDevices( HWND hwnd, int w, int h, int full=FALSE, HMENU menu=NULL);
extern int	GD_TermDevices(void);
extern void GD_Draw( int flash, int save );
extern void GD_LastBlt( void );
extern void GD_DisplayChange( void );
extern void GD_WindowModeChange( int mode );



extern int GD_ReleasePixelShader( int psno );
extern int GD_LoadPixelShader( int psno, char *filename );



extern int GD_ReleaseTexture( int tno );
extern int GD_CreateRenderTargetTexture( int tno, int w, int h );
extern int GD_WaitAviTexture( int tno  );
extern int GD_CreateAviTexture( int tno, char *fname );
extern int GD_CreateTextTexture( int tno, char *str, int font_size, int tw, int th, int pw, int ph, int kage, int r=255, int g=255, int b=255, int kr=0, int kg=0, int kb=0 );
extern int GD_UpdateTextTexture( int tno, char *str, int r=-1, int g=-1, int b=-1, int kr=-1, int kg=-1, int kb=-1 );

extern int GD_GetBackBuffer( int tno );
extern int GD_LoadTexture( int tno, char *filename, int alpha, int low_level=FALSE, int mip=FALSE, char *tfname=NULL, char *tfname2=NULL, int chroma=256, char *pak=NULL, int tex_size=D3DD_TEXTURE_CONTROL_SIZE );
extern int GD_LoadTextureBBM( int tno, char *filename, int alpha, int low_level=FALSE, int mip=FALSE, char *tfname=NULL, char *tfname2=NULL, int chroma=256, char *pak=NULL );

extern int GD_GetTextureWH( int tno, int *w, int *h );
extern int GD_GetBmpWH( int tno, int *w, int *h );


extern int GD_LoadSprite( int ani_no, char *fname,int alpha=ALPHA_4444, int low_level=FALSE, int mip=FALSE, char *tfname=NULL, char *tfname2=NULL, int chroma=256, char *pak=NULL );
extern int GD_ReleaseSprite( int ani_no );



extern void GDFNT_SetPackDir( char *pac_dir );
extern void GDFNT_ChangeFont( int ftype );
extern void GDFNT_LoadFont( int fsize, int ftype );
extern void GDFNT_ReleaseFont( int fsize );
extern void GDFNT_Init( void );
extern void GDFNT_Term( void );


extern int GD_SetSprite( int dno, int ani_no, int sp_no, int mode, int layer, int disp, int end=TRUE, int lnum=0, int cfade=FALSE );

extern BOOL GD_SetSpriteSpeed( int dno, int speed );
extern BOOL GD_SetSpritePlay( int dno, int ani_no, int sp_no, int mode=TRUE, int end=TRUE, int lnum=0, int cfade=FALSE  );
extern BOOL GD_SetSpriteNo( int dno, int sp_no );
extern BOOL GD_SetSpriteCfade( int dno, int flag );
extern BOOL GD_SetSpriteLoop( int dno, int end, int lnum );
extern BOOL GD_SetSpriteLoopThrough( int dno );
extern int  GD_GetSpriteRepeatFind( int dno );
extern BOOL GD_SetSpriteRepeatFind( int dno );
extern RECT *GD_GetSpriteRect( int dno );
extern BOOL GD_WaitSpriteCondition( int dno );
extern int  GD_GetSpriteMode( int dno );
extern BOOL GD_SetSpriteMode( int dno, int mode );
extern int GD_GetSpriteEvent( int dno );


extern void GD_ResetGraph( int dno );
extern void GD_SetGraph( int dno, int tno, int layer, int disp );
extern void GD_SetGraphx9( int dno, int tno, int layer, int disp );
extern void GD_SetGraphPrimitive( int dno, int vnum, int layer, int disp );
extern void GD_SetGraphCircle( int dno, int vnum, int paint, int layer, int disp );
extern void GD_SetGraphGrid( int dno, int grid, int layer, int disp );
extern void GD_SetGraphText( int dno, int tno, int layer, int disp );

extern void GD_SetGraphStr( int dno, int tno, int layer, int disp, char *str );
extern void GD_SetGraphStrChange( int dno, char *str );

extern void GD_SetGraphDigit( int dno, int tno, int layer, int disp, int digit, int keta );
extern void GD_SetGraphDigitNo( int dno, int digit );

extern BOOL GD_SetGraphTextStep( int dno, int step );
extern BOOL GD_SetGraphTextCount( int dno, int count );

extern void GD_SetGraphRenderNo( int dno, int rno );

extern void GD_SetGraphLayer( int dno, int layer );
extern void GD_SetGraphGroup( int dno, DWORD group );

extern void GD_SetGraphPointPos( int dno, int pno, float dx, float dy, float rhw=1.0f );
extern void GD_SetGraphRect( int dno, float dx, float dy,  float sx, float sy, float w, float h );
extern void GD_SetGraphDstRect( int dno, float dx, float dy, float dw, float dh );
extern void GD_SetGraphSrcRect( int dno, float sx, float sy, float sw, float sh );
extern void GD_SetGraphSrc2Rect( int dno, float sx, float sy, float sw, float sh );
extern void GD_SetGraphDst( int dno, float dx1, float dy1, float dx2, float dy2, float dx3, float dy3, float dx4, float dy4 );
extern void GD_SetGraphRoll2( int dno, float cx, float cy, float dw, float dh, float count );
extern void GD_SetGraphDstMove( int dno, float mx, float my );
extern void GD_SetGraphDstMoveWH( int dno, float dw, float dh );

extern void GD_SetGraphCenter( int dno, float cx, float cy );

extern void GD_SetGraphRoll( int dno, float kaiten );
extern void GD_SetGraphZoom( int dno, float zoom );
extern void GD_SetGraphZoomXY( int dno, float zoom_x, float zoom_y );
extern void GD_SetGraphDisp( int dno, char disp );
extern void GD_SetGraphSrc( int dno, int tno );
extern void GD_SetGraphSrc2( int dno, int tno2, int param );
extern void GD_ResetGraphSrc2( int dno );
extern void GD_SetGraphPixelFade( int dno, float c0_alpha );
extern void GD_SetGraphPixelShader( int dno, int ps_no, int c, float r, float g, float b, float a );
extern void GD_SetGraphBlendParam( int dno, int bparam );
extern void GD_SetGraphScaleParam( int dno, int sparam );
extern void GD_SetGraphRevParam( int dno, int rparam );
extern void GD_SetGraphColor( int dno, int a, int r, int g, int b );
extern void GD_SetGraphColorNo( int dno, int cno, int a, int r, int g, int b );
extern void GD_SetGraphFade( int dno, int fade );
extern void GD_SetGraphColor2( int dno, int r, int g, int b );
extern void GD_SetGraphAlpha( int dno, int a );
extern void GD_SetGraphBaseColor( int dno, int r, int g, int b );
extern void GD_SetGraphSpecular( int dno, int sr, int sg, int sb );

extern void GD_SetGraphBrightNo( int dno, int bright_no );
extern void GD_SetGlobalColor( int r, int g, int b, int type=0, int no=0 );

extern void GD_SetDispGroup( DWORD group );
extern void GD_AddDispGroup( DWORD group );
extern void GD_SubDispGroup( DWORD group );


#endif	