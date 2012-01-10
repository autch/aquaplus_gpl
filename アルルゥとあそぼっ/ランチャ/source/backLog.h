#ifndef _MY_BACKLOG

#include "msgWnd.h"

#define	LOG_SIZE	((26*2*3+1)*100)
#define BACKLOGLINE	17

class BackLog
{
	ColorBuf	backWndCBuf;
	ColorBuf	msgPlate;
	char		logWork[LOG_SIZE];
	WORD		logOffset[100];
	char		logVoice[100][10];
	BYTE		logLine[100];

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
	RECT		voiceRect;
public:
	BackLog(){
		Init();
	}
	~BackLog(){
		Release();
	}
	void Release(){
		backWndCBuf.Release();
		msgPlate.Release();
	}
	void Init(){
		ZeroMemory(this,sizeof(BackLog));
	}
	void Exit(void);
	void Exec(void);
	void SetBackLog(char *msg,int line);
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
		return &logWork[offset];
	}
};

extern BackLog		backLog;

#define _MY_BACKLOG
#endif  _MY_BACKLOG