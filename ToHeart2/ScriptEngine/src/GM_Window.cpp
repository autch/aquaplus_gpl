
#include "mm_std.h"
#include "main.h"

#include "disp.h"
#include "draw.h"
#include "text.h"
#include "mouse.h"
#include "keybord.h"

#include "GM_Avg.h"
#include "GM_Window.h"
#include "GM_Save.h"
#include "GM_Title.h"

static int	BmpSetting;
static int	GrpSetting;
static int	TxtSetting;

static int	GrpBack;
static int	TxtBack;

static int	MouseLayer;
static int	MouseLayerBack;

static int	TitleLoad=0;



void AVG_SetCorsol( int index, int x, int y, int w, int h )
{
	int	gc = 128+SIN(GlobalCount2*4%256)/102;

	DSP_SetGraphPrim( index,  PRM_YGRA, POL_RECT, LAY_SYSTEM+3, ON );
	DSP_SetGraphPosRect( index, x, y, w, h/2 );
	DSP_SetGraphParam( index, DRW_BLD(gc) );
	DSP_SetGraphBrightYGra( index, 0, y,      3, 122, 43 );
	DSP_SetGraphBrightYGra( index, 1, y+h/2,  0, 236, 77 );

	DSP_SetGraphPrim( index+1,  PRM_YGRA, POL_RECT, LAY_SYSTEM+3, ON );
	DSP_SetGraphPosRect( index+1, x, y+h/2, w, h/2 );
	DSP_SetGraphParam( index+1, DRW_BLD(gc) );
	DSP_SetGraphBrightYGra( index+1, 0, y+h/2, 0, 236, 77 );
	DSP_SetGraphBrightYGra( index+1, 1, y+h,   3, 122, 43 );
}

void AVG_ResetCorsol( int index )
{
	DSP_ResetGraph( index   );
	DSP_ResetGraph( index+1 );
}

#define SAVE_WINODW_X	(118)
#define SAVE_WINODW_Y	(74)
#define SAVE_WINODW_W	(DISP_X-SAVE_WINODW_X*2)
#define SAVE_WINODW_H	(28)

#define SAVE_PAGE		10
#define SAVE_PAGE_MAX	10


enum{
	SAV_NOT,
	SAV_OPEN,
	SAV_CLOSE,
	SAV_LOAD_OPEN,
	SAV_LOAD_CLOSE,
	SAV_LOAD_CHECK,
	SAV_NORMAL,
};

SAVE_WINDOW		SaveWindow;
#include "escript.h"


void GWIN_RenewSaveLoadWindow( void )
{
	int		i,j;
	char	buf[256];
	char	buf2[256];
	SYSTEMTIME	sys_time;
	BMP_SET		bmp_set;
	int			game_time;

	DSP_CreateBmp( BMP_WORK, BMP_FULL, 80, 60 );
	if(TitleLoad){
		DSP_LoadBmp( BmpSetting+1,  BMP_FULL, "sys0201.tga" );
		DSP_LoadBmp( BmpSetting+4,  BMP_FULL, "sys0202.tga" );
	}else{
		DSP_LoadBmp( BmpSetting+1,  BMP_FULL, "sys0201.tga" );
		DSP_LoadBmp( BmpSetting+4,  BMP_FULL, "sys0202.tga" );
	}

	for(i=0;i<SAVE_PAGE;i++){
		DSP_SetGraph( GrpSetting+20+i, BmpSetting+1, LAY_SYSTEM+3, OFF, CHK_ANTI );
		DSP_SetGraphPos( GrpSetting+20+i, 16+390*(i/5), 112+76*(i%5), 390*(i/5), 76*(i%5), 380, 74 );

		DSP_SetDigit( GRP_WORK, BmpSetting+3, LAY_SYSTEM+4+10, ON, CHK_ANTI, i+SaveWindow.page*SAVE_PAGE+1, -3 );
		DSP_SetGraphPos( GRP_WORK, 100+390*(i/5), 10+76*(i%5), 0, 15, 140, 15 );
		DSP_SetGraphTarget( GRP_WORK, BmpSetting+1, MainWindow.draw_mode2 );
		DSP_SetGraphTarget( GRP_WORK, BmpSetting+4, MainWindow.draw_mode2 );

		SaveWindow.file[i] = SAV_LoadHead( i+SaveWindow.page*SAVE_PAGE, buf, &sys_time, &bmp_set, &game_time );
		if( SaveWindow.file[i] || (SaveWindow.type==SW_SAVE_MODE)  ){
			MUS_SetMouseRect( MouseLayer, i, 16+390*(i/5), 112+76*(i%5), 380, 74, ON );
		}else{
			MUS_ResetMouseRect( MouseLayer, i );
		}
		if( !SaveWindow.file[i] ) continue;

		
		DSP_CopyBmp3( BMP_WORK, bmp_set );

		DSP_SetGraph( GRP_WORK, BMP_WORK, 0, ON, CHK_NO );
		DSP_SetGraphMove( GRP_WORK, 16+390*(i/5), 6+76*(i%5) );
		DSP_SetGraphTarget( GRP_WORK, BmpSetting+1, MainWindow.draw_mode2 );
		DSP_SetGraphTarget( GRP_WORK, BmpSetting+4, MainWindow.draw_mode2 );


		if( i+SaveWindow.page*SAVE_PAGE==SaveWindow.new_no ) {
			j = 2;
			DSP_SetGraph( GRP_WORK, BmpSetting+5, 0, ON, CHK_ANTI );


			DSP_SetGraphMove( GRP_WORK, 318+390*(i/5), 37+76*(i%5) );
			DSP_SetGraphTarget( GRP_WORK, BmpSetting+1, MainWindow.draw_mode2 );
			DSP_SetGraphTarget( GRP_WORK, BmpSetting+4, MainWindow.draw_mode2 );
		}else{
			j = 1;
		}



		{
			int	_times = game_time&0xff;
			int	_days  = (game_time>>8)&0xff;
			int	_month = (game_time>>16)&0xff;
			char		*_time_tbl[8] = { "朝","休み時間","昼休み","授業中","授業終了後","放課後","夜","次の日へ" };

			if(_month==0){
				wsprintf( buf2, "?A?B" );
			}else{
				wsprintf( buf2, "%dA%dB", _month, _days );
			}
			DSP_SetGraphStr( GRP_WORK, BmpSetting+3, 0, ON, CHK_ANTI, buf2 );
			DSP_SetGraphMove( GRP_WORK, 154+390*(i/5), 10+76*(i%5) );
			DSP_SetGraphTarget( GRP_WORK, BmpSetting+1, MainWindow.draw_mode2 );
			DSP_SetGraphTarget( GRP_WORK, BmpSetting+4, MainWindow.draw_mode2 );

			DSP_SetText( TXT_WORK, 0, RUBI_FONT, ON, buf );
			DSP_SetTextMove( TXT_WORK, 154+14*5+390*(i/5), 10+76*(i%5) );
			DSP_SetTextTarget( TXT_WORK, BmpSetting+1, MainWindow.draw_mode2 );
			DSP_SetTextTarget( TXT_WORK, BmpSetting+4, MainWindow.draw_mode2 );
		}

		wsprintf( buf2, "%04d", sys_time.wYear );
		DSP_SetGraphStr( GRP_WORK, BmpSetting+3, 0, ON, CHK_ANTI, buf2 );
		DSP_SetGraphMove( GRP_WORK, 100+390*(i/5), 43+76*(i%5) );
		DSP_SetGraphBright( GRP_WORK, 120, 43, 56 );
		DSP_SetGraphTarget( GRP_WORK, BmpSetting+1, MainWindow.draw_mode2 );
		DSP_SetGraphTarget( GRP_WORK, BmpSetting+4, MainWindow.draw_mode2 );

		wsprintf( buf2, "%02d/%02d", sys_time.wMonth, sys_time.wDay );
		DSP_SetGraphStr( GRP_WORK, BmpSetting+3, 0, ON, CHK_ANTI, buf2 );
		DSP_SetGraphMove( GRP_WORK, 14*4+10+100+390*(i/5), 43+76*(i%5) );
		DSP_SetGraphBright( GRP_WORK, 120, 43, 56 );
		DSP_SetGraphTarget( GRP_WORK, BmpSetting+1, MainWindow.draw_mode2 );
		DSP_SetGraphTarget( GRP_WORK, BmpSetting+4, MainWindow.draw_mode2 );

		wsprintf( buf2, "%02d:%02d", sys_time.wHour, sys_time.wMinute );
		DSP_SetGraphStr( GRP_WORK, BmpSetting+3, 0, ON, CHK_ANTI, buf2 );
		DSP_SetGraphMove( GRP_WORK, 14*9+20+100+390*(i/5), 43+76*(i%5) );
		DSP_SetGraphBright( GRP_WORK, 120, 43, 56 );
		DSP_SetGraphTarget( GRP_WORK, BmpSetting+1, MainWindow.draw_mode2 );
		DSP_SetGraphTarget( GRP_WORK, BmpSetting+4, MainWindow.draw_mode2 );

		DSP_ResetGraph( GRP_WORK );
	

		DSP_ResetText( TXT_WORK );

	}
	DSP_ReleaseBmp( BMP_WORK );
}

