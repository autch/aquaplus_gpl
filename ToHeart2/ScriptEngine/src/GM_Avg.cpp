
 
  
   
    
     
      

#include "mm_std.h"
#include "main.h"

#include "bmp.h"
#include "draw.h"
#include "escript.h"
#include "disp.h"
#include "text.h"
#include "keybord.h"
#include "mouse.h"




#include <soundDS.h>

#include "escript.h"
#include "font.h"


#include "movPlayer.h"
#include "main.h"
#include "GM_avg.h"
#include "GM_Save.h"
#include "GM_title.h"
#include "GM_Window.h"

int	EndNext=0;

//AVG_STRUCT	Avg = { 60, MES_NORMAL, DRAW_HIGHEND, 0,
AVG_STRUCT	Avg = { 60, MES_NORMAL, DRAW_HIGHEND, 0,
					OFF,
					ON, WIN_BLEND, __RESERV,

					1,1,
					OFF, MES_NORMAL, OFF, OFF, 0, 64,
					BGM_PCM, ON, ON, ON, 0, PCM_44_16, ON,
					128+64-16, 256, 256,
					DEMO_NOT, 0, 30,
					OFF,
					0,
					1,0,0,0,
					ON, OFF,
					OFF,120,240,
};
int	CharVoiceFlag[CHAR_VOICE_MAX]={ON,ON,ON,ON,ON, ON,ON,ON,ON,ON,ON};


void AVG_SetDemoFlag( int flag, int demo_count )
{
	Avg.msg_cut = 0;
	Avg.demo = flag;
	Avg.demo_cnt = 0;
	Avg.demo_max = demo_count;
}

void AVG_ReadConfigParam( void )
{
	if(1){
		SetIniFileName( NULL, "CONFIG" );






	
			ReadIniFileData( "Debug", (char*)&Debug, sizeof(DEBUG_TEXT) );
	
	
	
	
		MainWindow.full_screen = ReadIniFileAmount( "screen", MainWindow.full_screen );

		Avg.wait = ReadIniFileAmount( "frame",  Avg.frame );

		Avg.wait = ReadIniFileAmount( "wait",  Avg.wait );

		Avg.wheel = ReadIniFileAmount( "wheel", Avg.wheel );
		Avg.msg_cut_optin = ReadIniFileAmount( "msg_cut_optin", Avg.msg_cut_optin );

		Avg.msg_page  = ReadIniFileAmount( "msg_page",  Avg.msg_page );
		Avg.msg_wait  = ReadIniFileAmount( "msg_wait",  Avg.msg_wait );
		Avg.msg_font  = ReadIniFileAmount( "msg_font",  Avg.msg_font );
		
		Avg.half_tone = ReadIniFileAmount( "half_tone", Avg.half_tone );

		Avg.sound     = ReadIniFileAmount( "sound",     Avg.sound     );
		Avg.bgm       = ReadIniFileAmount( "bgm",       Avg.bgm       );
	
		Avg.se        = ReadIniFileAmount( "se",        Avg.se        );
		Avg.se_buf    = ReadIniFileAmount( "se_buf",    Avg.se_buf    );
		Avg.pcm_fmt   = ReadIniFileAmount( "pcm_fmt",   Avg.pcm_fmt   );
		Avg.voice     = ReadIniFileAmount( "voice",     Avg.voice     );
		
		Avg.bgm_vol   = ReadIniFileAmount( "bgm_vol",   Avg.bgm_vol   );
		Avg.se_vol    = ReadIniFileAmount( "se_vol",    Avg.se_vol    );
		Avg.voice_vol = ReadIniFileAmount( "voice_vol", Avg.voice_vol );

		
		Avg.side_option = ReadIniFileAmount( "side_option", Avg.side_option );

		int	mov_cpu;
		if(fCpuClock>1400){
			mov_cpu=2;
		}else if(fCpuClock>900){
			mov_cpu=1;
		}else{
			mov_cpu=0;
		}
		Avg.mov_lv = ReadIniFileAmount( "mov_lv", mov_cpu );

		Avg.mov1 = PAC_CheckFile( "mov", "TH2_OP_480x272_2M.avi" );
		Avg.mov2 = PAC_CheckFile( "mov", "TH2_OP_640x352_4M.avi" );
		Avg.mov3 = PAC_CheckFile( "mov", "TH2_OP_800x448_5M.avi" );

		OverLay = ReadIniFileAmount( "OverLay", OverLay );

		switch(Avg.mov_lv){
		case 0:
			if(Avg.mov1) break;
			if(Avg.mov2) { Avg.mov_lv=1; break; }
			if(Avg.mov3) { Avg.mov_lv=2; break; }
			break;
		case 1:
			if(Avg.mov2) { break; }
			if(Avg.mov3) { Avg.mov_lv=2; break; }
			if(Avg.mov1) { Avg.mov_lv=0; break; }
			break;
		case 2:
			if(Avg.mov3) { break; }
			if(Avg.mov2) { Avg.mov_lv=1; break; }
			if(Avg.mov1) { Avg.mov_lv=0; break; }
			break;
		}
		Avg.tx_pdwon = ReadIniFileAmount( "tx_pdwon", Avg.tx_pdwon );
		Avg.tx_lbtrg = ReadIniFileAmount( "tx_lbtrg", Avg.tx_lbtrg );

		Avg.auto_key  = ReadIniFileAmount( "auto_key",  Avg.auto_key  );
		Avg.auto_page = ReadIniFileAmount( "auto_page", Avg.auto_page );

		int		i;
		char	buf[32];
		for(i=0;i<CHAR_VOICE_MAX;i++){
			wsprintf( buf, "CharVoiceFlag_%d", i );
			CharVoiceFlag[i] = ReadIniFileAmount( buf, CharVoiceFlag[i] );
		}
	}else{
		SetRegistryKey( REG_KEY_CONF );
		
		ReadRegistryName();

	
			ReadRegistryData( "Debug", (char*)&Debug, sizeof(DEBUG_TEXT) );
	
	
	
	
		ReadRegistryAmount( "screen", (long *)&MainWindow.full_screen );
	
		ReadRegistryAmount( "wait", &Avg.wait );

		ReadRegistryAmount( "wheel", &Avg.wheel );
		ReadRegistryAmount( "msg_cut_optin", &Avg.msg_cut_optin );

		ReadRegistryAmount( "msg_page", &Avg.msg_page );
		ReadRegistryAmount( "msg_wait", &Avg.msg_wait );
		ReadRegistryAmount( "msg_font", &Avg.msg_font );

		ReadRegistryAmount( "half_tone", &Avg.half_tone );

		ReadRegistryAmount( "sound", &Avg.sound );
		ReadRegistryAmount( "bgm", &Avg.bgm );
	
		ReadRegistryAmount( "se", &Avg.se );
		ReadRegistryAmount( "se_buf", &Avg.se_buf );
		ReadRegistryAmount( "pcm_fmt", &Avg.pcm_fmt );
		ReadRegistryAmount( "voice", &Avg.voice );

		ReadRegistryAmount( "bgm_vol",   &Avg.bgm_vol );
		ReadRegistryAmount( "se_vol",    &Avg.se_vol );
		ReadRegistryAmount( "voice_vol", &Avg.voice_vol );

		
		ReadRegistryAmount( "side_option", &Avg.side_option );

		ReadRegistryAmount( "mov_lv", &Avg.mov_lv );
		
		switch(Avg.mov_lv){
		case 0:
			if(Avg.mov1) break;
			if(Avg.mov2) { Avg.mov_lv=1; break; }
			if(Avg.mov3) { Avg.mov_lv=2; break; }
			break;
		case 1:
			if(Avg.mov2) { break; }
			if(Avg.mov3) { Avg.mov_lv=2; break; }
			if(Avg.mov1) { Avg.mov_lv=0; break; }
			break;
		case 2:
			if(Avg.mov3) { break; }
			if(Avg.mov2) { Avg.mov_lv=1; break; }
			if(Avg.mov1) { Avg.mov_lv=0; break; }
			break;
		}


		ReadRegistryAmount( "tx_pdwon", &Avg.tx_pdwon );
		ReadRegistryAmount( "tx_lbtrg", &Avg.tx_lbtrg );


		int		i;
		char	buf[32];
		for(i=0;i<CHAR_VOICE_MAX;i++){
			wsprintf( buf, "CharVoiceFlag_%d", i );
			ReadRegistryAmount( buf, (long*)&CharVoiceFlag[i] );
		}
	}
}
void AVG_WriteConfigParam( void )
{
	if(1){
		SetIniFileName( NULL, "CONFIG" );



		WriteIniFileData( "Debug", (char*)&Debug, sizeof(DEBUG_TEXT) );
		WriteIniFileAmount( "screen", MainWindow.full_screen );


		WriteIniFileAmount( "frame",  Avg.frame );

		WriteIniFileAmount( "wait",  Avg.wait );
		WriteIniFileAmount( "level", Avg.level );
		WriteIniFileAmount( "ami",   Avg.ami );

		WriteIniFileAmount( "win_eff", Avg.win_eff );
		WriteIniFileAmount( "win_bld", Avg.win_bld );
		WriteIniFileAmount( "win_prm", Avg.win_prm );

		WriteIniFileAmount( "wheel", Avg.wheel );
		WriteIniFileAmount( "msg_cut_optin", Avg.msg_cut_optin );

		WriteIniFileAmount( "msg_page", Avg.msg_page );
		WriteIniFileAmount( "msg_wait", Avg.msg_wait );
		WriteIniFileAmount( "msg_font", Avg.msg_font );
		WriteIniFileAmount( "voice", Avg.voice );

		WriteIniFileAmount( "half_tone", Avg.half_tone );

		WriteIniFileAmount( "bgm_vol",   Avg.bgm_vol );
		WriteIniFileAmount( "se_vol",    Avg.se_vol );
		WriteIniFileAmount( "voice_vol", Avg.voice_vol );

		WriteIniFileAmount( "sound",   Avg.sound );
		WriteIniFileAmount( "bgm",     Avg.bgm );
		WriteIniFileAmount( "cd_fade", Avg.cd_fade );
		WriteIniFileAmount( "se",      Avg.se );
		WriteIniFileAmount( "se_buf",  Avg.se_buf );
		WriteIniFileAmount( "pcm_fmt", Avg.pcm_fmt );

		WriteIniFileAmount( "side_option", Avg.side_option );
		WriteIniFileAmount( "mov_lv", Avg.mov_lv );

		WriteIniFileAmount( "OverLay", OverLay );


		WriteIniFileAmount( "tx_pdwon", Avg.tx_pdwon );
		WriteIniFileAmount( "tx_lbtrg", Avg.tx_lbtrg );

		WriteIniFileAmount( "auto_key",  Avg.auto_key );
		WriteIniFileAmount( "auto_page", Avg.auto_page );

		int		i;
		char	buf[32];
		for(i=0;i<CHAR_VOICE_MAX;i++){
			wsprintf( buf, "CharVoiceFlag_%d", i );
			WriteIniFileAmount( buf, CharVoiceFlag[i] );
		}
	}else{
		SetRegistryKey( REG_KEY_CONF );

		WriteRegistryName();
		WriteRegistryData( "Debug", (char*)&Debug, sizeof(DEBUG_TEXT) );
		WriteRegistryAmount( "screen", MainWindow.full_screen );

	
		WriteRegistryAmount( "wait",  Avg.wait );
		WriteRegistryAmount( "level", Avg.level );
		WriteRegistryAmount( "ami",   Avg.ami );

		WriteRegistryAmount( "win_eff", Avg.win_eff );
		WriteRegistryAmount( "win_bld", Avg.win_bld );
		WriteRegistryAmount( "win_prm", Avg.win_prm );

		WriteRegistryAmount( "wheel", Avg.wheel );
		WriteRegistryAmount( "msg_cut_optin", Avg.msg_cut_optin );

		WriteRegistryAmount( "msg_page", Avg.msg_page );
		WriteRegistryAmount( "msg_wait", Avg.msg_wait );
	
		WriteRegistryAmount( "msg_font", Avg.msg_font );
		WriteRegistryAmount( "voice", Avg.voice );

		WriteRegistryAmount( "half_tone", Avg.half_tone );

		WriteRegistryAmount( "bgm_vol",   Avg.bgm_vol );
		WriteRegistryAmount( "se_vol",    Avg.se_vol );
		WriteRegistryAmount( "voice_vol", Avg.voice_vol );

		WriteRegistryAmount( "sound",   Avg.sound );
		WriteRegistryAmount( "bgm",     Avg.bgm );
		WriteRegistryAmount( "cd_fade", Avg.cd_fade );
		WriteRegistryAmount( "se",      Avg.se );
		WriteRegistryAmount( "se_buf",  Avg.se_buf );
		WriteRegistryAmount( "pcm_fmt", Avg.pcm_fmt );

		WriteRegistryAmount( "side_option", Avg.side_option );
		WriteRegistryAmount( "mov_lv", Avg.mov_lv );

		WriteRegistryAmount( "tx_pdwon", Avg.tx_pdwon );
		WriteRegistryAmount( "tx_lbtrg", Avg.tx_lbtrg );

	
	
		int		i;
		char	buf[32];
		for(i=0;i<CHAR_VOICE_MAX;i++){
			wsprintf( buf, "CharVoiceFlag_%d", i );
			WriteRegistryAmount( buf, CharVoiceFlag[i] );
		}
	}
}

enum{
	AVG_INIT=0,
	AVG_GAME,
	AVG_MAP,
	AVG_CALENDER,
	AVG_CONFIG,
	AVG_SAVE,
	AVG_LOAD,
	AVG_SETTING,
	AVG_END,
};


GAME_KEY	GameKey;
extern int Flag_MenuSelected;

void AVG_GetGameKey(void)
{
	int	wclick   = KeyCond.btn.enter || MUS_GetMouseButton( MOUSE_LBUTTON);
	int	wcansel  = KeyCond.btn.bs    || MUS_GetMouseButton( MOUSE_RBUTTON);
	int	wheel    = MUS_GetMouseWheel();

	if( KeyCond.trg.enter && KeyCond.btn.alt ){
		KeyCond.trg.enter=0;
	}
	if(Debug.flag){
		MainWindow.save_disp = (GetAsyncKeyState(VK_NONCONVERT)&0x8001) ? 1 : 0;
	}

	GameKey.click   = KeyCond.trg.enter || MUS_GetMouseTrigger( MOUSE_LBUTTON);
	GameKey.cansel  = KeyCond.trg.esc   || KeyCond.trg.bs    || MUS_GetMouseTrigger( MOUSE_RBUTTON);

	if(TaikenBan){
		GameKey.diswin  |= MUS_GetMouseTrigger( MOUSE_RBUTTON);
	}

	switch(Avg.wheel){
		case 0:
			GameKey.diswin        = KeyCond.trg.space || MUS_GetMouseTrigger( MOUSE_MBUTTON);

			GameKey.mes_cut       = KeyCond.btn.ctrl;
			GameKey.mes_cut_mode  = KeyCond.trg.shift;	
			break;
		case 1:
			GameKey.diswin        = KeyCond.trg.space;	
			
			GameKey.mes_cut       = KeyCond.btn.ctrl  || MUS_GetMouseButton(MOUSE_MBUTTON);
			GameKey.mes_cut_mode  = KeyCond.trg.shift;
			break;
	}

	if(MainWindow.cutcut){
		GameKey.mes_cut = OFF;	
		GameKey.mes_cut_mode = OFF;
	}

	GameKey.end     = KeyCond.trg.end    && !wclick;	
	GameKey.home    = KeyCond.trg.home   && !wclick;	
	if( wheel>0 ){
		GameKey.pup     = 1 && !wclick;	
		GameKey.pdown   = 0 && !wclick;	
	}else if( wheel<0 ){
		GameKey.pup     = 0 && !wclick;	
		GameKey.pdown   = 1 && !wclick;	
	}else{
		GameKey.pup     = KeyCond.btrg.pup   && !wclick;	
		GameKey.pdown   = KeyCond.btrg.pdown && !wclick;	
	}

	if(GameKey.click&&GameKey.cansel){
		GameKey.click  = OFF;
		GameKey.cansel = ON;
	}
	if(GameKey.click &&GameKey.mes_cut){
		GameKey.mes_cut = OFF;
		GameKey.mes_cut_mode = OFF;
		GameKey.click   = ON;
	}
	if(GameKey.cansel&&GameKey.mes_cut){
		GameKey.mes_cut = OFF;
		GameKey.mes_cut_mode = OFF;
		GameKey.cansel = ON;
	}

	if(GameKey.end&&GameKey.home){
		GameKey.home = ON;
		GameKey.end  = OFF;
	}

	if(GameKey.pup&&GameKey.pdown){
		GameKey.pup   = ON;
		GameKey.pdown = OFF;
	}


	if(Avg.demo || Flag_MenuSelected){
		if(Flag_MenuSelected>0){
			Flag_MenuSelected--;
		}
		GameKey.click   = 0;
		GameKey.cansel  = 0;
		GameKey.diswin  = 0;

		GameKey.mes_cut  = 0;

		GameKey.end     = 0;	
		GameKey.home    = 0;	
		GameKey.pup     = 0;	
		GameKey.pdown   = 0;	
	}



	GameKey.u = KeyCond.btrg.up;
	GameKey.d = KeyCond.btrg.down;
	GameKey.l = KeyCond.btrg.left;
	GameKey.r = KeyCond.btrg.right;

	GameKey.num[0] = KeyCond.trg.k0 || KeyCond.trg.n0;
	GameKey.num[1] = KeyCond.trg.k1 || KeyCond.trg.n1;
	GameKey.num[2] = KeyCond.trg.k2 || KeyCond.trg.n2;
	GameKey.num[3] = KeyCond.trg.k3 || KeyCond.trg.n3;
	GameKey.num[4] = KeyCond.trg.k4 || KeyCond.trg.n4;
	GameKey.num[5] = KeyCond.trg.k5 || KeyCond.trg.n5;
	GameKey.num[6] = KeyCond.trg.k6 || KeyCond.trg.n6;
	GameKey.num[7] = KeyCond.trg.k7 || KeyCond.trg.n7;
	GameKey.num[8] = KeyCond.trg.k8 || KeyCond.trg.n8;
	GameKey.num[9] = KeyCond.trg.k9 || KeyCond.trg.n9;
}



