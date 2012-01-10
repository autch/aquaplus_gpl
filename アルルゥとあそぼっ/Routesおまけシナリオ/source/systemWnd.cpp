#include "Clcommon.h"
#include "backLog.h"
#include "saveWnd.h"
#include "soundDS.h"

#include "systemWnd.h"

SideWnd		sideWnd;
TitleWnd	*titleWnd = NULL;
CGmodeWnd	*lpCGmodeWnd = NULL;
StaffRoll	*lpStaffRoll = NULL;

BOOL CALLBACK ConfDialogBoxProc( HWND hWnd, UINT wmes, UINT wparam, LONG lparam );

void SideWnd::Create()
{
	int		i;
	RECT	ptRect[4];

	type = 0;
	alpha = sysInf.transparent / 100.0f;
	baseCBuf.loadLGF(pack_gparts,"sideWindowB");
	btnCBuf.loadLGF( pack_gparts,"sideWindowP");
	rect.left = 800 -30 +10;	rect.right = rect.left +15;
	rect.top = 315;				rect.bottom = rect.top +36;
	for(i=0;i<4;i++){
		ptRect[i].left = 15*(i+1);
		if(i==3)ptRect[i].left = 0;
		ptRect[i].right = ptRect[i].left +15;
		ptRect[i].top = 0;	ptRect[i].bottom = 36;
	}
	btn[0].Create(2,&btnCBuf,&rect,ptRect,&g_DibInf.colorBuf);
	rect.left = 800 -30 +10;	rect.right = rect.left +15;
	rect.top = 356;				rect.bottom = rect.top +36;
	for(i=0;i<4;i++){
		ptRect[i].left = 15*(i+1);
		if(i==3)ptRect[i].left = 0;
		ptRect[i].right = ptRect[i].left +15;
		ptRect[i].top = 36;	ptRect[i].bottom = ptRect[i].top +36;
	}
	btn[1].Create(2,&btnCBuf,&rect,ptRect,&g_DibInf.colorBuf);
	btn[1].SetState(btn_disable);
	rect.left = 800 -30 +7;	rect.right = rect.left +21;
	rect.top = 399;			rect.bottom = rect.top +20;
	for(i=0;i<4;i++){
		ptRect[i].left = 21*(i+1);
		if(i==3)ptRect[i].left = 0;
		ptRect[i].right = ptRect[i].left +21;
		ptRect[i].top = 72;	ptRect[i].bottom = ptRect[i].top +20;
	}
	btn[2].Create(2,&btnCBuf,&rect,ptRect,&g_DibInf.colorBuf);
	rect.left = 800 -30 +7;	rect.right = rect.left +21;
	rect.top = 422;			rect.bottom = rect.top +20;
	for(i=0;i<4;i++){
		ptRect[i].left = 21*(i+1);
		if(i==3)ptRect[i].left = 0;
		ptRect[i].right = ptRect[i].left +21;
		ptRect[i].top = 92;	ptRect[i].bottom = ptRect[i].top +20;
	}
	btn[3].Create(2,&btnCBuf,&rect,ptRect,&g_DibInf.colorBuf);
	rect.left = 800 -30 +7;	rect.right = rect.left +21;
	rect.top = 445;				rect.bottom = rect.top +20;
	for(i=0;i<4;i++){
		ptRect[i].left = 21*(i+1);
		if(i==3)ptRect[i].left = 0;
		ptRect[i].right = ptRect[i].left +21;
		ptRect[i].top = 112;	ptRect[i].bottom = ptRect[i].top +20;
	}
	btn[4].Create(2,&btnCBuf,&rect,ptRect,&g_DibInf.colorBuf);
	rect.left = 800 -30 +7;	rect.right = rect.left +21;
	rect.top = 468;			rect.bottom = rect.top +20;
	for(i=0;i<4;i++){
		ptRect[i].left = 21*(i+1);
		if(i==3)ptRect[i].left = 0;
		ptRect[i].right = ptRect[i].left +21;
		ptRect[i].top = 132;	ptRect[i].bottom = ptRect[i].top +20;
	}
	btn[5].Create(2,&btnCBuf,&rect,ptRect,&g_DibInf.colorBuf);
} // SideWnd::Create