void GWIN_SetSaveLoadWindow( int type, int bmp_set, int grp_set, int txt_set, int grp_bak, int txt_bak, int mus_lay, int mus_bak )
{
	int		bmp_bpp = BPP(MainWindow.draw_mode2);
	int		i;
	char	buf2[256];

	BmpSetting=bmp_set;
	GrpSetting=grp_set;
	TxtSetting=txt_set;




	MouseLayer     = mus_lay;
	MouseLayerBack = mus_bak;

	if(type==SW_LOAD_MODE2){
		type = SW_LOAD_MODE;
		TitleLoad=1;
	}else{
		TitleLoad=0;
	}

	SaveWindow.flag=1;
	SaveWindow.type=type;
	SaveWindow.cnt =0;
	SaveWindow.mode=SAV_OPEN;
	SaveWindow.load_flag=0;
	SaveWindow.save_flag=0;
		if(type==SW_SAVE_MODE)	DSP_LoadBmp( BmpSetting,    BMP_FULL, "sys0200.tga" );
		else					DSP_LoadBmp( BmpSetting,    BMP_FULL, "sys0300.tga" );


		DSP_LoadBmp( BmpSetting+3,  BMP_TRUE, "sys0230.tga" );
		DSP_LoadBmp( BmpSetting+4,  BMP_FULL, "sys0202.tga" );
		DSP_LoadBmp( BmpSetting+5,  BMP_TRUE, "sys0210.tga" );
		DSP_LoadBmp( BmpSetting+6,  BMP_TRUE, "sys0111.tga" );

		if(type==SW_SAVE_MODE)	DSP_LoadBmp( BmpSetting+7,  BMP_TRUE, "sys0250.tga" );
		else					DSP_LoadBmp( BmpSetting+7,  BMP_TRUE, "sys0350.tga" );
		DSP_LoadBmp( BmpSetting+8,  BMP_TRUE, "sys0251.tga" );
		DSP_LoadBmp( BmpSetting+9,  BMP_TRUE, "sys0203.tga" );


	DSP_SetGraph( GrpSetting+0, BmpSetting, LAY_SYSTEM+2, ON, CHK_NO );


	SYSTEMTIME	sys_time;
	int	k=0;
	for(i=0;i<SAVE_PAGE*SAVE_PAGE_MAX;i++){
		int	j=0;
		if( SAV_LoadHeadTime( i, &sys_time ) ){
			j = ((int)(sys_time.wYear)%100)*12;
			j = (j+(int)(sys_time.wMonth))*31;
			j = (j+(int)(sys_time.wDay))*24;
			j = (j+(int)(sys_time.wHour))*60;
			j = (j+(int)(sys_time.wMinute))*60;
			j = (j+(int)(sys_time.wSecond));
			if(j>k){
				k = j;
				SaveWindow.new_no = i;
			}
		}
	}
	SaveWindow.page = SaveWindow.new_no/SAVE_PAGE;

	wsprintf( buf2, "%02d/%02d", SaveWindow.page+1, 10 );
	DSP_SetGraphStr( GrpSetting+11, BmpSetting+3, LAY_SYSTEM+3, ON, CHK_ANTI, buf2 );
	DSP_SetGraphMove( GrpSetting+11, 364, 78 );




	DSP_SetGraph( GrpSetting+15, BmpSetting+9, LAY_SYSTEM+3, OFF, CHK_ANTI );
	DSP_SetGraph( GrpSetting+16, BmpSetting+9, LAY_SYSTEM+3, OFF, CHK_ANTI );
	DSP_SetGraph( GrpSetting+17, BmpSetting+6, LAY_SYSTEM+3, OFF, CHK_ANTI );
	DSP_SetGraphPos( GrpSetting+15, 190,72,      0,    64*0, 130, 32 );	
	DSP_SetGraphPos( GrpSetting+16, 482,72,      0, 32+64*0, 130, 32 );	
	DSP_SetGraphPos( GrpSetting+17, 306,496, 188*0,       0, 188, 32 );	

	DSP_SetGraph( GrpSetting+18, BmpSetting+8, LAY_SYSTEM+6, OFF, CHK_NO );

	DSP_SetGraph( GrpSetting+19, BmpSetting+4, LAY_SYSTEM+6, OFF, CHK_ANTI );

	
	MUS_SetMouseLayer( MouseLayer );
	MUS_SetMouseRect( MouseLayer, 10, 190,72, 130, 32 , ON );	
	MUS_SetMouseRect( MouseLayer, 11, 482,72, 130, 32 , ON );	
	MUS_SetMouseRect( MouseLayer, 12, 306,496,188, 32 , ON );	

	GWIN_RenewSaveLoadWindow( );
	
	MainWindow.draw_flag=ON;
}

void GWIN_ResetSaveLoadWindow( void )
{
	int		i;

	if(SaveWindow.save_flag){
		SaveWindow.save_flag=OFF;
		MUS_ResetMouseRect_Layer( MouseLayer );


	}else{
		MUS_ResetMouseRect_Layer( MouseLayer );
		MUS_SetMouseLayer( MouseLayerBack );
	}

	SaveWindow.flag=0;

	for(i=0;i<70;i++)	DSP_ResetGraph( GrpSetting+i );
	for(i=0;i<10;i++)	DSP_ReleaseBmp( BmpSetting+i );
	DSP_ReleaseBmp( BMP_CAP+1 );


}




int GWIN_SetSaveLoadCheck( int select )
{
	if( SaveWindow.type==SW_SAVE_MODE && !SaveWindow.file[select] ){	
		AVG_SetSave( select+SaveWindow.page*SAVE_PAGE );
		SaveWindow.new_no = select+SaveWindow.page*SAVE_PAGE;
		GWIN_RenewSaveLoadWindow();
		return 0;
	}else{									
		SaveWindow.select = select+SaveWindow.page*SAVE_PAGE;
		SaveWindow.cnt    = 0;
		SaveWindow.mode   = SAV_LOAD_OPEN;

		DSP_CopyBmp2( BMP_CAP+1, -1, NULL, 0, 64, 64, 64 );


		DSP_SetGraphDisp( GrpSetting+19, ON );
		DSP_SetGraphParam( GrpSetting+19, DRW_BLD(0) );

		DSP_SetGraphPos( GrpSetting+19, 20+5, 267+4, 390*(select/5)+5, 76*(select%5)+4, 369, 64 );

		DSP_SetGraph( GRP_WORK, BmpSetting+7, 0, ON, CHK_ANTI );
		DSP_SetGraphMove( GRP_WORK, 0, 246 );
		DSP_SetGraphTarget( GRP_WORK, BMP_CAP+1, MainWindow.draw_mode2 );
		DSP_ResetGraph( GRP_WORK );

		DSP_SetGraph( GrpSetting+30, BMP_CAP+1, LAY_SYSTEM+4, ON, CHK_NO );


			MUS_SetMouseRect( MouseLayer+1, 0, 461, 320, 130, 32, ON );
			MUS_SetMouseRect( MouseLayer+1, 1, 606, 320, 130, 32, ON );
		MUS_SetMouseLayer( MouseLayer+1 );
		return 1;
	}
}

void GWIN_ResetSaveLoadCheck( void )
{
	int		i;

	for(i=0; i<6 ;i++)
		DSP_ResetGraph( GrpSetting+40+i );
	DSP_ResetText( TxtSetting+20 );

	
	MUS_ResetMouseRect_Layer( MouseLayer+1 );
	MUS_SetMouseLayer( MouseLayer );
}

