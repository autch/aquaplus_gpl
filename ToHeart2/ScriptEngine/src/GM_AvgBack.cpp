#include <mbstring.h>

#include "mm_std.h"
#include "main.h"


#include "bmp.h"
#include "draw.h"
#include "escript.h"
#include "disp.h"
#include "text.h"
#include "keybord.h"
#include "mouse.h"


#include "comp_bmppac.h"

#include "main.h"
#include "GM_avg.h"
#include "GM_avgMsg.h"
#include "GM_avgBack.h"
#include "GM_Save.h"
#include "GM_title.h"

#include "escript.h"


BACK_STRUCT		BackStruct;
char			*tone_fname[] = { 
	NULL, 				
	"evening.amp", 		
	"night.amp", 		
	"indoor.AMP",		
};
char	tone_fname2[32];
char	spani_fname[32];
char	*eff_fname;
static int DemoFede=OFF;


void AVG_SetWeatherMode( int weather )
{
	BackStruct.wether = weather;
}
void AVG_SetTimeMode( int tone_no, int eff_no )
{
	BackStruct.tone_no = tone_no + eff_no*TONE_MAX;
	BackStruct.tone_back = -1;
	BackStruct.tone_char = -1;
}
void AVG_SetTimeBack( int tone_no, int eff_no )
{
	BackStruct.tone_back = tone_no + eff_no*TONE_MAX;
}
void AVG_SetTimeChar( int tone_no, int eff_no )
{
	BackStruct.tone_char = tone_no + eff_no*TONE_MAX;
}


void AVG_GetBack( void )
{
	AVG_ResetBack(1);
	MainWindow.draw_flag = 1;
	DSP_GetDispBmp(         BMP_BACK, MainWindow.draw_mode2 );	
	DSP_SetGraph( GRP_BACK, BMP_BACK, LAY_BACK, ON, CHK_NO );

	BackStruct.flag    = 1;
	BackStruct.bno     = -1;
	BackStruct.cg_flag = 0;
	BackStruct.zoom    = 0;
	BackStruct.sc_type = 0;
	BackStruct.sx = 0;
	BackStruct.sy = 0;
}

void AVG_SetBack_Delete( void )
{
	AVG_ResetBack(0);
}

void AVG_SetNoise( int disp, int rate, int fade )
{
	if(disp){
		if(fade){
			BackStruct.ns_flag = 1;
			BackStruct.ns_rate = rate;
			BackStruct.ns_fade = fade;
			BackStruct.ns_count = 0;
		}else{
			BackStruct.ns_flag = 2;
			BackStruct.ns_rate = rate;
			BackStruct.ns_fade = fade;
			BackStruct.ns_count = 0;
			DSP_SetGraphParam( GRP_BACK, DRW_FLT(BackStruct.ns_rate) );
			DSP_SetGraphParam( GRP_BACK+1, DRW_FLT(BackStruct.ns_rate) );
		}
	}else{
		if( !BackStruct.ns_flag ) return ;
		if(fade){
			BackStruct.ns_flag = 3;
			BackStruct.ns_fade = fade;
			BackStruct.ns_count = 0;
		}else{
			BackStruct.ns_flag = 0;
			BackStruct.ns_rate = 0;
			BackStruct.ns_fade = 0;
			BackStruct.ns_count = 0;
			DSP_SetGraphParam( GRP_BACK, DRW_NML );
			DSP_SetGraphParam( GRP_BACK+1, DRW_NML );
		}
	}
}
int AVG_WaitNoise( void )
{
	return (BackStruct.ns_flag==2 || BackStruct.ns_flag==0);
}

void AVG_ControlNoise( void )
{
	int		cnt;
	switch(BackStruct.ns_flag){
		case 0:
			break;
		case 1:
			BackStruct.ns_count++;
			if(BackStruct.ns_fade){
				cnt = BackStruct.ns_count*BackStruct.ns_rate/BackStruct.ns_fade;
				DSP_SetGraphParam( GRP_BACK, DRW_FLT(cnt) );
				DSP_SetGraphParam( GRP_BACK+1, DRW_FLT(cnt) );
			}
			if(BackStruct.ns_count>=BackStruct.ns_fade){
				BackStruct.ns_count=0;
				BackStruct.ns_flag=2;
				DSP_SetGraphParam( GRP_BACK, DRW_FLT(BackStruct.ns_rate) );
				DSP_SetGraphParam( GRP_BACK+1, DRW_FLT(BackStruct.ns_rate) );
			}
			break;
		case 2:
			DSP_SetGraphParam( GRP_BACK, DRW_FLT(BackStruct.ns_rate) );
			DSP_SetGraphParam( GRP_BACK+1, DRW_FLT(BackStruct.ns_rate) );
			break;
		case 3:
			BackStruct.ns_count++;
			if(BackStruct.ns_fade){
				cnt = BackStruct.ns_rate-BackStruct.ns_count*BackStruct.ns_rate/BackStruct.ns_fade;
				DSP_SetGraphParam( GRP_BACK, DRW_FLT(cnt) );
				DSP_SetGraphParam( GRP_BACK+1, DRW_FLT(cnt) );
			}
			if(BackStruct.ns_count>=BackStruct.ns_fade){
				BackStruct.ns_flag = 0;
				BackStruct.ns_rate = 0;
				BackStruct.ns_fade = 0;
				BackStruct.ns_count= 0;
				DSP_SetGraphParam( GRP_BACK, DRW_NML );
				DSP_SetGraphParam( GRP_BACK+1, DRW_NML );
			}
			break;
	}
}

void AVG_ResetBackHalfTone( int bak_no, int chg_type )
{

		if(BackStruct.flag && bak_no==BackStruct.bno && chg_type==BAK_DIRECT) return ;

		AVG_ResetHalfTone();
		AVG_SetNovelMessageDisp(OFF);
		MainWindow.draw_flag = 1;

}

void AVG_OpenBack( void )
{
	DSP_SetGraphDisp( GRP_BACK+1, ON );
}
void AVG_CloseBack( void )
{
	int	i;
	DSP_SetGraphDisp( GRP_BACK+1, OFF );
	for(i=0;i<200;i++){
		DSP_SetGraphDisp( GRP_WEATHER+i, OFF );
	}
	
}

int AVG_SakuraSakuTH2( int scene )
{
	int ret = scene % 10000;











	
	for(;;) {
		
		
		if( scene >= 10000) break;
		
		
		
		if(      ( scene >=  1 && scene <=  4) || scene ==  78) scene =  1;
		else if( ( scene >=  5 && scene <=  8) || scene ==  79) scene =  5;
		else if( ( scene >= 34 && scene <= 37) || scene ==  80) scene = 34;
		else if( ( scene >= 48 && scene <= 51) || scene ==  81) scene = 48;
		else break;
		
		
		
		int m, d;
		d = ESC_GetFlag( _DAY  );
		m = ESC_GetFlag( _MONTH );
		
		
		
		int type = 3;
		if( m == 3) {
			
			if( d >= 1 && d<=15)		type = 4;
			else if( d >= 16 && d<= 28)	type = 2;
			else 						type = 0;
		} else if( m == 4) {
			
			if( d >= 1 && d <= 15)		type = 0;
			else if( d>= 16 && d<= 27)	type = 1;
			else						type = 3;
		} else {
			
			type = 3;
		}
		
		
		
		if( type != 4) {
			ret = scene + type;
		} else {
			if( scene == 1)			ret = 78;
			else if( scene ==  5)	ret = 79;
			else if( scene == 34)	ret = 80;
			else if( scene == 48)	ret = 81;
		}		
		break;
	}	

	return ret;
}



