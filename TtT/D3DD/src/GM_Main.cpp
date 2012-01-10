#include <windows.h>
#include <mouse.h>
#include <keybord.h>


#include "Escript.h"

#include "STD_Windows.h"
#include "STD_Debug.h"
#include "STD_File.h"

#include "main.h"

#include <GlobalDraw.h>
#include "dispSprite.h"
#include "resource.h"

#include "GM_Battle.h"
#include "GM_World.h"

#include "GM_Main.h"
#include "GM_Char.h"

#include "GM_Avg.h"


int	NowTime = 0;
int	GlobalCount = 0;
int	DrawCount = 0;
int	MainFrame = 0;
int	DrawFrame = 0;

GAME_KEY	GameKey;

DRAW_SETTING	DrawSetting = { FALSE, 0 };

static EXEC_DATA 	ScriptData;		
static int			ScriptFlag = 0;


#define GAME_WINDOW_MAX	16

typedef struct{
	long	flag;
	long	gno;
	long	gmax;
	long	wtype;
	long	layer;
	long	disp;

	long	str;
	long	sbno;

	long	btl;

	float	wx;
	float	wy;
	float	ww;
	float	wh;

	float	tx;
	float	ty;
	
	float	zoom_x;
	float	zoom_y;

	long	a,r,g,b;
}GAME_WINDOW;

GAME_WINDOW		GameWindow[GAME_WINDOW_MAX];

void ResetGameWindow_All( void )
{
	int	i;
	for(i=0;i<GAME_WINDOW_MAX;i++){
		ResetGameWindow( i );
	}
}

void ResetGameWindow( int wno )
{
	int	i;
	int	gno = GameWindow[wno].gno;
	if(GameWindow[wno].flag){
		for(i=0;i<2;i++){
			GD_ResetGraph( gno+i );
		}
		if(GameWindow[wno].str){
			GD_ResetGraph( gno+2 );
			GD_ReleaseTexture( GameWindow[wno].sbno );
		}
	}
	ZeroMemory( &GameWindow[wno], sizeof(GAME_WINDOW) );
}

void SetGameWindow( int wno, int gno, int bno, int layer, int disp, int wtype, int btl )
{
	int	i;
	ResetGameWindow(wno );

	GameWindow[wno].flag  = TRUE;
	GameWindow[wno].gno   = gno;
	GameWindow[wno].layer = layer;
	GameWindow[wno].disp  = disp;

	GameWindow[wno].btl  = btl;

	GameWindow[wno].a  = 255;
	GameWindow[wno].r  = 128;
	GameWindow[wno].g  = 128;
	GameWindow[wno].b  = 128;

	GameWindow[wno].wx = 0;
	GameWindow[wno].wy = 0;
	GameWindow[wno].ww = 100;
	GameWindow[wno].wh = 100;
	
	GameWindow[wno].zoom_x = 1.0f;
	GameWindow[wno].zoom_y = 1.0f;
	GameWindow[wno].wtype  = wtype;

	if(GameWindow[wno].wtype!=-1){
		GameWindow[wno].gmax=2;

		GD_SetGraph( gno+1, bno, layer, disp );	
		GD_SetGraphScaleParam( gno+1, SCALE_HIGHT );
		GD_SetGraphLayer( gno+1, layer+1 );
		GD_SetGraphSrcRect( gno+1, 360, 432 + GameWindow[wno].wtype*24, 168, 24 );
		GD_SetGraphBrightNo( gno+1, 1 );
	}else{
		GameWindow[wno].gmax=1;
	}
	GD_SetGraphx9( gno, bno, layer, disp );	
	GD_SetGraphScaleParam( gno, SCALE_HIGHT );
	GD_SetGraphBrightNo( gno, 1 );

	SetGameWindowBackType( wno, btl );


}
void SetGameWindowBackType( int wno, int btl )
{
	int	gno = GameWindow[wno].gno;
	GameWindow[wno].btl  = btl;
	switch(btl){
		default:
		case 0:	GD_SetGraphSrcRect( gno+0, 952,    128,    24*3, 24*3 );	break;
		case 1:	GD_SetGraphSrcRect( gno+0, 952,    128+72, 24*3, 24*3 );	break;
		case 2:	GD_SetGraphSrcRect( gno+0, 952-72, 128,    24*3, 24*3 );	break;
		case 3:	GD_SetGraphSrcRect( gno+0, 952-72, 128+72, 24*3, 24*3 );	break;
	}
}

