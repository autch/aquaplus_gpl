#include <windows.h>
#include <mouse.h>
#include <keybord.h>

#include <STD_Windows.h>
#include <STD_Debug.h>
#include <STD_File.h>

#include "comp_pac.h"

#include "GlobalDraw.h"

#include "main.h"
#include "Escript.h"

#include "GM_Main.h"
#include "GM_Battle.h"
#include "GM_Avg.h"
#include "GM_Char.h"
#include "GM_Magic.h"

#include "GM_CharThink.h"

#include <math.h>



AVG_STRUCT	Avg = { 60, MES_NORMAL, 
					0,1,
					MES_NORMAL, OFF, OFF,
					ON, ON, ON,
					192, 256, 256,
					FALSE, 0, 30,
					OFF,
					0, 0,0,0,
};

BOOL AVG_SetDemoFlag( int flag, int count )
{
	Avg.demo = flag;			
	Avg.demo_max = count;		
	return 1;
}


BOOL AVG_GetHitKey(void)
{
	if(Avg.demo){
		if( Avg.demo_cnt>=Avg.demo_max){
			Avg.demo_cnt=0;
			return 1;
		}else{
			Avg.demo_cnt++;
			return 0;
		}
	}
	return GameKey.click;
}

BOOL AVG_GetMesCut(void)
{

	if(Avg.demo){
		Avg.msg_cut_mode=0;
		return 0;
	}
	if(appData.mov_mode){
		Avg.msg_cut_mode=0;
		return 0;
	}
	if(Avg.msg_cut_optin&1){
		return GameKey.mes_cut || Avg.msg_cut_mode;
	}else{	
		return (GameKey.mes_cut || Avg.msg_cut_mode) && AVG_CheckScenarioFlag();
	}
}
BOOL AVG_WaitKey( void )
{
	if(Avg.demo){
		if( Avg.demo_cnt>=Avg.demo_max){
			Avg.demo_cnt=0;
			return 1;
		}else{
			Avg.demo_cnt++;
			return 0;
		}
	}
	return AVG_GetHitKey() || AVG_GetMesCut();
}






int AVG_EffCnt( int cnt )
{
	int	cut = AVG_GetMesCut();
	int	ret;
	if(Avg.demo){
		ret = AVG_EffCnt3( cnt );
	}else{
		if(cnt==-1){	
			if( cut )	ret = 0;
			else		ret = int(Avg.wait*15*Avg.frame/60);	
		}else if(cnt==-2){	
			if( cut )	ret = 0;
			else		ret = int(Avg.wait*30*Avg.frame/60);	
		}else{		
			if( cut )	ret = 0;
			else		ret = int(Avg.wait*cnt*Avg.frame/60);
		}
	}

	return ret;
}

int AVG_EffCnt2( int cnt )
{
	int	cut = AVG_GetMesCut();

	if(cnt==-1){	
		if( cut )	cnt = 15*Avg.frame/60/3;
		else		cnt = 15*Avg.frame/60;
	}else if(cnt==-2){	
		if( cut )	cnt = 30*Avg.frame/60/3;
		else		cnt = 30*Avg.frame/60;
	}else{
		if( cut )	cnt = cnt*Avg.frame/60/3;
		else		cnt = cnt*Avg.frame/60;

	}
	return cnt;
}

int AVG_EffCnt3( int cnt )
{
	if(cnt==-1){	
		cnt = 15*Avg.frame/60;
	}else if(cnt==-2){
		cnt = 30*Avg.frame/60;
	}else{
		cnt = cnt*Avg.frame/60;
	}

	return cnt;
}
int AVG_EffCnt4( int cnt )
{
	int	cut = AVG_GetMesCut();
	int	ret;
	
	if(cnt==-1){	
		if( cut )	ret = 0;
		else		ret = 15*Avg.frame/60;
	}else if(cnt==-2){	
		if( cut )	ret = 0;
		else		ret = 30*Avg.frame/60;
	}else{		
		if( cut )	ret = 0;
		else		ret = cnt*Avg.frame/60;
	}
	return ret;
}


int AVG_EffCntPuls( void )
{
	int	ret;

	if(Avg.demo){
		ret = int(2 * 60/Avg.frame);
	}else{
		if( AVG_GetMesCut() ){
			ret = 9999;
		}else{
			if(Avg.wait==MES_CUT){
				ret = 9999;
			}else{
				ret = int(2/Avg.wait * 60/Avg.frame);
			}
		}
	}
	if(ret==0) ret=1;
	return ret;
}


int AVG_MsgCnt( void )
{
	int	ret;


	if(Avg.demo){
		ret = int(2 * 60/Avg.frame);
	}else{
		if( AVG_WaitKey() ){
			ret = 9999;
		}else{
			if(Avg.msg_wait==MES_CUT){
				ret = 9999;
			}else{
				ret = int(2/Avg.msg_wait * 60/Avg.frame);
			}
		}
	}
	if(ret==0) ret=1;
	return ret;
}



typedef struct{
	int		flag;
	int		type;
	int		count;
	int		max;
}WAIT_STRUCT;
WAIT_STRUCT		WaitStruct;

void AVG_SetWaitFrame( int type, int wait )
{
	WaitStruct.flag  = 1;
	WaitStruct.type  = type;
	WaitStruct.count = 0;
	WaitStruct.max   = wait;
}

BOOL AVG_WaitFrame( void )
{
	int	wait_max;
	BOOL	ret=FALSE;
	
	if( WaitStruct.flag ){
		switch(WaitStruct.type) {
			case -1:
				if(1)	wait_max = AVG_EffCnt3(WaitStruct.max);
				else	wait_max = AVG_EffCnt4(WaitStruct.max);
				break;
			case 0:	wait_max = AVG_EffCnt(WaitStruct.max);	break;
			case 1:	wait_max = AVG_EffCnt2(WaitStruct.max);	break;
			case 2:	wait_max = AVG_EffCnt3(WaitStruct.max);	break;
		}
		WaitStruct.count++;
		if( WaitStruct.count>=wait_max ){
			WaitStruct.flag  = 0;
			ret = TRUE;
		}
	}

	return ret;
}



void SND_Select( void )
{
	AVG_PlaySE3( 9211);
}
void SND_Ok( void )
{
	AVG_PlaySE3( 9212);
}
void SND_Cansel( void )
{
	AVG_PlaySE3( 9213);
}
void SND_Ng( void )
{
	AVG_PlaySE3( 9214);
}
void SND_Haiti( void )
{
	AVG_PlaySE3( 9210);
}



char			*time_tone_fname[] = { 
	NULL, 				
	"evening.amp", 		
	"night.amp", 		
	"indoor.AMP",		
};
char	bak_tone_fname[32];
char	eff_tone_fname_b[32];
char	eff_tone_fname_c[32];

BACK_STRUCT		BackStruct;

void AVG_SetTimeMode( int time_no )
{
	BackStruct.time_no = time_no;
}
void AVG_SetChromaMode( int chroma_no, int cb )
{
	switch(cb){
	case 0:
		BackStruct.chroma_char = chroma_no;
		BackStruct.chroma_back = chroma_no;
		break;
	case 1:
		BackStruct.chroma_back = chroma_no;
		break;
	case 2:
		BackStruct.chroma_char = chroma_no;
		break;
	}
}
void AVG_SetEffctMode( char *tone_fname, int cb )
{
	switch(cb){
	case 0:
		_mbscpy( (BYTE*)eff_tone_fname_b, (BYTE*)tone_fname );
		_mbscpy( (BYTE*)eff_tone_fname_c, (BYTE*)tone_fname );
		break;
	case 1:
		_mbscpy( (BYTE*)eff_tone_fname_b, (BYTE*)tone_fname );
		break;
	case 2:
		_mbscpy( (BYTE*)eff_tone_fname_c, (BYTE*)tone_fname );
		break;
	}
}






void AVG_ResetBack( int chg_type, int char_disp, int fd_max, int vague )
{
	if(!char_disp){
		AVG_ResetCharBack( CHAR_TYPE_CFADE, fd_max );
	}

	if(BackStruct.flag){

		BackStruct.flag    = 1;
		BackStruct.bno     = 0;
		BackStruct.cg_flag = 0;
		BackStruct.char_disp = char_disp;	
		BackStruct.zoom    = 0;

		BackStruct.sc_flag = 0;
		BackStruct.sc_type = 0;
		BackStruct.sw = BackStruct.sx = BackStruct.sx2 = BackStruct.sx3 = BackStruct.sx4 = 0;
		BackStruct.sw = BackStruct.sy = BackStruct.sy2 = BackStruct.sy3 = BackStruct.sy4 = 0;
		BackStruct.sc_cnt = 0;		
		BackStruct.sc_max = 0;		

		GD_AddDispGroup( 0x01 );

		BackStruct.fd_flag = 2;
		BackStruct.fd_type = chg_type;
		BackStruct.fd_cnt  = 0;
		if( chg_type!=BAK_DIRECT ){
			BackStruct.fd_max  = fd_max;
		}else{
			BackStruct.fd_max  = 0;
		}
	}
}

void AVG_SetBack(int bak_no, int chg_type, int char_disp, int cg_flag, int fd_max, int vague, int x, int y, int w, int h )
{
	char	fname[256];
	int		back_max = AVG_EffCnt(fd_max);
	int		i, time_tone, rev_wh;
	char	*pack=NULL;

	
	time_tone = BackStruct.time_no;


	if(char_disp){	


	}else{			

	}
	
	
	if(bak_no==-1){	
		AVG_ResetBack( chg_type, char_disp, fd_max, vague );
		return ;
	}
	if(bak_no==-2) bak_no = BackStruct.bno;
	if(bak_no>=0){
		if(cg_flag==1){
			AVG_SaveVisual( 0, bak_no );
			wsprintf( fname, "v%05d.tga", bak_no );
		}else if(cg_flag==2){
			AVG_SaveVisual( 1, bak_no );
			wsprintf( fname, "h%05d.tga", bak_no );
		}else{
			pack = "bak";
			wsprintf( fname, "B%03d%1d%1d.tga", bak_no/10, time_tone, bak_no%10 );
			wsprintf( bak_tone_fname, "B%03d%1d%1d.amp", bak_no/10, time_tone, bak_no%10 );	
		}
	}
	BackStruct.grp = !BackStruct.grp;
	int	fb1 = BackStruct.grp;
	int	fb2 = !BackStruct.grp;

	int		ret;
	if( eff_tone_fname_b[0]==0 ){
		ret = GD_LoadTexture( BMP_BACK+fb1, fname, ALPHA_1555, FALSE, FALSE, NULL,NULL, BackStruct.chroma_back, pack );
	}else{
		ret = GD_LoadTexture( BMP_BACK+fb1, fname, ALPHA_1555, FALSE, FALSE, eff_tone_fname_b,NULL, BackStruct.chroma_back, pack );
	}
	if(ret==0) return ;

	BackStruct.flag    = 1;
	BackStruct.bno     = bak_no;
	BackStruct.cg_flag = cg_flag;
	BackStruct.char_disp = char_disp;	
	BackStruct.zoom    = 0;

	BackStruct.sc_flag = 0;
	BackStruct.sc_type = 0;
	BackStruct.sx = x;	BackStruct.sy = y;
	BackStruct.sw = w;	BackStruct.sh = h;


	BackStruct.sc_cnt = 0;		
	BackStruct.sc_max = 0;		


	if(char_disp){	
		GD_SetGraph( GRP_BACK+fb1, BMP_BACK+fb1, LAY_BACK+1, TRUE );	
	}else{
		GD_SetGraph( GRP_BACK+fb1, BMP_BACK+fb1, LAY_FADE_BACK, TRUE );	
	}
	GD_SetGraphGroup( GRP_BACK+fb1, 0x02 );
	GD_SetGraphDstRect( GRP_BACK+fb1, 0, 0, 800, 600 );
	GD_SetGraphSrcRect( GRP_BACK+fb1, x, y, w, h );
	GD_SetGraphScaleParam( GRP_BACK+fb1, SCALE_HIGHT );


	if( chg_type!=BAK_DIRECT ){
		if(back_max) {
			switch(chg_type){
			case BAK_CFADE:
			case BAK_CFZOOM1:	case BAK_CFZOOM4:
			case BAK_CFZOOM2:	case BAK_CFZOOM3:
			case BAK_SLIDE_UP:	case BAK_SLIDE_DO:
			case BAK_SLIDE_RI:	case BAK_SLIDE_LE:
			case BAK_ROLL:
				break;
			default:
				if( chg_type>=BAK_PATTERN ){




					if( !(D3DCapsStruct.m_NowDisplayMode.Format==D3DFMT_X8R8G8B8 || D3DCapsStruct.m_NowDisplayMode.Format==D3DFMT_A8R8G8B8) ) break;

					wsprintf( fname, "f0%03d.bmp", chg_type&0x7f );

			char	fname2[256];
			char	fname3[256];
			wsprintf( fname2, "%s\\%s", "grp", fname );
			wsprintf( fname3, "%s\\buf\\%s", "grp", fname );
			CopyFile(fname2,fname3,TRUE);


					rev_wh = 0;
					if(chg_type&BAK_PTF_REV_W)	rev_wh |= REV_U;
					if(chg_type&BAK_PTF_REV_H)	rev_wh |= REV_V;

					if( chg_type&BAK_PTF_REV ){
						GD_LoadTexture( BMP_BACK+2, fname, ALPHA_FADE|rev_wh, FALSE, FALSE, "rev.amp" );
					}else{
						GD_LoadTexture( BMP_BACK+2, fname, ALPHA_FADE|rev_wh );
					}
					GD_SetGraphSrc2( GRP_BACK+fb1, BMP_BACK+2, vague );
				}else{
					DebugBox( NULL, "B命令の背景表示方法が異常です。%d", chg_type );
				}
				break;
			}
			
			BackStruct.fd_flag = 1;
			BackStruct.fd_type = chg_type;
			BackStruct.fd_cnt  = 0;
			BackStruct.fd_max  = fd_max;
		}else{
			if( chg_type>=BAK_PATTERN ){
				wsprintf( fname, "f0%03d.bmp", chg_type&0x7f );
				char	fname2[256];
				char	fname3[256];
				wsprintf( fname2, "%s\\%s", "grp", fname );
				wsprintf( fname3, "%s\\buf\\%s", "grp", fname );
				CopyFile(fname2,fname3,TRUE);
			}

			BackStruct.fd_flag = 1;
			BackStruct.fd_type = BAK_DIRECT;
			BackStruct.fd_cnt  = 0;
			BackStruct.fd_max  = 0;
		}
	}else{
		BackStruct.fd_flag = 1;
		BackStruct.fd_type = chg_type;
		BackStruct.fd_cnt  = 0;
		BackStruct.fd_max  = 0;
	}

	appData.draw_flag=1;
}


