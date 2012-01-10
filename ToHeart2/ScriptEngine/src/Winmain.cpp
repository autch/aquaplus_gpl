
#include "resource.h"
#include "mm_std.h"
#include "main.h"

#include <winnls32.h>         
#include <winuser.h>

#include <enum.h>

#include "main.h"
#include "bmp.h"
#include "draw.h"
#include "palette.h"
#include "disp.h"
#include "keybord.h"
#include "mouse.h"
#include "font.h"
#include "ctrl.h"

#include "movPlayer.h"


#include <soundDS.h>

#include "GM_Avg.h"
#include "GM_Title.h"
#include "escript.h"



#define USE_DD	0

#define DISP_MODE	24

WINDOW_STRUCT	MainWindow;

APP_STRUCT	AppStruct;

static BOOL WIN_CreateVram( int direct_draw, int draw_mode );
static NotInitDirectDraw=0;

CRITICAL_SECTION	MainCriticalSection;	




void strcatfn( char *buf, char *mes, ...)
{
	char	buf2[256];
	int	len = wvsprintf( buf2, mes, (char *)(&mes+1));
	buf2[len]   = 0x0d;	
	buf2[len+1] = 0x0a;
	buf2[len+2] = '\0';
	strcat(buf,buf2);
}

BOOL CALLBACK UsoErrDialogBoxProc( HWND hwnd, UINT wmes, UINT wparam, LONG lparam )
{
	switch( wmes ){
		case WM_CREATE:
		case WM_INITDIALOG:
			STD_SetDialogPos( hwnd, -1, -1 );	
			break;
		case WM_COMMAND:
			switch( LOWORD( wparam ) ){
				case IDOK:
				case IDCANCEL:
					EndDialog( hwnd, TRUE );
					break;
			}
			break;
		case WM_CLOSE:
			EndDialog( hwnd, TRUE );
			break;
	}
	return 0;
}

void WIN_OsoErrDialog(void)
{
	if( STD_CheckOSw2k() ){
		MessageBox2( MainWindow.hwnd, MB_OK|MB_ICONWARNING, "アプリケーション エラー",
			"エラーが発生したため、雫.exe を終了します。プログラムをもう一度開始す\nる必要があります。\n\nエラー ロクを作成しています。" );
	}else{
		DialogBox( AppStruct.inst, MAKEINTRESOURCE(IDD_DIALOG7), MainWindow.hwnd, UsoErrDialogBoxProc );
	}
}
BOOL CALLBACK VolumeControlDialogProc( HWND hwnd, UINT wmes, UINT wparam, LONG lparam )
{
	static int	bgm_param=16;
	static int	se_param=16;
	static int	voice_param=16;

	switch( wmes ){
		case WM_CREATE:
		case WM_INITDIALOG:
			bgm_param = Avg.bgm_vol/16;
			se_param  = Avg.se_vol/16;
			voice_param  = Avg.voice_vol/16;

			SetSliderParamEx( hwnd, IDC_SLIDER1, 0, 16, 1, 1, 1*4, bgm_param );
			SetSliderParamEx( hwnd, IDC_SLIDER2, 0, 16, 1, 1, 1*4, se_param );
			SetSliderParamEx( hwnd, IDC_SLIDER3, 0, 16, 1, 1, 1*4, voice_param );
			break;
		case WM_HSCROLL:
			if( (HWND)lparam == GetDlgItem(hwnd,IDC_SLIDER1) ){
				bgm_param = SliderGetPos(hwnd,IDC_SLIDER1);
				AVG_SetDirectVoumeMusicAll( bgm_param*16 );
			}
			if( (HWND)lparam == GetDlgItem(hwnd,IDC_SLIDER2) ){
				se_param = SliderGetPos(hwnd,IDC_SLIDER2);
				AVG_SetDirectVoumeSeAll( se_param*16 );
			}
			if( (HWND)lparam == GetDlgItem(hwnd,IDC_SLIDER3) ){
				voice_param = SliderGetPos(hwnd,IDC_SLIDER3);
				AVG_SetDirectVoumeVoiceAll( voice_param*16 );
			}
			break;
		case WM_COMMAND:
			switch( LOWORD( wparam ) ){
				case IDOK:
					Avg.bgm_vol = bgm_param*16;
					Avg.se_vol  = se_param*16;
					Avg.voice_vol = voice_param*16;

					EndDialog( hwnd, TRUE );
					break;
				case IDCANCEL:
					EndDialog( hwnd, TRUE );
					break;
				case IDC_TEST:
					AVG_PlaySE( 8025, OFF, 255*se_param/16, TRUE );
					break;
				case IDC_TEST2:
					{
						static int	work;
						work = Avg.voice_vol;
						Avg.voice_vol = voice_param*16;
						AVG_PlayVoice( 0, 1, 10301000, 2, 256, FALSE, 0, 1 );
						Avg.voice_vol = work;
					}
					break;
			}
			break;
		case WM_CLOSE:
			EndDialog( hwnd, TRUE );
			break;
	}
	return 0;
}

BOOL CALLBACK InfoDialogBoxProc( HWND hwnd, UINT wmes, UINT wparam, LONG lparam )
{
	int	i;
	switch( wmes ){
		case WM_CREATE:
		case WM_INITDIALOG:
			{
				char	buf[1024];
				int		device=0;
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
				strcatfn( buf, "OS %s", STD_GetOSName() );

				SetDlgItemText( hwnd, IDC_EDIT1, buf );
			}
			break;
		case WM_COMMAND:
			switch( LOWORD( wparam ) ){
				case IDOK:
					EndDialog( hwnd, TRUE );
					break;
			}
			break;
		case WM_CLOSE:
			EndDialog( hwnd, TRUE );
			break;
	}
	return 0;
}



BOOL CALLBACK VoiceDialogBoxProc( HWND hwnd, UINT wmes, UINT wparam, LONG lparam )
{
	switch( wmes ){
		case WM_CREATE:
		case WM_INITDIALOG:
			SendDlgItemMessage( hwnd, IDC_CHECK1,  BM_SETCHECK, CharVoiceFlag[0], 0 );
			SendDlgItemMessage( hwnd, IDC_CHECK2,  BM_SETCHECK, CharVoiceFlag[1], 0 );
			SendDlgItemMessage( hwnd, IDC_CHECK3,  BM_SETCHECK, CharVoiceFlag[2], 0 );
			SendDlgItemMessage( hwnd, IDC_CHECK4,  BM_SETCHECK, CharVoiceFlag[3], 0 );
			SendDlgItemMessage( hwnd, IDC_CHECK5,  BM_SETCHECK, CharVoiceFlag[4], 0 );
			SendDlgItemMessage( hwnd, IDC_CHECK6,  BM_SETCHECK, CharVoiceFlag[5], 0 );
			SendDlgItemMessage( hwnd, IDC_CHECK7,  BM_SETCHECK, CharVoiceFlag[6], 0 );
			SendDlgItemMessage( hwnd, IDC_CHECK8,  BM_SETCHECK, CharVoiceFlag[7], 0 );
			SendDlgItemMessage( hwnd, IDC_CHECK9,  BM_SETCHECK, CharVoiceFlag[8], 0 );
			SendDlgItemMessage( hwnd, IDC_CHECK10, BM_SETCHECK, CharVoiceFlag[9], 0 );
			SendDlgItemMessage( hwnd, IDC_CHECK11, BM_SETCHECK, CharVoiceFlag[10], 0 );

			if( AVG_GetGameFlag(95) ){
				SetDlgItemText( hwnd, IDC_CHECK9, "優希" );
			}else{
				SetDlgItemText( hwnd, IDC_CHECK9, "？？？" );
			}

			SendDlgItemMessage( hwnd, IDC_CHECK_ALL, BM_SETCHECK, Avg.voice, 0 );

			if(Avg.voice==0){
				EnableWindow( GetDlgItem(hwnd,IDC_CHECK1), OFF );
				EnableWindow( GetDlgItem(hwnd,IDC_CHECK2), OFF );
				EnableWindow( GetDlgItem(hwnd,IDC_CHECK3), OFF );
				EnableWindow( GetDlgItem(hwnd,IDC_CHECK4), OFF );
				EnableWindow( GetDlgItem(hwnd,IDC_CHECK5), OFF );
				EnableWindow( GetDlgItem(hwnd,IDC_CHECK6), OFF );
				EnableWindow( GetDlgItem(hwnd,IDC_CHECK7), OFF );
				EnableWindow( GetDlgItem(hwnd,IDC_CHECK8), OFF );
				EnableWindow( GetDlgItem(hwnd,IDC_CHECK9), OFF );
				EnableWindow( GetDlgItem(hwnd,IDC_CHECK10), OFF );
				EnableWindow( GetDlgItem(hwnd,IDC_CHECK11), OFF );
			}else{
				EnableWindow( GetDlgItem(hwnd,IDC_CHECK1), ON );
				EnableWindow( GetDlgItem(hwnd,IDC_CHECK2), ON );
				EnableWindow( GetDlgItem(hwnd,IDC_CHECK3), ON );
				EnableWindow( GetDlgItem(hwnd,IDC_CHECK4), ON );
				EnableWindow( GetDlgItem(hwnd,IDC_CHECK5), ON );
				EnableWindow( GetDlgItem(hwnd,IDC_CHECK6), ON );
				EnableWindow( GetDlgItem(hwnd,IDC_CHECK7), ON );
				EnableWindow( GetDlgItem(hwnd,IDC_CHECK8), ON );
				EnableWindow( GetDlgItem(hwnd,IDC_CHECK9), ON );
				EnableWindow( GetDlgItem(hwnd,IDC_CHECK10), ON );
				EnableWindow( GetDlgItem(hwnd,IDC_CHECK11), ON );
			}

			break;
		case WM_COMMAND:
			switch( LOWORD( wparam ) ){
				case IDC_CHECK_ALL:
					if( SendDlgItemMessage( hwnd, IDC_CHECK_ALL, BM_GETCHECK, 0, 0)==0 ){
						EnableWindow( GetDlgItem(hwnd,IDC_CHECK1), OFF );
						EnableWindow( GetDlgItem(hwnd,IDC_CHECK2), OFF );
						EnableWindow( GetDlgItem(hwnd,IDC_CHECK3), OFF );
						EnableWindow( GetDlgItem(hwnd,IDC_CHECK4), OFF );
						EnableWindow( GetDlgItem(hwnd,IDC_CHECK5), OFF );
						EnableWindow( GetDlgItem(hwnd,IDC_CHECK6), OFF );
						EnableWindow( GetDlgItem(hwnd,IDC_CHECK7), OFF );
						EnableWindow( GetDlgItem(hwnd,IDC_CHECK8), OFF );
						EnableWindow( GetDlgItem(hwnd,IDC_CHECK9), OFF );
						EnableWindow( GetDlgItem(hwnd,IDC_CHECK10), OFF );
						EnableWindow( GetDlgItem(hwnd,IDC_CHECK11), OFF );
					}else{
						EnableWindow( GetDlgItem(hwnd,IDC_CHECK1), ON );
						EnableWindow( GetDlgItem(hwnd,IDC_CHECK2), ON );
						EnableWindow( GetDlgItem(hwnd,IDC_CHECK3), ON );
						EnableWindow( GetDlgItem(hwnd,IDC_CHECK4), ON );
						EnableWindow( GetDlgItem(hwnd,IDC_CHECK5), ON );
						EnableWindow( GetDlgItem(hwnd,IDC_CHECK6), ON );
						EnableWindow( GetDlgItem(hwnd,IDC_CHECK7), ON );
						EnableWindow( GetDlgItem(hwnd,IDC_CHECK8), ON );
						EnableWindow( GetDlgItem(hwnd,IDC_CHECK9), ON );
						EnableWindow( GetDlgItem(hwnd,IDC_CHECK10), ON );
						EnableWindow( GetDlgItem(hwnd,IDC_CHECK11), ON );
					}
					break;

				case IDCANCEL:
					EndDialog( hwnd, TRUE );
					break;
				case IDOK:
					CharVoiceFlag[0] = SendDlgItemMessage( hwnd, IDC_CHECK1, BM_GETCHECK, 0, 0 );
					CharVoiceFlag[1] = SendDlgItemMessage( hwnd, IDC_CHECK2, BM_GETCHECK, 0, 0 );
					CharVoiceFlag[2] = SendDlgItemMessage( hwnd, IDC_CHECK3, BM_GETCHECK, 0, 0 );
					CharVoiceFlag[3] = SendDlgItemMessage( hwnd, IDC_CHECK4, BM_GETCHECK, 0, 0 );
					CharVoiceFlag[4] = SendDlgItemMessage( hwnd, IDC_CHECK5, BM_GETCHECK, 0, 0 );
					CharVoiceFlag[5] = SendDlgItemMessage( hwnd, IDC_CHECK6, BM_GETCHECK, 0, 0 );
					CharVoiceFlag[6] = SendDlgItemMessage( hwnd, IDC_CHECK7, BM_GETCHECK, 0, 0 );
					CharVoiceFlag[7] = SendDlgItemMessage( hwnd, IDC_CHECK8, BM_GETCHECK, 0, 0 );
					CharVoiceFlag[8] = SendDlgItemMessage( hwnd, IDC_CHECK9, BM_GETCHECK, 0, 0 );
					CharVoiceFlag[9] = SendDlgItemMessage( hwnd, IDC_CHECK10, BM_GETCHECK, 0, 0 );
					CharVoiceFlag[10] = SendDlgItemMessage( hwnd, IDC_CHECK11, BM_GETCHECK, 0, 0 );

					Avg.voice = SendDlgItemMessage( hwnd, IDC_CHECK_ALL, BM_GETCHECK, 0, 0 );
					AVG_SetVoiveFlag( Avg.voice );

					EndDialog( hwnd, TRUE );
					break;
			}
			break;
		case WM_CLOSE:
			EndDialog( hwnd, TRUE );
			break;
	}
	return 0;
}




