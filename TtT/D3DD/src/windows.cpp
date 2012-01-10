

#include <windows.h>
#include <mouse.h>
#include <keybord.h>

#include "STD_Windows.h"
#include "STD_Debug.h"
#include "STD_File.h"

#include "main.h"

#include "ctrl.h"


#include "dispSprite.h"
#include "GlobalDraw.h"
#include "resource.h"


#include "Escript.h"

#include "GM_Main.h"
#include "GM_Avg.h"

#include "GM_Battle.h"










#if TAIKEN_FLAG
	APPLICATION_DATA	appData = { "Tears To Tiara 体験版", "TearsToTiaraCN", TRUE };
#else
	APPLICATION_DATA	appData = { "Tears to Tiara", "TearsToTiaraCN", TRUE };
#endif

static LRESULT CALLBACK WndProc(HWND hWnd,UINT msg,UINT wParam,LONG lParam);





int WindowCreate( int x, int y )
{
	WNDCLASS	wndclass;
	BOOL		ret;
	RECT		rect;
	
	ZeroMemory( &wndclass, sizeof(WNDCLASS) );
	wndclass.lpszClassName	= appData.ClassName;
	wndclass.lpfnWndProc	= WndProc;
	wndclass.style			= CS_DBLCLKS;
	wndclass.cbClsExtra 	= 0;
	wndclass.cbWndExtra 	= 0;
	wndclass.hInstance		= appData.Inst;
	wndclass.hIcon			= LoadIcon( appData.Inst, MAKEINTRESOURCE(IDI_ICON1) );

	wndclass.hCursor		= LoadCursor(NULL, IDC_ARROW);


	wndclass.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszMenuName	= MAKEINTRESOURCE(IDR_MENU1);

	
	ret = RegisterClass(&wndclass);
	if( !ret ) return FALSE;

	
	rect.left = 0;
	rect.top = 0;
	rect.right  = WIN_SIZE_W;
	rect.bottom = WIN_SIZE_H;
	AdjustWindowRect(&rect,WS_MINIMIZEBOX | WS_SYSMENU | WS_OVERLAPPED | WS_CAPTION | WS_CLIPCHILDREN, TRUE );
	appData.Hwnd = CreateWindow( appData.ClassName, appData.AppName,


							WINDOWMODE_STYLE,
							x, y, rect.right - rect.left,rect. bottom - rect.top,
							NULL, NULL,
							appData.Inst,NULL);

	appData.hMenu = GetMenu( appData.Hwnd );

	appData.MenuHwnd = CreateWindow( appData.ClassName, "menuWnd", WS_POPUP,
					0,0, 800,GetSystemMetrics(SM_CYMENUSIZE),
					appData.Hwnd, appData.hMenu, appData.Inst, NULL );

	EnableMenuItem( appData.hMenu, ID_FULL_MODE,    (D3DMain.m_WindowMode)?MF_ENABLED:MF_GRAYED   );
	EnableMenuItem( appData.hMenu, ID_WINDOW_MODE,  (D3DMain.m_WindowMode)?MF_GRAYED:MF_ENABLED  );




	
	CheckMenuItem( appData.hMenu, ID_EFFECT_CUT,    (Avg.wait==MES_CUT   )?MF_CHECKED:MF_UNCHECKED );
	CheckMenuItem( appData.hMenu, ID_EFFECT_HIGH,   (Avg.wait==MES_FAST  )?MF_CHECKED:MF_UNCHECKED ); 
	CheckMenuItem( appData.hMenu, ID_EFFECT_NORMAL, (Avg.wait==MES_NORMAL)?MF_CHECKED:MF_UNCHECKED );
	CheckMenuItem( appData.hMenu, ID_EFFECT_LOW,    (Avg.wait==MES_SLOW  )?MF_CHECKED:MF_UNCHECKED ); 

	CheckMenuItem( appData.hMenu, ID_SKIP_READ,     (Avg.msg_cut_optin)?MF_UNCHECKED:MF_CHECKED );
	CheckMenuItem( appData.hMenu, ID_SKIP_NOREAD,   (Avg.msg_cut_optin)?MF_CHECKED:MF_UNCHECKED );

	CheckMenuItem( appData.hMenu, ID_TEXT_CUT,    (Avg.msg_wait==MES_CUT   )?MF_CHECKED:MF_UNCHECKED );
	CheckMenuItem( appData.hMenu, ID_TEXT_HIGH,   (Avg.msg_wait==MES_FAST  )?MF_CHECKED:MF_UNCHECKED ); 
	CheckMenuItem( appData.hMenu, ID_TEXT_NORMAL, (Avg.msg_wait==MES_NORMAL)?MF_CHECKED:MF_UNCHECKED );
	CheckMenuItem( appData.hMenu, ID_TEXT_LOW,    (Avg.msg_wait==MES_SLOW  )?MF_CHECKED:MF_UNCHECKED ); 

	CheckMenuItem( appData.hMenu, ID_MOV1,   (Avg.mov_lv==0)?MF_CHECKED:MF_UNCHECKED );
	CheckMenuItem( appData.hMenu, ID_MOV2,   (Avg.mov_lv==1)?MF_CHECKED:MF_UNCHECKED );
	CheckMenuItem( appData.hMenu, ID_MOV3,   (Avg.mov_lv==2)?MF_CHECKED:MF_UNCHECKED );

	CheckMenuItem( appData.hMenu, ID_AUTO_RET,  (Avg.auto_modo==0)?MF_CHECKED:MF_UNCHECKED );
	CheckMenuItem( appData.hMenu, ID_WAIT_MODE, (Avg.auto_modo==1)?MF_CHECKED:MF_UNCHECKED );


	if(Avg.menu)CheckMenuItem( appData.hMenu, ID_MENU_MODE, MF_CHECKED   );
	else		CheckMenuItem( appData.hMenu, ID_MENU_MODE, MF_UNCHECKED );

	SetWindowPos( appData.Hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW|SWP_DRAWFRAME );
	return TRUE;
}





