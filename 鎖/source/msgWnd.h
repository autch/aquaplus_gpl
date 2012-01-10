#ifndef _MY_MSGWIND

#include "graphic.h"
#include "saveWnd.h"
#define		msgHmax			3
#define		msgWmax			(25*2)

#define		systemFontSize	12
#define		mainFontSize	24
#define		FontSize2X		40
#define		FontSize4X		56
#define		msgWndWidth		624
#define		msgWndHeight	109
#define		rubiHeight		12

#define		WaitOneMsg		150
class ClMsgWnd
{
	ColorBuf	msgBase[3];
	ColorBuf	msgFrame;
	ColorBuf	msgPlate;
	ColorBuf	msgWait[2];
	ColorBuf	nameFrame;
	ColorBuf	namePlate;
	POINT		offset;
	BYTE		bFade;
	int			fadeTime;
	float		alpha;

	ColorBuf	savePrt;
	ColorBuf	loadPrt;
	ColorBuf	configPrt;
	ColorBuf	autoskipPrt;
	ColorBuf	skipPrt;
	ColorBuf	backlogPrt;
	ColorBuf	slidePrt;
	BYTE		bSlideUse;

	DWORD		waitStep;
	BYTE		bName;
	BYTE		bIndent;	
public:
	MyButton	msgBtn[6];
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
		for(int i=0;i<3;i++)msgBase[i].Release();
		msgFrame.Release();
		msgPlate.Release();
		msgWait[0].Release();
		msgWait[1].Release();
		nameFrame.Release();
		namePlate.Release();
		savePrt.Release();
		loadPrt.Release();
		configPrt.Release();
		autoskipPrt.Release();
		skipPrt.Release();
		backlogPrt.Release();
		slidePrt.Release();
	}
	BOOL InitMessageParts(void);
	BOOL BltMessage(void);
	void MsgTypeChar(BYTE *msgBuf);
	char *DisplayMessage(BYTE *msgBuf);
	void MsgCLS(void);
	void NameCLS(void);
	void MsgNewline(void);
	BOOL CheckButton(void);
	void toggleAutoRead(void);
	void toggleMessageSkip(void);
	BOOL msgStopCheck(void);
	void TextOut(int x,int y,char *msg);
	void SetAlpha(){
		msgFrame.alpha = msgPlate.alpha = alpha;
		for(int i=0;i<3;i++)msgBase[i].alpha = alpha;
		savePrt.alpha = loadPrt.alpha = configPrt.alpha = autoskipPrt.alpha = skipPrt.alpha = backlogPrt.alpha = slidePrt.alpha = alpha;
	}
	void ForceHide(void){
		if(saveWnd){
			if(sysInf.execMode==event_exec_mode || sysInf.execMode==select_exec_mode)my_delete(saveWnd);
		}
		alpha = 0;
		namePlate.alpha = 0;
		SetAlpha();
		sysInf.bShowMsgwin = off;
		sysInf.bHideMsgwin = off;
	}
	float GetAlpha(void){return alpha;}
};

BOOL OutputOutlineFont(ColorBuf *destCBuf,BYTE *str,int mx,int my,int size,BOOL useKage=TRUE);
void SetDemoFlag( int flag, int demo_count );

extern ClMsgWnd		msgWnd;


#define _MY_MSGWIND
#endif	_MY_MSGWIND