void AVG_SetBack(int bak_no, int x, int y, int chg_type, int char_disp, int cg_flag, int fd_max, int vague )
{
	char	fname[256];
	int		bmp_bpp = BPP(MainWindow.draw_mode2);
	int		back_max = AVG_EffCnt(fd_max);
	int		sp = Avg.level;
	int		i, tone;
	char	*pack=NULL;


	if(BackStruct.flag && bak_no==BackStruct.bno && chg_type==BAK_DIRECT) return ;






	AVG_ResetBack(0);

	
	if(BackStruct.tone_back==-1)tone = BackStruct.tone_no;		
	else						tone = BackStruct.tone_back;	
	BackStruct.tone_no2 = BackStruct.tone_no;

	
	if(bak_no==-1) return ;
	if(bak_no==-2) bak_no = BackStruct.bno;
	if(bak_no>=0){
		if(cg_flag==1){
			AVG_SetCgFlag( 0, bak_no );
			wsprintf( fname, "v%06d.tga", bak_no );
		}else if(cg_flag==2){
			AVG_SetCgFlag( 1, bak_no );
			wsprintf( fname, "h%06d.tga", bak_no );

		}else{
			bak_no = AVG_SakuraSakuTH2( bak_no/10 )*10 + bak_no%10;
			pack = "bak";


			wsprintf( fname, "b%03d%1d%1d%1d.bmp", bak_no/10, tone%TONE_MAX, BackStruct.wether, bak_no%10 );
			wsprintf( tone_fname2, "b%03d%1d%1d%1d.amp", bak_no/10, tone%TONE_MAX, BackStruct.wether, bak_no%10 );
		}
	}
	switch(bak_no/10){
		case  1:	case  5:
		case  34:	case  48:
			AVG_SetSakura( 32, 0, 0, 0 );
			break;
		default:
			if( !AVG_GetWeatherNoReset() ){
				AVG_StopSakura(  );
			}
			break;
	}

	int		ret, viv;
	switch( tone/TONE_MAX ){
		default:	eff_fname = NULL;			viv = 256;	break;	
		case 1:	eff_fname = "sepia.amp";		viv =   0;	break;
		case 2:	eff_fname = "nega.amp";			viv = 256;	break;	
		case 3:	eff_fname = NULL;				viv =   0;	break;
		case 4:	eff_fname = "blue.amp";			viv = 128;	break;
		case 5:	eff_fname = "red.amp";			viv = 128;	break;
		case 6:	eff_fname = "green.amp";		viv = 128;	break;
		case 7:	eff_fname = "blue2.amp";		viv = 128;	break;
		case 8:	eff_fname = "brown.amp";		viv = 128;	break;
		case 9:	eff_fname = "sepia_half.AMP";	viv = 128;	break;

		case 10:	eff_fname = "black.AMP";	viv = 256;	break;
		case 11:	eff_fname = "yoritomo.AMP";	viv = 256;	break;
	}
	ret = DSP_LoadBmp( BMP_BACK, bmp_bpp, fname, eff_fname, viv, pack );
	STD_FileAttrChange( spani_fname, fname, "ani" );


	if( PAC_CheckFile( "grp", spani_fname ) ){
		BackStruct.sp_flag = 1;
		DSP_LoadSprite( BMP_SPBACK, spani_fname, CHK_NO, eff_fname, viv );
	}else{
		BackStruct.sp_flag = 0;
	}

	if(ret==0) return ;


	BackStruct.flag    = 1;
	BackStruct.bno     = bak_no;
	BackStruct.cg_flag = cg_flag;
	BackStruct.zoom    = 0;

	BackStruct.sc_flag = 0;
	BackStruct.sc_type = 0;
	BackStruct.sw = BackStruct.sx = BackStruct.sx2 = BackStruct.sx3 = BackStruct.sx4 = 0;
	BackStruct.sw = BackStruct.sy = BackStruct.sy2 = BackStruct.sy3 = BackStruct.sy4 = 0;
	BackStruct.sc_cnt = 0;		
	BackStruct.sc_max = 0;		

	DSP_CopyBmp( BMP_BACK2, BMP_BACK );

	DSP_SetGraph( GRP_BACK, BMP_BACK, LAY_BACK, ON, CHK_NO );



	AVG_SetBackPos( x, y );


	if(char_disp){	
		AVG_SetBackReleaseChar();
		AVG_SetBackChar( x, y, char_disp );
	}else{			
		for(i=0;i<MAX_CHAR;i++)
			AVG_ReleaseChar( i );
		BackStruct.cscop = 0;
	}

	if( chg_type!=BAK_DIRECT ){
		if(back_max) {
			switch(chg_type){
			case BAK_FADE:
				if( BackStruct.r || BackStruct.g || BackStruct.b ){
					DSP_GetDispBmp(           BMP_BACK+1, MainWindow.draw_mode2 );	

					DSP_SetGraph( GRP_BACK+1, BMP_BACK+1, LAY_BACK, ON, CHK_NO );
					DSP_SetGraphDisp( GRP_BACK, OFF );
				}
				break;
			case BAK_CFADE:
				if(sp){
					DSP_GetDispBmp(           BMP_BACK+1, MainWindow.draw_mode2 );	

					DSP_SetGraph( GRP_BACK+1, BMP_BACK+1, LAY_BACK, ON, CHK_NO );
					DSP_SetGraphLayer( GRP_BACK, LAY_BACK+1 );
				}
				break;
			case BAK_CFADE_UP:	case BAK_CFADE_DO:
			case BAK_CFADE_RI:	case BAK_CFADE_LE:
			case BAK_CFADE_CE:	case BAK_CFADE_OU:
				if(sp){
					DSP_GetDispBmp(           BMP_BACK+1, MainWindow.draw_mode2 );	
					DSP_SetGraph( GRP_BACK+1, BMP_BACK+1, LAY_BACK, ON, CHK_NO );
					DSP_SetGraphLayer( GRP_BACK, LAY_BACK+1 );
				}
				break;
			case BAK_DIA1:	case BAK_DIA2:	case BAK_DIA3:
				break;
			case BAK_CFZOOM1:
			case BAK_CFZOOM4:
				break;
			case BAK_CFZOOM2:
				DSP_GetDispBmp(           BMP_BACK+1, MainWindow.draw_mode2 );	
				DSP_SetGraph( GRP_BACK+1, BMP_BACK+1, LAY_BACK, ON, CHK_NO );
				DSP_SetGraphLayer( GRP_BACK, LAY_BACK+1 );
				break;
			case BAK_CFZOOM3:
				DSP_GetDispBmp(           BMP_BACK+1, MainWindow.draw_mode2 );	
				DSP_SetGraph( GRP_BACK+1, BMP_BACK+1, LAY_BACK+1, ON, CHK_NO );
				break;
			case BAK_KAMI:
				DSP_GetDispBmp(           BMP_BACK+1, MainWindow.draw_mode2 );	
				DSP_SetGraph( GRP_BACK+1, BMP_BACK+1, LAY_BACK, ON, CHK_NO );
				DSP_SetGraphLayer( GRP_BACK, LAY_BACK+1 );
				break;
			case BAK_SLIDE_UP:	case BAK_SLIDE_DO:
			case BAK_SLIDE_RI:	case BAK_SLIDE_LE:
				DSP_GetDispBmp(           BMP_BACK+1, MainWindow.draw_mode2 );	
				DSP_SetGraph( GRP_BACK+1, BMP_BACK+1, LAY_BACK, ON, CHK_NO );
				DSP_SetGraphLayer( GRP_BACK, LAY_BACK+1 );
			case BAK_LASTERIN:
				DSP_GetDispBmp(           BMP_BACK+1, MainWindow.draw_mode2 );	
				DSP_SetGraph( GRP_BACK+1, BMP_BACK+1, LAY_BACK, ON, CHK_NO );
				DSP_SetGraphLayer( GRP_BACK, LAY_BACK+1 );
				break;
			case BAK_NOISE:
				DSP_GetDispBmp(           BMP_BACK+1, MainWindow.draw_mode2 );	
				DSP_SetGraph( GRP_BACK+1, BMP_BACK+1, LAY_BACK, ON, CHK_NO );
				DSP_SetGraphLayer( GRP_BACK, LAY_BACK+1 );
				break;
			case BAK_PIPLE:
				DSP_GetDispBmp(           BMP_BACK+1, MainWindow.draw_mode2 );	
				DSP_SetGraph( GRP_BACK+1, BMP_BACK+1, LAY_BACK, ON, CHK_NO );
				DSP_SetGraphLayer( GRP_BACK, LAY_BACK+1 );
				break;
			case BAK_TV:
				DSP_GetDispBmp(           BMP_BACK+1, MainWindow.draw_mode2 );	
				DSP_SetGraph( GRP_BACK+1, BMP_BACK+1, LAY_BACK+1, ON, CHK_NO );
				DSP_LoadBmp( BMP_BACK+2, BMP_256P, "e2000.bmp" );
				DSP_SetGraph( GRP_BACK+2, BMP_BACK+2, LAY_BACK+2, ON, CHK_NO );
				break;
			case BAK_ROLL:
				DSP_GetDispBmp(           BMP_BACK+1, MainWindow.draw_mode2 );	
				DSP_SetGraph( GRP_BACK+1, BMP_BACK+1, LAY_BACK, ON, CHK_NO );
				DSP_SetGraphLayer( GRP_BACK, LAY_BACK+1 );
				break;
			default:
				if( chg_type>=BAK_PATTERN ){
					wsprintf( fname, "f0%03d.bmp", chg_type&0x7f );
					int	rparam=0;
					if(chg_type&BAK_PTF_REV_W)rparam |= REV_W;
					if(chg_type&BAK_PTF_REV_H)rparam |= REV_H;

					if( chg_type&BAK_PTF_REV ){
						if( chg_type&BAK_PTF_ACCL1 ){

							DSP_LoadBmp2( BMP_BACK+2, BMP_256P, fname, "rev_accel1.AMP", 256,128,128,128,DRW_NML, rparam );
						}else if( chg_type&BAK_PTF_ACCL2 ){
							DSP_LoadBmp2( BMP_BACK+2, BMP_256P, fname, "rev_accel2.AMP", 256,128,128,128,DRW_NML, rparam );
						}else{
							DSP_LoadBmp2( BMP_BACK+2, BMP_256P, fname,  "rev.amp" );
						}
					}else{
						if( chg_type&BAK_PTF_ACCL1 ){
							DSP_LoadBmp2( BMP_BACK+2, BMP_256P, fname, "accel1.AMP", 256,128,128,128,DRW_NML, rparam );
						}else if( chg_type&BAK_PTF_ACCL2 ){
							DSP_LoadBmp2( BMP_BACK+2, BMP_256P, fname, "accel2.AMP", 256,128,128,128,DRW_NML, rparam );
						}else{
							DSP_LoadBmp2( BMP_BACK+2, BMP_256P, fname, NULL, 256,128,128,128,DRW_NML, rparam );
						}
					}
					DSP_GetDispBmp(           BMP_BACK+1, MainWindow.draw_mode2 );	
					DSP_SetGraph( GRP_BACK+1, BMP_BACK+1, LAY_BACK, ON, CHK_NO );
					DSP_SetGraphLayer( GRP_BACK, LAY_BACK+1 );
				}
				break;
			}
		}else{
			if( chg_type>=BAK_PATTERN ){
				wsprintf( fname, "f0%03d.bmp", chg_type&0x7f );
				int	rparam=0;
				if(chg_type&BAK_PTF_REV_W)rparam |= REV_W;
				if(chg_type&BAK_PTF_REV_H)rparam |= REV_H;

				if( chg_type&BAK_PTF_REV ){
					if( chg_type&BAK_PTF_ACCL1 ){

						DSP_LoadBmp2( BMP_BACK+2, BMP_256P, fname, "rev_accel1.AMP", 256,128,128,128,DRW_NML, rparam );
					}else if( chg_type&BAK_PTF_ACCL2 ){
						DSP_LoadBmp2( BMP_BACK+2, BMP_256P, fname, "rev_accel2.AMP", 256,128,128,128,DRW_NML, rparam );
					}else{
						DSP_LoadBmp2( BMP_BACK+2, BMP_256P, fname,  "rev.amp" );
					}
				}else{
					if( chg_type&BAK_PTF_ACCL1 ){
						DSP_LoadBmp2( BMP_BACK+2, BMP_256P, fname, "accel1.AMP", 256,128,128,128,DRW_NML, rparam );
					}else if( chg_type&BAK_PTF_ACCL2 ){
						DSP_LoadBmp2( BMP_BACK+2, BMP_256P, fname, "accel2.AMP", 256,128,128,128,DRW_NML, rparam );
					}else{
						DSP_LoadBmp2( BMP_BACK+2, BMP_256P, fname, NULL, 256,128,128,128,DRW_NML, rparam );
					}
				}
			}
		}
		
		BackStruct.fd_flag = 1;
		BackStruct.fd_type = chg_type;
		BackStruct.fd_vague = vague;
		BackStruct.fd_cnt  = 0;
		BackStruct.fd_max  = fd_max;
	}else{
		if( BackStruct.sp_flag ){
			BackStruct.sp_flag = 2;
			DSP_SetSprite( GRP_SPBACK, BMP_SPBACK, 0, LAY_SPBACK, ON, 0, 0, 0 );
			DSP_SetGraphMove( GRP_SPBACK, DISP_X/2, DISP_Y/2 );

			DSP_CopySprite( BMP_SPBACKHALF, BMP_SPBACK, NULL, 256, BackStruct.r*Avg.half_tone/128, BackStruct.g*Avg.half_tone/128, BackStruct.b*Avg.half_tone/128 );
			DSP_SetSprite( GRP_SPBACKHALF, BMP_SPBACKHALF, 0, LAY_SPBACK+1, OFF, 0, 0, 0 );
			DSP_SetGraphMove( GRP_SPBACKHALF, DISP_X/2, DISP_Y/2 );
		}
	}
	DSP_SetGraphBright( GRP_BACK, BackStruct.r, BackStruct.g, BackStruct.b );
	if(BackStruct.cscop){
		BackStruct.cs_clip.x = 0;
		BackStruct.cs_clip.y = 66+7;
		BackStruct.cs_clip.w = DISP_X;
		BackStruct.cs_clip.h = DISP_Y-132-14;




	}else{
		BackStruct.cs_clip.x = 0;
		BackStruct.cs_clip.y = 0;
		BackStruct.cs_clip.w = DISP_X;
		BackStruct.cs_clip.h = DISP_Y;
	}
	DSP_SetGraphClip( GRP_BACK,   0, 0, DISP_X, DISP_Y );
	DSP_SetGraphClip( GRP_BACK+1, 0, 0, DISP_X, DISP_Y );
	DSP_SetGraphClip( GRP_BACK+2, 0, 0, DISP_X, DISP_Y );
	DSP_SetGraphClip( GRP_BACK+3, 0, 0, DISP_X, DISP_Y );
	if(Avg.msg_page){

	}
}