void SetGameWindowText( int wno, char *str, int bno, float x, float y, int w  )
{
	int	gno = GameWindow[wno].gno;

	if(GameWindow[wno].flag){
		if(!GameWindow[wno].str){
			GameWindow[wno].str  = 1;
			GameWindow[wno].sbno = bno;
			GameWindow[wno].gmax = 3;
			GameWindow[wno].tx = x;
			GameWindow[wno].ty = y;

			if(GameWindow[wno].btl){
				if(w){
					GD_CreateTextTexture( bno, str, 24, w, (GameWindow[wno].wh-48)/24+1, 0, 0, 2, 255, 255, 255, 52, 12, 13 );
				}else{
					GD_CreateTextTexture( bno, str, 24, (GameWindow[wno].ww-48)/24, (GameWindow[wno].wh-48)/24+1, 0, 0, 2, 255, 255, 255, 52, 12, 13 );
				}
			}else{
				if(w){
					GD_CreateTextTexture( bno, str, 24, w, (GameWindow[wno].wh-48)/24+1, 0, 0, 0, 91, 55, 21 );
				}else{
					GD_CreateTextTexture( bno, str, 24, (GameWindow[wno].ww-48)/24, (GameWindow[wno].wh-48)/24+1, 0, 0, 0, 91, 55, 21 );
				}
			}
			GD_SetGraph( gno+2, bno, GameWindow[wno].layer+2, GameWindow[wno].disp );
			GD_SetGraphRect( gno+2, GameWindow[wno].wx+x+24, GameWindow[wno].wy+y+24, 0, 0, (GameWindow[wno].ww-48), (GameWindow[wno].wh-48) );
			GD_SetGraphCenter( gno+2, GameWindow[wno].wx+GameWindow[wno].ww/2, GameWindow[wno].wy+GameWindow[wno].wh/2 );
			GD_SetGraphScaleParam( gno+2, SCALE_HIGHT );

			GD_SetGraphBrightNo( gno+2, 1 );
		}else{
			GD_UpdateTextTexture( GameWindow[wno].sbno, str );
		}
	}
}

void SetGameWindowRect( int wno, float wx, float wy, float ww, float wh, int src_move  )
{
	int	i;
	float	wx1, wx2;
	float	wy1, wy2;
	float	ww1, ww2, ww3;
	float	wh1, wh2, wh3;
	int		gno = GameWindow[wno].gno;

	if(GameWindow[wno].flag){
		GameWindow[wno].wx = wx;
		GameWindow[wno].wy = wy;
		GameWindow[wno].ww = ww;
		GameWindow[wno].wh = wh;

		if(GameWindow[wno].wtype!=-1){
			GD_SetGraphDstRect( gno+1, wx+24, wy, 168, 24 );
		}
		GD_SetGraphDstRect( gno+0, wx,  wy, ww, wh );

		if(GameWindow[wno].str){
			GD_SetGraphDstMove( gno+2, wx+GameWindow[wno].tx+24, wy+GameWindow[wno].ty+24 );
		}

		for(i=0;i<GameWindow[wno].gmax;i++){
			GD_SetGraphCenter( gno+i, wx+ww/2, wy+wh/2 );
		}
	}
}
void SetGameWindowMove( int wno, float wx, float wy, int src_move )
{
	SetGameWindowRect( wno, wx, wy, GameWindow[wno].ww, GameWindow[wno].wh, src_move );
}

void SetGameWindowCenter( int wno, float cx, float cy )
{
	int	i;
	int	gno = GameWindow[wno].gno;

	for(i=0;i<GameWindow[wno].gmax;i++){
		GD_SetGraphCenter( gno+i, cx, cy );
	}
}

void SetGameWindowZoom( int wno, float zoom )
{
	int	i;
	int	gno = GameWindow[wno].gno;

	if(GameWindow[wno].flag){
		GameWindow[wno].zoom_x=zoom;
		GameWindow[wno].zoom_y=zoom;
		for(i=0;i<GameWindow[wno].gmax;i++){
			GD_SetGraphZoom( gno+i, zoom );
		}
	}
}
void SetGameWindowZoomXY( int wno, float zoom_x, float zoom_y )
{
	int	i;
	int	gno = GameWindow[wno].gno;

	if(GameWindow[wno].flag){
		GameWindow[wno].zoom_x=zoom_x;
		GameWindow[wno].zoom_y=zoom_y;
		for(i=0;i<GameWindow[wno].gmax;i++){
			GD_SetGraphZoomXY( gno+i, zoom_x, zoom_y );
		}
	}
}
void SetGameWindowAlpha( int wno, int a )
{
	int	i;
	int	gno = GameWindow[wno].gno;
	a=LIM(a,0,255);

	if(GameWindow[wno].flag){
		GameWindow[wno].a=a;
		for(i=0;i<GameWindow[wno].gmax;i++){
			GD_SetGraphAlpha( gno+i, a );
		}
	}
}

