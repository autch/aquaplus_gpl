
 
  
   
    
     
      

#include "mm_std.h"
#include "main.h"

#include "bmp.h"
#include "draw.h"
#include "escript.h"
#include "disp.h"
#include "text.h"
#include "keybord.h"
#include "mouse.h"



#include "main.h"
#include "GM_avg.h"
#include "GM_avgBack.h"
#include "GM_Save.h"
#include "GM_title.h"





CHIP_BACK_STRUCT	ChipBackStruct;



typedef struct{
	int		flag;
	int		x,y;
	int		nuki;
	int		w,h;
	int		lw;		
	int		ln;		
	int		depth;
	DWORD	param;

	char	gno[8];
}CHIP_PARTS_STRUCT;
CHIP_PARTS_STRUCT	ChipBackParts[GRP_BACK_CHIP];

typedef struct{
	int		flag;
	float	x, y;
	int		gno;	

	int		act;	
	int		rev;	

	float	px, py;	
	float	tx, ty;	
	float	sx, sy;	
	int		frame;	
	int		count;	

	int		cno;	
	int		cond;	
	int		end;	
	int		lnum;	

	DWORD	param;
	int		r,g,b;

	short	sound[1000][4];
	char	svol[1000][4];
	int		sflag;
	int		scnt;
	int		smax;
}CHIP_CHAR_STRUCT;
CHIP_CHAR_STRUCT	ChipChar[GRP_CHAR_CHIP];

#define ACT_NULL	0x00	
#define ACT_MOVE	0x01	
#define ACT_ANI		0x02	
#define ACT_SPEED	0x04	

char	BackChipFlag[GRP_BACK_CHIP+GRP_CHAR_CHIP];

static int	ChipDrawPose=0;

static char	ChipBackNo[100];
static char	ChipCharNo[100];


void AVG_PoseChipBack( void )
{
	int		i;
	ChipDrawPose = ON;	
	
	for( i=0 ; i<GRP_BACK_CHIP ; i++ )	DSP_SetGraphDisp( GRP_BACK+i, OFF );
	for( i=0; i<GRP_CHAR_CHIP ;i++ ){
		if( ChipChar[i].flag )	DSP_SetGraphDisp( ChipChar[i].gno, OFF );
	}

}

void AVG_PlayChipBack( void )
{
	int		i;
	ChipDrawPose = OFF;	
	for( i=0 ; i<GRP_BACK_CHIP ; i++ )	DSP_SetGraphDisp( GRP_BACK+i, ON );
	for( i=0; i<GRP_CHAR_CHIP ;i++ ){
		if( ChipChar[i].flag )	DSP_SetGraphDisp( ChipChar[i].gno, ON );
	}

}

void AVG_ResetBack( int bmp_init)
{
	int		i;
	for(i=GRP_BACK2;i<GRP_CHAR;i++) DSP_ResetGraph( i );
	for(i=BMP_BACK;i<BMP_CHAR;i++) DSP_ReleaseBmp( i );

	for(i=GRP_WORK;i<GRP_SCRIPT;i++) DSP_ResetGraph( i );
	if(bmp_init) for(i=GRP_SCRIPT;i<GRP_ENDING;i++) DSP_ResetGraph( i );
	for(i=GRP_SYSTEM2;i<GRP_SELECT;i++) DSP_ResetGraph( i );

	DSP_ResetGraph( GRP_SPBACK );

	DSP_ReleaseSpriteAll();

	for(i=0;i<GRP_BACK_CHIP+GRP_CHAR_CHIP;i++) DSP_ReleaseSprite( i );

	ZeroMemory( ChipChar, sizeof(CHIP_CHAR_STRUCT)*GRP_CHAR_CHIP );
	ZeroMemory( ChipBackParts, sizeof(CHIP_PARTS_STRUCT)*GRP_BACK_CHIP );
	ZeroMemory( BackChipFlag, GRP_BACK_CHIP+GRP_CHAR_CHIP );

	FillMemory( ChipBackNo, 100, 0xff );
	FillMemory( ChipCharNo, 100, 0xff );
	
	BackStruct.flag     = 0;
	ZeroMemory( &ChipBackStruct, sizeof(ChipBackStruct) );
}

void AVG_ShowChipBack( int chg_type, int fd_max )
{
	int		bmp_bpp = BPP(MainWindow.draw_mode2);
	int		back_max = AVG_EffCnt(fd_max);
	int		sp = Avg.level,i;

	MainWindow.draw_flag = 1;

	
	AVG_ControlChipBack();
	AVG_ControlChipChar();

	
	DSP_CopyBmp( BMP_BACK+1, -1 );
	DSP_SetDrawTemp( BMP_BACK, MainWindow.draw_mode2, Avg.frame );
	DSP_SetGraph( GRP_WORK, BMP_BACK, LAY_BACK, ON, CHK_NO );

	for(i=GRP_BACK;i<GRP_WINDOW;i++ ){
		if( DSP_GetGraphFlag(i) ){
			DSP_SetGraphDisp( i, OFF );
		}
	}
	DSP_SetGraphDisp( GRP_SYSTEM2, OFF );
	DSP_SetGraphDisp( GRP_SYSTEM2+1, OFF );

	if( chg_type!=BAK_DIRECT ){
		if(back_max) {
			switch(chg_type){
			case BAK_FADE:
				if( BackStruct.r || BackStruct.g || BackStruct.b ){

					DSP_SetGraph( GRP_WORK+1, BMP_BACK+1, LAY_BACK, ON, CHK_NO );
					DSP_SetGraphDisp( GRP_WORK, OFF );
				}
				break;
			case BAK_CFADE:
				if(sp){

					DSP_SetGraph( GRP_WORK+1, BMP_BACK+1, LAY_BACK, ON, CHK_NO );
					DSP_SetGraphLayer( GRP_WORK, LAY_BACK+1 );
				}
				break;
			case BAK_CFADE_UP:	case BAK_CFADE_DO:
			case BAK_CFADE_RI:	case BAK_CFADE_LE:
			case BAK_CFADE_CE:	case BAK_CFADE_OU:
				if(sp){

					DSP_SetGraph( GRP_WORK+1, BMP_BACK+1, LAY_BACK, ON, CHK_NO );
					DSP_SetGraphLayer( GRP_WORK, LAY_BACK+1 );
				}
				break;
			case BAK_DIA1:	case BAK_DIA2:	case BAK_DIA3:
				break;
			case BAK_CFZOOM1:
			case BAK_CFZOOM4:
				break;
			case BAK_CFZOOM2:

				DSP_SetGraph( GRP_WORK+1, BMP_BACK+1, LAY_BACK, ON, CHK_NO );
				DSP_SetGraphLayer( GRP_WORK, LAY_BACK+1 );
				break;
			case BAK_CFZOOM3:

				DSP_SetGraph( GRP_WORK+1, BMP_BACK+1, LAY_BACK+1, ON, CHK_NO );
				break;
			case BAK_KAMI:

				DSP_SetGraph( GRP_WORK+1, BMP_BACK+1, LAY_BACK, ON, CHK_NO );
				DSP_SetGraphLayer( GRP_WORK, LAY_BACK+1 );
				break;
			case BAK_SLIDE_UP:	case BAK_SLIDE_DO:
			case BAK_SLIDE_RI:	case BAK_SLIDE_LE:

				DSP_SetGraph( GRP_WORK+1, BMP_BACK+1, LAY_BACK, ON, CHK_NO );
				DSP_SetGraphLayer( GRP_WORK, LAY_BACK+1 );
			case BAK_LASTERIN:

				DSP_SetGraph( GRP_WORK+1, BMP_BACK+1, LAY_BACK, ON, CHK_NO );
				DSP_SetGraphLayer( GRP_WORK, LAY_BACK+1 );
				break;
			case BAK_NOISE:

				DSP_SetGraph( GRP_WORK+1, BMP_BACK+1, LAY_BACK, ON, CHK_NO );
				DSP_SetGraphLayer( GRP_WORK, LAY_BACK+1 );
				break;
			}
		}
		
		BackStruct.fd_flag = 1;
		BackStruct.fd_type = chg_type;
		BackStruct.fd_cnt  = 0;
		BackStruct.fd_max  = fd_max;
	}

	DSP_SetGraphClip( GRP_WORK,   0, 0, DISP_X, DISP_Y );
	DSP_SetGraphClip( GRP_WORK+1, 0, 0, DISP_X, DISP_Y );
	DSP_SetGraphClip( GRP_WORK+2, 0, 0, DISP_X, DISP_Y );
	DSP_SetGraphClip( GRP_WORK+3, 0, 0, DISP_X, DISP_Y );
}