int GWIN_ControlSaveLoadWindow(void)
{
	int		cmax = AVG_EffCnt(-1);
	int		select = MUS_GetMouseNoEx(-1, MouseLayer );
	int		click  = GameKey.click;
	int		cansel = GameKey.cansel;
	int		rate=256, rate2=0;
	int		i;
	int		ret = -1;
	char	buf2[256];

	static int	select_fade = 0;

	if( SaveWindow.flag ){
		switch( SaveWindow.mode ){
			case SAV_NOT:

				break;
			case SAV_OPEN:
				SaveWindow.cnt++;
				if( SaveWindow.cnt>=cmax  ){
					SaveWindow.mode = SAV_NORMAL;
					rate=256;
				}else{
					rate = 256*SaveWindow.cnt/cmax;
				}
				break;
			case SAV_CLOSE:
				SaveWindow.cnt++;
				if( SaveWindow.cnt>=cmax  ){
					SaveWindow.mode = SAV_NOT;
					rate=0;
					GWIN_ResetSaveLoadWindow();	
					return 0;
				}else{
					rate = 256-256*SaveWindow.cnt/cmax;
				}
				break;
			case SAV_LOAD_OPEN:
				cmax = AVG_EffCnt(8);
				SaveWindow.cnt++;
				if( SaveWindow.cnt>=cmax  ){
					SaveWindow.mode = SAV_LOAD_CHECK;
				}
				if(cmax){
					rate2 = 256*SaveWindow.cnt/cmax;
				}else{
					rate2 = 256;
				}
				break;
			case SAV_LOAD_CLOSE:
				cmax = AVG_EffCnt(8);
				SaveWindow.cnt++;
				if( SaveWindow.cnt>=cmax  ){
					SaveWindow.mode = SAV_NORMAL;
					GWIN_ResetSaveLoadCheck();
					GWIN_RenewSaveLoadWindow();
				}
				if(cmax){
					rate2 = 256-256*SaveWindow.cnt/cmax;
				}else{
					rate2 = 0;
				}
				break;
			case SAV_LOAD_CHECK:
				rate2=256;
				select = MUS_GetMouseNoEx( -1, MouseLayer+1 );
				
				if(GameKey.l){	MUS_SetMousePosRect( MainWindow.hwnd, MouseLayer+1, 0 ); }
				if(GameKey.r){	MUS_SetMousePosRect( MainWindow.hwnd, MouseLayer+1, 1 ); }

				DSP_SetGraphDisp( GrpSetting+18, OFF );
				if(!SaveWindow.load_flag){
					int	fade = 256-STD_LimitLoop( select_fade, 128 );
					select_fade+=8;

					
					if(select==0){
						DSP_SetGraphDisp( GrpSetting+18, ON );

						DSP_SetGraphPos( GrpSetting+18, 461, 320,   0, 32, 130, 32 );
					}else if(select==1){
						DSP_SetGraphDisp( GrpSetting+18, ON );

						DSP_SetGraphPos( GrpSetting+18, 606, 320, 130, 32, 130, 32 );
					}
					if(click){
						if(select==0){

							DSP_SetGraphPos( GrpSetting+18, 461, 320, 0, 64, 130, 32 );
							DSP_SetGraphParam( GrpSetting+18, DRW_NML );
							AVG_PlaySE3( 9104, 255 );
							if(SaveWindow.type==SW_SAVE_MODE){
								SaveWindow.new_no = SaveWindow.select;
								AVG_SetSave( SaveWindow.select );



								AVG_ResetConfigWindow();
								ConfigOpenMode=1;

								SaveWindow.save_flag = ON;
								SaveWindow.mode = SAV_CLOSE;
								SaveWindow.cnt = 0;
							}else{
								SaveWindow.flag=OFF;
								SaveWindow.load_flag = ON;
								AVG_SetLoad( SaveWindow.select );
								AVG_StopBGM( FADE_MUS );
								
								GWIN_ResetSaveLoadWindow();
								AVG_ResetConfigWindow();
								return -2;
							}
						}else if(select==1){
							DSP_SetGraphPos( GrpSetting+18, 606, 320, 128, 64, 130, 32 );

							DSP_SetGraphParam( GrpSetting+18, DRW_NML );
							AVG_PlaySE3( 9104, 255 );

							SaveWindow.cnt  = 0;
							SaveWindow.mode = SAV_LOAD_CLOSE;
						}
					}else if(cansel){
						AVG_PlaySE3( 9107, 255 );

						SaveWindow.cnt  = 0;
						SaveWindow.mode = SAV_LOAD_CLOSE;
					}
				}
				break;
			case SAV_NORMAL:
				if(GameKey.pup)   { select=10; click=ON; }	
				if(GameKey.pdown) { select=11; click=ON; }	

				DSP_SetGraphDisp( GrpSetting+15, ON );
				DSP_SetGraphDisp( GrpSetting+16, ON );
				DSP_SetGraphDisp( GrpSetting+17, ON );
				DSP_SetGraphParam( GrpSetting+15, DRW_NML );
				DSP_SetGraphParam( GrpSetting+16, DRW_NML );
				DSP_SetGraphParam( GrpSetting+17, DRW_NML );
				DSP_SetGraphSMove( GrpSetting+15,  0, 64*0 );	
				DSP_SetGraphSMove( GrpSetting+16,  0, 32+64*0 );	
				DSP_SetGraphSMove( GrpSetting+17,  0, 0 );

				if(select!=-1){
					if(select<SAVE_PAGE){
						DSP_SetGraphDisp( GrpSetting+19, ON );
						DSP_SetGraphPos( GrpSetting+19, 16+390*(select/5), 112+76*(select%5), 390*(select/5), 76*(select%5), 380, 74 );
						DSP_SetGraphParam( GrpSetting+19, DRW_NML );

					}else{
						DSP_SetGraphDisp( GrpSetting+19, OFF );

						int	fade = 256-STD_LimitLoop( select_fade, 128 );
						select_fade+=8;
						
						switch(select){
							case 10:	DSP_SetGraphSMove( GrpSetting+15, 0,    64 );	break;
							case 11:	DSP_SetGraphSMove( GrpSetting+16, 0, 32+64 );	break;
							case 12:	DSP_SetGraphSMove( GrpSetting+17, 188, 0 );		break;
						}

					}
					if( click ){
						MainWindow.draw_flag=1;

						switch(select){
							default:
								AVG_PlaySE3( 9104, 255 );
								if( GWIN_SetSaveLoadCheck(select) == 0){
									AVG_ResetConfigWindow();
									ConfigOpenMode=1;

									SaveWindow.save_flag = ON;
									SaveWindow.mode = SAV_CLOSE;
									SaveWindow.cnt = 0;
								}
								break;
							case 10:	
								AVG_PlaySE3( 9104, 255 );
								DSP_SetGraphSMove( GrpSetting+15, 0, 64*2 );
								DSP_SetGraphParam( GrpSetting+15, DRW_NML );
								SaveWindow.page = (SaveWindow.page+9)%10;
								GWIN_RenewSaveLoadWindow( );
								break;
							case 11:	
								AVG_PlaySE3( 9104, 255 );
								DSP_SetGraphSMove( GrpSetting+16, 0, 32+64*2 );
								DSP_SetGraphParam( GrpSetting+16, DRW_NML );
								SaveWindow.page = (SaveWindow.page+1)%10;
								GWIN_RenewSaveLoadWindow( );
								break;
							case 12:	
								DSP_SetGraphSMove( GrpSetting+17, 188*2, 0 );
								DSP_SetGraphParam( GrpSetting+17, DRW_NML );

								AVG_PlaySE3( 9104, 255 );
								SaveWindow.mode = SAV_CLOSE;
								SaveWindow.cnt = 0;
								break;
						}
					}
				}else{
					DSP_SetGraphDisp( GrpSetting+19, OFF );
				}
				if(cansel){
					AVG_PlaySE3( 9107, 255 );
					SaveWindow.mode = SAV_CLOSE;
					SaveWindow.cnt = 0;
				}
				break;
		}

		if( rate<128 ){
			DSP_SetGraphParam( GrpSetting+0, DRW_BLD(rate*2)  );
			DSP_SetGraphDisp( GrpSetting+0, ON  );

			DSP_SetGraphDisp( GrpSetting+11, ON  );
			DSP_SetGraphParam( GrpSetting+11, DRW_BLD(rate*2)  );

			DSP_SetGraphDisp( GrpSetting+15, ON  );
			DSP_SetGraphDisp( GrpSetting+16, ON  );
			DSP_SetGraphDisp( GrpSetting+17, ON  );
			DSP_SetGraphParam( GrpSetting+15, DRW_BLD(rate*2)  );
			DSP_SetGraphParam( GrpSetting+16, DRW_BLD(rate*2)  );
			DSP_SetGraphParam( GrpSetting+17, DRW_BLD(rate*2)  );

			for(i=0;i<SAVE_PAGE;i++){
				DSP_SetGraphParam( GrpSetting+1+i, DRW_BLD(rate*2)  );
				DSP_SetGraphDisp( GrpSetting+1+i, ON  );
				DSP_SetGraphDisp( GrpSetting+20+i, OFF );
			}
			ret = 1;
		}else{
			ret = 2;
			if(rate2==0){
				DSP_SetGraphParam( GrpSetting+0, DRW_NML  );
				DSP_SetGraphDisp( GrpSetting+0, ON  );
				
				DSP_SetGraphDisp( GrpSetting+18, OFF  );
				if(rate!=256)
					DSP_SetGraphDisp( GrpSetting+19, OFF  );


				wsprintf( buf2, "%02d/%02d", SaveWindow.page+1, 10 );
				DSP_SetGraphStr( GrpSetting+11, BmpSetting+3, LAY_SYSTEM+3, ON, CHK_ANTI, buf2 );
				DSP_SetGraphMove( GrpSetting+11, 364, 78 );
				for(i=0;i<SAVE_PAGE;i++){
					DSP_SetDigitNum( GrpSetting+1+i, i+SaveWindow.page*SAVE_PAGE+1 );
					DSP_SetGraphParam( GrpSetting+1+i, DRW_NML );
					DSP_SetGraphDisp( GrpSetting+1+i, ON  );


						DSP_SetGraphDisp( GrpSetting+20+i, ON );
						DSP_SetGraphParam( GrpSetting+20+i, DRW_BLD((rate-128)*2) );



				}
				DSP_SetGraphDisp( GrpSetting+30, OFF );
			}else if(rate2<256){
				DSP_SetGraphDisp( GrpSetting+0,  ON  );
				DSP_SetGraphDisp( GrpSetting+18, OFF );
				for(i=0;i<SAVE_PAGE;i++){
					DSP_SetGraphDisp( GrpSetting+1+i, ON  );

						DSP_SetGraphDisp( GrpSetting+20+i, ON );
				}

				DSP_SetGraphDisp( GrpSetting+19, ON );
				DSP_SetGraphParam( GrpSetting+19, DRW_BLD(rate2) );
				DSP_SetGraphDisp( GrpSetting+30, ON );
				DSP_SetGraphParam( GrpSetting+30, DRW_BLD(rate2) );
			}else{
				DSP_SetGraphDisp( GrpSetting+0,  OFF  );
				for(i=0;i<SAVE_PAGE;i++){
					DSP_SetGraphDisp( GrpSetting+1+i, OFF  );
					DSP_SetGraphDisp( GrpSetting+20+i, OFF );
				}

				DSP_SetGraphDisp( GrpSetting+19, ON  );
				DSP_SetGraphParam( GrpSetting+19, DRW_NML );
				DSP_SetGraphDisp( GrpSetting+30, ON );
				DSP_SetGraphParam( GrpSetting+30, DRW_NML );
			}
		}
	}
	return ret;
}


enum{
	SIDE_NOT,
	SIDE_OPEN,
	SIDE_CLOSE,
	SIDE_NORMAL,
};

typedef struct{
	int		flag;
	int		cnt;
	int		mode;

	int		mus_no;
	int		vol;
	int		loop;
}SIDE_BAR_WINDOW;
SIDE_BAR_WINDOW		SideBarWindow;

RECT2	OptionPosition[10][5]={
	{	{0, 36*0,240,34},	{240,36*0,240,34},	{0,0,0,0},			{0,0,0,0},			{0,0,0,0}	},
	{	{0, 36*1,160,34},	{160,36*1,160,34},	{320,36*1,160,34},	{0,0,0,0},			{0,0,0,0}	},
	{	{0, 36*2,120,34},	{120,36*2,120,34},	{240,36*2,120,34},	{360,36*2,120,34},	{0,0,0,0}	},

	{	{4, 112+36*0,276,24},	{296,36*3,92,34},	{388,36*3,92,34},	{0,0,0,0},	{0,0,0,0}	},
	{	{4, 112+36*1,276,24},	{296,36*4,92,34},	{388,36*4,92,34},	{0,0,0,0},	{0,0,0,0}	},
	{	{4, 112+36*2,276,24},	{296,36*5,92,34},	{388,36*5,92,34},	{0,0,0,0},	{0,0,0,0}	},

	{	{0, 36*6,120,34},	{120,36*6,120,34},	{240,36*6,120,34},	{360,36*6,120,34},	{0,0,0,0}	},
	{	{0, 36*7,120,34},	{120,36*7,120,34},	{240,36*7,120,34},	{360,36*7,120,34},	{0,0,0,0}	},
	
	{	{0, 36*8,96,34},	{96*1,36*8,96,34},	{96*2,36*8,96,34},	{96*3,36*8,96,34},	{96*4,36*8,96,34}	},
	{	{0, 36*9,240,34},	{240,36*9,240,34},	{0,0,0,0},			{0,0,0,0},			{0,0,0,0}	},
};

