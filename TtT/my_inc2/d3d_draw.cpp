

#include <mbstring.h>
#include <windows.h>

#include "STD_Windows.h"
#include "STD_Debug.h"
#include "STD_File.h"

#include "Comp_pac.h"

#include "d3d_draw.h"
#include "d3d_main.h"
#include "d3d_font.h"
#include "d3d_text.h"

#include <tga.h>

#include <xvid_dec.h>

float T05F = 0.5f;

void SetScaleParam( float t05f )
{
	T05F = t05f;
}


#define RENDER_OFF		0	
#define RENDER_SET		1	
#define RENDER_RESET	2	

static int	BackSufaceSizeW=800;
static int	BackSufaceSizeH=600;

static DWORD	DitherSetting = D3DX_FILTER_DITHER;


#define BRIGHT_GROUP_MAX	8

typedef struct{
	unsigned char	r;
	unsigned char	g;
	unsigned char	b;
	unsigned char	type;
}BRIGHT_GROUP;

static BRIGHT_GROUP	GlobalColor[BRIGHT_GROUP_MAX]={
	{128,128,128,2},
	{128,128,128,2},
	{128,128,128,2},
	{128,128,128,2},

	{128,128,128,2},
	{128,128,128,2},
	{128,128,128,2},
	{128,128,128,2},
};



DWORD	PixelShader[16];

D3DD_TEXTURE	d3dTexture[D3DD_MAX_TEXTURE_AMOUNT];
D3DD_TEXT		d3dText[D3DD_MAX_TEXT_AMOUNT];
D3DD_DISP		d3dDisp[D3DD_MAX_DISP_AMOUNT];





static char	*PackD3DDDir=NULL;


void D3DD_ResetAll( void )
{
	int	i;
	
	for(i=0;i<D3DD_MAX_TEXTURE_AMOUNT;i++){
		D3DD_ReleaseTexture( i );
	}

	for(i=0;i<D3DD_MAX_DISP_AMOUNT;i++){
		D3DD_ResetGraph( i );
	}
}



void D3DD_SetPackDir( char *pac_dir )
{
	PackD3DDDir = pac_dir;
}
char *D3DD_GetPackDir( void )
{
	return PackD3DDDir;
}


void D3DD_SetBackBuffer( int bbw, int bbh )
{
	BackSufaceSizeW=bbw;
	BackSufaceSizeH=bbh;
}




int D3DD_ReleasePixelShader( int psno )
{
	if(PixelShader[psno]) pD3DDevice->DeletePixelShader( PixelShader[psno] );
	PixelShader[psno]=0;
	return TRUE; 
}

int D3DD_LoadPixelShader( int psno, char *filename )
{
	char	fname[MAX_PATH];
	BYTE	*buf=NULL;
	char	buf2[1024];
	int		size;

	if(PackD3DDDir)	wsprintf( fname, "%s\\%s", PackD3DDDir, filename );
	else			wsprintf( fname, "%s", filename );

	int	i=HIBYTE(D3DCapsStruct.m_d3dCaps.PixelShaderVersion);
	int	j=LOBYTE(D3DCapsStruct.m_d3dCaps.PixelShaderVersion);
	if( i>=1 ){
		LPD3DXBUFFER	ppCompiledShader=NULL;
		LPD3DXBUFFER	ppCompiledShaderDebug=NULL;


		size = PAC_LoadFile( PackD3DDDir, filename, &buf );
		if(size==0) {
			DebugBox( NULL,"ピクセルシェーダファイルが読み込めません。[%s]", fname );
			return FALSE;
		}
		D3DXAssembleShader( buf, size, 0, NULL, &ppCompiledShader, &ppCompiledShaderDebug);
		GFree(buf);

		if(PixelShader[psno]) pD3DDevice->DeletePixelShader( PixelShader[psno] );
		PixelShader[psno]=0;

		pD3DDevice->CreatePixelShader( (DWORD*)ppCompiledShader->GetBufferPointer(), &PixelShader[psno] );
		ppCompiledShader->Release();

		if(ppCompiledShaderDebug){
			strncpy( buf2, (char*)ppCompiledShaderDebug->GetBufferPointer(), ppCompiledShaderDebug->GetBufferSize() );
			buf2[ppCompiledShaderDebug->GetBufferSize()]='\0';
			ppCompiledShaderDebug->Release();
	
			MessageBox( NULL, buf2, "警告", MB_OK|MB_SETFOREGROUND );
		}
	}else{
		DebugPrintf( "PSがありません\n" );
		return FALSE;
	}
	return TRUE;
}




int	D3DD_ResetAllRenderTargetTexture( void )
{
	int	i,j;

	for(i=0;i<D3DD_MAX_TEXTURE_AMOUNT;i++){
		if( d3dTexture[i].render_flag==RENDER_SET ){
			d3dTexture[i].render_flag=RENDER_RESET;

			for(j=0;j<d3dTexture[i].pitch_w*d3dTexture[i].pitch_h;j++){
				RELEASE_3D( d3dTexture[i].texture[j] );
			}
		}
	}
	
	return TRUE;
}


int	D3DD_ResetAllTextur( void )
{
	int	i;
	for(i=0;i<D3DD_MAX_TEXTURE_AMOUNT;i++){
		if(d3dTexture[i].flag){
			if( d3dTexture[i].render_flag==RENDER_OFF ){
				if(d3dTexture[i].low_level){
					D3DD_LoadTexture( i, d3dTexture[i].filename, d3dTexture[i].w*2, d3dTexture[i].h*2, d3dTexture[i].alpha, d3dTexture[i].fit, d3dTexture[i].low_level );
				}else{
					D3DD_LoadTexture( i, d3dTexture[i].filename, d3dTexture[i].w, d3dTexture[i].h, d3dTexture[i].alpha, d3dTexture[i].fit, d3dTexture[i].low_level );
				}
			}
		}
	}
	return TRUE;
}

int D3DD_ReleaseTexture( int tno )
{
	int	i;
	if(d3dTexture[tno].flag){
		if(d3dTexture[tno].texture){
			for( i=0 ; i < d3dTexture[tno].pitch_h*d3dTexture[tno].pitch_w ; i++ ){
				RELEASE_3D( d3dTexture[tno].texture[i] );
			}
			GFree( d3dTexture[tno].texture );
		}
		GFree( d3dTexture[tno].fade_txt );

		if( d3dTexture[tno].type==GTYPE_TEXT ){
			GFree( d3dText[d3dTexture[tno].index_no].str );
			ZeroMemory( &d3dText[d3dTexture[tno].index_no], sizeof(D3DD_TEXT) );
		}
		if( d3dTexture[tno].type==GTYPE_MOVIE ){
			D3DD_ResetAvi( d3dTexture[tno].index_no );
		}
	}
	ZeroMemory( &d3dTexture[tno], sizeof(D3DD_TEXTURE) );
	return TRUE;
}

int D3DD_CheckTextureSize( int w, int h )
{
	if(w!=-1){

		if( (D3DD_TEXTURE_SIZE-1)*D3DD_TEXTURE_CONTROL_SIZE < w || (D3DD_TEXTURE_SIZE-1)*D3DD_TEXTURE_CONTROL_SIZE < h ){
			DebugBox( NULL, "現在の設定では、%d を超えるサイズのテクスチャーは生成できません", (D3DD_TEXTURE_SIZE-1)*D3DD_TEXTURE_CONTROL_SIZE );
			return 0;
		}
		if( w < D3DD_TEXTURE_CONTROL_SIZE ){
			if( GetBitCountDWORD(w)!=1 ){
				DebugBox( NULL, "%d を以下のサイズのテクスチャーでは、２のべき乗のサイズを指定してください［%d］",D3DD_TEXTURE_CONTROL_SIZE, w );
				return 0;
			}
		}else{
			if(w % D3DD_TEXTURE_CONTROL_SIZE){
				DebugBox( NULL, "%d を超えるサイズのテクスチャーでは、%dの倍数のサイズを指定してください［%d］",D3DD_TEXTURE_CONTROL_SIZE,D3DD_TEXTURE_CONTROL_SIZE,w );
				return 0;
			}
		}
		if( h < D3DD_TEXTURE_CONTROL_SIZE ){
			if( GetBitCountDWORD(h)!=1 ){
				DebugBox( NULL, "%d を以下のサイズのテクスチャーでは、２のべき乗のサイズを指定してください［%d］",D3DD_TEXTURE_CONTROL_SIZE, h );
				return 0;
			}
		}else{
			if(h % D3DD_TEXTURE_CONTROL_SIZE ){
				DebugBox( NULL, "%d を超えるサイズのテクスチャーでは、%dの倍数のサイズを指定してください［%d］",D3DD_TEXTURE_CONTROL_SIZE,D3DD_TEXTURE_CONTROL_SIZE,h );
				return 0;
			}
		}
	}
	return 1;
}




int D3DD_CreateRenderTargetTexture( int tno, int w, int h )
{
	BOOL			ret=0;
	D3DD_TEXTURE	*d3dTex;

	if( D3DCapsStruct.m_ttCaps.m_A8R8G8B8 ){
		if( (int)D3DCapsStruct.m_d3dCaps.MaxTextureWidth < w || (int)D3DCapsStruct.m_d3dCaps.MaxTextureHeight < h ){
			DebugBox( NULL, "使用できるテクスチャの幅を超えています。[レンダリングターゲット %d,%d pixel]", w, h );
			return 0;
		}
		if( GetBitCountDWORD(w)!=1 ){ DebugBox( NULL, "２のべき乗のサイズを指定してください［%d］", w ); return 0; }
		if( GetBitCountDWORD(h)!=1 ){ DebugBox( NULL, "２のべき乗のサイズを指定してください［%d］", h ); return 0; }

		D3DD_ReleaseTexture( tno );
		d3dTex = &d3dTexture[tno];

		d3dTex->flag = TRUE;
		d3dTex->alpha = 1;
		d3dTex->fit = 1;
		d3dTex->low_level = FALSE;
		d3dTex->format = D3DFMT_A8R8G8B8;
		d3dTex->texture = (LPDIRECT3DTEXTURE8*)GAlloc( sizeof(LPDIRECT3DTEXTURE8) );
		d3dTex->render_flag=RENDER_RESET;
		d3dTex->w = w;
		d3dTex->h = h;
		d3dTex->pitch_w = 1;
		d3dTex->pitch_h = 1;
		d3dTex->grid_w = w;
		d3dTex->grid_h = h;
		ret=TRUE;
	}else{
		DebugBox( NULL, "A8R9G9B9フォーマットのレンダリングターゲットテクスチャーに対応していません。" );
	}
	return ret;
}



#define D3DD_MAX_AVI_AMOUNT		8

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
}D3DD_AVI;

D3DD_AVI		d3dAvi[D3DD_MAX_AVI_AMOUNT];

struct Chunk{
	FOURCC	ckID;
	DWORD	size;
	FOURCC	type;
};


#include <stdio.h>
#include <Aviriff.h>

void D3DD_ResetAvi( int i )
{
	if(d3dAvi[i].flag){
		if(d3dAvi[i].fpr) fclose(d3dAvi[i].fpr);
		d3dAvi[i].Xvid.Clear_XviD();
		GFree( d3dAvi[i].SrcBuf );
		GFree( d3dAvi[i].DstBuf );
	}

	ZeroMemory( &d3dAvi[i], sizeof(D3DD_AVI) );
}



int D3DD_SetAviStart( char *fname, int *w, int *h, D3DFORMAT format )
{
	int	i;
	for( i=0 ; i <D3DD_MAX_AVI_AMOUNT ; i++ ){
		if(!d3dAvi[i].flag){
			break;
		}
	}
	if(i==D3DD_MAX_AVI_AMOUNT) return D3DD_MAX_AVI_AMOUNT;
	D3DD_ResetAvi(i);

	d3dAvi[i].fpr = fopen(fname,"rb");
	if(NULL==d3dAvi[i].fpr)return D3DD_MAX_AVI_AMOUNT;

	Chunk		aviFileHead;
	Chunk		aviInfo;
	AVIMAINHEADER aviMainHead;

	fread(&aviFileHead,sizeof(Chunk),1,d3dAvi[i].fpr);	
	if(MAKEFOURCC('R','I','F','F')!=aviFileHead.ckID){
		fclose(d3dAvi[i].fpr);
		d3dAvi[i].fpr = NULL;
		return FALSE;
	}
	fread(&aviInfo,sizeof(Chunk),1,d3dAvi[i].fpr);
	fread(&aviMainHead,sizeof(AVIMAINHEADER),1,d3dAvi[i].fpr);


	d3dAvi[i].flag = TRUE;
	d3dAvi[i].fpms = aviMainHead.dwMicroSecPerFrame;
	d3dAvi[i].w = *w = aviMainHead.dwWidth;
	d3dAvi[i].h = *h = aviMainHead.dwHeight;
	d3dAvi[i].buffer_size = d3dAvi[i].w*d3dAvi[i].h*2;

	d3dAvi[i].SrcBuf = (BYTE*)GAlloc( d3dAvi[i].buffer_size );

	BOOL	dv_ret=0;
	switch(format){
		case D3DFMT_R8G8B8:
			d3dAvi[i].pitch = 3;
			dv_ret=d3dAvi[i].Xvid.Start_XviD( d3dAvi[i].w, d3dAvi[i].h, XVID_CSP_BGR );
			break;
		case D3DFMT_X8R8G8B8:	case D3DFMT_A8R8G8B8:
			d3dAvi[i].pitch = 3;
			dv_ret=d3dAvi[i].Xvid.Start_XviD( d3dAvi[i].w, d3dAvi[i].h, XVID_CSP_BGR );


			break;
		case D3DFMT_R5G6B5:
			d3dAvi[i].pitch = 2;
			dv_ret=d3dAvi[i].Xvid.Start_XviD( d3dAvi[i].w, d3dAvi[i].h, XVID_CSP_RGB565 );
			break;
		case D3DFMT_X1R5G5B5:	case D3DFMT_A1R5G5B5:
			d3dAvi[i].pitch = 2;
			dv_ret=d3dAvi[i].Xvid.Start_XviD( d3dAvi[i].w, d3dAvi[i].h, XVID_CSP_RGB555 );
			break;
	}
	if(!dv_ret){
		DebugBox( NULL, "XVIDの初期化に失敗しました" );
	}
	d3dAvi[i].DstBuf = (BYTE*)GAlloc( d3dAvi[i].w*d3dAvi[i].h*d3dAvi[i].pitch );
	d3dAvi[i].read_size = fread( d3dAvi[i].SrcBuf, 1, d3dAvi[i].buffer_size, d3dAvi[i].fpr );
	d3dAvi[i].start_time = timeGetTime();
	d3dAvi[i].frame_cnt = 0;
	return i;
}

int D3DD_WaitAviTexture( int tno  )
{
	return d3dAvi[d3dTexture[tno].index_no].flag;
}

int D3DD_UpdateAviTexture( int tno  )
{
	DWORD			dither_setting=0;
	D3DD_TEXTURE	*d3dTex;
	int				i, skiped=FALSE;
	d3dTex = &d3dTexture[tno];
	i = d3dTex->index_no;

	if(d3dAvi[i].flag){
		DWORD play_time = DWORD(d3dAvi[i].start_time+d3dAvi[i].frame_cnt*d3dAvi[i].fpms/1000);
		DWORD now_time = timeGetTime();

		if( play_time<=now_time ){
			if( play_time+d3dAvi[i].fpms/1000<=now_time && d3dAvi[i].skip_cnt<10 ){
				d3dAvi[i].Xvid.SetSkip(TRUE);
				d3dAvi[i].skip_cnt++;
				skiped = TRUE;
			}else{
				d3dAvi[i].Xvid.SetSkip(FALSE);
				d3dAvi[i].skip_cnt=0;
				skiped = FALSE;
			}

			d3dAvi[i].frame_cnt++;
			switch( d3dAvi[i].Xvid.DecodeXviD( d3dAvi[i].SrcBuf, d3dAvi[i].read_size, &d3dAvi[i].rest, d3dAvi[i].DstBuf, d3dAvi[i].w*d3dAvi[i].pitch ) ){
				case XviD_DEC_ERR:		
					D3DD_ResetAvi( i );
					break;
				case XviD_DEC_END:		
					D3DD_ResetAvi( i );
					break;
				case XviD_DEC_CONTINUE:	
					i=i;
					break;
				case XviD_DEC_NEEDMORE:	
					d3dAvi[i].read_size = fread( d3dAvi[i].SrcBuf + d3dAvi[i].rest, 1, d3dAvi[i].buffer_size-d3dAvi[i].rest, d3dAvi[i].fpr );
					break;
			}

			if(d3dAvi[i].flag && skiped==FALSE){
				int		x,y;
				int	ww = d3dTex->grid_w;
				int	hh = d3dTex->grid_w;
				int	sw = d3dAvi[i].w;
				int	sh = d3dAvi[i].h;

				for(y=0;y<d3dTex->pitch_h;y++){
					for(x=0;x<d3dTex->pitch_w;x++){
						LPDIRECT3DSURFACE8	work_surface;
						RECT	srect;
						if( D3D_OK==d3dTex->texture[y*d3dTex->pitch_w+x]->GetSurfaceLevel( 0, &work_surface ) ){
							SetRect( &srect, x*ww, y*hh, min((x+1)*ww,sw), min((y+1)*hh,sh) );
							if(d3dAvi[i].pitch==3){
								D3DXLoadSurfaceFromMemory( work_surface, NULL, NULL, d3dAvi[i].DstBuf, D3DFMT_R8G8B8, sw*d3dAvi[i].pitch, NULL, &srect, D3DX_FILTER_NONE|dither_setting|D3DX_FILTER_MIRROR, 0 );
							}else{
								D3DXLoadSurfaceFromMemory( work_surface, NULL, NULL, d3dAvi[i].DstBuf, d3dTex->format, sw*d3dAvi[i].pitch, NULL, &srect, D3DX_FILTER_NONE|dither_setting|D3DX_FILTER_MIRROR, 0 );
							}
							work_surface->Release();
						}
					}
				}
			}
		}
	}
	return skiped;
}


int D3DD_CreateAviTexture( int tno, char *fname )
{
	int				i,j;
	BOOL			ret=0;
	D3DD_TEXTURE	*d3dTex;
	D3DFORMAT		format=D3DFMT_R8G8B8;
	int				w,h;


	if( (D3DCapsStruct.m_NowDisplayMode.Format==D3DFMT_X8R8G8B8 || D3DCapsStruct.m_NowDisplayMode.Format==D3DFMT_A8R8G8B8) ){
		if( D3DCapsStruct.m_ttCaps.m_R8G8B8 )		format = D3DFMT_R8G8B8;
		else if( D3DCapsStruct.m_ttCaps.m_X8R8G8B8 )	format = D3DFMT_X8R8G8B8;
		else if( D3DCapsStruct.m_ttCaps.m_A8R8G8B8 )	format = D3DFMT_A8R8G8B8;
		else	DebugBox( NULL, "適正なテクスチャーフォーマットが存在しません" );
	}else{
		if( D3DCapsStruct.m_ttCaps.m_R5G6B5 )			format = D3DFMT_R5G6B5;
		else if( D3DCapsStruct.m_ttCaps.m_X1R5G5B5 )	format = D3DFMT_X1R5G5B5;
		else if( D3DCapsStruct.m_ttCaps.m_A1R5G5B5 )	format = D3DFMT_A1R5G5B5;
		else	DebugBox( NULL, "適正なテクスチャーフォーマットが存在しません" );
	}

	i = D3DD_SetAviStart( fname, &w, &h, format );
	if(i==D3DD_MAX_AVI_AMOUNT) return FALSE;

	if( w<=32 ){
		if     ( w<=16 )	w=16;
		else				w=32;
	}else{
		w = (w+63)/64*64;
	}
	if( h<=32 ){
		if     ( h<=16 )	h=16;
		else				h=32;
	}else{
		h = (h+63)/64*64;
	}
	int	ww,hh;
	hh = ww = min( 64, (w>h)?h:w );

	if( (D3DD_TEXTURE_SIZE-1)*64 < w || (D3DD_TEXTURE_SIZE-1)*64 < h ){
		DebugBox( NULL, "現在の設定では、%d を超えるサイズのテクスチャーは生成できません", (D3DD_TEXTURE_SIZE-1)*64 );
		return 0;
	}

	D3DD_ReleaseTexture( tno );
	d3dTex = &d3dTexture[tno];

	d3dTex->alpha = ALPHA_1555;
	d3dTex->fit   = FALSE;
	d3dTex->low_level = FALSE;
	d3dTex->type     = GTYPE_MOVIE;
	d3dTex->index_no = i;
	d3dTex->format = format;

	HRESULT	d3d_ret;

	d3dTex->pitch_w = (w/ww);
	d3dTex->pitch_h = (h/hh);
	if( d3dTex->pitch_w>16 || d3dTex->pitch_h>16 ) DebugBox(NULL,"");
	d3dTex->texture = (LPDIRECT3DTEXTURE8*)GAlloc( sizeof(LPDIRECT3DTEXTURE8) * d3dTex->pitch_w * d3dTex->pitch_h );

	for( i=0 ; i < d3dTex->pitch_h ; i++ ){
		for( j=0 ; j < d3dTex->pitch_w ; j++ ){
			d3d_ret = D3DXCreateTexture( pD3DDevice, ww, hh, 1, 0, d3dTex->format, D3DPOOL_MANAGED, &d3dTex->texture[i*d3dTex->pitch_w+j] );
			if( d3d_ret != D3D_OK ) break;
		}
		if( d3d_ret != D3D_OK ) break;
	}
	switch( d3d_ret ){
		default:	DebugBox( NULL, "なんかエラーでた。" );
		case D3D_OK:
			d3dTex->flag = TRUE;

			d3dTex->w = w;
			d3dTex->h = h;
			d3dTex->grid_w = min(ww,w);
			d3dTex->grid_h = min(hh,h);

			ret=TRUE;
			break;
		case D3DERR_NOTAVAILABLE:	DebugBox( NULL, "このデバイスは、照会されたテクニックをサポートしていない。" );	break;
		case D3DERR_OUTOFVIDEOMEMORY:DebugBox( NULL, "Direct3D が処理を行うのに十分なディスプレイ メモリがない。" );	break;
		case D3DERR_INVALIDCALL:	DebugBox( NULL, "メソッドの呼び出しが無効である。たとえば、メソッドのパラメータに無効な値が設定されている場合など。" );	break;
		case E_OUTOFMEMORY:			DebugBox( NULL, "Direct3D が呼び出しを完了するための十分なメモリを割り当てることができなかった。" );break;
	}
	return TRUE;
}