BOOL SideWnd::Check(POINT *msPoint)
{
	RECT	barRect = {770,0,800,600};
	RECT	scrollBarRect = {770+9,9,770+25,310};
	RECT	brightRect = {770+9,491,770+27,589};
	int			i;
	float ptr;

	switch(sysInf.execMode){
	  case event_exec_mode: case select_exec_mode: case backlog_mode:
		break;
	  default:
		return FALSE;
	}
	if(PtInRect(&scrollBarRect,sysInf.msPoint)){
		if(keyState.push_action || keyState.data_action){
			switch(sysInf.execMode){
				case event_exec_mode: case select_exec_mode:
				backLog.DispInit();
				break;
			}
			if(sysInf.msPoint.y <= 24)ptr = 0;
			else if(sysInf.msPoint.y >= 295)ptr = 1.0f;
			else{
				ptr = float(sysInf.msPoint.y -24)/271;
			}
			backLog.SetScrollPtr(ptr);
			keyState.push_action = 0;
			return TRUE;
		}
	}
	if(PtInRect(&brightRect,sysInf.msPoint)){
		if(keyState.push_action || keyState.data_action){
			if(sysInf.msPoint.y <= 494)ptr = 0;
			else if(sysInf.msPoint.y >= 587)ptr = 1.0f;
			else{
				ptr = float(sysInf.msPoint.y -494)/93;
			}
			sysInf.transparent = SHORT(100.0f -ptr*100);
		}
	}
	if(msPoint->x >= barRect.left){
		switch(type){
		  case 0:
			alpha += ((100-sysInf.transparent)/1000.0f);
			if(alpha > 1.0f) alpha = 1.0f;
			break;
		}
		for(i=0;i<6;i++){
			if(FALSE==btn[i].CheckState(msPoint) && !(i==0&&keyState.push_up) && !(i==1&&keyState.push_down)) continue;
			switch(i){
			  case 0:	
				switch(sysInf.execMode){
				  case event_exec_mode: case select_exec_mode:
					backLog.DispInit();
					break;
				  case backlog_mode:
					keyState.push_up = 1;
					return FALSE;
				}
				break;
			  case 1:	
				switch(sysInf.execMode){
				  case backlog_mode:
					keyState.push_down = 1;
					return FALSE;
				}
				break;
			  case 2:	
				if(NULL==saveWnd){
					PlayEffectNum(evtHandle[1],SelectSnd);
					saveWnd = new SaveWnd(save_mode);
				}
				break;
			  case 3:	
				if(NULL==saveWnd){
					PlayEffectNum(evtHandle[1],SelectSnd);
					saveWnd = new SaveWnd(load_mode);
				}
				break;
			  case 4:	
				sysInf.bShowMsgwin = !sysInf.bShowMsgwin;
				break;
			  case 5:	
				DialogBox( sysInf.hInstance, MAKEINTRESOURCE(IDD_CONFIGDIALOG), sysInf.hWnd, ConfDialogBoxProc );
				break;
			}
			break;
		}
		keyState.push_action = 0;
		return TRUE;
	}else{
		switch(type){
		  case 0:
			alpha -= ((100-sysInf.transparent)/1000.0f);
			if(alpha < (sysInf.transparent/100.0f)) alpha = (sysInf.transparent/100.0f);
			break;
		}
		for(i=2;i<6;i++){
			if(btn[i].GetState()!=btn_disable) btn[i].SetState(btn_normal);
		}
	}
	return FALSE;
} // SideWnd::Check