void AVG_ControlBackChange(void)
{
	int	rate,i;
	int	back_max = AVG_EffCnt(BackStruct.fd_max);
	int	sp = Avg.level;
	int	x,y;

	if( BackStruct.fd_type==BAK_FADE ){
		if( BackStruct.r || BackStruct.g || BackStruct.b ){
			back_max *= 2;
		}
	}
	if( BackStruct.fd_type==BAK_LASTERIN && BackStruct.fd_max==-2 && sp ){
		back_max *= 2;
	}

	if(BackStruct.fd_flag){
		BackStruct.fd_cnt++;




		if( BackStruct.fd_cnt>=back_max ){
			BackStruct.fd_flag = 0;
			MainWindow.draw_flag = 1;

			for( i=1; i<4 ;i++ )
				DSP_ResetGraph( GRP_BACK+i );
			DSP_ReleaseBmp( BMP_BACK+1 );

			DSP_ResetGraphBSet(GRP_BACK);
			DSP_SetGraph( GRP_BACK, BMP_BACK, LAY_BACK, ON, CHK_NO );
			AVG_SetBackPos( BackStruct.x, BackStruct.y );

			if(BackStruct.fd_type==BAK_FADE) AVG_SetBackFadeDirect(128,128,128);
			DSP_SetGraphBright( GRP_BACK, BackStruct.r,BackStruct.g,BackStruct.b );

			if( BackStruct.sp_flag ){
				BackStruct.sp_flag = 2;
				DSP_SetSprite( GRP_SPBACK, BMP_SPBACK, 0, LAY_SPBACK, ON, 0, 0, 0 );
				DSP_SetGraphMove( GRP_SPBACK, DISP_X/2, DISP_Y/2 );

				DSP_CopySprite( BMP_SPBACKHALF, BMP_SPBACK, NULL, 256, BackStruct.r*Avg.half_tone/128, BackStruct.g*Avg.half_tone/128, BackStruct.b*Avg.half_tone/128 );
				DSP_SetSprite( GRP_SPBACKHALF, BMP_SPBACKHALF, 0, LAY_SPBACK+1, OFF, 0, 0, 0 );
				DSP_SetGraphMove( GRP_SPBACKHALF, DISP_X/2, DISP_Y/2 );
			}
		}else{
			rate = 256*BackStruct.fd_cnt/back_max;
			switch(BackStruct.fd_type){
			case BAK_FADE:
				if( BackStruct.r || BackStruct.g || BackStruct.b ){
					if(rate<128){
						DSP_SetGraphBright( GRP_BACK+1, 128-rate, 128-rate, 128-rate );
					}else{
						DSP_SetGraphDisp( GRP_BACK+1, OFF );
						DSP_SetGraphDisp( GRP_BACK,   ON );
						DSP_SetGraphBright( GRP_BACK, rate-128, rate-128, rate-128 );
					}
				}else{
					DSP_SetGraphBright( GRP_BACK, rate/2, rate/2, rate/2 );
				}
				break;
			case BAK_CFADE:
				if(sp)	DSP_SetGraphParam( GRP_BACK, DRW_BLD(rate) );
				else	DSP_SetGraphParam( GRP_BACK, DRW_AMI(rate) );
				break;
			case BAK_CFADE_UP:
				if(sp)	DSP_SetGraphParam( GRP_BACK, DRW_LCF(UP,rate) );
				else	DSP_SetGraphParam( GRP_BACK, DRW_LPP(UP,rate) );
				break;
			case BAK_CFADE_DO:
				if(sp)	DSP_SetGraphParam( GRP_BACK, DRW_LCF(DO,rate) );
				else	DSP_SetGraphParam( GRP_BACK, DRW_LPP(DO,rate) );
				break;
			case BAK_CFADE_RI:
				if(sp)	DSP_SetGraphParam( GRP_BACK, DRW_LCF(RI,rate) );
				else	DSP_SetGraphParam( GRP_BACK, DRW_LPP(RI,rate) );
				break;
			case BAK_CFADE_LE:
				if(sp)	DSP_SetGraphParam( GRP_BACK, DRW_LCF(LE,rate) );
				else	DSP_SetGraphParam( GRP_BACK, DRW_LPP(LE,rate) );
				break;
			case BAK_CFADE_CE:
				if(sp)	DSP_SetGraphParam( GRP_BACK, DRW_LCF(CE,rate) );
				else	DSP_SetGraphParam( GRP_BACK, DRW_LPP(CE,rate) );
				break;
			case BAK_CFADE_OU:
				if(sp)	DSP_SetGraphParam( GRP_BACK, DRW_LCF(OU,rate) );
				else	DSP_SetGraphParam( GRP_BACK, DRW_LPP(OU,rate) );
				break;
			case BAK_DIA1:	case BAK_DIA2:	case BAK_DIA3:
				if(sp)	DSP_SetGraphParam( GRP_BACK, DRW_DIO(BackStruct.fd_type-BAK_DIA1,rate) );
				else	DSP_SetGraphParam( GRP_BACK, DRW_DIA(BackStruct.fd_type-BAK_DIA1,rate) );
				break;
			case BAK_CFZOOM1:
				rate = 256*BackStruct.fd_cnt/back_max;
				rate = 256-rate*rate/256;
				if(sp && MainWindow.draw_mode2!=16)	DSP_SetGraphParam( GRP_BACK, DRW_BLD(128-rate/2) );
				else								DSP_SetGraphParam( GRP_BACK, DRW_AMI(256-rate)   );
				rate = 256 - 256*BackStruct.fd_cnt/back_max;
				rate = rate*rate/256;
				DSP_SetGraphZoom2( GRP_BACK, DISP_X/2, DISP_Y/2, rate );
				break;
			case BAK_CFZOOM2:
				rate = 256-256*BackStruct.fd_cnt/back_max;
				rate = rate*rate/256;
				rate = 256-rate;
				if(sp)	DSP_SetGraphParam( GRP_BACK, DRW_BLD(rate) );
				else	DSP_SetGraphParam( GRP_BACK, DRW_AMI(rate) );
				DSP_SetGraphZoom2( GRP_BACK+1, DISP_X/2, DISP_Y/2, rate*2 );
				break;
			case BAK_CFZOOM3:
				rate = 256-256*BackStruct.fd_cnt/back_max;
				rate = rate*rate/256;
				if(sp)	DSP_SetGraphParam( GRP_BACK+1, DRW_BLD(rate) );

				DSP_SetGraphZoom2( GRP_BACK+1, DISP_X/2, DISP_Y/2, rate/2-128 );
				break;
			case BAK_CFZOOM4:
				rate = 256 - 256*BackStruct.fd_cnt/back_max;
				if(sp && MainWindow.draw_mode2!=16)
					DSP_SetGraphParam( GRP_BACK, DRW_BLD(32-rate/8) );


				rate = rate*rate/256;
				DSP_SetGraphZoom2( GRP_BACK, DISP_X/2, DISP_Y/2, -rate/2 );
				break;
			case BAK_KAMI:
				x = DISP_X*rate/256;
				DSP_SetGraph( GRP_BACK, BMP_BACK, LAY_BACK, ON, CHK_NO );
				if(x<320){
					DSP_SetGraphPos( GRP_BACK, 0, 0, 0, 0, x, DISP_Y );
					DSP_SetGraph( GRP_BACK, BMP_BACK, LAY_BACK, ON, CHK_NO );
					DSP_SetGraphPos( GRP_BACK+4, DISP_X-x, 0, DISP_X-x, 0, x, DISP_Y );
				}else{
					DSP_ResetGraph( GRP_BACK+4 );
					DSP_SetGraphPos( GRP_BACK, 0, 0, 0, 0, DISP_X, DISP_Y );
					DSP_SetGraph( GRP_BACK, BMP_BACK, LAY_BACK, ON, CHK_NO );
				}

				DSP_SetGraph( GRP_BACK+1, BMP_BACK+1, LAY_BACK+2, ON, CHK_NO );
				DSP_SetGraph( GRP_BACK+2, BMP_BACK+1, LAY_BACK+2, ON, CHK_NO );
				DSP_SetGraph( GRP_BACK+3, BMP_BACK+1, LAY_BACK+1, ON, CHK_NO );
				DSP_SetGraphPos( GRP_BACK+1, -x, 0, 0, 0, min(DISP_X,x*2), DISP_Y );
				DSP_SetGraphPos( GRP_BACK+2, x, 0, 0, 0, DISP_X, DISP_Y );
				DSP_SetGraphPos( GRP_BACK+3, x, 0, min(DISP_X,x*2), 0, max(0,DISP_X-x*2), DISP_Y );
				
				if(sp){
					DSP_SetGraphParam( GRP_BACK+1, DRW_BLD(128) );
					DSP_SetGraphParam( GRP_BACK+2, DRW_BLD(128) );
				}else{
					DSP_SetGraphParam( GRP_BACK+1, DRW_AMI(128) );
					DSP_SetGraphParam( GRP_BACK+2, DRW_AMI(128) );
				}
				break;
			case BAK_SLIDE_UP:	case BAK_SLIDE_DO:
			case BAK_SLIDE_RI:	case BAK_SLIDE_LE:
				if(sp){
					DSP_SetGraphParam( GRP_BACK+0, DRW_BLD(rate) );
					DSP_SetGraphParam( GRP_BACK+1, DRW_BLD(rate) );
				}
				rate=256-rate;
				y = DISP_Y-DISP_Y*rate*rate/(256*256);
				x = DISP_X-DISP_X*rate*rate/(256*256);
				
				switch(BackStruct.fd_type){
					case BAK_SLIDE_UP:
						DSP_SetGraphMove( GRP_BACK+0, 0, y-DISP_Y );
						DSP_SetGraphMove( GRP_BACK+1, 0, y );
						break;
					case BAK_SLIDE_DO:
						DSP_SetGraphMove( GRP_BACK+0, 0, -y+DISP_Y );
						DSP_SetGraphMove( GRP_BACK+1, 0, -y );
						break;
					case BAK_SLIDE_RI:
						DSP_SetGraphMove( GRP_BACK+0, -x+DISP_X, 0 );
						DSP_SetGraphMove( GRP_BACK+1, -x, 0 );
						break;
					case BAK_SLIDE_LE:
						DSP_SetGraphMove( GRP_BACK+0, x-DISP_X, 0 );
						DSP_SetGraphMove( GRP_BACK+1, x, 0 );
						break;
				}
				break;
			case BAK_LASTERIN:
				if(sp){
					DSP_SetGraphParam( GRP_BACK+1, DRW_LST(min(255,rate),GlobalCount2*2) );
					DSP_SetGraphParam( GRP_BACK, DRW_BLD(rate) );
				}else{
					DSP_SetGraphParam( GRP_BACK, DRW_NIS(rate) );
				}
				break;
			case BAK_NOISE:
				DSP_SetGraphParam( GRP_BACK, DRW_NIS(rate) );
				break;
			case BAK_PIPLE:
				DSP_SetGraphParam( GRP_BACK+1, DRW_RPL(rate) );
				DSP_SetGraphParam( GRP_BACK, DRW_BLD(rate) );
				break;
			case BAK_TV:
				rate = 150*BackStruct.fd_cnt/back_max;
				rate = 150-rate;
				rate = rate*rate*rate/(150*150);
				rate = 150-rate;
				if(rate<back_max*120){
					DSP_SetGraphZoom( GRP_BACK+1, 0, (DISP_Y/2)*rate/120, DISP_X, DISP_Y-(DISP_Y/2)*rate/120*2 );
				}else{
					DSP_SetGraphDisp( GRP_BACK+1, OFF );
				}
				{
					int	rate2 = 150*BackStruct.fd_cnt/back_max;
					rate2 = max(0,rate2-110);
					DebugPrintf("%d:%d\n", rate2, BackStruct.fd_cnt );
					DSP_SetGraphZoom( GRP_BACK+2, rate2*10, (DISP_Y/2)*rate/150, DISP_X-rate2*20, DISP_Y-299*rate/150*2 );
					DSP_SetGraphFade( GRP_BACK+2, 128*BackStruct.fd_cnt/back_max );
					DSP_SetGraphParam( GRP_BACK+2, DRW_ADD );
				}
				DSP_SetGraphPrim( GRP_BACK, PRM_FLAT, POL_RECT, LAY_BACK, ON );
				DSP_SetGraphFade( GRP_BACK, 0 );
				DSP_SetGraphPosRect( GRP_BACK, 0,0,DISP_X,DISP_Y );
				break;
			case BAK_ROLL:
				rate = 128*BackStruct.fd_cnt/back_max;
				DSP_SetGraphRoll( GRP_BACK+1, DISP_X/2, DISP_Y/2, rate, rate, 0,0, DISP_X, DISP_Y );
				DSP_SetGraphParam( GRP_BACK+1, DRW_BLD(32) );
				DSP_SetGraphFade( GRP_BACK+1, 128-rate );
				rate = rate*rate/256;

				DSP_SetGraphDisp( GRP_BACK, OFF );
				break;
			default:
				DSP_SetGraphBSet( GRP_BACK, BMP_BACK+2, rate, BackStruct.fd_vague );
				break;
			}
		}
	}
}

BOOL AVG_WaitBack(void)
{
	return BackStruct.fd_flag;
}

void AVG_SetBackFade(int r,int g,int b, int fade )
{
	AVG_ResetHalfTone();
	AVG_SetNovelMessageDisp(OFF);

	BackStruct.br_flag = 1;	
	BackStruct.br_cnt  = 0;	
	BackStruct.br_fade = fade;	
	BackStruct.br_pat  = -1;	
	BackStruct.er = r;
	BackStruct.eg = g;
	BackStruct.eb = b;
}
void AVG_SetBackFadePatern( int in_out, int add_sub, int pno, int fade, int rev, int kaiten, int rx, int ry )
{
	char	fname[256];

	AVG_ResetHalfTone();
	AVG_SetNovelMessageDisp(OFF);

	
	if( !Avg.level ){
		if(in_out){
			AVG_SetBackFade( 128, 128, 128, fade );
		}else{
			if(add_sub)	AVG_SetBackFade( 255, 255, 255, fade );
			else		AVG_SetBackFade(   0,   0,   0, fade );
		}
		return ;
	}

	BackStruct.br_flag = 1;		
	BackStruct.br_cnt  = 0;		
	BackStruct.br_fade = fade;	
	BackStruct.br_pat  = pno;	
	BackStruct.br_type = ((in_out)? 0x01 : 0x00 ) | (add_sub<<4) | (kaiten<<5);
	BackStruct.rx = rx;
	BackStruct.ry = ry;

	if(in_out){	
		BackStruct.er = 128;
		BackStruct.eg = 128;
		BackStruct.eb = 128;
	}else{		
		if(add_sub){
			BackStruct.er = 255;
			BackStruct.eg = 255;
			BackStruct.eb = 255;
		}else{		
			BackStruct.er = 0;
			BackStruct.eg = 0;
			BackStruct.eb = 0;
		}
	}
	wsprintf( fname, "f%04d.bmp", pno );
	if(rev)	DSP_LoadBmp( BMP_WORK+10, BMP_256P, fname, "nega.amp" );
	else	DSP_LoadBmp( BMP_WORK+10, BMP_256P, fname );
	DSP_SetGraph( GRP_WORK+20, BMP_WORK+10, LAY_FORE+3, ON, CHK_NO );
	if(add_sub)	DSP_SetGraphParam( GRP_WORK+20, DRW_ADD );
	else		DSP_SetGraphParam( GRP_WORK+20, DRW_SUB );
	
	if(!DemoFede){
		if(ChipBackStruct.flag){
			DSP_SetGraphClip( GRP_WORK+20, 0, CHIPBACK_SY, 640, 352 );
		}
	}
}


void AVG_SetBackFadeDirect(int r,int g,int b )
{
	BackStruct.rr = BackStruct.r = BackStruct.er = r;
	BackStruct.gg = BackStruct.g = BackStruct.eg = g;
	BackStruct.bb = BackStruct.b = BackStruct.eb = b;
}
BOOL AVG_WaitBackFade(void)
{
	return BackStruct.br_flag;
}

