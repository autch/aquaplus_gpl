#ifndef	_MAIN_H_
#define _MAIN_H_

#define WIN_SIZE_W		800
#define WIN_SIZE_H		600

#define BAKBUF_SIZE_W	800
#define BAKBUF_SIZE_H	600


#define FULLMODE_STYLE		(WS_POPUP | WS_CLIPCHILDREN | WS_SYSMENU | WS_VISIBLE)
#define WINDOWMODE_STYLE	(FULLMODE_STYLE | WS_CAPTION | WS_BORDER | WS_MINIMIZEBOX)



typedef struct{
	char		*AppName;
	char		*ClassName;
	char		flash;
	HINSTANCE	Inst;
	HWND		Hwnd;
	int			Active;

	HWND		MenuHwnd;
	HMENU		hMenu;
	BOOL		ShowMenu;
	int			menu;


	int			draw_flag;

	int			mov_mode;
	int			frame_free;
	int			debug_mouse;
}APPLICATION_DATA;

extern APPLICATION_DATA	appData;

extern int EndApp(int last);

#endif