void AVG_ControlBack(void)
{
	float	rate2,rate3;
	int		rate,i,count;
	int		back_max = AVG_EffCnt(BackStruct.fd_max);
	int		x,y;

	if(BackStruct.fd_flag){
		int	fb1 = BackStruct.grp;
		int	fb2 = !BackStruct.grp;

		BackStruct.fd_cnt++;

		if( BackStruct.fd_cnt>=back_max ){
			if(!BackStruct.char_disp){	
				AVG_ResetCharBack( CHAR_TYPE_CFADE, 0 );
			}

			if(BackStruct.fd_flag==1){
				BackStruct.fd_flag = 0;

				GD_SubDispGroup( 0x01 );

				GD_ResetGraphSrc2( GRP_BACK+fb1 );
				GD_SetGraphAlpha( GRP_BACK+fb1, 255 );
				GD_SetGraphLayer( GRP_BACK+fb1, LAY_BACK );
				GD_SetGraphZoom( GRP_BACK+fb1, 1.0f );

				GD_SetGraphSrcRect( GRP_BACK+fb1, BackStruct.sx, BackStruct.sy, BackStruct.sw, BackStruct.sh );

				GD_ResetGraph( GRP_BACK+fb2 );

				GD_ReleaseTexture( BMP_BACK+fb2 );
				GD_ReleaseTexture( BMP_BACK+2 );
			}else{
				BackStruct.flag = 0;
				BackStruct.fd_flag = 0;

				GD_ResetGraph( GRP_BACK+0 );
				GD_ReleaseTexture( BMP_BACK+0 );
				GD_ResetGraph( GRP_BACK+1 );
				GD_ReleaseTexture( BMP_BACK+1 );

				GD_ReleaseTexture( BMP_BACK+2 );
			}
		}else{
			if(BackStruct.fd_flag==1){
				count = BackStruct.fd_cnt;
			}else{
				count = back_max-BackStruct.fd_cnt;
			}
			rate = 255*count/back_max;
			switch(BackStruct.fd_type){
			case BAK_CFADE:
				GD_SetGraphAlpha( GRP_BACK+fb1, rate );
				break;
			case BAK_CFZOOM1:

				GD_SetGraphAlpha( GRP_BACK+fb1, rate );

				rate2 = POW_CONUT( back_max-count, back_max );
				GD_SetGraphCenter( GRP_BACK+fb1, 400, 300 );
				GD_SetGraphZoom( GRP_BACK+fb1, 1.0f+rate2 );
				break;
			case BAK_CFZOOM2:
				GD_SetGraphAlpha( GRP_BACK+fb1, rate );
				rate2 = 1.0f-1.0f*count/back_max;
				rate2 = rate2*rate2;
				rate2 = 1.0f-rate2;
				
				GD_SetGraphCenter( GRP_BACK+fb2, 400, 300 );
				GD_SetGraphZoom( GRP_BACK+fb2, 1.0f+rate2 );
				GD_SetGraphScaleParam( GRP_BACK+fb2, SCALE_HIGHT );
				break;
			case BAK_CFZOOM3:
				GD_SetGraphAlpha( GRP_BACK+fb1, rate );

				rate2 = 1.0f-1.0f*count/back_max;
				rate2 = rate2*rate2;

				GD_SetGraphCenter( GRP_BACK+fb1, 400, 300 );
				GD_SetGraphZoom( GRP_BACK+fb1, 1.0f+rate2 );
				GD_SetGraphScaleParam( GRP_BACK+fb1, SCALE_HIGHT );
				
				GD_SetGraphCenter( GRP_BACK+fb2, 400, 300 );
				GD_SetGraphZoom( GRP_BACK+fb2, 2.0f-rate2 );
				GD_SetGraphScaleParam( GRP_BACK+fb2, SCALE_HIGHT );
				break;
			case BAK_CFZOOM4:


				GD_SetGraphAlpha( GRP_BACK+fb1, rate );

				rate2 = POYO_CONUT( count, back_max, back_max*2/3 );
				GD_SetGraphCenter( GRP_BACK+fb1, 400, 300 );
				GD_SetGraphZoom( GRP_BACK+fb1, rate2/2+0.5f );

				break;
			default:
				if( BackStruct.fd_type>=BAK_PATTERN ){
					GD_SetGraphAlpha( GRP_BACK+fb1, rate );
				}
				break;
			}
		}
	}else{
		GD_SetGraphDstRect( GRP_BACK+0, ShakeDx, ShakeDy, 800, 600 );
		GD_SetGraphDstRect( GRP_BACK+1, ShakeDx, ShakeDy, 800, 600 );

		GD_SetGraphCenter( GRP_BACK+0, 400, 300 );
		GD_SetGraphCenter( GRP_BACK+1, 400, 300 );
		if( ShakeZoom==1.0f && ShakeRoll==0.0f){
			GD_SetGraphAlpha( GRP_BACK+0, 255 );
			GD_SetGraphAlpha( GRP_BACK+1, 255 );
			GD_SetGraphScaleParam( GRP_BACK+0, SCALE_POINT );
			GD_SetGraphScaleParam( GRP_BACK+1, SCALE_POINT );
		}else{
			GD_SetGraphAlpha( GRP_BACK+0, 128 );
			GD_SetGraphAlpha( GRP_BACK+1, 128 );
			GD_SetGraphScaleParam( GRP_BACK+0, SCALE_HIGHT );
			GD_SetGraphScaleParam( GRP_BACK+1, SCALE_HIGHT );
		}
		GD_SetGraphZoom( GRP_BACK+0, ShakeZoom );
		GD_SetGraphZoom( GRP_BACK+1, ShakeZoom );

		GD_SetGraphRoll( GRP_BACK+0, ShakeRoll );
		GD_SetGraphRoll( GRP_BACK+1, ShakeRoll );
	}
}

BOOL AVG_WaitBack(void)
{
	return BackStruct.fd_flag;
}


void AVG_SetBackScroll( int x, int y, int w, int h, int frame, int type )
{
	BackStruct.sc_flag = 1;
	BackStruct.sx    = x;
	BackStruct.sy    = y;
	BackStruct.sw    = w;
	BackStruct.sh    = h;
	BackStruct.sc_type = type;
	BackStruct.sc_cnt  = 0;
	BackStruct.sc_max  = frame;

	BackStruct.sx = x;	BackStruct.sy = y;
	BackStruct.sw = w;	BackStruct.sh = h;


}



int		ShakeDx=0, ShakeDy=0;
float	ShakeZoom = 1.0f;
float	ShakeRoll = 0.0f;

BOOL AVG_SetShake( int type, int pich, int speed, int dir, int swing )
{


	BackStruct.sk_flag  = 1;	
	BackStruct.sk_dir   = dir;	
	BackStruct.sk_pich  = pich;	
	BackStruct.sk_cnt   = 0;	
	BackStruct.sk_type  = type;
	BackStruct.sk_speed = speed;
	BackStruct.sk_swing = swing;

	ShakeDx=0;
	ShakeDy=0;
	return TRUE;
}
void AVG_StopShake( void )
{
	if( !BackStruct.sk_flag ) return ;

GD_ResetGraph( GRP_BACK+2 );

	BackStruct.sk_flag = 0;			
	ShakeDx=0;
	ShakeDy=0;
	ShakeZoom = 1.0f;
	ShakeRoll = 0.0f;
}
BOOL AVG_WaitShake( void )
{
	if( BackStruct.sk_speed==0 ) return 0;
	if( BackStruct.sk_flag ==0 ) return 0;
	return 1;
}
void AVG_ControlShake( void )
{
	int	x,y,cnt,cnt2,w,h;
	int	back_max = AVG_EffCnt4( BackStruct.sk_speed );
	int	pich = BackStruct.sk_pich;
	static	int	rand_flag=0;

	if( BackStruct.sk_flag ){
		BackStruct.sk_cnt++;
		if(BackStruct.sk_cnt>=back_max && BackStruct.sk_speed!=0 ){
			AVG_StopShake();
		}else{
			switch(BackStruct.sk_type){
				case SHAKE_SIN:
					cnt2 = BackStruct.sk_cnt*BackStruct.sk_swing/8;

					if(back_max){
						switch(BackStruct.sk_dir){
							case DIR_L: case DIR_DL: case DIR_UL:	x =-COS(cnt2)*pich/512/8;	break;
							case DIR_U: case DIR_D:					x = 0;						break;
							case DIR_R: case DIR_DR: case DIR_UR:	x = COS(cnt2)*pich/512/8;	break;
						}
						switch(BackStruct.sk_dir){
							case DIR_D: case DIR_DL: case DIR_DR:	y = COS(cnt2)*pich/512/8;	break;
							case DIR_L: case DIR_R:					y = 0;						break;
							case DIR_U: case DIR_UL: case DIR_UR:	y =-COS(cnt2)*pich/512/8;	break;
						}
						x = x*(back_max-BackStruct.sk_cnt)/back_max;
						y = y*(back_max-BackStruct.sk_cnt)/back_max;
					}else{
						switch(BackStruct.sk_dir){
							case DIR_L: case DIR_DL: case DIR_UL:	x =-COS(cnt2)*pich/4096;	break;
							case DIR_U: case DIR_D:					x = 0;						break;
							case DIR_R: case DIR_DR: case DIR_UR:	x = COS(cnt2)*pich/4096;	break;
						}
						switch(BackStruct.sk_dir){
							case DIR_D: case DIR_DL: case DIR_DR:	y = COS(cnt2)*pich/4096;	break;
							case DIR_L: case DIR_R:					y = 0;						break;
							case DIR_U: case DIR_UL: case DIR_UR:	y =-COS(cnt2)*pich/4096;	break;
						}
					}
					ShakeDx=x;
					ShakeDy=y;
					appData.flash=1;

GD_SetGraphPrimitive( GRP_BACK+2, 4, LAY_FADE_BACK, ON );
GD_SetGraphFade( GRP_BACK+2, 0 );
GD_SetGraphDstRect( GRP_BACK+2, 0, 600+ShakeDy, 800, abs(ShakeDy) );
					break;
				case SHAKE_2TI:
					cnt = (BackStruct.sk_cnt%8)/4*2-1;
					switch(BackStruct.sk_dir){
						case DIR_L: case DIR_DL: case DIR_UL:	x =-cnt*pich;	break;
						case DIR_U: case DIR_D:					x = 0;			break;
						case DIR_R: case DIR_DR: case DIR_UR:	x = cnt*pich;	break;
					}
					switch(BackStruct.sk_dir){
						case DIR_D: case DIR_DL: case DIR_DR:	y = cnt*pich;	break;
						case DIR_L: case DIR_R:					y = 0;			break;
						case DIR_U: case DIR_UL: case DIR_UR:	y =-cnt*pich;	break;
					}
					ShakeDx=x;
					ShakeDy=y;
					appData.flash=1;
					break;
				case SHAKE_ZOOM:
					cnt = BackStruct.sk_cnt;

					cnt2 = STD_LimitLoop(cnt*2, BackStruct.sk_pich );

					ShakeZoom = 1.0f+cnt2*cnt2 / (float)(BackStruct.sk_pich*BackStruct.sk_pich);
					break;
				case SHAKE_RAND:
					if(rand_flag==0){
						rand_flag=1;
						cnt = rand()%8;
						while( BackStruct.sk_dir==cnt ){
							cnt = rand()%8;
						}
						BackStruct.sk_dir=cnt;
					}else{
						rand_flag=0;
					}
					switch(BackStruct.sk_dir){
						case DIR_L: case DIR_DL: case DIR_UL:	x =-pich;	break;
						case DIR_U: case DIR_D:					x = 0;		break;
						case DIR_R: case DIR_DR: case DIR_UR:	x = pich;	break;
					}
					switch(BackStruct.sk_dir){
						case DIR_D: case DIR_DL: case DIR_DR:	y = pich;	break;
						case DIR_L: case DIR_R:					y = 0;		break;
						case DIR_U: case DIR_UL: case DIR_UR:	y =-pich;	break;
					}
					ShakeDx=x;
					ShakeDy=y;
					appData.flash=1;
					break;
				case SHAKE_ROLL:
					if(back_max){
						cnt = 360-BackStruct.sk_cnt*360/back_max;
						cnt = 360-cnt*cnt/360;
						if(BackStruct.sk_dir%2){
							cnt = cnt*pich/2%360;
						}else{
							cnt = 360-cnt*pich/2%360;
						}
						ShakeRoll = cnt;
					}else{
						cnt = BackStruct.sk_cnt;
						if(BackStruct.sk_dir%2){
							ShakeRoll = cnt*pich/100.0f;
						}else{
							ShakeRoll = -cnt*pich/100.0f;
						}

						ShakeZoom = BackStruct.sk_swing/4096.0f;
					}
					break;
				case SHAKE_ROLL_SIN:
					cnt = BackStruct.sk_cnt*BackStruct.sk_swing/8;
					y =-COS(cnt)*pich/4096;
					if(back_max){
						y = y*(back_max-BackStruct.sk_cnt)/back_max;
					}
					ShakeRoll = y;
					break;
				case SHAKE_ROLL_2TI:
					cnt = BackStruct.sk_cnt/2%4;
					switch(cnt){
						case 0:	y =-pich;	break;
						case 3:
						case 1:	y =0;		break;
						case 2:	y =pich;	break;
					}
					ShakeRoll = y;
					break;
					
			}
		}
	}
}



static int	CharLocateTable[9]={ -180, 0, 180, -240, 240, -300, 300, -400, 400 };
#define CHAR_LOCATE(W,L)		((800-W)/2+CharLocateTable[L])	
#define MAX_CHAR			8

typedef struct{
	short	flag;
	short	type;
	short	cond;

	short	cno;
	short	pose;
	short	layer;

	short	loc1;
	short	loc2;
	BYTE	r1,g1,b1;
	BYTE	r2,g2,b2;
	short	alph1;
	short	alph2;

	short	w,h;

	short	grp;
	short	cnt;
	short	max;

}CHAR_STRUCT;

CHAR_STRUCT		CharStruct[MAX_CHAR];



static int GetCharIndex( int char_no )
{
	int	i;
	for(i=0;i<MAX_CHAR;i++){
		if( CharStruct[i].flag && CharStruct[i].cno==char_no ){
			break;
		}
	}
	return i; 
}
static int GetSpaceIndex( void )
{
	int	i;
	for(i=0;i<MAX_CHAR;i++){
		if( !CharStruct[i].flag ){
			break;
		}
	}
	return i; 
}


