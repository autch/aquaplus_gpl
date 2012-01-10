#include "AVGYMZ.h"
#include "graphic.h"
#include "keyInput.h"
#include "msgWnd.h"
#include "saveWnd.h"
#include "select.h"
#include "soundDS.h"
#include "movie.h"
#include "systemWnd.h"
#include <commctrl.h>
#include <winnls32.h>

SystemInfo		sysInf;
DIBInfo			g_DibInf;
char			debugBuf[256];
int				DispFreqMax = 0;
#define			TITLE_NAME	"鎖－クサリ－"

BOOL initApplication( HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE hInstance,int nCmdShow);
LRESULT APIENTRY MainWndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
void SystemInit(LPSTR lpCmdLine);
void readProfile(void);
void writeProfile(void);
void cleanup(void);
void mainLoop(void);

void SetMyMenuItem(void);
BOOL CALLBACK DebugDlgProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
BOOL CALLBACK AboutDialogBoxProc( HWND hwnd, UINT wmes, UINT wparam, LONG lparam );
BOOL CALLBACK InfoDialogBoxProc( HWND hWnd, UINT wmes, UINT wparam, LONG lparam );
BOOL CALLBACK FlagDialogBoxProc( HWND hWnd, UINT wmes, UINT wparam, LONG lparam );

BOOL OpenPackFile()
{
	int			i,ret;

	char		packfileName[pack_tail][28] =
	{	"BGMFILE",  "SOUNDDS","VOICE","GPARTS", "EVENTCG","SCRIPT","ANIME"};

	readFile = new ClReadFile;
	for(i=0;i<pack_tail;i++){
		ret = readFile->OpenPackFile(packfileName[i]);
		if(-1 == ret){
			myOutputDebugString("ﾌｧｲﾙ『%s.PAK』が見つかりませんでした。ゲームは起動できません。\n",packfileName[i]);
			my_delete(readFile);
			return FALSE;
		}
	}
	return TRUE;
} // OpenPackFile

void SystemInit(LPSTR lpCmdLine)
{
	CreateAlphaTable();
	createInitColorBuf();
	msgWnd.InitMessageParts();
	MessageMenuSet( off, TRUE );
	InitDirectInput(sysInf.hInstance);
	lpSoundDS = new ClSoundDS(
		sysInf.hWnd, readFile,
		pack_bgmfile, pack_sound, pack_voice,
		sysInf.bgmVolume, sysInf.seVolume, sysInf.voiceVolume,
		FALSE);

	if(sysInf.bAutoRead){
		CheckMenuItem( sysInf.hMenu, ID_AUTOREAD,   MF_CHECKED );
		msgWnd.msgBtn[3].SetState(btn_lock);	
	}
	if(sysInf.bInfo){
		sysInf.hDebugWnd = CreateDialog(
			sysInf.hInstance, MAKEINTRESOURCE(IDD_DEBUGWND), sysInf.hWnd, (DLGPROC)DebugDlgProc);
	}
	if(3==sysInf.movieQuality){
		GetCpuID();
		DWORD clock = GetCpuClock();
		if(clock >= 1800){
			sysInf.movieQuality = 2;
		}else if(clock>=1000){
			sysInf.movieQuality = 1;
		}else{
			sysInf.movieQuality = 0;
		}
	}
	if(2==sysInf.movieQuality){
		CheckMenuItem( sysInf.hMenu, ID_MOVIEHI,   MF_CHECKED );
	}else if(1==sysInf.movieQuality){
		CheckMenuItem( sysInf.hMenu, ID_MOVIEMID,  MF_CHECKED );
	}else{
		CheckMenuItem( sysInf.hMenu, ID_MOVIELOW,  MF_CHECKED );
	}
	if(2==sysInf.setFPS){
		CheckMenuItem( sysInf.hMenu, ID_FPS60,  MF_CHECKED );
	}else if(1==sysInf.setFPS){
		CheckMenuItem( sysInf.hMenu, ID_FPS30,  MF_CHECKED );
	}else{
		CheckMenuItem( sysInf.hMenu, ID_FPS20,  MF_CHECKED );
	}
	if(2==sysInf.nouseOverlay){
		char	buf[1024];
		int		i,j,device;
		device = EnumGraphicsDevice(  );
		for(i=0 ; i<device ; i++ ){
			strcpy( buf,GetGraphicsDeviceName(i) );
			for(j=0;j<strlen(buf);j++){
				buf[j] = tolower( buf[j] );
			}
			if( strstr(buf,"matrox") ){
				break;
			}
		}
		if(i<device){
			sysInf.nouseOverlay = 1;
		}else{
			sysInf.nouseOverlay = 0;
		}
	}
	if(sysInf.nouseOverlay==1){
		CheckMenuItem( sysInf.hMenu, ID_NOUSEOVERLAY,  MF_CHECKED );
	}
	toneConvert.ClearTable();
	InitReadFlag();
	rudderWnd = new RudderWnd;
	if(lpCmdLine[0]=='\0'){
		lpMovie = new CMovie(sysInf.nouseOverlay);
		lpMovie->OpenMovie("leaflogo.avi",-1,TRUE);
	}else{
		sysInf.bLookTitle = 1;	
		sysInf.bLookOpening = 1;
		int index = EXEC_ReadLang( lpCmdLine, &LangData );
		AVG_SetScenarioNo(index);
		EXEC_StartLang( &LangData, MAIN_SCRIPT );
		c_cls_all();
		msgWnd.MsgCLS();
		lpSoundDS->FadeOut(bgmHandle);
		changeExecMode( event_exec_mode );
	}
} // SystemInit

