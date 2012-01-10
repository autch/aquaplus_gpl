

#include "mm_std.h"
#include "main.h"

#include "bmp.h"
#include "draw.h"

#include "Palette.h"
#include "Escript.h"
#include "disp.h"
#include "dispSprite.h"
#include "font.h"
#include "keybord.h"
#include "mouse.h"


#include "GM_Title.h"
#include "GM_demo.h"
#include "GM_avg.h"
#include "GM_save.h"

#include <math.h>

#include "main.h"


static EXEC_DATA 	ScriptData;		
static int			ScriptFlag = 1;

int	MainFrames     = 0;
int	SystemFrames   = 0;
int	DrawCount      = 0;

int		GlobalCount=0;
int		GlobalCount2=0;
DWORD	NowTime=0;

static char	*Vrambuf=NULL;	

int		MainStep     = MAIN_INIT;
int		NextMainStep = MAIN_INIT;



	int		GameStep     = GAME_TITLE;

DEBUG_TEXT	Debug={OFF,ON};


int	TentouFlag=OFF;





void MAIN_Init( void )
{

	
	DRW_InitDigit( AppStruct.inst, MAKEINTRESOURCE(IDB_BITMAP1) );
	DRW_SetRippleTable( DISP_X, DISP_Y );

	
	if( DirectDrawFlag && (MainWindow.draw_mode != MainWindow.draw_mode2) ){
		Vrambuf = (char*)GAlloc( DISP_X*DISP_Y*4 );
	}
	DSP_InitDisp( GRP_DISP, TXT_DISP, BMP_DISP, LAY_DISP );


	

	WIN_InitMenu();


	{



		if(MainWindow.full_screen){
			if(STD_CheckOSw2k()){
				SendMessage( MainWindow.hwnd, WM_COMMAND, ID_SUBMENU+MainWindow.full_screen-1, 0 );
			}else{
				SendMessage( MainWindow.hwnd, WM_COMMAND, ID_DISP_FULL, 0 );
			}
		}
	}

	FNT_Init( );

	if( GameStep == GAME_AVG ){	MAIN_StartScenario(); }	
	MUS_SetSomeTimeClickFlag( OFF );					


}



void MAIN_Term( void )
{
	AVG_WriteConfigParam();
	EXEC_ReleaseLang( &ScriptData );


	DSP_TermDisp( );
	PAL_Terminate( );
	DRW_TermDigit( );
	FNT_Term();
	GFree( Vrambuf );
	SendMessage( MainWindow.hwnd, WM_DESTROY, 0, 0 );
}

static int	EndFlag = 0;

void MAIN_SetEnd( void )
{
DebugPrintf( "end\n" );
	EndFlag = 1;
}

int MAIN_GetEnd( void )
{
	return EndFlag;
}

void MAIN_SetScriptFlag( int flag )
{
	ScriptFlag = flag;
}
int MAIN_GetScriptFlag( void )
{
	return ScriptFlag;
}
int GetScenarioNo( char *sname )
{
	int	i=0, j;
	char	buf[256];
	i=0;
	j=0;
	while( sname[i] && sname[i]!='.' ){
		if( '0' <= sname[i] && sname[i] <= '9' ){
			buf[j] = sname[i];
			j++;
		}
		i++;
	}
	buf[j]=NULL;

	return Str2Digit(buf);
}

void MAIN_StartScenario( void )
{

	int		i=0;



	

	ESC_InitEOprFlag();



	AVG_SetScenarioNo(GetScenarioNo( ScriptFileName ));
	EXEC_ReadLang( ScriptFileName, &ScriptData );
	EXEC_StartLang( &ScriptData, MAIN_SCRIPT );
}
void MAIN_StartOmakeScenario( int omake_no )
{
	char	buf[256];
	ESC_InitEOprFlag();

	AVG_SetScenarioNo(omake_no);
	wsprintf(buf,"8000%05d.sdt",omake_no);
	EXEC_ReadLang( buf, &ScriptData );
	EXEC_StartLang( &ScriptData, MAIN_SCRIPT );
}

void MAIN_TermScript( void )
{
	EXEC_EndLang( &ScriptData, ON );
	EXEC_ReleaseLang( &ScriptData );
}




