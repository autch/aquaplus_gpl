#include "Clcommon.h"
#include "backLog.h"
#include "escript.h"
#include "keyInput.h"
#include "saveWnd.h"
#include "soundDS.h"

#include "systemWnd.h"

TitleWnd		*titleWnd = NULL;
MyMenuWnd		*lpMenuWnd = NULL;
KabeSetWnd		*lpKabeSetWnd = NULL;
StaffmodeWnd	*lpStaffmodeWnd= NULL;
StaffRoll		*lpStaffRoll = NULL;

BOOL CALLBACK ConfDialogBoxProc( HWND hWnd, UINT wmes, UINT wparam, LONG lparam );
void SideWnd::Create()
{
	int		i;
	RECT	ptRect[4];

	type = 0;
	alpha = 0.5f;
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
	int			i;

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
			float ptr;
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
	if(msPoint->x >= barRect.left){
		switch(type){
		  case 0:
			alpha += 0.1f;
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
				if(NULL==saveWnd)saveWnd = new SaveWnd(save_mode);
				break;
			  case 3:
				if(NULL==saveWnd)saveWnd = new SaveWnd(load_mode);
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
			alpha -= 0.1f;
			if(alpha < 0.5f) alpha = 0.5f;
			break;
		}
		for(i=2;i<6;i++){
			btn[i].SetState(btn_normal);
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
}

TitleWnd::TitleWnd()
{
	int		i;
	RECT	ptRect[4];

	ZeroMemory(this,sizeof(TitleWnd));
	ZeroMemory(&backSelectInf,sizeof(BackSelectInf));
	ESC_InitEOprFlag();
	lpSoundDS->ChangeMusic(bgmHandle,pack_bgmfile,2,TRUE,0);
	titleCBuf.loadLGF(pack_gparts,"title_op");
	titleBtnCBuf.loadLGF( pack_gparts,"TitleButton");
	rect.left = 200;	rect.right = rect.left +400;
	rect.top = 460;		rect.bottom = rect.top +70;
	for(i=0;i<4;i++){
		ptRect[i].left = 0;
		ptRect[i].right = ptRect[i].left +400;
		ptRect[i].top = 70*i;
		ptRect[i].bottom = ptRect[i].top +70;
	}
	btn.Create(1,&titleBtnCBuf,&rect,ptRect,&g_DibInf.colorBuf);
	SetWindowCursorPos(sysInf.hWnd,rect.left+10, rect.top+10);
	titleCBuf.alpha = 0;
	changeExecMode( opening_mode );
} // TitleWnd::TitleWnd

void AVG_SetScenarioNo( int scenario_no );
void TitleWnd::Exec()
{
	int		i;
	BOOL	bMoveCur = FALSE;

	switch(state){
	  case 0:
		titleCBuf.alpha += 0.02f;
		if(titleCBuf.alpha >= 1.0f){
			backColorBuf.clearColorBuf();
			titleCBuf.alpha = 1.0f;
			state = 1;
		}
		return;
	}
	int selectNum = -1;
	if(btn.CheckState(&sysInf.msPoint)){
		selectNum = 0;
	}
	switch(selectNum){
		case 0:
		EXEC_ReadLang( "0000.sdt", &LangData );
		AVG_SetScenarioNo(0);
		EXEC_StartLang( &LangData, MAIN_SCRIPT );
		c_cls_all();
		msgWnd.MsgCLS();
		lpSoundDS->FadeOut(bgmHandle);
		saveInf.musicNum = -1;
		changeExecMode( event_exec_mode );
		break;
		case 5:
		sysInf.bGameEnd = on;
		break;
	}
} // TitleWnd::Exec

void TitleWnd::Draw()
{
	int			i;

	if(opening_mode!=sysInf.execMode)return;
	g_DibInf.colorBuf.BltFast(0,0,&titleCBuf,NULL,TRUE);
	if(1!=state)return;
	btn.Draw();
} // TitleWnd::Draw


MyMenuWnd::MyMenuWnd(){
	int		i;
	char	str[32];
	ZeroMemory(this,sizeof(MyMenuWnd));
	menuCBuf.loadLGF(pack_eventcg,"s20100");
	backColorBuf.BltFast(40, 30, &menuCBuf, NULL, FALSE);
	saveInf.btRect.left = saveInf.btRect.top = 0;
	saveInf.btRect.right = 800;	saveInf.btRect.bottom = 600;
	saveInf.btPoint.x = 40; saveInf.btPoint.y = 30;

	menuCBuf.alpha = 1.0f;
	for(int i=0;i<9;i++){
		wsprintf(str,"icon%02d",i+1);
		menuIcon[i][0].loadLGF(pack_gparts,str);
		wsprintf(str,"name%02d",i+1);
		menuIcon[i][1].loadLGF(pack_gparts,str);
		menuIcon[i][0].alpha = menuIcon[i][1].alpha = 0;
	}
	msgWnd.MsgCLS();
	msgWnd.ForceHide();
	changeExecMode( menu_mode );

	bgInf.look_max = 900;
	bgInf.look_cnt = timeGetTime() +bgInf.look_max;
	sysInf.bOPMessage = 1;
}

RECT menuRect[9] = {
{ 45, 69, 45+135, 69+120},
{234,  5,234+227,  5+155},
{478, 49,478+112, 49+221},
{620, 19,620+135, 19+166},
{ 33,245, 33+230,245+137},
{284,269,284+188,269+197},
{559,278,559+197,278+ 93},
{ 38,440, 38+201,440+113},
{530,416,530+194,416+126} };

POINT menuNameRect[9] = {
{  4,164},{241,139},{418,215},
{594,128},{ 45,365},{271,448},
{558,354},{ 29,522},{513,531} };

void MyMenuWnd::Exec()
{
	int			i;
	BOOL		bPush = FALSE;
	POINT		icPt;
	float		alpha;
	char		oldSelect = selectNum;
	BOOL bStaffRoll = ScenarioFlag[SCRIPT_MAX-1][0];

	if(menu_mode!=sysInf.execMode)return;
	switch(state){
	case 0:
		time = bgInf.look_cnt-timeGetTime();
		alpha = 1.0f -float(time) / bgInf.look_max;
		if(time<=0){
			alpha = 1.0f;
			bgInf.look_cnt = 0;
			state = 1;
		}
		for(i=0;i<9;i++){
			menuIcon[i][0].alpha = alpha;
			menuIcon[i][1].alpha = alpha/2;
		}
		return;
	}
	if(keyState.push_action && state==1 && selectNum>=0){
		PlayEffectNum(evtHandle[1],SelectSnd);
		EXEC_LangInfo->reg[ 0 ] = selectNum;
		state = 2;
		c_cls_all();
		changeExecMode( event_exec_mode );
	}else if(keyState.push_left || keyState.push_up){
		bPush = TRUE;
		if(selectNum<=0)selectNum = 8;
		else selectNum --;
		if(!bStaffRoll && selectNum==3) selectNum = 2;
	}else if(keyState.push_right || keyState.push_down){
		bPush = TRUE;
		selectNum ++;
		if(selectNum > 8)selectNum = 0;
		if(!bStaffRoll && selectNum==3) selectNum = 4;
	}
	if(bPush){
		icPt.x = menuRect[selectNum].left +50;
		icPt.y = menuRect[selectNum].top +50;
		ClientToScreen(sysInf.hWnd,&icPt);
		SetCursorPos(icPt.x, icPt.y);
	}
	selectNum = -1;
	for(i=0;i<9;i++){
		if(PtInRect(&menuRect[i],sysInf.msPoint)){
			if(!bStaffRoll && i==3)break;
			selectNum = i;
			break;
		}
	}
	if(selectNum>=0 && selectNum!=oldSelect){
		PlayEffectNum(evtHandle[1],CurMoveSnd);
	}
	for(i=0;i<9;i++){
		if(selectNum!=i){
			if(menuIcon[i][1].alpha > 0.5f){
				menuIcon[i][1].alpha -= 0.05f;
				if(menuIcon[i][1].alpha < 0.5f) menuIcon[i][1].alpha = 0.5f;
			}
		}else{
			if(menuIcon[i][1].alpha < 1.0f){
				menuIcon[i][1].alpha += 0.05f;
				if(menuIcon[i][1].alpha > 1.0f) menuIcon[i][1].alpha = 1.0f;
			}
		}
	}
}// MyMenuWnd::Exec

void MyMenuWnd::Draw()
{
	int			i;
	BOOL bStaffRoll = ScenarioFlag[SCRIPT_MAX-1][0];

	g_DibInf.colorBuf.BltFast(0,0,&menuCBuf,NULL,TRUE);
	for(i=0;i<9;i++){
		g_DibInf.colorBuf.BltFast(menuRect[i].left,menuRect[i].top,&menuIcon[i][0],NULL,TRUE);
		if(!bStaffRoll && i==3) continue;
		g_DibInf.colorBuf.BltFast(menuNameRect[i].x,menuNameRect[i].y,&menuIcon[i][1],NULL,TRUE);
	}
} // MyMenuWnd::Draw

BOOL menuMode()
{
	if(NULL==lpMenuWnd){
		lpMenuWnd = new MyMenuWnd;
	}
	if(lpMenuWnd->state==2){
		my_delete(lpMenuWnd);
		return TRUE;
	}
	return FALSE;
}

KabeSetWnd::KabeSetWnd()
{
	int		i;
	ZeroMemory(this,sizeof(KabeSetWnd));
	baseWnd.loadLGF(pack_gparts,"kabeBase");
	kabeIcon.loadLGF(pack_gparts,"kabeIcon");
	kabeButton.loadLGF(pack_gparts,"kabeBtn");

	RECT	ptRect[3];
	rect.left = 319;			rect.right = rect.left +72;
	rect.top = kabeWndTOP +25;	rect.bottom = rect.top +20;
	for(i=0;i<3;i++){
		ptRect[i].left = 319;		ptRect[i].right = ptRect[i].left +72;
		ptRect[i].top = 25 +i*68;	ptRect[i].bottom = ptRect[i].top +21;
	}
	btn[0].Create(1,&kabeButton,&rect,ptRect,&g_DibInf.colorBuf);
	rect.left = 415;			rect.right = rect.left +72;
	rect.top = kabeWndTOP +25;	rect.bottom = rect.top +20;
	for(i=0;i<3;i++){
		ptRect[i].left = 415;		ptRect[i].right = ptRect[i].left +72;
		ptRect[i].top = 25+i*68;	ptRect[i].bottom = ptRect[i].top +21;
	}
	btn[1].Create(1,&kabeButton,&rect,ptRect,&g_DibInf.colorBuf);
	rect.left = 567;	rect.right = rect.left +58;
	rect.top = kabeWndTOP +5;	rect.bottom = rect.top +58;
	for(i=0;i<3;i++){
		ptRect[i].left = 567;	ptRect[i].right = ptRect[i].left +58;
		ptRect[i].top = 5+i*68;	ptRect[i].bottom = ptRect[i].top +58;
	}
	btn[2].Create(1,&kabeButton,&rect,ptRect,&g_DibInf.colorBuf);

	rect.left = 667;			rect.right = rect.left +58;
	rect.top = kabeWndTOP +5;	rect.bottom = rect.top +58;
	for(i=0;i<3;i++){
		ptRect[i].left = 667;	ptRect[i].right = ptRect[i].left +58;
		ptRect[i].top = 5+i*68;	ptRect[i].bottom = ptRect[i].top +58;
	}
	btn[3].Create(1,&kabeButton,&rect,ptRect,&g_DibInf.colorBuf);
	rect.left = 567;			rect.right = rect.left +58;
	rect.top = kabeWndTOP +5;	rect.bottom = rect.top +58;
	for(i=0;i<3;i++){
		ptRect[i].left = 567;		ptRect[i].right = ptRect[i].left +58;
		ptRect[i].top = 5+(i+3)*68;	ptRect[i].bottom = ptRect[i].top +58;
	}
	set_btn[0].Create(1,&kabeButton,&rect,ptRect,&g_DibInf.colorBuf);
	rect.left = 667;			rect.right = rect.left +58;
	rect.top = kabeWndTOP +5;	rect.bottom = rect.top +58;
	for(i=0;i<3;i++){
		ptRect[i].left = 667;		ptRect[i].right = ptRect[i].left +58;
		ptRect[i].top = 5+(i+3)*68;	ptRect[i].bottom = ptRect[i].top +58;
	}
	set_btn[1].Create(1,&kabeButton,&rect,ptRect,&g_DibInf.colorBuf);

	for(i=0;i<4;i++)ct_height[i] = 399;
	c_cls_all();
	msgWnd.MsgCLS();
	msgWnd.ForceHide();
	changeExecMode( kabegami_mode );
	bgInf.look_max = 900;
	bgInf.look_cnt = timeGetTime() +bgInf.look_max;
	baseWnd.alpha = 0;
	selectNum = -1;
	selectBtn = -1;
}

BOOL KabeSetWnd::Exec()
{
	int			i,j,oldSelect = selectBtn,btnNo;
	BOOL		bPush = FALSE;
	POINT		icPt;
	char		str[MAX_PATH];

	if(kabegami_mode!=sysInf.execMode)return FALSE;
	switch(state){
	  case 0:
	  case 2:
	  case 5:
	  case 8:
		time = bgInf.look_cnt-timeGetTime();
		baseWnd.alpha = float(bgInf.look_max-time)/bgInf.look_max;
		if(time<=0){
			baseWnd.alpha = 1.0f;
			bgInf.look_cnt = 0;
			if(state==5)state = 1;
			else state ++;
		}
		if(9==state){
			changeExecMode( event_exec_mode );
		}
		return TRUE;
	  case 1:
		if(kabeIcon.alpha < 1.0f){
			kabeIcon.alpha += 0.033334f;
			if(kabeIcon.alpha > 1.0f) kabeIcon.alpha = 1.0f;
			break;
		}else if(keyState.push_action && selectNum>=0){
			PlayEffectNum(evtHandle[1],SelectSnd);
			wsprintf(str,"v01%02d0",ct_info[selectNum].num);
			baseWnd.loadLGF(pack_eventcg,str);
			state = 2;
			bgInf.look_max = 900;
			bgInf.look_cnt = timeGetTime() +bgInf.look_max;
			kabeButton.alpha = baseWnd.alpha = 0;
		}else if(keyState.push_left || keyState.push_up){
			bPush = TRUE;
			if(selectNum<=0)selectNum = dropStart -1;
			else selectNum --;
		}else if(keyState.push_right || keyState.push_down){
			bPush = TRUE;
			selectNum ++;
			if(selectNum >= dropStart)selectNum = 0;
		}
		if(dropStart==0)bPush = FALSE;
		if(bPush){
			icPt.x = (selectNum%4)*200 +100;
			icPt.y = 399-(selectNum/4)*133 +50;
			ClientToScreen(sysInf.hWnd,&icPt);
			SetCursorPos(icPt.x, icPt.y);
		}else{
			selectNum = -1;
		}
		for(i=0;i<dropStart;i++){
			rect.left = 200*(i%4);	rect.right = rect.left +200;
			rect.top = 399 -133*(i/4);	rect.bottom = rect.top +133;
			if(PtInRect(&rect,sysInf.msPoint)){
				selectNum = i;
				break;
			}
		}
		for(btnNo=0;btnNo<4;btnNo++){
			if(btn[btnNo].CheckState(&sysInf.msPoint))break;
		}
		if(keyState.push_fairyAttack || btnNo<=1){
			PlayEffectNum(evtHandle[1],CancelSnd);
			state = 7;
			for(i=0;i<dropStart;i++) ct_info[i].speed = 1.0f;
			break;
		}
		if(keyState.push_cancel || 3==btnNo){
			PlayEffectNum(evtHandle[1],CancelSnd);
			baseWnd.loadLGF(pack_eventcg,"s20100");
			bgInf.colorBuf.loadLGF(pack_eventcg,"s20100",Type_back);
			backColorBuf.BltFast(40, 30, &bgInf.colorBuf, NULL, FALSE);
			saveInf.btRect.left = saveInf.btRect.top = 0;
			saveInf.btRect.right = 800;	saveInf.btRect.bottom = 600;
			saveInf.btPoint.x = 40; saveInf.btPoint.y = 30;

			state = 8;
			bgInf.look_max = 900;
			bgInf.look_cnt = timeGetTime() +bgInf.look_max;
			return TRUE;
		}
		if(keyState.push_camp || 2==btnNo){
			if(dropStart>=KABE_NUM){
				PlayEffectNum(evtHandle[1],9007);
				break;
			}
			PlayEffectNum(evtHandle[1],SelectSnd);
			dropNum = rndSelect(4)+1;
			if((dropStart+dropNum)>KABE_NUM){
				dropNum = KABE_NUM -dropStart;
			}
			for(i=0;i<dropNum;i++){
				ct_info[i+dropStart].bDrop = TRUE;
				ct_info[i+dropStart].point.x = 200*( (i+dropStart)%4 );
				ct_info[i+dropStart].point.y = -133 -rndSelect(100);
				while(1){
					ct_info[i+dropStart].num = rndSelect(KABE_TYPE);
					for(j=0;j<i+dropStart;j++){
						if( ct_info[i+dropStart].num == ct_info[j].num )break;
					}
					if(j == (i+dropStart))break;
				}
				ct_info[i+dropStart].speed = 1.0f;
				ct_info[i+dropStart].rect.left = 0;
				ct_info[i+dropStart].rect.right = 200;
				ct_info[i+dropStart].rect.top = 133*(ct_info[i+dropStart].num);
				ct_info[i+dropStart].rect.bottom = ct_info[i+dropStart].rect.top +133;
			}
			state = 6;
		}
		break;
	  case 3:
		if(kabeButton.alpha > 0){
			kabeButton.alpha += 0.033334f;
			if(kabeButton.alpha >= 1.0f){
				kabeButton.alpha = 1.0f;
				state = 4;
			}
		}else if(keyState.push_action){
			kabeButton.alpha = 0.033334f;
		}
		break;
	  case 4:
		for(btnNo=0;btnNo<2;btnNo++){
			if(set_btn[btnNo].CheckState(&sysInf.msPoint)){
				break;
			}
		}
		if(btnNo==2){
			if(keyState.push_cancel) btnNo = 1;
			if(keyState.push_camp) btnNo = 0;
		}
		if(btnNo < 2){
			if(btnNo==0){
				PlayEffectNum(evtHandle[1],SelectSnd);
				char newfile[MAX_PATH];
				wsprintf(str,"\\LeafFanDisc%02d.bmp",ct_info[selectNum].num);
				GetWindowsDirectory(newfile,MAX_PATH);
				strcat(newfile,str);
				wsprintf(str,".\\ï«éÜ\\LeafFanDisc%02d.bmp",ct_info[selectNum].num);

				CopyFile(str,newfile,FALSE);

				SetWallPaper(newfile);
			}else{
				PlayEffectNum(evtHandle[1],CancelSnd);
			}
			baseWnd.loadLGF(pack_gparts,"kabeBase");
			bgInf.look_max = 900;
			bgInf.look_cnt = timeGetTime() +bgInf.look_max;
			kabeIcon.alpha = baseWnd.alpha = 0;
			state = 5;
		}
		break;
	  case 6:
		j = 0;
		for(i=0;i<dropNum;i++){
			if(!ct_info[i+dropStart].bDrop)continue;
			j ++;
			ct_info[i+dropStart].point.y += int(ct_info[i+dropStart].speed);
			ct_info[i+dropStart].speed += 0.5f;
			if(ct_height[(i+dropStart)%4] <= ct_info[i+dropStart].point.y){
				ct_info[i+dropStart].point.y = ct_height[(i+dropStart)%4];
				ct_info[i+dropStart].bDrop = FALSE;
				ct_height[(i+dropStart)%4] -= 133;
				PlayEffectNum(evtHandle[i],1012);
			}
		}
		if(j==0){
			dropStart += dropNum;
			dropNum = 0;
			state = 1;
		}
		break;
	  case 7:
		j = 0;
		for(i=0;i<dropStart;i++){
			ct_info[i].point.y += int(ct_info[i].speed);
			ct_info[i].speed += 0.5f;
			if(ct_info[i].point.y < 600) j++;
		}
		if(j==0){
			dropStart = dropNum = 0;
			for(i=0;i<4;i++)ct_height[i] = 399;
			state = 1;
		}
		break;
	}
	return TRUE;
} // KabeSetWnd::Exec

void KabeSetWnd::Draw()
{
	int		i;
	BOOL bAlpha = FALSE;

	g_DibInf.colorBuf.BltFast(0,0,&baseWnd,NULL,TRUE);
	switch(state){
	  case 3:
	  case 4:
		rect.left = 0; rect.right = 800;
		rect.top = 204; rect.bottom = rect.top +68;
		g_DibInf.colorBuf.BltFast(0,kabeWndTOP,&kabeButton,&rect,TRUE);
		if(4==state)
			for(i=0;i<2;i++) set_btn[i].Draw();
		break;
	  case 0: case 1: case 6: case 7:
		if(state==1)for(i=0;i<4;i++){
			btn[i].Draw();
		}
		for(i=0;i<dropStart+dropNum;i++){
			if(i!=selectNum){
				g_DibInf.colorBuf.BltFast(ct_info[i].point.x,ct_info[i].point.y,&kabeIcon,&ct_info[i].rect,TRUE);
			}else{
				rect = ct_info[i].rect;
				rect.left = 200;	rect.right = 400;
				g_DibInf.colorBuf.BltFast(ct_info[i].point.x,ct_info[i].point.y,&kabeIcon,&rect,TRUE);
			}
		}
	}
} // KabeSetWnd::Draw()


StaffmodeWnd::StaffmodeWnd()
{
	int		i,j;
	ZeroMemory(this,sizeof(StaffmodeWnd));
	InitMaze();
	baseWnd.loadLGF(pack_gparts,"staffBase");
	logo.loadLGF(pack_gparts,"staffLogo");
	nameList.createColorBuf(800,22,32);

	dest.buf = (RGB24 *)g_DibInf.colorBuf.pBuf;
	dest.sx = 800; dest.sy = 600;
	selectNum = rndSelect(43);
	for(i=0;i<4;i++){
		j = selectNum +i;
		if(i==3) j = selectNum -1;
		if( j <  0) j += 44;
		if( j > 43) j -= 44;
		wsprintf(str,"stt%03d",j);
		panel[i].loadLGF(pack_eventcg,str);
		src[i].buf = (BYTE *)panel[i].pBuf;
		src[i].pal = (RGB32 *)panel[i].palColor;
		src[i].alp = NULL;
		src[i].sx = 400; src[i].sy = 300;
	}
	button.loadLGF(pack_gparts,"staffBtn");
	RECT	ptRect[3];
	rect.left = 20;	rect.right = rect.left +188;
	rect.top = 268;		rect.bottom = rect.top +172;
	for(i=0;i<3;i++){
		ptRect[i].left = 0;		ptRect[i].right = ptRect[i].left +188;
		ptRect[i].top = i*172;	ptRect[i].bottom = ptRect[i].top +172;
	}
	btn[0].Create(1,&button,&rect,ptRect,&g_DibInf.colorBuf);
	rect.left = 592;	rect.right = rect.left +188;
	rect.top = 268;		rect.bottom = rect.top +172;
	for(i=0;i<3;i++){
		ptRect[i].left = 188;	ptRect[i].right = ptRect[i].left +188;
		ptRect[i].top = i*172;	ptRect[i].bottom = ptRect[i].top +172;
	}
	btn[1].Create(1,&button,&rect,ptRect,&g_DibInf.colorBuf);
	rect.left = 400-216/2;	rect.right = rect.left +216;
	rect.top = 340;	rect.bottom = rect.top +156;
	for(i=0;i<3;i++){
		ptRect[i].left = 376;		ptRect[i].right = ptRect[i].left +216;
		ptRect[i].top = i*156;	ptRect[i].bottom = ptRect[i].top +156;
	}
	btn[2].Create(1,&button,&rect,ptRect,&g_DibInf.colorBuf);

	c_cls_all();
	msgWnd.MsgCLS();
	msgWnd.ForceHide();
	changeExecMode( staffmode_mode );
	bgInf.look_max = 900;
	bgInf.look_cnt = timeGetTime() +bgInf.look_max;
	panel[0].alpha = panel[1].alpha = button.alpha = baseWnd.alpha = logo.alpha = 0;


	DrawMaze(&baseWnd);
	writeName();
}

BOOL StaffmodeWnd::Exec()
{
	int			i,loadTex,nextStaff;
	BOOL		bPush = FALSE;
	POINT		icPt;

	if(staffmode_mode!=sysInf.execMode)return FALSE;
	switch(state){
	  case 0:
	  case 6:
		time = bgInf.look_cnt-timeGetTime();
		baseWnd.alpha = float(bgInf.look_max-time)/bgInf.look_max;
		if(time<=0){
			baseWnd.alpha = 1.0f;
			bgInf.look_cnt = 0;
			state ++;
		}
		if(1==state)baseWnd.loadLGF(pack_gparts,"staffBase");
		if(7==state)changeExecMode( event_exec_mode );
		panel[0].alpha = panel[1].alpha = button.alpha = logo.alpha = baseWnd.alpha;
		return TRUE;
	  case 1:
		if(keyState.push_cancel || keyState.push_down){
			PlayEffectNum(evtHandle[1],CancelSnd);
			baseWnd.loadLGF(pack_eventcg,"s20100");
			bgInf.colorBuf.loadLGF(pack_eventcg,"s20100",Type_back);
			backColorBuf.BltFast(40, 30, &bgInf.colorBuf, NULL, FALSE);
			saveInf.btRect.left = saveInf.btRect.top = 0;
			saveInf.btRect.right = 800;	saveInf.btRect.bottom = 600;
			saveInf.btPoint.x = 40; saveInf.btPoint.y = 30;

			state = 6;
			bgInf.look_max = 900;
			bgInf.look_cnt = timeGetTime() +bgInf.look_max;
			panel[0].alpha = panel[1].alpha = button.alpha = logo.alpha = baseWnd.alpha = 0;
		}else if(keyState.push_left){
			bTurn = 1;
		}else if(keyState.push_right){
			bTurn = 2;
		}else if(keyState.push_up){
			bTurn = 3;
		}
		for(i=0;i<3;i++){
			if(btn[i].CheckState(&sysInf.msPoint)){
				bTurn = i+1;
			}
		}
		if(bTurn==1){
			selectNum --;
			if(selectNum < 0) selectNum = 43;
			nextStaff = selectNum -1;
			if(nextStaff < 0)nextStaff += 44;
			frontNum --;
			if(frontNum < 0)frontNum = 3;
			loadTex = frontNum -1;
			if(loadTex < 0) loadTex = 3;
			wsprintf(str,"stt%03d",nextStaff);
			panel[loadTex].loadLGF(pack_eventcg,str);
			src[loadTex].buf = (BYTE *)panel[loadTex].pBuf;
			src[loadTex].pal = (RGB32 *)panel[loadTex].palColor;
			state = 2;
			writeName();
			PlayEffectNum(evtHandle[1],CurMoveSnd);
		}else if(bTurn==2){
			selectNum ++;
			if(selectNum > 43) selectNum = 0;
			nextStaff = selectNum+1;
			if(nextStaff > 43)nextStaff -= 44;
			frontNum ++;
			if(frontNum > 3)frontNum = 0;
			loadTex = frontNum +1;
			if(loadTex > 3) loadTex = 0;
			wsprintf(str,"stt%03d",nextStaff);
			panel[loadTex].loadLGF(pack_eventcg,str);
			src[loadTex].buf = (BYTE *)panel[loadTex].pBuf;
			src[loadTex].pal = (RGB32 *)panel[loadTex].palColor;
			state = 2;
			writeName();
			PlayEffectNum(evtHandle[1],CurMoveSnd);
		}else if(bTurn==3){
			state = 2;
			moveStep = 0;
			PlayEffectNum(evtHandle[1],SelectSnd);
		}
		break;
	  case 2:
		if(3==bTurn){
			moveStep ++;
			button.alpha = logo.alpha = float(19-moveStep)/19;
			if(moveStep==19){
				bTurn = 0;
				wsprintf(str,"st%03d",selectNum);
				baseWnd.loadLGF(pack_eventcg,str);
				clearPadState();
				state = 3;
			}
			break;
		}
		if(1==bTurn){
			deg += 5;
			if(deg >= 360) deg -= 360;
		}else if(2==bTurn){
			deg -= 5;
			if(deg < 0)deg += 360;
		}
		if(deg % 90 == 0){
			bTurn = 0;
			state = 1;
		}
		break;
	  case 3:
		if(keyState.push_action || keyState.push_down || keyState.push_cancel){
			PlayEffectNum(evtHandle[1],CancelSnd);
			baseWnd.loadLGF(pack_gparts,"staffBase");
			bTurn = 3;
			moveStep = 18;
			state = 4;
		}
		break;
	  case 4:
		moveStep --;
		button.alpha = logo.alpha = float(19-moveStep)/19;
		if(moveStep==0){
			bTurn = 0;
			state = 1;
		}
		break;
	}
	return TRUE;
} // StaffmodeWnd::Exec


void StaffmodeWnd::DrawMaze(ColorBuf *destCBuf)
{
	D3DXVECTOR3	inVec[4];
	float		rad,leng,ratio;
	int		i,j,bSetDiff;

	dest.buf = (RGB24 *)destCBuf->pBuf;
	dest.sx = destCBuf->width; dest.sy = destCBuf->height;

	for(j=0;j<4;j++){
		int ddeg = (deg +90*j) % 360;
		if(ddeg>110 && ddeg<250)continue; 
		bSetDiff = 0;
		for(i=0;i<4;i++){
			rad = (float)ddeg-45+90*(i&1);

			leng = 7.07107f;
			inVec[i].x = leng *sinf(DEGtoRAD(rad));
			inVec[i].y = 5.0f -(i/2)*10.0f;
			inVec[i].z = leng *cosf(DEGtoRAD(rad));
		}
		for(i=0;i<4;i++){
			if(inVec[i].z >= 0)continue;
			if(i==1 || i==3){
				inVec[i].x = (inVec[i].x -inVec[i-1].x)/(inVec[i].z -inVec[i-1].z) *(-inVec[i-1].z) + inVec[i-1].x;
				inVec[i].z = 0;
				leng = lineLength(inVec[i-1].x,inVec[i-1].z,inVec[i].x,inVec[i].z);
				bSetDiff = 1;
			}else{
				inVec[i].x = (inVec[i].x -inVec[i+1].x)/(inVec[i].z -inVec[i+1].z) *(-inVec[i+1].z) + inVec[i+1].x;
				inVec[i].z = 0;
				leng = lineLength(inVec[i+1].x,inVec[i+1].z,inVec[i].x,inVec[i].z);
				bSetDiff = 2;
			}
			ratio = (7.07107f*1.142f) /leng ;
		}
		for(i=0;i<4;i++){
			GetMazeVec(&inVec[i],0);
		}
		if(bSetDiff==1)for(i=1;i<4;i+=2){
			leng = lineLength(inVec[i-1].x,inVec[i-1].y,inVec[i].x,inVec[i].y);
			leng = leng *ratio;
			rad = atan2f(inVec[i].x-inVec[i-1].x,-(inVec[i].y-inVec[i-1].y));
			float rtd = RADtoDEG(rad);
			inVec[i].x = inVec[i-1].x +leng *sinf(rad);
			inVec[i].y = inVec[i-1].y -leng *cosf(rad);
		}
		if(bSetDiff==2)for(i=0;i<4;i+=2){
			leng = lineLength(inVec[i+1].x,inVec[i+1].y,inVec[i].x,inVec[i].y);
			leng = leng *ratio;
			rad = atan2f(inVec[i].x-inVec[i+1].x,-(inVec[i].y-inVec[i+1].y));
			float rtd = RADtoDEG(rad);
			inVec[i].x = inVec[i+1].x +leng *sinf(rad);
			inVec[i].y = inVec[i+1].y -leng *cosf(rad);
		}
		int light;
		if(ddeg < 180){
			light = 128 -100*ddeg/180;
		}else{
			light = 128 -100*(360-ddeg)/180;
		}
		if(3!=bTurn){
			DRW_DrawPOLY4_FB_Simple(&dest, &src[j],
				(int)inVec[0].x, (int)inVec[0].y,
				(int)inVec[1].x, (int)inVec[1].y,
				(int)inVec[2].x, (int)inVec[2].y,
				(int)inVec[3].x, (int)inVec[3].y, light,light,light);
		}else{
			D3DXVECTOR2	pt[4];
			if(ddeg == 0){
				pt[0].x = inVec[0].x -177*moveStep/18;
				pt[1].x = inVec[1].x +177*moveStep/18;
				pt[2].x = inVec[2].x -177*moveStep/18;
				pt[3].x = inVec[3].x +177*moveStep/18;
			}else if(ddeg == 90){
				for(i=0;i<4;i++){
					pt[i].x = inVec[i].x +177*moveStep/18;
				}
			}else if(ddeg == 270){
				for(i=0;i<4;i++){
					pt[i].x = inVec[i].x -177*moveStep/18;
				}
			}
			for(i=0;i<4;i++){
				if(i/2==0){
					pt[i].y = inVec[i].y -133*moveStep/18;
				}else{
					pt[i].y = inVec[i].y +133*moveStep/18;
				}
			}
			DRW_DrawPOLY4_FB_Simple(&dest, &src[j],
				(int)pt[0].x, (int)pt[0].y,
				(int)pt[1].x, (int)pt[1].y,
				(int)pt[2].x, (int)pt[2].y,
				(int)pt[3].x, (int)pt[3].y, light,light,light);
		}
	}
} //StaffmodeWnd::DrawMaze

extern char StaffName[44][17];


void StaffmodeWnd::writeName()
{
	int		i,j,x,y,fontSize,s_st,len;
	myRGB	*lpColor,color = {0,0,0xff};

	nameList.clearColorBuf();
	for(j=0;j<5;j++){
		if(j==2){
			fontSize = mainFontSize;
			lpColor = &color;
			y = 0;
		}else{
			fontSize = systemFontSize;
			lpColor = NULL;
			y = 4;
		}
		s_st = selectNum -2+j;
		if(s_st>43)s_st -= 44;
		if(s_st< 0)s_st += 44;
		len = strlen(StaffName[s_st]);
		x = 160*j +(160-len*(fontSize/2))/2; 
		OutputOutlineFont(&nameList,(BYTE *)"Å|",x-fontSize,y,fontSize,lpColor);
		OutputOutlineFont(&nameList,(BYTE *)"Å|",x+(fontSize/2*len),y,fontSize,lpColor);
		for(i=0;i<len;i++){
			OutputOutlineFont(&nameList,(BYTE *)StaffName[s_st] +i,x,y,fontSize,lpColor);
			if(BYTE(StaffName[s_st][i])>=0x80){
				i++;
				x+=fontSize;
			}else{
				x+=fontSize/2;
			}
		}
	}
}

void StaffmodeWnd::Draw()
{
	int		i;

	g_DibInf.colorBuf.BltFast(0,0,&baseWnd,NULL,TRUE);
	if(state!=3 && state!=0 && state!=6){
		DrawMaze(&g_DibInf.colorBuf);
	}
	if(!(state==2&&bTurn==3) && state<3){
		g_DibInf.colorBuf.BltFast(0,46,&nameList,NULL,TRUE);
	}
	if(state<6){
		for(i=0;i<3;i++){
			btn[i].Draw();
		}
		g_DibInf.colorBuf.BltFast(510,490,&logo,NULL,TRUE);
	}
} // StaffmodeWnd::Draw()



DWORD	StaffTime[] =
{
	0,2000,			
	 5130, 7130,	
	 9110,12010,	
	14010,16210,	
	18000,20000,	
	31000,33000,	
	44000,46000,	
	57000,59000,	
	70000,72000,	
	83000,85000,	
	96000,98000,	
	109000,111000,	
	122000,124000,	
	135000,137000,	
	148000,150000,	
	161000,163000,	
	174000,176000,	
	187000,189000,	
	200000,202000,	
	213000,215000,	
	226000,228000,	
	239000,241000,	

	252000,254000,

	255130,258100,	
	259160,262160,	
	267080,268280,	
	269280,270280,	
	273280,274280,	
};

#define StaffStart	17230
#define StaffEnd	252260

StaffRoll::StaffRoll()
{
	ZeroMemory(this,sizeof(StaffRoll));

	c_cls_all();
	msgWnd.MsgCLS();
	msgWnd.ForceHide();

	baseCBuf[primary].createColorBuf(WIN_SIZEX,WIN_SIZEY,24,FALSE);
	baseCBuf[primary].BltFast(0,0,&g_DibInf.colorBuf,NULL,FALSE);
	primary = !primary;
	baseCBuf[primary].loadLGF(pack_gparts,"ed_000");
	baseCBuf[primary].alpha = 0;
	baseCBuf[primary].offset.x = baseCBuf[primary].offset.y = 0;
	partsCBuf.loadLGF(pack_gparts,"ed_004");
	partsCBuf.alpha = 0;
	partsCBuf.offset.x = partsCBuf.offset.y = 168;
	pictCBuf[0].alpha = pictCBuf[1].alpha = 0;

	nammeCBuf[0].loadLGF(pack_gparts,"ed_name00");
	nammeCBuf[0].offset.x = 50;
	nammeCBuf[1].alpha = 0;

	startTime = timeGetTime();
	nextTime = StaffTime[step+1];
	fadeTime = StaffTime[step+1] -StaffTime[step];
	lpSoundDS->FadeOut(bgmHandle);
	changeExecMode(staffRoll_mode);
}

void StaffRoll::Exec()
{
	char		p_name[32];
	float		alpha;
	DWORD		read_time,time = timeGetTime();

	if(keyState.push_action || keyState.push_cancel || step==100){
		if(step<100){
			primary = !primary;
			baseCBuf[primary].loadLGF(pack_gparts,"ed_000");
			baseCBuf[primary].alpha = 0;
			baseCBuf[primary].offset.x = baseCBuf[primary].offset.y = 0;
			nextTime = timeGetTime() +2500;
			lpSoundDS->FadeOut(bgmHandle);
			step = 100;
		}else if(time < nextTime){
			baseCBuf[primary].alpha = 1.0f -(float(nextTime -time) / 2500);
		}else{
			backColorBuf.BltFast(40, 30,&baseCBuf[primary],NULL,FALSE);
			changeExecMode(event_exec_mode);
		}
		return;
	}
	bDrawName = FALSE;
	if(time>=StaffStart+startTime && time<=StaffEnd+startTime){
		staffNameY = 16800 * (time-(StaffStart+startTime)) / (StaffEnd -StaffStart);
		int nowNum = staffNameY / 600;
		if( nameNum < nowNum){
			nameNum = nowNum;
			primary_n = !primary_n;
			if(nameNum <= 26){
				sprintf(p_name,"ed_name%02d",nameNum);
				nammeCBuf[primary_n].loadLGF(pack_gparts,p_name);
				read_time = timeGetTime();
				startTime += (read_time -time);
				time = read_time;
			}else{
				nammeCBuf[primary_n].alpha = 0;
			}
			nammeCBuf[primary_n].offset.x = 50;
		}
		nammeCBuf[primary_n].offset.y = 600 -(staffNameY -600*nowNum);
		nammeCBuf[!primary_n].offset.y = nammeCBuf[primary_n].offset.y -600;
		bDrawName = TRUE;
	}
	if(step & 1){
		if(time >= (nextTime+startTime)){
			step ++;
			switch(step){
			  case 4:
				primary = !primary;
				baseCBuf[primary].loadLGF(pack_gparts,"ed_001");
				baseCBuf[primary].alpha = 0;
				baseCBuf[primary].offset.x = baseCBuf[primary].offset.y = 0;
				break;
			  case  8: case 10: case 12: case 14: case 16:
			  case 18: case 20: case 22: case 24: case 26:
			  case 28: case 30: case 32: case 34: case 36:
			  case 38: case 40: case 42:
				sprintf(p_name,"ed_pic%02d",picNum++);
				primary_p = !primary_p;
				pictCBuf[primary_p].loadLGF(pack_gparts,p_name);
				pictCBuf[primary_p].alpha = 0;
				pictCBuf[primary_p].offset.x = 416;
				pictCBuf[primary_p].offset.y = 314;
				break;
			  case 46:
				partsCBuf.loadLGF(pack_gparts,"ed_003");
				partsCBuf.alpha = 0;
				partsCBuf.offset.x = 200;
				partsCBuf.offset.y = 216;
				break;
			  case 48:
			  case 54:
				primary = !primary;
				baseCBuf[primary].loadLGF(pack_gparts,"ed_000");
				baseCBuf[primary].alpha = 0;
				baseCBuf[primary].offset.x = baseCBuf[primary].offset.y = 0;
				break;
			  case 52:
				primary = !primary;
				baseCBuf[primary].loadLGF(pack_gparts,"ed_002");
				baseCBuf[primary].alpha = 0;
				baseCBuf[primary].offset.x = baseCBuf[primary].offset.y = 0;
				break;
			}
			read_time = timeGetTime();
			startTime += (read_time -time);
			time = read_time;
			nextTime = StaffTime[step+1];
			fadeTime = StaffTime[step+1] -StaffTime[step];
		}else{
			return;
		}
	}
	if(time < (nextTime+startTime)){
		alpha = float(nextTime+startTime -time) / fadeTime;
		switch(step){
		  case 0:
		  case 4:
		  case 48:
		  case 52:
		  case 54:
			baseCBuf[primary].alpha = 1.0f -alpha;
			break;
		  case 2:
		  case 46:
			partsCBuf.alpha = 1.0f -alpha;
			break;
		  case 6:
		  case 50: 
			partsCBuf.alpha = alpha;
			break;
		  case  8: case 10: case 12: case 14: case 16:
		  case 18: case 20: case 22: case 24: case 26:
		  case 28: case 30: case 32: case 34: case 36:
		  case 38: case 40: case 42:
			pictCBuf[primary_p].alpha = 1.0f -alpha;
			break;
		  case 44:
			pictCBuf[primary_p].alpha = alpha;
			break;
		}
	}else{
		baseCBuf[primary].alpha = 1.0f;
		baseCBuf[!primary].alpha = 0;
		step ++;
		switch(step){
		  case 1:
			lpSoundDS->ChangeMusic(bgmHandle,pack_bgmfile,3,TRUE,1);
			break;
		  case 3:
		  case 47:
			partsCBuf.alpha = 1.0f;
			break;
		  case 7:
		  case 51:
			partsCBuf.alpha = 0;
			break;
		  case 45:
			pictCBuf[primary_p].alpha = 0;
			break;
		}
		if(step>=9 && step<=43){
			pictCBuf[primary_p].alpha = 1.0f;
			pictCBuf[!primary_p].alpha = 0;
		}
		if(55<=step){
			backColorBuf.BltFast(40, 30,&baseCBuf[primary],NULL,FALSE);
			changeExecMode(event_exec_mode);
			return;
		}
		nextTime = StaffTime[step+1];
		fadeTime = StaffTime[step+1] -StaffTime[step];
	}
}

void StaffRoll::Draw()
{
	g_DibInf.colorBuf.BltFast(0,0,&baseCBuf[!primary],NULL,TRUE);
	if(100>step)g_DibInf.colorBuf.BltFast(0,0,&baseCBuf[primary],NULL,TRUE);
	g_DibInf.colorBuf.BltFast(0,0,&pictCBuf[!primary_p],NULL,TRUE);
	g_DibInf.colorBuf.BltFast(0,0,&pictCBuf[primary_p],NULL,TRUE);
	g_DibInf.colorBuf.BltFast(0,0,&partsCBuf,NULL,TRUE);
	if(bDrawName){
		g_DibInf.colorBuf.BltFast(0,0,&nammeCBuf[!primary_n],NULL,TRUE);
		g_DibInf.colorBuf.BltFast(0,0,&nammeCBuf[primary_n],NULL,TRUE);
	}
	if(100==step)g_DibInf.colorBuf.BltFast(0,0,&baseCBuf[primary],NULL,TRUE);
}

char StaffName[44][17] = {
"ÉCÉPÉ_",
"êŒêÏê^ñÁ",
"ÉGÉCÉWêÏè„",
"ëÂãvï€ãÛãõ",
"ëÂíJå\",
"Ç©ÇµÇ‹Ç‰Ç§",
"ã‡ä€",
"â¡î[èCìÒ",
"ÉJÉèÉ^ÉqÉTÉV",
"àﬂä}ìπóY",
"Ç´ÇﬁÇÁÇΩÇ©Ç®",
"Ç±Ç∑Ç™Ç©Ç»Çﬂ",
"å√éõ",
"Ç≥Ç¬Ç‹Ç†Ç∞",
"é∞",
"ÉZÉåÉX",
"TAKEMi",
"ìåäCó—",
"íÜè„òaâp",
"íÜíJïêéj",
"ìÒã{àÍóY",
"î‰òCãeîTèï",
"ñçó¨",
"èºâ™èÉñÁ",
"Ç‹ÇÈÇ¢ÇΩÇØÇµ",
"Ç›Ç·Ç≠Ç≥Ç‹Ç≥Ç©Ç∏",
"ÇﬁÇÁÇ¡Çø",
"éRçËäxéu",

"èHótèGé˜",
"ä√òIé˜",
"çÅåéÅôàÍ",
"ç¿ä‘ê≠èH",
"êõè@åı",
"åéìáÇ›ÇøÇ‚",
"Ç»Ç©ÇﬁÇÁÇΩÇØÇµ",
"ì°å¥è\ñÈ",
"çÇã¥ê≠ãg",
"Ç›Ç¬Ç›î¸ó¢",
"Ç›Ç‚ÇØÇµÇÂÇ§Ç∑ÇØ",
"YUMÇ¢ÇÌÇ´",
"â°îˆå™àÍ",
"Ryu-FJR",
"òhå©ìw",
"Ç‰Ç¢Ç‹Ç‰ÇΩÇ©"};
