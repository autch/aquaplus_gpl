#include "AVGYMZ.h"
#include "soundDS.h"
#include "escript.h"
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
				if(DevMode[i].dmDisplayFrequency>=60 && DevMode[i].dmDisplayFrequency<100) {
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
			ShowWindow(hPage[3], SW_HIDE);
			break;
		  case 1:
			ShowWindow(hPage[1], SW_SHOW);
			ShowWindow(hPage[0], SW_HIDE);
			ShowWindow(hPage[2], SW_HIDE);
			ShowWindow(hPage[3], SW_HIDE);
			break;
		  case 2:
			ShowWindow(hPage[2], SW_SHOW);
			ShowWindow(hPage[0], SW_HIDE);
			ShowWindow(hPage[1], SW_HIDE);
			ShowWindow(hPage[3], SW_HIDE);
			break;
		  case 3:
			ShowWindow(hPage[3], SW_SHOW);
			ShowWindow(hPage[0], SW_HIDE);
			ShowWindow(hPage[1], SW_HIDE);
			ShowWindow(hPage[2], SW_HIDE);
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
		if(sysInf.bBackStop)CheckDlgButton(hWnd,IDC_BACKSTOP,BST_CHECKED);
		return TRUE;
	  case WM_HSCROLL:
		EnableWindow(GetDlgItem(GetParent(hWnd),IDC_CFGUPDATE),TRUE);
		break;
	  case WM_COMMAND:
		switch( LOWORD( wp ) ){
		  case IDC_SKIPENABLE:
		  case IDC_SKIPEFFECT:
		  case IDC_BACKSTOP:
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

LRESULT CALLBACK MyDlgVoiceProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	int		i;

	switch(msg) {
	  case WM_INITDIALOG:
		CheckRadioButton(hWnd,IDC_VSELECT1,IDC_VSELECT1+3,IDC_VSELECT1+sysInf.voiceSelect);
		for(i=0;i<13;i++){
			if(sysInf.voiceFlag & (1<<i))
				CheckDlgButton(hWnd,IDC_VCHECK1+i,BST_CHECKED);
		}
		return TRUE;
	  case WM_COMMAND:
		for(i=0;i<4;i++){
			if(LOWORD( wp ) == IDC_VSELECT1+i){
				switch(i){
				  case 0:	
					for(i=0;i<11;i++)CheckDlgButton(hWnd,IDC_VCHECK1+i,BST_CHECKED);
					break;
				  case 1:
					for(i=0;i<6;i++)CheckDlgButton(hWnd,IDC_VCHECK1+i,BST_CHECKED);
					CheckDlgButton(hWnd,IDC_VCHECK1+9,BST_CHECKED);
					for(i=6;i<9;i++)CheckDlgButton(hWnd,IDC_VCHECK1+i,BST_UNCHECKED);
					CheckDlgButton(hWnd,IDC_VCHECK1+10,BST_UNCHECKED);
					break;
				  case 2:
					for(i=0;i<11;i++)CheckDlgButton(hWnd,IDC_VCHECK1+i,BST_UNCHECKED);
					break;
				}
				EnableWindow(GetDlgItem(GetParent(hWnd),IDC_CFGUPDATE),TRUE);
				break;
			}
		}
		for(i=0;i<13;i++){
			if(LOWORD( wp ) == IDC_VCHECK1+i){
				if(i<11)CheckRadioButton(hWnd,IDC_VSELECT1,IDC_VSELECT1+3,IDC_VSELECT1+3);
				EnableWindow(GetDlgItem(GetParent(hWnd),IDC_CFGUPDATE),TRUE);
				break;
			}
		}
		break;
	}
	return FALSE;
} // MyDlgVoiceProc

#ifdef _MASTER
BOOL CALLBACK FlagDialogBoxProc( HWND hWnd, UINT wmes, UINT wparam, LONG lparam ){
	EndDialog( hWnd, IDCANCEL );
	return 0;
}
#else

struct FlagName{
	char	name[30];
	BYTE	num;
};

FlagName	flagName[83] = {
	{"_珠美に中華鍋",				1},
	{"_クロスボウ発見",				2},	
	{"_可憐にクロスボウ",			3},
	{"_選択ループ_回数",			4},
	{"_選択ループ_前場所",			5},
	{"_選択ループ_済１",			6},
	{"_選択ループ_済２",			7},
	{"_選択ループ_済３",			8},
	{"_選択ループ_済４",			9},
	{"_選択ループ_済５",			10},
	{"_選択ループ_ワーク１",		11},
	{"_選択ループ_ワーク２",		12},
	{"_選択ループ_ワーク３",		13},
	{"_選択ループ_ワーク４",		14},
	{"_選択ループ_ワーク５",		15},
	{"_選択ループ_22600",			16},
	{"_隔壁封鎖にロープ使用",		17},
	{"_ちはや値",					18},
	{"_拳銃所持者",					19},
	{"_可憐を慰めた",				20},
	{"_船内偵察に同行",				21},
	{"_話す",						22},
	{"_ノートＰＣ起動",				23},
	{"_拳銃の鍵",					24},
	{"_船長のペンダント",			25},
	{"_隠し金庫開く",				26},
	{"_岸田の部屋開けた",			27},
	{"_恵のビデオを観た",			28},
	{"_恵の裏切りを知っている",		29},
	{"_友則を閉じ込めた",			30},	
	{"_手錠使用",					31},	
	{"_手錠を取っている",			32},	
	{"_縫い物を頼んだ",				33},
	{"_24831or24841",				34},	
	{"_24832or24842",				35},	
	{"_ルート_24500",				36},	
	{"_ルート_24600",				37},	
	{"_ルート_25000or27000",		38},	
	{"_ルート_25132",				39},	
	{"_ルート_30810",				40},	
	{"_ルート_31000",				41},	
	{"_選択肢_30600",				42},	
	{"_選択肢_23120",				43},	
	{"_選択肢_24541",				44},	
	{"_選択肢_30848_1",				45},	
	{"_選択肢_32040",				46},	
	{"_選択肢_32123",				47},	
	{"_プラチナリング所持者",		48},	
	{"_選択肢_30200",				49},	
	{"_ルート_32400",				50},	
	{"_拳銃そろった",				51},	
	{"_信号弾使用",					52},
	{"_ペットボトル爆弾の色",		53},	
	{"_手錠持ってた？",				54},	
	{"_ストリング入手",				100},	
	{"_偽マスターキー",				200},
	{"_まち針",						201},
	{"_手錠",						202},
	{"_オイルライター",				203},
	{"_モップ",						204},
	{"_消火器",						205},
	{"_ロープ",						206},
	{"_瞬間接着剤",					207},
	{"_鉄やすりの破片",				208},
	{"_ペティナイフ",				209},
	{"_アイスピック",				210},
	{"_ブラシ",						211},
	{"_金の針",						212},
	{"_火炎瓶",						213},
	{"_拳銃リボルバー",				214},
	{"_拳銃オートマチック",			215},
	{"_クロスボウ",					216},
	{"_ペットボトル爆弾・赤",		217},
	{"_ペットボトル爆弾・青",		218},
	{"_信号弾",						219},
	{"_クラッカー",					220},
	{"_プラチナリング",				221},
	{"_スタンガン",					222},
	{"_黒い剣",						223},
	{"_銃弾",						224},
	{"_セーブロード無効",			252},
	{"_インフォメーション無効",		253},
	{"_Ｈシーン中",					254} };


int		vSel = 0;
BOOL CALLBACK FlagDialogBoxProc( HWND hWnd, UINT wmes, UINT wparam, LONG lparam )
{
	LVCOLUMN	LvColumn;
	LV_DISPINFO	*lvinfo;
	char		szText[45];
	HWND		itemWnd;
	int			i,j,value;
	BOOL		bSuccess;

	switch( wmes ){
	  case WM_INITDIALOG:
		vSel = 0;
		SetDlgItemInt(hWnd, IDC_FLAGVALUE, ESC_FlagBuf[vSel], TRUE);
		itemWnd = GetDlgItem(hWnd, IDC_FLAGLIST);
		ZeroMemory(&LvColumn, sizeof(LvColumn));
		LvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		LvColumn.pszText = szText;
		lstrcpy(szText, "フラグ番号");
		LvColumn.fmt = LVCFMT_LEFT;
		LvColumn.cx = 120;
		ListView_InsertColumn(itemWnd, 0, &LvColumn);
		lstrcpy(szText, "フラグ値");
		LvColumn.cx = 70;
		ListView_InsertColumn(itemWnd, 1, &LvColumn);
		for(i=0;i<256;i++){
			LVITEM item;
			item.mask = LVIF_TEXT | LVIF_PARAM;
			item.iItem = i;
			item.iSubItem = 0;
			itoa(i, szText, 10);
			for(j=0;j<83;j++){
				if(i==flagName[j].num){
					strcat(szText,flagName[j].name);
					break;
				}
			}
			item.pszText = szText;
			item.lParam = 0;
			ListView_InsertItem(itemWnd, &item);
			itoa(ESC_FlagBuf[i], szText, 10);
			ListView_SetItemText(itemWnd, i, 1, szText);

		}
		return TRUE;
	  case WM_NOTIFY:
		if ((int)wparam == IDC_FLAGLIST) {
			lvinfo = (LV_DISPINFO *)lparam;
			switch (lvinfo->hdr.code) {
			  case LVN_ITEMACTIVATE:
				vSel = lvinfo->item.mask;
				wsprintf(szText, "FLAG_%d",vSel);
				SetDlgItemText(hWnd, IDC_FLAGNUM, szText);
				SetDlgItemInt(hWnd, IDC_FLAGVALUE, ESC_FlagBuf[vSel], TRUE);
				break;
			}
		}
		break;
	  case WM_COMMAND:
		switch( LOWORD( wparam ) ){
		  case IDC_FLAGUPDATE:
			value = GetDlgItemInt(hWnd,IDC_FLAGVALUE,&bSuccess,TRUE);
			if(bSuccess){
				ESC_FlagBuf[vSel] = value;
				itoa(value, szText, 10);
				itemWnd = GetDlgItem(hWnd, IDC_FLAGLIST);
				ListView_SetItemText(itemWnd, vSel, 1, szText);
			}
			break;
		  case IDOK:
			value = GetDlgItemInt(hWnd,IDC_FLAGVALUE,&bSuccess,TRUE);
			if(bSuccess){
				ESC_FlagBuf[vSel] = value;
			}
			EndDialog( hWnd, IDOK);
			break;
		}
		break;
	  case WM_CLOSE:
		EndDialog( hWnd, IDCANCEL );
		break;
	}
	return 0;
}
#endif

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
			strcatfn( buf, "SSE3  %s", (SSE3_Flag)?"有":"無" );	
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
		EndDialog( hWnd, IDCANCEL );
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
	if(lpSoundDS){
		lpSoundDS->bgmVolume = sysInf.bgmVolume;
		lpSoundDS->voiceVolume = sysInf.voiceVolume;
		lpSoundDS->seVolume = sysInf.seVolume;
	}
	sysInf.bAllSkip    = (IsDlgButtonChecked(hPage[0],IDC_SKIPENABLE)==BST_CHECKED)?on:off;
	sysInf.bGEffectSkip= (IsDlgButtonChecked(hPage[0],IDC_SKIPEFFECT)==BST_CHECKED)?on:off;
	sysInf.bBackStop = (IsDlgButtonChecked(hPage[0],IDC_BACKSTOP)==BST_CHECKED)?on:off;
	
	for(i=0;i<4;i++){
		if(IsDlgButtonChecked(hPage[2], IDC_VSELECT1+i)==BST_CHECKED){
			sysInf.voiceSelect = i;
			break;
		}
	}
	sysInf.voiceFlag = 0;
	for(i=0;i<13;i++){
		if(IsDlgButtonChecked(hPage[2], IDC_VCHECK1+i)==BST_CHECKED){
			sysInf.voiceFlag |= (1<<i);
		}
	}
	EnableWindow(GetDlgItem(hWnd,IDC_CFGUPDATE),FALSE);
	EnableWindow(GetDlgItem(hWnd,IDCANCEL),FALSE);
	SetDlgItemText(hWnd,IDOK,"閉じる");
	SetVolumeLevelG(bgmHandle,sysInf.bgmVolume);
} // UpdateMyPop

