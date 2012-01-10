#include <windows.h>
#include <mouse.h>
#include <keybord.h>

#include <STD_Windows.h>
#include <STD_Debug.h>
#include <STD_File.h>

#include "comp_pac.h"

#include "GlobalDraw.h"

#include "main.h"

#include "escript.h"

#include "GM_Avg.h"

#include "GM_Main.h"
#include "GM_World.h"
#include "GM_Char.h"
#include "GM_Magic.h"

int	SetCharParamWindowEquip( int win_no, int gno, int bno, int spno, int layer, int cno, int eq_type, int ino )
{
	int	work1,work2, work, iwork;
	int	i;
	char	buf[256];

	if(ino==-1){
		for( i=0 ; i < 16 ; i++ ){
			GD_SetGraphDisp( gno+40+i*2+1, FALSE );
		}
		return 1;
	}
	for( i=0 ; i < 17 ; i++ ){
		switch(i){
			case 0:	work1 = CharParam[ cno ].GetHp();	break;
			case 1:	work1 = CharParam[ cno ].GetMov();	break;

			case 2:	work1 = CharParam[ cno ].GetStr();	break;
			case 3:	work1 = CharParam[ cno ].GetVit();	break;
			case 4:	work1 = CharParam[ cno ].GetDex();	break;
			case 5:	work1 = CharParam[ cno ].GetSpd();	break;
			case 6:	work1 = CharParam[ cno ].GetWiz();	break;
			case 7:	work1 = CharParam[ cno ].GetMnd();	break;

			case 8:	work1 = CharParam[ cno ].GetMp();	break;
			case 9:	work1 = CharParam[ cno ].GetWsp();	break;

			case 10:work1 = CharParam[ cno ].GetAtk();	break;
			case 11:work1 = CharParam[ cno ].GetDef();	break;
			case 12:work1 = CharParam[ cno ].GetHit();	break;
			case 13:work1 = CharParam[ cno ].GetAvd();	break;
			case 14:work1 = CharParam[ cno ].GetMgk();	break;
			case 15:work1 = CharParam[ cno ].GetRgs();	break;

			case 16:work1 = CharParam[ cno ].GetRange()/32;	break;
		}
		switch(eq_type){
			case 0:	iwork = CharParam[ cno ].buki;			CharParam[ cno ].buki       = ino; break;
			case 1:	iwork = CharParam[ cno ].yoroi;			CharParam[ cno ].yoroi      = ino; break;
			case 2:	iwork = CharParam[ cno ].accessary1;	CharParam[ cno ].accessary1 = ino; break;
			case 3:	iwork = CharParam[ cno ].accessary2;	CharParam[ cno ].accessary2 = ino; break;
		}
		switch(i){
			case 0:	work2 = CharParam[ cno ].GetHp();	break;
			case 1:	work2 = CharParam[ cno ].GetMov();	break;
			case 2:	work2 = CharParam[ cno ].GetStr();	break;
			case 3:	work2 = CharParam[ cno ].GetVit();	break;
			case 4:	work2 = CharParam[ cno ].GetDex();	break;
			case 5:	work2 = CharParam[ cno ].GetSpd();	break;
			case 6:	work2 = CharParam[ cno ].GetWiz();	break;
			case 7:	work2 = CharParam[ cno ].GetMnd();	break;

			case 8:	work2 = CharParam[ cno ].GetMp();	break;
			case 9:	work2 = CharParam[ cno ].GetWsp();	break;
			case 10:work2 = CharParam[ cno ].GetAtk();	break;
			case 11:work2 = CharParam[ cno ].GetDef();	break;
			case 12:work2 = CharParam[ cno ].GetHit();	break;
			case 13:work2 = CharParam[ cno ].GetAvd();	break;
			case 14:work2 = CharParam[ cno ].GetMgk();	break;
			case 15:work2 = CharParam[ cno ].GetRgs();	break;

			case 16:work2 = CharParam[ cno ].GetRange()/32;	break;
		}
		switch(eq_type){
			case 0:	CharParam[ cno ].buki       = iwork; break;
			case 1:	CharParam[ cno ].yoroi      = iwork; break;
			case 2:	CharParam[ cno ].accessary1 = iwork; break;
			case 3:	CharParam[ cno ].accessary2 = iwork; break;
		}

		work = work2-work1;
		if( work ){
			if( work<0 ){	wsprintf( buf, "-%d", -work );	}
			else		{	wsprintf( buf, "+%d", work );	}
			GD_SetGraphDisp( gno+40+i*2+1, TRUE );
			GD_SetGraphStrChange( gno+40+i*2+1, buf );
			if( (i==9 && work<0) || (i!=9 && work>0) ){
				GD_SetGraphColor( gno+40+i*2+1, 255,61,116,255 );
			}else{
				GD_SetGraphColor( gno+40+i*2+1, 255,255,56,56 );
			}

		}else{
			GD_SetGraphDisp( gno+40+i*2+1, FALSE );
		}
	}
	return 1;
}

int	RenweCharParamWindow( int gno, int lm, int rm, int click )
{
	if(lm){
		if(click)	GD_SetGraphSrcRect( gno+21, 528+64, 432, 32, 48 ),appData.draw_flag=1;
		else		GD_SetGraphSrcRect( gno+21, 528+32, 432, 32, 48 );
	}else	GD_SetGraphSrcRect( gno+21, 528, 432, 32, 48 );
	if(rm){
		if(click)	GD_SetGraphSrcRect( gno+22, 528+64, 480, 32, 48 ),appData.draw_flag=1;
		else		GD_SetGraphSrcRect( gno+22, 528+32, 480, 32, 48 );
	}else		GD_SetGraphSrcRect( gno+22, 528, 480, 32, 48 );

	int	i;
	for( i=0 ; i < 17 ; i++ ){
		GD_SetGraphDisp( gno+40+i*2+1, FALSE );
	}

	return 1;
}
int	SetCharParamWindow( int win_no, int win_bno, int gno, int bno, int spno, int layer, int cno, int yajirusi_disp )
{
	char	buf[256];
	int		i,y;

	GD_SetGraph( gno+20, win_bno, layer, TRUE );

	GD_SetGraphRect( gno+20, 4, 24, 0, 432, 352, 400 );

	
	GD_SetGraph( gno+21, win_bno, layer+1, yajirusi_disp );
	GD_SetGraphRect( gno+21, 0, 72, 528, 432, 32, 48 );
	GD_SetGraph( gno+22, win_bno, layer+1, yajirusi_disp );
	GD_SetGraphRect( gno+22, 320, 72, 528, 480, 32, 48 );

	GD_SetGraph( gno+23, win_bno, layer+1, TRUE );
	GD_SetGraphRect( gno+23, 31, 14, 352+CharParam[cno].job%7*96, 640+CharParam[cno].job/7*128, 96, 128 );

	GD_SetGraph( gno+24, win_bno, layer+2, TRUE );
	GD_SetGraphRect( gno+24, 20, 16, 352, 424, 160, 24 );

	
	buf[0] = '\0';
	_mbscat( (BYTE*)buf, (BYTE*)CharParam[ cno ].name );
	_mbscat( (BYTE*)buf, (BYTE*)"\n" );
	
	_mbscat( (BYTE*)buf, (BYTE*)JobParam[ CharParam[ cno ].job ].name );
	GD_CreateTextTexture( bno+3, buf, 24,    8, 2, 0, 0, 0, 91,55,21 );
	GD_SetGraph( gno+31, bno+3, layer+1, TRUE );
	GD_SetGraphRect( gno+31, 172-10, 48, 0, 0, 24*8, 24*2 );

	
	i=max(0,CharParam[ cno ].attr);
	GD_SetGraph( gno+32, win_bno, layer+1, TRUE );
	GD_SetGraphRect( gno+32, 180, 96, 768+i*24, 320, 22, 22 );

	
	GD_SetGraphDigit( gno+33, bno+2, layer+1, TRUE, CharParam[ cno ].lv, 3 );
	GD_SetGraphRect( gno+33, 276-12, 102, 0, 16, 10, 16 );
	GD_SetGraphColor( gno+33, 255, 91, 55, 21 );

	
	buf[0] = '\0';
	wsprintf( buf, "%7d/%-7d", CharParam[ cno ].exp, CharParam[ cno ].GetNextExp() );	
	GD_SetGraphStr( gno+34, bno+2, layer+1, TRUE, buf );
	GD_SetGraphRect( gno+34, 172+12, 123, 0, 0, 10, 16 );
	GD_SetGraphColor( gno+34, 255, 91, 55, 21 );

	
	buf[0] = '\0';
	_mbscat( (BYTE*)buf, (BYTE*)ItemParam[ CharParam[ cno ].buki ].name );
	_mbscat( (BYTE*)buf, (BYTE*)"\n" );
	_mbscat( (BYTE*)buf, (BYTE*)ItemParam[ CharParam[ cno ].yoroi ].name );
	_mbscat( (BYTE*)buf, (BYTE*)"\n" );
	_mbscat( (BYTE*)buf, (BYTE*)ItemParam[ CharParam[ cno ].accessary1 ].name );
	_mbscat( (BYTE*)buf, (BYTE*)"\n" );
	_mbscat( (BYTE*)buf, (BYTE*)ItemParam[ CharParam[ cno ].accessary2 ].name );
	GD_CreateTextTexture( bno+4, buf, 24,    8, 5, 0, 0, 0, 91,55,21 );
	GD_SetGraph( gno+35, bno+4, layer+1, TRUE );
	GD_SetGraphRect( gno+35, 84, 303, 0, 0, 24*8, 24*4 );

	i = ItemParam[ CharParam[ cno ].buki ].type;
	GD_SetGraph( gno+36, win_bno, layer+1, TRUE );
	GD_SetGraphRect( gno+36, 60, 303, 768+(i%10)*24, 344+(i/10)*24, 22, 22 );
	i = ItemParam[ CharParam[ cno ].yoroi ].type;
	GD_SetGraph( gno+37, win_bno, layer+1, TRUE );
	GD_SetGraphRect( gno+37, 60, 303+24, 768+(i%10)*24, 344+(i/10)*24, 22, 22 );
	i = ItemParam[ CharParam[ cno ].accessary1 ].type;
	GD_SetGraph( gno+38, win_bno, layer+1, TRUE );
	GD_SetGraphRect( gno+38, 60, 303+48, 768+(i%10)*24, 344+(i/10)*24, 22, 22 );
	i = ItemParam[ CharParam[ cno ].accessary2 ].type;
	GD_SetGraph( gno+39, win_bno, layer+1, TRUE );
	GD_SetGraphRect( gno+39, 60, 303+72, 768+(i%10)*24, 344+(i/10)*24, 22, 22 );

	
	for( i=0 ; i < 8 ; i++ ){
		switch(i){
			case 0:	wsprintf( buf, "%5d", CharParam[ cno ].GetHp() );	y=-1;	break;

			case 1:	wsprintf( buf, "%4d", CharParam[ cno ].GetMov() );	y=-1+i*16;	break;

			case 2:	wsprintf( buf, "%4d", CharParam[ cno ].GetStr() );	y=5+i*16;	break;
			case 3:	wsprintf( buf, "%4d", CharParam[ cno ].GetVit() );	y=5+i*16;	break;
			case 4:	wsprintf( buf, "%4d", CharParam[ cno ].GetDex() );	y=5+i*16;	break;
			case 5:	wsprintf( buf, "%4d", CharParam[ cno ].GetSpd() );	y=5+i*16;	break;
			case 6:	wsprintf( buf, "%4d", CharParam[ cno ].GetWiz() );	y=5+i*16;	break;
			case 7:	wsprintf( buf, "%4d", CharParam[ cno ].GetMnd() );	y=5+i*16;	break;
		}
		GD_SetGraphStr( gno+40+i*2, bno+2, layer+1, TRUE, buf );
		if(i==0){
			GD_SetGraphRect( gno+40+i*2, 28+30, 148+y, 0, 0, 10, 16 );
		}else{
			GD_SetGraphRect( gno+40+i*2, 28+40, 148+y, 0, 0, 10, 16 );
		}
		GD_SetGraphColor( gno+40+i*2, 255, 91, 55, 21 );

		GD_SetGraphStr( gno+40+i*2+1, bno+2, layer+1, FALSE, " " );
		GD_SetGraphRect( gno+40+i*2+1,  28+40+10*4, 148+y, 0, 0, 10, 16 );

		switch(i){
			case 0:	wsprintf( buf, "%4d", CharParam[ cno ].GetMp() );	break;
			case 1:	wsprintf( buf, "%4d", CharParam[ cno ].GetWsp() );	break;

			case 2:	wsprintf( buf, "%4d", CharParam[ cno ].GetAtk() );	break;
			case 3:	wsprintf( buf, "%4d", CharParam[ cno ].GetDef() );	break;
			case 4:	wsprintf( buf, "%4d", CharParam[ cno ].GetHit() );	break;
			case 5:	wsprintf( buf, "%4d", CharParam[ cno ].GetAvd() );	break;
			case 6:	wsprintf( buf, "%4d", CharParam[ cno ].GetMgk() );	break;
			case 7:	wsprintf( buf, "%4d", CharParam[ cno ].GetRgs() );	break;

		}

		GD_SetGraphStr( gno+56+i*2, bno+2, layer+1, TRUE, buf );
		GD_SetGraphRect( gno+56+i*2, 28+40+154, 148+y, 0, 0, 10, 16 );
		GD_SetGraphColor( gno+56+i*2, 255, 91, 55, 21 );

		GD_SetGraphStr( gno+56+i*2+1, bno+2, layer+1, FALSE, " " );
		GD_SetGraphRect( gno+56+i*2+1,  28+40+10*4+154, 148+y, 0, 0, 10, 16 );
	}
	wsprintf( buf, "%4d", CharParam[ cno ].GetRange()/32 );
	GD_SetGraphStr( gno+72, bno+2, layer+1, TRUE, buf );

	GD_SetGraphRect( gno+72, 28+40+154, 285, 0, 0, 10, 16 );
	GD_SetGraphColor( gno+72, 255, 91, 55, 21 );

	GD_SetGraphStr( gno+73, bno+2, layer+1, FALSE, " " );
	GD_SetGraphRect( gno+73, 28+40+154+40, 285, 0, 0, 10, 16 );

	for( i=20 ; i < 80 ; i++ ){
		GD_SetGraphBlendParam( gno+i, BLEND_ALPHA );
		GD_SetGraphScaleParam( gno+i, SCALE_HIGHT );
	}
	return 1;
}
void ResetCharParamWindow( int win_no, int win_bno, int gno, int bno, int spno )
{
	int	i;
	for( i=20 ; i < 80 ; i++ ){
		GD_ResetGraph( gno+i );
	}
}

void WLD_SetRightBar18( int win_bno, int gno, int layer, int mouse )
{
	GD_SetGraph( gno+0, win_bno, layer, FALSE );	
	GD_SetGraphRect( gno+0, 700,24-8, 624+0*44, 432, 44, 32 );
	GD_SetGraph( gno+1, win_bno, layer, FALSE );	
	GD_SetGraphRect( gno+1, 700,472+8, 624+0*44, 464, 44, 32 );

	GD_SetGraph( gno+2, win_bno, layer, FALSE );
	GD_SetGraph( gno+3, win_bno, layer, FALSE );
	GD_SetGraph( gno+4, win_bno, layer, FALSE );
	GD_SetGraphSrcRect( gno+2, 640, 496,    16,  6 );
	GD_SetGraphSrcRect( gno+3, 640, 496+6,  16, 20 );
	GD_SetGraphSrcRect( gno+4, 640, 496+26, 16,  6 );
	GD_SetGraphDstRect( gno+2, 756,  48,   16,  6 );
	GD_SetGraphDstRect( gno+3, 756,  48+6, 16,  420 );
	GD_SetGraphDstRect( gno+4, 756, 474,   16,  6 );

	GD_SetGraph( gno+5, win_bno, layer+1, FALSE );
	GD_SetGraph( gno+6, win_bno, layer+1, FALSE );
	GD_SetGraph( gno+7, win_bno, layer+1, FALSE );
	GD_SetGraphSrcRect( gno+5, 624, 496,    16,  6 );
	GD_SetGraphSrcRect( gno+6, 624, 496+6,  16, 20 );
	GD_SetGraphSrcRect( gno+7, 624, 496+26, 16,  6 );

	MUS_SetMouseRect( mouse, 18, 700,  24-8, 44,  32, ON );
	MUS_SetMouseRect( mouse, 19, 700, 472+8, 44,  32, ON );
	MUS_SetMouseRect( mouse, 20, 756,    48, 16, 432, ON );
}
void WLD_SetRightBar18Disp( int gno, int disp )
{
	int	i;
	for( i=gno ; i < gno+8 ; i++ ){
		GD_SetGraphDisp( i, disp );
	}
}
void WLD_ResetRightBar18( int gno )
{
	int	i;
	for( i=gno ; i < gno+8 ; i++ ){
		GD_ResetGraph( i );
	}
}

void WLD_RenewRightBar18( int gno, int page, int page_max, int um, int dm, int click )
{
	int	i,j;
	int	page_max2 = max(18,page_max);

	if( page_max>18 ){
		for( i=gno ; i < gno+8 ; i++ ){
			GD_SetGraphDisp( i, TRUE );
		}

		if(um){
			if(click)	GD_SetGraphSrcRect( gno+0, 624+2*44, 432, 44, 32 ),appData.draw_flag=1;
			else		GD_SetGraphSrcRect( gno+0, 624+1*44, 432, 44, 32 );
		}else	GD_SetGraphSrcRect( gno+0, 624+0*44, 432, 44, 32 );
		if(dm){
			if(click)	GD_SetGraphSrcRect( gno+1, 624+2*44, 464, 44, 32 ),appData.draw_flag=1;
			else		GD_SetGraphSrcRect( gno+1, 624+1*44, 464, 44, 32 );
		}else	GD_SetGraphSrcRect( gno+1, 624+0*44, 464, 44, 32 );

		i = page*(432-12-420*18/page_max2)/(page_max2-18);
		j = 420*18/page_max2;

		GD_SetGraphDstRect( gno+5, 756,  i + 48,   16,  6 );	
		GD_SetGraphDstRect( gno+6, 756,  i + 48+6, 16,  j );
		GD_SetGraphDstRect( gno+7, 756,  i + 48+6+j,   16,  6 );
	}else{
		for( i=gno ; i < gno+8 ; i++ ){
			GD_SetGraphDisp( i, FALSE );
		}
	}

}
int WLD_DragRightBar18( int *page, int *page_max, int select_no )
{
	int	i,j,draw=0;

	if(*page_max>18){
		static int	drag_flag=0, drag_page;
		int			mx,my;

		*page = LIM( *page, 0, max(0,*page_max-18) );

		if(select_no==20){

			i = 48+*page*(420-420*18/(*page_max)) / (*page_max-18);
			j = 420*18 / (*page_max)+12;
			
			MUS_GetMousePos( &mx, &my );

			if(GameKey.click){
				SND_Ok();
				if( i<=my && my<i+j ){
					drag_flag = 1;
					drag_page = *page;
				}else{
					*page = LIM( *page+1, 0, *page_max-18 );
					draw=1;
				}
			}
		}
		if(drag_flag){
			draw=1;
			if( MUS_GetMouseDragMove( MOUSE_LBUTTON, &mx, &my ) ){
				i = drag_page + my/(420.0f/(*page_max));
				*page = LIM( i, 0, *page_max-18 );
			}else{
				drag_flag = 0;
			}
		}
		if(GameKey.pup   ||(select_no==18&&GameKey.click)){
			if(GameKey.pup){
				SND_Select();
			}else{
				SND_Ok();
			}
			*page = LIM( *page-1, 0, max(0,*page_max-18) ); draw=1;
		}
		if(GameKey.pdown ||(select_no==19&&GameKey.click)){
			if(GameKey.pdown){
				SND_Select();
			}else{
				SND_Ok();
			}
			*page = LIM( *page+1, 0, max(0,*page_max-18) ); draw=1;
		}
	}else{
		*page=0;
	}
	return draw;
}


typedef struct{
	int			ver_no;		
	SYSTEMTIME	sys_time;	
	int			rank;		
	int			save_point1;
	char		save_scr[32];
	DWORD		times;		
	int			lv;			
	int			retry;		
	int			clr;		
}SAVE_HEADER;

typedef struct{
	SAVE_HEADER	save_head;

	int				ESC_FlagBuf[ ESC_FLAG_MAX ];	

	JOB_PARAM2		JobParam2[32+1];			
	CHAR_PARAM_SAVE	CharParam[CHAR_PARAM_MAX];	

	PLAYER_PARAM	PlayerParam;		
	ITEM_BOX		ItemBox[ITEM_MAX];	

	short	ShopItemTable[5][256];
	short	ShopItemMax[5];
	short	BGMNo;
}SAVE_DATA;


void SAV_StartReset( void )
{
	int	i;
	GameStartTime = timeGetTime();

	AVG_StartMessage();
	
	ZeroMemory( ESC_FlagBuf, sizeof(int)*ESC_FLAG_MAX );	

	ZeroMemory( JobParam2, sizeof(JOB_PARAM2)*(32+1) );	

	TestSetParam(1);

	
	for(i=9;i<CHAR_PARAM_MAX;i++){
		ZeroMemory( &CharParam[i], sizeof(CHAR_PARAM) );
	}
	
	ZeroMemory( &PlayerParam, sizeof(PLAYER_PARAM) );	
	ZeroMemory( ItemBox, sizeof(ITEM_BOX)*ITEM_MAX );	

	if( ESC_GetGameFlag(20) ){
		ITM_GetItem( 284, 1 );	
	}
	if( ESC_GetGameFlag(21) ){
		ITM_GetItem( 285, 1 );	
	}
	if( ESC_GetGameFlag(22) ){
		ITM_GetItem( 286, 1 );	
	}
	if( ESC_GetGameFlag(23) ){
		ITM_GetItem( 287, 1 );	
	}
	if( ESC_GetGameFlag(24) ){
		ITM_GetItem( 288, 1 );	
	}
	if( ESC_GetGameFlag(25) ){
		ITM_GetItem( 289, 1 );	
	}
	if( ESC_GetGameFlag(26) ){
		ITM_GetItem( 290, 1 );	
	}
	if( ESC_GetGameFlag(27) ){
		ITM_GetItem( 291, 1 );	
	}
	if( ESC_GetGameFlag(28) ){
		ITM_GetItem( 292, 1 );	
	}
	
	ZeroMemory( ShopItemTable, sizeof(short)*5*256 );
	ZeroMemory( ShopItemMax, sizeof(short)*5 );

}


int	SAV_SaveFile( int save_no, int rank, int save_point1, char *save_scr, int clr )
{
	int	i;
	SAVE_DATA	SaveData;

	SaveData.save_head.ver_no  = 1;		
	GetLocalTime( &SaveData.save_head.sys_time );	

	SaveData.save_head.rank  = rank;	
	if(clr){
		SaveData.save_head.save_point1 = SYS_CASTLE;	
	}else{
		SaveData.save_head.save_point1 = save_point1;	
	}
	
	wsprintf( SaveData.save_head.save_scr, save_scr );	
	
	SaveData.save_head.times    = timeGetTime()-GameStartTime;	
	SaveData.save_head.lv       = CharParam[0].lv;				
	SaveData.save_head.retry    = PlayerParam.retry;			

	SaveData.save_head.clr      = ESC_GetFlag(5);		

	CopyMemory( SaveData.ESC_FlagBuf, ESC_FlagBuf, sizeof(int)*ESC_FLAG_MAX );	

	CopyMemory( SaveData.JobParam2, JobParam2, sizeof(JOB_PARAM2)*(32+1) );	

	
	for(i=0;i<CHAR_PARAM_MAX;i++){
		CopyMemory( &SaveData.CharParam[i], &CharParam[i], sizeof(CHAR_PARAM_SAVE) );
	}
	
	CopyMemory( &SaveData.PlayerParam, &PlayerParam, sizeof(PLAYER_PARAM) );	

	CopyMemory( SaveData.ItemBox, ItemBox, sizeof(ITEM_BOX)*ITEM_MAX );	

	
	CopyMemory( SaveData.ShopItemTable, ShopItemTable, sizeof(short)*5*256 );
	CopyMemory( SaveData.ShopItemMax, ShopItemMax, sizeof(short)*5 );

	SaveData.BGMNo = AVG_GetBGMEno();

	char	fname[256];
	wsprintf( fname, "save_%02d.sav", save_no );
	STD_WriteFile( fname, (BYTE*)&SaveData, sizeof(SAVE_DATA) );

	return 1;
}
SAVE_HEADER	*SAV_LoadFileHeader( int save_no )
{
	static SAVE_HEADER	save_head;
	char	fname[256];
	wsprintf( fname, "save_%02d.sav", save_no );
	if( STD_ReadFile( fname, (BYTE*)&save_head, sizeof(SAVE_HEADER) ) ){
	}else{
		return NULL;
	}
	return &save_head;
}

SAVE_HEADER	*SAV_LoadFile( int save_no )
{
	int	i;
	static SAVE_DATA	SaveData;

	char	fname[256];
	wsprintf( fname, "save_%02d.sav", save_no );
	if( STD_ReadFile( fname, (BYTE*)&SaveData, sizeof(SAVE_DATA) ) ){
		SaveData.save_head.ver_no   ;	
		SaveData.save_head.sys_time ;	
		SaveData.save_head.rank   ;	
		SaveData.save_head.save_point1;	

		GameStartTime = timeGetTime()-SaveData.save_head.times;		
		SaveData.save_head.lv       ;		
		SaveData.save_head.retry    ;		
		SaveData.save_head.clr      ;		


		CopyMemory( ESC_FlagBuf, SaveData.ESC_FlagBuf, sizeof(int)*ESC_FLAG_MAX );	

		CopyMemory( JobParam2, SaveData.JobParam2, sizeof(JOB_PARAM2)*(32+1) );	

		
		for(i=0;i<CHAR_PARAM_MAX;i++){
			CopyMemory( &CharParam[i], &SaveData.CharParam[i], sizeof(CHAR_PARAM_SAVE) );
		}
		
		CopyMemory( &PlayerParam, &SaveData.PlayerParam, sizeof(PLAYER_PARAM) );	

		CopyMemory( ItemBox, SaveData.ItemBox, sizeof(ITEM_BOX)*ITEM_MAX );	

		
		CopyMemory( ShopItemTable, SaveData.ShopItemTable, sizeof(short)*5*256 );
		CopyMemory( ShopItemMax,   SaveData.ShopItemMax,   sizeof(short)*5 );

		if(SaveData.save_head.save_point1==SYS_SAVE_POINT || SaveData.save_head.save_point1==SYS_SAVE_POINT2 ){
			AVG_SetBGMLoad( SaveData.BGMNo );
		}
	}else{
		return NULL;
	}

	return (SAVE_HEADER*)&SaveData;
}