void AVG_ControlChipBackChange(void)
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

			for( i=0; i<4 ;i++ )
				DSP_ResetGraph( GRP_WORK+i );
			DSP_ReleaseBmp( BMP_BACK );
			DSP_ReleaseBmp( BMP_BACK+1 );

			for(i=GRP_BACK;i<GRP_WINDOW;i++ ){
				if( DSP_GetGraphFlag(i) ){
					DSP_SetGraphDisp( i, ON );
				}
			}
			DSP_SetGraphDisp( GRP_SYSTEM2, ON );
			DSP_SetGraphDisp( GRP_SYSTEM2+1, ON );

			if(BackStruct.fd_type==BAK_FADE){
				AVG_SetBackFadeDirect(128,128,128);
			}
		}else{
			rate = 256*BackStruct.fd_cnt/back_max;
			switch(BackStruct.fd_type){
			case BAK_FADE:
				if( BackStruct.r || BackStruct.g || BackStruct.b ){
					if(rate<128){
						DSP_SetGraphBright( GRP_WORK+1, 128-rate, 128-rate, 128-rate );
					}else{
						DSP_SetGraphDisp( GRP_WORK+1, OFF );
						DSP_SetGraphDisp( GRP_WORK,   ON );
						DSP_SetGraphBright( GRP_WORK, rate-128, rate-128, rate-128 );
					}
				}else{
					DSP_SetGraphBright( GRP_WORK, rate/2, rate/2, rate/2 );
				}
				break;
			case BAK_CFADE:
				if(sp)	DSP_SetGraphParam( GRP_WORK, DRW_BLD(rate) );
				else	DSP_SetGraphParam( GRP_WORK, DRW_AMI(rate) );
				break;
			case BAK_CFADE_UP:
				if(sp)	DSP_SetGraphParam( GRP_WORK, DRW_LCF(UP,rate) );
				else	DSP_SetGraphParam( GRP_WORK, DRW_LPP(UP,rate) );
				break;
			case BAK_CFADE_DO:
				if(sp)	DSP_SetGraphParam( GRP_WORK, DRW_LCF(DO,rate) );
				else	DSP_SetGraphParam( GRP_WORK, DRW_LPP(DO,rate) );
				break;
			case BAK_CFADE_RI:
				if(sp)	DSP_SetGraphParam( GRP_WORK, DRW_LCF(RI,rate) );
				else	DSP_SetGraphParam( GRP_WORK, DRW_LPP(RI,rate) );
				break;
			case BAK_CFADE_LE:
				if(sp)	DSP_SetGraphParam( GRP_WORK, DRW_LCF(LE,rate) );
				else	DSP_SetGraphParam( GRP_WORK, DRW_LPP(LE,rate) );
				break;
			case BAK_CFADE_CE:
				if(sp)	DSP_SetGraphParam( GRP_WORK, DRW_LCF(CE,rate) );
				else	DSP_SetGraphParam( GRP_WORK, DRW_LPP(CE,rate) );
				break;
			case BAK_CFADE_OU:
				if(sp)	DSP_SetGraphParam( GRP_WORK, DRW_LCF(OU,rate) );
				else	DSP_SetGraphParam( GRP_WORK, DRW_LPP(OU,rate) );
				break;
			case BAK_DIA1:	case BAK_DIA2:	case BAK_DIA3:
				if(sp)	DSP_SetGraphParam( GRP_WORK, DRW_DIO(BackStruct.fd_type-BAK_DIA1,rate) );
				else	DSP_SetGraphParam( GRP_WORK, DRW_DIA(BackStruct.fd_type-BAK_DIA1,rate) );
				break;
			case BAK_CFZOOM1:
				rate = 256*BackStruct.fd_cnt/back_max;
				rate = 256-rate*rate/256;
				if(sp && MainWindow.draw_mode2!=16)	DSP_SetGraphParam( GRP_WORK, DRW_BLD(128-rate/2) );
				else								DSP_SetGraphParam( GRP_WORK, DRW_AMI(256-rate)   );
				rate = 256 - 256*BackStruct.fd_cnt/back_max;
				rate = rate*rate/256;
				DSP_SetGraphZoom2( GRP_WORK, DISP_X/2, DISP_Y/2, rate );
				break;
			case BAK_CFZOOM2:
				rate = 256-256*BackStruct.fd_cnt/back_max;
				rate = rate*rate/256;
				rate = 256-rate;
				if(sp)	DSP_SetGraphParam( GRP_WORK, DRW_BLD(rate) );
				else	DSP_SetGraphParam( GRP_WORK, DRW_AMI(rate) );
				DSP_SetGraphZoom2( GRP_WORK+1, DISP_X/2, DISP_Y/2, rate*2 );
				break;
			case BAK_CFZOOM3:
				rate = 256-256*BackStruct.fd_cnt/back_max;
				rate = rate*rate/256;
				if(sp)	DSP_SetGraphParam( GRP_WORK+1, DRW_BLD(rate) );

				DSP_SetGraphZoom2( GRP_WORK+1, DISP_X/2, DISP_Y/2, rate-256 );
				break;
			case BAK_CFZOOM4:
				rate = 256 - 256*BackStruct.fd_cnt/back_max;
				if(sp && MainWindow.draw_mode2!=16)
					DSP_SetGraphParam( GRP_WORK, DRW_BLD(128-rate/2) );


				rate = rate*rate/256;
				DSP_SetGraphZoom2( GRP_WORK, DISP_X/2, DISP_Y/2, -rate );
				break;
			case BAK_KAMI:
				x = DISP_X*rate/256;
				DSP_SetGraph( GRP_WORK, BMP_BACK, LAY_BACK, ON, CHK_NO );
				if(x<320){
					DSP_SetGraphPos( GRP_WORK, 0, 0, 0, 0, x, DISP_Y );
					DSP_SetGraph( GRP_WORK, BMP_BACK, LAY_BACK, ON, CHK_NO );
					DSP_SetGraphPos( GRP_WORK+4, DISP_X-x, 0, DISP_X-x, 0, x, DISP_Y );
				}else{
					DSP_ResetGraph( GRP_WORK+4 );
					DSP_SetGraphPos( GRP_WORK, 0, 0, 0, 0, DISP_X, DISP_Y );
					DSP_SetGraph( GRP_WORK, BMP_BACK, LAY_BACK, ON, CHK_NO );
				}

				DSP_SetGraph( GRP_WORK+1, BMP_BACK+1, LAY_BACK+2, ON, CHK_NO );
				DSP_SetGraph( GRP_WORK+2, BMP_BACK+1, LAY_BACK+2, ON, CHK_NO );
				DSP_SetGraph( GRP_WORK+3, BMP_BACK+1, LAY_BACK+1, ON, CHK_NO );
				DSP_SetGraphPos( GRP_WORK+1, -x, 0, 0, 0, min(DISP_X,x*2), DISP_Y );
				DSP_SetGraphPos( GRP_WORK+2, x, 0, 0, 0, DISP_X, DISP_Y );
				DSP_SetGraphPos( GRP_WORK+3, x, 0, min(DISP_X,x*2), 0, max(0,DISP_X-x*2), DISP_Y );
				
				if(sp){
					DSP_SetGraphParam( GRP_WORK+1, DRW_BLD(128) );
					DSP_SetGraphParam( GRP_WORK+2, DRW_BLD(128) );
				}else{
					DSP_SetGraphParam( GRP_WORK+1, DRW_AMI(128) );
					DSP_SetGraphParam( GRP_WORK+2, DRW_AMI(128) );
				}
				break;
			case BAK_SLIDE_UP:	case BAK_SLIDE_DO:
			case BAK_SLIDE_RI:	case BAK_SLIDE_LE:
				if(sp){
					DSP_SetGraphParam( GRP_WORK+0, DRW_BLD(rate) );
					DSP_SetGraphParam( GRP_WORK+1, DRW_BLD(rate) );
				}
				rate=256-rate;
				y = DISP_Y-DISP_Y*rate*rate/(256*256);
				x = DISP_X-DISP_X*rate*rate/(256*256);
				
				switch(BackStruct.fd_type){
					case BAK_SLIDE_UP:
						DSP_SetGraphMove( GRP_WORK+0, 0, y-DISP_Y );
						DSP_SetGraphMove( GRP_WORK+1, 0, y );
						break;
					case BAK_SLIDE_DO:
						DSP_SetGraphMove( GRP_WORK+0, 0, -y+DISP_Y );
						DSP_SetGraphMove( GRP_WORK+1, 0, -y );
						break;
					case BAK_SLIDE_RI:
						DSP_SetGraphMove( GRP_WORK+0, -x+DISP_X, 0 );
						DSP_SetGraphMove( GRP_WORK+1, -x, 0 );
						break;
					case BAK_SLIDE_LE:
						DSP_SetGraphMove( GRP_WORK+0, x-DISP_X, 0 );
						DSP_SetGraphMove( GRP_WORK+1, x, 0 );
						break;
				}
				break;
			case BAK_LASTERIN:
				if(sp){
					DSP_SetGraphParam( GRP_WORK+1, DRW_LST(min(255,rate),GlobalCount2*2) );
					DSP_SetGraphParam( GRP_WORK, DRW_BLD(rate) );
				}else{
					DSP_SetGraphParam( GRP_WORK, DRW_NIS(rate) );
				}
				break;
			case BAK_NOISE:
				DSP_SetGraphParam( GRP_WORK, DRW_NIS(rate) );
				break;
			}
		}
	}
}