POINT	OptinOfset={292,185};


void GWIN_SetSideBarWindowEx( int bmp_set, int grp_set, int txt_set, int mus_lay, int mus_bak )
{
	int		bmp_bpp = BPP(MainWindow.draw_mode2);

	BmpSetting=bmp_set;
	GrpSetting=grp_set;
	TxtSetting=txt_set;

	MouseLayer     = mus_lay;
	MouseLayerBack = mus_bak;

	SideBarWindow.flag=1;
	SideBarWindow.cnt =0;
	SideBarWindow.mode=SIDE_OPEN;

	DSP_LoadBmp( BmpSetting+0,  BMP_FULL, "sys0400.bmp" );
	DSP_LoadBmp( BmpSetting+1,  BMP_FULL, "sys0410.bmp" );
	DSP_LoadBmp( BmpSetting+2,  BMP_FULL, "sys0411.bmp" );
	DSP_LoadBmp( BmpSetting+3,  BMP_FULL, "sys0412.bmp" );
	DSP_LoadBmp( BmpSetting+4,  BMP_FULL, "sys0413.bmp" );
	DSP_LoadBmp( BmpSetting+5,  BMP_FULL, "sys0414.bmp" );
	DSP_LoadBmp( BmpSetting+6,  BMP_FULL, "sys0415.bmp" );
	DSP_LoadBmp( BmpSetting+7,  BMP_FULL, "sys0416.bmp" );
	DSP_LoadBmp( BmpSetting+8,  BMP_FULL, "sys0420.bmp" );
	DSP_LoadBmp( BmpSetting+9,  BMP_FULL, "sys0430.bmp" );

	DSP_SetGraph( GrpSetting+0, BmpSetting, LAY_SYSTEM+2, ON, CHK_NO );
	DSP_SetGraph( GrpSetting+1, BmpSetting+8, LAY_SYSTEM+3, ON, CHK_NO );

	DSP_SetGraph( GrpSetting+2, BmpSetting+9, LAY_SYSTEM+4, ON, CHK_NO );
	DSP_SetGraph( GrpSetting+3, BmpSetting+9, LAY_SYSTEM+4, ON, CHK_NO );
	DSP_SetGraph( GrpSetting+4, BmpSetting+9, LAY_SYSTEM+4, ON, CHK_NO );

	
	MUS_SetMouseLayer( MouseLayer );
	int	i,j;
	for(i=0;i<10;i++){
		for(j=0;j<5;j++){
			DSP_SetGraph( GrpSetting+10+i*5+j, BmpSetting+1, LAY_SYSTEM+3, (OptionPosition[i][j].w)?ON:OFF, CHK_NO );
			DSP_SetGraphPos( GrpSetting+10+i*5+j, OptinOfset.x+OptionPosition[i][j].x, OptinOfset.y+OptionPosition[i][j].y, OptionPosition[i][j].x, OptionPosition[i][j].y, OptionPosition[i][j].w, OptionPosition[i][j].h );
			
			MUS_SetMouseRect( MouseLayer, i*5+j, OptinOfset.x+OptionPosition[i][j].x, OptinOfset.y+OptionPosition[i][j].y, OptionPosition[i][j].w, OptionPosition[i][j].h, (OptionPosition[i][j].w)?ON:OFF );	
		}
	}

	
	DSP_SetGraphPos( GrpSetting+10+5*3+0, OptinOfset.x+0, OptinOfset.y+36*3, 0, 36*3, 296, 34 );
	DSP_SetGraphPos( GrpSetting+10+5*4+0, OptinOfset.x+0, OptinOfset.y+36*4, 0, 36*4, 296, 34 );
	DSP_SetGraphPos( GrpSetting+10+5*5+0, OptinOfset.x+0, OptinOfset.y+36*5, 0, 36*5, 296, 34 );

	DSP_SetGraphPos( GrpSetting+1, 628, 553, 0, 28*0, 124, 28 );

	DSP_SetGraphPos( GrpSetting+2, 307-4+Avg.bgm_vol,   293+36*0, 8, 0, 8, 34 );
	DSP_SetGraphPos( GrpSetting+3, 307-4+Avg.se_vol,    293+36*1, 8, 0, 8, 34 );
	DSP_SetGraphPos( GrpSetting+4, 307-4+Avg.voice_vol, 293+36*2, 8, 0, 8, 34 );

	SideBarWindow.mus_no = PlayMusicNo;
	SideBarWindow.vol    = PlayMusicVol;
	SideBarWindow.loop   = PlayMusicLoop;

	MUS_SetMouseRect( MouseLayer, 50, 628,553,124, 28, ON );	

	MainWindow.draw_flag=ON;
}

void GWIN_ResetSideBarWindowEx( void )
{
	int		i;
	SideBarWindow.flag=0;

	for(i=0;i<60;i++)	DSP_ResetGraph( GrpSetting+i );
	for(i=0;i<10;i++)	DSP_ReleaseBmp( BmpSetting+i );

	AVG_PlayBGMEx( SideBarWindow.mus_no, 30, SideBarWindow.loop, SideBarWindow.vol, FALSE, FALSE );

	MUS_ResetMouseRect_Layer( MouseLayer );
	MUS_SetMouseLayer( MouseLayerBack );
}