int D3DD_UpdateTextTexture( int tno, char *str, int r, int g, int b, int kr, int kg, int kb )
{
	int				txno;
	D3DD_TEXTURE	*d3dTex = &d3dTexture[tno];

	if( d3dTex->type != GTYPE_TEXT ) return 0;
	txno = d3dTex->index_no;
	if( d3dText[txno].flag == 0 ) return 0;

	if( r!=-1 ){
		d3dText[txno].color.r=r;
		d3dText[txno].color.g=g;
		d3dText[txno].color.b=b;
	}
	if( kr!=-1 ){
		d3dText[txno].kcolor.r=kr;
		d3dText[txno].kcolor.g=kg;
		d3dText[txno].kcolor.b=kb;
	}

	int				x,y;
	D3DLOCKED_RECT	d3dlr;
	for(y=0;y<d3dTex->pitch_h;y++){
		for(x=0;x<d3dTex->pitch_w;x++){
			if( d3dTex->texture[y*d3dTex->pitch_w+x]->LockRect( 0, &d3dlr, NULL, D3DLOCK_NOSYSLOCK )==D3D_OK ){
				ZeroMemory( d3dlr.pBits, d3dlr.Pitch*d3dTex->grid_h );
				d3dTex->texture[y*d3dTex->pitch_w+x]->UnlockRect(0);
			}
		}
	}
	GFree( d3dText[txno].str );
	if(str){
		d3dText[txno].str = (char *)GAlloc( strlen(str)+1 );
		strcpy( d3dText[txno].str, str );

		d3dText[txno].step      = -1;
		d3dText[txno].count     = -1;
		d3dText[txno].step_wait = 0;
		d3dText[txno].wait      = 0;

		D3DTXT_DrawText( &d3dTexture[tno], 0, d3dText[txno].ph*2, d3dText[txno].w, d3dText[txno].h,
			d3dText[txno].pw, d3dText[txno].ph, d3dText[txno].fsize, d3dText[txno].str, d3dText[txno].kage, 
			d3dText[txno].color.r,  d3dText[txno].color.g,  d3dText[txno].color.b,
			d3dText[txno].kcolor.r, d3dText[txno].kcolor.g, d3dText[txno].kcolor.b );
	}else{
		d3dText[txno].str = NULL;
	}

	return TRUE;
}


int D3DD_CreateTextTexture( int tno, char *str, int font_size, int tw, int th, int pw, int ph, int kage, int r, int g, int b, int kr, int kg, int kb )
{
	int				i,j;
	BOOL			ret=0;
	D3DD_TEXTURE	*d3dTex;
	int				w,h,txno;

	for(txno=0;txno<D3DD_MAX_TEXT_AMOUNT;txno++)
		if( d3dText[txno].flag==0 ) break;
	if(txno==D3DD_MAX_TEXT_AMOUNT) { DebugBox( NULL,"文字列用バッファが足りません[%d]", D3DD_MAX_TEXT_AMOUNT ); return 0; }

	w = (tw+1)*(font_size+pw);
	if( w<=32 ){
		if     ( w<=16 )	w=16;
		else				w=32;



	}else{

		w = (w+63)/64*64;
	}
	h = th*(font_size+ph)+ph*2;
	if( h<=32 ){
		if     ( h<=16 )	h=16;
		else				h=32;



	}else{
		h = (h+63)/64*64;
	}



	int	ww,hh;
	hh = ww = min( 64, (w>h)?h:w );

	D3DD_ReleaseTexture( tno );
	d3dTex = &d3dTexture[tno];

	GFree(d3dText[txno].str);
	d3dTex->alpha = ALPHA_4444;
	d3dTex->fit = FALSE;
	d3dTex->low_level = FALSE;
	d3dTex->type     = GTYPE_TEXT;
	d3dTex->index_no = txno;

	if( D3DCapsStruct.m_ttCaps.m_A4R4G4B4 )			d3dTex->format = D3DFMT_A4R4G4B4;
	else if( D3DCapsStruct.m_ttCaps.m_A8R8G8B8 )	d3dTex->format = D3DFMT_A8R8G8B8;
	else{
		DebugBox( NULL, "このビデオカードはアルファ付きテクスチャに対応していません" );
		return 0;
	}

	HRESULT	d3d_ret;

	d3dTex->pitch_w = (w/ww);
	d3dTex->pitch_h = (h/hh);
	if( d3dTex->pitch_w>16 || d3dTex->pitch_h>16 ) DebugBox(NULL,"");
	d3dTex->texture = (LPDIRECT3DTEXTURE8*)GAlloc( sizeof(LPDIRECT3DTEXTURE8) * d3dTex->pitch_w * d3dTex->pitch_h );

	for( i=0 ; i < d3dTex->pitch_h ; i++ ){
		for( j=0 ; j < d3dTex->pitch_w ; j++ ){
			d3d_ret = D3DXCreateTexture( pD3DDevice, ww, hh, 1, 0, d3dTex->format, D3DPOOL_MANAGED, &d3dTex->texture[i*d3dTex->pitch_w+j] );
			if( d3d_ret != D3D_OK ) break;
		}
		if( d3d_ret != D3D_OK ) break;
	}

	switch( d3d_ret ){
		default:	DebugBox( NULL, "なんかエラーでた。" );
		case D3D_OK:
			d3dTex->flag = TRUE;

			d3dTex->w = w;
			d3dTex->h = h;
			d3dTex->grid_w = min(ww,w);
			d3dTex->grid_h = min(hh,h);

			d3dText[txno].flag  = TRUE;
			d3dText[txno].w     = tw;
			d3dText[txno].h     = th;
			d3dText[txno].pw    = pw;
			d3dText[txno].ph    = ph;
			d3dText[txno].fsize = font_size;
			d3dText[txno].kage  = kage;

			d3dText[txno].kcolor.r = kr;	
			d3dText[txno].kcolor.g = kg;	
			d3dText[txno].kcolor.b = kb;	
			d3dText[txno].color.r  = r;	
			d3dText[txno].color.g  = g;	
			d3dText[txno].color.b  = b;	

			if(str){
				d3dText[txno].str   = (char *)GAlloc( strlen(str)+1 );
				strcpy( d3dText[txno].str, str );
				d3dText[txno].step    = -1;
				d3dText[txno].count   = -1;
				d3dText[txno].step_wait = 0;
				d3dText[txno].wait      = 0;

				D3DTXT_DrawText( &d3dTexture[tno], 0, d3dText[txno].ph*2, d3dText[txno].w, d3dText[txno].h,
					d3dText[txno].pw, d3dText[txno].ph, d3dText[txno].fsize, d3dText[txno].str, d3dText[txno].kage, r, g, b, kr, kg, kb );

			}else{
				d3dText[txno].str = NULL;
				d3dText[txno].step    = -1;
				d3dText[txno].count   = -1;
				d3dText[txno].step_wait = 0;
				d3dText[txno].wait      = 0;
			}

			ret=TRUE;
			break;
		case D3DERR_NOTAVAILABLE:	DebugBox( NULL, "このデバイスは、照会されたテクニックをサポートしていない。" );	break;
		case D3DERR_OUTOFVIDEOMEMORY:DebugBox( NULL, "Direct3D が処理を行うのに十分なディスプレイ メモリがない。" );	break;
		case D3DERR_INVALIDCALL:	DebugBox( NULL, "メソッドの呼び出しが無効である。たとえば、メソッドのパラメータに無効な値が設定されている場合など。" );	break;
		case E_OUTOFMEMORY:			DebugBox( NULL, "Direct3D が呼び出しを完了するための十分なメモリを割り当てることができなかった。" );break;
	}
	return TRUE;
}




BYTE	BlendTable[257][257];	
void D3DD_CreateTable( void )
{
	int	i,j;

	
	for( i=0 ; i<257 ; i++ ){
		for( j=0 ; j<257 ; j++ ){
			BlendTable[i][j]  = LIM( (i*j)>>8, 0, 255 );
		}
	}
}


BOOL BMP_SetChroma( RGBA *bmp, int size, int chroma, int rgb_rev=FALSE  )
{
	int	i;
	if(chroma==256) return TRUE;
	if(chroma==0){
		if(rgb_rev){
			for( i=0 ; i<size ; i++){
				bmp[i].red   =
				bmp[i].green =
				bmp[i].blue  = (bmp[i].blue*77 + bmp[i].green*151 + bmp[i].red*28)>>8;
			}
		}else{
			for( i=0 ; i<size ; i++){
				bmp[i].red   =
				bmp[i].green =
				bmp[i].blue  = (bmp[i].red*77 + bmp[i].green*151 + bmp[i].blue*28)>>8;
			}
		}
	}else{
		BYTE	*chromatabl1 = BlendTable[255-chroma];
		BYTE	*chromatabl2 = BlendTable[chroma];
		int		glay;

		if(rgb_rev){
			for( i=0 ; i<size ; i++){
				glay = chromatabl1[ (bmp[i].blue*77 + bmp[i].green*151 + bmp[i].red*28)>>8 ];
				bmp[i].red   = glay + chromatabl2[bmp[i].red  ];
				bmp[i].green = glay + chromatabl2[bmp[i].green];
				bmp[i].blue  = glay + chromatabl2[bmp[i].blue ];
			}
		}else{
			for( i=0 ; i<size ; i++){
				glay = chromatabl1[ (bmp[i].red*77 + bmp[i].green*151 + bmp[i].blue*28)>>8 ];
				bmp[i].red   = glay + chromatabl2[bmp[i].red  ];
				bmp[i].green = glay + chromatabl2[bmp[i].green];
				bmp[i].blue  = glay + chromatabl2[bmp[i].blue ];
			}
		}
	}
	return TRUE;
}
BOOL BMP_SetChroma( RGB_ *bmp, int size, int chroma, int rgb_rev=FALSE  )
{
	int	i;
	if(chroma==256) return TRUE;
	if(chroma==0){
		for( i=0 ; i<size ; i++){
			bmp[i].red   =
			bmp[i].green =
			bmp[i].blue  = (bmp[i].red*77 + bmp[i].green*151 + bmp[i].blue*28)>>8;
		}
	}else{
		BYTE	*chromatabl1 = BlendTable[255-chroma];
		BYTE	*chromatabl2 = BlendTable[chroma];
		int		glay;

		for( i=0 ; i<size ; i++){
			glay = chromatabl1[ (bmp[i].red*77 + bmp[i].green*151 + bmp[i].blue*28)>>8 ];
			bmp[i].red   = glay + chromatabl2[bmp[i].red  ];
			bmp[i].green = glay + chromatabl2[bmp[i].green];
			bmp[i].blue  = glay + chromatabl2[bmp[i].blue ];
		}
	}
	return TRUE;
}




BYTE	*ToneCourve=NULL;