void AVG_SetChipBack( int bak_no, int wx, int wy, int ww, int wh )
{
	AVG_ResetBack(1);
	ChipBackStruct.flag   = 1;
	ChipBackStruct.bak_no = bak_no;
	ChipBackStruct.wx = ChipBackStruct.tx = (float)wx;
	ChipBackStruct.wy = ChipBackStruct.ty = (float)wy;
	ChipBackStruct.ww = (float)ww;
	ChipBackStruct.wh = (float)wh;

	DSP_SetGraphPrim( GRP_SYSTEM2+0,   PRM_FLAT, POL_RECT, LAY_BACK+1, ON );
	DSP_SetGraphPosRect( GRP_SYSTEM2+0, 0, 352+CHIPBACK_SY, 640, 128-CHIPBACK_SY );
	DSP_SetGraphFade( GRP_SYSTEM2+0, 0 );

	DSP_SetGraphPrim( GRP_SYSTEM2+1,   PRM_FLAT, POL_RECT, LAY_BACK+1, ON );
	DSP_SetGraphPosRect( GRP_SYSTEM2+1, 0, 0, 640, CHIPBACK_SY );
	DSP_SetGraphFade( GRP_SYSTEM2+1, 0 );

	if(BackStruct.tone_back==-1)wsprintf( tone_fname2, "tb%02d%1d.amp", bak_no, BackStruct.tone_no );
	else						wsprintf( tone_fname2, "tb%02d%1d.amp", bak_no, BackStruct.tone_back );

}