BOOL CALLBACK DialogBoxProc( HWND hwnd, UINT wmes, UINT wparam, LONG lparam )
{
	switch( wmes ){
		case WM_CREATE:
		case WM_INITDIALOG:
			break;
		case WM_COMMAND:
			switch( LOWORD( wparam ) ){
				case IDOK:
					EndDialog( hwnd, TRUE );
					break;
			}
			break;
		case WM_CLOSE:
			EndDialog( hwnd, TRUE );
			break;
	}
	return 0;
}




#include "escript.h"

BOOL CALLBACK NameDialogBoxProc( HWND hwnd, UINT wmes, UINT wparam, LONG lparam )
{
	int	i;
	switch( wmes ){
		case WM_CREATE:
		case WM_INITDIALOG:
			SetDlgItemText( hwnd, IDC_EDIT1, NameL );
			SetDlgItemText( hwnd, IDC_EDIT2, NameF );
			SetDlgItemText( hwnd, IDC_EDIT3, NameLK );
			SetDlgItemText( hwnd, IDC_EDIT4, NameFK );
			SetDlgItemText( hwnd, IDC_EDIT5, NameNN );
			WINNLSEnableIME(MainWindow.hwnd,TRUE);	
			break;
		case WM_COMMAND:
			switch( LOWORD( wparam ) ){
				case IDC_EDIT1:
					break;
				case IDC_EDIT2:
					break;
				case IDC_EDIT3:
					break;
				case IDC_EDIT4:
					break;
				case IDC_EDIT5:
					break;
				case IDC_RESET_NAME:

					SetDlgItemText( hwnd, IDC_EDIT1, DEF_NAME_L );
					SetDlgItemText( hwnd, IDC_EDIT2, DEF_NAME_F );
					SetDlgItemText( hwnd, IDC_EDIT3, DEF_NAME_LK );
					SetDlgItemText( hwnd, IDC_EDIT4, DEF_NAME_FK );
					SetDlgItemText( hwnd, IDC_EDIT5, DEF_NAME_NN );
					break;
				case IDOK:
					ZeroMemory( NameL, 13 );
					ZeroMemory( NameF, 13 );
					ZeroMemory( NameLK, 13 );
					ZeroMemory( NameFK, 13 );
					ZeroMemory( NameNN, 13 );

					GetDlgItemText( hwnd, IDC_EDIT1, NameL, 13 );
					GetDlgItemText( hwnd, IDC_EDIT2, NameF, 13 );
					GetDlgItemText( hwnd, IDC_EDIT3, NameLK, 13 );
					GetDlgItemText( hwnd, IDC_EDIT4, NameFK, 13 );
					GetDlgItemText( hwnd, IDC_EDIT5, NameNN, 13 );

					if( strlen(NameL)<1 ||
						strlen(NameLK)<1 ||
						strlen(NameF)<1 ||
						strlen(NameFK)<1 ||
						strlen(NameNN)<1 ){ 
						DebugBox( NULL, "名前に未入力の項目があります" );
						break;
					}
					if( strlen(NameL) !=_mbslen((BYTE*)NameL)*2 ||
						strlen(NameLK)!=_mbslen((BYTE*)NameLK)*2 ||
						strlen(NameF) !=_mbslen((BYTE*)NameF)*2 ||
						strlen(NameFK)!=_mbslen((BYTE*)NameFK)*2 ||
						strlen(NameNN)!=_mbslen((BYTE*)NameNN)*2 ){

						DebugBox( hwnd, "名前に半角が含まれています" );
						break;
					}
					

					i=0;
					i += _mbscmp( (BYTE*)NameL,  (BYTE*)DEF_NAME_L );
					i += _mbscmp( (BYTE*)NameLK, (BYTE*)DEF_NAME_LK );
					i += _mbscmp( (BYTE*)NameF,  (BYTE*)DEF_NAME_F );
					i += _mbscmp( (BYTE*)NameFK, (BYTE*)DEF_NAME_FK );
					i += _mbscmp( (BYTE*)NameNN, (BYTE*)DEF_NAME_NN );

					if(i==0){
						DefaultCharName=1;
					}else{
						DefaultCharName=0;
					}
					ESC_SetFlag( _DEFAULT_NAME, DefaultCharName );

					WINNLSEnableIME(MainWindow.hwnd,FALSE);	
					EndDialog( hwnd, TRUE );
					break;
				case IDCANCEL:
					WINNLSEnableIME(MainWindow.hwnd,FALSE);	
					EndDialog( hwnd, TRUE );
					break;
			}
			break;
		case WM_CLOSE:
			WINNLSEnableIME(MainWindow.hwnd,FALSE);	
			EndDialog( hwnd, TRUE );
			break;
	}
	return 0;
}



BOOL CALLBACK AutoModeDialog( HWND hwnd, UINT wmes, UINT wparam, LONG lparam )
{
	static int	auto_key=10;
	static int	auto_page=10;

	switch( wmes ){
		case WM_CREATE:
		case WM_INITDIALOG:
			auto_key = (Avg.auto_key-60)/24;
			auto_page  = (Avg.auto_page-120)/48;

			SetSliderParamEx( hwnd, IDC_SLIDER1, 0, 10, 1, 1, 1*4, auto_key );
			SetSliderParamEx( hwnd, IDC_SLIDER2, 0, 10, 1, 1, 1*4, auto_page );
			break;
		case WM_HSCROLL:
			if( (HWND)lparam == GetDlgItem(hwnd,IDC_SLIDER1) ){
				auto_key = SliderGetPos(hwnd,IDC_SLIDER1);
			}
			if( (HWND)lparam == GetDlgItem(hwnd,IDC_SLIDER2) ){
				auto_page = SliderGetPos(hwnd,IDC_SLIDER2);
			}
			break;
		case WM_COMMAND:
			switch( LOWORD( wparam ) ){
				case IDOK:
					Avg.auto_key   = auto_key*24+60;
					Avg.auto_page  = auto_page*48+120;
					EndDialog( hwnd, TRUE );
					break;
				case IDCANCEL:
					EndDialog( hwnd, TRUE );
					break;
			}
			break;
		case WM_CLOSE:
			EndDialog( hwnd, TRUE );
			break;
	}
	return 0;
}





static HWND	hColorWarning[2];
static long	ReCheckFlag[2];
BOOL CALLBACK ColorWarningDialog( HWND hwnd, UINT wmes, UINT wparam, LONG lparam )
{
	int	ret=0;
	switch( wmes ){
		case WM_CREATE:
		case WM_INITDIALOG:
			hColorWarning[lparam] = hwnd;
			STD_SetDialogPos( hwnd, -1, -1);	
			break;
		case WM_COMMAND:
			switch( LOWORD( wparam ) ){
				case IDOK:
					if(hColorWarning[0]==hwnd)	ReCheckFlag[0] = SendDlgItemMessage(hwnd,IDC_CHECK1,BM_GETCHECK,0,0);
					else						ReCheckFlag[1] = SendDlgItemMessage(hwnd,IDC_CHECK1,BM_GETCHECK,0,0);
					EndDialog( hwnd, TRUE );
					break;
				case IDC_CHECK1:
					break;
			}
			break;
		case WM_CLOSE:
			EndDialog( hwnd, TRUE );
			break;
	}
	return 0;
}

static int			DevModeNum = 0;
static DEVMODE		*DevModeStruct=NULL;



static DEVMODE *CheckScreenMode( void )
{
	DEVMODE		Dummy;
	
	DevModeNum=0;
	while( EnumDisplaySettings(NULL, DevModeNum, &Dummy ) ){
		DevModeNum++;
	}
	GFree(DevModeStruct);
	DevModeStruct = (DEVMODE*)GAlloc( sizeof(DEVMODE)*(DevModeNum+1) );
	DevModeNum=0;
	while( EnumDisplaySettings(NULL, DevModeNum, &DevModeStruct[DevModeNum]) ){
		DevModeNum++;
	}
	return DevModeStruct;
}



static BOOL CheckScreenMode_BPP24( void )
{
	int			iModeNum = 0;
	DEVMODE		DevMode;
	BOOL		ret=FALSE;

	while( EnumDisplaySettings(NULL, iModeNum, &DevMode) )
	{




		iModeNum++;
		if(DevMode.dmBitsPerPel==24){
			ret=TRUE;
		}
	}
	return ret;
}





void SetFullScreenMenu( int menu )
{
	int	mx = GetSystemMetrics(SM_CXDLGFRAME);
	int	my = GetSystemMetrics(SM_CYMENUSIZE)+GetSystemMetrics(SM_CYBORDER)*2
			+GetSystemMetrics(SM_CYSIZE)+GetSystemMetrics(SM_CYDLGFRAME);

	if(menu){
		my -= GetSystemMetrics(SM_CYMENUSIZE)+GetSystemMetrics(SM_CYBORDER);



	}else{



	}
	if(Debug.flag)	SetWindowPos( MainWindow.hwnd, NULL, -mx, -my, 0, 0, SWP_NOSIZE|SWP_NOZORDER );
	else			SetWindowPos( MainWindow.hwnd, HWND_TOPMOST, -mx, -my, 0, 0, SWP_NOSIZE );
	DebugPrintf("SetWindowPos %d : %d\n", -mx, -my );
}




