#include <windows.h>
#include <mouse.h>
#include <keybord.h>

#include <STD_Windows.h>
#include <STD_Debug.h>
#include <STD_File.h>

#include "comp_pac.h"

#include "Escript.h"

#include "GlobalDraw.h"
#include "GM_Avg.h"

#include "main.h"

#include "GM_Main.h"
#include "GM_World.h"
#include "GM_Char.h"
#include "GM_Magic.h"



int	NextCaslStep = SYS_WORLD;

int	CSL_Item2System( void )
{
	return CSL_Item1System(1);
}

int	CSL_Item1System( int kajiya)
{
	enum{
		CSL_INIT,
		CSL_TERM,

		CSL_HEITAN,
		CSL_ITEM_SHOP,
		CSL_ITEM_LIST,
		CSL_BUTAI,

		CSL_MIAN,
		CSL_COMMAND,
	};
	static int	step=CSL_INIT;
	static int	count=0;
	static int	cx,cy;

	float		rate;
	int			select_no = MUS_GetMouseNo( -1 );

	int			ret=-1;
	int	i,j;

	int	mx,my;

	switch(step){
		case CSL_INIT:
			AVG_PlayBGM( 30, 0, TRUE, 255, FALSE );

			GD_LoadTexture( WLD_BMP_SYSTEM, "btsys_0000.tga", ALPHA_4444|DITHER_OFF );
			if(kajiya){
				if( ESC_GetFlag(64)==1 ){	
					GD_LoadTexture( WLD_BMP_WORLD+1, "B02300.tga", ALPHA_1555, FALSE,FALSE,NULL,NULL,256,"bak" );
				}else{
					if( ESC_GetFlag(69) ){
						GD_LoadTexture( WLD_BMP_WORLD+1, "v10201.tga", ALPHA_1555 );
					}else{
						GD_LoadTexture( WLD_BMP_WORLD+1, "v10200.tga", ALPHA_1555 );
					}
				}
			}else{
				if( ESC_GetFlag(63)==1 ){	
					GD_LoadTexture( WLD_BMP_WORLD+1, "B02200.tga", ALPHA_1555, FALSE,FALSE,NULL,NULL,256,"bak" );
				}else{	
					GD_LoadTexture( WLD_BMP_WORLD+1, "v07000.tga", ALPHA_1555 );
				}

			}

			MUS_SetMouseLayer( 0 );

			GD_SetGraph( WLD_GRP_WORLD+10, WLD_BMP_WORLD+1, WLD_LAY_WORLD+1, FALSE );	
			GD_SetGraphRect( WLD_GRP_WORLD+10, 0,0,  0,0, 800, 600 );
			GD_SetGraphBlendParam( WLD_GRP_WORLD+10, BLEND_ALPHA );
			GD_SetGraphScaleParam( WLD_GRP_WORLD+10, SCALE_HIGHT );
			GD_SetGraphCenter( WLD_GRP_WORLD+10, 400, 0 );



			count=0;
			step = CSL_ITEM_SHOP;
			break;
		case CSL_TERM:
			ESC_SetFlag(68,0);
			ESC_SetFlag(69,0);

			for(i=0;i<WLD_GRP_DEBUG;i++){
				GD_ResetGraph( i );
			}
			for(i=0;i<WLD_BMP_DEBUG;i++){
				GD_ReleaseTexture( i );
			}
			for(i=0;i<WLD_SPR_DEBUG;i++){
				GD_ReleaseSprite( i );
			}
			MUS_ResetMouseRect_All();
			MUS_SetMouseLayer( 0 );
			ResetGameWindow_All();

			ret = SYS_BATTLE;
			if(kajiya){
				_mbscpy( (BYTE*)GoScript, (BYTE*)"sys_item11.sdt" );
			}else{
				_mbscpy( (BYTE*)GoScript, (BYTE*)"sys_item10.sdt" );
			}

			step = CSL_INIT;
			count=0;
			break;
		case CSL_ITEM_SHOP:
			switch( WLD_ItemShop( 1, WLD_BMP_SYSTEM, WLD_GRP_WINDW, WLD_BMP_SYSTEM+2, WLD_SPR_CHAR_SP, WLD_LAY_SYSTEM+2, 2, kajiya ) ){
			case 0:
				count = min(30,count+1);
				rate = 1-POYO_CONUT(15-count,15,10);

				SetGameWindowDisp( 0, TRUE );
				SetGameWindowZoomXY( 0, 1-rate, 1+rate );
				SetGameWindowColor( 0, 255-255*count/15, 128, 128, 128 );
				
				GD_SetGraphDisp( WLD_GRP_WORLD+0, FALSE );

				rate = POW_CONUT(30-count,30);
				GD_SetGraphDisp( WLD_GRP_WORLD+10, TRUE );
				if(kajiya){
					GD_SetGraphRect( WLD_GRP_WORLD+10, 0,0,  0, 0, 800, 600 );
				}else{
					if( ESC_GetFlag(63)==2 ){	
						GD_SetGraphRect( WLD_GRP_WORLD+10, 0,0,  0, (1440-600)+(1440-600)*rate, 800, 600 );
					}else{
						GD_SetGraphRect( WLD_GRP_WORLD+10, 0,0,  0, 0, 800, 600 );
					}
				}
				GD_SetGraphAlpha( WLD_GRP_WORLD+10, 255*count/30 );

				GD_SetGraphZoom( WLD_GRP_WORLD+10, 1+rate );
				break;
			case 1:
				GD_SetGraphDisp( WLD_GRP_WORLD+0, TRUE );
				GD_SetGraphDisp( WLD_GRP_WORLD+10, FALSE );
				count=0;
				MUS_SetMouseLayer( 1 );
				step = CSL_TERM;
				break;
			case 2:
				count = max(0,count-1);
				GD_SetGraphDisp( WLD_GRP_WORLD+0, TRUE );
				GD_SetGraphDisp( WLD_GRP_WORLD+10, TRUE );
				GD_SetGraphFade( WLD_GRP_WORLD+10, 128*(count-15)/15 );
				break;
			}
			break;
	}
	return ret;
}


