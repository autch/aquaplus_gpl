
#include "mm_std.h"
#include "main.h"

#include "bmp.h"
#include "draw.h"
#include "Escript.h"
#include "disp.h"
#include "text.h"
#include "keybord.h"
#include "mouse.h"



#include "main.h"
#include "GM_avg.h"
#include "GM_avgBack.h"
#include "GM_Save.h"
#include "GM_title.h"

typedef struct{
	int		flag;
	int		no;
	int		cut;
	int		count;
	int		step;
	DWORD	start;
	DWORD	time1;
	DWORD	time2;
	DWORD	time3;
	DWORD	end;
}ENDING_STRUCT;

static ENDING_STRUCT	Ending;


#define	_GFLG_NORMAL_END			60
#define	_GFLG_BAD_END				61
#define	_GFLG_TRUE_END				62



void AVG_SetEnding( int eno, DWORD time )
{
	int		bmp_bpp = BPP(MainWindow.draw_mode2);




	ZeroMemory( &Ending, sizeof(Ending) );
	Ending.flag  = ON;
	Ending.no    = eno;

	if(time==0){
		time=timeGetTime();
	}

	Ending.cut = AVG_GetGameFlag(eno);
	AVG_SetGameFlag(eno, ON);
	switch(eno){
		default:
		case _GFLG_NORMAL_END:
			Ending.end   = time+130000;	
			break;
		case _GFLG_BAD_END:
			Ending.end   = time+167000;	
			break;
		case _GFLG_TRUE_END:
			Ending.end   = time+200000;	
			break;
	}
	Ending.step  = 0;
	Ending.count = 0;
}

void AVG_ResetEnding( void )
{
	ZeroMemory( &Ending, sizeof(Ending) );
}


