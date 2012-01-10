#include "AVGYMZ.h"
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
	  case event_exec_mode:
		if(sysInf.bMsgStop)MessageMenuSet( on );
		break;
	  case select_exec_mode:
		MessageMenuSet( on, TRUE);
		break;
	  case movie_mode:
		MessageMenuSet( off, TRUE );
		break;
	  default:
		MessageMenuSet( off );
		break;
	}
} // changeExecMode

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
			if(0==sysInf.bLookOpening){
				sysInf.bLookOpening = 1;
				if(ESC_GetGameFlag( 20 )){
					lpMovie->OpenMovie("kusari_op.avi",20,TRUE);
					return;
				}
			}
			my_delete(lpMovie);
			changeExecMode( event_exec_mode );	
			if(0==sysInf.bLookTitle){
				sysInf.bLookTitle = 1;
				titleWnd = new TitleWnd;
			}
		}
		break;
	  case opening_mode:
		if(titleWnd->Exec()){
			my_delete(titleWnd);
		}
		break;
	  case event_exec_mode:
	  case select_exec_mode:
		if(rudderWnd)rudderWnd->Exec();
		EXEC_ControlLang( &LangData );
		break;
	  case save_mode:
	  case load_mode:
		if(FALSE==saveWnd->selectExec()){
			if(opening_mode!=sysInf.execMode){
				my_delete(titleWnd);
			}
			my_delete(saveWnd);
		}
		break;
	  case backlog_mode:
		backLog.Exec();
		break;
	  case cg_look_mode:
	  case scene_play_mode:
		if(FALSE==cgmodeWnd->Exec()){
			my_delete(cgmodeWnd);
		}
		break;
	  case music_mode:
		if(FALSE==musicWnd->Exec()){
			my_delete(musicWnd);
		}
		break;
	  case option_mode:
		if(FALSE==optionWnd->Exec()){
			my_delete(optionWnd);
		}
		break;
	  case map_mode:
		if(FALSE==mapWnd->Exec()){
			my_delete(mapWnd);
		}
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
#define FPS_CYCLE		33
BOOL Render()
{
	int			i,fps_cycle;
	sysInf.animeCnt ++;
	if(saveInf.bWave) saveInf.waveCnt ++;
	if(saveInf.demo && saveInf.demo_max){
		saveInf.demo_cnt ++;
		if(saveInf.demo_cnt>=saveInf.demo_max)saveInf.demo = 0;
	}
	switch(sysInf.setFPS){
		case 0:
			fps_cycle = 100;
			break;
		case 1:
			fps_cycle = 66;	
			break;
		default:
			fps_cycle = 33;	
			break;
	}
	DWORD	nowTime = timeGetTime() -sysInf.oldTime;
	if(nowTime<=0 || sysInf.bForceRender){
		fps = 1000 /fps_cycle;
	}else{
#ifdef bFpsSet
		if(nowTime <DWORD(fps_cycle*(sysInf.render_skip+1))){
			if(nowTime <DWORD(fps_cycle*(sysInf.render_skip+1)/2)){
				Sleep(fps_cycle*(sysInf.render_skip+1)/2-nowTime);
				nowTime = timeGetTime() -sysInf.oldTime;
			}else if(0==sysInf.setFPS){
				Sleep(1);
			}
		}else if(sysInf.render_skip<8){
			sysInf.render_skip ++;
			return FALSE;
		}
#endif
		fps = 1000/(nowTime);
	}
	sysInf.bForceRender = off;
	sysInf.oldTime = timeGetTime();
	sysInf.render_skip = 0;
	sysInf.g_skip ++;

	switch(sysInf.execMode){
	  case opening_mode:
		titleWnd->Draw();
		break;
	  case backlog_mode:
		backLog.DrawBackLog();
		break;
	  case save_mode:
	  case load_mode:
		if(saveWnd)saveWnd->Blt();
		break;
	  case cg_look_mode:
	  case scene_play_mode:
		cgmodeWnd->Draw();
		break;
	  case music_mode:
		musicWnd->Draw();
		break;
	  case option_mode:
		optionWnd->Draw();
		break;
	  case map_mode:
		mapWnd->Draw();
		break;
	  case game_end_mode:
		break;
	  case movie_mode:
		return FALSE;
	  default:
		SetBmpDraw();
		drawBack();
		if(itemList)itemList->Draw();
		if(rudderWnd)rudderWnd->Draw();
		msgWnd.BltMessage();
		selectWnd.selectBlt();
		if(saveWnd){
			saveWnd->Blt();
			if(FALSE==saveWnd->bFadeOut)
				my_delete(saveWnd);
		}
		break;
	}
	if(sysInf.bInfo){
		sprintf(debugBuf,"fps=%3d",fps);
		TextOut(g_DibInf.memDC,10,10,debugBuf,strlen(debugBuf));
	}
	return TRUE;
} // Render