int	WLD_SaveLoad( int win_no, int win_bno, int gno, int bno, int spno, int layer, int mouse, int sav_point )
{
	enum{
		SAV_INIT,
		SAV_TERM,

		SAV_FADE_IN,
		SAV_FADE_OUT,

		SAV_SELECT,
		SAV_SAVE,
		SAV_LOAD,

		SAV_SAVE_CHECK,
		SAV_LOAD_CHECK,
	};
	static int	step=SAV_INIT;
	static int	count=0;
	static int	page=0;
	int			ret=0;
	static int	ret_bak=0;
	int			i,j,k;
	char		buf[256];
	static SAVE_HEADER	sh_buf[10];

	static int	select_sav=0;
	int			select_no = MUS_GetMouseNo( -1 );
	static int	select_bak=-1;
	static int	new_data=-1;
	int			moji=24;
	char		*rank_name[5]={"超簡単"," 簡単 "," 普通 ","難しい"," 無理 "};
	char		*point_name[4]={"全体MAP","アヴァロン城","戦闘準備","セーブ画面"};

	switch(step){
		case SAV_INIT:
			MUS_SetMouseLayer( mouse );

			GD_LoadTexture( bno+2, "Tm200.tga", ALPHA_4444|DITHER_OFF );

			SetGameWindow( win_no+1, gno+10, win_bno, layer, FALSE,-1, WINDOW_COLOR_SYS );
			SetGameWindowRect( win_no+1, 800-(24*4+48)-16, 24, 24*4+48,  24*2+48 );
			SetGameWindowText( win_no+1, "セーブ\nロード", bno+1 );

			MUS_SetMouseRect( mouse, 0, 800-(24*4+48)-16+24, 24+24*1, 24*4,  24, ON );
			MUS_SetMouseRect( mouse, 1, 800-(24*4+48)-16+24, 24+24*2, 24*4,  24, ON );

			GD_SetGraphx9( gno+13, win_bno, layer+1, FALSE );
			GD_SetGraphSrcRect( gno+13, 0, 64, 72, 24 );

			GD_SetGraph( gno+14, bno+2, layer+1, FALSE );
			GD_SetGraphDstRect( gno+14, (800-24*moji)/2-12, 22, 288, 32 );
			GD_SetGraph( gno+15, bno+2, layer+1, FALSE );
			GD_SetGraphDstRect( gno+15, 512, 552, 192, 32 );

			GD_SetGraph( gno+16, bno+2, layer+1, FALSE );
			GD_SetGraph( gno+17, bno+2, layer+2, FALSE );
			GD_SetGraph( gno+18, bno+2, layer+2, FALSE );
			GD_SetGraphDstRect( gno+16, 512, 22, 192, 32 );
			GD_SetGraphDstRect( gno+17, 508, 22, 24, 32 );
			GD_SetGraphDstRect( gno+18, 512+192-28, 22, 24, 32 );

			GD_SetGraphx9( gno+19, bno+2, layer+2, FALSE );
			GD_SetGraphSrcRect( gno+19, 216, 96, 72, 72 );

			for(i=0;i<10;i++){
				GD_CreateTextTexture( bno+3+i, "a", 24, moji, 2, 0, 0, 0 );
				GD_SetGraph( gno+20+i, bno+3+i, layer+2, FALSE );
				GD_SetGraphDstMove( gno+20+i, (800-24*moji)/2, 66+50*i );

				GD_SetGraphx9( gno+30+i, bno+2, layer+1, FALSE );
				GD_SetGraphDstRect( gno+30+i, (800-24*moji)/2-24, 42+50*i, 24*moji+48, 72 );

				MUS_SetMouseRect( mouse+1, i, (800-24*moji)/2-12, 42+12+50*i, 24*moji+24, 48, ON );
			}
			MUS_SetMouseRect( mouse+1, 10, 512,552,192,32, ON );

			MUS_SetMouseRect( mouse+1, 11, 508,       22, 24,32, ON );
			MUS_SetMouseRect( mouse+1, 12, 512+192-28,22, 24,32, ON );


			SetGameWindow( win_no+2, gno+42, win_bno, layer+4, FALSE,-1, 1 );
			SetGameWindowRect( win_no+2, 400-(24*3+48), 300-24, 24*3+48, 72  );
			SetGameWindowText( win_no+2, " はい ",  bno+14, 0  );

			SetGameWindow( win_no+3, gno+45, win_bno, layer+4, FALSE,-1, 2 );
			SetGameWindowRect( win_no+3, 400, 300-24, 24*3+48, 72  );
			SetGameWindowText( win_no+3, "いいえ",  bno+15, 0  );

			GD_SetGraphPrimitive( gno+48, 4, layer+3, FALSE );
			GD_SetGraphDstRect( gno+48, 0,0, 800,600 );
			GD_SetGraphColor( gno+48, 128, 0,0,0);

			GD_CreateTextTexture( bno+16, "このセーブデータをロードして良いですか？", 24, 20, 2, 0, 0, 2 );
			GD_SetGraph( gno+49, bno+16, layer+4, FALSE );
			GD_SetGraphDstMove( gno+49, 160, 300-64 );

			MUS_SetMouseRect( mouse+2, 0, 412-(24*3+48), 300-12, 24*3+24, 48, ON );
			MUS_SetMouseRect( mouse+2, 1, 412,           300-12, 24*3+24, 48, ON );

			new_data=-1;
			k=0;
			for(i=0;i<100;i++){
				SAVE_HEADER	*save_head = SAV_LoadFileHeader(i);
				if(save_head){
					j = ((int)(save_head->sys_time.wYear)%100)*12;
					j = (j+(int)(save_head->sys_time.wMonth))*31;
					j = (j+(int)(save_head->sys_time.wDay))*24;
					j = (j+(int)(save_head->sys_time.wHour))*60;
					j = (j+(int)(save_head->sys_time.wMinute))*60;
					j = (j+(int)(save_head->sys_time.wSecond));
					if(j>k){
						k = j;
						new_data = i;
					}
				}
			}
			page=new_data/10;

			appData.draw_flag=1;
			count=0;
			select_bak=-1;
			if(sav_point==SYS_END_SAVE ){
				MUS_SetMouseLayer( mouse+1 );
				step=SAV_SAVE;
			}else if(sav_point==SYS_TITLE ){
				MUS_SetMouseLayer( mouse+1 );
				step=SAV_LOAD;
			}else{
				step=SAV_SELECT;
			}
			break;
		case SAV_TERM:
			for(i=10;i<200;i++)	{	GD_ResetGraph(gno+i);				}
			for(i=1;i<13;i++)	{	GD_ReleaseTexture(bno+i);			}
			for(i=0;i<32;i++)	{	GD_ReleaseSprite(spno+i);			}
			for(i=0;i<4;i++)	{	MUS_ResetMouseRect_Layer(mouse+i);	}
			for(i=1;i<5;i++)	{	ResetGameWindow(win_no+i);			}
			step=SAV_INIT;
			ret=ret_bak;
			break;

		case SAV_FADE_IN:
			break;
		case SAV_FADE_OUT:
			break;

		case SAV_SELECT:
			SetGameWindowDisp( win_no, TRUE );
			SetGameWindowDisp( win_no+1, TRUE );

			if( select_no!=-1 ){
				GD_SetGraphDisp( gno+13, TRUE );
				GD_SetGraphDstRect( gno+13, 800-(24*4+24)-16, 24+24+24*select_no, 24*4,  24 );
				if(select_bak != select_no){
					SND_Select();
				}

				if(GameKey.click){
					if(select_no){
						step = SAV_LOAD;
					}else{
						step = SAV_SAVE;
					}
					GD_SetGraphDisp( gno+13, FALSE );
					SetGameWindowDisp( win_no+1, FALSE );
					SetGameWindowDisp( win_no, FALSE );

					count = 0;
					MUS_SetMouseLayer( mouse+1 );
					SND_Ok();
				}
			}else{
				GD_SetGraphDisp( gno+13, FALSE );
				if(select_bak != select_no){
					SetGameWindowText( win_no, "セーブロード" );
				}
			}

			if(GameKey.cansel){
				count = 0;
				ret_bak=1;
				step = SAV_TERM;
				SND_Cansel();
			}
			select_bak = select_no;
			break;
		case SAV_SAVE_CHECK:
			GD_SetGraphDisp( gno+48, TRUE );
			GD_SetGraphDisp( gno+49, TRUE );

			SetGameWindowDisp( win_no+2, TRUE );
			SetGameWindowDisp( win_no+3, TRUE );
			SetGameWindowBackType( win_no+2, 1 );
			SetGameWindowBackType( win_no+3, 1 );
			if(select_no!=-1){
				if(select_bak != select_no){
					SND_Select();
				}
				SetGameWindowBackType( win_no+2+select_no, 2 );
				if(select_no==0 && GameKey.click){
					SAV_SaveFile( select_sav, ESC_GetFlag(3), sav_point, NextScript, SYS_END_SAVE==sav_point );

					new_data = select_sav;

					SetGameWindowDisp( win_no+2, FALSE );
					SetGameWindowDisp( win_no+3, FALSE );
					GD_SetGraphDisp( gno+48, FALSE );
					GD_SetGraphDisp( gno+49, FALSE );
					count = 0;
					step = SAV_SAVE;
					MUS_SetMouseLayer( mouse+1 );

					SND_Ok();
				}
			}
			if(GameKey.cansel || select_no==1 && GameKey.click){
				SetGameWindowDisp( win_no+2, FALSE );
				SetGameWindowDisp( win_no+3, FALSE );
				GD_SetGraphDisp( gno+48, FALSE );
				GD_SetGraphDisp( gno+49, FALSE );
				count = 0;
				step = SAV_SAVE;
				MUS_SetMouseLayer( mouse+1 );
				SND_Cansel();
			}
			select_bak = select_no;
			break;
		case SAV_LOAD_CHECK:
			GD_SetGraphDisp( gno+48, TRUE );
			GD_SetGraphDisp( gno+49, TRUE );

			SetGameWindowDisp( win_no+2, TRUE );
			SetGameWindowDisp( win_no+3, TRUE );
			SetGameWindowBackType( win_no+2, 1 );
			SetGameWindowBackType( win_no+3, 1 );
			if(select_no!=-1){
				if(select_bak != select_no){
					SND_Select();
				}
				SetGameWindowBackType( win_no+2+select_no, 2 );
				if(select_no==0 && GameKey.click){
					AVG_StopBGM( 30 );
					count = 0;
					step = SAV_TERM;
					
					SAV_LoadFile( select_sav );

					ret_bak=2+sh_buf[select_sav%10].save_point1;
					wsprintf( NextScript, sh_buf[select_sav%10].save_scr );
					SND_Ok();
				}
			}
			if(GameKey.cansel || select_no==1 && GameKey.click){
				SetGameWindowDisp( win_no+2, FALSE );
				SetGameWindowDisp( win_no+3, FALSE );
				GD_SetGraphDisp( gno+48, FALSE );
				GD_SetGraphDisp( gno+49, FALSE );
				count = 0;
				step = SAV_LOAD;
				MUS_SetMouseLayer( mouse+1 );
				SND_Cansel();
			}
			select_bak = select_no;
			break;
		case SAV_SAVE:
			if(GameKey.pup   || (select_no==11&&GameKey.click)){	page = LOOP( page-1, 10 ); count=0;SND_Ok();}
			if(GameKey.pdown || (select_no==12&&GameKey.click)){	page = LOOP( page+1, 10 ); count=0;SND_Ok();}

			if(count==0){
				ZeroMemory( sh_buf, sizeof(SAVE_HEADER)*10 );
				for(i=0;i<10;i++){
					SAVE_HEADER	*save_head = SAV_LoadFileHeader(i+10*page);
					if(save_head){
						sh_buf[i] = *save_head;

						wsprintf( buf, "%2d. %12s Lv%02d %03d:%02d %6s 作戦失敗%02d回",
							i+10*page,
							point_name[save_head->save_point1-SYS_WORLD],
							save_head->lv,
							min(save_head->times/1000/60/60,999),
							(save_head->times/1000/60/60>999)?59:save_head->times/1000/60%60,
							rank_name[max(0,save_head->rank-1)],
							min(99,save_head->retry) );
	
	
	
	
	
	
						if(new_data==i+10*page){
							GD_UpdateTextTexture( bno+3+i, buf, 255,255,0 );
						}else{
							GD_UpdateTextTexture( bno+3+i, buf,255,255,255 );
						}
						GD_SetGraphDisp( gno+20+i, TRUE );
						GD_SetGraphAlpha( gno+20+i, 255 );

						if(save_head->clr)	GD_SetGraphSrcRect( gno+30+i, 144-72*1, 168, 72, 72 );
						else				GD_SetGraphSrcRect( gno+30+i, 144+72*1, 168, 72, 72 );
					}else{
						wsprintf( buf, "%2d.     No Data       ---:-- ", i+10*page );
						GD_UpdateTextTexture( bno+3+i, buf,255,255,255 );
						GD_SetGraphDisp( gno+20+i, TRUE );
						GD_SetGraphAlpha( gno+20+i, 128 );

						GD_SetGraphSrcRect( gno+30+i, 0, 168, 72, 72 );
					}
					GD_SetGraphDisp( gno+30+i, TRUE );
				}
				GD_SetGraphSrcRect( gno+14, 0,32, 288, 32 );

				wsprintf( buf, "%2d/10 ページ", page+1 );
				GD_CreateTextTexture( bno+13, buf, 24, 12, 2, 0, 0, 0 );
				GD_SetGraph( gno+40, bno+13, layer+2, TRUE );
				GD_SetGraphDstMove( gno+40, 528, 26 );
			}
			count++;

			
			GD_SetGraphDisp( gno+14, TRUE );
			GD_SetGraphDisp( gno+15, TRUE );
			GD_SetGraphDisp( gno+16, TRUE );
			GD_SetGraphDisp( gno+17, TRUE );
			GD_SetGraphDisp( gno+18, TRUE );

			GD_SetGraphSrcRect( gno+15, 0,64, 192, 32 );
			GD_SetGraphSrcRect( gno+16, 0,128, 192, 32 );
			GD_SetGraphSrcRect( gno+17, 192, 64, 24, 32 );
			GD_SetGraphSrcRect( gno+18, 192+24, 64, 24, 32 );

			if(select_no==10)GD_SetGraphSrcRect( gno+15, 0,96, 192, 32 );
			if(select_no==11)GD_SetGraphSrcRect( gno+17, 192+24*2, 64, 24, 32 );
			if(select_no==12)GD_SetGraphSrcRect( gno+18, 192+24*3, 64, 24, 32 );

			if( select_no!=-1 && select_no<10){
				if(select_bak != select_no){
					SND_Select();
				}

				GD_SetGraphDisp( gno+19, TRUE );
				GD_SetGraphDstRect( gno+19, (800-24*moji)/2-24, 42+50*select_no, 24*moji+48, 72 );
				if(GameKey.click){
					if(sh_buf[select_no].lv){
						GD_UpdateTextTexture( bno+16, "このセーブデータに上書きして良いですか？" );
						select_sav = select_no+page*10;
						MUS_SetMouseLayer( mouse+2 );
						count = 0;
						step = SAV_SAVE_CHECK;

						SND_Ok();
					}else{
						new_data = select_no+page*10;

						SAV_SaveFile( select_no+page*10, ESC_GetFlag(3), sav_point, NextScript, SYS_END_SAVE==sav_point);
						count = 0;

						ret_bak=1;

						SND_Ok();
					}
				}
			}else{
				GD_SetGraphDisp( gno+19, FALSE );
			}

			if(GameKey.cansel || select_no==10 && GameKey.click){

				for(i=0;i<27;i++){
					GD_SetGraphDisp( gno+14+i, FALSE );
				}
				count = 0;

				if(sav_point==SYS_END_SAVE ){
					ret_bak=1;
					step = SAV_TERM;
				}else{
					step = SAV_SELECT;
				}
				MUS_SetMouseLayer( mouse );

				SND_Cansel();
			}
			select_bak = select_no;
			break;
		case SAV_LOAD:
			if(GameKey.pup   || (select_no==11&&GameKey.click)){	page = LOOP( page-1, 10 ); count=0;SND_Ok();}
			if(GameKey.pdown || (select_no==12&&GameKey.click)){	page = LOOP( page+1, 10 ); count=0;SND_Ok();}

			if(count==0){
				ZeroMemory( sh_buf, sizeof(SAVE_HEADER)*10 );
				for(i=0;i<10;i++){
					SAVE_HEADER	*save_head = SAV_LoadFileHeader(i+10*page);
					if(save_head){
						sh_buf[i] = *save_head;

						wsprintf( buf, "%2d. %12s Lv%02d %03d:%02d %6s 作戦失敗%02d回",
							i+10*page,
							point_name[save_head->save_point1-SYS_WORLD],
							save_head->lv,
							min(save_head->times/1000/60/60,999),
							(save_head->times/1000/60/60>999)?59:save_head->times/1000/60%60,
							rank_name[max(0,save_head->rank-1)],
							min(99,save_head->retry) );
	
	
	
	
	
	
						if(new_data==i+10*page){
							GD_UpdateTextTexture( bno+3+i, buf, 255,255,0 );
						}else{
							GD_UpdateTextTexture( bno+3+i, buf,255,255,255 );
						}
						GD_SetGraphDisp( gno+20+i, TRUE );
						GD_SetGraphAlpha( gno+20+i, 255 );

						if(save_head->clr)	GD_SetGraphSrcRect( gno+30+i, 144-72*1, 168, 72, 72 );
						else				GD_SetGraphSrcRect( gno+30+i, 144+72*0, 168, 72, 72 );
					}else{
						wsprintf( buf, "%2d.     No Data       ---:-- ", i+10*page );
						GD_UpdateTextTexture( bno+3+i, buf,255,255,255 );
						GD_SetGraphDisp( gno+20+i, TRUE );
						GD_SetGraphAlpha( gno+20+i, 128 );

						GD_SetGraphSrcRect( gno+30+i, 0, 168, 72, 72 );
					}
					GD_SetGraphDisp( gno+30+i, TRUE );
				}
				GD_SetGraphSrcRect( gno+14, 0,0, 288, 32 );

				wsprintf( buf, "%2d/10 ページ", page+1 );
				GD_CreateTextTexture( bno+13, buf, 24, 12, 2, 0, 0, 0 );
				GD_SetGraph( gno+40, bno+13, layer+2, TRUE );
				GD_SetGraphDstMove( gno+40, 528, 26 );
			}
			count++;

			
			GD_SetGraphDisp( gno+14, TRUE );
			GD_SetGraphDisp( gno+15, TRUE );
			GD_SetGraphDisp( gno+16, TRUE );
			GD_SetGraphDisp( gno+17, TRUE );
			GD_SetGraphDisp( gno+18, TRUE );

			GD_SetGraphSrcRect( gno+15, 0,64, 192, 32 );
			GD_SetGraphSrcRect( gno+16, 0,128, 192, 32 );
			GD_SetGraphSrcRect( gno+17, 192, 64, 24, 32 );
			GD_SetGraphSrcRect( gno+18, 192+24, 64, 24, 32 );

			if(select_no==10)GD_SetGraphSrcRect( gno+15, 0,96, 192, 32 );
			if(select_no==11)GD_SetGraphSrcRect( gno+17, 192+24*2, 64, 24, 32 );
			if(select_no==12)GD_SetGraphSrcRect( gno+18, 192+24*3, 64, 24, 32 );

			if( select_no!=-1 && select_no<10){
				if(select_bak != select_no){
					SND_Select();
				}

				if(sh_buf[select_no].lv){
					GD_SetGraphDisp( gno+19, TRUE );
					GD_SetGraphDstRect( gno+19, (800-24*moji)/2-24, 42+50*select_no, 24*moji+48, 72 );
				}else{
					GD_SetGraphDisp( gno+19, FALSE );
				}
				if(GameKey.click){
					if(sh_buf[select_no].lv){
						GD_UpdateTextTexture( bno+16, "このセーブデータをロードして良いですか？" );
						select_sav = select_no+10*page;
						MUS_SetMouseLayer( mouse+2 );

						count = 0;
						step = SAV_LOAD_CHECK;
						SND_Ok();
					}
				}
			}else{
				GD_SetGraphDisp( gno+19, FALSE );
			}

			if(GameKey.cansel || select_no==10 && GameKey.click){
				for(i=0;i<27;i++){
					GD_SetGraphDisp( gno+14+i, FALSE );
				}

				count = 0;
				if(sav_point==SYS_TITLE ){
					ret_bak=1;
					step = SAV_TERM;
				}else{
					step = SAV_SELECT;
				}
				MUS_SetMouseLayer( mouse );
				SND_Cansel();
			}
			select_bak = select_no;
			break;
	}

	return ret;
}




short	ShopItemTable[5][256];
short	ShopItemMax[5];

void WLD_SetItemShopMenu( int kajiya )
{
	int	i,j, rank;
	int	equip_type;


	i = ESC_GetFlag(4);
	ESC_SetFlag(4, max(1,i) );

	ShopItemMax[0]=0;
	ShopItemMax[1]=0;
	ShopItemMax[2]=0;
	ShopItemMax[3]=0;
	ShopItemMax[4]=0;

	if( ESC_GetFlag(68) ){}
	if( ESC_GetFlag(69) ){	
		rank = ESC_GetFlag(4)+1;
		if(rank>=12){
			rank=49;
		}
	}else{
		rank = ESC_GetFlag(4);
	}

	for(i=0;i<ITEM_MAX;i++){
		if( ItemParam[i].shop>rank ) continue;
		if( ItemParam[i].shop==0 ) continue;

		if( kajiya ){
			switch(ItemParam[i].type){
				case ITEM_BUKI_SWORD:	
				case ITEM_BUKI_SPEAR:	
				case ITEM_BUKI_HAMMER:	
				case ITEM_YOROI_ARMOUR1:	
				case ITEM_YOROI_ARMOUR2:	
				case ITEM_YOROI_CHAIN:	
				case ITEM_ACCE_GAUNTLET:	
				case ITEM_ACCE_HELMET:	
					break;
				default:
					continue;
					break;
			}
		}else{
			switch(ItemParam[i].type){
				case ITEM_BUKI_BOW:		
				case ITEM_BUKI_WAND:		
				case ITEM_YOROI_CLOTHE:	
				case ITEM_ACCE_RING:		
				case ITEM_ACCE_BRACELET:	
				case ITEM_ACCE_BOOTS:	
				case ITEM_ACCE_NECKLACE:	
				case ITEM_ACCE_CAP:		
				case ITEM_TOOL_POTION1:	
				case ITEM_TOOL_POTION2:	
				case ITEM_TOOL_POTION3:	
				case ITEM_TOOL_POTION4:	
					break;
				case ITEM_TOOL_EVENT:	
					if(ESC_GetFlag(63)!=2)continue;
					if( ITM_GetItemAmount(i) ) continue;
					break;

				default:
					continue;
					break;
			}
		}

		equip_type = ITM_GetEquipType( i );
		j = ShopItemMax[equip_type];

		ShopItemTable[equip_type][j] = i;

		ShopItemMax[equip_type]++;




	}





}