int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance, LPSTR lpCmdLine,int nCmdShow )
{
    MSG				msg;

#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG)); 
#endif

	if( !initApplication(hInstance) ) return FALSE;

	HANDLE hMutex = CreateMutex(NULL,NULL,className);
	if(ERROR_ALREADY_EXISTS == GetLastError()){
		if( lpCmdLine[0]!='\0'){
			HWND hWin=FindWindow( className, NULL );
			if( hWin ){
				SendMessage( hWin, WM_COMMAND, ID_END, 0 );
			}
		}else{
			return FALSE;
		}
	}
	HANDLE	hAccel = LoadAccelerators( hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR) );

	CoInitialize(NULL);
	readProfile();
	if( !OpenPackFile() ) return FALSE;
	if( !InitInstance(hInstance,nCmdShow) ) return FALSE;
	SystemInit(lpCmdLine);
	WINNLSEnableIME(sysInf.hWnd,FALSE);
	TIMECAPS tc;
	timeGetDevCaps(&tc,sizeof(tc));
	timeBeginPeriod(tc.wPeriodMin);

	while(FALSE==sysInf.bGameEnd){
		BOOL bGotMsg;

		if(sysInf.bIsActive || lpMovie || !sysInf.bBackStop){
			bGotMsg = PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE);
		}else{
            bGotMsg = GetMessage( &msg, NULL, 0U, 0U );
		}
		if(bGotMsg){
			if(TranslateAccelerator( sysInf.hWnd, (HACCEL)hAccel, &msg ))continue;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if(sysInf.bIsActive || lpMovie || !sysInf.bBackStop){
			mainLoop();
		}
	}
	WINNLSEnableIME(sysInf.hWnd,TRUE);
	timeEndPeriod(tc.wPeriodMin);
	writeProfile();

	cleanup();
	CoUninitialize();
	CloseHandle(hMutex);
	return msg.wParam;
} // WinMain


BOOL initApplication( HINSTANCE hInstance)
{
	WNDCLASSEX    wc;
	BOOL		  rc;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance,MAKEINTRESOURCE(IDI_WINAVG));
	wc.hCursor = LoadCursor(NULL,IDC_ARROW);
	wc.hbrBackground = CreateSolidBrush(RGB(0, 0, 8));
	wc.lpszMenuName =  NULL;
	wc.lpszClassName = className;
	wc.hIconSm = (HICON)LoadImage(hInstance,MAKEINTRESOURCE(IDI_WINAVG),IMAGE_ICON,16,16,LR_DEFAULTCOLOR);
	rc = RegisterClassEx( &wc );

    sysInf.hCurLink = LoadCursor(hInstance,MAKEINTRESOURCE(IDC_LINKCURSOR));
	return (rc);
} // initApplication