int	WIN_SetScreenMode( int w, int h, int rfrate )
{
	DEVMODE		DevMode;
	int			ret=DISP_CHANGE_SUCCESSFUL;
	int			mx = GetSystemMetrics(SM_CXDLGFRAME);
	int			my = GetSystemMetrics(SM_CYMENUSIZE)+GetSystemMetrics(SM_CYBORDER)*2
					+GetSystemMetrics(SM_CYSIZE)+GetSystemMetrics(SM_CYDLGFRAME);

	if(DirectDrawFlag) return 0;


	if( w<0 || h<0 ){
		long	px,py;
		SetRegistryKey( REG_KEY );
		if( ReadRegistryAmount( "pos_x", &px ) ) px=0;
		if( ReadRegistryAmount( "pos_y", &py ) ) py=0;





		ret = ChangeDisplaySettings( NULL, CDS_FULLSCREEN);
		SetWindowPos( MainWindow.hwnd, HWND_NOTOPMOST, px, py, 0, 0, SWP_NOSIZE );
	}else{


		int		w2,h2;
		long	px,py;
		STD_GetScreenBpp( &w2, &h2 );
		if( !(w==w2 && h==h2) ){
			SetRegistryKey( REG_KEY );
			STD_GetWindowPos( MainWindow.hwnd, (int*)&px, (int*)&py );
			WriteRegistryAmount( "pos_x", px );
			WriteRegistryAmount( "pos_y", py );
		}

		ZeroMemory( &DevMode, sizeof(DEVMODE) );
		DevMode.dmSize = sizeof(DEVMODE);
		if(rfrate==0){
			DevMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;
		}else{
			DevMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL | DM_DISPLAYFREQUENCY;
			DevMode.dmDisplayFrequency = rfrate;
		}
		DevMode.dmPelsWidth  = w;
		DevMode.dmPelsHeight = h;

		if(CheckScreenMode_BPP24())	DevMode.dmBitsPerPel = 24;
		else						DevMode.dmBitsPerPel = 32;

		ret = ChangeDisplaySettings(&DevMode,CDS_FULLSCREEN);

		if(ret!=DISP_CHANGE_SUCCESSFUL){
			ZeroMemory( &DevMode, sizeof(DEVMODE) );
			DevMode.dmSize = sizeof(DEVMODE);
			DevMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;
			DevMode.dmPelsWidth  = w;
			DevMode.dmPelsHeight = h;
			ret = ChangeDisplaySettings(&DevMode,CDS_FULLSCREEN);
		}

		if(Debug.flag)	SetWindowPos( MainWindow.hwnd, NULL, -mx, -my, 0, 0, SWP_NOSIZE|SWP_NOZORDER );
		else			SetWindowPos( MainWindow.hwnd, HWND_TOPMOST, -mx, -my, 0, 0, SWP_NOSIZE );
		DebugPrintf("SetWindowPos %d : %d\n", -mx, -my );
		SetFullScreenMenu( 0 );
	}

	switch( ret ){
		case DISP_CHANGE_SUCCESSFUL:	
			break;
		case DISP_CHANGE_RESTART:		
			DebugBox( NULL, "画面モード変更時、再起動必要" );
			break;
		default:
			DebugBox( NULL, "画面モード変更失敗(原因不明・・・・)" );
			break;
		case DISP_CHANGE_BADFLAGS:		
			DebugBox( NULL, "画面モード変更失敗(フラグミス)" );
			break;
		case DISP_CHANGE_FAILED:		
			DebugBox( NULL, "画面モード変更失敗(ドライバがそのモード変更をサーポートしていません)" );
			break;
		case DISP_CHANGE_BADMODE:		
			DebugBox( NULL, "画面モード変更失敗(そのモードは存在しません)" );
			break;
		case DISP_CHANGE_NOTUPDATED:	
			DebugBox( NULL, "画面モード変更失敗(よーわからん、NT用？)" );
			break;
	}
	return ret;
}



static int DispFreq[16];
static int DispFreqMax=0;
static int DispDef60No=0;

void WIN_InitMenu( void )
{
	HMENU	hmenu = GetMenu( MainWindow.hwnd );

	CheckMenuItem( hmenu, ID_DISP_FULL,     MF_UNCHECKED );
	CheckMenuItem( hmenu, ID_DISP_WINDOW,   MF_UNCHECKED );





	CheckMenuItem( hmenu, ID_EFFECT_DISP,   MF_UNCHECKED );
	CheckMenuItem( hmenu, ID_EFFECT_SPEED,  MF_UNCHECKED );
	CheckMenuItem( hmenu, ID_EFFECT_CUT,    MF_UNCHECKED );
	CheckMenuItem( hmenu, ID_EFFECT_HIGH,   MF_UNCHECKED );
	CheckMenuItem( hmenu, ID_EFFECT_NORMAL, MF_UNCHECKED );
	CheckMenuItem( hmenu, ID_EFFECT_LOW,    MF_UNCHECKED );


	CheckMenuItem( hmenu, ID_TEXT_CUT,      MF_UNCHECKED );
	CheckMenuItem( hmenu, ID_TEXT_HIGH,     MF_UNCHECKED );
	CheckMenuItem( hmenu, ID_TEXT_NORMAL,   MF_UNCHECKED );
	CheckMenuItem( hmenu, ID_TEXT_LOW,      MF_UNCHECKED );











	CheckMenuItem( hmenu, ID_WINDOW_CUT,    MF_UNCHECKED );
	CheckMenuItem( hmenu, ID_MES_SKIP,      MF_UNCHECKED );
	CheckMenuItem( hmenu, ID_SKIP_READ,     MF_UNCHECKED );
	CheckMenuItem( hmenu, ID_SKIP_NOREAD,   MF_UNCHECKED );
	CheckMenuItem( hmenu, ID_SKIP_SELECT_R, MF_UNCHECKED );
	CheckMenuItem( hmenu, ID_SKIP_SELECT_N, MF_UNCHECKED );

	CheckMenuItem( hmenu, ID_44100_16,      MF_UNCHECKED );
	CheckMenuItem( hmenu, ID_22050_16,      MF_UNCHECKED );
	CheckMenuItem( hmenu, ID_11025_16,      MF_UNCHECKED );
	CheckMenuItem( hmenu, ID_44100_8,       MF_UNCHECKED );
	CheckMenuItem( hmenu, ID_22050_8,       MF_UNCHECKED );
	CheckMenuItem( hmenu, ID_11025_8,       MF_UNCHECKED );


	CheckMenuItem( hmenu, ID_BGM_CHECK,     MF_UNCHECKED );
	CheckMenuItem( hmenu, ID_SE_CHECK,      MF_UNCHECKED );

	
	if(Avg.se_buf)	CheckMenuItem( hmenu, ID_SE_BUF,  MF_CHECKED );
	else			CheckMenuItem( hmenu, ID_SE_BUF,  MF_UNCHECKED );

	EnableMenuItem( GetMenu( MainWindow.hwnd ), ID_TITLE, MF_GRAYED );



	if(STD_CheckOSw2k()){
		int		i=0, j=0;
		char	buf[32];
		DEVMODE			*DevMode = CheckScreenMode();
		unsigned int	bpp = (CheckScreenMode_BPP24())?24:32;
		HMENU			hsubmenu = CreatePopupMenu();

		DeleteMenu( GetSubMenu(hmenu,0), ID_DISP_FULL, MF_BYCOMMAND );
		InsertMenu( GetSubMenu(hmenu,0), ID_DISP_WINDOW, MF_BYCOMMAND|MF_STRING|MF_POPUP, (unsigned int)hsubmenu, "フルスクリーンモード" );

		while(i<DevModeNum){
			if(!DevMode[i].dmBitsPerPel) break;
			if( DevMode[i].dmBitsPerPel==bpp && DevMode[i].dmPelsWidth==800 && DevMode[i].dmPelsHeight==600 ){
				if(DevMode[i].dmDisplayFrequency>=60 && DevMode[i].dmDisplayFrequency<=85) {
					if(DevMode[i].dmDisplayFrequency==60){
						DispDef60No = j;
						
						wsprintf( buf, "%d hz (標準)", DevMode[i].dmDisplayFrequency );
					}else{
						wsprintf( buf, "%d hz", DevMode[i].dmDisplayFrequency );
					}
					DispFreq[j] = DevMode[i].dmDisplayFrequency;
					InsertMenu( hsubmenu, -1, MF_BYPOSITION|MF_STRING, ID_SUBMENU+j++, buf );
					if(j>=16) break;
				}
			}
			i++;
		}
		DispFreqMax = j;

		if(MainWindow.full_screen){
			CheckMenuItem( hmenu, ID_SUBMENU+MainWindow.full_screen-1,  MF_CHECKED );
			EnableMenuItem( hmenu, ID_SUBMENU+MainWindow.full_screen-1, MF_GRAYED );	
		}else{
			CheckMenuItem( hmenu, ID_DISP_WINDOW,   MF_CHECKED );	
			EnableMenuItem( hmenu, ID_DISP_WINDOW, MF_GRAYED );
		}
	}else{
		if(MainWindow.full_screen){
			CheckMenuItem( hmenu, ID_DISP_FULL,   MF_CHECKED );	
			EnableMenuItem( hmenu, ID_DISP_FULL,  MF_GRAYED  );
		}else{
			CheckMenuItem( hmenu, ID_DISP_WINDOW,  MF_CHECKED );
			EnableMenuItem( hmenu, ID_DISP_WINDOW, MF_GRAYED );
		}
	}
	switch(Avg.frame){
		case 15:	CheckMenuItem( hmenu, ID_FRAME_15, MF_CHECKED );	break;
		case 20:	CheckMenuItem( hmenu, ID_FRAME_20, MF_CHECKED );	break;
		case 30:	CheckMenuItem( hmenu, ID_FRAME_30, MF_CHECKED ); 	break;
		case 45:	CheckMenuItem( hmenu, ID_FRAME_45, MF_CHECKED ); 	break;
		case 60:	CheckMenuItem( hmenu, ID_FRAME_60, MF_CHECKED );	break;
	}

	if(Avg.level)	CheckMenuItem( hmenu, ID_EFFECT_DISP,  MF_CHECKED );	
	else			CheckMenuItem( hmenu, ID_EFFECT_SPEED, MF_CHECKED ); 




	switch(Avg.wait){
		case 0:	CheckMenuItem( hmenu, ID_EFFECT_CUT,    MF_CHECKED );	break;
		case 1:	CheckMenuItem( hmenu, ID_EFFECT_HIGH,   MF_CHECKED ); 	break;
		case 2:	CheckMenuItem( hmenu, ID_EFFECT_NORMAL, MF_CHECKED );	break;
		case 4:	CheckMenuItem( hmenu, ID_EFFECT_LOW,    MF_CHECKED ); 	break;
	}
	if(Avg.win_eff)	CheckMenuItem( hmenu, ID_EFFECT_WIN, MF_UNCHECKED );
	else			CheckMenuItem( hmenu, ID_EFFECT_WIN, MF_CHECKED   );


	if(Avg.wheel){
		CheckMenuItem( hmenu, ID_MES_SKIP,      MF_CHECKED );
	}else{
		CheckMenuItem( hmenu, ID_WINDOW_CUT,    MF_CHECKED );
	}
	switch(Avg.msg_cut_optin){
		case 0:	CheckMenuItem( hmenu, ID_SKIP_READ,     MF_CHECKED );	break;
		case 1:	CheckMenuItem( hmenu, ID_SKIP_NOREAD,   MF_CHECKED );	break;
		case 2:	CheckMenuItem( hmenu, ID_SKIP_SELECT_R, MF_CHECKED );	break;
		case 3:	CheckMenuItem( hmenu, ID_SKIP_SELECT_N, MF_CHECKED );	break;
	}

	switch(Avg.msg_wait){
		case 0:


			CheckMenuItem( hmenu, ID_TEXT_CUT,   MF_CHECKED );
			break;
		case 1:	CheckMenuItem( hmenu, ID_TEXT_HIGH,   MF_CHECKED ); break;
		case 2:	CheckMenuItem( hmenu, ID_TEXT_NORMAL, MF_CHECKED );	break;
		case 3:	CheckMenuItem( hmenu, ID_TEXT_LOW,    MF_CHECKED );	break;
	}
	switch(Avg.msg_font){
		case 0:	CheckMenuItem( hmenu, ID_FONT_01,     MF_CHECKED );	break;
		case 1:	CheckMenuItem( hmenu, ID_FONT_02,     MF_CHECKED );	break;
		case 2:	CheckMenuItem( hmenu, ID_FONT_03,     MF_CHECKED );	break;
		case 3:	CheckMenuItem( hmenu, ID_FONT_04,     MF_CHECKED );	break;
	}
	switch(Avg.win_bld){
		case 0:	CheckMenuItem( hmenu, ID_WIN_STD,   MF_CHECKED );	break;
		case 1:	CheckMenuItem( hmenu, ID_WIN_BLEND, MF_CHECKED );	break;
		case 2:	CheckMenuItem( hmenu, ID_WIN_AMI,   MF_CHECKED );	break;
	}
	if( Avg.sound )	CheckMenuItem( hmenu, ID_SOUND_WAV,  MF_CHECKED );
	else			CheckMenuItem( hmenu, ID_SOUND_CDDA, MF_CHECKED );

	switch(Avg.pcm_fmt){
		case PCM_44_16:	CheckMenuItem( hmenu, ID_44100_16, MF_CHECKED );	break;
		case PCM_22_16:	CheckMenuItem( hmenu, ID_22050_16, MF_CHECKED );	break;
		case PCM_11_16:	CheckMenuItem( hmenu, ID_11025_16, MF_CHECKED );	break;
		case PCM_44_08:	CheckMenuItem( hmenu, ID_44100_8,  MF_CHECKED );	break;
		case PCM_22_08:	CheckMenuItem( hmenu, ID_22050_8,  MF_CHECKED );	break;
		case PCM_11_08:	CheckMenuItem( hmenu, ID_11025_8,  MF_CHECKED );	break;
	}


	if( Avg.bgm )	CheckMenuItem( hmenu, ID_BGM_CHECK,   MF_CHECKED );
	if( Avg.se )	CheckMenuItem( hmenu, ID_SE_CHECK,    MF_CHECKED );


	if( Avg.tx_pdwon )	CheckMenuItem( hmenu, ID_TEXT_PDOWN,  MF_CHECKED );
	if( Avg.tx_lbtrg )	CheckMenuItem( hmenu, ID_TEXT_LBTRG,  MF_CHECKED );

	if(!Avg.mov1)EnableMenuItem( hmenu, ID_MOV_LOW,  MF_GRAYED );
	if(!Avg.mov2)EnableMenuItem( hmenu, ID_MOV_HIGH, MF_GRAYED );
	if(!Avg.mov3)EnableMenuItem( hmenu, ID_MOV_MAX,  MF_GRAYED );
	CheckMenuItem( hmenu, ID_MOV_LOW,  (Avg.mov_lv==0)?MF_CHECKED:MF_UNCHECKED );
	CheckMenuItem( hmenu, ID_MOV_HIGH, (Avg.mov_lv==1)?MF_CHECKED:MF_UNCHECKED );
	CheckMenuItem( hmenu, ID_MOV_MAX,  (Avg.mov_lv==2)?MF_CHECKED:MF_UNCHECKED );

	CheckMenuItem( hmenu, ID_MOV_OL,  (OverLay)?MF_CHECKED:MF_UNCHECKED );

	CheckMenuItem( hmenu, ID_60FPS,  (Avg.frame==60)?MF_CHECKED:MF_UNCHECKED );
	CheckMenuItem( hmenu, ID_30FPS,  (Avg.frame==30)?MF_CHECKED:MF_UNCHECKED );

	EnableMenuItem( hmenu, ID_DISP_FULL,  MF_GRAYED );
	EnableMenuItem( hmenu, ID_DISP_WINDOW,  MF_GRAYED );

}