int MAIN_GameControl( int script )
{
	int	ret = MAIN_GAME;




	switch( GameStep ){
		default:
		case GAME_DEMO:
			DEM_System( );
			break;

		case GAME_TITLE:
			GameStep = TTL_System();
			break;

		case GAME_OPENNING:
			break;

		case GAME_AVG:
			GameStep = AVG_System(script);
			break;

		case GAME_ENDING:
			break;

		case GAME_OMAKE:
			break;
	}
	if( EndFlag ) ret = MAIN_TERM;

	
	return ret;
}




void MAIN_SystemControl( void )
{
	static int	script = SCCODE_NOOPR;

	KEY_RenewKeybord( MainWindow.active );
	MUS_RenewMouse( MainWindow.hwnd, MainWindow.active, 0 );
	SPR_SetAntiFlag( Avg.level );
	SPR_RenewSprite( Avg.frame, Avg.interless );

#if !DIRECT_SOUND
	PCM_ReleaseWaveAuto();
#endif

	switch( MainStep ){
		default:
		case MAIN_INIT:
			MAIN_Init();
			NextMainStep = MAIN_GAME;
			break;

		case MAIN_GAME:
			



				Debug.flag = 0;

				if(KeyCond.trg.f) Debug.frame = !Debug.frame;	
				if(KeyCond.trg.p) Debug.pos   = !Debug.pos;		
				if(KeyCond.trg.n) Debug.number = (Debug.number+1)%3;
				if(KeyCond.trg.s) Debug.string = !Debug.string;
				if(KeyCond.trg.a) Debug.scenario = (Debug.scenario+1)%3;
				if(KeyCond.trg.g) Debug.game = !Debug.game;
				if(KeyCond.trg.r) Debug.reg = (Debug.reg+1)%3;

				if(KeyCond.trg.c) ZeroMemory( &Debug, sizeof(Debug) );


			if(ScriptFlag){
				script = EXEC_ControlLang( &ScriptData );
			}
			NextMainStep = MAIN_GameControl( script );
			break;
		case MAIN_TERM:
			DebugPrintf("main_term\n");
			MAIN_Term();
			break;
	}
}