static int		AvgStep[5];
static int		NextAvgStep[5];
static int		AvgCounter[5];

static int		BackStep;

void AVG_ChangeSetp( int index, int step )
{
	NextAvgStep[index] = step;
}

static void AVG_RenewSetp( void )
{
	int	index;

	for( index=0; index<5 ;index++ ){
		if( AvgStep[index] != NextAvgStep[index] )
			AvgCounter[index] = 0;
		else
			AvgCounter[index]++;
		AvgStep[index] = NextAvgStep[index];
	}
}

BOOL AVG_GetHitKey( void )
{
	return GameKey.click && (MUS_GetMouseNo(-1)==-1);
}








int AVG_EffCnt( int cnt )
{
	int	cut = AVG_GetMesCut();
	int	ret;

	if(Avg.demo){
		if(cnt==-1){	
			ret = MES_NORMAL*15*Avg.frame/60;
		}else if(cnt==-2){	
			ret = MES_NORMAL*30*Avg.frame/60;
		}else{		
			ret = MES_NORMAL*cnt*Avg.frame/60;
		}
	}else{
		if(cnt==-1){	
			if( cut )	ret = 0;
			else		ret = Avg.wait*15*Avg.frame/60;
		}else if(cnt==-2){	
			if( cut )	ret = 0;
			else		ret = Avg.wait*30*Avg.frame/60;
		}else{		
			if( cut )	ret = 0;
			else		ret = Avg.wait*cnt*Avg.frame/60;
		}
	}

	return ret;
}

int AVG_EffCnt2( int cnt )
{
	int	cut = AVG_GetMesCut();

	if(cnt==-1){	
		if( cut )	cnt = 15*Avg.frame/30/3;
		else		cnt = 15*Avg.frame/30;
	}else if(cnt==-2){	
		if( cut )	cnt = 30*Avg.frame/30/3;
		else		cnt = 30*Avg.frame/30;
	}else{
		if( cut )	cnt = cnt*Avg.frame/30/3;
		else		cnt = cnt*Avg.frame/30;

	}
	return cnt;
}

int AVG_EffCnt3( int cnt )
{

	if(cnt==-1){	
		cnt = 15*Avg.frame/30;
	}else if(cnt==-2){
		cnt = 30*Avg.frame/30;
	}else{
		cnt = cnt*Avg.frame/30;
	}

	return cnt;
}
int AVG_EffCnt4( int cnt )
{
	int	cut = AVG_GetMesCut();
	int	ret;
	
	if(cnt==-1){	
		if( cut )	ret = 0;
		else		ret = 15*Avg.frame/30;
	}else if(cnt==-2){	
		if( cut )	ret = 0;
		else		ret = 30*Avg.frame/30;
	}else{		
		if( cut )	ret = 0;
		else		ret = cnt*Avg.frame/30;
	}
	return ret;
}


int AVG_EffCntPuls( void )
{
	int	ret;

	if( AVG_GetMesCut() ){
		ret = 9999;
	}else{
		if(Avg.demo){
			ret = 2*30/Avg.frame;
		}else{
			switch(Avg.wait){
				default:
				case 0:	ret = 9999;				break;
				case 1:	ret = 4*30/Avg.frame;	break;
				case 2:	ret = 2*30/Avg.frame;	break;
				case 4:	ret = 1*30/Avg.frame;	break;
			}
		}
		if(ret==0) ret=1;
	}
	return ret;
}


int AVG_MsgCnt( void )
{
	int	ret;

	if( AVG_GetMesCut() ){
		ret = 9999;
	}else{
		if(Avg.demo){
			ret = 2*30/Avg.frame;
		}else{
			switch(Avg.msg_wait){
				default:
				case 0:	ret = 9999;			break;
				case 1:	ret = 4*30/Avg.frame;	break;
				case 2:	ret = 2*30/Avg.frame;	break;
				case 3:	ret = 1*30/Avg.frame;	break;
			}
		}
		if(ret==0) ret=1;
	}
	return ret;
}



static char *SysFlagName = "Sys.sav";


#define CG_POS		(0)
static BYTE CgFlag[2][CG_MAX/8];



void AVG_SetCgFlag( int vh, int cg_no )
{
	STD_SetBit( CgFlag[vh], 1, cg_no, ON );
	if(!TentouFlag)
		STD_WriteFilePos( SysFlagName, (char*)CgFlag, CG_POS, CG_MAX/8*2 );
}




BOOL AVG_GetCgFlag( int vh,int cg_no )
{
	return STD_GetBit( CgFlag[vh], 1, cg_no );

}



void AVG_LoadCgFlag( void )
{
	if( !STD_ReadFilePos( SysFlagName, (char*)CgFlag, CG_POS, CG_MAX/8*2 ) ){
		ZeroMemory( CgFlag, CG_MAX/8*2 );
		if(!TentouFlag)
			STD_WriteFilePos( SysFlagName, (char*)CgFlag, CG_POS, CG_MAX/8*2 );
	}
}



#define REPLAY_POS		(CG_POS+CG_MAX/8*2)
static char ReplayFlag[REPLAY_MAX];



void AVG_SetReplayFlag( int rp_no )
{
	ReplayFlag[rp_no] = ON;
	if(!TentouFlag)
		STD_WriteFilePos( SysFlagName, (char*)ReplayFlag, REPLAY_POS, REPLAY_MAX );
}




BOOL AVG_GetReplayFlag( int rp_no )
{
	return ReplayFlag[rp_no];
}



void AVG_LoadReplayFlag( void )
{
	if( !STD_ReadFilePos( SysFlagName, (char*)ReplayFlag, REPLAY_POS, REPLAY_MAX ) ){
		ZeroMemory( ReplayFlag, REPLAY_MAX );
		if(!TentouFlag)
			STD_WriteFilePos( SysFlagName, (char*)ReplayFlag, REPLAY_POS, REPLAY_MAX );
	}else{
		int	i;
		for(i=0;i<REPLAY_MAX;i++){
			if(ReplayFlag[i]!=1){
				ReplayFlag[i]=0;
			}
		}
	}
}




#define GAMEFLG_MAX		(ESC_FLAG_MAX*sizeof(int))
#define GAMEFLG_POS		(REPLAY_POS+REPLAY_MAX)
#include "escript.h"



void AVG_SetGameFlag( int no, int flag )
{
	ESC_GameFlagBuf[no] = flag;
	






	if(!TentouFlag)
		STD_WriteFilePos( SysFlagName, (char*)ESC_GameFlagBuf, GAMEFLG_POS, GAMEFLG_MAX );
}




int AVG_GetGameFlag( int ed_no )
{
	return ESC_GameFlagBuf[ed_no];
}

void AVG_LoadGameFlag( void )
{
	int i = GAMEFLG_POS;
	
	if( !STD_ReadFilePos( SysFlagName, (char*)ESC_GameFlagBuf, GAMEFLG_POS, GAMEFLG_MAX ) ){
		ZeroMemory( ESC_GameFlagBuf, GAMEFLG_MAX );
		if(!TentouFlag)
			STD_WriteFilePos( SysFlagName, (char*)ESC_GameFlagBuf, GAMEFLG_POS, GAMEFLG_MAX );
	}else{
		int	i;

		if(ESC_GameFlagBuf[20]!=1)	ESC_GameFlagBuf[20]=0;
		if(ESC_GameFlagBuf[21]!=1)	ESC_GameFlagBuf[21]=0;
		if(ESC_GameFlagBuf[22]!=1)	ESC_GameFlagBuf[22]=0;

		if(ESC_GameFlagBuf[80]>=10)	ESC_GameFlagBuf[80]=0;

		for(i=81;i<=87;i++){
			if(ESC_GameFlagBuf[i]!=1)	ESC_GameFlagBuf[i]=0;
		}
		if(ESC_GameFlagBuf[95]!=1)	ESC_GameFlagBuf[95]=0;
		if(ESC_GameFlagBuf[98]!=1)	ESC_GameFlagBuf[98]=0;
		if(ESC_GameFlagBuf[99]!=1)	ESC_GameFlagBuf[99]=0;
		for(i=128;i<=128+40;i++){
			if(ESC_GameFlagBuf[i]!=1)	ESC_GameFlagBuf[i]=0;
		}
	}
}





#define SCRIPT_MAX	1000	
#define BLOCK_MAX	512		
static BYTE ScenarioFlag[SCRIPT_MAX][BLOCK_MAX/8];

#define EXEC_POS		(GAMEFLG_POS+GAMEFLG_MAX)





static int ScenarioNameTable[SCRIPT_MAX] = {
#include "scr_name.dat"
};
static int ScenarioNo = 0;
static int BlockNo    = 0;
static long	VI_EventVoiceNo = -1;
static long	VI_EventVoiceNoAll = -1;


void AVG_SetScenarioNo( int scenario_no )
{

	int	i;
	for(i=0;i<SCRIPT_MAX;i++){
		if(ScenarioNameTable[i]==ScenarioNo){
			break;
		}
	}
	if(i!=SCRIPT_MAX){
		if(BlockNo!=-1){
			STD_SetBit( ScenarioFlag[i], 1, BlockNo, ON );
			if(!TentouFlag)
				STD_WriteFilePos( SysFlagName, (char*)ScenarioFlag, EXEC_POS, SCRIPT_MAX*BLOCK_MAX/8 );
		}
	}
	for(i=0;i<SCRIPT_MAX;i++){
		if(ScenarioNameTable[i]==scenario_no){
			break;
		}
	}
	if(i!=SCRIPT_MAX){
		BlockNo = 0;
		ScenarioNo = scenario_no;
		VI_EventVoiceNo = -1;
		VI_EventVoiceNoAll = -1;
	}
}


void AVG_SetScenarioFlag( int block_no )
{
	int	i;
	for(i=0;i<SCRIPT_MAX;i++){
		if(ScenarioNameTable[i]==ScenarioNo){
			break;
		}
	}
	if( block_no >= BLOCK_MAX ){
		DebugBox(NULL,"ちょぉいと文章が多過ぎやしませんか？[%d]",block_no);
	}else{
		if(BlockNo!=-1){
			STD_SetBit( ScenarioFlag[i], 1, BlockNo, ON );
			if(!TentouFlag)
				STD_WriteFilePos( SysFlagName, (char*)ScenarioFlag, EXEC_POS, SCRIPT_MAX*BLOCK_MAX/8 );
		}
		BlockNo = block_no;
	}
}


BOOL AVG_CheckScenarioFlag( void )
{
	int	i;
	for(i=0;i<SCRIPT_MAX;i++){
		if(ScenarioNameTable[i]==ScenarioNo){
			break;
		}
	}
	if( BlockNo >= BLOCK_MAX ){
		DebugBox(NULL,"ちょぉいとシナリオブロックが多過ぎやしませんか？[%d]",BlockNo);
		return FALSE;
	}else{
		if( BlockNo!=-1){
			return STD_GetBit( ScenarioFlag[i], 1, BlockNo ) || Avg.omake;
		}else{
			return FALSE;
		}
	}
	return TRUE;
}