int	WLD_ItemShop( int win_no, int win_bno, int gno, int bno, int spno, int layer, int mouse, int kajiya )
{
	enum{
		ITS_INIT,
		ITS_TERM,

		ITS_FADE_IN,
		ITS_FADE_OUT,

		ITS_BUY_CHK,
		ITS_SELL_CHK,

		ITS_BUY,	
		ITS_SELL,	

		ITS_MIAN,
	};
	static int	step=ITS_INIT;
	static int	count=0;

	int			draw=0;
	int			ret=0;
	int			i,j,k,ino;
	char		buf[512];
	char		buf2[256];

	static int	sell_no[18];

	static int	pt_no[32];
	static int	page=0, page_max;

	static int	eq_type=0;
	static int	buy_count=1;
	int			buy_max=1;

	static int	iselect=-1, ipage=0, ipage_max;
	static int	sselect=-1, spage=0, spage_max;

	static int	select_bak=-1;
	int			select_no = MUS_GetMouseNo( -1 );


	switch(step){
		case ITS_INIT:
			SetGameWindow( 1, WLD_GRP_WINDW, WLD_BMP_SYSTEM, WLD_LAY_SYSTEM, TRUE, 6, WINDOW_COLOR_SYS );
			SetGameWindowRect( 1, 4, 600-(24*2+48), 800-8, 24*2+48  );
			if(kajiya){
				if( ESC_GetFlag(64)==2 ){	
					SetGameWindowText( win_no, "武器・防具一式『金物屋』" );
				}else{
					SetGameWindowText( win_no, "武器屋です。" );
				}
			}else{
				if( ESC_GetFlag(63)==2 ){	
					SetGameWindowText( win_no, "道具屋『グッド・ピープルズ』" );
				}else{
					SetGameWindowText( win_no, "道具屋です。" );
				}
			}

			WLD_SetItemShopMenu( kajiya );
			MUS_SetMouseLayer( mouse );

			GD_LoadTexture( bno+2, "digit002.tga", ALPHA_4444 );
			for(j=0,i=0;i<32;i++){
				if(CharParam[i].flag && CharParam[i].unit==0){
					pt_no[j++]=i;
				}
			}
			eq_type=0;
			page=0;
			page_max=j;

			SetGameWindow( win_no+2, gno+80, win_bno, layer, TRUE,-1, WINDOW_COLOR_SYS );
			SetGameWindowRect( win_no+2, 124, 448, 24*8+48,  16*1+48 );

			GD_SetGraph( gno+89, win_bno, layer+1, TRUE );
			GD_SetGraphRect( gno+89, 124+24+24*6, 448+24, 656, 496+16, 48, 16 );
			GD_SetGraphBlendParam( gno+89, BLEND_ALPHA );

			GD_SetGraphDigit( gno+90, bno+2, layer+1, TRUE, PlayerParam.money, 8 );
			GD_SetGraphRect( gno+90, 124+85, 448+24, 0, 16, 10, 16 );
			GD_SetGraphBlendParam( gno+90, BLEND_ALPHA );
			GD_SetGraphColor( gno+90, 255, 91,55,21 );

			GD_SetGraph( gno+91, win_bno, layer+1, TRUE );
			GD_SetGraphRect( gno+91, 124+24, 448+24, 656, 496, 48, 16 );
			GD_SetGraphBlendParam( gno+91, BLEND_ALPHA );

			SetGameWindow( win_no+3, gno+92, win_bno, layer, FALSE,-1,WINDOW_COLOR_SYS );
			SetGameWindowRect( win_no+3, 800-(24*4+48)-16, 24, 24*4+48,  24*2+48 );
			SetGameWindowText( win_no+3, "買う\n売る", bno+10 );

			GD_SetGraphx9( gno+95, win_bno, layer+1, FALSE );	
			GD_SetGraphSrcRect( gno+95, 0, 64, 72, 24 );

			MUS_SetMouseRect( mouse, 0, 800-(24*4+48)+24-16, 24+24*1, 24*4, 24, ON );
			MUS_SetMouseRect( mouse, 1, 800-(24*4+48)+24-16, 24+24*2, 24*4, 24, ON );

			SetGameWindow( win_no+4, gno+103, win_bno, layer, FALSE,2, WINDOW_COLOR_SYS);
			SetGameWindowRect( win_no+4, 800-440-4, 24, 440, 480 );
			SetGameWindowText( win_no+4, "アイテム", bno+11, 24, 0, 8 );


			if(kajiya)	j=4;
			else		j=5;
			for( i=0 ; i < j ; i++ ){
				GD_SetGraph( gno+106+i, win_bno, layer+2, FALSE );
				GD_SetGraphRect( gno+106+i, 800-440-4+128+24*i, 24, 768+24*i, 416, 22, 22 );
				GD_SetGraphCenter( gno+106+i, 800-440-4+128+24*i+11, 24+11 );

				MUS_SetMouseRect( mouse+1, 21+i, 800-440-4+128+24*i, 24, 22, 22, ON );
			}

			for( i=0 ; i < 18 ; i++ ){

				GD_SetGraph( gno+114+i, win_bno, layer+2, FALSE );
				GD_SetGraphRect( gno+114+i, 800-440-4+24, 24+24+24*i, 768, 344, 22, 22 );
				GD_SetGraphScaleParam( gno+114+i, SCALE_HIGHT );

				GD_SetGraphStr( gno+132+i, bno+2, layer+2, FALSE, " " );
				GD_SetGraphRect( gno+132+i, 800-440-4+24*10, 24+24+24*i+6, 0, 0, 10, 16 );
				GD_SetGraphScaleParam( gno+132+i, SCALE_HIGHT );
				GD_SetGraphColor2( gno+132+i, 91,55,21 );

				GD_SetGraph( gno+150+i, win_bno, layer+2, FALSE );
				GD_SetGraphRect( gno+150+i, 800-440-4+24*10+70, 24+24+24*i+6, 656, 496+16, 48, 16 );
				GD_SetGraphScaleParam( gno+150+i, SCALE_HIGHT );
				GD_SetGraphColor2( gno+150+i, 91,55,21 );

				MUS_SetMouseRect( mouse+1, i, 800-440-4+24, 24+24+24*i, 24*14+42, 24, ON );
			}
			WLD_SetRightBar18( win_bno, gno+168, layer+1, mouse+1 );

			MUS_SetMouseRect( mouse+1, 30,   0, 72, 32, 48, ON );
			MUS_SetMouseRect( mouse+1, 31, 320, 72, 32, 48, ON );

			for( i=0 ; i < 4 ; i++ ){
				MUS_SetMouseRect( mouse+1, 40+i, 60, 302+24*i, 24*9, 24, ON );
			}

			for( i=0 ; i < ITEM_MAX ; i++ ){
				if(ItemBox[i].ino==0)	break;
			}
			ipage=0;
			ipage_max=i;



			SetGameWindow( win_no+5, gno+180, win_bno, layer+3, FALSE,-1, WINDOW_COLOR_SYS );
			SetGameWindowRect( win_no+5, 400, 360, 24*11+48, 168 );
			SetGameWindowText( win_no+5, "　あいてむ名１２３\n\n\n\n装備購入  購入  中止", bno+12, 8, 0 );

			GD_SetGraph( gno+183, win_bno, layer+4, FALSE );
			GD_SetGraphRect( gno+183, 408+24, 360+24, 768, 344, 22, 22 );
			GD_SetGraphScaleParam( gno+183, SCALE_HIGHT );

			GD_SetGraphx9( gno+184, win_bno, layer+4, FALSE );
			GD_SetGraphDstRect( gno+184, 430, 432,    38,  18 );	GD_SetGraphSrcRect( gno+184, 704, 496,    12,  12 );

			GD_SetGraphx9( gno+185, win_bno, layer+4, FALSE );
			GD_SetGraphDstRect( gno+185, 473, 432,    38,  18 );	GD_SetGraphSrcRect( gno+185, 704, 496,    12,  12 );

			GD_SetGraphx9( gno+186, win_bno, layer+4, FALSE );
			GD_SetGraphDstRect( gno+186, 430, 452,    38,  18 );	GD_SetGraphSrcRect( gno+186, 704, 496,    12,  12 );

			GD_SetGraphx9( gno+187, win_bno, layer+4, FALSE );
			GD_SetGraphDstRect( gno+187, 473, 452,    38,  18 );	GD_SetGraphSrcRect( gno+187, 704, 496,    12,  12 );


			GD_SetGraphx9( gno+188, win_bno, layer+4, FALSE );
			GD_SetGraphDstRect( gno+188, 430, 480,    102,  24 );	GD_SetGraphSrcRect( gno+188, 704, 496,    12,  12 );

			GD_SetGraphx9( gno+189, win_bno, layer+4, FALSE );
			GD_SetGraphDstRect( gno+189, 541, 480,    66,  24 );	GD_SetGraphSrcRect( gno+189, 704, 496,    12,  12 );

			GD_SetGraphx9( gno+190, win_bno, layer+4, FALSE );
			GD_SetGraphDstRect( gno+190, 616, 480,    66,  24 );	GD_SetGraphSrcRect( gno+190, 704, 496,    12,  12 );

			GD_SetGraphStr( gno+191, bno+2, layer+4, FALSE, "    1234567 ELEM" );
			GD_SetGraphRect( gno+191, 400+24+88, 360+52, 0, 0, 10, 16 );
			GD_SetGraphScaleParam( gno+191, SCALE_HIGHT );
			GD_SetGraphColor2( gno+191, 91,55,21 );

			GD_SetGraphStr( gno+192, bno+2, layer+4, FALSE, " *       01/99" );
			GD_SetGraphRect( gno+192, 400+24+88, 360+52+20, 0, 0, 10, 16 );
			GD_SetGraphScaleParam( gno+192, SCALE_HIGHT );
			GD_SetGraphColor2( gno+192, 91,55,21 );

			GD_SetGraphStr( gno+193, bno+2, layer+4, FALSE, " = 12345678 ELEM" );
			GD_SetGraphRect( gno+193, 400+24+88, 360+52+40, 0, 0, 10, 16 );
			GD_SetGraphBlendParam( gno+193, BLEND_ALPHA );
			GD_SetGraphScaleParam( gno+193, SCALE_HIGHT );
			GD_SetGraphColor2( gno+193, 91,55,21 );

			GD_SetGraphStr( gno+194, bno+2, layer+5, FALSE, "+1" );
			GD_SetGraphRect( gno+194, 437, 433, 0, 0, 10, 16 );
			GD_SetGraphColor2( gno+194, 91,55,21 );

			GD_SetGraphStr( gno+195, bno+2, layer+5, FALSE, "-1" );
			GD_SetGraphRect( gno+195, 480, 433, 0, 0, 10, 16 );
			GD_SetGraphColor2( gno+195, 91,55,21 );

			GD_SetGraphStr( gno+196, bno+2, layer+5, FALSE, "+10" );
			GD_SetGraphRect( gno+196, 432, 453, 0, 0, 10, 16 );
			GD_SetGraphColor2( gno+196, 91,55,21 );

			GD_SetGraphStr( gno+197, bno+2, layer+5, FALSE, "-10" );
			GD_SetGraphRect( gno+197, 475, 453, 0, 0, 10, 16 );
			GD_SetGraphColor2( gno+197, 91,55,21 );



			MUS_SetMouseRect( mouse+2, 0, 430, 432,  38,  18, ON );
			MUS_SetMouseRect( mouse+2, 1, 473, 432,  38,  18, ON );
			MUS_SetMouseRect( mouse+2, 2, 430, 452,  38,  18, ON );
			MUS_SetMouseRect( mouse+2, 3, 473, 452,  38,  18, ON );

			MUS_SetMouseRect( mouse+2, 4, 430, 480,   102,  24, ON );
			MUS_SetMouseRect( mouse+2, 5, 541, 480,    66,  24, ON );
			MUS_SetMouseRect( mouse+2, 6, 616, 480,    66,  24, ON );

			count=0;
			select_bak=-1;
			step=ITS_FADE_IN;
			break;
		case ITS_TERM:
WLD_ResetRightBar18( gno+168 );
			for(i=20;i<200;i++)	{	GD_ResetGraph(gno+i);				}
			for(i=1;i<12;i++)	{	GD_ReleaseTexture(bno+i);			}
			for(i=0;i<32;i++)	{	GD_ReleaseSprite(spno+i);			}
			for(i=0;i<4;i++)	{	MUS_ResetMouseRect_Layer(mouse+i);	}
			for(i=1;i<6;i++)	{	ResetGameWindow(win_no+i);			}
			step=ITS_INIT;
			ret=1;
			break;

		case ITS_FADE_IN:
			count++;

			SetGameWindowDisp( win_no+3, TRUE );
			SetGameWindowDisp( win_no+2, TRUE );
			SetGameWindowDisp( win_no, TRUE );
			SetGameWindowAlpha( win_no,   255*count/15 );
			SetGameWindowAlpha( win_no+3, 255*count/15 );
			SetGameWindowAlpha( win_no+2, 255*count/15 );
			GD_SetGraphAlpha( gno+89, 255*count/15 );
			GD_SetGraphAlpha( gno+90, 255*count/15 );
			GD_SetGraphAlpha( gno+91, 255*count/15 );

			if( count>=15 ){
				step=ITS_MIAN;
			}
			break;
		case ITS_FADE_OUT:
			count++;

			SetGameWindowAlpha( win_no,   255-255*count/15 );
			SetGameWindowAlpha( win_no+3, 255-255*count/15 );
			SetGameWindowAlpha( win_no+2, 255-255*count/15 );

			GD_SetGraphAlpha( gno+89, 255-255*count/15 );
			GD_SetGraphAlpha( gno+90, 255-255*count/15 );
			GD_SetGraphAlpha( gno+91, 255-255*count/15 );

			GD_ResetGraph( gno+95 );
			
			if( count>=15 ){
				SetGameWindowColor( win_no+3, 255, 0, 0, 0 );
				SetGameWindowColor( win_no+4, 255, 0, 0, 0 );

				step=ITS_TERM;
			}
			ret=2;
			break;

		case ITS_BUY_CHK:	
			ino = ShopItemTable[eq_type][sselect+spage];
			buy_max = 99-ITM_GetItemAmount( ino );

			if(ItemParam[ino].buy){
				buy_max = min( buy_max, PlayerParam.money/ItemParam[ino].buy );
			}
			
			wsprintf( buf, "   %8d ELEM", ItemParam[ino].buy );
			GD_SetGraphStrChange( gno+191, buf );
			wsprintf( buf, " *       %02d/%02d", buy_count, buy_max );
			GD_SetGraphStrChange( gno+192, buf );
			wsprintf( buf, "   %8d ELEM", ItemParam[ino].buy*buy_count );
			GD_SetGraphStrChange( gno+193, buf );


			if(count==0){
				wsprintf( buf, "　%s\n\n\n\n装備購入  購入  中止", ItemParam[ino].name );
				SetGameWindowText( win_no+5, buf );
				SetGameWindowDisp( win_no+5, TRUE );

				GD_SetGraphSrcRect( gno+183, 768+(ItemParam[ino].type%10)*24, 344+(ItemParam[ino].type/10)*24, 22, 22 );
				for( i=183 ; i <=197  ; i++ ){
					GD_SetGraphDisp( gno+i, TRUE );
				}
			}
			count++;

			for( i=0 ; i <7 ; i++ ){
				GD_SetGraphSrcRect( gno+184+i, 704, 496,    12,  12 );
			}
			if( select_no!=-1 ){
				GD_SetGraphSrcRect( gno+184+select_no, 704+12, 496,    12,  12 );

				if(select_bak != select_no ){
					SND_Select();
				}

				
				if(GameKey.click){
					appData.draw_flag=1;
					GD_SetGraphSrcRect( gno+184+select_no, 704+12, 496+12,    12,  12 );

					switch( select_no ){
					case 0:	buy_count = LIM(buy_count+ 1,1,buy_max);	SND_Ok();	break;
					case 1:	buy_count = LIM(buy_count- 1,1,buy_max);	SND_Ok();	break;
					case 2:	buy_count = LIM(buy_count+10,1,buy_max);	SND_Ok();	break;
					case 3:	buy_count = LIM(buy_count-10,1,buy_max);	SND_Ok();	break;

					case 4:
						if( CharEquipCheck( pt_no[page], ino, eq_type ) ){
							PlayerParam.money = PlayerParam.money-buy_count*ItemParam[ino].buy;
							ITM_GetItem( ino, buy_count );
							SetCharEquip( pt_no[page], eq_type, ino );
							step=ITS_BUY;
							SND_Ok();
						}else{
							SND_Ng();
						}
						break;
					case 5:
						PlayerParam.money = PlayerParam.money-buy_count*ItemParam[ino].buy;
						ITM_GetItem( ino, buy_count );
						step=ITS_BUY;
						SND_Ok();
						break;
					case 6:
						step=ITS_BUY;
						break;
					}
				}
			}
			if(GameKey.cansel){
				step=ITS_BUY;
			}
			if(step==ITS_BUY){
				MUS_SetMouseLayer( mouse+1 );
				count=0;

				SetGameWindowDisp( win_no+5, FALSE );
				for( i=183 ; i <=197  ; i++ ){
					GD_SetGraphDisp( gno+i, FALSE );
				}
				SND_Cansel();
			}
			select_bak = select_no;
			break;
		case ITS_SELL_CHK:	
			ino = ItemBox[iselect].ino;
			buy_max = ITM_GetItemAmount( ino );
			
			wsprintf( buf, "   %8d ELEM", ItemParam[ino].buy/2 );
			GD_SetGraphStrChange( gno+191, buf );
			wsprintf( buf, " *       %02d/%02d", buy_count, buy_max );
			GD_SetGraphStrChange( gno+192, buf );
			wsprintf( buf, "   %8d ELEM", ItemParam[ino].buy/2*buy_count );
			GD_SetGraphStrChange( gno+193, buf );


			if(count==0){
				wsprintf( buf, "　%s\n\n\n\n　　　　  売却  中止", ItemParam[ino].name );
				SetGameWindowText( win_no+5, buf );
				SetGameWindowDisp( win_no+5, TRUE );

				GD_SetGraphSrcRect( gno+183, 768+(ItemParam[ino].type%10)*24, 344+(ItemParam[ino].type/10)*24, 22, 22 );
				for( i=183 ; i <=197  ; i++ ){
					GD_SetGraphDisp( gno+i, TRUE );
				}
				GD_SetGraphDisp( gno+188, FALSE );
			}
			count++;

			for( i=0 ; i <7 ; i++ ){
				GD_SetGraphSrcRect( gno+184+i, 704, 496,    12,  12 );
			}
			if( select_no!=-1 ){
				if(select_bak != select_no && select_no!=4 ){
					SND_Select();
				}

				GD_SetGraphSrcRect( gno+184+select_no, 704+12, 496,    12,  12 );
				if(GameKey.click){
					appData.draw_flag=1;
					GD_SetGraphSrcRect( gno+184+select_no, 704+12, 496+12,    12,  12 );
					switch( select_no ){
					case 0:	buy_count = LIM(buy_count+ 1,1,buy_max);	SND_Ok();	break;
					case 1:	buy_count = LIM(buy_count- 1,1,buy_max);	SND_Ok();	break;
					case 2:	buy_count = LIM(buy_count+10,1,buy_max);	SND_Ok();	break;
					case 3:	buy_count = LIM(buy_count-10,1,buy_max);	SND_Ok();	break;

					case 5:
						PlayerParam.money = PlayerParam.money+ItemParam[ino].buy/2*buy_count;
						ITM_LostItem( ino, buy_count );
						count=0;
						step=ITS_SELL;
						SND_Ok();
						break;
					case 6:
						step=ITS_SELL;
						break;
					}
				}
			}
			if(GameKey.cansel){
				step=ITS_SELL;
			}

			if(step==ITS_SELL){
				count=0;
				MUS_SetMouseLayer( mouse+1 );

				SetGameWindowDisp( win_no+5, FALSE );
				for( i=183 ; i <=197  ; i++ ){
					GD_SetGraphDisp( gno+i, FALSE );
				}
				SND_Cansel();
			}
			select_bak = select_no;
			break;

		case ITS_BUY:	
			GD_SetGraphDigitNo( gno+90, PlayerParam.money );
			
			draw = WLD_DragRightBar18( &spage, &spage_max, select_no );

			if(GameKey.click && 21<=select_no && select_no<=25 ){
				draw=1;
				eq_type=select_no-21;
				spage=0;
				SND_Ok();
			}

			if(select_no==30&&GameKey.click){	page = LOOP( page-1, page_max ); draw=1;SND_Ok();}
			if(select_no==31&&GameKey.click){	page = LOOP( page+1, page_max ); draw=1;SND_Ok();}

			spage_max = ShopItemMax[eq_type];


			if(GameKey.pup || GameKey.pdown || count==0 || draw ){
				buf[0] = '\0';
				j=0;
				for( i=0 ; i < min(18,spage_max) ; i++ ){
					ino = ShopItemTable[eq_type][i+spage];

					if( ino ){
						if(ItemParam[ino].buy<=PlayerParam.money){
							_mbscat( (BYTE*)buf, (BYTE*)ItemParam[ino].name );
							_mbscat( (BYTE*)buf, (BYTE*)"\n" );

							GD_SetGraphColor2( gno+132+i, 91,55,21 );
						}else{
							_mbscat( (BYTE*)buf, (BYTE*)"<C2:" );
							_mbscat( (BYTE*)buf, (BYTE*)ItemParam[ino].name );
							_mbscat( (BYTE*)buf, (BYTE*)">\n" );

							GD_SetGraphColor2( gno+132+i, 128,128,128 );
						}

						int	type = ItemParam[ ino ].type;
						GD_SetGraphSrcRect( gno+114+i, 768+(type%10)*24, 344+(type/10)*24, 22, 22 );
						GD_SetGraphDisp( gno+114+i, TRUE );

						wsprintf( buf2, "%7d    %2d/%-2d", ItemParam[ino].buy, ITM_GetItemEquipAmount(ino), ITM_GetItemAmount(ino) );

						GD_SetGraphStrChange( gno+132+i, buf2 );
						GD_SetGraphDisp( gno+132+i, TRUE );

						GD_SetGraphDisp( gno+150+i, TRUE );
					}else{
						GD_SetGraphDisp( gno+114+i, FALSE );
						GD_SetGraphDisp( gno+132+i, FALSE );
						GD_SetGraphDisp( gno+150+i, FALSE );
					}
				}
				SetGameWindowText( win_no+4, buf );
				for( ; i < 18 ; i++ ){
					GD_SetGraphDisp( gno+114+i, FALSE );
					GD_SetGraphDisp( gno+132+i, FALSE );
					GD_SetGraphDisp( gno+150+i, FALSE );
				}

			}
			for( i=0 ; i < 5 ; i++ ){
				GD_SetGraphDisp( gno+106+i, TRUE );
				GD_SetGraphZoom( gno+106+i, 1.0f );
				GD_SetGraphLayer( gno+106+i, layer+2 );
				GD_SetGraphColor2( gno+106+i, 128,128,128 );
			}
			GD_SetGraphColor2( gno+106+eq_type, 128,128,192 );
			
			if(21<=select_no && select_no<=25 ){
				GD_SetGraphZoom( gno+106+select_no-21, 2.0f );
				GD_SetGraphLayer( gno+106+select_no-21, layer+3 );
			}

			SetGameWindowDisp( win_no+4, TRUE );


WLD_RenewRightBar18( gno+168, spage, spage_max, 18==select_no, 19==select_no, GameKey.click );
			if(count==0||draw)
				SetCharParamWindow( win_no, win_bno, gno, bno, spno, layer, pt_no[page], TRUE );
			RenweCharParamWindow( gno, 30==select_no, 31==select_no, GameKey.click );

			count++;

			if(40<=select_no && select_no<=43 ){
				if(select_bak != select_no+spage ){
					SND_Select();

				}

				GD_SetGraphDisp( gno+95, TRUE );
				GD_SetGraphDstRect( gno+95, 60, 302+24*(select_no-40), 24*10, 24 );
				if(GameKey.click){
					eq_type=select_no-40;
					count = 0;
					spage=0;
					SND_Ok();
				}
			}else if( select_no!=-1 && select_no<min(18,spage_max) ){
				GD_SetGraphDisp( gno+95, TRUE );
				GD_SetGraphDstRect( gno+95, 800-440-4+24, 24+24+24*select_no, 24*15+48-32,  24 );

				ino = ShopItemTable[eq_type][select_no+spage];

				if(select_bak != select_no+spage ){
					SND_Select();
					SetGameWindowText( win_no, ItemParam[ino].help );
				}

				if( CharEquipCheck( pt_no[page], ino, eq_type ) ){
					SetCharParamWindowEquip( win_no, gno, bno, spno, layer, pt_no[page], eq_type, ino );
				}

				if(GameKey.click){
					if(ItemParam[ino].buy<=PlayerParam.money){
						sselect = select_no;
						buy_count = 1;
						count = 0;
						step=ITS_BUY_CHK;
						MUS_SetMouseLayer( mouse+2 );

						SND_Ok();
					}else{
						SND_Ng();
					}
				}
			}else{
				if(select_bak != select_no+spage ){
					if(select_no!=-1 && select_no!=20){
						if( select_no>=18 || select_no<spage_max){
							SND_Select();
						}
					}
					SetGameWindowText( win_no,"買いたいアイテムを選んで下さい。※右上のアイコンクリックで、アイテムの種類が切り替わります。" );
				}
				GD_SetGraphDisp( gno+95, FALSE );
			}

			if(GameKey.cansel){
				for( i=0; i < 18 ; i++ ){
					GD_SetGraphDisp( gno+114+i, FALSE );
					GD_SetGraphDisp( gno+132+i, FALSE );
					GD_SetGraphDisp( gno+150+i, FALSE );
				}
				for( i=0 ; i < 5 ; i++ ){
					GD_SetGraphDisp( gno+106+i, FALSE );
				}

				SetGameWindowDisp( win_no+4, FALSE );

				WLD_SetRightBar18Disp( gno+168, FALSE );
				ResetCharParamWindow( win_no, win_bno, gno, bno, spno );
				GD_SetGraphDisp( gno+95, FALSE );

				
				count = 0;
				MUS_SetMouseLayer( mouse );
				step = ITS_MIAN;

				SND_Cansel();
			}
			select_bak = select_no+spage;
			break;

		case ITS_SELL:	
			GD_SetGraphDigitNo( gno+90, PlayerParam.money );
			
			draw = WLD_DragRightBar18( &ipage, &ipage_max, select_no );

			if(GameKey.click && 21<=select_no && select_no<=25 ){
				draw=1;
				eq_type=select_no-21;
				ipage=0;
				SND_Ok();
			}

			if(GameKey.pup || GameKey.pdown || count==0 || draw ){
				j=0;
				for( i=0 ; i < ITEM_MAX ; i++ ){
					ino = ItemBox[i].ino;
					if( ITM_GetEquipType( ino )==eq_type ){
						if( ino ){
							j++;
						}else{
							break;
						}
					}
				}
				ipage_max = j;
				ipage = min( max(ipage_max-18,0), ipage );

				buf[0] = '\0';
				j=0;
				k=0;
				for( i=0 ; i < ITEM_MAX ; i++ ){
					ino = ItemBox[i].ino;
					if( ITM_GetEquipType( ino )==eq_type ){
						if( ino ){
							if( ipage+18>j && j>=ipage ){
								sell_no[k] = i;

								if( NotSellCheck(ino) ){
									_mbscat( (BYTE*)buf, (BYTE*)ItemParam[ino].name );
									_mbscat( (BYTE*)buf, (BYTE*)"\n" );
									GD_SetGraphColor2( gno+132+k, 91,55,21 );
								}else{
									_mbscat( (BYTE*)buf, (BYTE*)"<C2:" );
									_mbscat( (BYTE*)buf, (BYTE*)ItemParam[ino].name );
									_mbscat( (BYTE*)buf, (BYTE*)">\n" );
									GD_SetGraphColor2( gno+132+k, 128,128,128 );
								}

								int	type = ItemParam[ ino ].type;
								GD_SetGraphSrcRect( gno+114+k, 768+(type%10)*24, 344+(type/10)*24, 22, 22 );
								GD_SetGraphDisp( gno+114+k, TRUE );

								wsprintf( buf2, "%7d    %2d/%-2d", ItemParam[ino].buy/2, ITM_GetItemEquipAmount(ino), ITM_GetItemAmount(ino) );
								GD_SetGraphStrChange( gno+132+k, buf2 );
								GD_SetGraphDisp( gno+132+k, TRUE );

								GD_SetGraphDisp( gno+150+k, TRUE );
								k++;
							}
							j++;
						}else{
							break;
						}
					}
				}
				ipage_max = j;
				for( ; k < 18 ; k++ ){
					GD_SetGraphDisp( gno+114+k, FALSE );
					GD_SetGraphDisp( gno+132+k, FALSE );
					GD_SetGraphDisp( gno+150+k, FALSE );
				}
				SetGameWindowText( win_no+4, buf );
			}
			SetGameWindowDisp( win_no+4, TRUE );

			for( i=0 ; i < 5 ; i++ ){
				GD_SetGraphDisp( gno+106+i, TRUE );
				GD_SetGraphZoom( gno+106+i, 1.0f );
				GD_SetGraphLayer( gno+106+i, layer+2 );
				GD_SetGraphColor2( gno+106+i, 128,128,128 );
			}
			GD_SetGraphColor2( gno+106+eq_type, 128,128,192 );
			if(21<=select_no && select_no<=25 ){
				GD_SetGraphZoom( gno+106+select_no-21, 2.0f );
				GD_SetGraphLayer( gno+106+select_no-21, layer+3 );
			}

			SetGameWindowDisp( win_no+4, TRUE );

WLD_RenewRightBar18( gno+168, ipage, ipage_max, 18==select_no, 19==select_no, GameKey.click );

			count++;

			if( select_no!=-1 && select_no<min(18,ipage_max) ){
				ino = ItemBox[ sell_no[select_no] ].ino;
				GD_SetGraphDisp( gno+95, TRUE );
				GD_SetGraphDstRect( gno+95, 800-440-4+24, 24+24+24*select_no, 24*15+48-32,  24 );

				if(select_bak != select_no+ipage ){
					SND_Select();
					SetGameWindowText( win_no, ItemParam[ino].help );
				}

				if(GameKey.click){
					if( NotSellCheck( ino ) && ino ){
						iselect = sell_no[select_no];

						buy_count = 1;
						count = 0;
						step=ITS_SELL_CHK;
						MUS_SetMouseLayer( mouse+2 );

						SND_Ok();
					}else{
						SND_Ng();
					}
				}
			}else{
				GD_SetGraphDisp( gno+95, FALSE );
				if(select_bak != select_no+ipage ){
					if(select_no!=-1 && select_no!=20 && select_no<30 ){
						if( select_no>=18 || select_no<ipage_max){
							SND_Select();
						}
					}
					SetGameWindowText( win_no, "売りたいアイテムを選んでください" );
				}
			}

			if(GameKey.cansel){
				for( i=0; i < 18 ; i++ ){
					GD_SetGraphDisp( gno+114+i, FALSE );
					GD_SetGraphDisp( gno+132+i, FALSE );
					GD_SetGraphDisp( gno+150+i, FALSE );
				}
				for( i=0 ; i < 5 ; i++ ){
					GD_SetGraphDisp( gno+106+i, FALSE );
				}

				SetGameWindowDisp( win_no+4, FALSE );

				WLD_SetRightBar18Disp( gno+168, FALSE );
				GD_SetGraphDisp( gno+95, FALSE );

				count = 0;
				MUS_SetMouseLayer( mouse );
				step = ITS_MIAN;
			}
			select_bak = select_no+ipage;
			break;

		case ITS_MIAN:
			if(count==0){
				if(kajiya){
					if( ESC_GetFlag(64)==2 ){	
						SetGameWindowText( win_no, "武器・防具一式『金物屋』" );
					}else{
						SetGameWindowText( win_no, "武器屋です。" );
					}
				}else{
					if( ESC_GetFlag(63)==2 ){	
						SetGameWindowText( win_no, "道具屋『グッド・ピープルズ』" );
					}else{
						SetGameWindowText( win_no, "道具屋" );
					}
				}
			}

			SetGameWindowDisp( win_no+3, TRUE );

			count++;

			if( select_no!=-1 ){
				if(select_bak != select_no ){
					SND_Select();
				}

				GD_SetGraphDisp( gno+95, TRUE );
				GD_SetGraphDstRect( gno+95, 800-(24*4+48)-16+24, 24+24+24*select_no, 24*4,  24 );
			}else{
				GD_SetGraphDisp( gno+95, FALSE );
			}

			if( select_no!=-1 && GameKey.click ){
				SetGameWindowDisp( win_no+3, FALSE );

				MUS_SetMouseLayer( mouse+1 );
				if(select_no==0){
					spage = 0;
					count = 0;
					step = ITS_BUY;
				}else{
					ipage=0;
					count = 0;
					step = ITS_SELL;
				}
				SND_Ok();
			}else if(GameKey.cansel){
				count = 0;
				step = ITS_FADE_OUT;

				SND_Cansel();
			}
			select_bak = select_no;
			break;
	}

	return ret;
	
}



