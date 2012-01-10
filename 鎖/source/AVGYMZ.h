#ifndef _AVGYMZ
#include "ClCommon.h"

#define bFpsSet
#define	className	"ADV System YMZ"

#define WIN_SIZEX	800
#define WIN_SIZEY	600

enum pack_file_no{
	pack_bgmfile = 0,
	pack_sound,
	pack_voice,
	pack_gparts,
	pack_eventcg,
	pack_script,
	pack_anime,
	pack_tail,
	pack_texture,
	pack_objectx,
	pack_objtex,
	pack_shader,
};

enum exec_mode{
	event_exec_mode = 0,
	select_exec_mode,
	backlog_mode,
	save_mode,
	load_mode,
	option_mode,
	map_mode,
	movie_mode,
	opening_mode,
	cg_look_mode,
	scene_play_mode,
	music_mode,
	game_end_mode,
	debug_mode,
};

#define G_SKIP	(sysInf.bGEffectSkip&&(keyState.data_skip || sysInf.bReadSkip))

#define STCHAR_MAX		5
#define STBMP_MAX		10

struct CharSave
{
	BYTE		char_no;	
	BYTE		locate;		
	BYTE		layer;		
	BYTE		bWaitTone;	
	BYTE		alpha;		
	BYTE		bright;		
	int			pose;		
};

struct SaveInfo
{
	BYTE		cg_type;		
	int			bak_no;			
	RECT		btRect;			
	POINT		btPoint;		
	SHORT		shakeSize;
	BYTE		bWave;			
	int			waveSize;		
	int			waveCnt;		
	int			waveCntMax;		
	BYTE		bFade;
	BYTE		f_r, f_g, f_b;
	BYTE		old_r, old_g, old_b;
	BYTE		tone_type[2];		
	BYTE		tone_backOnly;		
	BYTE		noiseType;			
	WORD		noiseFrameMin;		
	WORD		noiseFrameMax;		
	DWORD		noiseTime;			
	CharSave	charSave[STCHAR_MAX];
	BYTE		selectNum;		
	BYTE		bBackSelect;	
	BYTE		demo;
	WORD		demo_cnt;
	WORD		demo_max;
	BYTE		textColor[3];
	int			musicNum;
	int			musicVol;
	int			voiceCnt;
	int			voiceScriptNo;
	int			RestartVoice[3];
	int			dummy[10];
	int			loopSeNum[12];	
	BYTE		loopSeVol[12];	
	SaveInfo(){
		Reset();
	}
	void Reset(){
		ZeroMemory(this,sizeof(SaveInfo));
		f_r = f_g = f_b = 128;
		musicNum = -1;
		textColor[0] = textColor[1] = textColor[2] = 0xff;
	}
};

struct BackSelectInf
{
	SaveInfo		saveInf;
	char			selectMsg[60*10];	
	char			msgBuf[256];		
	WORD			msgLine;
};

extern SaveInfo			saveInf;		

class SystemInfo
{
public:
	HWND				hWnd;			
	HWND				hMenuWnd;		
	HWND				hDebugWnd;		
	HINSTANCE			hInstance;
	HCURSOR				hCurLink;		
	HMENU				hMenu;			
	WORD				ScreenW;		
	WORD				ScreenH;		
	SHORT				zDelta;			
	BYTE				full_screen;	
	BYTE				refreshRate;	
	BYTE				bpp;			
	BYTE				g_skip;			
	POINT				msPoint;		
	BYTE				bgmVolume;		
	BYTE				voiceVolume;	
	BYTE				seVolume;		
	BYTE				msgSpeed;		
	BYTE				org_msgSpeed;	
	BYTE				autoMsg;		
	BYTE				movieQuality;	
	BYTE				nouseOverlay;	
	BYTE				setFPS;			
	BYTE				msgWndAlpha;	
	BYTE				render_skip;	
	BYTE				FontSize;
	BYTE				voiceSelect;	
	WORD				voiceFlag;		
	BYTE				noiseCnt;		
	WORD				noiseW[3];
	WORD				noiseOffset[3];
	BYTE				back_textColor[3];
	DWORD				oldTime;		
	DWORD				msgWaitTime;	
	DWORD				msgStopTime;	
	DWORD				animeCnt;		
	exec_mode			execMode;		
	exec_mode			execMode_backUp;
	HANDLE				configHF;		
	DWORD				bGameEnd	:1;	
	DWORD				bIsActive	:1;	
	DWORD				charFlag	:1;	
	DWORD				bgFlag		:1;	
	DWORD				bUseBG		:1;	
	DWORD				drawFlag	:1;	
	DWORD				bInfo		:1;	
	DWORD				bShowMenu	:1;	
	DWORD				bUseBPP24	:1;	
	DWORD				bForceRender:1;	
	DWORD				bMsgStop	:1;	
	DWORD				bMsgAlreadyStop	:1;	
	DWORD				bShowMsgwin	:1;	
	DWORD				bMsgCLS		:1;	
	DWORD				bAutoRead	:1;	
	DWORD				bReadSkip	:1;	
	DWORD				bAllSkip	:1;	
	DWORD				bAlreadyRead:1; 
	DWORD				bMsgAtOnce	:1; 
	DWORD				bGEffectSkip:1;	
	DWORD				bBackStop	:1; 
	DWORD				bMsgWndCls	:1;	
	DWORD				bHideMsgwin :1; 
	DWORD				bDisplayChange:1;
	DWORD				g_highSpeed	:1;	
	DWORD				bLookTitle	:1;	
	DWORD				bMenuEnable	:1;	
	DWORD				bLookOpening:1; 