void AVG_LoadScenarioFlag( void )
{
	if( !STD_ReadFilePos( SysFlagName, (char*)ScenarioFlag, EXEC_POS, SCRIPT_MAX*BLOCK_MAX/8 ) ){
		ZeroMemory( (char*)ScenarioFlag, SCRIPT_MAX*BLOCK_MAX/8 );
		if(!TentouFlag)
			STD_WriteFilePos( SysFlagName, (char*)ScenarioFlag, EXEC_POS, SCRIPT_MAX*BLOCK_MAX/8 );
	}

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


				wait_max = AVG_EffCnt4(WaitStruct.max);

				if( WaitStruct.max==1 && AVG_GetMesCut() ){
					if(MainWindow.draw_flag<0){
					}else{
						MainWindow.draw_flag=-10;
					}
				}
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




typedef struct{
	char	flag;
	char	type;
	float	x,y;
	float	ax,ay;
	DWORD	cnt;
}WEATHER_OBJ;

typedef struct{
	int		flag;
	int		wno;		
	DWORD	cnt;
	WEATHER_OBJ	obj[200];
	int		reset;
	int		reset_cnt;
	int		noreset;	
	
	float	wind;		
	int		speed;		
	int		amount;		
	
	int		twind;		
	int		tspeed;		
	int		tamount;	
}WEATHER_STRUCT;
WEATHER_STRUCT	Weather;

#define WEATH_RAIN		0
#define WEATH_SAKURA	1

int AVG_GetWeatherNoReset( void )
{
	if(Weather.flag && Weather.noreset){
		return 1;
	}
	return 0;
}

void AVG_SetWeather( int wno, int wind, int speed, int amount, int no_reset )
{


	Weather.wno  = wno;
	Weather.noreset = no_reset;
	switch(wno){
		case WEATH_RAIN:	
			break;
		case WEATH_SAKURA:	
			DSP_LoadBmp( BMP_WEATHER+0, BMP_256P, "sakura.bmp" );
			DSP_LoadBmp( BMP_WEATHER+1, BMP_256P, "sakura2.bmp" );

			if(Weather.flag){
				Weather.twind  = wind;
				Weather.tspeed = speed;
				Weather.tamount = amount;
			}else{
				Weather.wind  = Weather.twind  = wind;
				Weather.speed = Weather.tspeed = speed;
				Weather.amount  = 0;
				Weather.tamount = amount;
			}

			break;
	}
	Weather.reset=OFF;
	Weather.flag = 1;
}

void AVG_ResetWeather( void )
{

	Weather.noreset = 0;
	if(Weather.reset==OFF){
		Weather.reset=ON;
		Weather.reset_cnt=0;
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

BOOL AVG_WaitWeather( void )
{
	int	i;
	int	wait=0;
	for(i=0;i<Weather.amount;i++){
		wait |= Weather.obj[i].flag;
	}
	if(wait==0){
		for(i=0;i<200;i++)
			DSP_ResetGraph( GRP_WEATHER+i );
		ZeroMemory( &Weather, sizeof(Weather) );
		return TRUE;
	}else{
		return FALSE;
	}
}

void AVG_ControlWeather( void )
{
	int			i;



	if( Weather.flag ){
		if(Weather.wind  < Weather.twind  )	Weather.wind  = Weather.wind + 0.1f;
		if(Weather.speed < Weather.tspeed )	Weather.speed = min( Weather.tspeed,  Weather.speed +1 );

		
		if(Weather.wind  > Weather.twind  )	Weather.wind  = Weather.wind - 0.1f;
		if(Weather.speed > Weather.tspeed )	Weather.speed = max( Weather.tspeed,  Weather.speed -1 );



		Weather.cnt++;
		if(Weather.reset)
			Weather.reset_cnt++;
		if(Weather.reset_cnt>=16){
			for(i=0;i<200;i++)
				DSP_ResetGraph( GRP_WEATHER+i );
			ZeroMemory( &Weather, sizeof(Weather) );
		}
		
		switch( Weather.wno ){
			case WEATH_RAIN:	
				break;
			case WEATH_SAKURA:	
				if( Weather.amount<Weather.tamount && Weather.cnt%5==0 ){
					if(!Weather.reset) {
						i = Weather.amount;
						Weather.obj[i].flag = 1;
						Weather.obj[i].type = rand()%6;
						Weather.obj[i].x  = (float)(rand()%800);



							Weather.obj[i].y  = (float)-(rand()%100);


						Weather.obj[i].ax = (float)(rand()%((6-Weather.obj[i].type)*100)/100.0+1)/2;
						Weather.obj[i].ay = (float)(rand()%((6-Weather.obj[i].type)*100)/100.0+1)/2;
						Weather.obj[i].cnt = rand()%256;
						Weather.amount++;
					}else{
						AVG_WaitWeather();
					}
				}

				for(i=0;i<Weather.amount;i++){
					WEATHER_OBJ	*wos = &Weather.obj[i];

					DSP_ResetGraph( GRP_WEATHER+i );

					if(!wos->flag) continue;
					wos->x += (SIN(wos->cnt)*wos->ax/4096)+Weather.wind;
					wos->y += wos->ay;

					if( wos->y>600 )	{
						if(Weather.reset) {
							wos->flag = 0;
							continue;
						}else{
							wos->x  = (float)(rand()%800);
							wos->y = (float)-(rand()%100);
						}
					}


					if( wos->x>=DISP_X)	{
						if(Weather.reset) {
							wos->flag = 0;
							continue;
						}else{
							wos->x -= DISP_X+30;
						}
					}
					if( wos->x < -30)	{
						if(Weather.reset) {
							wos->flag = 0;
							continue;
						}else{
							wos->x += DISP_X+30;
						}
					}

					if(Weather.obj[i].type/3){
						DSP_SetGraph( GRP_WEATHER+i, BMP_WEATHER+0, LAY_WINDOW+10, ON, CHK_ANTI );
					}else{
						DSP_SetGraph( GRP_WEATHER+i, BMP_WEATHER+1, LAY_WINDOW+10, ON, CHK_ANTI );
					}
					DSP_SetGraphMove( GRP_WEATHER+i, (int)wos->x, (int)wos->y );
					int	sakura;
					switch(wos->type){
					case 0:
						sakura = wos->cnt/2%23;
						DSP_SetGraphSPos( GRP_WEATHER+i, 40*(sakura%10), (int)(26.6666667f*(sakura/10)), 40,27 );
						break;
					case 1:
						sakura = wos->cnt/2%20;
						DSP_SetGraphSPos( GRP_WEATHER+i, (int)(26.666666667f*(sakura%15)), 80+20*(sakura/15), 27,20 );
						break;
					case 2:
						sakura = wos->cnt/2%17;
						DSP_SetGraphSPos( GRP_WEATHER+i, (int)(13.3333333333f*(sakura%30)), 120, 13,13 );
						break;
					case 3:
						sakura = wos->cnt/2%23;
						DSP_SetGraphSPos( GRP_WEATHER+i, 30*(sakura%10), 20*(sakura/10), 30,20 );
						break;
					case 4:
						sakura = wos->cnt/2%20;
						DSP_SetGraphSPos( GRP_WEATHER+i, 20*(sakura%15), 60+15*(sakura/15), 20,15 );
						break;
					case 5:
						sakura = wos->cnt/2%17;
						DSP_SetGraphSPos( GRP_WEATHER+i, 10*(sakura%30), 90, 10,10 );
						break;
					}
					if(Weather.reset){
						DSP_SetGraphParam( GRP_WEATHER+i, DRW_BLD( 256-Weather.reset_cnt*16 ) );
					}

					wos->cnt++;
				}
				break;
		}
	}
}




typedef struct{
	int		flag;		

	int		light;		
	int		rev;		
	int		brink;		
	int		count;		
	int		max;		
}LENS_FREA;
LENS_FREA	LensFrea;

void AVG_SetLensFrea( int light, int rev, int count )
{
	LensFrea.flag  = 1;
	LensFrea.light = light;
	LensFrea.rev   = rev;
	LensFrea.count = 0;
	LensFrea.max   = count;

	DSP_LoadBmp( BMP_WEATHER,   BMP_256P, "e0000.bmp" );
	DSP_LoadBmp( BMP_WEATHER+1, BMP_256P, "e0001.bmp" );
	DSP_LoadBmp( BMP_WEATHER+2, BMP_256P, "e0002.bmp" );

	DSP_SetGraph( GRP_WEATHER, BMP_WEATHER+1, LAY_WEATHER, OFF, CHK_256 );
	DSP_SetGraphParam( GRP_WEATHER, DRW_ADD );
	DSP_SetGraphBright( GRP_WEATHER, 32, 32, 64 );

	DSP_SetGraph( GRP_WEATHER+1, BMP_WEATHER, LAY_WEATHER, OFF, CHK_256 );
	DSP_SetGraphParam( GRP_WEATHER+1, DRW_ADD );
	DSP_SetGraphBright( GRP_WEATHER+1, 128, 64, 128 );

	DSP_SetGraph( GRP_WEATHER+2, BMP_WEATHER+1, LAY_WEATHER, OFF, CHK_256 );
	DSP_SetGraphParam( GRP_WEATHER+2, DRW_ADD );
	DSP_SetGraphBright( GRP_WEATHER+2, 16, 16, 64 );

	DSP_SetGraph( GRP_WEATHER+3, BMP_WEATHER, LAY_WEATHER, OFF, CHK_256 );
	DSP_SetGraphParam( GRP_WEATHER+3, DRW_ADD );
	DSP_SetGraphBright( GRP_WEATHER+3, 64, 64, 32 );

	DSP_SetGraph( GRP_WEATHER+4, BMP_WEATHER+1, LAY_WEATHER, OFF, CHK_256 );
	DSP_SetGraphParam( GRP_WEATHER+4, DRW_ADD );
	DSP_SetGraphBright( GRP_WEATHER+4, 64, 32, 32 );

	DSP_SetGraph( GRP_WEATHER+5, BMP_WEATHER+2, LAY_WEATHER, OFF, CHK_256 );
	DSP_SetGraphParam( GRP_WEATHER+5, DRW_ADD );
	DSP_SetGraphBright( GRP_WEATHER+5, 16, 64, 16 );

	if(ChipBackStruct.flag){
		int	i;
		for(i=0;i<6;i++)
			DSP_SetGraphClip( GRP_WEATHER+i, 0, CHIPBACK_SY, 640, 352 );
	}
}

void AVG_ResetLensFrea( void )
{
	int			i;
	if(LensFrea.flag){
		ZeroMemory( &LensFrea, sizeof(LENS_FREA) );

		for(i=0; i<20 ;i++){
			DSP_ResetGraph( GRP_WEATHER+i );
		}
		DSP_ReleaseBmp( BMP_WEATHER   );
		DSP_ReleaseBmp( BMP_WEATHER+1 );
		DSP_ReleaseBmp( BMP_WEATHER+2 );
	}
}

void AVG_ControlLensFrea( void )
{
	int			i,x,y;
	int			x2,y2=-100;
	int			dc;
	int			lf_max = AVG_EffCnt4( LensFrea.max );

	if( LensFrea.flag ){
		LensFrea.count++;
		if( LensFrea.count >= lf_max ){
			AVG_ResetLensFrea();
		}else{
			if(LensFrea.rev)dc = 96-96*LensFrea.count/lf_max-16+256;
			else			dc = 96*LensFrea.count/lf_max-16+256;

			if(LensFrea.light){
				x2=DISP_X;
				x = SIN( dc+64 );
				y = COS( dc+64 )/3;
			}else{
				x2=0;
				x = SIN( dc );
				y = COS( dc )/3;
			}
			if( LensFrea.brink%4<2 ){
				DSP_SetGraphZoom( GRP_WEATHER,   x/6-100+x2, y/6-100+y2, 200, 200 );
				DSP_SetGraphZoom( GRP_WEATHER+1, x/4-160+x2, y/4-160+y2, 320, 320 );
				DSP_SetGraphZoom( GRP_WEATHER+2, x/5- 16+x2, y/5- 16+y2,  32,  32 );
				DSP_SetGraphZoom( GRP_WEATHER+3, x/8- 80+x2, y/8- 80+y2, 160, 160 );
				DSP_SetGraphZoom( GRP_WEATHER+4, x/7- 64+x2, y/7- 64+y2, 128, 128 );
				DSP_SetGraphZoom( GRP_WEATHER+5, x/12- 8+x2, y/12- 8+y2,  16,  16 );
				for(i=0; i<6 ;i++){
					DSP_SetGraphDisp( GRP_WEATHER+i, ON );
				}
			}else{
				for(i=0; i<6 ;i++){
					DSP_SetGraphDisp( GRP_WEATHER+i, OFF );
				}
			}
			LensFrea.brink++;
		}
	}
}

typedef struct{
	int		flag;	
	int		type;	
	int		level;	
	int		frame;	
	int		loop;	
	int		cx,cy;	
	int		blend;	

	int		dir;	
	int		cnt;	
}WARP_EFFECT;
WARP_EFFECT	WarpEffect;

void AVG_SetWarp( int type, int level, int frame, int loop, int dir, int cx, int cy, int blend )
{
	WarpEffect.flag  = 1;
	WarpEffect.type  = type;
	WarpEffect.level = level;
	WarpEffect.frame = frame;
	WarpEffect.loop  = loop;
	WarpEffect.dir   = dir;
	WarpEffect.cnt   = 0;
	WarpEffect.cx    = cx;
	WarpEffect.cy    = cy;
	WarpEffect.blend = blend;

	DSP_CopyBmp( BMP_BACK, BMP_BACK2 );
	SetCharPosShake( 0, 0, ON );
}
void AVG_StopWarp( int loop )
{
	WarpEffect.loop = loop;
}
void AVG_ResetWarp( void )
{
	DSP_SetGraphParam( GRP_BACK,   DRW_NML );
	DSP_SetGraphParam( GRP_BACK+1, DRW_NML );
	ZeroMemory( &WarpEffect, sizeof(WARP_EFFECT) );
	SetCharPosShake( 0, 0, OFF );
}
BOOL AVG_WaitWarp( void )
{
	return WarpEffect.flag && WarpEffect.loop;
}
void AVG_ControlWarp( void )
{
	int	wav_max = AVG_EffCnt3( WarpEffect.frame );
	int	cnt,blend = WarpEffect.blend;
	
	if(WarpEffect.flag){
		if(wav_max==0){
			AVG_ResetWarp();
			return;
		}
		if( WarpEffect.cnt >= wav_max ){
			switch(WarpEffect.loop){
			case 1:
				AVG_ResetWarp();
				return;
			case 0:
			default:
				WarpEffect.cnt=0;
				WarpEffect.loop = max(0,WarpEffect.loop-1);
				WarpEffect.dir = -WarpEffect.dir;
				break;
			}
		}
		if(WarpEffect.loop==1){
			blend = WarpEffect.blend + (256-WarpEffect.blend)*WarpEffect.cnt/wav_max;
		}

		DSP_SetGraphParam2( GRP_BACK, WAP_PARAM(WarpEffect.type,blend) );
		DSP_SetGraphParam2( GRP_BACK+1, WAP_PARAM(WarpEffect.type,blend) );
		DSP_SetGraphPosPoint( GRP_BACK, 1, WarpEffect.cx, WarpEffect.cy );
		DSP_SetGraphPosPoint( GRP_BACK+1, 1, WarpEffect.cx, WarpEffect.cy );



		cnt = WarpEffect.cnt*128/wav_max + ((WarpEffect.dir<0)?128:0);

		DSP_SetGraphParam( GRP_BACK, DRW_WAP(WarpEffect.level,cnt) );
		DSP_SetGraphParam( GRP_BACK+1, DRW_WAP(WarpEffect.level,cnt) );

		WarpEffect.cnt++;
	}
}



typedef struct{
	int		flag;	
	int		type;
	int		wav;	
	int		swav;	
	int		ewav;	
	int		cnt;	
	int		max;	
	int		vcnt;	
}WAV_EFFECT;
WAV_EFFECT	WavEffect;

BOOL AVG_GetWavEffectFlag( void )
{
	return WavEffect.flag;
}


void AVG_SetWavEffect( int wav, int frame, int wave_no )
{
	int		bmp_bpp = BPP(MainWindow.draw_mode2);

	if(frame<1){
		WavEffect.flag = 1;
		WavEffect.swav = WavEffect.ewav = WavEffect.wav = wav;
		WavEffect.type = wave_no;
		WavEffect.cnt  = 1;
		WavEffect.max  = 1;
		WavEffect.vcnt = 0;
	}else{
		if(WavEffect.flag){
			WavEffect.type = wave_no;
			WavEffect.swav = WavEffect.wav;
			WavEffect.ewav = wav;
			WavEffect.cnt  = 0;
			WavEffect.max  = frame;
		}else{
			WavEffect.type = wave_no;
			WavEffect.flag = 1;
			WavEffect.swav = WavEffect.wav;
			WavEffect.ewav = wav;
			WavEffect.cnt  = 0;
			WavEffect.max  = frame;
			WavEffect.vcnt = 0;
		}
	}
}
void AVG_ResetWavEffect( void )
{
	DSP_SetGraphParam( GRP_BACK, DRW_NML );
	ZeroMemory( &WavEffect, sizeof(WAV_EFFECT) );
}
BOOL AVG_WaitWavEffect( void )
{
	int	wav_max = AVG_EffCnt3( WavEffect.max );
	return (WavEffect.cnt >= wav_max);
}
void AVG_ControlWavEffect( void )
{
	int	wav_max = AVG_EffCnt3( WavEffect.max );
	
	if(WavEffect.flag){
		if( WavEffect.cnt < wav_max ){
			WavEffect.cnt++;
			WavEffect.wav = (WavEffect.swav*(wav_max-WavEffect.cnt)+WavEffect.ewav*WavEffect.cnt) / wav_max;
		}else{
			WavEffect.wav = WavEffect.ewav;
		}
		switch(WavEffect.type){
			default:
			case 0:
				DSP_SetGraphParam( GRP_BACK, DRW_WAV(WavEffect.wav,WavEffect.vcnt) );
				DSP_SetGraphParam( GRP_BACK+1, DRW_WAV(WavEffect.wav,WavEffect.vcnt) );
				break;
			case 1:
				DSP_SetGraphParam( GRP_BACK, DRW_SUI(WavEffect.wav,WavEffect.vcnt) );
				DSP_SetGraphParam( GRP_BACK+1, DRW_SUI(WavEffect.wav,WavEffect.vcnt) );
				break;
		}
		WavEffect.vcnt += max( 1, 60/Avg.frame );
	}
}



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
	int		cd;			
	int		fcnt;		
	int		fmax;		
	int		loop;		
	int		vol;		
	int		prev_vol;	
	int		next_vol;	
	DWORD	bar[2];
}PLAY_MUSIC;
PLAY_MUSIC		PlayMusic[WAVE_STREAM_NUM] = {
	{ -1, -1, MUSIC_STOP },
	{ -1, -1, MUSIC_STOP },
	{ -1, -1, MUSIC_STOP },
	{ -1, -1, MUSIC_STOP },
};
int			DB_No=0;

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
short	VoiceVolumeChar[VOICE_CHAR_MAX];
short	VoiceVolumeScript=256;

int	PlayMusicNo   = -1;
int	PlayMusicVol  = 255;
int	PlayMusicLoop = TRUE;



void AVG_InitSound( void )
{
	int	i;

	for( i=0 ; i<WAVE_STREAM_NUM ; i++ ){
		PlayMusic[i].handle=-1;
	}
	readFile = new ClReadFile;
	readFile->OpenPackFile( PAC_BGM );
	readFile->OpenPackFile( PAC_VOICE );
	readFile->OpenPackFile( PAC_SE );
	
	lpSoundDS = new ClSoundDS( MainWindow.hwnd, readFile, FALSE );

	for( i=0 ; i<VOICE_MAX ; i++ ){
		VoiceStruct[i].handle=-1;
	}
	for( i=0 ; i<VOICE_CHAR_MAX ; i++ ){
		VoiceVolumeChar[i] = 256;
	}
	VoiceVolumeScript = 256;
}
void AVG_TermSound( void )
{
	delete lpSoundDS;
	delete readFile;
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
		if(VoiceStruct[vc_no].handle==-1){
			return 1;
		}else{
			if( lpSoundDS->GetStatus(VoiceStruct[vc_no].handle)==PCM_STOP ){
				return 1;
			}else{
				return 0;
			}
		}
	}else{
		return 1;
	}
}
void AVG_SetVolumeVoiceChar( int cno, int vol )
{
	VoiceVolumeChar[cno] = vol;
}
void AVG_SetVolumeVoiceScript( int vol )
{
	VoiceVolumeScript = vol;
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
			vol = (float)VoiceStruct[i].vol * voice_vol * VoiceVolumeScript / (256*256);
			lpSoundDS->SetVolume( VoiceStruct[i].handle, LIM(vol,0,255), FALSE );
		}
	}
}


long	DefaultCharName=1;

void AVG_SizukuPlayVoice( int vc_no, int vno, int char_no, int ex_no, int vol, int loop, int goin )
{
	char	fname[256];
	int		cno;
	int		vol2;

	AVG_StopVoice(vc_no);



	cno = char_no;
	if(ex_no==-1)	{	wsprintf( fname, "%04d_%03d", vno, cno );			}
	else			{	wsprintf( fname, "%04d_%03d_%d", vno, cno, ex_no );	}

	if( -1>=cno || cno>=100){
		AddVoiceDialogMes( "CHR_NOT[%s][%s]", fname, NowLangFileName );
		DebugBox(NULL,"音声にキャラクターの登録がありません[%s] SCRIPT[%s]", fname, NowLangFileName );
		return ;
	}

	if( lpSoundDS->CheckVoiceFile( pack_voice, fname )!=Cl_OK ){
		AddVoiceDialogMes( "FALSE_NOT[%s][%s]", fname, NowLangFileName );
		DebugBox(NULL,"音声ファイルが存在しません[%s] SCRIPT[%s]", fname, NowLangFileName );
		return ;
	}
	

	if( Debug.flag ){
		char	src[MAX_PATH],dst[MAX_PATH];
		wsprintf( dst, "voice\\buf\\%s.ogg", fname );
		wsprintf( src, "voice\\%s.ogg", fname );
		CopyFile( src, dst, TRUE);
	}

	if( AVG_GetMesCut() ){
		AddVoiceDialogMes( "SKIP[%s] NO_CHECK", fname );
		return ;
	}
	if(!goin){
		int	skip_flag=0;
		if(cno<=5){
			if(CharVoiceFlag[cno]==OFF) skip_flag=1;
		}else{
			if(cno==6 || cno==7 || cno==98) { if(CharVoiceFlag[7]==OFF) skip_flag=1; }
			else if(cno==99)				{ if(CharVoiceFlag[6]==OFF) skip_flag=1; }
		}
		if(skip_flag){
			AddVoiceDialogMes( "SKIP[%s] NO_CHECK", fname );
			return ;
		}
	}
	if( Avg.voice==0 ){
		AddVoiceDialogMes( "VOICE_OFF[%s] NO_CHECK", fname );
		return ;
	}
		
	VoiceStruct[vc_no].char_no = cno;
	if(vol==-1){
		VoiceStruct[vc_no].prev_vol = VoiceStruct[vc_no].next_vol = VoiceStruct[vc_no].vol = VoiceVolumeChar[VoiceStruct[vc_no].char_no];
	}else{
		VoiceStruct[vc_no].prev_vol = VoiceStruct[vc_no].next_vol = VoiceStruct[vc_no].vol = vol;
	}

	vol2 = (float)VoiceStruct[vc_no].vol * Avg.voice_vol * VoiceVolumeScript / (256*256);
	if( lpSoundDS->playVoice( VoiceStruct[vc_no].handle, pack_voice, fname, !loop, LIM(vol2,0,255) )!=Cl_OK ){
		AddVoiceDialogMes( "PLAY_NG[%s][%s]", fname, NowLangFileName );
		DebugBox(NULL,"音声の再生に失敗しました[%s] SCRIPT[%s]", fname, NowLangFileName );
	}else{
		if(vol==-1){
			AddVoiceDialogMes( "OK[%s]CHR_VOL[%d]%s", fname, VoiceStruct[vc_no].vol, NowLangFileName );
		}else{
			AddVoiceDialogMes( "OK[%s]SET_VOL[%d]%s", fname, vol, NowLangFileName );
		}
	}
}