int GWIN_ControlSideBarWindowEx(void)
{
	int		cmax = AVG_EffCnt(-1);
	int		select = MUS_GetMouseNoEx(-1, MouseLayer );
	int		click  = GameKey.click;
	int		cansel = GameKey.cansel;
	int		drag   = MUS_GetMouseButton(MOUSE_LBUTTON);
	int		rate=256;
	int		ret = -1;
	static int	select_fade = 0;
	int	i,j;

	if( SideBarWindow.flag ){
		switch( SideBarWindow.mode ){
			case SIDE_NOT:
				break;
			case SIDE_OPEN:
				SideBarWindow.cnt++;
				if( SideBarWindow.cnt>=cmax  ){
					SideBarWindow.mode = SIDE_NORMAL;
					rate = 256;
				}else{
					rate = 256*SideBarWindow.cnt/cmax;
				}
				break;
			case SIDE_CLOSE:
				SideBarWindow.cnt++;
				if( SideBarWindow.cnt>=cmax  ){
					SideBarWindow.mode = SIDE_NOT;
					rate=0;
					GWIN_ResetSideBarWindowEx();	
					return 0;
				}else{
					rate = 256-256*SideBarWindow.cnt/cmax;
				}
				break;
			case SIDE_NORMAL:
				i=0;
				DSP_SetGraphBmp( GrpSetting+10+i*5+0, BmpSetting+((MainWindow.full_screen)?5:2) );
				DSP_SetGraphBmp( GrpSetting+10+i*5+1, BmpSetting+((MainWindow.full_screen)?2:5) );

				i=1;
				DSP_SetGraphBmp( GrpSetting+10+i*5+0, BmpSetting+((Avg.side_option==1)?5:2) );
				DSP_SetGraphBmp( GrpSetting+10+i*5+1, BmpSetting+((Avg.side_option==0)?5:2) );
				DSP_SetGraphBmp( GrpSetting+10+i*5+2, BmpSetting+((Avg.side_option==3)?5:2) );

				i=2;
				DSP_SetGraphBmp( GrpSetting+10+i*5+0, BmpSetting+((Avg.wait==0)?5:2) );
				DSP_SetGraphBmp( GrpSetting+10+i*5+1, BmpSetting+((Avg.wait==1)?5:2) );
				DSP_SetGraphBmp( GrpSetting+10+i*5+2, BmpSetting+((Avg.wait==2)?5:2) );
				DSP_SetGraphBmp( GrpSetting+10+i*5+3, BmpSetting+((Avg.wait==4)?5:2) );

				
				i=3;
				DSP_SetGraphBmp( GrpSetting+10+i*5+0, BmpSetting+1 );
				DSP_SetGraphBmp( GrpSetting+10+i*5+1, BmpSetting+(Avg.bgm?2:1) );
				DSP_SetGraphBmp( GrpSetting+10+i*5+2, BmpSetting+(Avg.bgm?5:2) );
				DSP_SetGraph( GrpSetting+10+i*5+3, BmpSetting+(Avg.bgm?3:2), LAY_SYSTEM+3, ON, CHK_NO );

				MUS_SetMouseRectFlag( MouseLayer, i*5+0, Avg.bgm );	
				MUS_SetMouseRectFlag( MouseLayer, i*5+1, Avg.bgm );	

				
				i=4;
				DSP_SetGraphBmp( GrpSetting+10+i*5+0, BmpSetting+1 );
				DSP_SetGraphBmp( GrpSetting+10+i*5+1, BmpSetting+(Avg.se?2:1) );
				DSP_SetGraphBmp( GrpSetting+10+i*5+2, BmpSetting+(Avg.se?5:2) );
				DSP_SetGraph( GrpSetting+10+i*5+3, BmpSetting+(Avg.se?3:2), LAY_SYSTEM+3, ON, CHK_NO );

				MUS_SetMouseRectFlag( MouseLayer, i*5+0, Avg.se );	
				MUS_SetMouseRectFlag( MouseLayer, i*5+1, Avg.se );	

				
				i=5;
				DSP_SetGraphBmp( GrpSetting+10+i*5+0, BmpSetting+1 );
				DSP_SetGraphBmp( GrpSetting+10+i*5+1, BmpSetting+(Avg.voice?2:1) );
				DSP_SetGraphBmp( GrpSetting+10+i*5+2, BmpSetting+(Avg.voice?5:2) );
				DSP_SetGraph( GrpSetting+10+i*5+3, BmpSetting+(Avg.voice?3:2), LAY_SYSTEM+3, ON, CHK_NO );

				MUS_SetMouseRectFlag( MouseLayer, i*5+0, Avg.voice );	
				MUS_SetMouseRectFlag( MouseLayer, i*5+1, Avg.voice );	

				
				DSP_SetGraphSMove( GrpSetting+2, (Avg.bgm?8:0), 0 );
				DSP_SetGraphSMove( GrpSetting+3, (Avg.se?8:0), 0 );
				DSP_SetGraphSMove( GrpSetting+4, (Avg.voice?8:0), 0 );


				
				for(i=0;i<CHAR_VOICE_MAX;i++){
					if(CharVoiceFlag[i]){
						DSP_SetGraphBmp( GrpSetting+10+(6+i/4)*5+i%4, BmpSetting+5 );
					}else{
						DSP_SetGraphBmp( GrpSetting+10+(6+i/4)*5+i%4, BmpSetting+2 );
					}
				}

				i=8;
				if(Avg.msg_page){
					DSP_SetGraphBmp( GrpSetting+10+i*5+0, BmpSetting+5 );
					for(j=1;j<5;j++){
						DSP_SetGraphBmp( GrpSetting+10+i*5+j, BmpSetting+2 );
					}
				}else{
					DSP_SetGraphBmp( GrpSetting+10+i*5+0, BmpSetting+2 );
					for(j=1;j<5;j++){
						DSP_SetGraphBmp( GrpSetting+10+i*5+j, BmpSetting+((Avg.msg_wait==j-1)?5:2) );
					}
				}
				i=9;
				DSP_SetGraphBmp( GrpSetting+10+i*5+0, BmpSetting+(Avg.msg_cut_optin?2:5) );
				DSP_SetGraphBmp( GrpSetting+10+i*5+1, BmpSetting+(Avg.msg_cut_optin?5:2) );

				for(i=0;i<10;i++){
					for(j=0;j<5;j++){
						DSP_SetGraphParam( GrpSetting+10+i*5+j, DRW_NML );
					}
				}
				DSP_SetGraphSMove( GrpSetting+1, 0, 28*1 );
				DSP_SetGraphParam( GrpSetting+1, DRW_NML );
				if(select!=-1){
					int	fade = 256-STD_LimitLoop( select_fade, 128 );
					select_fade+=8;

					int	select_flag=0;
					if(select<50){
						if(select==5*3+0 || select==5*4+0 || select==5*5+0){
						}else{
							if(DSP_GetGraphBmpNo(GrpSetting+10+select)==BmpSetting+5){
								select_flag=1;
								DSP_SetGraphBmp( GrpSetting+10+select, BmpSetting+6 );
							}else{
								select_flag=0;
								DSP_SetGraphBmp( GrpSetting+10+select, BmpSetting+3 );
							}
							DSP_SetGraphParam( GrpSetting+10+select, DRW_BLD(fade) );
						}
					}else{
						DSP_SetGraphSMove( GrpSetting+1, 0, 28*2 );
						DSP_SetGraphParam( GrpSetting+1, DRW_BLD(fade) );
					}
					if( click ){
						MainWindow.draw_flag=1;
						if( !( (select/5==3 && (select%5==0 || select%5==1)) 
							|| (select/5==4 && (select%5==0 || select%5==1))
							|| (select/5==5 && (select%5==0 || select%5==1))
							|| (select/5==0)
							) ){
							AVG_PlaySE3( 9104, 255 );
						}
						if(select<50){
							if( !(select==5*3+0 || select==5*4+0 || select==5*5+0) ){
								DSP_SetGraphBmp( GrpSetting+10+select, BmpSetting+((select_flag)?7:4) );
							}else{
								DSP_SetGraphSMove( GrpSetting+2+(select/5-3), 16, 0 );
							}
						}
						HMENU	hmenu = GetMenu( MainWindow.hwnd );
						switch(select/5){
							case 0:
								if( MainWindow.full_screen == 0 && select%5==1 ) break;
								if( MainWindow.full_screen != 0 && select%5==0 ) break;
								MainWindow.full_screen = !LIM(select%5,0,1);
								if(MainWindow.full_screen){
									SendMessage( (HWND)MainWindow.hwnd, WM_COMMAND, ID_DISP_FULL, 0 );
								}else{
									SendMessage( (HWND)MainWindow.hwnd, WM_COMMAND, ID_DISP_WINDOW, 0 );
								}
								SendMessage( (HWND)MainWindow.hwnd, WM_LBUTTONUP, 0, 0 );
								break;
							case 1:
								switch(select%5){
								default:
								case 0:	Avg.side_option = 1;	break;
								case 1:	Avg.side_option = 0;	break;
								case 2:	Avg.side_option = 3;	break;
								}
								break;
							case 2:
								switch(select%5){
								default:
								case 0:	SendMessage( (HWND)MainWindow.hwnd, WM_COMMAND, ID_EFFECT_CUT, 0 );	break;
								case 1:	SendMessage( (HWND)MainWindow.hwnd, WM_COMMAND, ID_EFFECT_HIGH, 0 );	break;
								case 2:	SendMessage( (HWND)MainWindow.hwnd, WM_COMMAND, ID_EFFECT_NORMAL, 0 );	break;
								case 3:	SendMessage( (HWND)MainWindow.hwnd, WM_COMMAND, ID_EFFECT_LOW, 0 );	break;
								}

								break;
							case 3:
								switch(select%5){
								default:
								case 0:
									Avg.bgm_vol = LIM(MUS_GetMousePosX()-(307-4),0,256);
									break;
								case 1:	AVG_PlayBGMEx( 24, 0, TRUE, 255, FALSE, FALSE );	break;
								case 2:	SendMessage( (HWND)MainWindow.hwnd, WM_COMMAND, ID_BGM_CHECK, 0 );	break;
								}
								break;
							case 4:
								switch(select%5){
								default:
								case 0:
									Avg.se_vol = LIM(MUS_GetMousePosX()-(307-4),0,256);
									break;
								case 1:	AVG_PlaySE2( 0, 8025, 0, FALSE, 255 );	break;
								case 2:	SendMessage( (HWND)MainWindow.hwnd, WM_COMMAND, ID_SE_CHECK, 0 );	break;
								}
						
					
								break;
							case 5:
								switch(select%5){
								default:
								case 0:
									Avg.voice_vol = LIM(MUS_GetMousePosX()-(307-4),0,256);
									break;
								case 1:	AVG_SizukuPlayVoice( 0, 1803, -1, -1, 256, FALSE,TRUE );	break;
								case 2:	SendMessage( (HWND)MainWindow.hwnd, WM_COMMAND, ID_VOICE_CHECK, 0 );	break;
								}
								break;
							case 6:
							case 7:
								CharVoiceFlag[ (select/5-6)*4+LIM(select%5,0,4) ] = !CharVoiceFlag[ (select/5-6)*4+LIM(select%5,0,4) ];
								break;
							case 8:
								switch(select%5){
								default:

								case 1:	SendMessage( (HWND)MainWindow.hwnd, WM_COMMAND, ID_TEXT_CUT, 0 );		break;
								case 2:	SendMessage( (HWND)MainWindow.hwnd, WM_COMMAND, ID_TEXT_HIGH, 0 );		break;
								case 3:	SendMessage( (HWND)MainWindow.hwnd, WM_COMMAND, ID_TEXT_NORMAL, 0 );	break;
								case 4:	SendMessage( (HWND)MainWindow.hwnd, WM_COMMAND, ID_TEXT_LOW, 0 );		break;
								}
								break;
							case 9:
								switch(select%5){
								default:
								case 0:	SendMessage( (HWND)MainWindow.hwnd, WM_COMMAND, ID_SKIP_READ, 0 );	break;
								case 1:	SendMessage( (HWND)MainWindow.hwnd, WM_COMMAND, ID_SKIP_NOREAD, 0 );	break;
								}

								break;
							case 10:
								DSP_SetGraphSMove( GrpSetting+1, 0, 28*3 );
								DSP_SetGraphParam( GrpSetting+1, DRW_NML );

								SideBarWindow.mode = SIDE_CLOSE;
								SideBarWindow.cnt = 0;
								break;
						}
					}else if(drag){
						if( (select==5*3+0 || select==5*4+0 || select==5*5+0) ){
							DSP_SetGraphSMove( GrpSetting+2+(select/5-3), 16, 0 );
						}

						if(select/5==3 && select%5==0){
							Avg.bgm_vol = LIM(MUS_GetMousePosX()-(307-4),0,256);
						}
						if(select/5==4 && select%5==0){
							Avg.se_vol = LIM(MUS_GetMousePosX()-(307-4),0,256);
						}
						if(select/5==5 && select%5==0){
							Avg.voice_vol = LIM(MUS_GetMousePosX()-(307-4),0,256);
						}
					}
				}
				if(cansel){
					AVG_PlaySE3( 9107, 255 );
					SideBarWindow.mode = SIDE_CLOSE;
					SideBarWindow.cnt = 0;
				}
				
				DSP_SetGraphPos( GrpSetting+10+3*5+3, 292, 293+36*0, 0, 36*3, 307-292+Avg.bgm_vol,  34 );
				DSP_SetGraphPos( GrpSetting+10+4*5+3, 292, 293+36*1, 0, 36*4, 307-292+Avg.se_vol,    34 );
				DSP_SetGraphPos( GrpSetting+10+5*5+3, 292, 293+36*2, 0, 36*4, 307-292+Avg.voice_vol, 34 );
				DSP_SetGraphMove( GrpSetting+2, 307-4+Avg.bgm_vol,   293+36*0 );
				DSP_SetGraphMove( GrpSetting+3, 307-4+Avg.se_vol,    293+36*1 );
				DSP_SetGraphMove( GrpSetting+4, 307-4+Avg.voice_vol, 293+36*2 );
				break;
		}

		if( rate<256 ){
			for(i=0;i<10;i++){
				for(j=0;j<5;j++){
					DSP_SetGraphDisp( GrpSetting+10+i*5+j, ON  );
					DSP_SetGraphParam( GrpSetting+10+i*5+j, DRW_BLD(rate)  );
				}
			}
			for(i=0;i<5;i++){
				DSP_SetGraphDisp( GrpSetting+i, ON );
				DSP_SetGraphParam( GrpSetting+i, DRW_BLD(rate)  );
			}

			ret = 1;
		}else{

			for(i=0;i<10;i++){
				for(j=0;j<5;j++){
					DSP_SetGraphDisp( GrpSetting+10+i*5+j, ON  );
					DSP_SetGraphParam( GrpSetting+10+i*5+j, DRW_NML  );
				}
			}
			for(i=0;i<5;i++){
				DSP_SetGraphDisp( GrpSetting+i, ON );
				DSP_SetGraphParam( GrpSetting+i, DRW_NML );
			}
			ret = 2;
		}
	}
	return ret;
}