int MAIN_DrawGraph( void *dest )
{
	int	ret=FALSE;
	int	i;

	switch( MainStep ){
		default:
		case MAIN_INIT:
			break;
		case MAIN_GAME:
			if( AVG_GetMesCut() ){
				DSP_DrawGraph( dest, MainWindow.draw_mode2, MainWindow.save_disp, Avg.frame, Avg.interless, 10 );
			}else{
				DSP_DrawGraph( dest, MainWindow.draw_mode2, MainWindow.save_disp, Avg.frame, Avg.interless, 1 );
			}
			
			if(Debug.flag){

				char	work[256];
				int		y=8;

				wsprintf( work, "DEBUG MODE!" );
				DRW_DrawDmoji(  dest, 8,  y, work,  0, MainWindow.draw_mode2 );	y+=8;
				wsprintf( work, "SDT FILE : %s", NowLangFileName );
				DRW_DrawDmoji(  dest, 8,  y, work,  0, MainWindow.draw_mode2 );	y+=8;
				if( Debug.frame ){
					wsprintf( work, "%-3d:%-3d", MainFrames, SystemFrames );
					DRW_DrawDmoji(  dest, 8,  y, work,  0, MainWindow.draw_mode2 );	y+=8;
				}
				if(Debug.game){
					DRW_DrawDmoji(  dest, 8,  y, "*GAME FLAG",  0, MainWindow.draw_mode2 );	y+=8;
					wsprintf( work, "    00  01  02  03  04  05  06  07  08  09  10  11  12  13  14  15" );
					DRW_DrawDmoji(  dest, 8,  y, work,  0, MainWindow.draw_mode2 );	y+=8;
					for(i=0;i<256;i+=16){
						wsprintf( work, "%02d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d", i/16,
							ESC_GameFlagBuf[i   ], ESC_GameFlagBuf[i+ 1], ESC_GameFlagBuf[i+ 2], ESC_GameFlagBuf[i+ 3],
							ESC_GameFlagBuf[i+ 4], ESC_GameFlagBuf[i+ 5], ESC_GameFlagBuf[i+ 6], ESC_GameFlagBuf[i+ 7],
							ESC_GameFlagBuf[i+ 8], ESC_GameFlagBuf[i+ 9], ESC_GameFlagBuf[i+10], ESC_GameFlagBuf[i+11],
							ESC_GameFlagBuf[i+12], ESC_GameFlagBuf[i+13], ESC_GameFlagBuf[i+14], ESC_GameFlagBuf[i+15] );
						DRW_DrawDmoji(  dest, 8,  y, work,  0, MainWindow.draw_mode2 );	y+=8;
					}
				}

				if( Debug.pos ){

					wsprintf( work, "x=%-3d y=%-3d", (int)ChipBackStruct.wx, (int)ChipBackStruct.wy );
					DRW_DrawDmoji(  dest, 8,  y, work,  0, MainWindow.draw_mode2 );	y+=8;
				}
				if(Debug.number==1){
					wsprintf( work, "( 0:%08x) ( 1:%08x) ( 2:%08x) ( 3:%08x)", Debug.num[0], Debug.num[1], Debug.num[2], Debug.num[3] );
					DRW_DrawDmoji(  dest, 8,  y, work,  0, MainWindow.draw_mode2 );	y+=8;
					wsprintf( work, "( 4:%08x) ( 5:%08x) ( 6:%08x) ( 7:%08x)", Debug.num[4], Debug.num[5], Debug.num[6], Debug.num[7] );
					DRW_DrawDmoji(  dest, 8,  y, work,  0, MainWindow.draw_mode2 );	y+=8;
					wsprintf( work, "( 8:%08x) ( 9:%08x) (10:%08x) (11:%08x)", Debug.num[8], Debug.num[9], Debug.num[10], Debug.num[11] );
					DRW_DrawDmoji(  dest, 8,  y, work,  0, MainWindow.draw_mode2 );	y+=8;
					wsprintf( work, "(12:%08x) (13:%08x) (14:%08x) (15:%08x)", Debug.num[12], Debug.num[13], Debug.num[14], Debug.num[15] );
					DRW_DrawDmoji(  dest, 8,  y, work,  0, MainWindow.draw_mode2 );	y+=8;
				}else if(Debug.number==2){
					wsprintf( work, "( 0:%d) ( 1:%d) ( 2:%d) ( 3:%d)", Debug.num[0], Debug.num[1], Debug.num[2], Debug.num[3] );
					DRW_DrawDmoji(  dest, 8,  y, work,  0, MainWindow.draw_mode2 );	y+=8;
					wsprintf( work, "( 4:%d) ( 5:%d) ( 6:%d) ( 7:%d)", Debug.num[4], Debug.num[5], Debug.num[6], Debug.num[7] );
					DRW_DrawDmoji(  dest, 8,  y, work,  0, MainWindow.draw_mode2 );	y+=8;
					wsprintf( work, "( 8:%d) ( 9:%d) (10:%d) (11:%d)", Debug.num[8], Debug.num[9], Debug.num[10], Debug.num[11] );
					DRW_DrawDmoji(  dest, 8,  y, work,  0, MainWindow.draw_mode2 );	y+=8;
					wsprintf( work, "(12:%d) (13:%d) (14:%d) (15:%d)", Debug.num[12], Debug.num[13], Debug.num[14], Debug.num[15] );
					DRW_DrawDmoji(  dest, 8,  y, work,  0, MainWindow.draw_mode2 );	y+=8;
				}
				if(Debug.scenario){
					DRW_DrawDmoji(  dest, 8,  y, "*FLAG",  0, MainWindow.draw_mode2 );	y+=8;
					if(Debug.scenario==1){
						wsprintf( work, "    00  01  02  03  04  05  06  07  08  09  10  11  12  13  14  15" );
						DRW_DrawDmoji(  dest, 8,  y, work,  0, MainWindow.draw_mode2 );	y+=8;
						for(i=0;i<256;i+=16){
							wsprintf( work, "%02d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d", i/16,
								ESC_FlagBuf[i   ], ESC_FlagBuf[i+ 1], ESC_FlagBuf[i+ 2], ESC_FlagBuf[i+ 3],
								ESC_FlagBuf[i+ 4], ESC_FlagBuf[i+ 5], ESC_FlagBuf[i+ 6], ESC_FlagBuf[i+ 7],
								ESC_FlagBuf[i+ 8], ESC_FlagBuf[i+ 9], ESC_FlagBuf[i+10], ESC_FlagBuf[i+11],
								ESC_FlagBuf[i+12], ESC_FlagBuf[i+13], ESC_FlagBuf[i+14], ESC_FlagBuf[i+15] );
							DRW_DrawDmoji(  dest, 8,  y, work,  0, MainWindow.draw_mode2 );	y+=8;
						}
					}else{
						wsprintf( work, "   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F" );
						DRW_DrawDmoji(  dest, 8,  y, work,  0, MainWindow.draw_mode2 );	y+=8;
						for(i=0;i<256;i+=16){
							wsprintf( work, "%x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x", i/16,
								ESC_FlagBuf[i   ], ESC_FlagBuf[i+ 1], ESC_FlagBuf[i+ 2], ESC_FlagBuf[i+ 3],
								ESC_FlagBuf[i+ 4], ESC_FlagBuf[i+ 5], ESC_FlagBuf[i+ 6], ESC_FlagBuf[i+ 7],
								ESC_FlagBuf[i+ 8], ESC_FlagBuf[i+ 9], ESC_FlagBuf[i+10], ESC_FlagBuf[i+11],
								ESC_FlagBuf[i+12], ESC_FlagBuf[i+13], ESC_FlagBuf[i+14], ESC_FlagBuf[i+15] );
							DRW_DrawDmoji(  dest, 8,  y, work,  0, MainWindow.draw_mode2 );	y+=8;
						}
					}
				}
				if(Debug.reg){
					DRW_DrawDmoji(  dest, 8,  y, "*REGISTER",  0, MainWindow.draw_mode2 );	y+=8;
					if(Debug.reg==1){
						wsprintf( work, "    00  01  02  03  04  05  06  07  08  09" );
						DRW_DrawDmoji(  dest, 8,  y, work,  0, MainWindow.draw_mode2 );	y+=8;
						for(i=0;i<5;i++){
							wsprintf( work, "%02d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d", i,
								EXEC_LangInfo->reg[0+i*10], EXEC_LangInfo->reg[1+i*10], EXEC_LangInfo->reg[2+i*10],
								EXEC_LangInfo->reg[3+i*10], EXEC_LangInfo->reg[4+i*10], EXEC_LangInfo->reg[5+i*10],
								EXEC_LangInfo->reg[6+i*10], EXEC_LangInfo->reg[7+i*10], EXEC_LangInfo->reg[8+i*10], EXEC_LangInfo->reg[9+i*10] );
							DRW_DrawDmoji(  dest, 8,  y, work,  0, MainWindow.draw_mode2 );	y+=8;
						}
					}else{
						wsprintf( work, "   0  1  2  3  4  5  6  7  8  9" );
						DRW_DrawDmoji(  dest, 8,  y, work,  0, MainWindow.draw_mode2 );	y+=8;
						for(i=0;i<5;i++){
							wsprintf( work, "%x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x", i,
								EXEC_LangInfo->reg[0+i*10], EXEC_LangInfo->reg[1+i*10], EXEC_LangInfo->reg[2+i*10],
								EXEC_LangInfo->reg[3+i*10], EXEC_LangInfo->reg[4+i*10], EXEC_LangInfo->reg[5+i*10],
								EXEC_LangInfo->reg[6+i*10], EXEC_LangInfo->reg[7+i*10], EXEC_LangInfo->reg[8+i*10], EXEC_LangInfo->reg[9+i*10] );
							DRW_DrawDmoji(  dest, 8,  y, work,  0, MainWindow.draw_mode2 );	y+=8;
						}
					}
				}
				if(Debug.string){
					for(i=0;i<4;i++){
						if(Debug.str[i])wsprintf( work, "s%d:%s",  i, Debug.str[i] );
						else			wsprintf( work, "s%d:NULL",i );
						DRW_DrawDmoji(  dest, 8,  y, work,  0, MainWindow.draw_mode2 );	y+=8;
					}
				}

			}
			ret = TRUE;
			break;
		case MAIN_TERM:
			break;
	}
	return ret;
}




