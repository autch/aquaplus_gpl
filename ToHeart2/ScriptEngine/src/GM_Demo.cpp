
#include "mm_std.h"
#include "main.h"

#include "bmp.h"
#include "draw.h"
#include "drawprim.h"
#include "math.h"
#include "text.h"
#include "disp.h"

#include "GM_demo.h"
#include "GM_avg.h"

#include "font.h"

#include "dispSprite.h"

enum{
	DEMO_INIT = 0,
	DEMO_LOAD,
	DEMO_MAIN,
	DEMO_TERM,
};

static int		DemoStep[5]    ={ 0, 0, 0, 0, 0 };
static int		NextDemoStep[5]={ 0, 0, 0, 0, 0 };
static int		DemoCounter[5] ={ 0, 0, 0, 0, 0 };
static int		DemoCounter2[5] ={ 0, 0, 0, 0, 0 };





static void DEM_ChangeSetp( int index, int step )
{
	NextDemoStep[index] = step;
}

static void DEM_RenewSetp( void )
{
	int	index, j;

	for( index=0; index<5 ;index++ ){
		if( DemoStep[index] != NextDemoStep[index] ){
			for( j=index; j<5 ;j++ ){
				DemoCounter[j] = 0;
				DemoCounter2[j]= 0;
				if(j!=index)
					DemoStep[j] = NextDemoStep[j]=0;
			}
			DemoStep[index] = NextDemoStep[index];
			break;
		}else{
			DemoCounter[index]++;
			DemoCounter2[index]++;

		}
	}
}

void DEM_Init( void )
{
	MAIN_SetScriptFlag( OFF );	



}

void DEM_Load( void )
{
	int		bmp_bpp = BPP(MainWindow.draw_mode2);
	int		anti_bpp = (MainWindow.draw_mode2==16)? BMP_HIGH: BMP_TRUE;

	DSP_LoadBmp( 0, bmp_bpp, "v00020.bmp" );

DSP_LoadBmp( 10, BMP_256P, "smple0000.bmp" );
DSP_LoadBmp( 11, BMP_256P, "smple0001.bmp" );
DSP_LoadBmp( 12, BMP_256P, "smple0002.bmp" );
DSP_LoadBmp( 13, BMP_256P, "smple0003.bmp" );



	FNT_LoadFont( 28, 0 );




	FrameFormat = 1;
}

void DEM_Term( void )
{
}

void ChangeSetpCounter( int index, int step, int counter )
{
	if( DemoCounter[index] >= counter ){
		DEM_ChangeSetp( index, step );
	}
}

#include "movie.h"
#include "keybord.h"
#include "mouse.h"



#include <soundDS.h>


void SetRipple( int go )
{
	short	*depth = DRW_GetRippleTableAddr();
	int		x,y;
	static int	wsctr = 0;

	if(wsctr<100){
		if(wsctr%10==0){
			for( y = -10; y<=10; y++ ){
if( (DRW_GetRippleTableNwf()+y+150)%2 ){
				for( x = -10; x<=10; x++ ){
					if( sqrt( (double)(x*x+y*y) ) <80 ){

					}
				}
}
			}
		}
	}

		if(wsctr%256==8){
			for( y = -10; y<=10; y++ ){
				for( x = -10; x<=10; x++ ){
					if( sqrt( (double)(x*x+y*y) ) < 4 ){
						*(depth+(100+y)*400+150+x) = -LIM(256-(x*x+y*y),0,256);
					}
				}
			}
		}
		if(wsctr%256==16){
			for( y = -10; y<=10; y++ ){
				for( x = -10; x<=10; x++ ){
					if( sqrt( (double)(x*x+y*y) ) < 6 ){
						*(depth+(160+y)*400+300+x) = -LIM(256-(x*x+y*y),0,256);
					}
				}
			}
		}
		if(wsctr%256==20){
			for( y = -32; y<=32; y++ ){
				for( x = -32; x<=32; x++ ){
	
	
	
	
				}
			}
		}

		wsctr++;
}



static int	NetCharX[100];
static int	NetCharY[100];
static int	NetCharSPD[100];

int	RippleGno = 1;
int	RippleCount[200];
int	RippleMax[200];
int	RippleX[200];
int	RippleY[200];