int BMP_LoadTonecurve( char *fname, char *pak )
{
	int		size;

	GFree(ToneCourve);
	if(fname==NULL) return 0;
	if(fname[0]=='\0')	return FALSE;
	if(!pak) pak=PackD3DDDir;

	if( !(size=PAC_LoadFile( pak, fname, &ToneCourve)) ){
		DebugBox( NULL, "トーンカーブファイル[%s]がありません", fname );
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


BOOL BMP_SetTonecurve( RGBA *bmp, int size2, char *fname, char *pak, int rgb_rev=FALSE )
{
	int		i;
	BYTE	*rgb, *r, *g, *b;

	if(fname==NULL)	return FALSE;
	if(fname[0]=='\0')	return FALSE;
	if(bmp==NULL)	return FALSE;
	int		size = BMP_LoadTonecurve( fname, pak );
	
	if( !size )	return FALSE;
	if(size==1280){
		rgb = ToneCourve;
		if(rgb_rev){
			r = ToneCourve+768;
			g = ToneCourve+512;
			b = ToneCourve+256;
		}else{
			r = ToneCourve+256;
			g = ToneCourve+512;
			b = ToneCourve+768;
		}

		for( i=0 ; i<size2 ; i++){
			bmp[i].red   = rgb[ r[ bmp[i].red   ] ];
			bmp[i].green = rgb[ g[ bmp[i].green ] ];
			bmp[i].blue  = rgb[ b[ bmp[i].blue  ] ];
		}
	}else if(size==768){
		if(rgb_rev){
			r = ToneCourve+512;
			g = ToneCourve+256;
			b = ToneCourve;
		}else{
			r = ToneCourve;
			g = ToneCourve+256;
			b = ToneCourve+512;
		}

		for( i=0 ; i<size2 ; i++){
			bmp[i].red   = r[ bmp[i].red   ];
			bmp[i].green = g[ bmp[i].green ];
			bmp[i].blue  = b[ bmp[i].blue  ];
		}
	}else if(size==256){
		rgb = ToneCourve;

		for( i=0 ; i<size2 ; i++){
			bmp[i].red   = rgb[ bmp[i].red   ];
			bmp[i].green = rgb[ bmp[i].green ];
			bmp[i].blue  = rgb[ bmp[i].blue  ];
		}
	}
	
	BMP_ReleaseTonecurve();
	return TRUE;
}

BOOL BMP_SetTonecurve( RGB_ *bmp, int size2, char *fname, char *pak, int rgb_rev=FALSE )
{
	int		i;
	BYTE	*rgb, *r, *g, *b;

	if(fname==NULL)	return FALSE;
	if(fname[0]=='\0')	return FALSE;
	if(bmp==NULL)	return FALSE;
	int		size = BMP_LoadTonecurve( fname, pak );

	if( !size )	return FALSE;
	if(size==1280){
		rgb = ToneCourve;
		if(rgb_rev){
			r = ToneCourve+256;
			g = ToneCourve+512;
			b = ToneCourve+768;
		}else{
			r = ToneCourve+768;
			g = ToneCourve+512;
			b = ToneCourve+256;
		}

		for( i=0 ; i<size2 ; i++){
			bmp[i].red   = rgb[ r[ bmp[i].red   ] ];
			bmp[i].green = rgb[ g[ bmp[i].green ] ];
			bmp[i].blue  = rgb[ b[ bmp[i].blue  ] ];
		}
	}else if(size==768){
		if(rgb_rev){
			r = ToneCourve;
			g = ToneCourve+256;
			b = ToneCourve+512;
		}else{
			r = ToneCourve+512;
			g = ToneCourve+256;
			b = ToneCourve;
		}

		for( i=0 ; i<size2 ; i++){
			bmp[i].red   = r[ bmp[i].red   ];
			bmp[i].green = g[ bmp[i].green ];
			bmp[i].blue  = b[ bmp[i].blue  ];
		}
	}else if(size==256){
		rgb = ToneCourve;

		for( i=0 ; i<size2 ; i++){
			bmp[i].red   = rgb[ bmp[i].red   ];
			bmp[i].green = rgb[ bmp[i].green ];
			bmp[i].blue  = rgb[ bmp[i].blue  ];
		}
	}

	BMP_ReleaseTonecurve();
	return TRUE;
}



int D3DD_GetBackBuffer( int tno )
{
	BOOL			ret=0;
	IDirect3DSurface8	*back_buffer;
	static BYTE			*buf10=NULL;
	D3DLOCKED_RECT	d3dlr;
	HRESULT			d3d_ret;
	D3DD_TEXTURE	*d3dTex;

	IDirect3DSurface8	*work_surface;
	int	ww,hh;
	int	sw,sh;
	int		bpp, i,j, w,h;

	if( pD3DDevice->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &back_buffer )==D3D_OK ){
		if( back_buffer->LockRect( &d3dlr, NULL, D3DLOCK_NOSYSLOCK|D3DLOCK_READONLY  )==D3D_OK ){
			buf10 = (BYTE*)GAlloc(d3dlr.Pitch*BackSufaceSizeH);
			CopyMemory( (BYTE*)buf10, (BYTE*)d3dlr.pBits, d3dlr.Pitch*BackSufaceSizeH );
			back_buffer->UnlockRect();
		}
		back_buffer->Release();
	}
	if(buf10==NULL) return ret;

	D3DD_ReleaseTexture( tno );
	d3dTex = &d3dTexture[tno];

	d3dTex->alpha = ALPHA_1555;
	d3dTex->fit = FALSE;
	d3dTex->low_level = FALSE;


	sw  = BackSufaceSizeW;
	sh  = BackSufaceSizeH;
	d3dTex->format = D3DCapsStruct.m_NowDisplayMode.Format;
	if(d3dTex->format==D3DFMT_A8R8G8B8 || d3dTex->format==D3DFMT_X8R8G8B8){
		bpp = 32;
		sw  = d3dlr.Pitch/4;
	}else{
		bpp = 16;
		sw  = d3dlr.Pitch/2;
	}

	if( sw<=128 ){
		if     ( sw<=16 )	w=16;
		else if( sw<=32 )	w=32;
		else if( sw<=64 )	w=64;
		else				w=128;
	}else{
		w = (sw+256-1)/256*256;
	}
	if( sh<=128 ){
		if     ( sh<=16 )	h=16;
		else if( sh<=32 )	h=32;
		else if( sh<=64 )	h=64;
		else				h=128;
	}else{
		h = (sh+256-1)/256*256;
	}

	hh = ww = min( 256, (w>h)?h:w );

	d3dTex->pitch_w = (w/ww);
	d3dTex->pitch_h = (h/hh);
	if( d3dTex->pitch_w>16 || d3dTex->pitch_h>16 ) DebugBox(NULL,"");
	d3dTex->texture = (LPDIRECT3DTEXTURE8*)GAlloc( sizeof(LPDIRECT3DTEXTURE8) * d3dTex->pitch_w * d3dTex->pitch_h );

	for( i=0 ; i < d3dTex->pitch_h ; i++ ){
		for( j=0 ; j < d3dTex->pitch_w ; j++ ){
			d3d_ret = D3DXCreateTexture( pD3DDevice, ww, hh, 1, 0, d3dTex->format, D3DPOOL_MANAGED, &d3dTex->texture[i*d3dTex->pitch_w+j] );
			if( d3d_ret == D3D_OK ){
				RECT	srect;
				d3dTex->texture[i*d3dTex->pitch_w+j]->GetSurfaceLevel( 0, &work_surface );

				SetRect( &srect, j*ww, i*hh, min((j+1)*ww,sw), min((i+1)*hh,sh) );
				d3d_ret = D3DXLoadSurfaceFromMemory( work_surface, NULL, NULL, buf10, D3DCapsStruct.m_NowDisplayMode.Format, sw*bpp/8, NULL, &srect, D3DX_FILTER_NONE|D3DX_FILTER_MIRROR, 0 );
				work_surface->Release();
			}
			if( d3d_ret != D3D_OK ) break;
		}
		if( d3d_ret != D3D_OK ) break;
	}
	GFree( buf10 );

	switch( d3d_ret ){
		default:	DebugBox( NULL, "なんかエラーでた。" );
		case D3D_OK:
			d3dTex->flag = TRUE;

			d3dTex->w = w;
			d3dTex->h = h;
			d3dTex->grid_w = min(ww,w);
			d3dTex->grid_h = min(hh,h);

			ret=bpp;
			break;
		case D3DERR_NOTAVAILABLE:	DebugBox( NULL, "このデバイスは、照会されたテクニックをサポートしていない。" );	break;
		case D3DERR_OUTOFVIDEOMEMORY:DebugBox( NULL, "Direct3D が処理を行うのに十分なディスプレイ メモリがない。" );	break;
		case D3DERR_INVALIDCALL:	DebugBox( NULL, "メソッドの呼び出しが無効である。たとえば、メソッドのパラメータに無効な値が設定されている場合など。" );	break;
		case D3DXERR_INVALIDDATA:	DebugBox( NULL, "データが無効である。 " );	break;
		case E_OUTOFMEMORY:			DebugBox( NULL, "Direct3D が呼び出しを完了するための十分なメモリを割り当てることができなかった。" );	break;
	}
	return ret;
}


int D3DD_LoadTextureBBM( int tno, char *filename, int alpha, int low_level, int mip, char *tfname, char *tfname2, int chroma, char *pak )
{
	BOOL			ret=0;
	D3DD_TEXTURE	*d3dTex;
	char			fname[MAX_PATH];
	DWORD			size;
	VECTOR_BITMAP	*VectorBitMap;
	BYTE			*buf=NULL, *ptr;
	DWORD			dither_setting=DitherSetting;

	if(alpha&DITHER_OFF)	dither_setting = 0;
	alpha = alpha&0xff;

	if(pak){
		if(pak[0]=='\0'){
			pak=PackD3DDDir;
		}
	}else{
		pak=PackD3DDDir;
	}
	size = PAC_LoadFile( pak, filename, &buf );

	if(size==0) return 0;
	ptr = buf;
	VectorBitMap = (VECTOR_BITMAP*)ptr;	ptr += sizeof( VECTOR_BITMAP);

	D3DD_ReleaseTexture( tno );
	d3dTex = &d3dTexture[tno];

	d3dTex->alpha = ALPHA_1555;
	d3dTex->fit = FALSE;
	d3dTex->low_level = low_level;
	strcpy( d3dTex->filename ,filename );


	LPDIRECT3DSURFACE8	work_surface;
	HRESULT	d3d_ret;
	int	w,h, x,y;
	int	ww,hh;
	int	sw,sh;

	w  = VectorBitMap->w;
	h  = VectorBitMap->h;
	sw = VectorBitMap->w;
	sh = VectorBitMap->h;
	ww = VectorBitMap->grid_w;
	hh = VectorBitMap->grid_h;
	if(low_level) {
		w/=2; h/=2;
		ww/=2; hh/=2;
	}

	d3dTex->pitch_w = w/ww;
	d3dTex->pitch_h = h/hh;
	if( d3dTex->pitch_w>16 || d3dTex->pitch_h>16 ) DebugBox(NULL,"");
	d3dTex->texture = (LPDIRECT3DTEXTURE8*)GAlloc( sizeof(LPDIRECT3DTEXTURE8) * d3dTex->pitch_w * d3dTex->pitch_h );

	if( (D3DCapsStruct.m_NowDisplayMode.Format==D3DFMT_X8R8G8B8 || D3DCapsStruct.m_NowDisplayMode.Format==D3DFMT_A8R8G8B8) ){
		if( D3DCapsStruct.m_ttCaps.m_A8R8G8B8 ){
			d3dTex->format = D3DFMT_A8R8G8B8;
		}else{
			if(alpha){
				DebugBox( NULL, "このビデオカードは32BIT-A8R8G8B8テクスチャに対応していません。[%s]",fname );
			}
			if( D3DCapsStruct.m_ttCaps.m_X8R8G8B8 ){
				d3dTex->format = D3DFMT_X8R8G8B8;
			}else if( D3DCapsStruct.m_ttCaps.m_R8G8B8 ){
				d3dTex->format = D3DFMT_R8G8B8;
			}
		}
	}else{
		if(alpha==ALPHA_4444){
			if( D3DCapsStruct.m_ttCaps.m_A4R4G4B4 ){
				d3dTex->format = D3DFMT_A4R4G4B4;
			}else if( D3DCapsStruct.m_ttCaps.m_A1R5G5B5 ){
				d3dTex->format = D3DFMT_A1R5G5B5;
			}else{
				DebugBox( NULL, "このビデオカードは16BIT-アルファ付きテクスチャに対応していません。[%s]",fname );
			}
		}else{
			if( D3DCapsStruct.m_ttCaps.m_A1R5G5B5 ){
				d3dTex->format = D3DFMT_A1R5G5B5;
			}else if( D3DCapsStruct.m_ttCaps.m_X1R5G5B5 ){
				d3dTex->format = D3DFMT_X1R5G5B5;
			}else if( D3DCapsStruct.m_ttCaps.m_A4R4G4B4 ){
				d3dTex->format = D3DFMT_A4R4G4B4;
			}else{
				DebugBox( NULL, "このビデオカードは16BITテクスチャに対応していません。[%s]",fname );
			}
		}
	}


	int		palnum, vram=0;
	RGBA	*pal;
	BYTE	*bmp;

	for( y=0 ; y<VectorBitMap->pitch_h ; y++ ){
		for( x=0 ; x<VectorBitMap->pitch_w ; x++ ){
			int	srcw, srch, bbp, alpha, hrev;

			LoadTgaInMemory( ptr, &bmp, (BYTE**)&pal, &palnum, &srcw, &srch, &bbp, &alpha, &hrev, TRUE );
			if( bbp!=8 ) { DebugBox(NULL,"ビット数が８ビットではありません。[%s(%d:%d)]",fname,x,y); return 0; }
			int	tadr = y*d3dTex->pitch_w + x;
			d3d_ret = D3D_OK;
			if(d3dTex->texture[tadr]==NULL)
				d3d_ret = D3DXCreateTexture( pD3DDevice, ww, hh, (mip)?2:1, 0, d3dTex->format, D3DPOOL_MANAGED, &d3dTex->texture[tadr] );
			if( d3d_ret == D3D_OK ){
				if( y*hh >= sh/(low_level+1) ) continue;
				if( x*ww >= sw/(low_level+1) ) continue;
				RECT	srect={ 0, 0, ww*(low_level+1),hh*(low_level+1)};
				RECT	drect={ 0, 0, ww, hh };

				BMP_SetChroma( (RGBA*)pal, palnum, chroma );
				BMP_SetTonecurve( (RGBA*)pal, palnum, tfname, PackD3DDDir, TRUE );
				BMP_SetTonecurve( (RGBA*)pal, palnum, tfname2, PackD3DDDir, TRUE );

				d3dTex->texture[tadr]->GetSurfaceLevel( 0, &work_surface );

					d3d_ret = D3DXLoadSurfaceFromMemory( work_surface, NULL, &drect, bmp, D3DFMT_P8, ww*(low_level+1), (PALETTEENTRY*)pal, &srect, D3DX_FILTER_TRIANGLE|dither_setting|D3DX_FILTER_MIRROR, 0 );

				work_surface->Release();
				if(mip){
					d3dTex->texture[tadr]->GetSurfaceLevel( 1, &work_surface );
					d3d_ret = D3DXLoadSurfaceFromMemory( work_surface, NULL, NULL, bmp, D3DFMT_P8, ww*(low_level+1), (PALETTEENTRY*)pal, &srect, D3DX_FILTER_TRIANGLE|dither_setting|D3DX_FILTER_MIRROR, 0 );
					work_surface->Release();
				}
			}
			ptr += VectorBitMap->rect_size;
			if( d3d_ret != D3D_OK ) break;
		}
		if( d3d_ret != D3D_OK ) break;
	}
	GFree(buf);
	switch( d3d_ret ){
		default:	DebugBox( NULL, "なんかエラーでた。" );
		case D3D_OK:
			d3dTex->flag = TRUE;
			if( low_level )	{	d3dTex->w = w*2;	d3dTex->h = h*2;}
			else			{	d3dTex->w = w;		d3dTex->h = h;	}
			d3dTex->grid_w = min(ww,w);
			d3dTex->grid_h = min(hh,h);

			ret=TRUE;
			break;
		case D3DERR_NOTAVAILABLE:	DebugBox( NULL, "このデバイスは、照会されたテクニックをサポートしていない。" );	break;
		case D3DERR_OUTOFVIDEOMEMORY:DebugBox( NULL, "Direct3D が処理を行うのに十分なディスプレイ メモリがない。" );	break;
		case D3DERR_INVALIDCALL:	DebugBox( NULL, "メソッドの呼び出しが無効である。たとえば、メソッドのパラメータに無効な値が設定されている場合など。" );	break;
		case D3DXERR_INVALIDDATA:	DebugBox( NULL, "データが無効である。 " );	break;
		case E_OUTOFMEMORY:			DebugBox( NULL, "Direct3D が呼び出しを完了するための十分なメモリを割り当てることができなかった。" );	break;
	}

	
	return 1;
}

int D3DD_LoadTexture( int tno, char *fname,int w, int h, int alpha, int fit, int low_level, int mip, char *tfname, char *tfname2, int chroma, char *pak, int tex_size )
{
	int				i,j;
	BOOL			ret=0;
	D3DD_TEXTURE	*d3dTex;
	DWORD			dither_setting=DitherSetting;
	BOOL			rev_w=FALSE;
	BOOL			rev_h=FALSE;

	if(alpha&REV_U)	rev_w = TRUE;
	if(alpha&REV_V)	rev_h = TRUE;
	if(alpha&DITHER_OFF)	dither_setting = 0;

	alpha = alpha&0xff;
	if(pak){
		if(pak[0]=='\0'){
			pak=PackD3DDDir;
		}
	}else{
		pak=PackD3DDDir;
	}
	D3DD_ReleaseTexture( tno );
	d3dTex = &d3dTexture[tno];


	d3dTex->alpha = alpha;
	d3dTex->fit = fit;
	d3dTex->low_level = low_level;
	strcpy( d3dTex->filename ,fname );

	if( alpha==ALPHA_FADE){
		if( (D3DCapsStruct.m_NowDisplayMode.Format==D3DFMT_X8R8G8B8 || D3DCapsStruct.m_NowDisplayMode.Format==D3DFMT_A8R8G8B8) ){
			d3dTex->format = D3DFMT_A8;
		}else{
			return FALSE;
		}
	}else{
		if( !D3DD_CheckTextureSize( w, h ) ){
			return 0;
		}
		if( (D3DCapsStruct.m_NowDisplayMode.Format==D3DFMT_X8R8G8B8 || D3DCapsStruct.m_NowDisplayMode.Format==D3DFMT_A8R8G8B8) ){
			if(alpha){
				if( D3DCapsStruct.m_ttCaps.m_A8R8G8B8 ){
					d3dTex->format = D3DFMT_A8R8G8B8;
				}else{
					DebugBox( NULL, "このビデオカードは32BIT-A8R8G8B8テクスチャに対応していません。[%s]",fname );
					if( D3DCapsStruct.m_ttCaps.m_X8R8G8B8 ){
						d3dTex->format = D3DFMT_X8R8G8B8;
					}else if( D3DCapsStruct.m_ttCaps.m_R8G8B8 ){
						d3dTex->format = D3DFMT_R8G8B8;
					}
				}
			}else{
				if( D3DCapsStruct.m_ttCaps.m_X8R8G8B8 ){
					d3dTex->format = D3DFMT_X8R8G8B8;
				}else if( D3DCapsStruct.m_ttCaps.m_A8R8G8B8 ){
					d3dTex->format = D3DFMT_A8R8G8B8;
				}else if( D3DCapsStruct.m_ttCaps.m_R8G8B8 ){
					d3dTex->format = D3DFMT_R8G8B8;
				}else{
					DebugBox( NULL, "このビデオカードは24,32BIT-フルカラーテクスチャに対応していません。[%s]",fname );
				}
			}
		}else{
			if(alpha){
				if(alpha==ALPHA_4444){
					if( D3DCapsStruct.m_ttCaps.m_A4R4G4B4 ){
						d3dTex->format = D3DFMT_A4R4G4B4;
					}else if( D3DCapsStruct.m_ttCaps.m_A1R5G5B5 ){
						d3dTex->format = D3DFMT_A1R5G5B5;
					}else{
						DebugBox( NULL, "このビデオカードは16BIT-アルファ付きテクスチャに対応していません。[%s]",fname );
					}
				}else{
					if( D3DCapsStruct.m_ttCaps.m_A1R5G5B5 ){
						d3dTex->format = D3DFMT_A1R5G5B5;
					}else if( D3DCapsStruct.m_ttCaps.m_A4R4G4B4 ){
						d3dTex->format = D3DFMT_A4R4G4B4;
					}else{
						DebugBox( NULL, "このビデオカードは16BIT-アルファ付きテクスチャに対応していません。[%s]",fname );
					}
				}
			}else{
				if( D3DCapsStruct.m_ttCaps.m_X1R5G5B5 ){
					d3dTex->format = D3DFMT_X1R5G5B5;
				}else if( D3DCapsStruct.m_ttCaps.m_A1R5G5B5 ){
					d3dTex->format = D3DFMT_A1R5G5B5;
				}else if( D3DCapsStruct.m_ttCaps.m_R5G6B5 ){
					d3dTex->format = D3DFMT_R5G6B5;
				}else if( D3DCapsStruct.m_ttCaps.m_A4R4G4B4 ){
					d3dTex->format = D3DFMT_A4R4G4B4;
				}else if( D3DCapsStruct.m_ttCaps.m_X4R4G4B4 ){
					d3dTex->format = D3DFMT_X4R4G4B4;
				}else{
					DebugBox( NULL, "このビデオカードは16BITテクスチャに対応していません。[%s]",fname );
				}
			}
		}
	}


	LPDIRECT3DSURFACE8	work_surface;
	HRESULT	d3d_ret;
	int	ww,hh;
	int	sw,sh;

	if(fit){
		char	fname2[256];
		if(low_level){
			w/=2;
			h/=2;
		}
		if( h==w && (int)D3DCapsStruct.m_d3dCaps.MaxTextureWidth >= w ){
			hh = ww = w;
		}else{
			DebugBox( NULL, "fit オプションを指定したテクスチャは正方形で、かつ最大テクスチャサイズを超えることはできません。[%s,%d,%d]",fname,w,h );
			return 0;
		}
		d3dTex->pitch_w = (w/ww);
		d3dTex->pitch_h = (h/hh);
		if( d3dTex->pitch_w>16 || d3dTex->pitch_h>16 ) DebugBox(NULL,"");
		d3dTex->texture = (LPDIRECT3DTEXTURE8*)GAlloc( sizeof(LPDIRECT3DTEXTURE8) * d3dTex->pitch_w * d3dTex->pitch_h );

		D3DXIMAGE_INFO	d3dii;
		D3DXGetImageInfoFromFile( fname, &d3dii );
		d3dii.Format = D3DFMT_A8R8G8B8;


		wsprintf( fname2, "%s\\%s", pak, fname );
		d3d_ret = D3DXCreateTextureFromFileEx(	pD3DDevice, fname2, ww, hh, 1, 0,
												d3dTex->format, D3DPOOL_MANAGED,
												D3DX_FILTER_TRIANGLE|dither_setting|D3DX_FILTER_MIRROR,
												D3DX_FILTER_TRIANGLE|dither_setting|D3DX_FILTER_MIRROR,
												0, &d3dii, NULL, &d3dTex->texture[0] );
	}else{
		BYTE				*buf=NULL, *ptr;
		int					palnum, vram=0;
		int					bpp, y,x;
		PALETTEENTRY		*pal;
		BYTE				*bmp, *bmp2;
		int					alp;
		DWORD				size;

		size = PAC_LoadFile( pak, fname, &buf );

		if(size==0) { DebugBox(NULL,"ファイルがありません[%s]",fname); return 0; }
		ptr = buf;

		if( _mbsicmp( (BYTE *)STD_GetAttr(fname), (BYTE *)"tga")==0 ){
			int	rev_bmp=0;

			if( !LoadTgaInMemory( ptr, &bmp, (BYTE**)&pal, &palnum, &sw, &sh, &bpp, &alp, &rev_bmp, FALSE ) ){

				DebugBox(NULL,"%sの読み込みに失敗しました",fname);
				GlobalFree( buf );
				return FALSE;
			}
			palnum=palnum*4;
			if(!rev_bmp){
				rev_h = !rev_h;
			}
		}else{
			BITMAPFILEHEADER    *BMPFileHead;
			BITMAPINFOHEADER    *BMPFileInfo;
			
			BMPFileHead = (BITMAPFILEHEADER*)ptr;	ptr += sizeof( BITMAPFILEHEADER);	
			BMPFileInfo = (BITMAPINFOHEADER*)ptr;	ptr += sizeof( BITMAPINFOHEADER);	

			
			palnum = BMPFileHead->bfOffBits-(sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER));
			
			sw  = BMPFileInfo->biWidth;
			sh  = abs(BMPFileInfo->biHeight);
			bpp = BMPFileInfo->biBitCount;
			pal = (PALETTEENTRY *)ptr;
			ptr += palnum;
			bmp = (BYTE *)ptr;
			alp = 0;

			if(BMPFileInfo->biHeight>0){
				rev_h = !rev_h;
			}
		}
		d3dTex->bw = sw;
		d3dTex->bh = sh;
		switch(bpp){
			case 8:  BMP_SetChroma( (RGBA*)pal, palnum/4, chroma );	break;
			case 24: BMP_SetChroma( (RGB_*)bmp, sw*sh, chroma, TRUE );	break;
			case 32: BMP_SetChroma( (RGBA*)bmp, sw*sh, chroma );	break;
		}
		if(tfname){
			switch(bpp){
				case 8:  BMP_SetTonecurve( (RGBA*)pal, palnum/4, tfname, PackD3DDDir );	break;
				case 24: BMP_SetTonecurve( (RGB_*)bmp, sw*sh, tfname, PackD3DDDir, TRUE );	break;
				case 32: BMP_SetTonecurve( (RGBA*)bmp, sw*sh, tfname, PackD3DDDir );	break;
			}
		}
		if(tfname2){
			switch(bpp){
				case 8:  BMP_SetTonecurve( (RGBA*)pal, palnum/4, tfname2, PackD3DDDir );	break;
				case 24: BMP_SetTonecurve( (RGB_*)bmp, sw*sh, tfname2, PackD3DDDir, TRUE );	break;
				case 32: BMP_SetTonecurve( (RGBA*)bmp, sw*sh, tfname2, PackD3DDDir );	break;
			}
		}


		if( d3dTex->format == D3DFMT_A8 ){
			if(bpp==8){
				d3dTex->fade_txt = (BYTE*)GAlloc( sw * sh * 1 );

				i=0;
				for( y=0 ; y < sh ; y++ ){
					for( x=0 ; x < sw ; x++,i++ ){
						if(rev_h){
							if(rev_w)	j = (sh-y-1)*sw + (sw-x-1);
							else		j = (sh-y-1)*sw + x;
						}else{
							if(rev_w)	j = y*sw + (sw-x-1);
							else		j = y*sw + x;
						}
						d3dTex->fade_txt[i] = pal[bmp[j]].peRed;
					}
				}
				d3dTex->flag = TRUE;

				d3dTex->pitch_w = 1;
				d3dTex->pitch_h = 1;
				d3dTex->w = sw;
				d3dTex->h = sh;
				d3dTex->grid_w = sw;
				d3dTex->grid_h = sh;
				GFree(buf);

				return TRUE;
			}else{
				GFree(buf);
				return FALSE;
			}
		}

		if(w==-1){
			if( sw<=tex_size/2 ){
				if     ( sw<=16 )	w=16;
				else if( sw<=32 )	w=32;
				else if( sw<=64 )	w=64;
				else				w=128;
			}else{
				w = (sw+tex_size-1)/tex_size*tex_size;
			}
			if( sh<=tex_size/2 ){
				if     ( sh<=16 )	h=16;
				else if( sh<=32 )	h=32;
				else if( sh<=64 )	h=64;
				else				h=128;
			}else{
				h = (sh+tex_size-1)/tex_size*tex_size;
			}
		}
		if(low_level){
			w/=2;
			h/=2;
		}
		hh = ww = min( tex_size/(low_level+1), (w>h)?h:w );

	

		d3dTex->pitch_w = (w/ww);
		d3dTex->pitch_h = (h/hh);
		if( d3dTex->pitch_w>16 || d3dTex->pitch_h>16 ) DebugBox(NULL,"");
		d3dTex->texture = (LPDIRECT3DTEXTURE8*)GAlloc( sizeof(LPDIRECT3DTEXTURE8) * d3dTex->pitch_w * d3dTex->pitch_h );

		if(bpp==8){
			for( i=0 ; i < palnum/4 ; i++ ){
				BYTE	work;

				work = pal[i].peRed;
				pal[i].peRed   = pal[i].peBlue;
				pal[i].peGreen = pal[i].peGreen;
				pal[i].peBlue  = work;

				if(alpha==ALPHA_256P && i==0)	{ pal[i].peFlags=0;    }
				else if(alpha==ALPHA_FADE)		{ pal[i].peFlags=pal[i].peRed; }
				else{
					if( d3dTex->format == D3DFMT_A1R5G5B5 && DitherSetting == D3DX_FILTER_DITHER && alp ){
						if( pal[i].peFlags>240 ) pal[i].peFlags=255;
						else					 pal[i].peFlags=0;
					}else{
						if( !alp ) pal[i].peFlags=255;
					}
				}
			}
		}
		if( d3dTex->format == D3DFMT_A1R5G5B5 && DitherSetting == D3DX_FILTER_DITHER && bpp==32 ){
			bmp2 = bmp;
			for( y=0 ; y < sh ; y++ ){
				for( x=0 ; x < sw ; x++, bmp2+=4 ){
					if( *(bmp2+3)>250 )	*(bmp2+3)=255;
					else				*(bmp2+3)=0;
				}
			}
			bmp2=NULL;
		}

		if(rev_h){
			bmp2 = (BYTE*)GAlloc( sw * sh * bpp/8 );

			for( y=0 ; y < sh ; y++ ){
				CopyMemory( bmp2 + y*(sw*bpp/8), bmp + (sh-y-1)*(sw*bpp/8), sw*bpp/8 );
			}
		}else{
			bmp2=bmp;
		}

		for( i=0 ; i < d3dTex->pitch_h ; i++ ){
			for( j=0 ; j < d3dTex->pitch_w ; j++ ){
				d3d_ret = D3DXCreateTexture( pD3DDevice, ww, hh, (mip)?2:1, 0, d3dTex->format, D3DPOOL_MANAGED, &d3dTex->texture[i*d3dTex->pitch_w+j] );
				if( d3d_ret == D3D_OK ){
					if( i*hh>=sh/(low_level+1) ) continue;
					if( j*ww>=sw/(low_level+1) ) continue;
					RECT	srect;
					d3dTex->texture[i*d3dTex->pitch_w+j]->GetSurfaceLevel( 0, &work_surface );

					if(low_level){

						SetRect( &srect, max(0,j*ww*2), max(0,i*hh*2), min((j+1)*ww*2,sw), min((i+1)*hh*2,sh) );
						
						switch(bpp){
							case 8:	d3d_ret = D3DXLoadSurfaceFromMemory( work_surface, NULL, NULL, bmp2, D3DFMT_P8, sw, pal, &srect, D3DX_FILTER_TRIANGLE|dither_setting|D3DX_FILTER_MIRROR, 0 );	break;
							case 16:DebugBox( NULL, "16bit素材です、ロードしません。" );	break;
							case 24:d3d_ret = D3DXLoadSurfaceFromMemory( work_surface, NULL, NULL, bmp2, D3DFMT_R8G8B8, sw*3, NULL, &srect, D3DX_FILTER_TRIANGLE|dither_setting|D3DX_FILTER_MIRROR, 0 );	break;
							case 32:d3d_ret = D3DXLoadSurfaceFromMemory( work_surface, NULL, NULL, bmp2, D3DFMT_A8R8G8B8, sw*4, NULL, &srect, D3DX_FILTER_TRIANGLE|dither_setting|D3DX_FILTER_MIRROR, 0 );	break;
						}
					}else{
						SetRect( &srect, j*ww, i*hh, min((j+1)*ww,sw), min((i+1)*hh,sh) );
						switch(bpp){
							case 8:	d3d_ret = D3DXLoadSurfaceFromMemory( work_surface, NULL, NULL, bmp2, D3DFMT_P8, sw, pal, &srect, D3DX_FILTER_NONE|dither_setting|D3DX_FILTER_MIRROR, 0 );	break;
							case 16:DebugBox( NULL, "16bit素材です、ロードしません。" );	break;
							case 24:d3d_ret = D3DXLoadSurfaceFromMemory( work_surface, NULL, NULL, bmp2, D3DFMT_R8G8B8, sw*3, NULL, &srect, D3DX_FILTER_NONE|dither_setting|D3DX_FILTER_MIRROR, 0 );	break;
							case 32:d3d_ret = D3DXLoadSurfaceFromMemory( work_surface, NULL, NULL, bmp2, D3DFMT_A8R8G8B8, sw*4, NULL, &srect, D3DX_FILTER_NONE|dither_setting|D3DX_FILTER_MIRROR, 0 );	break;
						}

					}
					work_surface->Release();
					if(mip){
						d3dTex->texture[i*d3dTex->pitch_w+j]->GetSurfaceLevel( 1, &work_surface );
						if(low_level){
							switch(bpp){
								case 8:	d3d_ret = D3DXLoadSurfaceFromMemory( work_surface, NULL, NULL, bmp2, D3DFMT_P8, sw, pal, &srect, D3DX_FILTER_TRIANGLE|dither_setting|D3DX_FILTER_MIRROR, 0 );	break;
								case 16:DebugBox( NULL, "16bit素材です、ロードしません。" );	break;
								case 24:d3d_ret = D3DXLoadSurfaceFromMemory( work_surface, NULL, NULL, bmp2, D3DFMT_R8G8B8, sw*3, NULL, &srect, D3DX_FILTER_TRIANGLE|dither_setting|D3DX_FILTER_MIRROR, 0 );	break;
								case 32:d3d_ret = D3DXLoadSurfaceFromMemory( work_surface, NULL, NULL, bmp2, D3DFMT_A8R8G8B8, sw*4, NULL, &srect, D3DX_FILTER_TRIANGLE|dither_setting|D3DX_FILTER_MIRROR, 0 );	break;
							}
						}else{
							switch(bpp){
								case 8:	d3d_ret = D3DXLoadSurfaceFromMemory( work_surface, NULL, NULL, bmp2, D3DFMT_P8, sw, pal, &srect, D3DX_FILTER_TRIANGLE|dither_setting|D3DX_FILTER_MIRROR, 0 );	break;
								case 16:DebugBox( NULL, "16bit素材です、ロードしません。" );	break;
								case 24:d3d_ret = D3DXLoadSurfaceFromMemory( work_surface, NULL, NULL, bmp2, D3DFMT_R8G8B8, sw*3, NULL, &srect, D3DX_FILTER_TRIANGLE|dither_setting|D3DX_FILTER_MIRROR, 0 );	break;
								case 32:d3d_ret = D3DXLoadSurfaceFromMemory( work_surface, NULL, NULL, bmp2, D3DFMT_A8R8G8B8, sw*4, NULL, &srect, D3DX_FILTER_TRIANGLE|dither_setting|D3DX_FILTER_MIRROR, 0 );	break;
							}

						}
						work_surface->Release();
					}
				}
				if( d3d_ret != D3D_OK ) break;
			}
			if( d3d_ret != D3D_OK ) break;
		}
		if(rev_h){
			GFree(bmp2);
		}
		GFree(buf);
	}

	switch( d3d_ret ){
		default:	DebugBox( NULL, "なんかエラーでた。" );
		case D3D_OK:

			d3dTex->flag = TRUE;

			if( low_level )	{	d3dTex->w = w*2;	d3dTex->h = h*2;}
			else			{	d3dTex->w = w;		d3dTex->h = h;	}
			d3dTex->grid_w = min(ww,w);
			d3dTex->grid_h = min(hh,h);

			ret=TRUE;
			break;
		case D3DERR_NOTAVAILABLE:	DebugBox( NULL, "このデバイスは、照会されたテクニックをサポートしていない。" );	break;
		case D3DERR_OUTOFVIDEOMEMORY:DebugBox( NULL, "Direct3D が処理を行うのに十分なディスプレイ メモリがない。" );	break;
		case D3DERR_INVALIDCALL:	DebugBox( NULL, "メソッドの呼び出しが無効である。たとえば、メソッドのパラメータに無効な値が設定されている場合など。" );	break;
		case D3DXERR_INVALIDDATA:	DebugBox( NULL, "データが無効である。 " );	break;
		case E_OUTOFMEMORY:			DebugBox( NULL, "Direct3D が呼び出しを完了するための十分なメモリを割り当てることができなかった。" );	break;
	}


	return ret;
}