void SideWnd::Draw()
{
	int		i;

	switch(sysInf.execMode){
	  case event_exec_mode: case select_exec_mode: case backlog_mode:
		break;
	  default:
		return;
	}
	baseCBuf.alpha = alpha;
	btnCBuf.alpha = alpha;
	g_DibInf.colorBuf.BltFast(770,0,&baseCBuf,NULL,TRUE);
	for(i=0;i<6;i++){
		btn[i].Draw();
	}
	rect.left = 60; rect.top = 0;
	rect.right = rect.left +13;
	rect.bottom = rect.top +30;
	int scBar = int(270 *backLog.GetScrollPtr()) +10;
	g_DibInf.colorBuf.BltFast(770+11,scBar,&btnCBuf,&rect,TRUE);
	rect.left = 60; rect.top = 60;
	rect.right = rect.left +19;
	rect.bottom = rect.top +6;
	scBar = 584 - int(93*sysInf.transparent/100);
	g_DibInf.colorBuf.BltFast(770+9,scBar,&btnCBuf,&rect,TRUE);
}

TitleWnd::TitleWnd()
{
	int		i;
	RECT	ptRect[4];

	ZeroMemory(this,sizeof(TitleWnd));
	ZeroMemory(&backSelectInf,sizeof(BackSelectInf));
	EnableMenuItem( sysInf.hMenu, ID_BACKSELECT, MF_GRAYED );
	ESC_InitEOprFlag();
	lpSoundDS->ChangeMusic(bgmHandle,pack_bgmfile,2,TRUE,0);
	baseCBuf.loadLGF(pack_gparts,"TitleBase");
	btnCBuf.loadLGF( pack_gparts,"TitleButton");
	rect.left = 484;	rect.right = rect.left +272;
	rect.top = 216;		rect.bottom = rect.top +40;
	for(i=0;i<4;i++){
		ptRect[i].left = 272*i;
		ptRect[i].right = ptRect[i].left +272;
		ptRect[i].top = 0;	ptRect[i].bottom = 40;
	}
	btn[0].Create(1,&btnCBuf,&rect,ptRect,&baseCBuf);
	btn[0].Draw();
	btn[0].Create(1,&btnCBuf,&rect,ptRect,&g_DibInf.colorBuf);
	SetWindowCursorPos(sysInf.hWnd,rect.left+10, rect.top+10);
	rect.left = 484;	rect.right = rect.left +272;
	rect.top = 256;		rect.bottom = rect.top +40;
	for(i=0;i<4;i++){
		ptRect[i].left = 272*i;
		ptRect[i].right = ptRect[i].left +272;
		ptRect[i].top = 40;	ptRect[i].bottom = ptRect[i].top +40;
	}
	btn[1].Create(1,&btnCBuf,&rect,ptRect,&baseCBuf);
	btn[1].Draw();
	btn[1].Create(1,&btnCBuf,&rect,ptRect,&g_DibInf.colorBuf);
	rect.left = 484;	rect.right = rect.left +272;
	rect.top = 296;		rect.bottom = rect.top +40;
	for(i=0;i<4;i++){
		ptRect[i].left = 272*i;
		ptRect[i].right = ptRect[i].left +272;
		ptRect[i].top = 80;	ptRect[i].bottom = ptRect[i].top +40;
	}
	btn[2].Create(1,&btnCBuf,&rect,ptRect,&baseCBuf);
	btn[2].Draw();
	btn[2].Create(1,&btnCBuf,&rect,ptRect,&g_DibInf.colorBuf);
	rect.left = 484;	rect.right = rect.left +272;
	rect.top = 336;		rect.bottom = rect.top +40;
	for(i=0;i<4;i++){
		ptRect[i].left = 272*i;
		ptRect[i].right = ptRect[i].left +272;
		ptRect[i].top = 120;	ptRect[i].bottom = ptRect[i].top +40;
	}
	if(AVG_GetCgFlag(0)){
		btn[3].Create(1,&btnCBuf,&rect,ptRect,&baseCBuf);
		btn[3].Draw();
		btn[3].Create(1,&btnCBuf,&rect,ptRect,&g_DibInf.colorBuf);
	}else{
		btn[3].Create(1,&btnCBuf,&rect,ptRect,&g_DibInf.colorBuf);
		btn[3].SetState(btn_disable);
	}
	rect.left = 484;	rect.right = rect.left +272;
	rect.top = 416;		rect.bottom = rect.top +40;
	for(i=0;i<4;i++){
		ptRect[i].left = 272*i;
		ptRect[i].right = ptRect[i].left +272;
		ptRect[i].top = 200;	ptRect[i].bottom = ptRect[i].top +40;
	}
	btn[4].Create(1,&btnCBuf,&rect,ptRect,&baseCBuf);
	btn[4].Draw();
	btn[4].Create(1,&btnCBuf,&rect,ptRect,&g_DibInf.colorBuf);
	baseCBuf.alpha = 0;
	changeExecMode( opening_mode );	
} // TitleWnd::TitleWnd

