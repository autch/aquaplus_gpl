#include "STD_Windows.h"
#include "STD_Debug.h"
#include "STD_File.h"

#include "d3d_main.h"
#include "d3d_draw.h"
#include "d3d_Font.h"


#include "GlobalDraw.h"
#include "dispSprite.h"





void GD_ResetAll( void )
{
	D3DD_ResetAll();
	SPR_ReleaseSpriteAll();
}



void GD_SetPackDir( char *pac_dir )
{
	D3DD_SetPackDir( pac_dir );
}

void GD_SetWindowStyle( DWORD win, DWORD full )
{
	D3DM_SetWindowStyle( win, full );
}

int	GD_InitDevices( HWND hwnd, int w, int h, int full, HMENU menu )
{
	return D3DM_InitDevices( hwnd, w, h, full, menu );
}


int GD_TermDevices(void)
{
	return D3DM_TermDevices();
}


void GD_Draw( int flash, int save )
{
	D3DD_Draw( flash, save );
}


void GD_LastBlt( void )
{
	D3DD_Present( NULL, NULL );		
	D3DM_ChangeF2W( );	

}


void GD_DisplayChange( void )
{
	if(D3DMain.m_DisplayModeChange==2){
		D3DMain.m_DisplayModeChange=0;
	}else{
		D3DMain.m_DisplayModeChange=1;
	}
}

void GD_WindowModeChange( int mode )
{
	if( mode != D3DMain.m_WindowMode ){
		D3DMain.m_WindowModeChange=TRUE;
	}
}




int GD_ReleasePixelShader( int psno )
{
	return D3DD_ReleasePixelShader( psno );
}

int GD_LoadPixelShader( int psno, char *filename )
{
	return D3DD_LoadPixelShader( psno, filename );
}



int GD_ReleaseTexture( int tno )
{
	return D3DD_ReleaseTexture( tno );
}
int GD_CreateRenderTargetTexture( int tno, int w, int h )
{
	return D3DD_CreateRenderTargetTexture( tno, w, h );
}
int GD_WaitAviTexture( int tno  )
{

	return D3DD_WaitAviTexture( tno  );
}

int GD_CreateAviTexture( int tno, char *fname )
{
	return D3DD_CreateAviTexture( tno, fname );
}


int GD_CreateTextTexture( int tno, char *str, int font_size, int tw, int th, int pw, int ph, int kage, int r, int g, int b, int kr, int kg, int kb )
{
	return D3DD_CreateTextTexture( tno, str, font_size, tw, th, pw, ph, kage, r, g, b, kr, kg, kb );
}

int GD_UpdateTextTexture( int tno, char *str, int r, int g, int b, int kr, int kg, int kb )
{
	return D3DD_UpdateTextTexture( tno, str, r, g, b, kr, kg, kb );
}

int GD_GetBackBuffer( int tno )
{
	return D3DD_GetBackBuffer( tno );
}

int GD_LoadTexture( int tno, char *filename, int alpha, int low_level, int mip, char *tfname, char *tfname2, int chroma, char *pak, int tex_size )
{
	return D3DD_LoadTexture( tno, filename, -1, -1, alpha, FALSE, low_level, mip, tfname, tfname2, chroma, pak, tex_size );
}

int GD_LoadTextureBBM( int tno, char *filename, int alpha, int low_level, int mip, char *tfname, char *tfname2, int chroma, char *pak )
{
	return D3DD_LoadTextureBBM( tno, filename, alpha, low_level, mip, tfname, tfname2, chroma, pak );
}
int GD_GetTextureWH( int tno, int *w, int *h )
{
	D3DD_TEXTURE *d3dd_tex=NULL;
	
	d3dd_tex = D3DD_GetTexture( tno );
	*w = d3dd_tex->w;
	*h = d3dd_tex->h;
	return 1;
}
int GD_GetBmpWH( int tno, int *w, int *h )
{
	D3DD_TEXTURE *d3dd_tex=NULL;
	
	d3dd_tex = D3DD_GetTexture( tno );
	*w = d3dd_tex->bw;
	*h = d3dd_tex->bh;
	return 1;
}




int GD_LoadSprite( int ani_no, char *fname, int alpha, int low_level, int mip, char *tfname, char *tfname2, int chroma, char *pak )
{
	return SPR_LoadSprite(  ani_no, fname, tfname, tfname2, chroma, alpha, low_level, mip, pak );
}
int GD_ReleaseSprite( int ani_no )
{
	return SPR_ReleaseSprite( ani_no );
}

int GD_SetSprite( int dno, int ani_no, int sp_no, int mode, int layer, int disp, int end, int loop, int cfade )
{
	return D3DD_SetGraphSprite( dno, ani_no, sp_no, mode, layer, disp, end, loop, cfade );
}