BOOL AVG_SetChipBackParts( int bak_ch, int dx, int dy, int w, int h, int lw, int ln, int depth, int nuki, int lno, int cfade, DWORD param )
{
	char	fname[256];
	int		layer;
	int		i,j,gnum,bp_no;
	int		tone=0, tone_no;

	for( i=0 ; i<GRP_BACK_CHIP ; i++ ){
		if( !ChipBackParts[i].flag ){
			bp_no=i;
			break;
		}
	}
	if(i==GRP_BACK_CHIP){
		DebugBox(NULL,"背景チップ登録数が多過ぎます。[最大%d]",GRP_BACK_CHIP);
		return FALSE;
	}
	ChipBackNo[bak_ch] = bp_no;

	ChipBackParts[bp_no].flag  = 1;
	ChipBackParts[bp_no].x     = dx;
	ChipBackParts[bp_no].y     = dy;
	ChipBackParts[bp_no].w     = w;
	ChipBackParts[bp_no].h     = h;
	ChipBackParts[bp_no].lw    = lw;
	ChipBackParts[bp_no].ln    = ln;
	ChipBackParts[bp_no].depth = depth;
	ChipBackParts[bp_no].nuki  = nuki;

	if(BackStruct.tone_back==-1)tone_no = BackStruct.tone_no;
	else						tone_no = BackStruct.tone_back;

	wsprintf( fname, "tb%02d%d%02d.ani", ChipBackStruct.bak_no, tone_no, bak_ch );
	if( !PAC_CheckFile( BMP_GetPackDir(), fname ) ){
		wsprintf( fname, "tb%02d%d%02d.bmp", ChipBackStruct.bak_no, tone_no, bak_ch );
		if( !PAC_CheckFile( BMP_GetPackDir(), fname ) ){
			switch(tone_no){
				default:
					DebugPrintf( "ファイルが見つかりません[tb%02d%1d%02d.ani/.bmp]",ChipBackStruct.bak_no, tone_no, bak_ch );
				case TONE_SEPIA:
					wsprintf( fname, "tb%02d0%02d.ani", ChipBackStruct.bak_no, bak_ch );
					if( !PAC_CheckFile( BMP_GetPackDir(), fname ) ){
						wsprintf( fname, "tb%02d0%02d.bmp", ChipBackStruct.bak_no, bak_ch );
						if( !PAC_CheckFile( BMP_GetPackDir(), fname ) ){
							DebugBox( NULL, "ファイルが見つかりません[tb%02d%d%02d.ani/.bmp]", ChipBackStruct.bak_no, tone_no, bak_ch );
							ChipBackNo[bak_ch] = 0xff;
							ZeroMemory( &ChipBackParts[bp_no], sizeof(CHIP_PARTS_STRUCT) );
							return FALSE;
						}
					}
					tone=1;
					break;
				case TONE_NORMAL:
					DebugBox( NULL, "ファイルが見つかりません[tb%02d%d%02d.ani/.bmp]", ChipBackStruct.bak_no, tone_no, bak_ch );
					ChipBackNo[bak_ch] = 0xff;
					ZeroMemory( &ChipBackParts[bp_no], sizeof(CHIP_PARTS_STRUCT) );
					return FALSE;
			}
		}
	}


		if(tone){
			DSP_LoadSprite( bp_no, fname, nuki, tone_fname[tone_no%4] );
		}else{
			DSP_LoadSprite( bp_no, fname, nuki );
		}




	if( lno<4 )	layer = LAY_BACK+lno;
	else		layer = LAY_FORE+lno%4;

	if(ln){
		if(lw)	gnum = min( (DISP_X+lw-1)/lw+1, ln );
		else	gnum = 1;
	}else{
		if(lw)	gnum = (DISP_X+lw-1)/lw+1;
		else	gnum = 1;
	}

	for( i=0 ; i<gnum ; i++ ){
		for( j=0 ; j<GRP_BACK_CHIP ; j++ ){
			if(!BackChipFlag[j]){
				BackChipFlag[j] = 1;
				ChipBackParts[bp_no].gno[i] = j;
				ChipBackParts[bp_no].param = param;

				DSP_SetSprite( GRP_BACK+j, bp_no, bp_no, layer, OFF, OFF, 0, cfade );
				DSP_SetGraphParam( GRP_BACK+j, param );
				DSP_SetGraphClip( GRP_BACK+j,   0, CHIPBACK_SY, 640, 352 );
				break;
			}
		}
		if(j==GRP_BACK_CHIP){
			DebugBox(NULL,"背景チップのワーク使用数が多過ぎます。[最大%d]",GRP_BACK_CHIP);
			break;
		}
	}
	return TRUE;
}
#define SCRLL_NULL		0
#define SCRLL_NORMAL	1
#define SCRLL_ACCEL		2
#define SCRLL_BRAKE		3
#define SCRLL_MANUAL	4

void AVG_SetChipBackScrool( int tx, int ty, int frame, int flag )
{
	if(frame){
		ChipBackStruct.scroll = flag+1;

		ChipBackStruct.sx = ChipBackStruct.wx;
		ChipBackStruct.sy = ChipBackStruct.wy;
		ChipBackStruct.tx = (float)tx;
		ChipBackStruct.ty = (float)ty;
		ChipBackStruct.sc_cnt = 0;
		ChipBackStruct.sc_max = frame;
	}else{
		ChipBackStruct.scroll = SCRLL_NULL;

		ChipBackStruct.wx = (float)tx;
		ChipBackStruct.wy = (float)ty;
		ChipBackStruct.sc_cnt = 0;
		ChipBackStruct.sc_max = 0;
	}
}
void AVG_SetChipBackScrool2( int tx, int ty, int frame, int flag )
{
	if(frame){
		ChipBackStruct.scroll = flag+1;

		ChipBackStruct.sx = ChipBackStruct.wx;
		ChipBackStruct.sy = ChipBackStruct.wy;
		ChipBackStruct.tx = ChipBackStruct.wx+tx;
		ChipBackStruct.ty = ChipBackStruct.wy+ty;
		ChipBackStruct.sc_cnt = 0;
		ChipBackStruct.sc_max = frame;
	}else{
		ChipBackStruct.scroll = SCRLL_NULL;

		ChipBackStruct.wx = ChipBackStruct.wx+tx;
		ChipBackStruct.wy = ChipBackStruct.wy+ty;
		ChipBackStruct.sc_cnt = 0;
		ChipBackStruct.sc_max = 0;
	}
}


void AVG_SetChipBackScroolSpeed( int sx, int sy, int frame )
{
	if(frame){
		ChipBackStruct.scroll = SCRLL_MANUAL;

		ChipBackStruct.sx = ChipBackStruct.px;
		ChipBackStruct.sy = ChipBackStruct.py;
		ChipBackStruct.tx = (float)(sx/100.0);	
		ChipBackStruct.ty = (float)(sy/100.0);	
		ChipBackStruct.sc_cnt = 0;
		ChipBackStruct.sc_max = frame;
	}else{
		if(sx==0 && sy==0)
			ChipBackStruct.scroll = SCRLL_NULL;
		else
			ChipBackStruct.scroll = SCRLL_MANUAL;
		ChipBackStruct.px = (float)(sx/100.0);
		ChipBackStruct.py = (float)(sy/100.0);
		ChipBackStruct.sc_cnt = 0;
		ChipBackStruct.sc_max = 0;
	}
}

BOOL AVG_WaitChipBackScrool(void)
{
	return ChipBackStruct.scroll;
}

BOOL AVG_WaitChipBackScroolSpeed(void)
{
	return !(ChipBackStruct.px==ChipBackStruct.tx && ChipBackStruct.py==ChipBackStruct.ty);
}