	SystemInfo(){
		ZeroMemory(this,sizeof(SystemInfo));
		FontSize = 24;
		org_msgSpeed = 0xff;
	}
	void backupTextColor(){
		for(int i=0;i<3;i++)back_textColor[i] = saveInf.textColor[i];
	}
	void restoreTextColor(){
		for(int i=0;i<3;i++)saveInf.textColor[i] = back_textColor[i];
	}
};	

struct KeyState{
	float	moveLeng;
	float	deg;
	DWORD	repeatCntL;
	DWORD	repeatCntR;
	DWORD	repeatCntU;
	DWORD	repeatCntD;
	DWORD	noPad		:1;
	DWORD	data_move	:1;
	DWORD	data_left	:1;
	DWORD	data_right	:1;
	DWORD	data_up		:1;
	DWORD	data_down	:1;
	DWORD	push_left	:1;
	DWORD	push_right	:1;
	DWORD	push_up		:1;
	DWORD	push_down	:1;
	DWORD	data_action	:1;
	DWORD	push_action	:1;
	DWORD	data_sword	:1;
	DWORD	push_sword	:1;
	DWORD	data_cancel	:1;
	DWORD	push_cancel	:1;
	DWORD	data_skip	:1;
	DWORD	push_skip	:1;
	DWORD	data_camp	:1;
	DWORD	push_camp	:1;
	DWORD	data_test	:1;
	DWORD	push_test	:1;
	DWORD	data_start	:1;
	DWORD	push_start	:1;
	DWORD	data_back	:1;
	DWORD	push_back	:1;
	DWORD	push_mouse_a:1;
	DWORD	push_mouse_b:1;
	DWORD	push_mouse_c:1;
	DWORD	data_fairyAttack :1;
	DWORD	push_fairyAttack :1;
	DWORD	data_leftTrigger :1;
	DWORD	push_leftTrigger :1;
	DWORD	data_rightTrigger :1;
	DWORD	push_rightTrigger :1;
	DWORD	data_submap :1;
	DWORD	data_kLeft	:1;
	DWORD	data_kRight	:1;
	DWORD	data_kUp	:1;
	DWORD	data_kDown	:1;
	DWORD	data_kLeftUp	:1;
	DWORD	data_kRightUp	:1;
	DWORD	data_kLeftDown	:1;
	DWORD	data_kRightDown	:1;
	DWORD	data_mouse_a:1;
	DWORD	data_mouse_b:1;
	DWORD	data_mouse_c:1;
	void ClearRepeat(){
		data_right = data_left = data_up = data_down = 0;
	}
};

extern SystemInfo		sysInf;
extern KeyState			keyState;
extern BackSelectInf	backSelectInf;
extern char				debugBuf[256];
extern POINT			point;
extern RECT				rect;
void changeExecMode(exec_mode mode);
int GetScenarioNo( char *str );
void MessageMenuSet(BOOL sw, BOOL bForce=FALSE);
void InitRestartData(void);
int checkPlayVoice(int charNo);

inline void SetWindowCursorPos(HWND hWnd,int x,int y)
{
	if(!sysInf.bIsActive)return;
	POINT	pnt;
	sysInf.msPoint.x = x;
	sysInf.msPoint.y = y;
	pnt.x = x *sysInf.ScreenW /WIN_SIZEX;
	pnt.y = y *sysInf.ScreenH /WIN_SIZEY;

	ClientToScreen(hWnd,&pnt);
	SetCursorPos(pnt.x,pnt.y);
} // SetWindowCursorPos

#define	SE_SELECT		9007	
#define	SE_PUSH			9002
#define	SE_CANCEL		9012

#define	_回想モード中				251
#define _セーブロード無効			252
#define _インフォメーション無効		253
#define _Ｈシーン中					254

#include "..\res\resource.h"

#define _AVGYMZ
#endif  _AVGYMZ