void AVG_ControlBackFade(void)
{
	int	back_max = AVG_EffCnt( BackStruct.br_fade );
	int	rate,i;
	
	if(BackStruct.br_flag){	
		if(ChipBackStruct.flag){
			back_max = AVG_EffCnt3( BackStruct.br_fade );
		}

		BackStruct.br_cnt++;	

		if(BackStruct.br_pat==-1){
			if( BackStruct.br_cnt>=back_max ){
				BackStruct.rr = BackStruct.r = BackStruct.er;
				BackStruct.gg = BackStruct.g = BackStruct.eg;
				BackStruct.bb = BackStruct.b = BackStruct.eb;
				BackStruct.br_flag = 0;
			}else{
				int		cnt = BackStruct.br_cnt;
				BackStruct.rr = (BackStruct.er*cnt + BackStruct.r*(back_max-cnt))/back_max;
				BackStruct.gg = (BackStruct.eg*cnt + BackStruct.g*(back_max-cnt))/back_max;
				BackStruct.bb = (BackStruct.eb*cnt + BackStruct.b*(back_max-cnt))/back_max;
			}
			if(ChipBackStruct.flag){
				if(DemoFede){
					DSP_SetGraphBright( GRP_SYSTEM2+0, max(0,(BackStruct.rr-128)*2), max(0,(BackStruct.gg-128)*2), max(0,(BackStruct.bb-128)*2) );
					DSP_SetGraphBright( GRP_SYSTEM2+1, max(0,(BackStruct.rr-128)*2), max(0,(BackStruct.gg-128)*2), max(0,(BackStruct.bb-128)*2) );
				}
			}else{
				DSP_SetGraphBright( GRP_BACK, BackStruct.rr, BackStruct.gg, BackStruct.bb );
			}
			for(i=GRP_SCRIPT;i<GRP_ENDING;i++)
				DSP_SetGraphBright( i, BackStruct.rr, BackStruct.gg, BackStruct.bb );
		}else{
			if( BackStruct.br_cnt>=back_max ){
				BackStruct.rr = BackStruct.r = BackStruct.er;
				BackStruct.gg = BackStruct.g = BackStruct.eg;
				BackStruct.bb = BackStruct.b = BackStruct.eb;

				BackStruct.br_flag = 0;
				BackStruct.br_pat  = -1;

				DSP_ResetGraph(GRP_WORK+20);
				DSP_ReleaseBmp(BMP_WORK+10);

				if(ChipBackStruct.flag){
					if(DemoFede){
						DSP_SetGraphBright( GRP_SYSTEM2+0, max(0,(BackStruct.rr-128)*2), max(0,(BackStruct.gg-128)*2), max(0,(BackStruct.bb-128)*2) );
						DSP_SetGraphBright( GRP_SYSTEM2+1, max(0,(BackStruct.rr-128)*2), max(0,(BackStruct.gg-128)*2), max(0,(BackStruct.bb-128)*2) );
					}
				}else{
					DSP_SetGraphBright( GRP_BACK, BackStruct.rr, BackStruct.gg, BackStruct.bb );
				}
				for(i=GRP_SCRIPT;i<GRP_ENDING;i++)
					DSP_SetGraphBright( i, BackStruct.rr, BackStruct.gg, BackStruct.bb );

			}else{
				switch(BackStruct.br_type){
					case 0x01:	rate = 255-255*BackStruct.br_cnt/back_max;	break;	
					case 0x00:	rate = 255*BackStruct.br_cnt/back_max;		break;	
					case 0x31:	
					case 0x11:	rate = 255-255*BackStruct.br_cnt/back_max;	break;	
					case 0x30:	
					case 0x10:	rate = 255*BackStruct.br_cnt/back_max;		break;	
				}
				BackStruct.rr = 128;
				BackStruct.gg = 128;
				BackStruct.bb = 128;

				if(!ChipBackStruct.flag){
					DSP_SetGraphBright( GRP_BACK, 128, 128, 128 );
				}
				DSP_SetGraphBright( GRP_WORK+20, rate, rate, rate );

				if(BackStruct.br_type&0x20){
					rate = timeGetTime()/50%256;
					DSP_SetGraphPosPoly( GRP_WORK+20,
						BackStruct.rx+COS(rate)/4,     BackStruct.ry+SIN(rate)/4,		BackStruct.rx+COS(rate+64)/4,  BackStruct.ry+SIN(rate+64)/4,
						BackStruct.rx+COS(rate+192)/4, BackStruct.ry+SIN(rate+192)/4,	BackStruct.rx+COS(rate+128)/4, BackStruct.ry+SIN(rate+128)/4,
											  0, 0, 799, 0, 0, 599, 799, 599 );
				}
			}
		}
	}
}
void AVG_SetBackPos( int x, int y )
{
	BackStruct.zoom = 0;
	BackStruct.x = x;
	BackStruct.y = y;
	BackStruct.w = DISP_X;
	BackStruct.h = DISP_Y;

	BackStruct.x2 = x+DISP_X;
	BackStruct.y2 = y;
	BackStruct.x3 = x;
	BackStruct.y3 = y+DISP_Y;
	BackStruct.x4 = x+DISP_X;
	BackStruct.y4 = y+DISP_Y;

	DSP_SetGraphPos( GRP_BACK, 0,   0, x, y, DISP_X, DISP_Y );
}

void AVG_SetBackPosZoom( int x, int y, int w, int h )
{
	BackStruct.zoom = 1;
	BackStruct.x = x;	BackStruct.y = y;
	BackStruct.w = w;	BackStruct.h = h;
	BackStruct.x2 = x+w;
	BackStruct.y2 = y;
	BackStruct.x3 = x;
	BackStruct.y3 = y+h;
	BackStruct.x4 = x+w;
	BackStruct.y4 = y+h;


	DSP_SetGraphPosZoom( GRP_BACK, 0,   0, DISP_X, DISP_Y, x, y, w, h );
}

void AVG_SetBackPosPoly( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4 )
{
	BackStruct.zoom = 1;
	BackStruct.x  = x1;	BackStruct.y  = y1;
	BackStruct.x2 = x2;	BackStruct.y2 = y2;
	BackStruct.x3 = x3;	BackStruct.y3 = y3;
	BackStruct.x4 = x4;	BackStruct.y4 = y4;

	DSP_SetGraphPosPoly( GRP_BACK,  0, 0, DISP_X, 0, 0, DISP_Y, DISP_X, DISP_Y, x1, y1, x2, y2, x3, y3, x4, y4 );
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


}

void AVG_SetBackScrollPoly( int x1, int y1, int x2, int y2,
							int x3, int y3, int x4, int y4, int frame, int type )
{
	BackStruct.sc_flag = 1;
	BackStruct.sx    = x1;
	BackStruct.sy    = y1;
	BackStruct.sx2   = x2;
	BackStruct.sy2   = y2;
	BackStruct.sx3   = x3;
	BackStruct.sy3   = y3;
	BackStruct.sx4   = x4;
	BackStruct.sy4   = y4;

	BackStruct.sc_type = type+6;
	BackStruct.sc_cnt  = 0;
	BackStruct.sc_max  = frame;

	DSP_CopyBmp( BMP_BACK, BMP_BACK2 );
}

void AVG_CopyBack( int sc )
{
	if(BackStruct.flag){
		if(sc){
			switch( BackStruct.sc_type/3 ){
				case 0:	AVG_SetBackPos( BackStruct.sx, BackStruct.sy );											break;
				case 1:	AVG_SetBackPosZoom( BackStruct.sx, BackStruct.sy, BackStruct.sw, BackStruct.sh );		break;
				case 2:	AVG_SetBackPosPoly( BackStruct.sx,  BackStruct.sy,  BackStruct.sx2, BackStruct.sy2,
											BackStruct.sx3, BackStruct.sy3, BackStruct.sx4, BackStruct.sy4 );	break;
			}
		}else{
			switch( BackStruct.sc_type/3 ){
				case 0:	AVG_SetBackPos( BackStruct.x, BackStruct.y );										break;
				case 1:	AVG_SetBackPosZoom( BackStruct.x, BackStruct.y, BackStruct.w, BackStruct.h );		break;
				case 2:	AVG_SetBackPosPoly( BackStruct.x,  BackStruct.y,  BackStruct.x2, BackStruct.y2,
											BackStruct.x3, BackStruct.y3, BackStruct.x4, BackStruct.y4 );	break;
			}
		}







	
		DSP_CopyBmp( BMP_BACK, BMP_BACK2 );
	}
}

void AVG_ControlBackScroll( void )
{
	int		x,y,x2,y2,x3,y3,x4,y4;
	int		w,h;
	int		cnt, max;
	int		back_max = AVG_EffCnt4( BackStruct.sc_max );
	void	*bmp = DSP_GetGraphBmp(GRP_BACK);
	int		sx,sy;

	if(BackStruct.sc_flag){
		BackStruct.sc_cnt++;
		if( BackStruct.sc_cnt >= back_max ){
			BackStruct.sc_flag = 0;
			AVG_CopyBack(ON);
			AVG_SetBackChar( BackStruct.x,BackStruct.y, ON );
		}else{
			cnt = BackStruct.sc_cnt;
			max = back_max;
			switch(BackStruct.sc_type){
				case 0:	case 3:	
					break;
				case 1:	case 4:	
					cnt = cnt*cnt;
					max = max*max;
					break;
				case 2:	case 5:	
					cnt = max-cnt;
					cnt = cnt*cnt;
					max = max*max;
					cnt = max-cnt;
					break;
			}

			switch( BackStruct.sc_type/3 ){
				case 0:
					x = ( BackStruct.sx*cnt + BackStruct.x*(max-cnt) )/max;
					y = ( BackStruct.sy*cnt + BackStruct.y*(max-cnt) )/max;
					w = ( BackStruct.sw*cnt + BackStruct.w*(max-cnt) )/max;
					h = ( BackStruct.sh*cnt + BackStruct.h*(max-cnt) )/max;

					DSP_SetGraphPos( GRP_BACK, 0,   0, x, y, DISP_X, DISP_Y );
					break;
				case 1:
					switch( MainWindow.draw_mode2 ){
						case 16:	sx = ((BMP_H*)bmp)->sx;	sy = ((BMP_H*)bmp)->sy;	break;
						case 24:	sx = ((BMP_F*)bmp)->sx;	sy = ((BMP_F*)bmp)->sy;	break;
						case 32:	sx = ((BMP_T*)bmp)->sx;	sy = ((BMP_T*)bmp)->sy;	break;
					}
					x = -( BackStruct.sx*DISP_X/BackStruct.sw * cnt + BackStruct.x*DISP_X/BackStruct.w*(max-cnt) )/max;
					y = -( BackStruct.sy*DISP_Y/BackStruct.sh * cnt + BackStruct.y*DISP_Y/BackStruct.h*(max-cnt) )/max;
					w =  (            sx*DISP_X/BackStruct.sw * cnt +           sx*DISP_X/BackStruct.w*(max-cnt) )/max;
					h =  (            sy*DISP_Y/BackStruct.sh * cnt +           sy*DISP_Y/BackStruct.h*(max-cnt) )/max;





					DSP_SetGraphPosZoom( GRP_BACK, x, y, w, h, 0, 0, sx, sy );
					break;
				case 2:
					x  = ( cnt * BackStruct.sx  + (max-cnt) * BackStruct.x ) / max;
					y  = ( cnt * BackStruct.sy  + (max-cnt) * BackStruct.y ) / max;
					x2 = ( cnt * BackStruct.sx2 + (max-cnt) * BackStruct.x2) / max;
					y2 = ( cnt * BackStruct.sy2 + (max-cnt) * BackStruct.y2) / max;
					x3 = ( cnt * BackStruct.sx3 + (max-cnt) * BackStruct.x3) / max;
					y3 = ( cnt * BackStruct.sy3 + (max-cnt) * BackStruct.y3) / max;
					x4 = ( cnt * BackStruct.sx4 + (max-cnt) * BackStruct.x4) / max;
					y4 = ( cnt * BackStruct.sy4 + (max-cnt) * BackStruct.y4) / max;

					DSP_SetGraphPosPoly( GRP_BACK,  0, 0, DISP_X, 0, 0, DISP_Y, DISP_X, DISP_Y, x, y, x2, y2, x3, y3, x4, y4 );
					break;
			}
			cnt = 64+192*BackStruct.sc_cnt/back_max;


		}
	}
}
BOOL AVG_WaitBackScroll(void)
{
	return BackStruct.sc_flag;
}

#define CSCOPE	OFF