void AVG_ControlChipBack( void )
{
	int		i,j,gcnt,gnum,gno;
	float	x,y,w,h, x2;
	int		sc_max = AVG_EffCnt3(ChipBackStruct.sc_max), count;

	if(ChipBackStruct.flag){
		
		switch( ChipBackStruct.scroll ){
			case SCRLL_NULL:
				break;
			case SCRLL_NORMAL:
			case SCRLL_ACCEL:
			case SCRLL_BRAKE:
				if(ChipBackStruct.sc_cnt<sc_max){
					ChipBackStruct.sc_cnt++;
					count = ChipBackStruct.sc_cnt;
					if(ChipBackStruct.scroll==SCRLL_ACCEL) count = count*count/sc_max;
					if(ChipBackStruct.scroll==SCRLL_BRAKE) count = sc_max-(sc_max-count)*(sc_max-count)/sc_max;

					ChipBackStruct.wx = (ChipBackStruct.sx*(sc_max-count) + ChipBackStruct.tx*count)/sc_max;
					ChipBackStruct.wy = (ChipBackStruct.sy*(sc_max-count) + ChipBackStruct.ty*count)/sc_max;

				}else{
					ChipBackStruct.scroll=SCRLL_NULL;
					ChipBackStruct.sc_cnt=0;
					ChipBackStruct.wx = ChipBackStruct.tx;
					ChipBackStruct.wy = ChipBackStruct.ty;
				}
				break;
			case SCRLL_MANUAL:
				if(ChipBackStruct.sc_cnt<sc_max){
					ChipBackStruct.sc_cnt++;
					count = ChipBackStruct.sc_cnt;
					ChipBackStruct.px = (ChipBackStruct.sx*(sc_max-count) + ChipBackStruct.tx*count)/sc_max;
					ChipBackStruct.py = (ChipBackStruct.sy*(sc_max-count) + ChipBackStruct.ty*count)/sc_max;

				}else{
					ChipBackStruct.px = ChipBackStruct.tx;
					ChipBackStruct.py = ChipBackStruct.ty;
					if(ChipBackStruct.px==0 && ChipBackStruct.py==0){
						ChipBackStruct.scroll = SCRLL_NULL;
						ChipBackStruct.sc_cnt = 0;
					}
				}
				ChipBackStruct.wx += ChipBackStruct.px;
				ChipBackStruct.wy += ChipBackStruct.py;
				break;
		}

		
		if(!ChipDrawPose){
			for( i=0; i<GRP_BACK_CHIP ;i++ ){
				DSP_SetGraphDisp( GRP_BACK+i, OFF );
				DSP_SetGraphParam( GRP_BACK+i, DRW_NML );
			}

			for( i=0; i<GRP_BACK_CHIP ;i++ ){
				x = (float)ChipBackParts[i].x-(int)(ChipBackStruct.wx*ChipBackParts[i].depth/100);
				y = (float)ChipBackParts[i].y-(int)ChipBackStruct.wy;
				w = (float)ChipBackParts[i].w;
				h = (float)ChipBackParts[i].h;

				if(ChipBackParts[i].ln){
					if(ChipBackParts[i].lw)
						gnum = min( (DISP_X+ChipBackParts[i].lw-1)/ChipBackParts[i].lw+1, ChipBackParts[i].ln );
					else
						gnum = 1;
					gcnt = 0;
					for( j=0 ; j<ChipBackParts[i].ln ; j++ ){
						x2 = x+j*ChipBackParts[i].lw;
						if( x2+ChipBackParts[i].w>0 && x2<DISP_X ){
							gno = ChipBackParts[i].gno[gcnt];
							DSP_SetGraphDisp( GRP_BACK+gno, ON );
							DSP_SetGraphPos( GRP_BACK+gno, (int)x2, (int)y+CHIPBACK_SY, 0, 0, (int)w, (int)h );

							DSP_SetGraphBright( GRP_BACK+gno, BackStruct.rr, BackStruct.gg, BackStruct.bb );

							if(MainWindow.draw_matrix){
								if(MainWindow.draw_matrix==1)
									DSP_SetGraphParam( GRP_BACK+gno, DRW_BLD(32) );
								else
									DSP_SetGraphParam( GRP_BACK+gno, DRW_BLD(256*MainWindow.draw_matrix/100) );
							}else{
								DSP_SetGraphParam( GRP_BACK+gno, ChipBackParts[i].param );
							}

							gcnt++;
							if(gcnt>=gnum) break;
						}
					}
				}else{
				}
			}
		}
	}
}


static BOOL LoadChipChar( int char_no, int pose, int end, int lnum )
{
	char	fname[256];
	char	ch_no = ChipCharNo[char_no];
	char	*tname,spia=0;
	BOOL	ret=FALSE;

	if( BackStruct.tone_char==-1 ){	
		if( PAC_CheckFile( BMP_GetPackDir(), tone_fname2 ) ){
			tname=tone_fname2;
		}else{
			tname=tone_fname[BackStruct.tone_no%4];
			if(BackStruct.tone_no==TONE_SEPIA) spia=ON;
		}
	}else{
		tname=tone_fname[BackStruct.tone_char%4];
		if(BackStruct.tone_no==TONE_SEPIA) spia=ON;
	}
	wsprintf( fname, "tc%02d0%04d.ani", char_no, pose );
	ret = DSP_LoadSprite( GRP_BACK_CHIP+ch_no, fname, CHK_ANTI, tname, (spia)?0:256 );
	return ret;
}

BOOL AVG_SetChipChar( int char_no, int pose, int dx, int dy, int end, int lnum, int layer )
{

	int		ch_no,i,j;

	AVG_ResetChipChar( char_no );

	for( i=0 ; i<GRP_CHAR_CHIP ; i++ ){
		if( !ChipChar[i].flag ){
			ch_no=i;
			break;
		}
	}
	if(i==GRP_CHAR_CHIP){
		DebugBox(NULL,"チップキャラクターの登録数が多過ぎます。[最大%d]",GRP_CHAR_CHIP);
		return FALSE;
	}
	
	ChipCharNo[char_no] = ch_no;
	i = LoadChipChar( char_no, pose, end, lnum );
	if( !i ) return FALSE;



	ChipChar[ch_no].flag = ON;
	ChipChar[ch_no].act = ACT_NULL;	
	ChipChar[ch_no].tx = ChipChar[ch_no].x = (float)dx;
	ChipChar[ch_no].ty = ChipChar[ch_no].y = (float)dy;
	ChipChar[ch_no].cond = 1;		
	ChipChar[ch_no].cno  = char_no;	

	ChipChar[ch_no].end  = end;		
	ChipChar[ch_no].lnum = lnum;	
	ChipChar[ch_no].smax = 1;

	ChipChar[ch_no].param = DRW_NML;
	ChipChar[ch_no].r = 128;
	ChipChar[ch_no].g = 128;
	ChipChar[ch_no].b = 128;

	ZeroMemory( &ChipChar[ch_no].sound, sizeof(short)*1000*4 );
	FillMemory( &ChipChar[ch_no].svol,  sizeof(char)*1000*4, 0xff );

	for( j=0 ; j<GRP_CHAR_CHIP ; j++ ){
		if(!BackChipFlag[GRP_CHIP+j]){
			BackChipFlag[GRP_CHIP+j] = ON;

			ChipChar[ch_no].gno = GRP_CHIP+j;

			DSP_SetSprite( GRP_CHIP+j, GRP_BACK_CHIP+ch_no, GRP_BACK_CHIP+ch_no, LAY_CHIP+2+layer, ON, end, lnum, OFF );
			DSP_SetGraphClip( GRP_CHIP+j,  0, CHIPBACK_SY, 640, 352 );
			DSP_SetGraphMove( GRP_CHIP+j, dx, dy+CHIPBACK_SY );
			break;
		}
	}
	return TRUE;
}