void EnableWindosChengMenu( int mode )
{
	HMENU	hmenu = GetMenu( MainWindow.hwnd );
	int		i;
	if(mode==0){
		if(MainWindow.full_screen){
			if( STD_CheckOSw2k() ){
				for( i=0; i<DispFreqMax ;i++ ){
					if(MainWindow.full_screen-1==i){
						EnableMenuItem( hmenu, ID_SUBMENU+i,MF_GRAYED);
					}else{
						EnableMenuItem( hmenu, ID_SUBMENU+i,MF_ENABLED);
					}
				}
			}else{
				EnableMenuItem( hmenu, ID_DISP_FULL,  MF_GRAYED );
			}
			EnableMenuItem( hmenu, ID_DISP_WINDOW,  MF_ENABLED );
		}else{
			if( STD_CheckOSw2k() ){
				for( i=0; i<DispFreqMax ;i++ ){
					EnableMenuItem( hmenu, ID_SUBMENU+i,MF_ENABLED);
				}
			}else{
				EnableMenuItem( hmenu, ID_DISP_FULL,  MF_ENABLED );
			}
			EnableMenuItem( hmenu, ID_DISP_WINDOW,  MF_GRAYED );
		}
	}else{
		if( STD_CheckOSw2k() ){
			for( i=0; i<DispFreqMax ;i++ ){
				EnableMenuItem( hmenu, ID_SUBMENU+i,  MF_GRAYED );
			}
		}else{
			EnableMenuItem( hmenu, ID_DISP_FULL,  MF_GRAYED );
		}
		EnableMenuItem( hmenu, ID_DISP_WINDOW,  MF_GRAYED );
	}


}

void MovPlayerFrmEnd( void )
{
	if( movPlayerFrm ){
		movPlayerFrm->Release();
		delete movPlayerFrm;
		movPlayerFrm=NULL;

		MovPlaying2=FALSE;
		EnableWindosChengMenu( 0 );
	}
}