D3DD_TEXTURE *D3DD_GetTexture( int tno )
{
	return 	&d3dTexture[tno];
}




struct TLVERTEX
{
	float		x,y,z;
	float		rhw;
	D3DCOLOR	color;
	D3DCOLOR	specular;
	float		tu,tv;
};
#define	FVF_TLVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 | D3DFVF_SPECULAR)


struct TLVERTEX2
{
	float		x,y,z;
	float		rhw;
	D3DCOLOR	color;
	D3DCOLOR	specular;
	float		tu0,tv0;
	float		tu1,tv1;
};
#define	FVF_TLVERTEX2 (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX2 | D3DFVF_SPECULAR)


struct TLVERTEX_NOTEX
{
	float		x,y,z;
	float		rhw;
	D3DCOLOR	color;

};
#define	FVF_TLVERTEX_NOTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)


#include "dispSprite.h"

BOOL D3DD_SetGraphSpriteFrame( int dno, int ani_no, int sp_no, int frame_no, int layer, int disp )
{
	D3DD_ResetGraph(dno);

	d3dDisp[dno].rander_no  = -1;	
	d3dDisp[dno].textue_no  = ani_no;
	d3dDisp[dno].textue_no2 = -1;	
	
	d3dDisp[dno].flag  = ON;		
	d3dDisp[dno].disp  = disp;		
	d3dDisp[dno].layer = layer;		
	d3dDisp[dno].group = 0x01;

	d3dDisp[dno].digit = frame_no;	

	d3dDisp[dno].type = GTYPE_SP_FRAME;		
	d3dDisp[dno].vnum = 4;		

	d3dDisp[dno].ps_flag = OFF;

	d3dDisp[dno].blend_param = BLEND_NONE;	
	d3dDisp[dno].scale_param = SCALE_POINT;	
	d3dDisp[dno].rev_param   = REVERSE_NORMAL;		

	d3dDisp[dno].bright_no = 0;	

	*(DWORD*)&d3dDisp[dno].color[0] = D3DCOLOR_ARGB(255,255,255,255);
	*(DWORD*)&d3dDisp[dno].color[1] = D3DCOLOR_ARGB(255,255,255,255);
	*(DWORD*)&d3dDisp[dno].color[2] = D3DCOLOR_ARGB(255,255,255,255);
	*(DWORD*)&d3dDisp[dno].color[3] = D3DCOLOR_ARGB(255,255,255,255);

	*(DWORD*)&d3dDisp[dno].specular[0] = D3DCOLOR_ARGB(255,0,0,0);
	*(DWORD*)&d3dDisp[dno].specular[1] = D3DCOLOR_ARGB(255,0,0,0);
	*(DWORD*)&d3dDisp[dno].specular[2] = D3DCOLOR_ARGB(255,0,0,0);
	*(DWORD*)&d3dDisp[dno].specular[3] = D3DCOLOR_ARGB(255,0,0,0);

	d3dDisp[dno].dx[0]=0;	d3dDisp[dno].dy[0]=0;	

	d3dDisp[dno].rhw[0] = d3dDisp[dno].rhw[1] = d3dDisp[dno].rhw[2] = d3dDisp[dno].rhw[3] = 1.0f;

	d3dDisp[dno].zoom_x   = 1.0f;
	d3dDisp[dno].zoom_y   = 1.0f;
	d3dDisp[dno].kaiten = 0.0f;

	d3dDisp[dno].cx     = 0;
	d3dDisp[dno].cy     = 0;

	d3dDisp[dno].dx[0]=0;							d3dDisp[dno].dy[0]=0;	

	return TRUE;
}

BOOL D3DD_SetGraphSprite( int dno, int ani_no, int sp_no, int mode, int layer, int disp, int end, int loop, int cfade )
{
	D3DD_ResetGraph(dno);

	d3dDisp[dno].rander_no  = -1;	
	d3dDisp[dno].textue_no  = SPR_SetSprite( ani_no, sp_no, mode, end, loop, cfade );
	d3dDisp[dno].textue_no2 = -1;	

	if(d3dDisp[dno].textue_no==-1){
		D3DD_ResetGraph( dno );
		return FALSE;
	}
	
	d3dDisp[dno].flag  = ON;		
	d3dDisp[dno].disp  = disp;		
	d3dDisp[dno].layer = layer;		
	d3dDisp[dno].group = 0x01;

	d3dDisp[dno].type = GTYPE_SPRITE;		
	d3dDisp[dno].vnum = 4;		

	d3dDisp[dno].ps_flag = OFF;

	d3dDisp[dno].blend_param = BLEND_NONE;	
	d3dDisp[dno].scale_param = SCALE_POINT;	
	d3dDisp[dno].rev_param   = REVERSE_NORMAL;		

	d3dDisp[dno].bright_no = 0;	

	*(DWORD*)&d3dDisp[dno].color[0] = D3DCOLOR_ARGB(255,255,255,255);
	*(DWORD*)&d3dDisp[dno].color[1] = D3DCOLOR_ARGB(255,255,255,255);
	*(DWORD*)&d3dDisp[dno].color[2] = D3DCOLOR_ARGB(255,255,255,255);
	*(DWORD*)&d3dDisp[dno].color[3] = D3DCOLOR_ARGB(255,255,255,255);

	*(DWORD*)&d3dDisp[dno].specular[0] = D3DCOLOR_ARGB(255,0,0,0);
	*(DWORD*)&d3dDisp[dno].specular[1] = D3DCOLOR_ARGB(255,0,0,0);
	*(DWORD*)&d3dDisp[dno].specular[2] = D3DCOLOR_ARGB(255,0,0,0);
	*(DWORD*)&d3dDisp[dno].specular[3] = D3DCOLOR_ARGB(255,0,0,0);

	d3dDisp[dno].dx[0]=0;	d3dDisp[dno].dy[0]=0;	

	d3dDisp[dno].rhw[0] = d3dDisp[dno].rhw[1] = d3dDisp[dno].rhw[2] = d3dDisp[dno].rhw[3] = 1.0f;

	d3dDisp[dno].zoom_x   = 1.0f;
	d3dDisp[dno].zoom_y   = 1.0f;
	d3dDisp[dno].kaiten = 0.0f;

	d3dDisp[dno].cx     = 0;
	d3dDisp[dno].cy     = 0;

	return TRUE;
}

BOOL D3DD_SetSpriteSpeed( int dno, int speed )
{
	if( 0>dno && dno>=D3DD_MAX_DISP_AMOUNT ) return FALSE;
	if( !d3dDisp[dno].flag ) return FALSE;

	return SPR_SetSpriteSpeed( d3dDisp[dno].textue_no, speed );
}
BOOL D3DD_SetSpritePlay( int dno, int ani_no, int sp_no, int mode, int end, int lnum, int cfade )
{
	if( 0>dno && dno>=D3DD_MAX_DISP_AMOUNT ) return FALSE;
	if( !d3dDisp[dno].flag ) return FALSE;

	return SPR_SetSpritePlay( d3dDisp[dno].textue_no, ani_no, sp_no, mode, end, lnum, cfade );
}
BOOL D3DD_SetSpriteNo( int dno, int sp_no )
{
	if( 0>dno && dno>=D3DD_MAX_DISP_AMOUNT ) return FALSE;
	if( !d3dDisp[dno].flag ) return FALSE;

	return SPR_SetSpriteNo( d3dDisp[dno].textue_no, sp_no );
}
BOOL D3DD_SetSpriteCfade( int dno, int flag )
{
	if( 0>dno && dno>=D3DD_MAX_DISP_AMOUNT ) return FALSE;
	if( !d3dDisp[dno].flag ) return FALSE;

	return SPR_SetSpriteCfade( d3dDisp[dno].textue_no, flag );
}
BOOL D3DD_SetSpriteLoop( int dno, int end, int lnum )
{
	if( 0>dno && dno>=D3DD_MAX_DISP_AMOUNT ) return FALSE;
	if( !d3dDisp[dno].flag ) return FALSE;

	return SPR_SetSpriteLoop( d3dDisp[dno].textue_no, end, lnum );
}
BOOL D3DD_SetSpriteLoopThrough( int dno )
{
	if( 0>dno && dno>=D3DD_MAX_DISP_AMOUNT ) return FALSE;
	if( !d3dDisp[dno].flag ) return FALSE;

	return SPR_SetSpriteLoopThrough( d3dDisp[dno].textue_no );
}

int  D3DD_GetSpriteRepeatFind( int dno )
{
	if( 0>dno && dno>=D3DD_MAX_DISP_AMOUNT ) return REPEAT_NULL;
	if( !d3dDisp[dno].flag ) return REPEAT_NULL;

	return SPR_GetSpriteRepeatFind( d3dDisp[dno].textue_no );
}
BOOL D3DD_SetSpriteRepeatFind( int dno )
{
	if( 0>dno && dno>=D3DD_MAX_DISP_AMOUNT ) return FALSE;
	if( !d3dDisp[dno].flag ) return FALSE;

	return SPR_SetSpriteRepeatFind( d3dDisp[dno].textue_no );
}

RECT *D3DD_GetSpriteRect( int dno )
{
	if( 0>dno && dno>=D3DD_MAX_DISP_AMOUNT ) return NULL;
	if( !d3dDisp[dno].flag ) return NULL;

	return SPR_GetSpriteRect( d3dDisp[dno].textue_no );
}

BOOL D3DD_WaitSpriteCondition( int dno )
{
	if( 0>dno && dno>=D3DD_MAX_DISP_AMOUNT ) return FALSE;
	if( !d3dDisp[dno].flag ) return FALSE;

	return SPR_WaitSpriteCondition( d3dDisp[dno].textue_no );
}

int  D3DD_GetSpriteMode( int dno )
{
	if( 0>dno && dno>=D3DD_MAX_DISP_AMOUNT ) return SP_STOP;
	if( !d3dDisp[dno].flag ) return SP_STOP;

	return SPR_GetSpriteMode( d3dDisp[dno].textue_no );
}
BOOL D3DD_SetSpriteMode( int dno, int mode )
{
	if( 0>dno && dno>=D3DD_MAX_DISP_AMOUNT ) return FALSE;
	if( !d3dDisp[dno].flag ) return FALSE;

	return SPR_SetSpriteMode( d3dDisp[dno].textue_no, mode );
}

int D3DD_GetSpriteEvent( int dno )
{
	if( 0>dno && dno>=D3DD_MAX_DISP_AMOUNT ) return 0;
	if( !d3dDisp[dno].flag ) return 0;

	return SPR_GetSpriteEvent( d3dDisp[dno].textue_no );
}

void D3DD_ResetGraph( int dno )
{
	if(d3dDisp[dno].flag){
		switch( d3dDisp[dno].type ){
			case GTYPE_POLY:	
				break;
			case GTYPE_SPRITE:	
				SPR_ResetSprite( d3dDisp[dno].textue_no );
				break;
			case GTYPE_MOVIE:	
				break;
			case GTYPE_GRID:	
				break;
			case GTYPE_DIGIT:	
				break;
			case GTYPE_TEXT:	
				break;
		}
		GFree(d3dDisp[dno].str);
	}
	ZeroMemory( &d3dDisp[dno], sizeof(D3DD_DISP) );
}





void D3DD_SetGraph( int dno, int tno, int layer, int disp )
{
	D3DD_ResetGraph(dno);
	d3dDisp[dno].flag  = ON;		
	d3dDisp[dno].disp  = disp;		
	d3dDisp[dno].layer = layer;		
	d3dDisp[dno].group = 0x01;

	d3dDisp[dno].type = GTYPE_POLY;		
	d3dDisp[dno].vnum = 4;		

	d3dDisp[dno].ps_flag = OFF;

	d3dDisp[dno].blend_param = BLEND_ALPHA;	
	d3dDisp[dno].scale_param = SCALE_POINT;	
	d3dDisp[dno].rev_param   = REVERSE_NORMAL;		

	d3dDisp[dno].rander_no  = -1;	
	d3dDisp[dno].textue_no  = tno;	
	d3dDisp[dno].textue_no2 = -1;	

	d3dDisp[dno].bright_no = 0;	

	*(DWORD*)&d3dDisp[dno].color[0] = D3DCOLOR_ARGB(255,255,255,255);
	*(DWORD*)&d3dDisp[dno].color[1] = D3DCOLOR_ARGB(255,255,255,255);
	*(DWORD*)&d3dDisp[dno].color[2] = D3DCOLOR_ARGB(255,255,255,255);
	*(DWORD*)&d3dDisp[dno].color[3] = D3DCOLOR_ARGB(255,255,255,255);

	*(DWORD*)&d3dDisp[dno].specular[0] = D3DCOLOR_ARGB(255,0,0,0);
	*(DWORD*)&d3dDisp[dno].specular[1] = D3DCOLOR_ARGB(255,0,0,0);
	*(DWORD*)&d3dDisp[dno].specular[2] = D3DCOLOR_ARGB(255,0,0,0);
	*(DWORD*)&d3dDisp[dno].specular[3] = D3DCOLOR_ARGB(255,0,0,0);

	d3dDisp[dno].zoom_x   = 1.0f;
	d3dDisp[dno].zoom_y   = 1.0f;
	d3dDisp[dno].kaiten = 0.0f;
	d3dDisp[dno].cx = 0;
	d3dDisp[dno].cy = 0;

	d3dDisp[dno].dx[0]=0;							d3dDisp[dno].dy[0]=0;	
	d3dDisp[dno].dx[1]=(float)d3dTexture[tno].w;	d3dDisp[dno].dy[1]=0;
	d3dDisp[dno].dx[2]=0;							d3dDisp[dno].dy[2]=(float)d3dTexture[tno].h;
	d3dDisp[dno].dx[3]=(float)d3dTexture[tno].w;	d3dDisp[dno].dy[3]=(float)d3dTexture[tno].h;

	d3dDisp[dno].rhw[0] = d3dDisp[dno].rhw[1] = d3dDisp[dno].rhw[2] = d3dDisp[dno].rhw[3] = 1.0f;

	d3dDisp[dno].sx1[0]=0;							d3dDisp[dno].sy1[0]=0;
	d3dDisp[dno].sx1[1]=(float)d3dTexture[tno].w;	d3dDisp[dno].sy1[1]=(float)d3dTexture[tno].h;

	d3dDisp[dno].sx2[0]=0; d3dDisp[dno].sy2[0]=0;
	d3dDisp[dno].sx2[1]=0; d3dDisp[dno].sy2[1]=0;
}

