#include "AVGYMZ.h"
#include "graphic.h"
#include "msgwnd.h"
#include "soundDS.h"
#include "select.h"

void selectExec();
void selectEnd();

SelectWnd	selectWnd;

void SelectWnd::addSelect(char *msg)
{
	cl_free(slctMsg[slctCnt]);
	slctMsg[slctCnt] = (BYTE *)cl_malloc(lstrlen(msg)+1);
	strcpy((char *)slctMsg[slctCnt], msg);
	if(strlen((char *)slctMsg[slctCnt])>44){
		slctMsg[slctCnt][44] = '\0';
	}
	if(width < lstrlen((char *)slctMsg[slctCnt]))width = lstrlen((char *)slctMsg[slctCnt]);
	slctCnt ++;
} // SelectWnd::addSelect

void SelectWnd::SelectStart(BOOL cancel)
{
	int		i,j,offsetX,offsetY;

	bSelect = on;
	bCancel = cancel;
	selectNum = -1;
	width = 640;
	height = 48*slctCnt;
	offsetX = (WIN_SIZEX -width -(slctCnt-1)*10) /2;
	offsetX += (slctCnt-1)*10;
	offsetY = (WIN_SIZEY -62*slctCnt)/2;
	saveInf.textColor[0] = saveInf.textColor[1] = saveInf.textColor[2] = 0xff;
	selectBuf.loadLGF(pack_gparts,"SELECTWND");
	selectMsg.createColorBuf(width,height,32);
	for(j=0;j<slctCnt;j++){
		int offset = (616 -lstrlen((char *)slctMsg[j])*(mainFontSize/2)) /2;
		for(i=0;i<(int)lstrlen((char *)slctMsg[j]);i+=2){
			BOOL bHankaku = OutputOutlineFont(&selectMsg,(BYTE *)&slctMsg[j][i] ,i*(mainFontSize/2)+offset,48*j +13,mainFontSize);
			if(bHankaku) i--;
		}
		selectRect[j].left = offsetX;
		selectRect[j].right = offsetX +width;
		selectRect[j].top = offsetY +62*j;
		selectRect[j].bottom = selectRect[j].top +48;
		offsetX -= 10;
	}
	point.x = selectRect[0].left + 30;
	point.y = selectRect[0].top - 20;
	SetWindowCursorPos(sysInf.hWnd, point.x, point.y);
	if(sysInf.bReadSkip){
		sysInf.bReadSkip = off;
		sysInf.bAlreadyRead = off;
		CheckMenuItem( sysInf.hMenu, ID_READSKIP,   MF_UNCHECKED );
		msgWnd.msgBtn[4].SetState(btn_normal);
	}
	MessageMenuSet( on );
	back_mode = sysInf.execMode;
	changeExecMode(select_exec_mode);
	state = 0;
	startTime = timeGetTime();
	fadeStep = 0;
} // SelectWnd::SelectStart