static void MenuControl( HWND hwnd, UINT wparam )
{
	HMENU	hmenu = GetMenu( hwnd );
	int		i;

	switch( LOWORD(wparam) ){
		case ID_SUBMENU:	case ID_SUBMENU+1:	case ID_SUBMENU+2:	case ID_SUBMENU+3:
		case ID_SUBMENU+4:	case ID_SUBMENU+5:	case ID_SUBMENU+6:	case ID_SUBMENU+7:
		case ID_SUBMENU+8:	case ID_SUBMENU+9:	case ID_SUBMENU+10:	case ID_SUBMENU+11:
		case ID_SUBMENU+12:	case ID_SUBMENU+13:	case ID_SUBMENU+14:	case ID_SUBMENU+15:

			if( movPlayerFrm )	break;

			MainWindow.full_screen = LOWORD(wparam)-ID_SUBMENU+1;
			WIN_SetScreenMode( DISP_X, DISP_Y, DispFreq[LOWORD(wparam)-ID_SUBMENU] );

			for(i=0;i<DispFreqMax;i++){
				EnableMenuItem( hmenu, ID_SUBMENU+i,  MF_ENABLED  );
				CheckMenuItem( hmenu, ID_SUBMENU+i,   MF_UNCHECKED );
			}
			EnableMenuItem( hmenu, LOWORD(wparam), MF_GRAYED  );
			CheckMenuItem( hmenu, LOWORD(wparam),  MF_CHECKED );
			EnableMenuItem( hmenu, ID_DISP_WINDOW, MF_ENABLED );
			CheckMenuItem( hmenu, ID_DISP_WINDOW,  MF_UNCHECKED );
			break;
		case ID_WINCHANGE:
			if( movPlayerFrm )	break;

			if(MainWindow.full_screen){
				SendMessage( hwnd, WM_COMMAND, ID_DISP_WINDOW, 0 );
			}else{
				if( STD_CheckOSw2k() ){
					SendMessage( hwnd, WM_COMMAND, ID_SUBMENU+DispDef60No, 0 );
				}else{
					SendMessage( hwnd, WM_COMMAND, ID_DISP_FULL, 0 );
				}
			}
			break;
		case ID_DISP_FULL:	
			if( movPlayerFrm )	break;

			if(STD_CheckOSw2k()){
				SendMessage( hwnd, WM_COMMAND, ID_SUBMENU+DispDef60No, 0 );
			}else{
				MainWindow.full_screen = ON;
				WIN_SetScreenMode( DISP_X, DISP_DY, 0 );

				EnableMenuItem( hmenu, ID_DISP_FULL,   MF_GRAYED  );
				CheckMenuItem( hmenu, ID_DISP_FULL,   MF_CHECKED );	
				EnableMenuItem( hmenu, ID_DISP_WINDOW, MF_ENABLED );
				CheckMenuItem( hmenu, ID_DISP_WINDOW, MF_UNCHECKED );
			}
			break;

		case ID_DISP_WINDOW:
			if( movPlayerFrm )	break;

			MainWindow.full_screen = OFF;
			WIN_SetScreenMode( -1, -1, 0 );

			if(STD_CheckOSw2k()){
				for(i=0;i<DispFreqMax;i++){
					EnableMenuItem( hmenu, ID_SUBMENU+i,  MF_ENABLED  );
					CheckMenuItem( hmenu, ID_SUBMENU+i,   MF_UNCHECKED );
				}
			}else{
				EnableMenuItem( hmenu, ID_DISP_FULL,  MF_ENABLED );
				CheckMenuItem( hmenu, ID_DISP_FULL,   MF_UNCHECKED );	
			}
			EnableMenuItem( hmenu, ID_DISP_WINDOW, MF_GRAYED  );
			CheckMenuItem( hmenu, ID_DISP_WINDOW,  MF_CHECKED   );
			break;
		case ID_FRAME_15:	
			Avg.frame = 15;
			CheckMenuItem( hmenu, ID_FRAME_15, MF_CHECKED   );
			CheckMenuItem( hmenu, ID_FRAME_20, MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_FRAME_30, MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_FRAME_45, MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_FRAME_60, MF_UNCHECKED );
			break;
		case ID_FRAME_20:	
			Avg.frame = 20;
			CheckMenuItem( hmenu, ID_FRAME_15, MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_FRAME_20, MF_CHECKED   );
			CheckMenuItem( hmenu, ID_FRAME_30, MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_FRAME_45, MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_FRAME_60, MF_UNCHECKED );
			break;
		case ID_FRAME_30:	
			Avg.frame = 30;
			CheckMenuItem( hmenu, ID_FRAME_15, MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_FRAME_20, MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_FRAME_30, MF_CHECKED   );
			CheckMenuItem( hmenu, ID_FRAME_45, MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_FRAME_60, MF_UNCHECKED );
			break;
		case ID_FRAME_45:	
			Avg.frame = 45;
			CheckMenuItem( hmenu, ID_FRAME_15, MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_FRAME_20, MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_FRAME_30, MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_FRAME_45, MF_CHECKED   );
			CheckMenuItem( hmenu, ID_FRAME_60, MF_UNCHECKED );
			break;
		case ID_FRAME_60:	
			Avg.frame = 60;
			CheckMenuItem( hmenu, ID_FRAME_15, MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_FRAME_20, MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_FRAME_30, MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_FRAME_45, MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_FRAME_60, MF_CHECKED   );
			break;
		case ID_EFFECT_DISP:	
			Avg.level = 1;		
			CheckMenuItem( hmenu, ID_EFFECT_DISP,  MF_CHECKED   );	
			CheckMenuItem( hmenu, ID_EFFECT_SPEED, MF_UNCHECKED ); 
			break;
		case ID_EFFECT_SPEED:	
			Avg.level = 0;		
			CheckMenuItem( hmenu, ID_EFFECT_DISP,  MF_UNCHECKED );	
			CheckMenuItem( hmenu, ID_EFFECT_SPEED, MF_CHECKED   ); 
			break;










		case ID_EFFECT_CUT:		
			Avg.wait  = 0;		
			CheckMenuItem( hmenu, ID_EFFECT_CUT,    MF_CHECKED   );	
			CheckMenuItem( hmenu, ID_EFFECT_HIGH,   MF_UNCHECKED ); 
			CheckMenuItem( hmenu, ID_EFFECT_NORMAL, MF_UNCHECKED );	
			CheckMenuItem( hmenu, ID_EFFECT_LOW,    MF_UNCHECKED ); 
			break;
		case ID_EFFECT_HIGH:	
			Avg.wait  = 1;		
			CheckMenuItem( hmenu, ID_EFFECT_CUT,    MF_UNCHECKED );	
			CheckMenuItem( hmenu, ID_EFFECT_HIGH,   MF_CHECKED   ); 
			CheckMenuItem( hmenu, ID_EFFECT_NORMAL, MF_UNCHECKED );	
			CheckMenuItem( hmenu, ID_EFFECT_LOW,    MF_UNCHECKED ); 
			break;
		case ID_EFFECT_NORMAL:	
			Avg.wait  = 2;		
			CheckMenuItem( hmenu, ID_EFFECT_CUT,    MF_UNCHECKED );	
			CheckMenuItem( hmenu, ID_EFFECT_HIGH,   MF_UNCHECKED ); 
			CheckMenuItem( hmenu, ID_EFFECT_NORMAL, MF_CHECKED   );	
			CheckMenuItem( hmenu, ID_EFFECT_LOW,    MF_UNCHECKED ); 
			break;
		case ID_EFFECT_LOW:		
			Avg.wait  = 4;		
			CheckMenuItem( hmenu, ID_EFFECT_CUT,    MF_UNCHECKED );	
			CheckMenuItem( hmenu, ID_EFFECT_HIGH,   MF_UNCHECKED ); 
			CheckMenuItem( hmenu, ID_EFFECT_NORMAL, MF_UNCHECKED );	
			CheckMenuItem( hmenu, ID_EFFECT_LOW,    MF_CHECKED   ); 
			break;
		case ID_EFFECT_WIN:		
			if(Avg.win_eff){
				Avg.win_eff = 0;
				CheckMenuItem( hmenu, ID_EFFECT_WIN, MF_CHECKED );
			}else{
				Avg.win_eff = 1;
				CheckMenuItem( hmenu, ID_EFFECT_WIN, MF_UNCHECKED );
			}
			break;
		case ID_WINDOW_CUT:
			Avg.wheel=0;
			CheckMenuItem( hmenu, ID_WINDOW_CUT,    MF_CHECKED );
			CheckMenuItem( hmenu, ID_MES_SKIP,      MF_UNCHECKED );
			break;
		case ID_MES_SKIP:
			Avg.wheel=1;
			CheckMenuItem( hmenu, ID_WINDOW_CUT,    MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_MES_SKIP,      MF_CHECKED );
			break;
		case ID_SKIP_READ:
			Avg.msg_cut_optin=0;
			CheckMenuItem( hmenu, ID_SKIP_READ,     MF_CHECKED );
			CheckMenuItem( hmenu, ID_SKIP_NOREAD,   MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_SKIP_SELECT_R, MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_SKIP_SELECT_N, MF_UNCHECKED );
			break;
		case ID_SKIP_NOREAD:
			Avg.msg_cut_optin=1;
			CheckMenuItem( hmenu, ID_SKIP_READ,     MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_SKIP_NOREAD,   MF_CHECKED );
			CheckMenuItem( hmenu, ID_SKIP_SELECT_R, MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_SKIP_SELECT_N, MF_UNCHECKED );
			break;
		case ID_SKIP_SELECT_R:
			Avg.msg_cut_optin=2;
			CheckMenuItem( hmenu, ID_SKIP_READ,     MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_SKIP_NOREAD,   MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_SKIP_SELECT_R, MF_CHECKED );
			CheckMenuItem( hmenu, ID_SKIP_SELECT_N, MF_UNCHECKED );
			break;
		case ID_SKIP_SELECT_N:
			Avg.msg_cut_optin=3;
			CheckMenuItem( hmenu, ID_SKIP_READ,     MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_SKIP_NOREAD,   MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_SKIP_SELECT_R, MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_SKIP_SELECT_N, MF_CHECKED );
			break;
		case ID_AUTO_MODE:
			MovPlayerFrmEnd();
			DialogBox( AppStruct.inst, MAKEINTRESOURCE(IDD_DIALOG13), MainWindow.hwnd, AutoModeDialog );
			break;
		
		case ID_TEXT_CUT:		
			Avg.msg_page = 0;

			Avg.msg_wait = 0;	
			CheckMenuItem( hmenu, ID_TEXT_CUT,    MF_CHECKED   );	
			CheckMenuItem( hmenu, ID_TEXT_HIGH,   MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_TEXT_NORMAL, MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_TEXT_LOW,    MF_UNCHECKED );
			break;
		case ID_TEXT_HIGH:		
			Avg.msg_page = 0;

			Avg.msg_wait = 1;	
			CheckMenuItem( hmenu, ID_TEXT_CUT,    MF_UNCHECKED );	
			CheckMenuItem( hmenu, ID_TEXT_HIGH,   MF_CHECKED   );
			CheckMenuItem( hmenu, ID_TEXT_NORMAL, MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_TEXT_LOW,    MF_UNCHECKED );
			break;
		case ID_TEXT_NORMAL:	
			Avg.msg_page = 0;

			Avg.msg_wait = 2;	
			CheckMenuItem( hmenu, ID_TEXT_CUT,    MF_UNCHECKED );	
			CheckMenuItem( hmenu, ID_TEXT_HIGH,   MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_TEXT_NORMAL, MF_CHECKED   );
			CheckMenuItem( hmenu, ID_TEXT_LOW,    MF_UNCHECKED );
			break;
		case ID_TEXT_LOW:		
			Avg.msg_page = 0;

			Avg.msg_wait = 3;	
			CheckMenuItem( hmenu, ID_TEXT_CUT,    MF_UNCHECKED );	
			CheckMenuItem( hmenu, ID_TEXT_HIGH,   MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_TEXT_NORMAL, MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_TEXT_LOW,    MF_CHECKED   );
			break;
		case ID_TEXT_CUT_FLAG:	
			if(Avg.msg_cut){
				Avg.msg_cut  = 0;	
				CheckMenuItem( hmenu, ID_TEXT_CUT_FLAG, MF_UNCHECKED );
			}else{
				Avg.msg_cut  = 1;	
				CheckMenuItem( hmenu, ID_TEXT_CUT_FLAG, MF_CHECKED );
			}
			break;
		case ID_MOV_LOW:
			Avg.mov_lv=0;
			CheckMenuItem( hmenu, ID_MOV_LOW,  MF_CHECKED   );
			CheckMenuItem( hmenu, ID_MOV_HIGH, MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_MOV_MAX,  MF_UNCHECKED );
			break;
		case ID_MOV_HIGH:
			Avg.mov_lv=1;
			CheckMenuItem( hmenu, ID_MOV_LOW,  MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_MOV_HIGH, MF_CHECKED   );
			CheckMenuItem( hmenu, ID_MOV_MAX,  MF_UNCHECKED );
			break;
		case ID_MOV_MAX:
			Avg.mov_lv=2;
			CheckMenuItem( hmenu, ID_MOV_LOW,  MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_MOV_HIGH, MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_MOV_MAX,  MF_CHECKED   );
			break;
		case ID_MOV_OL:
			OverLay=!OverLay;

			if(OverLay){
				CheckMenuItem( hmenu, ID_MOV_OL,  MF_CHECKED   );
			}else{
				CheckMenuItem( hmenu, ID_MOV_OL,  MF_UNCHECKED );
			}
			
			break;
		case ID_60FPS:
			Avg.frame=60;
			
			CheckMenuItem( hmenu, ID_60FPS, MF_CHECKED );
			CheckMenuItem( hmenu, ID_30FPS, MF_UNCHECKED );
			break;
		case ID_30FPS:
			Avg.frame=30;
			
			CheckMenuItem( hmenu, ID_60FPS, MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_30FPS, MF_CHECKED );
			break;

		case ID_FONT_01:
			Avg.msg_font = 0;	
			FNT_ChangeFont( 0 );
			CheckMenuItem( hmenu, ID_FONT_01, MF_CHECKED   );
			CheckMenuItem( hmenu, ID_FONT_02, MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_FONT_03, MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_FONT_04, MF_UNCHECKED );
			break;
		case ID_FONT_02:
			Avg.msg_font = 1;	
			FNT_ChangeFont( 1 );
			CheckMenuItem( hmenu, ID_FONT_01, MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_FONT_02, MF_CHECKED   );
			CheckMenuItem( hmenu, ID_FONT_03, MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_FONT_04, MF_UNCHECKED );
			break;
		case ID_FONT_03:
			Avg.msg_font = 2;	
			FNT_ChangeFont( 2 );
			CheckMenuItem( hmenu, ID_FONT_01, MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_FONT_02, MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_FONT_03, MF_CHECKED   );
			CheckMenuItem( hmenu, ID_FONT_04, MF_UNCHECKED );
			break;
		case ID_FONT_04:
			Avg.msg_font = 3;	
			FNT_ChangeFont( 3 );
			CheckMenuItem( hmenu, ID_FONT_01, MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_FONT_02, MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_FONT_03, MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_FONT_04, MF_CHECKED   );
			break;
		case ID_WIN_STD:		
			Avg.win_bld = 0;	
			CheckMenuItem( hmenu, ID_WIN_STD,   MF_CHECKED   );
			CheckMenuItem( hmenu, ID_WIN_BLEND, MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_WIN_AMI,   MF_UNCHECKED );
			break;
		case ID_WIN_BLEND:		
			Avg.win_bld = 1;	
			CheckMenuItem( hmenu, ID_WIN_STD,   MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_WIN_BLEND, MF_CHECKED   );
			CheckMenuItem( hmenu, ID_WIN_AMI,   MF_UNCHECKED );
			break;
		case ID_WIN_AMI:		
			Avg.win_bld = 2;	
			CheckMenuItem( hmenu, ID_WIN_STD,   MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_WIN_BLEND, MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_WIN_AMI,   MF_CHECKED   );
			break;
		case ID_SOUND_WAV:		
			AVG_ChangeDeviceBGM( 1 );
			CheckMenuItem( hmenu, ID_SOUND_WAV,  MF_CHECKED   );
			CheckMenuItem( hmenu, ID_SOUND_CDDA, MF_UNCHECKED );
			break;
		case ID_SOUND_CDDA:		
			AVG_ChangeDeviceBGM( 0 );
			CheckMenuItem( hmenu, ID_SOUND_WAV,   MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_SOUND_CDDA,  MF_CHECKED   );
			break;

		case ID_44100_16:	case ID_22050_16:	case ID_11025_16:
		case ID_44100_8:	case ID_22050_8:	case ID_11025_8:
			break;
		case ID_BGM_CHECK:
			AVG_SetBGMFlag( !Avg.bgm );
			if( Avg.bgm )	CheckMenuItem( hmenu, ID_BGM_CHECK,  MF_CHECKED );
			else			CheckMenuItem( hmenu, ID_BGM_CHECK,  MF_UNCHECKED );
			break;
		case ID_SE_CHECK:
			AVG_SetSEFlag( !Avg.se );
			if( Avg.se )	CheckMenuItem( hmenu, ID_SE_CHECK,   MF_CHECKED );
			else			CheckMenuItem( hmenu, ID_SE_CHECK,  MF_UNCHECKED );
			break;
		case ID_VOICE_CHECK:
			MovPlayerFrmEnd();

			DialogBox( AppStruct.inst, MAKEINTRESOURCE(IDD_VOICE), MainWindow.hwnd, VoiceDialogBoxProc );




			break;
			
		case ID_SE_BUF:
			Avg.se_buf=!Avg.se_buf;
			if(Avg.se_buf){
				DebugBox( MainWindow.hwnd, "サウンドのバッファを多く確保することで、一部環境でのノイズ問題を解決します。\nただし、BGMのレスポンスが悪くなるため、正常に音が聞ける環境でこの設定をすることにメリットはありません。\n問題のある方のみ設定してください。\nまた、この設定は、ゲームをいったん終了し、再起動した後に有効になります。"  );
			}
			if(Avg.se_buf)	CheckMenuItem( hmenu, ID_SE_BUF,  MF_CHECKED );
			else			CheckMenuItem( hmenu, ID_SE_BUF,  MF_UNCHECKED );

			break;
		case ID_TEXT_PDOWN:
			Avg.tx_pdwon = !Avg.tx_pdwon;
			if( Avg.tx_pdwon )	CheckMenuItem( hmenu, ID_TEXT_PDOWN,  MF_CHECKED );
			else				CheckMenuItem( hmenu, ID_TEXT_PDOWN,  MF_UNCHECKED );
			break;
		case ID_TEXT_LBTRG:
			Avg.tx_lbtrg = !Avg.tx_lbtrg;
			if( Avg.tx_lbtrg )	CheckMenuItem( hmenu, ID_TEXT_LBTRG,  MF_CHECKED );
			else				CheckMenuItem( hmenu, ID_TEXT_LBTRG,  MF_UNCHECKED );
			break;
		case ID_VOLUME:
			MovPlayerFrmEnd();

			DialogBox( AppStruct.inst, MAKEINTRESOURCE(IDD_DIALOG8), MainWindow.hwnd, VolumeControlDialogProc );
			if(0){
				STARTUPINFO			stinfo;		
				PROCESS_INFORMATION	procinfo;	
				static HWND			hVolWin=NULL;
				RECT				rect;
				int					up = GetSystemMetrics(SM_CYDLGFRAME)+GetSystemMetrics(SM_CYSIZE)
										+ GetSystemMetrics(SM_CYBORDER)*2+GetSystemMetrics(SM_CYMENUSIZE);
				int					left = GetSystemMetrics(SM_CXDLGFRAME);
	
				ZeroMemory(&procinfo,sizeof(PROCESS_INFORMATION));
				ZeroMemory(&stinfo,sizeof(STARTUPINFO));
				stinfo.cb=sizeof(STARTUPINFO);
				stinfo.dwFlags      = STARTF_USESHOWWINDOW|STARTF_USESTDHANDLES;
				stinfo.wShowWindow  = SW_SHOW;

				hVolWin = GetVolumeContorolHandle();

				if(MainWindow.full_screen)
					SendMessage( MainWindow.hwnd, WM_COMMAND, ID_DISP_WINDOW, 0 );	
				if( !hVolWin ){
					if( !CreateProcess(NULL,"SNDVOL32.EXE",NULL,NULL,TRUE,NORMAL_PRIORITY_CLASS,NULL,NULL,&stinfo,&procinfo) ){
						MessageBox2( NULL, MB_OK|MB_SETFOREGROUND, "ボリュームコントロールが見つかりません", "ボリュームコントロールがインストールされていません。" );
						break;
					}
					WaitForInputIdle( procinfo.hProcess, INFINITE );
					hVolWin = GetVolumeContorolHandle();
					STD_SetForgroundMyWnd( hVolWin );
					
					GetWindowRect( MainWindow.hwnd, &rect );
					SetWindowPos( hVolWin, HWND_TOPMOST, rect.left+left, rect.top+up, 0, 0, SWP_NOSIZE );
				}else{
					STD_SetForgroundMyWnd( hVolWin );

					GetWindowRect( MainWindow.hwnd, &rect );
					SetWindowPos( hVolWin, HWND_TOPMOST, rect.left+left, rect.top+up, 0, 0, SWP_NOSIZE );
				}
			}
			break;
		case ID_INFO:			
			MovPlayerFrmEnd();

			DialogBox( AppStruct.inst, MAKEINTRESOURCE(IDD_DIALOG5), MainWindow.hwnd, InfoDialogBoxProc );
			break;

		case ID_TITLE:			
			AVG_SetGotoTitle();
			break;

		case ID_END2:			
			MAIN_SetEnd();
			NotInitDirectDraw = 1;
			break;
		case ID_END:			
			if( movPlayerWnd ){
				delete movPlayerWnd;
				movPlayerWnd=NULL;
			}
			MovPlayerFrmEnd();


			if(Debug.flag){


				MAIN_SetEnd();
				NotInitDirectDraw = 1;
			}else{
				if( MessageBox2( MainWindow.hwnd, MB_YESNO|MB_SETFOREGROUND|MB_SYSTEMMODAL, "終了", "%s を、終了してもよろしいですか？", TITLE_NAME )==IDYES ){
					MAIN_SetEnd();
					NotInitDirectDraw = 1;
				}
			}
			break;
		case ID_NAME:
			DialogBox( AppStruct.inst, MAKEINTRESOURCE(IDD_DIALOG12), MainWindow.hwnd, NameDialogBoxProc );
			break;

		case ID_GAMEHELP:
			ShellExecute( MainWindow.hwnd, NULL, "help\\help.html", NULL, NULL, 0 );
			break;
		case ID_VER:
			MovPlayerFrmEnd();

			DialogBox( AppStruct.inst, MAKEINTRESOURCE(IDD_DIALOG11), MainWindow.hwnd, DialogBoxProc );

			break;
		case ID_UPDATE:
			ShellExecute( MainWindow.hwnd, NULL, "http://leaf.aquaplus.co.jp", NULL, NULL, 0 );
			break;
	}
}


