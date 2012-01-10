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
} // changeExecMode

void AVG_SetScenarioNo( int scenario_no );
void StartMainLoop();

void mainLoop()
{
	if(lpSoundDS && lpSoundDS->lpNextMusic){
		lpSoundDS->NextMusicStart();
	}
	lookPadState();
	lookPadRepeat();

	switch(sysInf.execMode){
	  case movie_mode:
		if(FALSE==lpMovie->IsPlay()){
			my_delete(lpMovie);
			static BOOL bOpOnece = TRUE;
			if(bOpOnece){
				lpSoundDS->ChangeMusic(bgmHandle,pack_bgmfile,2,FALSE,0);
				lpMovie = new CMovie();
				lpMovie->OpenMovie("title.avi",-1,TRUE);
				bOpOnece = FALSE;
			}else{
				PlayEffectNum(evtHandle[1],SelectSnd);
				StartMainLoop();
			}
		}
		return;
	  case opening_mode:
		titleWnd->Exec();
		break;
	  case staffRoll_mode:
		lpStaffRoll->Exec();
		break;
	  case event_exec_mode:
	  case select_exec_mode:
		EXEC_ControlLang( &LangData );
		break;
	  case menu_mode:
		lpMenuWnd->Exec();
		break;
	  case kabegami_mode:
		lpKabeSetWnd->Exec();
		break;
	  case staffmode_mode:
		lpStaffmodeWnd->Exec();
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
	  case backlog_mode:
		backLog.DrawBackLog();
		break;
	  case staffRoll_mode:
		lpStaffRoll->Draw();
		break;
	  case save_mode:
	  case load_mode:
		if(saveWnd)saveWnd->Blt();
		break;
	  case menu_mode:
		lpMenuWnd->Draw();
		break;
	  case kabegami_mode:
		lpKabeSetWnd->Draw();
		break;
	  case staffmode_mode:
		lpStaffmodeWnd->Draw();
		break;
	  default:
		drawBG();
		SetBmpDraw();
		drawChar();
		drawBack();
		msgWnd.BltMessage();
		selectWnd.selectBlt();
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


void StartMainLoop()
{
	EXEC_ReadLang( "0000.sdt", &LangData );
	AVG_SetScenarioNo(0);
	EXEC_StartLang( &LangData, MAIN_SCRIPT );
	c_cls_all();
	msgWnd.MsgCLS();
	changeExecMode( event_exec_mode );
	SYSTEMTIME	sysTime;
	GetLocalTime(&sysTime);
	srand( sysTime.wMilliseconds );
	for(int i=0;i<sysTime.wMilliseconds;i++)rand();

	if(sysInf.bOPMessage) LangData.reg[3] = 1;
}