BOOL GD_SetSpriteSpeed( int dno, int speed )
{
	return D3DD_SetSpriteSpeed( dno, speed );
}
BOOL GD_SetSpritePlay( int dno, int ani_no, int sp_no, int mode, int end, int lnum, int cfade )
{
	return D3DD_SetSpritePlay( dno, ani_no, sp_no, mode, end, lnum, cfade );
}
BOOL GD_SetSpriteNo( int dno, int sp_no )
{
	return D3DD_SetSpriteNo( dno, sp_no );
}
BOOL GD_SetSpriteCfade( int dno, int flag )
{
	return D3DD_SetSpriteCfade( dno, flag );
}
BOOL GD_SetSpriteLoop( int dno, int end, int lnum )
{
	return D3DD_SetSpriteLoop( dno, end, lnum );
}
BOOL GD_SetSpriteLoopThrough( int dno )
{
	return D3DD_SetSpriteLoopThrough( dno );
}

int  GD_GetSpriteRepeatFind( int dno )
{
	return D3DD_GetSpriteRepeatFind( dno );
}
BOOL GD_SetSpriteRepeatFind( int dno )
{
	return D3DD_SetSpriteRepeatFind( dno );
}

RECT *GD_GetSpriteRect( int dno )
{
	return D3DD_GetSpriteRect( dno );
}

BOOL GD_WaitSpriteCondition( int dno )
{
	return D3DD_WaitSpriteCondition( dno );
}

int  GD_GetSpriteMode( int dno )
{
	return D3DD_GetSpriteMode( dno );
}
BOOL GD_SetSpriteMode( int dno, int mode )
{
	return D3DD_SetSpriteMode( dno, mode );
}

int GD_GetSpriteEvent( int dno )
{
	return D3DD_GetSpriteEvent( dno );
}




void GDFNT_SetPackDir( char *pac_dir )
{
	D3DFNT_SetPackDir( pac_dir );
}

void GDFNT_ChangeFont( int ftype )
{
	D3DFNT_ChangeFont( ftype );
}
void GDFNT_LoadFont( int fsize, int ftype )
{
	D3DFNT_LoadFont( fsize, ftype );
}
void GDFNT_ReleaseFont( int fsize )
{
	D3DFNT_ReleaseFont( fsize );
}

void GDFNT_Init( void )
{
	D3DFNT_Init();
}
void GDFNT_Term( void )
{
	D3DFNT_Term();
}


void GD_ResetGraph( int dno )
{
	D3DD_ResetGraph( dno );
}
void GD_SetGraph( int dno, int tno, int layer, int disp )
{
	D3DD_SetGraph( dno, tno, layer, disp );
}
void GD_SetGraphx9( int dno, int tno, int layer, int disp )
{
	D3DD_SetGraphx9( dno, tno, layer, disp );
}

void GD_SetGraphPrimitive( int dno, int vnum, int layer, int disp )
{
	D3DD_SetGraphPrimitive( dno, vnum, layer, disp );
}
void GD_SetGraphCircle( int dno, int vnum, int paint, int layer, int disp )
{
	D3DD_SetGraphCircle( dno, vnum, paint, layer, disp );
}
void GD_SetGraphGrid( int dno, int grid, int layer, int disp )
{
	D3DD_SetGraphGrid( dno, grid, layer, disp );
}

void GD_SetGraphText( int dno, int tno, int layer, int disp )
{
	D3DD_SetGraphText( dno, tno, layer, disp );
}
void GD_SetGraphStr( int dno, int tno, int layer, int disp, char *str )
{
	D3DD_SetGraphStr( dno, tno, layer, disp, str );
}
void GD_SetGraphStrChange( int dno, char *str )
{
	D3DD_SetGraphStrChange( dno, str );
}


void GD_SetGraphDigit( int dno, int tno, int layer, int disp, int digit, int keta )
{
	D3DD_SetGraphDigit( dno, tno, layer, disp, digit, keta );
}
void GD_SetGraphDigitNo( int dno, int digit )
{
	D3DD_SetGraphDigitNo( dno, digit );
}


BOOL GD_SetGraphTextStep( int dno, int step )
{
	return D3DD_SetGraphTextStep( dno, step );
}
BOOL GD_SetGraphTextCount( int dno, int count )
{
	return D3DD_SetGraphTextCount( dno, count );
}

void GD_SetGraphRenderNo( int dno, int rno )
{
	D3DD_SetGraphRenderNo( dno, rno );
}
void DG_SetGraphBrightNo( int dno, int bright_no )
{
	D3DD_SetGraphBrightNo( dno, bright_no );
}
void GD_SetGraphLayer( int dno, int layer )
{
	D3DD_SetGraphLayer( dno, layer );
}

void GD_SetGraphGroup( int dno, DWORD group )
{
	D3DD_SetGraphGroup( dno, group );
}