int Flag_MenuSelected = 0;





static LRESULT CALLBACK WndProc( HWND hwnd, UINT wmes, UINT wparam, LONG lparam)
{
	PAINTSTRUCT		ps;
	static int		mouse_show=1;
	


	switch( wmes) {

		
		case WM_INITMENU:
			Flag_MenuSelected = 4;
			break;
		case WM_EXITMENULOOP:
			Flag_MenuSelected = 3;
			break;

		case WM_GETMINMAXINFO:
			{
				LPMINMAXINFO	mminfo = (MINMAXINFO*)lparam;	
				int	mx = GetSystemMetrics(SM_CXDLGFRAME)*2;
				int	my = GetSystemMetrics(SM_CYMENUSIZE)+GetSystemMetrics(SM_CYBORDER)*2
						+GetSystemMetrics(SM_CYSIZE)+GetSystemMetrics(SM_CYDLGFRAME)*2;

				mminfo->ptMinTrackSize.x = DISP_X+mx;	
				mminfo->ptMinTrackSize.y = DISP_DY+my;	
				mminfo->ptMaxTrackSize.x = DISP_X+mx;	
				mminfo->ptMaxTrackSize.y = DISP_DY+my;	
			}
			break;
		case WM_CREATE:
			break;
		case 0x020a:
			if(!Avg.demo) MUS_RenewWheel( HIWORD(wparam) );


			break;
		case WM_MOUSEMOVE:
			if(MainWindow.full_screen){
				static char menu_flag = 0;
				int			my = (GetSystemMetrics(SM_CYMENUSIZE)+GetSystemMetrics(SM_CYBORDER))*2/3;

				if( ((short)LOWORD(lparam) >= 0 && (short)LOWORD(lparam) < DISP_X )
				 && ((short)HIWORD(lparam) >= 0 && (short)HIWORD(lparam) < ((menu_flag)?0:my)  ) ){
					SetFullScreenMenu( 1 );
					menu_flag = 1;

				}else if(menu_flag == 1){
					SetFullScreenMenu( 0 );
					menu_flag = 0;
				}
			}
			if(TentouFlag){
				SetTimer( hwnd, 0, 3000, NULL );
			}
			MainWindow.min_=OFF;
			MUS_ShowCursor( ON );
			break;
		case WM_TIMER:
			if(TentouFlag){
				int	mx,my;
				KillTimer( hwnd, 0 );
				MUS_GetMousePos( &mx, &my );
				if( (mx > 0 && mx < DISP_X-1 ) && (my > 0 && my < DISP_Y-1  ) ){
					MUS_ShowCursor( OFF );
				}
			}
			break;

		case WM_ACTIVATE:
			MainWindow.active = LOWORD(wparam);


			KEY_RenewKeybord( MainWindow.active );
			MUS_RenewMouse( MainWindow.hwnd, MainWindow.active, 0 );


			break;
		case WM_COMMAND:
			MUS_SetClickThrough();
			MenuControl( hwnd, wparam );
			break;
		case WM_DISPLAYCHANGE:

		{
			int	bit = wparam;
			int	w = LOWORD(lparam);
			int	h = HIWORD(lparam);

			if(w==DISP_X && !MainWindow.full_screen ){
				SendMessage( MainWindow.hwnd, WM_COMMAND, ID_DISP_FULL, 0 );
			}

			if( bit != MainWindow.disp_mode && !MainWindow.min_ ){
				if( bit == 8 ){	
					if( !MAIN_GetEnd() ){
						WIN_GetRegistryAmount( "256_check", &ReCheckFlag[0] );
						if(!ReCheckFlag[0])
							DialogBoxParam( AppStruct.inst, MAKEINTRESOURCE(IDD_DIALOG3), MainWindow.hwnd, ColorWarningDialog, 0 );
						WIN_SetRegistryAmount( "256_check", ReCheckFlag[0] );
					}
				}else if( bit == 16  ){
					if( !MAIN_GetEnd() ){
						WIN_GetRegistryAmount( "high_check", &ReCheckFlag[1] );
						if(!ReCheckFlag[1])
							DialogBoxParam( AppStruct.inst, MAKEINTRESOURCE(IDD_DIALOG1), MainWindow.hwnd, ColorWarningDialog, 1 );
						WIN_SetRegistryAmount( "high_check", ReCheckFlag[1] );
					}
				}

				if( MainWindow.disp_mode==8 ){
					BMP_ReleaseBMP_H( &MainWindow.vram_high );
					BMP_ReleaseBMP_F( &MainWindow.vram_full );
					BMP_ReleaseBMP_T( &MainWindow.vram_true );
					PAL_Terminate();
				}else{
					MainWindow.vram_high.buf = NULL;
					MainWindow.vram_full.buf = NULL;
					MainWindow.vram_true.buf = NULL;
				}
				WIN_CreateVram( USE_DD, DISP_MODE );
			}
			MainWindow.ww = w;
			MainWindow.wh = h;

			MainWindow.min_=OFF;
		}

			break;
		case WM_KEYDOWN:
			switch( wparam) {
				case VK_ESCAPE:
					if(Debug.flag)
						SendMessage( hwnd, WM_COMMAND, ID_END, 0 );
					break;
			}
			break;
		case WM_PAINT:
			if( !MovPlaying && !MovPlaying2 ){
				BeginPaint( hwnd, &ps );
				
				DRW_DrawBMP( hwnd, MainWindow.MemDC, MainWindow.hbmp, DISP_X, DISP_Y, Avg.interless );

				EndPaint( hwnd, &ps );
			}
			if(MovPlaying2){
				if(movPlayerFrm)
					movPlayerFrm->OnPaint();
			}
			break;
		case WM_CLOSE:
			SendMessage( hwnd, WM_COMMAND, ID_END, 0 );
			break;
		case WM_DESTROY:			
			PostQuitMessage(0);
			break;
		default:
			if(wmes==WM_SYSCOMMAND){
				switch(wparam&0xFFF0){
					case SC_MAXIMIZE:
						
						if( MainWindow.min_ ) return 0;
						if( STD_CheckOSw2k() ){
							SendMessage( hwnd, WM_COMMAND, ID_SUBMENU+DispDef60No, 0 );
						}else{
							SendMessage( hwnd, WM_COMMAND, ID_DISP_FULL, 0 );
						}
						return 0;	
					case SC_MINIMIZE:
						MainWindow.min_=ON;
						break;
				}
			}
			return DefWindowProc(hwnd, wmes, wparam, lparam);	
	}

	return 0;
}