void AVG_LoadChar( int index, int layer, int *w, int *h )
{
	char	fname[256];
	char	*tname;

	appData.draw_flag=1;

	wsprintf( fname, "c%02d%04d.tga", CharStruct[index].cno, CharStruct[index].pose );

	if( PAC_CheckFile( D3DD_GetPackDir(), bak_tone_fname ) ){
		tname = bak_tone_fname;						
	}else{
		tname = time_tone_fname[BackStruct.time_no];
	}

	
	int		ret;
	int		alpha=ALPHA_1555|DITHER_OFF;
	if( (CharStruct[index].cno==0  && CharStruct[index].pose==400)
	 || (CharStruct[index].cno==24 && CharStruct[index].pose==20)
	 || (CharStruct[index].cno==24 && CharStruct[index].pose==21) ){
		alpha = ALPHA_4444|DITHER_OFF;
	}
	ret = GD_LoadTexture( BMP_CHAR+index*2+CharStruct[index].grp, fname, alpha, FALSE, FALSE, tname, eff_tone_fname_c, BackStruct.chroma_char, NULL, 128 );
	if(ret==0) return ;

	GD_GetTextureWH( BMP_CHAR+index*2+CharStruct[index].grp, w, h );
	if(*h==768){
		*h=650;
	}

	GD_SetGraph( GRP_CHAR+index*2+CharStruct[index].grp, BMP_CHAR+index*2+CharStruct[index].grp, LAY_CHAR+CharStruct[index].layer+layer, ON );
	GD_SetGraphDstMove( GRP_CHAR+index*2+CharStruct[index].grp, CHAR_LOCATE(*w,CharStruct[index].loc1), 600-*h );
	GD_SetGraphGroup( GRP_CHAR+index*2+CharStruct[index].grp, 0x02 );
	GD_SetGraphColor2( GRP_CHAR+index*2+CharStruct[index].grp, CharStruct[index].r1, CharStruct[index].g1, CharStruct[index].b1 );
	GD_SetGraphAlpha( GRP_CHAR+index*2+CharStruct[index].grp, CharStruct[index].alph1 );
}
void AVG_SetChar( int char_no, int pose, int locate, int layer, int in_type, int r, int g, int b, int alph, int frame )
{
	int	w,h;
	int	index = GetCharIndex(char_no);

	if(in_type==-1 && in_type==-2){
		in_type = CHAR_TYPE_CFADE;
		frame=1;
	}

	if( index!=MAX_CHAR ){
		if(pose==-1)	pose   = CharStruct[index].pose;
		if(locate==-1)	locate = CharStruct[index].loc1;
		if(layer==-1)	layer  = CharStruct[index].layer;
		if(r==-1)		r = CharStruct[index].r1;
		if(g==-1)		g = CharStruct[index].g1;
		if(b==-1)		b = CharStruct[index].b1;
		if(alph==-1)	alph   = CharStruct[index].alph1;

		if( CharStruct[index].layer  == layer && CharStruct[index].loc1  == locate &&
			CharStruct[index].r1 == r && CharStruct[index].g1 == g && CharStruct[index].b1 == b &&
			CharStruct[index].alph1 == alph && CharStruct[index].pose == pose ){
			return ;
		}

		CharStruct[index].cond = CHAR_COND_CHANGE;
		CharStruct[index].type = in_type;

		CharStruct[index].layer = layer;

		CharStruct[index].loc2  = CharStruct[index].loc1;
		CharStruct[index].loc1  = locate;

		CharStruct[index].r2 = CharStruct[index].r1;
		CharStruct[index].r1 = r;
		CharStruct[index].g2 = CharStruct[index].g1;
		CharStruct[index].g1 = g;
		CharStruct[index].b2 = CharStruct[index].b1;
		CharStruct[index].b1 = b;

		CharStruct[index].alph2 = CharStruct[index].alph1;
		CharStruct[index].alph1 = alph;

		CharStruct[index].cnt  = 0;
		CharStruct[index].max  = frame;

		if(pose!=CharStruct[index].pose || in_type==CHAR_TYPE_CFADE){
			if( pose/100==CharStruct[index].pose/100
				&& CharStruct[index].loc1==CharStruct[index].loc2
				&& CharStruct[index].r1==CharStruct[index].r2
				&& CharStruct[index].g1==CharStruct[index].g2
				&& CharStruct[index].b1==CharStruct[index].b2
				&& CharStruct[index].alph1==CharStruct[index].alph2 ){
				CharStruct[index].max  = 0;
			}
			if(frame>60){
				CharStruct[index].max  = frame;
			}
			CharStruct[index].type = CHAR_TYPE_CFADE;
			CharStruct[index].pose  = pose;
			CharStruct[index].grp  = (CharStruct[index].grp)?0:1;


			AVG_LoadChar( index, 0, &w, &h );
			CharStruct[index].w = w;
			CharStruct[index].h = h;
		}
	}else{
		index = GetSpaceIndex();

		if( index!=MAX_CHAR ){
			if(pose==-1)	pose=0;
			if(layer==-1)	layer=0;
			if(locate==-1)	locate=1;
			if(r==-1)		r=128;
			if(g==-1)		g=128;
			if(b==-1)		b=128;
			if(alph==-1)	alph=255;

			CharStruct[index].flag  = 1;
			CharStruct[index].cond  = CHAR_COND_IN;
			CharStruct[index].type  = in_type;

			CharStruct[index].cno   = char_no;
			CharStruct[index].pose  = pose;
			CharStruct[index].layer = layer;

			CharStruct[index].loc1  = locate;
			
			CharStruct[index].r1 = r;
			CharStruct[index].g1 = g;
			CharStruct[index].b1 = b;

			CharStruct[index].alph1 = alph;

			CharStruct[index].grp   = 0;
			CharStruct[index].cnt   = 0;
			CharStruct[index].max   = frame;

			AVG_LoadChar( index, 0, &w, &h );

			CharStruct[index].w = w;
			CharStruct[index].h = h;
		}else{
			DebugBox( NULL, "これ以上、キャラクターを登録出来ません" );
		}
	}
}


void AVG_ReleaseChar( int char_index )
{
	GD_ReleaseTexture( BMP_CHAR+char_index*2 );
	GD_ReleaseTexture( BMP_CHAR+char_index*2+1 );
	GD_ResetGraph( GRP_CHAR+char_index*2 );
	GD_ResetGraph( GRP_CHAR+char_index*2+1 );

	ZeroMemory( &CharStruct[char_index], sizeof(CHAR_STRUCT) );
}
void AVG_ResetChar( int char_no, int out_type, int frame )
{
	int	index = GetCharIndex(char_no);
	
	if(out_type==-1 && out_type==-2){
		out_type = CHAR_TYPE_CFADE;
		frame = 1;
	}
	if( index!=MAX_CHAR ){
		CharStruct[index].cond = CHAR_COND_OUT;
		CharStruct[index].type = out_type;
		CharStruct[index].cnt  = 0;
		CharStruct[index].max  = frame;
	}else{

	}
}
void AVG_ResetCharBack( int out_type, int frame )
{
	int	i;
	for(i=0;i<MAX_CHAR;i++){
		if( CharStruct[i].flag ){
			CharStruct[i].cond = CHAR_COND_OUT;
			CharStruct[i].type = out_type;
			CharStruct[i].cnt  = 0;
			CharStruct[i].max  = frame;
		}
	}
}

void AVG_SetCharPose( int char_no, int pose, int frame )
{
	AVG_SetChar( char_no, pose, -1, -1, CHAR_TYPE_CFADE, -1,-1,-1, -1, frame );
}
void AVG_SetCharLocate( int char_no, int locate, int frame )
{
	AVG_SetChar( char_no, -1, locate, -1, !CHAR_TYPE_CFADE, -1,-1,-1, -1, frame );
}
void AVG_SetCharLayer( int char_no, int layer )
{
	AVG_SetChar( char_no, -1, -1, layer, !CHAR_TYPE_CFADE, -1,-1,-1, -1, 0 );
}
void AVG_SetCharBright( int char_no, int r, int g, int b, int frame )
{
	AVG_SetChar( char_no, -1, -1, -1, !CHAR_TYPE_CFADE, r,g,b, -1, frame );
}
void AVG_SetCharAlph( int char_no, int alph, int frame )
{
	AVG_SetChar( char_no, -1, -1, -1, !CHAR_TYPE_CFADE, -1,-1,-1, alph, frame );
}

void AVG_ControlChar( void )
{
	int		i;
	float	work,rate;
	BYTE	r,g,b;

	for(i=0;i<MAX_CHAR;i++){
		if( CharStruct[i].flag ){
			int	fade_max = AVG_EffCnt(CharStruct[i].max);
			float	x =CHAR_LOCATE(CharStruct[i].w, CharStruct[i].loc1), y=600-CharStruct[i].h;
			float	x2=CHAR_LOCATE(CharStruct[i].w, CharStruct[i].loc2);

			switch(CharStruct[i].cond){
				case CHAR_COND_NOMAL:	
					break;
				case CHAR_COND_IN:		
				case CHAR_COND_OUT:		
					CharStruct[i].cnt++;
					if( CharStruct[i].cnt>=fade_max){
						if(CharStruct[i].cond==CHAR_COND_IN){
							GD_SetGraphAlpha( GRP_CHAR+i*2+CharStruct[i].grp, CharStruct[i].alph1 );

							CharStruct[i].cond = CHAR_COND_NOMAL;
						}else{
							AVG_ReleaseChar( i );
						}
					}else{
						if(CharStruct[i].cond==CHAR_COND_IN)rate = 1.0f*CharStruct[i].cnt/fade_max;
						else								rate = 1.0f-1.0f*CharStruct[i].cnt/fade_max;

						if(CharStruct[i].type==CHAR_TYPE_CFADE){
							GD_SetGraphAlpha( GRP_CHAR+i*2+CharStruct[i].grp, rate*CharStruct[i].alph1 );
						}else{
							GD_SetGraphAlpha( GRP_CHAR+i*2+CharStruct[i].grp, rate*CharStruct[i].alph1 );
							rate = 1.0f-rate;
							rate = rate*rate;
							rate = 1.0f-rate;
							if(CharStruct[i].type==CHAR_TYPE_MOVE_R){
								x = 800*(1.0f-rate) + CHAR_LOCATE(CharStruct[i].w, CharStruct[i].loc1)*rate;
							}else{
								x = -768*(1.0f-rate) + CHAR_LOCATE(CharStruct[i].w, CharStruct[i].loc1)*rate;
							}

						}
					}
					break;
				case CHAR_COND_CHANGE:	
					CharStruct[i].cnt++;
					if( CharStruct[i].cnt>=fade_max){

						GD_SetGraphAlpha( GRP_CHAR+i*2+CharStruct[i].grp, CharStruct[i].alph1 );
						GD_SetGraphColor2( GRP_CHAR+i*2+CharStruct[i].grp, CharStruct[i].r1, CharStruct[i].g1, CharStruct[i].b1 );

						CharStruct[i].cond = CHAR_COND_NOMAL;

						if(CharStruct[i].type==CHAR_TYPE_CFADE){
							GD_ResetGraph( GRP_CHAR+i*2+!CharStruct[i].grp );
							GD_ReleaseTexture( BMP_CHAR+i*2+!CharStruct[i].grp );
						}
					}else{
						rate = 1.0f*CharStruct[i].cnt/fade_max;
						if(CharStruct[i].type==CHAR_TYPE_CFADE){
							if( CharStruct[i].loc2 == CharStruct[i].loc1 ){
								work = 1.0f-(1.0f-rate)*(1.0f-rate);
								GD_SetGraphAlpha( GRP_CHAR+i*2+CharStruct[i].grp, work*CharStruct[i].alph1 );

								work = rate*rate;
								GD_SetGraphAlpha( GRP_CHAR+i*2+!CharStruct[i].grp, (1.0f-work)*CharStruct[i].alph2 );
							}else{
								x  = CHAR_LOCATE(CharStruct[i].w, CharStruct[i].loc1);
								x2 = CHAR_LOCATE(CharStruct[i].w, CharStruct[i].loc2);

								GD_SetGraphAlpha( GRP_CHAR+i*2+CharStruct[i].grp, rate*CharStruct[i].alph1 );
								GD_SetGraphAlpha( GRP_CHAR+i*2+!CharStruct[i].grp, (1.0f-rate)*CharStruct[i].alph2 );
							}
						}else{
							rate = 1.0f-(1.0f-rate)*(1.0f-rate);

							x = CHAR_LOCATE(CharStruct[i].w, CharStruct[i].loc2)*(1.0f-rate) + CHAR_LOCATE(CharStruct[i].w, CharStruct[i].loc1)*rate;


							work = CharStruct[i].alph2*(1.0f-rate) + CharStruct[i].alph1*rate;
							GD_SetGraphAlpha( GRP_CHAR+i*2+CharStruct[i].grp, work );

							r = CharStruct[i].r2*(1.0f-rate) + CharStruct[i].r1*rate;
							g = CharStruct[i].g2*(1.0f-rate) + CharStruct[i].g1*rate;
							b = CharStruct[i].b2*(1.0f-rate) + CharStruct[i].b1*rate;

							GD_SetGraphColor2( GRP_CHAR+i*2+CharStruct[i].grp, r,g,b );
						}
					}
					break;
				case CHAR_COND_WAIT:	
					break;
			}
			GD_SetGraphDstMove( GRP_CHAR+i*2+CharStruct[i].grp,  ShakeDx/2+x, ShakeDy/2+y );
			GD_SetGraphDstMove( GRP_CHAR+i*2+!CharStruct[i].grp, ShakeDx/2+x2, ShakeDy/2+y );

			GD_SetGraphCenter( GRP_CHAR+i*2+CharStruct[i].grp, 400, 300 );
			GD_SetGraphCenter( GRP_CHAR+i*2+!CharStruct[i].grp, 400, 300 );

			GD_SetGraphZoom( GRP_CHAR+i*2+CharStruct[i].grp, ShakeZoom );
			GD_SetGraphZoom( GRP_CHAR+i*2+!CharStruct[i].grp, ShakeZoom );

			GD_SetGraphRoll( GRP_CHAR+i*2+CharStruct[i].grp, ShakeRoll );
			GD_SetGraphRoll( GRP_CHAR+i*2+!CharStruct[i].grp, ShakeRoll );
		}
	}
}
BOOL AVG_WaitChar( int char_no )
{
	int	index = GetCharIndex(char_no);

	if( index!=MAX_CHAR ){
		return CharStruct[index].cond!=CHAR_COND_NOMAL;
	}else{
		return FALSE;
	}
}