void GWIN_SetSideBarWindow( int bmp_set, int grp_set, int txt_set, int mus_lay, int mus_bak )
{
	int		bmp_bpp = BPP(MainWindow.draw_mode2);

	BmpSetting=bmp_set;
	GrpSetting=grp_set;
	TxtSetting=txt_set;

	MouseLayer     = mus_lay;
	MouseLayerBack = mus_bak;

	SideBarWindow.flag=1;
	SideBarWindow.cnt =0;
	SideBarWindow.mode=SIDE_OPEN;

	DSP_LoadBmp( BmpSetting+0,  BMP_FULL, "sys0101.tga" );
	DSP_LoadBmp( BmpSetting+1,  BMP_TRUE, "sys0110.tga" );
	DSP_LoadBmp( BmpSetting+2,  BMP_TRUE, "sys0111.tga" );

	DSP_SetGraph( GrpSetting+0, BmpSetting, LAY_SYSTEM+2, ON, CHK_NO );

	DSP_SetGraph( GrpSetting+1, BmpSetting+1, LAY_SYSTEM+3, ON, CHK_ANTI );
	DSP_SetGraph( GrpSetting+2, BmpSetting+1, LAY_SYSTEM+3, ON, CHK_ANTI );
	DSP_SetGraph( GrpSetting+3, BmpSetting+1, LAY_SYSTEM+3, ON, CHK_ANTI );
	DSP_SetGraph( GrpSetting+4, BmpSetting+1, LAY_SYSTEM+3, ON, CHK_ANTI );

	DSP_SetGraph( GrpSetting+5, BmpSetting+2, LAY_SYSTEM+3, ON, CHK_ANTI );

	
	MUS_SetMouseLayer( MouseLayer );
	int	i,j;
	for(i=0;i<4;i++){
		DSP_SetGraphPos( GrpSetting+1+i, 200, 112+88*i, 400*(i%2), 492+82*3*(i/2), 400, 82 );
		MUS_SetMouseRect( MouseLayer, i, 200, 112+88*i, 400, 82, ON );
	}
	DSP_SetGraphPos( GrpSetting+5, 306, 480, 0, 0, 188, 32 );
	MUS_SetMouseRect( MouseLayer, 5, 306, 480, 188, 32, ON );	

	MainWindow.draw_flag=ON;
}

void GWIN_ResetSideBarWindow( void )
{
	int		i;
	SideBarWindow.flag=0;

	for(i=0;i<6;i++)	DSP_ResetGraph( GrpSetting+i );
	for(i=0;i<3;i++)	DSP_ReleaseBmp( BmpSetting+i );

	MUS_ResetMouseRect_Layer( MouseLayer );
	MUS_SetMouseLayer( MouseLayerBack );
}


int GWIN_ControlSideBarWindow(void)
{
	int		cmax = AVG_EffCnt(-1);
	int		select = MUS_GetMouseNoEx(-1, MouseLayer );
	int		click  = GameKey.click;
	int		cansel = GameKey.cansel;
	int		drag   = MUS_GetMouseButton(MOUSE_LBUTTON);
	int		rate=256;
	int		ret = -1;
	static int	select_fade = 0;
	static int	select_back;
	int	i,j;

	if( SideBarWindow.flag ){
		switch( SideBarWindow.mode ){
			case SIDE_NOT:
				break;
			case SIDE_OPEN:
				SideBarWindow.cnt++;
				if( SideBarWindow.cnt>=cmax  ){
					SideBarWindow.mode = SIDE_NORMAL;
					rate = 256;
				}else{
					rate = 256*SideBarWindow.cnt/cmax;
				}
				break;
			case SIDE_CLOSE:
				SideBarWindow.cnt++;
				if( SideBarWindow.cnt>=cmax  ){
					SideBarWindow.mode = SIDE_NOT;
					rate=0;
					GWIN_ResetSideBarWindow();	
					return 0;
				}else{
					rate = 256-256*SideBarWindow.cnt/cmax;
				}
				break;
			case SIDE_NORMAL:

				for(i=0;i<4;i++){
					DSP_SetGraphFade( GrpSetting+1+i, 128 );
					DSP_SetGraphSMove( GrpSetting+1+i, 400*(i%2), 492+82*3*(i/2) );
				}
				DSP_SetGraphFade( GrpSetting+1+Avg.side_option, 192 );
				DSP_SetGraphSMove( GrpSetting+5, 188*0, 0 );

				if(select!=-1){
					if(select != select_back )
						AVG_PlaySE3( 9108, 255 );

					if(select<4){
						DSP_SetGraphSMove( GrpSetting+1+select, 400*(select%2), 492+82*3*(select/2)+82*1 );
					}else{
						DSP_SetGraphSMove( GrpSetting+5, 188*1, 0 );
					}

					if( click ){
						if(select<4){
							DSP_SetGraphSMove( GrpSetting+1+select, 400*(select%2), 492+82*3*(select/2)+82*1 );

							AVG_PlaySE3( 9104, 255 );
							Avg.side_option = select;
						}else{
							DSP_SetGraphSMove( GrpSetting+5, 188*2, 0 );

							AVG_PlaySE3( 9104, 255 );
							SideBarWindow.mode = SIDE_CLOSE;
							SideBarWindow.cnt = 0;
						}
					}
				}
				if(cansel){
					AVG_PlaySE3( 9107, 255 );
					SideBarWindow.mode = SIDE_CLOSE;
					SideBarWindow.cnt = 0;
				}
				select_back=select;
				break;
		}

		if( rate<256 ){
			for(i=0;i<6;i++){
				DSP_SetGraphDisp( GrpSetting+i, ON );
				DSP_SetGraphParam( GrpSetting+i, DRW_BLD(rate)  );
			}
			ret = 1;
		}else{
			for(i=0;i<6;i++){
				DSP_SetGraphDisp( GrpSetting+i, ON );
				DSP_SetGraphParam( GrpSetting+i, DRW_NML );
			}
			ret = 2;
		}
	}
	return ret;
}




enum{
	SET_NOT,
	SET_OPEN,
	SET_CLOSE,
	SET_NORMAL,
};

typedef struct{
	int		flag;
	int		mode;
	int		cnt;
	int		type;

	short	select[9];
	short	backup[9];
	short	count[9];
}SETTING_WINDOW;
SETTING_WINDOW		SettingWindow;

char	*SettingMes[9][5]={
	{ "　表示サイズ　", "　ウィンドウ　", "フルスクリーン", "", "" },
	{ "　 視覚効果 　", "　　標　準　　", "　　簡　易　　", "", "" },
	{ "　ウインドウ　", " 標準(半透明) ", " 簡易(網掛け) ", "", "" },
	{ "表示エフェクト", "　無し　", "　標準　", "　高速　", "" },

	{ "　　　曲　　　", "　　無　し　　", "　　有　り　　", "", "" },
	{ "フェードアウト", "　　無　し　　", "　　有　り　　", "", "" },
	{ "　 効 果 音 　", "　　無　し　　", "　　有　り　　", "", "" },

	{ "　 表示速度 　", "　標準　", "　高速　", "瞬間表示", "" },

	{ "フォントタイプ", "　標準　", "ゴシック", "　明朝　", "" },
};

RECT2	SettingMouse[9][4]={
	{	{218, 86,169,28},	{389, 86,169,28},	{0,0,0,0},			{0,0,0,0}	},
	{	{218,118,169,28},	{389,118,169,28},	{0,0,0,0},			{0,0,0,0}	},
	{	{218,150,169,28},	{389,150,169,28},	{0,0,0,0},			{0,0,0,0}	},
	{	{218,182,112,28},	{332,182,112,28},	{446,182,112,28},	{0,0,0,0}	},

	{	{218,246,169,28},	{389,246,169,28},	{0,0,0,0},			{0,0,0,0}	},
	{	{218,278,169,28},	{389,278,169,28},	{0,0,0,0},			{0,0,0,0}	},
	{	{218,310,169,28},	{389,310,169,28},	{0,0,0,0},			{0,0,0,0}	},
	
	{	{218,374,112,28},	{332,374,112,28},	{446,374,112,28},	{0,0,0,0}	},

	{	{218,406,112,28},	{332,406,112,28},	{446,406,112,28},	{0,0,0,0}	},
};

