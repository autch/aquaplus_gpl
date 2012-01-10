#include "ClCommon.h"
#include "escript.h"
#include "soundDS.h"
#include <commctrl.h>

void UpdateMyPop(HWND hWnd);
void CreateMyProp(HWND hWnd);

extern int				DispFreqMax;

void SetMyMenuItem()
{
	if(CheckOSw2k()){
		int		i=0, j=0;
		char	buf[32];
		DEVMODE		*DevMode = CheckScreenMode();
		BYTE		bpp = (sysInf.bUseBPP24)?24:32;
		HMENU			hsubmenu = CreatePopupMenu();

		DeleteMenu( GetSubMenu(sysInf.hMenu,1), ID_DISPFULL, MF_BYCOMMAND );
		InsertMenu( GetSubMenu(sysInf.hMenu,1), ID_DISPWINDOW, MF_BYCOMMAND|MF_STRING|MF_POPUP, (unsigned int)hsubmenu, "フルスクリーンモード" );

		while(1){
			if(!DevMode[i].dmBitsPerPel) break;
			if( DevMode[i].dmBitsPerPel==bpp && DevMode[i].dmPelsWidth==WIN_SIZEX && DevMode[i].dmPelsHeight==WIN_SIZEY ){
				if(DevMode[i].dmDisplayFrequency>=60) {
					if(DevMode[i].dmDisplayFrequency==60){
						wsprintf( buf, "%d hz (標準)", DevMode[i].dmDisplayFrequency );
					}else{
						wsprintf( buf, "%d hz", DevMode[i].dmDisplayFrequency );
					}
					DispFreq[j] = DevMode[i].dmDisplayFrequency;
					InsertMenu( hsubmenu, -1, MF_BYPOSITION|MF_STRING, ID_SUBMENU+j++, buf );
				}
			}
			i++;
		}
		DispFreqMax = j;

		if(sysInf.full_screen){
			CheckMenuItem( sysInf.hMenu, ID_SUBMENU+sysInf.full_screen-1,  MF_CHECKED );
			EnableMenuItem( sysInf.hMenu, ID_SUBMENU+sysInf.full_screen-1, MF_GRAYED );
		}else{
			CheckMenuItem( sysInf.hMenu, ID_DISPWINDOW,   MF_CHECKED );
			EnableMenuItem( sysInf.hMenu, ID_DISPWINDOW, MF_GRAYED );
		}
	}else{
		if(sysInf.full_screen){
			CheckMenuItem( sysInf.hMenu, ID_DISPFULL,   MF_CHECKED );
			EnableMenuItem( sysInf.hMenu, ID_DISPFULL,  MF_GRAYED  );
		}else{
			CheckMenuItem( sysInf.hMenu, ID_DISPWINDOW,  MF_CHECKED );
			EnableMenuItem( sysInf.hMenu, ID_DISPWINDOW, MF_GRAYED );
		}
	}
} // SetMyMenuItem

HFONT       hFontLink;
WNDPROC     oldLinkProc=NULL;
LRESULT CALLBACK linkProc(HWND hWnd,UINT uMessage,WPARAM wParam,LPARAM lParam)
{
	switch(uMessage){
		case WM_SETCURSOR:
			SetCursor(sysInf.hCurLink);
			return 0;
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC         hdc;
			char        szTextTmp[100];
	        
			GetWindowText(hWnd,szTextTmp,100);
			hdc=BeginPaint(hWnd,&ps);
			SelectObject(hdc,hFontLink);
			SetTextColor(hdc,RGB(0,0,255));
			SetBkColor(hdc,GetSysColor(COLOR_BTNFACE));
			SetBkMode(hdc,OPAQUE);
			TextOut(hdc,0,0,szTextTmp,lstrlen(szTextTmp));
			EndPaint(hWnd,&ps);
			return 0;
		}
	}
	return CallWindowProc(oldLinkProc,hWnd,uMessage,wParam,lParam);
} // linkProc

