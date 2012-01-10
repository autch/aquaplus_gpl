
#include "mm_std.h"
#include "main.h"

#include "disp.h"
#include "draw.h"
#include "movie.h"



#include "comp_bmppac.h"

#include "GM_Avg.h"
#include "GM_Title.h"
#include "GM_Window.h"
#include "GM_Omake.h"

#include "font.h"
#include "mouse.h"
#include "escript.h"
#include "movPlayer.h"

enum{
	TTL_INIT = 0,
	TTL_LOAD,
	TTL_MAIN,
	TTL_TERM,
};

enum{
	END_INITLOOP = 0,
	END_INITSTART,
	END_LOADSTART,
	END_OMAKESTART,
	END_MOVIE_MODE,
	END_GAMEEND,
};

static int		TitleStep[5]     = { 0, 0, 0, 0, 0 };
static int		NextTitleStep[5] = { 0, 0, 0, 0, 0 };
static int		TitleCounter[5]  = { 0, 0, 0, 0, 0 };


int	InitBootFlag=1;
int	TaikenBan=0;


typedef struct{
	int		end_mode;	
	int		omake;
}TITLE_STRUCT;

TITLE_STRUCT	Title;




static void TTL_ChangeSetp( int index, int step )
{
	NextTitleStep[index] = step;
}

static void TTL_RenewSetp( void )
{
	int	index;

	for( index=0; index<5 ;index++ ){
		if( TitleStep[index] != NextTitleStep[index] )
			TitleCounter[index] = 0;
		else
			TitleCounter[index]++;
		TitleStep[index] = NextTitleStep[index];
	}
}
BOOL TTL_ChangeSetpCounter( int index, int step, int counter )
{
	BOOL	ret=FALSE;
	if( TitleCounter[index] >= counter ){
		TTL_ChangeSetp( index, step );
		ret=TRUE;
	}
	return ret;
}



void TTL_Init( void )
{
	MAIN_SetScriptFlag( OFF );	


	TitleStep[1] = 0;
}

void TTL_Load( void )
{
}

void TTL_Term( void )
{
	DSP_TermDisp();

	MAIN_SetScriptFlag( ON );	
}

static int	menu_act[2][5];
void TTL_MouseSetting( int load, int omake )
{
	MUS_ResetMouseRect_All();
	MUS_SetMouseLayer( 0 );

	MUS_SetMouseRect( 0, 0,  306, 385+40*0,  188, 32, menu_act[0][0]==2 );	
	MUS_SetMouseRect( 0, 1,  306, 385+40*1,  188, 32, menu_act[0][1]==2 );	
	MUS_SetMouseRect( 0, 2,  306, 385+40*2,  188, 32, menu_act[0][2]==2 );	
	MUS_SetMouseRect( 0, 3,  306, 385+40*3,  188, 32, menu_act[0][3]==2 );	
	MUS_SetMouseRect( 0, 4,  306, 385+40*4,  188, 32, menu_act[0][4]==2 );	
}

void TTL_MouseSettingMode2( void )
{
	MUS_SetMouseLayer( 1 );

	MUS_SetMouseRect( 1, 0,  306, 385+40*0,  188, 32, menu_act[1][0]==2 );	
	MUS_SetMouseRect( 1, 1,  306, 385+40*1,  188, 32, menu_act[1][1]==2 );	
	MUS_SetMouseRect( 1, 2,  306, 385+40*2,  188, 32, menu_act[1][2]==2 );	
	MUS_SetMouseRect( 1, 3,  306, 385+40*3,  188, 32, menu_act[1][3]==2 );	
	MUS_SetMouseRect( 1, 4,  306, 385+40*4,  188, 32, menu_act[1][4]==2 );	
}


int	TTL_CheckSaveData( void )
{
	BOOL	ret=0;
	int		i;
	char	buf[256];


	for(i=0;i<100;i++){
		wsprintf( buf, "save_%02d.sav", i );
		ret |= STD_CheckFile( buf );
		if(ret) break;
	}

	return ret;
}


#define	_GFLG_HINT00				20
#define	_GFLG_HINT01				21
#define	_GFLG_HINT02				22
#define	_GFLG_HINT03				23
#define	_GFLG_HINT04				24
#define	_GFLG_HINT05				25
#define	_GFLG_HINT06				26
#define	_GFLG_HINT07				27
#define	_GFLG_HINT08				28
#define	_GFLG_HINT09				29
#define	_GFLG_HINT10				30
#define	_GFLG_HINT11				31