BOOL AVG_LoadChipChar( int char_no, int pose, int end, int lnum )
{
	char	ch_no = ChipCharNo[char_no];
	BOOL	ret=FALSE;

	if(ch_no==0xff) return ret;
	if( ChipChar[ch_no].flag ){
		ret = LoadChipChar( char_no, pose, end, lnum );
		if(!ret) return ret;
		DSP_SetSpritePlay( GRP_BACK_CHIP+ch_no,GRP_BACK_CHIP+ch_no, end, lnum, OFF );
		ChipChar[ch_no].end  = end;
		ChipChar[ch_no].lnum = lnum;

	}
	return ret;
}

BOOL AVG_LoadChipCharCash( int char_no, int pose )
{
	char	fname[256];
	char	*tname,spia=0;
	BOOL	ret;

	if( BackStruct.tone_char==-1 ){	
		if( PAC_CheckFile( BMP_GetPackDir(), tone_fname2 ) ){
			tname=tone_fname2;
		}else{
			tname=tone_fname[BackStruct.tone_no%4];
			if(BackStruct.tone_no==TONE_SEPIA) spia=ON;
		}
	}else{
		tname=tone_fname[BackStruct.tone_char%4];
		if(BackStruct.tone_no==TONE_SEPIA) spia=ON;
	}

	wsprintf( fname, "tc%02d0%04d.ani", char_no, pose );
	ret = DSP_LoadSpriteCash( fname, tname, (spia)?0:256, -2 );
	return ret;
}

void AVG_ResetChipCharCash( void )
{
	DSP_ResetSpriteCash();
}


void AVG_ResetChipChar( int char_no )
{
	char	ch_no = ChipCharNo[char_no];

	if(ch_no==0xff) return ;
	if( ChipChar[ch_no].flag ){
		ChipCharNo[char_no] = 0xff;
		BackChipFlag[ ChipChar[ch_no].gno ] = OFF;

		DSP_ResetGraph( ChipChar[ch_no].gno );
		DSP_ReleaseSprite( GRP_BACK_CHIP+ch_no );
	}
	ZeroMemory( &ChipChar[ch_no], sizeof(CHIP_CHAR_STRUCT) );
}

void AVG_SetChipCharPos( int char_no, int dx, int dy )
{
	char	ch_no = ChipCharNo[char_no];

	if(ch_no==0xff) return ;
	if( ChipChar[ch_no].flag ){
		ChipChar[ch_no].tx = ChipChar[ch_no].x = (float)dx;
		ChipChar[ch_no].ty = ChipChar[ch_no].y = (float)dy;
	}
}

void AVG_SetChipCharRev( int char_no, int rev )
{
	char	ch_no = ChipCharNo[char_no];

	if(ch_no==0xff) return ;
	if( ChipChar[ch_no].flag ){
		ChipChar[ch_no].rev = rev;
		if(rev)	DSP_SetGraphRevParam( ChipChar[ch_no].gno, REV_W );
		else	DSP_SetGraphRevParam( ChipChar[ch_no].gno, REV_NOT );
	}
}

void AVG_SetChipCharParam( int char_no, DWORD param )
{
	char	ch_no = ChipCharNo[char_no];

	if(ch_no==0xff) return ;
	if( ChipChar[ch_no].flag ){
		ChipChar[ch_no].param = param;
	}
}

void AVG_SetChipCharBright( int char_no, int r, int g, int b )
{
	char	ch_no = ChipCharNo[char_no];

	if(ch_no==0xff) return ;
	if( ChipChar[ch_no].flag ){
		ChipChar[ch_no].r = r;
		ChipChar[ch_no].g = g;
		ChipChar[ch_no].b = b;
	}
}

void AVG_SetChipCharMove( int char_no, int tx, int ty, int frame )
{
	char	ch_no = ChipCharNo[char_no];

	if(ch_no==0xff) return ;
	if( ChipChar[ch_no].flag ){
		if(frame){
			ChipChar[ch_no].sx = ChipChar[ch_no].x;
			ChipChar[ch_no].sy = ChipChar[ch_no].y;
			ChipChar[ch_no].tx = (float)tx;
			ChipChar[ch_no].ty = (float)ty;
			ChipChar[ch_no].count = 0;
			ChipChar[ch_no].frame = frame;
			ChipChar[ch_no].act |= ACT_MOVE;
		}else{
			ChipChar[ch_no].x = ChipChar[ch_no].tx = (float)tx;
			ChipChar[ch_no].y = ChipChar[ch_no].ty = (float)ty;
			ChipChar[ch_no].count = 0;
			ChipChar[ch_no].frame = 0;
			ChipChar[ch_no].act = ChipChar[ch_no].act&(~(ACT_MOVE|ACT_SPEED));
		}
	}
}

void AVG_SetChipCharMove2( int char_no, int mx, int my, int frame )
{
	char	ch_no = ChipCharNo[char_no];

	if(ch_no==0xff) return ;
	if( ChipChar[ch_no].flag ){
		if(frame){
			ChipChar[ch_no].sx = ChipChar[ch_no].x;
			ChipChar[ch_no].sy = ChipChar[ch_no].y;
			ChipChar[ch_no].tx = ChipChar[ch_no].x+mx;
			ChipChar[ch_no].ty = ChipChar[ch_no].y+my;
			ChipChar[ch_no].count = 0;
			ChipChar[ch_no].frame = frame;
			ChipChar[ch_no].act |= ACT_MOVE;
		}else{
			ChipChar[ch_no].x = ChipChar[ch_no].tx = ChipChar[ch_no].x+mx;
			ChipChar[ch_no].y = ChipChar[ch_no].ty = ChipChar[ch_no].y+my;
			ChipChar[ch_no].count = 0;
			ChipChar[ch_no].frame = 0;
			ChipChar[ch_no].act = ChipChar[ch_no].act&(~(ACT_MOVE|ACT_SPEED));
		}
	}
}

