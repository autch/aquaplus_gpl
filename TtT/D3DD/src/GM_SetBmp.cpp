#include <windows.h>
#include <mouse.h>
#include <keybord.h>

#include <STD_Windows.h>
#include <STD_Debug.h>
#include <STD_File.h>

#include "comp_pac.h"
#include "Escript.h"

#include "GlobalDraw.h"

#include "main.h"


#include "GM_Battle.h"
#include "GM_Avg.h"
#include "GM_SetBmp.h"

#include <math.h>


SPRITE_BMP_STRUCT		SpriteBmp[MAX_SCRIPT_OBJ];

void AVG_LoadBmpSetting( int s_bno, SPRITE_BMP_STRUCT sbs )
{

}


void AVG_LoadBmp( int s_bno, char *fname, int layer, int bit16, int tone_type, char *tfname, int chroma, char *pack )
{
	char	*tname_time=NULL;

	if(fname){
		if(fname[0]=='v' || fname[0]=='V'){
			AVG_SaveVisual( 0, Str2Digit( fname+1 ) );
		}
		if(fname[0]=='h' || fname[0]=='H'){
			AVG_SaveVisual( 1, Str2Digit( fname+1 ) );
		}
	}
	AVG_ReleaseBmp(s_bno);
	switch(tone_type){
	case 0:
		GD_LoadTexture( BMP_SCRIPT+s_bno, fname, bit16, FALSE, FALSE, eff_tone_fname_b, NULL, BackStruct.chroma_back, pack );
		break;
	case 1:
		if( PAC_CheckFile( D3DD_GetPackDir(), bak_tone_fname ) ){
			
			GD_LoadTexture( BMP_SCRIPT+s_bno, fname, bit16, FALSE, FALSE, bak_tone_fname, eff_tone_fname_c, BackStruct.chroma_char, pack );
		}else{
			
			GD_LoadTexture( BMP_SCRIPT+s_bno, fname, bit16, FALSE, FALSE, time_tone_fname[BackStruct.time_no], eff_tone_fname_c, BackStruct.chroma_char, pack );
		}
		break;
	default:
		GD_LoadTexture( BMP_SCRIPT+s_bno, fname, bit16, FALSE, FALSE, tfname, NULL, chroma, pack );
		break;
	}
	GD_SetGraph( GRP_SCRIPT+s_bno, BMP_SCRIPT+s_bno, LAY_BACK+layer, TRUE );	
	GD_SetGraphScaleParam( GRP_SCRIPT+s_bno, SCALE_HIGHT );
	GD_SetGraphGroup( GRP_SCRIPT+s_bno, 0x02 );
	appData.draw_flag = 1;


	int	sx,sy;
	GD_GetTextureWH( BMP_SCRIPT+s_bno, &sx, &sy );
	ZeroMemory( &SpriteBmp[s_bno], sizeof(SPRITE_BMP_STRUCT) );

	_mbscpy( (BYTE*)SpriteBmp[s_bno].fname, (BYTE*)fname );

	SpriteBmp[s_bno].tone_type = tone_type;
	if(tfname)	_mbscpy( (BYTE*)SpriteBmp[s_bno].tfname, (BYTE*)tfname );
	if(pack)	_mbscpy( (BYTE*)SpriteBmp[s_bno].pack, (BYTE*)pack );
	else		_mbscpy( (BYTE*)SpriteBmp[s_bno].pack, (BYTE*)D3DD_GetPackDir() );

	SpriteBmp[s_bno].flag = ON;
	SpriteBmp[s_bno].disp = ON;
	SpriteBmp[s_bno].type = GTYPE_POLY;
	SpriteBmp[s_bno].param= BLEND_ALPHA;
	SpriteBmp[s_bno].alpha = 255;

	SpriteBmp[s_bno].rev_param = 0;
	SpriteBmp[s_bno].viv = chroma;			

	SpriteBmp[s_bno].r = 128;	
	SpriteBmp[s_bno].g = 128;	
	SpriteBmp[s_bno].b = 128;	

	SpriteBmp[s_bno].loop    = 0;
	SpriteBmp[s_bno].layer   = layer;
	SpriteBmp[s_bno].nuki    = bit16;

	SpriteBmp[s_bno].dx=0;
	SpriteBmp[s_bno].dy=0;
	SpriteBmp[s_bno].dw=sx;
	SpriteBmp[s_bno].dh=sy;
	SpriteBmp[s_bno].sx=0;
	SpriteBmp[s_bno].sy=0;
	SpriteBmp[s_bno].sw=sx;
	SpriteBmp[s_bno].sh=sy;
	SpriteBmp[s_bno].zoom=1.0f;
	SpriteBmp[s_bno].roll=0;
	SpriteBmp[s_bno].cx=0;
	SpriteBmp[s_bno].cy=0;
	
	return ;
}




