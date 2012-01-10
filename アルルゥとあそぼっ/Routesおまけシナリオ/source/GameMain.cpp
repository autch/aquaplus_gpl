#include "ClCommon.h"
#include "backLog.h"
#include "keyInput.h"
#include "movie.h"
#include "saveWnd.h"
#include "select.h"
#include "soundDS.h"
#include "systemWnd.h"

BOOL			Render(void);
RECT			rect;
POINT			point;

void changeExecMode(exec_mode mode)
{
	if(sysInf.execMode==mode)return;	
	sysInf.execMode_backUp = sysInf.execMode;
	sysInf.execMode = mode;
	switch(mode){
	  case event_exec_mode: select_exec_mode:
		sideWnd.btn[2].SetEnable(TRUE);
		sideWnd.btn[3].SetEnable(TRUE);
		sideWnd.btn[4].SetEnable(TRUE);
		break;
	  default:
		sideWnd.btn[2].SetEnable(FALSE);
		sideWnd.btn[3].SetEnable(FALSE);
		sideWnd.btn[4].SetEnable(FALSE);
	}
} // changeExecMode

void mainLoop()
{
	if(lpSoundDS && lpSoundDS->lpNextMusic){
		lpSoundDS->NextMusicStart();
	}
	lookPadState();
	lookPadRepeat();

	if(sideWnd.Check(&sysInf.msPoint)) goto keyInOut;

	switch(sysInf.execMode){
	  case movie_mode:
		if(FALSE==lpMovie->IsPlay()){
			my_delete(lpMovie);
			changeExecMode( event_exec_mode );	
			titleWnd = new TitleWnd;
		}
		return;
	  case staffRoll_mode:
		lpStaffRoll->Exec();
		break;
	  case opening_mode:
		titleWnd->Exec();
		break;
	  case cgview_mode:
		lpCGmodeWnd->Exec();
		break;
	  case event_exec_mode:
	  case select_exec_mode:
		EXEC_ControlLang( &LangData );
		break;
	  case save_mode:
	  case load_mode:
		if(FALSE==saveWnd->selectExec()){
			my_delete(saveWnd);
		}
		break;
	  case backlog_mode:
		backLog.Exec();
		break;
	}
keyInOut:
	if(Render()){
		HDC		hDC = GetDC(sysInf.hWnd);
		BitBlt(hDC,0,0,WIN_SIZEX,WIN_SIZEY,g_DibInf.memDC,0,0,SRCCOPY);
		ReleaseDC(sysInf.hWnd,hDC);
	}
} // mainLoop

int		fps;
#define FPS_CYCLE		16
BOOL Render()
{
	int			i;
	sysInf.animeCnt ++;
	if(bgInf.shake_cnt) bgInf.shake_cnt ++;
	if(saveInf.bWave) saveInf.waveCnt ++;
	if(saveInf.demo && saveInf.demo_max){
		saveInf.demo_cnt ++;
		if(saveInf.demo_cnt>=saveInf.demo_max)saveInf.demo = 0;
	}
	DWORD	nowTime = timeGetTime() -sysInf.oldTime;
	if(nowTime<=0 || sysInf.bForceRender){
		fps = 1000 /FPS_CYCLE;
	}else{
#ifdef bFpsSet
		if(nowTime <DWORD(FPS_CYCLE*(sysInf.render_skip+1))){
			Sleep(FPS_CYCLE*(sysInf.render_skip+1)-nowTime);
			nowTime = timeGetTime() -sysInf.oldTime;
		}else if(sysInf.render_skip<5){
			sysInf.render_skip ++;
			return FALSE;
		}
#endif
		fps = 1000/(nowTime);
	}
	sysInf.bForceRender = off;
	sysInf.oldTime = timeGetTime();
	sysInf.render_skip = 0;

	switch(sysInf.execMode){
	  case opening_mode:
		titleWnd->Draw();
		break;
	  case staffRoll_mode:
		lpStaffRoll->Draw();
		break;
	  case cgview_mode:
		lpCGmodeWnd->Draw();
		break;
	  case backlog_mode:	
		backLog.DrawBackLog();
		sideWnd.Draw();
		break;
	  case save_mode:
	  case load_mode:
		if(saveWnd)saveWnd->Blt();
		break;
	  default:
		drawBG();
		SetBmpDraw();
		drawChar();
		drawBack();
		msgWnd.BltMessage();
		selectWnd.selectBlt();
		sideWnd.Draw();
		break;
	}
#ifdef _DEBUG
	if(sysInf.bInfo){
		sprintf(debugBuf,"fps=%3d",fps);
		TextOut(g_DibInf.memDC,10,10,debugBuf,strlen(debugBuf));
	}
#endif
	return TRUE;
} // Render
