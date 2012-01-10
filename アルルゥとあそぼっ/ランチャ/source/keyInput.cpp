#include "ClCommon.h"
#include "backLog.h"
#include "keyInput.h"

KeyState				keyState;
LPDIRECTINPUT8			lpDInput;
LPDIRECTINPUTDEVICE8	g_pDIDevice[MAX_DEV] = {NULL,NULL,NULL,NULL,NULL,NULL};
DIACTIONFORMAT			g_DIActionFormat;
int g_iDIDeviceNum = 0;
DIACTION g_GameAction[] =
{
	{INPUT_LEFTRIGHT,	DIAXIS_STRATEGYR_LATERAL,	0, "Turn",},
	{INPUT_UPDOWN,		DIAXIS_STRATEGYR_MOVE,		0, "Move",},
	{INPUT_ACTION,		DIBUTTON_ANY(0),			0, "Action",},
	{INPUT_CANCEL,		DIBUTTON_ANY(1),			0, "Cancel",},
	{INPUT_CAMP,		DIBUTTON_ANY(2),			0, "Camp",},
	{INPUT_FAIRY,		DIBUTTON_ANY(3),			0, "Fairy",},
	{INPUT_LEFTTRIGGER,	DIBUTTON_ANY(4),			0, "Change Magic D"},
	{INPUT_RIGHTTRIGGER,DIBUTTON_ANY(5),			0, "Change Magic U"},
	{INPUT_MAP,			DIBUTTON_ANY(6),			0, "Map",},
	{INPUT_SKIP,		DIBUTTON_ANY(7),			0, "Skip",},
	{INPUT_DEBUG,		DIBUTTON_ANY(8),			0, "Pause",},
	{INPUT_END,			DIBUTTON_ANY(9),			0, "End"},
	{INPUT_LEFT,			DIKEYBOARD_LEFT,	0, "Turn left", },
	{INPUT_LEFT,			DIKEYBOARD_NUMPAD4,	0, "Turn left", },
	{INPUT_RIGHT,			DIKEYBOARD_RIGHT,	0, "Turn right", },
	{INPUT_RIGHT,			DIKEYBOARD_NUMPAD6,	0, "Turn right", },
	{INPUT_LEFTUP,			DIKEYBOARD_NUMPAD7,	0, "Turn left up", },
	{INPUT_RIGHTUP,			DIKEYBOARD_NUMPAD9,	0, "Turn right up", },
	{INPUT_LEFTDOWN,		DIKEYBOARD_NUMPAD1,	0, "Turn left down", },
	{INPUT_RIGHTDOWN,		DIKEYBOARD_NUMPAD3,	0, "Turn right down", },
	{INPUT_UP,				DIKEYBOARD_UP,		0, "Forward thrust", },
	{INPUT_UP,				DIKEYBOARD_NUMPAD8,	0, "Forward thrust", },
	{INPUT_DOWN,			DIKEYBOARD_DOWN,	0, "Reverse thrust", },
	{INPUT_DOWN,			DIKEYBOARD_NUMPAD2,	0, "Reverse thrust", },
	{INPUT_ACTION,			DIKEYBOARD_SPACE,	0, "Action", },
	{INPUT_ACTION,			DIKEYBOARD_RETURN,	0, "Action", },
	{INPUT_CANCEL,			DIKEYBOARD_ESCAPE,	0, "Cancel",},
	{INPUT_SKIP,			DIKEYBOARD_LCONTROL,0, "Skip", },
	{INPUT_SKIP,			DIKEYBOARD_RCONTROL,0, "Skip", },
	{INPUT_LEFTTRIGGER,		DIKEYBOARD_LMENU,	0, "Change Magic D", },
	{INPUT_RIGHTTRIGGER,	DIKEYBOARD_RMENU,	0, "Change Magic U", },
	{INPUT_DEBUG,			DIKEYBOARD_PAUSE,	0, "Pause",},
	{INPUT_MAP,				DIKEYBOARD_M,		0, "Map",},
	{INPUT_FAIRY,			DIKEYBOARD_F,		0, "Fairy", },
	{INPUT_CAMP,			DIKEYBOARD_C,		0, "Camp",},
	{INPUT_MOUSEA,			DIMOUSE_BUTTON0,	0, "mouse A", },
	{INPUT_MOUSEB,			DIMOUSE_BUTTON1,	0, "mouse B", },
	{INPUT_MOUSEC,			DIMOUSE_BUTTON2,	0, "mouse C", },
};	// g_GameAction