void AVG_LoadBmpAnime( int s_bno, char *fname, int layer, char *tfname, int chroma, char *pack )
{
	AVG_ReleaseBmp(s_bno);

	GD_LoadSprite( SPR_SCRIPT+s_bno, fname, ALPHA_4444, FALSE, FALSE, tfname, NULL, chroma, pack );

	GD_SetSprite( GRP_SCRIPT+s_bno, SPR_SCRIPT+s_bno, 0, TRUE, LAY_BACK+layer, TRUE, FALSE, 1 );
	GD_SetGraphScaleParam( GRP_SCRIPT+s_bno, SCALE_HIGHT );
	GD_SetGraphGroup( GRP_SCRIPT+s_bno, 0x02 );
	appData.draw_flag = 1;


	ZeroMemory( &SpriteBmp[s_bno], sizeof(SPRITE_BMP_STRUCT) );

	_mbscpy( (BYTE*)SpriteBmp[s_bno].fname, (BYTE*)fname );

	SpriteBmp[s_bno].tone_type = 0;
	if(tfname)	_mbscpy( (BYTE*)SpriteBmp[s_bno].tfname, (BYTE*)tfname );
	if(pack)	_mbscpy( (BYTE*)SpriteBmp[s_bno].pack, (BYTE*)pack );
	else		_mbscpy( (BYTE*)SpriteBmp[s_bno].pack, (BYTE*)D3DD_GetPackDir() );

	SpriteBmp[s_bno].flag = ON;
	SpriteBmp[s_bno].disp = ON;

	SpriteBmp[s_bno].type  = GTYPE_SPRITE;
	SpriteBmp[s_bno].param = BLEND_ALPHA;
	SpriteBmp[s_bno].alpha = 255;

	SpriteBmp[s_bno].rev_param   = 0;
	SpriteBmp[s_bno].viv = chroma;			

	SpriteBmp[s_bno].r = 128;	
	SpriteBmp[s_bno].g = 128;	
	SpriteBmp[s_bno].b = 128;	

	SpriteBmp[s_bno].loop    = 1;
	SpriteBmp[s_bno].layer   = layer;
	SpriteBmp[s_bno].nuki    = ALPHA_4444;
	SpriteBmp[s_bno].dx = 0;
	SpriteBmp[s_bno].dy = 0;
	SpriteBmp[s_bno].zoom=1.0f;
	SpriteBmp[s_bno].roll = 0;
	SpriteBmp[s_bno].cx=0;
	SpriteBmp[s_bno].cy=0;
}
void AVG_SetBmpAvi( int s_bno, char *fname, int layer)
{
	AVG_ReleaseBmp(s_bno);

	GD_CreateAviTexture( BMP_SCRIPT+s_bno, fname );
	GD_SetGraph( GRP_SCRIPT+s_bno, BMP_SCRIPT+s_bno, LAY_BACK+layer, TRUE );	
	GD_SetGraphScaleParam( GRP_SCRIPT+s_bno, SCALE_HIGHT );
	GD_SetGraphGroup( GRP_SCRIPT+s_bno, 0x02 );
	appData.draw_flag = 1;


	int	sx,sy;
	GD_GetTextureWH( BMP_SCRIPT+s_bno, &sx, &sy );
	ZeroMemory( &SpriteBmp[s_bno], sizeof(SPRITE_BMP_STRUCT) );
	
	_mbscpy( (BYTE*)SpriteBmp[s_bno].fname, (BYTE*)fname );

	SpriteBmp[s_bno].tone_type = 0;

	SpriteBmp[s_bno].flag = ON;
	SpriteBmp[s_bno].disp = ON;
	SpriteBmp[s_bno].type = GTYPE_MOVIE;
	SpriteBmp[s_bno].param= BLEND_ALPHA;
	SpriteBmp[s_bno].alpha = 255;

	SpriteBmp[s_bno].rev_param = 0;
	SpriteBmp[s_bno].viv = 256;			

	SpriteBmp[s_bno].r = 128;	
	SpriteBmp[s_bno].g = 128;	
	SpriteBmp[s_bno].b = 128;	

	SpriteBmp[s_bno].loop    = 0;
	SpriteBmp[s_bno].layer   = layer;
	SpriteBmp[s_bno].nuki    = ALPHA_1555;

	SpriteBmp[s_bno].dx=0;
	SpriteBmp[s_bno].dy=0;
	SpriteBmp[s_bno].dw=sx;
	SpriteBmp[s_bno].dh=sy;
	SpriteBmp[s_bno].sx=0;
	SpriteBmp[s_bno].sy=0;
	SpriteBmp[s_bno].sw=sx;
	SpriteBmp[s_bno].sh=sy;
	SpriteBmp[s_bno].zoom=1.0f;
	SpriteBmp[s_bno].roll=0;
	SpriteBmp[s_bno].cx=0;
	SpriteBmp[s_bno].cy=0;
}