void AVG_ControlEndingNormal( void )
{
	int		bmp_bpp = BPP(MainWindow.draw_mode2);
	int		time;
	int		now,fcnt, bcnt, work,i;
	int		cnt, step_type,step_no;
	int		flag=0;
	int		click  = GameKey.click;
	int		cansel = GameKey.cansel;
	static int	fade=0;
	static int	bgm=0;
	char	buf[256];

	enum{
		END_INIT,
		END_ROLL,
		END_FIN,
		END_FADE,
		END_TERM
	};


	switch(Ending.step){
		case END_INIT:
			for(i=0;i<14;i++){
				wsprintf( buf, "EDb%03d.bmp", 100+i );
				DSP_LoadBmp( BMP_ENDING+i, BMP_FULL, buf );
			}
			for(i=0;i<26;i++){
				wsprintf( buf, "EDbT_%03d.bmp", 100+i+1 );
				DSP_LoadBmp( BMP_ENDING+14+i, BMP_256P, buf );
			}
			DSP_SetGraph( GRP_ENDING+0, BMP_ENDING+1, LAY_WINDOW+0, OFF, CHK_NO );
			DSP_SetGraph( GRP_ENDING+1, BMP_ENDING+1, LAY_WINDOW+1, OFF, CHK_NO );
			DSP_SetGraph( GRP_ENDING+2, BMP_ENDING+1, LAY_WINDOW+2, OFF, CHK_ANTI );
			
			Ending.step = END_ROLL;
			MainWindow.draw_flag = ON;
			AVG_PlayBGM( 17, 0, ON, 255, ON );
			Ending.start = timeGetTime();


			fade=-1;
			bgm=0;
			
		case END_FADE:
			if(fade>=64){
				Ending.step = END_TERM;
				Ending.count=0;
			}
			fade++;
			cnt = LIM( 128-fade*2, 0, 128 );
			DSP_SetGraphGlobalBright( cnt,cnt,cnt );
		case END_ROLL:
			now = timeGetTime();
			time = Ending.end;
			fcnt = now-Ending.start;
			cnt = (Ending.end-Ending.start)/46;

			if(cnt<0){
				AVG_StopBGM( 60 );
				Ending.step = END_TERM;
				break;
			}

			bcnt = fcnt%cnt;
			switch(fcnt/cnt){
			case 0:	step_type=0;	step_no=0;	break;

			case 1:	step_type=3;	step_no=0;	break;

			case 2:	step_type=2;	step_no=0;	break;
			case 3:	step_type=2;	step_no=1;	break;

			case 4:	step_type=1;	step_no=1;	break;

			case 5:	step_type=2;	step_no=2;	break;
			case 6:	step_type=2;	step_no=3;	break;

			case 7:	step_type=4;	step_no=1;	break;
			case 8:	step_type=3;	step_no=2;	break;

			case 9:	step_type=2;	step_no=4;	break;
			case 10:step_type=2;	step_no=5;	break;

			case 11:step_type=1;	step_no=3;	break;

			case 12:step_type=2;	step_no=6;	break;
			case 13:step_type=2;	step_no=7;	break;

			case 14:step_type=4;	step_no=3;	break;
			case 15:step_type=3;	step_no=4;	break;

			case 16:step_type=2;	step_no=8;	break;
			case 17:step_type=2;	step_no=9;	break;

			case 18:step_type=1;	step_no=5;	break;

			case 19:step_type=2;	step_no=10;	break;
			case 20:step_type=2;	step_no=11;	break;

			case 21:step_type=4;	step_no=5;	break;
			case 22:step_type=3;	step_no=6;	break;

			case 23:step_type=2;	step_no=12;	break;
			case 24:step_type=2;	step_no=13;	break;

			case 25:step_type=1;	step_no=7;	break;

			case 26:step_type=2;	step_no=14;	break;
			case 27:step_type=2;	step_no=15;	break;

			case 28:step_type=4;	step_no=7;	break;
			case 29:step_type=3;	step_no=8;	break;

			case 30:step_type=2;	step_no=16;	break;
			case 31:step_type=2;	step_no=17;	break;

			case 32:step_type=1;	step_no=9;	break;

			case 33:step_type=2;	step_no=18;	break;
			case 34:step_type=2;	step_no=19;	break;

			case 35:step_type=4;	step_no=9;	break;
			case 36:step_type=3;	step_no=10;	break;

			case 37:step_type=2;	step_no=20;	break;
			case 38:step_type=2;	step_no=21;	break;

			case 39:step_type=1;	step_no=11;	break;

			case 40:step_type=2;	step_no=22;	break;
			case 41:step_type=2;	step_no=23;	break;

			case 42:step_type=4;	step_no=11;	break;
			case 43:step_type=3;	step_no=12;	break;

			case 44:
			case 45:
				if(bgm==0){
					bgm=1;
					AVG_StopBGM( cnt*4/33 );
				}
				step_type=6;	step_no=24;	break;
			case 46:
				step_type=8;	step_no=12;	break;

			case 47:step_type=10;	break;
			case 48:step_type=2;	step_no=25;	break;

			case 49:step_type=5;	step_no=0;	break;

			case 50:
			case 51:
				step_type=-1;
				break;
			default:
				step_type=-1;
				Ending.step = END_TERM;
				Ending.count=0;
				break;
			}
			switch(step_type){
			default:
				break;
			case -1:
				DSP_SetGraphBmp( GRP_ENDING+0, BMP_ENDING );
				DSP_SetGraphFade( GRP_ENDING+0, 0 );
				DSP_SetGraphParam( GRP_ENDING+0, DRW_NML );

				DSP_SetGraphDisp( GRP_ENDING+0, ON );
				DSP_SetGraphDisp( GRP_ENDING+1, OFF );
				DSP_SetGraphDisp( GRP_ENDING+2, OFF );
				break;
			case 0:
				DSP_SetGraphBmp( GRP_ENDING+0, BMP_ENDING+step_no );

				DSP_SetGraphDisp( GRP_ENDING+0, ON );
				DSP_SetGraphDisp( GRP_ENDING+1, OFF );
				DSP_SetGraphDisp( GRP_ENDING+2, OFF );

				work = LIM( bcnt*128/cnt, 0, 128 ) ;
				DSP_SetGraphFade( GRP_ENDING+0, work );
				break;
			case 1:
				DSP_SetGraphFade( GRP_ENDING+0, 128 );
				DSP_SetGraphBmp( GRP_ENDING+0, BMP_ENDING+step_no );
				DSP_SetGraphBmp( GRP_ENDING+1, BMP_ENDING+step_no+1 );
				DSP_SetGraphDisp( GRP_ENDING+0, ON );
				DSP_SetGraphDisp( GRP_ENDING+1, ON );
				DSP_SetGraphDisp( GRP_ENDING+2, OFF );

				work = LIM( bcnt*256/cnt, 0, 256 ) ;
				DSP_SetGraphParam( GRP_ENDING+1, DRW_BLD(work) );
				break;
			case 2:
				DSP_SetGraphBmp( GRP_ENDING+2, BMP_ENDING+14+step_no );

				DSP_SetGraphDisp( GRP_ENDING+0, OFF );
				DSP_SetGraphParam( GRP_ENDING+1, DRW_NML );

				DSP_SetGraphDisp( GRP_ENDING+2, ON );

				if(bcnt<cnt*1/4){
					work = LIM( bcnt*4*256/cnt, 0, 256 ) ;
					DSP_SetGraphParam( GRP_ENDING+2, DRW_BLD(work) );
				}else if (bcnt<cnt*2/4){
					DSP_SetGraphParam( GRP_ENDING+2, DRW_NML );
				}else{
					work = 256-LIM( (bcnt-3*cnt/4)*4*256/cnt, 0, 256 ) ;
					DSP_SetGraphParam( GRP_ENDING+2, DRW_BLD(work) );
				}
				break;
			case 3:
				DSP_SetGraphFade( GRP_ENDING+0, 128 );
				DSP_SetGraphBmp( GRP_ENDING+0, BMP_ENDING+0 );
				DSP_SetGraphBmp( GRP_ENDING+1, BMP_ENDING+step_no+1 );
				DSP_SetGraphDisp( GRP_ENDING+0, ON );
				DSP_SetGraphDisp( GRP_ENDING+1, ON );
				DSP_SetGraphDisp( GRP_ENDING+2, OFF );

				work = LIM( bcnt*256/cnt, 0, 256 ) ;
				DSP_SetGraphParam( GRP_ENDING+1, DRW_BLD(work) );
				break;
			case 4:
				DSP_SetGraphFade( GRP_ENDING+0, 128 );
				DSP_SetGraphBmp( GRP_ENDING+0, BMP_ENDING+step_no+1 );
				DSP_SetGraphBmp( GRP_ENDING+1, BMP_ENDING+0 );
				DSP_SetGraphDisp( GRP_ENDING+0, ON );
				DSP_SetGraphDisp( GRP_ENDING+1, ON );
				DSP_SetGraphDisp( GRP_ENDING+2, OFF );

				work = LIM( bcnt*256/cnt, 0, 256 ) ;
				DSP_SetGraphParam( GRP_ENDING+1, DRW_BLD(work) );
				break;
			case 5:
				DSP_SetGraphBmp( GRP_ENDING+0, BMP_ENDING+0 );

				DSP_SetGraphDisp( GRP_ENDING+0, ON );
				DSP_SetGraphDisp( GRP_ENDING+1, OFF );
				DSP_SetGraphDisp( GRP_ENDING+2, OFF );

				work = 128-LIM( bcnt*128/cnt, 0, 128 ) ;
				DSP_SetGraphFade( GRP_ENDING+0, work );
				break;
			case 6:
				DSP_SetGraphDisp( GRP_ENDING+0, OFF );
				DSP_SetGraphDisp( GRP_ENDING+1, ON );
				DSP_SetGraphDisp( GRP_ENDING+2, ON );

				switch(fcnt/cnt){
					case 44:
						work = LIM( bcnt*256/cnt, 0, 256 ) ;
						break;
					case 45:
						work = 256;
						break;
				}
				DSP_SetGraphFade( GRP_ENDING+1, 128 );
				DSP_SetGraphParam( GRP_ENDING+1, DRW_NML );
				DSP_SetGraphBmp( GRP_ENDING+2, BMP_ENDING+14+step_no );
				DSP_SetGraphParam( GRP_ENDING+2, DRW_BLD(work) );
				break;
			case 8:
				DSP_SetGraphFade( GRP_ENDING+0, 128 );
				DSP_SetGraphBmp( GRP_ENDING+0, BMP_ENDING+step_no+1 );
				DSP_SetGraphBmp( GRP_ENDING+1, BMP_ENDING+0 );
				DSP_SetGraphDisp( GRP_ENDING+0, ON );
				DSP_SetGraphDisp( GRP_ENDING+1, ON );
				DSP_SetGraphDisp( GRP_ENDING+2, ON );

				work = LIM( bcnt*256/cnt, 0, 256 ) ;
				DSP_SetGraphParam( GRP_ENDING+1, DRW_BLD(work) );
				DSP_SetGraphParam( GRP_ENDING+2, DRW_BLD(256-work) );
				break;
			}
			if(click || cansel){
				if( Ending.cut && Ending.step==END_ROLL ){
					AVG_StopBGM( 60 );
					Ending.step = END_FADE;
					Ending.count=0;
				}
			}
			break;
		case END_TERM:
			DSP_SetGraphGlobalBright( 128,128,128 );
			AVG_StopBGM( 30 );
			for(i=0;i<40;i++){
				DSP_ReleaseBmp( BMP_ENDING+i );
			}
			for(i=0;i<20;i++){
				DSP_ResetGraph( GRP_ENDING+i );
			}
			ZeroMemory( &Ending, sizeof(Ending) );
			break;
	}
	Ending.count++;
}
void AVG_ControlEndingBad( void )
{
	int		bmp_bpp = BPP(MainWindow.draw_mode2);
	int		time;
	int		now,fcnt, bcnt, work,i;
	int		cnt, step_type,step_no;
	int		flag=0;
	int		click  = GameKey.click;
	int		cansel = GameKey.cansel;
	static int	fade=0;
	static int	bgm=0;
	char	buf[256];
	

	enum{
		END_INIT,
		END_ROLL,
		END_FIN,
		END_FADE,
		END_TERM
	};


	switch(Ending.step){
		case END_INIT:
			for(i=0;i<14;i++){
				wsprintf( buf, "EDb%03d.bmp", i );
				DSP_LoadBmp( BMP_ENDING+i, BMP_FULL, buf );
			}
			for(i=0;i<26;i++){
				wsprintf( buf, "EDbT_%03d.bmp", i+1 );
				DSP_LoadBmp( BMP_ENDING+14+i, BMP_256P, buf );
			}
			DSP_SetGraph( GRP_ENDING+0, BMP_ENDING+1, LAY_WINDOW+0, OFF, CHK_NO );
			DSP_SetGraph( GRP_ENDING+1, BMP_ENDING+1, LAY_WINDOW+1, OFF, CHK_NO );
			DSP_SetGraph( GRP_ENDING+2, BMP_ENDING+1, LAY_WINDOW+2, OFF, CHK_ANTI );
			
			Ending.step = END_ROLL;
			MainWindow.draw_flag = ON;
			AVG_PlayBGM( 18, 0, ON, 255, ON );
			Ending.start = timeGetTime();


			fade=-1;
			bgm=0;
			
		case END_FADE:
			if(fade>=64){
				Ending.step = END_TERM;
				Ending.count=0;
			}
			fade++;
			cnt = LIM( 128-fade*2, 0, 128 );
			DSP_SetGraphGlobalBright( cnt,cnt,cnt );
		case END_ROLL:
			now = timeGetTime();
			time = Ending.end;
			fcnt = now-Ending.start;
			cnt = (Ending.end-Ending.start)/46;

			if(cnt<0){
				AVG_StopBGM( 60 );
				Ending.step = END_TERM;
				break;
			}

			bcnt = fcnt%cnt;
			switch(fcnt/cnt){
			case 0:	step_type=0;	step_no=0;	break;
			case 1:	step_type=3;	step_no=0;	break;

			case 2:	step_type=2;	step_no=0;	break;
			case 3:	step_type=2;	step_no=1;	break;
			case 4:	step_type=2;	step_no=2;	break;

			case 5:	step_type=1;	step_no=1;	break;

			case 6:	step_type=2;	step_no=3;	break;
			case 7:	step_type=2;	step_no=4;	break;

			case 8:	step_type=1;	step_no=2;	break;

			case 9:	step_type=2;	step_no=5;	break;
			case 10:step_type=2;	step_no=6;	break;

			case 11:step_type=4;	step_no=2;	break;
			case 12:step_type=3;	step_no=3;	break;

			case 13:step_type=2;	step_no=7;	break;
			case 14:step_type=2;	step_no=8;	break;
			case 15:step_type=2;	step_no=9;	break;

			case 16:step_type=1;	step_no=4;	break;

			case 17:step_type=2;	step_no=10;	break;
			case 18:step_type=2;	step_no=11;	break;

			case 19:step_type=1;	step_no=5;	break;

			case 20:step_type=2;	step_no=12;	break;
			case 21:step_type=2;	step_no=13;	break;

			case 22:step_type=1;	step_no=6;	break;

			case 23:step_type=2;	step_no=14;	break;
			case 24:step_type=2;	step_no=15;	break;

			case 25:step_type=4;	step_no=6;	break;
			case 26:step_type=3;	step_no=7;	break;



			case 27:step_type=2;	step_no=16;	break;
			case 28:step_type=2;	step_no=17;	break;

			case 29:step_type=1;	step_no=8;	break;

			case 30:step_type=2;	step_no=18;	break;
			case 31:step_type=2;	step_no=19;	break;

			case 32:step_type=1;	step_no=9;	break;

			case 33:step_type=2;	step_no=20;	break;
			case 34:step_type=2;	step_no=21;	break;

			case 35:step_type=4;	step_no=9;	break;
			case 36:step_type=3;	step_no=10;	break;

			case 37:step_type=2;	step_no=22;	break;
			case 38:step_type=2;	step_no=23;	break;

			case 39:step_type=1;	step_no=11;	break;

			case 40:
			case 41:
			case 42:
				if(bgm==0){
					bgm=1;
					AVG_StopBGM( cnt*4/33 );
				}
				step_type=6;	step_no=24;	break;
			case 43:
				step_type=8;	step_no=12;	break;

			case 44:step_type=10;	break;
			case 45:step_type=7;	step_no=25;	break;

			case 46:step_type=5;	step_no=0;	break;
			case 48:
				step_type=-1;
				Ending.step = END_TERM;
				Ending.count=0;
				break;
			default:
				step_type=-1;
				break;
			}
			switch(step_type){
			default:
				break;
			case -1:
				DSP_SetGraphBmp( GRP_ENDING+0, BMP_ENDING );
				DSP_SetGraphFade( GRP_ENDING+0, 0 );
				DSP_SetGraphParam( GRP_ENDING+0, DRW_NML );

				DSP_SetGraphDisp( GRP_ENDING+0, ON );
				DSP_SetGraphDisp( GRP_ENDING+1, OFF );
				DSP_SetGraphDisp( GRP_ENDING+2, OFF );
				break;
			case 0:
				DSP_SetGraphBmp( GRP_ENDING+0, BMP_ENDING+step_no );

				DSP_SetGraphDisp( GRP_ENDING+0, ON );
				DSP_SetGraphDisp( GRP_ENDING+1, OFF );
				DSP_SetGraphDisp( GRP_ENDING+2, OFF );

				work = LIM( bcnt*128/cnt, 0, 128 ) ;
				DSP_SetGraphFade( GRP_ENDING+0, work );
				break;
			case 1:
				DSP_SetGraphFade( GRP_ENDING+0, 128 );
				DSP_SetGraphBmp( GRP_ENDING+0, BMP_ENDING+step_no );
				DSP_SetGraphBmp( GRP_ENDING+1, BMP_ENDING+step_no+1 );
				DSP_SetGraphDisp( GRP_ENDING+0, ON );
				DSP_SetGraphDisp( GRP_ENDING+1, ON );
				DSP_SetGraphDisp( GRP_ENDING+2, OFF );

				work = LIM( bcnt*256/cnt, 0, 256 ) ;
				DSP_SetGraphParam( GRP_ENDING+1, DRW_BLD(work) );
				break;
			case 2:
				DSP_SetGraphBmp( GRP_ENDING+2, BMP_ENDING+14+step_no );

				DSP_SetGraphDisp( GRP_ENDING+0, OFF );
				DSP_SetGraphParam( GRP_ENDING+1, DRW_NML );

				DSP_SetGraphDisp( GRP_ENDING+2, ON );

				work = LIM( (bcnt-cnt/5)*256/1, 0, 256 ) ;
				DSP_SetGraphParam( GRP_ENDING+2, DRW_BLD(work) );
				break;
			case 3:
				DSP_SetGraphFade( GRP_ENDING+0, 128 );
				DSP_SetGraphBmp( GRP_ENDING+0, BMP_ENDING+0 );
				DSP_SetGraphBmp( GRP_ENDING+1, BMP_ENDING+step_no+1 );
				DSP_SetGraphDisp( GRP_ENDING+0, ON );
				DSP_SetGraphDisp( GRP_ENDING+1, ON );
				DSP_SetGraphDisp( GRP_ENDING+2, OFF );

				work = LIM( bcnt*256/cnt, 0, 256 ) ;
				DSP_SetGraphParam( GRP_ENDING+1, DRW_BLD(work) );
				break;
			case 4:
				DSP_SetGraphFade( GRP_ENDING+0, 128 );
				DSP_SetGraphBmp( GRP_ENDING+0, BMP_ENDING+step_no+1 );
				DSP_SetGraphBmp( GRP_ENDING+1, BMP_ENDING+0 );
				DSP_SetGraphDisp( GRP_ENDING+0, ON );
				DSP_SetGraphDisp( GRP_ENDING+1, ON );
				DSP_SetGraphDisp( GRP_ENDING+2, OFF );

				work = LIM( bcnt*256/cnt, 0, 256 ) ;
				DSP_SetGraphParam( GRP_ENDING+1, DRW_BLD(work) );
				break;
			case 5:
				DSP_SetGraphBmp( GRP_ENDING+0, BMP_ENDING+0 );

				DSP_SetGraphDisp( GRP_ENDING+0, ON );
				DSP_SetGraphDisp( GRP_ENDING+1, OFF );
				DSP_SetGraphDisp( GRP_ENDING+2, OFF );

				work = 128-LIM( bcnt*128/cnt, 0, 128 ) ;
				DSP_SetGraphFade( GRP_ENDING+0, work );
				break;
			case 6:
				DSP_SetGraphBmp( GRP_ENDING+0, BMP_ENDING+12 );
				DSP_SetGraphBmp( GRP_ENDING+1, BMP_ENDING+13 );
				DSP_SetGraphBmp( GRP_ENDING+2, BMP_ENDING+14+step_no );
				DSP_SetGraphDisp( GRP_ENDING+0, ON );
				DSP_SetGraphDisp( GRP_ENDING+1, ON );
				DSP_SetGraphDisp( GRP_ENDING+2, ON );

				switch(fcnt/cnt){
					case 40:
						work = 0 ;
						break;
					case 41:
						work = LIM( bcnt*256/cnt, 0, 256 ) ;
						break;
					case 42:
						work = 256;
						break;
				}
				DSP_SetGraphFade( GRP_ENDING+0, 128 );
				DSP_SetGraphFade( GRP_ENDING+1, 128 );
				DSP_SetGraphParam( GRP_ENDING+0, DRW_NML );
				DSP_SetGraphParam( GRP_ENDING+1, DRW_BLD(work) );


				DSP_SetGraphParam( GRP_ENDING+2, DRW_NML );
				break;
			case 7:
				DSP_SetGraphBmp( GRP_ENDING+2, BMP_ENDING+14+step_no );

				DSP_SetGraphDisp( GRP_ENDING+0, OFF );
				DSP_SetGraphParam( GRP_ENDING+1, DRW_NML );

				DSP_SetGraphDisp( GRP_ENDING+2, ON );

				if(bcnt<cnt*1/4){
					work = LIM( bcnt*4*256/cnt, 0, 256 ) ;
					DSP_SetGraphParam( GRP_ENDING+2, DRW_BLD(work) );
				}else if (bcnt<cnt*2/4){
					DSP_SetGraphParam( GRP_ENDING+2, DRW_NML );
				}else{
					work = 256-LIM( (bcnt-3*cnt/4)*4*256/cnt, 0, 256 ) ;
					DSP_SetGraphParam( GRP_ENDING+2, DRW_BLD(work) );
				}
				break;
			case 8:
				DSP_SetGraphFade( GRP_ENDING+0, 128 );
				DSP_SetGraphBmp( GRP_ENDING+0, BMP_ENDING+step_no+1 );
				DSP_SetGraphBmp( GRP_ENDING+1, BMP_ENDING+0 );
				DSP_SetGraphDisp( GRP_ENDING+0, ON );
				DSP_SetGraphDisp( GRP_ENDING+1, ON );
				DSP_SetGraphDisp( GRP_ENDING+2, ON );

				work = LIM( bcnt*256/cnt, 0, 256 ) ;
				DSP_SetGraphParam( GRP_ENDING+1, DRW_BLD(work) );
				DSP_SetGraphParam( GRP_ENDING+2, DRW_BLD(256-work) );
				break;
			}
			if(click || cansel){
				if( Ending.cut && Ending.step==END_ROLL ){
					AVG_StopBGM( 60 );
					Ending.step = END_FADE;
					Ending.count=0;
				}
			}
			break;
		case END_TERM:
			DSP_SetGraphGlobalBright( 128,128,128 );
			AVG_StopBGM( 30 );
			for(i=0;i<40;i++){
				DSP_ReleaseBmp( BMP_ENDING+i );
			}
			for(i=0;i<20;i++){
				DSP_ResetGraph( GRP_ENDING+i );
			}
			ZeroMemory( &Ending, sizeof(Ending) );
			break;
	}
	Ending.count++;
}