typedef struct{
	int		flag;
	int		mode;
	int		type;

	int		count;
	int		fd_max;

	int		mes_step;
	int		mes_add;
	int		mes_count;

	char	str[512];
	int		cno;

	char	name[32];
	int		name_count;
	int		name_flag;
	int		name_xx;

	char	LogBuf[256][512];
	char	LogName[256][64];

	int		LogCount;
	int		LogMax;

	int		wait_fade;

}MESSAGE_WINDOW;

MESSAGE_WINDOW	MessageWindow;


enum{
	MES_COUNT,
	MES_OPEN,
	MES_CLOSE,
	MES_WAITSTEP,
	MES_NEXT,

	MES_LOG,
	MES_NO_WINDOW,
};



char	*CharNameTable[32]={

	"アロウン",			
	"リアンノン",		
	"オクタヴィア",		
	"モルガン",			
	"リムリス",			
	"スィール",			

	"エルミン",			
	"エポナ",			
	"ラスティ",			
	NULL,					

	"アルサル",			
	"オガム",			
	"タリエシン",		
	"プリムラ",					
	NULL,					
	NULL,					
	NULL,					
	NULL,					
	NULL,					
	"？？？",			

	"ドルウク",				
	"ガイウス",				
	"クレオン",				
	"クレオン",				
	"レクトール",			
	"兵士",					
	NULL,					
	NULL,					
	NULL,					
	NULL,					

	"ミルディン",		
	"プィル",			
};

int AVG_GetNameNo( void )
{
	return MessageWindow.cno;
}

static char LogNameBuf[64];

void AVG_StartMessage(void)
{
	LogNameBuf[0]='\0';
	ZeroMemory( &MessageWindow, sizeof(MESSAGE_WINDOW) );
}


BOOL AVG_SetNameWindow( char *str, int reload )
{
	if(str){
		if( str[0] ){
			_mbscpy( (BYTE*)MessageWindow.name, (BYTE*)str );
			if(!reload){
				_mbscpy( (BYTE*)LogNameBuf, (BYTE*)str );
				MessageWindow.name_count = 0;
				GD_CreateTextTexture( BMP_MAIN_TEXT+1, MessageWindow.name, 24, 10, 1, 0, 0, 2 );

				GD_SetGraph( GRP_SYSTEM+5, BMP_SYSTEM, LAY_SYSTEM+3, FALSE );
				GD_SetGraphGroup( GRP_SYSTEM+5, 0x02 );
				GD_SetGraphBrightNo( GRP_SYSTEM+5, 1 );

				GD_SetGraphText( GRP_MAIN_TEXT+1, BMP_MAIN_TEXT+1, LAY_SYSTEM+4, FALSE );
				GD_SetGraphGroup( GRP_MAIN_TEXT+1, 0x02 );
				GD_SetGraphBrightNo( GRP_MAIN_TEXT+1, 1 );
			}else{
				MessageWindow.name_count = 30;
				if(reload==2){
					GD_CreateTextTexture( BMP_MAIN_TEXT+1, MessageWindow.name, 24, 10, 1, 0, 0, 2 );
				}else{
					GD_CreateTextTexture( BMP_MAIN_TEXT+1, MessageWindow.name, 24, 10, 1, 0, 0, 2, 255,128,0 );
				}
			}

			MessageWindow.name_flag = 1;


			int	len = strlen(str);
			MessageWindow.name_xx = (16-len)*6;
		}else{
			if(!reload){
				_mbscpy( (BYTE*)LogNameBuf, (BYTE*)"" );
			}else{
				MessageWindow.name_count = 0;
			}
			MessageWindow.name_flag = 0;
		}
	}else{
		if(!reload){
			_mbscpy( (BYTE*)LogNameBuf, (BYTE*)"" );
		}else{
			MessageWindow.name_count = 0;
		}
		MessageWindow.name_flag = 0;
	}
	return 1;
}
BOOL AVG_SetNameWindowFree( char *str )
{
	MessageWindow.cno = 99;
	return AVG_SetNameWindow( str, 0 );
}


BOOL AVG_SetMessageWindow( int cno, int type, int frame )
{
	int	i;

	if(cno!=-1){
		AVG_SetNameWindow( CharNameTable[cno], 0 );
		MessageWindow.cno = cno;
	}

	if(MessageWindow.flag) return 0;





	GD_SetGraph( GRP_SYSTEM+1, BMP_SYSTEM, LAY_SYSTEM+1, TRUE );
	GD_SetGraphRect( GRP_SYSTEM+1, -7,    600-192,   0, 104,  814,191 );

	GD_SetSprite( GRP_TEXT_WAIT, SPR_TEXT_WAIT+0, 0, TRUE, LAY_SYSTEM+2, FALSE, TRUE, 0 );
	GD_SetGraphDstMove( GRP_TEXT_WAIT, 676+12, 538+12 );
	GD_SetGraphBrightNo( GRP_TEXT_WAIT, 1 );
	GD_SetGraphGroup( GRP_TEXT_WAIT, 0x02 );
	GD_SetGraphBlendParam( GRP_TEXT_WAIT, BLEND_ADD );

	GD_SetSprite( GRP_TEXT_WAIT+1, SPR_TEXT_WAIT+1, 0, TRUE, LAY_SYSTEM+2, FALSE, TRUE, 0 );
	GD_SetGraphDstMove( GRP_TEXT_WAIT+1, 676+12, 538+12 );
	GD_SetGraphBrightNo( GRP_TEXT_WAIT+1, 1 );
	GD_SetGraphGroup( GRP_TEXT_WAIT+1, 0x02 );


	GD_SetGraph( GRP_SYSTEM+3, BMP_SYSTEM, LAY_SYSTEM+2, TRUE );
	GD_SetGraph( GRP_SYSTEM+4, BMP_SYSTEM, LAY_SYSTEM+2, TRUE );

	GD_SetGraphRect( GRP_SYSTEM+3, -7+541+68,   600-192+6,   541+68,   53,      68, 24 );
	GD_SetGraphRect( GRP_SYSTEM+4, -7+541+68*2, 600-192+6,   541+68*2, 53+24*0, 68, 24 );


	MUS_SetMouseRect( 0, 1, -7+541+68,   600-192+6, 68, 24 , ON );
	MUS_SetMouseRect( 0, 2, -7+541+68*2, 600-192+6, 68, 24 , ON );

	for(i=0;i<5;i++){
		GD_SetGraphBrightNo( GRP_SYSTEM+i, 1 );
		GD_SetGraphGroup( GRP_SYSTEM+i, 0x02 );
		GD_SetGraphCenter( GRP_SYSTEM+i, 400, 600-192/2 );
	}

	MessageWindow.flag  = 1;
	MessageWindow.mode   = MES_OPEN;
	MessageWindow.type   = type;
	MessageWindow.count  = 0;
	MessageWindow.fd_max = frame;

	return 1;
}
BOOL AVG_ResetMessageWindow(  int type, int frame )
{
	if(MessageWindow.flag==0) return 0;

	AVG_SetNameWindow( NULL, 0 );

	MessageWindow.mode   = MES_CLOSE;
	MessageWindow.type   = type;
	MessageWindow.count  = 0;
	MessageWindow.fd_max = frame;

	return 1;
}

BOOL AVG_SetMessageWindowDisp(  int disp )
{
	int	i;
	GD_SetGraphDisp( GRP_MAIN_TEXT, disp );

	for(i=0;i<5;i++){
		GD_SetGraphDisp( GRP_SYSTEM+i, disp );
	}

	GD_SetGraphDisp( GRP_TEXT_WAIT+0, disp );
	GD_SetGraphDisp( GRP_TEXT_WAIT+1, disp );
	
	GD_SetGraphDisp( GRP_SYSTEM+5, disp );
	GD_SetGraphDisp( GRP_SYSTEM+6, disp );
	GD_SetGraphDisp( GRP_MAIN_TEXT+1, disp );

	return 1;
}


void AVG_SetMessage( char *str, int type )
{
	int	i;
	for(i=MessageWindow.LogMax-1;i>=0;i--){
		_mbscpy( (BYTE*)MessageWindow.LogBuf[i+1], (BYTE*)MessageWindow.LogBuf[i] );
		_mbscpy( (BYTE*)MessageWindow.LogName[i+1], (BYTE*)MessageWindow.LogName[i] );
	}
	_mbscpy( (BYTE*)MessageWindow.str, (BYTE*)str );
	_mbscpy( (BYTE*)MessageWindow.LogBuf[0], (BYTE*)str );
	_mbscpy( (BYTE*)MessageWindow.LogName[0], (BYTE*)LogNameBuf );
	
	MessageWindow.LogMax = LIM( MessageWindow.LogMax+1, 0, 255 );

	GD_CreateTextTexture( BMP_MAIN_TEXT, str, 24, 26, 4, 0, 6, 2 );

	GD_SetGraphText( GRP_MAIN_TEXT, BMP_MAIN_TEXT, LAY_MAIN_TEXT, TRUE );
	GD_SetGraphGroup( GRP_MAIN_TEXT, 0x02 );
	GD_SetGraphDstMove( GRP_MAIN_TEXT, -7+44+36, 600-192+33 );
	GD_SetGraphBrightNo( GRP_MAIN_TEXT, 1 );
	GD_SetGraphCenter( GRP_MAIN_TEXT, 400, 600-192/2 );
	GD_SetGraphScaleParam( GRP_MAIN_TEXT, SCALE_HIGHT );

	switch(MessageWindow.mode){
		default:	MessageWindow.mode = MES_COUNT;		break;
		case MES_OPEN:
		case MES_CLOSE:		MessageWindow.mode = MES_OPEN;	break;
	}

	MessageWindow.mes_add   = type;
	MessageWindow.mes_step  = 0;
	MessageWindow.mes_count = 0;

	GD_SetGraphTextStep( GRP_MAIN_TEXT, MessageWindow.mes_step );
	GD_SetGraphTextCount( GRP_MAIN_TEXT, MessageWindow.mes_count );


}
void AVG_AddMessage( char *str, int type )
{
	_mbscat( (BYTE*)MessageWindow.str, (BYTE*)"\\k" );
	_mbscat( (BYTE*)MessageWindow.str, (BYTE*)str );

	_mbscat( (BYTE*)MessageWindow.LogBuf[0], (BYTE*)str );

	switch(MessageWindow.mode){
		default:	MessageWindow.mode = MES_COUNT;		break;
		case MES_OPEN:
		case MES_CLOSE:		MessageWindow.mode = MES_OPEN;	break;
	}
	MessageWindow.mes_add   = type;

	GD_UpdateTextTexture( BMP_MAIN_TEXT, MessageWindow.str );

	MessageWindow.mes_count = 0;
	GD_SetGraphTextCount( GRP_MAIN_TEXT, MessageWindow.mes_count );
	GD_SetGraphTextStep( GRP_MAIN_TEXT, MessageWindow.mes_step );
}

void AVG_ResetMessage( void )
{
	GD_ResetGraph( GRP_MAIN_TEXT );
	GD_ReleaseTexture( BMP_MAIN_TEXT );
	MessageWindow.flag = 0;
}


BOOL AVG_WaitMessage( void )
{
	return MessageWindow.mode==MES_NEXT;
}