static BOOL SetWindowClass(void)
{
	WNDCLASS	wndclass;
	BOOL		ret;
	
	ZeroMemory( &wndclass, sizeof(WNDCLASS) );
	wndclass.lpszClassName	= AppStruct.title;
	wndclass.lpfnWndProc	= WndProc;
	wndclass.style			= CS_DBLCLKS;
	wndclass.cbClsExtra 	= 0;
	wndclass.cbWndExtra 	= 0;
	wndclass.hInstance		= AppStruct.inst;
	wndclass.hIcon			= LoadIcon( AppStruct.inst, MAKEINTRESOURCE(IDI_ICON1) );

	wndclass.hCursor		= LoadCursor(NULL, IDC_ARROW);



	wndclass.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszMenuName	= MAKEINTRESOURCE(IDR_MENU1);
	
	ret = RegisterClass(&wndclass);
	return ret;
}
static BOOL ResetWindowClass(void)
{
	if( UnregisterClass( AppStruct.title, AppStruct.inst ) ){
		AppStruct.inst=0;
	}else{
		AppStruct.inst=0;
	}
	return 1;
}




BOOL SetMainWindow( char *title, int x, int y, int w, int h )
{

	strcpy( MainWindow.title, title );

	MainWindow.l = GetSystemMetrics(SM_CXDLGFRAME);
	MainWindow.r = GetSystemMetrics(SM_CXDLGFRAME);
	MainWindow.u = GetSystemMetrics(SM_CYDLGFRAME)+GetSystemMetrics(SM_CYSIZE)
				 + GetSystemMetrics(SM_CYBORDER)*2+GetSystemMetrics(SM_CYMENUSIZE);
	MainWindow.d = GetSystemMetrics(SM_CYDLGFRAME);

	MainWindow.x = x;		MainWindow.y = y;
	MainWindow.w = w;		MainWindow.h = h;

	MainWindow.hwnd = CreateWindow( AppStruct.title,			
									MainWindow.title,
									WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU | WS_OVERLAPPED | WS_CAPTION | WS_CLIPCHILDREN,
									MainWindow.x,
									MainWindow.y,
									MainWindow.w + MainWindow.l + MainWindow.r,
									MainWindow.h + MainWindow.u + MainWindow.d,
									NULL, NULL, AppStruct.inst, NULL );

	return 1;
}




BOOL WIN_CreateVram( int direct_draw, int draw_mode )
{
	int		w, h;
	RECT	WindowRect;
	static int	flag=1;


	DirectDrawFlag	= direct_draw;

	MainWindow.disp_mode = STD_GetScreenBpp(&w,&h);
	MainWindow.ww = w;
	MainWindow.wh = h;
	if(flag){
		flag=0;
		if(MainWindow.disp_mode==8){
			WIN_GetRegistryAmount( "256_check", &ReCheckFlag[0] );
			if(!ReCheckFlag[0])
				DialogBoxParam( AppStruct.inst, MAKEINTRESOURCE(IDD_DIALOG3), MainWindow.hwnd, ColorWarningDialog, 0 );
			WIN_SetRegistryAmount( "256_check", ReCheckFlag[0] );
		}
		if(MainWindow.disp_mode==16){
			WIN_GetRegistryAmount( "high_check", &ReCheckFlag[1] );
			if(!ReCheckFlag[1])
				DialogBoxParam( AppStruct.inst, MAKEINTRESOURCE(IDD_DIALOG1), MainWindow.hwnd, ColorWarningDialog, 1 );
			WIN_SetRegistryAmount( "high_check", ReCheckFlag[1] );
		}
	}

	if(draw_mode==-1 ){
		if(MainWindow.disp_mode==8)	draw_mode = 24;
		else						draw_mode = MainWindow.disp_mode;
	}else if(draw_mode==-2){
		switch(MainWindow.disp_mode){
			case 8:		draw_mode = 24;	break;
			case 16:
				if(CheckScreenMode_BPP24())	draw_mode = 24;
				else						draw_mode = 32;
				break;
			case 24:	draw_mode = 24;	break;
			case 32:	draw_mode = 32;	break;
		}
	}
	MainWindow.draw_mode2 = draw_mode;
	
	GetClientRect( MainWindow.hwnd, &WindowRect );	


	w = DISP_X;
	h = DISP_Y;





	MainWindow.draw_mode = MainWindow.disp_mode;	

	DSP_ChangeBmpAll( BPP(MainWindow.draw_mode2) );
	if( (MainWindow.draw_mode == 8) && (MainWindow.draw_mode != MainWindow.draw_mode2) ){
		BMP_GetImageVram8(  MainWindow.hwnd, &MainWindow.MemDC, &MainWindow.hbmp, &MainWindow.hbmp_old, &MainWindow.vram_256, w, h );
		switch( MainWindow.draw_mode2 ){
			default:	DebugBox( NULL, "画面モードが異常です" );			break;
			case 16:	BMP_CreateBMP_H( &MainWindow.vram_high, w, h, 0 );	break;
			case 24:	BMP_CreateBMP_F( &MainWindow.vram_full, w, h );		break;
			case 32:	BMP_CreateBMP_T( &MainWindow.vram_true, w, h );		break;
		}
	}else{
		switch( MainWindow.draw_mode2 ){
			default:	DebugBox( NULL, "画面モードが異常です" );	return 0;	break;
			case 8:		BMP_GetImageVram8(  MainWindow.hwnd, &MainWindow.MemDC, &MainWindow.hbmp, &MainWindow.hbmp_old, &MainWindow.vram_256, w, h );	break;
			case 16:	BMP_GetImageVram16( MainWindow.hwnd, &MainWindow.hbmp, &MainWindow.vram_high, w, h );	break;
			case 24:	BMP_GetImageVram24( MainWindow.hwnd, &MainWindow.MemDC, &MainWindow.hbmp, &MainWindow.hbmp_old, &MainWindow.vram_full, w, h );	break;
			case 32:	BMP_GetImageVram32( MainWindow.hwnd, &MainWindow.MemDC, &MainWindow.hbmp, &MainWindow.hbmp_old, &MainWindow.vram_true, w, h );	break;
		}
	}

	if( MainWindow.draw_mode == 8 || MainWindow.draw_mode2 == 8 ){

		PAL_Initialize();
		PAL_SetEntry( NULL, 0, 256 );
		PAL_SetPaletteTable( );
		MainWindow.vram_256.pal = GlobalPal;
		BMP_GetImageVram8( MainWindow.hwnd, &MainWindow.MemDC, &MainWindow.hbmp, &MainWindow.hbmp_old, &MainWindow.vram_256, w, h);
	}
	return 1;
}



HBITMAP		DebugHbmp=NULL;

BOOL WIN_ReleaseVram( void )
{
	if(MainWindow.MemDC){
		SelectObject(MainWindow.MemDC,MainWindow.hbmp_old);
		DeleteObject(MainWindow.hbmp);
		MainWindow.hbmp = NULL;
		DeleteDC(MainWindow.MemDC);
		MainWindow.MemDC=NULL;
	}
	PAL_ResetPaletteTable();
	return 1;
}



void SetAppData( char *title, HINSTANCE hInstance )
{
	strcpy( AppStruct.title, title );
	AppStruct.inst = hInstance;
}




void WIN_GetRegistryAmount( char *key, long *amount )
{
	SetRegistryKey( REG_KEY_CONF );
	if( ReadRegistryAmount( key, amount ) ){
		WriteRegistryAmount( key, *amount );
	}
}

void WIN_SetRegistryAmount( char *key, long amount )
{
	SetRegistryKey( REG_KEY_CONF );
	WriteRegistryAmount( key, amount );
}



void WIN_Init( void )
{
	long	px,py;
	SetRegistryKey( REG_KEY );
	if( ReadRegistryAmount( "pos_x", &px ) ) px = GetSystemMetrics(SM_CXFULLSCREEN)/2-400;
	if( ReadRegistryAmount( "pos_y", &py ) ) py = GetSystemMetrics(SM_CYFULLSCREEN)/2-300;

	px = LIM(px,0,GetSystemMetrics(SM_CXFULLSCREEN)-800);
	py = LIM(py,0,GetSystemMetrics(SM_CYFULLSCREEN)-600);



	InitBootFlag = !STD_CheckFile( "Sys.sav" );

	BMP_SetPackDir( PAC_GRP );
	FNT_SetPackDir( PAC_FNT );
	DSP_SetPackDirMov( PAC_MOV );
	EXEC_SetPackDir( PAC_SDT );

	BMP_InitBmp( DISP_X, DISP_Y );		
	DRW_SetDispWH( DISP_X, DISP_Y );
	DRW_SetClipRectDefault( 0, 0, DISP_X, DISP_Y );
	MUS_InitMouse( DISP_X, DISP_Y );

	SetWindowClass();
	SetMainWindow( TITLE_NAME, px, py, DISP_X, DISP_DY );	
	STD_SetMainHwnd( MainWindow.hwnd );

	WIN_CreateVram( USE_DD, DISP_MODE );	
	STD_InitTable();						

	AVG_LoadCgFlag();
	AVG_LoadReplayFlag();
	ESC_InitFlag();
	AVG_LoadGameFlag();
	AVG_LoadScenarioFlag();

	AVG_ReadConfigParam();

	switch(Avg.pcm_fmt){
		case PCM_44_16:	SendMessage( MainWindow.hwnd, WM_COMMAND, ID_44100_16, 0 );	break;	
		case PCM_22_16:	SendMessage( MainWindow.hwnd, WM_COMMAND, ID_22050_16, 0 );	break;	
		case PCM_11_16:	SendMessage( MainWindow.hwnd, WM_COMMAND, ID_11025_16, 0 );	break;	
		case PCM_44_08:	SendMessage( MainWindow.hwnd, WM_COMMAND, ID_44100_8, 0 );	break;	
		case PCM_22_08:	SendMessage( MainWindow.hwnd, WM_COMMAND, ID_22050_8, 0 );	break;	
		case PCM_11_08:	SendMessage( MainWindow.hwnd, WM_COMMAND, ID_11025_8, 0 );	break;	
	}

	AVG_InitSound();
}



void WIN_Term( void )
{
	long	px,py;
	STD_GetWindowPos( MainWindow.hwnd, (int*)&px, (int*)&py );
	SetRegistryKey( REG_KEY );
	WriteRegistryAmount( "pos_x", px );
	WriteRegistryAmount( "pos_y", py );

	WIN_ReleaseVram();
	ResetWindowClass();

	AVG_TermSound();

	GFree(DevModeStruct);
}