void SetGameWindowColor( int wno, int a, int r, int g, int b )
{
	int	i;
	int	gno = GameWindow[wno].gno;

	a=LIM(a,0,255);
	r=LIM(r,0,255);
	g=LIM(g,0,255);
	b=LIM(b,0,255);

	if(GameWindow[wno].flag){
		GameWindow[wno].a=a;
		GameWindow[wno].r=r;
		GameWindow[wno].g=g;
		GameWindow[wno].b=b;
		for(i=0;i<GameWindow[wno].gmax;i++){
			GD_SetGraphColor( gno+i, a, r, g, b );
		}
	}
}
void SetGameWindowDisp( int wno, int disp )
{
	int	i;
	int	gno = GameWindow[wno].gno;

	if(GameWindow[wno].flag){
		GameWindow[wno].disp=disp;
		for(i=0;i<GameWindow[wno].gmax;i++){
			GD_SetGraphDisp( gno+i, disp );
		}
	}
}



void AVG_GetGameKey(void)
{
	int	wclick   = KeyCond.btn.enter || MUS_GetMouseButton( MOUSE_LBUTTON);
	int	wcansel  = KeyCond.btn.bs    || MUS_GetMouseButton( MOUSE_RBUTTON);
	int	wheel    = MUS_GetMouseWheel();
	int	mx,my;
	MUS_GetMousePos( &GameKey.mx, &GameKey.my );

	if( KeyCond.trg.enter && KeyCond.btn.alt ){
		KeyCond.trg.enter=0;
	}





	GameKey.click   = KeyCond.trg.enter || MUS_GetMouseTrigger( MOUSE_LBUTTON);
	GameKey.cansel  = KeyCond.trg.esc   || KeyCond.trg.bs    || MUS_GetMouseTrigger( MOUSE_RBUTTON);






	switch(1){
		case 0:
			GameKey.diswin        = KeyCond.trg.space || MUS_GetMouseTrigger( MOUSE_MBUTTON);

			GameKey.mes_cut       = KeyCond.btn.ctrl;
			GameKey.mes_cut_mode  = KeyCond.trg.shift;	
			break;
		case 1:
			GameKey.diswin        = KeyCond.trg.space;	
			
			GameKey.mes_cut       = KeyCond.btn.ctrl  || MUS_GetMouseButton(MOUSE_MBUTTON);
			GameKey.mes_cut_mode  = KeyCond.trg.shift;
			break;
	}






	GameKey.end     = KeyCond.trg.end    && !wclick;	
	GameKey.home    = KeyCond.trg.home   && !wclick;	
	if( wheel>0 ){
		GameKey.pup     = 1 && !wclick;	
		GameKey.pdown   = 0 && !wclick;	
	}else if( wheel<0 ){
		GameKey.pup     = 0 && !wclick;	
		GameKey.pdown   = 1 && !wclick;	
	}else{
		GameKey.pup     = KeyCond.btrg.pup   && !wclick;	
		GameKey.pdown   = KeyCond.btrg.pdown && !wclick;	
	}

	if(GameKey.click&&GameKey.cansel){
		GameKey.click  = OFF;
		GameKey.cansel = ON;
	}
	if(GameKey.click &&GameKey.mes_cut){
		GameKey.mes_cut = OFF;
		GameKey.mes_cut_mode = OFF;
		GameKey.click   = ON;
	}
	if(GameKey.cansel&&GameKey.mes_cut){
		GameKey.mes_cut = OFF;
		GameKey.mes_cut_mode = OFF;
		GameKey.cansel = ON;
	}

	if(GameKey.end&&GameKey.home){
		GameKey.home = ON;
		GameKey.end  = OFF;
	}

	if(GameKey.pup&&GameKey.pdown){
		GameKey.pup   = ON;
		GameKey.pdown = OFF;
	}




	GameKey.u = KeyCond.btrg.up;
	GameKey.d = KeyCond.btrg.down;
	GameKey.l = KeyCond.btrg.left;
	GameKey.r = KeyCond.btrg.right;

	GameKey.num[0] = KeyCond.trg.k0 || KeyCond.trg.n0;
	GameKey.num[1] = KeyCond.trg.k1 || KeyCond.trg.n1;
	GameKey.num[2] = KeyCond.trg.k2 || KeyCond.trg.n2;
	GameKey.num[3] = KeyCond.trg.k3 || KeyCond.trg.n3;
	GameKey.num[4] = KeyCond.trg.k4 || KeyCond.trg.n4;
	GameKey.num[5] = KeyCond.trg.k5 || KeyCond.trg.n5;
	GameKey.num[6] = KeyCond.trg.k6 || KeyCond.trg.n6;
	GameKey.num[7] = KeyCond.trg.k7 || KeyCond.trg.n7;
	GameKey.num[8] = KeyCond.trg.k8 || KeyCond.trg.n8;
	GameKey.num[9] = KeyCond.trg.k9 || KeyCond.trg.n9;
}