BOOL AVG_PlaySound( int vc_no, char *fname, int loop )
{
	BOOL	ret;
	char	fname2[256];
	wsprintf( fname2, "voice\\%s.wav", fname );
	ret = STD_CheckFile( fname2 );
	if(ret){
		if(loop)PlaySound( fname2, NULL, SND_FILENAME|SND_ASYNC|SND_LOOP );
		else	PlaySound( fname2, NULL, SND_FILENAME|SND_ASYNC );
	}else{
		ret = lpSoundDS->playVoice( VoiceStruct[vc_no].handle, pack_voice, fname, !loop, LIM(VoiceStruct[vc_no].vol*Avg.voice_vol/256,0,255) )==Cl_OK;
	}
	return ret;
}
void AVG_SetVI_EventVoiceNo( int vi_ee, int all )
{
	if(all!=-1){
		VI_EventVoiceNoAll = all;
	}else{
		if(vi_ee!=-1){
			VI_EventVoiceNo = vi_ee;
		}
	}
}
void AVG_PlayVoice( int vc_no, int cno, int sno, int vno, int vol, int loop, int a_cut, int test )
{
	char	fname[256];

	AVG_StopVoice(vc_no);


	if(VI_EventVoiceNoAll < 0){
		if(VI_EventVoiceNo < 0){
			if(sno==-1)	sno = ScenarioNo;
		}else{
			if(sno==-1)	sno = ScenarioNo/100*100 + VI_EventVoiceNo;
		}
	}else{
		if(sno==-1)	sno = VI_EventVoiceNoAll;
	}
	
	
	
	VoiceStruct[vc_no].prev_vol = VoiceStruct[vc_no].next_vol = VoiceStruct[vc_no].vol = vol;

	int	skip_voice;
	if(cno>=10 && cno!=28 ){
		cno=99;
		skip_voice = CharVoiceFlag[9]==OFF;
	}else{
		if(cno==0){
			DebugBox( NULL, "Voiceキャラ番号「０」は使用できません。" );
		}else{
			if(cno==28)	skip_voice = CharVoiceFlag[10]==OFF;
			else		skip_voice = CharVoiceFlag[cno-1]==OFF;
		}
	}

	if(test==0)
		SetNovelMessageVoice1( sno, vno, cno, a_cut );
	if( skip_voice || Avg.voice==0 || (AVG_GetMesCut() && Avg.demo==0 && test==0) ){
		if( !Debug.flag ) return ;

		if( DefaultCharName ){
			wsprintf( fname, "K%09d_%03d%03d", sno, vno, cno );
			if( lpSoundDS->CheckVoiceFile( pack_voice, fname )!=Cl_OK ){
				AddVoiceDialogMes( "[K%09d_%03d%03d] ERR_NO_FILE", sno, vno, cno );
			}else{
				AddVoiceDialogMes( "[K%09d_%03d%03d] FIND_FILE", sno, vno, cno );
			}
		}else{
			wsprintf( fname, "K%09d_%03d%03da", sno, vno, cno );
			if( lpSoundDS->CheckVoiceFile( pack_voice, fname )!=Cl_OK ){
				if(a_cut){
					AddVoiceDialogMes( "[K%09d_%03d%03dA] NO_PLAY", sno, vno, cno );
				}else{
					wsprintf( fname, "K%09d_%03d%03d", sno, vno, cno );
					if( lpSoundDS->CheckVoiceFile( pack_voice, fname )!=Cl_OK ){
						AddVoiceDialogMes( "ERR [K%09d_%03d%03d] NO_FILE", sno, vno, cno );
					}else{
						AddVoiceDialogMes( "[K%09d_%03d%03d] FIND_FILE", sno, vno, cno );
					}
				}
			}else{
				AddVoiceDialogMes( "[K%09d_%03d%03da] FIND_FILE", sno, vno, cno );
			}
		}
if( FILE_SPOIT ){
	char	src[MAX_PATH],dst[MAX_PATH];
	wsprintf( dst, "voice\\buf\\%s.ogg", fname );
	wsprintf( src, "voice\\%s.ogg", fname );
	CopyFile( src, dst, TRUE);

	wsprintf( dst, "voice\\buf\\%sa.ogg", fname );
	wsprintf( src, "voice\\%sa.ogg", fname );
	CopyFile( src, dst, TRUE);
}
		return ;
	}


	if( DefaultCharName ){
		wsprintf( fname, "K%09d_%03d%03d", sno, vno, cno );
		if( !AVG_PlaySound( vc_no, fname, loop ) ){


				AddVoiceDialogMes( "[K%09d_%03d%03d] ERR_NO_FILE", sno, vno, cno );


		}else{
			AddVoiceDialogMes( "[K%09d_%03d%03d] PLAY", sno, vno, cno );
		}
	}else{
		wsprintf( fname, "K%09d_%03d%03da", sno, vno, cno );
		if( !AVG_PlaySound( vc_no, fname, loop ) ){
			if(a_cut){
				AddVoiceDialogMes( "[K%09d_%03d%03dA] NO_PLAY", sno, vno, cno );
			}else{
				wsprintf( fname, "K%09d_%03d%03d", sno, vno, cno );
				if( !AVG_PlaySound( vc_no, fname, loop ) ){
					AddVoiceDialogMes( "ERR [K%09d_%03d%03d] NO_FILE", sno, vno, cno );

				}else{
					AddVoiceDialogMes( "[K%09d_%03d%03d] PLAY", sno, vno, cno );
				}
			}
		}else{
			AddVoiceDialogMes( "[K%09d_%03d%03dA] PLAY", sno, vno, cno );
		}
	}
	if( FILE_SPOIT ){
		char	src[MAX_PATH],dst[MAX_PATH];
		wsprintf( dst, "voice\\buf\\%s.ogg", fname );
		wsprintf( src, "voice\\%s.ogg", fname );
		CopyFile( src, dst, TRUE);

		wsprintf( dst, "voice\\buf\\%sa.ogg", fname );
		wsprintf( src, "voice\\%sa.ogg", fname );
		CopyFile( src, dst, TRUE);
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

	PlayMusic[pno].cd       = OFF;
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



	
	if( lpSoundDS->ChangeMusic( PlayMusic[pno].handle, pack_bgmfile, mus_no, FALSE, !loop, LIM(PlayMusic[pno].vol*Avg.bgm_vol/256,0,255) ) ){
		DebugBox( NULL, "BGMのデータがありません[bgm_%03d.ogg]", mus_no );
	}

	if( FILE_SPOIT ){
		char	src[MAX_PATH],dst[MAX_PATH];
		wsprintf( dst, "bgm\\buf\\bgm_%03d.ogg", mus_no );
		wsprintf( src, "bgm\\bgm_%03d.ogg", mus_no );
		CopyFile( src, dst, TRUE);
		wsprintf( dst, "bgm\\buf\\bgm_%03d_a.ogg", mus_no );
		wsprintf( src, "bgm\\bgm_%03d_a.ogg", mus_no );
		CopyFile( src, dst, TRUE);
		wsprintf( dst, "bgm\\buf\\bgm_%03d_b.ogg", mus_no );
		wsprintf( src, "bgm\\bgm_%03d_b.ogg", mus_no );
		CopyFile( src, dst, TRUE);
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
				lpSoundDS->SetVolume( PlayMusic[i].handle, LIM(PlayMusic[i].vol*bgm_vol/256,0,255), FALSE );
	}
}

void AVG_PlayCD( int mus_no, BOOL fade, int loop )
{
	if(mus_no!=-1){


		PlayMusic[0].cd       = ON;
		PlayMusic[0].mus_no   = mus_no;		
		PlayMusic[0].mode     = MUSIC_PLAY;	
		PlayMusic[0].loop     = loop;		
		PlayMusic[0].vol = 0xff;			
		PlayMusic[0].next_vol = 0xff;		
	}
}
int AVG_StopCD( BOOL fade )
{
	return 0;
}
void AVG_VolumeCD( int vol )
{

}

#include "GM_omake.h"
void AVG_PlayBGM( int mus_no, int fade, int loop, int vol, int change )
{
	int	i;
	for(i=0;i<MUSIC_MODE_MAX;i++){
		if( MusTable[i]==mus_no){
			break;
		}
	}

	if(i<MUSIC_MODE_MAX){
		AVG_SetGameFlag( i+128, ON );
	}
	AVG_PlayBGMEx( mus_no, fade, loop, vol, change, OFF );
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
		if(Avg.sound){
			if(fade==-2){
				DB_No = (DB_No)?0:1;								
				AVG_PlayMusic( DB_No, mus_no, 0, loop, vol, 0 );	
				AVG_StopMusic( bk_no, 15 );						
			}else{
				DB_No = (DB_No)?0:1;								
				AVG_PlayMusic( DB_No, mus_no, fade, loop, vol, 0 );	
				AVG_StopMusic( bk_no, min(fade/2,30) );						
			}
		}else{
			AVG_PlayCD( mus_no, !!fade, loop );
		}
	}
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

			AVG_PlayBGM( PlayMusicNo, FADE_MUS, PlayMusicLoop, PlayMusicVol, 1 );


	}else{

			AVG_StopMusic( DB_No, FADE_MUS );




	}
}

void AVG_ChangeDeviceBGM( int device )
{
	if(Avg.sound)	{	AVG_StopMusic( DB_No, FADE_MUS );	}
	else			{	AVG_StopCD(TRUE);					}


	Avg.sound = device;
	
	if(Avg.sound)	{	AVG_PlayBGM( PlayMusicNo, FADE_MUS, PlayMusicLoop, PlayMusicVol, 1  );	}
	else			{	AVG_PlayCD( PlayMusicNo, TRUE, TRUE );				}
}
void AVG_SetVolumeBGM( int vol, int fade )
{
	AVG_SetVoumeMusic( DB_No, vol, fade );
}


void AVG_ReleaseSeAll( void )
{
	int	i;
	
	for(i=0;i<WAVE_SOUND_NUM;i++)AVG_StopSE2( i, 0 );
	ZeroMemory( SeStruct, sizeof(SE_STRUCT)*WAVE_SOUND_NUM );
}
void AVG_FadeSeAll( int fade )
{
	int	i;
	for(i=0;i<WAVE_SOUND_NUM;i++)
		AVG_StopSE2(i, fade );
}

#define SE_FILE_DEF	"se_%04d"




int AVG_PlaySePan( int se_no, int loop, int vol, int lr )
{
	int	ret;
	int	pan=0;
	int	voll=0, volr=0;

	if(!Avg.se) return -1;

	ret = AVG_PlaySE( se_no, loop, vol );


	
	return ret;
}
int		FreeSeHandle[8] = { -1,-1,-1,-1, -1,-1,-1,-1 };
void AVG_PlaySE3( int se_no, int vol )
{
	int		ret=-1,i;
	if(!Avg.se) return ;
	

	
	for(i=0;i<8;i++) {
		if( FreeSeHandle[i] == -1){



			if(FILE_SPOIT){
				char	src[MAX_PATH],dst[MAX_PATH];
				wsprintf( dst, "se\\buf\\se_%04d.wav", se_no );
				wsprintf( src, "se\\se_%04d.wav", se_no );
				CopyFile( src, dst, TRUE);
			}

			if( PlayEffectNum( FreeSeHandle[i], se_no, vol*Avg.se_vol/256, FALSE ) ){
				char	fname[256];
				wsprintf( fname, "se\\SE_%04d.wav", se_no );
				PlaySound( fname, NULL, SND_FILENAME|SND_ASYNC );

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
	


	if( FILE_SPOIT ){
		char	src[MAX_PATH],dst[MAX_PATH];
		wsprintf( dst, "se\\buf\\se_%04d.wav", se_no );
		wsprintf( src, "se\\se_%04d.wav", se_no );
		CopyFile( src, dst, TRUE);
	}

	if(direct_vol){
		PlayEffectNum( ret, se_no, vol, loop );
	}else{
		PlayEffectNum( ret, se_no, vol*Avg.se_vol/256, loop );
	}
	if(ret==-1){




		DebugBox( NULL, "SEファイルが見つかりません[%04d].wav", se_no );
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
				lpSoundDS->SetVolume( SeStruct[i].dno, LIM(SeStruct[i].vol*se_vol/256,0,255), TRUE );
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
			if( SeStruct[sno].dno!=-1 ){
				if( AVG_GetMesCut() ){
					AVG_StopSE2( sno, 0 );
					return FALSE;
				}else{
					return !SeStruct[sno].loop;
				}
			}else{
				return FALSE;
			}

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
			vol = (float)VoiceStruct[i].vol * Avg.voice_vol * VoiceVolumeScript / (256*256);
			lpSoundDS->SetVolume( VoiceStruct[i].handle, LIM(vol,0,255), FALSE );
		}
	}

	for(i=0;i<8;i++) {
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
				SeStruct[i].fcnt = SeStruct[i].fmax;

				SeStruct[i].vol = SeStruct[i].next_vol;
				if(SeStruct[i].stop){
					AVG_StopSE( SeStruct[i].dno );
					ZeroMemory( &SeStruct[i], sizeof(SeStruct[i]) );
					break;
				}
			}
			if(SeStruct[i].dno!=-1){
				lpSoundDS->SetVolume( SeStruct[i].dno, LIM(SeStruct[i].vol*Avg.se_vol/256,0,255), TRUE);
			}
		}
	}

}
void AVG_ControlMusic( void )
{
	
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




		switch(PlayMusic[i].mode){
			default:
			case MUSIC_PAUSE:
			case MUSIC_STOP:
				break;
			case MUSIC_PLAY:
#if DIRECT_SOUND

#endif
				break;
			case MUSIC_FADEOUT:
				if( mus_max<=PlayMusic[i].fcnt ){
					PlayMusic[i].mus_no = -1;
					PlayMusic[i].loop = 0;
					PlayMusic[i].mode = MUSIC_STOP;

#if DIRECT_SOUND
					if( PlayMusic[i].handle != -1){
						lpSoundDS->Stop( PlayMusic[i].handle );
						lpSoundDS->ReleaseSoundObj( PlayMusic[i].handle );
					}
					PlayMusic[i].handle=-1;

#else
					PCM_ReleaseWaveStream( i );
#endif
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
			if(PlayMusic[i].cd){
				AVG_VolumeCD( PlayMusic[i].vol );
				DebugPrintf( " bgm %d\n", PlayMusic[i].vol*Avg.bgm_vol/256 );
			}else{
#if DIRECT_SOUND
				PlayMusic[i].bar[0]=PlayMusic[i].bar[0]*PlayMusic[i].vol/256*Avg.bgm_vol/256;
				PlayMusic[i].bar[1]=PlayMusic[i].bar[1]*PlayMusic[i].vol/256*Avg.bgm_vol/256;
				if( PlayMusic[i].handle != -1){
					lpSoundDS->SetVolume( PlayMusic[i].handle, LIM(PlayMusic[i].vol*Avg.bgm_vol/256,0,255), FALSE );
				}
#else
				PCM_SetWaveStreamVolume( i, LIM(PlayMusic[i].vol*Avg.bgm_vol/256,0,255)<<8, VOL_CENTER );
#endif
			}
		}
	}
}



void AVG_OpenNormalDisp( void )
{






}
void AVG_CloseNormalDisp( void )
{







}
void AVG_InitIndicator( int gno, int bno )
{
	DSP_LoadBmp( bno+0, BMP_256P, "t0310.bmp" );


	DSP_SetGraph( gno+0, bno+0, LAY_SYSTEM+3, OFF, CHK_256 );	
	DSP_SetGraph( gno+1, bno+0, LAY_SYSTEM+3, OFF, CHK_256 );	
	DSP_SetGraph( gno+2, bno+0, LAY_SYSTEM+4, OFF, CHK_NO );	
	DSP_SetGraph( gno+3, bno+0, LAY_SYSTEM+4, OFF, CHK_NO );	

}
void AVG_TermIndicator( int gno, int bno )
{
	DSP_ResetGraph( gno+0 );	
	DSP_ResetGraph( gno+1 );	
	DSP_ResetGraph( gno+2 );	
	DSP_ResetGraph( gno+3 );	


	DSP_ReleaseBmp( bno+0 );

}
void AVG_RenewIndicator( int disp, int gno, int lay, int fade )
{
	int			bar[2],i;
	static int	bar_max[2];
	static int	bar_maxcnt[2];
	
	if(disp){
		for(i=0;i<2;i++){
			bar[i] = AVG_GetBarBGM(i);
			bar[i] = bar[i]/56;
			bar[i] = bar[i]/4*4;
			bar[i] = LIM(bar[i],0,254);




			bar_maxcnt[i]-=4;
			if( bar_maxcnt[i] < bar[i] ){
				bar_maxcnt[i] = bar[i];
			}
			bar_max[i] = bar_maxcnt[i]/4*4;
		}
		DSP_SetGraphPos( gno+0, 164,                  506,           0, 0, bar[0], 7 );
		DSP_SetGraphPos( gno+1, 164+(254-bar[1]),     506,  254-bar[1], 7, bar[1], 7 );
		DSP_SetGraphPos( gno+2, 164+bar_max[0]-2,     506,         254, 0, 2, 7 );
		DSP_SetGraphPos( gno+3, 164+(254-bar_max[1]), 506,         254, 7, 2, 7 );



		DSP_SetGraphParam( gno+0, DRW_ADD  );
		DSP_SetGraphParam( gno+1, DRW_ADD  );

		DSP_SetGraphFade( gno+0, fade/2 );
		DSP_SetGraphFade( gno+1, fade/2 );

		DSP_SetGraphParam( gno+2, DRW_BLD(fade) );
		DSP_SetGraphParam( gno+3, DRW_BLD(fade) );

		DSP_SetGraphDisp( gno+0, ON  );
		DSP_SetGraphDisp( gno+1, ON  );
		DSP_SetGraphDisp( gno+2, ON  );
		DSP_SetGraphDisp( gno+3, ON  );


		DSP_SetGraphLayer( gno+0, lay  );
		DSP_SetGraphLayer( gno+1, lay  );
		DSP_SetGraphLayer( gno+2, lay  );
		DSP_SetGraphLayer( gno+3, lay  );

	}else{
		DSP_SetGraphDisp( gno+0, OFF );
		DSP_SetGraphDisp( gno+1, OFF );
		DSP_SetGraphDisp( gno+2, OFF );
		DSP_SetGraphDisp( gno+3, OFF );

	}
}