void AVG_SetScenarioNo( int scenario_no );
void TitleWnd::Exec()
{
	int		i;
	BOOL	bMoveCur = FALSE;
	short	oldSelect = selectNum;

	switch(state){
	  case 0:	
		baseCBuf.alpha += 0.02f;
		if(baseCBuf.alpha >= 1.0f){
			backColorBuf.clearColorBuf();
			baseCBuf.alpha = 1.0f;
			state = 1;
		}
		return;
	}
	if(keyState.push_up){
		selectNum --;
		while(1){
			if(selectNum<0){
				selectNum = 4;
				break;
			}
			if(btn[selectNum].GetState()!=btn_disable)break;
			selectNum --;
		}
		bMoveCur = TRUE;
	}else if(keyState.push_down){
		selectNum ++;
		while(1){
			if(selectNum>4){
				selectNum = 0;
				break;
			}
			if(btn[selectNum].GetState()!=btn_disable)break;
			selectNum ++;
		}
		bMoveCur = TRUE;
	}else if(keyState.push_left){
		selectNum = 0;
		bMoveCur = TRUE;
	}else if(keyState.push_right){
		selectNum = 4;
		bMoveCur = TRUE;
	}
	if(bMoveCur){
		SetWindowCursorPos(sysInf.hWnd,btn[selectNum].selectRect.left+10, btn[selectNum].selectRect.top+10);
	}else{
		selectNum = -1;
	}
	for(i=0;i<5;i++){
		if(FALSE==btn[i].CheckState(&sysInf.msPoint)){
			if(btn[i].GetState()==btn_onCursor)selectNum = i;
			continue;
		}
		selectNum = i;
		if(selectNum != 4) PlayEffectNum(evtHandle[1],SelectSnd);
		switch(i){
		  case 0:	
			EXEC_ReadLang( "0500.sdt", &LangData );
			AVG_SetScenarioNo(500);
			EXEC_StartLang( &LangData, MAIN_SCRIPT );
			c_cls_all();
			msgWnd.MsgCLS();
			lpSoundDS->FadeOut(bgmHandle);
			saveInf.musicNum = -1;
			changeExecMode( event_exec_mode );	
			break;
		  case 1:	
			EXEC_ReadLang( "0000.sdt", &LangData );
			AVG_SetScenarioNo(0);
			EXEC_StartLang( &LangData, MAIN_SCRIPT );
			c_cls_all();
			msgWnd.MsgCLS();
			lpSoundDS->FadeOut(bgmHandle);
			saveInf.musicNum = -1;
			changeExecMode( event_exec_mode );
			break;
		  case 2:
			EXEC_LangBuf  = (char *)&LangData.LangBuf;
			EXEC_LangInfo = &LangData;
			saveWnd = new SaveWnd(load_mode);
			break;
		  case 3:
			lpCGmodeWnd = new CGmodeWnd;
			break;
		  case 4:	
			sysInf.bGameEnd = on;
			break;
		}
	}
} // TitleWnd::Exec

void TitleWnd::Draw()
{
	int			i;

	if(opening_mode!=sysInf.execMode)return;
	g_DibInf.colorBuf.BltFast(0,0,&baseCBuf,NULL,TRUE);
	if(1!=state)return;
	for(i=0;i<5;i++){
		btn[i].Draw();
	}
} // TitleWnd::Draw

#define kabeWndLEFT		39
#define kabeWndTOP		58

