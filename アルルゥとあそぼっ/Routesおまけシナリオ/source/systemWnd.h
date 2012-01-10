#ifndef _MYSYSTEMWND
#define _MYSYSTEMWND

#include "graphic.h"

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
	ColorBuf		baseCBuf;		
	ColorBuf		btnCBuf;		
	MyButton		btn[6];
	BYTE			state;			
	short			selectNum;		
public:
	TitleWnd();
	~TitleWnd(){
		baseCBuf.Release();
		btnCBuf.Release();
	}
	void Exec(void);
	void Draw(void);
};

class CGmodeWnd
{
	ColorBuf	baseWnd;		
	ColorBuf	panel[2];		
	ColorBuf		backWnd;	
	ColorBuf	button;
	MyButton	btn;

	int			selectNum;
	int			time;
	POINT		scrollOffset;

public:
	int			state;		
	CGmodeWnd();
	~CGmodeWnd(){
		baseWnd.Release();
		panel[0].Release();
		panel[1].Release();
		button.Release();
		backWnd.Release();
	}
	BOOL Exec();
	void Draw();
};

class StaffRoll
{
	ColorBuf		baseCBuf[2];
	BYTE			type;
	BYTE			primary;
	BYTE			step;
	BYTE			bShift;
	DWORD			startTime;
	DWORD			nextTime;
	DWORD			fadeTime;
public:
	StaffRoll(int type);
	~StaffRoll(){
		baseCBuf[0].Release();
		baseCBuf[1].Release();
	}
	void Exec();
	void Draw();
};


extern SideWnd		sideWnd;
extern TitleWnd		*titleWnd;
extern CGmodeWnd	*lpCGmodeWnd;
extern StaffRoll	*lpStaffRoll;

#endif _MYSYSTEMWND