void AVG_ControlMessage( void )
{
	int		fd_max = AVG_EffCnt(MessageWindow.fd_max);
	int		i;
	int		select = MUS_GetMouseNo( -1 );
	int		step_flag;
	int		msg_cut_mode=0;

	if(GameKey.mes_cut_mode){
		Avg.msg_cut_mode = !Avg.msg_cut_mode;
	}else if(GameKey.click || GameKey.cansel){
		msg_cut_mode = Avg.msg_cut_mode;
		Avg.msg_cut_mode = 0;
	}

	if( MessageWindow.flag ){
		int	nm_max = AVG_EffCnt(-2);
		int	nm_count;

		if(nm_max==0){
			if(MessageWindow.name_flag){
				nm_max=30;
				nm_count=30;
			}else{
				nm_max=30;
				nm_count=0;
			}
		}else{
			if(MessageWindow.mode==MES_OPEN || MessageWindow.name_flag==0){
				MessageWindow.name_count = LIM( MessageWindow.name_count-1, 0, nm_max );
			}else{
				MessageWindow.name_count = LIM( MessageWindow.name_count+1, 0, nm_max );
			}
			nm_count = MessageWindow.name_count;
		}
		float	xx = 256.0f * POW_CONUT( nm_max-nm_count, nm_max );

		if(MessageWindow.mode!=MES_NO_WINDOW){
			GD_SetGraphDisp( GRP_SYSTEM+5, TRUE );

			GD_SetGraphDisp( GRP_MAIN_TEXT+1, TRUE );

			GD_SetGraphRect( GRP_SYSTEM+5, 64-xx,   600-192-24,   768,  0,      256, 55 );

			GD_SetGraphDstMove( GRP_MAIN_TEXT+1, 64+30+MessageWindow.name_xx-xx,   600-192-11 );

			GD_SetGraphAlpha( GRP_SYSTEM+5, 255*nm_count/nm_max );

			GD_SetGraphAlpha( GRP_MAIN_TEXT+1, 255*nm_count/nm_max );
		}

		switch(MessageWindow.mode){
			default:
			case MES_LOG:
				if(GameKey.pdown){
					MessageWindow.LogCount--;
					if(MessageWindow.LogCount==0){
						MessageWindow.mode = MES_WAITSTEP;

						GD_SetGraphDisp( GRP_MAIN_TEXT+2, FALSE );
						GD_SetGraphDisp( GRP_MAIN_TEXT, TRUE );

						AVG_SetNameWindow( MessageWindow.LogName[0], 2 );
					}else{
						AVG_SetNameWindow( MessageWindow.LogName[MessageWindow.LogCount], 1 );
						GD_UpdateTextTexture( BMP_MAIN_TEXT+2, MessageWindow.LogBuf[MessageWindow.LogCount] );
					}
				}else if(GameKey.pup){
					MessageWindow.LogCount = min( MessageWindow.LogCount+1, MessageWindow.LogMax-1 );

					AVG_SetNameWindow( MessageWindow.LogName[MessageWindow.LogCount], 1 );
					GD_UpdateTextTexture( BMP_MAIN_TEXT+2, MessageWindow.LogBuf[MessageWindow.LogCount] );
				}else if(GameKey.click || GameKey.cansel){
					MessageWindow.LogCount=0;
					MessageWindow.mode = MES_WAITSTEP;
					GD_SetGraphDisp( GRP_MAIN_TEXT+2, FALSE );
					GD_SetGraphDisp( GRP_MAIN_TEXT, TRUE );

					AVG_SetNameWindow( MessageWindow.LogName[0], 2 );
				}
				break;
			case MES_NO_WINDOW:
				if(GameKey.click || GameKey.cansel || GameKey.diswin){
					MessageWindow.mode = MES_WAITSTEP;
					AVG_SetMessageWindowDisp( TRUE );
					SND_Cansel();
				}
				break;

			case MES_NEXT:
				MessageWindow.wait_fade = 0;;
				GD_SetGraphDisp( GRP_TEXT_WAIT, FALSE );
				GD_SetGraphDisp( GRP_TEXT_WAIT+1, FALSE );
				appData.draw_flag = 1;
				break;

			case MES_WAITSTEP:

				if(MessageWindow.mes_count){
					appData.draw_flag = 1;
					MessageWindow.mes_count=0;
					MessageWindow.mes_step++;
				}
				GD_SetGraphTextCount( GRP_MAIN_TEXT, MessageWindow.mes_count );
				step_flag = GD_SetGraphTextStep( GRP_MAIN_TEXT, MessageWindow.mes_step );

				
				
				if(step_flag){
					GD_SetGraphDisp( GRP_TEXT_WAIT, TRUE );
					GD_SetGraphAlpha( GRP_TEXT_WAIT, 255*MessageWindow.wait_fade/30 );

					GD_SetGraphDisp( GRP_TEXT_WAIT+1, FALSE );
				}else{
					GD_SetGraphDisp( GRP_TEXT_WAIT, FALSE );
					GD_SetGraphDisp( GRP_TEXT_WAIT+1, TRUE );
					GD_SetGraphAlpha( GRP_TEXT_WAIT+1, 255*MessageWindow.wait_fade/30 );
				}
				MessageWindow.wait_fade = LIM(MessageWindow.wait_fade+1,0,30);;

				
				if(select==-1 || AVG_GetMesCut() ){
					int	click = AVG_WaitKey();
					if( step_flag ){
						if( MessageWindow.mes_add==1 || click || GameKey.pdown ){
							MessageWindow.mode = MES_NEXT;
						}
					}else{
						if( click || GameKey.pdown ){
							MessageWindow.mode = MES_COUNT;
						}
					}
				}
				GD_SetGraphDstMove( GRP_SYSTEM+3, -7+541+68,   600-192+6 );
				GD_SetGraphSrcRect( GRP_SYSTEM+3, 541+68,   53+24*0, 68, 24 );
				GD_SetGraphSrcRect( GRP_SYSTEM+4, 541+68*2, 53+24*0, 68, 24 );
				if(select==1 || Avg.msg_cut_mode){
					if(Avg.msg_cut_mode)
						GD_SetGraphDstMove( GRP_SYSTEM+3, -7+541+68,   600-192+6+1 );
					GD_SetGraphSrcRect( GRP_SYSTEM+3, 541+68,   53+24*1, 68, 24 );
				}

				if(select==2)	GD_SetGraphSrcRect( GRP_SYSTEM+4, 541+68*2, 53+24*1, 68, 24 );

				if(GameKey.pup && MessageWindow.LogMax>1){
					if(!Avg.demo){
						MessageWindow.LogCount++;

						AVG_SetNameWindow( MessageWindow.LogName[MessageWindow.LogCount], 1 );

						GD_CreateTextTexture( BMP_MAIN_TEXT+2, MessageWindow.LogBuf[MessageWindow.LogCount], 24, 26, 4, 0, 6, 2, 255,128,0 );

						GD_SetGraphText( GRP_MAIN_TEXT+2, BMP_MAIN_TEXT+2, LAY_MAIN_TEXT, TRUE );
						GD_SetGraphGroup( GRP_MAIN_TEXT+2, 0x02 );
						GD_SetGraphDstMove( GRP_MAIN_TEXT+2, -7+44+36, 600-192+33 );
						GD_SetGraphBrightNo( GRP_MAIN_TEXT+2, 1 );
						GD_SetGraphCenter( GRP_MAIN_TEXT+2, 400, 600-192/2 );
						GD_SetGraphScaleParam( GRP_MAIN_TEXT+2, SCALE_HIGHT );
						
						GD_SetGraphDisp( GRP_MAIN_TEXT, FALSE );
	
						MessageWindow.mode = MES_LOG;
					}
				}else if( GameKey.click ){
					switch(select){
						case 0:
							break;
						case 1:
							SND_Ok();
							GD_SetGraphDstMove( GRP_SYSTEM+3, -7+541+68,   600-192+6+1 );

							Avg.msg_cut_mode = !msg_cut_mode;
							break;
						case 2:
							SND_Ok();

							MessageWindow.mode = MES_NO_WINDOW;
							AVG_SetMessageWindowDisp( FALSE );
							break;
					}
				}else if(GameKey.diswin || GameKey.cansel){
					SND_Ok();
					MessageWindow.mode = MES_NO_WINDOW;
					AVG_SetMessageWindowDisp( FALSE );
				}
				break;

			case MES_COUNT:
				MessageWindow.wait_fade = 0;;
				GD_SetGraphDisp( GRP_TEXT_WAIT, FALSE );
				GD_SetGraphDisp( GRP_TEXT_WAIT+1, FALSE );

				MessageWindow.mes_count += AVG_MsgCnt();

				if( GD_SetGraphTextCount( GRP_MAIN_TEXT, MessageWindow.mes_count ) ){
					MessageWindow.mode = MES_WAITSTEP;
					break;
				}
				break;
			case MES_OPEN:
			case MES_CLOSE:
				MessageWindow.count++;
				if(MessageWindow.count<fd_max){
					float	rate;
					int		count;
					if(MessageWindow.mode==MES_CLOSE){
						count = fd_max-MessageWindow.count;
					}else{
						count = MessageWindow.count;
					}
					rate = POYO_CONUT( count, fd_max, fd_max-5 );

					for(i=0;i<5;i++){
						GD_SetGraphAlpha( GRP_SYSTEM+i, 255*count/fd_max );
						GD_SetGraphZoomXY( GRP_SYSTEM+i, rate*3/4+0.25f, rate*3/4+0.25f );
					}


					GD_SetGraphAlpha( GRP_MAIN_TEXT, 255*count/fd_max );
					GD_SetGraphZoomXY( GRP_MAIN_TEXT, rate*3/4+0.25f, rate*3/4+0.25f );

					GD_SetGraphDisp( GRP_TEXT_WAIT+0, FALSE );
					GD_SetGraphDisp( GRP_TEXT_WAIT+1, FALSE );
				}else{
					if(MessageWindow.mode==MES_CLOSE){
						for(i=0;i<7;i++){
							GD_ResetGraph( GRP_SYSTEM+i );
						}
						GD_ResetGraph( GRP_MAIN_TEXT+1 );
						GD_ReleaseTexture( BMP_MAIN_TEXT+1 );

						GD_ResetGraph( GRP_TEXT_WAIT+0 );
						GD_ResetGraph( GRP_TEXT_WAIT+1 );

						AVG_ResetMessage();

					}else{
						for(i=0;i<5;i++){
							GD_SetGraphAlpha( GRP_SYSTEM+i, 255 );
							GD_SetGraphZoom( GRP_SYSTEM+i, 1.0f );
						}


						GD_SetGraphAlpha( GRP_MAIN_TEXT, 255 );
						GD_SetGraphZoom( GRP_MAIN_TEXT, 1.0f );

						MessageWindow.mode=MES_COUNT;
						MessageWindow.count=0;
						MessageWindow.fd_max=0;
					}
				}
				break;
		}
	}
}



#define SELECT_WINODW_X			(184)
#define SELECT_WINODW_Y(N)		(75+(5-(N))*13)
#define SELECT_WINODW_W			(500)
#define SELECT_WINODW_H(N)		(30+(N)*26)

#define SELECT_MAX				(8)
#define SELECT_MES_MAX			(256)

typedef struct{
	int		flag;		
	int		ex;
	int		disp;
	int		cond;		
	int		cnt;		

	int		mnum;		

	char	mes[SELECT_MAX][SELECT_MES_MAX];	
	char	mes_sav[SELECT_MAX][SELECT_MES_MAX];	

	int		res;		
	int		select;		

	int		gf_no[SELECT_MAX];
	int		gf_value[SELECT_MAX];
	int		nanido_flag;
}SELECT_WINDOW;
SELECT_WINDOW	SelectWindow;

void AVG_SetSelectMessage( char *mes, int game_flag_no, int game_flag_value )
{
	wsprintf( SelectWindow.mes[SelectWindow.mnum], "%c%c．%s", '１'>>8, '１'+SelectWindow.mnum, mes );
	wsprintf( SelectWindow.mes_sav[SelectWindow.mnum], "%s", mes );

	SelectWindow.gf_no[SelectWindow.mnum]    = game_flag_no;
	SelectWindow.gf_value[SelectWindow.mnum] = game_flag_value;
	SelectWindow.mnum = min( SelectWindow.mnum+1, SELECT_MAX );
}

void AVG_SetSelectWindow( void )
{
	int		i;
	appData.draw_flag = 1;

	SelectWindow.flag = 1;	
	SelectWindow.cond = 1;	
	SelectWindow.cnt  = 0;	
	SelectWindow.disp = 1;
	SelectWindow.res    =-1;
	SelectWindow.select =-1;
	SelectWindow.nanido_flag =0;

	for(i=0;i<SelectWindow.mnum;i++){
		if( ESC_GetGameFlag(SelectWindow.gf_no[i])==SelectWindow.gf_value[i] ){
			GD_SetGraphx9( GRP_SELECT_TEXT+8+i, BMP_SYSTEM, LAY_MAIN_TEXT, TRUE );
			GD_SetGraphSrcRect( GRP_SELECT_TEXT+8+i, 952, 128+72, 24*3, 24*3 );
			GD_SetGraphDstRect( GRP_SELECT_TEXT+8+i, SELECT_WINODW_X-24, SELECT_WINODW_Y(SelectWindow.mnum) + i*48-17, 16*24+48,  60 );
			GD_SetGraphGroup( GRP_SELECT_TEXT+8+i, 0x03 );
			GD_SetGraphBrightNo( GRP_SELECT_TEXT+8+i, 1 );


			GD_CreateTextTexture( BMP_SELECT_TEXT+i, SelectWindow.mes[i], 24, 16, 1, 0, 0, 2 );
			GD_SetGraphText( GRP_SELECT_TEXT+i, BMP_SELECT_TEXT+i, LAY_MAIN_TEXT+1, TRUE );
			GD_SetGraphDstMove( GRP_SELECT_TEXT+i,  SELECT_WINODW_X, SELECT_WINODW_Y(SelectWindow.mnum) + i*48 );
			GD_SetGraphGroup( GRP_SELECT_TEXT+i, 0x03 );
			GD_SetGraphBrightNo( GRP_SELECT_TEXT+i, 1 );

			MUS_SetMouseRect( 0, 16+i, SELECT_WINODW_X, SELECT_WINODW_Y(SelectWindow.mnum) + i*48, 24*16, 26, ON );
		}
	}

}
void AVG_ResetSelectWindow( void )
{
	int		i;
	SelectWindow.cnt  = 0;	
	SelectWindow.res   = SelectWindow.select;
	SelectWindow.flag  = 0;
	SelectWindow.cond  = 0;
	SelectWindow.mnum  = 0;
	SelectWindow.nanido_flag =0;

	for(i=0;i<SELECT_MAX;i++){
		GD_ReleaseTexture( BMP_SELECT_TEXT+i );
		GD_ResetGraph( GRP_SELECT_TEXT+i );
		GD_ResetGraph( GRP_SELECT_TEXT+8+i );

		MUS_ResetMouseRect( 0, 16+i );
	}
}

int AVG_WaitSelect( int nanido )
{
	SelectWindow.nanido_flag = nanido;
	return SelectWindow.res;
}

void AVG_ControlSelectWindow( void )
{
	int		i,j;
	int		cmax = AVG_EffCnt(-1);
	int		px,py;
	int		select = MUS_GetMouseNo( -1 )-16;
	int		click = GameKey.click;
	int		h = 0, max_cnt=0;
	static int	bak_select=-1;;

	if( SelectWindow.flag && SelectWindow.disp  ){
		switch(SelectWindow.cond){
			case 0:
				if(GameKey.u)	MUS_SetMousePosRect( appData.Hwnd, 0, (select<0)? 16 : select+16-1 );
				if(GameKey.d)	MUS_SetMousePosRect( appData.Hwnd, 0, (select<0)? 16 : select+16+1 );
				for(j=0;j<SelectWindow.mnum+1;j++){
					GD_SetGraphSrcRect( GRP_SELECT_TEXT+8+j, 952, 128+72, 24*3, 24*3 );
					if(GameKey.num[j]){
						select = j-1;
						click = 1;
					}
				}
				if(bak_select != select){
					if(SelectWindow.nanido_flag){
						switch(select){
						case 0:
							GD_UpdateTextTexture( BMP_MAIN_TEXT, "「シミュレーションゲームなんてしたことがない」\nという人向けのモードです。\nじっくり、お話を楽しみたい人にも。" );
							break;
						case 1:
							GD_UpdateTextTexture( BMP_MAIN_TEXT, "ゲーム好きには物足りない設定になってはいますが、気軽にプレイできます。" );
							break;
						case 2:
							GD_UpdateTextTexture( BMP_MAIN_TEXT, "途中で全滅することも、きっとあるでしょう。\nしかし、ゲームとしてバランスの取れたモードです。" );
							break;
						case 3:
							GD_UpdateTextTexture( BMP_MAIN_TEXT, "敵の思考ルーチンが少し賢くなり、敵パラメータにも強化補正があります。\n傭兵を駆使しないと、ちょっとつらいかも。\nハードゲーマーは、当然ここから。" );
							break;
						case 4:
							GD_UpdateTextTexture( BMP_MAIN_TEXT, "無理です。\nごめんなさい。" );
							break;
						default:
							GD_UpdateTextTexture( BMP_MAIN_TEXT, "難易度を選んでください。" );
							break;
						}
					}
				}

				if( select>=0 ){
					if(bak_select != select){
						SND_Select();
					}
					GD_SetGraphSrcRect( GRP_SELECT_TEXT+8+select, 952-72, 128, 24*3, 24*3 );
				}
				if( click && select>=0 ){
		
					SelectWindow.select = select;
					AVG_ResetSelectWindow();

		
					Avg.msg_cut = OFF;
		
					SND_Ok();
				}
				bak_select = select;
				break;
			case 1:
				SelectWindow.cnt++;
				if(SelectWindow.cnt>=cmax){

					SelectWindow.cond=0;
					for(i=0;i<SelectWindow.mnum;i++){
						if( ESC_GetGameFlag(SelectWindow.gf_no[i])==SelectWindow.gf_value[i] ){
							GD_SetGraphAlpha( GRP_SELECT_TEXT+i, 255 );
							GD_SetGraphAlpha( GRP_SELECT_TEXT+8+i, 256 );
						}
					}
				}else{
					for(i=0;i<SelectWindow.mnum;i++){
						if( ESC_GetGameFlag(SelectWindow.gf_no[i])==SelectWindow.gf_value[i] ){
							GD_SetGraphAlpha( GRP_SELECT_TEXT+i, SelectWindow.cnt*255/cmax );
							GD_SetGraphAlpha( GRP_SELECT_TEXT+8+i, SelectWindow.cnt*255/cmax );
						}
					}
				}
				break;
			case 2:
				break;
		}
	}
}



