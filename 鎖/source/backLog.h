#ifndef _MY_BACKLOG

#include "msgWnd.h"

#define	LOG_SIZE	((26*2*3+1+1)*100)
#define	backlogHeight	16

struct VoiceCheck
{
	WORD		logLine;
	WORD		voiceLine;
	WORD		useLine;
	WORD		org_msgWidth;
	WORD		org_msgHeight;
	WORD		org_blockOffset;
	BYTE		bContinue;
	RECT		Rect[3];	
};

class BackLog
{
	BYTE		state;		
	int			fadeTime;
	ColorBuf	backWndCBuf;
	ColorBuf	backupWnd;
	ColorBuf	msgPlate;
	ColorBuf	voiceMark;
	ColorBuf	scrollBar;
	ColorBuf	retBtnPrt;
	MyButton	retBtn;
	char		logWork[LOG_SIZE];	
	WORD		logOffset[100];
	char		logVoice[100][14];
	char		playVoice[14];
	BYTE		logLine[100];
	VoiceCheck	voiceCheck[30];
	WORD		voiceUse;

	WORD		logTop;
	WORD		logBottom;
	WORD		dispTop;
	WORD		dispBottom;
	WORD		blockOffset;
	WORD		tmpBlockOffset;
	WORD		bottomBlockOffset;
	exec_mode	bak_execMode;

	WORD		allLogLine;		
	WORD		dispLogLine;	

	WORD		msgWidth;
	WORD		msgHeight;

	BYTE		bSelectVoice;
	BYTE		bIndent;	
	BYTE		bOnScrBar;	
	BYTE		ScrBarFlash;
	WORD		voiceNum;
public:
	BackLog(){
		Init();
	}
	~BackLog(){
		Release();
	}
	void Release(){
		backWndCBuf.Release();
		backupWnd.Release();
		msgPlate.Release();
		voiceMark.Release();
		scrollBar.Release();
		retBtnPrt.Release();
	}
	void Init(){
		ZeroMemory(this,sizeof(BackLog));
	}
	void Exit(void);
	void Exec(void);
	void SetBackLog(char *msg,int line, char bContinue=FALSE);
	void SetVoiceNum(int charNum,int voice_set=-1);
	void SetVoice(char *voicefile);
	void DispInit(void);
	BOOL TextDraw(void);
	void MsgTypeChar(BYTE *msgBuf);
	char *DisplayMessage(BYTE *msgBuf);
	void DrawBackLog(void);
	void BackLog::SetScrollPtr(float ptr);
	float GetScrollPtr(){
		if(allLogLine==0)return 1.0f;
		return (float(dispLogLine)/allLogLine);
	}
	char *GetBottomLogBuffer(){
		int		offset;
		if(logBottom>0){
			offset = logOffset[logBottom-1];
		}else{
			offset = logOffset[99];
		}
		return &logWork[offset+1];
	}
	void ClearLog()	
	{
		dispLogLine = allLogLine = 0;
		logTop = logBottom = 0;
		ZeroMemory(logWork,LOG_SIZE);
		ZeroMemory(logOffset,100*2);
		ZeroMemory(logVoice,100*14);
		ZeroMemory(logLine,100);
	}
};

extern BackLog		backLog;

#define _MY_BACKLOG
#endif  _MY_BACKLOG