#define	_GFLG_END					0	
#define	_GFLG_ALL_END				1	
#define	_GFLG_1BU_CLR				2	
#define	_GFLG_2BU_CLR				3	

#define	_GFLG_2SYU_SP				10	
#define	_GFLG_TRUE_SP				11	

#define	_GFLG_TRUE_END				26	

#define	_GFLG_2BU_START				50	

int	TTL_GetOmakeCount( void )
{
	int	ret=0;


	if( AVG_GetGameFlag( 80 ) ){	
		ret = 1;
	}
	if( AVG_GetGameFlag( 87 ) ){	
		ret = 2;
	}
	return ret;
}

int TTL_GetTime()
{
	if(0){
		return GlobalCount*34;
	}else{
		return STD_timeGetTime();
	}
}

void TTL_SystemDisp( int omake, int disp )
{
	int	i;

	for(i=0;i<5;i++){
		if(!omake){
			switch( menu_act[0][i] ){
				case 0:
					DSP_SetGraphDisp( 5+i, OFF );
					break;
				case 1:
					DSP_SetGraphDisp( 5+i, disp );
					DSP_SetGraphFade( 5+i, 64 );
					break;
				case 2:
					DSP_SetGraphDisp( 5+i, disp );
					break;
			}
			DSP_SetGraphDisp( 10+i, OFF );
		}else{
			switch( menu_act[1][i] ){
				case 0:
					DSP_SetGraphDisp( 10+i, OFF );
					break;
				case 1:
					DSP_SetGraphDisp( 10+i, disp );
					DSP_SetGraphFade( 10+i, 64 );
					break;
				case 2:
					DSP_SetGraphDisp( 10+i, disp );
					break;
			}
			DSP_SetGraphDisp( 5+i, OFF );
		}
	}
}