#define SCRIPT_MAX	1024	
#define MES_MAX		4096	


#define SYS_POS		0
#define VIS_POS		(SYS_POS+SCRIPT_MAX*MES_MAX/8)
#define FLAG_POS	(VIS_POS+2*VISUAL_MAX*10)


static BYTE ScenarioFlag[SCRIPT_MAX][MES_MAX/8];
BYTE	SaveVisualFlag[2][VISUAL_MAX][10];

#include "DAT_ScriptListName.h"
static char *SysFlagName = "Sys.sav";

static int ScenarioNo = 0;

static int ScenarioNo2 = 0;
static int MesNo = 0;

static long	VI_EventVoiceNo = -1;


void AVG_SetScenario( char *scenario_name )
{
	int	i;
	EXEC_SLoad( scenario_name );

	_mbscpy( (BYTE*)GoScript, (BYTE*)scenario_name );

	ScenarioNo = Str2Digit(GoScript)*10;

	ScenarioNo2=0;
	for(i=0;i<SCRIPT_MAX;i++){
		if( ScriptListName[i] ){
			if( _stricmp( GoScript, ScriptListName[i] )==0){
				ScenarioNo2=i;
				break;
			}
		}else{
			break;
		}
	}

	VI_EventVoiceNo = -1;
}


void AVG_SetScenarioFlag( int mes_no )
{
	if( mes_no >= SCRIPT_MAX ){
		DebugBox(NULL,"ちょぉいと文章が多過ぎやしませんか？[%d]",mes_no);
	}else{
		STD_SetBit( (BYTE*)ScenarioFlag[ScenarioNo2], 1, mes_no, ON );
		if(mes_no%100==0)
			STD_WriteFilePos( SysFlagName, (BYTE*)ScenarioFlag, SYS_POS, SCRIPT_MAX*MES_MAX/8 );
		MesNo=mes_no;
	}
}
void AVG_SavetScenarioFlag( void )
{
	STD_WriteFilePos( SysFlagName, (BYTE*)ScenarioFlag, SYS_POS, SCRIPT_MAX*MES_MAX/8 );
}


BOOL AVG_CheckScenarioFlag( void )
{
	if(MesNo!=-1){
		return STD_GetBit( (BYTE*)ScenarioFlag[ScenarioNo2], 1, MesNo+1 ) || Avg.omake;
	}else{
		return FALSE;
	}
	return TRUE;
}


void AVG_LoadScenarioFlag( void )
{
	if( !STD_ReadFilePos( SysFlagName, (BYTE*)ScenarioFlag, SYS_POS, SCRIPT_MAX*MES_MAX/8 ) ){
		ZeroMemory( ScenarioFlag, SCRIPT_MAX*MES_MAX/8 );
	}
}

void AVG_SaveVisual( int vh, int no )
{
	SaveVisualFlag[vh][no/10][no%10]=TRUE;
	STD_WriteFilePos( SysFlagName, (BYTE*)SaveVisualFlag, VIS_POS, 2*VISUAL_MAX*10 );
}

void AVG_LoadVisual( void )
{
	if( !STD_ReadFilePos( SysFlagName, (BYTE*)SaveVisualFlag, VIS_POS, 2*VISUAL_MAX*10 ) ){
		ZeroMemory( SaveVisualFlag, 2*VISUAL_MAX*10 );
	}
}

void AVG_SetGameFlag( int fno, int flag )
{
	ESC_SetGameFlag( fno, flag );
	STD_WriteFilePos( SysFlagName, (BYTE*)ESC_GameFlagBuf, FLAG_POS, sizeof(int)*ESC_FLAG_MAX );
}
void AVG_LoadGameFlag( void )
{
	if( !STD_ReadFilePos( SysFlagName, (BYTE*)ESC_GameFlagBuf, FLAG_POS, sizeof(int)*ESC_FLAG_MAX ) ){
		ZeroMemory( ESC_GameFlagBuf, sizeof(int)*ESC_FLAG_MAX );
	}
}






#include <soundDS.h>

enum{
	MUSIC_STOP=0,		
	MUSIC_PLAY,			
	MUSIC_PAUSE,		
	MUSIC_FADEIN,		
	MUSIC_FADEOUT,		
	MUSIC_FADEVOL,		
};

typedef struct{
	int		mus_no;		
	int		handle;
	int		mode;		
	int		fcnt;		
	int		fmax;		
	int		loop;		
	int		vol;		
	int		prev_vol;	
	int		next_vol;	
	DWORD	bar[2];
}PLAY_MUSIC;

#define WAVE_STREAM_NUM	4
#define WAVE_SOUND_NUM	16
#define VOICE_MAX		4
#define VOICE_CHAR_MAX	16

PLAY_MUSIC		PlayMusic[WAVE_STREAM_NUM] = {
	{ -1, -1, MUSIC_STOP },
	{ -1, -1, MUSIC_STOP },
	{ -1, -1, MUSIC_STOP },
	{ -1, -1, MUSIC_STOP },
};
int			DB_No=0;
int	PlayMusicNo   = -1;
int	PlayMusicVol  = 255;
int	PlayMusicLoop = TRUE;


typedef struct{
	int		flag;
	int		se_no;
	int		dno;
	int		fcnt;		
	int		fmax;		
	int		loop;
	int		vol;
	int		prev_vol;	
	int		next_vol;	
	int		stop;
}SE_STRUCT;
SE_STRUCT	SeStruct[WAVE_SOUND_NUM];

#define SE_FILE_DEF		"se_%04d"
#define SE_FREE_MAX		8

int		FreeSeHandle[SE_FREE_MAX] = { -1,-1,-1,-1, -1,-1,-1,-1 };


typedef struct{
	int		handle;		
	int		char_no;	
	int		vol;		

	int		fcnt;		
	int		fmax;		
	int		prev_vol;	
	int		next_vol;	
}VOICE_STRUCT;
VOICE_STRUCT	VoiceStruct[VOICE_MAX];

int		DefaultCharName=0;

void AVG_InitSound( void )
{
	int	i;

	for( i=0 ; i<WAVE_STREAM_NUM ; i++ ){
		PlayMusic[i].handle=-1;
	}
	readFile = new ClReadFile;
	readFile->OpenPackFile( "BGM" );
	readFile->OpenPackFile( "Voice" );
	readFile->OpenPackFile( "Se" );
	
	lpSoundDS = new ClSoundDS( appData.Hwnd, readFile, FALSE );

	for( i=0 ; i<VOICE_MAX ; i++ ){
		VoiceStruct[i].handle=-1;
	}
}
void AVG_TermSound( void )
{
	Sleep(50);
	if(lpSoundDS)	delete lpSoundDS;	lpSoundDS=NULL;
	Sleep(50);
	if(readFile)	delete readFile;	readFile=NULL;
	Sleep(50);
}


void AVG_SetVI_EventVoiceNo( int vi_ee )
{
	if(vi_ee!=-1){
		VI_EventVoiceNo = vi_ee;
	}
}



void AVG_SetVoiveFlag( int flag )
{
	Avg.voice = flag;
	if( Avg.voice==0 ){
		AVG_FadeOutVoiceAll( 0 );
	}
}
void AVG_StopVoice( int vc_no )
{
	if( VoiceStruct[vc_no].handle!=-1 ){
		lpSoundDS->Stop( VoiceStruct[vc_no].handle );
		lpSoundDS->ReleaseSoundObj(VoiceStruct[vc_no].handle);
	}
	ZeroMemory( &VoiceStruct[vc_no], sizeof(VoiceStruct[vc_no]) );
	VoiceStruct[vc_no].handle=-1;
}
void AVG_FadeOutVoice( int vc_no, int fade )
{
	VoiceStruct[vc_no].prev_vol = VoiceStruct[vc_no].vol;
	VoiceStruct[vc_no].next_vol = 0;
	VoiceStruct[vc_no].fcnt = 0;
	VoiceStruct[vc_no].fmax = fade;
}
int AVG_WaitVoice( int vc_no )
{
	if( Avg.voice==1 ){
		return VoiceStruct[vc_no].handle==-1;
	}else{
		return 1;
	}
}

void AVG_FadeOutVoiceAll( int fade )
{
	int	i;
	for( i=0 ; i<VOICE_MAX ; i++ ){
		if( VoiceStruct[i].handle!=-1 ){
			AVG_FadeOutVoice( i, fade );
		}
	}
}
void AVG_SetDirectVoumeVoiceAll( int voice_vol )
{
	int	i;
	int	vol;
	for( i=0 ; i<VOICE_MAX ; i++ ){
		if( VoiceStruct[i].handle!=-1 ){
			vol = VoiceStruct[i].vol * voice_vol / 256;
			lpSoundDS->SetVolume( VoiceStruct[i].handle, LIM(vol,0,255) );
		}
	}
}


short	BTVoiceTable[22][2][4]={

	{
		{ 8, 9,11,12},
		{13,14,15,16}
	},{

		{ 0, 0, 0, 0},
		{46,47,48,49}
	},{

		{105,106,107,108},
		{109,110,111,112}
	},{

		{94,95,96,94},
		{97,98,99,97}
	},{
		{ 0, 0, 0, 0},
		{ 0, 0, 0, 0}
	},{

		{120,121,122,122},
		{125,126,127,128}
	},{
		{ 0, 0, 0, 0},
		{ 0, 0, 0, 0}
	},{
		{ 0, 0, 0, 0},
		{ 0, 0, 0, 0}
	},{

		{132,133,134,136},
		{138,139,138,139}
	},{
		{ 0, 0, 0, 0},
		{ 0, 0, 0, 0}
	},{

		{23,24,26,28},
		{31,32,33,31}
	},{

		{ 0, 0, 0, 0},
		{85,86,87,85}
	},{

		{55,56,57,55},
		{58,59,62,62}
	},{
		{ 0, 0, 0, 0},
		{ 0, 0, 0, 0},
	},{
		{ 0, 0, 0, 0},
		{ 0, 0, 0, 0},
	},{
		{ 0, 0, 0, 0},
		{ 0, 0, 0, 0},
	},{
		{ 0, 0, 0, 0},
		{ 0, 0, 0, 0},
	},{
		{ 0, 0, 0, 0},
		{ 0, 0, 0, 0},
	},{
		{ 0, 0, 0, 0},
		{ 0, 0, 0, 0},
	},{
		{ 0, 0, 0, 0},
		{ 0, 0, 0, 0},
	},{
		{ 0, 0, 0, 0},
		{ 0, 0, 0, 0},
	},{

		{142,143,149,150},
		{ 0, 0, 0, 0},
	}
};
void AVG_PlayBTVoice( int btno, int cno )
{
	char	fname[256];
	int		i;
	if(cno<0) return ;
	if(cno>=22) return ;

	if( !Avg.voice) return;

	for(i=0;i<VOICE_MAX;i++){
		if(VoiceStruct[i].handle==-1){
			break;
		}
	}
	if(i==VOICE_MAX){
		i=0;
	}
	int	bsno = BTVoiceTable[cno][btno][rand()%4];
	if(bsno==0) return ;
	wsprintf( fname, "%04d_%03d_1", bsno, cno );
	if( lpSoundDS->playVoice( VoiceStruct[i].handle, pack_voice, fname, 1, 255 )!=Cl_OK ){
		wsprintf( fname, "%04d_%03d", bsno, cno );
		if( lpSoundDS->playVoice( VoiceStruct[i].handle, pack_voice, fname, 1, 255 )!=Cl_OK ){
			AddVoiceDialogMes("%04d_%03d BTTLE VOICE ERR", bsno, cno );
		}
	}
	VoiceStruct[i].prev_vol = VoiceStruct[i].next_vol = VoiceStruct[i].vol = 255;
}

char	*OmakeVoiceTable[13]={
	"AROUN_KANSOU",
	"riannon_kansou",
	"okutavia_kansou",
	"morugan_kansou",
	"RIMURISU_KANSOU",
	"suiru_kansou",
	"erumin_kansou",
	"epona_kansou",
	"lasty_kansou",
	NULL,
	"arusaru_kansou",
	"OGAMU_KANSOU",
};

void AVG_PlayOmakeVoice( int cno )
{
	char	fname[256];
	if(cno<0) return ;
	if(cno>=13) return ;

	if( !Avg.voice) return;

	AVG_StopVoice(0);

	if( OmakeVoiceTable[cno] ){
		wsprintf( fname, OmakeVoiceTable[cno] );
		if( lpSoundDS->playVoice( VoiceStruct[0].handle, pack_voice, fname, 1, 255 )!=Cl_OK ){
		}
		VoiceStruct[0].prev_vol = VoiceStruct[0].next_vol = VoiceStruct[0].vol = 255;
	}
}

