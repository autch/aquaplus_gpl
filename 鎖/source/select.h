#ifndef	_MY_SELECT


class SelectWnd
{
	BYTE		state;
	BYTE		slctCnt;
	WORD		width;
	WORD		height;
	int			startTime;
	float		fadeStep;
	int			selectNum;
	BYTE		bSelect;	
	BYTE		bCancel;	
	RECT		selectRect[20];
	BYTE		*slctMsg[20];
	exec_mode	back_mode;

	ColorBuf	selectBuf;	
	ColorBuf	selectMsg;

public:
	SelectWnd(){
		ZeroMemory(this,sizeof(SelectWnd));
	}
	~SelectWnd(){
		Release();
    }
	void Release(){
		int			i;
		selectBuf.Release();
		selectMsg.Release();
		for(i=0;i<slctCnt;i++){
			cl_free(slctMsg[i]);
		}
		slctCnt = 0;
	}
	void addSelect(char *msg);
	void SelectStart(BOOL cancel=FALSE);
	int	selectExec(void);
	void selectEnd(void);
	void selectBlt(void);
	void selectBackup(char **selectMsgBuf);
};

extern SelectWnd	selectWnd;

#define	_MY_SELECT
#endif	_MY_SELECT