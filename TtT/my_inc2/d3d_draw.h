


#ifndef	_D3D_DRAW_H_
#define _D3D_DRAW_H_


#include <d3d8.h>
#include <d3dx8.h>
#include <ddraw.h>

#define D3DD_MAX_DISP_AMOUNT		2048
#define D3DD_MAX_TEXTURE_AMOUNT		1024
#define D3DD_SPRITE_TEXTURE_START	512

#define D3DD_MAX_TEXT_AMOUNT		64
#define D3DD_TEXTURE_SIZE			(16*256/D3DD_TEXTURE_CONTROL_SIZE+1)
#define D3DD_TEXTURE_CONTROL_SIZE	256

#define D3DD_MAX_PIXEL_SHADER		16

#define D3DD_DISP_LAYER_MAX			128



#define ALPHA_OFF			0	
#define ALPHA_4444			1	
#define ALPHA_1555			2	
#define ALPHA_FADE			3	
#define ALPHA_256P			4	

#define REV_U				0x0100	
#define REV_V				0x0200	
#define REV_UV				(REV_U|REV_V)	
#define DITHER_OFF			0x0800	



#define GTYPE_POLY		0	
#define GTYPE_SPRITE	1	
#define GTYPE_MOVIE		2	
#define GTYPE_GRID		3	
#define GTYPE_SP_FRAME	4	
#define GTYPE_TABLE		5	
#define GTYPE_POLYx9	6	

#define GTYPE_STR		7	

#define GTYPE_DIGIT		8	
#define GTYPE_TEXT		9	
#define GTYPE_FADE		10	

#define BLEND_NONE		0	
#define BLEND_ALPHA		1	
#define BLEND_ALPHA2	2	
#define BLEND_ADD		3	
#define BLEND_SCREEN	4	
#define BLEND_HARDLIGHT	5	
#define BLEND_MUL		6	
#define BLEND_SUB		7	
#define BLEND_REVMUL	8	
#define BLEND_REVSUB	9	
#define BLEND_EXCLUDE	10	
#define BLEND_PIX_FADE	11	

#define SCALE_POINT		0	
#define SCALE_LINER		1	
#define SCALE_HIGHT		2	

#define REVERSE_NORMAL		0x0	
#define REVERSE_W			0x1	
#define REVERSE_H			0x2	

typedef struct{
	short	w,h;
	short	grid_w,grid_h;
	short	pitch_w,pitch_h;

	DWORD	rect_size;
}VECTOR_BITMAP;

typedef struct{
	BYTE	b;
	BYTE	g;
	BYTE	r;
	BYTE	a;
}D3DD_ARGB;

typedef struct{
	unsigned	b:4;
	unsigned	g:4;
	unsigned	r:4;
	unsigned	a:4;
}D3DD_ARGB_4444;


typedef struct{
	int		psno;
	float	c0[4];
	float	c1[4];
}PIXEL_SHADER;


typedef struct{
	char		flag;
	char		alpha;
	char		src_alpha;
	char		fit;
	char		low_level;

	char		filename[16];

	short		w, h;
	short		grid_w, grid_h;
	short		pitch_w, pitch_h;

	short				render_flag;	
	D3DFORMAT			format;			
	LPDIRECT3DTEXTURE8	*texture;
	BYTE				*fade_txt;

	
	char		type;		
	short		index_no;	

	short		bw,bh;
}D3DD_TEXTURE;

typedef struct{
	char	flag;
	char	fsize;
	char	w,h;
	char	pw,ph;
	char	kage;
	char	*str;

	long	step;
	long	count;

	BOOL	step_wait;
	BOOL	wait;

	D3DD_ARGB	kcolor;	
	D3DD_ARGB	color;	


}D3DD_TEXT;