BOOL TTL_SystemMain(void )
{
	int		bmp_bpp = BPP(MainWindow.draw_mode2);
	BOOL	ret = TRUE;
	int		cnt;
	float	cnt2;
	static int	mus_lay=0;
	int		select = MUS_GetMouseNoEx( -1, mus_lay );
	int		key_flag=0, select2;
	int		click  = GameKey.click;
	int		cansel = GameKey.cansel;
	int		fmax = AVG_EffCnt4(-1);
	int		i,j,disp_title=1;

	static int	load  = 0;
	int		omake = TTL_GetOmakeCount();


	DWORD			now_time = TTL_GetTime();
	static DWORD	op_time=0, opse_flag=0;
	int				dcnt;
	static int	select_fade=0;
	static int	select_trg=0;
	static int	select_back=0;

	static int	cut = 0;
	static int	se = -1;
	static int	mh = -1;
	static int	mv_mode = 0;
	static int	op_count = 0;
	static int	op_fade = 0;
	static int	sel_bak = -1;
	static int	sel_cnt = 0;

	enum{
		TTL_LEAF_INIT,
		TTL_LEAF_IN,
		TTL_LEAF_MAIN,
		TTL_LEAF_OUT,
		TTL_LEAF_END,

		TTL_MOVIE_INIT,
		TTL_MOVIE_MAIN,

		TTL_SP1_IN,
		TTL_SP1_OUT,
		TTL_SP1_SELECT,

		TTL_SP2_IN,
		TTL_SP2_OUT,
		TTL_SP2_SELECT,

		TTL_SYS_INIT,
		TTL_SYS_FADEIN,
		TTL_SYS_INSERT,
		TTL_SYS_INSERT2,
		TTL_SYS_MENU,

		TTL_SYS_LOAD,
		TTL_SYS_SETTING,
		TTL_SYS_CG,
		TTL_SYS_REPLAY,
		TTL_SYS_SOUND,

		TTL_SYS_OLD_CG,
		TTL_SYS_SEIYU,

		



		TTL_SYS_FADEOUT,
		TTL_SYS_END,
		TTL_SYS_END2,
	};


	Avg.msg_cut=0;
	Avg.msg_cut_mode=0;
	switch( TitleStep[1] ){

		
		case TTL_LEAF_INIT:



			DSP_TermDisp();
			DSP_LoadBmp( 0, BMP_FULL, "black.bmp" );
			DSP_SetGraph( 0, 0, 0, ON, CHK_NO );
			MainWindow.draw_flag=1;

			AVG_StopBGM( 0 );
			EnableMenuItem( GetMenu( MainWindow.hwnd ), ID_TITLE, MF_GRAYED );
			FNT_Term();
			FNT_LoadFont( 16, 0 );


			
			menu_act[0][0] = 2;	
			menu_act[0][1] = 2;	
			menu_act[0][2] = 2;	
			menu_act[0][3] = (omake)?2:0;	
			menu_act[0][4] = 2;	

			menu_act[1][0] = 2;	
			menu_act[1][1] = 2;	
			menu_act[1][2] = 2;	
			menu_act[1][3] = 0;	
			menu_act[1][4] = 2;	

			Title.end_mode = END_INITLOOP;	

			if(Avg.omake){
				TTL_ChangeSetp( 1, TTL_SYS_INIT );
				break;
			}
			if(GotoTitle){
				TTL_ChangeSetp( 1, TTL_SYS_INIT );
				break;
			}
			

			TTL_ChangeSetp( 1, TTL_LEAF_IN );
			break;
		
		case TTL_LEAF_IN:
			AVG_SetMovie( 3,0 );
			TTL_ChangeSetp( 1, TTL_LEAF_MAIN );
			break;
		case TTL_LEAF_MAIN:
			if( !AVG_WaitMovie() ){
				if( AVG_GetGameFlag(98)==0 ){
					TTL_ChangeSetp( 1, TTL_SYS_INIT );
				}else{
					TTL_ChangeSetp( 1, TTL_MOVIE_INIT );
				}
			}
			break;
		
		case TTL_LEAF_OUT:
		
		case TTL_LEAF_END:
			break;

		
		case TTL_MOVIE_INIT:
			AVG_SetMovie( 0,0 );
			TTL_ChangeSetp( 1, TTL_MOVIE_MAIN );
			break;

		
		case TTL_MOVIE_MAIN:
			if( !AVG_WaitMovie() ){
				TTL_ChangeSetp( 1, TTL_SYS_INIT );
			}
			break;

		
		default:
		case TTL_SYS_INIT:


			DSP_SetGraphGlobalBright( 128,128,128 );

			DSP_LoadBmp( 0, BMP_FULL, "t0000.tga" );
			DSP_LoadBmp( 1, BMP_FULL, "t0001.tga" );
			DSP_LoadBmp( 2, BMP_TRUE, "t0010.tga" );

			DSP_LoadBmp2( 3, BMP_256P, "f0052.bmp");
			

			DSP_SetGraph( 0, 0, 0, OFF, CHK_NO );
			DSP_SetGraph( 1, 1, 1, OFF, CHK_NO );


				DSP_SetGraph( 2, 2, 2, OFF, CHK_ANTI );
				DSP_SetGraphPos( 2, 477,304, 188*3,0, 177,38 );	
				DSP_SetGraph( 3, 2, 3, OFF, CHK_ANTI );
				DSP_SetGraphPos( 3, 477,304, 188*3,0, 177,38 );	


			for(i=0;i<5;i++){
				DSP_SetGraph(  5+i, 2, 2, OFF, CHK_ANTI );
				DSP_SetGraph( 10+i, 2, 3, OFF, CHK_ANTI );
				DSP_SetGraphPos(  5+i, 306, 385+40*i, 188*0, 32*i,     188,32 );	
				DSP_SetGraphPos( 10+i, 306, 385+40*i, 188*0, 32*(5+i), 188,32 );	
				DSP_SetGraphParam( 10+i, DRW_BLD(0) );
			}
			if(Avg.omake){
				for(i=0;i<5;i++){
					DSP_SetGraphParam( 5+i, DRW_BLD(0) );
					DSP_SetGraphDisp( 10+i, ON );
				}
				DSP_SetGraphDisp( 13, FALSE );

				TTL_MouseSetting( load, omake );

				OMK_SetReplayMode( 40, 40, 40, 2, 0 );
				TTL_ChangeSetp( 1, TTL_SYS_REPLAY );
				break;
			}

			MainWindow.draw_flag = 1;
			TTL_ChangeSetp( 1, TTL_SYS_INSERT );
			break;
		
		case TTL_SYS_INSERT:	
			dcnt = TitleCounter[1];

			DSP_SetGraphDisp( 0, ON );
			DSP_SetGraphDisp( 1, ON );

			DSP_SetGraphFade( 0, LIM(dcnt*8,0,128) );
			DSP_SetGraphBSet( 1, 3, (dcnt-16)*8, 32 );

			if( TTL_ChangeSetpCounter( 1, TTL_SYS_INSERT2, 48) ){
				DSP_SetGraphDisp( 0, OFF );
				DSP_ResetGraphBSet( 1 );

			}
			break;
		case TTL_SYS_INSERT2:
			if( TitleCounter[1]==0){
				InitBootFlag = !STD_CheckFile( "Sys.sav" );
				TTL_MouseSetting( load, omake );
			}


				cnt = TitleCounter[1];
				DSP_SetGraphDisp( 2, ON );
				DSP_SetGraphParam( 2, DRW_BOM(LIM(128-cnt*4,0,255)) );
				if( TitleCounter[1] >= 32){
					cnt = TitleCounter[1]-32;
					DSP_SetGraphDisp( 3, ON );
					DSP_SetGraphParam( 3, DRW_ADD );
					DSP_SetGraphFade( 3, LIM(128-16*cnt,0,128) );
					DSP_SetGraphZoom2( 3, 477+177/2, 304+38/2, cnt*8  );
				}



			for(i=0;i<5;i++){
				switch( menu_act[0][i] ){
					case 0:
						DSP_SetGraphDisp( 5+i, OFF );
						break;
					case 1:
						DSP_SetGraphDisp( 5+i, ON );
						DSP_SetGraphFade( 5+i, 64 );
						break;
					case 2:
						DSP_SetGraphDisp( 5+i, ON );
						break;
				}

					cnt = TitleCounter[1]-i*4-40;



				DSP_SetGraphParam( 5+i, DRW_BLD( LIM(cnt*256/16,0,256) ) );
			}


				if( TTL_ChangeSetpCounter( 1, TTL_SYS_MENU, 40+32) ){
					DSP_SetGraphDisp( 3, OFF );
				}




			break;
		
		case TTL_SYS_MENU:


			op_count++;

			mus_lay = 0;
			if(GameKey.u) {
				select2 = LIM(select-1, 0, 4 );
				i=0;
				while( !MUS_GetMouseRectFlag(0,select2-i) ){
					i++;
					if(select2-i<0){
						break;
					}
				}
				select2 = LIM(select2-i, 0, 4 );
				key_flag=1;
			}
			if(GameKey.d) {
				select2 = LIM(select+1, 0, 4 );
				i=0;
				while( !MUS_GetMouseRectFlag(0,select2+i) ){
					i++;
					if(i+select2>4){
						break;
					}
				}
				select2 = LIM(select2+i, 0, 4 );
				key_flag=1;
			}
			if(key_flag){
				if( MUS_GetMouseRectFlag(0,select2) ){
					select=select2;
					MUS_SetMousePosRect( MainWindow.hwnd, 0, select );
				}
			}

			for(i=0;i<5;i++){
				DSP_SetGraphPos(  5+i, 306, 385+40*i, 188*0, 32*i,     188,32 );	

				DSP_SetGraphDisp( 10+i, FALSE );

				DSP_SetGraphParam( 5+i, DRW_BLD(256) );
			}

			if(select!=-1){
				if(select != select_back )
					AVG_PlaySE3( 9108, 255 );

				DSP_SetGraphPos(  5+select, 306, 385+40*select, 188*1, 32*select,     188,32 );	
				if( click ){
					DSP_SetGraphPos(  5+select, 306, 385+40*select, 188*2, 32*select,     188,32 );	
					switch(select){
						case 0:
							AVG_PlaySE3( 9111, 255 );
							Title.end_mode = END_INITSTART;	
							TTL_ChangeSetp( 1, TTL_SYS_FADEOUT );
							break;
						case 1:
							AVG_PlaySE3( 9104, 255 );
							GWIN_SetSaveLoadWindow( SW_LOAD_MODE2, 40, 40, 40, -1,-1, 1, 0 );
							GWIN_ControlSaveLoadWindow();
							TTL_ChangeSetp( 1, TTL_SYS_LOAD );
							break;
						case 2:
							AVG_PlaySE3( 9104, 255 );
							GWIN_SetSideBarWindow(  40, 40, 40, 1, 0 );
							GWIN_ControlSideBarWindow();
							TTL_ChangeSetp( 1, TTL_SYS_SETTING );
							break;
						case 3:
							AVG_PlaySE3( 9104, 255 );
							TTL_ChangeSetp( 1, TTL_SP1_IN );
							break;
						case 4:
							AVG_PlaySE3( 9104, 255 );
							Title.end_mode = END_GAMEEND;	
							TTL_ChangeSetp( 1, TTL_SYS_FADEOUT );
							break;
					}
				}
			}
			select_back=select;


			break;

		case TTL_SP1_IN:


			op_count++;

			cnt = TitleCounter[1];

			for(i=0;i<5;i++){
				DSP_SetGraphPos( 10+i, 306, 385+40*i, 188*0, 32*(5+i), 188,32 );
				DSP_SetGraphDisp( 10+i, ON );
				DSP_SetGraphParam( 10+i, DRW_BLD( (cnt-i*2)*16) );

				DSP_SetGraphParam( 5+i, DRW_BLD( 256-(cnt-i*2)*16) );
			}
			DSP_SetGraphDisp( 13, FALSE );
			if( TTL_ChangeSetpCounter( 1, TTL_SP1_SELECT, 24 ) ){
				mus_lay = 1;
				TTL_MouseSettingMode2();
			}
			break;
		case TTL_SP1_OUT:


			op_count++;

			cnt = TitleCounter[1];

			for(i=0;i<5;i++){
				DSP_SetGraphDisp( 5+i, ON );
				DSP_SetGraphParam( 5+i, DRW_BLD( (cnt-i*2)*16) );

				DSP_SetGraphPos( 10+i, 306, 385+40*i, 188*0, 32*(5+i), 188,32 );
				DSP_SetGraphDisp( 10+i, ON );
				DSP_SetGraphParam( 10+i, DRW_BLD( 256-(cnt-i*2)*16) );
			}
			DSP_SetGraphDisp( 13, FALSE );


			if( TTL_ChangeSetpCounter( 1, TTL_SYS_MENU, 24 ) ){
				for(i=0;i<5;i++){
					DSP_SetGraphDisp( 10+i, OFF );
				}
				mus_lay = 0;
				MUS_SetMouseLayer( 0 );
			}
			break;
		case TTL_SP1_SELECT:


			op_count++;

			if(GameKey.u) {
				select2 = LIM(select-1, 0, 3 );
				key_flag=1;
			}
			if(GameKey.d) {
				select2 = LIM(select+1, 0, 3 );
				key_flag=1;
			}
			if(key_flag){
				if( MUS_GetMouseRectFlag(1,select2) ){
					select=select2;
					MUS_SetMousePosRect( MainWindow.hwnd, 1, select );
				}
			}

			for(i=0;i<5;i++){
				DSP_SetGraphPos( 10+i, 306, 385+40*i, 188*0, 32*(5+i), 188,32 );
				DSP_SetGraphDisp( 5+i, FALSE );

				DSP_SetGraphParam( 10+i, DRW_BLD(256) );
			}

			if(select!=-1){
				if(select != select_back )
					AVG_PlaySE3( 9108, 255 );

				DSP_SetGraphPos(  10+select, 306, 385+40*select, 188*1, 32*(select+5),     188,32 );	
				if( click ){
					DSP_SetGraphPos(  10+select, 306, 385+40*select, 188*2, 32*(select+5),     188,32 );	
					switch(select){
						case 0:
							AVG_PlaySE3( 9104, 255 );
							OMK_SetCgMode( 40, 40, 40, 2 );
							TTL_ChangeSetp( 1, TTL_SYS_CG );
							break;
						case 1:
							AVG_PlaySE3( 9104, 255 );
							OMK_SetSoundMode( 40, 40, 40, 2 );
							TTL_ChangeSetp( 1, TTL_SYS_SOUND );
							break;
						case 2:
							AVG_PlaySE3( 9104, 255 );
							OMK_SetReplayMode( 40, 40, 40, 2, 0 );
							TTL_ChangeSetp( 1, TTL_SYS_REPLAY );
							break;
						case 4:
							AVG_PlaySE3( 9104, 255 );
							TTL_ChangeSetp( 1, TTL_SP1_OUT );
							break;
					}
				}
			}else{
				select_fade=0;
				select_trg=0;
				DSP_SetGraphDisp( 20, OFF );
			}
			if(cansel){
				AVG_PlaySE3( 9107, 255 );
				TTL_ChangeSetp( 1, TTL_SP1_OUT );
			}

			if(select_back!=select){
				select_fade=0;
				select_trg=0;
			}
			select_back=select;
			break;

		
		case TTL_SYS_SETTING:
			switch( GWIN_ControlSideBarWindow() ){
			case 2:
				TTL_SystemDisp( 0, OFF );
				break;
			case 1:
				TTL_SystemDisp( 0, ON );
				break;
			case 0:
				TTL_SystemDisp( 0, ON );

				TTL_ChangeSetp( 1, TTL_SYS_MENU );
				break;
			case -1:
				break;
			}
			break;

		
		case TTL_SYS_LOAD:




			switch( GWIN_ControlSaveLoadWindow() ){
			case 2:
				for(i=0;i<21;i++){
					DSP_SetGraphDisp( i, OFF );
				}
				TTL_SystemDisp( 0, OFF );
				break;
			case 1:
				for(i=0;i<21;i++){
					DSP_SetGraphDisp( i, ON );
				}
				TTL_SystemDisp( 0, ON );
				break;
			case 0:

				for(i=0;i<21;i++){
					DSP_SetGraphDisp( i, ON );
				}
				TTL_SystemDisp( 0, ON );

				TTL_ChangeSetp( 1, TTL_SYS_MENU );
				break;
			case -1:
				break;
			case -2:
				if( SaveWindow.load_flag ){
					SaveWindow.load_flag = OFF;
					Title.end_mode = END_LOADSTART;
					TTL_ChangeSetp( 1, TTL_SYS_END );
				}
				break;
			}
			for(i=0;i<4;i++){
				DSP_SetGraphDisp( 15+i, OFF );
			}
			break;

		
		case TTL_SYS_SOUND:
			switch( OMK_ControlSoundMode() ){
				case -1:
					for(i=5;i<10;i++){
						DSP_SetGraphDisp( i, ON );
					}
					break;
				case 0:
					for(i=5;i<10;i++){
						DSP_SetGraphDisp( i, OFF );
					}
					break;
				case 1:
					for(i=5;i<10;i++){
						DSP_SetGraphDisp( i, ON );
					}
					TTL_ChangeSetp( 1, TTL_SP1_SELECT );
					MUS_SetMouseLayer( 1 );
					break;
			}
			break;
		
		case TTL_SYS_CG:
			switch( OMK_ControlCgMode() ){
				case -1:
					for(i=5;i<10;i++){
						DSP_SetGraphDisp( i, ON );
					}
					break;
				case 0:
					for(i=5;i<10;i++){
						DSP_SetGraphDisp( i, OFF );
					}
					break;
				case 1:
					for(i=5;i<10;i++){
						DSP_SetGraphDisp( i, ON );
					}
					TTL_ChangeSetp( 1, TTL_SP1_SELECT );
					MUS_SetMouseLayer( 1 );
					break;
			}
			break;

		
		case TTL_SYS_REPLAY:
			{
				int	mode_no = OMK_ControlReplayMode();

				switch( mode_no ){
					case -1:
						for(i=0;i<10;i++){
							DSP_SetGraphDisp( i, ON );
						}
						break;
					case 0:
						for(i=5;i<10;i++){
							DSP_SetGraphDisp( i, OFF );
						}
						break;
					case 1:	
						for(i=5;i<10;i++){
							DSP_SetGraphDisp( i, ON );
						}
						mus_lay = 1;
						TTL_MouseSettingMode2();
						TTL_ChangeSetp( 1, TTL_SP1_SELECT );
						MUS_SetMouseLayer( 1 );
						break;
					default:	
						for(i=0;i<15;i++){
							DSP_SetGraphDisp( i, OFF );
						}
						Title.end_mode = END_OMAKESTART;	
						Title.omake    = mode_no-2;
						TTL_ChangeSetp( 1, TTL_SYS_END );
						break;
				}
			}
			break;

		
		case TTL_SYS_FADEOUT:
			DSP_SetGraphPosZoom( 0, 0, 0, 800, 256,
									0, 128*(op_count/2%189), 400, 128 );
			op_count++;
			if(TitleCounter[1]==0){
				AVG_StopBGM(60);
			}
			cnt = LIM(128-TitleCounter[1]*256/32, 0, 128 );
			DSP_SetGraphGlobalBright( cnt,cnt,cnt );

			if( Title.end_mode==END_GAMEEND ){
				if( TTL_ChangeSetpCounter( 1, TTL_SYS_END2, 32 ) ){
					SendMessage( (HWND)MainWindow.hwnd, WM_COMMAND, ID_END2, 0 );
				}
			}else{
				if( TTL_ChangeSetpCounter( 1, TTL_SYS_END, 32 ) ){
				}
			}
			break;
		case TTL_SYS_END2:
			if(omake==2){
				cnt = LIM(TitleCounter[1]*128/45, 0, 128 );
				DSP_SetGraph( 0, 0, 0, ON, CHK_NO );
				DSP_SetGraphFade( 0, cnt);
			}else{
				cnt = LIM(TitleCounter[1]*128/15, 0, 128 );
				DSP_SetGraph( 0, 0, 0, ON, CHK_NO );
				DSP_SetGraph( 1, 1, 1, ON, CHK_NO );

				cnt = LIM(TitleCounter[1]*128/15, 0, 128 );
				DSP_SetGraphFade( 0, cnt);
				cnt = LIM((TitleCounter[1]-30)*256/30, 0, 256 );
				DSP_SetGraphParam( 1, DRW_BLD(cnt) );
			}



			if( TTL_ChangeSetpCounter( 1, TTL_SYS_END2, 60) || click || cansel ){
				SendMessage( (HWND)MainWindow.hwnd, WM_COMMAND, ID_END2, 0 );
			}
			break;
		case TTL_SYS_END:
			if( !AVG_WaitBGM() && Title.end_mode!=END_OMAKESTART ){
				break;
			}

			DSP_TermDisp();
			
			
			switch(Title.end_mode){
				case END_MOVIE_MODE:
					Title.end_mode=END_INITLOOP;
					TTL_ChangeSetp( 1, TTL_MOVIE_INIT );
					break;
				case END_INITSTART:
				case END_LOADSTART:
				case END_OMAKESTART:
					ret=FALSE;
				case END_INITLOOP:
					TTL_ChangeSetp( 1, TTL_LEAF_INIT );
					break;
				case END_GAMEEND:
					break;
			}
			break;
	}
	return ret;
}