RECT2	CnfCons[5] = {
	{ 200, 112+88*0, 400, 82 },
	{ 200, 112+88*1, 400, 82 },
	{ 200, 112+88*2, 400, 82 },
	{ 200, 112+88*3, 400, 82 },
	{ 306,      480, 188, 32 },
};

CONFIG_STRUCT	Config;

enum{
	CNF_NOT,
	CNF_OPEN,
	CNF_CLOSE,
	CNF_NORMAL,
	CNF_DIS_TEXT,
	CNF_ANOTHER,

	CNF_NEXT_OPEN,
	CNF_NEXT_CLOSE,

	CNF_CHECK,
};

enum{
	CNF_NEXT_SAVE,
	CNF_NEXT_LOAD,
	CNF_NEXT_SIDE,
};




BOOL AVG_GetConfigFlag( void )
{
	return Config.flag;
}

void AVG_SetConfigWindow( int open_mode )
{
	int		bmp_bpp = BPP(MainWindow.draw_mode2);

	MainWindow.draw_flag=ON;
	Config.flag = 1;		
	Config.mode = CNF_OPEN;	


	DSP_LoadBmp( BMP_SYSTEM,   BMP_FULL, "sys0100.tga" );
	DSP_LoadBmp( BMP_SYSTEM+1, BMP_TRUE, "sys0110.tga" );
	DSP_LoadBmp( BMP_SYSTEM+2, BMP_TRUE, "sys0111.tga" );
	DSP_LoadBmp( BMP_SYSTEM+3,  BMP_TRUE, "sys0230.tga" );

	
	DSP_SetGraph( GRP_SYSTEM,   BMP_SYSTEM+0, LAY_SYSTEM+0, OFF, CHK_NO );	

	DSP_SetGraph( GRP_SYSTEM+1, BMP_SYSTEM+1, LAY_SYSTEM+1, OFF, CHK_ANTI );	
	DSP_SetGraph( GRP_SYSTEM+2, BMP_SYSTEM+1, LAY_SYSTEM+1, OFF, CHK_ANTI );	
	DSP_SetGraph( GRP_SYSTEM+3, BMP_SYSTEM+1, LAY_SYSTEM+1, OFF, CHK_ANTI );	
	DSP_SetGraph( GRP_SYSTEM+4, BMP_SYSTEM+1, LAY_SYSTEM+1, OFF, CHK_ANTI );	
	DSP_SetGraph( GRP_SYSTEM+5, BMP_SYSTEM+2, LAY_SYSTEM+1, OFF, CHK_ANTI );	

			int	_month = ESC_GetFlag( _MONTH );
			int	_days   = ESC_GetFlag( _DAY  );
			char	buf2[256];
			if(_month==0 || Avg.omake){
				wsprintf( buf2, "?A?B" );
			}else{
				wsprintf( buf2, "%dA%dB", _month, _days );
			}
			DSP_SetGraphStr( GRP_SYSTEM+6, BMP_SYSTEM+3,  LAY_SYSTEM+1, OFF, CHK_ANTI, buf2 );
			DSP_SetGraphMove( GRP_SYSTEM+6, 138, 12 );




	DSP_SetGraphPos( GRP_SYSTEM,   0, 0,  0, 0, DISP_X, DISP_Y );

	int	i;
	for(i=0;i<4;i++){
		DSP_SetGraphPos( GRP_SYSTEM+1+i, CnfCons[i].x, CnfCons[i].y, 400*(i%2), 82*3*(i/2), CnfCons[i].w, CnfCons[i].h );
	}

	DSP_SetGraphPos( GRP_SYSTEM+5, CnfCons[i].x, CnfCons[i].y, 188*0,0, CnfCons[i].w, CnfCons[i].h );
	Config.mouse = MUS_GetMouseLayer();

	if(Avg.omake){
		MUS_SetMouseRect( 1, 0, CnfCons[0].x,  CnfCons[0].y, CnfCons[0].w, CnfCons[0].h, OFF );
		MUS_SetMouseRect( 1, 1, CnfCons[1].x,  CnfCons[1].y, CnfCons[1].w, CnfCons[1].h, OFF );
		DSP_SetGraphFade( GRP_SYSTEM+1+0, 64 );
		DSP_SetGraphFade( GRP_SYSTEM+1+1, 64 );
	}else{
		MUS_SetMouseRect( 1, 0, CnfCons[0].x,  CnfCons[0].y, CnfCons[0].w, CnfCons[0].h, ON );
		MUS_SetMouseRect( 1, 1, CnfCons[1].x,  CnfCons[1].y, CnfCons[1].w, CnfCons[1].h, ON );
	}
	if(MapStep){
		MUS_SetMouseRect( 1, 2, CnfCons[2].x, CnfCons[2].y, CnfCons[2].w, CnfCons[2].h, OFF );
		DSP_SetGraphFade( GRP_SYSTEM+1+2, 64 );
	}else{
		MUS_SetMouseRect( 1, 2, CnfCons[2].x, CnfCons[2].y, CnfCons[2].w, CnfCons[2].h, ON );
	}
	MUS_SetMouseRect( 1, 3, CnfCons[3].x, CnfCons[3].y, CnfCons[3].w, CnfCons[3].h, ON );
	MUS_SetMouseRect( 1, 4, CnfCons[4].x, CnfCons[4].y, CnfCons[4].w, CnfCons[4].h, ON );
	MUS_SetMouseLayer( 1 );
}

void AVG_ResetConfigWindow( void )
{
	int		i;

	MUS_ResetMouseRect_Layer( 1 );
	MUS_SetMouseLayer( Config.mouse );

	ZeroMemory( &Config, sizeof(Config) );
	for(i=0;i<20;i++)
		DSP_ResetGraph( GRP_SYSTEM+i );
	for(i=0;i<10;i++)
		DSP_ReleaseBmp( BMP_SYSTEM+i   );
	for(i=0;i<5;i++)
		DSP_ResetText( TXT_SYSTEM+i );

}
void AVG_OpenConfigWindow( void )
{
	int		i;
	for(i=0;i<7;i++)
		DSP_SetGraphDisp( GRP_SYSTEM+i, ON );

	if(Avg.omake){
		MUS_SetMouseRectFlag( 1, 0, OFF );
		MUS_SetMouseRectFlag( 1, 1, OFF );
	}else{
		MUS_SetMouseRectFlag( 1, 0, ON );
		MUS_SetMouseRectFlag( 1, 1, ON );
	}
	if(MapStep){
		MUS_SetMouseRectFlag( 1, 2, OFF );
	}else{
		MUS_SetMouseRectFlag( 1, 2, ON );
	}
	MUS_SetMouseRectFlag( 1, 3, ON );
	MUS_SetMouseRectFlag( 1, 4, ON );



}
void AVG_CloseConfigWindow( void )
{
	int		i;
	for(i=0;i<7;i++)
		DSP_SetGraphDisp( GRP_SYSTEM+i, OFF );
	for(i=0;i<5;i++)
		MUS_SetMouseRectFlag( 1, i, OFF );
}

void AVG_CloseStartConfigWindow( int cmax )
{
	if( !(Config.mode==CNF_OPEN && Config.cnt==0) && Config.mode!=CNF_CLOSE ){
		if( Config.mode==CNF_OPEN )	{ Config.cnt = cmax-Config.cnt;	}
		else						{ Config.cnt = 0;				}
		Config.mode = CNF_CLOSE;
	}
}
int AVG_ControlConfigWindow( void )
{
	int		cmax = AVG_EffCnt(-1);
	int		select = MUS_GetMouseNoEx( -1, 1 );
	int		click  = GameKey.click;
	int		cansel = GameKey.cansel;
	int		rate=256,rate2=0,i;
	static int	cnf_chk_step = 0;
	static int	cnf_chk_cnt = 0;

	static int	select_fade = 0;
	static int	select_back=0;

	static int	sakura=0;

	int		ret = 2;
	enum{
		CNF_CHK_IN=0,
		CNF_CHK_OUT,
		CNF_CHK_MAIN,
	};

	if( Config.flag ){
		switch( Config.mode ){
			case CNF_NOT:
				break;
			case CNF_ANOTHER:
				rate2 = 256;

				break;
			case CNF_OPEN:

				Config.cnt++;
				if( Config.cnt>=cmax  ){
					Config.mode = CNF_NORMAL;
					rate=256;
				}else{
					rate = 256*Config.cnt/cmax;
				}
				break;
			case CNF_CLOSE:
				Config.cnt++;
				if( Config.cnt>=cmax ){
					Config.mode = CNF_NOT;
					rate=0;

					AVG_ResetConfigWindow();		
					AVG_EndConfig();
					ret=0;
				}else{
					rate = 256-256*Config.cnt/cmax;
				}
				break;
			case CNF_NEXT_CLOSE:
				cmax = AVG_EffCnt(10);
				Config.next_cnt++;

				if( Config.next_cnt>=cmax ){
					Config.mode = CNF_NORMAL;
					Config.next_cnt=0;
					rate2 = 0;
				}else{
					rate2 = 256-256*Config.next_cnt/cmax;
				}
				break;
			case CNF_NEXT_OPEN:
				cmax = AVG_EffCnt(10);
				Config.next_cnt++;

				if( Config.next_cnt>=cmax ){
					switch( Config.next_mode ){
						case CNF_NEXT_SAVE:
							GWIN_SetSaveLoadWindow(SW_SAVE_MODE,BMP_SAVE, GRP_SAVE, TXT_SAVE, GRP_SYSTEM, TXT_SYSTEM, 2, 1);
							AVG_ChangeSetp( 0, AVG_SAVE );
							Config.mode = CNF_ANOTHER;
							break;
						case CNF_NEXT_LOAD:
							GWIN_SetSaveLoadWindow(SW_LOAD_MODE,BMP_SAVE, GRP_SAVE, TXT_SAVE, GRP_SYSTEM, TXT_SYSTEM, 2, 1);
							AVG_ChangeSetp( 0, AVG_LOAD );
							Config.mode = CNF_ANOTHER;
							break;
						case CNF_NEXT_SIDE:
							GWIN_SetSideBarWindow( BMP_SAVE, GRP_SAVE, TXT_SAVE, 2, 1 );
							AVG_ChangeSetp( 0, AVG_SETTING );
							Config.mode = CNF_ANOTHER;
							break;
					}
					DSP_SetGraphDisp( GRP_SYSTEM+3, OFF  );

					Config.next_cnt=0;
					rate2 = 256;
				}else{
					rate2 = 256*Config.next_cnt/cmax;
				}
				break;
			case CNF_NORMAL:
				if(GameKey.u)	MUS_SetMousePosRect( MainWindow.hwnd, 1, (select<=0)? 0 : select-1 );
				if(GameKey.d)	MUS_SetMousePosRect( MainWindow.hwnd, 1, (select<0)? 0 : select+1 );

	
				for(i=0;i<4;i++)
					DSP_SetGraphSMove( GRP_SYSTEM+1+i, 400*(i%2), 82*3*(i/2)+0*82 );
				DSP_SetGraphSMove( GRP_SYSTEM+5, 188*0, 0 );
				if( select!=-1 ){
					if(select != select_back )
						AVG_PlaySE3( 9108, 255 );

					if(select<4){
						DSP_SetGraphSMove( GRP_SYSTEM+1+select, 400*(select%2), 82*3*(select/2)+1*82 );
					}else{
						DSP_SetGraphSMove( GRP_SYSTEM+5, 188*1, 0 );
					}

					if( click ){
						if(select<4){
							DSP_SetGraphSMove( GRP_SYSTEM+1+select, 400*(select%2), 82*3*(select/2)+2*82 );
						}else{
							DSP_SetGraphSMove( GRP_SYSTEM+5, 188*2, 0 );
						}

						MainWindow.draw_flag=1;
						switch(select){
							case 0:
								AVG_PlaySE3( 9014, 255 );

								Config.next_mode = CNF_NEXT_SAVE;
								Config.mode = CNF_NEXT_OPEN;
								break;
							case 1:
								AVG_PlaySE3( 9014, 255 );

								Config.next_mode = CNF_NEXT_LOAD;
								Config.mode = CNF_NEXT_OPEN;
								break;
							case 2:
								AVG_PlaySE3( 9014, 255 );
								
								AVG_ResetHalfTone();
								Config.mode = CNF_DIS_TEXT;
								break;
							case 3:
								AVG_PlaySE3( 9014, 255 );

								Config.next_mode = CNF_NEXT_SIDE;
								Config.mode = CNF_NEXT_OPEN;

								break;
							case 4:
								AVG_PlaySE3( 9014, 255 );
								AVG_CloseStartConfigWindow(cmax);
								break;
						}
					}
				}else{
				}
				if(cansel){
					AVG_PlaySE3( 9107, 255 );
					AVG_CloseStartConfigWindow(cmax);
				}
				select_back=select;
				break;
			case CNF_DIS_TEXT:
				for(i=0;i<6;i++) DSP_SetGraphDisp( GRP_SYSTEM+i, OFF );

				AVG_OpenBack();
				rate=0;

				if( click || cansel ){
					rate=256;
					for(i=0;i<6;i++) DSP_SetGraphDisp( GRP_SYSTEM+i, ON );
					AVG_SetHalfTone();
					Config.mode = CNF_NORMAL;
				}
				break;
		}

		if( Config.mode != CNF_ANOTHER){
			if( rate<256 ){


				DSP_SetGraphParam( GRP_SYSTEM+0, DRW_BLD( rate )  );
				DSP_SetGraphDisp( GRP_SYSTEM+0, ON  );

				DSP_SetGraphDisp( GRP_SYSTEM+1, ON  );
				DSP_SetGraphDisp( GRP_SYSTEM+2, ON  );
				DSP_SetGraphDisp( GRP_SYSTEM+3, ON  );
				DSP_SetGraphDisp( GRP_SYSTEM+4, ON  );
				DSP_SetGraphDisp( GRP_SYSTEM+5, ON );
				DSP_SetGraphDisp( GRP_SYSTEM+6, ON );

				DSP_SetGraphParam( GRP_SYSTEM+1, DRW_BLD( LIM((rate- 64)*2,0,255)) );
				DSP_SetGraphParam( GRP_SYSTEM+2, DRW_BLD( LIM((rate- 80)*2,0,255)) );
				DSP_SetGraphParam( GRP_SYSTEM+3, DRW_BLD( LIM((rate- 96)*2,0,255)) );
				DSP_SetGraphParam( GRP_SYSTEM+4, DRW_BLD( LIM((rate-112)*2,0,255)) );
				DSP_SetGraphParam( GRP_SYSTEM+5, DRW_BLD( LIM((rate-128)*2,0,255)) );
				DSP_SetGraphParam( GRP_SYSTEM+6, DRW_BLD( LIM((rate-128)*2,0,255)) );

				AVG_OpenBack();
			}else{

					DSP_SetGraphDisp( GRP_SYSTEM+0, ON  );
					DSP_SetGraphParam( GRP_SYSTEM+0, DRW_NML );
					for(i=1;i<7;i++){
						DSP_SetGraphDisp( GRP_SYSTEM+i, ON );
						DSP_SetGraphParam( GRP_SYSTEM+i, DRW_NML );
					}
				AVG_CloseBack();
			}
		}else{
			for(i=0;i<7;i++){
				DSP_SetGraphDisp( GRP_SYSTEM+i, OFF );
			}

		}
	}
	return ret;
}
int	ConfigOpenMode=0;

void AVG_ControlSystem( void )
{
	int	mode=-1, mode2;
	AVG_ControlConfigWindow();

	mode2 = GWIN_ControlSaveLoadWindow();
	if( mode2 >= 0 ) mode = mode2;

	mode2 = GWIN_ControlSideBarWindow();
	if( mode2 >= 0 ) mode = mode2;

	switch( mode ){
		case 0:
			if(ConfigOpenMode==0){
				AVG_ChangeSetp( 0, AVG_CONFIG );
				Config.mode = CNF_NEXT_CLOSE;
			}else{
				AVG_EndConfig();
			}
			break;
		case 1:
			if(ConfigOpenMode==0){



				AVG_OpenConfigWindow();
			}else{

				AVG_OpenBack();
			}
			break;
		case 2:
			if(ConfigOpenMode==0){



			}else{

				AVG_CloseBack();
			}
			break;
	}





}



typedef struct{
	int		flag;		
	int		r,g,b;		
	int		sr,sg,sb;	
	int		er,eg,eb;	
	int		cnt;
	int		disp;		
	int		flash;
	int		fade;
	char	grp_flag[GRP_DISP];	
	char	txt_flag[TXT_DISP];	

}FADE_STRUCT;
FADE_STRUCT		FadeStruct = { 0,128,128,128 };