int EndApp( int last)
{
	AVG_SavetScenarioFlag();

	if(last)
		AVG_TermSound();
	GD_ReleasePixelShader(0);


	GD_TermDevices();

	return TRUE;
}




int	window_x=0;
int	window_y=0;

int StartApp(void)
{
	STD_InitTable();						

	if(D3DMain.m_WindowMode){
		RECT rect;
		SetRect( &rect, 0, 0, WIN_SIZE_W, WIN_SIZE_H );
		AdjustWindowRect( &rect, WINDOWMODE_STYLE, !!appData.hMenu );

		int	wx = (GetSystemMetrics(SM_CXFULLSCREEN)-(rect.right-rect.left))/2;
		int	wy = (GetSystemMetrics(SM_CYFULLSCREEN)-(rect.bottom-rect.top))/2;
		WindowCreate(window_x,window_y);

	}else{
		WindowCreate(0,0);

		SetWindowLong(appData.Hwnd, GWL_STYLE, FULLMODE_STYLE);
		SetMenu(appData.Hwnd, NULL);

	}
	STD_SetMainHwnd( appData.Hwnd );

	D3DM_SetWindowXY( window_x, window_y );
	D3DM_SetWindowStyle( WINDOWMODE_STYLE, FULLMODE_STYLE );
	if( !GD_InitDevices( appData.Hwnd, WIN_SIZE_W, WIN_SIZE_H, !D3DMain.m_WindowMode, appData.hMenu ) ){
		EndApp(0);
		return FALSE;
	}

DebugPrintf( "%d/%d\n", BMP_DEBUG, D3DD_MAX_TEXTURE_AMOUNT );
DebugPrintf( "%d/%d\n", GRP_DEBUG, D3DD_MAX_DISP_AMOUNT );
DebugPrintf( "%d/%d\n", SPR_DEBUG, SPRITE_MAX );



	MUS_InitMouse( WIN_SIZE_W, WIN_SIZE_H );
	AVG_InitSound();

	
	GD_SetPackDir( "grp" );
	EXEC_SetPackDir( "sdt" );




	
	return TRUE;
}