void AVG_ReleaseBmp( int s_bno )
{
	if(SpriteBmp[s_bno].flag){
		if(SpriteBmp[s_bno].type == GTYPE_SPRITE){
			GD_ReleaseSprite( SPR_SCRIPT+s_bno );
		}
		GD_ReleaseTexture( BMP_SCRIPT+s_bno );
		GD_ResetGraph( GRP_SCRIPT+s_bno );
	}
	ZeroMemory( &SpriteBmp[s_bno], sizeof(SPRITE_BMP_STRUCT) );
}
BOOL AVG_WaitBmpAvi( int s_bno )
{
	if( !SpriteBmp[s_bno].flag ) return 1;
	return !GD_WaitAviTexture( BMP_SCRIPT+s_bno );
}
BOOL AVG_WaitBmpAnime( int s_bno )
{
	if( !SpriteBmp[s_bno].flag ) return 1;
	return !GD_GetSpriteMode( GRP_SCRIPT+s_bno );
}

void AVG_SetBmpAnimePlay( int s_bno, int sp_no, int play, int loop )
{
	GD_SetSpritePlay( GRP_SCRIPT+s_bno, SPR_SCRIPT+s_bno, sp_no, play, TRUE, loop );

	SpriteBmp[s_bno].sp_no = sp_no;
	SpriteBmp[s_bno].loop  = loop;
}