void AVG_SetBackCScope( int flag, int fade )
{

#if	CSCOPE
	BackStruct.cs_flag = 1;
	BackStruct.cs_type = flag;
	BackStruct.cs_cnt  = 0;
	BackStruct.cs_fade = fade;
#endif

	AVG_CopyBack(OFF);
}
void AVG_CreateBackCScope( void )
{
#if	CSCOPE
	int	i;

	if( BackStruct.cs_type && BackStruct.cscop ){
		DSP_SetGraphPrim( GRP_WORK,   PRM_FLAT, POL_RECT, LAY_BACK+1, ON );
		DSP_SetGraphPrim( GRP_WORK+1, PRM_FLAT, POL_RECT, LAY_BACK+1, ON );

		DSP_SetGraphPosRect( GRP_WORK,   0,           0, DISP_X, 64 );
		DSP_SetGraphPosRect( GRP_WORK+1, 0,   DISP_Y-64, DISP_X, 64 );

		DSP_SetGraphBright( GRP_WORK,   0, 0, 0 );
		DSP_SetGraphBright( GRP_WORK+1, 0, 0, 0 );

		DSP_SetGraphTarget( GRP_WORK+0, BMP_BACK, MainWindow.draw_mode2 );
		DSP_SetGraphTarget( GRP_WORK+1, BMP_BACK, MainWindow.draw_mode2 );
		for(i=0;i<2;i++)
			DSP_ResetGraph( GRP_WORK+i );
	}
#endif
}

void AVG_ControlBackCScope(void)
{
#if	CSCOPE
	int	back_max = AVG_EffCnt(BackStruct.cs_fade);
	int	h,i;

	if(BackStruct.cs_flag && BackStruct.cs_type!=-1){
		if( BackStruct.cs_cnt >= back_max ){
			BackStruct.cs_flag = 0;
			BackStruct.cscop = BackStruct.cs_type;
			if(!BackStruct.cs_type){
				for(i=0;i<2;i++)
					DSP_ResetGraph( GRP_WORK+i );

				BackStruct.cs_clip.x = 0;
				BackStruct.cs_clip.y = 0;
				BackStruct.cs_clip.w = DISP_X;
				BackStruct.cs_clip.h = DISP_Y;
			}else{
				AVG_CopyBack(OFF);
				AVG_CreateBackCScope();

				BackStruct.cs_clip.x = 0;
				BackStruct.cs_clip.y = 64;
				BackStruct.cs_clip.w = DISP_X;
				BackStruct.cs_clip.h = DISP_Y-128;
			}
			DSP_SetGraphClip( GRP_BACK,   0, 0, DISP_X, DISP_Y );
			DSP_SetGraphClip( GRP_BACK+1, 0, 0, DISP_X, DISP_Y );
			DSP_SetGraphClip( GRP_BACK+2, 0, 0, DISP_X, DISP_Y );
			DSP_SetGraphClip( GRP_BACK+3, 0, 0, DISP_X, DISP_Y );
		}else{
			if(BackStruct.cs_type){	h = 64*(BackStruct.cs_cnt+1)/back_max;		}
			else				  {	h = 64-64*(BackStruct.cs_cnt+1)/back_max;	}

			BackStruct.cs_clip.x = 0;
			BackStruct.cs_clip.y = h;
			BackStruct.cs_clip.w = DISP_X;
			BackStruct.cs_clip.h = DISP_Y-h*2;
			DSP_SetGraphClip( GRP_BACK,   0, h, DISP_X, DISP_Y-h*2 );
			DSP_SetGraphClip( GRP_BACK+1, 0, h, DISP_X, DISP_Y-h*2 );
			DSP_SetGraphClip( GRP_BACK+2, 0, h, DISP_X, DISP_Y-h*2 );
			DSP_SetGraphClip( GRP_BACK+3, 0, h, DISP_X, DISP_Y-h*2 );

			DSP_SetGraphPrim( GRP_WORK,   PRM_FLAT, POL_RECT, LAY_BACK+1, ON );
			DSP_SetGraphPrim( GRP_WORK+1, PRM_FLAT, POL_RECT, LAY_BACK+1, ON );
			DSP_SetGraphPosRect( GRP_WORK,   0,        0, DISP_X, h );
			DSP_SetGraphPosRect( GRP_WORK+1, 0, DISP_Y-h, DISP_X, h );
			DSP_SetGraphBright( GRP_WORK,   0, 0, 0 );
			DSP_SetGraphBright( GRP_WORK+1, 0, 0, 0 );
		}
		BackStruct.cs_cnt++;
	}
#endif
}
BOOL AVG_WaitBackCScope(void)
{
#if	CSCOPE
	return BackStruct.cs_flag;
#else
	return 1;
#endif
}
void AVG_LockBackCScope( int flag )
{
#if	CSCOPE
	BackStruct.cs_lock = flag;
#endif
}

static int	ShakeDx, ShakeDy;

static int	ShakeSpFlag=0;

#define	SHAKE_SIN			0
#define	SHAKE_2TI			1
#define	SHAKE_ZOOM			2

#define	SHAKE_TXT_SIN		3
#define	SHAKE_TXT_2TI		4

#define	SHAKE_ALL_SIN		6
#define	SHAKE_ALL_2TI		7

#define	SHAKE_RAND			9
#define	SHAKE_TXT_RAND		10
#define	SHAKE_ALL_RAND		11

#define	SHAKE_ROLL			12
#define	SHAKE_ROLL_SIN		13
#define	SHAKE_ROLL_2TI		14

#define	SHAKE_SIN_SET		15
#define	SHAKE_ALL_SIN_SET	16


BOOL AVG_SetShake( int type, int pich, int speed, int dir, int swing )
{
	if(Avg.msg_cut) return FALSE;



	BackStruct.sk_flag  = 1;	
	BackStruct.sk_dir   = dir;	
	BackStruct.sk_pich  = pich;	
	BackStruct.sk_cnt   = 0;	
	BackStruct.sk_type  = type;
	BackStruct.sk_speed = speed;
	BackStruct.sk_swing = swing;

	if( BackStruct.sk_type==SHAKE_SIN && speed==0 ){
		BackStruct.sk_type=SHAKE_SIN_SET;
	}
	if( BackStruct.sk_type==SHAKE_ALL_SIN && speed==0 ){
		BackStruct.sk_type=SHAKE_ALL_SIN_SET;
	}

	if( BackStruct.sk_type==SHAKE_SIN || BackStruct.sk_type==SHAKE_ALL_SIN){
		AVG_ResetHalfTone();
		AVG_SetNovelMessageDisp(OFF);
	}
	if( BackStruct.sk_type==SHAKE_ZOOM || BackStruct.sk_type==SHAKE_ROLL || BackStruct.sk_type==SHAKE_ROLL_SIN || BackStruct.sk_type==SHAKE_ROLL_2TI ){
		if( DSP_GetGraphDisp( GRP_SPBACK ) ){
			ShakeSpFlag = 1;
			BackStruct.sp_flag = 3;
			DSP_SetGraphDisp( GRP_SPBACK, OFF );
		}else{
			if( DSP_GetGraphDisp( GRP_SPBACKHALF ) ){
				ShakeSpFlag = 2;
				BackStruct.sp_flag = 3;
				DSP_SetGraphDisp( GRP_SPBACKHALF, OFF );
			}else{
				ShakeSpFlag = 0;
			}
		}
	}






	ShakeDx=MES_POS_X;
	ShakeDy=MES_POS_Y;
	return TRUE;
}
void AVG_StopShake( void )
{
	if( !BackStruct.sk_flag ) return ;
	BackStruct.sk_flag = 0;			

	if( BackStruct.sk_dir<2 || BackStruct.sk_type != SHAKE_ROLL ){
		DSP_SetGraphZoom2( GRP_BACK, DISP_X/2, DISP_Y/2, 0 );
		DSP_SetGraphParam( GRP_BACK, DRW_NML );
		DSP_SetGraphMove(  GRP_BACK, BackStruct.sx, BackStruct.sy );
		DSP_SetGraphSMove( GRP_BACK, BackStruct.x, BackStruct.y );

		DSP_SetGraphZoom2( GRP_BACK+1, DISP_X/2, DISP_Y/2, 0 );
		DSP_SetGraphParam( GRP_BACK+1, DRW_NML );
		DSP_SetGraphMove(  GRP_BACK+1, BackStruct.sx, BackStruct.sy );
		DSP_SetGraphSMove( GRP_BACK+1, BackStruct.x, BackStruct.y );
	}

	DSP_ResetGraph( GRP_WORK );

		DSP_SetTextMove( TXT_WINDOW, ShakeDx, ShakeDy );

	if( ShakeSpFlag==1 ){
		BackStruct.sp_flag = 2;
		DSP_SetGraphDisp( GRP_SPBACK, ON );
	}
	if( ShakeSpFlag==2 ){
		BackStruct.sp_flag = 2;
		DSP_SetGraphDisp( GRP_SPBACKHALF, ON );
	}

	if( BackStruct.sk_type==SHAKE_SIN || BackStruct.sk_type==SHAKE_ALL_SIN){
		SetCharPosShake( 0, 0, OFF );
	}
	DSP_SetGraphGlobalPos( 0, 0 );
}
BOOL AVG_WaitShake( void )
{
	if( BackStruct.sk_speed==0 ) return 0;
	if( BackStruct.sk_flag ==0 ) return 0;
	return 1;
}
#include <math.h>
void AVG_ControlShake( void )
{
	int	x,y,cnt,cnt2,w,h;
	int	back_max = AVG_EffCnt4( BackStruct.sk_speed );
	int	pich = BackStruct.sk_pich;

	if( BackStruct.sk_flag ){
		BackStruct.sk_cnt++;
		if(BackStruct.sk_cnt>=back_max && BackStruct.sk_speed!=0 ){
			AVG_StopShake();

			DSP_SetGraphBmp( GRP_BACK, BMP_BACK );
			DSP_SetGraphBmp( GRP_BACK+1, BMP_BACKHALF );
		}else{
			switch(BackStruct.sk_type){
				case SHAKE_SIN:
				case SHAKE_ALL_SIN:
				case SHAKE_TXT_SIN:
				case SHAKE_SIN_SET:
				case SHAKE_ALL_SIN_SET:
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

					switch(BackStruct.sk_type){
					case SHAKE_SIN_SET:
					case SHAKE_SIN:
						DSP_SetGraphSMove( GRP_BACK,   BackStruct.x-x, BackStruct.y-y );
						DSP_SetGraphSMove( GRP_BACK+1, BackStruct.x-x, BackStruct.y-y );

						DSP_SetGraphBmp( GRP_BACK, BMP_BACK2 );
						DSP_SetGraphBmp( GRP_BACK+1, BMP_BACK2 );

DSP_SetGraphPrim( GRP_WORK, PRM_FLAT, POL_RECT, 0, ON );
DSP_SetGraphPosRect( GRP_WORK, 0, 0, DISP_X, DISP_X );
DSP_SetGraphFade( GRP_WORK, 0 );
						
						SetCharPosShake( x, y, ON );
						break;
					case SHAKE_TXT_SIN:
						DSP_SetTextMove( TXT_WINDOW, ShakeDx+x, ShakeDy+y );
						break;
					case SHAKE_ALL_SIN_SET:
					case SHAKE_ALL_SIN:
						DSP_SetGraphGlobalPos( x, y );

						break;
					}
					break;
				case SHAKE_2TI:
				case SHAKE_ALL_2TI:
				case SHAKE_TXT_2TI:
					cnt = (BackStruct.sk_cnt%2)*2-1;
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

					switch(BackStruct.sk_type){
					case SHAKE_2TI:
						DSP_SetGraphSMove( GRP_BACK,   BackStruct.x-x, BackStruct.y-y );
						DSP_SetGraphSMove( GRP_BACK+1, BackStruct.x-x, BackStruct.y-y );


DSP_SetGraphPrim( GRP_WORK, PRM_FLAT, POL_RECT, 0, ON );
DSP_SetGraphPosRect( GRP_WORK, 0, 0, DISP_X, DISP_Y );
DSP_SetGraphFade( GRP_WORK, 0 );
						break;
					case SHAKE_TXT_2TI:
						DSP_SetTextMove( TXT_WINDOW, ShakeDx+x, ShakeDy+y );
						break;
					case SHAKE_ALL_2TI:
						DSP_SetGraphGlobalPos( x, y );

						break;
					}
					break;
				case SHAKE_ZOOM:


					cnt = BackStruct.sk_cnt*((long)sqrt( (double)BackStruct.sk_pich )*2)/back_max;
					
					cnt2 = STD_LimitLoop(cnt, (long)sqrt( (double)BackStruct.sk_pich ) );
					cnt2 = cnt2;
					cnt2 = cnt2*cnt2;
					DSP_SetGraphZoom2( GRP_BACK,   DISP_X/2, DISP_Y/2, cnt2 );
					DSP_SetGraphParam( GRP_BACK, DRW_BLD(128) );

					DSP_SetGraphZoom2( GRP_BACK+1,   DISP_X/2, DISP_Y/2, cnt2 );
					DSP_SetGraphParam( GRP_BACK+1, DRW_BLD(128) );
					break;
				case SHAKE_RAND:
				case SHAKE_ALL_RAND:
				case SHAKE_TXT_RAND:
					cnt = rand()%8;
					while( BackStruct.sk_dir==cnt ){
						cnt = rand()%8;
					}
					BackStruct.sk_dir=cnt;
					switch(BackStruct.sk_dir){
						case DIR_L: case DIR_DL: case DIR_UL:	x =-pich;	break;
						case DIR_U: case DIR_D:					x = 0;			break;
						case DIR_R: case DIR_DR: case DIR_UR:	x = pich;	break;
					}
					switch(BackStruct.sk_dir){
						case DIR_D: case DIR_DL: case DIR_DR:	y = pich;	break;
						case DIR_L: case DIR_R:					y = 0;			break;
						case DIR_U: case DIR_UL: case DIR_UR:	y =-pich;	break;
					}

					switch(BackStruct.sk_type){
					case SHAKE_RAND:
						DSP_SetGraphSMove( GRP_BACK,   BackStruct.x-x, BackStruct.y-y );
						DSP_SetGraphSMove( GRP_BACK+1, BackStruct.x-x, BackStruct.y-y );

DSP_SetGraphPrim( GRP_WORK, PRM_FLAT, POL_RECT, 0, ON );
DSP_SetGraphPosRect( GRP_WORK, 0, 0, DISP_X, DISP_Y );
DSP_SetGraphFade( GRP_WORK, 0 );

						break;
					case SHAKE_TXT_RAND:
						DSP_SetTextMove( TXT_WINDOW, ShakeDx+x, ShakeDy+y );
						break;
					case SHAKE_ALL_RAND:
						DSP_SetGraphGlobalPos( x, y );

						break;
					}
					break;
				case SHAKE_ROLL:
					cnt = 256-BackStruct.sk_cnt*256/back_max;
					cnt = 256-cnt*cnt/256;
					if(BackStruct.sk_dir%2){
						cnt = cnt*pich/2%256;
					}else{
						cnt = 256-cnt*pich/2%256;
					}

DSP_SetGraphPrim( GRP_WORK, PRM_FLAT, POL_RECT, 0, ON );
DSP_SetGraphPosRect( GRP_WORK, 0, 0, DISP_X, DISP_Y );
DSP_SetGraphFade( GRP_WORK, 0 );

					DSP_GetGraphBmpSize( GRP_BACK, &w, &h );
					DSP_SetGraphRoll( GRP_BACK, DISP_X/2, DISP_Y/2, 0, cnt, 0, 0, w, h );


					DSP_GetGraphBmpSize( GRP_BACK+1, &w, &h );
					DSP_SetGraphRoll( GRP_BACK+1, DISP_X/2, DISP_Y/2, 0, cnt, 0, 0, w, h );

					break;
				case SHAKE_ROLL_SIN:
					cnt = BackStruct.sk_cnt*BackStruct.sk_swing/8;

					y =-COS(cnt)*pich/4096;
					if(back_max){
						y = y*(back_max-BackStruct.sk_cnt)/back_max;
					}



					DSP_GetGraphBmpSize( GRP_BACK, &w, &h );
					DSP_SetGraphRoll( GRP_BACK, DISP_X/2, DISP_Y/2, 0, (256+y)%256, 0, 0, w, h );


					DSP_GetGraphBmpSize( GRP_BACK+1, &w, &h );
					DSP_SetGraphRoll( GRP_BACK+1, DISP_X/2, DISP_Y/2, 0, (256+y)%256, 0, 0, w, h );

					break;
				case SHAKE_ROLL_2TI:
					cnt = BackStruct.sk_cnt%4;
					switch(cnt){
						case 0:	y =-pich;	break;
						case 3:
						case 1:	y =0;		break;
						case 2:	y =pich;	break;
					}



					DSP_GetGraphBmpSize( GRP_BACK, &w, &h );
					DSP_SetGraphRoll( GRP_BACK, DISP_X/2, DISP_Y/2, 0, (256+y)%256, 0, 0, w, h );

					DSP_GetGraphBmpSize( GRP_BACK+1, &w, &h );
					DSP_SetGraphRoll( GRP_BACK+1, DISP_X/2, DISP_Y/2, 0, (256+y)%256, 0, 0, w, h );
					break;
					
			}
		}
	}
}