void ScreenChange(BYTE full_screen, BOOL bUseRrate)
{
	if(sysInf.full_screen == full_screen)return;

	if(optionWnd){
		if(full_screen==0){
			optionWnd->screenBtn[0].SetState(btn_push);
			optionWnd->screenBtn[1].SetState(btn_normal);
		}else{
            optionWnd->screenBtn[0].SetState(btn_normal);
			optionWnd->screenBtn[1].SetState(btn_push);
		}
	}
	sysInf.full_screen = full_screen;
	sysInf.bDisplayChange = TRUE;
	if(0==full_screen){
		if(sysInf.bShowMenu){
			ShowWindow(sysInf.hMenuWnd, SW_HIDE);
			sysInf.bShowMenu = off;
		}
		SetWindowLong(sysInf.hWnd, GWL_STYLE, WINDOWMODE_STYLE);
		SetMenu(sysInf.hWnd, sysInf.hMenu);
		sysInf.refreshRate = 0;
		ChangeDisplaySettings(NULL,CDS_FULLSCREEN);
	}else{
		DEVMODE	devMode;
		SetMenu(sysInf.hWnd, NULL);
		SetWindowLong(sysInf.hWnd, GWL_STYLE, FULLMODE_STYLE);
		ZeroMemory(&devMode,sizeof(DEVMODE));
		devMode.dmSize = sizeof(DEVMODE);
		devMode.dmPelsWidth = sysInf.ScreenW;
		devMode.dmPelsHeight = sysInf.ScreenH;
		devMode.dmBitsPerPel = (sysInf.bUseBPP24)?24:32;
		if(!bUseRrate){
			devMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;
		}else{
			devMode.dmDisplayFrequency = sysInf.refreshRate = DispFreq[full_screen-1];
			devMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL | DM_DISPLAYFREQUENCY;
		}
		ChangeDisplaySettings(&devMode,CDS_FULLSCREEN);
	}
} // ScreenChange

BOOL InitInstance(HINSTANCE hInstance,int nCmdShow )
{
	HDC		hDC;
	int		i;

	sysInf.ScreenW = WIN_SIZEX;
	sysInf.ScreenH = WIN_SIZEY;
	sysInf.hInstance = hInstance;
	sysInf.hMenu = LoadMenu( NULL, MAKEINTRESOURCE(IDR_MAINMENU) );
	sysInf.bUseBPP24 = CheckScreenMode_BPP24();
	SetMyMenuItem();

retryCreateWin:
	if(0==sysInf.full_screen){
		if(GetSystemMetrics(SM_CXSCREEN)<=WIN_SIZEX || GetSystemMetrics(SM_CYSCREEN)<=WIN_SIZEY){
			if(IDYES!=MessageBox(NULL,"この解像度では、Windowモードでは起動できません。フルスクリーンモードでゲームを起動しますか？","カラーモード",MB_YESNO|MB_ICONWARNING)){
				return (FALSE);
			}else{
				sysInf.full_screen = 1;
				goto retryCreateWin;
			}
		}
		sysInf.hWnd = CreateWindowEx(
			0,className,TITLE_NAME,
			WINDOWMODE_STYLE ,
			(GetSystemMetrics(SM_CXSCREEN)-sysInf.ScreenW)/2,(GetSystemMetrics(SM_CYSCREEN)-sysInf.ScreenH)/2,
			sysInf.ScreenW +GetSystemMetrics(SM_CXDLGFRAME)*2,
			sysInf.ScreenH +GetSystemMetrics(SM_CYDLGFRAME)*2 +GetSystemMetrics(SM_CYBORDER) +GetSystemMetrics(SM_CYSIZE) +GetSystemMetrics(SM_CYMENU),
			NULL, sysInf.hMenu, hInstance, NULL);

	}else{
		int full_screen = 1;
		sysInf.full_screen = 0;
		for(i=0; i<DispFreqMax; i++){
			if(sysInf.refreshRate==DispFreq[i]){
				full_screen = i+1;
				break;
			}
		}
		if(i==DispFreqMax)sysInf.refreshRate = 0;
		sysInf.hWnd = CreateWindowEx(
			0,className,"鎖－クサリ－",
			FULLMODE_STYLE,
			0, 0, sysInf.ScreenW, sysInf.ScreenH,
			NULL, NULL, hInstance, NULL);
		sysInf.hMenuWnd = CreateWindow(className,"menuWnd",WS_POPUP,
			0,0,
			WIN_SIZEX,GetSystemMetrics(SM_CYMENUSIZE),sysInf.hWnd,sysInf.hMenu,sysInf.hInstance,NULL);
		ScreenChange(full_screen , sysInf.refreshRate);
	}
	if( !sysInf.hWnd )return FALSE;
	hDC = GetDC(sysInf.hWnd);
	sysInf.bpp = GetDeviceCaps(hDC,BITSPIXEL);
	ReleaseDC(sysInf.hWnd,hDC);
	if(sysInf.bpp<16){	
		MessageBox(sysInf.hWnd,"このゲームはHightColor(16Bit 65536色)モード以下のカラーモードでは動作しません。\nTrueColorかHightColorモードに切り替えて起動してください。","カラーモード",MB_OK|MB_ICONWARNING);
		return (FALSE);
	}
	SetCursor(LoadCursor(NULL,IDC_ARROW));
	return (TRUE);
} // InitInstance