void AVG_GetBright( int *r, int *g, int *b )
{
	*r = FadeStruct.r;
	*g = FadeStruct.g;
	*b = FadeStruct.b;
}
void AVG_SetBright( int r, int g, int b )
{
	FadeStruct.sr = FadeStruct.er = FadeStruct.r = r;
	FadeStruct.sg = FadeStruct.eg = FadeStruct.g = g;
	FadeStruct.sb = FadeStruct.eb = FadeStruct.b = b;

	DSP_SetGraphBright( GRP_DISP, r, g, b );

}
void AVG_SetFade( int r, int g, int b, int disp, int fade )
{
	int		bmp_bpp = BPP(MainWindow.draw_mode2);
	int		i;

	FadeStruct.flag  = 1;
	FadeStruct.cnt   = 0;
	FadeStruct.sr    = FadeStruct.r;
	FadeStruct.sg    = FadeStruct.g;
	FadeStruct.sb    = FadeStruct.b;
	FadeStruct.er    = r;
	FadeStruct.eg    = g;
	FadeStruct.eb    = b;
	FadeStruct.disp  = disp;
	FadeStruct.flash = 0;
	FadeStruct.fade  = fade;

	MainWindow.draw_flag = 1;
	if(disp){
		DSP_GetDispBmp( BMP_DISP, MainWindow.draw_mode2 );

		DSP_SetGraph( GRP_DISP, BMP_DISP, LAY_BACK, ON, CHK_NO );

		for(i=0; i<GRP_DISP ;i++){
			if( !DSP_GetGraphBrightFlag(i) ){
				DSP_SetGraphDisp( i, OFF );
				FadeStruct.grp_flag[i] = 1;	
			}else{
				FadeStruct.grp_flag[i] = 0;	
			}
		}
		for(i=0; i<TXT_DISP ;i++){
			if( !DSP_GetTextBrightFlag(i) ){
				DSP_SetTextDisp( i, OFF );
				FadeStruct.txt_flag[i] = 1;	
			}else{
				FadeStruct.txt_flag[i] = 0;	
			}
		}
	}
}

void AVG_SetFlash( int r, int g, int b, int fade1, int fade2 )
{
	AVG_SetFade( r, g, b, ON, fade1 );
	if(fade2<=0) fade2=1;
	FadeStruct.flash = fade2;
}

BOOL AVG_WaitFade( void )
{
	return FadeStruct.flag;
}

void AVG_ColtrolFade( void )
{
	int		bmp_bpp = BPP(MainWindow.draw_mode2);
	int		max = 0;
	int		r,g,b,i;



	max = AVG_EffCnt( FadeStruct.fade );
	
	if(FadeStruct.flag){
		FadeStruct.cnt++;
		if(FadeStruct.cnt>=max){
			FadeStruct.flag  = 0;
			r = FadeStruct.r = FadeStruct.er;
			g = FadeStruct.g = FadeStruct.eg;
			b = FadeStruct.b = FadeStruct.eb;
			if( !FadeStruct.disp ){
				for(i=0; i<GRP_DISP ;i++){
					if( FadeStruct.grp_flag[i] ) {	DSP_SetGraphDisp( i, ON );	}
				}
				for(i=0; i<TXT_DISP ;i++){
					if( FadeStruct.txt_flag[i] ) {	DSP_SetTextDisp( i, ON );	}
				}
				DSP_ResetGraph( GRP_DISP );
				DSP_ReleaseBmp( BMP_DISP );
			}else{
				if(FadeStruct.flash){
					AVG_SetFade( 128, 128, 128, OFF, FadeStruct.flash );
				}
			}
		}else{
			r = FadeStruct.r = (FadeStruct.sr*(max-FadeStruct.cnt) + FadeStruct.er*FadeStruct.cnt) / max;
			g = FadeStruct.g = (FadeStruct.sg*(max-FadeStruct.cnt) + FadeStruct.eg*FadeStruct.cnt) / max;
			b = FadeStruct.b = (FadeStruct.sb*(max-FadeStruct.cnt) + FadeStruct.eb*FadeStruct.cnt) / max;
		}
		DSP_SetGraphBright( GRP_DISP, r, g, b );

	}
}



enum{
	LOAD_NOT=0,
	LOAD_FADEOUT,
	LOAD_WORK,
	LOAD_FADEIN,
};

static int		LoadNo=0;

void AVG_SetSaveData( AVG_SAVE_DATA	*sdata )
{
	int				i;


	if( BackStruct.sk_flag&&(BackStruct.sk_speed==0)  ){
		sdata->sk_flag  = 1;
		sdata->sk_dir   = BackStruct.sk_dir;
		sdata->sk_pich  = BackStruct.sk_pich;
		sdata->sk_type  = BackStruct.sk_type;
	}else{
		sdata->sk_flag  = 0;
		sdata->sk_dir   = 0;
		sdata->sk_pich  = 0;
		sdata->sk_type  = 0;
	}

	sdata->bs_bno    = BackStruct.bno;		
	sdata->bs_x      = BackStruct.x;		
	sdata->bs_y      = BackStruct.y;
	sdata->bs_w      = BackStruct.w;		
	sdata->bs_h      = BackStruct.h;
	sdata->bs_effect = BackStruct.effect;
	sdata->bs_cg     = BackStruct.cg_flag;

	sdata->bs_flag   = BackStruct.ns_flag;
	sdata->bs_rate   = BackStruct.ns_rate;
	sdata->bs_fade   = BackStruct.ns_fade;
	
	sdata->bs_zoom   = BackStruct.zoom;		
	sdata->bs_cscop  = BackStruct.cscop;	
	sdata->bs_tone   = BackStruct.tone_no;
	sdata->bs_tone2  = BackStruct.tone_no2;
	sdata->bs_tone_ch= BackStruct.tone_char;
	sdata->bs_tone_bk= BackStruct.tone_back;
	sdata->bs_wether = BackStruct.wether;
	sdata->bs_r      = BackStruct.r;
	sdata->bs_g      = BackStruct.g;
	sdata->bs_b      = BackStruct.b;

	

	AVG_SetSaveDataChar( sdata );

	AVG_SetSaveDataNovelMessage( sdata );


	AVG_SetSaveDataSelect( sdata );

	sdata->ws_flag    = Weather.flag;		
	sdata->ws_wno     = Weather.wno;		
	sdata->ws_twind   = Weather.twind;		
	sdata->ws_tspeed  = Weather.tspeed;		
	sdata->ws_tamount = Weather.tamount;	

	sdata->we_flag    = WavEffect.flag;		
	sdata->we_ewav    = WavEffect.ewav;		
	sdata->we_type    = WavEffect.type;		

	

	if(PlayMusicNo!=-1){
		sdata->pm_mus_no[0]   = PlayMusicNo;	
		sdata->pm_mode[0]     = MUSIC_PLAY;		
		sdata->pm_loop[0]     = PlayMusicLoop;		
		sdata->pm_next_vol[0] = PlayMusicVol;	
	}else{
		sdata->pm_mus_no[0]   = 0;	
		sdata->pm_mode[0]     = 0;		
		sdata->pm_loop[0]     = 0;		
		sdata->pm_next_vol[0] = 0;	
	}
	sdata->pm_mus_no[1]   = 0;	
	sdata->pm_mode[1]     = 0;	
	sdata->pm_loop[1]     = 0;	
	sdata->pm_next_vol[1] = 0;	
	for( i=2; i<WAVE_STREAM_NUM ;i++ ){
		sdata->pm_mus_no[i]   = PlayMusic[i].mus_no;	
		sdata->pm_mode[i]     = PlayMusic[i].mode;		
		sdata->pm_loop[i]     = PlayMusic[i].loop;		
		sdata->pm_next_vol[i] = PlayMusic[i].next_vol;	
	}

	for( i=0; i<WAVE_SOUND_NUM ;i++ ){
		sdata->ss_flag[i]     = SeStruct[i].flag;		
		sdata->ss_se_no[i]    = SeStruct[i].se_no;		
		sdata->ss_loop[i]     = SeStruct[i].loop;		
		sdata->ss_vol[i]      = SeStruct[i].next_vol;	
	}

	for( i=0; i<VOICE_CHAR_MAX ;i++ ){
		sdata->sv_cv[i]=VoiceVolumeChar[i];
	}
	sdata->sv_sv=VoiceVolumeScript;
	sdata->VI_EventVoiceNo = VI_EventVoiceNo;
	sdata->VI_EventVoiceNoAll = VI_EventVoiceNoAll;

	CopyMemory( sdata->MapEvent, MapEvent, sizeof(MAP_EVENT)*MAP_EVENT_MAX );
	sdata->MapMax = MapMax;
	sdata->MapStep = MapStep;

	
	CopyMemory( sdata->spright_bmp, SpriteBmp, sizeof(SPRITE_BMP_STRUCT)*MAX_SCRIPT_OBJ );

	sdata->Set_month = Set_month;
	sdata->Set_day   = Set_day;

	memcpy( sdata->NameL,   NameL,   13 );
	memcpy( sdata->NameLK,  NameLK,  13 );
	memcpy( sdata->NameF,   NameF,   13 );
	memcpy( sdata->NameFK,  NameFK,  13 );
	memcpy( sdata->NameNN,  NameNN,  13 );
	memcpy( sdata->NameNNK, NameNNK, 13 );
}
void AVG_SetLoadData( AVG_SAVE_DATA sdata )
{
	int				i;



	for( i=0; i<MAX_CHAR ;i++ ){
		AVG_ReleaseChar( i );
	}
	for( i=0; i<MAX_CHAR ;i++ ){
		if( sdata.cs_flag[i] ){
			AVG_SetChar( sdata.cs_cno[i], sdata.cs_pose[i], sdata.cs_loc1[i], sdata.cs_layer[i], -1, sdata.cs_fade[i], sdata.cs_alph[i], -1 );


			BackStruct.redraw = ON;
		}
	}

	BackStruct.cscop     = sdata.bs_cscop;
	BackStruct.tone_no   = sdata.bs_tone2;
	BackStruct.tone_char = sdata.bs_tone_ch;
	BackStruct.tone_back = sdata.bs_tone_bk;
	BackStruct.wether    = sdata.bs_wether;

	BackStruct.r         = sdata.bs_r;
	BackStruct.g         = sdata.bs_g;
	BackStruct.b         = sdata.bs_b;

	AVG_SetBack( sdata.bs_bno, sdata.bs_x, sdata.bs_y, -1, 2, sdata.bs_cg, 0 );
	if(BackStruct.zoom)	AVG_SetBackPosZoom( sdata.bs_x, sdata.bs_y, sdata.bs_w, sdata.bs_h );
	else				AVG_SetBackPos( sdata.bs_x, sdata.bs_y );
	BackStruct.tone_no  = sdata.bs_tone;
	if( sdata.sk_flag ){
		AVG_SetShake( sdata.sk_type, sdata.sk_pich, 0, sdata.sk_dir );
	}

	AVG_SetNoise( sdata.bs_flag, sdata.bs_rate, 0 );


	AVG_SetLoadDataNovelMessage( &sdata );

	AVG_ResetSelectWindow();
	if( sdata.sw_flag ){
		for( i=0; i<sdata.sw_mnum ;i++ ){
			if(sdata.sw_ex)	AVG_SetSelectMessageEx( sdata.sw_mes[i], sdata.sw_sno[i], sdata.sw_no[i], sdata.sw_value[i] );
			else			AVG_SetSelectMessage( sdata.sw_mes[i], sdata.sw_no[i], sdata.sw_value[i],0 );
		}


		
		AVG_SetHalfTone();

	}

	if(sdata.ws_flag)
		AVG_SetWeather( sdata.ws_wno, sdata.ws_twind, sdata.ws_tspeed, sdata.ws_tamount );

	if(sdata.we_flag)
		AVG_SetWavEffect( sdata.we_ewav, 0, sdata.we_type );

	for( i=0; i<2 ;i++ ){
		if(sdata.pm_mus_no[i]!=-1){
			if(sdata.pm_mode[i]!=MUSIC_STOP && sdata.pm_mode[i]!=MUSIC_FADEOUT){
				AVG_PlayBGM( sdata.pm_mus_no[i], FADE_MUS, sdata.pm_loop[i], sdata.pm_next_vol[i], 1 );
				break;
			}
		}
	}
	for( i=2; i<WAVE_STREAM_NUM ;i++ ){
		if(sdata.pm_mus_no[i]!=-1){
			if(sdata.pm_mode[i]!=MUSIC_STOP && sdata.pm_mode[i]!=MUSIC_FADEOUT){
				AVG_PlayMusic( i, sdata.pm_mus_no[i], FADE_MUS, sdata.pm_loop[i], sdata.pm_next_vol[i], 1 );
			}
		}
	}

	AVG_ReleaseSeAll();
	for( i=0; i<WAVE_SOUND_NUM ;i++ ){
		if(sdata.ss_flag[i] && sdata.ss_loop[i] ){		
			AVG_PlaySE2( i, sdata.ss_se_no[i], FADE_MUS, END_LOOP, sdata.ss_vol[i] );
		}
	}

	for( i=0; i<VOICE_CHAR_MAX ;i++ ){
		VoiceVolumeChar[i]=sdata.sv_cv[i];
	}
	VoiceVolumeScript=sdata.sv_sv;

	VI_EventVoiceNo    = sdata.VI_EventVoiceNo;
	VI_EventVoiceNoAll = sdata.VI_EventVoiceNoAll;

	CopyMemory( MapEvent, sdata.MapEvent, sizeof(MAP_EVENT)*MAP_EVENT_MAX );
	MapMax  = sdata.MapMax;
	MapStep = sdata.MapStep;

	
	for( i=0; i<MAX_SCRIPT_OBJ ;i++ ){
		AVG_LoadBmpSetting( i, sdata.spright_bmp[i] );
	}

	Set_month=sdata.Set_month;
	Set_day=sdata.Set_day;

	memcpy( NameL,   sdata.NameL,   13 );
	memcpy( NameLK,  sdata.NameLK,  13 );
	memcpy( NameF,   sdata.NameF,   13 );
	memcpy( NameFK,  sdata.NameFK,  13 );
	memcpy( NameNN,  sdata.NameNN,  13 );
	memcpy( NameNNK, sdata.NameNNK, 13 );
}

void AVG_SetSave( int save_no )
{
	SAV_Save( save_no );
}
void AVG_SetLoad( int save_no )
{
	LoadNo = save_no;

	AVG_FadeSeAll( 15 );
	MAIN_SetScriptFlag( OFF );	
	AVG_SetFade( 0, 0, 0, ON, -1 );		
	AVG_ChangeSetp( 1, LOAD_FADEOUT );
	MUS_ResetMouseRect_All( );
	MUS_SetMouseLayer( 0 );
}

void AVG_ControlLoad( void )
{
	int		i;
   switch( AvgStep[1] ){
		default:
		case LOAD_NOT:
			break;
		case LOAD_FADEOUT:

			if( !AVG_WaitFade() ){
				AVG_Init();

				AVG_SetBackFadeDirect( 128, 128, 128 );
				SAV_Load( LoadNo );





				DSP_CreateBmp( BMP_DISP, BPP(MainWindow.draw_mode2), DISP_X, DISP_Y );
				DSP_SetDispTemp( BMP_DISP );

				MAIN_SetScriptFlag( OFF );		

				if(MapStep){
					MapStep=0;
					DSP_SetTextDisp( TXT_WINDOW, OFF );
					AVG_ChangeSetp( 0, AVG_MAP );
				}else{
					AVG_ChangeSetp( 0, AVG_GAME );
				}
				AVG_ChangeSetp( 1, LOAD_WORK );
			}
			break;
		case LOAD_WORK:
			AVG_SetBright( 0, 0, 0 );
			AVG_SetFade( 128, 128, 128, OFF, -2 );	

			DSP_ResetDispTemp( );

			DSP_SetGraph( GRP_DISP, BMP_DISP, LAY_CHIP, ON, CHK_NO );
			AVG_ChangeSetp( 1, LOAD_FADEIN );

			for(i=0; i<GRP_DISP ;i++){

				FadeStruct.grp_flag[i] = DSP_GetGraphDisp( i );
				DSP_SetGraphDisp( i, OFF );
			}
			for(i=0; i<TXT_DISP ;i++){
				FadeStruct.txt_flag[i] = DSP_GetTextDisp( i );
				DSP_SetTextDisp( i, OFF );
			}
			MainWindow.draw_flag=ON;
			break;
		case LOAD_FADEIN:





			if( !AVG_WaitFade() ){
				DSP_ResetGraph( GRP_DISP );
				DSP_ReleaseBmp( BMP_DISP );
				for(i=0; i<GRP_DISP ;i++){
					if( FadeStruct.grp_flag[i] )DSP_SetGraphDisp( i, ON );
				}
				for(i=0; i<TXT_DISP ;i++){
					if( FadeStruct.txt_flag[i] )DSP_SetTextDisp( i, ON );
				}

				MAIN_SetScriptFlag( ON );	
				AVG_ChangeSetp( 1, LOAD_NOT );
			}
			break;
	}
}

enum{
	GOTILE_NOT,
	GOTILE_FADEOUT,
	GOTILE_END,
};
int	GotoTitle=0;
BOOL AVG_CheckGotoTitle( void )
{


	return AVG_ConfigCheck() && AvgStep[2]!=GOTILE_FADEOUT;
}

void AVG_SetGotoTitle( void )
{
	if(AvgStep[2]!=GOTILE_FADEOUT){
		GotoTitle = 1;
		Avg.demo=OFF;
		Avg.msg_cut=OFF;
		Avg.msg_cut_mode=OFF;
		Avg.auto_flag=OFF;

		AVG_StopBGM( 15 );
		AVG_FadeSeAll( 15 );

		MAIN_SetScriptFlag( OFF );			
		AVG_SetFade( 0, 0, 0, ON, -1 );		


		AVG_ChangeSetp( 2, GOTILE_FADEOUT );
		MUS_ResetMouseRect_All( );
		AVG_ResetSelectWindow();

		AVG_ResetEnding();
	}
}

void AVG_ControlGotoTitle( void )
{
	int	i;

	switch( AvgStep[2] ){
		default:
		case GOTILE_NOT:
			break;
		case GOTILE_FADEOUT:
			DSP_ResetGraph( GRP_KEYWAIT );
			if( !AVG_WaitFade() ){
				AVG_ChangeSetp( 2, GOTILE_END );
			}
			break;
		case GOTILE_END:
			DSP_TermDisp();
			AVG_StopBGM( FADE_MUS );
			for(i=0; i<WAVE_SOUND_NUM ;i++)
				AVG_StopSE2( i,0 );
			AVG_FadeOutVoiceAll( 30 );
			
			MAIN_TermScript();

			AVG_Init();
			AVG_ChangeSetp( 2, GOTILE_NOT );
			AVG_ChangeSetp( 0, AVG_END );
			EndNext = GAME_TITLE;


			break;
	}
}




