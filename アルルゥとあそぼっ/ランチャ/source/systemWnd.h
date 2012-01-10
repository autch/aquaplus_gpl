#ifndef _MYSYSTEMWND
#define _MYSYSTEMWND

#include "graphic.h"
#include "DrawPoly4.h"

class SideWnd
{
	BYTE			type;			
	float			alpha;
	ColorBuf		baseCBuf;		
	ColorBuf		btnCBuf;		

public:
	MyButton		btn[6];
	SideWnd(){
		ZeroMemory(this,sizeof(SideWnd));
	}
	~SideWnd(){
		Release();
	}
	void Release(){
		baseCBuf.Release();
		btnCBuf.Release();
	}
	void Create(void);
	BOOL Check(POINT *msPoint);
	void Draw(void);
};

class TitleWnd
{
	ColorBuf		titleCBuf;		
	ColorBuf		titleBtnCBuf;	

	MyButton		btn;
	BYTE			state;		
public:
	TitleWnd();
	~TitleWnd(){
		titleCBuf.Release();
		titleBtnCBuf.Release();
	}
	void Exec(void);
	void Draw(void);
};

class MyMenuWnd
{
	ColorBuf		menuCBuf;			
	ColorBuf		menuIcon[9][2];		
	char			selectNum;
	int				time;

public:
	BYTE			state;
	MyMenuWnd();
	~MyMenuWnd(){
		menuCBuf.Release();
		for(int i=0;i<9;i++){
			menuIcon[i][0].Release();
			menuIcon[i][1].Release();
		}
	}
	void Exec();
	void Draw();
};


#define		kabeWndTOP	532
#define		KABE_NUM	16		
#define		KABE_TYPE	21	
struct CassetteInfo
{
	BYTE	num;
	BYTE	bDrop;
	RECT	rect;
	POINT	point;
	float	speed;
};

class KabeSetWnd
{
	ColorBuf	baseWnd;	
	ColorBuf	kabeIcon;	
	ColorBuf	kabeButton;	
	MyButton	btn[4];
	MyButton	set_btn[2];

	int			selectNum;	
	int			selectBtn;
	int			time;
	int			dropStart;		
	int			dropNum;		
	int			ct_height[4];	
	CassetteInfo	ct_info[KABE_NUM];

public:
	int			state;
	KabeSetWnd();
	~KabeSetWnd(){
		baseWnd.Release();
		kabeIcon.Release();
		kabeButton.Release();
	}
	BOOL Exec();
	void Draw();
};

class StaffmodeWnd
{
	ColorBuf	baseWnd;
	ColorBuf	panel[4];
	ColorBuf	logo;
	ColorBuf	nameList;
	ColorBuf	button;
	MyButton	btn[3];

	BMP_F	dest;
	BMP_BT	src[4];

	int			pageNum;	
	int			selectNum;	
	int			frontNum;	
	int			time;
	char		str[MAX_PATH];

	int			deg;
	int			moveStep;
	BOOL		bTurn;
public:
	int			state;		
	StaffmodeWnd();
	~StaffmodeWnd(){
		baseWnd.Release();
		panel[0].Release();
		panel[1].Release();
		logo.Release();
		nameList.Release();
		button.Release();
	}
	BOOL Exec();
	void Draw();
	void writeName();
	void DrawMaze(ColorBuf *destCBuf);
};

BOOL menuMode();

class StaffRoll
{
	ColorBuf		baseCBuf[2];		
	ColorBuf		nammeCBuf[2];		
	ColorBuf		partsCBuf;			
	ColorBuf		pictCBuf[2];
	BYTE			primary;
	BYTE			primary_p;
	BYTE			primary_n;
	BYTE			step;
	BYTE			picNum;
	BYTE			nameNum;
	BYTE			bDrawName;
	DWORD			startTime;
	DWORD			nextTime;
	DWORD			fadeTime;
	DWORD			staffNameY;
public:
	StaffRoll();
	void Exec();
	void Draw();
};

extern TitleWnd		*titleWnd;
extern MyMenuWnd	*lpMenuWnd;
extern KabeSetWnd	*lpKabeSetWnd;
extern StaffmodeWnd	*lpStaffmodeWnd;
extern StaffRoll	*lpStaffRoll;

#endif _MYSYSTEMWND