static void GWIN_InitSettingBar( void )
{
	int	i;


	SettingWindow.backup[0] = SettingWindow.select[0] = MainWindow.full_screen;

	SettingWindow.backup[1] = SettingWindow.select[1] = (Avg.level)?0:1;

	SettingWindow.backup[2] = SettingWindow.select[2] = Avg.win_bld-1;

	switch(Avg.wait){
		case 0:	SettingWindow.backup[3] = SettingWindow.select[3] = 0;	break;
		case 1:	SettingWindow.backup[3] = SettingWindow.select[3] = 2;	break;
		case 2:	SettingWindow.backup[3] = SettingWindow.select[3] = 1;	break;
	}

	SettingWindow.backup[4] = SettingWindow.select[4] = (short)Avg.bgm;

	SettingWindow.backup[5] = SettingWindow.select[5] = (short)Avg.cd_fade;

	SettingWindow.backup[6] = SettingWindow.select[6] = (short)Avg.se;

	switch(Avg.msg_wait){
		case 0:	SettingWindow.backup[7] = SettingWindow.select[7] = 2;	break;
		case 1:	SettingWindow.backup[7] = SettingWindow.select[7] = 1;	break;
		case 2:	SettingWindow.backup[7] = SettingWindow.select[7] = 0;	break;
	}

	SettingWindow.backup[8] = SettingWindow.select[8] = (short)Avg.msg_font;

	for(i=0;i<9;i++)
		SettingWindow.count[i] = 256;
}
static void GWIN_CreateSettingBarBMP( int bno, int w, int h )
{
	int		bmp_bpp = BPP(MainWindow.draw_mode2);
	
	DSP_CreateBmp( bno,   bmp_bpp, w, h );
	
	DSP_SetGraphPrim(       GrpSetting, PRM_YGRA, POL_RECT, 0, ON );
	DSP_SetGraphPosRect(    GrpSetting, 2,  2, w-4, h/2-2 );


	DSP_SetGraphBrightYGra( GrpSetting, 0,  2,  33, 194, 180 );
	DSP_SetGraphBrightYGra( GrpSetting, 1, h/2, 140, 251, 240 );
	DSP_SetGraphTarget(     GrpSetting, bno, MainWindow.draw_mode2 );

	DSP_SetGraphPrim(       GrpSetting, PRM_YGRA, POL_RECT, 0, ON );
	DSP_SetGraphPosRect(    GrpSetting, 2, h/2, w-4, h/2-2 );


	DSP_SetGraphBrightYGra( GrpSetting, 0, h/2, 140, 251, 240 );
	DSP_SetGraphBrightYGra( GrpSetting, 1, h-2,  33, 194, 180 );
	DSP_SetGraphTarget(     GrpSetting, bno, MainWindow.draw_mode2 );

	DSP_SetGraphPrim(    GrpSetting, PRM_FLAT, POL_BOX, 0, ON );
	DSP_SetGraphPosRect( GrpSetting, 0, 0, w, h );
	DSP_SetGraphBright(  GrpSetting, 172, 230, 223 );
	DSP_SetGraphTarget(  GrpSetting, bno, MainWindow.draw_mode2 );

	DSP_ResetGraph( GrpSetting );
}

static void GWIN_CreateSettingBar( int *gno, int *tno, int num, int mes )
{
	int	i=0;
	switch(num){
		case 2:
			for(i=0;i<2;i++)
				DSP_SetGraph( *gno+i,  BmpSetting, LAY_SYSTEM+2, ON, CHK_NO );
			for(i=2;i<5;i++){
				DSP_SetGraphPrim(   *gno+i, PRM_FLAT, POL_RECT, LAY_SYSTEM+2, ON );
				DSP_SetGraphBright( *gno+i, 172, 230, 223 );
			}
			for(i=0;i<3;i++)
				DSP_SetText( *tno+i, LAY_SYSTEM+2, SYS_FONT, ON, SettingMes[mes][i] );
			(*gno)+=5;
			(*tno)+=3;
			break;
		case 3:
			for(i=0;i<3;i++)
				DSP_SetGraph( *gno+i,   BmpSetting+1, LAY_SYSTEM+2, ON, CHK_NO );
			for(i=3;i<7;i++){
				DSP_SetGraphPrim(   *gno+i, PRM_FLAT, POL_RECT, LAY_SYSTEM+2, ON );
				DSP_SetGraphBright( *gno+i, 172, 230, 223 );
			}
			for(i=0;i<4;i++)
				DSP_SetText( *tno+i, LAY_SYSTEM+2, SYS_FONT, ON, SettingMes[mes][i] );
			(*gno)+=7;
			(*tno)+=4;
			break;
		case 4:
			for(i=0;i<4;i++)
				DSP_SetGraph( *gno+i,   BmpSetting+2, LAY_SYSTEM+2, ON, CHK_NO );
			for(i=4;i<9;i++){
				DSP_SetGraphPrim(   *gno+i, PRM_FLAT, POL_RECT, LAY_SYSTEM+2, ON );
				DSP_SetGraphBright( *gno+i, 172, 230, 223 );
			}




			DSP_SetText( *tno, LAY_SYSTEM+2, SYS_FONT, ON, SettingMes[mes][0] );
			for(i=0;i<4;i++){
				DSP_SetGraph( *gno+9+i,   BmpSetting+3, LAY_SYSTEM+2, ON, CHK_NO );
				DSP_SetGraphParam( *gno+9+i, DRW_ADD );
				DSP_SetGraphPos( *gno+9+i, 0, 0, 80*i, 0, 77, 20 );

			}
			(*gno)+=9+4;
			(*tno)+=5-4;
			break;
	}
}

static void GWIN_SetSettingBar( int gno, int num, int light, int select, int backup, int cnt )
{
	int	i;

	DSP_SetGraphDisp( gno++, light );
	DSP_SetGraphDisp( gno++, light );

	for(i=0;i<num;i++, gno++){
		DSP_SetGraphDisp( gno, ON );
		if(select==i)		{	DSP_SetGraphParam( gno, DRW_BLD(cnt) );		}
		else if(backup==i)	{	DSP_SetGraphParam( gno, DRW_BLD(256-cnt) );	}
		else				{	DSP_SetGraphDisp( gno, OFF );				}
	}
}
void GWIN_SetSettingWindow( int bmp_set, int grp_set, int txt_set, int grp_bak, int txt_bak, int mus_lay, int mus_bak, int title )
{
	int		bmp_bpp = BPP(MainWindow.draw_mode2);
	int		i,j;
	char	buf[256];

	BmpSetting=bmp_set;
	GrpSetting=grp_set;
	TxtSetting=txt_set;

	GrpBack=grp_bak;
	TxtBack=txt_bak;

	MouseLayer     = mus_lay;
	MouseLayerBack = mus_bak;
	
	SettingWindow.flag=1;
	SettingWindow.cnt =0;
	SettingWindow.mode=SET_OPEN;
	SettingWindow.type=title;

	GWIN_InitSettingBar();

	wsprintf( buf, "p01%d1%c.bmp", title, 'a'+Avg.msg_font );
	DSP_LoadBmp( BmpSetting+3, bmp_bpp, buf );
	DSP_LoadBmp( BmpSetting+4, bmp_bpp, "p0102.bmp" );
	DSP_LoadBmp( BmpSetting+5, BMP_TRUE, "p0103.bmp" );


	DSP_LoadBmp( BmpSetting+6, BMP_256P, "f0007.bmp" );

	DSP_SetGraph( GrpSetting, BmpSetting+3, LAY_BACK, OFF, CHK_NO );

	DSP_SetGraph( GrpSetting+1, BmpSetting+4, LAY_BACK+1, OFF, CHK_NO );
	DSP_SetGraphBright( GrpSetting+1, 255, 255, 255 );
	DSP_SetGraphParam(  GrpSetting+1, DRW_SUB );	

	DSP_SetGraph( GrpSetting+2, BmpSetting+5, LAY_SYSTEM+2, OFF, CHK_ANTI );


	
	for( i=0; i<9 ;i++ ){
		int		addr1 = 3+i*6;
		RECT2	rect = SettingMouse[i][0];
		DSP_SetGraph( GrpSetting+addr1+0, BmpSetting+3, LAY_BACK, OFF, CHK_NO );
		DSP_SetGraph( GrpSetting+addr1+1, BmpSetting+3, LAY_BACK, OFF, CHK_NO );
		DSP_SetGraphPos( GrpSetting+addr1+0,  43, rect.y,  43, rect.y+480, 173, 28 );
		DSP_SetGraphPos( GrpSetting+addr1+1, 560, rect.y, 560, rect.y+480,  37, 28 );
		for( j=0; j<4 ;j++ ){
			int		addr2 = 3+i*6+j+2;
			int		addr3 = i*4+j;
			rect = SettingMouse[i][j];

			if(rect.x){
				DSP_SetGraph(    GrpSetting+addr2, BmpSetting+3, LAY_BACK, OFF, CHK_NO );
				DSP_SetGraphPos( GrpSetting+addr2, rect.x, rect.y, rect.x, rect.y+480, rect.w, rect.h );

				MUS_SetMouseRect( MouseLayer, addr3, rect.x, rect.y, rect.w, rect.h, ON );
			}
		}
	}
	DSP_SetGraph(    GrpSetting+3+9*6, BmpSetting+3, LAY_BACK, OFF, CHK_NO );
	DSP_SetGraphPos( GrpSetting+3+9*6, 218, 438, 218, 438+480, 204, 28 );
	MUS_SetMouseRect( MouseLayer, 4*9, 218, 438, 204, 28, ON );
}

static void GWIN_ResetSettingWindow( void )
{
	int		i;
	SettingWindow.flag=0;

	for(i=0;i<70;i++)	DSP_ResetGraph( GrpSetting+i );
	for(i=0;i<40;i++)	DSP_ResetText(  TxtSetting+i );
	for(i=0;i<4;i++)	DSP_ReleaseBmp( BmpSetting+i );
	MUS_ResetMouseRect_Layer( MouseLayer );
	MUS_SetMouseLayer( MouseLayerBack );

	DSP_SetGraphDisp( GrpBack, ON );

	AVG_CloseNormalDisp();
}