BOOL CALLBACK AboutDialogBoxProc( HWND hWnd, UINT wmes, UINT wparam, LONG lparam )
{
	switch( wmes ){
	  case WM_INITDIALOG:
	  {
		LOGFONT logfont;
		hFontLink=(HFONT)SendMessage(GetDlgItem(hWnd,IDC_WWW),WM_GETFONT,0,0);
		GetObject(hFontLink,sizeof(logfont),&logfont);
		logfont.lfUnderline=1;
		hFontLink=CreateFontIndirect(&logfont);
		SendDlgItemMessage(hWnd,IDC_WWW,WM_SETFONT,(WPARAM)hFontLink,0);
		oldLinkProc = (WNDPROC)GetWindowLong(GetDlgItem(hWnd,IDC_WWW),GWL_WNDPROC);
		SetWindowLong(GetDlgItem(hWnd,IDC_WWW),GWL_WNDPROC,(LONG)linkProc);
		return TRUE;
	  }
	  case WM_COMMAND:
		switch( LOWORD( wparam ) ){
			case IDOK:
				EndDialog( hWnd, TRUE );
				break;
			case IDC_WWW:
				if(HIWORD(wparam) == STN_CLICKED)
				{
					char szTextTmp[100];
					GetDlgItemText(hWnd,LOWORD(wparam),szTextTmp,100);
					ShellExecute(hWnd,NULL,szTextTmp,NULL,NULL,SW_SHOWNORMAL);
				}
				break;
		}
		break;
	  case WM_CLOSE:
		EndDialog( hWnd, TRUE );
		break;
	  case WM_DESTROY:
		DeleteObject(hFontLink);
		break;
	}
	return 0;
} // AboutDialogBoxProc


HWND	hPage[4];
HWND	hTabWnd;
BOOL CALLBACK ConfDialogBoxProc( HWND hWnd, UINT wmes, UINT wparam, LONG lparam )
{
	int			i;
	switch( wmes ){
	  case WM_INITDIALOG:
		CreateMyProp(hWnd);
		GetClientRect(hWnd, &rect);
		SendMessage(hWnd, WM_SIZE, 0, MAKELPARAM(rect.right, rect.bottom-40));
		break;
	  case WM_SIZE:
		GetClientRect(hWnd, &rect);
		rect.bottom -= 40;
		TabCtrl_AdjustRect(hTabWnd, FALSE, &rect);
		MoveWindow(hTabWnd, 0, 0, LOWORD(lparam), HIWORD(lparam), TRUE);
		for(i=0;i<4;i++){
			MoveWindow(hPage[i],
				rect.left, rect.top,
				rect.right - rect.left,
				rect.bottom - rect.top,
				TRUE);
		}
		break;
	  case WM_NOTIFY:
		switch(TabCtrl_GetCurSel(((NMHDR *)lparam)->hwndFrom)) {
		  case 0:
			ShowWindow(hPage[0], SW_SHOW);
			ShowWindow(hPage[1], SW_HIDE);
			ShowWindow(hPage[2], SW_HIDE);
			break;
		  case 1:
			ShowWindow(hPage[1], SW_SHOW);
			ShowWindow(hPage[0], SW_HIDE);
			ShowWindow(hPage[2], SW_HIDE);
			break;
		  case 2:
			ShowWindow(hPage[2], SW_SHOW);
			ShowWindow(hPage[0], SW_HIDE);
			ShowWindow(hPage[1], SW_HIDE);
			break;
		}
		break;
	  case WM_COMMAND:
		switch( LOWORD( wparam ) ){
		  case IDC_CFGUPDATE:
			UpdateMyPop(hWnd);
			break;
		  case IDOK:
			UpdateMyPop(hWnd);
			EndDialog( hWnd, IDOK );
			break;
		  case IDCANCEL:
			EndDialog( hWnd, IDCANCEL);
			break;
		}
		break;
	  case WM_CLOSE:
		EndDialog( hWnd, TRUE );
		break;
	}
	return 0;
} // ConfDialogBoxProc