void D3DD_SetGraphx9( int dno, int tno, int layer, int disp )
{
	D3DD_SetGraph( dno, tno, layer, disp );
	d3dDisp[dno].type = GTYPE_POLYx9;		
}

void D3DD_SetGraphPrimitive( int dno, int vnum, int layer, int disp )
{
	D3DD_ResetGraph(dno);
	d3dDisp[dno].flag  = ON;		
	d3dDisp[dno].disp  = disp;		
	d3dDisp[dno].layer = layer;		
	d3dDisp[dno].group = 0x01;

	d3dDisp[dno].type = GTYPE_POLY;		
	d3dDisp[dno].vnum = vnum;		

	d3dDisp[dno].ps_flag = OFF;

	d3dDisp[dno].blend_param = BLEND_ALPHA;	
	d3dDisp[dno].scale_param = 0;	
	d3dDisp[dno].rev_param   = 0;		

	d3dDisp[dno].rander_no  = -1;	
	d3dDisp[dno].textue_no  = -1;	
	d3dDisp[dno].textue_no2 = -1;	

	d3dDisp[dno].bright_no = 0;	

	*(DWORD*)&d3dDisp[dno].color[0] = D3DCOLOR_ARGB(255,255,255,255);
	*(DWORD*)&d3dDisp[dno].color[1] = D3DCOLOR_ARGB(255,255,255,255);
	*(DWORD*)&d3dDisp[dno].color[2] = D3DCOLOR_ARGB(255,255,255,255);
	*(DWORD*)&d3dDisp[dno].color[3] = D3DCOLOR_ARGB(255,255,255,255);
	*(DWORD*)&d3dDisp[dno].specular[0] = D3DCOLOR_ARGB(255,0,0,0);
	*(DWORD*)&d3dDisp[dno].specular[1] = D3DCOLOR_ARGB(255,0,0,0);
	*(DWORD*)&d3dDisp[dno].specular[2] = D3DCOLOR_ARGB(255,0,0,0);
	*(DWORD*)&d3dDisp[dno].specular[3] = D3DCOLOR_ARGB(255,0,0,0);

	d3dDisp[dno].zoom_x   = 1.0f;
	d3dDisp[dno].zoom_y   = 1.0f;
	d3dDisp[dno].kaiten = 0.0f;
	d3dDisp[dno].cx = 0;
	d3dDisp[dno].cy = 0;

	d3dDisp[dno].dx[0]=0;						d3dDisp[dno].dy[0]=0;	
	d3dDisp[dno].dx[1]=(float)BackSufaceSizeW;	d3dDisp[dno].dy[1]=0;
	d3dDisp[dno].dx[2]=0;						d3dDisp[dno].dy[2]=(float)BackSufaceSizeH;
	d3dDisp[dno].dx[3]=(float)BackSufaceSizeW;	d3dDisp[dno].dy[3]=(float)BackSufaceSizeH;

	d3dDisp[dno].rhw[0] = d3dDisp[dno].rhw[1] = d3dDisp[dno].rhw[2] = d3dDisp[dno].rhw[3] = 1.0f;

	d3dDisp[dno].sx1[0]=0;	d3dDisp[dno].sy1[0]=0;
	d3dDisp[dno].sx1[1]=0;	d3dDisp[dno].sy1[1]=0;

	d3dDisp[dno].sx2[0]=0; d3dDisp[dno].sy2[0]=0;
	d3dDisp[dno].sx2[1]=0; d3dDisp[dno].sy2[1]=0;
}
void D3DD_SetGraphCircle( int dno, int vnum, int paint, int layer, int disp )
{
	D3DD_ResetGraph(dno);
	d3dDisp[dno].flag  = ON;		
	d3dDisp[dno].disp  = disp;		
	d3dDisp[dno].layer = layer;		
	d3dDisp[dno].group = 0x01;

	d3dDisp[dno].type = GTYPE_POLY;		
	d3dDisp[dno].vnum = vnum;		

	d3dDisp[dno].ps_flag = OFF;

	d3dDisp[dno].blend_param = BLEND_ALPHA;	
	d3dDisp[dno].scale_param = 0;	
	d3dDisp[dno].rev_param   = 0;		

	d3dDisp[dno].rander_no  = -1;	
	if(paint){
		d3dDisp[dno].textue_no  = -3;	
	}else{
		d3dDisp[dno].textue_no  = -4;	
	}
	d3dDisp[dno].textue_no2 = -1;	

	d3dDisp[dno].bright_no = 0;	

	*(DWORD*)&d3dDisp[dno].color[0] = D3DCOLOR_ARGB(255,255,255,255);
	*(DWORD*)&d3dDisp[dno].color[1] = D3DCOLOR_ARGB(255,255,255,255);
	*(DWORD*)&d3dDisp[dno].color[2] = D3DCOLOR_ARGB(255,255,255,255);
	*(DWORD*)&d3dDisp[dno].color[3] = D3DCOLOR_ARGB(255,255,255,255);
	*(DWORD*)&d3dDisp[dno].specular[0] = D3DCOLOR_ARGB(255,0,0,0);
	*(DWORD*)&d3dDisp[dno].specular[1] = D3DCOLOR_ARGB(255,0,0,0);
	*(DWORD*)&d3dDisp[dno].specular[2] = D3DCOLOR_ARGB(255,0,0,0);
	*(DWORD*)&d3dDisp[dno].specular[3] = D3DCOLOR_ARGB(255,0,0,0);

	d3dDisp[dno].zoom_x   = 1.0f;
	d3dDisp[dno].zoom_y   = 1.0f;
	d3dDisp[dno].kaiten = 0.0f;
	d3dDisp[dno].cx = 0;
	d3dDisp[dno].cy = 0;

	d3dDisp[dno].dx[0]=0;						d3dDisp[dno].dy[0]=0;	
	d3dDisp[dno].dx[1]=(float)BackSufaceSizeW;	d3dDisp[dno].dy[1]=0;
	d3dDisp[dno].dx[2]=0;						d3dDisp[dno].dy[2]=(float)BackSufaceSizeH;
	d3dDisp[dno].dx[3]=(float)BackSufaceSizeW;	d3dDisp[dno].dy[3]=(float)BackSufaceSizeH;

	d3dDisp[dno].rhw[0] = d3dDisp[dno].rhw[1] = d3dDisp[dno].rhw[2] = d3dDisp[dno].rhw[3] = 1.0f;

	d3dDisp[dno].sx1[0]=0;	d3dDisp[dno].sy1[0]=0;
	d3dDisp[dno].sx1[1]=0;	d3dDisp[dno].sy1[1]=0;

	d3dDisp[dno].sx2[0]=0; d3dDisp[dno].sy2[0]=0;
	d3dDisp[dno].sx2[1]=0; d3dDisp[dno].sy2[1]=0;
}

void D3DD_SetGraphGrid( int dno, int grid, int layer, int disp )
{
	D3DD_ResetGraph(dno);
	d3dDisp[dno].flag  = ON;		
	d3dDisp[dno].disp  = disp;		
	d3dDisp[dno].layer = layer;		
	d3dDisp[dno].group = 0x01;

	d3dDisp[dno].type = GTYPE_GRID;	
	d3dDisp[dno].vnum = 2;		

	d3dDisp[dno].ps_flag = OFF;

	d3dDisp[dno].blend_param = 0;	
	d3dDisp[dno].scale_param = grid;	
	d3dDisp[dno].rev_param   = 0;		

	d3dDisp[dno].rander_no  = -1;	
	d3dDisp[dno].textue_no  = -1;	
	d3dDisp[dno].textue_no2 = -1;	

	d3dDisp[dno].bright_no = 0;	

	*(DWORD*)&d3dDisp[dno].color[0] = D3DCOLOR_ARGB(255,255,255,255);
	*(DWORD*)&d3dDisp[dno].color[1] = D3DCOLOR_ARGB(255,255,255,255);
	*(DWORD*)&d3dDisp[dno].color[2] = D3DCOLOR_ARGB(255,255,255,255);
	*(DWORD*)&d3dDisp[dno].color[3] = D3DCOLOR_ARGB(255,255,255,255);
	*(DWORD*)&d3dDisp[dno].specular[0] = D3DCOLOR_ARGB(255,0,0,0);
	*(DWORD*)&d3dDisp[dno].specular[1] = D3DCOLOR_ARGB(255,0,0,0);
	*(DWORD*)&d3dDisp[dno].specular[2] = D3DCOLOR_ARGB(255,0,0,0);
	*(DWORD*)&d3dDisp[dno].specular[3] = D3DCOLOR_ARGB(255,0,0,0);

	d3dDisp[dno].zoom_x   = 1.0f;
	d3dDisp[dno].zoom_y   = 1.0f;
	d3dDisp[dno].kaiten = 0.0f;
	d3dDisp[dno].cx = 0;
	d3dDisp[dno].cy = 0;

	d3dDisp[dno].dx[0]=0;						d3dDisp[dno].dy[0]=0;	
	d3dDisp[dno].dx[1]=(float)BackSufaceSizeW;	d3dDisp[dno].dy[1]=0;
	d3dDisp[dno].dx[2]=0;						d3dDisp[dno].dy[2]=(float)BackSufaceSizeH;
	d3dDisp[dno].dx[3]=(float)BackSufaceSizeW;	d3dDisp[dno].dy[3]=(float)BackSufaceSizeH;

	d3dDisp[dno].rhw[0] = d3dDisp[dno].rhw[1] = d3dDisp[dno].rhw[2] = d3dDisp[dno].rhw[3] = 1.0f;

	d3dDisp[dno].sx1[0]=0;	d3dDisp[dno].sy1[0]=0;
	d3dDisp[dno].sx1[1]=0;	d3dDisp[dno].sy1[1]=0;

	d3dDisp[dno].sx2[0]=0; d3dDisp[dno].sy2[0]=0;
	d3dDisp[dno].sx2[1]=0; d3dDisp[dno].sy2[1]=0;
}

void D3DD_SetGraphText( int dno, int tno, int layer, int disp )
{
	D3DD_ResetGraph(dno);

	d3dDisp[dno].flag  = ON;		
	d3dDisp[dno].disp  = disp;		
	d3dDisp[dno].layer = layer;		
	d3dDisp[dno].group = 0x01;

	d3dDisp[dno].type = GTYPE_TEXT;		
	d3dDisp[dno].vnum = 4;					

	d3dDisp[dno].ps_flag = OFF;

	d3dDisp[dno].blend_param = BLEND_ALPHA;	
	d3dDisp[dno].scale_param = SCALE_POINT;	
	d3dDisp[dno].rev_param   = REVERSE_NORMAL;	

	d3dDisp[dno].rander_no  = -1;	
	d3dDisp[dno].textue_no  = tno;	
	d3dDisp[dno].textue_no2 = -1;	

	d3dDisp[dno].bright_no = 0;	

	*(DWORD*)&d3dDisp[dno].color[0] = D3DCOLOR_ARGB(255,255,255,255);
	*(DWORD*)&d3dDisp[dno].color[1] = D3DCOLOR_ARGB(255,255,255,255);
	*(DWORD*)&d3dDisp[dno].color[2] = D3DCOLOR_ARGB(255,255,255,255);
	*(DWORD*)&d3dDisp[dno].color[3] = D3DCOLOR_ARGB(255,255,255,255);
	*(DWORD*)&d3dDisp[dno].specular[0] = D3DCOLOR_ARGB(255,0,0,0);
	*(DWORD*)&d3dDisp[dno].specular[1] = D3DCOLOR_ARGB(255,0,0,0);
	*(DWORD*)&d3dDisp[dno].specular[2] = D3DCOLOR_ARGB(255,0,0,0);
	*(DWORD*)&d3dDisp[dno].specular[3] = D3DCOLOR_ARGB(255,0,0,0);

	d3dDisp[dno].zoom_x   = 1.0f;
	d3dDisp[dno].zoom_y   = 1.0f;
	d3dDisp[dno].kaiten = 0.0f;
	d3dDisp[dno].cx = 0;
	d3dDisp[dno].cy = 0;
	d3dDisp[dno].dx[0]=0;							d3dDisp[dno].dy[0]=0;	
	d3dDisp[dno].dx[1]=(float)d3dTexture[tno].w;	d3dDisp[dno].dy[1]=0;
	d3dDisp[dno].dx[2]=0;							d3dDisp[dno].dy[2]=(float)d3dTexture[tno].h;
	d3dDisp[dno].dx[3]=(float)d3dTexture[tno].w;	d3dDisp[dno].dy[3]=(float)d3dTexture[tno].h;

	d3dDisp[dno].rhw[0] = d3dDisp[dno].rhw[1] = d3dDisp[dno].rhw[2] = d3dDisp[dno].rhw[3] = 1.0f;

	d3dDisp[dno].sx1[0]=0;							d3dDisp[dno].sy1[0]=0;
	d3dDisp[dno].sx1[1]=(float)d3dTexture[tno].w;	d3dDisp[dno].sy1[1]=(float)d3dTexture[tno].h;

	d3dDisp[dno].sx2[0]=0; d3dDisp[dno].sy2[0]=0;
	d3dDisp[dno].sx2[1]=0; d3dDisp[dno].sy2[1]=0;
}

void D3DD_SetGraphStr( int dno, int tno, int layer, int disp, char *str )
{
	D3DD_ResetGraph(dno);

	d3dDisp[dno].flag  = ON;		
	d3dDisp[dno].disp  = disp;		
	d3dDisp[dno].layer = layer;		
	d3dDisp[dno].group = 0x01;

	d3dDisp[dno].str = (BYTE*)GAlloc( strlen(str)+1 );
	strcpy( (char*)d3dDisp[dno].str, str );

	d3dDisp[dno].type = GTYPE_STR;		
	d3dDisp[dno].vnum = 4;					

	d3dDisp[dno].ps_flag = OFF;

	d3dDisp[dno].blend_param = BLEND_ALPHA;	
	d3dDisp[dno].scale_param = SCALE_POINT;	
	d3dDisp[dno].rev_param   = REVERSE_NORMAL;	

	d3dDisp[dno].rander_no  = -1;	
	d3dDisp[dno].textue_no  = tno;	
	d3dDisp[dno].textue_no2 = -1;	

	d3dDisp[dno].bright_no = 0;	

	*(DWORD*)&d3dDisp[dno].color[0] = D3DCOLOR_ARGB(255,255,255,255);
	*(DWORD*)&d3dDisp[dno].color[1] = D3DCOLOR_ARGB(255,255,255,255);
	*(DWORD*)&d3dDisp[dno].color[2] = D3DCOLOR_ARGB(255,255,255,255);
	*(DWORD*)&d3dDisp[dno].color[3] = D3DCOLOR_ARGB(255,255,255,255);
	*(DWORD*)&d3dDisp[dno].specular[0] = D3DCOLOR_ARGB(255,0,0,0);
	*(DWORD*)&d3dDisp[dno].specular[1] = D3DCOLOR_ARGB(255,0,0,0);
	*(DWORD*)&d3dDisp[dno].specular[2] = D3DCOLOR_ARGB(255,0,0,0);
	*(DWORD*)&d3dDisp[dno].specular[3] = D3DCOLOR_ARGB(255,0,0,0);

	d3dDisp[dno].zoom_x   = 1.0f;
	d3dDisp[dno].zoom_y   = 1.0f;
	d3dDisp[dno].kaiten = 0.0f;
	d3dDisp[dno].cx = 0;
	d3dDisp[dno].cy = 0;
	d3dDisp[dno].dx[0]=0;							d3dDisp[dno].dy[0]=0;	
	d3dDisp[dno].dx[1]=(float)d3dTexture[tno].w/10;	d3dDisp[dno].dy[1]=0;
	d3dDisp[dno].dx[2]=0;							d3dDisp[dno].dy[2]=(float)d3dTexture[tno].h;
	d3dDisp[dno].dx[3]=(float)d3dTexture[tno].w/10;	d3dDisp[dno].dy[3]=(float)d3dTexture[tno].h;

	d3dDisp[dno].rhw[0] = d3dDisp[dno].rhw[1] = d3dDisp[dno].rhw[2] = d3dDisp[dno].rhw[3] = 1.0f;

	d3dDisp[dno].sx1[0]=0;							d3dDisp[dno].sy1[0]=0;
	d3dDisp[dno].sx1[1]=(float)d3dTexture[tno].w;	d3dDisp[dno].sy1[1]=(float)d3dTexture[tno].h;

	d3dDisp[dno].sx2[0]=0; d3dDisp[dno].sy2[0]=0;
	d3dDisp[dno].sx2[1]=0; d3dDisp[dno].sy2[1]=0;
}

void D3DD_SetGraphStrChange( int dno, char *str )
{
	if(d3dDisp[dno].flag){
		GFree( d3dDisp[dno].str );
		d3dDisp[dno].str = (BYTE*)GAlloc( strlen(str)+1 );
		strcpy( (char*)d3dDisp[dno].str, str );
	}
}
void D3DD_SetGraphDigit( int dno, int tno, int layer, int disp, int digit, int keta )
{
	D3DD_ResetGraph(dno);

	d3dDisp[dno].flag  = ON;		
	d3dDisp[dno].disp  = disp;		
	d3dDisp[dno].layer = layer;		
	d3dDisp[dno].group = 0x01;

	d3dDisp[dno].digit = digit;
	d3dDisp[dno].keta  = keta;

	d3dDisp[dno].type = GTYPE_DIGIT;		
	d3dDisp[dno].vnum = 4;					

	d3dDisp[dno].ps_flag = OFF;

	d3dDisp[dno].blend_param = BLEND_ALPHA;	
	d3dDisp[dno].scale_param = SCALE_POINT;	
	d3dDisp[dno].rev_param   = REVERSE_NORMAL;	

	d3dDisp[dno].rander_no  = -1;	
	d3dDisp[dno].textue_no  = tno;	
	d3dDisp[dno].textue_no2 = -1;	

	d3dDisp[dno].bright_no = 0;	

	*(DWORD*)&d3dDisp[dno].color[0] = D3DCOLOR_ARGB(255,255,255,255);
	*(DWORD*)&d3dDisp[dno].color[1] = D3DCOLOR_ARGB(255,255,255,255);
	*(DWORD*)&d3dDisp[dno].color[2] = D3DCOLOR_ARGB(255,255,255,255);
	*(DWORD*)&d3dDisp[dno].color[3] = D3DCOLOR_ARGB(255,255,255,255);
	*(DWORD*)&d3dDisp[dno].specular[0] = D3DCOLOR_ARGB(255,0,0,0);
	*(DWORD*)&d3dDisp[dno].specular[1] = D3DCOLOR_ARGB(255,0,0,0);
	*(DWORD*)&d3dDisp[dno].specular[2] = D3DCOLOR_ARGB(255,0,0,0);
	*(DWORD*)&d3dDisp[dno].specular[3] = D3DCOLOR_ARGB(255,0,0,0);

	d3dDisp[dno].zoom_x   = 1.0f;
	d3dDisp[dno].zoom_y   = 1.0f;
	d3dDisp[dno].kaiten = 0.0f;
	d3dDisp[dno].cx = 0;
	d3dDisp[dno].cy = 0;
	d3dDisp[dno].dx[0]=0;							d3dDisp[dno].dy[0]=0;	
	d3dDisp[dno].dx[1]=(float)d3dTexture[tno].w/10;	d3dDisp[dno].dy[1]=0;
	d3dDisp[dno].dx[2]=0;							d3dDisp[dno].dy[2]=(float)d3dTexture[tno].h;
	d3dDisp[dno].dx[3]=(float)d3dTexture[tno].w/10;	d3dDisp[dno].dy[3]=(float)d3dTexture[tno].h;

	d3dDisp[dno].rhw[0] = d3dDisp[dno].rhw[1] = d3dDisp[dno].rhw[2] = d3dDisp[dno].rhw[3] = 1.0f;

	d3dDisp[dno].sx1[0]=0;							d3dDisp[dno].sy1[0]=0;
	d3dDisp[dno].sx1[1]=(float)d3dTexture[tno].w;	d3dDisp[dno].sy1[1]=(float)d3dTexture[tno].h;

	d3dDisp[dno].sx2[0]=0; d3dDisp[dno].sy2[0]=0;
	d3dDisp[dno].sx2[1]=0; d3dDisp[dno].sy2[1]=0;
}