void MAIN_DrawControl( void )
{
	
	if( (MainWindow.draw_mode == 8) && (MainWindow.draw_mode != MainWindow.draw_mode2) ){
		switch( MainWindow.draw_mode2 ){
			default:	DebugBox( NULL, "画面モードが異常です" );	break;
			case 16:	if( MAIN_DrawGraph(&MainWindow.vram_high) )	DRW_ChangeBMP_H2B( &MainWindow.vram_256, &MainWindow.vram_high );	break;
			case 24:	if( MAIN_DrawGraph(&MainWindow.vram_full) )	DRW_ChangeBMP_F2B( &MainWindow.vram_256, &MainWindow.vram_full );	break;
			case 32:	if( MAIN_DrawGraph(&MainWindow.vram_true) )	DRW_ChangeBMP_T2B( &MainWindow.vram_256, &MainWindow.vram_true );	break;
		}
	}else{
		switch( MainWindow.draw_mode2 ){
			default:	DebugBox( NULL, "画面モードが異常です" );	break;
			case 8:		MAIN_DrawGraph( &MainWindow.vram_256  );	break;
			case 16:	MAIN_DrawGraph( &MainWindow.vram_high );	break;
			case 24:	MAIN_DrawGraph( &MainWindow.vram_full );	break;
			case 32:	MAIN_DrawGraph( &MainWindow.vram_true );	break;
		}
	}
	SendMessage( MainWindow.hwnd, WM_PAINT,0,0);
}