void AVG_ControlBack(void)
{
	AVG_ControlSpChar();
	AVG_ControlNoise();
	AVG_ControlBackFade();

	if(ChipBackStruct.flag){
		if(BackStruct.fd_flag){
			AVG_ControlChipBackChange();
			if(!BackStruct.fd_flag){
				AVG_ControlChipBack();
				AVG_ControlChipChar();
			}
		}else{
			AVG_ControlChipBack();
			AVG_ControlChipChar();
		}
	}else{
		AVG_ControlBackChange();
		AVG_ControlBackScroll();
		AVG_ControlBackCScope();
	}

	AVG_ControlShake();

	AVG_ControlRipple();
}


typedef struct{
	char	flag;
	short	cnt;
	short	size;
	short	x;
	short	y;
}PIPPLE_OBJ;

typedef struct{
	int			flag;
	int			cnt;
	PIPPLE_OBJ	obj[200];

	int			angle;
	int			amount;
	int			fade;

	int			tangle;
	int			tamount;
	int			tfade;
}PIPPLE_STRUCT;
PIPPLE_STRUCT	Ripple;

typedef struct{
	int			flag;
	int			cnt;
}PIPPLE_LOST;
PIPPLE_LOST	RippleLost;


void AVG_SetRipple( int x, int y, int size, int single )
{
	int	i;
	for(i=0;i<200;i++){
		if(Ripple.obj[i].flag==0){
			Ripple.obj[i].flag = 1;
			Ripple.obj[i].cnt  = 0;
			Ripple.obj[i].size = LIM(size,4,255);

			if(x==-1 && y==-1){
				Ripple.obj[i].x = rand()%(DISP_X-200)+100;
				Ripple.obj[i].y = rand()%(DISP_Y-200)+100;
			}else{
				Ripple.obj[i].x = x;
				Ripple.obj[i].y = y;
			}
			if(single){
				Ripple.flag = 1;
				Ripple.tangle = Ripple.angle = single;
			}
			break;
		}
	}
}

void AVG_SetRippleSet( int angle, int amount, int fade )
{
	if(amount==0){
		int	i;
		for(i=0;i<200;i++){
			DSP_ResetGraph( GRP_WEATHER+i );
		}
		ZeroMemory( &Ripple, sizeof(Ripple) );
		return ;
	}

	Ripple.flag = 1;
	Ripple.tamount = amount;
	
	if(Ripple.angle==0){
		Ripple.tangle = Ripple.angle = angle;
	}else{
		Ripple.tangle  = angle;
	}
	Ripple.tfade  = fade;
}

BOOL AVG_WaitRipple( void )
{
	return Ripple.flag;
}



static int	wsctr = 0;
void RenewRippleLost( void )
{
	short	*depth = DRW_GetRippleTableAddr();
	int		x,y;

	int	i,j,k;
	i = rand()%(DISP_X/2);
	j = rand()%(DISP_Y/2);
	k = rand()%10+2;
	for( y = -32; y<=32; y++ ){
		if( (DRW_GetRippleTableNwf()+y+j)%2 ){
			for( x = -32; x<=32; x++ ){
				if( sqrt( (double)(x*x+y*y) ) < k ){

					*(depth+(LIM(j+y,2,297))*(DISP_X/2)+LIM(i+x,2,397)) = -LIM(256-(x*x+y*y),0,256);

				}
			}
		}
	}
	wsctr++;
	DRW_RenewRippleTable(0);
}

void AVG_ControlRipple( void )
{
	int	i;
	int	ret=0;
	int	bld;
	int	angle;

	if( Ripple.flag ){
		if(Ripple.angle  < Ripple.tangle  )	Ripple.angle  = LIM( Ripple.angle + 1, 0, 255 );
		if(Ripple.angle  > Ripple.tangle  )	Ripple.angle  = LIM( Ripple.angle - 1, 0, 255 );

		if(Ripple.fade  < Ripple.tfade  )	Ripple.fade  = LIM( Ripple.fade + 1, 0, 255 );
		if(Ripple.fade  > Ripple.tfade  )	Ripple.fade  = LIM( Ripple.fade - 1, 0, 255 );

		angle = Ripple.angle;

		if(Ripple.tamount > Ripple.amount && Ripple.cnt%1==0 ){
			Ripple.amount++;
			AVG_SetRipple( rand()%1200-200, rand()%DISP_X-200, 32+rand()%64, OFF );
			if(Ripple.tamount > Ripple.amount ){
				Ripple.amount++;
				AVG_SetRipple( rand()%1200-200, rand()%DISP_X-200, 32+rand()%64, OFF );
			}
		}
		Ripple.cnt++;

		for(i=0;i<200;i++){
			if( Ripple.obj[i].flag && Ripple.obj[i].cnt<Ripple.obj[i].size ){
				ret|=1;

				int x = ((Ripple.obj[i].x-400)*angle*600 + (Ripple.obj[i].x-400)*(255-angle)*Ripple.obj[i].y)/255/600 +400;
				int y = (600*255-(600-Ripple.obj[i].y)*angle)/255;
				int r = Ripple.obj[i].cnt*(600*angle + Ripple.obj[i].y*(255-angle))/255/600;

				DSP_SetGraphPrim( GRP_WEATHER+i, PRM_FLAT, POL_RPLE, LAY_SPBACK, ON );
				DSP_SetGraphBright( GRP_WEATHER+i, 0,0,64 );
				DSP_SetGraphMove( GRP_WEATHER+i, x, y );
				


				bld = 255-Ripple.obj[i].cnt*255 / Ripple.obj[i].size;

				DSP_SetGraphParam( GRP_WEATHER+i, DRW_PRM4( bld, r, 255, angle ) );

				if(Ripple.obj[i].size<=64){
					Ripple.obj[i].cnt++;
				}else if(Ripple.obj[i].size<=128){
					Ripple.obj[i].cnt+=2;
				}else if(Ripple.obj[i].size<=192){
					Ripple.obj[i].cnt+=3;
				}else{
					Ripple.obj[i].cnt+=4;
				}
			}else{
				if(Ripple.obj[i].flag){
					Ripple.amount = max(0,Ripple.amount-1);
				}
				Ripple.obj[i].flag=0;
				DSP_ResetGraph( GRP_WEATHER+i );
			}
		}
		Ripple.flag = ret;
	}else{
		ZeroMemory( &Ripple, sizeof(Ripple) );
	}
	if(RippleLost.flag){
		RenewRippleLost();
		int	bno;

		if( DSP_GetGraphFlag(GRP_BACK+1) && DSP_GetGraphDisp(GRP_BACK+1) ){
			DSP_SetGraphParam( GRP_BACK+1, DRW_RP2(255,255) );
			DSP_SetGraphFade( GRP_BACK+1, 128-LIM(RippleLost.cnt-128,0,128) );
			bno = DSP_GetGraphBmpNo( GRP_BACK+1);
		}else if( DSP_GetGraphFlag(GRP_BACK) && DSP_GetGraphDisp(GRP_BACK) ){
			DSP_SetGraphParam( GRP_BACK, DRW_RP2(255,255) );
			DSP_SetGraphFade( GRP_BACK, 128-LIM(RippleLost.cnt-128,0,128) );
			bno = DSP_GetGraphBmpNo( GRP_BACK+1);
		}

		DSP_SetGraph( GRP_WORK+0, bno, LAY_BACK, ON, CHK_NO );
		DSP_SetGraph( GRP_WORK+1, bno, LAY_BACK, ON, CHK_NO );
		DSP_SetGraph( GRP_WORK+2, bno, LAY_BACK, ON, CHK_NO );
		DSP_SetGraph( GRP_WORK+3, bno, LAY_BACK, ON, CHK_NO );

		DSP_SetGraphPos( GRP_WORK+0,   0,   0,   0,   0, DISP_X,   2 );
		DSP_SetGraphPos( GRP_WORK+1,   0,   0,   0,   0,   2, DISP_Y );
		DSP_SetGraphPos( GRP_WORK+2, DISP_X-2,   0, DISP_X-2,   0,   2, DISP_Y );
		DSP_SetGraphPos( GRP_WORK+3,   0, DISP_Y-2,   0, DISP_Y-2, DISP_X,   2 );

		DSP_SetGraphFade( GRP_WORK+0, 128-LIM(RippleLost.cnt-128,0,128) );
		DSP_SetGraphFade( GRP_WORK+1, 128-LIM(RippleLost.cnt-128,0,128) );
		DSP_SetGraphFade( GRP_WORK+2, 128-LIM(RippleLost.cnt-128,0,128) );
		DSP_SetGraphFade( GRP_WORK+3, 128-LIM(RippleLost.cnt-128,0,128) );

		RippleLost.cnt+=2;
		if(RippleLost.cnt>256){
			DSP_SetGraphFade( GRP_BACK+1, 0 );
			DSP_SetGraphFade( GRP_BACK, 0 );
			for(i=0;i<4;i++){
				DSP_ResetGraph( GRP_WORK );
			}
			RippleLost.flag=0;
			RippleLost.cnt=0;
			wsctr = 0;
			DRW_SetRippleTable( DISP_X, DISP_Y );
		}
	}
}