void D3DD_SetGraphDigitNo( int dno, int digit )
{
	d3dDisp[dno].digit = digit;
}


void D3DD_SetGraphRenderNo( int dno, int rno )
{
	d3dDisp[dno].rander_no = rno;
}


void D3DD_SetGraphLayer( int dno, int layer )
{
	d3dDisp[dno].layer = layer;
}

void D3DD_SetGraphGroup( int dno, DWORD group )
{
	d3dDisp[dno].group = group;
}


void D3DD_SetGraphPointPos( int dno, int pno, float dx, float dy, float rhw )
{
	d3dDisp[dno].dx[pno]=dx;	d3dDisp[dno].dy[pno]=dy;	d3dDisp[dno].rhw[pno]=rhw;
}

void D3DD_SetGraphRect( int dno, float dx, float dy,  float sx, float sy, float w, float h )
{
	if( d3dDisp[dno].type==GTYPE_SPRITE ){
		d3dDisp[dno].cx = dx;
		d3dDisp[dno].cy = dy;
	}

	d3dDisp[dno].dx[0]=dx;		d3dDisp[dno].dy[0]=dy;	
	d3dDisp[dno].dx[1]=dx+w;	d3dDisp[dno].dy[1]=dy;
	d3dDisp[dno].dx[2]=dx;		d3dDisp[dno].dy[2]=dy+h;
	d3dDisp[dno].dx[3]=dx+w;	d3dDisp[dno].dy[3]=dy+h;

	d3dDisp[dno].sx1[0]=sx;		d3dDisp[dno].sy1[0]=sy;
	d3dDisp[dno].sx1[1]=sx+w;	d3dDisp[dno].sy1[1]=sy+h;
}

void D3DD_SetGraphDstRect( int dno, float dx, float dy, float dw, float dh )
{
	if( d3dDisp[dno].type==GTYPE_SPRITE ){
		d3dDisp[dno].cx = dx;
		d3dDisp[dno].cy = dy;
	}
	d3dDisp[dno].dx[0]=dx;		d3dDisp[dno].dy[0]=dy;	
	d3dDisp[dno].dx[1]=dx+dw;	d3dDisp[dno].dy[1]=dy;
	d3dDisp[dno].dx[2]=dx;		d3dDisp[dno].dy[2]=dy+dh;
	d3dDisp[dno].dx[3]=dx+dw;	d3dDisp[dno].dy[3]=dy+dh;
}

void D3DD_SetGraphSrcRect( int dno, float sx, float sy, float sw, float sh )
{
	d3dDisp[dno].sx1[0]=sx;		d3dDisp[dno].sy1[0]=sy;
	d3dDisp[dno].sx1[1]=sx+sw;	d3dDisp[dno].sy1[1]=sy+sh;
}
void D3DD_SetGraphSrc2Rect( int dno, float sx, float sy, float sw, float sh )
{
	d3dDisp[dno].sx2[0]=sx;		d3dDisp[dno].sy2[0]=sy;
	d3dDisp[dno].sx2[1]=sx+sw;	d3dDisp[dno].sy2[1]=sy+sh;
}
void D3DD_SetGraphDst( int dno, float dx1, float dy1, float dx2, float dy2, float dx3, float dy3, float dx4, float dy4 )
{
	d3dDisp[dno].dx[0]=dx1;	d3dDisp[dno].dy[0]=dy1;	
	d3dDisp[dno].dx[1]=dx2;	d3dDisp[dno].dy[1]=dy2;
	d3dDisp[dno].dx[2]=dx3;	d3dDisp[dno].dy[2]=dy3;
	d3dDisp[dno].dx[3]=dx4;	d3dDisp[dno].dy[3]=dy4;
}

void D3DD_SetGraphRoll2( int dno, float cx, float cy, float dw, float dh, float count )
{
	double	radian;

	radian = (count+  0)/360*PAI*2;
	D3DD_SetGraphPointPos( dno, 0, (float)(cx+cos(radian)*dw), (float)(cy+sin(radian)*dh), 1.0f );

	radian = (count+ 90)/360*PAI*2;
	D3DD_SetGraphPointPos( dno, 1, (float)(cx+cos(radian)*dw), (float)(cy+sin(radian)*dh), 1.0f );

	radian = (count+270)/360*PAI*2;
	D3DD_SetGraphPointPos( dno, 2, (float)(cx+cos(radian)*dw), (float)(cy+sin(radian)*dh), 1.0f );

	radian = (count+180)/360*PAI*2;
	D3DD_SetGraphPointPos( dno, 3, (float)(cx+cos(radian)*dw), (float)(cy+sin(radian)*dh), 1.0f );
}




BOOL D3DD_SetGraphTextStep( int dno, int step )
{
	D3DD_TEXT	*d3dtx = &d3dText[d3dTexture[d3dDisp[dno].textue_no].index_no];

	d3dtx->step=step;

	return d3dtx->step_wait;
}
BOOL D3DD_SetGraphTextCount( int dno, int count )
{
	D3DD_TEXT	*d3dtx = &d3dText[d3dTexture[d3dDisp[dno].textue_no].index_no];

	d3dtx->count=count;

	return d3dtx->wait;
}


void D3DD_SetGraphDstMove( int dno, float mx, float my )
{
	if( d3dDisp[dno].type==GTYPE_SPRITE ){
		d3dDisp[dno].cx = mx;
		d3dDisp[dno].cy = my;
	}

	d3dDisp[dno].dx[3] += mx-d3dDisp[dno].dx[0];
	d3dDisp[dno].dx[2] += mx-d3dDisp[dno].dx[0];
	d3dDisp[dno].dx[1] += mx-d3dDisp[dno].dx[0];
	d3dDisp[dno].dx[0] = mx;

	d3dDisp[dno].dy[3] += my-d3dDisp[dno].dy[0];
	d3dDisp[dno].dy[2] += my-d3dDisp[dno].dy[0];
	d3dDisp[dno].dy[1] += my-d3dDisp[dno].dy[0];
	d3dDisp[dno].dy[0] = my;
}


void D3DD_SetGraphDstMoveWH( int dno, float dw, float dh )
{
	float dx = d3dDisp[dno].dx[0];
	float dy = d3dDisp[dno].dy[0];
	d3dDisp[dno].dx[1]=dx+dw;
	d3dDisp[dno].dy[2]=dy+dh;
	d3dDisp[dno].dx[3]=dx+dw;	d3dDisp[dno].dy[3]=dy+dh;
}

void D3DD_SetGraphCenter( int dno, float cx, float cy )
{
	d3dDisp[dno].cx = cx;
	d3dDisp[dno].cy = cy;
}
void D3DD_SetGraphRoll( int dno, float kaiten )
{
	d3dDisp[dno].kaiten = kaiten;
}
void D3DD_SetGraphZoom( int dno, float zoom )
{
	d3dDisp[dno].zoom_x = zoom;
	d3dDisp[dno].zoom_y = zoom;
}

void D3DD_SetGraphZoomXY( int dno, float zoom_x, float zoom_y )
{
	d3dDisp[dno].zoom_x = zoom_x;
	d3dDisp[dno].zoom_y = zoom_y;
}


void D3DD_SetGraphDisp( int dno, char disp )
{
	d3dDisp[dno].disp = disp;
}


void D3DD_SetGraphSrc( int dno, int tno )
{
	d3dDisp[dno].textue_no = tno;	
}
void D3DD_SetGraphSrc2( int dno, int tno2, int param )
{
	d3dDisp[dno].textue_no2 = tno2;	
	d3dDisp[dno].tex2_param = param;
}
void D3DD_ResetGraphSrc2( int dno )
{
	if(d3dDisp[dno].textue_no != -1 && d3dDisp[dno].textue_no2 != -1){
		d3dDisp[dno].textue_no2 = -1;	
		D3DD_TEXTURE	*d3dTex = &d3dTexture[d3dDisp[dno].textue_no];
		if( d3dTex->format != D3DFMT_A8R8G8B8 ) return ;
		int				x,y,xx,yy;

		for(y=0;y<d3dTex->pitch_h;y++){
			for(x=0;x<d3dTex->pitch_w;x++){
				D3DLOCKED_RECT	d3dlr;
				if( d3dTex->texture[y*d3dTex->pitch_w+x]->LockRect( 0, &d3dlr, NULL, D3DLOCK_NOSYSLOCK )==D3D_OK ){
					D3DD_ARGB	*pBits = (D3DD_ARGB*)d3dlr.pBits;

					for( yy=0 ; yy<d3dTex->grid_h ; yy++ ){
						for( xx=0 ; xx<d3dTex->grid_w ; xx++, pBits++ ){
							pBits->a=255;
						}
					}
					d3dTex->texture[y*d3dTex->pitch_w+x]->UnlockRect(0);
				}
			}
		}
	}
}
void D3DD_SetGraphPixelFade( int dno, int ps_no, float c0_alpha )
{
	if( PixelShader[ps_no] ){
		d3dDisp[dno].ps_flag  = TRUE;
		d3dDisp[dno].ps.psno  = ps_no;
		d3dDisp[dno].ps.c0[0] = 0;
		d3dDisp[dno].ps.c0[1] = 0;
		d3dDisp[dno].ps.c0[2] = 0;
		d3dDisp[dno].ps.c0[3] = c0_alpha;
	}
}
void D3DD_SetGraphPixelShader( int dno, int ps_no, int c, float r, float g, float b, float a )
{
	if( PixelShader[ps_no] ){
		d3dDisp[dno].ps_flag  = TRUE;
		d3dDisp[dno].ps.psno  = ps_no;
		if(c==0){
			d3dDisp[dno].ps.c0[0] = r;
			d3dDisp[dno].ps.c0[1] = g;
			d3dDisp[dno].ps.c0[2] = b;
			d3dDisp[dno].ps.c0[3] = a;
		}else{
			d3dDisp[dno].ps.c1[0] = r;
			d3dDisp[dno].ps.c1[1] = g;
			d3dDisp[dno].ps.c1[2] = b;
			d3dDisp[dno].ps.c1[3] = a;
		}
	}
}

void D3DD_SetGraphBlendParam( int dno, int bparam )
{
	d3dDisp[dno].blend_param=bparam;
}

void D3DD_SetGraphScaleParam( int dno, int sparam )
{
	d3dDisp[dno].scale_param=sparam;
}

void D3DD_SetGraphRevParam( int dno, int rparam )
{
	d3dDisp[dno].rev_param=rparam;
}

void D3DD_SetGraphColor( int dno, int a, int r, int g, int b )
{
	int	sr=0,sg=0,sb=0;
	if( d3dDisp[dno].textue_no>=0 ){
		if(r>=128){
			sr = min( (r-128)*2, 255 );
			r=255;
		}else{
			r=max(0,r*2);
		}
		if(g>=128){
			sg = min( (g-128)*2, 255 );
			g=255;
		}else{
			g=max(0,g*2);
		}
		if(b>=128){
			sb = min( (b-128)*2, 255 );
			b=255;
		}else{
			b=max(0,b*2);
		}
	}
	a = LIM(a,0,255);


	*(DWORD*)&d3dDisp[dno].specular[0] = 
	*(DWORD*)&d3dDisp[dno].specular[1] = 
	*(DWORD*)&d3dDisp[dno].specular[2] = 
	*(DWORD*)&d3dDisp[dno].specular[3] = D3DCOLOR_ARGB( 255, sr, sg, sb );

	*(DWORD*)&d3dDisp[dno].color[0] = 
	*(DWORD*)&d3dDisp[dno].color[1] = 
	*(DWORD*)&d3dDisp[dno].color[2] = 
	*(DWORD*)&d3dDisp[dno].color[3] = D3DCOLOR_ARGB( a, r, g, b );
}
void D3DD_SetGraphColorNo( int dno, int cno, int a, int r, int g, int b )
{
	int	sr=0,sg=0,sb=0;
	if( d3dDisp[dno].textue_no>=0 ){
		if(r>=128){
			sr = min( (r-128)*2, 255 );
			r=255;
		}else{
			r=max(0,r*2);
		}
		if(g>=128){
			sg = min( (g-128)*2, 255 );
			g=255;
		}else{
			g=max(0,g*2);
		}
		if(b>=128){
			sb = min( (b-128)*2, 255 );
			b=255;
		}else{
			b=max(0,b*2);
		}
	}
	a=LIM(a,0,255);

	*(DWORD*)&d3dDisp[dno].specular[cno] = D3DCOLOR_ARGB( 255, sr, sg, sb );
	*(DWORD*)&d3dDisp[dno].color[cno] = D3DCOLOR_ARGB( a, r, g, b );
}
void D3DD_SetGraphFade( int dno, int fade )
{
	D3DD_SetGraphColorNo( dno, 0, d3dDisp[dno].color[0].a, fade, fade, fade );
	D3DD_SetGraphColorNo( dno, 1, d3dDisp[dno].color[1].a, fade, fade, fade );
	D3DD_SetGraphColorNo( dno, 2, d3dDisp[dno].color[2].a, fade, fade, fade );
	D3DD_SetGraphColorNo( dno, 3, d3dDisp[dno].color[3].a, fade, fade, fade );
}
void D3DD_SetGraphColor2( int dno, int r, int g, int b )
{
	D3DD_SetGraphColorNo( dno, 0, d3dDisp[dno].color[0].a, r, g, b );
	D3DD_SetGraphColorNo( dno, 1, d3dDisp[dno].color[1].a, r, g, b );
	D3DD_SetGraphColorNo( dno, 2, d3dDisp[dno].color[2].a, r, g, b );
	D3DD_SetGraphColorNo( dno, 3, d3dDisp[dno].color[3].a, r, g, b );
}

void D3DD_SetGraphAlpha( int dno, int a )
{
	a=LIM(a,0,255);
	*(DWORD*)&d3dDisp[dno].color[0] = D3DCOLOR_ARGB( a, d3dDisp[dno].color[0].r, d3dDisp[dno].color[0].g, d3dDisp[dno].color[0].b );
	*(DWORD*)&d3dDisp[dno].color[1] = D3DCOLOR_ARGB( a, d3dDisp[dno].color[1].r, d3dDisp[dno].color[1].g, d3dDisp[dno].color[1].b );
	*(DWORD*)&d3dDisp[dno].color[2] = D3DCOLOR_ARGB( a, d3dDisp[dno].color[2].r, d3dDisp[dno].color[2].g, d3dDisp[dno].color[2].b );
	*(DWORD*)&d3dDisp[dno].color[3] = D3DCOLOR_ARGB( a, d3dDisp[dno].color[3].r, d3dDisp[dno].color[3].g, d3dDisp[dno].color[3].b );
}

void D3DD_SetGraphBaseColor( int dno, int r, int g, int b )
{
	r=LIM(r,0,255);
	g=LIM(g,0,255);
	b=LIM(b,0,255);

	*(DWORD*)&d3dDisp[dno].color[0] = D3DCOLOR_ARGB( d3dDisp[dno].color[0].a, r, g, b );
	*(DWORD*)&d3dDisp[dno].color[1] = D3DCOLOR_ARGB( d3dDisp[dno].color[1].a, r, g, b );
	*(DWORD*)&d3dDisp[dno].color[2] = D3DCOLOR_ARGB( d3dDisp[dno].color[2].a, r, g, b );
	*(DWORD*)&d3dDisp[dno].color[3] = D3DCOLOR_ARGB( d3dDisp[dno].color[3].a, r, g, b );
}
void D3DD_SetGraphSpecular( int dno, int sr, int sg, int sb )
{
	sr=LIM(sr,0,255);
	sg=LIM(sg,0,255);
	sb=LIM(sb,0,255);

	*(DWORD*)&d3dDisp[dno].specular[0] = 
	*(DWORD*)&d3dDisp[dno].specular[1] = 
	*(DWORD*)&d3dDisp[dno].specular[2] = 
	*(DWORD*)&d3dDisp[dno].specular[3] = D3DCOLOR_ARGB( 255, sr, sg, sb );
}

void D3DD_SetGraphBrightNo( int dno, int bright_no )
{
	d3dDisp[dno].bright_no= bright_no;
}

void D3DD_SetGlobalColor( int r, int g, int b, int type, int no )
{
	GlobalColor[0].r = LIM(r,0,255);
	GlobalColor[0].g = LIM(g,0,255);
	GlobalColor[0].b = LIM(b,0,255);
	GlobalColor[0].type = type;
}