BOOL GWIN_ControlSettingWindow(void)
{
	int		cmax = AVG_EffCnt(-2);
	int		select = MUS_GetMouseNoEx( -1, MouseLayer );
	int		select2;
	int		offtrg = MUS_GetMouseOffTrigger2(MOUSE_LBUTTON) || KeyCond.trg.space;
	int		click  = GameKey.click;
	int		cansel = GameKey.cansel;
	int		rate=256, rate2;
	int		y, key_flag=0;
	int		i,j;

	if( SettingWindow.flag ){
		switch( SettingWindow.mode ){
			case SET_NOT:
				break;
			case SET_OPEN:
				SettingWindow.cnt++;
				if( SettingWindow.cnt>=cmax  ){
					MUS_SetMouseLayer( MouseLayer );
					SettingWindow.mode = SET_NORMAL;
					rate=256;
				}else{
					rate = 256*SettingWindow.cnt/cmax;
				}
				break;
			case SET_CLOSE:
				SettingWindow.cnt++;
				if( SettingWindow.cnt>=cmax  ){
					SettingWindow.mode = SET_NOT;
					rate=0;

					GWIN_ResetSettingWindow();			






					return FALSE;
				}else{
					rate = 256-256*SettingWindow.cnt/cmax;
				}
				break;
			case SET_NORMAL:
				if(GameKey.l) {	select2 = select-1;	key_flag=1; }
				if(GameKey.r) {	select2 = select+1;	key_flag=1;	}
				if(GameKey.u) { select2 = select-4; key_flag=2; }
				if(GameKey.d) {	select2 = select+4;	key_flag=2; }
				if(key_flag){
					if(select==-1){
						select2=0;
					}
					if( MUS_GetMouseRectFlag(MouseLayer,select2) ){
						select = select2;
						MUS_SetMousePosRect( MainWindow.hwnd, MouseLayer, select );
					}else{
						if(key_flag==2){
							for(i=0;i<5;i++){
								if( MUS_GetMouseRectFlag(MouseLayer,select2-i) ){
									select = select2-i;
									break;
								}
							}
							MUS_SetMousePosRect( MainWindow.hwnd, MouseLayer, select );
						}
					}
				}

				DSP_SetGraphDisp( GrpSetting+3+6*9, OFF );	
				DSP_SetGraphDisp( GrpSetting+2, OFF );		
				if( select!=-1 ){
					DSP_SetGraphDisp( GrpSetting+2, ON );
					if(select<36){
						RECT2	rect = SettingMouse[select/4][select%4];

						switch( rect.w ){
							case 169: y = 40;	break;
							case 112: y = 80;	break;
							case 82:  y = 120;	break;
						}
						DSP_SetGraphPos( GrpSetting+2, rect.x-6, rect.y-6, 0, y, 216, 40 );
					}else{
						DSP_SetGraphDisp( GrpSetting+3+6*9, ON );
						DSP_SetGraphPos( GrpSetting+2, 212, 432, 0, 0, 216, 40 );
					}
				}

				i = select/4;
				j = select%4;

				if( MainWindow.full_screen!=SettingWindow.select[0] ){
					select=MainWindow.full_screen;
					i=0;
					j=MainWindow.full_screen;
					offtrg=ON;
				}
				if( (click && i!=0) || (offtrg && i==0)){

					if(select==36){
						AVG_PlaySE3( 9107, 255 );
						SettingWindow.cnt = 0;
						SettingWindow.mode = SET_CLOSE;
					}else if(select!=-1){
						if( SettingWindow.select[i] != j ){
							if(SettingWindow.count[i]>=256){
								SettingWindow.select[i] = j;
								SettingWindow.count[i]  = 0;
							}else{
								SettingWindow.backup[i] = SettingWindow.select[i];
								SettingWindow.select[i] = j;
								SettingWindow.count[i]  = 0;
							}
						}
						if( i!=0 )AVG_PlaySE3( 9002, 255 );

						switch(i){
							case 0:	
								MainWindow.full_screen = j;
								if(MainWindow.full_screen){
									SendMessage( (HWND)MainWindow.hwnd, WM_COMMAND, ID_DISP_FULL, 0 );
								}else{
									SendMessage( (HWND)MainWindow.hwnd, WM_COMMAND, ID_DISP_WINDOW, 0 );
								}
								SendMessage( (HWND)MainWindow.hwnd, WM_LBUTTONUP, 0, 0 );
								AVG_PlaySE3( 9002, 255 );
								break;
							case 1:	
								if(j){
									Avg.level = 0;			

								}else{
									Avg.level = 1;			

								}
								break;
							case 2:	
								Avg.win_bld = j+1;		
								break;
							case 3:	
								switch(j){
									case 0:	Avg.wait=0;	break;
									case 1:	Avg.wait=2;	break;
									case 2:	Avg.wait=1;	break;
								}
								break;
							case 4:	AVG_SetBGMFlag(j);	break;	
							case 5:	Avg.cd_fade = j;	break;	
							case 6:	AVG_SetSEFlag(j);	break;	
							case 7:	
								switch(j){
									case 0:	Avg.msg_wait=2;	break;
									case 1:	Avg.msg_wait=1;	break;
									case 2:	Avg.msg_wait=0;	break;
								}
								break;
							case 8:	
								Avg.msg_font = j;

								{
									char	buf[256];
									wsprintf( buf, "p01%d1%c.bmp", SettingWindow.type, 'a'+Avg.msg_font );
									DSP_LoadBmp( BmpSetting+3, BPP(MainWindow.draw_mode2), buf );
								}
								break;
						}
						WIN_InitMenu();
					}
				}
				if(cansel){
					AVG_PlaySE3( 998, 255 );

					SettingWindow.cnt = 0;
					SettingWindow.mode = SET_CLOSE;
				}
				break;
		}
		if( SettingWindow.mode==SET_CLOSE || SettingWindow.mode==SET_OPEN ){
			DSP_SetGraphDisp( GrpSetting+2, OFF );

			if( (SettingWindow.cnt >= cmax/2 && SettingWindow.mode==SET_OPEN) ||
				(SettingWindow.cnt < cmax/2 && SettingWindow.mode==SET_CLOSE) ){
				rate2 = LIM((rate-128)*2,0,255);
				DSP_SetGraphDisp( GrpBack, OFF );
				DSP_SetGraphDisp( GrpSetting, ON );
				DSP_SetGraphDisp( GrpSetting+1, ON );

				DSP_SetGraphFade( GrpSetting+1, 255-rate2 );

				for(i=0;i<5;i++){
					DSP_SetTextDisp( TxtBack+i, OFF );
				}
			}else{
				rate2 = LIM(rate*2,0,255);

				DSP_SetGraphDisp( GrpBack, ON );
				DSP_SetGraphDisp( GrpSetting, OFF );
				DSP_SetGraphDisp( GrpSetting+1, OFF );

				DSP_SetGraphFade( GrpBack, 128-rate2/2 );
				for(i=0;i<5;i++){
					rate2 = 256-min( rate, 40*(6-i) )*256/(40*(6-i));
					DSP_SetTextDisp( TxtBack+i, ON );
					DSP_SetTextAlph( TxtBack+i, rate2 );
				}
			}
		}else{
			{
				DSP_SetGraphDisp( GrpBack, OFF );
				DSP_SetGraphDisp( GrpSetting, ON );
				DSP_SetGraphDisp( GrpSetting+1, OFF );


				for(i=0;i<5;i++){
					DSP_SetTextDisp( TxtBack+i, OFF );
				}
			}




		}


		for(i=0;i<9;i++){
			if( SettingWindow.backup[i] != SettingWindow.select[i] ){
				SettingWindow.count[i] += (short)(1200/Avg.frame);
				if( SettingWindow.count[i]>=256 ){
					SettingWindow.backup[i] = SettingWindow.select[i];
					SettingWindow.count[i]  = 256;
				}
			}else{
				SettingWindow.count[i]=256;
			}
			for( j=0; j<4 ;j++ ){
				int		addr2 = 3+i*6+j+2;
				DSP_SetGraphFade( GrpSetting+addr2, 112+STD_LimitLoop(GlobalCount2/2,48) );
			}
		}

		GWIN_SetSettingBar( GrpSetting+ 3, 2, select/4==0 && select!=-1, SettingWindow.select[0], SettingWindow.backup[0], SettingWindow.count[0] );
		GWIN_SetSettingBar( GrpSetting+ 9, 2, select/4==1, SettingWindow.select[1], SettingWindow.backup[1], SettingWindow.count[1] );
		GWIN_SetSettingBar( GrpSetting+15, 2, select/4==2, SettingWindow.select[2], SettingWindow.backup[2], SettingWindow.count[2] );
		GWIN_SetSettingBar( GrpSetting+21, 3, select/4==3, SettingWindow.select[3], SettingWindow.backup[3], SettingWindow.count[3] );
		GWIN_SetSettingBar( GrpSetting+27, 2, select/4==4, SettingWindow.select[4], SettingWindow.backup[4], SettingWindow.count[4] );
		GWIN_SetSettingBar( GrpSetting+33, 2, select/4==5, SettingWindow.select[5], SettingWindow.backup[5], SettingWindow.count[5] );
		GWIN_SetSettingBar( GrpSetting+39, 2, select/4==6, SettingWindow.select[6], SettingWindow.backup[6], SettingWindow.count[6] );
		GWIN_SetSettingBar( GrpSetting+45, 3, select/4==7, SettingWindow.select[7], SettingWindow.backup[7], SettingWindow.count[7] );
		GWIN_SetSettingBar( GrpSetting+51, 4, select/4==8, SettingWindow.select[8], SettingWindow.backup[8], SettingWindow.count[8] );
	}
	return TRUE;
}