LRESULT CALLBACK MyDlgMSGProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch(msg) {
	  case WM_INITDIALOG:
		SendMessage(GetDlgItem(hWnd, IDC_MSGSPEED),    TBM_SETRANGE, TRUE, MAKELPARAM(0,10));
		SendMessage(GetDlgItem(hWnd, IDC_MSGSPEED),    TBM_SETPOS,   TRUE, sysInf.msgSpeed);
		SendMessage(GetDlgItem(hWnd, IDC_AUTOMSGSPEED),TBM_SETRANGE, TRUE, MAKELPARAM(0,10));
		SendMessage(GetDlgItem(hWnd, IDC_AUTOMSGSPEED),TBM_SETPOS,   TRUE, sysInf.autoMsg);
		if(sysInf.bAllSkip)CheckDlgButton(hWnd,IDC_SKIPENABLE,BST_CHECKED);
		if(sysInf.bGEffectSkip)CheckDlgButton(hWnd,IDC_SKIPEFFECT,BST_CHECKED);
		return TRUE;
	  case WM_HSCROLL:
		EnableWindow(GetDlgItem(GetParent(hWnd),IDC_CFGUPDATE),TRUE);
		break;
	  case WM_COMMAND:
		switch( LOWORD( wp ) ){
		  case IDC_SKIPENABLE:
		  case IDC_SKIPEFFECT:
			EnableWindow(GetDlgItem(GetParent(hWnd),IDC_CFGUPDATE),TRUE);
			break;
		}
		break;
	}
	return FALSE;
} // MyDlgMSGProc

LRESULT CALLBACK MyDlgVolumeProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch(msg) {
	  case WM_INITDIALOG:
		SendMessage(GetDlgItem(hWnd, IDC_SLIDERBGM),  TBM_SETRANGE, TRUE, MAKELPARAM(0,10));
		SendMessage(GetDlgItem(hWnd, IDC_SLIDERBGM),  TBM_SETPOS,   TRUE, sysInf.bgmVolume);
		SendMessage(GetDlgItem(hWnd, IDC_SLIDERVOICE),TBM_SETRANGE, TRUE, MAKELPARAM(0,10));
		SendMessage(GetDlgItem(hWnd, IDC_SLIDERVOICE),TBM_SETPOS,   TRUE, sysInf.voiceVolume);
		SendMessage(GetDlgItem(hWnd, IDC_SLIDERSE),   TBM_SETRANGE, TRUE, MAKELPARAM(0,10));
		SendMessage(GetDlgItem(hWnd, IDC_SLIDERSE),   TBM_SETPOS,   TRUE, sysInf.seVolume);
		return TRUE;
	  case WM_HSCROLL:
		EnableWindow(GetDlgItem(GetParent(hWnd),IDC_CFGUPDATE),TRUE);
		break;
	}
	return FALSE;
} // MyDlgVolumeProc

BOOL CALLBACK InfoDialogBoxProc( HWND hWnd, UINT wmes, UINT wparam, LONG lparam )
{
	int		i;
	switch( wmes ){
	  case WM_INITDIALOG:
		{
			char	buf[1024];
			int		device=0;
			CheckCPU();
			buf[0]='\0';
			strcatfn( buf, "VenderID %s", szVenderID );
			strcatfn( buf, "TypeID %d, FamilyID %d, ModelID %d, SteppingID %d", TypeID, FamilyID, ModelID, SteppingID);
			strcatfn( buf, "%s", szCpuName );
			strcatfn( buf, "CpuClock実測値 %dMHz", fCpuClock );

			strcatfn( buf, "MMX  %s", (MMX_Flag)?"有":"無" );
			strcatfn( buf, "SSE  %s", (SSE_FP_Flag)?"有":"無" );
			strcatfn( buf, "SSE2  %s", (SSE2_Flag)?"有":"無" );
			strcatfn( buf, "3DNow!  %s", (_3DNow_Flag)?"有":"無" );
			strcatfn( buf, "e3DNow!  %s", (_3DNowExt_Flag)?"有":"無" );
			strcatfn( buf, "" );
			
			device = EnumGraphicsDevice(  );
			for( i=0 ; i<device ; i++ ){
				strcatfn( buf, "VideoCard(%d) %s", i+1, GetGraphicsDeviceName(i) );
			}
			device = EnumSoundDevice();
			for( i=0 ; i<device ; i++ ){
				strcatfn( buf, "SoundCard(%d) %s", i+1, GetSoundDeviceName(i) );
			}

			strcatfn( buf, "" );
			strcatfn( buf, "OS %s", GetOSName() );

			SetDlgItemText( hWnd, IDC_EDIT1, buf );

			LOGFONT logfont;
			hFontLink=(HFONT)SendMessage(GetDlgItem(hWnd,IDC_WWW),WM_GETFONT,0,0);
			GetObject(hFontLink,sizeof(logfont),&logfont);
			logfont.lfUnderline=1;
			hFontLink=CreateFontIndirect(&logfont);
			SendDlgItemMessage(hWnd,IDC_WWW,WM_SETFONT,(WPARAM)hFontLink,0);
			oldLinkProc = (WNDPROC)GetWindowLong(GetDlgItem(hWnd,IDC_WWW),GWL_WNDPROC);
			SetWindowLong(GetDlgItem(hWnd,IDC_WWW),GWL_WNDPROC,(LONG)linkProc);
			return TRUE;
		}
		break;
	  case WM_COMMAND:
		switch( LOWORD( wparam ) ){
			case IDC_WWW:
				if(HIWORD(wparam) == STN_CLICKED){
					char szTextTmp[100];
					GetDlgItemText(hWnd,LOWORD(wparam),szTextTmp,100);
					ShellExecute(hWnd,NULL,szTextTmp,NULL,NULL,SW_SHOWNORMAL);
				}
				break;
		}
		break;
	  case WM_DESTROY:
		DeleteObject(hFontLink);
		break;
	}
	return FALSE;
} // InfoDialogBoxProc