void DEM_Ripple( int x, int y, int rate, int bld, int angle )
{
	DSP_SetGraphPrim( RippleGno, PRM_FLAT, POL_RPLE, 1, ON );
	DSP_SetGraphBright( RippleGno, 0,0,32 );
	DSP_SetGraphMove( RippleGno, x,y );
	DSP_SetGraphParam( RippleGno, DRW_PRM4( bld, rate, 255,angle) );
RippleGno++;
}
BOOL DEM_SystemMain(void )
{
	int		bmp_bpp = BPP(MainWindow.draw_mode2);
	BOOL	ret = TRUE;
	int		dc = (0x1000-DemoCounter[0])%96-16;
	int		r2 = STD_LimitLoop( GlobalCount2*4, 255 );

	int		r = timeGetTime();
	int		lr = STD_LimitLoop( r/8, 255 );
	int		x = r%DISP_X;
	int		y = r%DISP_Y;
	int		i=0,j=0;
	static int	flag = 0;
	char		*str = "<Aアクセント>が出せます。\n\\k<s6一次停止、<w30><s4スピード変更>とか>\\k表示中の演出挿入";
	static int	mh;
	int			Angle = 0;
	static		an=0;
	int			count = DemoCounter[1]*4;
	int			mx,my;
	MUS_GetMousePos( &mx, &my );










	MainWindow.draw_flag=1;

	Avg.frame=30;

	static int	wavw_cnt=0;
	AVG_GetGameKey();
	if(GameKey.click){


		wavw_cnt=0;
	}
	if(GameKey.cansel){
		ChangeSetpCounter( 1, (DemoStep[1]+1)%2, 0 );
	}


	
	if(KeyCond.trg.f1){
	}
	if(KeyCond.trg.f2){
	}
	if(KeyCond.trg.f3){
	}
	if(KeyCond.trg.f4){
	}



	r = SIN( timeGetTime()/10%256 )/2 ;







	switch( DemoStep[1] ){
		default:
		case 0:
			DSP_SetGraphPrim( 0, PRM_FLAT, POL_RECT, 0, ON );
			DSP_SetGraphPosRect( 0, 0,0,800,600 );
			DSP_SetGraphBright( 0, 255,128,50 );

			r = DemoCounter[0]*2%256;

			DSP_SetGraph( 1, 10 + (DemoCounter[0]*2/256+0)%4, 1, ON, CHK_NO );
			DSP_SetGraph( 2, 10 + (DemoCounter[0]*2/256+1)%4, 1, ON, CHK_NO );
			DSP_SetGraph( 3, 10 + (DemoCounter[0]*2/256+2)%4, 1, ON, CHK_NO );
			DSP_SetGraph( 4, 10 + (DemoCounter[0]*2/256+3)%4, 1, !!r, CHK_NO );


			DSP_SetGraphFade( 1, 64-r/8 );
			DSP_SetGraphPosPoly( 1, -r,    -r, 200-r*200/256, 150-r*150/256,
									-r, 600+r, 200-r*200/256, 150+300+r*150/256,
									0, 0, 400,   0, 0, 300, 400,     300 );

			DSP_SetGraphFade( 2, 128-r/4 );
			DSP_SetGraphPosPoly( 2, 200-r*200/256,     150-r*150/256, 200+400-r*400/256, 150,
									200-r*200/256, 300+150+r*150/256, 400+200-r*400/256, 150+300,
									0, 0, 400,   0, 0, 300, 400,     300 );

			DSP_SetGraphFade( 3, 64+r/4 );
			DSP_SetGraphPosPoly( 3, 400+200-r*400/256,     150, 800-r*200/256, 0+r*150/256,
									400+200-r*400/256, 300+150, 800-r*200/256, 600-r*150/256,
									0, 0, 400,   0, 0, 300, 400,     300 );

			DSP_SetGraphFade( 4, 32+r/8 );
			DSP_SetGraphPosPoly( 4, 800-r*200/256,     r*150/256, 800+256-r,    -256+r,
									800-r*200/256, 600-r*150/256, 800+256-r, 600+256-r,
									0, 0, 400,   0, 0, 300, 400,     300 );

			break;
		case 1:
			DSP_SetGraph( 0, 0, 0, ON, CHK_NO );
			DSP_ResetGraph( 1);
			DSP_ResetGraph( 2);
			DSP_ResetGraph( 3);
			DSP_ResetGraph( 4);
			if(0){
				long	xxx[36];
				long	yyy[36];
				long	xx[6];
				long	yy[6];

				if(wavw_cnt<256) wavw_cnt+=4;
				r = COS( wavw_cnt )*1536/4096;
				r = r/32;

				xxx[0*6+0] = r*3;	yyy[0*6+0] = r*3;
				xxx[0*6+1] = r*2;	yyy[0*6+1] = r*2;
				xxx[0*6+2] = r;		yyy[0*6+2] = r;
				xxx[0*6+3] = -r;	yyy[0*6+3] = r;
				xxx[0*6+4] = -r*2;	yyy[0*6+4] = r*2;
				xxx[0*6+5] = -r*3;	yyy[0*6+5] = r*3;

				xxx[1*6+0] = r*2;	yyy[1*6+0] = r*2;
				xxx[1*6+1] = r;		yyy[1*6+1] = r;
				xxx[1*6+2] = r/2;	yyy[1*6+2] = r/2;
				xxx[1*6+3] = -r/2;	yyy[1*6+3] = r/2;
				xxx[1*6+4] = -r;	yyy[1*6+4] = r;
				xxx[1*6+5] = -r*2;	yyy[1*6+5] = r*2;

				xxx[2*6+0] = r;		yyy[2*6+0] = r;
				xxx[2*6+1] = r/2;	yyy[2*6+1] = r/2;
				xxx[2*6+2] = -r;	yyy[2*6+2] = -r;
				xxx[2*6+3] = r;		yyy[2*6+3] = -r;
				xxx[2*6+4] = -r/2;	yyy[2*6+4] = r/2;
				xxx[2*6+5] = -r;	yyy[2*6+5] = r;

				xxx[3*6+0] = r;		yyy[3*6+0] = -r;
				xxx[3*6+1] = r/2;	yyy[3*6+1] = -r/2;
				xxx[3*6+2] = -r;	yyy[3*6+2] = r;
				xxx[3*6+3] = r;		yyy[3*6+3] = r;
				xxx[3*6+4] = -r/2;	yyy[3*6+4] = -r/2;
				xxx[3*6+5] = -r;	yyy[3*6+5] = -r;

				xxx[4*6+0] = r*2;	yyy[4*6+0] = -r*2;
				xxx[4*6+1] = r;		yyy[4*6+1] = -r;
				xxx[4*6+2] = r/2;	yyy[4*6+2] = -r/2;
				xxx[4*6+3] = -r/2;	yyy[4*6+3] = -r/2;
				xxx[4*6+4] = -r;	yyy[4*6+4] = -r;
				xxx[4*6+5] = -r*2;	yyy[4*6+5] = -r*2;

				xxx[5*6+0] = r*3;	yyy[5*6+0] = -r*3;
				xxx[5*6+1] = r*2;	yyy[5*6+1] = -r*2;
				xxx[5*6+2] = r*1;	yyy[5*6+2] = -r*1;
				xxx[5*6+3] = -r*1;	yyy[5*6+3] = -r*1;
				xxx[5*6+4] = -r*2;	yyy[5*6+4] = -r*2;
				xxx[5*6+5] = -r*3;	yyy[5*6+5] = -r*3;








				x=mx;
				y=my;
				xx[0] = -128;
				xx[1] = 128+x*1/5-128;
				xx[2] = 128+x*4/5-128;
				xx[3] = 128+x+(800-x)/5-128;
				xx[4] = 128+x+(800-x)*4/5-128;
				xx[5] = 800+256-128;

				yy[0] = -96;
				yy[1] = 96+y*1/5-96;
				yy[2] = 96+y*4/5-96;
				yy[3] = 96+y+(600-y)/5-96;
				yy[4] = 96+y+(600-y)*4/5-96;
				yy[5] = 600+192-96;

				for(i=0;i<5;i++){
					for(j=0;j<5;j++){
						DSP_SetGraph( i*5+j, 1, 0, ON, CHK_NO );
						DSP_SetGraphPosPoly( i*5+j,
							xx[j]+xxx[ i   *6+j],   yy[i  ]+yyy[ i   *6+j],   xx[j+1]+xxx[ i   *6+j+1], yy[i  ]+yyy[ i   *6+j+1],
							xx[j]+xxx[(i+1)*6+j],   yy[i+1]+yyy[(i+1)*6+j],   xx[j+1]+xxx[(i+1)*6+j+1], yy[i+1]+yyy[(i+1)*6+j+1],
							xx[j]+128, yy[i]+96,   xx[j+1]+128, yy[i]+96,   xx[j]+128, yy[i+1]+96,   xx[j+1]+128, yy[i+1]+96 );
					}
				}
			}
			break;
		case 2:
			for( i=0 ; i<RippleGno ; i++ ){
				DSP_ResetGraph( i );
			}




				SetRipple( (DrawCount)%256 );
				DRW_RenewRippleTable(1);




			DSP_SetGraph( 0, 0, 1, ON, CHK_NO );
			DSP_SetGraphPos( 0, 0,0, 0,0, 800, 600 );
			DSP_SetGraphParam( 0, DRW_RP2( 255-LIM(r/10%512*4/3,0,255), r/20%256 ) );
			DSP_SetGraphFade( 0, 128-LIM(DrawCount%256-128,0,128) );
			break;
		case 3:
			DSP_SetGraph( 0, 0, 0, ON, CHK_NO );
			DSP_SetGraph( 1, 1, 1, ON, CHK_NO );
			DSP_SetGraphBSet2( 1, 2, 3, lr*lr/255 );
			break;
		case 4:
			DSP_SetTextStr( 0, "モザイクワイプ" );

			DSP_SetGraph( 0, 0, 0, ON, CHK_NO );
			DSP_SetGraphDisp( 1, OFF );
			DSP_SetGraphParam( 0, DRW_MOZ(count*2) );

			ChangeSetpCounter( 1, 5, 64 );
			break;
		case 5:
			DSP_SetGraph( 0, 1, 0, ON, CHK_NO );
			DSP_SetGraphDisp( 1, OFF );
			DSP_SetGraphParam( 0, DRW_MOZ(128-count*2) );

			ChangeSetpCounter( 1, 6, 64 );
			break;
		case 6:
			DSP_SetTextStr( 0, "モザイクフェード" );

			DSP_SetGraph( 0, 0, 1, ON, CHK_NO );
			DSP_SetGraph( 1, 1, 0, ON, CHK_NO );
			DSP_SetGraphParam( 0, DRW_BLD(count*4) );
			DSP_SetGraphParam( 1, DRW_MOZ(count*4) );

			ChangeSetpCounter( 1, 7, 64 );
			break;
		case 7:
			DSP_SetTextStr( 0, "パターンフェード１" );

			DSP_SetGraph( 0, 0, 0, ON, CHK_NO );
			DSP_SetGraph( 1, 1, 1, ON, CHK_NO );
			DSP_SetGraphBSet( 1, 2, count*4 );

			ChangeSetpCounter( 1, 8, 64 );
			break;
		case 8:
			DSP_SetTextStr( 0, "パターンフェード２" );

			DSP_SetGraph( 0, 0, 1, ON, CHK_NO );
			DSP_SetGraph( 1, 1, 0, ON, CHK_NO );
			DSP_SetGraphBSet( 0, 3, count*4 );

			ChangeSetpCounter( 1, 9, 64 );
			break;
		case 9:
			DSP_SetTextStr( 0, "パターンフェード３" );

			DSP_SetGraph( 0, 0, 0, ON, CHK_NO );
			DSP_SetGraph( 1, 1, 1, ON, CHK_NO );
			DSP_SetGraphBSet( 1, 4, count*4 );

			ChangeSetpCounter( 1, 10, 64 );
			break;
		case 10:
			DSP_SetTextStr( 0, "パターンフェード４（放射）" );

			DSP_SetGraph( 0, 0, 1, ON, CHK_NO );
			DSP_SetGraph( 1, 1, 0, ON, CHK_NO );
			DSP_SetGraphBSet( 0, 5, count*4 );

			ChangeSetpCounter( 1, 11, 64 );
			break;
		case 11:
			DSP_SetTextStr( 0, "パターンフェード５（文字）" );

			DSP_SetGraph( 0, 0, 0, ON, CHK_NO );
			DSP_SetGraph( 1, 1, 1, ON, CHK_NO );
			DSP_SetGraphBSet( 1, 6, count*4 );

			ChangeSetpCounter( 1, 12, 64 );
			break;

		case 12:
			DSP_SetTextStr( 0, "スクロールワイプ（横）" );

			DSP_SetGraph( 0, 0, 0, ON, CHK_NO );
			DSP_SetGraph( 1, 1, 1, ON, CHK_NO );


			r = count*count/64;
			x = 800*r/64;
			DSP_SetGraphMove( 0, x-800, 0 );
			DSP_SetGraphMove( 1, x, 0 );

			DSP_SetGraphParam( 0, DRW_BLD(32+r*2) );
			DSP_SetGraphParam( 1, DRW_BLD(32) );

			ChangeSetpCounter( 1, 13, 64 );
			break;

		case 13:
			DSP_SetTextStr( 0, "スクロールワイプ（縦）" );

			DSP_SetGraph( 0, 0, 0, ON, CHK_NO );
			DSP_SetGraph( 1, 1, 1, ON, CHK_NO );

			r = count*count/64;
			y = -600*r/64;
			DSP_SetGraphMove( 0, 0, y );
			DSP_SetGraphPosRect( 1, 0, y+600, 800, 600 );

			DSP_SetGraphParam( 0, DRW_BLD(32) );
			DSP_SetGraphParam( 1, DRW_BLD(32+r*2) );

			ChangeSetpCounter( 1, 14, 64 );
			break;

		case 14:
			DSP_SetTextStr( 0, "ズームワイプ１" );

			DSP_SetGraph( 0, 0, 1, ON, CHK_NO );
			DSP_SetGraph( 1, 1, 0, ON, CHK_NO );

			r = 64-(64-count)*(64-count)/64;
			DSP_SetGraphZoom2( 1, 400, 300, r*4 );
			DSP_SetGraphParam( 0, DRW_BLD(r*4) );

			ChangeSetpCounter( 1, 15, 64 );
			break;
		case 15:
			DSP_SetTextStr( 0, "ズームワイプ２" );

			DSP_SetGraphDisp( 0, OFF );
			DSP_SetGraph( 1, 1, 0, ON, CHK_NO );

			r = 64-count*count/64;
			DSP_SetGraphZoom2( 1, 400, 300, r*4 );
			DSP_SetGraphParam( 1, DRW_BLD(16+128-r*2) );

			ChangeSetpCounter( 1, 16, 64 );
			break;
		case 16:
			DSP_SetTextStr( 0, "ズームワイプ３" );

			DSP_SetGraph( 0, 0, 0, ON, CHK_NO );
			DSP_SetGraph( 1, 1, 1, ON, CHK_NO );

			r = 64-count;
			r = r*r/64;
			DSP_SetGraphPos( 1, 0, 0, 0,0, 800, 600 );
			DSP_SetGraphZoom2( 1, 400, 300, r*4-256 );
			DSP_SetGraphParam( 1, DRW_BLD(r*4) );

			ChangeSetpCounter( 1, 17, 64 );
			break;
		case 17:
			DSP_SetTextStr( 0, "ズームワイプ４" );

			DSP_SetGraphDisp( 0, OFF );
			DSP_SetGraph( 1, 1, 1, ON, CHK_NO );

			r = 64-count;
			r = 64-r*r/64;
			DSP_SetGraphPos( 1, 0, 0, 0,0, 800, 600 );
			DSP_SetGraphZoom2( 1, 400, 300, r*4-256 );
			DSP_SetGraphParam( 1, DRW_BLD(r*2) );

			ChangeSetpCounter( 1, 18, 64 );
			break;
		case 18:
			DSP_SetTextStr( 0, "ノイズワイプ" );

			DSP_SetGraph( 0, 0, 1, ON, CHK_NO );
			DSP_SetGraph( 1, 1, 0, ON, CHK_NO );

			r = count*2;
			DSP_SetGraphParam( 0, DRW_NIS(r) );

			ChangeSetpCounter( 1, 19, 128 );
			break;
		case 19:
			DSP_SetTextStr( 0, "加算パターンホワイトフェード" );

			DSP_SetGraph( 0, 0, 0, ON, CHK_NO );
			DSP_SetGraph( 1, 5, 1, ON, CHK_NO );

			r = STD_LimitLoop( count, 64 )*4;
			DSP_SetGraphParam( 1, DRW_ADD );
			DSP_SetGraphFade( 1, r );

			ChangeSetpCounter( 1, 20, 128 );
			break;
		case 20:
			DSP_SetTextStr( 0, "減算パターンフェード" );

			DSP_SetGraph( 0, 0, 0, ON, CHK_NO );
			DSP_SetGraph( 1, 5, 1, ON, CHK_NO );

			r = STD_LimitLoop( count, 64 )*4;
			DSP_SetGraphParam( 1, DRW_SUB );
			DSP_SetGraphFade( 1, r );

			ChangeSetpCounter( 1, 0, 128 );
			break;
	}
	AVG_ControlWeather();
	return ret;
}

void DEM_DrawMain( void *dest )
{

}

int	DEM_System( void )
{
	switch( DemoStep[0] ){
		default:
		case DEMO_INIT:
			DEM_Init( );
			DEM_ChangeSetp( 0, DEMO_LOAD );
			break;
		case DEMO_LOAD:
			DEM_Load( );
			DEM_ChangeSetp( 0, DEMO_MAIN );
			break;
		case DEMO_MAIN:
			if( !DEM_SystemMain() )
				DEM_ChangeSetp( 0, DEMO_TERM );
			break;
		case DEMO_TERM:
			DEM_Term( );
			break;
	}
	DEM_RenewSetp();

	return GAME_DEMO;
}

void DEM_Draw( void *dest )
{
	switch( DemoStep[0] ){
		default:
		case DEMO_INIT:
			break;
		case DEMO_LOAD:
			break;
		case DEMO_MAIN:
			DEM_DrawMain( dest );
			break;
		case DEMO_TERM:
			break;
	}
}