int	CSL_System(void)
{
	enum{
		CSL_INIT,
		CSL_TERM,

		CSL_FADE_IN,
		CSL_FADE_OUT,

		CSL_SAVE_LOAD,
		CSL_HEITAN,
		CSL_ITEM_LIST,
		CSL_BUTAI,

		CSL_MIAN,
		CSL_COMMAND,
	};
	static int	step=CSL_INIT;
	static int	count=0;
	static int	cx,cy;
	char		buf[256];

	float		rate;
	static int	select_bak=-1;
	int			select_no = MUS_GetMouseNo( -1 );

	int			ret=-1;
	int	i,j;

	int	mx,my;

	switch(step){
		case CSL_INIT:
			AVG_PlayBGM( 30, 0, TRUE, 255, FALSE );
	NextCaslStep = SYS_WORLD;
			GD_LoadTexture( WLD_BMP_SYSTEM, "btsys_0000.tga", ALPHA_4444|DITHER_OFF );
			GD_LoadTexture( WLD_BMP_WORLD+0, "castle_00.tga", ALPHA_1555 );
			cx=400;
			cy=300;

			MUS_SetMouseLayer( 0 );

			GD_SetGraph( WLD_GRP_WORLD+0, WLD_BMP_WORLD+0, WLD_LAY_WORLD, FALSE );	
			GD_SetGraphRect( WLD_GRP_WORLD+0, 0,0,  0,0, 800, 600 );
			GD_SetGraphBlendParam( WLD_GRP_WORLD+0, BLEND_ALPHA );
			GD_SetGraphScaleParam( WLD_GRP_WORLD+0, SCALE_HIGHT );
			GD_SetGraphCenter( WLD_GRP_WORLD+0, 100, cy );

			GD_SetGraph( WLD_GRP_WORLD+1, WLD_BMP_WORLD+0, WLD_LAY_WORLD+1, FALSE );	
			GD_SetGraphRect( WLD_GRP_WORLD+1, 0,0,  0,0, 800, 600 );
			GD_SetGraphBlendParam( WLD_GRP_WORLD+1, BLEND_ALPHA );
			GD_SetGraphScaleParam( WLD_GRP_WORLD+1, SCALE_HIGHT );
			GD_SetGraphCenter( WLD_GRP_WORLD+1, 700, cy );




		
			SetGameWindow( 0, WLD_GRP_SYSTEM, WLD_BMP_SYSTEM, WLD_LAY_SYSTEM, FALSE, -1, WINDOW_COLOR_SYS );

			_mbscpy( (BYTE*)buf, (BYTE*)"出陣\nアイテム\n部隊編成\n兵站所\n記録" );
			j=5;
			if( ESC_GetFlag(63)==2 ){
				_mbscat( (BYTE*)buf, (BYTE*)"\n道具屋" );
				j++;
			}else if( ESC_GetFlag(63)==1 ){
				if(ESC_GetFlag(64)){
					_mbscat( (BYTE*)buf, (BYTE*)"\n道具屋" );
				}else{
					_mbscat( (BYTE*)buf, (BYTE*)"\n道具屋" );
				}
				j++;
			}
			if( ESC_GetFlag(64)==2 ){
				_mbscat( (BYTE*)buf, (BYTE*)"\n武器屋" );
				j++;
			}else if( ESC_GetFlag(64)==1 ){
				_mbscat( (BYTE*)buf, (BYTE*)"\n武器屋" );
				j++;
			}
			if( ESC_GetFlag(70) ){
				if( ESC_GetFlag(70)==1 ){
					_mbscat( (BYTE*)buf, (BYTE*)"\n城内を散策する" );
				}else{
					_mbscat( (BYTE*)buf, (BYTE*)"\n軍議" );
				}
				j++;
			}
			SetGameWindowRect( 0, 800-(24*8+48)-16, 16, 24*8+48, 24*j+48  );
			SetGameWindowText( 0, buf, WLD_BMP_SYSTEM+1 );

			for(i=0;i<j;i++){
				MUS_SetMouseRect( 1, i, 800-16-(24*8+48)+24, 16+24+24*i, 24*8, 24, ON );
			}


			SetGameWindow( 1, WLD_GRP_SYSTEM+10, WLD_BMP_SYSTEM, WLD_LAY_SYSTEM, FALSE, 6, WINDOW_COLOR_SYS );
			SetGameWindowRect( 1, 4, 600-(24*2+48), 800-8, 24*2+48  );
			SetGameWindowText( 1, "拠点アヴァロンです。\nメニューを選択してください。", WLD_BMP_SYSTEM+2, 0,0, 30 );

			GD_SetGraphx9( WLD_GRP_SYSTEM+5, WLD_BMP_SYSTEM, WLD_LAY_SYSTEM+1, FALSE );	
			GD_SetGraphSrcRect( WLD_GRP_SYSTEM+5, 0, 64, 72, 24 );

			select_bak=-1;
			count=0;
			step = CSL_FADE_IN;
			break;
		case CSL_TERM:
			for(i=0;i<WLD_GRP_DEBUG;i++){
				GD_ResetGraph( i );
			}
			for(i=0;i<WLD_BMP_DEBUG;i++){
				GD_ReleaseTexture( i );
			}
			for(i=0;i<WLD_SPR_DEBUG;i++){
				GD_ReleaseSprite( i );
			}
			MUS_ResetMouseRect_All();
			MUS_SetMouseLayer( 0 );
			ResetGameWindow_All();

			ret = NextCaslStep;

			step = CSL_INIT;
			count=0;
			break;
		case CSL_FADE_IN:
			count++;
			rate = 1-POW_CONUT(count,30);

			GD_SetGraphDisp( WLD_GRP_WORLD+0, ON );
			GD_SetGraphAlpha( WLD_GRP_WORLD+0, 32 );
			GD_SetGraphFade( WLD_GRP_WORLD+0, count*128/30 );
			GD_SetGraphZoomXY( WLD_GRP_WORLD+0, 1+rate*10, 1 );

			GD_SetGraphDisp( WLD_GRP_WORLD+1, ON );
			GD_SetGraphAlpha( WLD_GRP_WORLD+1, 32 );
			GD_SetGraphFade( WLD_GRP_WORLD+1, count*128/30 );
			GD_SetGraphZoomXY( WLD_GRP_WORLD+1, 1+rate*10, 1 );

			if(count>30){
				GD_SetGraphAlpha( WLD_GRP_WORLD+0, 255 );
				GD_SetGraphFade( WLD_GRP_WORLD+0, 128 );
				GD_SetGraphZoom( WLD_GRP_WORLD+0, 1 );

				GD_ResetGraph( WLD_GRP_WORLD+1 );
				count=0;
				step = CSL_MIAN;
			}
			break;
		case CSL_FADE_OUT:

			count++;
			rate = POW_CONUT(count,20);

			
			GD_SetGraphDisp( WLD_GRP_WORLD+0, ON );



			GD_SetGraphFade( WLD_GRP_WORLD+0, 128-128*rate );



			SetGameWindowDisp( 0, TRUE );
			SetGameWindowZoomXY( 0, 1-rate, 1+rate );
			SetGameWindowColor( 0, 255-255*rate, 128, 128, 128 );

			rate = POW_CONUT(max(0,count-10),20);
			
			SetGameWindowDisp( 1, TRUE );
			SetGameWindowZoomXY( 1, 1-rate, 1+rate );
			SetGameWindowColor( 1, 255-255*rate, 128, 128, 128 );

			if(count>=30){
				appData.draw_flag=1;

				step = CSL_TERM;
			}
			break;
		case CSL_MIAN:
			count+=1;
			rate = 1-POYO_CONUT(count,15,10);

			SetGameWindowDisp( 1, TRUE );
			SetGameWindowZoomXY( 1, 1-rate, 1+rate );
			SetGameWindowColor( 1, 255*count/15, 128, 128, 128 );

			if(count>=15){
				count=0;
				MUS_SetMouseLayer( 1 );
				step = CSL_COMMAND;

			}
			break;
		case CSL_COMMAND:
			count+=1;
			rate = 1-POYO_CONUT(count,15,10);
			
			SetGameWindowDisp( 0, TRUE );
			SetGameWindowZoomXY( 0, 1-rate, 1+rate );
			SetGameWindowColor( 0, 255*count/15, 128, 128, 128 );

			if( select_no!=-1 ){
				if(select_bak != select_no ){
					SND_Select();
					switch(select_no){
					case 0:	SetGameWindowText( 1, "城の外に出ます。" );	break;

					case 1:	SetGameWindowText( 1, "アイテム一覧を見ます。" );	break;
					case 2:	SetGameWindowText( 1, "キャラクターの能力の確認、及び装備の変更ができます。" );		break;
					case 3:	SetGameWindowText( 1, "傭兵の雇用、転職などを行います。" );	break;
					case 4:	SetGameWindowText( 1, "セーブ／ロードができます。" );	break;
					case 5:
						if(ESC_GetFlag(63)==0){
							if( ESC_GetFlag(70)==1 ){
								SetGameWindowText( 1, "城内を散策します。" );
							}else{
								SetGameWindowText( 1, "軍議を開きます。" );
							}
						}else if( ESC_GetFlag(63)==2 ){
							SetGameWindowText( 1, "エポナの道具屋「グッドピープルズ」に行きます。" );
						}else{
							if(ESC_GetFlag(64)){
								SetGameWindowText( 1, "道具屋に行きます。" );
							}else{
								SetGameWindowText( 1, "道具屋に行きます。" );
							}
						}
						break;
					case 6:
						if(ESC_GetFlag(64)==0){
							if( ESC_GetFlag(70)==1 ){
								SetGameWindowText( 1, "城内を散策します。" );
							}else{
								SetGameWindowText( 1, "軍議を開きます。" );
							}
						}else{
							SetGameWindowText( 1, "武器屋へ向かいます。" );
						}
						break;
					case 7:
						if( ESC_GetFlag(70)==1 ){
							SetGameWindowText( 1, "城内を散策します。" );
						}else{
							SetGameWindowText( 1, "軍議を開きます。" );
						}
						break;
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
						count=0;
						step = CSL_FADE_OUT;
						SND_Ok();
						break;
					case 1:
						count=15;
						step = CSL_ITEM_LIST;
						SND_Ok();
						break;
					case 2:
						count=15;
						step = CSL_BUTAI;
						SND_Ok();
						break;
					case 3:
						count=15;
						step = CSL_HEITAN;
						SND_Ok();
						break;
					case 4:
						count=15;
						step = CSL_SAVE_LOAD;
						SND_Ok();
						break;
					case 5:
						if(ESC_GetFlag(63)==0){
							if( ESC_GetFlag(70) ){
								AVG_StopBGM( 30 );
								_mbscpy( (BYTE*)GoScript, (BYTE*)"sys_jyounai.sdt" );
								NextCaslStep = SYS_BATTLE;
								count=0;
								step = CSL_FADE_OUT;
								SND_Ok();
							}
						}else{
							_mbscpy( (BYTE*)GoScript, (BYTE*)"sys_item00.sdt" );
							NextCaslStep = SYS_BATTLE;
							count=0;
							step = CSL_FADE_OUT;
							SND_Ok();
						}
						break;
					case 6:
						if(ESC_GetFlag(64)==0){
							if( ESC_GetFlag(70) ){
								AVG_StopBGM( 30 );
								_mbscpy( (BYTE*)GoScript, (BYTE*)"sys_jyounai.sdt" );
								NextCaslStep = SYS_BATTLE;
								count=0;
								step = CSL_FADE_OUT;
								SND_Ok();
							}
						}else{
							_mbscpy( (BYTE*)GoScript, (BYTE*)"sys_item01.sdt" );
							NextCaslStep = SYS_BATTLE;
							count=0;
							step = CSL_FADE_OUT;
							SND_Ok();
						}
						break;
					case 7:
						AVG_StopBGM( 30 );
						_mbscpy( (BYTE*)GoScript, (BYTE*)"sys_jyounai.sdt" );
						NextCaslStep = SYS_BATTLE;
						count=0;
						step = CSL_FADE_OUT;
						SND_Ok();
						break;
					}
				}
			}else{
				if(select_bak != select_no ){
					SetGameWindowText( 1, "拠点アヴァロンです。\nメニューを選択してください。" );
				}
				GD_SetGraphDisp( WLD_GRP_SYSTEM+5, FALSE );
			}
			select_bak=select_no;
			break;
		case CSL_SAVE_LOAD:
			count = max(0,count-1);
			rate = 1-POYO_CONUT(count,15,10);

			SetGameWindowDisp( 0, TRUE );
			SetGameWindowZoomXY( 0, 1-rate, 1+rate );
			SetGameWindowColor( 0, 255*count/15, 128, 128, 128 );
			
			i = WLD_SaveLoad( 1, WLD_BMP_SYSTEM, WLD_GRP_WINDW, WLD_BMP_SYSTEM+2, WLD_SPR_CHAR_SP, WLD_LAY_SYSTEM+2, 2, SYS_CASTLE );
			
			switch( i ){
				case 0:
					break;
				case 1:
					count=0;
					MUS_SetMouseLayer( 1 );
					step = CSL_COMMAND;
					break;
				default:
				case 2:

					NextCaslStep = i-2;
					count=0;
					step = CSL_FADE_OUT;
					break;
			}
			break;
		case CSL_HEITAN:
			count = max(0,count-1);
			rate = 1-POYO_CONUT(count,15,10);

			SetGameWindowDisp( 0, TRUE );
			SetGameWindowZoomXY( 0, 1-rate, 1+rate );
			SetGameWindowColor( 0, 255*count/15, 128, 128, 128 );
			
			if( WLD_Heitan( 1, WLD_BMP_SYSTEM, WLD_GRP_WINDW, WLD_BMP_SYSTEM+2, WLD_SPR_CHAR_SP, WLD_LAY_SYSTEM+2, 2 ) ){
				count=0;
				MUS_SetMouseLayer( 1 );
				step = CSL_COMMAND;
			}
			break;
		case CSL_ITEM_LIST:
			count = max(0,count-1);
			rate = 1-POYO_CONUT(count,15,10);

			SetGameWindowDisp( 0, TRUE );
			SetGameWindowZoomXY( 0, 1-rate, 1+rate );
			SetGameWindowColor( 0, 255*count/15, 128, 128, 128 );
			
			if( WLD_ItemList( 1, WLD_BMP_SYSTEM, WLD_GRP_WINDW, WLD_BMP_SYSTEM+2, WLD_SPR_CHAR_SP, WLD_LAY_SYSTEM+2, 2 ) ){
				count=0;
				MUS_SetMouseLayer( 1 );
				step = CSL_COMMAND;
			}
			break;

		case CSL_BUTAI:
			count = max(0,count-1);
			rate = 1-POYO_CONUT(count,15,10);

			SetGameWindowDisp( 0, TRUE );
			SetGameWindowZoomXY( 0, 1-rate, 1+rate );
			SetGameWindowColor( 0, 255*count/15, 128, 128, 128 );
			
			if( WLD_Butai( 1, WLD_BMP_SYSTEM, WLD_GRP_WINDW, WLD_BMP_SYSTEM+2, WLD_SPR_CHAR_SP, WLD_LAY_SYSTEM+2, 2 ) ){
				count=0;
				MUS_SetMouseLayer( 1 );
				step = CSL_COMMAND;
			}
			break;
	}

	if(appData.debug_mouse!=-1){
		if(KeyCond.trg.f1){

			for(i=0;i<32;i++){
				if(CharParam[i].flag){
					int lv = CharParam[i].lv;
					for(j=lv;j<lv+10;j++){
						CharParam[i].SetLvUp(TRUE);
					}
				}
			}
			PlayerParam.money+=100000;
		}
		if(KeyCond.trg.f2){
		}
		if(KeyCond.trg.f3){
		}

		if(KeyCond.btn.z){
		}
	}
	
	return ret;
}