DWORD timeGetTime2(void)
{
	DWORD	ret;

	ret = timeGetTime();

	return ret;
}
#define FRAME_COUNT	0
#define TIME_COUNT	1


	int	FrameFormat = FRAME_COUNT;





void MAIN_Loop( void )
{
	static int		FrameLimit  = 1000;
	static DWORD	FrameTime   = 0;
	static int		FrameCount  = 0;
	static int		SystemCount = 0;

	RandCount += timeGetTime2();
	if( FrameFormat == FRAME_COUNT ){
		static DWORD	next_time = 0;
		DWORD			time = 0;
		DWORD			skip=0, frame;
		static int		skip_cnt=0;
		static int		skip_frame=0;
		static int		activ=0;

		NowTime = (DWORD)timeGetTime2();
		if( FrameTime == 0 ) FrameTime = NowTime;
		if( next_time == 0 ) next_time = NowTime;

	
	
	
	
	

		if ( next_time <= NowTime ){
			skip = NowTime-next_time;

			if(ChipBackStruct.flag){
				frame = (DWORD)Avg.frame;
				if(MainWindow.draw_matrix==1)	frame = frame/4;
			}else{
				frame = (DWORD)Avg.frame;
			}

			next_time = next_time + 1000/frame;
			time = NowTime - FrameTime;

			if( time>=1000 ) {
				MainFrames   = FrameCount;
				SystemFrames = SystemCount;
				FrameTime  = NowTime;
				FrameCount = 0;
				SystemCount= 0;
	
			}
				MAIN_SystemControl();

				SystemCount++;
				GlobalCount++;
				TimerCount += 1000.0/30;
				GlobalCount2 = (int)(GlobalCount*60/frame);
	

			if(MainWindow.save_disp) MainWindow.draw_flag = 1;

			if(MainWindow.save_disp==2)	MainWindow.save_disp = 0;
			if( MainWindow.save_disp )	MainWindow.save_disp = 2;

			if( MainWindow.draw_flag == 2 )	MainWindow.draw_flag = 0;
			

			if( MainWindow.draw_flag==1 ){
				MainWindow.draw_flag = 2;
				skip_cnt = 0;
				skip = 0;
				next_time = timeGetTime2() + 1000/frame;
	
			}

			if( skip_cnt<=0 ) {
				if(skip_frame)	skip_cnt = (int)min( skip*frame/1000, 40 );
				else			skip_cnt = (int)min( skip*frame/1000, 10 );
	
				if( next_time+500 < NowTime ){
					next_time = NowTime + 1000/frame;
				}
				FrameCount++;
				DrawCount++;
				if( MainWindow.draw_flag<0 ){
					MainWindow.draw_flag++;
					next_time = NowTime;
					if(MainWindow.draw_flag==0){
						MAIN_DrawControl();
					}
				}else{
					MAIN_DrawControl();
				}
			}else{
				skip_cnt--;
			}

			MainStep = NextMainStep;
		}else{
			if(Avg.frame==60){
				Sleep( 2 );
			}else{
				Sleep( 4 );
			}
		}
	}else{
		static DWORD	bak_time=0;
		
		NowTime = (DWORD)timeGetTime2();

		MAIN_SystemControl();
		MAIN_DrawControl();

		GlobalCount++;
		DrawCount++;

		TimerCount += NowTime - bak_time;

		FrameCount++;
		SystemCount += int(NowTime - bak_time);

		if(FrameCount>5){
			if( SystemCount!=0 ){
				MainFrames   = SystemFrames = 5000/SystemCount;
				FrameCount=0;
				SystemCount=0;
			}
		}
		
		MainStep = NextMainStep;
		bak_time = NowTime;

		Sleep(0);
	}
}