int D3DD_DrawTexture( D3DD_DISP *disp, D3DD_TEXTURE *exTex )
{
	int i,j;
	D3DD_TEXTURE	*d3dTex=NULL;
	D3DD_TEXTURE	*d3dTex2=NULL;

	if(disp==NULL) return 0;
	if(exTex)	d3dTex = exTex;
	else{
		if(disp->textue_no>=0){

			d3dTex = &d3dTexture[disp->textue_no];
			if(d3dTex->type==GTYPE_MOVIE){
	
				D3DD_UpdateAviTexture( disp->textue_no );
			}
		}
	}



	if(disp->rander_no>=0){
		switch(d3dTexture[disp->rander_no].render_flag){
		case RENDER_OFF:	
			break;
		case RENDER_RESET:	
			d3dTexture[disp->rander_no].render_flag = RENDER_SET;
			if( D3DXCreateTexture( pD3DDevice, d3dTexture[disp->rander_no].w, d3dTexture[disp->rander_no].h, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &d3dTexture[disp->rander_no].texture[0] )!=D3D_OK){
				DebugBox(NULL,"レンダリングターゲットを生成出来ません");;
				break;
			}

			break;
		case RENDER_SET:	
			break;
		}
	}

	static int before_render=-1;
	if(disp->rander_no!=before_render){
		LPDIRECT3DSURFACE8	pRenderTarget=NULL;
		if(disp->rander_no>=0){
			if(d3dTexture[disp->rander_no].texture){
				d3dTexture[disp->rander_no].texture[0]->GetSurfaceLevel(0,&pRenderTarget );
			}else{
				pD3DDevice->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pRenderTarget );
			}
		}else{
			pD3DDevice->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pRenderTarget );
		}
		pD3DDevice->SetRenderTarget(pRenderTarget,NULL);
		RELEASE_3D( pRenderTarget );
	}
	before_render = disp->rander_no;


	
	pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE );
	pD3DDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD);
	switch(disp->blend_param){
		default:
		case BLEND_NONE:
		
			pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE );
			pD3DDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
			pD3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );
			break;

		case BLEND_ALPHA:
		case BLEND_PIX_FADE:
		
			pD3DDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
			pD3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			break;

		case BLEND_ALPHA2:
			pD3DDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
			pD3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			break;

		case BLEND_ADD:
		
			pD3DDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );	
			pD3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );		
			break;
		case BLEND_SCREEN:
		
			pD3DDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
			pD3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );
			break;
		case BLEND_HARDLIGHT:
		
			pD3DDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_DESTCOLOR );
			pD3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR );


			break;

		case BLEND_MUL:
		
			pD3DDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ZERO );
			pD3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR );
			break;
		case BLEND_SUB:
		
			if( D3DCapsStruct.m_d3dCaps.PrimitiveMiscCaps&D3DPMISCCAPS_BLENDOP ){
				pD3DDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_SUBTRACT);
				pD3DDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );	
				pD3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );		
			}else{
				pD3DDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ZERO );
				pD3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR );
			}
			break;
		case BLEND_REVMUL:
		
			pD3DDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ZERO );			
			pD3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );	
			break;
		case BLEND_REVSUB:
		
			if( D3DCapsStruct.m_d3dCaps.PrimitiveMiscCaps&D3DPMISCCAPS_BLENDOP ){
				pD3DDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT);
				pD3DDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );	
				pD3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );		
			}else{
				pD3DDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ZERO );			
				pD3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );	
			}
			break;

		case BLEND_EXCLUDE:
		
			pD3DDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_INVDESTCOLOR );
			pD3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );
			break;
	}


	float	kcos = (float)cos( D3DXToRadian( disp->kaiten ) );
	float	ksin = (float)sin( D3DXToRadian( disp->kaiten ) );

	float	dx[4], dy[4];
	for(i=0;i<4;i++){
		float	rdx, rdy;
		if( disp->rev_param&REVERSE_W )	rdx = -(disp->dx[i]-disp->cx);
		else							rdx =   disp->dx[i]-disp->cx;
		if( disp->rev_param&REVERSE_H )	rdy = -(disp->dy[i]-disp->cy);
		else							rdy =   disp->dy[i]-disp->cy;

		dx[i] = disp->cx + (  (rdx)*kcos + (rdy)*ksin )*disp->zoom_x;
		dy[i] = disp->cy + ( -(rdx)*ksin + (rdy)*kcos )*disp->zoom_y;
	}

	D3DD_ARGB	color[4];
	D3DD_ARGB	spclr[4];

	for(i=0;i<4;i++){
		color[i] = disp->color[i];
		spclr[i] = disp->specular[i];
		int	r,g,b, sr,sg,sb;


		
		switch(GlobalColor[disp->bright_no].type){
			case 0:
				if(GlobalColor[disp->bright_no].r<=128){
					r = GlobalColor[disp->bright_no].r*(color[i].r+spclr[i].r)/128;
				}else{
					r = color[i].r+spclr[i].r;
					r = r+(GlobalColor[disp->bright_no].r-128)*(510-r)/127;
				}

				if(GlobalColor[disp->bright_no].g<=128){
					g = GlobalColor[disp->bright_no].g*(color[i].g+spclr[i].g)/128;
				}else{
					g = color[i].g+spclr[i].g;
					g = g+(GlobalColor[disp->bright_no].g-128)*(510-g)/127;
				}

				if(GlobalColor[disp->bright_no].b<=128){
					b = GlobalColor[disp->bright_no].b*(color[i].b+spclr[i].b)/128;
				}else{
					b = color[i].b+spclr[i].b;
					b = b+(GlobalColor[disp->bright_no].b-128)*(510-b)/127;
				}
				break;
			case 1:
				if(GlobalColor[disp->bright_no].r<=128)
					r = (color[i].r+spclr[i].r) + (GlobalColor[disp->bright_no].r*4-512);
				else
					r = (color[i].r+spclr[i].r) + (GlobalColor[disp->bright_no].r*4-510);
				if(GlobalColor[disp->bright_no].g<=128)
					g = (color[i].g+spclr[i].g) + (GlobalColor[disp->bright_no].g*4-512);
				else
					g = (color[i].g+spclr[i].g) + (GlobalColor[disp->bright_no].g*4-510);
				if(GlobalColor[disp->bright_no].b<=128)
					b = (color[i].b+spclr[i].b) + (GlobalColor[disp->bright_no].b*4-512);
				else
					b = (color[i].b+spclr[i].b) + (GlobalColor[disp->bright_no].b*4-510);
				break;
			default:
			case 2:
				break;
		}

		if( GlobalColor[disp->bright_no].type == 0 || GlobalColor[disp->bright_no].type == 1 ){
			r = LIM(r,0,510);
			g = LIM(g,0,510);
			b = LIM(b,0,510);
			if(r>=255){
				sr = min( (r-255), 255 );
				r=255;
			}else{
				sr = 0;
				r=max(0,r);
			}
			if(g>=255){
				sg = min( (g-255), 255 );
				g=255;
			}else{
				sg = 0;
				g=max(0,g);
			}
			if(b>=255){
				sb = min( (b-255), 255 );
				b=255;
			}else{
				sb = 0;
				b=max(0,b);
			}
			color[i].r = r;
			color[i].g = g;
			color[i].b = b;
			color[i].a = disp->color[i].a;
		
			spclr[i].r = sr;
			spclr[i].g = sg;
			spclr[i].b = sb;
			spclr[i].a = 255;
		}
	}


	if(disp->textue_no<0){

		TLVERTEX_NOTEX	*vertex;
		switch(disp->textue_no){
		case -1:
			vertex = (TLVERTEX_NOTEX*)GAlloc( disp->vnum*sizeof(TLVERTEX_NOTEX) );
			for(i=0;i<disp->vnum;i++){
				vertex[i].x = dx[i];	vertex[i].y = dy[i];	vertex[i].z = disp->dz;	vertex[i].rhw = 1.0f;
				vertex[i].color    = D3DCOLOR_ARGB( color[i].a, color[i].r, color[i].g, color[i].b );
			}

			pD3DDevice->SetVertexShader( FVF_TLVERTEX_NOTEX );										

			switch(disp->vnum){
				default:
				case 1:	pD3DDevice->DrawPrimitiveUP( D3DPT_POINTLIST, 1, vertex, sizeof(TLVERTEX_NOTEX) );		break;
				case 2:	pD3DDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, vertex, sizeof(TLVERTEX_NOTEX) );		break;
				case 3:	pD3DDevice->DrawPrimitiveUP( D3DPT_TRIANGLELIST, 1, vertex, sizeof(TLVERTEX_NOTEX) );	break;
				case 4:	pD3DDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, vertex, sizeof(TLVERTEX_NOTEX) );	break;
			}
			GFree(vertex);
			break;
		case -2:
			pD3DDevice->Clear(0,NULL,D3DCLEAR_TARGET,D3DCOLOR_ARGB(0,0,0,0),1.0,0);
			break;
		case -3:
		case -4:
			{
				float	ddx,ddy;
				float	dr = (dx[3]-dx[0])/2;	
				
				vertex = (TLVERTEX_NOTEX*)GAlloc( (disp->vnum+2)*sizeof(TLVERTEX_NOTEX) );
				pD3DDevice->SetVertexShader( FVF_TLVERTEX_NOTEX );										
				switch(disp->textue_no){
					default:
					case -3:
						vertex[0].x = dx[0]+dr;	vertex[0].y = dy[0]+dr;	vertex[0].z = disp->dz;	vertex[0].rhw = 1.0f;
						vertex[0].color    = D3DCOLOR_ARGB( color[0].a, color[0].r, color[0].g, color[0].b );

						for(i=0;i<disp->vnum+1;i++){
							ddx = (float)( dx[0]+dr + dr*cos(2*PAI*i/disp->vnum) );
							ddy = (float)( dy[0]+dr + dr*sin(2*PAI*i/disp->vnum) );
							vertex[i+1].x = ddx;	vertex[i+1].y = ddy;	vertex[i+1].z = disp->dz;	vertex[i+1].rhw = 1.0f;
							vertex[i+1].color    = D3DCOLOR_ARGB( color[1].a, color[1].r, color[1].g, color[1].b );
						}
						pD3DDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, disp->vnum, vertex, sizeof(TLVERTEX_NOTEX) );
						break;
					case -4:
						for(i=0;i<disp->vnum+1;i++){
							ddx = (float)( dx[0]+dr + dr*cos(2*PAI*i/disp->vnum) );
							ddy = (float)( dy[0]+dr + dr*sin(2*PAI*i/disp->vnum) );
							vertex[i].x = ddx;	vertex[i].y = ddy;	vertex[i].z = disp->dz;	vertex[i].rhw = 1.0f;
							vertex[i].color    = D3DCOLOR_ARGB( color[0].a, color[0].r, color[0].g, color[0].b );
						}

						pD3DDevice->DrawPrimitiveUP( D3DPT_LINESTRIP, disp->vnum, vertex, sizeof(TLVERTEX_NOTEX) );
						break;
				}
				GFree(vertex);
			}
			break;
		}
	}else{
		if(d3dTex==NULL) return D3D_OK;
		if(!d3dTex->flag) return D3D_OK;



			pD3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );					
		pD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

		pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
		pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

		
		switch(disp->scale_param){
		default:
		case 0:
	
	




				pD3DDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_POINT );	
				pD3DDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_POINT );	

				pD3DDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_POINT);

			break;
		case 1:
			pD3DDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );	
			pD3DDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );	

			pD3DDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );
			break;
		case 2:
			pD3DDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );
			if( D3DCapsStruct.m_d3dCaps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFGAUSSIANCUBIC ){
				pD3DDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_GAUSSIANCUBIC );	
			}else if( D3DCapsStruct.m_d3dCaps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFAFLATCUBIC ){
				pD3DDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_FLATCUBIC );	
			}else if( D3DCapsStruct.m_d3dCaps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFANISOTROPIC ){
				pD3DDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_ANISOTROPIC );	
			}else{
				pD3DDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );	
			}

			if( D3DCapsStruct.m_d3dCaps.TextureFilterCaps & D3DPTFILTERCAPS_MINFANISOTROPIC ){
				pD3DDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_ANISOTROPIC );	
			}else{
				pD3DDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );	
			}
			break;
		}
		
	
		switch( 0 ){
			default:
			case 0:
				pD3DDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
				pD3DDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
				break;
			case 1:
				pD3DDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
				pD3DDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );
				break;
			case 2:
				pD3DDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_MIRROR );
				pD3DDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_MIRROR );
				break;
		}

		float	sx1[2];		
		float	sy1[2];
		if(d3dTex->low_level){
			sx1[0] = disp->sx1[0]/2;	sx1[1] = disp->sx1[1]/2;
			sy1[0] = disp->sy1[0]/2;	sy1[1] = disp->sy1[1]/2;
		}else{
			sx1[0] = disp->sx1[0];		sx1[1] = disp->sx1[1];
			sy1[0] = disp->sy1[0];		sy1[1] = disp->sy1[1];
		}

		if(0){
			TLVERTEX2	vertex[4];
			if( d3dTexture[disp->textue_no].texture[0] && d3dTexture[disp->textue_no2].texture[0] ){
				for(i=0;i<4;i++){
					vertex[i].x = dx[i];	vertex[i].y = dy[i];	vertex[i].z = 0.0f;	vertex[i].rhw = 1.0f;

					vertex[i].tu0 =       sx1[i%2]/d3dTexture[disp->textue_no].w;	vertex[i].tv0 =       sy1[i/2]/d3dTexture[disp->textue_no].h;
					vertex[i].tu1 = disp->sx2[i%2]/d3dTexture[disp->textue_no2].w;	vertex[i].tv1 = disp->sy2[i/2]/d3dTexture[disp->textue_no2].h;
					vertex[i].color    = D3DCOLOR_ARGB( color[i].a, color[i].r, color[i].g, color[i].b );
					vertex[i].specular = D3DCOLOR_ARGB( spclr[i].a, spclr[i].r, spclr[i].g, spclr[i].b );
				}

				pD3DDevice->SetTexture( 0, d3dTexture[disp->textue_no].texture[0] );
				pD3DDevice->SetTexture( 1, d3dTexture[disp->textue_no2].texture[0] );

				if(disp->ps_flag){
					pD3DDevice->SetPixelShaderConstant( 0, disp->ps.c0, 1 );

					pD3DDevice->SetPixelShader( PixelShader[disp->ps.psno] );
				}
				pD3DDevice->SetVertexShader( FVF_TLVERTEX2 );										
				pD3DDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, vertex, sizeof(TLVERTEX2) );	

				pD3DDevice->SetTexture( 0, NULL );
				pD3DDevice->SetTexture( 1, NULL );
				if(disp->ps_flag){
					pD3DDevice->SetPixelShader( 0 );
				}
			}
		}else{
			TLVERTEX	vertex[4];
			float	sxx1[D3DD_TEXTURE_SIZE], sxx2[D3DD_TEXTURE_SIZE];		
			float	syy1[D3DD_TEXTURE_SIZE], syy2[D3DD_TEXTURE_SIZE];
			float	dxx[D3DD_TEXTURE_SIZE*D3DD_TEXTURE_SIZE], dyy[D3DD_TEXTURE_SIZE*D3DD_TEXTURE_SIZE];	
			D3DD_ARGB	cc[D3DD_TEXTURE_SIZE*D3DD_TEXTURE_SIZE];			
			D3DD_ARGB	ss[D3DD_TEXTURE_SIZE*D3DD_TEXTURE_SIZE];			

			ZeroMemory( sxx1, sizeof(float)*D3DD_TEXTURE_SIZE);
			ZeroMemory( sxx2, sizeof(float)*D3DD_TEXTURE_SIZE);
			ZeroMemory( syy1, sizeof(float)*D3DD_TEXTURE_SIZE);
			ZeroMemory( syy2, sizeof(float)*D3DD_TEXTURE_SIZE);
			for( i=0 ; i<d3dTex->pitch_w+1 ; i++ ){
				sxx1[i] = (sx1[0]-d3dTex->grid_w*i) / d3dTex->grid_w;
				sxx2[i] = (sx1[1]-d3dTex->grid_w*i) / d3dTex->grid_w;
				sxx1[i] = max(min(sxx1[i],1.0f),0.0f);
				sxx2[i] = max(min(sxx2[i],1.0f),0.0f);
			}
			for( i=0 ; i<d3dTex->pitch_h+1 ; i++ ){
				syy1[i] = (sy1[0]-d3dTex->grid_h*i) / d3dTex->grid_h;
				syy2[i] = (sy1[1]-d3dTex->grid_h*i) / d3dTex->grid_h;
				syy1[i] = max(min(syy1[i],1.0f),0.0f);
				syy2[i] = max(min(syy2[i],1.0f),0.0f);
			}

			float	count_x, count_y;	
			float	mul_x, mul_y;		
			float	top_x, end_x;		
			float	top_y, end_y;		

			float	top_a, end_a;		
			float	top_r, end_r;		
			float	top_g, end_g;		
			float	top_b, end_b;		

			float	top_sr, end_sr;		
			float	top_sg, end_sg;		
			float	top_sb, end_sb;		
			count_y = 0.0f;
			for(i=0;i<d3dTex->pitch_h+1;i++){
				mul_y = count_y*d3dTex->grid_h/(sy1[1]-sy1[0]);
				
				top_x = dx[0]+(dx[2]-dx[0])*mul_y;	end_x = dx[1]+(dx[3]-dx[1])*mul_y;
				top_y = dy[0]+(dy[2]-dy[0])*mul_y;	end_y = dy[1]+(dy[3]-dy[1])*mul_y;

				top_a = color[0].a+(color[2].a-color[0].a)*mul_y;	end_a = color[1].a+(color[3].a-color[1].a)*mul_y;
				top_r = color[0].r+(color[2].r-color[0].r)*mul_y;	end_r = color[1].r+(color[3].r-color[1].r)*mul_y;
				top_g = color[0].g+(color[2].g-color[0].g)*mul_y;	end_g = color[1].g+(color[3].g-color[1].g)*mul_y;
				top_b = color[0].b+(color[2].b-color[0].b)*mul_y;	end_b = color[1].b+(color[3].b-color[1].b)*mul_y;

				top_sr = spclr[0].r+(spclr[2].r-spclr[0].r)*mul_y;	end_sr = spclr[1].r+(spclr[3].r-spclr[1].r)*mul_y;
				top_sg = spclr[0].g+(spclr[2].g-spclr[0].g)*mul_y;	end_sg = spclr[1].g+(spclr[3].g-spclr[1].g)*mul_y;
				top_sb = spclr[0].b+(spclr[2].b-spclr[0].b)*mul_y;	end_sb = spclr[1].b+(spclr[3].b-spclr[1].b)*mul_y;

				count_x = 0.0f;
				for(j=0;j<d3dTex->pitch_w+1;j++){
					int	txy = i*D3DD_TEXTURE_SIZE+j;

					mul_x = count_x*d3dTex->grid_w/(sx1[1]-sx1[0]);

					dxx[txy] = top_x+(end_x-top_x) * mul_x;
					dyy[txy] = top_y+(end_y-top_y) * mul_x;
					
					cc[txy].a = (BYTE)(top_a+(end_a-top_a) * mul_x);
					cc[txy].r = (BYTE)(top_r+(end_r-top_r) * mul_x);
					cc[txy].g = (BYTE)(top_g+(end_g-top_g) * mul_x);
					cc[txy].b = (BYTE)(top_b+(end_b-top_b) * mul_x);

					ss[txy].r = (BYTE)(top_sr+(end_sr-top_sr) * mul_x);
					ss[txy].g = (BYTE)(top_sg+(end_sg-top_sg) * mul_x);
					ss[txy].b = (BYTE)(top_sb+(end_sb-top_sb) * mul_x);
					
					count_x += sxx2[j]-sxx1[j];
				}
				count_y += syy2[i]-syy1[i];
			}

			for(i=0;i<D3DD_TEXTURE_SIZE;i++){
				if(syy1[i]==syy2[i]) continue;
				if(i>=d3dTex->pitch_h) break;

				for(j=0;j<D3DD_TEXTURE_SIZE;j++){
					if(sxx1[j]==sxx2[j]) continue;
					if(j>=d3dTex->pitch_w) break;

					LPDIRECT3DTEXTURE8	texture = d3dTex->texture[i*d3dTex->pitch_w+j];
					int	txy = i*D3DD_TEXTURE_SIZE+j;
					if( texture ){

						BOOL	draw=FALSE;
						if( (j+1)*d3dTex->grid_w > sx1[0] && (i+1)*d3dTex->grid_h > sy1[0] &&
							(j  )*d3dTex->grid_w < sx1[1] && (i  )*d3dTex->grid_h < sy1[1] ) draw = TRUE;

						if( draw ){
							if(disp->textue_no2>=0 && d3dTex->format == D3DFMT_A8R8G8B8){
								d3dTex2 = &d3dTexture[disp->textue_no2];
								int				x,y,w,h;

								D3DLOCKED_RECT	d3dlr;
								if( texture->LockRect( 0, &d3dlr, NULL, D3DLOCK_NOSYSLOCK )==D3D_OK ){
									D3DD_ARGB	*pBits = (D3DD_ARGB*)d3dlr.pBits;
									BYTE		*pBits2 = d3dTex2->fade_txt+ i*d3dTex2->w*d3dTex->grid_h + j*d3dTex->grid_w;

									int		sikii = LIM(disp->tex2_param,1,255);
									BYTE	pfade_tbl[256];

									for(y=0;y<256;y++){
										x = (y + (color[0].a*(255+sikii)/255)-255)*255/sikii;
										x = max( x, 0 );
										pfade_tbl[y] = min( x, 255);
									}

									x = (int)(sxx1[j]*d3dTex->grid_w);
									w = (int)( (sxx2[j]*d3dTex->grid_w+j*d3dTex->grid_w > d3dTex2->w) ? d3dTex2->w-j*d3dTex->grid_w : sxx2[j]*d3dTex->grid_w );
									y = (int)(syy1[i]*d3dTex->grid_h);
									h = (int)( (syy2[i]*d3dTex->grid_h+i*d3dTex->grid_h > d3dTex2->h) ? d3dTex2->h-i*d3dTex->grid_h  : syy2[i]*d3dTex->grid_h );
									for( ;y<h;y++, pBits+=d3dTex->grid_w-w-x, pBits2+=d3dTex2->grid_w-w-x ){
										for( ;x<w;x++, pBits++, pBits2++){
											pBits->a = pfade_tbl[ *pBits2 ];
										}
										x = (int)(sxx1[j]*d3dTex->grid_w);
									}
									texture->UnlockRect(0);
								}
							}

							vertex[0].x = dxx[(i  )*D3DD_TEXTURE_SIZE+j  ]-T05F;	vertex[0].y = dyy[(i  )*D3DD_TEXTURE_SIZE+j  ]-T05F;	vertex[0].z = disp->dz;	vertex[0].rhw = disp->rhw[0];
							vertex[1].x = dxx[(i  )*D3DD_TEXTURE_SIZE+j+1]-T05F;	vertex[1].y = dyy[(i  )*D3DD_TEXTURE_SIZE+j+1]-T05F;	vertex[1].z = disp->dz;	vertex[1].rhw = disp->rhw[1];
							vertex[2].x = dxx[(i+1)*D3DD_TEXTURE_SIZE+j  ]-T05F;	vertex[2].y = dyy[(i+1)*D3DD_TEXTURE_SIZE+j  ]-T05F;	vertex[2].z = disp->dz;	vertex[2].rhw = disp->rhw[2];
							vertex[3].x = dxx[(i+1)*D3DD_TEXTURE_SIZE+j+1]-T05F;	vertex[3].y = dyy[(i+1)*D3DD_TEXTURE_SIZE+j+1]-T05F;	vertex[3].z = disp->dz;	vertex[3].rhw = disp->rhw[3];




							vertex[0].tu = sxx1[j];	vertex[0].tv = syy1[i];
							vertex[1].tu = sxx2[j];	vertex[1].tv = syy1[i];
							vertex[2].tu = sxx1[j];	vertex[2].tv = syy2[i];
							vertex[3].tu = sxx2[j];	vertex[3].tv = syy2[i];
							
							if(disp->textue_no2>=0 && d3dTex->format == D3DFMT_A8R8G8B8){
								vertex[0].color = D3DCOLOR_ARGB( 255, cc[(i+0)*D3DD_TEXTURE_SIZE+j+0].r, cc[(i+0)*D3DD_TEXTURE_SIZE+j+0].g, cc[(i+0)*D3DD_TEXTURE_SIZE+j+0].b );
								vertex[1].color = D3DCOLOR_ARGB( 255, cc[(i+0)*D3DD_TEXTURE_SIZE+j+1].r, cc[(i+0)*D3DD_TEXTURE_SIZE+j+1].g, cc[(i+0)*D3DD_TEXTURE_SIZE+j+1].b );
								vertex[2].color = D3DCOLOR_ARGB( 255, cc[(i+1)*D3DD_TEXTURE_SIZE+j+0].r, cc[(i+1)*D3DD_TEXTURE_SIZE+j+0].g, cc[(i+1)*D3DD_TEXTURE_SIZE+j+0].b );
								vertex[3].color = D3DCOLOR_ARGB( 255, cc[(i+1)*D3DD_TEXTURE_SIZE+j+1].r, cc[(i+1)*D3DD_TEXTURE_SIZE+j+1].g, cc[(i+1)*D3DD_TEXTURE_SIZE+j+1].b );
							}else{
								vertex[0].color = D3DCOLOR_ARGB( cc[(i+0)*D3DD_TEXTURE_SIZE+j+0].a, cc[(i+0)*D3DD_TEXTURE_SIZE+j+0].r, cc[(i+0)*D3DD_TEXTURE_SIZE+j+0].g, cc[(i+0)*D3DD_TEXTURE_SIZE+j+0].b );
								vertex[1].color = D3DCOLOR_ARGB( cc[(i+0)*D3DD_TEXTURE_SIZE+j+1].a, cc[(i+0)*D3DD_TEXTURE_SIZE+j+1].r, cc[(i+0)*D3DD_TEXTURE_SIZE+j+1].g, cc[(i+0)*D3DD_TEXTURE_SIZE+j+1].b );
								vertex[2].color = D3DCOLOR_ARGB( cc[(i+1)*D3DD_TEXTURE_SIZE+j+0].a, cc[(i+1)*D3DD_TEXTURE_SIZE+j+0].r, cc[(i+1)*D3DD_TEXTURE_SIZE+j+0].g, cc[(i+1)*D3DD_TEXTURE_SIZE+j+0].b );
								vertex[3].color = D3DCOLOR_ARGB( cc[(i+1)*D3DD_TEXTURE_SIZE+j+1].a, cc[(i+1)*D3DD_TEXTURE_SIZE+j+1].r, cc[(i+1)*D3DD_TEXTURE_SIZE+j+1].g, cc[(i+1)*D3DD_TEXTURE_SIZE+j+1].b );
							}

							vertex[0].specular = D3DCOLOR_ARGB( 255, ss[(i+0)*D3DD_TEXTURE_SIZE+j+0].r, ss[(i+0)*D3DD_TEXTURE_SIZE+j+0].g, ss[(i+0)*D3DD_TEXTURE_SIZE+j+0].b );
							vertex[1].specular = D3DCOLOR_ARGB( 255, ss[(i+0)*D3DD_TEXTURE_SIZE+j+1].r, ss[(i+0)*D3DD_TEXTURE_SIZE+j+1].g, ss[(i+0)*D3DD_TEXTURE_SIZE+j+1].b );
							vertex[2].specular = D3DCOLOR_ARGB( 255, ss[(i+1)*D3DD_TEXTURE_SIZE+j+0].r, ss[(i+1)*D3DD_TEXTURE_SIZE+j+0].g, ss[(i+1)*D3DD_TEXTURE_SIZE+j+0].b );
							vertex[3].specular = D3DCOLOR_ARGB( 255, ss[(i+1)*D3DD_TEXTURE_SIZE+j+1].r, ss[(i+1)*D3DD_TEXTURE_SIZE+j+1].g, ss[(i+1)*D3DD_TEXTURE_SIZE+j+1].b );

							pD3DDevice->SetTexture( 0, texture );
							pD3DDevice->SetVertexShader( FVF_TLVERTEX );										
							pD3DDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, vertex, sizeof(TLVERTEX) );	
							pD3DDevice->SetTexture( 0, NULL );
						}
					}
				}
			}
		}
	}
	return D3D_OK;
}