#define	FLAG_KENSI	10

void SetYouheiChar( int *hpage, int *hpage_max, int *pt_no_han, int *kpage, int *kpage_max, int *pt_no_kun )
{
	int	i,j,k;
	ZeroMemory( pt_no_han, sizeof(int)*32 );
	ZeroMemory( pt_no_kun, sizeof(int)*32 );
	for(k=0,j=0,i=9;i<32;i++){
		if(CharParam[i].flag){
			switch(CharParam[i].unit){
				default:
				case 0:	pt_no_han[j++]=i;	break;
				case 3:	pt_no_kun[k++]=i;	break;
			}
		}
	}
	*hpage=0;
	*hpage_max=j;
	*kpage=0;
	*kpage_max=k;
}

int	WLD_Heitan( int win_no, int win_bno, int gno, int bno, int spno, int layer, int mouse )
{
	enum{
		HEI_INIT,
		HEI_TERM,

		HEI_FADE_IN,
		HEI_FADE_OUT,

		HEI_MIAN,
		HEI_SEX,
		HEI_JOB,
		HEI_KOYOU_CHECK,

		HEI_KAIKO,
		HEI_KAIKO_CHECK,

		HEI_CHENG,
		HEI_CHENG_JOB,
		HEI_CHENG_CHECK,

		HEI_KUNREN_IN,
		HEI_KUNREN_OUT,
	};
	static int	step=HEI_INIT;
	static int	count=0;
	int			ret=0;
	int			redraw=0;
	int			i,j,k;

	char		buf[768];
	char		buf2[256];
	static int	hpage=0, hpage_max;
	static int	pt_no_han[32];
	static int	kpage=0, kpage_max;
	static int	pt_no_kun[32];

	static int	select_list[5];
	static int	select_list2[6];
	static int	select_list3[6];
	static int	select_bak=-1;
	int			select_no = MUS_GetMouseNo( -1 );

	switch(step){
		case HEI_INIT:
			MUS_SetMouseLayer( mouse );
			GD_LoadTexture( bno+2, "digit002.tga",  ALPHA_4444 );

			SetGameWindow( win_no+1, gno+80, win_bno, layer, TRUE,-1, WINDOW_COLOR_SYS );
			SetGameWindowRect( win_no+1, 800-(24*8+48)-4, 24, 24*8+48, 24*5+48 );
			SetGameWindowText( win_no+1, "雇用\n解雇\n転職\n訓練所に入れる\n訓練所から出す", bno+1 );

			for(i=0;i<5;i++){
				MUS_SetMouseRect( mouse+0, i, 800-(24*8+48)-4+24, 48+24*i, 24*8, 24, ON );
			}

			SetGameWindow( win_no+2, gno+83, win_bno, layer, TRUE,-1, WINDOW_COLOR_SYS );
			SetGameWindowRect( win_no+2, 124, 448, 24*8+48,  16*1+48 );

			GD_SetGraph( gno+86, win_bno, layer+1, TRUE );
			GD_SetGraphRect( gno+86, 124+24+24*6, 448+24, 656, 496+16, 48, 16 );

			GD_SetGraphDigit( gno+87, bno+2, layer+1, TRUE, PlayerParam.money, 8 );
			GD_SetGraphRect( gno+87, 124+85, 448+24, 0, 16, 10, 16 );
			GD_SetGraphColor( gno+87, 255, 91,55,21 );

			GD_SetGraph( gno+88, win_bno, layer+1, TRUE );
			GD_SetGraphRect( gno+88, 124+24, 448+24, 656, 496, 48, 16 );

			SetGameWindow( win_no+3, gno+90, win_bno, layer+3, FALSE,-1, WINDOW_COLOR_SYS );
			SetGameWindowRect( win_no+3, 800-(24*8+48)-4-64, 24+64, 24*3+48, 24*2+48 );
			SetGameWindowText( win_no+3, "男性\n女性", bno+5 );
			MUS_SetMouseRect( mouse+1, 0, 800-(24*8+24)-4-64, 48+64, 24*3, 24, ON );
			MUS_SetMouseRect( mouse+1, 1, 800-(24*8+24)-4-64, 72+64, 24*3, 24, ON );

			SetGameWindow( win_no+4, gno+93, win_bno, layer+6, FALSE,-1, WINDOW_COLOR_SYS );
			SetGameWindowRect( win_no+4, 800-(24*8+48)-4-128, 24+128, 24*5+48, 6*24+48 );
			SetGameWindowText( win_no+4, "職業", bno+6 );


			SetGameWindow( win_no+5, gno+96, win_bno, layer+9, FALSE,-1, WINDOW_COLOR_SYS );
			SetGameWindowRect( win_no+5, 800-(24*8+48)-4-192, 24+192, 24*3+48, 24*2+48 );
			SetGameWindowText( win_no+5, "はい\nいいえ", bno+7 );
			MUS_SetMouseRect( mouse+3, 0, 800-(24*8+24)-4-192, 48+192, 24*3, 24, ON );
			MUS_SetMouseRect( mouse+3, 1, 800-(24*8+24)-4-192, 72+192, 24*3, 24, ON );


			SetYouheiChar( &hpage, &hpage_max, pt_no_han, &kpage, &kpage_max, pt_no_kun );

			SetGameWindow( win_no+6, gno+108, win_bno, layer, FALSE, 1, WINDOW_COLOR_SYS );
			SetGameWindowRect( win_no+6, 800-360-4, 24, 360, 480 );
			SetGameWindowText( win_no+6, "キャラクター", bno+10 );

			WLD_SetRightBar18( win_bno, gno+100, layer+1, mouse+4 );
			for( i=0 ; i < 18 ; i++ ){
				MUS_SetMouseRect( mouse+4, i, 800-360-4+24, 24+24+24*i, 24*12, 24, ON );
			}

			SetGameWindowText( win_no, "傭兵の雇用、解雇、訓練などが出来ます。" );

			GD_SetGraphx9( gno+150, win_bno, layer+1, FALSE );	
			GD_SetGraphSrcRect( gno+150, 0, 64, 72, 24 );
			GD_SetGraphx9( gno+151, win_bno, layer+4, FALSE );	
			GD_SetGraphSrcRect( gno+151, 0, 64, 72, 24 );
			GD_SetGraphx9( gno+152, win_bno, layer+7, FALSE );	
			GD_SetGraphSrcRect( gno+152, 0, 64, 72, 24 );
			GD_SetGraphx9( gno+153, win_bno, layer+10, FALSE );	
			GD_SetGraphSrcRect( gno+153, 0, 64, 72, 24 );

			{
				int	kensi=0;
				int	masen=0;
				int	miko=0;
				int	yumi=0;
				int	sensi=0;
				int	mahou=0;
				int	kari=0;
				int	kisi=0;
				for(i=9;i<32;i++){
					if(CharParam[i].flag){
						if( CharParam[i].job==0 && kensi <CharParam[i].lv )	kensi  = CharParam[i].lv;
						if( CharParam[i].job==1 && masen <CharParam[i].lv )	masen  = CharParam[i].lv;
						if( CharParam[i].job==2 && miko  <CharParam[i].lv )	miko   = CharParam[i].lv;
						if( CharParam[i].job==4 && yumi  <CharParam[i].lv )	yumi   = CharParam[i].lv;

						if( CharParam[i].job==6 && sensi <CharParam[i].lv )	sensi  = CharParam[i].lv;
						if( CharParam[i].job==8 && mahou <CharParam[i].lv )	mahou  = CharParam[i].lv;
						if( CharParam[i].job==10&& kari  <CharParam[i].lv )	kari   = CharParam[i].lv;
					}
					
				}

				if( kensi>=10 && mahou>=10 ){
					ESC_SetFlag( FLAG_KENSI+1, ON );
				}
				if( miko>=20 && mahou>=15 ){
					ESC_SetFlag( FLAG_KENSI+3, ON );
				}
				if( masen>10 && yumi>=20 ){
					ESC_SetFlag( FLAG_KENSI+5, ON );
				}
				if( yumi>=10 ){
					ESC_SetFlag( FLAG_KENSI+10, ON );
				}
				if( miko>=15 && mahou>=20 ){
					ESC_SetFlag( FLAG_KENSI+9, ON );
				}
				if( kari>=20 ){
					ESC_SetFlag( FLAG_KENSI+11, ON );
				}


				ESC_SetFlag( FLAG_KENSI+0, ON );
				ESC_SetFlag( FLAG_KENSI+2, ON );
				ESC_SetFlag( FLAG_KENSI+4, ON );

				ESC_SetFlag( FLAG_KENSI+6, ON );
				ESC_SetFlag( FLAG_KENSI+7, ON );
				ESC_SetFlag( FLAG_KENSI+8, ON );
			}

			select_bak=-1;
			count=0;
			step=HEI_MIAN;
			break;
		case HEI_TERM:
WLD_ResetRightBar18( gno+100 );
			for(i=10;i<160;i++)	{	GD_ResetGraph(gno+i);				}
			for(i=1;i<13;i++)	{	GD_ReleaseTexture(bno+i);			}
			for(i=0;i<32;i++)	{	GD_ReleaseSprite(spno+i);			}
			for(i=0;i<4;i++)	{	MUS_ResetMouseRect_Layer(mouse+i);	}
			for(i=1;i<7;i++)	{	ResetGameWindow(win_no+i);			}
			step=HEI_INIT;
			ret=1;
			break;

		case HEI_MIAN:
			if(count==0){
				GD_SetGraphDigitNo( gno+87, PlayerParam.money );
				SetYouheiChar( &hpage, &hpage_max, pt_no_han, &kpage, &kpage_max, pt_no_kun );
				SetGameWindowText( win_no, "傭兵の雇用、解雇、訓練などが出来ます。" );
			}
			count++;

			SetGameWindowDisp( win_no+1, TRUE );
			if( select_no!=-1 ){
				GD_SetGraphDisp( gno+150, TRUE );
				GD_SetGraphDstRect( gno+150, 800-(24*8+24)-4, 24+24+24*select_no, 24*8,  24 );

				if(select_bak != select_no ){
					SND_Select();
					switch( select_no ){
					case  0:	SetGameWindowText( win_no, "新規に傭兵を雇用します。\n傭兵を育てることで、雇用できる職業も増えていきます。" );	break;
					case  1:	SetGameWindowText( win_no, "傭兵を解雇します。\n多すぎると邪魔な時に。" );	break;
					case  2:	SetGameWindowText( win_no, "傭兵を転職させます。\n転職を繰り返すことで多彩な技を覚え、基本能力も向上します。" );	break;
					case  3:	SetGameWindowText( win_no, "傭兵を訓練所に入れます。\nプレイヤーが戦闘をするごとに勝手に育っていきます。" );	break;
					case  4:	SetGameWindowText( win_no, "傭兵を訓練所から出します。" );	break;
					}
				}

				if( GameKey.click ){
					select_list[0] = select_no;
					switch(select_no){
						case 0:
							if( NokoriYouhei() ){	
								MUS_SetMouseLayer( mouse+1 );
								count = 0;
								step = HEI_SEX;
								SND_Ok();
							}else{
								SND_Ng();
							}
							break;
						case 1:
							MUS_SetMouseLayer( mouse+4 );
							count = 0;
							step = HEI_KAIKO;
							SND_Ok();
							break;
						case 2:
							MUS_SetMouseLayer( mouse+4 );
							count = 0;
							step = HEI_CHENG;
							SND_Ok();
							break;
						case 3:
							MUS_SetMouseLayer( mouse+4 );
							count = 0;
							step = HEI_KUNREN_IN;
							SND_Ok();
							break;
						case 4:
							MUS_SetMouseLayer( mouse+4 );
							count = 0;
							step = HEI_KUNREN_OUT;
							SND_Ok();
							break;
					}
				}
			}else{
				GD_SetGraphDisp( gno+150, FALSE );
			}
			if(GameKey.cansel){
				count = 0;
				step = HEI_TERM;

				SND_Cansel();
			}
			select_bak=select_no;
			break;
		case HEI_SEX:
			if(count==0)SetGameWindowText( win_no, "雇用する傭兵の性別を選んでください。" );
			SetGameWindowDisp( win_no+3, TRUE );

			if( select_no!=-1){
				GD_SetGraphDisp( gno+151, TRUE );
				GD_SetGraphDstRect( gno+151, 800-(24*8+24)-4-64, 48+64+24*select_no, 24*3, 24 );
				if(select_bak != select_no ){
					SND_Select();
				}

				if( GameKey.click ){
					select_list[1] = select_no;

					MUS_SetMouseLayer( mouse+2 );
					count = 0;
					step = HEI_JOB;
				
					MUS_ResetMouseRect_Layer( mouse+2 );
					buf[0] = '\0';
					j=0;
					k = (select_list[1])?0:6;
					for(i=0;i<6;i++){
						if( ESC_GetFlag( FLAG_KENSI+i+k ) ){
							select_list3[i] = j;
							select_list2[j] = i+k;
							_mbscat( (BYTE*)buf, (BYTE*)JobParam[i+k].name );
							_mbscat( (BYTE*)buf, (BYTE*)"\n" );

							MUS_SetMouseRect( mouse+2, i, 800-(24*8+24)-4-128, 48+24*j+128, 24*5, 24, ON );
							j++;
						}
					}
					SetGameWindowRect( win_no+4, 800-(24*8+48)-4-128, 24+128, 24*5+48, j*24+48 );
					SetGameWindowText( win_no+4, buf );
					SND_Ok();
				}
			}
			if(GameKey.cansel){
				MUS_SetMouseLayer( mouse );
				SetGameWindowDisp( win_no+3, FALSE );
				GD_SetGraphDisp( gno+151, FALSE );

				count = 0;
				step = HEI_MIAN;
				SND_Cansel();
			}
			select_bak=select_no;
			break;
		case HEI_JOB:
			SetGameWindowDisp( win_no+4, TRUE );

			if( select_no!=-1 ){
				GD_SetGraphDisp( gno+152, TRUE );
				GD_SetGraphDstRect( gno+152, 800-(24*8+24)-4-128, 48+128+24*select_list3[select_no], 24*5, 24 );
				if(select_bak != select_no ){

					switch( select_no + ((select_list[1])?0:6) ){
					case  0:	SetGameWindowText( win_no, "剣の達人。高い攻撃力を誇ります。\n職業特性として物理攻撃回避にボーナスがあります。" );	break;
					case  1:	SetGameWindowText( win_no, "多彩な魔法と剣、そして槍を使いこなします。\n職業特性は魔法の速射。" );	break;
					case  2:	SetGameWindowText( win_no, "回復魔法のスペシャリスト。攻撃能力は皆無です。でも…" );	break;
					case  3:	SetGameWindowText( win_no, "回復魔法に秀で、さらに、ある程度の攻撃魔法も使えます。" );	break;
					case  4:	SetGameWindowText( win_no, "弓のスペシャリスト。高い攻撃力と移動力で敵を翻弄します。\n職業特性は広視野。側面からの攻撃が不利になりません。" );	break;
					case  5:	SetGameWindowText( win_no, "歌って踊れる旅芸人。伝承歌と弓を使います。\n職業特性は減衰無効。デメリットのある効果を受けません。" );	break;

					case  6:	SetGameWindowText( win_no, "前線の要、高い防御力で戦線を維持します。\n職業特性は属性防御、反属性からの攻撃を無効化します。" );	break;
					case  7:	SetGameWindowText( win_no, "戦士にして、回復魔法も使えます。\n職業特性は詠唱防御。魔法詠唱中も隙がありません。" );	break;
					case  8:	SetGameWindowText( win_no, "攻撃魔法のスペシャリスト、強力な魔力で敵をなぎ払え！" );	break;
					case  9:	SetGameWindowText( win_no, "攻撃魔法に加え、ちょっとした回復も覚えます。\n攻守のバランスが取れた最高位の魔法使い。" );	break;
					case 10:	SetGameWindowText( win_no, "モンスターハンティングならおまかせ。\n戦闘に参加することで取得アイテムが増えます。弓も強力。" );	break;
					case 11:	SetGameWindowText( win_no, "伝承歌を伝える一族の語り部。\n職業特性は、平静。行動中でもMPが自然回復します。" );	break;
					}
					SND_Select();
				}

				if( GameKey.click ){
					select_list[2] = select_no;

					MUS_SetMouseLayer( mouse+3 );
					count = 0;
					step = HEI_KOYOU_CHECK;

					SetCharParam( 32, select_no + ((select_list[1])?0:6), 1, 1, 0, CHR_GetCharName(!select_list[1]) );
					SetCharParamWindow( win_no, win_bno, gno, bno, spno, layer, 32, FALSE );
					SND_Ok();
				}
			}else{
				if(select_bak != select_no ){
					SetGameWindowText( win_no, "傭兵の職業を選んでください。" );
				}
			}
			if(GameKey.cansel){
				MUS_SetMouseLayer( mouse+1 );
				SetGameWindowDisp( win_no+4, FALSE );
				GD_SetGraphDisp( gno+152, FALSE );

				count = 0;
				step = HEI_SEX;
			}
			select_bak=select_no;
			break;
		case HEI_KOYOU_CHECK:
			if(count==0)SetGameWindowText( win_no, "雇用料100ELEMです。\nこのキャラクターを雇用します。よろしいですか？" );
			SetGameWindowDisp( win_no+5, TRUE );

			if( select_no!=-1 ){
				GD_SetGraphDisp( gno+153, TRUE );
				GD_SetGraphDstRect( gno+153, 800-(24*8+24)-4-192, 48+192+24*select_no, 24*3, 24);
				if(select_bak != select_no ){
					SND_Select();
				}

				if( GameKey.click ){
					select_list[3] = select_no;
					if(select_no==0){
						if(PlayerParam.money>=100){
							NewYouhei();
							count = 0;
		

							MUS_SetMouseLayer( mouse );
							SetGameWindowDisp( win_no+3, FALSE );
							SetGameWindowDisp( win_no+4, FALSE );
							SetGameWindowDisp( win_no+5, FALSE );
							ResetCharParamWindow( win_no, win_bno, gno, bno, spno );
							GD_SetGraphDisp( gno+151, FALSE );
							GD_SetGraphDisp( gno+152, FALSE );
							GD_SetGraphDisp( gno+153, FALSE );

							PlayerParam.money-=100;
							step = HEI_MIAN;
							SND_Ok();
						}else{
							SND_Ng();
						}
					}else{

						MUS_SetMouseLayer( mouse+2 );
						SetGameWindowDisp( win_no+5, FALSE );
						ResetCharParamWindow( win_no, win_bno, gno, bno, spno );
						GD_SetGraphDisp( gno+153, FALSE );

						count = 0;
						step = HEI_JOB;
						SND_Cansel();
					}
				}
			}
			if(GameKey.cansel){
				MUS_SetMouseLayer( mouse+2 );
				SetGameWindowDisp( win_no+5, FALSE );
				GD_SetGraphDisp( gno+153, FALSE );
				ResetCharParamWindow( win_no, win_bno, gno, bno, spno );

				count = 0;
				step = HEI_JOB;
				SND_Cansel();
			}
			select_bak=select_no;
			break;
		case HEI_KAIKO_CHECK:
			if(count==0)SetGameWindowText( win_no, "このキャラクターを解雇します。よろしいですか？" );
			SetGameWindowDisp( win_no+5, TRUE );

			if( select_no!=-1 ){
				GD_SetGraphDisp( gno+153, TRUE );
				GD_SetGraphDstRect( gno+153, 800-(24*8+24)-4-192, 48+192+24*select_no, 24*3, 24);
				if(select_bak != select_no ){
					SND_Select();
				}

				if( GameKey.click ){
					select_list[3] = select_no;
					if(select_no==0){
						ResetCharParam( pt_no_han[select_list[1]] );
						count = 0;
	
						MUS_SetMouseLayer( mouse+4 );
						SetGameWindowDisp( win_no+5, FALSE );
						GD_SetGraphDisp( gno+153, FALSE );

						step = HEI_KAIKO;
						SND_Ok();
					}else{
						MUS_SetMouseLayer( mouse+4 );
						SetGameWindowDisp( win_no+5, FALSE );
						GD_SetGraphDisp( gno+153, FALSE );

						count = 0;
						step = HEI_KAIKO;
						SND_Cansel();
					}
				}
			}else if(GameKey.cansel){
				MUS_SetMouseLayer( mouse+4 );
				SetGameWindowDisp( win_no+5, FALSE );
				GD_SetGraphDisp( gno+153, FALSE );

				count = 0;
				step = HEI_KAIKO;
				SND_Cansel();
			}
			select_bak=select_no;
			break;
		case HEI_KAIKO:
			if(count==0)SetGameWindowText( win_no, "解雇するキャラクターを選んでください。" );
			if(count==0)SetYouheiChar( &hpage, &hpage_max, pt_no_han, &kpage, &kpage_max, pt_no_kun );

			SetGameWindowDisp( win_no+1, FALSE );

			redraw = WLD_DragRightBar18( &hpage, &hpage_max, select_no );
			if( redraw || count==0 ){
				buf[0] = '\0';
				for( i=0 ; i < min(18,hpage_max-hpage) ; i++ ){
					wsprintf( buf2, "%2d.%-14s Lv.%3d\n", pt_no_han[hpage+i], CharParam[ pt_no_han[hpage+i] ].name, CharParam[ pt_no_han[hpage+i] ].lv );
					_mbscat( (BYTE*)buf, (BYTE*)buf2 );
				}
				SetGameWindowText( win_no+6, buf );
			}
			SetGameWindowDisp( win_no+6, TRUE );
			WLD_RenewRightBar18( gno+100, hpage, hpage_max, 18==select_no, 19==select_no, GameKey.click );
			count++;

			if(select_bak != select_no+hpage ){
				if( select_no!=-1 && select_no!=20 ){
					if(select_no<18 && select_no<min(18,hpage_max) ){
						SND_Select();
					}
				}
			}
			if( select_no!=-1 && select_no<min(18,hpage_max) ){
				GD_SetGraphDisp( gno+150, TRUE );
				GD_SetGraphDstRect( gno+150, 800-(24*13+24)-4, 48+24*select_no, 24*12+8, 24);
				if( GameKey.click){
					select_list[1] = hpage+select_no;
					MUS_SetMouseLayer( mouse+3 );
					count = 0;
					step = HEI_KAIKO_CHECK;
					SND_Ok();
				}
			}else{
				GD_SetGraphDisp( gno+150, FALSE );
			}
			if(GameKey.cansel){
				MUS_SetMouseLayer( mouse );
				SetGameWindowDisp( win_no+6, FALSE );
				WLD_SetRightBar18Disp( gno+100, FALSE );

				ResetCharParamWindow( win_no, win_bno, gno, bno, spno );

				count = 0;
				step = HEI_MIAN;
				SND_Cansel();
			}
			if(step==HEI_KAIKO){
				if( select_no!=-1 && select_no<min(18,hpage_max) ){
					if(select_bak!=select_no || redraw){
						SetCharParamWindow( win_no, win_bno, gno, bno, spno, layer, pt_no_han[hpage+select_no], FALSE );
					}
				}else{
					ResetCharParamWindow( win_no, win_bno, gno, bno, spno );
				}
			}
			select_bak=select_no+hpage;
			break;

		case HEI_CHENG:
			if(count==0){
				GD_SetGraphDigitNo( gno+87, PlayerParam.money );
				SetGameWindowText( win_no, "転職するキャラクターを選んでください。" );
				SetYouheiChar( &hpage, &hpage_max, pt_no_han, &kpage, &kpage_max, pt_no_kun );
			}

			SetGameWindowDisp( win_no+1, FALSE );
			redraw = WLD_DragRightBar18( &hpage, &hpage_max, select_no );
			if( redraw || count==0 ){
				buf[0] = '\0';
				for( i=0 ; i < min(18,hpage_max-hpage) ; i++ ){
					if( CharParam[ pt_no_han[hpage+i] ].lv>=10 ){
						wsprintf( buf2, "%2d.%-14s Lv.%3d\n", pt_no_han[hpage+i], CharParam[ pt_no_han[hpage+i] ].name, CharParam[ pt_no_han[hpage+i] ].lv );
					}else{
						wsprintf( buf2, "<C2:%2d.%-14s Lv.%3d>\n", pt_no_han[hpage+i], CharParam[ pt_no_han[hpage+i] ].name, CharParam[ pt_no_han[hpage+i] ].lv );
					}
					_mbscat( (BYTE*)buf, (BYTE*)buf2 );
				}
				SetGameWindowText( win_no+6, buf );
			}
			SetGameWindowDisp( win_no+6, TRUE );
			WLD_RenewRightBar18( gno+100, hpage, hpage_max, 18==select_no, 19==select_no, GameKey.click );
			count++;

			if(select_bak != select_no+hpage ){
				if( select_no!=-1 && select_no!=20 ){
					if(select_no<18 && select_no<min(18,hpage_max) ){
						SND_Select();
					}
				}
			}
			if( select_no!=-1 && select_no<min(18,hpage_max) ){
				GD_SetGraphDisp( gno+150, TRUE );
				GD_SetGraphDstRect( gno+150, 800-(24*13+24)-4, 48+24*select_no, 24*12+8, 24);
				if( GameKey.click){
					if( CharParam[ pt_no_han[hpage+select_no] ].lv>=10 ){
						select_list[1] = hpage+select_no;

						MUS_ResetMouseRect_Layer( mouse+2 );
						MUS_SetMouseLayer( mouse+2 );
						count = 0;
						step = HEI_CHENG_JOB;
					
						MUS_ResetMouseRect_Layer( mouse+2 );
						buf[0] = '\0';
						j=0;
						k = (CharParam[ pt_no_han[select_list[1]] ].job/6)?6:0;
						for(i=0;i<6;i++){
							select_list2[i] = j;
							if( ESC_GetFlag( FLAG_KENSI+i+k ) && CharParam[ pt_no_han[select_list[1]] ].job!=i+k ){
								_mbscat( (BYTE*)buf, (BYTE*)JobParam[i+k].name );
								_mbscat( (BYTE*)buf, (BYTE*)"\n" );

								MUS_SetMouseRect( mouse+2, i, 800-(24*8+24)-4-128, 48+24*j+128, 24*5, 24, ON );
								j++;
							}
						}
						SetGameWindowRect( win_no+4, 800-(24*8+48)-4-128, 24+128, 24*5+48, j*24+48 );
						SetGameWindowText( win_no+4, buf );

						SND_Ok();
					}else{
						SND_Ng();
					}
				}
			}else{
				GD_SetGraphDisp( gno+150, FALSE );
			}
			if(GameKey.cansel){
				MUS_SetMouseLayer( mouse );
				SetGameWindowDisp( win_no+6, FALSE );
				WLD_SetRightBar18Disp( gno+100, FALSE );
				ResetCharParamWindow( win_no, win_bno, gno, bno, spno );

				count = 0;
				step = HEI_MIAN;
			}
			if(step==HEI_CHENG){
				if( select_no!=-1 && select_no<min(18,hpage_max) ){
					if(select_bak!=select_no || redraw){
						SetCharParamWindow( win_no, win_bno, gno, bno, spno, layer, pt_no_han[hpage+select_no], FALSE );
					}
				}else{
					ResetCharParamWindow( win_no, win_bno, gno, bno, spno );
				}
			}
			select_bak=select_no+hpage;
			break;

		case HEI_CHENG_JOB:
			SetGameWindowDisp( win_no+4, TRUE );

			if( select_no!=-1 ){
				GD_SetGraphDisp( gno+152, TRUE );
				GD_SetGraphDstRect( gno+152, 800-(24*8+24)-4-128, 48+128+24*select_list2[select_no], 24*5, 24 );
				if(select_bak != select_no ){
					k = (CharParam[ pt_no_han[select_list[1]] ].job/6)?6:0;
					switch( select_no + k ){
					case  0:	SetGameWindowText( win_no, "剣の達人。高い攻撃力を誇ります。\n職業特性として物理攻撃回避にボーナスがあります。" );	break;
					case  1:	SetGameWindowText( win_no, "多彩な魔法と剣、そして槍を使いこなします。\n職業特性は魔法の速射。" );	break;
					case  2:	SetGameWindowText( win_no, "回復魔法のスペシャリスト。攻撃能力は皆無です。でも…" );	break;
					case  3:	SetGameWindowText( win_no, "回復魔法に秀で、さらに、ある程度の攻撃魔法も使えます。" );	break;
					case  4:	SetGameWindowText( win_no, "弓のスペシャリスト。高い攻撃力と移動力で敵を翻弄します。\n職業特性は後方防御。死角がありません。" );	break;
					case  5:	SetGameWindowText( win_no, "歌って踊れる旅芸人。伝承歌と弓を使います。\n職業特性は減衰無効。デメリットのある効果を受けません。" );	break;

					case  6:	SetGameWindowText( win_no, "前線の要、高い防御力で戦線を維持します。\n職業特性は属性防御、反属性からの攻撃を無効化します。" );	break;
					case  7:	SetGameWindowText( win_no, "戦士にして、回復魔法も使えます。\n職業特性は詠唱防御。魔法詠唱中も隙がありません。" );	break;
					case  8:	SetGameWindowText( win_no, "攻撃魔法のスペシャリスト、強力な魔力で敵をなぎ払え！" );	break;
					case  9:	SetGameWindowText( win_no, "攻撃魔法に加え、ちょっとした回復も覚えます。\n攻守のバランスが取れた最高位の魔法使い。" );	break;
					case 10:	SetGameWindowText( win_no, "モンスターハンティングならおまかせ。\n戦闘に参加することで取得アイテムが増えます。弓も強力。" );	break;
					case 11:	SetGameWindowText( win_no, "伝承歌を伝える一族の語り部。\n職業特性は、平静。行動中でもMPが自然回復します。" );	break;
					}
					SND_Select();
				}

				if( GameKey.click ){
					select_list[2] = select_no;

					MUS_SetMouseLayer( mouse+3 );
					count = 0;
					step = HEI_CHENG_CHECK;

					k = (CharParam[ pt_no_han[select_list[1]] ].job/6)?6:0;

					ChangeCharJob32( pt_no_han[select_list[1]], select_no + k );
					SetCharParamWindow( win_no, win_bno, gno, bno, spno, layer, 32, FALSE );
					SND_Ok();
				}
			}else{
				if(select_bak != select_no ){
					SetGameWindowText( win_no, "転職する職業を選んでください。" );
				}
			}
			if(GameKey.cansel){
				MUS_SetMouseLayer( mouse+4 );
				SetGameWindowDisp( win_no+4, FALSE );
				GD_SetGraphDisp( gno+152, FALSE );

				count = 0;
				step = HEI_CHENG;
				SND_Cansel();
			}
			select_bak=select_no;
			break;
		case HEI_CHENG_CHECK:
			if(count==0)SetGameWindowText( win_no, "転職料1000ELEMです。\nこのキャラクターを転職させます。よろしいですか？" );
			SetGameWindowDisp( win_no+5, TRUE );

			if( select_no!=-1 ){
				GD_SetGraphDisp( gno+153, TRUE );
				GD_SetGraphDstRect( gno+153, 800-(24*8+24)-4-192, 48+192+24*select_no, 24*3, 24);
				if( GameKey.click ){
					select_list[3] = select_no;
					if(select_no==0){
						if(PlayerParam.money>=1000){
							ChangeCharJob( pt_no_han[select_list[1]] );
							count = 0;
		

							MUS_SetMouseLayer( mouse+4 );
							SetGameWindowDisp( win_no+4, FALSE );
							SetGameWindowDisp( win_no+5, FALSE );

							GD_SetGraphDisp( gno+152, FALSE );
							GD_SetGraphDisp( gno+153, FALSE );

							PlayerParam.money-=1000;
							count = 0;
							step = HEI_CHENG;
							SND_Ok();
						}else{
							SND_Ng();
						}
					}else{

						MUS_SetMouseLayer( mouse+2 );
						SetGameWindowDisp( win_no+5, FALSE );
						SetCharParamWindow( win_no, win_bno, gno, bno, spno, layer, pt_no_han[select_list[1]], FALSE );
						GD_SetGraphDisp( gno+153, FALSE );

						count = 0;
						step = HEI_CHENG_JOB;
						SND_Cansel();
					}
				}
			}
			if(GameKey.cansel){
				MUS_SetMouseLayer( mouse+2 );
				SetGameWindowDisp( win_no+5, FALSE );
				SetCharParamWindow( win_no, win_bno, gno, bno, spno, layer, pt_no_han[select_list[1]], FALSE );
				GD_SetGraphDisp( gno+153, FALSE );

				count = 0;
				step = HEI_CHENG_JOB;
				SND_Cansel();
			}
			break;

		case HEI_KUNREN_IN:
			if(count==0)SetGameWindowText( win_no, "訓練所に入れるキャラクターを選んでください。\n※注：訓練所から出すまでは、戦闘には出撃できません。" );
			SetGameWindowDisp( win_no+1, FALSE );
			redraw = WLD_DragRightBar18( &hpage, &hpage_max, select_no );
			if( redraw || count==0 ){
				buf[0] = '\0';
				for( i=0 ; i < min(18,hpage_max-hpage) ; i++ ){
					wsprintf( buf2, "%2d.%-14s Lv.%3d\n", pt_no_han[hpage+i], CharParam[ pt_no_han[hpage+i] ].name, CharParam[ pt_no_han[hpage+i] ].lv );
					_mbscat( (BYTE*)buf, (BYTE*)buf2 );
				}
				SetGameWindowText( win_no+6, buf );
			}
			SetGameWindowDisp( win_no+6, TRUE );
			WLD_RenewRightBar18( gno+100, hpage, hpage_max, 18==select_no, 19==select_no, GameKey.click );
			count++;

			if(select_bak != select_no+hpage ){
				if( select_no!=-1 && select_no!=20 ){
					if(select_no<18 && select_no<min(18,hpage_max) ){
						SND_Select();
					}
				}
			}
			if( select_no!=-1 && select_no<min(18,hpage_max) ){
				GD_SetGraphDisp( gno+150, TRUE );
				GD_SetGraphDstRect( gno+150, 800-(24*13+24)-4, 48+24*select_no, 24*12+8, 24);
				if( GameKey.click){
					CharParam[ pt_no_han[hpage+select_no] ].unit = 3;
					SetYouheiChar( &hpage, &hpage_max, pt_no_han, &kpage, &kpage_max, pt_no_kun );
					count = 0;
					
					SND_Ok();
				}
			}else{
				GD_SetGraphDisp( gno+150, FALSE );
			}
			if(GameKey.cansel){
				MUS_SetMouseLayer( mouse );
				SetGameWindowDisp( win_no+6, FALSE );
				WLD_SetRightBar18Disp( gno+100, FALSE );

				ResetCharParamWindow( win_no, win_bno, gno, bno, spno );

				count = 0;
				step = HEI_MIAN;

				SND_Cansel();
			}
			if(step==HEI_KUNREN_IN){
				if( select_no!=-1 && select_no<min(18,hpage_max) ){
					if(select_bak!=select_no+hpage || redraw){
						if(pt_no_han[hpage+select_no]){
							SetCharParamWindow( win_no, win_bno, gno, bno, spno, layer, pt_no_han[hpage+select_no], FALSE );
						}else{
							ResetCharParamWindow( win_no, win_bno, gno, bno, spno );
						}
					}
				}else{
					ResetCharParamWindow( win_no, win_bno, gno, bno, spno );
				}
			}
			select_bak=select_no+hpage;
			break;
		case HEI_KUNREN_OUT:
			if(count==0)SetGameWindowText( win_no, "訓練所から出すキャラクターを選んでください。" );
			SetGameWindowDisp( win_no+1, FALSE );
			redraw = WLD_DragRightBar18( &kpage, &kpage_max, select_no );
			if( redraw || count==0 ){
				buf[0] = '\0';
				for( i=0 ; i < min(18,kpage_max-kpage) ; i++ ){
					wsprintf( buf2, "%2d.%-14s Lv.%3d\n", pt_no_kun[kpage+i], CharParam[ pt_no_kun[kpage+i] ].name, CharParam[ pt_no_kun[kpage+i] ].lv );
					_mbscat( (BYTE*)buf, (BYTE*)buf2 );
				}
				SetGameWindowText( win_no+6, buf );
			}
			SetGameWindowDisp( win_no+6, TRUE );
			WLD_RenewRightBar18( gno+100, kpage, kpage_max, 18==select_no, 19==select_no, GameKey.click );
			count++;

			if(select_bak != select_no+kpage ){
				if( select_no!=-1 && select_no!=20 ){
					if(select_no<18 && select_no<min(18,kpage_max) ){
						SND_Select();
					}
				}
			}
			if( select_no!=-1 && select_no<min(18,kpage_max) ){
				GD_SetGraphDisp( gno+150, TRUE );
				GD_SetGraphDstRect( gno+150, 800-(24*13+24)-4, 48+24*select_no, 24*12+8, 24);
				if( GameKey.click){
					CharParam[ pt_no_kun[kpage+select_no] ].unit = 0;

					SetYouheiChar( &hpage, &hpage_max, pt_no_han, &kpage, &kpage_max, pt_no_kun );
					
					count = 0;
					SND_Ok();
				}
			}else{
				GD_SetGraphDisp( gno+150, FALSE );
			}
			if(GameKey.cansel){
				MUS_SetMouseLayer( mouse );
				SetGameWindowDisp( win_no+6, FALSE );
				WLD_SetRightBar18Disp( gno+100, FALSE );

				ResetCharParamWindow( win_no, win_bno, gno, bno, spno );

				count = 0;
				step = HEI_MIAN;
				SND_Cansel();
			}
			if(step==HEI_KUNREN_OUT){
				if( select_no!=-1 && select_no<min(18,kpage_max) ){
					if(select_bak!=select_no+kpage || redraw){
						if(pt_no_kun[kpage+select_no]){
							SetCharParamWindow( win_no, win_bno, gno, bno, spno, layer, pt_no_kun[kpage+select_no], FALSE );
						}else{
							ResetCharParamWindow( win_no, win_bno, gno, bno, spno );
						}
					}
				}else{
					ResetCharParamWindow( win_no, win_bno, gno, bno, spno );
				}
			}
			select_bak=select_no+kpage;
			break;
	}

	return ret;
}