void AVG_Init(void)
{


	FNT_Term();

	FNT_LoadFont( RUBI_FONT, Avg.msg_font );
	FNT_LoadFont( SYS_FONT,  Avg.msg_font );



	AVG_ResetBack( ON);
	InitNovelMessage();

	ESC_InitFlag();
	ZeroMemory( &AvgStep, sizeof(int)*5 );

	ZeroMemory( &AvgCounter, sizeof(int)*5 );

	ZeroMemory( &BackStruct, sizeof(BACK_STRUCT) );
	AVG_SetBackFadeDirect( 128, 128, 128 );
	AVG_SetTimeMode( 0, 0 );

	AVG_InitChar();

	ZeroMemory( &Weather, sizeof(WEATHER_STRUCT) );
	ZeroMemory( &WavEffect, sizeof(WAV_EFFECT) );
	ZeroMemory( &LensFrea, sizeof(LENS_FREA) );


	ZeroMemory( &SaveWindow, sizeof(SAVE_WINDOW) );
	ZeroMemory( &Config, sizeof(CONFIG_STRUCT) );
	ZeroMemory( &FadeStruct, sizeof(FADE_STRUCT) );
	AVG_SetBright( 128,128,128 );

	AVG_ResetHalfTone();

	BackStep = 0;


	MUS_ResetMouseRect_All();
	AVG_InitWindow();
	MUS_SetMouseLayer( 0 );

	MainWindow.draw_matrix = OFF;
	MainWindow.cutcut      = OFF;




	SetVoiceDialog( Debug.flag );
}

void AVG_Term(void)
{
	AVG_TermWindow();
}

BOOL AVG_ConfigCheck(void)
{
	BOOL	bk_flag;
	BOOL	char_flag;
	BOOL	sl_flag;
	BOOL	fd_flag;
	BOOL	wf_flag;
	BOOL	go_flag;
	BOOL	ch_flag, load_flag;

	
	bk_flag = !(BackStruct.cs_flag || BackStruct.sc_flag || BackStruct.fd_flag
			||  BackStruct.br_flag || AVG_WaitShake() );

	
	char_flag = AVG_ConfigCheckChar();
	
	sl_flag = AVG_ConfigCheckSelect();
	
	fd_flag = !FadeStruct.flag;
	
	wf_flag = AVG_ConfigCheckMessage();
	
	
	go_flag = (AVG_GetSelectMessageFlag() || AVG_GetNovelMessageFlag() ) && AVG_GetNovelMessageConfig();
	go_flag = AVG_ChexkMapEventConfig() || go_flag;

	ch_flag = !ChipBackStruct.flag;

	load_flag = !AVG_GetLaodFlag();


	return bk_flag && char_flag && sl_flag && fd_flag && wf_flag && go_flag && ch_flag && load_flag;
}
int AVG_GetLaodFlag(void)
{
	return AvgStep[1]!=LOAD_NOT;
}

void AVG_EndConfig( void )
{
	if(BackStep==AVG_GAME){
		MAIN_SetScriptFlag( ON );	

		AVG_OpenSelectWindow();
		AVG_OpenWindow( ON, 1 );				
		AVG_OpenBack();
	}
	AVG_ChangeSetp( 0, BackStep );
	AVG_TermIndicator( GRP_SYSTEM+6, BMP_SYSTEM+5 );
}

void AVG_GoConfig( int mode )
{
	if(AvgStep[0]==AVG_MAP){
		AVG_CloseMapEvent();
	}
	if(AvgStep[0]==AVG_GAME){
		MAIN_SetScriptFlag( OFF );	
		AVG_CloseSelectWindow();
		AVG_CloseWindow(1);			
		AVG_CloseChar();

		AVG_SetHalfTone();	
		AVG_CloseBack();	

		AVG_ResetLensFrea();
	}
	AVG_FadeOutVoiceAll( 30 );

	ConfigOpenMode = mode;
	switch(mode){
		case 0:
			AVG_PlaySE3( 9002, 150 );
			AVG_SetConfigWindow(0);
			AVG_ChangeSetp( 0, AVG_CONFIG );
			break;
		case 1:

			GWIN_SetSaveLoadWindow(SW_SAVE_MODE,BMP_SAVE, GRP_SAVE, TXT_SAVE, GRP_SYSTEM, TXT_SYSTEM, 1, 0);
			AVG_ChangeSetp( 0, AVG_SAVE );
			break;
		case 2:

			GWIN_SetSaveLoadWindow(SW_LOAD_MODE,BMP_SAVE, GRP_SAVE, TXT_SAVE, GRP_SYSTEM, TXT_SYSTEM, 1, 0);
			AVG_ChangeSetp( 0, AVG_LOAD );
			break;
		case 3:

			GWIN_SetSideBarWindow( BMP_SAVE, GRP_SAVE, TXT_SAVE, 1, 0 );
			AVG_ChangeSetp( 0, AVG_SETTING );
			break;
	}
	DSP_GetDispBmp( BMP_CAP, MainWindow.draw_mode2 );

	BackStep = AvgStep[0];
}


void AVG_SetOpening( void )
{
	movPlayerWnd = new MovPlayerWnd;
	movPlayerWnd->PlayMediaFile( "toHeart2.mpg", MainWindow.hwnd );
}


void AVG_SetEndingMov( int end_no )
{
	movPlayerWnd = new MovPlayerWnd;
	movPlayerWnd->PlayMediaFile( "End00.mpg", MainWindow.hwnd );
}

int	MovieMode=0;
void AVG_SetMovie( int mode, int no )
{
	char	fname[256];

	movPlayerFrm = new CMovPlayerFrm;
	MovieMode = mode;

EnableWindosChengMenu(1);
	switch(mode){
		case 0:
			switch(Avg.mov_lv){
				default:
				case 0:	movPlayerFrm->OpenMovie( MainWindow.hwnd, "TH2_OP_480x272_2M.avi", 0,76,800, 448 );	break;
				case 1:	movPlayerFrm->OpenMovie( MainWindow.hwnd, "TH2_OP_640x352_4M.avi", 0,76,800, 448 );	break;
				case 2:	movPlayerFrm->OpenMovie( MainWindow.hwnd, "TH2_OP_800x448_5M.avi", 0,76,800, 448 );	break;
			}
			AVG_PlayBGM( 0, 0, FALSE, 255, TRUE );
			break;
		case 1:
			switch(Avg.mov_lv){
				default:
				case 0:	wsprintf( fname, "TH2_ED_%02d_640_1_5M.avi", no );	break;
				case 1:	wsprintf( fname, "TH2_ED_%02d_640_1_5M.avi", no );	break;
				case 2:	wsprintf( fname, "TH2_ED_%02d_800_3M.avi", no );	break;
			}

			movPlayerFrm->OpenMovie( MainWindow.hwnd, fname, 0,0,800, 600 );
			AVG_PlayBGM( 50, 0, FALSE, 255, TRUE );
			break;
		case 2:
			switch(Avg.mov_lv){
				default:
				case 0:	movPlayerFrm->OpenMovie( MainWindow.hwnd, "TH2_TR_480x360_2M.avi", 0,0,800, 600 );	break;
				case 1:	movPlayerFrm->OpenMovie( MainWindow.hwnd, "TH2_TR_640x480_4M.avi", 0,0,800, 600 );	break;
				case 2:	movPlayerFrm->OpenMovie( MainWindow.hwnd, "TH2_TR_800x600_5M.avi", 0,0,800, 600 );	break;
			}
			AVG_PlayBGM( 99, 0, FALSE, 255, TRUE );
			break;
		case 3:
			switch(Avg.mov_lv){
				default:
				case 0:	movPlayerFrm->OpenMovie( MainWindow.hwnd, "Leaf_480x360_2M.avi", 0,0,800,600 );	break;
				case 1:	movPlayerFrm->OpenMovie( MainWindow.hwnd, "Leaf_640x480_4M.avi", 0,0,800,600 );	break;
				case 2:	movPlayerFrm->OpenMovie( MainWindow.hwnd, "Leaf_800x600_5M.avi", 0,0,800,600 );	break;
			}
			break;
	}

}

BOOL AVG_KeySkip( void )
{
	if(MovieMode!=1){
		if( MUS_GetMouseTrigger(MOUSE_LBUTTON) || KeyCond.trg.enter ){
			if( MovieMode==0 && AVG_GetGameFlag(98)==0 ){
				return 0;
			}
			if( MovieMode==2 && AVG_GetGameFlag(99)==0 ){
				return 0;
			}
			return 1;
		}
	}
	if( MUS_GetMouseTrigger(MOUSE_RBUTTON) || KeyCond.trg.space || KeyCond.trg.esc){
		if( MovieMode==0 && AVG_GetGameFlag(98)==0 ){
			return 0;
		}
		if( MovieMode==2 && AVG_GetGameFlag(99)==0 ){
			return 0;
		}
		if( MovieMode==1 && AVG_GetGameFlag(80)==0){
			return 0;
		}
		return 1;
	}else{
		return 0;
	}
}

BOOL AVG_WaitMovie( void )
{
	if(movPlayerFrm){
		if(movPlayerFrm->bEnd){
			AVG_StopBGM( 0 );
			return 0;
		}else{
			return 1;
		}
	}else{
		AVG_StopBGM( 0 );
		return 0;
	}

}

void AVG_ControlSystem2( void )
{
	int	cansel = GameKey.cansel;
	int	click  = GameKey.click;




		Avg.msg_cut = GameKey.mes_cut;
		if( GameKey.mes_cut_mode ){
			Avg.msg_cut_mode = !Avg.msg_cut_mode;
		}





	
	if( GameKey.cansel || GameKey.click || GameKey.diswin || 
		GameKey.home || GameKey.end || GameKey.pdown || GameKey.pup || Avg.demo ){
		Avg.msg_cut = OFF;
		Avg.msg_cut_mode = OFF;
	}
	if(GameKey.mes_cut){
		Avg.msg_cut_mode = OFF;
		Avg.auto_flag=OFF;
	}
	if( GameKey.cansel || GameKey.diswin || GameKey.pup ){
		Avg.auto_flag=OFF;
	}

	if(!TaikenBan){
		if( cansel && AVG_ConfigCheck() ){
			int	i;
			for(i=0;i<MAX_SCRIPT_OBJ;i++){
				if(SpriteBmp[i].flag){
					DebugBox( NULL, "SetBmpの解除し忘れ(%d)%s", i, SpriteBmp[i].fname );
					AVG_ReleaseBmp(i);
				}
			}

			AVG_GoConfig(0);
		}
	}



	return ;
}
#include "movie.h"
#include "movPlayer.h"

#include <math.h>

void AVG_SetSakura( int a, int b, int c, int scr )
{
	AVG_SetWeather( WEATH_SAKURA, 1, 10, a, scr );




}
void AVG_StopSakura( void )
{
	AVG_ResetWeather();


}


MAP_EVENT	MapEvent[MAP_EVENT_MAX];
int	MapMax = 0;
int	MapStep = 0;

char	*MapEventCharName[15] = {	"( 0)不正　　",	
									"( 1)このみ　",
									"( 2)愛佳　　",
									"( 3)環　　　",
									"( 4)花梨　　",
									"( 5)珊瑚　　",
									"( 6)瑠璃　　",
									"( 7)由真　　",
									"( 8)ルーシー",
									"( 9)優希　　",
									"(10)珊瑚瑠璃",
									"(11)ハテナ　",
									"(12)このみ環",
									"(13)雄二　　",
									"(14)ささら　"
 };

char	*MapEventPosName[22] = {	"( 0)自宅　　",	
									"( 1)商店街　",
									"( 2)ゲーセン",
									"( 3)公園　　",
									"( 4)中学校　",
									"( 5)坂道　　",
									"( 6)校門前　",
									"( 7)校庭　　",
									"( 8)裏庭　　",
									"( 9)駐輪場　",
									"(10)下駄箱　",
									"(11)体育館　",
									"(12)書庫　　",
									"(13)図書室　",
									"(14)視聴覚室",
									"(15)廊下１Ｆ",
									"(16)廊下２Ｆ",
									"(17)教室１Ｆ",
									"(18)教室２Ｆ",
									"(19)廊下３Ｆ",
									"(20)教室３Ｆ",
									"(21)生徒会室"
};

char	*MapEventCharType[4] = {	"(0)制服　　",	
									"(1)私服　　",	
									"(2)体操服　",	
									"(3)中学制服" };	

typedef struct {
	int		map;
	int		cx;
	int		cy;
	int		kaburi;
}MAP_EVENT_CHAR;

MAP_EVENT_CHAR	MapEventCharPos[22] = {
	{0, 280,340, 0},
	
	{2, 488,210, 1},
	{2, 326,356, 2},
	
	{3, 262,304, 3},
	{3, 464,220, 4},
	
	{4, 330,216, 5},
	{4, 490,294, 6},
	
	{1, 356,412, 7},
	{1, 288,210, 8},
	{1, 288,210, 8},
	{1, 288,210, 8},
	{1, 567,326, 9},
	{1, 288,210, 8},
	{1, 288,210, 8},
	{1, 288,210, 8},
	{1, 288,210, 8},
	{1, 288,210, 8},
	{1, 288,210, 8},
	{1, 288,210, 8},
	{1, 288,210, 8},
	{1, 288,210, 8},
	{1, 288,210, 8},
};

void AVG_SetMapEvent( int chr_no, int pos, int type, char *filename )
{
	MapEvent[MapMax].chr_no = chr_no;
	MapEvent[MapMax].pos    = pos;
	MapEvent[MapMax].type   = type;
	wsprintf( MapEvent[MapMax].script_fname, "%s.sdt", filename );

	MapMax++;
}


static int AVG_LoadScript( char *str )
{
	int	ret;
	int	i;

	for(i=0;i<MAX_SCRIPT_OBJ;i++){
		if(SpriteBmp[i].flag){
			DebugBox( NULL, "SetBmpの解除し忘れ(%d)%s", i, SpriteBmp[i].fname );
			AVG_ReleaseBmp(i);
		}
	}


	
	ret = EXEC_SLoad( str );
	if( ret ){
		AVG_SetScenarioNo( GetScenarioNo(str) );
	}
	return ret;

}

int MapEventConfigFlag=0;

int AVG_ChexkMapEventConfig( void )
{
	return MapEventConfigFlag;
}

void AVG_CloseMapEvent( void )
{
	int	i;
	for(i=0;i<40;i++)
		DSP_SetGraphDisp( GRP_MAP+i, OFF );
}