void AVG_SetRippleLost( void )
{
	RippleLost.flag = 1;
	RippleLost.cnt = 0;

}

BOOL AVG_WaitRippleLost( void )
{
	return RippleLost.flag;
}

int	Set_month=0;
int	Set_day=0;
void AVG_SkipDate( int month, int day )
{
	Set_month = month;
	Set_day   = day;
}


typedef struct{
	int	month;
	int	day_s;
	int	day_e;
	int	index;
	int	yasumi;
}KINENBI;

KINENBI	KinenbiTbl[10]={
	{3,12,12,0,0},
	{3,20,20,1,1},
	{3,24,24,2,0},
	{3,25,31,3,0},

	{4, 1, 7,3,0},
	{4, 8, 8,4,0},
	{4,29,29,5,1},

	{5, 3, 3,6,1},
	{5, 4, 4,7,1},
	{5, 5, 5,8,1}
};

BOOL AVG_SetCalender( int _month, int _days )
{
	static	int	step=0;
	int	ret=FALSE;
	static	int	count=0;
	char	fname[256];
	int		i;
	int			_youbi=0;
	int			_youbi_tbl[7]={2,0,0,0,0,0,1};
	int			_kinenbi=10;

	switch(step){
	case 0:
		if(Set_month!=0){
			ESC_SetFlag( _MONTH, Set_month );
			ESC_SetFlag( _DAY  , Set_day );
			ESC_SetFlag( _TIME, -1 );

			ESC_SetFlag( _EVENT_NEXT, -1 );
			ESC_SetFlag( _EVENT_END, 0 );
			ESC_SetFlag( _CLOCK_TIME, 0 );

			Set_month = 0;
			Set_day   = 0;
		}
		if(_month==-1){
			_days  = ESC_GetFlag( _DAY  );
			_month = ESC_GetFlag( _MONTH );
		}
		
		switch( _month ){
			case 3:	_youbi = (_days)%7;	break;
			case 4:	_youbi = (_days+3)%7; break;
			case 5:	_youbi = (_days+5)%7; break;
		}
		for(i=0;i<10;i++){
			if(KinenbiTbl[i].month==_month){
				if( KinenbiTbl[i].day_s<=_days && _days <= KinenbiTbl[i].day_e ){
					_kinenbi = i;
				}
			}
		}

		AVG_ResetBack(0);

		wsprintf( fname, "cal00%d.tga", _month );
		DSP_LoadBmp( BMP_MAP, BMP_FULL, fname );
		DSP_LoadBmp( BMP_MAP+1, BMP_TRUE, "cal010.tga" );
		DSP_LoadBmp( BMP_MAP+2, BMP_256P, "cal011.tga" );

		DSP_SetGraph( GRP_MAP, BMP_MAP, LAY_SYSTEM, ON, CHK_NO );

		DSP_SetGraph( GRP_MAP+1, BMP_MAP+2, LAY_SYSTEM+1, ON, CHK_ANTI );
		DSP_SetGraphPos( GRP_MAP+1, 256, 240, _youbi_tbl[_youbi]*248, (_days-1)*144, 248, 144);
		if(_kinenbi<10){
			if(KinenbiTbl[_kinenbi].yasumi){
				DSP_SetGraphPos( GRP_MAP+1, 256, 240, 2*248, (_days-1)*144, 248, 144);
			}
		}

		DSP_SetGraph( GRP_MAP+2, BMP_MAP+1, LAY_SYSTEM+1, ON, CHK_ANTI );
		DSP_SetGraph( GRP_MAP+3, BMP_MAP+1, LAY_SYSTEM+1, ON, CHK_ANTI );
		DSP_SetGraph( GRP_MAP+4, BMP_MAP+1, LAY_SYSTEM+1, ON, CHK_ANTI );

		DSP_SetGraphPos( GRP_MAP+2,  88, 352,   0, _youbi*32, 168, 32 );
		DSP_SetGraphPos( GRP_MAP+3, 504, 347, 168, _youbi*34,  34, 34 );
		if(_kinenbi==10){
			DSP_SetGraphPos( GRP_MAP+4, 538, 331, 202, 3*50, 164, 50 );
		}else{
			DSP_SetGraphPos( GRP_MAP+4, 538, 331, 202+KinenbiTbl[_kinenbi].index/3*164, KinenbiTbl[_kinenbi].index%3*50, 164, 50 );
		}

		step  = 1;
		count=0;
	case 1:
		count++;
		for(i=0;i<5;i++)DSP_SetGraphParam( GRP_MAP+i, DRW_BLD(count*16) );
		if( count>=16 ){
			for(i=0;i<5;i++)DSP_SetGraphParam( GRP_MAP+i, DRW_NML );
			step  = 2;
		}
		break;
	case 2:
		if( AVG_GetMesCut() || AVG_GetHitKey() ){
			step  = 3;
			count = 0;
		}
		break;
	case 3:
		count++;
		for(i=0;i<5;i++)DSP_SetGraphFade( GRP_MAP+i, 128-count*8 );
		if( count>=16 ){
			step  = 4;
		}
		break;
	case 4:
		for(i=0;i<5;i++)DSP_ResetGraph( GRP_MAP+i );
		for(i=0;i<3;i++)DSP_ReleaseBmp( BMP_MAP+i );

		ret = TRUE;
		step  = 0;
		break;
	}
	return ret;
}

int	ViewClockTimeTable[20]={
	 8*60+43,	
	 9*60+ 5,	
	 9*60+25,	
	 9*60+35,	
	10*60+ 0,	
	10*60+20,	
	10*60+30,	
	10*60+55,	
	11*60+15,	
	11*60+25,	
	11*60+50,	
	12*60+10,	
	12*60+35,	
	13*60+ 0,	
	13*60+25,	
	13*60+45,	
	13*60+55,	
	14*60+20,	
	14*60+40,	
	14*60+50,	
};


BOOL AVG_ViewClock( int clock )
{
	static	int	step=0;
	static	int	count=0;

	int	ret=FALSE;
	int	back_clock = ESC_GetFlag( _CLOCK_TIME );

	static int	clock_count=0;

	int	_month = ESC_GetFlag( _MONTH );
	int	_day   = ESC_GetFlag( _DAY  );
	int			_youbi=0;
	
	switch( _month ){
	case 3:	_youbi = (_day)%7; break;
	case 4:	_youbi = (_day+3)%7; break;
	case 5:	_youbi = (_day+5)%7; break;
	}


	switch(step){
	case 0:

		if(back_clock==clock){
			ret = TRUE;
			break;
		}
		if(_youbi==6){
			clock = min(clock,11);
		}
		clock_count = ViewClockTimeTable[back_clock];

		DSP_LoadBmp( BMP_WORK, BMP_TRUE, "clock98.tga" );
		DSP_SetGraph( GRP_WORK,   BMP_WORK,   LAY_SYSTEM,   TRUE, CHK_ANTI );

		DSP_LoadSprite( BMP_WORK+1, "clock99.ani");

		DSP_SetSprite( GRP_WORK+1, BMP_WORK+1, (clock_count/60%12*10 + clock_count%60/6), LAY_SYSTEM+1, TRUE, TRUE, 0, FALSE );
		DSP_SetSprite( GRP_WORK+2, BMP_WORK+1, 120+(clock_count%60*2), LAY_SYSTEM+2, TRUE, TRUE, 0, FALSE );

		DSP_SetGraphMove( GRP_WORK+1, 400, 300 );
		DSP_SetGraphMove( GRP_WORK+2, 400, 300 );

		count = 0;
		step  = 1;
	case 1:
		count = min(count+1,16);
		DSP_SetGraphParam( GRP_WORK, DRW_BLD( count*16) );

		DSP_SetGraphParam( GRP_WORK+1, DRW_BLD( count*16) );
		DSP_SetGraphParam( GRP_WORK+2, DRW_BLD( count*16) );





		if( count==16 ){
			step  = 2;
		}
		break;
	case 2:
		clock_count+=6;
		DSP_SetSpriteNo( GRP_WORK+1, (clock_count/60%12*10 + clock_count%60/6) );
		DSP_SetSpriteNo( GRP_WORK+2, 120 + (clock_count%60*2) );












		DSP_SetGraphParam( GRP_WORK, DRW_NML );
		DSP_SetGraphParam( GRP_WORK+1, DRW_NML );
		DSP_SetGraphParam( GRP_WORK+2, DRW_NML );


		if(_youbi==6){
			clock = min(clock,11);
		}
		if( ViewClockTimeTable[clock] <= clock_count ){
			count = 0;
			step  = 3;
		}
		break;
	case 3:
		count = min(count+1,16);
		DSP_SetGraphParam( GRP_WORK, DRW_BLD( 256-count*16) );
		DSP_SetGraphParam( GRP_WORK+1, DRW_BLD( 256-count*16) );
		DSP_SetGraphParam( GRP_WORK+2, DRW_BLD( 256-count*16) );

		if( count>=16 ){

			DSP_ResetGraph( GRP_WORK );
			DSP_ResetGraph( GRP_WORK+1 );
			DSP_ResetGraph( GRP_WORK+2 );

			DSP_ReleaseBmp( BMP_WORK );
			DSP_ReleaseSprite( BMP_WORK+1 );
			ESC_SetFlag( _CLOCK_TIME, clock );

			ret = TRUE;
			step  = 0;
		}
		break;
	}
	return ret;
}


SPRITE_BMP_STRUCT		SpriteBmp[MAX_SCRIPT_OBJ];

void AVG_LoadBmpSetting( int s_bno, SPRITE_BMP_STRUCT sbs )
{
	if(sbs.flag){
		switch(sbs.type){
		case PRM_BMP:
			AVG_LoadBmp( s_bno, sbs.bmp_bit, sbs.fname, sbs.layer, sbs.nuki, sbs.tone_type, sbs.tfname, sbs.viv, sbs.bright, sbs.pack );
			break;
		case PRM_BAPC:
			AVG_LoadBmpAnime( s_bno, sbs.bmp_bit, sbs.fname, sbs.loop, sbs.layer, sbs.nuki );
			break;
		default:
			AVG_ReleaseBmp( s_bno );
			return;
		}
		AVG_SetBmpDisp( s_bno, sbs.disp );
		AVG_SetBmpParam( s_bno, sbs.param );
		AVG_SetBmpRevParam( s_bno, sbs.rev_param );
		AVG_SetBmpBright( s_bno, sbs.r, sbs.g, sbs.b );


		AVG_SetBmpPos( s_bno, sbs.dx, sbs.dy, sbs.sx, sbs.sy, sbs.dw, sbs.dh );
		if(sbs.zoom){
			AVG_SetBmpZoom2( s_bno, sbs.cx, sbs.cy, sbs.zoom );
		}else{
			if(sbs.dw==sbs.sw && sbs.dh==sbs.sh){
			}else{
				AVG_SetBmpZoom( s_bno, sbs.dx, sbs.dy, sbs.dw, sbs.dh );
			}
		}
	}else{
		AVG_ReleaseBmp( s_bno );
	}
}