int	WLD_ItemList( int win_no, int win_bno, int gno, int bno, int spno, int layer, int mouse )
{
	enum{
		ITM_INIT,
		ITM_TERM,

		ITM_FADE_IN,
		ITM_FADE_OUT,

		ITM_MIAN,
	};
	static int	step=ITM_INIT;
	static int	count=0;
	int			ret=0;
	int			redraw=0;
	int			i,j;
	char		buf[512];
	char		buf2[256];
	static int	ipage=0, ipage_max;

	static int	select_bak=-1;
	int			select_no = MUS_GetMouseNo( -1 );

	switch(step){
		case ITM_INIT:
			MUS_SetMouseLayer( mouse );

			GD_LoadTexture( bno+2, "digit002.tga",  ALPHA_4444 );

			SetGameWindow( win_no+4, gno+90, win_bno, layer, FALSE, 2, WINDOW_COLOR_SYS );
			SetGameWindowRect( win_no+4, 800-(24*12+48)-4, 24, 24*12+48,  24*18+48 );
			SetGameWindowText( win_no+4, "アイテム", bno+11,24 );

			for( i=0 ; i < 18 ; i++ ){
				GD_SetGraph( gno+118+i, win_bno, layer+1, FALSE );
				GD_SetGraphRect( gno+118+i, 800-(24*12+48)-4+24, 24+24+24*i, 768, 344, 22, 22 );
				GD_SetGraphScaleParam( gno+118+i, SCALE_HIGHT );

				GD_SetGraphStr( gno+100+i, bno+2, layer+1, FALSE, " " );

				GD_SetGraphRect( gno+100+i, 800-(24*12+48)+24*10, 24+24+24*i+8, 0, 0, 10, 16 );
				GD_SetGraphScaleParam( gno+100+i, SCALE_HIGHT );
				GD_SetGraphColor2( gno+100+i, 91, 55, 21 );

				MUS_SetMouseRect( mouse, i, 800-(24*12+48)+24-4, 24+24+24*i, 24*11, 24, ON );
			}

			SetGameWindow( win_no+3, gno+136, win_bno, layer, FALSE, -1, WINDOW_COLOR_SYS);
			SetGameWindowRect( win_no+3, 4, 600-24-24*4-24*6, 24*11+48,  24*5+48 );
			SetGameWindowText( win_no+3, "　アイテム名\n武器：剣", bno+10 );

			GD_SetGraph( gno+139, win_bno, layer+1, FALSE );
			GD_SetGraphRect( gno+139, 4+24, 600-24-24*10+24, 768, 344, 22, 22 );
			GD_SetGraphScaleParam( gno+139, SCALE_HIGHT );

			for( i=0 ; i < 12 ; i++ ){
				GD_SetGraphStr( gno+140+i, bno+2, layer+1, FALSE, "ABC" );
				GD_SetGraphRect( gno+140+i, 4+24+i%3*90, 600-24-24*8+24+i/3*16, 0, 0, 10, 16 );
				GD_SetGraphColor2( gno+140+i, 91, 55, 21 );

				GD_SetGraphDisp( gno+140+i, TRUE );

				GD_SetGraphStr( gno+152+i, bno+2, layer+1, FALSE, "+9999" );
				GD_SetGraphRect( gno+152+i, 4+24+30+i%3*90, 600-24-24*8+24+i/3*16, 0, 0, 10, 16 );
				if( i%2 ){
					GD_SetGraphColor2( gno+152+i, 61,116,255 );
				}else{
					GD_SetGraphColor2( gno+152+i, 255,56,56 );
				}
				GD_SetGraphDisp( gno+152+i, TRUE );
			}

			WLD_SetRightBar18( win_bno, gno+168, layer+1, mouse );

			GD_SetGraphx9( gno+180, win_bno, layer+1, FALSE );	
			GD_SetGraphSrcRect( gno+180, 0, 64, 72, 24 );

			for( i=0 ; i < ITEM_MAX ; i++ ){
				if(ItemBox[i].ino==0)	break;
			}
			ipage=0;
			ipage_max=i;

			count=0;

			step=ITM_MIAN;

			break;
		case ITM_TERM:
			for(i=10;i<200;i++)	{	GD_ResetGraph(gno+i);				}
			for(i=1;i<13;i++)	{	GD_ReleaseTexture(bno+i);			}
			for(i=0;i<32;i++)	{	GD_ReleaseSprite(spno+i);			}
			for(i=0;i<4;i++)	{	MUS_ResetMouseRect_Layer(mouse+i);	}
			for(i=1;i<5;i++)	{	ResetGameWindow(win_no+i);			}
			step=ITM_INIT;
			ret=1;
			break;

		case ITM_FADE_IN:
			break;
		case ITM_FADE_OUT:
			break;

		case ITM_MIAN:
			redraw = WLD_DragRightBar18( &ipage, &ipage_max, select_no );



			if(GameKey.pup || GameKey.pdown || count==0 || redraw ){
				buf[0] = '\0';
				j=0;
				for( i=0 ; i < min(18,ipage_max) ; i++ ){
					if( ItemBox[ i+ipage ].ino){
						_mbscat( (BYTE*)buf, (BYTE*)ItemParam[ItemBox[i+ipage].ino].name );
						_mbscat( (BYTE*)buf, (BYTE*)"\n" );

						int	type = ItemParam[ ItemBox[i+ipage].ino ].type;
						GD_SetGraphSrcRect( gno+118+i, 768+(type%10)*24, 344+(type/10)*24, 22, 22 );
						GD_SetGraphDisp( gno+118+i, TRUE );

						wsprintf( buf2, "%2d/%-2d", ITM_GetItemEquipAmount(ItemBox[ i+ipage ].ino), ItemBox[ i+ipage ].amount );
						GD_SetGraphStrChange( gno+100+i, buf2 );
						GD_SetGraphDisp( gno+100+i, TRUE );
					}else{
						GD_SetGraphDisp( gno+100+i, FALSE );
					}
				}
				SetGameWindowText( win_no+4, buf );
				SetGameWindowDisp( win_no+4, TRUE );
			}
			count++;

WLD_RenewRightBar18( gno+168, ipage, ipage_max, 18==select_no, 19==select_no, GameKey.click );

			if(select_bak != select_no+ipage ){
				if( select_no!=-1 && select_no!=20 ){
					if(select_no<18 && select_no<min(18,ipage_max) ){
						SND_Select();
					}
				}
			}
			if( select_no!=-1 && select_no<min(18,ipage_max) ){
				char	*type[30]={ "武器（剣）", "武器（槍）", "武器（ハンマー）", "武器（弓）", "武器（杖）", "防具（その他）",
									"防具（甲冑）","防具（軽装鎧）", "防具（鎖帷子）", "防具（服／ローブ）", "防具（その他）",
									"装飾品・手（指輪）", "装飾品・手（腕輪）", "装飾品・手（篭手）", "装飾品・手（その他）",
									"装飾品・他（靴）", "装飾品・他（首輪）", "装飾品・他（兜）", "装飾品・他（帽子）", "装飾品・他（その他）",
									"消耗品（回復薬）", "消耗品（MP回復薬）", "消耗品（？？）", "消耗品（万能薬）", "？？？", "無し" };
				int		ino = ItemBox[ ipage+select_no ].ino;

				GD_SetGraphDisp( gno+180, TRUE );
				GD_SetGraphDstRect( gno+180, 800-(24*12+24)-4, 48+24*select_no, 24*11+8, 24);

				SetGameWindowDisp( win_no+3, TRUE );
				wsprintf( buf, "　%-18s\n%4s", ItemParam[ino].name, type[ItemParam[ino].type] );
				SetGameWindowText( win_no+3, buf );

				GD_SetGraphDisp( gno+139, TRUE );
				GD_SetGraphSrcRect( gno+139, 768+ItemParam[ino].type%10*24, 344+ItemParam[ino].type/10*24, 22, 22 );

				j=0;
				for( i=0 ; i < 17 ; i++ ){
					char	*buf2=NULL;
					int		work1=0, skip=0;
					switch(i){
						case 0:	work1 = ItemParam[ino].hp;		if(work1) buf2="HP";	break;
						case 1:	work1 = ItemParam[ino].str;		if(work1) buf2="STR";	break;
						case 2:	work1 = ItemParam[ino].vit;		if(work1) buf2="VIT";	break;
						case 3:	work1 = ItemParam[ino].dex;		if(work1) buf2="DEX";	break;
						case 4:	work1 = ItemParam[ino].spd;		if(work1) buf2="AGI";	break;
						case 5:	work1 = ItemParam[ino].wiz;		if(work1) buf2="WIZ";	break;
						case 6:	work1 = ItemParam[ino].mnd;		if(work1) buf2="MND";	break;
						case 7:	work1 = ItemParam[ino].mov;		if(work1) buf2="MOV";	break;

						case 8:	work1 = ItemParam[ino].mp;		if(work1) buf2="MP";	break;
						case 9:	work1 = ItemParam[ino].atk;		if(work1) buf2="ATK";	break;
						case 10:work1 = ItemParam[ino].def;		if(work1) buf2="DEF";	break;

						case 14:work1 = ItemParam[ino].wsp;		if(work1) buf2="WSP";	break;
						case 15:work1 = ItemParam[ino].wsp_max;	if(work1) buf2="MAX"; 	break;

						case 16:work1 = ItemParam[ino].range/32;	if(work1) buf2="RNG";	break;
					}
					if(buf2){
						GD_SetGraphStrChange( gno+140+j, buf2 );
						GD_SetGraphDisp( gno+140+j, TRUE );

						GD_SetGraphDisp( gno+152+j, TRUE );
						if(i==16){
							GD_SetGraphDstMove( gno+152+j, 4+24+30+j%3*90, 600-24-24*8+24+j/3*16 );
						}else{
							GD_SetGraphDstMove( gno+152+j, 4+24+30+j%3*90, 600-24-24*8+24+j/3*16 );
						}

						if( work1>0 ){
							if( (i==14 && ITM_GetEquipType(ino)==0) || i==15 && ITM_GetEquipType(ino)==0 ){
								wsprintf( buf, "%d", work1, ItemParam[ino].wsp_max );
								GD_SetGraphStrChange( gno+152+j, buf );
								GD_SetGraphColor2( gno+152+j, 61,116,255 );
							}else if( i==16 ){
								wsprintf( buf, " %d", work1 );
								GD_SetGraphStrChange( gno+152+j, buf );
								GD_SetGraphColor2( gno+152+j, 61,116,255 );
							}else{
								wsprintf( buf, "+%d", work1 );
								GD_SetGraphStrChange( gno+152+j, buf );
								GD_SetGraphColor2( gno+152+j, 61,116,255 );
							}
						}else{
							wsprintf( buf, "%d", work1 );
							GD_SetGraphStrChange( gno+152+j, buf );
							GD_SetGraphColor2( gno+152+j, 255,56,56 );
						}
						j++;
					}
				}
				for( ; j < 12 ; j++ ){
					GD_SetGraphDisp( gno+140+j, FALSE );
					GD_SetGraphDisp( gno+152+j, FALSE );
				}
				SetGameWindowText( win_no, ItemParam[ino].help );


			}else{
				
				if(select_bak != select_no+ipage ){
					SetGameWindowText( win_no, "確認したいアイテムをを選んでください。" );
				}
				
				GD_SetGraphDisp( gno+180, FALSE );
				SetGameWindowDisp( win_no+3, FALSE );
				GD_SetGraphDisp( gno+139, FALSE );
				for( j=0 ; j < 12 ; j++ ){
					GD_SetGraphDisp( gno+140+j, FALSE );
					GD_SetGraphDisp( gno+152+j, FALSE );
				}
			}

			if(GameKey.cansel){
				GD_SetGraphDisp( gno+180, FALSE );
				count = 0;
				step = ITM_TERM;
			}
			select_bak = select_no+ipage;
			break;
	}

	return ret;
}


RECT2	StatusRect[17] = {
	{ 32, 147,    140, 16 },
	{ 32, 147+16, 140, 16 },

	{ 32, 147+16+22+16*0, 140, 16 },
	{ 32, 147+16+22+16*1, 140, 16 },
	{ 32, 147+16+22+16*2, 140, 16 },
	{ 32, 147+16+22+16*3, 140, 16 },
	{ 32, 147+16+22+16*4, 140, 16 },
	{ 32, 147+16+22+16*5, 140, 16 },

	{ 186, 147,    140, 14 },
	{ 186, 147+16, 140, 14 },

	{ 186, 147+16+22+16*0, 140, 16 },
	{ 186, 147+16+22+16*1, 140, 16 },
	{ 186, 147+16+22+16*2, 140, 16 },
	{ 186, 147+16+22+16*3, 140, 16 },
	{ 186, 147+16+22+16*4, 140, 16 },
	{ 186, 147+16+22+16*5, 140, 16 },

	{ 186, 147+16+40+16*5, 140, 16 },
};
char	*StatusHelp[17] = {
	"生命力です。この値が０になると戦闘不能になり、戦線を離脱します。戦闘後、回復します。",
	"移動力です。この値が高いと、側面攻撃などの戦術的な移動がしやすくなります。この値は職業固有のもので、成長しません。",

	"筋力です。近接武器攻撃力(ATK)に大きく影響し、武器の攻撃間隔(WSP)にも影響します。",
	"耐久力です。防御力(DEF)に大きく影響します。",
	"器用さです。主に武器命中率(HIT)攻撃力(特に弓)に影響し、攻撃間隔(WSP)回避力(AVD)魔法詠唱速度(SNG)にも影響します。",
	"俊敏さです。回避力に主に影響し、防御力(DEF)、攻撃間隔(WSP)命中力(HIT)にも影響します。",
	"賢さです。魔法攻撃力(MGK)に大きく影響し、魔法詠唱速度(SNG)魔法防御力(RGS)にも影響します。",
	"精神力です。魔法防御力(RGS)に大きく影響し、\n魔法詠唱速度(SNG)魔法攻撃力(MGK)MP回復速度にも影響します。",

	"魔力です。魔法、技、伝承歌の使用により消費します。待機時には、自動で少しずつ回復します。",
	"武器攻撃間隔です。この値が少ないほど、速く次の攻撃ができます。自分の強さを越えた武器を持つと遅くなってしまいます。",

	"武器攻撃力です。通常攻撃、及び技のダメージに影響します。",
	"物理防御力です。物理攻撃、技攻撃からのダメージを減衰します。",
	"命中力です。武器攻撃の命中率に影響します。",
	"回避力です。武器攻撃の回避率に影響します。攻撃相手の命中力との比較で命中率が決まります。",
	"魔法攻撃力です。攻撃魔法のダメージに影響します。",
	"魔法防御力であり、同時に魔法回復力です。攻撃魔法ダメージを減衰し、回復魔法を強化します。",

	"射程距離です。武器固有の数値です。この値が大きいほど遠くへ届きます。",
};