int		GoTitle=0;
char	GoScript[256]="";
DWORD	GameStartTime=0;
char	NextScript[256];


int System( int frame)
{
	static int	step=SYS_INIT;
	int			script=0;
	int			ret;

	MUS_RenewMouse( appData.Hwnd, appData.Active, 0 );


	KEY_RenewKeybord( appData.Active );
	if(appData.menu<=0){
		appData.menu=0;
		AVG_GetGameKey();
	}else{
		appData.menu--;
	}

	SPR_RenewSprite( frame, 0 );

	switch(step){
		default:
		case SYS_INIT:
			GDFNT_LoadFont( 24, 0 );
			GDFNT_LoadFont( 12, 0 );
			EXEC_ControlScript( &ScriptData );
			AVG_LoadScenarioFlag();
			AVG_LoadVisual();
			AVG_LoadGameFlag();
	
			GameStartTime = timeGetTime();


			GD_LoadTexture( BMP_DEBUG, "bitmap.tga",  ALPHA_256P, FALSE, FALSE );


			TestSetParam(1);
			TestItemAll();

			if( GoScript[0]==0 ){
				step = SYS_TITLE;




			}else{
				AVG_SetScenario( GoScript );
				step = SYS_BATTLE;
			}
			break;
		case SYS_TERM:
	AVG_SavetScenarioFlag();


			GD_TermDevices();

			DestroyWindow(appData.Hwnd);
			break;
		case SYS_BATTLE:

			script = EXEC_ControlScript( &ScriptData );

			ret = BTL_System(script);
			if( ret!=-1 ){
				if( ret==SYS_BATTLE ){
					AVG_SetScenario( GoScript );
				}
				step = ret;
			}
			break;
		case SYS_TITLE:
			ret = TTL_System();
			if( ret!=-1 ){
				if( ret==SYS_BATTLE ){
					AVG_SetScenario( GoScript );
				}
				step = ret;
			}
			break;
		case SYS_WORLD:
			ret = WLD_System();
			if( ret!=-1 ){
				if( ret==SYS_BATTLE ){
					AVG_SetScenario( GoScript );
				}
				step = ret;
			}
			break;
		case SYS_CASTLE:
			ret = CSL_System();
			if( ret!=-1 ){
				if( ret==SYS_BATTLE ){
					AVG_SetScenario( GoScript );
				}
				step = ret;
			}
			break;
		case SYS_SAVE_POINT:
			ret = SVP_System();
			if( ret!=-1 ){
				if( ret==SYS_BATTLE ){
					wsprintf( GoScript, NextScript );
					AVG_SetScenario( GoScript );
				}
				step = ret;
			}
			break;
		case SYS_SAVE_POINT2:
			ret = SVP2_System();
			if( ret!=-1 ){
				if( ret==SYS_BATTLE ){
					wsprintf( GoScript, NextScript );
					AVG_SetScenario( GoScript );
				}
				step = ret;
			}
			break;
		case SYS_END_SAVE:
			ret = END_System();
			if( ret!=-1 ){
				step = ret;
			}
			break;
		case SYS_ITEM1:
			ret = CSL_Item1System(0);
			if( ret!=-1 ){
				AVG_SetScenario( GoScript );

				step = ret;
			}
			break;
		case SYS_ITEM2:
			ret = CSL_Item2System();
			if( ret!=-1 ){
				AVG_SetScenario( GoScript );

				step = ret;
			}
			break;

		case SYS_ENDING:
			break;
	}


	if(appData.debug_mouse!=-1){
		GD_SetGraphDigit( GRP_DEBUG, BMP_DEBUG, LAY_DEBUG, ON, MainFrame, -4 );
		GD_SetGraphDstRect( GRP_DEBUG, 16, 16, 16, 16 );
		GD_SetGraphSrcRect( GRP_DEBUG, 0, 8, 8, 8 );
		GD_SetGraphColor( GRP_DEBUG, 255,128,128,128 );
		GD_SetGraphBrightNo( GRP_DEBUG, 7 );
		GD_SetGraphGroup( GRP_DEBUG, ALL_DISP_GROUP );

		GD_SetGraphDigit( GRP_DEBUG+1, BMP_DEBUG, LAY_DEBUG, ON, DrawFrame, -4 );
		GD_SetGraphDstRect( GRP_DEBUG+1, 16, 32, 16, 16 );
		GD_SetGraphSrcRect( GRP_DEBUG+1, 0, 8, 8, 8 );
		GD_SetGraphColor( GRP_DEBUG+1, 255,128,128,128 );
		GD_SetGraphBrightNo( GRP_DEBUG+1, 7 );
		GD_SetGraphGroup( GRP_DEBUG+1, ALL_DISP_GROUP );
	}else{
		GD_ResetGraph( GRP_DEBUG );
		GD_ResetGraph( GRP_DEBUG+1 );
	}
	if(KeyCond.trg.m){
		appData.debug_mouse = (appData.debug_mouse+2)%3-1;

		SetVoiceDialog( appData.debug_mouse!=-1 );
	}

#if !TAIKEN_FLAG


#endif
	AVG_ControlMusic();

	MUS_SetMouseRect_DispActive( appData.debug_mouse );
	return 1;
}