GUID g_AppGuid = 
{ 0x232f4935, 0x8182, 0x4b1d, { 0x91, 0xe3, 0xf3, 0x9e, 0x8, 0x81, 0x80, 0x9c } };

BOOL CALLBACK EnumDevicesBySemanticsCallback(
		LPCDIDEVICEINSTANCE lpddi, LPDIRECTINPUTDEVICE8  lpdid, 
		DWORD  dwFlags, DWORD  dwRemaining, LPVOID pvRef)
{
	HRESULT hr;
	hr = lpdid->BuildActionMap(&g_DIActionFormat, NULL,  DIDBAM_INITIALIZE );
	if (FAILED(hr)){
		return DIENUM_CONTINUE;
	}
	hr = lpdid->SetActionMap(&g_DIActionFormat, NULL, DIDSAM_DEFAULT);
	if (FAILED(hr)){
		return DIENUM_CONTINUE;
	}
	hr = lpdid->SetCooperativeLevel(sysInf.hWnd,DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	lpdid->AddRef();
	g_pDIDevice[g_iDIDeviceNum++] = lpdid;

	if (MAX_DEV <= g_iDIDeviceNum)
		DIENUM_STOP;

	return DIENUM_CONTINUE;
} // EnumDevicesBySemanticsCallback

BOOL InitDirectInput(HINSTANCE hInstApp)
{
	int		i;
	HRESULT	hr;
	hr = DirectInput8Create(hInstApp,DIRECTINPUT_VERSION,IID_IDirectInput8A,(void **)&lpDInput,NULL);
	if(hr != DI_OK){
		return FALSE;
	}
	int numder_of_actions = sizeof(g_GameAction) / sizeof(g_GameAction[0]);
	ZeroMemory(&g_DIActionFormat,sizeof(DIACTIONFORMAT));
	g_DIActionFormat.dwSize        = sizeof(DIACTIONFORMAT);
	g_DIActionFormat.dwActionSize  = sizeof(DIACTION);
	g_DIActionFormat.dwDataSize    = numder_of_actions * sizeof(DWORD);
	g_DIActionFormat.dwNumActions  = numder_of_actions;
	g_DIActionFormat.guidActionMap = g_AppGuid;
	g_DIActionFormat.dwGenre       = DIVIRTUAL_STRATEGY_ROLEPLAYING;
	g_DIActionFormat.rgoAction     = g_GameAction;
	g_DIActionFormat.dwBufferSize  = 100;
	g_DIActionFormat.lAxisMin      = -32768;
	g_DIActionFormat.lAxisMax      = 32767;
	lstrcpy(g_DIActionFormat.tszActionMap, className);

	g_iDIDeviceNum = 0;
	hr = lpDInput->EnumDevicesBySemantics(NULL, &g_DIActionFormat,
				EnumDevicesBySemanticsCallback, NULL, DIEDBSFL_ATTACHEDONLY);
	if (FAILED(hr)){
		return FALSE;
	}
	for(i=0;i<g_iDIDeviceNum;i++){
		g_pDIDevice[i]->Acquire();
		g_pDIDevice[i]->Poll();
	}
	return TRUE;
} // InitDirectInput

void clearPadState()
{
	lookPadState();
	ZeroMemory(&keyState,sizeof(keyState));
} // clearPadState

void checkMouseState()
{
	if(sysInf.zDelta > 0){
		keyState.push_up = 1;
		keyState.push_down = 0;
	}else if(sysInf.zDelta < 0){
		keyState.push_down = 1;
		keyState.push_up = 0;
		if(sysInf.execMode==event_exec_mode){
			keyState.push_action = on;
		}
	}
	sysInf.zDelta = 0;
} // checkMouseState

void clearAllPushData()
{
	keyState.push_action = keyState.push_cancel = keyState.push_skip = keyState.push_test = keyState.push_fairyAttack = 0;
	keyState.push_left = keyState.push_right = keyState.push_up = keyState.push_down = 0;
	keyState.push_rightTrigger = keyState.push_leftTrigger = keyState.push_camp = keyState.push_start = keyState.push_back = 0;
	keyState.push_mouse_a = keyState.push_mouse_b = keyState.push_mouse_c = 0;
} // clearAllPushData

void lookPadRepeat()
{
	if(keyState.data_right){
		if(timeGetTime() > keyState.repeatCntR){
			keyState.push_right = 1;
			keyState.repeatCntR = timeGetTime() +80;
		}
	}
	if(keyState.data_left){
		if(timeGetTime() > keyState.repeatCntL){
			keyState.push_left = 1;
			keyState.repeatCntL = timeGetTime() +80;
		}
	}
	if(keyState.data_up){
		if(timeGetTime() > keyState.repeatCntU){
			keyState.push_up = 1;
			keyState.repeatCntU = timeGetTime() +80;
		}
	}
	if(keyState.data_down){
		if(timeGetTime() > keyState.repeatCntD){
			keyState.push_down = 1;
			keyState.repeatCntD = timeGetTime() +80;
		}
	}
} // lookPadRepeat

void lookPadState()
{
	KeyState		oldState = keyState;
	float			oldDeg = keyState.deg;

	int			i,j;
	BOOL		bMove,bUseKeyMove = FALSE;
	HRESULT		hr;
	static float		sThumbLX = 0,sThumbLY=0,leng;

	point = sysInf.msPoint;
	GetCursorPos(&sysInf.msPoint);
	ScreenToClient(sysInf.hWnd,&sysInf.msPoint);
	if(sysInf.full_screen){
		if(!sysInf.bShowMenu){
			if(sysInf.msPoint.y<GetSystemMetrics(SM_CYMENUSIZE)){
				ShowWindow(sysInf.hMenuWnd,SW_SHOW);
				UpdateWindow(sysInf.hMenuWnd);
				sysInf.bShowMenu = on;
			}
		}else{
			if(sysInf.msPoint.y>GetSystemMetrics(SM_CYMENUSIZE)){
				ShowWindow(sysInf.hMenuWnd, SW_HIDE);
				sysInf.bShowMenu = off;
				return;
			}
		}
	}
	if(sysInf.bShowMenu)return;
	clearAllPushData();
	for(i=0;i<g_iDIDeviceNum; i++)
	{
		DIDEVICEOBJECTDATA od[100];
		DWORD dwItems = 100;
		hr = g_pDIDevice[i]->Poll(); 
		if (FAILED(hr)){
			hr = g_pDIDevice[i]->Acquire();
			while (hr==DIERR_INPUTLOST)
				hr = g_pDIDevice[i]->Acquire();
		}
		hr = g_pDIDevice[i]->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), od, &dwItems, 0);
		if(DI_OK!=hr && DI_BUFFEROVERFLOW!=hr)
			continue;
		bMove = FALSE;
		for (j=0;j<dwItems;j++){
			BOOL bState = (od[j].dwData != 0 ) ? TRUE : FALSE;
			switch(od[j].uAppData){
			  case INPUT_LEFTRIGHT:
				bMove = TRUE;
				sThumbLX = float(int(od[j].dwData));
				break;
			  case INPUT_UPDOWN:
				bMove = TRUE;
				sThumbLY = float(-int(od[j].dwData));
				break;
			  case INPUT_LEFT:
				bUseKeyMove = TRUE;
				keyState.data_kLeft = bState;
				break;
			  case INPUT_RIGHT:
				bUseKeyMove = TRUE;
				keyState.data_kRight = bState;
				break;
			  case INPUT_UP:
				bUseKeyMove = TRUE;
				keyState.data_kUp = bState;
				break;
			  case INPUT_DOWN:
				bUseKeyMove = TRUE;
				keyState.data_kDown = bState;
				break;
			  case INPUT_LEFTUP:
				bUseKeyMove = TRUE;
				keyState.data_kLeftUp = bState;
				break;
			  case INPUT_RIGHTUP:
				bUseKeyMove = TRUE;
				keyState.data_kRightUp = bState;
				break;
			  case INPUT_LEFTDOWN:
				bUseKeyMove = TRUE;
				keyState.data_kLeftDown = bState;
				break;
			  case INPUT_RIGHTDOWN:
				bUseKeyMove = TRUE;
				keyState.data_kRightDown = bState;
				break;
			  case INPUT_ACTION:
				keyState.push_action = bState;
				if(keyState.push_action){
					keyState.data_action = TRUE;
				}else{
					keyState.data_action = FALSE;
				}
				break;
			  case INPUT_MOUSEA:
				keyState.push_mouse_a = bState;
				keyState.push_action = bState;
				if(keyState.push_mouse_a){
					keyState.data_action = TRUE;
				}else{
					keyState.data_action = FALSE;
				}
				break;
			  case INPUT_MOUSEB:
				keyState.push_mouse_b = bState;
				keyState.push_cancel = bState;
				if(keyState.push_mouse_b){
					keyState.data_cancel = TRUE;
				}else{
					keyState.data_cancel = FALSE;
				}
				break;
			  case INPUT_MOUSEC:
				keyState.push_mouse_c = bState;
				keyState.push_cancel = bState;
				if(keyState.push_mouse_c){
					keyState.data_cancel = TRUE;
				}else{
					keyState.data_cancel = FALSE;
				}
				break;
			  case INPUT_CANCEL:
				keyState.push_cancel = bState;
				if(keyState.push_cancel){
					keyState.data_cancel = TRUE;
				}else{
					keyState.data_cancel = FALSE;
				}
				break;
			  case INPUT_SKIP:
				keyState.push_skip = bState;
				if(sysInf.bAllSkip){
					if(keyState.push_skip){
						keyState.data_skip = TRUE;
					}else{
						keyState.data_skip = FALSE;
					}
				}else{
					keyState.data_skip = FALSE;
				}
				break;
			  case INPUT_FAIRY:
				keyState.push_fairyAttack = bState;
				break;
			  case INPUT_CAMP:
				keyState.push_camp = bState;
				break;
			  case INPUT_LEFTTRIGGER:
				keyState.push_leftTrigger = bState;
				break;
			  case INPUT_RIGHTTRIGGER:
				keyState.push_rightTrigger = bState;
				break;
			  case INPUT_DEBUG:
				keyState.push_start = bState;
				break;
			  case INPUT_END:
				if(bState){
				}
				break;
			}
		}
		if(bUseKeyMove){
			bMove = TRUE;
			if(keyState.data_kLeft || keyState.data_kLeftUp || keyState.data_kLeftDown){
				sThumbLX = -32767;
			}else if(keyState.data_kRight || keyState.data_kRightUp || keyState.data_kRightDown){
				sThumbLX = 32767;
			}else{
				sThumbLX = 0;
			}
			if(keyState.data_kUp || keyState.data_kLeftUp || keyState.data_kRightUp){
				sThumbLY = 32767;
			}else if(keyState.data_kDown || keyState.data_kLeftDown || keyState.data_kRightDown){
				sThumbLY = -32767;
			}else{
				sThumbLY = 0;
			}
		}
		if(bMove){
			keyState.data_left = keyState.data_right = keyState.data_up = keyState.data_down = 0;
			leng = sqrtf(square(sThumbLX) +square(sThumbLY));
			if(leng > 32768)leng = 32768;
			if(leng > 20000){
				keyState.data_move = 1;
				keyState.moveLeng = leng;
			}else{
				keyState.data_move = 0;
				keyState.moveLeng = 0;
			}
			if(0==keyState.data_mouse_b){
				if(sThumbLX >20000){
					if(oldState.data_right==0){
						keyState.push_right = 1;
						keyState.repeatCntR = timeGetTime() +1000;
					}
					keyState.data_right = 1;
				}else if(sThumbLX < -20000){
					if(oldState.data_left==0){
						keyState.push_left = 1;
						keyState.repeatCntL = timeGetTime() +1000;
					}
					keyState.data_left = 1;
				}
				if(sThumbLY >20000){
					if(oldState.data_up==0){
						keyState.push_up = 1;
						keyState.repeatCntU = timeGetTime() +1000;
					}
					keyState.data_up = 1;
				}else if(sThumbLY < -20000){
					if(oldState.data_down==0){
						keyState.push_down = 1;
						keyState.repeatCntD = timeGetTime() +1000;
					}
					keyState.data_down = 1;
				}
			}
		}
	}
	checkMouseState();
	if(keyState.push_up){
		switch(sysInf.execMode){
			case event_exec_mode:
			backLog.DispInit();
			break;
		}
	}
	if(keyState.push_action && sysInf.bReadSkip){
		sysInf.bReadSkip = FALSE;
		CheckMenuItem( sysInf.hMenu, ID_READSKIP,   MF_UNCHECKED );
	}
} // lookPadState