int	WLD_Butai( int win_no, int win_bno, int gno, int bno, int spno, int layer, int mouse )
{
	enum{
		BTI_INIT,
		BTI_TERM,

		BTI_FADE_IN,
		BTI_FADE_OUT,
		BTI_CHAR_COMMAND,
		BTI_CHAR_EQUIP,
		BTI_CHAR_EQUIP_ITEM,

		BTI_MIAN,
		BTI_COMMAND,
	};
	static int	step=BTI_INIT;
	static int	count=0;
	int			ret=0;
	int			i,j,k;
	char		buf[512];
	char		buf2[64];
	static int	ppage=0, ppage_max;
	static int	ipage=0, ipage_max, ipage_max2;
	static int	mpage=0, mpage_max;

	static int	pt_no[32];
	static int	cno;

	static int	magic_no[18];
	static int	item_no[18];
	static int	eq_type=0;
	static int	select_bak=-1;
	int			select_no = MUS_GetMouseNo( -1 );
	int			redraw=0;

	switch(step){
		case BTI_INIT:
			GD_LoadTexture( bno+2, "digit002.tga", ALPHA_4444 );

			MUS_SetMouseLayer( mouse );


			for( i=0 ; i < 4 ; i++ ){
				GD_SetGraph( gno+155+i, win_bno, layer+2, FALSE );
				GD_SetGraphRect( gno+155+i, 800-350-4+128+24*i, 24, 768+24*i, 416, 22, 22 );
				GD_SetGraphCenter( gno+155+i, 800-350-4+128+24*i+11, 24+11 );

				MUS_SetMouseRect( mouse+1, 40+i, 800-350-4+128+24*i, 24, 22, 22, ON );
			}

			MUS_SetMouseRect( mouse+1, 25,   0, 72, 32, 48, ON );
			MUS_SetMouseRect( mouse+1, 26, 320, 72, 32, 48, ON );



			for( i=0 ; i < 4 ; i++ ){
				MUS_SetMouseRect( mouse+1, 21+i, 60, 302+24*i, 24*9, 24, ON );
			}

			for(j=0,i=0;i<32;i++){
				if(CharParam[i].flag && CharParam[i].unit==0){


					pt_no[j++]=i;
				}
			}
			ppage=0;
			ppage_max=j;

			SetGameWindow( win_no+1, gno+80, win_bno, layer, TRUE, 1, WINDOW_COLOR_SYS );
			SetGameWindowRect( win_no+1, 800-360-4, 24, 360, 480 );
			SetGameWindowText( win_no+1, "キャラクター", bno+10 );

			WLD_SetRightBar18( win_bno, gno+92, layer+1, mouse );

			for( i=0 ; i < min(18,ppage_max) ; i++ ){
				MUS_SetMouseRect( mouse, i, 800-360-4+24, 24+24+24*i, 24*12, 24, ON );
			}


			SetGameWindow( win_no+4, gno+100, win_bno, layer, FALSE, 2, WINDOW_COLOR_SYS );
			SetGameWindowRect( win_no+4, 800-(24*12+48)-4, 24, 24*12+48,  24*18+48 );
			SetGameWindowText( win_no+4, "アイテム", bno+11, 24, 0  );

			SetGameWindow( win_no+5, gno+105, win_bno, layer, FALSE, -1, WINDOW_COLOR_SYS );
			SetGameWindowRect( win_no+5, 800-(24*14+48)-4, 24, 24*14+48,  24*18+48 );
			SetGameWindowText( win_no+5, "アイテム", bno+12  );

			for( i=0 ; i < 18 ; i++ ){
				GD_SetGraphDigit( gno+112+i, bno+2, layer+1, FALSE, 0, 2 );
				GD_SetGraphRect( gno+112+i, 800-(24*11+48)+24*10-4, 24+24+24*i+8, 0, 16, 10, 16 );
				GD_SetGraphBlendParam( gno+112+i, BLEND_ALPHA );
				GD_SetGraphScaleParam( gno+112+i, SCALE_HIGHT );
				GD_SetGraphColor( gno+112+i, 255, 91,55,21 );

				GD_SetGraph( gno+130+i, win_bno, layer+1, FALSE );
				GD_SetGraphRect( gno+130+i, 800-(24*12+48)+24-4, 24+24+24*i, 768, 344, 22, 22 );
				GD_SetGraphBlendParam( gno+130+i, BLEND_ALPHA );
				GD_SetGraphScaleParam( gno+130+i, SCALE_HIGHT );

				MUS_SetMouseRect( mouse+1, i, 800-(24*12+48)+24-4, 24+24+24*i, 24*11, 24, ON );
			}
			ipage=0;
			MUS_SetMouseRect( mouse+1, 18, 700,  24-8, 44,  32, ON );
			MUS_SetMouseRect( mouse+1, 19, 700, 472+8, 44,  32, ON );
			MUS_SetMouseRect( mouse+1, 20, 756,    48, 16, 432, ON );

			for( i=0 ; i < 17 ; i++ ){
				MUS_SetMouseRect( mouse+1, 45+i, StatusRect[i].x, StatusRect[i].y, StatusRect[i].w, StatusRect[i].h, ON );
			}

			GD_SetGraphx9( gno+150, win_bno, layer+1, FALSE );	
			GD_SetGraphSrcRect( gno+150, 0, 64, 72, 24 );
			GD_SetGraphx9( gno+151, win_bno, layer+1, FALSE );	
			GD_SetGraphSrcRect( gno+151, 0, 64, 72, 24 );

			count=0;
			select_bak=-1;
			step=BTI_MIAN;
			break;
		case BTI_TERM:
WLD_ResetRightBar18( gno+92 );
			for(i=10;i<160;i++)	{	GD_ResetGraph( gno+i );					}
			for(i=1;i<12;i++)	{	GD_ReleaseTexture( bno+i );				}
			for(i=0;i<32;i++)	{	GD_ReleaseSprite( spno+i );				}
			for(i=0;i<4;i++)	{	MUS_ResetMouseRect_Layer( mouse+i );	}
			for(i=1;i<5;i++)	{	ResetGameWindow( win_no+i );			}
			step=BTI_INIT;
			ret=1;
			break;
		case BTI_FADE_IN:
			break;
		case BTI_FADE_OUT:
			break;
		case BTI_CHAR_COMMAND:

			break;
		case BTI_CHAR_EQUIP_ITEM:
			redraw = WLD_DragRightBar18( &ipage, &ipage_max2, select_no );



			if(select_no==25&&GameKey.click){	cno = LOOP( cno-1, ppage_max );	count=0;	ipage=0; redraw=1;	SND_Ok();}
			if(select_no==26&&GameKey.click){	cno = LOOP( cno+1, ppage_max );	count=0;	ipage=0; redraw=1;	SND_Ok();}

			if(GameKey.pup || GameKey.pdown || count==0 || redraw ){
				j=0;
				for( i=0 ; i < ipage_max ; i++ ){
					if( ItemBox[ i ].ino){
						if( CharEquipCheck( pt_no[cno], ItemBox[i].ino, eq_type ) ){
							j++;
						}
					}
				}
				ipage_max2 = j;
				ipage = LIM( ipage, 0, max(0,ipage_max2-18) );
				
				for( i=0 ; i < 18 ; i++ ) item_no[i]=-1;
				buf[0] = '\0';
				j=0;
				for( i=0 ; i < ipage_max ; i++ ){
					if( ItemBox[ i ].ino){
						if( CharEquipCheck( pt_no[cno], ItemBox[i].ino, eq_type ) ){
							if( ipage <= j  && j < ipage+18 ){
								_mbscat( (BYTE*)buf, (BYTE*)ItemParam[ItemBox[i].ino].name );
								_mbscat( (BYTE*)buf, (BYTE*)"\n" );
								item_no[j-ipage] = i;

								GD_SetGraphDigitNo( gno+112+j-ipage, ItemBox[i].amount );
								GD_SetGraphDisp( gno+112+j-ipage, TRUE );


								k = ItemParam[ ItemBox[i].ino ].type;
								GD_SetGraphDisp( gno+130+j-ipage, TRUE );
								GD_SetGraphSrcRect( gno+130+j-ipage, 768+(k%10)*24, 344+(k/10)*24, 22, 22 );

							}
							j++;
						}
					}
				}
				ipage_max2 = j;
				ipage = LIM( ipage, 0, max(0,ipage_max2-18) );
				for( i=j ; i < ipage+18 ; i++ ){
					GD_SetGraphDisp( gno+112+i-ipage, FALSE );
					GD_SetGraphDisp( gno+130+i-ipage, FALSE );
				}
				SetGameWindowText( win_no+4, buf );
			}
			if(redraw || count==0 ){
				SetCharParamWindow( win_no, win_bno, gno, bno, spno, layer, pt_no[cno], TRUE );
			}
			RenweCharParamWindow( gno, 25==select_no, 26==select_no, GameKey.click );
			GD_SetGraphFade( gno+35, 128+STD_LimitLoop(count,48) );

			SetGameWindowDisp( win_no+4, TRUE );
WLD_RenewRightBar18( gno+92, ipage, ipage_max2, 18==select_no, 19==select_no, GameKey.click );
			count++;


			if( select_no!=-1 && select_no<18 ){
				if( item_no[select_no]!=-1 ){
					int	ino = ItemBox[ item_no[select_no] ].ino;

					if(select_bak != select_no+ppage){
						SetGameWindowText( win_no, ItemParam[ino].help );
						SND_Select();
					}
					GD_SetGraphDisp( gno+150, TRUE );
					GD_SetGraphDstRect( gno+150, 800-(24*12+24)-8, 48+24*select_no, 24*11+12, 24);

					if( CharEquipCheck( pt_no[cno], ino, eq_type ) ){
						SetCharParamWindowEquip( win_no, gno, bno, spno, layer, pt_no[cno], eq_type, ino );
						if(GameKey.click){
			
							SetCharEquip( pt_no[cno], eq_type, ino );




							for( j=0 ; j < ITEM_MAX ; j++ ){ if(ItemBox[j].ino==0)break;	}
							ipage_max=j;
							count=0;

							SND_Ok();
						}
					}else{
						SetCharParamWindowEquip( win_no, gno, bno, spno, layer, pt_no[cno], eq_type, -1 );
					}
				}else{
					GD_SetGraphDisp( gno+150, FALSE );
					SetCharParamWindowEquip( win_no, gno, bno, spno, layer, pt_no[cno], eq_type, -1 );
				}
			}else{
				GD_SetGraphDisp( gno+150, FALSE );
				SetCharParamWindowEquip( win_no, gno, bno, spno, layer, pt_no[cno], eq_type, -1 );
			}

			for( i=0 ; i < 4 ; i++ ){
				GD_SetGraphDisp( gno+155+i, TRUE );
				GD_SetGraphZoom( gno+155+i, 1.0f );
				GD_SetGraphLayer( gno+155+i, layer+2 );
				if(eq_type == i)	GD_SetGraphColor2( gno+155+i, 128,128,192 );
				else				GD_SetGraphColor2( gno+155+i, 128,128,128 );
			}
			if( 45<=select_no){
				if(select_bak != select_no+ppage){
					SetGameWindowText( win_no, StatusHelp[select_no-45] );
					SND_Select();
				}
			}
			if( 40<=select_no && select_no<=43){
				GD_SetGraphZoom( gno+155+select_no-40, 2.0f );
				GD_SetGraphLayer( gno+155+select_no-40, layer+3 );
				if( GameKey.click){
					eq_type = select_no-40;

					j=0;
					for( i=0 ; i < ipage_max ; i++ ){
						if( ItemBox[ i ].ino){
							if( CharEquipCheck( pt_no[cno], ItemBox[i].ino, eq_type ) ){
								j++;
							}
						}
					}
					ipage_max2 = j;
					ipage = min( ipage, max(0,ipage_max2-18) );
					count=0;
					SND_Ok();
				}
			}
			if( 21<=select_no && select_no<=24){
				if(select_bak != select_no+ppage){
					switch(select_no-21){
						case 0:	SetGameWindowText( win_no, ItemParam[ CharParam[ pt_no[cno] ].buki ].help );	break;
						case 1:	SetGameWindowText( win_no, ItemParam[ CharParam[ pt_no[cno] ].yoroi ].help );	break;
						case 2:	SetGameWindowText( win_no, ItemParam[ CharParam[ pt_no[cno] ].accessary1 ].help );	break;
						case 3:	SetGameWindowText( win_no, ItemParam[ CharParam[ pt_no[cno] ].accessary2 ].help );	break;
					}

					SND_Select();
				}
				GD_SetGraphDisp( gno+151, TRUE );
				GD_SetGraphDstRect( gno+151, 56, 303+24*(select_no-21), 24*10, 24 );
				if(GameKey.cansel){
					SetCharEquip( pt_no[cno], select_no-21, 0 );
					for( i=0 ; i < ITEM_MAX ; i++ ){ if(ItemBox[i].ino==0)break;	}
					ipage_max=i;

					count=0;
					SND_Ng();
				}
				if( GameKey.click){
					eq_type = select_no-21;

					j=0;
					for( i=0 ; i < ipage_max ; i++ ){
						if( ItemBox[ i ].ino){
							if( CharEquipCheck( pt_no[cno], ItemBox[i].ino, eq_type ) ){
								j++;
							}
						}
					}
					ipage_max2 = j;
					ipage = min( ipage, max(0,ipage_max2-18) );
					count=0;
					SND_Ok();
				}
			}else{
				if( (45>select_no && select_no>=18) || select_no==-1){
					if(select_bak != select_no+ppage){
						SetGameWindowText( win_no, "装備するアイテムを選んでください。" );
						if(select_no!=-1 && select_no!=20){
							SND_Select();
						}
					}
				}
				GD_SetGraphDisp( gno+151, FALSE );
			}
			if(GameKey.cansel && count){
				SetGameWindowDisp( win_no+4, FALSE );
				for( i=0 ; i < 4 ; i++ ){
					GD_SetGraphDisp( gno+155+i, FALSE );
				}
				for( i=0 ; i < 18 ; i++ ){
					GD_SetGraphDisp( gno+112+i, FALSE );
					GD_SetGraphDisp( gno+130+i, FALSE );
				}
WLD_SetRightBar18Disp( gno+92, FALSE );

				count=0;
				mpage=0;
				step = BTI_CHAR_EQUIP;
				SND_Cansel();
				GD_SetGraphDisp( gno+150, FALSE );
			}
			select_bak = select_no+ppage;
			break;
		case BTI_CHAR_EQUIP:
			SetGameWindowDisp( win_no+5, TRUE );

			redraw = WLD_DragRightBar18( &mpage, &mpage_max, select_no );

			if(GameKey.pup  || (select_no==25&&GameKey.click)){	cno = LOOP( cno-1, ppage_max );	redraw=1;	SND_Ok();mpage=0;}
			if(GameKey.pdown|| (select_no==26&&GameKey.click)){	cno = LOOP( cno+1, ppage_max );	redraw=1;	SND_Ok();mpage=0;}
				
			if(redraw|| count==0 ){
				mpage_max=0;
				for( i=0 ; i < 18 ; i++ ){
					magic_no[i]=-1;
				}
				for(i=0;i<MAGIC_CHAR_MAX;i++){
					if(CharParam[pt_no[cno]].skill[i]==-1) break;
					if(mpage<=i && i<mpage+18)
						magic_no[i-mpage] = CharParam[pt_no[cno]].skill[i];
				}
				mpage_max+=i;
				for(i=0;i<MAGIC_CHAR_MAX;i++){
					if(CharParam[pt_no[cno]].magic[i]==-1)break;
					if(mpage<=i+mpage_max && i+mpage_max<mpage+18)
						magic_no[i+mpage_max-mpage] = CharParam[pt_no[cno]].magic[i];
				}
				mpage_max+=i;

				buf[0]='\0';
				for( i=0 ; i < 18 ; i++ ){
					if(magic_no[i]==-1)break;
					wsprintf( buf2, "%-20s MP%3d\n", MagicParam[magic_no[i]].name, MagicParam[magic_no[i]].mp );
					_mbscat( (BYTE*)buf, (BYTE*)buf2 );
				}
				SetGameWindowText( win_no+5, buf );
				
				SetCharParamWindow( win_no, win_bno, gno, bno, spno, layer, pt_no[cno], TRUE );
			}
			RenweCharParamWindow( gno, 25==select_no, 26==select_no, GameKey.click );
WLD_RenewRightBar18( gno+92, mpage, mpage_max, 18==select_no, 19==select_no, GameKey.click );

			GD_SetGraphFade( gno+35, 128+STD_LimitLoop(count,48) );

			GD_SetGraphDisp( gno+21, TRUE );
			GD_SetGraphDisp( gno+22, TRUE );

			count++;
			if( 45<=select_no){
				if(select_bak != select_no+ppage){
					SetGameWindowText( win_no, StatusHelp[select_no-45] );
					SND_Select();
				}
			}
			if( 21<=select_no && select_no<=24 ){
				if(select_bak != select_no+ppage){
					switch(select_no-21){
						case 0:	SetGameWindowText( win_no, ItemParam[ CharParam[ pt_no[cno] ].buki ].help );	break;
						case 1:	SetGameWindowText( win_no, ItemParam[ CharParam[ pt_no[cno] ].yoroi ].help );	break;
						case 2:	SetGameWindowText( win_no, ItemParam[ CharParam[ pt_no[cno] ].accessary1 ].help );	break;
						case 3:	SetGameWindowText( win_no, ItemParam[ CharParam[ pt_no[cno] ].accessary2 ].help );	break;
					}

					SND_Select();
				}
				GD_SetGraphDisp( gno+151, TRUE );
				GD_SetGraphDstRect( gno+151, 56, 303+24*(select_no-21), 24*10, 24 );

				if(GameKey.cansel){
					SetCharEquip( pt_no[cno], select_no-21, 0 );
					for( i=0 ; i < ITEM_MAX ; i++ ){ if(ItemBox[i].ino==0)break;	}
					ipage_max=i;

					count=0;
					SND_Ng();
				}
				if(GameKey.click){
					eq_type = select_no-21;

					for( i=0 ; i < ITEM_MAX ; i++ ){ if(ItemBox[i].ino==0)break;	}
					ipage_max=i;
					j=0;
					for( i=0 ; i < ipage_max ; i++ ){
						if( ItemBox[ i ].ino){
							if( CharEquipCheck( pt_no[cno], ItemBox[i].ino, eq_type ) ){
								j++;
							}
						}
					}
					ipage_max2 = j;
					ipage = LIM( ipage, 0, max(0,ipage_max2-18) );

					SetGameWindowDisp( win_no+5, FALSE );
					count=0;
					step = BTI_CHAR_EQUIP_ITEM;
					SND_Ok();
WLD_SetRightBar18Disp( gno+92, FALSE );
				}
			}else{

				int sc_cnt;

				if(mpage_max<18){
					sc_cnt=21;
				}else{
					sc_cnt=18;
				}

				if(GameKey.click && (select_no<sc_cnt || select_no>=40)){
						for( i=0 ; i < ITEM_MAX ; i++ ){ if(ItemBox[i].ino==0)break;	}
						ipage_max=i;
						j=0;
						for( i=0 ; i < ipage_max ; i++ ){
							if( ItemBox[ i ].ino){
								if( CharEquipCheck( pt_no[cno], ItemBox[i].ino, eq_type ) ){
									j++;
								}
							}
						}
						ipage_max2 = j;
						ipage = LIM( ipage, 0, max(0,ipage_max2-18) );

						SetGameWindowDisp( win_no+5, FALSE );
						count=0;
						step = BTI_CHAR_EQUIP_ITEM;
						SND_Ok();
	WLD_SetRightBar18Disp( gno+92, FALSE );
				}
				if(select_bak != select_no+ppage){
					if( 45>select_no ){
						SetGameWindowText( win_no, "使える魔法と技です。\n装備品をクリックすると装備画面に移ります。" );
						if( 25==select_no || select_no==26 ){
							SND_Select();
						}
					}
				}
				GD_SetGraphDisp( gno+151, FALSE );
			}
			if(GameKey.cansel && count){


				SetGameWindowDisp( win_no+5, FALSE );
				MUS_SetMouseLayer( mouse );
				count=0;
				step = BTI_MIAN;
				select_bak=-1;
				SND_Cansel();
				GD_SetGraphDisp( gno+151, FALSE );
WLD_SetRightBar18Disp( gno+92, FALSE );
			}
			select_bak = select_no+ppage;
			break;

		case BTI_MIAN:
			redraw = WLD_DragRightBar18( &ppage, &ppage_max, select_no );
			if( redraw || count==0 ){
				buf[0] = '\0';
				for( i=0 ; i < min(18,ppage_max-ppage) ; i++ ){
					wsprintf( buf2, "%2d.%-14s Lv.%3d\n", pt_no[ppage+i], CharParam[ pt_no[ppage+i] ].name, CharParam[ pt_no[ppage+i] ].lv );
					_mbscat( (BYTE*)buf, (BYTE*)buf2 );
				}
				SetGameWindowText( win_no+1, buf );

				SetGameWindowText( win_no, "ステータスの確認、あるいは装備変更をするキャラクターを選んでください。" );
			}
			SetGameWindowDisp( win_no+1, TRUE );

WLD_RenewRightBar18( gno+92, ppage, ppage_max, 18==select_no, 19==select_no, GameKey.click );
			count++;

			if( select_no!=-1 && select_no<min(18,ppage_max) ){
				if(select_bak != select_no+ppage){
					SND_Select();
				}
				GD_SetGraphDisp( gno+150, TRUE );
				GD_SetGraphDstRect( gno+150, 800-(24*13+24)-4, 48+24*select_no, 24*12+8, 24);
				if(GameKey.click){
					cno  = ppage+select_no;

					SetGameWindowDisp( win_no+1, FALSE );
					for( i=92 ; i < 100 ; i++ ){
						GD_SetGraphDisp( gno+i, FALSE );
					}

					SetGameWindowText( win_no, "使える魔法と技です。\n装備品をクリックすると装備画面に移ります。" );
					GD_SetGraphDisp( gno+150, FALSE );
					MUS_SetMouseLayer( mouse+1 );
					mpage=0;
					count=0;
					step = BTI_CHAR_EQUIP;
	
	
					SND_Ok();
				}
			}else{
				GD_SetGraphDisp( gno+150, FALSE );
			}
			if(GameKey.cansel){
				SND_Cansel();
				GD_SetGraphDisp( gno+150, FALSE );
				step=BTI_TERM;
			}

			if( select_no!=-1 && select_no<min(18,ppage_max) ){
				if(select_bak!=select_no+ppage){
					SetCharParamWindow( win_no, win_bno, gno, bno, spno, layer, pt_no[ppage+select_no], FALSE );
				}
			}else{
				ResetCharParamWindow( win_no, win_bno, gno, bno, spno );
			}
			select_bak = select_no+ppage;
			break;
		case BTI_COMMAND:
			break;
	}

	return ret;
}

#define WORLD_OBJECT_MAX	21
typedef struct{
	char	*scr;
	BYTE	flag;
	char	*name;
}WORLD_OBJECT;




WORLD_OBJECT	WorldObj[WORLD_OBJECT_MAX]={
	{ "map_00.sdt",     30, "『ゲール村』　ゲール族の暮らしていた村。" },										
	{ "map_01.sdt",     31, "『ヴァスティタス』　荒れ地です。" },												
	{ "map_02.sdt",     32, "『太古の聖堂』　現在は城として使用されている、太古の神殿。納骨堂があります。" },	
	{ "map_03.sdt",     33, "『ポルトス・マグナス』　帝国の軍港。「大きな港」という意味。" },					

	{ "map_04.sdt",     34, "『リートゥス』　浜辺。豊かな海の恵みが得られる。" },													
	{ "sys_kyoten.sdt", 35, "『城塞都市アヴァロン』　ゲール族の新たな本拠地。アロウンの居城であり、昔から妖精族も暮らしている。" },	
	{ "map_06.sdt",     36, "『アロウン稜』　巨石の墓場。中に葬られている者については、長く忘れさられていた。" },					
	{ "map_07.sdt",     37, "『シルウァヌス』　古森。豊かな森の恵みが得られる。" },													
	{ "map_08.sdt",     38, "『竜族の遺跡』　岩山のように見えるが、かつては竜族の巨大な神殿だった。" },								
	{ "map_09.sdt",     39, "『交易都市ロンディニウム』　帝国の植民都市として開かれた。古くより、交易によって栄えている。" },		

	{ "map_10.sdt",     40, "『コーラス山』　別名「北風山」。ふもとに洞窟がある。" },							
	{ "map_11.sdt",     41, "『ゴルセズ・アロウン』　アロウンの玉座と呼ばれる遺跡。先の大戦の戦勝記念碑。" },	
	{ "map_12.sdt",     42, "『城塞都市パラディウム』　アヴァロンと同時期に建造された、かつての管制局。先の大戦で破壊され、長く廃墟だった。" },		
	{ "map_13.sdt",     43, "『カストゥルム』　帝国の前線基地。" },						
	{ "map_14.sdt",     44, "『カストゥルム』　帝国の前線基地。" },						
	{ "map_15.sdt",     45, "『カストゥルム』　帝国の前線基地。" },						

	{ "map_16.sdt",     46, "『フリー平原』　レベルアップ・アイテム収集に励もう。" },	
	{ "map_17.sdt",     47, "『フリー平原』　レベルアップ・アイテム収集に励もう。" },	
	{ "map_18.sdt",     48, "『フリー平原』　レベルアップ・アイテム収集に励もう。" },	
	{ "map_19.sdt",     49, "『フリー雪原』　レベルアップ・アイテム収集に励もう。" },	

	{ "map_20.sdt",     50, "『リーフの塔』　突如出現した巨大な塔。" },					
};