void AVG_LoadBmp( int s_bno, int bmp_bit, char *fname, int layer, int nuki, int tone_type, char *tfname, int viv, int bright, char *pack )
{
	int		tone_no;
	char	*eff_fname=NULL;

	AVG_ReleaseBmp(s_bno);
	switch(tone_type){
	case 0:
		
		if(BackStruct.tone_back==-1)tone_no = BackStruct.tone_no;		
		else						tone_no = BackStruct.tone_back;	
		switch( tone_no/TONE_MAX ){
			default:	eff_fname = NULL;			viv = 256;	break;	
			case 1:	eff_fname = "sepia.amp";		viv =   0;	break;
			case 2:	eff_fname = "nega.amp";			viv = 256;	break;	
			case 3:	eff_fname = NULL;				viv =   0;	break;
			case 4:	eff_fname = "blue.amp";			viv = 128;	break;
			case 5:	eff_fname = "red.amp";			viv = 128;	break;
			case 6:	eff_fname = "green.amp";		viv = 128;	break;
			case 7:	eff_fname = "blue2.amp";		viv = 128;	break;
			case 8:	eff_fname = "brown.amp";		viv = 128;	break;
			case 9:	eff_fname = "sepia_half.AMP";	viv = 128;	break;

			case 10:	eff_fname = "black.AMP";	viv = 256;	break;
			case 11:	eff_fname = "yoritomo.AMP";	viv = 256;	break;
		}
		DSP_LoadBmp( BMP_SCRIPT+s_bno, bmp_bit, fname, eff_fname, viv, pack );
		break;
	case 1:
		{
			char	*tname=NULL;
			tone_no = BackStruct.tone_no;
			if( BackStruct.tone_char==-1 ){	
				if( PAC_CheckFile( BMP_GetPackDir(), tone_fname2 ) ){
					tname=tone_fname2;
				}else{
					tname=tone_fname[tone_no%4];
				}
			}else{
				tone_no = BackStruct.tone_char;
				tname=tone_fname[tone_no%4];	
			}
			
			DSP_LoadBmp( BMP_SCRIPT+s_bno, bmp_bit, fname, tname, 256, pack );

			switch( tone_no/TONE_MAX ){
				case 1: eff_fname = "sepia.amp";		viv=0;		break;	
				case 2: eff_fname = "nega.amp";			viv=256;	break;	
				case 3: eff_fname = NULL;				viv=0;		break;	
				case 4: eff_fname = "blue.amp";			viv=128;	break;	
				case 5: eff_fname = "red.amp";			viv=128;	break;	
				case 6: eff_fname = "green.amp";		viv=128;	break;	
				case 7: eff_fname = "blue2.amp";		viv=128;	break;	
				case 8: eff_fname = "brown.amp";		viv=128;	break;	
				case 9:	eff_fname = "sepia_half.AMP";	viv=128;	break;	

				case 10:eff_fname = "black.AMP";		viv=0;	break;	
				case 11:eff_fname = "yoritomo.AMP";		viv=0;	break;	
			}

			if(tone_no/TONE_MAX){
				DSP_SetTonecurve( BMP_SCRIPT+s_bno, eff_fname, viv );
			}
		}
		break;
	default:
		if( !tfname[0] || !strcmp(tfname,"NULL") || !strcmp(tfname,"null") ){
			if( strcmp(fname,"back") ){
				if(bmp_bit){
					DSP_LoadBmp2( BMP_SCRIPT+s_bno, bmp_bit, fname, NULL,256, bright, bright, bright, DRW_NML, REV_NOT, pack );
				}else{
					DSP_LoadBmp( BMP_SCRIPT+s_bno, bmp_bit, fname, NULL, 256, pack );
				}
			}else{
				DSP_CopyBmp( BMP_SCRIPT+s_bno, BMP_BACK );
			}
		} else {
			if(bmp_bit){
				DSP_LoadBmp2( BMP_SCRIPT+s_bno, bmp_bit, fname, tfname, viv, bright, bright, bright, DRW_NML, REV_NOT, pack );
			}else{
				DSP_LoadBmp( BMP_SCRIPT+s_bno, bmp_bit, fname, tfname, viv, pack );
			}
		}
		break;
	}
	DSP_SetGraph( GRP_SCRIPT+s_bno, BMP_SCRIPT+s_bno, layer, ON, nuki );
	MainWindow.draw_flag=ON;


	int	sx,sy;
	DSP_GetBmpSize( BMP_SCRIPT+s_bno, &sx, &sy );
	ZeroMemory( &SpriteBmp[s_bno], sizeof(SPRITE_BMP_STRUCT) );

	_mbscpy( (BYTE*)SpriteBmp[s_bno].fname, (BYTE*)fname );
	SpriteBmp[s_bno].tone_type = tone_type;
	if(tfname)	_mbscpy( (BYTE*)SpriteBmp[s_bno].tfname, (BYTE*)tfname );
	if(pack)	_mbscpy( (BYTE*)SpriteBmp[s_bno].pack, (BYTE*)pack );
	else		_mbscpy( (BYTE*)SpriteBmp[s_bno].pack, (BYTE*)PAC_GRP );

	SpriteBmp[s_bno].flag=ON;
	SpriteBmp[s_bno].disp=ON;
	SpriteBmp[s_bno].type =PRM_BMP;
	SpriteBmp[s_bno].param=DRW_NML;
	SpriteBmp[s_bno].rev_param   = 0;
	SpriteBmp[s_bno].viv = viv;			
	SpriteBmp[s_bno].bright = bright;	

	SpriteBmp[s_bno].r = 128;	
	SpriteBmp[s_bno].g = 128;	
	SpriteBmp[s_bno].b = 128;	

	SpriteBmp[s_bno].bmp_bit = bmp_bit;
	SpriteBmp[s_bno].loop    = 0;
	SpriteBmp[s_bno].layer   = layer;
	SpriteBmp[s_bno].nuki    = nuki;

	SpriteBmp[s_bno].dx=0;
	SpriteBmp[s_bno].dy=0;
	SpriteBmp[s_bno].dw=sx;
	SpriteBmp[s_bno].dh=sy;
	SpriteBmp[s_bno].sx=0;
	SpriteBmp[s_bno].sy=0;
	SpriteBmp[s_bno].sw=sx;
	SpriteBmp[s_bno].sh=sy;
	SpriteBmp[s_bno].zoom=0;
	SpriteBmp[s_bno].roll=0;
	SpriteBmp[s_bno].cx=0;
	SpriteBmp[s_bno].cy=0;
	
	return ;
}

void AVG_LoadBmpAnime( int s_bno, int bmp_bit, char *fname, int loop, int layer, int nuki )
{
	int	mh;
	AVG_ReleaseBmp(s_bno);
	if(s_bno!=-1){
		mh = DSP_LoadBmpAnime( BMP_SCRIPT+s_bno, bmp_bit, fname );

		DSP_SetGraph( GRP_SCRIPT+s_bno, BMP_SCRIPT+s_bno, layer, ON, nuki );
		DSP_PlayBmpAnime( mh, ON, loop );

	}else{
		s_bno = MAX_SCRIPT_OBJ-1;
		mh   = DSP_LoadBmpAnime( -1, bmp_bit, fname );
		DSP_PlayBmpAnime( mh, ON, loop );
	}
	if(SpriteBmp[s_bno].mh==-1){
		AVG_ReleaseBmp( s_bno );
		return ;
	}


	int	sx,sy;
	DSP_GetBmpAnimeSize( SpriteBmp[s_bno].mh, &sx, &sy );
	ZeroMemory( &SpriteBmp[s_bno], sizeof(SPRITE_BMP_STRUCT) );

	SpriteBmp[s_bno].mh = mh;

	_mbscpy( (BYTE*)SpriteBmp[s_bno].fname, (BYTE*)fname );
	SpriteBmp[s_bno].tone_type = 0;
	ZeroMemory( SpriteBmp[s_bno].tfname, 32 );
	ZeroMemory( SpriteBmp[s_bno].tfname, 8 );
	SpriteBmp[s_bno].flag = ON;
	SpriteBmp[s_bno].disp = ON;

	SpriteBmp[s_bno].type    = PRM_BAPC;
	SpriteBmp[s_bno].param   = DRW_NML;
	SpriteBmp[s_bno].rev_param   = 0;
	SpriteBmp[s_bno].viv     = 256;	
	SpriteBmp[s_bno].bright = 128;	

	SpriteBmp[s_bno].r = 128;	
	SpriteBmp[s_bno].g = 128;	
	SpriteBmp[s_bno].b = 128;	

	SpriteBmp[s_bno].bmp_bit = bmp_bit;
	SpriteBmp[s_bno].loop    = loop;
	SpriteBmp[s_bno].layer   = layer;
	SpriteBmp[s_bno].nuki    = nuki;

	SpriteBmp[s_bno].dx = 0;
	SpriteBmp[s_bno].dy = 0;
	SpriteBmp[s_bno].dw = sx;
	SpriteBmp[s_bno].dh = sy;

	SpriteBmp[s_bno].sx = 0;
	SpriteBmp[s_bno].sy = 0;
	SpriteBmp[s_bno].sw = sx;
	SpriteBmp[s_bno].sh = sy;
	SpriteBmp[s_bno].zoom = 0;
	SpriteBmp[s_bno].roll = 0;
	SpriteBmp[s_bno].cx=0;
	SpriteBmp[s_bno].cy=0;
}

void AVG_ReleaseBmp( int s_bno )
{
	if(s_bno==-1) s_bno=MAX_SCRIPT_OBJ-1;
	if(SpriteBmp[s_bno].flag){
		if(SpriteBmp[s_bno].type == PRM_BAPC){
			DSP_ReleaseBmpAnime( SpriteBmp[s_bno].mh );
		}
		DSP_ReleaseBmp( BMP_SCRIPT+s_bno );
		DSP_ResetGraph( GRP_SCRIPT+s_bno );
	}
	ZeroMemory( &SpriteBmp[s_bno], sizeof(SPRITE_BMP_STRUCT) );
}

BOOL AVG_WaitBmpAnime( int s_bno )
{
	if(s_bno==-1) s_bno=MAX_SCRIPT_OBJ-1;
	if( !SpriteBmp[s_bno].flag ) return 1;
	if( DSP_GetBmpAnimeLoop( SpriteBmp[s_bno].mh )==0 ) return 1; 
	return DSP_GetBmpAnimeCondition( SpriteBmp[s_bno].mh ) != MOVE_COND_PLAY;
}



void AVG_SetBmpDisp( int s_bno, int disp )
{
	DSP_SetGraphDisp( GRP_SCRIPT+s_bno, disp );

	SpriteBmp[s_bno].disp = disp;
}
void AVG_SetBmpLayer( int s_bno, int layer )
{
	DSP_SetGraphLayer( GRP_SCRIPT+s_bno, layer );

	SpriteBmp[s_bno].layer = layer;
}
void AVG_SetBmpParam( int s_bno, DWORD param )
{
	DSP_SetGraphParam( GRP_SCRIPT+s_bno, param );

	SpriteBmp[s_bno].param = param;
}
void AVG_SetBmpRevParam( int s_bno, DWORD rev_param )
{
	DSP_SetGraphRevParam( GRP_SCRIPT+s_bno, rev_param );

	SpriteBmp[s_bno].rev_param = rev_param;
}
void AVG_SetBmpBright(  int s_bno, int r, int g, int b )
{
	DSP_SetGraphBright( GRP_SCRIPT+s_bno, r, g, b );

	SpriteBmp[s_bno].r = r;
	SpriteBmp[s_bno].g = g;
	SpriteBmp[s_bno].b = b;
}
void AVG_SetBmpMove( int s_bno, int x, int y )
{
	DSP_SetGraphMove( GRP_SCRIPT+s_bno, x*800/640, y*600/448 );

	SpriteBmp[s_bno].dx = x;
	SpriteBmp[s_bno].dy = y;
}
void AVG_SetBmpPos( int s_bno, int dx, int dy, int sx, int sy, int w, int h )
{
	DSP_SetGraphPos( GRP_SCRIPT+s_bno, dx*800/640, dy*600/448, sx*800/640, sy*600/448, w*800/640, h*600/448 );

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
	DSP_SetGraphZoom( GRP_SCRIPT+s_bno, dx*800/640, dy*600/448, dw*800/640, dh*600/448 );


	SpriteBmp[s_bno].dx = dx;
	SpriteBmp[s_bno].dy = dy;
	SpriteBmp[s_bno].dw = dw;
	SpriteBmp[s_bno].dh = dh;
}
void AVG_SetBmpZoom2( int s_bno, int cx, int cy, int zoom )
{
	DSP_SetGraphZoom2( GRP_SCRIPT+s_bno, cx*800/640, cy*600/448, zoom );

	SpriteBmp[s_bno].zoom = zoom;
	SpriteBmp[s_bno].cx = cx;
	SpriteBmp[s_bno].cy = cy;
}




#include "movie.h"
#define AVI_CH	0

typedef struct{
	int		flag;
	int		bno;


	int		AviFull;
	int		Avi640;			
	int		cond;			
	int		end;
	int		end_start;
}AVG_AVI;
static AVG_AVI		AvgAvi[AVI_MAX];


void AVG_SetAvi( int avino, int bno, int w, int h, char *fname )
{
	AVG_ResetAvi( avino );




	DSP_LoadAvi( avino, BMP_SCRIPT+bno, w, h, MainWindow.draw_mode2, fname );
	DSP_SetGraph( GRP_SCRIPT+bno, BMP_SCRIPT+bno, LAY_DISP, ON, -1 );
	AVG_SetBmpMove( GRP_SCRIPT+bno, 0, 0 );

	AvgAvi[avino].flag=ON;
	AvgAvi[avino].bno =bno;
}

void AVG_ResetAvi( int avino )
{
	if(AvgAvi[avino].flag){
		DSP_ReleaseAvi(avino);
		DSP_ResetGraph( GRP_SCRIPT+AvgAvi[avino].bno );
	}
	ZeroMemory( &AvgAvi[avino], sizeof(AVG_AVI) );
}

BOOL AVG_WaitAvi( int avino )
{
	AvgAvi[avino].cond = DSP_GetAviParam(avino);	
	return AvgAvi[avino].cond!=AVI_PLAY;
}
