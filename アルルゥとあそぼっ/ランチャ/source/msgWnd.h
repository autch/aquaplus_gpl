#ifndef _MY_MSGWIND

#include "graphic.h"

#define		msgHmax			2
#define		msgWmax			(25*2)

#define		systemFontSize	14
#define		mainFontSize	22
#define		FontSize2X		40
#define		FontSize4X		56
#define		msgWndWidth		624
#define		msgWndHeight	69
#define		rubiHeight		12

#define		WaitOneMsg		150
class ClMsgWnd
{
	ColorBuf	msgFrame;
	ColorBuf	msgPlate;
	ColorBuf	msgWait;
	ColorBuf	namePlate;
	POINT		offset;

	WORD		waitStep;
	BYTE		bName;
	BYTE		bIndent;
public:
	WORD		msgWidth;
	WORD		msgHeight;
	WORD		lineHeight;
	WORD		widthOffset;
	WORD		heightOffset;
	WORD		logMsgLine;
	DWORD		autoWaitTime;
	DWORD		autoWaitStart;

	ClMsgWnd(){
		ZeroMemory(this,sizeof(ClMsgWnd));
	}
	~ClMsgWnd(){
		Release();
	}
	void Release(){
		msgFrame.Release();
		msgPlate.Release();
		msgWait.Release();
	}
	BOOL InitMessageParts(void);
	BOOL BltMessage(void);
	void MsgTypeChar(BYTE *msgBuf);
	char *DisplayMessage(BYTE *msgBuf);
	void MsgCLS(void);
	void NameCLS(void);
	void MsgNewline(void);
	BOOL msgStopCheck(void);
	void TextOut(int x,int y,char *msg);
	void ForceHide(void){
		msgFrame.alpha = msgPlate.alpha = 0;
		sysInf.bShowMsgwin = off;
	}
};

void OutputOutlineFont(ColorBuf *destCBuf,BYTE *str,int mx,int my,int size,myRGB *color=NULL);
void OutputOutlineFontWidth(ColorBuf *destCBuf,BYTE *str,int mx,int my,int size,int expansion);
void SetDemoFlag( int flag, int demo_count );

extern ClMsgWnd		msgWnd;


#define _MY_MSGWIND
#endif	_MY_MSGWIND