void TTL_DrawMain( void *dest )
{

}

extern BOOL CALLBACK NameDialogBoxProc( HWND hwnd, UINT wmes, UINT wparam, LONG lparam );

int	TTL_System( void )
{
	int	ret = GAME_TITLE;

	AVG_GetGameKey();

	switch( TitleStep[0] ){
		default:
		case TTL_INIT:
			TTL_Init();
			TTL_ChangeSetp( 0, TTL_LOAD );
			break;
		case TTL_LOAD:
			TTL_Load();
			TTL_ChangeSetp( 0, TTL_MAIN );
			break;
		case TTL_MAIN:
			if( !TTL_SystemMain() )
				TTL_ChangeSetp( 0, TTL_TERM );

			AVG_ControlMusic();
			AVG_ColtrolFade();
			break;
		case TTL_TERM:
			TTL_Term( );
			switch( Title.end_mode ){
				case END_INITSTART:
					DialogBox( AppStruct.inst, MAKEINTRESOURCE(IDD_DIALOG12), MainWindow.hwnd, NameDialogBoxProc );

					Avg.omake=OFF;
					MAIN_StartScenario();
					ret = GAME_AVG;
					break;
				case END_LOADSTART:
					Avg.omake=OFF;
					ret = GAME_AVG;
					break;
				case END_OMAKESTART:
					Avg.omake=ON;
					MAIN_StartOmakeScenario(Title.omake);
					MUS_SetMouseLayer(0);
					ret = GAME_AVG;
					break;
			}
			Title.end_mode=END_INITLOOP;
			TTL_ChangeSetp( 0, TTL_INIT );
			break;
	}
	TTL_RenewSetp();

	return ret;
}