RECT2	WorldObjyectRect[WORLD_OBJECT_MAX]={
	{229,  56,  44,  26 },
	{198,  98,  44,  26 },
	{127, 121,  34,  26 },
	{224, 142,  60,  48 },

	{263, 340,  70,  36 },
	{323, 379,  70,  44 },
	{384, 352,  52,  32 },
	{426, 306,  48,  34 },
	{385, 236,  62,  52 },
	{369, 487,  76,  65 },

	{391, 187,  82,  36 },
	{387,  73,  54,  32 },
	{558,  76+50,  74, 50 },
	{385-20, 457-14, 38,  26 },
	{433-22, 448-14, 38,  27 },
	{489-21, 462-13, 41,  25 },

	{386-25, 327-15,  49,  29 },
	{ 77-28, 223-13,  54,  25 },
	{330-27, 486-17,  52,  32 },
	{504-29, 149-16,  57,  31 },


	{ 474+20, 4, 185-50, 119 },
};


POINT	WorldObjyectPoint[WORLD_OBJECT_MAX]={
	{252,  79},
	{221, 117},
	{143, 140},
	{247, 176},

	{296, 347},
	{361, 390},
	{406, 365},
	{455, 331},
	{425, 271},
	{413, 529},
	{431, 215},
	{420, 101},
	{597, 133},
	{385, 457},
	{433, 448},
	{489, 462},

	{386, 327},
	{ 77, 223},
	{330, 486},
	{504, 149},

	{474+185/2, 4+119/2},
};

int	NextWLDStep = SYS_BATTLE;

int	WLD_System(void)
{
	enum{
		WLD_INIT,
		WLD_TERM,

		WLD_FADE_IN,
		WLD_FADE_OUT,

		WLD_SAVE_LOAD,
		WLD_ITEM_LIST,
		WLD_BUTAI,
		WLD_MIAN,
		WLD_COMMAND,
	};
	static int	step=WLD_INIT;
	static int	count=0;
	static int	cx,cy;
	static int	leaf_count=0;

	float		rate;
	static int	select_bak=-1;
	int			select_no = MUS_GetMouseNo( -1 );

	int			ret=-1;
	int	i,j;

	int	mx,my;

	switch(step){
		case WLD_INIT:
			AVG_PlayBGM( 30, 0, TRUE, 255, FALSE );

			AllPartyHeal();
			GD_LoadTexture( WLD_BMP_SYSTEM, "btsys_0000.tga", ALPHA_4444|DITHER_OFF );
			GD_LoadTexture( WLD_BMP_WORLD+0, "world_00.tga", ALPHA_1555 );
			GD_LoadTexture( WLD_BMP_WORLD+1, "world_01.tga", ALPHA_1555 );
			GD_LoadSprite( WLD_SPR_WORLD, "world_02.ani" );

			cx=400;
			cy=300;

			MUS_SetMouseLayer( 0 );

			GD_SetGraph( WLD_GRP_WORLD+0, WLD_BMP_WORLD+0, WLD_LAY_WORLD+1, FALSE );
			GD_SetGraphSrcRect( WLD_GRP_WORLD+0, 0, 0, 800, 600 );
			GD_SetGraphScaleParam( WLD_GRP_WORLD+0, SCALE_HIGHT );
			GD_SetGraphCenter( WLD_GRP_WORLD+0, cx, cy );

			GD_SetGraph( WLD_GRP_WORLD+1, WLD_BMP_WORLD+0, WLD_LAY_WORLD+0, FALSE );
			GD_SetGraphSrcRect( WLD_GRP_WORLD+1, 0, 0, 800, 600 );
			GD_SetGraphScaleParam( WLD_GRP_WORLD+1, SCALE_HIGHT );
			GD_SetGraphCenter( WLD_GRP_WORLD+1, cx, cy );

			for(i=0;i<WORLD_OBJECT_MAX;i++){
				if( ESC_GetFlag( WorldObj[i].flag) ){
					MUS_SetMouseRect( 0, i, WorldObjyectRect[i].x, WorldObjyectRect[i].y, WorldObjyectRect[i].w, WorldObjyectRect[i].h, ON );

					GD_SetSprite( WLD_GRP_WORLD+2+i, WLD_SPR_WORLD, i, TRUE, WLD_LAY_WORLD+3, FALSE );
					GD_SetGraphDstMove( WLD_GRP_WORLD+2+i, WorldObjyectPoint[i].x, WorldObjyectPoint[i].y );
					GD_SetGraphCenter( WLD_GRP_WORLD+2+i, cx, cy );

					if(ESC_GetFlag(62)==WorldObj[i].flag){
						GD_SetGraph( WLD_GRP_WORLD+2+WORLD_OBJECT_MAX, WLD_BMP_WORLD+1, WLD_LAY_WORLD+4, FALSE );
						GD_SetGraphDstMove( WLD_GRP_WORLD+2+WORLD_OBJECT_MAX, WorldObjyectRect[i].x+(WorldObjyectRect[i].w-32)/2, WorldObjyectRect[i].y+(WorldObjyectRect[i].h-32)/2 );
						GD_SetGraphCenter( WLD_GRP_WORLD+2+WORLD_OBJECT_MAX, cx, cy );
					}
				}
			}
			GD_SetGraphAlpha( WLD_GRP_WORLD+2+20, 0 );
			

			SetGameWindow( 0, WLD_GRP_SYSTEM, WLD_BMP_SYSTEM, WLD_LAY_SYSTEM, FALSE, -1, WINDOW_COLOR_SYS );
			SetGameWindowRect( 0, 800-(24*8+48)-16, 16, 24*8+48, 24*4+48  );
			SetGameWindowText( 0, "アイテム\n部隊編成\n記録\n戻る", WLD_BMP_SYSTEM+1, 0,0, 30 );
			for(i=0;i<4;i++){
				MUS_SetMouseRect( 1, i, 800-16-(24*8+48)+24, 16+24+24*i, 24*8, 24, ON );
			}



			SetGameWindow( 1, WLD_GRP_SYSTEM+10, WLD_BMP_SYSTEM, WLD_LAY_SYSTEM, FALSE, 6, WINDOW_COLOR_SYS );
			SetGameWindowRect( 1, 16, 600-(16+24*2+48), 24*30+48, 24*2+48  );
			SetGameWindowText( 1, "行き先選んでください。\nアイコンをクリックすると、イベントや戦闘が起こります。", WLD_BMP_SYSTEM+2, 0,0, 30 );

			GD_SetGraphx9( WLD_GRP_SYSTEM+5, WLD_BMP_SYSTEM, WLD_LAY_SYSTEM+1, FALSE );	
			GD_SetGraphSrcRect( WLD_GRP_SYSTEM+5, 0, 64, 72, 24 );


			NextWLDStep = SYS_BATTLE;
			leaf_count=0;
			count=0;
			step = WLD_FADE_IN;
			select_bak=-1;
			break;
		case WLD_TERM:
			for(i=0;i<WLD_GRP_DEBUG;i++){	GD_ResetGraph( i );			}
			for(i=0;i<WLD_BMP_DEBUG;i++){	GD_ReleaseTexture( i );		}
			for(i=0;i<WLD_SPR_DEBUG;i++){	GD_ReleaseSprite( i );		}
			MUS_ResetMouseRect_All();
			ResetGameWindow_All();

			ret = NextWLDStep;
			appData.draw_flag=1;
			appData.flash=1;

			leaf_count=0;
			step = WLD_INIT;
			count=0;
			break;
		case WLD_FADE_IN:
			rate = pow(30-count,2) / pow( 30.0f, 2 );

			GD_SetGraphDisp( WLD_GRP_WORLD+0, ON );
			GD_SetGraphDstRect( WLD_GRP_WORLD+0, 0+(0-cx)*4*rate, 0+(0-cy)*2*rate, 800+3200*rate, 600+1200*rate );

			GD_SetGraphAlpha( WLD_GRP_WORLD+0, count*255/30 );
			GD_SetGraphFade( WLD_GRP_WORLD+0, count*128/30 );
			GD_SetGraphRoll( WLD_GRP_WORLD+0, -rate*360/4 );


			for(i=0;i<WORLD_OBJECT_MAX;i++){
				GD_SetGraphDisp( WLD_GRP_WORLD+2+i, ON );
				GD_SetGraphAlpha( WLD_GRP_WORLD+2+i, count*255/30 );
				GD_SetGraphFade( WLD_GRP_WORLD+2+i, count*128/30 );
				GD_SetGraphRoll( WLD_GRP_WORLD+2+i, -rate*360/4 );
				GD_SetGraphZoom( WLD_GRP_WORLD+2+i, 1+rate*4 );
			}
			GD_SetGraphAlpha( WLD_GRP_WORLD+2+20, 0 );

			GD_SetGraphDisp( WLD_GRP_WORLD+2+WORLD_OBJECT_MAX, ON );
			GD_SetGraphAlpha( WLD_GRP_WORLD+2+WORLD_OBJECT_MAX, count*255/30 );
			GD_SetGraphFade( WLD_GRP_WORLD+2+WORLD_OBJECT_MAX, count*128/30 );
			GD_SetGraphRoll( WLD_GRP_WORLD+2+WORLD_OBJECT_MAX, -rate*360/4 );
			GD_SetGraphZoom( WLD_GRP_WORLD+2+WORLD_OBJECT_MAX, 1+rate*4 );

			rate=1-rate;
			GD_SetGraphDisp( WLD_GRP_WORLD+1, ON );
			GD_SetGraphDstRect( WLD_GRP_WORLD+1, -1600*rate, -600*rate, 800+3200*rate, 600+1200*rate );

			GD_SetGraphAlpha( WLD_GRP_WORLD+1, count*255/30 );




			count++;
			if(count>30){
				GD_ResetGraph( WLD_GRP_WORLD+1 );
				count=0;
				step = WLD_MIAN;
			}
			break;
		case WLD_FADE_OUT:

			SetGameWindowAlpha( 0, 255-count*255/28 );
			SetGameWindowAlpha( 1, 255-count*255/28 );

			SetGameWindowDisp( 1, 0 );
			
			rate = pow(count,2) / pow( 30.0f, 2 );
			
			GD_SetGraphDisp( WLD_GRP_WORLD+0, ON );
			GD_SetGraphDstRect( WLD_GRP_WORLD+0, 0+(0-cx)*4*rate, 0+(0-cy)*2*rate, 800+3200*rate, 600+1200*rate );

			GD_SetGraphAlpha( WLD_GRP_WORLD+0, 32 );
			GD_SetGraphFade( WLD_GRP_WORLD+0, 128-count*128/25 );
			GD_SetGraphRoll( WLD_GRP_WORLD+0, rate*360/4 );


			for(i=0;i<WORLD_OBJECT_MAX;i++){
				GD_SetGraphDisp( WLD_GRP_WORLD+2+i, ON );
				GD_SetGraphAlpha( WLD_GRP_WORLD+2+i, 32 );
				GD_SetGraphFade( WLD_GRP_WORLD+2+i, 128-count*128/25 );
				GD_SetGraphRoll( WLD_GRP_WORLD+2+i, rate*360/4 );
				GD_SetGraphZoom( WLD_GRP_WORLD+2+i, 1+rate*4 );
			}
			if(leaf_count){
				GD_SetGraphAlpha( WLD_GRP_WORLD+2+20, 32 );
			}else{
				GD_SetGraphAlpha( WLD_GRP_WORLD+2+20, 0 );
			}

			GD_SetGraphDisp( WLD_GRP_WORLD+2+WORLD_OBJECT_MAX, ON );
			GD_SetGraphAlpha( WLD_GRP_WORLD+2+WORLD_OBJECT_MAX, 32 );
			GD_SetGraphFade( WLD_GRP_WORLD+2+WORLD_OBJECT_MAX, 128-count*128/25 );
			GD_SetGraphRoll( WLD_GRP_WORLD+2+WORLD_OBJECT_MAX, rate*360/4 );


			count++;
			if(count>30){
				GD_SetGraphAlpha( WLD_GRP_WORLD+0, 255 );
				GD_SetGraphFade( WLD_GRP_WORLD+0, 0 );
				appData.draw_flag=1;

				step = WLD_TERM;
			}
			break;
		case WLD_MIAN:
			count+=1;
			rate = 1-POYO_CONUT(count,15,10);
			








			for(i=0;i<WORLD_OBJECT_MAX;i++){
				GD_SetGraphColor2( WLD_GRP_WORLD+2+i, 128, 128, 128  );
			}
			if( select_no!=-1 ){
				GD_SetGraphColor2( WLD_GRP_WORLD+2+select_no, 128, 160, 255  );
				SetGameWindowDisp( 1, TRUE );
				if(select_no==20){
					leaf_count = LIM(leaf_count+1,0,16);
				}else{
					leaf_count = LIM(leaf_count-1,0,16);
				}

				if(WorldObjyectPoint[select_no].y<300){
					SetGameWindowMove( 1, 16, 600-(16+24*2+48) );
				}else{
					SetGameWindowMove( 1, 16, 16 );
				}
				if(select_bak != select_no ){
					SND_Select();
					SetGameWindowText( 1, WorldObj[select_no].name );
				}
			}else{
				leaf_count = LIM(leaf_count-1,0,16);

				if(select_bak != select_no ){
					SetGameWindowText( 1, "行き先選んでください。\nアイコンをクリックすると、イベントや戦闘が起こります。" );
				}
				SetGameWindowDisp( 1, FALSE );
			}
			GD_SetGraphAlpha( WLD_GRP_WORLD+2+20, leaf_count*16 );

			if( select_no!=-1 && GameKey.click ){



				cx = WorldObjyectRect[select_no].x;
				cy = WorldObjyectRect[select_no].y;
				count=0;
				step = WLD_FADE_OUT;
				AVG_PlaySE3( 9300, 255 );

				if(select_no!=5){
					AVG_StopBGM( 30 );
				}
				
				_mbscpy( (BYTE*)GoScript, (BYTE*)WorldObj[select_no].scr );
			}else if( GameKey.cansel ){

				SetGameWindowMove( 1, 16, 600-(16+24*2+48) );
				SetGameWindowText( 1, "メニューを選んでください。" );
				count=0;
				MUS_SetMouseLayer( 1 );
				step = WLD_COMMAND;

			}
			select_bak = select_no;
			break;

		case WLD_COMMAND:
			count+=1;
			rate = 1-POYO_CONUT(count,15,12);
			




			SetGameWindowDisp( 0, TRUE );
			SetGameWindowZoomXY( 0, 1-rate, 1+rate );
			SetGameWindowColor( 0, 255*count/15, 128, 128, 128 );

			SetGameWindowDisp( 1, TRUE );
			SetGameWindowZoomXY( 1, 1-rate, 1-rate );
			SetGameWindowColor( 1, 255*count/15, 128, 128, 128 );


			if( select_no!=-1 ){
				if(select_bak != select_no ){
					SND_Select();
					switch(select_no){
					case 0:	SetGameWindowText( 1, "アイテム一覧を見ます。" );	break;
					case 1:	SetGameWindowText( 1, "キャラクターの能力の確認、及び装備の変更ができます。" );		break;
					case 2:	SetGameWindowText( 1, "セーブ／ロード" );	break;
					case 3:	SetGameWindowText( 1, "全体マップ画面に戻ります。" );	break;
					}
				}
				if(count>15){
					GD_SetGraphDisp( WLD_GRP_SYSTEM+5, TRUE );
					GD_SetGraphDstRect( WLD_GRP_SYSTEM+5, 800-(24*8+24)-16, 40+24*select_no, 24*8, 24);
				}
				if( GameKey.click ){
					GD_SetGraphDisp( WLD_GRP_SYSTEM+5, FALSE );
					switch(select_no){
					case 0:
						count=15;
						step = WLD_ITEM_LIST;
						SND_Ok();
						break;
					case 1:
						count=15;
						step = WLD_BUTAI;
						SND_Ok();
						break;
					case 2:
						count=15;
						step = WLD_SAVE_LOAD;
						SND_Ok();
						break;
					case 3:
						MUS_SetMouseLayer( 0 );
						SetGameWindowDisp( 0, FALSE );
						SetGameWindowDisp( 1, FALSE );

						step = WLD_MIAN;
						SND_Cansel();
						break;
					}
				}
			}else{
				if(select_bak != select_no ){
					SetGameWindowText( 1, "メニューを選んでください。" );
				}
				GD_SetGraphDisp( WLD_GRP_SYSTEM+5, FALSE );
			}
			if( GameKey.cansel ){
				GD_SetGraphDisp( WLD_GRP_SYSTEM+5, FALSE );
				MUS_SetMouseLayer( 0 );

				SetGameWindowDisp( 0, FALSE );
				SetGameWindowDisp( 1, FALSE );

				step = WLD_MIAN;
				SND_Cansel();
			}
			select_bak = select_no;
			break;

		case WLD_SAVE_LOAD:
			count = max(0,count-1);
			rate = 1-POYO_CONUT(count,15,10);

			SetGameWindowDisp( 0, TRUE );
			SetGameWindowZoomXY( 0, 1-rate, 1+rate );
			SetGameWindowColor( 0, 255*count/15, 128, 128, 128 );
			
			i = WLD_SaveLoad( 1, WLD_BMP_SYSTEM, WLD_GRP_WINDW, WLD_BMP_SYSTEM+2, WLD_SPR_CHAR_SP, WLD_LAY_SYSTEM+2, 2, SYS_WORLD );
			switch( i ){
				case 0:
					break;
				case 1:
					count=0;
					MUS_SetMouseLayer( 1 );
					step = WLD_COMMAND;
					break;
				default:
				case 2:
					NextWLDStep = i-2;
					count=0;
					step = WLD_FADE_OUT;
					break;
			}
			break;
		case WLD_ITEM_LIST:
			count = max(0,count-1);
			rate = 1-POYO_CONUT(count,15,10);
			
			SetGameWindowDisp( 0, TRUE );
			SetGameWindowZoomXY( 0, 1-rate, 1+rate );
			SetGameWindowColor( 0, 255*count/15, 128, 128, 128 );

			if( WLD_ItemList( 1, WLD_BMP_SYSTEM, WLD_GRP_SYSTEM+10, WLD_BMP_SYSTEM+2, WLD_SPR_CHAR_SP, WLD_LAY_SYSTEM+2, 2 ) ){
				count=0;
				MUS_SetMouseLayer( 1 );
				step = WLD_COMMAND;
			}
			break;
			
		case WLD_BUTAI:
			count = max(0,count-1);
			rate = 1-POYO_CONUT(count,15,10);
			
			SetGameWindowDisp( 0, TRUE );
			SetGameWindowZoomXY( 0, 1-rate, 1+rate );
			SetGameWindowColor( 0, 255*count/15, 128, 128, 128 );

			if( WLD_Butai( 1, WLD_BMP_SYSTEM, WLD_GRP_SYSTEM+10, WLD_BMP_SYSTEM+2, WLD_SPR_CHAR_SP, WLD_LAY_SYSTEM+2, 2 ) ){
				count=0;
				MUS_SetMouseLayer( 1 );
				step = WLD_COMMAND;
			}
			break;
	}

	if(appData.debug_mouse!=-1){
		if(KeyCond.trg.f1){
			count=0;
			step = WLD_FADE_OUT;
			AVG_PlaySE3( 9300, 255 );
			_mbscpy( (BYTE*)GoScript, (BYTE*)"9999.sdt" );
		}
		if(KeyCond.trg.f2){
		}
		if(KeyCond.trg.f3){
		}
	}
	return ret;
}

int	NextSVPStep = SYS_BATTLE;

int	SVP_System(void)
{
	enum{
		SVP_INIT,
		SVP_TERM,

		SVP_FADE_IN,
		SVP_FADE_OUT,

		SVP_SAVE_LOAD,
		SVP_ITEM_LIST,
		SVP_BUTAI,
		SVP_COMMAND,
	};
	static int	step=SVP_INIT;
	static int	count=0;
	static int	cx,cy;

	float		rate;
	static int	select_bak=-1;
	int			select_no = MUS_GetMouseNo( -1 );
	int			modoru=ESC_GetFlag(67);

	int			ret=-1;
	int	i,j;

	int	mx,my;

	switch(step){
		case SVP_INIT:
			AVG_PlayBGMLoad();

			AllPartyHeal();
			GD_LoadTexture( WLD_BMP_SYSTEM, "btsys_0000.tga", ALPHA_4444|DITHER_OFF );
			GD_LoadTexture( WLD_BMP_WORLD+0, "Tm000.tga", ALPHA_1555 );

			cx=400;
			cy=300;

			MUS_SetMouseLayer( 1 );

			GD_SetGraph( WLD_GRP_WORLD+0, WLD_BMP_WORLD+0, WLD_LAY_WORLD+1, FALSE );
			GD_SetGraphSrcRect( WLD_GRP_WORLD+0, 0, 0, 800, 600 );
			GD_SetGraphScaleParam( WLD_GRP_WORLD+0, SCALE_HIGHT );
			GD_SetGraphCenter( WLD_GRP_WORLD+0, cx, cy );

			GD_SetGraph( WLD_GRP_WORLD+1, WLD_BMP_WORLD+0, WLD_LAY_WORLD+0, FALSE );
			GD_SetGraphSrcRect( WLD_GRP_WORLD+1, 0, 0, 800, 600 );
			GD_SetGraphScaleParam( WLD_GRP_WORLD+1, SCALE_HIGHT );
			GD_SetGraphCenter( WLD_GRP_WORLD+1, cx, cy );


			SetGameWindow( 0, WLD_GRP_SYSTEM, WLD_BMP_SYSTEM, WLD_LAY_SYSTEM, FALSE,-1, WINDOW_COLOR_SYS );
			if(modoru){
				SetGameWindowRect( 0, 800-(24*8+48)-16, 16, 24*8+48, 24*5+48  );
				SetGameWindowText( 0, "戦闘開始\nアイテム\n部隊編成\n記録\n全体MAPに戻る", WLD_BMP_SYSTEM+1, 0,0, 30 );
				for(i=0;i<5;i++){
					MUS_SetMouseRect( 1, i, 800-16-(24*8+48)+24, 16+24+24*i, 24*8, 24, ON );
				}
			}else{
				SetGameWindowRect( 0, 800-(24*8+48)-16, 16, 24*8+48, 24*4+48  );
				SetGameWindowText( 0, "戦闘開始\nアイテム\n部隊編成\n記録", WLD_BMP_SYSTEM+1, 0,0, 30 );
				for(i=0;i<4;i++){
					MUS_SetMouseRect( 1, i, 800-16-(24*8+48)+24, 16+24+24*i, 24*8, 24, ON );
				}
			}

			SetGameWindow( 1, WLD_GRP_SYSTEM+10, WLD_BMP_SYSTEM, WLD_LAY_SYSTEM, FALSE,6, WINDOW_COLOR_SYS );
			SetGameWindowRect( 1, 16, 600-(16+24*2+48), 24*30+48, 24*2+48  );
			SetGameWindowText( 1, "戦闘準備画面です。\n装備を整え、戦いに備えてください。", WLD_BMP_SYSTEM+2, 0,0, 30 );

			GD_SetGraphx9( WLD_GRP_SYSTEM+5, WLD_BMP_SYSTEM, WLD_LAY_SYSTEM+1, FALSE );	
			GD_SetGraphSrcRect( WLD_GRP_SYSTEM+5, 0, 64, 72, 24 );

			NextSVPStep = SYS_BATTLE;
			select_bak=-1;
			count=0;
			step = SVP_FADE_IN;
			break;
		case SVP_TERM:
			for(i=0;i<WLD_GRP_DEBUG;i++){	GD_ResetGraph( i );			}
			for(i=0;i<WLD_BMP_DEBUG;i++){	GD_ReleaseTexture( i );		}
			for(i=0;i<WLD_SPR_DEBUG;i++){	GD_ReleaseSprite( i );		}
			MUS_ResetMouseRect_All();
			ResetGameWindow_All();


			appData.draw_flag=1;
			appData.flash=1;

			ret = NextSVPStep;

			step = SVP_INIT;
			count=0;
			break;
		case SVP_FADE_IN:
			rate = pow(30-count,2) / pow( 30.0f, 2 );

			GD_SetGraphDisp( WLD_GRP_WORLD+0, ON );
			GD_SetGraphDstRect( WLD_GRP_WORLD+0, 0+(0-cx)*4*rate, 0+(0-cy)*2*rate, 800+3200*rate, 600+1200*rate );

			GD_SetGraphAlpha( WLD_GRP_WORLD+0, count*255/30 );
			GD_SetGraphFade( WLD_GRP_WORLD+0, count*128/30 );
			GD_SetGraphRoll( WLD_GRP_WORLD+0, -rate*360/4 );


			rate=1-rate;
			GD_SetGraphDisp( WLD_GRP_WORLD+1, ON );
			GD_SetGraphDstRect( WLD_GRP_WORLD+1, -1600*rate, -600*rate, 800+3200*rate, 600+1200*rate );

			GD_SetGraphAlpha( WLD_GRP_WORLD+1, count*255/30 );




			count++;
			if(count>30){
				GD_ResetGraph( WLD_GRP_WORLD+1 );
				count=0;
				step = SVP_COMMAND;
			}
			break;
		case SVP_FADE_OUT:
			if(count==0)AVG_StopBGM( 30 );
			rate = pow(count,2) / pow( 30.0f, 2 );

			SetGameWindowAlpha( 0, 255-count*255/28 );
			SetGameWindowAlpha( 1, 255-count*255/28 );
			
			GD_SetGraphDisp( WLD_GRP_WORLD+0, ON );
			GD_SetGraphDstRect( WLD_GRP_WORLD+0, 0+(0-cx)*4*rate, 0+(0-cy)*2*rate, 800+3200*rate, 600+1200*rate );

			GD_SetGraphAlpha( WLD_GRP_WORLD+0, 32 );
			GD_SetGraphFade( WLD_GRP_WORLD+0, 128-count*128/25 );
			GD_SetGraphRoll( WLD_GRP_WORLD+0, rate*360/4 );


			count++;
			if(count>30){
				GD_SetGraphAlpha( WLD_GRP_WORLD+0, 255 );
				GD_SetGraphFade( WLD_GRP_WORLD+0, 0 );
				appData.draw_flag=1;

				step = SVP_TERM;
			}
			break;
		case SVP_COMMAND:
			count+=1;
			rate = 1-POYO_CONUT(count,15,12);

			SetGameWindowDisp( 0, TRUE );
			SetGameWindowZoomXY( 0, 1-rate, 1+rate );
			SetGameWindowColor( 0, 255*count/15, 128, 128, 128 );

			SetGameWindowDisp( 1, TRUE );
			SetGameWindowZoomXY( 1, 1-rate, 1-rate );
			SetGameWindowColor( 1, 255*count/15, 128, 128, 128 );


			if( select_no!=-1 ){
				if(select_bak != select_no ){
					SND_Select();
					switch(select_no){
					case 0:	SetGameWindowText( 1, "戦闘を開始します。" );	break;
					case 1:	SetGameWindowText( 1, "アイテム一覧を見ます。" );	break;
					case 2:	SetGameWindowText( 1, "キャラクターの能力の確認、及び装備の変更ができます。" );		break;
					case 3:	SetGameWindowText( 1, "セーブ／ロード" );	break;
					case 4:	SetGameWindowText( 1, "全体マップ画面に戻ります。" );	break;
					}
				}
				if(count>15){
					GD_SetGraphDisp( WLD_GRP_SYSTEM+5, TRUE );
					GD_SetGraphDstRect( WLD_GRP_SYSTEM+5, 800-(24*8+24)-16, 40+24*select_no, 24*8, 24);
				}
				if( GameKey.click ){
					GD_SetGraphDisp( WLD_GRP_SYSTEM+5, FALSE );
					switch(select_no){
					case 0:
						NextSVPStep=SYS_BATTLE;
						count=0;
						step = SVP_FADE_OUT;
						SND_Ok();
						break;
					case 1:
						count=15;
						step = SVP_ITEM_LIST;
						SND_Ok();
						break;
					case 2:
						count=15;
						step = SVP_BUTAI;
						SND_Ok();
						break;
					case 3:
						count=15;
						step = SVP_SAVE_LOAD;
						SND_Ok();
						break;
					case 4:
						NextSVPStep=SYS_WORLD;
						count=0;
						step = SVP_FADE_OUT;
						SND_Ok();
						break;
					}
				}
			}else{
				if(select_bak != select_no ){
					SetGameWindowText( 1, "戦闘準備画面です。\n装備を整え、戦いに備えてください。" );
				}
				GD_SetGraphDisp( WLD_GRP_SYSTEM+5, FALSE );
			}
			select_bak = select_no;
			break;

		case SVP_SAVE_LOAD:
			count = max(0,count-1);
			rate = 1-POYO_CONUT(count,15,10);

			SetGameWindowDisp( 0, TRUE );
			SetGameWindowZoomXY( 0, 1-rate, 1+rate );
			SetGameWindowColor( 0, 255*count/15, 128, 128, 128 );
			
			i = WLD_SaveLoad( 1, WLD_BMP_SYSTEM, WLD_GRP_WINDW, WLD_BMP_SYSTEM+2, WLD_SPR_CHAR_SP, WLD_LAY_SYSTEM+2, 2, SYS_SAVE_POINT );
			switch( i ){
				case 0:
					break;
				case 1:
					count=0;
					MUS_SetMouseLayer( 1 );
					step = SVP_COMMAND;
					break;
				default:
				case 2:
					NextSVPStep = i-2;
					count=0;
					step = SVP_FADE_OUT;
					break;
			}
			break;
		case SVP_ITEM_LIST:
			count = max(0,count-1);
			rate = 1-POYO_CONUT(count,15,10);

			SetGameWindowDisp( 0, TRUE );
			SetGameWindowZoomXY( 0, 1-rate, 1+rate );
			SetGameWindowColor( 0, 255*count/15, 128, 128, 128 );

			if( WLD_ItemList( 1, WLD_BMP_SYSTEM, WLD_GRP_SYSTEM+10, WLD_BMP_SYSTEM+2, WLD_SPR_CHAR_SP, WLD_LAY_SYSTEM+2, 2 ) ){
				count=0;
				MUS_SetMouseLayer( 1 );
				step = SVP_COMMAND;
			}
			break;
			
		case SVP_BUTAI:
			count = max(0,count-1);
			rate = 1-POYO_CONUT(count,15,10);
			
			SetGameWindowDisp( 0, TRUE );
			SetGameWindowZoomXY( 0, 1-rate, 1+rate );
			SetGameWindowColor( 0, 255*count/15, 128, 128, 128 );

			if( WLD_Butai( 1, WLD_BMP_SYSTEM, WLD_GRP_SYSTEM+10, WLD_BMP_SYSTEM+2, WLD_SPR_CHAR_SP, WLD_LAY_SYSTEM+2, 2 ) ){
				count=0;
				MUS_SetMouseLayer( 1 );
				step = SVP_COMMAND;
			}
			break;
	}
	if(KeyCond.trg.f1){
	}
	if(KeyCond.trg.f2){
	}
	if(KeyCond.trg.f3){
	}
	return ret;
}