void MAIN_Loop( int frame )
{
	if(appData.mov_mode){
		frame=appData.mov_mode;
	}

	static int	next_time = 0;
	static int	back_time = 0;
	static int	count=0, draw_count=0;
	static int	skip_flag = 0;

	int			now_time, i;
	static int	skip_time=0;
	static int	skip_count=0;

	POINT			mp;

	GetCursorPos( &mp );
	ScreenToClient( appData.Hwnd, &mp );


	if(appData.ShowMenu == 1) appData.ShowMenu = 0;
	if( !D3DMain.m_WindowMode ){
		int	my = mp.y;

		if(Avg.menu){
			if(!appData.ShowMenu){
				
				if( (GetAsyncKeyState(VK_MENU)&0x8001) && !D3DMain.m_FlipActive ){
					ShowWindow(appData.MenuHwnd,SW_SHOW);
					UpdateWindow(appData.MenuHwnd);
					appData.ShowMenu = 2;
				}
			}else{
				if(appData.ShowMenu == 2){
					if( my<GetSystemMetrics(SM_CYMENUSIZE) ){
						appData.ShowMenu = 3;
					}
				}
				if(appData.ShowMenu == 3){
					if( my>GetSystemMetrics(SM_CYMENUSIZE) ){
						ShowWindow(appData.MenuHwnd, SW_HIDE);
						appData.ShowMenu = 1;
					}
				}
			}
		}else{
			if(!appData.ShowMenu){
				
				if( my<GetSystemMetrics(SM_CYMENUSIZE) && !D3DMain.m_FlipActive ){
					ShowWindow(appData.MenuHwnd,SW_SHOW);
					UpdateWindow(appData.MenuHwnd);
					appData.ShowMenu = 2;
				}
			}else{
				if( my>GetSystemMetrics(SM_CYMENUSIZE) ){
					ShowWindow(appData.MenuHwnd, SW_HIDE);
					appData.ShowMenu = 1;
				}
			}
		}
	}
	if(!appData.ShowMenu){
		now_time = timeGetTime();
		if( next_time == 0 ) next_time = now_time;
		if( back_time == 0 ) back_time = now_time;




		if( next_time <= now_time || appData.frame_free){
			System(frame);

			count++;
			GlobalCount++;

			if( skip_count<=0 || appData.draw_flag || appData.frame_free ) {
				skip_time -= next_time-now_time;
				skip_time = LIM(skip_time,0,100);

				if( AVG_GetMesCut() ){
					skip_count = skip_time/(1000.0/frame)+5;
				}else{
					skip_count = skip_time/(1000.0/frame);
				}
				if(appData.draw_flag){
					skip_count=0;
					skip_time = 0;
				}else{
					skip_time -= skip_count*(1000.0/frame);
				}

				appData.draw_flag=0;

				NowTime = timeGetTime();
				if(NowTime-back_time>1000){
					MainFrame = count*1000/(NowTime-back_time);
					DrawFrame = draw_count*1000/(NowTime-back_time);
					count=0;
					draw_count=0;
					back_time = NowTime;
					DebugPrintf( "frame:%d/%d\n", MainFrame, DrawFrame );
				}

				next_time = now_time + 1000/frame;

				DrawCount++;
				draw_count++;
				GD_Draw( appData.flash, 0 ); 
				appData.flash=0;
				GD_LastBlt();
			}else{
				skip_count--;
			}
		}else{
			DrawCount++;
			draw_count++;
			GD_Draw( appData.flash, 0 );
			appData.flash=0;
			GD_LastBlt();
		}
	}
	Sleep( 2 );
}
