
#ifndef	_MAIN_H_
#define _MAIN_H_

#include "bmp.h"
#include "resource.h"

#define ID_SUBMENU		41000

#define FADE_MUS		60



#define PAC_BGM		"bgm"
#define PAC_SE		"se"
#define PAC_VOICE	"voice"
#define PAC_GRP		"grp"
#define PAC_MOV		"mov"
#define PAC_FNT		"fnt"
#define PAC_SDT		"sdt"



#define DVD_OPTION		FALSE

#define TITLE_NAME		"ToHeart2 XRATED"

#define	REG_KEY			"SoftWare\\Leaf\\"TITLE_NAME
#define	REG_KEY_CONF	(REG_KEY"\\config")


typedef struct{


	int				full_screen;	
	int				disp_mode;		
	int				draw_mode;		
	int				draw_mode2;		

	int				save_disp;		
	int				draw_flag;		
	int				draw_zoom;		
	int				draw_matrix;

	HWND			hwnd;			
	long			active;			

	char			title[256];		
	int				l, r;			
	int				u, d;			
	int				x, y;			
	int				w, h;			

	int				ww, wh;			
	
	HBITMAP			hbmp;			
	HBITMAP			hbmp_old;		

	BMP_BT			vram_256;		
	BMP_H			vram_high;		
	BMP_F			vram_full;		
	BMP_T			vram_true;		

	BMP_F			vvram_full[4];	
	
	HDC				MemDC;

	int				cutcut;

	int				min_;
}WINDOW_STRUCT;

extern WINDOW_STRUCT	MainWindow;

#define MAIN_HWND	(HWND)MainWindow.hwnd;


typedef struct{
	HINSTANCE	inst;
	char		title[32];
}APP_STRUCT;

extern APP_STRUCT	AppStruct;

extern int		DrawCount;
extern int		GlobalCount;
extern int		GlobalCount2;
extern DWORD	NowTime;

extern int		MainFrames;

extern int		GameStep;

extern int		FrameFormat;

extern char	ScriptFileName[MAX_PATH];


enum{
	MAIN_INIT = 0,
	MAIN_GAME,
	MAIN_TERM,
};

enum{
	GAME_DEMO = 0,
	GAME_TITLE,
	GAME_OPENNING,
	GAME_AVG,
	GAME_ENDING,
	GAME_OMAKE,
};

typedef struct{
	BOOL	flag;

	int		frame;		
	int		pos;		
	int		number;		
	int		string;		
	int		scenario;	
	int		game;		
	int		reg;		

	int		num[16];
	char	*str[4];
}DEBUG_TEXT;

extern DEBUG_TEXT	Debug;
extern int			TentouFlag;



extern void MAIN_SetScriptFlag( int flag );
extern int MAIN_GetScriptFlag( void );


extern int GetScenarioNo( char *sname );
extern void MAIN_StartScenario( void );
extern void MAIN_StartOmakeScenario( int omake_no );

extern void MAIN_TermScript( void );


extern void MAIN_SetEnd( void );
extern int  MAIN_GetEnd( void );

extern void MAIN_Loop( void );


extern void WIN_InitMenu( void );
extern int	WIN_SetScreenMode( int w, int h, int rfrate );

extern void MAIN_Err( char *buf);


extern void MAIN_PlayWav( int sbuf_no, char *filename, BOOL loop );
extern int  MAIN_PlayWavEx( char *filename );
extern void MAIN_StopWav( int sbuf_no );
extern BOOL MAIN_CheckWav( int sbuf_no );
extern void MAIN_SetWavPan( int sbuf_no, int pan );
extern void MAIN_SetWavVolume( int sbuf_no, int vol, int count );
extern int  MAIN_WaitWavVolume( int sbuf_no );
extern void MAIN_RenewWavVolume( void );

extern void WIN_GetRegistryAmount( char *key, long *amount );
extern void WIN_SetRegistryAmount( char *key, long amount );

extern void WIN_OsoErrDialog(void);

extern void EnableWindosChengMenu( int mode );

#endif