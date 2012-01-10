#ifndef _LEAFLIB_COMMON

#include <windows.h>
#include <stdio.h>
#include <math.h>
#define DIRECTINPUT_VERSION		0x0800
#include <dinput.h>

#define bFpsSet
#define	className	"Leaf FanDisc4"

#define WIN_SIZEX	800
#define WIN_SIZEY	600
#define BG_SIZEX	880
#define BG_SIZEY	660

enum OnOff{
	off = 0,
	on,
};

enum UpDown{
	down = 0,
	up,
};

#include "ymzutils.h"
enum pack_file_no{
	pack_bgmfile = 0,
	pack_sound,
	pack_gparts,
	pack_eventcg,
	pack_script,
	pack_tail,
	pack_voice,
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
	movie_mode,
	opening_mode,
	menu_mode,
	kabegami_mode,
	staffmode_mode,
	staffRoll_mode,
	game_end_mode,
	debug_mode,
};

#define FULLMODE_STYLE		(WS_POPUP | WS_CLIPCHILDREN | WS_SYSMENU | WS_VISIBLE)
#define WINDOWMODE_STYLE	(FULLMODE_STYLE | WS_CAPTION | WS_BORDER | WS_MINIMIZEBOX)


#define M_PI  3.14159265359f
#define M_PIR 0.0174532925199f
#define		my_delete(a)	if(NULL!=(a)){delete (a); (a)=NULL;}
#define		my_deletes(a)	if(NULL!=(a)){delete[] (a); (a)=NULL;}
#define		RELEASE(x)		if((x) != NULL){(x)->Release(); (x) = NULL;}
#define		DEGtoRAD(x)		float((x)*M_PIR)
#define		RADtoDEG(x)		float((x)/M_PI*180)

inline float square(float x){ return (x*x); };
inline float lineLength(float x1,float y1,float x2,float y2){
	return sqrtf(square(x1-x2)+square(y1-y2));
}

#define		MY_FILE_SCAN	FILE_FLAG_SEQUENTIAL_SCAN

#define cl_malloc(a)	HeapAlloc(GetProcessHeap(),0,a)
#define cl_free(a)		if(NULL!=(a)){HeapFree(GetProcessHeap(),0,a);(a)=NULL;}


#define G_SKIP	(sysInf.bGEffectSkip&&(keyState.data_skip || sysInf.bAlreadyRead))
#define	ClResult			WORD
#define	Cl_OK				0
#define	Cl_CANNOT_CREATFILE	0x0100
#define	Cl_FILE_NOTFOUND	0x0101
#define	Cl_FILE_SEEK_ERROR	0x0102
#define	Cl_PLAY_CONTINUE	0x0200
#define Cl_CD_NOTREADY		0x0201
#define	Cl_CD_SEEK_ERROR	0x0202
#define	Cl_PCM_NOTFOUND		0x0300

#define STCHAR_MAX		5
#define STBMP_MAX		10
#include "readfile.h"


struct CharSave
{
	BYTE		char_no;
	BYTE		locate;
	BYTE		layer;
	BYTE		bWaitTone;
	int			pose;
};

struct SaveInfo
{
	BYTE		cg_type;
	int			bak_no;
	RECT		btRect;
	POINT		btPoint;
	BYTE		bWave;
	int			waveSize;
	int			waveCnt;
	int			waveCntMax;
	BYTE		tone_type[2];
	BYTE		tone_backOnly;
	CharSave	charSave[STCHAR_MAX];
	BYTE		selectNum;
	BYTE		bBackSelect;
	BYTE		demo;
	WORD		demo_cnt;
	WORD		demo_max;
	int			musicNum;
	SaveInfo(){
		ZeroMemory(this,sizeof(SaveInfo));
		musicNum = -1;
	}
};

struct BackSelectInf
{
	SaveInfo		saveInf;
	char			selectMsg[60*10];
	char			msgBuf[256];
	WORD			msgLine;
};

class SystemInfo
{
public:
	HWND				hWnd;
	HWND				hMenuWnd;
	HINSTANCE			hInstance;
	HCURSOR				hCurLink;
	HMENU				hMenu;
	WORD				ScreenW;
	WORD				ScreenH;
	SHORT				zDelta;
	BYTE				full_screen;
	BYTE				refreshRate;
	BYTE				bpp;
	POINT				msPoint;
	BYTE				bgmVolume;
	BYTE				voiceVolume;
	BYTE				seVolume;
	BYTE				msgSpeed;
	BYTE				autoMsg;
	BYTE				render_skip;
	BYTE				FontSize;
	DWORD				oldTime;
	DWORD				msgWaitTime;
	DWORD				animeCnt;
	exec_mode			execMode;
	exec_mode			execMode_backUp;
	HANDLE				configHF;
	DWORD				bGameEnd	:1;	
	DWORD				bIsActive	:1;	
	DWORD				charFlag	:1;	
	DWORD				bgFlag		:1;	
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
	DWORD				bDisplayChange:1;
	DWORD				bOPMessage	:1; 

	SystemInfo(){
		ZeroMemory(this,sizeof(SystemInfo));
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
extern SaveInfo			saveInf;
extern BackSelectInf	backSelectInf;
extern char				debugBuf[256];
extern POINT			point;
extern RECT				rect;
void changeExecMode(exec_mode mode);

#include "..\\res\\resource.h"

#define _LEAFLIB_COMMON
#endif  _LEAFLIB_COMMON