CGmodeWnd::CGmodeWnd()
{
	int		i;
	ZeroMemory(this,sizeof(CGmodeWnd));
	baseWnd.loadLGF(pack_gparts,"CGmodeBase");
	panel[0].loadLGF(pack_gparts,"cgSelect");
	panel[1].loadLGF(pack_gparts,"cgNoSelect");
	button.loadLGF(pack_gparts,"CGmodeBtn");
	backWnd.createColorBuf(WIN_SIZEX,WIN_SIZEY,24,FALSE);
	backWnd.BltFast(0,0,&g_DibInf.colorBuf,NULL,FALSE);

	RECT	ptRect[4];
	rect.left = kabeWndLEFT +611;	rect.right = rect.left +100;
	rect.top = kabeWndTOP +455;	rect.bottom = rect.top +20;
	for(i=0;i<3;i++){
		ptRect[i].left = 0;		ptRect[i].right = ptRect[i].left +100;
		ptRect[i].top = i*20;	ptRect[i].bottom = ptRect[i].top +20;
	}
	btn.Create(1,&button,&rect,ptRect,&g_DibInf.colorBuf);

	c_cls_all();
	msgWnd.MsgCLS();
	msgWnd.ForceHide();	
	changeExecMode( cgview_mode );	
	bgInf.look_max = 900;
	bgInf.look_cnt = timeGetTime() +bgInf.look_max;
	panel[0].alpha = panel[1].alpha = button.alpha = baseWnd.alpha = 0;
	selectNum = -1;
}

BOOL CGmodeWnd::Exec()
{
	int			i;
	BOOL		bPush = FALSE;
	POINT		icPt;
	char		str[MAX_PATH];

	if(cgview_mode!=sysInf.execMode)return FALSE;
	switch(state){
	  case 0:
	  case 2:
	  case 4:
	  case 5:
		time = bgInf.look_cnt-timeGetTime();
		baseWnd.alpha = float(bgInf.look_max-time)/bgInf.look_max;
		if(time<=0){
			baseWnd.alpha = 1.0f;
			bgInf.look_cnt = 0;
			if(state==4)state = 1;
			else state ++;
		}
		if(state==3 && (selectNum==5 || selectNum==6 || selectNum==7)){
			bgInf.look_max = 2100;
			bgInf.look_cnt = timeGetTime() +bgInf.look_max;
		}else if(state==5){
			baseWnd.alpha = 1.0f -baseWnd.alpha;
		}else if(state==6){
			changeExecMode( opening_mode );	
		}
		panel[0].alpha = panel[1].alpha = button.alpha = baseWnd.alpha;
		return TRUE;
	  case 1:
		if(keyState.push_action && selectNum>=0){
			if(AVG_GetCgFlag(selectNum+1)==FALSE)break;
			PlayEffectNum(evtHandle[1],SelectSnd);
			wsprintf(str,"v00%02d0",selectNum);
			baseWnd.loadLGF(pack_eventcg,str);
			state = 2;
			bgInf.look_max = 900;
			bgInf.look_cnt = timeGetTime() +bgInf.look_max;
			panel[0].alpha = panel[1].alpha = button.alpha = baseWnd.alpha = 0;
			switch(selectNum){
			  case 5:
				scrollOffset.x = 680;
				scrollOffset.y = 0;
				break;
			  case 6:
				scrollOffset.x = 0;
				scrollOffset.y = 600;
				break;
			  case 7:
				scrollOffset.x = 0;
				scrollOffset.y = 0;
				break;
			}
			break;
		}else if(keyState.push_left || keyState.push_up){
			bPush = TRUE;
			if(selectNum==0)selectNum = 11;
			else selectNum --;
		}else if(keyState.push_right || keyState.push_down){
			bPush = TRUE;
			selectNum ++;
			if(selectNum > 11)selectNum = 0;
		}
		if(bPush){
			icPt.x = 54  +(selectNum%4)*177 +50;
			icPt.y = 104 +(selectNum/4)*136 +50;
			ClientToScreen(sysInf.hWnd,&icPt);
			SetCursorPos(icPt.x, icPt.y);
		}else{
			selectNum = -1;
		}
		for(i=0;i<12;i++){
			rect.left = 54+ 177*(i%4);	rect.right = rect.left +160;
			rect.top = 104+136*(i/4);	rect.bottom = rect.top +120;
			if(PtInRect(&rect,sysInf.msPoint)){
				selectNum = i;
				break;
			}
		}
		if(btn.CheckState(&sysInf.msPoint) || keyState.push_cancel){
			PlayEffectNum(evtHandle[1],CancelSnd);
			baseWnd.BltFast(0,0,&g_DibInf.colorBuf,NULL,FALSE);
			state = 5;
			bgInf.look_max = 900;
			bgInf.look_cnt = timeGetTime() +bgInf.look_max;
			return TRUE;
		}
		break;
	  case 3:
		switch(selectNum){
		  case 5:
			time = bgInf.look_cnt-timeGetTime();
			if(time<=0){
				scrollOffset.x = 0;
				scrollOffset.y = 680;
			}else{
				scrollOffset.x = 680 -(bgInf.look_max-time)*680/bgInf.look_max;
				scrollOffset.y = (bgInf.look_max-time)*680/bgInf.look_max;
			}
			break;
		  case 6:
			time = bgInf.look_cnt-timeGetTime();
			scrollOffset.x = 0;
			if(time<=0){
				scrollOffset.y = 0;
			}else{
				scrollOffset.y = 600-(bgInf.look_max-time)*600/bgInf.look_max;
			}
			break;
		  case 7:
			time = bgInf.look_cnt-timeGetTime();
			scrollOffset.x = 0;
			if(time<=0){
				scrollOffset.y = 600;
			}else{
				scrollOffset.y = (bgInf.look_max-time)*600/bgInf.look_max;
			}
			break;
		}
		if(keyState.push_action || keyState.push_cancel){
			PlayEffectNum(evtHandle[1],CancelSnd);
			baseWnd.loadLGF(pack_gparts,"CGmodeBase");
			state = 4;
			bgInf.look_max = 900;
			bgInf.look_cnt = timeGetTime() +bgInf.look_max;
			panel[0].alpha = panel[1].alpha = button.alpha = baseWnd.alpha = 0;
		}
		break;
	}
	return TRUE;
} // CGmodeWnd::Exec