void AVG_SetChipCharMoveSpeed( int char_no, int mx, int my, int frame )
{
	char	ch_no = ChipCharNo[char_no];

	if(ch_no==0xff) return ;
	if( ChipChar[ch_no].flag ){
		if(frame){
			ChipChar[ch_no].sx = ChipChar[ch_no].px;
			ChipChar[ch_no].sy = ChipChar[ch_no].py;
			ChipChar[ch_no].tx = (float)(mx/100.0);
			ChipChar[ch_no].ty = (float)(my/100.0);
			ChipChar[ch_no].count = 0;
			ChipChar[ch_no].frame = frame;
			ChipChar[ch_no].act |= ACT_SPEED;
		}else{
			ChipChar[ch_no].px = (float)(mx/100.0);
			ChipChar[ch_no].py = (float)(my/100.0);
			ChipChar[ch_no].count = 0;
			ChipChar[ch_no].frame = 0;
			if(mx==0 && my==0)
				ChipChar[ch_no].act = ChipChar[ch_no].act&(~(ACT_MOVE|ACT_SPEED));
			else
				ChipChar[ch_no].act |= ACT_SPEED;
		}
	}
}

void AVG_GetChipCharMove( int char_no, long *tx, long *ty )
{
	char	ch_no = ChipCharNo[char_no];

	if(ch_no==0xff) return ;
	if( ChipChar[ch_no].flag ){
		*tx = (long)ChipChar[ch_no].x;
		*ty = (long)ChipChar[ch_no].y;
	}
}

void AVG_GetChipCharMoveSpeed( int char_no, long *mx, long *my )
{
	char	ch_no = ChipCharNo[char_no];

	if(ch_no==0xff) return ;
	if( ChipChar[ch_no].flag ){
		*mx = (long)ChipChar[ch_no].px;
		*my = (long)ChipChar[ch_no].py;
	}
}

void AVG_CopyChipCharPos( int char_no1, int char_no2 )
{
	char	ch_no1 = ChipCharNo[char_no1];
	char	ch_no2 = ChipCharNo[char_no2];

	if(ch_no1==0xff) return ;
	if(ch_no2==0xff) return ;
	ChipChar[ch_no1].x = ChipChar[ch_no1].tx = ChipChar[ch_no2].x;
	ChipChar[ch_no1].y = ChipChar[ch_no1].ty = ChipChar[ch_no2].y;
	ChipChar[ch_no1].count = 0;
	ChipChar[ch_no1].frame = 0;
}

void AVG_ThroughChipCharAni( int char_no )
{
	char	ch_no = ChipCharNo[char_no];
	if(ch_no==0xff) return ;
	DSP_ThroughSpriteLoop( GRP_BACK_CHIP+ch_no );
}


void AVG_ThroughChipCharAniLoop( int char_no )
{
	char	ch_no = ChipCharNo[char_no];
	if(ch_no==0xff) return ;
	if( ChipChar[ch_no].flag ){
		ChipChar[ch_no].end  = ON;
		ChipChar[ch_no].lnum = 1;
		DSP_SetSpriteLoop( GRP_BACK_CHIP+ch_no, ON, 1 );
	}
}

void AVG_SetSpriteRepeatFind( int char_no )
{
	char	ch_no = ChipCharNo[char_no];
	if(ch_no==0xff) return ;
	if( ChipChar[ch_no].flag ){
		DSP_SetSpriteRepeatFind( GRP_BACK_CHIP+ch_no );
	}
}

BOOL AVG_WaitChipCharRepeat( int char_no )
{
	char	ch_no = ChipCharNo[char_no];

	if(ch_no==0xff) return FALSE;
	if( ChipChar[ch_no].flag ){
		return DSP_GetSpriteRepeatFind( GRP_BACK_CHIP+ch_no )!=2;
	}
	return FALSE;
}

BOOL AVG_WaitChipCharAni( int char_no )
{
	char	ch_no = ChipCharNo[char_no];
	if(ch_no==0xff) return FALSE;
	if( ChipChar[ch_no].flag ){
		if(ChipChar[ch_no].lnum==0){
			ChipChar[ch_no].lnum = 1;
			DSP_SetSpriteLoop( GRP_BACK_CHIP+ch_no, ChipChar[ch_no].end, 1 );
		}
		return DSP_WaitSpriteCondition( GRP_BACK_CHIP+ch_no );
	}
	return FALSE;
}

BOOL AVG_WaitChipCharMove( int char_no )
{
	char	ch_no = ChipCharNo[char_no];

	if(ch_no==0xff) return FALSE;
	if( ChipChar[ch_no].flag ){
		return ChipChar[ch_no].act&(ACT_MOVE|ACT_SPEED);
	}
	return 0;
}