BOOL CALLBACK VolumeControlDialogProc( HWND hwnd, UINT wmes, UINT wparam, LONG lparam )
{
	static int	bgm_param=16;
	static int	se_param=16;
	static int	voice_param=16;

	static int	bgm_flag  = 0;
	static int	se_flag   = 0;
	static int	voice_flag= 0;
	static int	ero_voice = 0;

	switch( wmes ){
		case WM_CREATE:
		case WM_INITDIALOG:
			bgm_param = Avg.bgm_vol/16;
			se_param  = Avg.se_vol/16;
			voice_param  = Avg.voice_vol/16;

			SetSliderParamEx( hwnd, IDC_SLIDER1, 0, 16, 1, 1, 1*4, bgm_param );
			SetSliderParamEx( hwnd, IDC_SLIDER2, 0, 16, 1, 1, 1*4, se_param );
			SetSliderParamEx( hwnd, IDC_SLIDER3, 0, 16, 1, 1, 1*4, voice_param );

			bgm_flag   = Avg.bgm;
			se_flag    = Avg.se;
			voice_flag = Avg.voice;
			
			ero_voice = Avg.ero_voice;

			SendDlgItemMessage(hwnd, IDC_CHECK_BGM,   BM_SETCHECK, Avg.bgm,   0 );
			SendDlgItemMessage(hwnd, IDC_CHECK_SE,    BM_SETCHECK, Avg.se,    0 );
			SendDlgItemMessage(hwnd, IDC_CHECK_VOICE, BM_SETCHECK, Avg.voice, 0 );

			SendDlgItemMessage(hwnd, IDC_CHECK_ERO_VOICE, BM_SETCHECK, Avg.ero_voice, 0 );

			
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
					AVG_SetBGMFlag( bgm_flag );
					AVG_SetSEFlag( se_flag );
					AVG_SetVoiveFlag( voice_flag );

					Avg.ero_voice = ero_voice;

					EndDialog( hwnd, TRUE );
					break;

				case IDC_CHECK_BGM:
					AVG_SetBGMFlag( !Avg.bgm );
					SendDlgItemMessage(hwnd, IDC_CHECK_BGM,   BM_SETCHECK, Avg.bgm,   0 );
					break;
				case IDC_CHECK_SE:
					AVG_SetSEFlag( !Avg.se );
					SendDlgItemMessage(hwnd, IDC_CHECK_SE,   BM_SETCHECK, Avg.se,   0 );
					break;
				case IDC_CHECK_VOICE:
					AVG_SetVoiveFlag( !Avg.voice );
					SendDlgItemMessage(hwnd, IDC_CHECK_VOICE,   BM_SETCHECK, Avg.voice,   0 );
					break;
				case IDC_CHECK_ERO_VOICE:
					Avg.ero_voice = !Avg.ero_voice;
					SendDlgItemMessage(hwnd, IDC_CHECK_ERO_VOICE,   BM_SETCHECK, Avg.ero_voice,   0 );
					break;

				case IDC_TEST:

					break;
				case IDC_TEST2:

					break;
			}
			break;
		case WM_CLOSE:
			EndDialog( hwnd, TRUE );
			break;
	}
	return 0;
}
#include <enum.h>

void strcatfn( char *buf, char *mes, ...)
{
	char	buf2[256];
	int	len = wvsprintf( buf2, mes, (char *)(&mes+1));
	_mbscat( (BYTE*)buf, (BYTE*)buf2);

	buf2[0] = 0x0d;	
	buf2[1] = 0x0a;
	buf2[2] = '\0';
	_mbscat( (BYTE*)buf, (BYTE*)buf2);
}

