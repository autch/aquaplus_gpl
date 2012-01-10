#include "ClCommon.h"
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
	height = 60*slctCnt;
	offsetX = (WIN_SIZEX -width -(slctCnt-1)*10) /2;
	offsetX += (slctCnt-1)*10;
	offsetY = (WIN_SIZEY -62*slctCnt)/2;
	selectBuf.loadLGF(pack_gparts,"SELECTWND");
	selectBase.loadLGF(pack_gparts,"SELECTWNDBase");
	selectMsg.createColorBuf(width,height,32);
	for(j=0;j<slctCnt;j++){
		selectMsg.BltFast(0,60*j,&selectBuf,NULL,FALSE);
		int offset = (616 -lstrlen((char *)slctMsg[j])*(mainFontSize/2)) /2;
		for(i=0;i<(int)lstrlen((char *)slctMsg[j]);i+=2){
			OutputOutlineFont(&selectMsg,(BYTE *)&slctMsg[j][i] ,i*(mainFontSize/2)+offset,60*j +14,mainFontSize);
			if(slctMsg[j][i]<0x80) i--;	
		}
		selectRect[j].left = offsetX;
		selectRect[j].right = offsetX +width;
		selectRect[j].top = offsetY +62*j;
		selectRect[j].bottom = selectRect[j].top +60;
		offsetX -= 10;
	}
	point.x = selectRect[0].left + 30;
	point.y = selectRect[0].top - 20;
	SetWindowCursorPos(sysInf.hWnd, point.x, point.y);
	if(sysInf.bReadSkip){
		sysInf.bReadSkip = off;
		sysInf.bAlreadyRead = off;
		CheckMenuItem( sysInf.hMenu, ID_READSKIP,   MF_UNCHECKED );
	}
	back_mode = sysInf.execMode;
	changeExecMode(select_exec_mode);
} // SelectWnd::SelectStart

int SelectWnd::selectExec()
{
	WORD			oldSelect;
	int				i;
	BOOL			bSetCursor = FALSE;

	oldSelect = selectNum;
	selectNum = -1;
	for(i=0;i<slctCnt;i++){
		if(PtInRect(&selectRect[i],sysInf.msPoint)){
			selectNum = i;
			break;
		}
	}
	if(keyState.push_action){
		if(selectNum >= 0){
			PlayEffectNum(evtHandle[1],SelectSnd);
			selectEnd();
			return selectNum;
		}
	}else if(bCancel && keyState.push_cancel){
		PlayEffectNum(evtHandle[1],CancelSnd);
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
		point.x = selectRect[selectNum].left + 30;
		point.y = selectRect[selectNum].top + 20;
		SetWindowCursorPos(sysInf.hWnd, point.x, point.y);
	}
	if(oldSelect!=selectNum && selectNum>=0){
		PlayEffectNum(evtHandle[1],CurMoveSnd);
	}
	return -1;
} // SelectWnd::selectExec

void ESC_SelectEnd(void);
void SelectWnd::selectEnd()
{
	bSelect = off;
	Release();
	ESC_SelectEnd();
	changeExecMode(back_mode);
} // SelectWnd::selectEnd

void SelectWnd::selectBackup(char **selectMsg)
{
	int			i;

	saveInf.selectNum = 0;
	if(!bSelect)return;
	saveInf.selectNum = slctCnt;
	*selectMsg = (char *)cl_malloc(slctCnt*60);
	for(i=0;i<slctCnt;i++){
		strcpy(*selectMsg +60*i,(char *)slctMsg[i]);
	}
} // SelectWnd::selectBackup

void SelectWnd::selectBlt()
{
	int		i;
	float fadeAlpha = (cosf(DEGtoRAD(6*sysInf.animeCnt)) +1.0f) /8.0f +0.75f;

	if(off==bSelect)return;
	for(i=0;i<slctCnt;i++){
		rect.left = 0; rect.right = 640;
		if(i==selectNum){
			rect.top = 120; rect.bottom = 180;
			selectBase.alpha = fadeAlpha;
			g_DibInf.colorBuf.BltFast(selectRect[i].left,selectRect[i].top,&selectBase,&rect,TRUE);
		}else{
			rect.top = 0; rect.bottom = 60;
			g_DibInf.colorBuf.BltFast(selectRect[i].left,selectRect[i].top,&selectBase,&rect,FALSE);
		}
		rect.left = 0;	rect.right = width;
		rect.top = i*60; rect.bottom = rect.top +60;
		g_DibInf.colorBuf.BltFast(selectRect[i].left,selectRect[i].top,&selectMsg,&rect,FALSE);
	}
} // SelectWnd::selectBlt
