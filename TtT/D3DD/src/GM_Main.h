#ifndef	_GM_MAIN_H_
#define _GM_MAIN_H_

enum{
	SYS_INIT,
	SYS_TERM,
	SYS_TITLE,
	SYS_BATTLE,
	SYS_WORLD,
	SYS_CASTLE,
	SYS_SAVE_POINT,
	SYS_SAVE_POINT2,
	SYS_END_SAVE,

	SYS_ITEM1,
	SYS_ITEM2,

	SYS_ENDING,
};



extern int	NowTime;
extern int	GlobalCount;
extern int	DrawCount;
extern int	MainFrame;
extern int	DrawFrame;

typedef struct{
	int		click;		
	int		cansel;		
	int		space;		

	int		mes_cut;	
	int		mes_cut_mode;	
	int		diswin;		
	
	int		u,d,l,r;	

	int		end;	
	int		home;	
	int		pup;	
	int		pdown;	

	int		num[10];	
	int		mx,my;		
}GAME_KEY;
extern GAME_KEY	GameKey;


extern int		GoTitle;
extern char		GoScript[256];
extern DWORD	GameStartTime;
extern char	NextScript[256];

extern void MAIN_Loop( int frame );


extern void ResetGameWindow_All( void );

extern void SetGameWindowBackType(int wno, int btl=0);
extern void SetGameWindowText( int wno, char *str, int bno=0, float x=0, float y=0, int w=0  );
extern void SetGameWindowRect( int wno, float wx, float wy, float ww, float wh, int src_move=1  );
extern void SetGameWindowMove( int wno, float wx, float wy, int src_move=1 );
extern void SetGameWindowCenter( int wno, float cx, float cy );
extern void SetGameWindowZoom( int wno, float zoom );
extern void SetGameWindowZoomXY( int wno, float zoom_x, float zoom_y );
extern void SetGameWindowAlpha( int wno, int a );
extern void SetGameWindowColor( int wno, int a, int r, int g, int b );
extern void SetGameWindowDisp( int wno, int disp );
extern void ResetGameWindow( int wno );
extern void SetGameWindow( int wno, int gno, int bno, int layer, int disp, int wtype=-1, int btl=0 );

#define TAIKEN_FLAG	0

#endif