#include <zmouse.h>

BOOL CALLBACK DebugDlgProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch(msg) {
	  case WM_INITDIALOG:
		SetFocus(sysInf.hWnd);
		return TRUE;
	  case WM_CLOSE:
		EndDialog( hWnd, TRUE );
		sysInf.hDebugWnd = NULL;
		break;
	}
	return FALSE;
} // MyDlgMSGProc

void myOutputDebugString(const char *formatstring, ...)
{
#ifndef _MASTER
	if(NULL==sysInf.hDebugWnd)return;

	int nSize = 0;
	char buff[255];
	va_list args;
	va_start(args, formatstring);
	nSize = _vsnprintf( buff, sizeof(buff), formatstring, args);
	ListAddString(sysInf.hDebugWnd,IDC_DEBUGTEXT,buff);
	int	index = SendDlgItemMessage(sysInf.hDebugWnd, IDC_DEBUGTEXT,LB_GETCOUNT,0,0);

	SendDlgItemMessage(sysInf.hDebugWnd, IDC_DEBUGTEXT,LB_SETTOPINDEX, index-24, 0);
	index = SendDlgItemMessage(sysInf.hDebugWnd, IDC_DEBUGTEXT,LB_GETTOPINDEX, 0, 0);

	va_end(args);
#endif
}