int	SVP2_System(void)
{
	enum{
		SVP_INIT,
		SVP_TERM,

		SVP_WAIT,
		SVP_FADE_IN,
		SVP_FADE_OUT,

		SVP_SAVE_LOAD,
		SVP_ITEM_LIST,
		SVP_BUTAI,
		SVP_COMMAND,
	};
	static int	step=SVP_INIT;
	static int	count=0;
	static int	cx,cy;

	float		rate;
	static int	select_bak=-1;
	int			select_no = MUS_GetMouseNo( -1 );
	int			modoru=ESC_GetFlag(67);
	static int	no_char=0;

	int			ret=-1;
	int	i,j;

	int	mx,my;

	switch(step){
		case SVP_INIT:
			AVG_PlayBGMLoad();

			AllPartyHeal();
			GD_LoadTexture( WLD_BMP_SYSTEM, "btsys_0000.tga", ALPHA_4444|DITHER_OFF );
			GD_LoadTexture( WLD_BMP_WORLD+0, "Tm000.tga", ALPHA_1555 );

			cx=400;
			cy=300;

			MUS_SetMouseLayer( 1 );

			GD_SetGraph( WLD_GRP_WORLD+0, WLD_BMP_WORLD+0, WLD_LAY_WORLD+1, FALSE );
			GD_SetGraphSrcRect( WLD_GRP_WORLD+0, 0, 0, 800, 600 );
			GD_SetGraphScaleParam( WLD_GRP_WORLD+0, SCALE_HIGHT );
			GD_SetGraphCenter( WLD_GRP_WORLD+0, cx, cy );

			GD_SetGraph( WLD_GRP_WORLD+1, WLD_BMP_WORLD+0, WLD_LAY_WORLD+0, FALSE );
			GD_SetGraphSrcRect( WLD_GRP_WORLD+1, 0, 0, 800, 600 );
			GD_SetGraphScaleParam( WLD_GRP_WORLD+1, SCALE_HIGHT );
			GD_SetGraphCenter( WLD_GRP_WORLD+1, cx, cy );


			SetGameWindow( 0, WLD_GRP_SYSTEM, WLD_BMP_SYSTEM, WLD_LAY_SYSTEM, FALSE,-1, WINDOW_COLOR_SYS );
			SetGameWindowRect( 0, 800-(24*8+48)-16, 16, 24*8+48, 24*4+48  );

			no_char=0;
			for(i=0;i<9;i++){
				if(CharParam[i].flag){
					no_char=1;
				}
			}
			if(no_char){
				SetGameWindowText( 0, "先へ進む\nアイテム\n部隊編成\n記録", WLD_BMP_SYSTEM+1, 0,0, 30 );
			}else{
				SetGameWindowText( 0, "先へ進む\n<B2:アイテム\n部隊編成>\n記録", WLD_BMP_SYSTEM+1, 0,0, 30 );
			}
			for(i=0;i<4;i++){
				MUS_SetMouseRect( 1, i, 800-16-(24*8+48)+24, 16+24+24*i, 24*8, 24, ON );
			}

			SetGameWindow( 1, WLD_GRP_SYSTEM+10, WLD_BMP_SYSTEM, WLD_LAY_SYSTEM, FALSE,6, WINDOW_COLOR_SYS );
			SetGameWindowRect( 1, 16, 600-(16+24*2+48), 24*30+48, 24*2+48  );
			SetGameWindowText( 1, "セーブポイントです。", WLD_BMP_SYSTEM+2, 0,0, 30 );

			GD_SetGraphx9( WLD_GRP_SYSTEM+5, WLD_BMP_SYSTEM, WLD_LAY_SYSTEM+1, FALSE );	
			GD_SetGraphSrcRect( WLD_GRP_SYSTEM+5, 0, 64, 72, 24 );

			NextSVPStep = SYS_BATTLE;
			select_bak=-1;
			count=0;
			step = SVP_WAIT;
			break;
		case SVP_TERM:
			for(i=0;i<WLD_GRP_DEBUG;i++){	GD_ResetGraph( i );			}
			for(i=0;i<WLD_BMP_DEBUG;i++){	GD_ReleaseTexture( i );		}
			for(i=0;i<WLD_SPR_DEBUG;i++){	GD_ReleaseSprite( i );		}
			MUS_ResetMouseRect_All();
			ResetGameWindow_All();


			appData.draw_flag=1;
			appData.flash=1;

			ret = NextSVPStep;

			step = SVP_INIT;
			count=0;
			break;
		case SVP_WAIT:
			count++;
			if(count>30){
				count=0;
				step = SVP_FADE_IN;
			}
			break;
		case SVP_FADE_IN:
			rate = pow(30-count,2) / pow( 30.0f, 2 );

			GD_SetGraphDisp( WLD_GRP_WORLD+0, ON );
			GD_SetGraphDstRect( WLD_GRP_WORLD+0, 0+(0-cx)*4*rate, 0+(0-cy)*2*rate, 800+3200*rate, 600+1200*rate );

			GD_SetGraphAlpha( WLD_GRP_WORLD+0, count*255/30 );
			GD_SetGraphFade( WLD_GRP_WORLD+0, count*128/30 );
			GD_SetGraphRoll( WLD_GRP_WORLD+0, -rate*360/4 );


			rate=1-rate;
			GD_SetGraphDisp( WLD_GRP_WORLD+1, ON );
			GD_SetGraphDstRect( WLD_GRP_WORLD+1, -1600*rate, -600*rate, 800+3200*rate, 600+1200*rate );

			GD_SetGraphAlpha( WLD_GRP_WORLD+1, count*255/30 );




			count++;
			if(count>30){
				GD_ResetGraph( WLD_GRP_WORLD+1 );
				count=0;
				step = SVP_COMMAND;
			}
			break;
		case SVP_FADE_OUT:
			if(count==0)AVG_StopBGM( 30 );
			rate = pow(count,2) / pow( 30.0f, 2 );
	
			SetGameWindowAlpha( 0, 255-count*255/28 );
			SetGameWindowAlpha( 1, 255-count*255/28 );
			
			GD_SetGraphDisp( WLD_GRP_WORLD+0, ON );
			GD_SetGraphDstRect( WLD_GRP_WORLD+0, 0+(0-cx)*4*rate, 0+(0-cy)*2*rate, 800+3200*rate, 600+1200*rate );

			GD_SetGraphAlpha( WLD_GRP_WORLD+0, 32 );
			GD_SetGraphFade( WLD_GRP_WORLD+0, 128-count*128/25 );
			GD_SetGraphRoll( WLD_GRP_WORLD+0, rate*360/4 );


			count++;
			if(count>30){
				GD_SetGraphAlpha( WLD_GRP_WORLD+0, 255 );
				GD_SetGraphFade( WLD_GRP_WORLD+0, 0 );
				appData.draw_flag=1;

				step = SVP_TERM;
			}
			break;
		case SVP_COMMAND:
			count+=1;
			rate = 1-POYO_CONUT(count,15,12);

			SetGameWindowDisp( 0, TRUE );
			SetGameWindowZoomXY( 0, 1-rate, 1+rate );
			SetGameWindowColor( 0, 255*count/15, 128, 128, 128 );

			SetGameWindowDisp( 1, TRUE );
			SetGameWindowZoomXY( 1, 1-rate, 1-rate );
			SetGameWindowColor( 1, 255*count/15, 128, 128, 128 );


			if( select_no!=-1 ){
				if(select_bak != select_no ){
					SND_Select();
					switch(select_no){
					case 0:	SetGameWindowText( 1, "先に進みます。" );	break;
					case 1:	SetGameWindowText( 1, "アイテム一覧を見ます。" );	break;
					case 2:	SetGameWindowText( 1, "キャラクターの能力の確認、及び装備の変更ができます。" );		break;
					case 3:	SetGameWindowText( 1, "セーブ／ロード" );	break;
					}
				}
				if(count>15){
					GD_SetGraphDisp( WLD_GRP_SYSTEM+5, TRUE );
					GD_SetGraphDstRect( WLD_GRP_SYSTEM+5, 800-(24*8+24)-16, 40+24*select_no, 24*8, 24);
				}
				if( GameKey.click ){
					GD_SetGraphDisp( WLD_GRP_SYSTEM+5, FALSE );

					switch(select_no){
					case 0:
						NextSVPStep=SYS_BATTLE;
						count=0;
						step = SVP_FADE_OUT;
						SND_Ok();
						break;
					case 1:
						if(no_char){
							count=15;
							step = SVP_ITEM_LIST;
							SND_Ok();
						}else{
							SND_Ng();
						}
						break;
					case 2:
						if(no_char){
							count=15;
							step = SVP_BUTAI;
							SND_Ok();
						}else{
							SND_Ng();
						}
						break;
					case 3:
						count=15;
						step = SVP_SAVE_LOAD;
						SND_Ok();
						break;
					}
				}
			}else{
				if(select_bak != select_no ){
					SetGameWindowText( 1, "セーブポイントです。" );
				}
				GD_SetGraphDisp( WLD_GRP_SYSTEM+5, FALSE );
			}
			select_bak = select_no;
			break;

		case SVP_SAVE_LOAD:
			count = max(0,count-1);
			rate = 1-POYO_CONUT(count,15,10);

			SetGameWindowDisp( 0, TRUE );
			SetGameWindowZoomXY( 0, 1-rate, 1+rate );
			SetGameWindowColor( 0, 255*count/15, 128, 128, 128 );
			
			i = WLD_SaveLoad( 1, WLD_BMP_SYSTEM, WLD_GRP_WINDW, WLD_BMP_SYSTEM+2, WLD_SPR_CHAR_SP, WLD_LAY_SYSTEM+2, 2, SYS_SAVE_POINT2 );
			switch( i ){
				case 0:
					break;
				case 1:
					count=0;
					MUS_SetMouseLayer( 1 );
					step = SVP_COMMAND;
					break;
				default:
				case 2:
					NextSVPStep = i-2;
					count=0;
					step = SVP_FADE_OUT;
					break;
			}
			break;
		case SVP_ITEM_LIST:
			count = max(0,count-1);
			rate = 1-POYO_CONUT(count,15,10);

			SetGameWindowDisp( 0, TRUE );
			SetGameWindowZoomXY( 0, 1-rate, 1+rate );
			SetGameWindowColor( 0, 255*count/15, 128, 128, 128 );

			if( WLD_ItemList( 1, WLD_BMP_SYSTEM, WLD_GRP_SYSTEM+10, WLD_BMP_SYSTEM+2, WLD_SPR_CHAR_SP, WLD_LAY_SYSTEM+2, 2 ) ){
				count=0;
				MUS_SetMouseLayer( 1 );
				step = SVP_COMMAND;
			}
			break;
			
		case SVP_BUTAI:
			count = max(0,count-1);
			rate = 1-POYO_CONUT(count,15,10);
			
			SetGameWindowDisp( 0, TRUE );
			SetGameWindowZoomXY( 0, 1-rate, 1+rate );
			SetGameWindowColor( 0, 255*count/15, 128, 128, 128 );

			if( WLD_Butai( 1, WLD_BMP_SYSTEM, WLD_GRP_SYSTEM+10, WLD_BMP_SYSTEM+2, WLD_SPR_CHAR_SP, WLD_LAY_SYSTEM+2, 2 ) ){
				count=0;
				MUS_SetMouseLayer( 1 );
				step = SVP_COMMAND;
			}
			break;
	}

	if(KeyCond.trg.f2){
	}
	if(KeyCond.trg.f3){
	}
	return ret;
}


int	END_System(void)
{
	enum{
		SVP_INIT,
		SVP_TERM,

		SVP_FADE_IN,
		SVP_FADE_OUT,

		SVP_SAVE_LOAD,
		SVP_COMMAND,
		SVP_YESNO,
	};
	static int	step=SVP_INIT;
	static int	count=0;
	static int	cx,cy;

	float		rate;
	static int	select_bak=-1;
	int			select_no = MUS_GetMouseNo( -1 );
	int			modoru=ESC_GetFlag(67);
	static int	no_char=0;

	int			ret=-1;
	int	i,j;

	int	mx,my;

	switch(step){
		case SVP_INIT:


			AllPartyHeal();
			GD_LoadTexture( WLD_BMP_SYSTEM, "btsys_0000.tga", ALPHA_4444|DITHER_OFF );
			GD_LoadTexture( WLD_BMP_WORLD+0, "Tm000.tga", ALPHA_1555 );

			cx=400;
			cy=300;

			MUS_SetMouseLayer( 1 );

			GD_SetGraph( WLD_GRP_WORLD+0, WLD_BMP_WORLD+0, WLD_LAY_WORLD+1, FALSE );
			GD_SetGraphSrcRect( WLD_GRP_WORLD+0, 0, 0, 800, 600 );
			GD_SetGraphScaleParam( WLD_GRP_WORLD+0, SCALE_HIGHT );
			GD_SetGraphCenter( WLD_GRP_WORLD+0, cx, cy );

			GD_SetGraph( WLD_GRP_WORLD+1, WLD_BMP_WORLD+0, WLD_LAY_WORLD+0, FALSE );
			GD_SetGraphSrcRect( WLD_GRP_WORLD+1, 0, 0, 800, 600 );
			GD_SetGraphScaleParam( WLD_GRP_WORLD+1, SCALE_HIGHT );
			GD_SetGraphCenter( WLD_GRP_WORLD+1, cx, cy );


			SetGameWindow( 0, WLD_GRP_SYSTEM, WLD_BMP_SYSTEM, WLD_LAY_SYSTEM, FALSE,-1, WINDOW_COLOR_SYS );
			SetGameWindowRect( 0, 800-(24*8+48)-16, 16, 24*8+48, 24*2+48  );
			SetGameWindowText( 0, "タイトルへ戻る\n終了データを記録", WLD_BMP_SYSTEM+1, 0,0, 30 );
			for(i=0;i<2;i++){
				MUS_SetMouseRect( 1, i, 800-16-(24*8+48)+24, 16+24+24*i, 24*8, 24, ON );
			}

			SetGameWindow( 1, WLD_GRP_SYSTEM+10, WLD_BMP_SYSTEM, WLD_LAY_SYSTEM, FALSE,6, WINDOW_COLOR_SYS );
			SetGameWindowRect( 1, 16, 600-(16+24*2+48), 24*30+48, 24*2+48  );
			SetGameWindowText( 1, "クリア後データをセーブできます。\nフリーマップで遊び放題。実は他にも…", WLD_BMP_SYSTEM+2, 0,0, 30 );

			SetGameWindow( 2, WLD_GRP_SYSTEM+15, WLD_BMP_SYSTEM, WLD_LAY_SYSTEM+3, FALSE,-1, WINDOW_COLOR_SYS );
			SetGameWindowRect( 2, 800-(24*8+48)-4-192, 24+192, 24*3+48, 24*2+48 );
			SetGameWindowText( 2, "はい\nいいえ", WLD_BMP_SYSTEM+3 );

			MUS_SetMouseRect( 2, 0, 800-(24*8+24)-4-192, 48+192, 24*3, 24, ON );
			MUS_SetMouseRect( 2, 1, 800-(24*8+24)-4-192, 72+192, 24*3, 24, ON );

			GD_SetGraphx9( WLD_GRP_SYSTEM+5, WLD_BMP_SYSTEM, WLD_LAY_SYSTEM+1, FALSE );	
			GD_SetGraphSrcRect( WLD_GRP_SYSTEM+5, 0, 64, 72, 24 );

			GD_SetGraphx9( WLD_GRP_SYSTEM+6, WLD_BMP_SYSTEM, WLD_LAY_SYSTEM+4, FALSE );	
			GD_SetGraphSrcRect( WLD_GRP_SYSTEM+6, 0, 64, 72, 24 );

			NextSVPStep = SYS_TITLE;
			select_bak=-1;
			count=0;
			step = SVP_FADE_IN;
			break;
		case SVP_TERM:
			for(i=0;i<WLD_GRP_DEBUG;i++){	GD_ResetGraph( i );			}
			for(i=0;i<WLD_BMP_DEBUG;i++){	GD_ReleaseTexture( i );		}
			for(i=0;i<WLD_SPR_DEBUG;i++){	GD_ReleaseSprite( i );		}
			MUS_ResetMouseRect_All();
			ResetGameWindow_All();

			appData.draw_flag=1;
			appData.flash=1;

			ret = NextSVPStep;

			step = SVP_INIT;
			count=0;
			break;
		case SVP_FADE_IN:
			rate = pow(30-count,2) / pow( 30.0f, 2 );

			GD_SetGraphDisp( WLD_GRP_WORLD+0, ON );
			GD_SetGraphDstRect( WLD_GRP_WORLD+0, 0+(0-cx)*4*rate, 0+(0-cy)*2*rate, 800+3200*rate, 600+1200*rate );

			GD_SetGraphAlpha( WLD_GRP_WORLD+0, count*255/30 );
			GD_SetGraphFade( WLD_GRP_WORLD+0, count*128/30 );
			GD_SetGraphRoll( WLD_GRP_WORLD+0, -rate*360/4 );


			rate=1-rate;
			GD_SetGraphDisp( WLD_GRP_WORLD+1, ON );
			GD_SetGraphDstRect( WLD_GRP_WORLD+1, -1600*rate, -600*rate, 800+3200*rate, 600+1200*rate );

			GD_SetGraphAlpha( WLD_GRP_WORLD+1, count*255/30 );



			count++;
			if(count>30){
				GD_ResetGraph( WLD_GRP_WORLD+1 );
				count=0;
				step = SVP_COMMAND;
			}
			break;
		case SVP_FADE_OUT:
			if(count==0)AVG_StopBGM( 30 );
			rate = pow(count,2) / pow( 30.0f, 2 );
			
			SetGameWindowAlpha( 0, 255-count*255/28 );
			SetGameWindowAlpha( 1, 255-count*255/28 );
			SetGameWindowAlpha( 2, 255-count*255/28 );

			GD_SetGraphDisp( WLD_GRP_WORLD+0, ON );
			GD_SetGraphDstRect( WLD_GRP_WORLD+0, 0+(0-cx)*4*rate, 0+(0-cy)*2*rate, 800+3200*rate, 600+1200*rate );

			GD_SetGraphAlpha( WLD_GRP_WORLD+0, 32 );
			GD_SetGraphFade( WLD_GRP_WORLD+0, 128-count*128/25 );
			GD_SetGraphRoll( WLD_GRP_WORLD+0, rate*360/4 );


			count++;
			if(count>30){
				GD_SetGraphAlpha( WLD_GRP_WORLD+0, 255 );
				GD_SetGraphFade( WLD_GRP_WORLD+0, 0 );
				appData.draw_flag=1;

				step = SVP_TERM;
			}
			break;
		case SVP_COMMAND:
			count+=1;
			rate = 1-POYO_CONUT(count,15,12);

			SetGameWindowDisp( 0, TRUE );
			SetGameWindowZoomXY( 0, 1-rate, 1+rate );
			SetGameWindowColor( 0, 255*count/15, 128, 128, 128 );

			SetGameWindowDisp( 1, TRUE );
			SetGameWindowZoomXY( 1, 1-rate, 1-rate );
			SetGameWindowColor( 1, 255*count/15, 128, 128, 128 );

			SetGameWindowDisp( 2, FALSE );
	
			if( select_no!=-1 ){
				if(select_bak != select_no ){
					SND_Select();
					switch(select_no){
					case 0:	SetGameWindowText( 1, "タイトルへ戻ります。" );	break;
					case 1:	SetGameWindowText( 1, "クリアデータのセーブ" );	break;
					}
				}
				if(count>15){
					GD_SetGraphDisp( WLD_GRP_SYSTEM+5, TRUE );
					GD_SetGraphDstRect( WLD_GRP_SYSTEM+5, 800-(24*8+24)-16, 40+24*select_no, 24*8, 24);
				}
				if( GameKey.click ){
					GD_SetGraphDisp( WLD_GRP_SYSTEM+5, FALSE );

					switch(select_no){
					case 0:
						MUS_SetMouseLayer( 2 );
						count=0;
						step = SVP_YESNO;
						SND_Ok();
						break;
					case 1:
						SetGameWindowDisp( 1, FALSE );
						count=15;
						step = SVP_SAVE_LOAD;
						SND_Ok();
						break;
					}
				}
			}else{
				if(select_bak != select_no ){
					SetGameWindowText( 1, "クリア後データをセーブできます。\nフリーマップで遊び放題。実は他にも…" );
				}
				GD_SetGraphDisp( WLD_GRP_SYSTEM+5, FALSE );
			}
			select_bak = select_no;
			break;
		case SVP_YESNO:
			SetGameWindowDisp( 2, TRUE );
			if( select_no!=-1 ){
				if(select_bak != select_no ){
					SND_Select();
				}

				GD_SetGraphDisp( WLD_GRP_SYSTEM+6, TRUE );
				GD_SetGraphDstRect( WLD_GRP_SYSTEM+6, 800-(24*8+24)-4-192, 48+192+24*select_no, 24*3, 24);
				if( GameKey.click ){
					GD_SetGraphDisp( WLD_GRP_SYSTEM+6, FALSE );

					switch(select_no){
					case 0:
						count=0;
						step = SVP_FADE_OUT;
						SND_Ok();
						break;
					case 1:
						MUS_SetMouseLayer( 1 );
						count=0;
						step = SVP_COMMAND;
						SND_Cansel();
						break;
					}
				}
			}else{
				if(select_bak != select_no ){
					SetGameWindowText( 1, "タイトルへ戻って良いですか？" );
				}
				GD_SetGraphDisp( WLD_GRP_SYSTEM+6, FALSE );
			}
			if(GameKey.cansel){
				GD_SetGraphDisp( WLD_GRP_SYSTEM+6, FALSE );
				MUS_SetMouseLayer( 1 );
				count=0;
				step = SVP_COMMAND;
				SND_Cansel();
			}
			select_bak = select_no;
			break;

		case SVP_SAVE_LOAD:
			count = max(0,count-1);
			rate = 1-POYO_CONUT(count,15,10);

			SetGameWindowDisp( 0, TRUE );
			SetGameWindowZoomXY( 0, 1-rate, 1+rate );
			SetGameWindowColor( 0, 255*count/15, 128, 128, 128 );
			
			i = WLD_SaveLoad( 1, WLD_BMP_SYSTEM, WLD_GRP_WINDW, WLD_BMP_SYSTEM+2, WLD_SPR_CHAR_SP, WLD_LAY_SYSTEM+2, 2, SYS_END_SAVE );
			switch( i ){
				default:
				case 0:
					break;
				case 1:
					SetGameWindow( 2, WLD_GRP_SYSTEM+15, WLD_BMP_SYSTEM, WLD_LAY_SYSTEM+3, FALSE,-1, WINDOW_COLOR_SYS );
					SetGameWindowRect( 2, 800-(24*8+48)-4-192, 24+192, 24*3+48, 24*2+48 );
					SetGameWindowText( 2, "はい\nいいえ", WLD_BMP_SYSTEM+3 );

					MUS_SetMouseRect( 2, 0, 800-(24*8+24)-4-192, 48+192, 24*3, 24, ON );
					MUS_SetMouseRect( 2, 1, 800-(24*8+24)-4-192, 72+192, 24*3, 24, ON );

					count=0;
					MUS_SetMouseLayer( 1 );
					step = SVP_COMMAND;
					break;
			}
			break;
	}

	return ret;
}