int SelectWnd::selectExec()
{
	WORD			oldSelect;
	int				i;
	BOOL			bSetCursor = FALSE;

	switch(state){
	  case 0:
		fadeStep = (timeGetTime() - startTime) /300.0f;
		if(fadeStep > 1.0f){
			fadeStep = 1.0f;
			state = 1;
		}
		return -1;
	  case 2:
		fadeStep = (startTime -(int)timeGetTime()) /300.0f;
		if(fadeStep < 0){
			fadeStep = 0;
			state = 0;
			selectEnd();
			return selectNum;
		}
		return -1;
	}
	if(0==sysInf.bIsActive) return -1;
	oldSelect = selectNum;
	selectNum = -1;
	for(i=0;i<slctCnt;i++){
		if(myPtInRect(&selectRect[i],&sysInf.msPoint)){
			selectNum = i;
			break;
		}
	}
	if(keyState.push_action){
		if(selectNum >= 0){
			PlayEffectNum(evtHandle[11],SE_PUSH);
			startTime = timeGetTime() +300;
			state = 2;
			return -1;
		}
	}else if(bCancel && keyState.push_cancel){
		selectEnd();
		return -2;
	}else if(keyState.push_up || keyState.push_down){
		if(keyState.push_up){
			if(selectNum == (-1))selectNum = 0;
			else if(selectNum == 0)selectNum = slctCnt -1;
			else selectNum --;
		}else{
			if(selectNum == (-1))selectNum = 0;
			else{
				selectNum ++;
				if(selectNum >= slctCnt)selectNum = 0;
			}
		}
		bSetCursor = TRUE;
	}else if(keyState.push_left || keyState.push_right){
		if(keyState.push_left){
			selectNum = 0;
		}else{
			selectNum = slctCnt -1;
		}
		bSetCursor = TRUE;
	}
	if(bSetCursor){
		if(selectRect[selectNum].left<=sysInf.msPoint.x && selectRect[selectNum].right>sysInf.msPoint.x){
			point.x = sysInf.msPoint.x;
		}else{
			point.x = selectRect[selectNum].left + 30;
		}
		point.y = selectRect[selectNum].top + 20;
		SetWindowCursorPos(sysInf.hWnd, point.x, point.y);
	}
	if(selectNum>=0 && selectNum!=oldSelect){
		PlayEffectNum(evtHandle[11],SE_SELECT);
	}
	return -1;
} // SelectWnd::selectExec

void ESC_SelectEnd(void);
void SelectWnd::selectEnd()
{
	Release();
	ESC_SelectEnd();
	if(bSelect){
		bSelect = off;
		changeExecMode(back_mode);
	}
} // SelectWnd::selectEnd

void SelectWnd::selectBackup(char **selectMsgBuf)
{
	int			i;

	saveInf.selectNum = 0;
	if(!bSelect)return;
	saveInf.selectNum = slctCnt;
	*selectMsgBuf = (char *)cl_malloc(slctCnt*60);
	for(i=0;i<slctCnt;i++){
		strcpy(*selectMsgBuf +60*i,(char *)slctMsg[i]);
	}
} // SelectWnd::selectBackup

void SelectWnd::selectBlt()
{
	int		i;
	int flashTime = timeGetTime() % 1000;
	float flash = fabsf((flashTime -500) /1000.0f) +0.5f;
	POINT	offset;

	if(off==bSelect)return;	

	selectMsg.alpha = selectBuf.alpha = fadeStep;
	for(i=0;i<slctCnt;i++){
		if(i&1){
			offset.x = LONG(selectRect[i].left -60*(1.0f-fadeStep));
		}else{
			offset.x = LONG(selectRect[i].left +60*(1.0f-fadeStep));
		}
		offset.y = selectRect[i].top;
		rect.left = 0; rect.right = 650; rect.top = 0; rect.bottom = 58;
		g_DibInf.colorBuf.BltFast(offset.x-4, offset.y-2 ,&selectBuf,&rect,TRUE);

		if(i==selectNum){
			rect.left = 0; rect.right = 650; rect.top = 58; rect.bottom = 58+58;
			selectBuf.alpha = fadeStep*flash;
			g_DibInf.colorBuf.BltFast(offset.x-4, offset.y-2,&selectBuf,&rect,TRUE,FALSE);
			rect.left = 0; rect.right = 650; rect.top = 58+58; rect.bottom = 58+58+58;
			selectBuf.alpha = fadeStep;
			g_DibInf.colorBuf.BltFast(offset.x-4, offset.y-2, &selectBuf,&rect,TRUE);
		}
		rect.left = 0;	rect.right = width;
		rect.top = i*48; rect.bottom = rect.top +48;
		g_DibInf.colorBuf.BltFast(offset.x,offset.y,&selectMsg,&rect,TRUE);
	}
} // SelectWnd::selectBlt