LRESULT APIENTRY MainWndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	int				i;
	PAINTSTRUCT		ps;

	switch( message ){
	case WM_CREATE:
		InitCommonControls();
		break;
	case WM_MOUSEWHEEL:
		sysInf.zDelta = (short) HIWORD(wParam);
		break;
    case WM_DESTROY:
	case WM_QUIT:
	case WM_CLOSE:
#ifdef _MASTER
		if(IDYES==MessageBox(sysInf.hWnd,"鎖－クサリ－ を終了します。よろしいですか？","終了確認",MB_YESNO)){
			sysInf.bGameEnd = on;
		}
#else
		sysInf.bGameEnd = on;
#endif
		return 0;
	case WM_ACTIVATE:
		for(i=0;i<g_iDIDeviceNum;i++){
			if(LOWORD(wParam) == WA_INACTIVE){
				g_pDIDevice[i]->Unacquire();
			}else{
				g_pDIDevice[i]->Acquire();
				g_pDIDevice[i]->Poll();
			}
		}
		sysInf.bIsActive = LOWORD(wParam) ? TRUE : FALSE;
		if(sysInf.bIsActive)myOutputDebugString("ActivateHandle: Active\n");
		else myOutputDebugString("ActivateHandle: Non Active\n");
		if(sysInf.bIsActive)clearPadState();
		break;
	case WM_KEYDOWN:
#ifndef _MASTER
		switch(wParam){
		  case (VK_F2):
			sysInf.bInfo = !sysInf.bInfo;
			if(!sysInf.bInfo && sysInf.hDebugWnd){
				SendMessage(sysInf.hDebugWnd,WM_CLOSE,0,0);
			}else if(sysInf.bInfo && NULL==sysInf.hDebugWnd){
				sysInf.hDebugWnd = CreateDialog(
					sysInf.hInstance, MAKEINTRESOURCE(IDD_DEBUGWND), sysInf.hWnd, (DLGPROC)DebugDlgProc);
			}
			break;
		}
#endif
		break;
	case WM_COMMAND:
		switch( LOWORD(wParam) ){
		  case ID_END:
			sysInf.bGameEnd = on;
			return 0;
		  case ID_MYAPP_EXIT:
#ifdef _MASTER
			if(IDYES==MessageBox(sysInf.hWnd,"鎖－クサリ－ を終了します。よろしいですか？","終了確認",MB_YESNO)){
				sysInf.bGameEnd = on;
			}
#else
			sysInf.bGameEnd = on;
#endif
			return 0;
		  case ID_DISPCHANGE:
			if(0==sysInf.full_screen){
				if(CheckOSw2k()){
					SendMessage(hWnd,WM_COMMAND,ID_SUBMENU,0);
				}else{
					SendMessage(hWnd,WM_COMMAND,ID_DISPFULL,0);
				}
			}else{
				SendMessage(hWnd,WM_COMMAND,ID_DISPWINDOW,0);
			}
			break;
		  case ID_DISPFULL:
			if(lpMovie)	return 0;
			ScreenChange(1,0);
			EnableMenuItem( sysInf.hMenu,ID_DISPFULL,   MF_GRAYED  );
			CheckMenuItem( sysInf.hMenu, ID_DISPFULL,   MF_CHECKED );	
			EnableMenuItem( sysInf.hMenu,ID_DISPWINDOW, MF_ENABLED );
			CheckMenuItem( sysInf.hMenu, ID_DISPWINDOW, MF_UNCHECKED );
			break;
		  case ID_DISPWINDOW:
			if(lpMovie)	return 0;
			ScreenChange(0,0);
			if(CheckOSw2k()){
				for(i=0;i<DispFreqMax;i++){
					EnableMenuItem( sysInf.hMenu, ID_SUBMENU+i,  MF_ENABLED  );
					CheckMenuItem( sysInf.hMenu, ID_SUBMENU+i,   MF_UNCHECKED );
				}
			}else{
				EnableMenuItem( sysInf.hMenu, ID_DISPFULL,  MF_ENABLED );
				CheckMenuItem( sysInf.hMenu, ID_DISPFULL,   MF_UNCHECKED );
			}
			EnableMenuItem( sysInf.hMenu, ID_DISPWINDOW, MF_GRAYED  );
			CheckMenuItem( sysInf.hMenu, ID_DISPWINDOW,  MF_CHECKED   );
			break;
		  case ID_SUBMENU:		case ID_SUBMENU+1:	case ID_SUBMENU+2:	case ID_SUBMENU+3:
		  case ID_SUBMENU+4:	case ID_SUBMENU+5:	case ID_SUBMENU+6:	case ID_SUBMENU+7:
			if(lpMovie)	return 0;
			ScreenChange( wParam-ID_SUBMENU+1 ,TRUE);
			for(i=0;i<DispFreqMax;i++){
				EnableMenuItem( sysInf.hMenu, ID_SUBMENU+i,  MF_ENABLED  );
				CheckMenuItem( sysInf.hMenu, ID_SUBMENU+i,   MF_UNCHECKED );
			}
			EnableMenuItem( sysInf.hMenu, LOWORD(wParam),  MF_GRAYED  );
			CheckMenuItem( sysInf.hMenu, LOWORD(wParam),   MF_CHECKED );
			EnableMenuItem( sysInf.hMenu, ID_DISPWINDOW, MF_ENABLED );
			CheckMenuItem( sysInf.hMenu, ID_DISPWINDOW,  MF_UNCHECKED );
			break;
		  case ID_AUTOREAD:
			msgWnd.toggleAutoRead();
			break;
		  case ID_READSKIP:
			msgWnd.toggleMessageSkip();
			break;
		  case ID_UPDATE:
			ShellExecute( sysInf.hWnd, NULL, "http://leaf.aquaplus.co.jp", NULL, NULL, SW_SHOWNORMAL );
			break;
		  case ID_VER:
			DialogBox( sysInf.hInstance, MAKEINTRESOURCE(IDD_ABOUTDIALOG), sysInf.hWnd, AboutDialogBoxProc );
			break;
		  case ID_SYSTEMINFO:
			DialogBox( sysInf.hInstance, MAKEINTRESOURCE(IDD_INFODIALOG), sysInf.hWnd, InfoDialogBoxProc );
			break;
		  case ID_CONFIG:
			if(NULL==optionWnd)	optionWnd = new OptionMode;
			break;
#ifndef _MASTER
		  case ID_FLAGEDIT:
			DialogBox( sysInf.hInstance, MAKEINTRESOURCE(IDD_FLAGEDIT), sysInf.hWnd, FlagDialogBoxProc );
			break;
#endif
		  case ID_LOADEXEC:
			if(ESC_GetFlag(_セーブロード無効))break;
			switch(sysInf.execMode){
			  case event_exec_mode: case select_exec_mode: case opening_mode:
				if(NULL==EXEC_LangBuf){
					EXEC_LangBuf  = (char *)&LangData.LangBuf;
					EXEC_LangInfo = &LangData;
				}
				if(NULL==saveWnd)saveWnd = new SaveWnd(load_mode);
				break;
			}
			break;
		  case ID_SAVEEXEC:
			if(ESC_GetFlag(_セーブロード無効))break;
			if(NULL==EXEC_LangBuf)break;
			switch(sysInf.execMode){
			  case event_exec_mode: case select_exec_mode:
				if(NULL==saveWnd)saveWnd = new SaveWnd(save_mode);
				break;
			}
			break;
		  case ID_BACKTITLE:
			ReturnTitile(TRUE);
			break;
		  case ID_BACKSELECT:
			SAV_Load( 98 );
			break;
		  case ID_MOVIEHI:
			sysInf.movieQuality = 2;
			CheckMenuItem( sysInf.hMenu, ID_MOVIEHI,   MF_CHECKED );
			CheckMenuItem( sysInf.hMenu, ID_MOVIEMID,  MF_UNCHECKED );
			CheckMenuItem( sysInf.hMenu, ID_MOVIELOW,  MF_UNCHECKED );
			break;
		  case ID_MOVIEMID:
			sysInf.movieQuality = 1;
			CheckMenuItem( sysInf.hMenu, ID_MOVIEHI,   MF_UNCHECKED );
			CheckMenuItem( sysInf.hMenu, ID_MOVIEMID,  MF_CHECKED );
			CheckMenuItem( sysInf.hMenu, ID_MOVIELOW,  MF_UNCHECKED );
			break;
		  case ID_MOVIELOW:
			sysInf.movieQuality = 0;
			CheckMenuItem( sysInf.hMenu, ID_MOVIEHI,   MF_UNCHECKED );
			CheckMenuItem( sysInf.hMenu, ID_MOVIEMID,  MF_UNCHECKED );
			CheckMenuItem( sysInf.hMenu, ID_MOVIELOW,  MF_CHECKED );
			break;
		  case ID_NOUSEOVERLAY:
			sysInf.nouseOverlay = !sysInf.nouseOverlay;
			if(sysInf.nouseOverlay){
				CheckMenuItem( sysInf.hMenu, ID_NOUSEOVERLAY,  MF_CHECKED );
			}else{
				CheckMenuItem( sysInf.hMenu, ID_NOUSEOVERLAY,  MF_UNCHECKED );
			}
			break;
		  case ID_FPS60:
			sysInf.setFPS = 2;
			CheckMenuItem( sysInf.hMenu, ID_FPS60,  MF_CHECKED );
			CheckMenuItem( sysInf.hMenu, ID_FPS30,  MF_UNCHECKED );
			CheckMenuItem( sysInf.hMenu, ID_FPS20,  MF_UNCHECKED );
			break;
		  case ID_FPS30:
			sysInf.setFPS = 1;
			CheckMenuItem( sysInf.hMenu, ID_FPS60,  MF_UNCHECKED );
			CheckMenuItem( sysInf.hMenu, ID_FPS30,  MF_CHECKED );
			CheckMenuItem( sysInf.hMenu, ID_FPS20,  MF_UNCHECKED );
			break;
		  case ID_FPS20:
			sysInf.setFPS = 0;
			CheckMenuItem( sysInf.hMenu, ID_FPS60,  MF_UNCHECKED );
			CheckMenuItem( sysInf.hMenu, ID_FPS30,  MF_UNCHECKED );
			CheckMenuItem( sysInf.hMenu, ID_FPS20,  MF_CHECKED );
			break;
		}
		break;
	case WM_SYSCOMMAND:
		if(sysInf.full_screen){
			switch (wParam) {
				case SC_KEYMENU :
					return 0;
			}
		}else{
			switch(wParam & 0xfff0){
			  case SC_MAXIMIZE:
				if(lpMovie)	return 0;
				ScreenChange(1,0);
				EnableMenuItem( sysInf.hMenu,ID_DISPFULL,   MF_GRAYED  );
				CheckMenuItem( sysInf.hMenu, ID_DISPFULL,   MF_CHECKED );
				EnableMenuItem( sysInf.hMenu,ID_DISPWINDOW, MF_ENABLED );
				CheckMenuItem( sysInf.hMenu, ID_DISPWINDOW, MF_UNCHECKED );
				return 0;
			}
		}
		break;
	case WM_DISPLAYCHANGE:
		if(!sysInf.bDisplayChange)break;
		SetTimer(sysInf.hWnd, 1, 500, NULL);	
		break;
	case WM_TIMER:
		if(!sysInf.bDisplayChange || 1!=wParam)break;
		KillTimer(sysInf.hWnd, wParam);
		if(0==sysInf.full_screen){
			MoveWindow(sysInf.hWnd,
				(GetSystemMetrics(SM_CXSCREEN)-sysInf.ScreenW)/2,(GetSystemMetrics(SM_CYSCREEN)-sysInf.ScreenH)/2,
				sysInf.ScreenW +GetSystemMetrics(SM_CXDLGFRAME)*2,
				sysInf.ScreenH +GetSystemMetrics(SM_CYDLGFRAME)*2 +GetSystemMetrics(SM_CYBORDER)+GetSystemMetrics(SM_CYSIZE) +GetSystemMetrics(SM_CYMENU),
				TRUE);
		}else{
			MoveWindow(sysInf.hWnd,
				0, 0, sysInf.ScreenW, sysInf.ScreenH, TRUE);
			if(NULL==sysInf.hMenuWnd){
				sysInf.hMenuWnd = CreateWindow(className,"menuWnd",WS_POPUP,
					0,0,
					WIN_SIZEX,GetSystemMetrics(SM_CYMENUSIZE),sysInf.hWnd,sysInf.hMenu,sysInf.hInstance,NULL);
			}
		}
		sysInf.bDisplayChange = FALSE;
		break;
	case WM_PAINT:
		if(lpMovie){
			myOutputDebugString("movie WM_PAINT\n");
			if(lpMovie->forccNum<3){
				BeginPaint( sysInf.hWnd, &ps );
				EndPaint( sysInf.hWnd, &ps );
			}
		}else{
			BeginPaint( sysInf.hWnd, &ps );
			BitBlt( ps.hdc, 0, 0, WIN_SIZEX, WIN_SIZEY, g_DibInf.memDC, 0, 0, SRCCOPY );
			EndPaint( sysInf.hWnd, &ps );
		}
		break;
	case WM_SETCURSOR:
	default:
		break;
    }
	return DefWindowProc(hWnd, message, wParam, lParam);

} // MainWndProc