int AVG_ControlMapEvent( void )
{
	int		_game=0;
	int		i;
	int		cnt;

	int		x1=0,x2=0;
	int		y1=0,y2=0;

	static	scnt=0;

	int		select = MUS_GetMouseNo(-1);
	static	select_back=0;
	static	select_chr;
	static	mleyer;
	
	int		CharErr[15];
	int		PosErr[22];
	char	fname[256];
	int		sakura_type = 3;

	static	int	EventFieldList[5];
	static	int	EventFieldCount=1;
	static	int	EventFieldCountBack=1;

	static	int	EventFieldRoolCount=0;
	static	int	EventFieldKaburi[10];

	MapEventConfigFlag=0;
	switch(MapStep){
		case 0:
			mleyer = MUS_GetMouseLayer();
			MUS_SetMouseLayer( 10 );

			for(i=0;i<15;i++){	CharErr[i]=0;	}
			for(i=0;i<22;i++){	PosErr[i]=0;	}
			for(i=0;i<10;i++){	EventFieldKaburi[i]=0; }

			if(MapMax){
				for(i=0;i<MapMax;i++){
					CharErr[ MapEvent[i].chr_no ]++;
					PosErr[ MapEvent[i].pos ]++;
				}
				for(i=0;i<15;i++){
					if(CharErr[i]>=2){
						DebugBox( NULL, "同じキャラが同時に出ています[%s]", MapEventCharName[i] );
					}
				}
				if( (CharErr[1] && CharErr[12]) || (CharErr[3] && CharErr[12]) ){
					DebugBox( NULL, "このみ環璃セットと、このみか、環が、単体で別々に出ています" );
				}

				if( (CharErr[4] && CharErr[10]) || (CharErr[5] && CharErr[10]) ){
					DebugBox( NULL, "珊瑚瑠璃セットと、珊瑚か、瑠璃が、単体が別々に出ています" );
				}





			}
			if(PosErr[0]==0){



				MapMax++;
			}

			MapStep=1;
			scnt = 0;
			break;
		case 1:
			if( AVG_ViewClock( 19 ) ){
				MapStep = 2;
				scnt = 0;

				AVG_PlayBGM( 10, 30, ON, 255, 0 );
				
				AVG_ResetBack(0);

				DSP_LoadBmp( BMP_MAP, BMP_FULL, "map000.tga" );
				DSP_SetGraph( GRP_MAP, BMP_MAP, LAY_MAP, OFF, CHK_NO );

				for(i=0;i<5;i++){
					EventFieldList[i]=0;
				}
				EventFieldList[1] = 1;
				EventFieldCount = 1;
				EventFieldRoolCount=0;

				for(i=0;i<MapMax;i++){
					EventFieldList[ MapEventCharPos[MapEvent[i].pos].map ] = 1;
				}

				{
					int m, d;
					d = ESC_GetFlag( _DAY  );
					m = ESC_GetFlag( _MONTH );
					
					
					
					
					
					
					
					if( m == 3) {
						
						if( d >= 1 && d<=15)		sakura_type = 4;
						else if( d >= 16 && d<= 28)	sakura_type = 2;
						else 						sakura_type = 0;
					} else if( m == 4) {
						
						if( d >= 1 && d <= 15)		sakura_type = 0;
						else if( d>= 16 && d<= 27)	sakura_type = 2;
						else						sakura_type = 3;
					} else {
						
						sakura_type = 3;
					}
				}

				for(i=0;i<5;i++){
					if(EventFieldList[i]){
						if(i==1 || i==4) wsprintf( fname, "map1%d%d.tga", i, sakura_type );
						else			 wsprintf( fname, "map1%d%d.tga", i, 0 );
						DSP_LoadBmp( BMP_MAP+1+i, BMP_FULL, fname );			

						DSP_SetGraph( GRP_MAP+1+i, BMP_MAP+1+i, LAY_MAP+1, OFF, CHK_ANTI );
						DSP_SetGraphMove( GRP_MAP+1+i, (800-640)/2, (600-448)/2 );
					}
				}

				DSP_LoadBmp( BMP_MAP+6, BMP_256P, "map010.tga" );
				DSP_SetGraph( GRP_MAP+6, BMP_MAP+6, LAY_MAP+4, OFF, CHK_ANTI );
				DSP_SetGraph( GRP_MAP+7, BMP_MAP+6, LAY_MAP+4, OFF, CHK_ANTI );
				DSP_SetGraphPos( GRP_MAP+6, 80-56,  (600-122)/2, 112*0,    0, 56, 122 );
				DSP_SetGraphPos( GRP_MAP+7, 80+640, (600-122)/2, 112*0+56, 0, 56, 122 );
				MUS_SetMouseRect( 10, 16, 80-56,  (600-122)/2, 56, 122, 1 );
				MUS_SetMouseRect( 10, 17, 80+640, (600-122)/2, 56, 122, 1 );

				
				DSP_LoadBmp( BMP_MAP+7, BMP_256P, "map011.tga" );
				for(i=0;i<MapMax;i++){
					EventFieldKaburi[ MapEventCharPos[MapEvent[i].pos].kaburi ]++;

					int	cx = MapEventCharPos[MapEvent[i].pos].cx;
					int	cy = MapEventCharPos[MapEvent[i].pos].cy;
					switch(EventFieldKaburi[ MapEventCharPos[MapEvent[i].pos].kaburi ]){
						case 1:		break;
						case 2:	cx-=200;	break;
						case 3:	cx+=200;	break;
						case 4:	cx-=100; cy+=160; break;
					
						default:
							DebugBox(NULL, "現状では同一場所に５人以上の表示は不正です。" );
							break;
					}
					
					DSP_SetGraph( GRP_MAP+8+i,  BMP_MAP+7,   LAY_MAP+2, OFF, CHK_ANTI );
					DSP_SetGraphPos( GRP_MAP+8+i, cx+20, cy-118, 0, 118*MapEvent[i].pos, 130, 118 );
					MUS_SetMouseRect( 10, i, cx+20, cy-118, 130, 118, 1 );
				
					if(MapEvent[i].chr_no!=0){
						wsprintf( fname, "mapc%02d%d.ani", MapEvent[i].chr_no, MapEvent[i].type );
						DSP_LoadSprite( BMP_MAP+8+i, fname );
						DSP_SetSprite( GRP_MAP+24+i, BMP_MAP+8+i, 0, LAY_MAP+3, OFF, TRUE, 0, OFF );
						DSP_SetGraphMove( GRP_MAP+24+i, cx, cy );
					}
				}



			}
			break;
		case 2:
			DSP_SetGraphDisp( GRP_MAP, ON );
			DSP_SetGraphDisp( GRP_MAP+1+1, ON );
			DSP_SetGraphDisp( GRP_MAP+6, ON );
			DSP_SetGraphDisp( GRP_MAP+7, ON );

			DSP_SetGraphParam( GRP_MAP, DRW_BLD(scnt*16) );
			DSP_SetGraphParam( GRP_MAP+1+1, DRW_BLD(scnt*16) );
			DSP_SetGraphParam( GRP_MAP+6, DRW_BLD(scnt*16) );
			DSP_SetGraphParam( GRP_MAP+7, DRW_BLD(scnt*16) );

			scnt++;
			if(scnt==16){
				DSP_SetGraphParam( GRP_MAP, DRW_NML );
				DSP_SetGraphParam( GRP_MAP+1+1, DRW_NML );
				DSP_SetGraphParam( GRP_MAP+6, DRW_NML );
				DSP_SetGraphParam( GRP_MAP+7, DRW_NML );

				MapStep=3;
			}
			break;
		case 3:
			if(AvgStep[0]!=AVG_MAP){
				AVG_CloseMapEvent();
				break;
			}
			DSP_SetGraphDisp( GRP_MAP, ON );

			MapEventConfigFlag=1;
			for(i=0;i<5;i++){
				DSP_SetGraphDisp( GRP_MAP+1+i, OFF );
			}
			if( EventFieldRoolCount==0 ){
				for(i=0;i<MapMax;i++){
					if( MapEventCharPos[MapEvent[i].pos].map==EventFieldCount){
						DSP_SetGraphDisp( GRP_MAP+8+i,  ON );
						DSP_SetGraphParam( GRP_MAP+8+i, DRW_NML );
						if(MapEvent[i].chr_no!=0){
							DSP_SetGraphDisp( GRP_MAP+24+i, ON );
							DSP_SetGraphParam( GRP_MAP+24+i, DRW_NML );
						}
					}else{
						DSP_SetGraphDisp( GRP_MAP+8+i,  OFF );
						if(MapEvent[i].chr_no!=0){
							DSP_SetGraphDisp( GRP_MAP+24+i, OFF );
						}
					}
				}
	
				DSP_SetGraphDisp( GRP_MAP+1+EventFieldCount, ON );
				DSP_SetGraphMove( GRP_MAP+1+EventFieldCount, (800-640)/2+x1, (600-448)/2+y1 );

				DSP_SetGraphDisp( GRP_MAP+6, ON );
				DSP_SetGraphDisp( GRP_MAP+7, ON );
				
				DSP_SetGraphSMove( GRP_MAP+6, 112*0, 0 );
				DSP_SetGraphSMove( GRP_MAP+7, 112*0+56, 0 );


				for(i=0;i<MapMax;i++){
					DSP_SetGraphSMove( GRP_MAP+8+i, 0, 118*MapEvent[i].pos );
					if( MapEventCharPos[MapEvent[i].pos].map==EventFieldCount){
						MUS_SetMouseRectFlag( 10, i, ON );
					}else{
						MUS_SetMouseRectFlag( 10, i, OFF );
					}
				}

				if(select!=-1 && select != select_back )
					AVG_PlaySE3( 9108, 255 );

				switch(select){
					case -1:
						break;
					default:
						if( MapEventCharPos[MapEvent[select].pos].map==EventFieldCount){

							DSP_SetGraphSMove( GRP_MAP+8+select, 130, 118*MapEvent[select].pos );
							if(GameKey.click){
								AVG_PlaySE3( 9014, 255 );

								DSP_SetGraphSMove( GRP_MAP+8+select, 130*2, 118*MapEvent[select].pos );

								select_chr = select;
								MapStep=4;
								scnt = 0;
							}
						}
						break;
					case 16:
						DSP_SetGraphSMove( GRP_MAP+6, 112*1, 0 );
						if(GameKey.click){
							AVG_PlaySE3( 9015, 255 );

							DSP_SetGraphSMove( GRP_MAP+6, 112*2, 0 );
							GameKey.pup=1;
						}
						break;
					case 17:
						DSP_SetGraphSMove( GRP_MAP+7, 112*1+56, 0 );
						if(GameKey.click){
							AVG_PlaySE3( 9015, 255 );

							DSP_SetGraphSMove( GRP_MAP+7, 112*2+56, 0 );
							GameKey.pdown=1;
						}
						break;

				}

				if(GameKey.pup){
					AVG_PlaySE3( 9015, 255 );
					EventFieldCountBack=EventFieldCount;
					EventFieldRoolCount=16;

					while(1){
						EventFieldCount = (EventFieldCount+1)%5;
						if(EventFieldList[EventFieldCount])break;

					}
				}
				if(GameKey.pdown){
					AVG_PlaySE3( 9015, 255 );
					EventFieldCountBack=EventFieldCount;
					EventFieldRoolCount=-16;

					while(1){
						EventFieldCount = (EventFieldCount-1+5)%5;
						if(EventFieldList[EventFieldCount])break;
					}
				}
				select_back=select;
			}else{

				for(i=0;i<MapMax;i++){
					if( MapEventCharPos[MapEvent[i].pos].map==EventFieldCount || MapEventCharPos[MapEvent[i].pos].map==EventFieldCountBack ){
						if( MapEventCharPos[MapEvent[i].pos].map==EventFieldCount ){
							cnt = 8-abs(EventFieldRoolCount);
						}else{
							cnt = abs(EventFieldRoolCount)-8;
						}
						DSP_SetGraphDisp( GRP_MAP+8+i,  ON );
						DSP_SetGraphParam( GRP_MAP+8+i, DRW_BLD(cnt*32) );
						if(MapEvent[i].chr_no!=0){
							DSP_SetGraphDisp( GRP_MAP+24+i, ON );
							DSP_SetGraphParam( GRP_MAP+24+i, DRW_BLD(cnt*32) );
						}
					}else{
						DSP_SetGraphDisp( GRP_MAP+8+i,  OFF );
						if(MapEvent[i].chr_no!=0){
							DSP_SetGraphDisp( GRP_MAP+24+i, OFF );
						}
					}
				}
				
				DSP_SetGraphDisp( GRP_MAP+1+EventFieldCount, ON );
				DSP_SetGraphDisp( GRP_MAP+1+EventFieldCountBack, ON );
				if( EventFieldRoolCount>0 ){
					EventFieldRoolCount--;
					cnt = EventFieldRoolCount*EventFieldRoolCount;

					x1  = 800*cnt/256;
					x2  = 800*(cnt-256)/256;
				}else{
					EventFieldRoolCount++;
					cnt = EventFieldRoolCount*EventFieldRoolCount;

					x1  = -800*cnt/256;
					x2  = -800*(cnt-256)/256;
				}
				DSP_SetGraphMove( GRP_MAP+1+EventFieldCount,     (800-640)/2+x1, (600-448)/2+y1 );
				DSP_SetGraphMove( GRP_MAP+1+EventFieldCountBack, (800-640)/2+x2, (600-448)/2+y2 );
			}
			break;
		case 4:
			scnt++;
			for(i=0;i<40;i++)
				DSP_SetGraphFade( GRP_MAP+i, 128-scnt*8 );
			if(scnt==16){
				AVG_ResetBack(0);

				MainWindow.draw_flag=1;
				DSP_SetGraphFade( GRP_MAP+i, 0 );
				MapStep = 5;
			}
			break;
		case 5:
			for(i=0;i<24;i++)
				DSP_ReleaseBmp( BMP_MAP+i );
			for(i=0;i<40;i++)
				DSP_ResetGraph( GRP_MAP+i );

			if( MapEvent[select_chr].script_fname[0] ){
				AVG_LoadScript(MapEvent[select_chr].script_fname);
				ESC_SetFlag(_EVENT_END,1);
			}else{

				ESC_SetFlag(_EVENT_END,1);
				ESC_SetFlag(_EVENT_NEXT,6);
			}

			MUS_ResetMouseRect_Layer( 10 );
			MUS_SetMouseLayer( mleyer );
			ZeroMemory( MapEvent, sizeof(MAP_EVENT)*MAP_EVENT_MAX );
			MapMax=0;
			MapStep = 0;

			_game = 1;
			break;
	}
	return _game;
}



int AVG_ToHertDaySinkou( int script )
{
	int	_map = 0;
	if( script==SCCODE_NOOPR && MAIN_GetScriptFlag()!=0){
		char buf[256];
		int	_times = ESC_GetFlag( _TIME );
		int	_days  = ESC_GetFlag( _DAY  );
		int	_month = ESC_GetFlag( _MONTH );
		int	_end   = ESC_GetFlag(_EVENT_END);
		int	_event = ESC_GetFlag(_EVENT_NEXT);

		if( ESC_GetFlag(_DEBUG_CALL	) ){
			AVG_LoadScript( "999999999.sdt" );
			return 0;
		}

		if( _end ){
			if( _event==-1 ){
				_times++;
			}else{
				_times = _event;
			}
			if( _times>=7 ){
				ZeroMemory( MapEvent, sizeof(MAP_EVENT)*MAP_EVENT_MAX );
				MapMax=0;

				if(Set_month!=0){
					ESC_SetFlag( _MONTH, Set_month );
					ESC_SetFlag( _DAY  , Set_day );

					Set_month = 0;
					Set_day   = 0;
					_days  = ESC_GetFlag( _DAY  );
					_month = ESC_GetFlag( _MONTH );
				}else{
					_days++;
				}
				_times = 0;

				ESC_SetFlag( _CLOCK_TIME, 0 );
				if( ESC_GetFlag(_CALENDER_SKIP) ){
					ESC_SetFlag(_CALENDER_SKIP,0);
				}else{
					_map=2;
				}

				if( (_month==3 && _days>=32) || (_month==4 && _days>=31) ){
					_days=1;
					_month++;
				}
			}
		}
		int			_youbi=0;
		int			_syukujitu=0;

		switch( _month ){
		case 3:	_youbi = (_days)%7; break;
		case 4:	_youbi = (_days+3)%7; break;
		case 5:	_youbi = (_days+5)%7; break;
		}
		if( _month==3 && _days==20 ) _syukujitu=1;
		if( _month==4 && _days==29 ) _syukujitu=1;
		if( _month==5 && _days==3 ) _syukujitu=1;
		if( _month==5 && _days==4 ) _syukujitu=1;
		if( _month==5 && _days==5 ) _syukujitu=1;

		if( _month==3 && _days>=25 ) _syukujitu=2;
		if( _month==4 && _days<=7  ) _syukujitu=2;


		if( (_youbi==0 || _syukujitu!=0) && _times==5 ){
			_times=6;
		}
		switch( _times ){
			case 0:	wsprintf( buf, "EV_%02d%02dMORNING.sdt",_month, _days );	break;	
			case 1:	wsprintf( buf, "EV_%02d%02dINTERVAL.sdt",_month, _days );	break;	
			case 2:	wsprintf( buf, "EV_%02d%02dLUNCH_BREAK.sdt",_month, _days );	break;	
			case 3:	wsprintf( buf, "EV_%02d%02dSCHOOL_HOURS.sdt",_month, _days );	break;	
			case 4:	wsprintf( buf, "EV_%02d%02dAFTER_SCHOOL.sdt",_month, _days );	break;	
			case 5:	
				_map=1;

				break;
			case 6:	wsprintf( buf, "EV_%02d%02dNIGHT.sdt",_month, _days );	break;	
		}

		ESC_SetFlag( _MONTH, _month );
		ESC_SetFlag( _DAY, _days );
		ESC_SetFlag( _TIME, _times );
		if( _map!=1 ){
			ESC_SetFlag(_EVENT_END, 0 );
			ESC_SetFlag( _EVENT_NEXT, -1 );
			AVG_LoadScript( buf );
		}
	}
	return _map;
}

int AVG_System( int script )
{
	int		ret = GAME_AVG;
	int		bmp_bpp = BPP(MainWindow.draw_mode2);
	static int ch_no;
	int		map;

	AVG_GetGameKey();

	if(Debug.flag){
		if(GameKey.click){
			GameKey.click=1;
		}
	}
	ESC_SetFlag( _DEFAULT_NAME, DefaultCharName );

	switch( AvgStep[0] ){
		default:
		case AVG_INIT:
			AVG_Init();

			ESC_SetFlag( _MONTH, 3  );
			ESC_SetFlag( _DAY, 1  );
			ESC_SetFlag( _TIME, 0  );
			ESC_SetFlag( _EVENT_END, 0 );
			ESC_SetFlag( _EVENT_NEXT, -1 );

			ESC_SetFlag( _CALENDER_SKIP, 0 );
			ESC_SetFlag( _CLOCK_TIME, 0 );
			
			AVG_SkipDate( 0, 0 );

			ZeroMemory( MapEvent, sizeof(MAP_EVENT)*MAP_EVENT_MAX );
			MapMax=0;

			if(!Avg.omake){
				AVG_SetBack( 0, 0, 0, -1, 1, 0, 0 );
			}
			MAIN_SetScriptFlag( ON );
			AVG_ChangeSetp( 0, AVG_GAME );
			
		case AVG_GAME:
			map = AVG_ToHertDaySinkou(script);
			if( map==1 ){
				AVG_ChangeSetp( 0, AVG_MAP );
			}else if( map==2 ){
				MAIN_SetScriptFlag( OFF );	
				AVG_ChangeSetp( 0, AVG_CALENDER );
			}else{
				AVG_ControlSystem2();

	
				AVG_ControlNovelMessage();
				AVG_ControlHalfTone();
				AVG_ControlText();

				AVG_ControlBack();
				AVG_ControlChar();
	
				AVG_ControlEnding();

				AVG_ControlLensFrea();
				AVG_ControlWeather();

				AVG_ControlWavEffect();
				AVG_ControlWarp();
				AVG_ControlSelectWindow();
				AVG_ControlSystem();
			}
	MapEventConfigFlag=0;
			break;
		case AVG_MAP:
			if( AVG_ControlMapEvent() ){
				AVG_ChangeSetp( 0, AVG_GAME );
			}
			AVG_ControlSystem2();
			break;
		case AVG_CALENDER:
			if( AVG_SetCalender( -1, -1 ) ){
				MAIN_SetScriptFlag( ON );	
				AVG_ChangeSetp( 0, AVG_GAME );
			}
			break;
		case AVG_CONFIG:
			AVG_ControlSystem();
			AVG_ControlHalfTone();
			break;
		case AVG_SAVE:
		case AVG_LOAD:
			AVG_ControlSystem();
			break;
		case AVG_SETTING:
			AVG_ControlSystem();
			break;
		case AVG_END:
			AVG_Term();
			ret = EndNext;
			AVG_ChangeSetp( 0, AVG_INIT );
			break;
	}
	AVG_ControlMusic();
	AVG_ControlLoad();
	AVG_ControlGotoTitle();
	AVG_ColtrolFade();
	
	AVG_RenewSetp();

	Debug.num[0] = BackStruct.tone_no;
	Debug.num[1] = BackStruct.tone_char;
	Debug.num[2] = BackStruct.tone_back;

	return ret;
}