void UpdateMyPop(HWND hWnd)
{
	int		i;

	sysInf.msgSpeed    = SendMessage(GetDlgItem(hPage[0], IDC_MSGSPEED  ),TBM_GETPOS, 0, 0);
	sysInf.autoMsg     = SendMessage(GetDlgItem(hPage[0], IDC_AUTOMSGSPEED),TBM_GETPOS, 0, 0);

	sysInf.bgmVolume   = SendMessage(GetDlgItem(hPage[1], IDC_SLIDERBGM  ),TBM_GETPOS, 0, 0);
	sysInf.voiceVolume = SendMessage(GetDlgItem(hPage[1], IDC_SLIDERVOICE),TBM_GETPOS, 0, 0);
	sysInf.seVolume    = SendMessage(GetDlgItem(hPage[1], IDC_SLIDERSE   ),TBM_GETPOS, 0, 0);
	sysInf.bAllSkip    = (IsDlgButtonChecked(hPage[0],IDC_SKIPENABLE)==BST_CHECKED)?on:off;
	sysInf.bGEffectSkip= (IsDlgButtonChecked(hPage[0],IDC_SKIPEFFECT)==BST_CHECKED)?on:off;
	EnableWindow(GetDlgItem(hWnd,IDC_CFGUPDATE),FALSE);
	EnableWindow(GetDlgItem(hWnd,IDCANCEL),FALSE);
	SetDlgItemText(hWnd,IDOK,"閉じる");
	if(bgmHandle>=0)lpSoundDS->SetVolume(bgmHandle, sysInf.bgmVolume*0xff/10);
} // UpdateMyPop

void CreateMyProp(HWND hWnd)
{
    TC_ITEM tc[3];

	hTabWnd = GetDlgItem(hWnd, IDC_CONFIGTAB);
	tc[0].mask = TCIF_TEXT;
    tc[0].pszText = "メッセージ";
	TabCtrl_InsertItem( hTabWnd, 0, &tc[0] );
	tc[1].mask = TCIF_TEXT;
    tc[1].pszText = "音量";
	TabCtrl_InsertItem( hTabWnd, 1, &tc[1] );
	tc[2].mask = TCIF_TEXT;
    tc[2].pszText = "システム情報";
	TabCtrl_InsertItem( hTabWnd, 3, &tc[2] );

	hPage[0] = CreateDialog( sysInf.hInstance, MAKEINTRESOURCE(IDD_CONFIG1), hWnd, (DLGPROC)MyDlgMSGProc);
	hPage[1] = CreateDialog( sysInf.hInstance, MAKEINTRESOURCE(IDD_CONFIG2), hWnd, (DLGPROC)MyDlgVolumeProc);
	hPage[2] = CreateDialog( sysInf.hInstance, MAKEINTRESOURCE(IDD_INFODIALOG), hWnd, (DLGPROC)InfoDialogBoxProc);

    ShowWindow(hPage[0], SW_SHOW);
    ShowWindow(hPage[1], SW_HIDE);
    ShowWindow(hPage[2], SW_HIDE);
	return;
} // CreateMyProp