void CreateMyProp(HWND hWnd)
{
    TC_ITEM tc[4];

	hTabWnd = GetDlgItem(hWnd, IDC_CONFIGTAB);
	tc[0].mask = TCIF_TEXT;
    tc[0].pszText = "メッセージ";
	TabCtrl_InsertItem( hTabWnd, 0, &tc[0] );
	tc[1].mask = TCIF_TEXT;
    tc[1].pszText = "音量";
	TabCtrl_InsertItem( hTabWnd, 1, &tc[1] );
	tc[2].mask = TCIF_TEXT;
    tc[2].pszText = "VOICE設定";
	TabCtrl_InsertItem( hTabWnd, 2, &tc[2] );
	tc[3].mask = TCIF_TEXT;
    tc[3].pszText = "システム情報";
	TabCtrl_InsertItem( hTabWnd, 3, &tc[3] );

	hPage[0] = CreateDialog( sysInf.hInstance, MAKEINTRESOURCE(IDD_CONFIG1), hWnd, (DLGPROC)MyDlgMSGProc);
	hPage[1] = CreateDialog( sysInf.hInstance, MAKEINTRESOURCE(IDD_CONFIG2), hWnd, (DLGPROC)MyDlgVolumeProc);
	hPage[2] = CreateDialog( sysInf.hInstance, MAKEINTRESOURCE(IDD_VOICECONFIG), hWnd, (DLGPROC)MyDlgVoiceProc);
	hPage[3] = CreateDialog( sysInf.hInstance, MAKEINTRESOURCE(IDD_INFODIALOG), hWnd, (DLGPROC)InfoDialogBoxProc);

    ShowWindow(hPage[0], SW_SHOW);
    ShowWindow(hPage[1], SW_HIDE);
    ShowWindow(hPage[2], SW_HIDE);
    ShowWindow(hPage[3], SW_HIDE);
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
	lstrcat(privateFile,"\\AVGYMZ.INI");
	sysInf.bgmVolume = MaxMin(10, GetPrivateProfileInt("System","BGM volume",7,privateFile));
	sysInf.voiceVolume = MaxMin(10, GetPrivateProfileInt("System","VOICE volume",8,privateFile));
	sysInf.seVolume = MaxMin(10, GetPrivateProfileInt("System","SE volume",8,privateFile));
	sysInf.full_screen = MaxMin(255, GetPrivateProfileInt("System","Screen mode",0,privateFile));
	sysInf.msgSpeed = MaxMin(10, GetPrivateProfileInt("System","Message speed",5,privateFile));
	sysInf.autoMsg = MaxMin(10, GetPrivateProfileInt("System","Auto message",5,privateFile));
	sysInf.bAutoRead = MaxMin(1, GetPrivateProfileInt("System","Auto Read",0,privateFile));
	sysInf.bAutoRead = 0;
	sysInf.bAllSkip = MaxMin(1, GetPrivateProfileInt("System","All Skip",0,privateFile));
	sysInf.bGEffectSkip = MaxMin(1, GetPrivateProfileInt("System","Graphic Skip",1,privateFile));
	sysInf.bBackStop = MaxMin(1, GetPrivateProfileInt("System","Background Stop",0,privateFile));
	sysInf.bMsgWndCls = MaxMin(1,GetPrivateProfileInt("System","MessageWindow Clear",1,privateFile));
	sysInf.voiceSelect = MaxMin(3, GetPrivateProfileInt("System","Voice Select",0,privateFile));
	sysInf.voiceFlag = MaxMin(0xFff, GetPrivateProfileInt("System","Voice Flag",0x03ff,privateFile));
	sysInf.g_highSpeed = MaxMin(1, GetPrivateProfileInt("System","Graphic Speed",0,privateFile));
	sysInf.g_highSpeed = 0;
	sysInf.movieQuality = MaxMin(3, GetPrivateProfileInt("System","Movie Quality",3,privateFile));
	sysInf.nouseOverlay = MaxMin(2, GetPrivateProfileInt("System","NoUse Overlay",2,privateFile));
	sysInf.msgWndAlpha = MaxMin(100, GetPrivateProfileInt("System","Window Transparent",20,privateFile));
	sysInf.setFPS = MaxMin(2, GetPrivateProfileInt("System","fps",2,privateFile));
#ifndef _MASTER
	sysInf.bInfo = MaxMin(1, GetPrivateProfileInt("System","Debug Mode",0,privateFile));
#else
	sysInf.bInfo = 0;
#endif
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

	if(sysInf.org_msgSpeed != 0xff){
		sysInf.msgSpeed = sysInf.org_msgSpeed;
		sysInf.org_msgSpeed = 0xff;
	}
	lstrcpy(privateFile,myPath);
	lstrcat(privateFile,"\\AVGYMZ.INI");
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
	_itoa( sysInf.bBackStop, str, 10 );
	WritePrivateProfileString("System","Background Stop",str,privateFile);
	_itoa( sysInf.bMsgWndCls, str, 10 );
	WritePrivateProfileString("System","MessageWindow Clear",str,privateFile);
	_itoa( sysInf.voiceSelect, str, 10);
	WritePrivateProfileString("System","Voice Select",str,privateFile);
	_itoa( sysInf.voiceFlag, str2, 16);
	wsprintf(str,"0x%s",str2);
	WritePrivateProfileString("System","Voice Flag",str,privateFile);
	_itoa( sysInf.bInfo, str, 10);
	WritePrivateProfileString("System","Debug Mode",str,privateFile);
	_itoa( sysInf.g_highSpeed, str, 10);
	WritePrivateProfileString("System","Graphic Speed",str,privateFile);
	_itoa( sysInf.movieQuality, str, 10);
	WritePrivateProfileString("System","Movie Quality",str,privateFile);
	_itoa( sysInf.msgWndAlpha, str, 10);
	WritePrivateProfileString("System","Window Transparent",str,privateFile);
	_itoa( sysInf.nouseOverlay, str, 10);
	WritePrivateProfileString("System","NoUse Overlay",str,privateFile);
	_itoa( sysInf.setFPS, str, 10);
	WritePrivateProfileString("System","fps",str,privateFile);
} // writeProfile