void AVG_PlayVoice( int vc_no, int cno, int sno, int vno, int vol, int loop )
{
	char	fname[256];
	int	i=0;

	if(Avg.ero_voice){
		if(ESC_GetFlag(6)){
			if(cno==0){
				return ;
			}
		}
	}

	AVG_StopVoice(vc_no);

	if(VI_EventVoiceNo==-1){
		if(sno==-1)	sno = ScenarioNo;
	}else{
		if(sno==-1)	sno = VI_EventVoiceNo;
	}

	if(cno==-1)	cno = AVG_GetNameNo();
	
	
	


	wsprintf( fname, "%05d_%03d_%03d.ogg", sno, vno, cno );
	char	fname2[256];
	char	fname3[256];
	wsprintf( fname2, "voice\\%s", fname );
	wsprintf( fname3, "voice\\buf\\%s", fname );
	CopyFile(fname2,fname3,TRUE);

	if( AVG_GetMesCut() || !Avg.voice){
		if( DefaultCharName ){
		}else{
			wsprintf( fname, "%05d_%03d_%03d", sno, vno, cno );
			if( lpSoundDS->CheckVoiceFile( pack_voice, fname )!=Cl_OK ){
				AddVoiceDialogMes( "[%05d_%03d_%03d] ERR_NO_FILE", sno, vno, cno );
			}else{
				AddVoiceDialogMes( "[%05d_%03d_%03d] FIND_FILE", sno, vno, cno );
			}
		}
		return ;
	}
	
	VoiceStruct[vc_no].prev_vol = VoiceStruct[vc_no].next_vol = VoiceStruct[vc_no].vol = vol;
	if( DefaultCharName ){
		wsprintf( fname, "%05d_%03d_%03d_a", sno, vno, cno );
		if( lpSoundDS->playVoice( VoiceStruct[vc_no].handle, pack_voice, fname, !loop, LIM(VoiceStruct[vc_no].vol*Avg.voice_vol/256,0,255) )!=Cl_OK ){
			wsprintf( fname, "%05d_%03d_%03d", sno, vno, cno );
			if( lpSoundDS->playVoice( VoiceStruct[vc_no].handle, pack_voice, fname, !loop, LIM(VoiceStruct[vc_no].vol*Avg.voice_vol/256,0,255) )!=Cl_OK ){
				DebugBox(NULL,"音声の再生に失敗しました[%s]", fname );
			}
		}
	}else{
		wsprintf( fname, "%05d_%03d_%03d", sno, vno, cno );
		if( lpSoundDS->playVoice( VoiceStruct[vc_no].handle, pack_voice, fname, !loop, LIM(VoiceStruct[vc_no].vol*Avg.voice_vol/256,0,255) )!=Cl_OK ){
			AddVoiceDialogMes( "[%05d_%03d_%03d] ERR_NO_FILE", sno, vno, cno );

		}else{
			AddVoiceDialogMes( "[%05d_%03d_%03d] PLAY", sno, vno, cno );
		}
	}
}


void AVG_ReleaseSeAll( void )
{
	int	i;

	for(i=0;i<SE_FREE_MAX;i++) {
		if( FreeSeHandle[i] != -1){
			AVG_StopSE( FreeSeHandle[i] );
			FreeSeHandle[i] = -1;
		}
	}
	
	for(i=0;i<WAVE_SOUND_NUM;i++)AVG_StopSE2( i, 0 );
	ZeroMemory( SeStruct, sizeof(SE_STRUCT)*WAVE_SOUND_NUM );
}
void AVG_FadeSeAll( int fade )
{
	int	i;
	for(i=0;i<WAVE_SOUND_NUM;i++)
		AVG_StopSE2(i, fade );
}

void AVG_PlaySE3( int se_no, int vol )
{
	int		ret=-1,i;
	if(!Avg.se) return ;
	
	
	for(i=0;i<SE_FREE_MAX;i++) {
		if( FreeSeHandle[i] == -1){




			if( PlayEffectNum( FreeSeHandle[i], se_no, vol*Avg.se_vol/256, FALSE ) ){
				DebugBox( NULL, "SEファイルが見つかりません[%04d].wav", se_no );
			}
			break;
		}
	}
	return ;
}

int AVG_PlaySE( int se_no, int loop, int vol, int direct_vol )
{
	int		ret=-1;
	if(!Avg.se) return ret;
	



	if(direct_vol){
		PlayEffectNum( ret, se_no, vol, loop );
	}else{
		PlayEffectNum( ret, se_no, vol*Avg.se_vol/256, loop );
	}







	return ret;
}

void AVG_PlaySE2( int sno, int se_no, int fade, int loop, int vol )
{
	if(SeStruct[sno].flag){
		DebugPrintf( "サウンド構造体が使用中です。AVG_PlaySE2[%d]\n",sno);
		AVG_StopSE( SeStruct[sno].dno );
	}
	ZeroMemory( &SeStruct[sno], sizeof(SeStruct[sno]) );
	SeStruct[sno].flag = 1;
	SeStruct[sno].se_no= se_no;
	if(Avg.se){
		if(fade) SeStruct[sno].dno = AVG_PlaySE( se_no, loop, 0 );
		else	 SeStruct[sno].dno = AVG_PlaySE( se_no, loop, vol );
		if(SeStruct[sno].dno==-1){
			ZeroMemory( &SeStruct[sno], sizeof(SeStruct[sno]) );
			SeStruct[sno].dno=-1;
			return ;
		}
		SeStruct[sno].fcnt = 0;			
		SeStruct[sno].fmax = fade;		
		SeStruct[sno].loop = loop;
		SeStruct[sno].prev_vol = 0;
		SeStruct[sno].next_vol = vol;
	}else{
		SeStruct[sno].dno=-1;
		SeStruct[sno].fcnt = 0;
		SeStruct[sno].fmax = 0;
		SeStruct[sno].loop = loop;
		SeStruct[sno].prev_vol = vol;
		SeStruct[sno].next_vol = vol;
	}
}

void AVG_StopSE( int dno )
{
	if(dno!=-1){
		lpSoundDS->Stop(dno);
		lpSoundDS->ReleaseSoundObj(dno);
	}
}

void AVG_StopSE2( int sno, int fade )
{
	if(SeStruct[sno].flag){
		if(fade>0){
			SeStruct[sno].stop=ON;
			AVG_SetVolumeSE( sno, 0, fade );
		}else{
			if(SeStruct[sno].dno!=-1){
				AVG_StopSE( SeStruct[sno].dno );
			}
			ZeroMemory( &SeStruct[sno], sizeof(SeStruct[sno]) );
		}
	}

}

void AVG_SetVolumeSE( int sno, int vol, int fade )
{
	if(SeStruct[sno].flag){
		vol = LIM(vol,0,255);
		SeStruct[sno].fcnt = 0;			
		SeStruct[sno].fmax = fade;		
		SeStruct[sno].prev_vol = SeStruct[sno].vol;	
		SeStruct[sno].next_vol = vol;	
	}
}
void AVG_SetDirectVoumeSeAll( int se_vol )
{
	int	i;
	for( i=0; i<WAVE_SOUND_NUM ;i++ ){
		if( SeStruct[i].flag ){
			if(SeStruct[i].dno!=-1){
				lpSoundDS->SetVolume( SeStruct[i].dno, LIM(SeStruct[i].vol*se_vol/256,0,255) );
			}
		}
	}
}

int AVG_WaitSe( int sno )
{
	if(!Avg.se){
		return 0;
	}else{
		if(SeStruct[sno].flag){
			return SeStruct[sno].dno!=-1;
		}else{
			return FALSE;
		}
	}
}

int AVG_WaitFadeSe( int sno )
{
	return SeStruct[sno].vol==SeStruct[sno].next_vol;
}

void AVG_SetSEFlag( int flag )
{
	int		i;

	if( Avg.se!=flag ){
		Avg.se = flag;
		
		if(flag){
			for(i=0;i<WAVE_SOUND_NUM;i++){
				if( SeStruct[i].flag && SeStruct[i].loop ){
					SeStruct[i].dno  = AVG_PlaySE( SeStruct[i].se_no, SeStruct[i].loop, SeStruct[i].vol );
				}
			}
		}else{
			for(i=0;i<WAVE_SOUND_NUM;i++){
				if( SeStruct[i].flag ){
					if( SeStruct[i].loop )	AVG_StopSE(  SeStruct[i].dno );
					else					AVG_StopSE2( i, 0 );
				}
			}
		}
	}
}


void AVG_ControlSound( void )
{
	int		i, cnt1, cnt2;
	int		mus_max, vol;

	for(i=0;i<VOICE_MAX;i++) {
		if( VoiceStruct[i].handle!=-1 ){
			if( lpSoundDS->GetStatus( VoiceStruct[i].handle ) == PCM_STOP ){
				AVG_StopVoice(i);
				continue;
			}
			mus_max  = AVG_EffCnt4( VoiceStruct[i].fmax );
			cnt1 = VoiceStruct[i].fcnt;
			cnt2 = mus_max - VoiceStruct[i].fcnt;

			if(mus_max && mus_max>VoiceStruct[i].fcnt){
				VoiceStruct[i].vol = (VoiceStruct[i].prev_vol*cnt2 + VoiceStruct[i].next_vol*cnt1) / mus_max;
				VoiceStruct[i].fcnt++;
			}else{
				VoiceStruct[i].vol = VoiceStruct[i].next_vol;
				if(VoiceStruct[i].vol==0){
					AVG_StopVoice(i);
					continue;
				}
			}
			vol = (float)VoiceStruct[i].vol * Avg.voice_vol / 256;
			lpSoundDS->SetVolume( VoiceStruct[i].handle, LIM(vol,0,255) );
		}
	}

	for(i=0;i<SE_FREE_MAX;i++) {
		if( FreeSeHandle[i] != -1){
			if( lpSoundDS->GetStatus( FreeSeHandle[i] ) == PCM_STOP ){
				AVG_StopSE( FreeSeHandle[i] );
				FreeSeHandle[i] = -1;
			}
		}
	}
	for( i=0; i<WAVE_SOUND_NUM ;i++ ){
		if( SeStruct[i].flag ){
			
			if( lpSoundDS->GetStatus( SeStruct[i].dno ) == PCM_STOP ){
				AVG_StopSE(SeStruct[i].dno);
				ZeroMemory( &SeStruct[i], sizeof(SeStruct[i]) );
				continue;
			}
			
			mus_max  = AVG_EffCnt4( SeStruct[i].fmax );

			cnt1 = SeStruct[i].fcnt;
			cnt2 = mus_max - SeStruct[i].fcnt;
			
			
			if(mus_max && mus_max>SeStruct[i].fcnt){
				SeStruct[i].vol = (SeStruct[i].prev_vol*cnt2 + SeStruct[i].next_vol*cnt1) / mus_max;
				SeStruct[i].fcnt++;
			}else{
				SeStruct[i].vol = SeStruct[i].next_vol;
				if(SeStruct[i].stop){
					AVG_StopSE( SeStruct[i].dno );
					ZeroMemory( &SeStruct[i], sizeof(SeStruct[i]) );
					break;
				}
			}
			if(SeStruct[i].dno!=-1){
				lpSoundDS->SetVolume( SeStruct[i].dno, LIM(SeStruct[i].vol*Avg.se_vol/256,0,255) );
			}
		}
	}

}




void AVG_PlayMusic( int pno, int mus_no, int fade, int loop, int vol, int change )
{
	if(!change){
		if( PlayMusic[pno].mode!=MUSIC_FADEOUT && PlayMusic[pno].mode!=MUSIC_STOP && PlayMusic[pno].mus_no==mus_no ){
			return ;	
		}
	}



	vol = LIM(vol,0,255);

	PlayMusic[pno].mus_no   = mus_no;
	PlayMusic[pno].mode     = MUSIC_FADEIN;
	PlayMusic[pno].fcnt     = 0;
	PlayMusic[pno].loop     = loop;

	if(fade>0){
		PlayMusic[pno].fmax     = fade;
		PlayMusic[pno].vol      = 0;
		PlayMusic[pno].prev_vol = 0;
		PlayMusic[pno].next_vol = vol;
	}else{
		PlayMusic[pno].fmax     = 1;
		PlayMusic[pno].vol      = vol;
		PlayMusic[pno].prev_vol = vol;
		PlayMusic[pno].next_vol = vol;
	}
	if( PlayMusic[pno].handle != -1 ){
		lpSoundDS->Stop( PlayMusic[pno].handle );
		lpSoundDS->ReleaseSoundObj( PlayMusic[pno].handle );
	}
	PlayMusic[pno].handle = -1;



	char	fname[256];

	char	fname2[256];
	char	fname3[256];
	wsprintf( fname, "BGM_%03d.ogg", mus_no );
	wsprintf( fname2, "bgm\\%s", fname );
	wsprintf( fname3, "bgm\\buf\\%s", fname );
	CopyFile(fname2,fname3,TRUE);

	wsprintf( fname, "BGM_%03d_a.ogg", mus_no );
	wsprintf( fname2, "bgm\\%s", fname );
	wsprintf( fname3, "bgm\\buf\\%s", fname );
	CopyFile(fname2,fname3,TRUE);

	wsprintf( fname, "BGM_%03d_b.ogg", mus_no );
	wsprintf( fname2, "bgm\\%s", fname );
	wsprintf( fname3, "bgm\\buf\\%s", fname );
	CopyFile(fname2,fname3,TRUE);

	
	if( lpSoundDS->ChangeMusic( PlayMusic[pno].handle, pack_bgmfile, mus_no, FALSE, !loop, LIM(PlayMusic[pno].vol*Avg.bgm_vol/256,0,255) ) ){
		DebugBox( NULL, "BGMのデータがありません[bgm_%03d.ogg]", mus_no );
	}


}


void AVG_StopMusic( int pno, int fade )
{

	if( PlayMusic[pno].mus_no != -1 ){
		PlayMusic[pno].mode   = MUSIC_FADEOUT;
		PlayMusic[pno].fcnt   = 0;
		PlayMusic[pno].fmax   = (fade>0)? fade : 1;
		PlayMusic[pno].prev_vol = PlayMusic[pno].vol;
		PlayMusic[pno].next_vol = 0;
	}
}
void AVG_PauseMusic( int pno, int flag )
{
	if( PlayMusic[pno].mus_no != -1 ){
		if(flag){
			PlayMusic[pno].mode   = MUSIC_PAUSE;
		}else{
			PlayMusic[pno].mode   = MUSIC_PLAY;
		}
		DebugBox(NULL,"BGM-PAUSE命令は未実装です");

	}
}

void AVG_SetVoumeMusic( int pno, int vol, int fade )
{
	if(PlayMusic[pno].mus_no!=-1){
		vol = LIM(vol,0,255);

		PlayMusic[pno].mode = MUSIC_FADEVOL;
		PlayMusic[pno].fcnt = 0;
		PlayMusic[pno].fmax = (fade>0)? fade : 1;
		PlayMusic[pno].prev_vol = PlayMusic[pno].vol;
		PlayMusic[pno].next_vol = vol;
	}
}

void AVG_SetDirectVoumeMusicAll( int bgm_vol )
{
	int	i;
	for( i=0; i<WAVE_STREAM_NUM ;i++ ){
		if( PlayMusic[i].mode!=MUSIC_STOP )
			if(PlayMusic[i].handle!=-1)
				lpSoundDS->SetVolume( PlayMusic[i].handle, LIM(PlayMusic[i].vol*bgm_vol/256,0,255) );
	}
}

