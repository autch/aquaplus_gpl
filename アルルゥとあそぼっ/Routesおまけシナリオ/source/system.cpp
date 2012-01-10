#include "ClCommon.h"
#include "backlog.h"
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

BOOL initApplication( HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE hInstance,int nCmdShow);
LRESULT APIENTRY MainWndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
void SystemInit(void);
void readProfile(void);
void writeProfile(void);
void cleanup(void);
void mainLoop(void);

void SetMyMenuItem(void);
BOOL CALLBACK AboutDialogBoxProc( HWND hwnd, UINT wmes, UINT wparam, LONG lparam );
BOOL CALLBACK ConfDialogBoxProc( HWND hwnd, UINT wmes, UINT wparam, LONG lparam );

BOOL OpenPackFile()
{
	int			i,ret;

	char		packfileName[pack_tail][28] =
	{	"BGMFILE",  "SOUNDDS","GPARTS", "EVENTCG","SCRIPT"};

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

void SystemInit()
{
	HDC		hDC;
	BITMAPINFOHEADER	bmpInfoHead;

	CreateAlphaTable();
	ZeroMemory(&bmpInfoHead,sizeof(BITMAPINFOHEADER));
	bmpInfoHead.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfoHead.biWidth = WIN_SIZEX;				
	bmpInfoHead.biHeight = -WIN_SIZEY;
	bmpInfoHead.biPlanes = 1;
    bmpInfoHead.biBitCount = 24;
    bmpInfoHead.biCompression = BI_RGB;
	bmpInfoHead.biSizeImage = WIN_SIZEX * WIN_SIZEY *3;
	hDC = GetDC(sysInf.hWnd);
	g_DibInf.memDC = CreateCompatibleDC(hDC);
	g_DibInf.hBmp = CreateDIBSection(hDC,(BITMAPINFO *)&bmpInfoHead,DIB_RGB_COLORS,
					(void **)&g_DibInf.colorBuf.pBuf,NULL,NULL);
	g_DibInf.oldBmp = (HBITMAP)SelectObject(g_DibInf.memDC,g_DibInf.hBmp);
	ReleaseDC(sysInf.hWnd,hDC);
	ZeroMemory(g_DibInf.colorBuf.pBuf, WIN_SIZEX*WIN_SIZEY*3);
	g_DibInf.colorBuf.width = WIN_SIZEX;
	g_DibInf.colorBuf.height = WIN_SIZEY;
	g_DibInf.colorBuf.bpp = 24;
	g_DibInf.colorBuf.wPitch = WIN_SIZEX *3;

	backColorBuf.createColorBuf(WIN_SIZEX+80,WIN_SIZEY+60,24);
	bak_backColorBuf.createColorBuf(WIN_SIZEX+80,WIN_SIZEY+60,24);
	bgInf.colorBuf.loadLGF(pack_eventcg,"s00000",Type_back);
	msgWnd.InitMessageParts();
	sideWnd.Create();
	InitDirectInput(sysInf.hInstance);
	lpSoundDS = new ClSoundDS(sysInf.hWnd,readFile,FALSE);

	if(sysInf.bAutoRead)CheckMenuItem( sysInf.hMenu, ID_AUTOREAD,   MF_CHECKED );
#ifdef _DEBUG
	sysInf.bInfo = TRUE;
#endif
	InitReadFlag();
	lpMovie = new CMovie;
	lpMovie->OpenMovie("leaflogo.avi",1);
} // SystemInit


int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance, LPSTR lpCmdLine,int nCmdShow )
{
    MSG				msg;

	if( !initApplication(hInstance) ) return FALSE;

	HANDLE hMutex = CreateMutex(NULL,NULL,className);
	if(ERROR_ALREADY_EXISTS == GetLastError())return FALSE;

	HANDLE	hAccel = LoadAccelerators( hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR) );

	CoInitialize(NULL);
	readProfile();
	if( !OpenPackFile() ) return FALSE;
	if( !InitInstance(hInstance,nCmdShow) ) return FALSE;
	SystemInit();
	WINNLSEnableIME(sysInf.hWnd,FALSE);	
	TIMECAPS tc;
	timeGetDevCaps(&tc,sizeof(tc));
	timeBeginPeriod(tc.wPeriodMin);

	while(FALSE==sysInf.bGameEnd){
		BOOL bGotMsg;

		if(sysInf.bIsActive || lpMovie || lpStaffRoll){
			bGotMsg = PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE);
		}else{
            bGotMsg = GetMessage( &msg, NULL, 0U, 0U );
		}
		if(bGotMsg){
			if(TranslateAccelerator( sysInf.hWnd, (HACCEL)hAccel, &msg ))continue;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if(sysInf.bIsActive || lpMovie || lpStaffRoll){
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
	CheckScreenMode_BPP24();
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
			0,className,"Routes EXTRA MISSION",
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
			0,className,"Routes EXTRA MISSION",
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
		if(IDYES==MessageBox(sysInf.hWnd,"Routesミニシナリオを終了してよろしいですか？","終了確認",MB_YESNO)){
			sysInf.bGameEnd = on;
		}
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
		myOutputDebugString("ActivateHandle: ");
		if(sysInf.bIsActive)myOutputDebugString("Active\n");
		else myOutputDebugString("Non Active\n");
		if(sysInf.bIsActive)clearPadState();
		break;
	case WM_COMMAND:
		switch( LOWORD(wParam) ){
		  case ID_MYAPP_EXIT:
			if(IDYES==MessageBox(sysInf.hWnd,"Routesミニシナリオを終了してよろしいですか？","終了確認",MB_YESNO)){
				sysInf.bGameEnd = on;
			}
			return 0;
		  case ID_DISPCHANGE:
			if(0==sysInf.full_screen){
				if(CheckOSw2k()){
					ScreenChange( 1 ,TRUE);
					for(i=0;i<DispFreqMax;i++){
						EnableMenuItem( sysInf.hMenu, ID_SUBMENU+i,  MF_ENABLED  );
						CheckMenuItem( sysInf.hMenu, ID_SUBMENU+i,   MF_UNCHECKED );
					}
					EnableMenuItem( sysInf.hMenu, ID_SUBMENU,  MF_GRAYED  );
					CheckMenuItem( sysInf.hMenu, ID_SUBMENU,   MF_CHECKED );
					EnableMenuItem( sysInf.hMenu, ID_DISPWINDOW, MF_ENABLED );
					CheckMenuItem( sysInf.hMenu, ID_DISPWINDOW,  MF_UNCHECKED );
				}else{
					ScreenChange(1,0);
					EnableMenuItem( sysInf.hMenu,ID_DISPFULL,   MF_GRAYED  );
					CheckMenuItem( sysInf.hMenu, ID_DISPFULL,   MF_CHECKED );	
					EnableMenuItem( sysInf.hMenu,ID_DISPWINDOW, MF_ENABLED );
					CheckMenuItem( sysInf.hMenu, ID_DISPWINDOW, MF_UNCHECKED );
				}
			}else{
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
			}
			break;
		  case ID_DISPFULL:
			ScreenChange(1,0);
			EnableMenuItem( sysInf.hMenu,ID_DISPFULL,   MF_GRAYED  );
			CheckMenuItem( sysInf.hMenu, ID_DISPFULL,   MF_CHECKED );	
			EnableMenuItem( sysInf.hMenu,ID_DISPWINDOW, MF_ENABLED );
			CheckMenuItem( sysInf.hMenu, ID_DISPWINDOW, MF_UNCHECKED );
			break;
		  case ID_DISPWINDOW:
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
			sysInf.bAutoRead = !sysInf.bAutoRead;
			if(sysInf.bAutoRead){
				CheckMenuItem( sysInf.hMenu, ID_AUTOREAD,   MF_CHECKED );
			}else{
				CheckMenuItem( sysInf.hMenu, ID_AUTOREAD,   MF_UNCHECKED );
			}
			break;
		  case ID_READSKIP:	
			sysInf.bReadSkip = !sysInf.bReadSkip;
			if(sysInf.bReadSkip){
				CheckMenuItem( sysInf.hMenu, ID_READSKIP,   MF_CHECKED );
				sysInf.bAlreadyRead = AVG_CheckScenarioFlag();
			}else{
				CheckMenuItem( sysInf.hMenu, ID_READSKIP,   MF_UNCHECKED );
			}
			break;
		  case ID_UPDATE:
			ShellExecute( sysInf.hWnd, NULL, "http://leaf.aquaplus.co.jp", NULL, NULL, SW_SHOWNORMAL );
			break;
		  case ID_VER:
			DialogBox( sysInf.hInstance, MAKEINTRESOURCE(IDD_ABOUTDIALOG), sysInf.hWnd, AboutDialogBoxProc );
			break;
		  case ID_CONFIG:
			DialogBox( sysInf.hInstance, MAKEINTRESOURCE(IDD_CONFIGDIALOG), sysInf.hWnd, ConfDialogBoxProc );
			break;
		  case ID_LOADEXEC:
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
			if(NULL==EXEC_LangBuf)break;
			switch(sysInf.execMode){
			  case event_exec_mode: case select_exec_mode:
				if(NULL==saveWnd)saveWnd = new SaveWnd(save_mode);
				break;
			}
			break;
		  case ID_BACKTITLE:
			backLog.Init();
			ESC_InitEOprFlag();
			selectWnd.selectEnd();
			my_delete(lpCGmodeWnd);
			my_delete(lpStaffRoll);
			titleWnd = new TitleWnd;
			break;
		  case ID_BACKSELECT:
			SAV_Load( 98 );
			break;
		}
		break;
	case WM_SYSCOMMAND:
		if(sysInf.full_screen){
			switch (wParam) {
				case SC_KEYMENU :
					return 0;
			}
		}
		break;
	case WM_DISPLAYCHANGE:
		if(!sysInf.bDisplayChange)break;
		SetTimer(sysInf.hWnd,1,500,NULL);
		break;
	case WM_TIMER:
		if(!sysInf.bDisplayChange || 1!=wParam)break;
		KillTimer(sysInf.hWnd,1);
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
		if(lpMovie)break;
		BeginPaint( sysInf.hWnd, &ps );
		BitBlt( ps.hdc, 0, 0, WIN_SIZEX, WIN_SIZEY, g_DibInf.memDC, 0, 0, SRCCOPY );
		EndPaint( sysInf.hWnd, &ps );
		break;
	case WM_SETCURSOR:
	default:
		break;
    }
	return DefWindowProc(hWnd, message, wParam, lParam);

} // MainWndProc

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
	my_delete(lpCGmodeWnd);
	my_delete(lpStaffRoll);
	my_delete(lpMovie);
	my_delete(lpSoundDS);
	my_delete(readFile);
	my_delete(saveWnd);
	msgWnd.Release();
	bgInf.Release();
	backColorBuf.Release();
	bak_backColorBuf.Release();
	ptnFadeBuf.Release();
	selectWnd.Release();
	for(i=0;i<STCHAR_MAX;i++)charInf[i].Release();
	for(i=0;i<STBMP_MAX;i++)setBmpInf[i].Release();
	g_DibInf.Release();
	if(sysInf.configHF){
		CloseHandle(sysInf.configHF);
		sysInf.configHF = NULL;
	}
	if(sysInf.full_screen) ScreenChange(0,0);
	cl_free(DevModeStruct);
	bClear = TRUE;
} // cleanup