typedef struct{
	char		flag;		
	char		disp;		
	short		layer;		

	DWORD		group;

	int			digit;
	short		keta;

	BYTE		*str;

	char		type;		
	char		vnum;		

	char			ps_flag;
	PIXEL_SHADER	ps;

	DWORD		blend_param;	
	DWORD		scale_param;	
	DWORD		rev_param;		

	short		rander_no;	
	short		textue_no;	
	short		textue_no2;	
	short		tex2_param;		
	
	char		bright_no;	
	D3DD_ARGB	specular[4];	
	D3DD_ARGB	color[4];		

	float		kaiten;	
	float		cx, cy;		
	float		zoom_x;		
	float		zoom_y;		

	float		dx[4], dy[4];	
	float		dz;	

	float		rhw[4];			

	float		sx1[2], sy1[2];	
	float		sx2[2], sy2[2];	
}D3DD_DISP;

extern D3DD_TEXTURE	d3dTexture[D3DD_MAX_TEXTURE_AMOUNT];
extern D3DD_TEXT	d3dText[D3DD_MAX_TEXT_AMOUNT];
extern D3DD_DISP	d3dDisp[D3DD_MAX_DISP_AMOUNT];


extern void SetScaleParam( float t05f );

extern void D3DD_ResetAll( void );

extern void D3DD_SetPackDir( char *pac_dir );
extern char *D3DD_GetPackDir( void );
extern void D3DD_SetBackBuffer( int bbw, int bbh );

extern int D3DD_ReleasePixelShader( int psno );
extern int D3DD_LoadPixelShader( int psno, char *filename );

extern int	D3DD_ResetAllRenderTargetTexture( void );
extern int	D3DD_ResetAllTextur( void );

extern void D3DD_ResetAvi( int i );

extern int D3DD_ReleaseTexture( int tno );
extern int D3DD_CreateRenderTargetTexture( int tno, int w, int h );

extern int D3DD_UpdateAviTexture( int tno  );
extern int D3DD_WaitAviTexture( int tno  );
extern int D3DD_CreateAviTexture( int tno, char *fname );

extern void D3DD_CreateTable( void );

extern int D3DD_GetBackBuffer( int tno );
extern int D3DD_CreateTextTexture( int tno, char *str, int font_size, int tw, int th, int pw, int ph, int kage, int r, int g, int b, int kr, int kg, int kb );
extern int D3DD_UpdateTextTexture( int tno, char *str, int r, int g, int b, int kr, int kg, int kb );

extern int D3DD_LoadTexture( int tno, char *filename, int w, int h, int alpha, int fit=FALSE, int low_level=FALSE, int mip=FALSE, char *tfname=NULL, char *tfname2=NULL, int chroma=256, char *pak=NULL, int tex_size=D3DD_TEXTURE_CONTROL_SIZE );
extern int D3DD_LoadTextureBBM( int tno, char *filename, int alpha, int low_level=FALSE, int mip=FALSE, char *tfname=NULL, char *tfname2=NULL, int chroma=256, char *pak=NULL );

extern D3DD_TEXTURE *D3DD_GetTexture( int tno );








extern BOOL D3DD_SetGraphSprite( int dno, int ani_no, int sp_no, int mode, int layer, int disp, int end=TRUE, int lnum=0, int cfade=FALSE );

extern BOOL D3DD_SetSpriteSpeed( int dno, int speed=100 );
extern BOOL D3DD_SetSpritePlay( int dno, int ani_no, int sp_no, int mode, int end=TRUE, int lnum=0, int cfade=FALSE );
extern BOOL D3DD_SetSpriteNo( int dno, int sp_no );
extern BOOL D3DD_SetSpriteCfade( int dno, int flag );
extern BOOL D3DD_SetSpriteLoop( int dno, int end, int lnum );
extern BOOL D3DD_SetSpriteLoopThrough( int dno );
extern int  D3DD_GetSpriteRepeatFind( int dno );
extern BOOL D3DD_SetSpriteRepeatFind( int dno );
extern RECT *D3DD_GetSpriteRect( int dno );
extern BOOL D3DD_WaitSpriteCondition( int dno );
extern int  D3DD_GetSpriteMode( int dno );
extern BOOL D3DD_SetSpriteMode( int dno, int mode );
extern int D3DD_GetSpriteEvent( int dno );