#define FULLMODE_STYLE		(WS_POPUP | WS_CLIPCHILDREN | WS_SYSMENU | WS_VISIBLE)
#define WINDOWMODE_STYLE	(FULLMODE_STYLE | WS_CAPTION | WS_BORDER | WS_MINIMIZEBOX)


#define ALL_DISP_GROUP	0xffffffff
DWORD	DispGroup = ALL_DISP_GROUP;

void D3DD_SetDispGroup( DWORD group )
{
	DispGroup = group;
}
void D3DD_AddDispGroup( DWORD group )
{
	DispGroup = DispGroup|group;
}
void D3DD_SubDispGroup( DWORD group )
{
	DispGroup = DispGroup&(~group);
}


int D3DD_Draw( int flash, int save )
{
	int		i,j,k;
	short	disp_count[D3DD_MAX_DISP_AMOUNT];
	short	disp_layer[D3DD_MAX_DISP_AMOUNT];

	if(!pD3D)		return FALSE;
	if(!pD3DDevice)	return FALSE;
	switch( pD3DDevice->TestCooperativeLevel() ){
		case D3DERR_DEVICELOST:
			return 0;
			break;
		case D3DERR_DEVICENOTRESET:
			
			if( FAILED(pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&D3DCapsStruct.m_NowDisplayMode)) ){	
				MessageBox(NULL,"ディスプレイモードの取得に失敗しました。[なにゆえ？]","致命的なエラー", MB_OK | MB_ICONSTOP);
				return FALSE;
			}
			ZeroMemory(&d3dppApp,sizeof(d3dppApp));
			d3dppApp.Windowed = D3DMain.m_WindowMode;			
			d3dppApp.SwapEffect = D3DSWAPEFFECT_FLIP;		
			if(D3DMain.m_FullScreenOnly==TRUE){
				d3dppApp.BackBufferFormat = D3DCapsStruct.m_DisplayMode[D3DCapsStruct.m_FullModeNum].Format;
			}else{
				d3dppApp.BackBufferFormat = D3DCapsStruct.m_NowDisplayMode.Format;
			}
			d3dppApp.BackBufferCount = 1;
			d3dppApp.BackBufferWidth  = BackSufaceSizeW;
			d3dppApp.BackBufferHeight = BackSufaceSizeH;
			d3dppApp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

			D3DMain.m_FlipActive = FALSE;
			pD3DDevice->Reset(&d3dppApp);

			if(D3DMain.m_WindowMode){	
				
				RECT rect;
				SetRect( &rect, 0, 0, 800, 600 );
				AdjustWindowRect( &rect, WINDOWMODE_STYLE, !!D3DMain.m_MenuHwnd );

				int	wx = (GetSystemMetrics(SM_CXFULLSCREEN)-(rect.right-rect.left))/2;
				int	wy = (GetSystemMetrics(SM_CYFULLSCREEN)-(rect.bottom-rect.top))/2;

				SetWindowPos( D3DMain.m_DrawHwnd, HWND_NOTOPMOST, wx, wy, rect.right-rect.left, rect.bottom-rect.top, SWP_SHOWWINDOW|SWP_DRAWFRAME );
			}else{


				SetWindowPos( D3DMain.m_DrawHwnd, HWND_NOTOPMOST, 0, 0, BackSufaceSizeW, BackSufaceSizeH, SWP_SHOWWINDOW|SWP_DRAWFRAME );
			}
			return 0;
			break;
	}

	
	pD3DDevice->BeginScene();



	pD3DDevice->SetRenderState(D3DRS_ZENABLE,D3DZB_FALSE);		
	pD3DDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);	
	pD3DDevice->SetRenderState(D3DRS_SPECULARENABLE,TRUE);		






	if(flash) pD3DDevice->Clear(0,NULL,D3DCLEAR_TARGET,D3DCOLOR_ARGB(0,0,0,0),1.0,0);

	for( i=0 ; i<D3DD_MAX_DISP_AMOUNT ; i++ ){
		disp_count[i]=-1;
		disp_layer[i]=30000;
	}
	for( i=0 ; i<D3DD_MAX_DISP_AMOUNT ; i++ ){
		if(d3dDisp[i].flag==0) continue;
		if(d3dDisp[i].disp==0) continue;
		if(!(d3dDisp[i].group&DispGroup)) continue;

		for( j=0 ; j< i+1 ; j++ ){
			if(j>=D3DD_MAX_DISP_AMOUNT){
				DebugBox( NULL,"111" );
			}
			if(disp_layer[j] >= d3dDisp[i].layer){
				if(disp_layer[j]!=30000){
					for( k=i-1 ; k>=j  ; k-- ){
						if(k+1>=D3DD_MAX_DISP_AMOUNT){
							DebugBox( NULL,"111" );
						}

						disp_count[k+1] = disp_count[k];
						disp_layer[k+1] = disp_layer[k];
					}
				}
				disp_layer[j] = d3dDisp[i].layer;
				disp_count[j] = i;
				break;
			}
		}
	}
	for( j=0 ; j<D3DD_MAX_DISP_AMOUNT ; j++ ){
		if(disp_count[j]==-1) continue;
		i = disp_count[j];
		if(d3dDisp[i].flag==0) continue;
		if(d3dDisp[i].disp==0) continue;

		switch( d3dDisp[i].type ){
		case GTYPE_MOVIE:
		case GTYPE_POLY:	
			D3DD_DrawTexture( &d3dDisp[i] );
			break;
		case GTYPE_POLYx9:	
			{
				int	k;
				for( k=0; k<9 ;k++ ){
					D3DD_DISP	d3dDispWork = d3dDisp[i];
					int			src_pitch_w = (d3dDisp[i].sx1[1]-d3dDisp[i].sx1[0])/3;
					int			src_pitch_h = (d3dDisp[i].sy1[1]-d3dDisp[i].sy1[0])/3;

					switch(k%3){
						case 0:
							d3dDispWork.dx[0] = d3dDispWork.dx[2] = d3dDisp[i].dx[0];
							d3dDispWork.dx[1] = d3dDispWork.dx[3] = d3dDisp[i].dx[0]+src_pitch_w;

							d3dDispWork.sx1[0] = d3dDisp[i].sx1[0];
							d3dDispWork.sx1[1] = d3dDisp[i].sx1[0] + src_pitch_w;
							break;
						case 1:
							d3dDispWork.dx[0] = d3dDispWork.dx[2] = d3dDisp[i].dx[0]+src_pitch_w;
							d3dDispWork.dx[1] = d3dDispWork.dx[3] = d3dDisp[i].dx[1]-src_pitch_w;

							d3dDispWork.sx1[0] = d3dDisp[i].sx1[0]+src_pitch_w;
							d3dDispWork.sx1[1] = d3dDisp[i].sx1[0]+src_pitch_w*2;
							break;
						case 2:
							d3dDispWork.dx[0] = d3dDispWork.dx[2] = d3dDisp[i].dx[1]-src_pitch_w;
							d3dDispWork.dx[1] = d3dDispWork.dx[3] = d3dDisp[i].dx[1];

							d3dDispWork.sx1[0] = d3dDisp[i].sx1[0]+src_pitch_w*2;
							d3dDispWork.sx1[1] = d3dDisp[i].sx1[0]+src_pitch_w*3;
							break;
					}

					switch(k/3){
						case 0:
							d3dDispWork.dy[0] = d3dDispWork.dy[1] = d3dDisp[i].dy[0];
							d3dDispWork.dy[2] = d3dDispWork.dy[3] = d3dDisp[i].dy[0]+src_pitch_h;

							d3dDispWork.sy1[0] = d3dDisp[i].sy1[0];
							d3dDispWork.sy1[1] = d3dDisp[i].sy1[0]+src_pitch_h;
							break;
						case 1:
							d3dDispWork.dy[0] = d3dDispWork.dy[1] = d3dDisp[i].dy[0]+src_pitch_h;
							d3dDispWork.dy[2] = d3dDispWork.dy[3] = d3dDisp[i].dy[2]-src_pitch_h;

							d3dDispWork.sy1[0] = d3dDisp[i].sy1[0]+src_pitch_h;
							d3dDispWork.sy1[1] = d3dDisp[i].sy1[0]+src_pitch_h*2;
							break;
						case 2:
							d3dDispWork.dy[0] = d3dDispWork.dy[1] = d3dDisp[i].dy[2]-src_pitch_h;
							d3dDispWork.dy[2] = d3dDispWork.dy[3] = d3dDisp[i].dy[2];

							d3dDispWork.sy1[0] = d3dDisp[i].sy1[0]+src_pitch_h*2;
							d3dDispWork.sy1[1] = d3dDisp[i].sy1[0]+src_pitch_h*3;
							break;
					}

					D3DD_DrawTexture( &d3dDispWork );
				}
			}
			break;
		case GTYPE_GRID:	
			{
				int	grid = d3dDisp[i].scale_param;
				int	g;
				if(grid<=0) break;
				D3DD_DISP	d3dDispWork = d3dDisp[i];
				d3dDispWork.scale_param = 0;

				for( g=(int)d3dDisp[i].dx[0] ; g<d3dDisp[i].dx[3] ; g+=grid ){
					d3dDispWork.dx[0] = d3dDispWork.dx[1] = (float)g;
					d3dDispWork.dy[0] = 0;
					d3dDispWork.dy[1] = d3dDisp[i].dy[3];
					D3DD_DrawTexture( &d3dDispWork );
				}
				for( g=(int)d3dDisp[i].dy[0] ; g<d3dDisp[i].dy[3] ; g+=grid ){
					d3dDispWork.dx[0] = 0;
					d3dDispWork.dx[1] = d3dDisp[i].dx[3];
					d3dDispWork.dy[0] = d3dDispWork.dy[1] = (float)g;
					D3DD_DrawTexture( &d3dDispWork );
				}
			}
			break;
		case GTYPE_SPRITE:	
			SPR_DrawSprite( d3dDisp[i].textue_no, &d3dDisp[i] );
			break;
		case GTYPE_SP_FRAME:

			break;

		case GTYPE_STR:
			{
				float	tx=0,ty=0;
				int		str_no;
				k=0;

				D3DD_DISP	d3dDispWork;
				while(d3dDisp[i].str[k]){
					switch(d3dDisp[i].str[k]){
						default:
							d3dDispWork = d3dDisp[i];

							d3dDispWork.dx[0] = d3dDisp[i].dx[0]+tx*(d3dDisp[i].dx[1]-d3dDisp[i].dx[0]);
							d3dDispWork.dx[2] = d3dDisp[i].dx[2]+tx*(d3dDisp[i].dx[3]-d3dDisp[i].dx[2]);
							d3dDispWork.dx[1] = d3dDispWork.dx[0]+(d3dDisp[i].dx[1]-d3dDisp[i].dx[0]);
							d3dDispWork.dx[3] = d3dDispWork.dx[2]+(d3dDisp[i].dx[3]-d3dDisp[i].dx[2]);

							d3dDispWork.dy[0] = d3dDisp[i].dy[0]+ty*(d3dDisp[i].dy[2]-d3dDisp[i].dy[0]);
							d3dDispWork.dy[1] = d3dDisp[i].dy[1]+ty*(d3dDisp[i].dy[3]-d3dDisp[i].dy[1]);
							d3dDispWork.dy[2] = d3dDispWork.dy[0]+(d3dDisp[i].dy[2]-d3dDisp[i].dy[0]);
							d3dDispWork.dy[3] = d3dDispWork.dy[1]+(d3dDisp[i].dy[3]-d3dDisp[i].dy[1]);

							str_no = d3dDisp[i].str[k]-' ';
							d3dDispWork.sx1[0] = d3dDisp[i].sx1[0]+str_no%16*(d3dDisp[i].sx1[1]-d3dDisp[i].sx1[0]);
							d3dDispWork.sx1[1] = d3dDisp[i].sx1[1]+str_no%16*(d3dDisp[i].sx1[1]-d3dDisp[i].sx1[0]);
							d3dDispWork.sy1[0] = d3dDisp[i].sy1[0]+str_no/16*(d3dDisp[i].sy1[1]-d3dDisp[i].sy1[0]);
							d3dDispWork.sy1[1] = d3dDisp[i].sy1[1]+str_no/16*(d3dDisp[i].sy1[1]-d3dDisp[i].sy1[0]);

							D3DD_DrawTexture( &d3dDispWork );

							tx++;
							break;
						case '\n':
							tx=0;
							ty++;
							break;
					}
					k++;

				}
			}
			break;
		case GTYPE_DIGIT:	
			{
				int	digit = d3dDisp[i].digit;
				int	keta = d3dDisp[i].keta;

				if( keta==0 ){		
					while(digit){
						keta++;
						digit /= 10;
					}
					digit = d3dDisp[i].digit;
					if(keta==0) keta=1;
				}
				if( d3dDisp[i].keta<0 )	keta = -keta;	

				for( k=0 ; k<keta ; k++ ){
					D3DD_DISP	d3dDispWork = d3dDisp[i];

					d3dDispWork.dx[0] = d3dDisp[i].dx[0]+(keta-k-1)*(d3dDisp[i].dx[1]-d3dDisp[i].dx[0]);
					d3dDispWork.dx[2] = d3dDisp[i].dx[2]+(keta-k-1)*(d3dDisp[i].dx[3]-d3dDisp[i].dx[2]);
					d3dDispWork.dx[1] = d3dDispWork.dx[0]+(d3dDisp[i].dx[1]-d3dDisp[i].dx[0]);
					d3dDispWork.dx[3] = d3dDispWork.dx[2]+(d3dDisp[i].dx[3]-d3dDisp[i].dx[2]);

					d3dDispWork.sx1[0] = d3dDisp[i].sx1[0]+digit%10*(d3dDisp[i].sx1[1]-d3dDisp[i].sx1[0]);
					d3dDispWork.sx1[1] = d3dDisp[i].sx1[1]+digit%10*(d3dDisp[i].sx1[1]-d3dDisp[i].sx1[0]);

					D3DD_DrawTexture( &d3dDispWork );

					digit /= 10;
					if( d3dDisp[i].keta>0 && digit==0 ){	
						break;
					}
				}
			}
			break;
		case GTYPE_TEXT:	
			if(d3dTexture[d3dDisp[i].textue_no].flag){
				D3DD_TEXT	*d3dtx = &d3dText[d3dTexture[d3dDisp[i].textue_no].index_no];

				d3dtx->wait = D3DTXT_DispText( &d3dDisp[i], 0, d3dtx->ph, d3dtx->w, d3dtx->h, d3dtx->pw, d3dtx->ph, NULL, NULL, NULL,
					d3dtx->fsize, d3dtx->str, d3dtx->count, d3dtx->step,
					d3dDisp[i].color[0].r, d3dDisp[i].color[0].g, d3dDisp[i].color[0].b, d3dDisp[i].color[0].a, d3dtx->kage, FALSE, NULL, &d3dtx->step_wait );
			}
			break;
		}
	}

	
	pD3DDevice->EndScene();

static int			tmp_cnt=0;
static int			tmp_cnt2=0;
if(save){
	IDirect3DSurface8	*back_buffer;
	BYTE				*buf10=NULL;
	D3DLOCKED_RECT		d3dlr;
	int					bpp;
	char				tmp_fname[256];

	if(tmp_cnt%2==0){
		if( pD3DDevice->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &back_buffer )==D3D_OK ){
			if( back_buffer->LockRect( &d3dlr, NULL, D3DLOCK_NOSYSLOCK|D3DLOCK_READONLY  )==D3D_OK ){
				buf10 = (BYTE*)GAlloc(d3dlr.Pitch*BackSufaceSizeH);
				
				CopyMemory( (BYTE*)buf10, (BYTE*)d3dlr.pBits, d3dlr.Pitch*BackSufaceSizeH );
				bpp = d3dlr.Pitch/BackSufaceSizeW;
				
				back_buffer->UnlockRect();
			}
			back_buffer->Release();
		}
		wsprintf( tmp_fname, "tmp\\%c%05d.tga", tmp_cnt2+'a' , tmp_cnt/2 ); 
		SaveTga( tmp_fname, buf10, BackSufaceSizeW, d3dlr.Pitch, BackSufaceSizeH, 32, 1 );
		GFree(buf10);
	}
	tmp_cnt++;
}else{
	if(tmp_cnt){
		tmp_cnt2++;
		tmp_cnt=0;
	}
}

	return TRUE;
}


void D3DD_Present( RECT *src, RECT *dst )
{
	if(!pD3D)		return ;
	if(!pD3DDevice)	return ;

	D3DMain.m_FlipActive = !D3DMain.m_FlipActive;
	pD3DDevice->Present(src,dst,NULL,NULL);
}