void GD_SetGraphPointPos( int dno, int pno, float dx, float dy, float rhw )
{
	D3DD_SetGraphPointPos( dno, pno, dx, dy, rhw );
}
void GD_SetGraphRect( int dno, float dx, float dy,  float sx, float sy, float w, float h )
{
	D3DD_SetGraphRect( dno, dx, dy, sx, sy, w, h );
}
void GD_SetGraphDstRect( int dno, float dx, float dy, float dw, float dh )
{
	D3DD_SetGraphDstRect( dno, dx, dy, dw, dh );
}
void GD_SetGraphSrcRect( int dno, float sx, float sy, float sw, float sh )
{
	D3DD_SetGraphSrcRect( dno, sx, sy, sw, sh );
}
void GD_SetGraphSrc2Rect( int dno, float sx, float sy, float sw, float sh )
{
	D3DD_SetGraphSrc2Rect( dno, sx, sy, sw, sh );
}

void GD_SetGraphDst( int dno, float dx1, float dy1, float dx2, float dy2, float dx3, float dy3, float dx4, float dy4 )
{
	D3DD_SetGraphDst( dno, dx1, dy1, dx2, dy2, dx3, dy3, dx4, dy4 );
}
void GD_SetGraphRoll2( int dno, float cx, float cy, float dw, float dh, float count )
{
	D3DD_SetGraphRoll2( dno, cx, cy, dw, dh, count );
}

void GD_SetGraphDstMove( int dno, float mx, float my )
{
	D3DD_SetGraphDstMove( dno, mx, my );
}
void GD_SetGraphDstMoveWH( int dno, float dw, float dh )
{
	D3DD_SetGraphDstMoveWH( dno, dw, dh );
}

void GD_SetGraphCenter( int dno, float cx, float cy )
{
	D3DD_SetGraphCenter( dno, cx, cy );
}
void GD_SetGraphRoll( int dno, float kaiten )
{
	D3DD_SetGraphRoll( dno, kaiten );
}
void GD_SetGraphZoom( int dno, float zoom )
{
	D3DD_SetGraphZoom( dno, zoom );
}
void GD_SetGraphZoomXY( int dno, float zoom_x, float zoom_y )
{
	D3DD_SetGraphZoomXY( dno, zoom_x, zoom_y );
}

void GD_SetGraphDisp( int dno, char disp )
{
	D3DD_SetGraphDisp( dno, disp );
}
void GD_SetGraphSrc( int dno, int tno )
{
	D3DD_SetGraphSrc( dno, tno );
}

void GD_SetGraphSrc2( int dno, int tno2, int param )
{
	D3DD_SetGraphSrc2( dno, tno2, param );
}
void GD_ResetGraphSrc2( int dno )
{
	D3DD_ResetGraphSrc2( dno );
}
void GD_SetGraphPixelFade( int dno, float c0_alpha )
{
	D3DD_SetGraphPixelFade( dno, 0, c0_alpha );
}
void GD_SetGraphPixelShader( int dno, int ps_no, int c, float r, float g, float b, float a )
{
	D3DD_SetGraphPixelShader( dno, ps_no, c, r, g, b, a );
}
void GD_SetGraphBlendParam( int dno, int bparam )
{
	D3DD_SetGraphBlendParam( dno, bparam );
}

void GD_SetGraphScaleParam( int dno, int sparam )
{
	D3DD_SetGraphScaleParam( dno, sparam );
}
void GD_SetGraphRevParam( int dno, int rparam )
{
	D3DD_SetGraphRevParam( dno, rparam );
}
void GD_SetGraphColor( int dno, int a, int r, int g, int b )
{
	D3DD_SetGraphColor( dno, a, r, g, b );
}
void GD_SetGraphColorNo( int dno, int cno, int a, int r, int g, int b )
{
	D3DD_SetGraphColorNo( dno, cno, a, r, g, b );
}

void GD_SetGraphFade( int dno, int fade )
{
	D3DD_SetGraphFade( dno, fade );
}
void GD_SetGraphColor2( int dno, int r, int g, int b )
{
	D3DD_SetGraphColor2( dno, r,  g, b );
}
void GD_SetGraphAlpha( int dno, int a )
{
	D3DD_SetGraphAlpha(  dno,  a );
}
void GD_SetGraphBaseColor( int dno, int r, int g, int b )
{
	D3DD_SetGraphBaseColor( dno, r,  g,  b );
}
void GD_SetGraphSpecular( int dno, int sr, int sg, int sb )
{
	D3DD_SetGraphSpecular(  dno,  sr,  sg,  sb );
}

void GD_SetGraphBrightNo( int dno, int bright_no )
{
	D3DD_SetGraphBrightNo( dno, bright_no );
}

void GD_SetGlobalColor( int r, int g, int b, int type, int no )
{
	D3DD_SetGlobalColor( r, g, b, type, no );
}


extern void GD_SetDispGroup( DWORD group )
{
	D3DD_SetDispGroup( group );
}
extern void GD_AddDispGroup( DWORD group )
{
	D3DD_AddDispGroup( group );
}
extern void GD_SubDispGroup( DWORD group )
{
	D3DD_SubDispGroup( group );
}