void AVG_ControlChipChar( void )
{
	int		i;
	int		x,y;
	int		se;

	if(ChipBackStruct.flag){
		if(!ChipDrawPose){
			for( i=0; i<GRP_CHAR_CHIP ;i++ ){
				if( ChipChar[i].flag ){
					ChipChar[i].cond = DSP_WaitSpriteCondition( GRP_BACK_CHIP+i );

					if( ChipChar[i].act&ACT_MOVE ){
						int	cmax = AVG_EffCnt3( ChipChar[i].frame );
						int	cnt  = ChipChar[i].count;

						if( cmax > cnt ){
							ChipChar[i].x = (ChipChar[i].sx*(cmax-cnt) + ChipChar[i].tx*cnt)/cmax;
							ChipChar[i].y = (ChipChar[i].sy*(cmax-cnt) + ChipChar[i].ty*cnt)/cmax;
							ChipChar[i].count++;
						}else{
							ChipChar[i].x = ChipChar[i].tx;
							ChipChar[i].y = ChipChar[i].ty;
							ChipChar[i].frame = 0;
							ChipChar[i].act = ChipChar[i].act&(~ACT_MOVE);
						}
					}
					if( ChipChar[i].act&ACT_SPEED ){
						int	cmax = AVG_EffCnt3( ChipChar[i].frame );
						int	cnt  = ChipChar[i].count;

						if( cmax > cnt ){
							ChipChar[i].px = (ChipChar[i].sx*(cmax-cnt) + ChipChar[i].tx*cnt)/cmax;
							ChipChar[i].py = (ChipChar[i].sy*(cmax-cnt) + ChipChar[i].ty*cnt)/cmax;
							ChipChar[i].count++;
						}else{
							ChipChar[i].px = ChipChar[i].tx;
							ChipChar[i].py = ChipChar[i].ty;
							ChipChar[i].frame = 0;
							if(ChipChar[i].px==0 && ChipChar[i].py==0){
								ChipChar[i].act = ChipChar[i].act&(~ACT_SPEED);
								ChipChar[i].count=0;
							}

						}
						ChipChar[i].x += ChipChar[i].px;
						ChipChar[i].y += ChipChar[i].py;
					}

					x = (int)(ChipChar[i].x-ChipBackStruct.wx);
					y = (int)(ChipChar[i].y-ChipBackStruct.wy+CHIPBACK_SY);

					DSP_SetGraphMove(  ChipChar[i].gno, x, y );
					DSP_SetGraphParam( ChipChar[i].gno, ChipChar[i].param );
					{
						int	r,g,b;
						if( BackStruct.rr<128 )	r = ChipChar[i].r*BackStruct.rr/128;
						else					r = (0xff-ChipChar[i].r)*(BackStruct.rr-128) / 128 + ChipChar[i].r;
						if( BackStruct.gg<128 )	g = ChipChar[i].g*BackStruct.gg/128;
						else					g = (0xff-ChipChar[i].g)*(BackStruct.gg-128) / 128 + ChipChar[i].g;
						if( BackStruct.bb<128 )	b = ChipChar[i].b*BackStruct.bb/128;
						else					b = (0xff-ChipChar[i].b)*(BackStruct.bb-128) / 128 + ChipChar[i].b;
						DSP_SetGraphBright( ChipChar[i].gno, r, g, b );
					}

					if(1){



					}
					
					se = DSP_GetSpriteEvent(GRP_BACK_CHIP+i);
					if( se && ChipChar[i].smax ){
						int	sx,sy;
						if( ChipChar[i].sflag ){
							ChipChar[i].scnt = rand()%ChipChar[i].smax;
						}else{
							ChipChar[i].scnt = (ChipChar[i].scnt+1)%ChipChar[i].smax;
						}
						switch( ChipChar[i].sound[se][ChipChar[i].scnt] ){
							case 0:		break;		
							case -1:	continue;	
							default: se = ChipChar[i].sound[se][ChipChar[i].scnt]; break;	
						}
						DSP_GetGraphBmpSize( ChipChar[i].gno, &sx, &sy );
						AVG_PlaySePan( se, OFF, ChipChar[i].svol[se][ChipChar[i].scnt], x+sx/2-320 );
					}
				}
			}
		}
	}
}
void AVG_SetSoundEventVolume( int char_no, int event, int sno, int vol )
{
	char	ch_no = ChipCharNo[char_no];
	if(sno!=-1){
		ChipChar[ch_no].svol[event][sno] = vol;
	}else{
		ChipChar[ch_no].svol[event][0] = vol;
		ChipChar[ch_no].svol[event][1] = vol;
		ChipChar[ch_no].svol[event][2] = vol;
		ChipChar[ch_no].svol[event][3] = vol;
	}
}

void AVG_SetSoundEvent( int char_no, int event, int sno1, int sno2, int sno3, int sno4, int flag )
{
	char	ch_no = ChipCharNo[char_no];
	int		i;

	if(ch_no==0xff) return ;
	if( ChipChar[ch_no].flag ){
		ChipChar[ch_no].sound[event][0] = sno1;
		ChipChar[ch_no].sound[event][1] = sno2;
		ChipChar[ch_no].sound[event][2] = sno3;
		ChipChar[ch_no].sound[event][3] = sno4;
		ChipChar[ch_no].sflag = flag;
		ChipChar[ch_no].scnt  = 0;

		for(i=0;i<4;i++){
			if( ChipChar[ch_no].sound[event][i]==-1 ){
				break;
			}
		}
		ChipChar[ch_no].smax  = i;
	}
}





BOOL AVG_SetPotaPota( int blood, int amount, int frame )
{

	int			i;
	BOOL		ret=FALSE;
	static int	step=0;
	static int	count=0,count2=0;

	amount+=2;
	if(amount<=0) amount=1;
	frame-=2;

	switch( step ){
		default:
		case 0:

			for( i=0 ; i<GRP_BACK_CHIP ; i++ )
				DSP_SetGraphDisp( GRP_BACK+i, OFF );
			for( i=0; i<GRP_CHAR_CHIP ;i++ ){
				if( ChipChar[i].flag )
					DSP_SetGraphDisp( ChipChar[i].gno, OFF );
			}

			DSP_LoadBmp( BMP_WEATHER+0, BMP_256P, "e1000.bmp" );
			DSP_LoadBmp( BMP_WEATHER+1, BMP_256P, "e1001.bmp" );
			DSP_LoadBmp( BMP_WEATHER+2, BMP_256P, "e1002.bmp" );

			DSP_LoadBmp( BMP_WEATHER+3, BMP_256P, "e1010.bmp" );
			DSP_LoadBmp( BMP_WEATHER+4, BMP_256P, "e1011.bmp" );
			DSP_LoadBmp( BMP_WEATHER+5, BMP_256P, "e1012.bmp" );

			DSP_LoadBmp( BMP_WEATHER+6, BMP_256P, "e1020.bmp" );
			DSP_LoadBmp( BMP_WEATHER+7, BMP_256P, "e1021.bmp" );
			DSP_LoadBmp( BMP_WEATHER+8, BMP_256P, "e1022.bmp" );

			MainWindow.draw_flag = ON;
			step=1;
			count=0;
			count2=0;
			break;
		case 1:

			step=2;
			
		case 2:

			if( 1){
				int	width[3]={100,248,400};
				int	sx=0,sy=0;
				
				
				i = rand()%((blood+1)*3);

				DSP_SetGraph( GRP_WEATHER+count, BMP_WEATHER+i, 1, ON, CHK_ANTI );
				DSP_GetGraphBmpSize( GRP_WEATHER+count, &sx, &sy );
				DSP_SetGraphPos( GRP_WEATHER+count, rand()%800-sx/6, rand()%600-sy/2, 0,0,sx/3,sy );
				DSP_SetGraphParam( GRP_WEATHER+count, DRW_NML );
				DSP_SetGraphBright( GRP_WEATHER+count, 128, 0,0 );


				DSP_GetGraphBmpSize( GRP_WEATHER+(count-1+100)%100, &sx, &sy );
				DSP_SetGraphSMove( GRP_WEATHER+(count-1+100)%100, sx/3, 0 );

				DSP_GetGraphBmpSize( GRP_WEATHER+(count-2+100)%100, &sx, &sy );
				DSP_SetGraphSMove( GRP_WEATHER+(count-2+100)%100, sx/3*2, 0 );

				count = (count+1)%100;
			}
			count2++;
			if(count2>frame) step=3;
			break;
		case 3:
			for(i=0;i<9;i++)	DSP_ReleaseBmp( BMP_WEATHER+i );
			for(i=0;i<100;i++)	DSP_ResetGraph( GRP_WEATHER+i );

			step  = 0;
			count = 0;
			count2= 0;

			ret=TRUE;
			break;
	}

	return ret;
}