static BOOL CALLBACK D3DSettingProc( HWND hwnd, UINT wmes, UINT wparam, LONG lparam )
{
	int	wx,wy;
	RECT rect;

	switch( wmes ){
		case WM_INITDIALOG:
			if(lparam){
			}

			GetWindowRect( hwnd, &rect );
			wx = (GetSystemMetrics(SM_CXFULLSCREEN)-(rect.right-rect.left))/2;
			wy = (GetSystemMetrics(SM_CYFULLSCREEN)-(rect.bottom-rect.top))/2;

			SetWindowPos( hwnd, HWND_TOP, wx,wy, 0,0, SWP_NOSIZE|SWP_SHOWWINDOW );

			SendDlgItemMessage( hwnd, IDC_RADIO1, BM_SETCHECK, !D3DMain.m_WindowMode, 0 );
			SendDlgItemMessage( hwnd, IDC_RADIO2, BM_SETCHECK, D3DMain.m_WindowMode, 0 );

			SendDlgItemMessage(hwnd, IDC_CHECK1, BM_SETCHECK,DrawSetting.low_level  , 0 );
			SendDlgItemMessage(hwnd, IDC_CHECK2, BM_SETCHECK,DrawSetting.mip_map    , 0 );
			SendDlgItemMessage(hwnd, IDC_CHECK3, BM_SETCHECK,DrawSetting.full_16bit , 0 );
			SendDlgItemMessage(hwnd, IDC_CHECK4, BM_SETCHECK,DrawSetting.txsel05    , 0 );
			SendDlgItemMessage(hwnd, IDC_CHECK5, BM_SETCHECK,DrawSetting.next_start , 0 );
			SendDlgItemMessage(hwnd, IDC_CHECK6, BM_SETCHECK,Avg._Radeon8500 , 0 );

			{
				char	buf[1024];
				int		device=0,i;
				buf[0]='\0';
				strcatfn( buf, "VenderID %s", szVenderID );
				strcatfn( buf, "%s", szCpuName );
				strcatfn( buf, "CpuClock実測値 %dMHz", fCpuClock );



	
	
	

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
			switch( LOWORD(wparam) ){
				case IDOK:
					if(DrawSetting.txsel05)	SetScaleParam( 0.0f );
					else					SetScaleParam( 0.5f );

					EndDialog( hwnd, TRUE );
					break;
				case IDCANCEL:
					EndDialog( hwnd, FALSE );
					break;
				case IDFLASH:
					D3DMain.m_WindowMode=FALSE;

					DrawSetting.low_level = 0;
					DrawSetting.mip_map   = 0;
					DrawSetting.full_16bit= 0;
					DrawSetting.txsel05   = 0;
					DrawSetting.next_start= 0;
					Avg._Radeon8500       = 0;
					SendDlgItemMessage( hwnd, IDC_RADIO1, BM_SETCHECK, !D3DMain.m_WindowMode, 0 );
					SendDlgItemMessage( hwnd, IDC_RADIO2, BM_SETCHECK, D3DMain.m_WindowMode, 0 );

					SendDlgItemMessage(hwnd, IDC_CHECK1, BM_SETCHECK,DrawSetting.low_level  , 0 );
					SendDlgItemMessage(hwnd, IDC_CHECK2, BM_SETCHECK,DrawSetting.mip_map    , 0 );
					SendDlgItemMessage(hwnd, IDC_CHECK3, BM_SETCHECK,DrawSetting.full_16bit , 0 );
					SendDlgItemMessage(hwnd, IDC_CHECK4, BM_SETCHECK,DrawSetting.txsel05    , 0 );
					SendDlgItemMessage(hwnd, IDC_CHECK5, BM_SETCHECK,DrawSetting.next_start , 0 );
					SendDlgItemMessage(hwnd, IDC_CHECK6, BM_SETCHECK,Avg._Radeon8500 , 0 );
					break;

				case IDC_RADIO1:	D3DMain.m_WindowMode = FALSE;	break;
				case IDC_RADIO2:	D3DMain.m_WindowMode = TRUE;	break;

				case IDC_CHECK1:	DrawSetting.low_level = SendDlgItemMessage( hwnd, IDC_CHECK1, BM_GETCHECK, 0,  0 );		break;
				case IDC_CHECK2:	DrawSetting.mip_map = SendDlgItemMessage( hwnd, IDC_CHECK2, BM_GETCHECK, 0,  0 );		break;
				case IDC_CHECK3:	DrawSetting.full_16bit = SendDlgItemMessage( hwnd, IDC_CHECK3, BM_GETCHECK, 0,  0 );	break;
				case IDC_CHECK4:
					if(DrawSetting.txsel05==0){
						if( MessageBox2( hwnd, MB_OKCANCEL,"テクセルの中心位置", "このオプションは、通常時、ピクセルが不自然にぼやけたり、潰れたり\n異常な状態になる方のみチェックしてください。") ==IDOK ){
							DrawSetting.txsel05 = SendDlgItemMessage( hwnd, IDC_CHECK4, BM_GETCHECK, 0,  0 );
						}else{
							SendDlgItemMessage(hwnd, IDC_CHECK4, BM_SETCHECK,DrawSetting.txsel05    , 0 );
						}
					}else{
						DrawSetting.txsel05 = SendDlgItemMessage( hwnd, IDC_CHECK4, BM_GETCHECK, 0,  0 );
					}
					break;
				case IDC_CHECK5:	DrawSetting.next_start = SendDlgItemMessage( hwnd, IDC_CHECK5, BM_GETCHECK, 0,  0 );	break;
				case IDC_CHECK6:	Avg._Radeon8500 = SendDlgItemMessage( hwnd, IDC_CHECK6, BM_GETCHECK, 0,  0 );	break;
			}
			break;
		case WM_DESTROY:
			break;
		case WM_CLOSE:
			DestroyWindow( hwnd );
			break;
	}
	return 0;
}



BOOL CALLBACK VarsinDialogBoxProc( HWND hwnd, UINT wmes, UINT wparam, LONG lparam )
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
BOOL CALLBACK InfoDialogBoxProc( HWND hwnd, UINT wmes, UINT wparam, LONG lparam )
{
	switch( wmes ){
		case WM_CREATE:
		case WM_INITDIALOG:
			{
				char	buf[1024];
				int		device=0,i;
				buf[0]='\0';
				strcatfn( buf, "VenderID %s", szVenderID );
				strcatfn( buf, "%s", szCpuName );
				strcatfn( buf, "CpuClock実測値 %dMHz", fCpuClock );

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


void SetMenuCommand( HWND hwnd, int menu_param )
{
	HMENU	hmenu = GetMenu( hwnd );

	appData.menu=2;
	switch( menu_param ){
		case ID_FULL_MODE:
			
			GD_WindowModeChange(FULLSCREEN_MODE);

			EnableMenuItem( hmenu, ID_FULL_MODE,    MF_GRAYED );
			EnableMenuItem( hmenu, ID_WINDOW_MODE,  MF_ENABLED );

			if(D3DMain.m_WindowMode){
				STD_GetWindowPos( appData.Hwnd, (int*)&window_x, (int*)&window_y );
				window_x = LIM(window_x,0,GetSystemMetrics(SM_CXFULLSCREEN)-800);
				window_y = LIM(window_y,0,GetSystemMetrics(SM_CYFULLSCREEN)-600);

				D3DM_SetWindowXY( window_x, window_y );
			}
			break;
		case ID_WINDOW_MODE:
			
			if(appData.ShowMenu){
				ShowWindow(appData.MenuHwnd, SW_HIDE);
				appData.ShowMenu = FALSE;
			}
			GD_WindowModeChange(WINDOW_MODE);

			EnableMenuItem( hmenu, ID_FULL_MODE,    MF_ENABLED );
			EnableMenuItem( hmenu, ID_WINDOW_MODE,  MF_GRAYED );
			break;




		case ID_EFFECT_CUT:		
			Avg.wait  = MES_CUT;		
			CheckMenuItem( hmenu, ID_EFFECT_CUT,    MF_CHECKED   );	
			CheckMenuItem( hmenu, ID_EFFECT_HIGH,   MF_UNCHECKED ); 
			CheckMenuItem( hmenu, ID_EFFECT_NORMAL, MF_UNCHECKED );	
			CheckMenuItem( hmenu, ID_EFFECT_LOW,    MF_UNCHECKED ); 
			break;
		case ID_EFFECT_HIGH:	
			Avg.wait  = MES_FAST;		
			CheckMenuItem( hmenu, ID_EFFECT_CUT,    MF_UNCHECKED );	
			CheckMenuItem( hmenu, ID_EFFECT_HIGH,   MF_CHECKED   ); 
			CheckMenuItem( hmenu, ID_EFFECT_NORMAL, MF_UNCHECKED );	
			CheckMenuItem( hmenu, ID_EFFECT_LOW,    MF_UNCHECKED ); 
			break;
		case ID_EFFECT_NORMAL:	
			Avg.wait  = MES_NORMAL;		
			CheckMenuItem( hmenu, ID_EFFECT_CUT,    MF_UNCHECKED );	
			CheckMenuItem( hmenu, ID_EFFECT_HIGH,   MF_UNCHECKED ); 
			CheckMenuItem( hmenu, ID_EFFECT_NORMAL, MF_CHECKED   );	
			CheckMenuItem( hmenu, ID_EFFECT_LOW,    MF_UNCHECKED ); 
			break;
		case ID_EFFECT_LOW:		
			Avg.wait  = MES_SLOW;		
			CheckMenuItem( hmenu, ID_EFFECT_CUT,    MF_UNCHECKED );	
			CheckMenuItem( hmenu, ID_EFFECT_HIGH,   MF_UNCHECKED ); 
			CheckMenuItem( hmenu, ID_EFFECT_NORMAL, MF_UNCHECKED );	
			CheckMenuItem( hmenu, ID_EFFECT_LOW,    MF_CHECKED   ); 
			break;

		case ID_SKIP_READ:
			Avg.msg_cut_optin=0;
			CheckMenuItem( hmenu, ID_SKIP_READ,     MF_CHECKED );
			CheckMenuItem( hmenu, ID_SKIP_NOREAD,   MF_UNCHECKED );
			break;
		case ID_SKIP_NOREAD:
			Avg.msg_cut_optin=1;
			CheckMenuItem( hmenu, ID_SKIP_READ,     MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_SKIP_NOREAD,   MF_CHECKED );
			break;

		case ID_TEXT_CUT:		
			Avg.msg_wait = MES_CUT;	
			CheckMenuItem( hmenu, ID_TEXT_CUT,    MF_CHECKED   );	
			CheckMenuItem( hmenu, ID_TEXT_HIGH,   MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_TEXT_NORMAL, MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_TEXT_LOW,    MF_UNCHECKED );
			break;
		case ID_TEXT_HIGH:		
			Avg.msg_wait = MES_FAST;	
			CheckMenuItem( hmenu, ID_TEXT_CUT,    MF_UNCHECKED );	
			CheckMenuItem( hmenu, ID_TEXT_HIGH,   MF_CHECKED   );
			CheckMenuItem( hmenu, ID_TEXT_NORMAL, MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_TEXT_LOW,    MF_UNCHECKED );
			break;
		case ID_TEXT_NORMAL:	
			Avg.msg_wait = MES_NORMAL;	
			CheckMenuItem( hmenu, ID_TEXT_CUT,    MF_UNCHECKED );	
			CheckMenuItem( hmenu, ID_TEXT_HIGH,   MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_TEXT_NORMAL, MF_CHECKED   );
			CheckMenuItem( hmenu, ID_TEXT_LOW,    MF_UNCHECKED );
			break;
		case ID_TEXT_LOW:		
			Avg.msg_wait = MES_SLOW;	
			CheckMenuItem( hmenu, ID_TEXT_CUT,    MF_UNCHECKED );	
			CheckMenuItem( hmenu, ID_TEXT_HIGH,   MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_TEXT_NORMAL, MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_TEXT_LOW,    MF_CHECKED   );
			break;



		case ID_MOV1:
			Avg.mov_lv=0;
			CheckMenuItem( hmenu, ID_MOV1, MF_CHECKED   );
			CheckMenuItem( hmenu, ID_MOV2, MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_MOV3, MF_UNCHECKED );
			break;
		case ID_MOV2:
			Avg.mov_lv=1;
			CheckMenuItem( hmenu, ID_MOV1, MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_MOV2, MF_CHECKED   );
			CheckMenuItem( hmenu, ID_MOV3, MF_UNCHECKED );
			break;
		case ID_MOV3:
			Avg.mov_lv=2;
			CheckMenuItem( hmenu, ID_MOV1, MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_MOV2, MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_MOV3, MF_CHECKED   );
			break;

		case ID_AUTO_RET:
			Avg.auto_modo=0;
			CheckMenuItem( hmenu, ID_AUTO_RET,  MF_CHECKED );
			CheckMenuItem( hmenu, ID_WAIT_MODE, MF_UNCHECKED );
			break;
		case ID_WAIT_MODE:
			Avg.auto_modo=1;
			CheckMenuItem( hmenu, ID_AUTO_RET,  MF_UNCHECKED );
			CheckMenuItem( hmenu, ID_WAIT_MODE, MF_CHECKED );
			break;


		case ID_SOUND:
			DialogBox( appData.Inst, MAKEINTRESOURCE(IDD_DIALOG8), appData.Hwnd, VolumeControlDialogProc );
			break;
		case ID_VER:	
			DialogBox( appData.Inst, MAKEINTRESOURCE(IDD_DIALOG_VERSION), appData.Hwnd, VarsinDialogBoxProc );
			break;

		case ID_INFO:			
			DialogBox( appData.Inst, MAKEINTRESOURCE(IDD_DIALOG1), appData.Hwnd, InfoDialogBoxProc );
			break;








		case ID_LEAFHP:	
			ShellExecute( hwnd, NULL, "http://leaf.aquaplus.co.jp", NULL, NULL, 0 );
			break;
		case ID_MENU_MODE:
			if( MessageBox2( hwnd, MB_YESNO,"フルスクリーン時メニューオフ", "フルスクリーン時、メニューをカーソル上部に持っていっても出なくなります。\n再度表示したい場合は　ALTキー を押してください。\n※注意：意味がわからない人は「いいえ」を選んでください。")==IDYES){
				Avg.menu = !Avg.menu;
				if(Avg.menu)CheckMenuItem( hmenu, ID_MENU_MODE, MF_CHECKED   );
				else		CheckMenuItem( hmenu, ID_MENU_MODE, MF_UNCHECKED );
			}
			break;


		case ID_END2:
		case ID_END:
			if(appData.debug_mouse!=-1){
				EndApp(0);
				DestroyWindow(hwnd);
			}else{
				if( MessageBox2( hwnd, MB_YESNO,"ゲーム終了", "ゲームを終了します。よろしいですか？") ==IDYES ){
					EndApp(0);
		
					DestroyWindow(hwnd);
				}
			}
			break;
	}
}

int	EndBoxFlag=0;

LRESULT CALLBACK WndProc(HWND hWnd,UINT msg,UINT wParam,LONG lParam)
{
	switch(msg){
		case WM_CREATE:
			break;


		case WM_ACTIVATEAPP:
			appData.Active = wParam;
			break;



		case WM_ENTERSIZEMOVE:
			break;
		case WM_EXITSIZEMOVE:
			break;
		case WM_LBUTTONDOWN:		
			break;
		case WM_RBUTTONDOWN:		
			break;
		case 0x020a:
			MUS_RenewWheel( HIWORD(wParam) );
			break;
		case WM_COMMAND:
			SetMenuCommand( hWnd, LOWORD(wParam) );
			break;

		case WM_SIZE:
			if(wParam==SIZE_MAXIMIZED || wParam==SIZE_MINIMIZED || wParam==SIZE_RESTORED){
				GD_DisplayChange();
			}
			break;
		case WM_DISPLAYCHANGE:
			GD_DisplayChange();
			break;
		case WM_TIMER:
			break;
		case WM_KEYDOWN:
			switch(wParam){
				case VK_SPACE:

					break;
				default:
					break;
			}
			break;
		case WM_CLOSE:
			if(appData.debug_mouse!=-1 || !D3DMain.m_WindowMode){
				EndApp(0);
				DestroyWindow(hWnd);
			}else{
				if( MessageBox2( hWnd, MB_YESNO,"ゲーム終了", "ゲームを終了します。よろしいですか？") ==IDYES ){
					EndApp(0);
					DestroyWindow(hWnd);
				}
			}
			break;
		case WM_DESTROY:
			if(D3DMain.m_WindowMode){
				STD_GetWindowPos( appData.Hwnd, (int*)&window_x, (int*)&window_y );
				window_x = LIM(window_x,0,GetSystemMetrics(SM_CXFULLSCREEN)-800);
				window_y = LIM(window_y,0,GetSystemMetrics(SM_CYFULLSCREEN)-600);
			}
			PostQuitMessage(0);
			break;

		case WM_SYSCOMMAND:
			if( !D3DMain.m_WindowMode ){ 
				switch (wParam) {
				
					case SC_KEYMENU :
						return 0;
				}
			}

		default:
			return DefWindowProc(hWnd,msg,wParam,lParam);
	}
	return 0L;
}

#include <time.h>




int WINAPI WinMain(HINSTANCE hInst,HINSTANCE hPrevInst,LPSTR lpCmdLine,int nCmdShow)
{

	SetDebug( TRUE );
	STD_CheckCPU( );						
	STD_CheckOS();							

	int	mov_cpu;
	if(fCpuClock>2000){
		mov_cpu=2;
	}else if(fCpuClock>1300){
		mov_cpu=1;
	}else{
		mov_cpu=0;
	}
#if TAIKEN_FLAG
	mov_cpu=0;
#endif

	SetIniFileName( NULL, "D3D" );
	DrawSetting.low_level  = ReadIniFileAmount( "low_level",  DrawSetting.low_level );
	DrawSetting.mip_map    = ReadIniFileAmount( "mip_map",    DrawSetting.mip_map );
	DrawSetting.full_16bit = ReadIniFileAmount( "full_16bit", DrawSetting.full_16bit );
	DrawSetting.txsel05    = ReadIniFileAmount( "txsel05",    DrawSetting.txsel05 );
	DrawSetting.next_start = ReadIniFileAmount( "next_start", DrawSetting.next_start );
	D3DMain.m_WindowMode = ReadIniFileAmount( "window_mode", FALSE );

	SetIniFileName( NULL, "SYSTEM" );
	Avg.wheel         = ReadIniFileAmount( "wheel",         Avg.wheel );
	Avg.wait          = ReadIniFileAmount( "wait",     (int)(Avg.wait*2) )/2.0f;
	Avg.msg_wait      = ReadIniFileAmount( "msg_wait", (int)(Avg.msg_wait*2) )/2.0f;
	Avg.msg_cut_optin = ReadIniFileAmount( "msg_cut_optin", Avg.msg_cut_optin );
	Avg.bgm           = ReadIniFileAmount( "bgm",           Avg.bgm );
	Avg.se            = ReadIniFileAmount( "se",            Avg.se );
	Avg.voice         = ReadIniFileAmount( "voice",         Avg.voice );
	Avg.bgm_vol       = ReadIniFileAmount( "bgm_vol",       Avg.bgm_vol );
	Avg.se_vol        = ReadIniFileAmount( "se_vol",        Avg.se_vol );
	Avg.voice_vol     = ReadIniFileAmount( "voice_vol",     Avg.voice_vol );
	Avg.mov_lv        = ReadIniFileAmount( "mov_lv",        mov_cpu );
	Avg.ero_voice     = ReadIniFileAmount( "ero_voice",     1 );
	Avg.menu          = ReadIniFileAmount( "menu",     0 );
	Avg.auto_modo     = ReadIniFileAmount( "auto_modo",     0 );

	Avg._Radeon8500     = ReadIniFileAmount( "_Radeon8500",     0 );
	

	appData.debug_mouse = ReadIniFileAmount( "debug_mouse", -1 );
	appData.debug_mouse = -1;

	if(appData.debug_mouse==-1){
		DrawSetting.next_start = 0;
	}

	RECT rect;
	SetRect( &rect, 0, 0, WIN_SIZE_W, WIN_SIZE_H );
	AdjustWindowRect( &rect, WINDOWMODE_STYLE, TRUE );
	window_x = ReadIniFileAmount( "window_x", (GetSystemMetrics(SM_CXFULLSCREEN)-(rect.right-rect.left))/2 );
	window_y = ReadIniFileAmount( "window_y", (GetSystemMetrics(SM_CYFULLSCREEN)-(rect.bottom-rect.top))/2 );
	window_x = LIM(window_x,0,GetSystemMetrics(SM_CXFULLSCREEN)-800);
	window_y = LIM(window_y,0,GetSystemMetrics(SM_CYFULLSCREEN)-600);

	if( (GetAsyncKeyState(VK_NONCONVERT)&0x8000) || !DrawSetting.next_start){
		if( !DialogBoxParam( appData.Inst, MAKEINTRESOURCE(IDD_DIALOG_D3D_SETTING), appData.Hwnd, D3DSettingProc, 0 ) ){
			return 0;
		}
	}

	MSG msg;
	HANDLE	hAccel = LoadAccelerators( hInst, MAKEINTRESOURCE(IDR_ACCELERATOR1) );

	CoInitialize(NULL);
	
	HANDLE	hMutex;
	HWND	hWin;
	hMutex = CreateMutex(NULL,TRUE,"TEARS_TO_TIARA");
	if(ERROR_ALREADY_EXISTS == GetLastError() ){
		if( lpCmdLine[0]!='\0' ){
			hWin=FindWindow( appData.ClassName, NULL );
			if( hWin ){
				SendMessage( hWin, WM_COMMAND, ID_END2, 0 );
			}
		}else{
			return FALSE;
		}
	}
	
	appData.Inst = hInst;



	if( lpCmdLine[0]!='\0' ){
		_mbscpy( (BYTE*)GoScript, (BYTE*)lpCmdLine );
	}



	if( !StartApp() ){
		return FALSE;	
	}

	WINNLSEnableIME(appData.Hwnd,FALSE);	

	srand( timeGetTime() );

	TIMECAPS tc;
	timeGetDevCaps(&tc,sizeof(tc));
	timeBeginPeriod(tc.wPeriodMin);

	SetVoiceDialog( appData.debug_mouse!=-1 );

	ShowWindow( appData.Hwnd, nCmdShow );
	UpdateWindow( appData.Hwnd );
	SetFocus( appData.Hwnd );
	appData.flash=1;

	
	while(TRUE){
		if(PeekMessage(&msg,NULL,0,0,PM_NOREMOVE)){
			if(!GetMessage(&msg,NULL,0,0))
				break;
			if( TranslateAccelerator( appData.Hwnd, (HACCEL)hAccel, &msg ) ) continue;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}


				MAIN_Loop(60);

				if(appData.debug_mouse!=-1){
					static char	BakScriptName[256]="";
					if( _mbscpy( (BYTE*)BakScriptName, (BYTE*)GoScript ) ){
						SetWindowText( appData.Hwnd, GoScript );
						_mbscpy( (BYTE*)BakScriptName, (BYTE*)GoScript );
					}
				}





	}

	timeEndPeriod(1);

	EndApp(1);
	timeEndPeriod(tc.wPeriodMin);

	SetIniFileName( NULL, "D3D" );
	WriteIniFileAmount( "low_level",  DrawSetting.low_level );
	WriteIniFileAmount( "mip_map",    DrawSetting.mip_map );
	WriteIniFileAmount( "full_16bit", DrawSetting.full_16bit );
	WriteIniFileAmount( "txsel05",    DrawSetting.txsel05 );
	WriteIniFileAmount( "next_start", DrawSetting.next_start );
	WriteIniFileAmount( "window_mode", D3DMain.m_WindowMode );

	SetIniFileName( NULL, "SYSTEM" );
	WriteIniFileAmount( "wheel",         Avg.wheel );
	WriteIniFileAmount( "wait",     (int)(Avg.wait*2) );
	WriteIniFileAmount( "msg_wait", (int)(Avg.msg_wait*2) );
	WriteIniFileAmount( "msg_cut_optin", Avg.msg_cut_optin );
	WriteIniFileAmount( "bgm",           Avg.bgm );
	WriteIniFileAmount( "se",            Avg.se );
	WriteIniFileAmount( "voice",         Avg.voice );
	WriteIniFileAmount( "bgm_vol",       Avg.bgm_vol );
	WriteIniFileAmount( "se_vol",        Avg.se_vol );
	WriteIniFileAmount( "voice_vol",     Avg.voice_vol );
	WriteIniFileAmount( "mov_lv",        Avg.mov_lv );
	WriteIniFileAmount( "ero_voice",     Avg.ero_voice );
	WriteIniFileAmount( "menu",          Avg.menu );
	WriteIniFileAmount( "auto_modo",     Avg.auto_modo );

	WriteIniFileAmount( "_Radeon8500",   Avg._Radeon8500 );

	WriteIniFileAmount( "debug_mouse",   appData.debug_mouse );

	WriteIniFileAmount( "window_x", window_x );
	WriteIniFileAmount( "window_y", window_y );

	return msg.wParam;
}