void CGmodeWnd::Draw()
{
	int		i;
	BOOL bAlpha = FALSE;

	if(state==5){
		g_DibInf.colorBuf.BltFast(0,0,&backWnd,NULL,FALSE);
	}
	if(state==0 || state==2 || state==4 || state==5)bAlpha = TRUE;
	if((state==2 || state==3) && (selectNum==5 || selectNum==6 || selectNum==7)){
		rect.left = scrollOffset.x;	rect.right = rect.left +WIN_SIZEX;
		rect.top = scrollOffset.y;	rect.bottom = rect.top +WIN_SIZEY;
		g_DibInf.colorBuf.BltFast(0,0,&baseWnd,&rect,bAlpha);
	}else{
		g_DibInf.colorBuf.BltFast(0,0,&baseWnd,NULL,bAlpha);
	}
	if(state==2 || state==3 || state==5)return;
	for(i=0;i<12;i++){
		if(AVG_GetCgFlag(i+1)==FALSE)continue;
		point.x = 54  +(i%4)*177;
		point.y = 104 +(i/4)*136;
		rect.left = 177*(i%4);	rect.right = rect.left +160;
		rect.top = 136*(i/4);	rect.bottom = rect.top +120;
		if(i==selectNum){
			g_DibInf.colorBuf.BltFast(point.x,point.y,&panel[0],&rect,bAlpha);
		}else{
			g_DibInf.colorBuf.BltFast(point.x,point.y,&panel[1],&rect,bAlpha);
		}
	}
	btn.Draw();
} // CGmodeWnd::Draw()