void AVG_SetBmpDisp( int s_bno, int disp )
{
	GD_SetGraphDisp( GRP_SCRIPT+s_bno, disp );

	SpriteBmp[s_bno].disp = disp;
}
void AVG_SetBmpLayer( int s_bno, int layer )
{
	GD_SetGraphLayer( GRP_SCRIPT+s_bno, LAY_BACK+layer );

	SpriteBmp[s_bno].layer = layer;
}
void AVG_SetBmpParam( int s_bno, int param, int alpha )
{
	GD_SetGraphBlendParam( GRP_SCRIPT+s_bno, param );
	GD_SetGraphAlpha( GRP_SCRIPT+s_bno, alpha );

	SpriteBmp[s_bno].param = param;
	SpriteBmp[s_bno].alpha = alpha;
}
void AVG_SetBmpRevParam( int s_bno, DWORD rev_param )
{
	GD_SetGraphRevParam( GRP_SCRIPT+s_bno, rev_param );

	SpriteBmp[s_bno].rev_param = rev_param;
}
void AVG_SetBmpBright(  int s_bno, int r, int g, int b )
{
	GD_SetGraphColor2( GRP_SCRIPT+s_bno, r, g, b );

	SpriteBmp[s_bno].r = r;
	SpriteBmp[s_bno].g = g;
	SpriteBmp[s_bno].b = b;
}
void AVG_SetBmpMove( int s_bno, int x, int y )
{
	GD_SetGraphDstMove( GRP_SCRIPT+s_bno, (float)x, (float)y );

	SpriteBmp[s_bno].dx = x;
	SpriteBmp[s_bno].dy = y;
}
void AVG_SetBmpPos( int s_bno, int dx, int dy, int sx, int sy, int w, int h )
{
	GD_SetGraphRect( GRP_SCRIPT+s_bno, (float)dx, (float)dy, (float)sx, (float)sy, (float)w, (float)h );

	SpriteBmp[s_bno].dx = dx;
	SpriteBmp[s_bno].dy = dy;
	SpriteBmp[s_bno].dw = w;
	SpriteBmp[s_bno].dh = h;

	SpriteBmp[s_bno].sx = sx;
	SpriteBmp[s_bno].sy = sy;
	SpriteBmp[s_bno].sw = w;
	SpriteBmp[s_bno].sh = h;
}
void AVG_SetBmpZoom( int s_bno, int dx, int dy, int dw, int dh )
{
	GD_SetGraphDstRect( GRP_SCRIPT+s_bno, (float)dx, (float)dy, (float)dw, (float)dh );

	SpriteBmp[s_bno].dx = dx;
	SpriteBmp[s_bno].dy = dy;
	SpriteBmp[s_bno].dw = dw;
	SpriteBmp[s_bno].dh = dh;
}
void AVG_SetBmpZoom2( int s_bno, int cx, int cy, int zoom )
{
	GD_SetGraphCenter( GRP_SCRIPT+s_bno, (float)cx, (float)cy );
	GD_SetGraphZoom( GRP_SCRIPT+s_bno, zoom/4096.0f );



	SpriteBmp[s_bno].zoom = zoom/4096.0f;
	SpriteBmp[s_bno].cx = cx;
	SpriteBmp[s_bno].cy = cy;
}
void AVG_SetBmpRoll( int s_bno, int cx, int cy, int roll )
{
	GD_SetGraphCenter( GRP_SCRIPT+s_bno, (float)cx, (float)cy );
	GD_SetGraphRoll( GRP_SCRIPT+s_bno, roll*360/4096.0f );

	SpriteBmp[s_bno].roll = roll*360/4096.0f;
	SpriteBmp[s_bno].cx = cx;
	SpriteBmp[s_bno].cy = cy;
}




static int	leafFlag=0;