int AVG_GetBGMEno( void )
{
	return PlayMusicNo;
}
void AVG_PlayBGMEx( int mus_no, int fade, int loop, int vol, int change, int goin )
{
	int		bk_no=DB_No;

	if( mus_no<0 ){ AVG_StopBGM( fade ); return; }
	if( !change ){
		if( PlayMusicNo==mus_no ) return ;	
	}
	PlayMusicNo   = mus_no;
	PlayMusicVol  = vol;
	PlayMusicLoop = loop;

	if(Avg.bgm || goin){
		DB_No = (DB_No)?0:1;								
		if(fade==-2){
			AVG_PlayMusic( DB_No, mus_no, 0, loop, vol, 0 );	
			AVG_StopMusic( bk_no, 15 );							
		}else{
			AVG_PlayMusic( DB_No, mus_no, fade, loop, vol, 0 );	

			AVG_StopMusic( bk_no, fade );				
		}
	}
}

int	LoadMus=-1;
void AVG_SetBGMLoad( int mus_no )
{
	LoadMus=mus_no;
}
void AVG_PlayBGMLoad( void )
{
	if(LoadMus==-1)return ;
	AVG_PlayBGM( LoadMus, 0, 1, 255, TRUE );
	LoadMus=-1;
}

void AVG_PlayBGM( int mus_no, int fade, int loop, int vol, int change )
{

	AVG_SetGameFlag( 128+mus_no, ON );
	AVG_PlayBGMEx( mus_no, fade, loop, vol, change, OFF );
}


int AVG_PauseBGM( int flag )
{
	AVG_PauseMusic( DB_No, flag );
	return 1;
}

int AVG_StopBGM( int fade )
{
	PlayMusicNo = -1;
	AVG_StopMusic( DB_No, fade );
	return 1;
}

BOOL AVG_WaitLoopBGM( void )
{
	return PlayMusic[DB_No].mode == MUSIC_STOP;
}

BOOL AVG_WaitBGM( void )
{
	return PlayMusic[DB_No].mode==MUSIC_STOP || PlayMusic[DB_No].mode==MUSIC_PLAY;
}
int AVG_GetBarBGM( int ste )
{
	return PlayMusic[DB_No].bar[ste];
}

void AVG_SetBGMFlag( int flag )
{
	Avg.bgm = flag;
	if(flag){
		AVG_PlayBGM( PlayMusicNo, 60, PlayMusicLoop, PlayMusicVol, 1 );
	}else{
		AVG_StopMusic( DB_No, 60 );
	}
}

void AVG_SetVolumeBGM( int vol, int fade )
{
	AVG_SetVoumeMusic( DB_No, vol, fade );
}


void AVG_ControlMusic( void )
{
	if(lpSoundDS==NULL) return;
	
	if(lpSoundDS && lpSoundDS->lpNextMusic){
		lpSoundDS->NextMusicStart();
	}

	int	i,cnt1,cnt2;
	int	mus_max;
	AVG_ControlSound();

	for( i=0; i<WAVE_STREAM_NUM ;i++ ){
		mus_max  = AVG_EffCnt3( PlayMusic[i].fmax );
		cnt1 = PlayMusic[i].fcnt;
		cnt2 = mus_max - PlayMusic[i].fcnt;

		if( PlayMusic[i].handle != -1)
			lpSoundDS->GetPlayStreamVolume( PlayMusic[i].handle, PlayMusic[i].bar );
		switch(PlayMusic[i].mode){
			default:
			case MUSIC_PAUSE:
			case MUSIC_STOP:
				break;
			case MUSIC_PLAY:
				break;
			case MUSIC_FADEOUT:
				if( mus_max<=PlayMusic[i].fcnt ){
					PlayMusic[i].mus_no = -1;
					PlayMusic[i].loop = 0;
					PlayMusic[i].mode = MUSIC_STOP;

					if( PlayMusic[i].handle != -1){
						lpSoundDS->Stop( PlayMusic[i].handle );
						lpSoundDS->ReleaseSoundObj( PlayMusic[i].handle );
					}
					PlayMusic[i].handle=-1;
					break;
				}
				
			case MUSIC_FADEIN:
			case MUSIC_FADEVOL:
				if(mus_max && mus_max>PlayMusic[i].fcnt){
					PlayMusic[i].vol = (PlayMusic[i].prev_vol*cnt2 + PlayMusic[i].next_vol*cnt1) / mus_max;
					PlayMusic[i].fcnt++;
				}else{
					PlayMusic[i].vol = PlayMusic[i].next_vol;
					PlayMusic[i].mode = MUSIC_PLAY;
				}
				break;
		}

		if( PlayMusic[i].mode != MUSIC_STOP ){
			PlayMusic[i].bar[0]=PlayMusic[i].bar[0]*PlayMusic[i].vol/256*Avg.bgm_vol/256;
			PlayMusic[i].bar[1]=PlayMusic[i].bar[1]*PlayMusic[i].vol/256*Avg.bgm_vol/256;
			if( PlayMusic[i].handle != -1){
				lpSoundDS->SetVolume( PlayMusic[i].handle, LIM(PlayMusic[i].vol*Avg.bgm_vol/256,0,255) );
			}
		}
	}
}




typedef struct{
	char	flag;
	char	type;
	float	x,y;
	float	ax,ay;
	short	cnt;
}WEATHER_OBJ;

typedef struct{
	int		flag;
	int		wno;		
	int		layer;
	int		cnt;
	WEATHER_OBJ	obj[200];
	int		reset;
	
	float	wind;		
	float	speed;		
	int		amount;		
	
	int		twind;		
	int		tspeed;		
	int		tamount;	
}WEATHER_STRUCT;
WEATHER_STRUCT	Weather;

#define WEATH_RAIN		0
#define WEATH_SAKURA	1
#define WEATH_HINOKO	2
#define WEATH_YUKI		3

void AVG_SetWeather( int wno, int wind, int speed, int amount, int layer )
{
	int	i;
	AVG_ResetWeather(1);

	Weather.flag = 1;
	Weather.wno  = wno;
	Weather.layer = layer;
	switch(wno){
		case WEATH_RAIN:	
			break;
		case WEATH_HINOKO:
		case WEATH_YUKI:
		case WEATH_SAKURA:	
			GD_LoadTexture( BMP_WEATHER+0, "weather.tga", ALPHA_4444 );

			Weather.wind  = Weather.twind  = wind;
			Weather.speed = Weather.tspeed = speed;
			Weather.amount  = 0;
			Weather.tamount = amount;
			break;
	}
}

void AVG_ResetWeather( int direct )
{
	int	i;
	if(direct){
		for(i=0;i<200;i++)
			GD_ResetGraph( GRP_WEATHER+i );
		GD_ReleaseTexture( BMP_WEATHER+0 );
		ZeroMemory( &Weather, sizeof(Weather) );
	}else{

		Weather.tamount = 0;
		Weather.reset=ON;
	}
}

void AVG_SetWeatherWind( int wind )
{
	Weather.twind = wind;
}

void AVG_SetWeatherSpeed( int speed )
{
	Weather.tspeed = speed;
}

void AVG_SetWeatherAmount( int amount )
{
	Weather.tamount = amount;
}
void AVG_SetWeatherLayer( int layer )
{
	Weather.layer = layer;
}


BOOL AVG_WaitWeather( void )
{
	int	i;
	int	wait=0;
	for(i=0;i<Weather.amount;i++){
		wait |= Weather.obj[i].flag;
	}
	if(wait==0 && Weather.reset){
		AVG_ResetWeather(1);

		return TRUE;
	}else{
		return FALSE;
	}
}

void AVG_ControlWeather( void )
{
	int			i;



	if( Weather.flag ){
		if(Weather.wind  < Weather.twind )	Weather.wind  = min( Weather.twind,   Weather.wind +1 );
		if(Weather.speed < Weather.tspeed )	Weather.speed = min( Weather.tspeed,  Weather.speed +1 );
		
		if(Weather.wind  > Weather.twind )	Weather.wind  = max( Weather.twind,   Weather.wind -1 );
		if(Weather.speed > Weather.tspeed )	Weather.speed = max( Weather.tspeed,  Weather.speed -1 );

		Weather.cnt++;
			
		switch( Weather.wno ){
			case WEATH_RAIN:
				break;
			case WEATH_YUKI:
				if( Weather.amount<Weather.tamount && Weather.cnt%5==0 ){
					i = Weather.amount;
					Weather.obj[i].flag = 1;
					Weather.obj[i].type = rand()%4;
					Weather.obj[i].x  = (float)(rand()%800);
					Weather.obj[i].y  = -(float)(rand()%100);

					Weather.obj[i].ax = (float)(rand()%((4-Weather.obj[i].type)*100)/100.0+1)/2;
					Weather.obj[i].ay = (float)(rand()%((4-Weather.obj[i].type)*100)/100.0+1)/2;

					Weather.obj[i].cnt = rand()%256;
					Weather.amount++;
				}

				for(i=0;i<200;i++){
					GD_ResetGraph( GRP_WEATHER+i );
				}
				for(i=0;i<Weather.amount;i++){
					WEATHER_OBJ	*wos = &Weather.obj[i];

					if(!wos->flag) continue;
					wos->x += (SIN(wos->cnt)*wos->ax/4096) + wos->ax*Weather.wind/10;
					wos->y += wos->ay*Weather.speed/10;
					if( wos->y>600 ){
						if(Weather.tamount<=i){

							Weather.amount--;
							wos->flag = 0;
							continue;
						}else{
							wos->y -= 600+32;
						}
					}
					if( wos->x>=800)	{
						if(Weather.tamount<=i){

							Weather.amount--;
							wos->flag = 0;
							continue;
						}else{
							wos->x -= 800+32;
						}
					}
					if( wos->x<-32){

						if(Weather.tamount<=i){
							Weather.amount--;
							wos->flag = 0;
							continue;
						}else{
							wos->x += 800+32;
						}
					}
					switch(Weather.layer){
					case 0:	GD_SetGraph( GRP_WEATHER+i, BMP_WEATHER+0, LAY_MAP_FRONT, TRUE );	break;
					case 1:	GD_SetGraph( GRP_WEATHER+i, BMP_WEATHER+0, LAY_BACK_FRONT, TRUE );	break;
					case 2:	GD_SetGraph( GRP_WEATHER+i, BMP_WEATHER+0, LAY_CHAR_FRONT, TRUE );	break;
					}
					GD_SetGraphGroup( GRP_WEATHER+i, 0x02 );
					GD_SetGraphScaleParam( GRP_WEATHER+i, SCALE_HIGHT );
					GD_SetGraphBlendParam( GRP_WEATHER+i, BLEND_ADD );

					GD_SetGraphDstRect( GRP_WEATHER+i, (int)wos->x, (int)wos->y, (4-Weather.obj[i].type+1)*8, (4-Weather.obj[i].type+1)*8 );
					GD_SetGraphSrcRect( GRP_WEATHER+i, 0, 32, 32,32 );
					wos->cnt++;
				}
				break;
			case WEATH_SAKURA:	
			case WEATH_HINOKO:
				if( Weather.amount<Weather.tamount && Weather.cnt%5==0 ){
					i = Weather.amount;
					Weather.obj[i].flag = 1;
					Weather.obj[i].type = rand()%6;
					Weather.obj[i].x  = (float)(rand()%800);
					Weather.obj[i].y  = (float)(600+rand()%100);

					Weather.obj[i].ax = (float)(rand()%((6-Weather.obj[i].type)*100)/100.0+1)/2;
					Weather.obj[i].ay = (float)(rand()%((6-Weather.obj[i].type)*100)/100.0+1)/2;
					Weather.obj[i].cnt = rand()%256;
					Weather.amount++;
				}

				for(i=0;i<200;i++){
					GD_ResetGraph( GRP_WEATHER+i );
				}
				for(i=0;i<Weather.amount;i++){
					WEATHER_OBJ	*wos = &Weather.obj[i];



					if(!wos->flag) continue;
					wos->x += (SIN(wos->cnt)*wos->ax/4096) + wos->ax*Weather.wind/10;
					wos->y -= wos->ay*Weather.speed/10;


					if( wos->y<-32 ){

						if(Weather.tamount<=i){
							Weather.amount--;
							wos->flag = 0;
							continue;
						}else{
							wos->y += 600+32;
						}
					}
					if( wos->x>=800){

						if(Weather.tamount<=i){
							Weather.amount--;
							wos->flag = 0;
							continue;
						}else{
							wos->x -= 800+32;
						}
					}
					if( wos->x<-32){

						if(Weather.tamount<=i){
							Weather.amount--;
							wos->flag = 0;
							continue;
						}else{
							wos->x += 800+32;
						}
					}

					switch(Weather.layer){
					case 0:	GD_SetGraph( GRP_WEATHER+i, BMP_WEATHER+0, LAY_MAP_FRONT, TRUE );	break;
					case 1:	GD_SetGraph( GRP_WEATHER+i, BMP_WEATHER+0, LAY_BACK_FRONT, TRUE );	break;
					case 2:	GD_SetGraph( GRP_WEATHER+i, BMP_WEATHER+0, LAY_CHAR_FRONT, TRUE );	break;
					}
					GD_SetGraphGroup( GRP_WEATHER+i, 0x02 );
					GD_SetGraphScaleParam( GRP_WEATHER+i, SCALE_HIGHT );
					GD_SetGraphBlendParam( GRP_WEATHER+i, BLEND_ADD );

					if(wos->type/3){
						GD_SetGraphDstRect( GRP_WEATHER+i, (int)wos->x, (int)wos->y, 8, 8 );
					}else{
						GD_SetGraphDstRect( GRP_WEATHER+i, (int)wos->x, (int)wos->y, 16, 16 );
					}
					int	sakura;
					switch(wos->type%3){
					case 0:
						sakura = STD_LimitLoop( wos->cnt, 23 )*64 / 23;
						GD_SetGraphColor( GRP_WEATHER+i, 255, 128+sakura, 64+sakura, 64+sakura );
						GD_SetGraphSrcRect( GRP_WEATHER+i, 32, 32, 32,32 );
						break;
					case 1:
						sakura = STD_LimitLoop( wos->cnt, 20 )*64 / 20;
						GD_SetGraphColor( GRP_WEATHER+i, 255, 128+sakura, 64+sakura, 64+sakura );
						GD_SetGraphSrcRect( GRP_WEATHER+i, 32, 0, 32,32 );
						break;
					case 2:
						sakura = STD_LimitLoop( wos->cnt, 17 )*64 / 17;
						GD_SetGraphColor( GRP_WEATHER+i, 255, 128+sakura, 64+sakura, 64+sakura );
						GD_SetGraphSrcRect( GRP_WEATHER+i, 0, 0, 32,32 );
						break;
					}
					wos->cnt++;
				}
				break;
		}
	}
}