char staffRollName[2][16][6] = {
{"ed00","ed00a","ed01a","ed00a",
"ed02a","ed00a","ed03a","ed00a",
"ed04a","ed00a","ed05a","ed00a",
"ed06a","ed00a","ed07" ,"ed08"},
{"ed00","ed00b","ed01b","ed00b",
"ed02b","ed00b","ed03b","ed00b",
"ed04b","ed00b","ed05b","ed00b",
"ed06b","ed00b","ed07", "ed08"}
};

StaffRoll::StaffRoll(int r_type)
{
	ZeroMemory(this,sizeof(StaffRoll));

	c_cls_all();
	msgWnd.MsgCLS();
	msgWnd.ForceHide();	

	type = r_type;
	baseCBuf[primary].createColorBuf(WIN_SIZEX,WIN_SIZEY,24,FALSE);
	baseCBuf[primary].BltFast(0,0,&g_DibInf.colorBuf,NULL,FALSE);
	primary = !primary;
	baseCBuf[primary].loadLGF(pack_eventcg,staffRollName[type][step++]);
	baseCBuf[primary].alpha = 0;
	startTime = timeGetTime();
	nextTime = (step/2) *14000 +(step&1)*10000 +startTime;
	fadeTime = timeGetTime() +2000;
	bShift = TRUE;
	lpSoundDS->ChangeMusic(bgmHandle,pack_bgmfile,51,TRUE,0);
	changeExecMode(staffRoll_mode);
}

#define SCRIPT_MAX	1000
#define BLOCK_MAX	512
extern BYTE ScenarioFlag[SCRIPT_MAX][BLOCK_MAX/8];

void StaffRoll::Exec()
{
	DWORD		time = timeGetTime();
	if(ScenarioFlag[SCRIPT_MAX-3][type]){
		if(keyState.push_action || keyState.push_cancel || step==100){
			if(step<100){
				primary = !primary;
				baseCBuf[primary].loadLGF(pack_eventcg,"ed08");
				baseCBuf[primary].alpha = 0;
				nextTime = timeGetTime() +3000;
				lpSoundDS->FadeOut(bgmHandle,90);
				step = 100;
			}else if(time < nextTime){
				baseCBuf[primary].alpha = 1.0f -(float(nextTime -time) / 3000);
			}else{
				backColorBuf.BltFast(40, 30,&baseCBuf[primary],NULL,FALSE);
				changeExecMode(event_exec_mode);
			}
			return;
		}
	}
	if(time > nextTime){
		if(step==16){
			DWORD	fSize;
			ScenarioFlag[SCRIPT_MAX-3][type] = 1;
			SetFilePointer(sysInf.configHF, (SCRIPT_MAX-3)*BLOCK_MAX/8, NULL, FILE_BEGIN);
			WriteFile(sysInf.configHF,ScenarioFlag[SCRIPT_MAX-3],2,&fSize,NULL);
			changeExecMode(event_exec_mode);
			return;
		}
		baseCBuf[primary].alpha = 1.0f;
		primary = !primary;
		baseCBuf[primary].loadLGF(pack_eventcg,staffRollName[type][step++]);
		baseCBuf[primary].alpha = 0;
		nextTime = (step/2) *14000 +(step&1)*10000 +startTime;
		if(step==16){
			nextTime += 5000;
			sysInf.bgFlag = FALSE;
			backColorBuf.BltFast(40, 30,&baseCBuf[primary],NULL,FALSE);
			lpSoundDS->FadeOut(bgmHandle,150);
		}
		fadeTime = timeGetTime() +2000;
		bShift = TRUE;
	}else if(bShift){
		if(time >= fadeTime){
			baseCBuf[primary].alpha = 1.0f;
			bShift = FALSE;
		}else{
			baseCBuf[primary].alpha = float(2000-(fadeTime -time)) / 2000;
		}
	}
}

void StaffRoll::Draw()
{
	if(bShift)g_DibInf.colorBuf.BltFast(0,0,&baseCBuf[!primary],NULL,TRUE);
	g_DibInf.colorBuf.BltFast(0,0,&baseCBuf[primary],NULL,TRUE);
}