void AVG_SetAviFull( int leaf )
{
	leafFlag=leaf;
	if(leaf==1){
		GD_CreateAviTexture( BMP_SCRIPT+0, "mov\\Logo.avi" );
		GD_SetGraph( GRP_SCRIPT+0, BMP_SCRIPT+0, LAY_BACK-1, TRUE );
		GD_SetGraphScaleParam( GRP_SCRIPT+0, SCALE_HIGHT );
		GD_SetGraphDstRect( GRP_SCRIPT+0, 0, 0, 800, 600 );
	}else{
		if(leaf==2){
			switch(Avg.mov_lv){
				case 0:
					GD_CreateAviTexture( BMP_SCRIPT+0, "mov\\TTT_ED_448_02M.avi" );
					GD_SetGraph( GRP_SCRIPT+0, BMP_SCRIPT+0, LAY_BACK-1, TRUE );
					GD_SetGraphScaleParam( GRP_SCRIPT+0, SCALE_HIGHT );
					GD_SetGraphDstRect( GRP_SCRIPT+0, 0, 0, 800, 600 );
					break;
				case 1:
					GD_CreateAviTexture( BMP_SCRIPT+0, "mov\\TTT_ED_640_03M.avi" );
					GD_SetGraph( GRP_SCRIPT+0, BMP_SCRIPT+0, LAY_BACK-1, TRUE );
					GD_SetGraphScaleParam( GRP_SCRIPT+0, SCALE_HIGHT );
					GD_SetGraphDstRect( GRP_SCRIPT+0, 0, 0, 800, 600 );
					break;
				case 2:
					GD_CreateAviTexture( BMP_SCRIPT+0, "mov\\TTT_ED_800_05M.avi" );
					GD_SetGraph( GRP_SCRIPT+0, BMP_SCRIPT+0, LAY_BACK-1, TRUE );
					GD_SetGraphSrcRect( GRP_SCRIPT+0, 0, 0, 800, 600 );
					GD_SetGraphDstRect( GRP_SCRIPT+0, 0, 0, 800, 600 );
					break;
			}
		}else{
			switch(Avg.mov_lv){
				case 0:
					GD_CreateAviTexture( BMP_SCRIPT+0, "mov\\TTT_OP_448_02M.avi" );
					GD_SetGraph( GRP_SCRIPT+0, BMP_SCRIPT+0, LAY_BACK-1, TRUE );
					GD_SetGraphScaleParam( GRP_SCRIPT+0, SCALE_HIGHT );
					GD_SetGraphDstRect( GRP_SCRIPT+0, 0, 0, 800, 600 );
					break;
				case 1:
					GD_CreateAviTexture( BMP_SCRIPT+0, "mov\\TTT_OP_640_03M.avi" );
					GD_SetGraph( GRP_SCRIPT+0, BMP_SCRIPT+0, LAY_BACK-1, TRUE );
					GD_SetGraphScaleParam( GRP_SCRIPT+0, SCALE_HIGHT );
					GD_SetGraphDstRect( GRP_SCRIPT+0, 0, 0, 800, 600 );
					break;
				case 2:
					GD_CreateAviTexture( BMP_SCRIPT+0, "mov\\TTT_OP_800_05M.avi" );
					GD_SetGraph( GRP_SCRIPT+0, BMP_SCRIPT+0, LAY_BACK-1, TRUE );
					GD_SetGraphSrcRect( GRP_SCRIPT+0, 0, 0, 800, 600 );
					GD_SetGraphDstRect( GRP_SCRIPT+0, 0, 0, 800, 600 );
					break;
			}
			AVG_PlayBGM( 1, 0, FALSE, 255, TRUE );
		}
	}
	GD_SetGraphGroup( GRP_SCRIPT+0, 0x02 );

	appData.mov_mode=30;
	appData.draw_flag = 1;
}

int AVG_WaitAviFull( void )
{
	static int	end_flag=0;
	static int	end_cnt=0;
#if !TAIKEN_FLAG
	if( AVG_GetHitKey() && end_flag==0 ){
		if( (ESC_GetGameFlag(61)&&leafFlag==0) || (ESC_GetGameFlag(60)&&leafFlag==2) || leafFlag==1){
			end_flag = 1;
			end_cnt  = 0;
			AVG_StopBGM( 60 );
		}
	}
	if(end_flag){
		if(leafFlag)end_cnt+=2;
		else		end_cnt++;
		GD_SetGraphFade( GRP_SCRIPT+0, 128-128*end_cnt/60 );
	}
#endif
	if( !GD_WaitAviTexture( BMP_SCRIPT+0 ) || end_cnt>60 ){

		if(leafFlag==0){
			AVG_StopBGM( 0 );
			AVG_SetGameFlag( 61, 1 );
		}else if(leafFlag==2){
			AVG_SetGameFlag( 60, 1 );
		}
		
		end_flag = 0;
		end_cnt  = 0;
		appData.mov_mode=0;
		GD_ResetGraph( GRP_SCRIPT+0 );
		GD_ReleaseTexture( BMP_SCRIPT+0 );
		return 1;
	}
	return 0;
}