char *STD_CheckFile4CDDrive(  char *dir, char *file, char *atr )
{
	int i;
	char buf[MAX_PATH];
	static char buf2[MAX_PATH];
	char *buf3;

	for( i=0; i<26; i++) {
		wsprintf( buf, "%c:\\", 'A'+i );
		if( GetDriveType(buf) == DRIVE_CDROM ){
			if(dir) strcat( buf, dir );
			if( SearchPath(	buf, file, atr, MAX_PATH, buf2, &buf3) != 0) {
				break;
			}
		}
	}
	if( i==26) {
		return NULL;
	}

	return buf2;
}

char *STD_CDKeyCheck(void)
{
	return STD_CheckFile4CDDrive( NULL, "Disc2", ".ID" );
}




int	STD_DVD_KeyCheck(void)
{
	int i;
	char buf[MAX_PATH];
	char buf2[MAX_PATH];
	char *buf3;

	for( i=0; i<26; i++) {
		wsprintf( buf, "%c:\\", 'A'+i );
		if( GetDriveType(buf) == DRIVE_CDROM ){

			if( SearchPath(	buf, "sizuku1", ".tmp", MAX_PATH, buf2, &buf3) != 0) {
				if( STD_GetFileSize( buf2 )==1024*1024*1024 ){
					if( SearchPath(	buf, "sizuku2", ".tmp", MAX_PATH, buf2, &buf3) != 0) {
						if( STD_GetFileSize( buf2 )==1024*1024*1024 ){
							if( SearchPath(	buf, "sizuku3", ".tmp", MAX_PATH, buf2, &buf3) != 0) {
								if( STD_GetFileSize( buf2 )==1024*1024*1024 ){
									break;
								}
							}
						}
					}
				}
			}
		}
	}
	if( i==26) {
		return FALSE;
	}

	return TRUE;
}




char	ScriptFileName[MAX_PATH] = "EV_0301MORNING.sdt";



int	Ninsyou( char *UserId, char *UserPass )
{
	int		cnt,i;
	char	buf[256];

	unsigned char	UTAWARE_KEY[9] = "S2KTI2G7";

	unsigned char	KeyTable[37] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	strcpy( (char*)buf, (char*)UTAWARE_KEY );
	
	cnt=0;
	while( *(UserId+cnt) ){
		switch(cnt%3){
			case 0:	for(i=0;i<8;i++) buf[i] ^= *(UserId+cnt);	break;
			case 1:	for(i=0;i<8;i++) buf[i] += *(UserId+cnt);	break;
			case 2:	for(i=0;i<8;i++) buf[i] -= *(UserId+cnt);	break;
		}
		cnt++;
	}
	for(i=0;i<8;i++){
		buf[i] = KeyTable[buf[i]%36];
	}

	return strcmp((char*)UserPass,(char*)buf)==0;
}

BOOL CALLBACK KeyDialogBoxProc( HWND hwnd, UINT wmes, UINT wparam, LONG lparam )
{
	RECT	rect;
	int		mx,my;
	char	UserId[256];
	char	UserPass[256];

	switch( wmes ){
		case WM_CREATE:
		case WM_INITDIALOG:

			GetWindowRect( hwnd, &rect );
			mx = (GetSystemMetrics(SM_CXSCREEN)-(rect.right-rect.left))/2;
			my = (GetSystemMetrics(SM_CYSCREEN)-(rect.bottom-rect.top))/2;
			SetWindowPos( hwnd, NULL, mx, my, 0, 0, SWP_NOSIZE|SWP_NOZORDER );
			break;
		case WM_COMMAND:
			switch( LOWORD( wparam ) ){
				case IDOK:
					GetDlgItemText( hwnd, IDC_EDIT1, UserId, 256 );
					GetDlgItemText( hwnd, IDC_EDIT2, UserPass, 256 );

					if( Ninsyou( UserId, UserPass ) ){
						SetRegistryKey( REG_KEY );
						WriteRegistryStr( "Id",   (char*)UserId );
						WriteRegistryStr( "Pass", (char*)UserPass );
						EndDialog( hwnd, TRUE );
					}else{
						DebugBox(NULL,"パスワードかユーザーIDが間違っています。");
						EndDialog( hwnd, FALSE );
					}
					break;
				case IDCANCEL:
					EndDialog( hwnd, FALSE );
					break;
			}
			break;
		case WM_CLOSE:
			EndDialog( hwnd, FALSE );
			break;
	}
	return 0;
}

#define ID_KEY_DISC	0



#include "escript.h"


#include <crtdbg.h>

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#define malloc(p1) _malloc_dbg(p1,_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam,int nCmdShow )
{
	InitCommonControls();



	_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG)); 


	HANDLE	hAccel = LoadAccelerators( hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1) );

	MSG		msg;
	int		ret;
	HANDLE	hMutex;
	HWND	hWin;

	CoInitialize(NULL);
	
	hMutex = CreateMutex(NULL,TRUE,"SCRIPT_ENGINE");
	if(ERROR_ALREADY_EXISTS == GetLastError() ){
		if( lpszCmdParam[0]!='\0' ){
			hWin=FindWindow( TITLE_NAME, NULL );
			if( hWin ){
				SendMessage( hWin, WM_COMMAND, ID_END2, 0 );
			}
		}else{
			return FALSE;
		}
	}
	SetAppData( TITLE_NAME, hInstance );


#if ID_KEY_DISC
	{
		char	UserId[256]="";
		char	UserPass[256]="";

		SetRegistryKey( REG_KEY );
		ReadRegistryStr( "Id",  (char*) UserId );
		ReadRegistryStr( "Pass", (char*)UserPass );

		if( !Ninsyou( UserId, UserPass ) ){
			if( !DialogBox( AppStruct.inst, MAKEINTRESOURCE(IDD_DIALOG9), MainWindow.hwnd, KeyDialogBoxProc ) ){
				return FALSE;
			}
		}
	}
#endif


	if( lpszCmdParam[0]!='\0' ){
		int		i=0;
		Debug.flag=ON;
		while( lpszCmdParam[i] ){
			if( lpszCmdParam[i]==' ' ){
				lpszCmdParam[i] = '\0';
				break;
			}
			i++;
		}
		if( !_stricmp(lpszCmdParam,"/t") ){
			GameStep = GAME_TITLE;
			GotoTitle=ON;
		}else if( !_stricmp(lpszCmdParam,"/d") ){
			GameStep = GAME_DEMO;
		}else{
			while( lpszCmdParam[i]!='\\' ){
				i--;
				if(i<=0){
					i=-1;
					break;
				}
			}
			strcpy(ScriptFileName,lpszCmdParam+i+1);
			GameStep = GAME_AVG;
		}
	}else{
		

	}

	
#if DVD_OPTION
	if( !(GetAsyncKeyState(VK_NONCONVERT)&0x8000) ){
		while(1) {
			if( STD_DVD_KeyCheck()==FALSE) {					
				int flag = MessageBox2( NULL, MB_RETRYCANCEL, "警告", "DVD-ROMドライブに「%s」のディスクをセットして下さい", TITLE_NAME );
				if( flag != IDRETRY) return 0;
			} else
				break;
		}
	}

#endif



	STD_CheckCPU( );						
	STD_CheckOS();							

	WIN_Init();



	if(Debug.flag){
		char	buf[256];
		HANDLE		fh;
		FILETIME	ft;

		fh = CREATE_READ_FILE( "ScriptEngine.exe" );
		if ( fh != INVALID_HANDLE_VALUE ) {
			GetFileTime(fh,NULL,NULL,&ft);
			CloseHandle( fh );
			SYSTEMTIME	system;
			FileTimeToSystemTime( &ft, &system );
			wsprintf( buf, "Date: %d/%d %02d:%02d",
											system.wMonth,
											system.wDay+(system.wHour+9)/24,
											(system.wHour+9)%24,
											system.wMinute );
			SetWindowText( MainWindow.hwnd, buf );
		}
	}

	TIMECAPS tc;
	timeGetDevCaps(&tc,sizeof(tc));
	timeBeginPeriod(tc.wPeriodMin);

	ret = ShowWindow(   MainWindow.hwnd, nCmdShow );		
	UpdateWindow( MainWindow.hwnd );				
	SetFocus(     MainWindow.hwnd );				

	WINNLSEnableIME(MainWindow.hwnd,FALSE);	

	while( 1 ) {
		if( PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) ) {	
			if( !GetMessage(&msg, NULL, 0, 0) )	break;		
			if( TranslateAccelerator( MainWindow.hwnd, (HACCEL)hAccel, &msg ) ) continue;
			
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}

		if( !MovPlaying && !MovPlaying2 ){
			MAIN_Loop();

			char	buf[256];
			static int	_month=0;
			static int	_day=0;
			static int	_time=0;

			if( _month!=ESC_GetFlag( 0 ) || _day!=ESC_GetFlag( 1 ) || _time!=ESC_GetFlag( 2 ) ){
			
				char		*_time_tbl[8] = { "朝","休み時間","昼休み","授業中","授業終了後","放課後","夜","次の日へ" };
				char		*_youbi_tbl[7] = {"日","月","火","水","木","金","土"};
				int			_youbi=0;
				char		*_syukujitu_tbl[3] = { "","/祝日","/春休み"};
				int			_syukujitu=0;

				_month = ESC_GetFlag(0);
				_day   = ESC_GetFlag(1);
				_time  = ESC_GetFlag(2);

				switch( _month ){
				case 3:	_youbi = (_day)%7; break;
				case 4:	_youbi = (_day+3)%7; break;
				case 5:	_youbi = (_day+5)%7; break;
				}
				if( _month==3 && _day==20 ) _syukujitu=1;
				if( _month==4 && _day==29 ) _syukujitu=1;
				if( _month==5 && _day==3 ) _syukujitu=1;
				if( _month==5 && _day==4 ) _syukujitu=1;
				if( _month==5 && _day==5 ) _syukujitu=1;

				if( _month==3 && _day>=25 ) _syukujitu=2;
				if( _month==4 && _day<=7  ) _syukujitu=2;


				if(_month==0){
					wsprintf( buf, "Date: ？？/？？" );
				}else{






				}

			}
		}else{
			if( movPlayerFrm ){
				if(movPlayerFrm->DecodeMovie()){
					movPlayerFrm->DrawMovie();
				}


				KEY_RenewKeybord( MainWindow.active );
				MUS_RenewMouse( MainWindow.hwnd, MainWindow.active, 0 );
				if( AVG_KeySkip() || movPlayerFrm->bEnd ){
					movPlayerFrm->Release();
					delete movPlayerFrm;
					movPlayerFrm=NULL;
				}
			}else{
				EnableWindosChengMenu( 0 );
				MovPlaying2=FALSE;
			}

			if( movPlayerWnd ){
				KEY_RenewKeybord( MainWindow.active );
				MUS_RenewMouse( MainWindow.hwnd, MainWindow.active, 0 );
				if( MUS_GetMouseTrigger(MOUSE_LBUTTON) || MUS_GetMouseTrigger(MOUSE_RBUTTON)
					|| KeyCond.trg.enter || KeyCond.trg.space || KeyCond.trg.esc
					|| movPlayerWnd->PlayCheck() ){
					delete movPlayerWnd;
					movPlayerWnd=NULL;
				}
			}else{
				MovPlaying=FALSE;
			}
		}



 		Sleep(1);
	}

	WINNLSEnableIME(MainWindow.hwnd,TRUE);	
	WIN_Term();

	if(MainWindow.full_screen)
		WIN_SetScreenMode( -1, -1, 0 );
	

	timeEndPeriod(tc.wPeriodMin);

	
	CloseHandle(hMutex);
	CoUninitialize();

    
	_CrtDumpMemoryLeaks();
    
	return msg.wParam;
}