void ReturnTitile(BOOL bForce)
{
	ESC_InitEOprFlag();
	ESC_InitFlag();
	InitRestartData();
	if(cgmodeWnd && scene_play_mode==cgmodeWnd->GetMode() && FALSE==bForce){
		changeExecMode(scene_play_mode);
	}else{
		my_delete(cgmodeWnd);
		my_delete(titleWnd);
		titleWnd = new TitleWnd;
	}
}

void cleanup()
{
	int		i;
	static	bClear = FALSE;

	if(bClear)return;

	for(i=0;i<g_iDIDeviceNum; i++){
		if (g_pDIDevice[i]) g_pDIDevice[i]->Unacquire();
		RELEASE(g_pDIDevice[i]);
	}
	g_iDIDeviceNum = 0;
	RELEASE(lpDInput);
	my_delete(titleWnd);
	my_delete(lpMovie);
	my_delete(lpSoundDS);
	my_delete(readFile);
	my_delete(saveWnd);
	my_delete(optionWnd);
	my_delete(musicWnd);
	my_delete(cgmodeWnd);
	my_delete(rudderWnd);
	msgWnd.Release();
	bgInf.Release();
	backColorBuf.Release();
	bak_backColorBuf.Release();
	ptnFadeBuf.Release();
	noisePatternBuf.Release();
	selectWnd.Release();
	for(i=0;i<STCHAR_MAX;i++)charInf[i].Release();
	for(i=0;i<STBMP_MAX;i++)setBmpInf[i].Release();
	g_DibInf.Release();
	if(sysInf.configHF){
		CloseHandle(sysInf.configHF);
		sysInf.configHF = NULL;
	}
	cl_free(LangData.LangBuf);
	if(sysInf.full_screen) ScreenChange(0,0);
	cl_free(DevModeStruct);
	bClear = TRUE;
} // cleanup