extern void D3DD_ResetGraph( int dno );
extern void D3DD_SetGraph( int dno, int tno, int layer, int disp );
extern void D3DD_SetGraphx9( int dno, int tno, int layer, int disp );
extern void D3DD_SetGraphPrimitive( int dno, int vnum, int layer, int disp );
extern void D3DD_SetGraphCircle( int dno, int vnum, int paint, int layer, int disp );
extern void D3DD_SetGraphGrid( int dno, int grid, int layer, int disp );
extern void D3DD_SetGraphText( int dno, int tno, int layer, int disp );
extern void D3DD_SetGraphStr( int dno, int tno, int layer, int disp, char *str );
extern void D3DD_SetGraphStrChange( int dno, char *str );
extern void D3DD_SetGraphDigit( int dno, int tno, int layer, int disp, int digit, int keta );
extern void D3DD_SetGraphDigitNo( int dno, int digit );

extern BOOL D3DD_SetGraphTextStep( int dno, int step );
extern BOOL D3DD_SetGraphTextCount( int dno, int count );

extern void D3DD_SetGraphRenderNo( int dno, int rno );

extern void D3DD_SetGraphLayer( int dno, int layer );
extern void D3DD_SetGraphGroup( int dno, DWORD group );

extern void D3DD_SetGraphPointPos( int dno, int pno, float dx, float dy, float rhw );
extern void D3DD_SetGraphRect( int dno, float dx, float dy,  float sx, float sy, float w, float h );
extern void D3DD_SetGraphDstRect( int dno, float dx, float dy, float dw, float dh );
extern void D3DD_SetGraphSrcRect( int dno, float sx, float sy, float sw, float sh );
extern void D3DD_SetGraphSrc2Rect( int dno, float sx, float sy, float sw, float sh );
extern void D3DD_SetGraphDst( int dno, float dx1, float dy1, float dx2, float dy2, float dx3, float dy3, float dx4, float dy4 );
extern void D3DD_SetGraphRoll2( int dno, float cx, float cy, float dw, float dh, float count );
extern void D3DD_SetGraphDstMove( int dno, float mx, float my );
extern void D3DD_SetGraphDstMoveWH( int dno, float dw, float dh );

extern void D3DD_SetGraphCenter( int dno, float cx, float cy );
extern void D3DD_SetGraphRoll( int dno, float kaiten );
extern void D3DD_SetGraphZoom( int dno, float zoom );
extern void D3DD_SetGraphZoomXY( int dno, float zoom_x, float zoom_y );
extern void D3DD_SetGraphDisp( int dno, char disp );
extern void D3DD_SetGraphSrc( int dno, int tno );
extern void D3DD_SetGraphSrc2( int dno, int tno2, int param );
extern void D3DD_ResetGraphSrc2( int dno );
extern void D3DD_SetGraphPixelFade( int dno, int ps_no, float c0_alpha );
extern void D3DD_SetGraphPixelShader( int dno, int ps_no, int c, float r, float g, float b, float a );
extern void D3DD_SetGraphBlendParam( int dno, int bparam );
extern void D3DD_SetGraphScaleParam( int dno, int sparam );
extern void D3DD_SetGraphRevParam( int dno, int rparam );
extern void D3DD_SetGraphColor( int dno, int a, int r, int g, int b );
extern void D3DD_SetGraphColorNo( int dno, int cno, int a, int r, int g, int b );
extern void D3DD_SetGraphFade( int dno, int fade );
extern void D3DD_SetGraphColor2( int dno, int r, int g, int b );
extern void D3DD_SetGraphAlpha( int dno, int a );
extern void D3DD_SetGraphBaseColor( int dno, int r, int g, int b );
extern void D3DD_SetGraphSpecular( int dno, int sr, int sg, int sb );

extern void D3DD_SetGraphBrightNo( int dno, int bright_no );
extern void D3DD_SetGlobalColor( int r, int g, int b, int type, int no );


extern int D3DD_DrawTexture( D3DD_DISP *disp, D3DD_TEXTURE *exTex=NULL );

#define ALL_DISP_GROUP	0xffffffff

extern void D3DD_SetDispGroup( DWORD group );
extern void D3DD_AddDispGroup( DWORD group );
extern void D3DD_SubDispGroup( DWORD group );


extern int D3DD_Draw( int flash, int save=0 );
extern void D3DD_Present( RECT *src, RECT *dst );


#endif	