extern void AVG_ControlEndingOmake(void)
{
	int		bmp_bpp = BPP(MainWindow.draw_mode2);
	int		time;
	int		now,fcnt, i;
	int		flag=0;
	int		click  = GameKey.click;
	int		cansel = GameKey.cansel;
	static int	fade=0;

	enum{
		END_INIT,
		END_ROLL,
		END_FIN,
		END_FADE,
		END_TERM
	};

	switch(Ending.step){
		case END_INIT:
			DSP_LoadBmp( BMP_ENDING+0, BMP_256P, "EDb200.bmp" );
			DSP_SetGraph( GRP_ENDING+0, BMP_ENDING+0, LAY_WINDOW+1, ON, CHK_NO );

			DSP_LoadBmp( BMP_ENDING+1, BMP_256P, "EDb200.bmp" );
			DSP_SetGraph( GRP_ENDING+1, BMP_ENDING+1, LAY_WINDOW+2, ON, CHK_NO );

			Ending.step = END_ROLL;
			MainWindow.draw_flag = ON;
			AVG_PlayBGM( 18, 0, ON, 255, ON );
			Ending.start = timeGetTime();
			
		case END_ROLL:




			now = timeGetTime();
			time = Ending.end;
			fcnt = now-Ending.start;

			DSP_SetGraphPos( GRP_ENDING+0, (800-442)/2,0,  0,-600+fcnt*(7680)/(Ending.end-Ending.start), 442, 600 );
			
			if(now<time){
				if(click  && Ending.cut ){
					Ending.time1 = timeGetTime();
					AVG_StopBGM( 128 );
					Ending.step = END_FADE;
					Ending.count=0;
				}
			}else{
				Ending.step = END_TERM;
				Ending.count=0;
			}
			break;
		case END_FADE:
			now = timeGetTime();
			time = Ending.end;
			fcnt = now-Ending.start;

			DSP_SetGraphPos( GRP_ENDING+0, (800-442)/2,0,  0,-600+fcnt*(7680)/(Ending.end-Ending.start), 442, 600 );


			fcnt = Ending.count;
			DSP_SetGraphFade( GRP_ENDING+0, 128-128*fcnt/128 );

			DSP_SetGraphDisp( GRP_ENDING+1, ON );
			DSP_SetGraphFade( GRP_ENDING+1, 19*(128-128*fcnt/128)/128 );

			if(fcnt>=128){
				Ending.step = END_TERM;
			}
			break;
		case END_TERM:
			AVG_StopBGM( 30 );
			for(i=0;i<40;i++){
				DSP_ReleaseBmp( BMP_ENDING+i );
			}
			for(i=0;i<20;i++){
				DSP_ResetGraph( GRP_ENDING+i );
			}
			ZeroMemory( &Ending, sizeof(Ending) );
			break;
	}
	Ending.count++;
}
void AVG_ControlEndingTrue( void )
{
	int		bmp_bpp = BPP(MainWindow.draw_mode2);
	int		time;
	int		now,fcnt, i;
	int		cnt;
	int		flag=0;
	int		click  = GameKey.click;
	int		cansel = GameKey.cansel;
	static int	fade=0;

	enum{
		END_INIT,
		END_ROLL,
		END_FIN,
		END_FADE,
		END_TERM
	};

	switch(Ending.step){
		case END_INIT:
			DSP_LoadBmp( BMP_ENDING+0, BMP_256P, "EDb300.bmp" );
			DSP_SetGraph( GRP_ENDING+0, BMP_ENDING+0, LAY_WINDOW+1, ON, CHK_NO );

			DSP_LoadBmp( BMP_ENDING+1, BMP_256P, "EDb201.bmp" );
			DSP_SetGraph( GRP_ENDING+1, BMP_ENDING+1, LAY_WINDOW+2, OFF, CHK_ANTI );

			Ending.step = END_ROLL;
			MainWindow.draw_flag = ON;
			AVG_PlayBGM( 16, 0, OFF, 255, ON );
			Ending.start = timeGetTime();
			
		case END_ROLL:
			now = timeGetTime();
			time = Ending.end;
			fcnt = now-Ending.start;

			cnt = LIM( (fcnt*(11790)/(Ending.end-Ending.start)), 0, 11790-600 );
			DSP_SetGraphPos( GRP_ENDING+0, 0, 0, 0, cnt, 800, 600 );
			
			if(now<time){
				if(click  && Ending.cut ){
					Ending.time1 = timeGetTime();
					AVG_StopBGM( 60 );
					Ending.step = END_FADE;
					Ending.count=0;
				}
			}else{
				Ending.step = END_FIN;
				Ending.count=0;
			}
			break;
		case END_FIN:
			fcnt = Ending.count;
			DSP_SetGraphPos( GRP_ENDING+0, 0, 0, 0, 11790-600, 800, 600 );
			DSP_SetGraphFade( GRP_ENDING+0, LIM(128-128*fcnt/120, 0,128) );

			DSP_SetGraphDisp( GRP_ENDING+1, ON );
			if(fcnt<180){
				cnt = LIM(256*(fcnt-150)/30, 0, 256 );
				DSP_SetGraphParam( GRP_ENDING+1, DRW_BLD(cnt) );
			}else{
				cnt = 256-LIM(256*(fcnt-210)/30, 0, 256 );
				DSP_SetGraphParam( GRP_ENDING+1, DRW_BLD(cnt) );
			}

			if(fcnt<270){
				if( click && Ending.cut  ){
					Ending.time1 = timeGetTime();
					Ending.step = END_FADE;
				}
			}else{
				Ending.step = END_TERM;
			}
			break;
		case END_FADE:
			now = timeGetTime();
			time = Ending.end;

			fcnt = now-Ending.start;

			cnt = LIM( (fcnt*(11790)/(Ending.end-Ending.start)), 0, 11790-600 );
			DSP_SetGraphPos( GRP_ENDING+0, 0, 0, 0, cnt, 800, 600 );

			fcnt = Ending.count;
			DSP_SetGraphFade( GRP_ENDING+0, 128-128*fcnt/128 );

			DSP_SetGraphDisp( GRP_ENDING+1, ON );
			DSP_SetGraphFade( GRP_ENDING+1, 19*(128-128*fcnt/128)/128 );

			if(fcnt>=128){
				Ending.step = END_TERM;
			}
			break;
		case END_TERM:
			AVG_StopBGM( 30 );
			for(i=0;i<40;i++){
				DSP_ReleaseBmp( BMP_ENDING+i );
			}
			for(i=0;i<20;i++){
				DSP_ResetGraph( GRP_ENDING+i );
			}
			ZeroMemory( &Ending, sizeof(Ending) );
			break;
	}
	Ending.count++;
}


int AVG_WaitEnding( void )
{
	return !Ending.flag;
}
//
void AVG_ControlEnding( void )
{
	int	char_end=0;
	if( Ending.flag ){
		switch(Ending.no){
			default:
			case _GFLG_BAD_END:
				AVG_ControlEndingBad();
				break;
			case _GFLG_NORMAL_END:
				AVG_ControlEndingNormal();
				break;
			case _GFLG_TRUE_END:
				AVG_ControlEndingTrue();
				break;
		}
	}
}