char	myPath[_MAX_PATH];
void readProfile()
{
	char	privateFile[_MAX_PATH];
	int		i;

	GetModuleFileName(sysInf.hInstance,myPath,sizeof(myPath));
	for(i=lstrlen(myPath)-1;i>0;i--){
		if(myPath[i] == '\\'){
			myPath[i] = '\0';
			break;
		}
	}
	lstrcpy(privateFile,myPath);
	lstrcat(privateFile,"\\FanDisc4.INI");
	sysInf.bgmVolume = MaxMin(10, GetPrivateProfileInt("System","BGM volume",10,privateFile));
	sysInf.voiceVolume = MaxMin(10, GetPrivateProfileInt("System","VOICE volume",10,privateFile));
	sysInf.seVolume = MaxMin(10, GetPrivateProfileInt("System","SE volume",10,privateFile));
	sysInf.full_screen = MaxMin(1, GetPrivateProfileInt("System","Screen mode",0,privateFile));
	sysInf.msgSpeed = MaxMin(10, GetPrivateProfileInt("System","Message speed",6,privateFile));
	sysInf.autoMsg = MaxMin(10, GetPrivateProfileInt("System","Auto message",5,privateFile));
	sysInf.bAutoRead = MaxMin(1, GetPrivateProfileInt("System","Auto Read",0,privateFile));
	sysInf.bAllSkip = MaxMin(1, GetPrivateProfileInt("System","All Skip",1,privateFile));
	sysInf.bGEffectSkip = MaxMin(1, GetPrivateProfileInt("System","Graphic Skip",1,privateFile));
	sysInf.bOPMessage = MaxMin(1, GetPrivateProfileInt("System","OP Message",0,privateFile));
	sysInf.refreshRate = 0;
	if(sysInf.full_screen){
		char	workBuf[512];
		sysInf.refreshRate = 0;
		if(CheckOSw2k()){
			sysInf.refreshRate = GetPrivateProfileInt("System","Refresh rate",0,privateFile);
		}
		if(sysInf.refreshRate>0){
			wsprintf(workBuf,"前回の画面設定：\nフルスクリーン，リフレッシュレート：%dHzで起動します。\nよろしいですか？",sysInf.refreshRate);
		}else{
			strcpy(workBuf,"前回の画面設定：\nフルスクリーン，リフレッシュレート：標準で起動します。\nよろしいですか？");
		}
		if(IDNO==MessageBox(NULL,workBuf,"画面切り替え確認",MB_YESNO|MB_ICONQUESTION)){
			sysInf.full_screen = 0;
			sysInf.refreshRate = 0;
		}
	}
} // readProfile

void writeProfile()
{
	char	privateFile[_MAX_PATH];
	char	str[256],str2[256];

	lstrcpy(privateFile,myPath);
	lstrcat(privateFile,"\\FanDisc4.INI");
	_itoa( sysInf.bgmVolume, str, 10 );
	WritePrivateProfileString("System","BGM volume",str,privateFile);
	_itoa( sysInf.voiceVolume, str, 10 );
	WritePrivateProfileString("System","VOICE volume",str,privateFile);
	_itoa( sysInf.seVolume, str, 10 );
	WritePrivateProfileString("System","SE volume",str,privateFile);
	_itoa( sysInf.full_screen, str, 10 );
	WritePrivateProfileString("System","Screen mode",str,privateFile);
	_itoa( sysInf.refreshRate, str, 10 );
	WritePrivateProfileString("System","Refresh rate",str,privateFile);
	_itoa( sysInf.msgSpeed, str, 10 );
	WritePrivateProfileString("System","Message speed",str,privateFile);
	_itoa( sysInf.autoMsg, str, 10 );
	WritePrivateProfileString("System","Auto message",str,privateFile);
	_itoa( sysInf.bAutoRead, str, 10 );
	WritePrivateProfileString("System","Auto Read",str,privateFile);
	_itoa( sysInf.bAllSkip, str, 10 );
	WritePrivateProfileString("System","All Skip",str,privateFile);
	_itoa( sysInf.bGEffectSkip, str, 10 );
	WritePrivateProfileString("System","Graphic Skip",str,privateFile);
	_itoa( LangData.reg[3], str, 10 );
	WritePrivateProfileString("System","OP Message",str,privateFile);
} // writeProfile
