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

#include "GM_Main.h"
#include "GM_Battle.h"
#include "GM_Avg.h"
#include "GM_Char.h"
#include "GM_Magic.h"

#include "GM_CharThink.h"

#include <math.h>






SMAP_FORMAT			MapData;
BTL_MAP_CONTROL		MapControl;


typedef struct{
	int		flag;	
	int		set;	
	int		x,y;	
}START_CHAR;


typedef struct{
	int		btype;	
	int		did_ok;
	int		btime;	
	int		bcount;	
	int		bx, by;	
	int		brange;	

	int		bcno;	
	int		all_atack;
	int		all_atack_back;

	int		win_flag;	
	
	float	px, py;

	START_CHAR	sc[32];
	int		select;

	int		unit_list[10];
	int		ucount;	
	int		umax;	

	int		scount;		
	int		smax;		
	int		fix_count;	
}START_POINT;
START_POINT		StartPoint;



int	BTL_ReleaseBattleMap( void )
{
	int		i;
	GD_ResetGraph( GRP_MAP_BACK );
	GD_ResetGraph( GRP_MAP_MAIN );
	if(MapData.head.obj_max){
		for(i=0;i<MapData.head.obj_max;i++){
			if(MapData.object[i].flag){
				GD_ResetGraph( GRP_MAP_OBJECT+i );
			}
		}
	}
	for(i=0;i<32;i++){
		if(MapData.evt[i].flag){
			ResetMapObj( i,0 );
		}
	}


	GFree(MapData.back);
	GFree(MapData.map);
	GFree(MapData.object);
	GFree(MapData.sinkin);
	GFree(MapData.attr);
	GFree(MapData.hight);

	ZeroMemory( &MapData, sizeof(SMAP_FORMAT) );

	GD_ReleaseTexture(BMP_MAP_MAIN);
	GD_ReleaseSprite(SPR_MAP_OBJECT);

	GFree(MapControl.aria_table);
	
	ResetBackUpMapChar();
	return 1;
}

int	LoadMapBBM_ANI(  int stage_no, int ver_no  )
{
	char	bak_fname[MAX_PATH];
	char	bbm_fname[MAX_PATH];
	char	obj_fname[MAX_PATH];
	int		bak_flag=ALPHA_1555|DITHER_OFF;


	wsprintf( bak_fname, "Stage_%02d_%02x_bak.bbm", stage_no, ver_no );
	wsprintf( bbm_fname, "Stage_%02d_%02x.bbm", stage_no, ver_no );
	wsprintf( obj_fname, "Stage_%02d_%02x_obj.ani", stage_no, ver_no );

	wsprintf( bak_tone_fname, "Stage_%02d_%02x.amp", stage_no, ver_no );	

	int	lmode;
	if(Avg.demo){
		lmode = 0;
	}else{
		lmode = DrawSetting.low_level;
	}
	if( PAC_CheckFile( "map", bak_fname ) ){
		bak_flag = ALPHA_4444;
		GD_LoadTextureBBM( BMP_MAP_BACK, bak_fname, bak_flag, lmode, FALSE, eff_tone_fname_b,NULL, BackStruct.chroma_back, "map" );
	}
	if(lmode){
		GD_LoadTextureBBM( BMP_MAP_MAIN, bbm_fname, bak_flag, TRUE, DrawSetting.mip_map, eff_tone_fname_b,NULL, BackStruct.chroma_back, "map" );
	}else{
		GD_LoadTextureBBM( BMP_MAP_MAIN, bbm_fname, bak_flag, FALSE, DrawSetting.mip_map, eff_tone_fname_b,NULL, BackStruct.chroma_back, "map" );
	}
	if( PAC_CheckFile( "map", obj_fname ) ){
		GD_LoadSprite( SPR_MAP_OBJECT, obj_fname, ALPHA_4444|DITHER_OFF, lmode, DrawSetting.mip_map, eff_tone_fname_b,NULL, BackStruct.chroma_back, "map" );
	}

	return TRUE;
}


BOOL BTL_LoadBattleMap( int stage_no, int ver_no  )
{
	int		pos=0, i;
	char	map_fname[MAX_PATH];
	BYTE	*buf=NULL, *ptr;
	BTL_ReleaseBattleMap();

	LoadMapBBM_ANI( stage_no, ver_no );

	wsprintf( map_fname, "Stage_%02d_%02x.map", stage_no, ver_no );
	if( !PAC_LoadFile( "map", map_fname, &buf ) ){
		DebugBox( NULL, "マップファイルの読み込みに失敗しました[%s]", map_fname ); 
		return FALSE;
	}
	ptr=buf;

	MapData.head = *(SMAP_FORMAT_HEADER*)buf;	ptr+=sizeof(MapData.head);
	
	MapData.stage_no = stage_no;
	MapData.ver_no   = ver_no;

	GD_GetTextureWH( BMP_MAP_BACK, &MapData.bw, &MapData.bh );

	MapData.back = NULL;
	MapData.map  = NULL;

	GD_SetGraph( GRP_MAP_BACK, BMP_MAP_BACK, LAY_MAP_BACK, FALSE );	
	GD_SetGraph( GRP_MAP_MAIN, BMP_MAP_MAIN, LAY_MAP_MAIN, FALSE );	
	if(MapData.head.obj_max){
		MapData.object = (OBJECT_FORMAT*)GAlloc( sizeof(OBJECT_FORMAT)*MapData.head.obj_max );
		CopyMemory( (BYTE*)MapData.object, ptr, sizeof(OBJECT_FORMAT)*MapData.head.obj_max );	ptr+=sizeof(OBJECT_FORMAT)*MapData.head.obj_max;
		for(i=0;i<MapData.head.obj_max;i++){
			if(MapData.object[i].flag){
				if(MapData.object[i].flag==2) MapData.object[i].flag=1;
				GD_SetSprite( GRP_MAP_OBJECT+i, SPR_MAP_OBJECT, MapData.object[i].obj_no, TRUE, LAY_MAP_OBJECT+MapData.object[i].y, FALSE, TRUE, 0, FALSE );
			}
		}
	}

	if(MapData.head.sgrid){
		MapControl.aria_table  = (BYTE*)GAlloc((MapData.head.w/MapData.head.sgrid) * (MapData.head.h/MapData.head.sgrid));
		MapData.sinkin = (BYTE*)GAlloc((MapData.head.w/MapData.head.sgrid) * (MapData.head.h/MapData.head.sgrid));
		CopyMemory( (BYTE*)MapData.sinkin, ptr, MapData.head.w/MapData.head.sgrid * MapData.head.h/MapData.head.sgrid );	ptr+=MapData.head.w/MapData.head.sgrid * MapData.head.h/MapData.head.sgrid;
	}
	MapData.attr  = NULL;
	MapData.hight = NULL;
	if(MapData.head.agrid){
		MapData.attr   = (BYTE*)GAlloc(MapData.head.w/MapData.head.agrid * MapData.head.h/MapData.head.agrid);
		MapData.hight  = (BYTE*)GAlloc(MapData.head.w/MapData.head.agrid * MapData.head.h/MapData.head.agrid);
		CopyMemory( (BYTE*)MapData.attr,  ptr, MapData.head.w/MapData.head.agrid * MapData.head.h/MapData.head.agrid );	ptr+=MapData.head.w/MapData.head.agrid * MapData.head.h/MapData.head.agrid;
		CopyMemory( (BYTE*)MapData.hight, ptr, MapData.head.w/MapData.head.agrid * MapData.head.h/MapData.head.agrid );	ptr+=MapData.head.w/MapData.head.agrid * MapData.head.h/MapData.head.agrid;
	}

	return TRUE;
}

BOOL SetMapObjEx( int ono, char *fname, char *pack, int x, int y, int play, int loop )
{
	if(ono<0 || ono>=16){
		DebugBox(NULL,"おっと");
	}
	MapData.evt[ono].flag   = 1;
	MapData.evt[ono].ani_no = 0;
	MapData.evt[ono].sp_no = 0;
	MapData.evt[ono].play   = play;
	MapData.evt[ono].x = x;
	MapData.evt[ono].y = y;
	MapData.evt[ono].zoom = 1.0f;

	MapData.evt[ono].param = BLEND_ALPHA;
	MapData.evt[ono].rate  = 255;

	MapData.evt[ono].fade  = 0;
	
	GD_LoadSprite( SPR_MAP_EVT+ono, fname, ALPHA_4444|DITHER_OFF, DrawSetting.low_level, DrawSetting.mip_map, eff_tone_fname_b,NULL, BackStruct.chroma_back, pack );
	GD_SetSprite( GRP_MAP_EVT+ono, SPR_MAP_EVT+ono, 0, play, LAY_MAP_OBJECT+y, TRUE, TRUE, loop );
	return 1;
}

BOOL SetMapObj( int ono, int ani_no, int sp_no, int x, int y, int play, int loop )
{
	char	evt_fname[256];

	if(ono<0 || ono>=16){
		DebugBox(NULL,"おっと");
	}
	MapData.evt[ono].flag   = 1;
	MapData.evt[ono].ani_no = ani_no;
	MapData.evt[ono].sp_no = sp_no;
	MapData.evt[ono].play   = play;
	MapData.evt[ono].x = x;
	MapData.evt[ono].y = y;
	MapData.evt[ono].zoom = 1.0f;

	MapData.evt[ono].param = BLEND_ALPHA;
	MapData.evt[ono].rate  = 255;

	MapData.evt[ono].fade  = 0;

	if(ani_no==-1){
		switch( sp_no ){
			case 0:
				if(DrawSetting.low_level)	GD_LoadTexture( BMP_MAP_EVT, "魔方陣3.tga", ALPHA_4444 );
				else						GD_LoadTexture( BMP_MAP_EVT, "魔方陣4.tga", ALPHA_4444 );
				GD_SetGraph( GRP_MAP_EVT+ono, BMP_MAP_EVT, LAY_MAP_OBJECT, FALSE );
				GD_SetGraphBlendParam( GRP_MAP_EVT+ono, BLEND_ADD );
				GD_SetGraphScaleParam( GRP_MAP_EVT+ono, SCALE_HIGHT );
				break;
			case 1:
				GD_SetSprite( GRP_MAP_EVT+ono, SPR_MAP_OBJECT, 4, play, LAY_MAP_OBJECT+y, FALSE, TRUE, loop );
				GD_SetGraphScaleParam( GRP_MAP_EVT+ono, SCALE_HIGHT );
				break;
			case 2:
				GD_LoadTexture( BMP_MAP_EVT, "波.tga", ALPHA_4444 );

				GD_SetGraph( GRP_MAP_EVT+ono, BMP_MAP_EVT, LAY_MAP_OBJECT, FALSE );
				GD_SetGraphScaleParam( GRP_MAP_EVT+ono, SCALE_HIGHT );
				GD_SetGraphRect( GRP_MAP_EVT+ono, x, y,  0, 0, 480, 128 );
				break;
			case 3:
				GD_LoadTexture( BMP_MAP_EVT+0, "ラス穴00.tga", ALPHA_4444 );
				GD_LoadTexture( BMP_MAP_EVT+1, "ラス穴01.tga", ALPHA_1555 );

				GD_SetGraph( GRP_MAP_EVT+0, BMP_MAP_EVT+0, LAY_EFFECT+1, TRUE );
				GD_SetGraph( GRP_MAP_EVT+1, BMP_MAP_EVT+0, LAY_EFFECT+1, TRUE );
				GD_SetGraph( GRP_MAP_EVT+2, BMP_MAP_EVT+0, LAY_EFFECT+1, TRUE );
				GD_SetGraph( GRP_MAP_EVT+3, BMP_MAP_EVT+0, LAY_EFFECT+1, TRUE );

				GD_SetGraph( GRP_MAP_EVT+4, BMP_MAP_EVT+1, LAY_EFFECT, TRUE );
				GD_SetGraph( GRP_MAP_EVT+5, BMP_MAP_EVT+1, LAY_EFFECT, TRUE );
				GD_SetGraph( GRP_MAP_EVT+6, BMP_MAP_EVT+1, LAY_EFFECT, TRUE );
				GD_SetGraph( GRP_MAP_EVT+7, BMP_MAP_EVT+1, LAY_EFFECT, TRUE );
				GD_SetGraphBrightNo( GRP_MAP_EVT+0, 1 );
				GD_SetGraphBrightNo( GRP_MAP_EVT+1, 1 );
				GD_SetGraphBrightNo( GRP_MAP_EVT+2, 1 );
				GD_SetGraphBrightNo( GRP_MAP_EVT+3, 1 );

				GD_SetGraphBrightNo( GRP_MAP_EVT+4, 1 );
				GD_SetGraphBrightNo( GRP_MAP_EVT+5, 1 );
				GD_SetGraphBrightNo( GRP_MAP_EVT+6, 1 );
				GD_SetGraphBrightNo( GRP_MAP_EVT+7, 1 );

				GD_SetGraphBlendParam( GRP_MAP_EVT+0, BLEND_ADD );
				GD_SetGraphBlendParam( GRP_MAP_EVT+1, BLEND_ADD );
				GD_SetGraphBlendParam( GRP_MAP_EVT+2, BLEND_ADD );
				GD_SetGraphBlendParam( GRP_MAP_EVT+3, BLEND_ADD );

				GD_SetGraphBlendParam( GRP_MAP_EVT+4, BLEND_ADD );
				GD_SetGraphBlendParam( GRP_MAP_EVT+5, BLEND_ADD );
				GD_SetGraphBlendParam( GRP_MAP_EVT+6, BLEND_ADD );
				GD_SetGraphBlendParam( GRP_MAP_EVT+7, BLEND_ADD );

				GD_SetGraphScaleParam( GRP_MAP_EVT+0, SCALE_HIGHT );
				GD_SetGraphScaleParam( GRP_MAP_EVT+1, SCALE_HIGHT );
				GD_SetGraphScaleParam( GRP_MAP_EVT+2, SCALE_HIGHT );
				GD_SetGraphScaleParam( GRP_MAP_EVT+3, SCALE_HIGHT );

				GD_SetGraphSrcRect( GRP_MAP_EVT+0, 0, 0, 230, 512 );
				GD_SetGraphSrcRect( GRP_MAP_EVT+1, 0, 0, 230, 512 );
				GD_SetGraphSrcRect( GRP_MAP_EVT+2, 0, 0, 230, 512 );
				GD_SetGraphSrcRect( GRP_MAP_EVT+3, 0, 0, 230, 512 );
				break;
		}
	}else{
		wsprintf( evt_fname, "Stage_%02d_%02x_EVT_%02d.ani", MapData.stage_no, MapData.ver_no, ani_no );
		GD_LoadSprite( SPR_MAP_EVT+ono, evt_fname, ALPHA_4444|DITHER_OFF, DrawSetting.low_level, DrawSetting.mip_map, eff_tone_fname_b,NULL, BackStruct.chroma_back, "map" );
		GD_SetSprite( GRP_MAP_EVT+ono, SPR_MAP_EVT+ono, sp_no, play, LAY_MAP_OBJECT+y, TRUE, TRUE, loop );
	}

	return 1;
}

BOOL SetMapObjNoLoad( int ono, int ono2, int sp_no, int x, int y, int play, int loop )
{
	if(ono<0 || ono>=16){
		DebugBox(NULL,"おっと");
	}
	MapData.evt[ono].flag   = 1;
	MapData.evt[ono].ani_no = MapData.evt[ono2].ani_no;
	MapData.evt[ono].sp_no = sp_no;
	MapData.evt[ono].play   = play;
	MapData.evt[ono].x = x;
	MapData.evt[ono].y = y;
	MapData.evt[ono].zoom = 1.0f;

	MapData.evt[ono].rev_param = 0;
	MapData.evt[ono].param = BLEND_ALPHA;
	MapData.evt[ono].rate  = 255;

	MapData.evt[ono].fade  = 0;

	GD_SetSprite( GRP_MAP_EVT+ono, SPR_MAP_EVT+ono2, sp_no, play, LAY_MAP_OBJECT+y, TRUE, TRUE, loop );
	return 1;
}

BOOL SetMapObjRevParam( int ono, int rev )
{
	if(ono<0 || ono>=16){
		DebugBox(NULL,"おっと");
	}
	MapData.evt[ono].rev_param = rev;
	return 1;

}

BOOL SetMapObjLayer( int ono, int layer )
{
	if(ono<0 || ono>=16){
		DebugBox(NULL,"おっと");
	}
	MapData.evt[ono].layer = layer;
	return 1;
}
BOOL SetMapObjMove( int ono, int tx, int ty, int frame, int type )
{
	if(ono<0 || ono>=16){
		DebugBox(NULL,"おっと");
	}
	MapData.evt[ono].tx   = tx;
	MapData.evt[ono].ty   = ty;
	MapData.evt[ono].tcnt = 0;
	MapData.evt[ono].tmax = frame;

	MapData.evt[ono].ttype = type;
	return 1;
}


BOOL SetMapObjZoom( int ono, int zoom )
{
	if(ono<0 || ono>=16){
		DebugBox(NULL,"おっと");
	}
	MapData.evt[ono].zoom = zoom/4096.0f;
	return 1;
}
BOOL SetMapObjParam( int ono, int param, int rate )
{
	if(ono<0 || ono>=16){
		DebugBox(NULL,"おっと");
	}
	MapData.evt[ono].param = param;
	MapData.evt[ono].rate  = rate;
	return 1;
}

BOOL PlayMapObj( int ono, int sp_no, int play, int loop )
{
	if(ono<0 || ono>=16){
		DebugBox(NULL,"おっと");
	}
	GD_SetSpritePlay( GRP_MAP_EVT+ono, SPR_MAP_EVT+ono, sp_no, play, TRUE, loop );
	return 1;
}

BOOL ResetMapObj( int ono, int fade )
{
	if(ono<0 || ono>=16){
		DebugBox(NULL,"おっと");
	}
	if(fade){
		MapData.evt[ono].fade_cnt=0;
		MapData.evt[ono].fade  = fade;
	}else{
		GD_ResetGraph( GRP_MAP_EVT+ono );
		GD_ReleaseSprite( SPR_MAP_EVT+ono );
		ZeroMemory( &MapData.evt[ono], sizeof(MapData.evt[ono]) );
	}
	return 1;
}



BOOL SetBrightBattleMap( int r, int g, int b, int frame, int type )
{
	if( (MapControl.r == r && MapControl.g == g && MapControl.b == b && MapControl.fd_type==type) || frame==0 ){
		MapControl.fd_type=0;
		MapControl.fd_count=0;
		MapControl.fd_max=0;
		MapControl.er = MapControl.sr = MapControl.r = r;
		MapControl.eg = MapControl.sg = MapControl.g = g;
		MapControl.eb = MapControl.sb = MapControl.b = b;
	}else{
		MapControl.fd_type = type;
		MapControl.fd_count = 0;
		MapControl.fd_max = frame;

		MapControl.sr = MapControl.r;
		MapControl.sg = MapControl.g;
		MapControl.sb = MapControl.b;

		MapControl.er = r;
		MapControl.eg = g;
		MapControl.eb = b;
	}
	return 1;
}


BOOL SetScrolBattleMap( float mcx, float mcy, float zoom, int frame, int type )
{
	if(zoom<0.0f){
		zoom = MapControl.ezoom;
	}
	float	zoom_lmt, zoom_lmt2;
	zoom_lmt = max(800.0f/MapData.head.w, 600.0f/MapData.head.h);
	if( !(MapData.bw==0 || MapData.bh==0) ){
		zoom_lmt2 = (max( 800.0f/MapData.bw, 600.0f/MapData.bh )*4-2)/2;
		zoom_lmt = max( zoom_lmt, zoom_lmt2 );
	}

	zoom_lmt = max( zoom_lmt, 0.5f );
	zoom = max( zoom, zoom_lmt );
	float mx = mcx-400/zoom + (800/zoom-800/MapControl.zoom)/2;
	float my = mcy-300/zoom + (600/zoom-600/MapControl.zoom)/2;


	if( (MapControl.px == mx && MapControl.py == my && MapControl.zoom==zoom) || frame==0 ){
		MapControl.sc_type = 0;
		MapControl.sc_count = 0;
		MapControl.sc_max = 0;

		MapControl.epx = MapControl.spx = MapControl.px = mx;
		MapControl.epy = MapControl.spy = MapControl.py = my;
		MapControl.ezoom = MapControl.szoom = MapControl.zoom = zoom;

		MapControl.ecno = 0;
	}else{
		MapControl.sc_type = type;
		MapControl.sc_count = 0;
		MapControl.sc_max = frame;

		MapControl.spx = MapControl.px;
		MapControl.spy = MapControl.py;

		MapControl.epx = mx;
		MapControl.epy = my;

		MapControl.szoom = MapControl.zoom;
		MapControl.ezoom = zoom;

		MapControl.ecno = 0;
	}
	return 1;
}

BOOL SetScrolBattleMapChar( int no, float zoom, int frame, int type )
{
	if(zoom<=0.0f){
		zoom = MapControl.ezoom;
	}
	zoom = max( 0.1f, zoom );
	float	zoom_lmt;
	zoom_lmt = max(800.0f/MapData.head.w, 600.0f/MapData.head.h);
	zoom_lmt = max( zoom_lmt, 0.5f );
	zoom = max( zoom, zoom_lmt );

	MapControl.zoom = max( 0.1f, MapControl.zoom );
	float mx = CharControl[no].px-400/zoom + (800/zoom-800/MapControl.zoom)/2;
	float my = CharControl[no].py-300/zoom + (600/zoom-600/MapControl.zoom)/2;

	MapControl.sc_type = type;
	MapControl.sc_count = 0;
	MapControl.sc_max = frame;

	MapControl.spx = MapControl.px;
	MapControl.spy = MapControl.py;

	MapControl.epx = mx;
	MapControl.epy = my;

	MapControl.szoom = MapControl.zoom;
	MapControl.ezoom = zoom;

	MapControl.ecno = no;
	return 1;


}
extern int	Oberisuku=0;


void RenewBattleMap( void )
{
	int	mx=GameKey.mx, my=GameKey.my;
	int	i,j;



	switch(MapControl.sc_mode){
		case 0:
		
			if(MapControl.sc_max==0){

				if(MapControl.battle_flag){
					if(GameKey.pup){
						if(MapControl.zoom<1.0f){
							SetScrolBattleMap( MapControl.px+mx/MapControl.zoom, MapControl.py+my/MapControl.zoom, 1.0f, 10, 2 );
						}else if(MapControl.zoom<2.0f){
							SetScrolBattleMap( MapControl.px+mx/MapControl.zoom, MapControl.py+my/MapControl.zoom, 2.0f, 10, 2 );
						}

					}else if(GameKey.pdown){
						if(MapControl.zoom>1.0f){
							SetScrolBattleMap( MapControl.px+mx/MapControl.zoom, MapControl.py+my/MapControl.zoom, 1.0f, 10, 2 );
						}else if(MapControl.zoom>0.5f){
							SetScrolBattleMap( MapControl.px+mx/MapControl.zoom, MapControl.py+my/MapControl.zoom, 0.5f, 10, 2 );
						}
					}
				}else{
					if(GameKey.pup){
						if(MapControl.zoom<1.0f){
							SetScrolBattleMap( MapControl.px+400/MapControl.zoom, MapControl.py+300/MapControl.zoom, 1.0f, 10, 2 );
						}else if(MapControl.zoom<=1.0f){
							SetScrolBattleMap( MapControl.px+400/MapControl.zoom, MapControl.py+300/MapControl.zoom, 2.0f, 10, 2 );
						}

					}else if(GameKey.pdown){
						if(MapControl.zoom>1.0f){
							SetScrolBattleMap( MapControl.px+400/MapControl.zoom, MapControl.py+300/MapControl.zoom, 1.0f, 10, 2 );
						}else if(MapControl.zoom>0.5f){
							SetScrolBattleMap( MapControl.px+400/MapControl.zoom, MapControl.py+300/MapControl.zoom, 0.5f, 10, 2 );
						}
					}
				}

				if( !(mx<0 || mx>=800 || my<0 || my>=600) && appData.Active ){
					if(my<0){
						my=my;
					}
					if( !(100<=mx && mx<700 && 100<=my && my<500) ){
						if( !(50<=mx && mx<750 && 50<=my && my<550) ){
							MapControl.px = LIM( MapControl.px + (mx-400)/25/MapControl.zoom, 0, MapData.head.w - 800/MapControl.zoom );
							MapControl.py = LIM( MapControl.py + (my-300)/25/MapControl.zoom, 0, MapData.head.h - 600/MapControl.zoom );
						}else{
							MapControl.px = LIM( MapControl.px + (mx-400)/50/MapControl.zoom, 0, MapData.head.w - 800/MapControl.zoom );
							MapControl.py = LIM( MapControl.py + (my-300)/50/MapControl.zoom, 0, MapData.head.h - 600/MapControl.zoom );
						}
					}
				}
			}
			break;
		case 1:
			break;
	}

	if(MapControl.sc_max){
		MapControl.sc_count++;
		if(MapControl.sc_count <= MapControl.sc_max){
			float	rate;
			switch(MapControl.sc_type&0xff){
				case 0:
					rate = (float)MapControl.sc_count/MapControl.sc_max;
					break;
				case 1:
					rate = (float)(MapControl.sc_count*MapControl.sc_count)/(MapControl.sc_max*MapControl.sc_max);
					break;
				case 2:
					rate = (float)(MapControl.sc_max - MapControl.sc_count);
					rate = (rate*rate)/(MapControl.sc_max*MapControl.sc_max);
					rate = 1.0f - rate;
					break;
				case 3:
					rate = POYO_CONUT( MapControl.sc_count, MapControl.sc_max, MapControl.sc_max-5 );
					break;
			}
			MapControl.zoom = float(MapControl.szoom * pow( MapControl.ezoom/MapControl.szoom, rate ));

			if(MapControl.sc_type&0x100){
				MapControl.epx = CharControl[MapControl.ecno].px-400/MapControl.zoom + (800/MapControl.zoom-800/MapControl.szoom)/2;
				MapControl.epy = CharControl[MapControl.ecno].py-300/MapControl.zoom + (600/MapControl.zoom-600/MapControl.szoom)/2;
			}
			MapControl.px = MapControl.spx - (MapControl.spx - MapControl.epx)*rate - (800/MapControl.zoom-800/MapControl.szoom)/2;
			MapControl.py = MapControl.spy - (MapControl.spy - MapControl.epy)*rate - (600/MapControl.zoom-600/MapControl.szoom)/2;

			if(MapControl.sc_count == MapControl.sc_max && !(MapControl.sc_type&0x100) ){
				MapControl.sc_count=0;
				MapControl.sc_max=0;
			}
		}else{
			if(MapControl.sc_type&0x100){
				MapControl.epx = CharControl[MapControl.ecno].px-400/MapControl.zoom;
				MapControl.epy = CharControl[MapControl.ecno].py-300/MapControl.zoom;

				MapControl.px = MapControl.epx;
				MapControl.py = MapControl.epy;
			}
		}
	}
	MapControl.px = LIM( MapControl.px, 0, MapData.head.w - 800/MapControl.zoom );
	MapControl.py = LIM( MapControl.py, 0, MapData.head.h - 600/MapControl.zoom );


	if(MapControl.fd_max){
		MapControl.fd_count++;
		if(MapControl.fd_count <= MapControl.fd_max){
			MapControl.r = MapControl.sr - (MapControl.sr - MapControl.er)*MapControl.fd_count/MapControl.fd_max;
			MapControl.g = MapControl.sg - (MapControl.sg - MapControl.eg)*MapControl.fd_count/MapControl.fd_max;
			MapControl.b = MapControl.sb - (MapControl.sb - MapControl.eb)*MapControl.fd_count/MapControl.fd_max;

			if(MapControl.fd_count >= MapControl.fd_max){
				MapControl.fd_count=0;
				MapControl.fd_max=0;
			}
		}
	}
	MapControl.r = LIM( MapControl.r, 0, 255 );
	MapControl.g = LIM( MapControl.g, 0, 255 );
	MapControl.b = LIM( MapControl.b, 0, 255 );



	float	x,y;
	if(Avg._Radeon8500){
		x = (int)(MapControl.px-ShakeDx);
		y = (int)(MapControl.py-ShakeDy);
	}else{
		x = MapControl.px-ShakeDx;
		y = MapControl.py-ShakeDy;
	}


	GD_SetGraphDisp( GRP_MAP_BACK, ON );




	if(MapData.head.w>800/MapControl.zoom && MapData.head.h>600/MapControl.zoom){
		GD_SetGraphDstMove( GRP_MAP_BACK, -x*(MapData.bw-800/((MapControl.zoom*2+2)/4))/(MapData.head.w-800/MapControl.zoom), -y*(MapData.bh-600/((MapControl.zoom*2+2)/4))/(MapData.head.h-600/MapControl.zoom) ); 
		GD_SetGraphZoom( GRP_MAP_BACK, (MapControl.zoom*2+2)/4 );
		GD_SetGraphCenter( GRP_MAP_BACK, 0, 0 );
		if(!DrawSetting.low_level)	GD_SetGraphScaleParam( GRP_MAP_BACK, SCALE_HIGHT );
	}

	GD_SetGraphDisp( GRP_MAP_MAIN, ON );
	GD_SetGraphCenter( GRP_MAP_MAIN, 0, 0 );
	GD_SetGraphDstMove( GRP_MAP_MAIN, -x, -y );
	GD_SetGraphZoom( GRP_MAP_MAIN, MapControl.zoom );
	if(!DrawSetting.low_level)
		GD_SetGraphScaleParam( GRP_MAP_MAIN, SCALE_HIGHT );
	GD_SetGraphBlendParam( GRP_MAP_MAIN, BLEND_ALPHA );




	for(i=0;i<MapData.head.obj_max;i++){
		if(MapData.object[i].flag){

			GD_SetGraphDisp( GRP_MAP_OBJECT+i, ON );

			GD_SetGraphDstMove( GRP_MAP_OBJECT+i, (MapData.object[i].x-x)*MapControl.zoom, (MapData.object[i].y-y)*MapControl.zoom );
			GD_SetGraphZoom( GRP_MAP_OBJECT+i, MapControl.zoom );
			if(!DrawSetting.low_level)
				GD_SetGraphScaleParam( GRP_MAP_OBJECT+i, SCALE_HIGHT );


		}
	}
	int	alpha=0;
	Oberisuku=0;

	for(i=0;i<32;i++){
		if(MapData.evt[i].flag){
			GD_SetGraphDisp( GRP_MAP_EVT+i, ON );
			if(MapData.evt[i].ani_no==-1){
				switch( MapData.evt[i].sp_no ){
				case 0:
					GD_SetGraphRoll2( GRP_MAP_EVT+i, (MapData.evt[i].x-x)*MapControl.zoom, (MapData.evt[i].y-y)*MapControl.zoom, 
						1050*MapControl.zoom, 550*MapControl.zoom, GlobalCount/4.0f );
					
					alpha=32+STD_LimitLoop( GlobalCount*2, 128)/2;
					if(MapControl.battle_flag){
						Oberisuku=0;
						for(j=0;j<CHAR_CONTROL_MAX;j++){
							if(!CharControl[j].flag)continue;
							if(CharParam[CharControl[j].cno].did)continue;
							if(CharParam[CharControl[j].cno].job==100){
								Oberisuku++;
							}
						}

						GD_SetGraphAlpha( GRP_MAP_EVT+i, alpha*Oberisuku/8 );
					}else{
						GD_SetGraphAlpha( GRP_MAP_EVT+i, alpha );
					}
					break;
				case 1:
					GD_SetGraphDstMove( GRP_MAP_EVT+i, (MapData.evt[i].x-x)*MapControl.zoom, (MapData.evt[i].y-y)*MapControl.zoom );
					GD_SetGraphZoom( GRP_MAP_EVT+i, MapControl.zoom );
					GD_SetGraphFade( GRP_MAP_EVT+i, 127+STD_LimitLoop( GlobalCount/2+64-i*16, 64) );
					break;
				case 2:
					{
						float	rate = (GlobalCount+i*50)%384/256.0f;
						if(rate<=1.0f){
							GD_SetGraphDstRect( GRP_MAP_EVT+i, (MapData.evt[i].x-x-10*rate)*MapControl.zoom, (MapData.evt[i].y-y-128*(1.0f-(1.0f-rate)*(1.0f-rate)))*MapControl.zoom, 
																(480+20*rate)*MapControl.zoom, (128+64*rate)*MapControl.zoom );
							GD_SetGraphAlpha( GRP_MAP_EVT+i, 255*rate );
						}else{
							rate = (rate-1.0f)*2;
						
							GD_SetGraphDstRect( GRP_MAP_EVT+i, (MapData.evt[i].x-x-10)*MapControl.zoom, (MapData.evt[i].y-y-128+16*rate)*MapControl.zoom, 
																(480+20)*MapControl.zoom, (128+64-64*rate)*MapControl.zoom );

							GD_SetGraphAlpha( GRP_MAP_EVT+i, 255*(1.0f-rate) );
						}
					}
					break;
				case 3:
					MapData.evt[i].x++;
					int	rate;
					rate = MapData.evt[i].x*4;

					GD_SetGraphAlpha( GRP_MAP_EVT+0, rate );
					GD_SetGraphAlpha( GRP_MAP_EVT+1, rate );
					GD_SetGraphAlpha( GRP_MAP_EVT+2, rate );
					GD_SetGraphAlpha( GRP_MAP_EVT+3, rate );

					rate = MapData.evt[i].x*8;

					GD_SetGraphDstRect( GRP_MAP_EVT+0,   0, -(rate%600),     400, 600 );
					GD_SetGraphDstRect( GRP_MAP_EVT+1,   0, -(rate%600)+600, 400, 600 );
					GD_SetGraphDstRect( GRP_MAP_EVT+2, 400, -(rate%600),     400, 600 );
					GD_SetGraphDstRect( GRP_MAP_EVT+3, 400, -(rate%600)+600, 400, 600 );

					rate = max(400, MapData.evt[i].x)-400;


					rate = rate*rate/100;


					GD_SetGraphDstRect( GRP_MAP_EVT+4, 400-rate, -(MapData.evt[i].x%600),     rate, 600 );
					GD_SetGraphDstRect( GRP_MAP_EVT+5, 400-rate, -(MapData.evt[i].x%600)+600, rate, 600 );
					GD_SetGraphDstRect( GRP_MAP_EVT+6, 400, -(MapData.evt[i].x%600),     rate, 600 );
					GD_SetGraphDstRect( GRP_MAP_EVT+7, 400, -(MapData.evt[i].x%600)+600, rate, 600 );

					rate = (max(800, MapData.evt[i].x)-800);

					GD_SetGraphColorNo( GRP_MAP_EVT+4, 0, 255, rate, rate, rate );
					GD_SetGraphColorNo( GRP_MAP_EVT+4, 1, 255, 255, 255, 255 );
					GD_SetGraphColorNo( GRP_MAP_EVT+4, 2, 255, rate, rate, rate );
					GD_SetGraphColorNo( GRP_MAP_EVT+4, 3, 255, 255, 255, 255 );

					GD_SetGraphColorNo( GRP_MAP_EVT+5, 0, 255, rate, rate, rate );
					GD_SetGraphColorNo( GRP_MAP_EVT+5, 1, 255, 255, 255, 255 );
					GD_SetGraphColorNo( GRP_MAP_EVT+5, 2, 255, rate, rate, rate );
					GD_SetGraphColorNo( GRP_MAP_EVT+5, 3, 255, 255, 255, 255 );

					GD_SetGraphColorNo( GRP_MAP_EVT+6, 0, 255, 255, 255, 255 );
					GD_SetGraphColorNo( GRP_MAP_EVT+6, 1, 255, rate, rate, rate );
					GD_SetGraphColorNo( GRP_MAP_EVT+6, 2, 255, 255, 255, 255 );
					GD_SetGraphColorNo( GRP_MAP_EVT+6, 3, 255, rate, rate, rate );

					GD_SetGraphColorNo( GRP_MAP_EVT+7, 0, 255, 255, 255, 255 );
					GD_SetGraphColorNo( GRP_MAP_EVT+7, 1, 255, rate, rate, rate );
					GD_SetGraphColorNo( GRP_MAP_EVT+7, 2, 255, 255, 255, 255 );
					GD_SetGraphColorNo( GRP_MAP_EVT+7, 3, 255, rate, rate, rate );


					break;

				}
			}else{
				int	xx,yy;
				if(MapData.evt[i].tmax){
					MapData.evt[i].tcnt++;
					if( MapData.evt[i].tcnt<MapData.evt[i].tmax ){
						float	rate = 0;
						switch(MapData.evt[i].ttype){
							case 0:
								rate = (float)MapData.evt[i].tcnt/MapData.evt[i].tmax;
								break;
							case 1:
								rate = (float)(MapData.evt[i].tcnt*MapData.evt[i].tcnt)/(MapData.evt[i].tmax*MapData.evt[i].tmax);
								break;
							case 2:
								rate = (float)(MapData.evt[i].tmax - MapData.evt[i].tcnt);
								rate = (rate*rate)/(MapData.evt[i].tmax*MapData.evt[i].tmax);
								rate = 1.0f - rate;
								break;
						}

						xx = MapData.evt[i].x*(1.0f-rate) + MapData.evt[i].tx*rate;
						yy = MapData.evt[i].y*(1.0f-rate) + MapData.evt[i].ty*rate;
					}else{
						MapData.evt[i].tcnt = 0;
						MapData.evt[i].tmax = 0;
						xx = MapData.evt[i].x = MapData.evt[i].tx;
						yy = MapData.evt[i].y = MapData.evt[i].ty;
					}
				}else{
					xx=MapData.evt[i].x;
					yy=MapData.evt[i].y;
				}

				GD_SetGraphDstMove( GRP_MAP_EVT+i, (xx-x)*MapControl.zoom, (yy-y)*MapControl.zoom );
				GD_SetGraphZoom( GRP_MAP_EVT+i, MapControl.zoom*MapData.evt[i].zoom );
				GD_SetGraphBlendParam( GRP_MAP_EVT+i, MapData.evt[i].param );
				if(!DrawSetting.low_level)
					GD_SetGraphScaleParam( GRP_MAP_EVT+i, SCALE_HIGHT );
				GD_SetGraphRevParam( GRP_MAP_EVT+i, MapData.evt[i].rev_param );


				

				if( MapData.evt[i].layer<0 ){
					GD_SetGraphLayer( GRP_MAP_EVT+i, LAY_MAP_BACK+1 );
				}else if( MapData.evt[i].layer>0 ){
					GD_SetGraphLayer( GRP_MAP_EVT+i, LAY_EFFECT );
				}


				if(MapData.evt[i].fade){
					MapData.evt[i].fade_cnt++;
					if(MapData.evt[i].fade>MapData.evt[i].fade_cnt){
						GD_SetGraphAlpha( GRP_MAP_EVT+i, MapData.evt[i].rate*(MapData.evt[i].fade-MapData.evt[i].fade_cnt)/MapData.evt[i].fade );
					}else{
						ResetMapObj( i, 0 );
					}
				}else{
					GD_SetGraphAlpha( GRP_MAP_EVT+i, MapData.evt[i].rate );
				}
			}

		}
	}
	GD_SetGlobalColor( MapControl.r, MapControl.g, MapControl.b, MapControl.fd_type, 0 );
}

BOOL WaitScrolBattleMap( int flag )
{
	BOOL	ret=0;
	if(flag&0x01){
		ret |= MapControl.fd_max;
	}
	if(flag&0x02){
		if(MapControl.sc_type&0x100){
			if(MapControl.sc_count <= MapControl.sc_max){
				ret |= 1;
			}
		}else{
			ret |= MapControl.sc_max;
		}
	}
	return ret;
}






#define BTL_COMMAND_MAX		7

#define BTL_COMMAND_X	253.0f	
#define BTL_COMMAND_Y	300.0f

#define BTL_POPUP_W			420.0f	
#define BTL_POPUP_H			(24.0f*7)

#define BTL_POPUP_U_X		(800.0f-BTL_POPUP_W-16.0f)	
#define BTL_POPUP_U_Y		16.0f

#define BTL_POPUP_D_X		(800.0f-BTL_POPUP_W-16.0f)	
#define BTL_POPUP_D_Y		(600.0f-BTL_POPUP_H-16.0f)


typedef struct{
	int	bak_no;
	int	hp;
	int	mp;
	int	exp;

	int	count;
}POPUP_WINDOW;

POPUP_WINDOW	PopupWindow[2];


void RenwePouupCharWindow( void )
{
	
}

void ResetPouupCharWindow( int type )
{
	ResetGameWindow( type );
	GD_ReleaseTexture( BMP_CHAR_POPUP+type );
	GD_ResetGraph( GRP_CHAR_POPUP+type*10+3 );

	PopupWindow[type].bak_no=-1;
	PopupWindow[type].count=0;
}

char *HpClr( int cno )
{
	CHAR_PARAM	*cp = &CharParam[cno];
	char	*ret=NULL;

	if( cp->php==cp->GetHp() ){
		ret = "255:255:255";	
	}else if( cp->php > cp->GetHp()/2 ){
		ret = "255:255:128";	
	}else{
		ret = "255:56:56";	
	}
	return ret;
}
char *MpClr( int cno )
{
	CHAR_PARAM	*cp = &CharParam[cno];
	char	*ret=NULL;

	if( cp->pmp==cp->GetMp() ){
		ret = "255:255:255";	
	}else if( cp->pmp > cp->GetMp()/2 ){
		ret = "255:255:128";	
	}else{
		ret = "255:56:56";	
	}
	return ret;
}
char *AtkClr( int cno )
{
	static char	ret[128];
	int	eff_param = CharParam[cno].CheckStatusEffct( STATUS_EFF_ATK);

	if(CharParam[cno].GetAtk()<10000){
		if( eff_param==0 ){
			wsprintf( ret,"<B225:131:27ATK>%4d ", CharParam[cno].GetAtk() );
		}else if( eff_param<0 ){
			wsprintf( ret,"<B225:131:27ATK><B255:56:56:%4d> ", CharParam[cno].GetAtk() );
		}else{
			wsprintf( ret,"<B225:131:27ATK><B61:116:255:%4d> ", CharParam[cno].GetAtk() );
		}
	}else{
		if( eff_param==0 ){
			wsprintf( ret,"<B225:131:27ATK>%5d", CharParam[cno].GetAtk() );
		}else if( eff_param<0 ){
			wsprintf( ret,"<B225:131:27ATK><B255:56:56:%5d>", CharParam[cno].GetAtk() );
		}else{
			wsprintf( ret,"<B225:131:27ATK><B61:116:255:%5d>", CharParam[cno].GetAtk() );
		}
	}

	return ret;
}
char *DefClr( int cno )
{
	static char	ret[128];
	int	eff_param = CharParam[cno].CheckStatusEffct( STATUS_EFF_DEF);

	if(CharParam[cno].GetDef()<10000){
		if( eff_param==0 ){
			wsprintf( ret,"<B225:131:27DEF>%4d ", CharParam[cno].GetDef() );
		}else if( eff_param<0 ){
			wsprintf( ret,"<B225:131:27DEF><B255:56:56:%4d> ", CharParam[cno].GetDef() );
		}else{
			wsprintf( ret,"<B225:131:27DEF><B61:116:255:%4d> ", CharParam[cno].GetDef() );
		}
	}else{
		if( eff_param==0 ){
			wsprintf( ret,"<B225:131:27DEF>%5d", CharParam[cno].GetDef() );
		}else if( eff_param<0 ){
			wsprintf( ret,"<B225:131:27DEF><B255:56:56:%5d>", CharParam[cno].GetDef() );
		}else{
			wsprintf( ret,"<B225:131:27DEF><B61:116:255:%5d>", CharParam[cno].GetDef() );
		}
	}
	return ret;
}
char *MgkClr( int cno )
{
	static char	ret[128];
	int	eff_param = CharParam[cno].CheckStatusEffct( STATUS_EFF_MGK);

	if(CharParam[cno].GetMgk()<10000){
		if( eff_param==0 ){
			wsprintf( ret,"<B225:131:27MGK>%4d ", CharParam[cno].GetMgk() );
		}else if( eff_param<0 ){
			wsprintf( ret,"<B225:131:27MGK><B255:56:56:%4d> ", CharParam[cno].GetMgk() );
		}else{
			wsprintf( ret,"<B225:131:27MGK><B61:116:255:%4d> ", CharParam[cno].GetMgk() );
		}
	}else{
		if( eff_param==0 ){
			wsprintf( ret,"<B225:131:27MGK>%5d", CharParam[cno].GetMgk() );
		}else if( eff_param<0 ){
			wsprintf( ret,"<B225:131:27MGK><B255:56:56:%5d>", CharParam[cno].GetMgk() );
		}else{
			wsprintf( ret,"<B225:131:27MGK><B61:116:255:%5d>", CharParam[cno].GetMgk() );
		}
	}
	return ret;
}
char *RgsClr( int cno )
{
	static char	ret[128];
	int	eff_param = CharParam[cno].CheckStatusEffct( STATUS_EFF_RGS);

	if(CharParam[cno].GetRgs()<10000){
		if( eff_param==0 ){
			wsprintf( ret,"<B225:131:27RGS>%4d ", CharParam[cno].GetRgs() );
		}else if( eff_param<0 ){
			wsprintf( ret,"<B225:131:27RGS><B255:56:56:%4d> ", CharParam[cno].GetRgs() );
		}else{
			wsprintf( ret,"<B225:131:27RGS><B61:116:255:%4d> ", CharParam[cno].GetRgs() );
		}
	}else{
		if( eff_param==0 ){
			wsprintf( ret,"<B225:131:27RGS>%5d", CharParam[cno].GetRgs() );
		}else if( eff_param<0 ){
			wsprintf( ret,"<B225:131:27RGS><B255:56:56:%5d>", CharParam[cno].GetRgs() );
		}else{
			wsprintf( ret,"<B225:131:27RGS><B61:116:255:%5d>", CharParam[cno].GetRgs() );
		}
	}
	return ret;
}
char *WspClr( int cno )
{
	static char	ret[128];
	int	eff_param = CharParam[cno].CheckStatusEffct( STATUS_EFF_WSP);

	if( eff_param==0 ){
		wsprintf( ret,"<B225:131:27WSP>%4d ", CharParam[cno].GetWsp() );
	}else if( eff_param>0 ){
		wsprintf( ret,"<B225:131:27WSP><B61:116:255:%4d> ", CharParam[cno].GetWsp() );
	}else{
		wsprintf( ret,"<B225:131:27WSP><B255:56:56:%4d> ", CharParam[cno].GetWsp() );
	}
	return ret;
}
char *MovClr( int cno )
{
	static char	ret[128];
	int	eff_param = CharParam[cno].CheckStatusEffct( STATUS_EFF_MOV);

	if( eff_param==0 ){
		wsprintf( ret,"<B225:131:27MOV>%4d ", CharParam[cno].GetMov() );
	}else if( eff_param<0 ){
		wsprintf( ret,"<B225:131:27MOV><B255:56:56:%4d> ", CharParam[cno].GetMov() );
	}else{
		wsprintf( ret,"<B225:131:27MOV><B61:116:255:%4d> ", CharParam[cno].GetMov() );
	}
	return ret;
}

char *HitClr( int cno )
{
	static char	ret[128];
	int	eff_param = CharParam[cno].CheckStatusEffct( STATUS_EFF_HIT);

	if(CharParam[cno].GetHit()<10000){
		if( eff_param==0 ){
			wsprintf( ret,"<B225:131:27HIT>%4d ", CharParam[cno].GetHit() );
		}else if( eff_param<0 ){
			wsprintf( ret,"<B225:131:27HIT><B255:56:56:%4d> ", CharParam[cno].GetHit() );
		}else{
			wsprintf( ret,"<B225:131:27HIT><B61:116:255:%4d> ", CharParam[cno].GetHit() );
		}
	}else{
		if( eff_param==0 ){
			wsprintf( ret,"<B225:131:27HIT>%5d", CharParam[cno].GetHit() );
		}else if( eff_param<0 ){
			wsprintf( ret,"<B225:131:27HIT><B255:56:56:%5d>", CharParam[cno].GetHit() );
		}else{
			wsprintf( ret,"<B225:131:27HIT><B61:116:255:%5d>", CharParam[cno].GetHit() );
		}
	}
	return ret;
}
char *AvdClr( int cno )
{
	static char	ret[128];
	int	eff_param = CharParam[cno].CheckStatusEffct( STATUS_EFF_AVD);

	if(CharParam[cno].GetAvd()<10000){
		if( eff_param==0 ){
			wsprintf( ret,"<B225:131:27AVD>%4d ", CharParam[cno].GetAvd() );
		}else if( eff_param<0 ){
			wsprintf( ret,"<B225:131:27AVD><B255:56:56:%4d> ", CharParam[cno].GetAvd() );
		}else{
			wsprintf( ret,"<B225:131:27AVD><B61:116:255:%4d> ", CharParam[cno].GetAvd() );
		}
	}else{
		if( eff_param==0 ){
			wsprintf( ret,"<B225:131:27AVD>%5d", CharParam[cno].GetAvd() );
		}else if( eff_param<0 ){
			wsprintf( ret,"<B225:131:27AVD><B255:56:56:%5d>", CharParam[cno].GetAvd() );
		}else{
			wsprintf( ret,"<B225:131:27AVD><B61:116:255:%5d>", CharParam[cno].GetAvd() );
		}
	}
	return ret;
}



char *CondStr( int cno )
{
	char	*ret="";
	
	if(CharThink[cno].flag){
		ret="オートバトル中";
	}else{
		switch(CharControl[CharParam[cno].no].action){
			default:
			case CHAR_ACTION_COMMAND:	
				ret="コマンド受付中";
				break;
			case CHAR_ACTION_STAND:		
				ret="待機中";
				break;
			case CHAR_ACTION_MOVE:		
				ret="移動中";
				break;
			case CHAR_ACTION_HANGEKI:	
			case CHAR_ACTION_ATACK:		
				ret="物理攻撃中";
				break;
			case CHAR_ACTION_TECHNIC:	
				ret="技使用中";
				break;
			case CHAR_ACTION_ITEM:	
				ret="アイテム使用中";
				break;
			case CHAR_ACTION_MAGIC:		
				ret="魔法詠唱中";
				break;
			case CHAR_ACTION_BARD:		
				ret="伝承歌発動中";
				break;
			case CHAR_ACTION_DID:		
				ret="意識不明";
				break;
		}
	}
	return ret;
}


void SetPouupCharWindow( int type, int cno, int renew=0 )
{
	float	rate;
	float	cx, cy;
	CHAR_PARAM	*cp;
	char	buf[2048];

	if(cno==-1) { ResetPouupCharWindow( type ); return; }
	if(cno<0 || cno>=CHAR_PARAM_MAX) { DebugBox(NULL, "おかしいぞ？%d", cno); return ; }
	if( !(type==0 || type==1) ) { DebugBox(NULL, "そんなウインドウはない。%d", type); return ; }

	cp = &CharParam[cno];

	if( PopupWindow[type].bak_no!=cno ){
		PopupWindow[type].count=0;

		SetGameWindow( type, GRP_CHAR_POPUP+type*10, BMP_SYSTEM, LAY_SYSTEM, ON, -1, TRUE );
		if(type==0){
			SetGameWindowRect( 0, BTL_POPUP_U_X, BTL_POPUP_U_Y, BTL_POPUP_W, BTL_POPUP_H  );
		}else{
			SetGameWindowRect( 1, BTL_POPUP_D_X, BTL_POPUP_D_Y, BTL_POPUP_W, BTL_POPUP_H  );
		}
	}else{
		PopupWindow[type].count = LIM(PopupWindow[type].count+1,0,15);
	}
	if(PopupWindow[type].bak_no!=cno || PopupWindow[type].hp != cp->php || PopupWindow[type].mp != cp->pmp || PopupWindow[type].exp != cp->exp || renew ){
		char	*atk_clr = AtkClr(cno);
		char	*def_clr = DefClr(cno);
		char	*mgk_clr = MgkClr(cno);
		char	*rgs_clr = RgsClr(cno);
		char	*mov_clr = MovClr(cno);
		char	*wsp_clr = WspClr(cno);
		char	*hit_clr = HitClr(cno);
		char	*avd_clr = AvdClr(cno);

		if(JobParam[cp->job].ex&EX_BOSS){
			wsprintf( buf, 
				"%-14s %s%s\n"
				"<B225:131:27LV.>%3d <B225:131:27ATTR>    <B225:131:27NEXT>%11d\n"
				"<B225:131:27HP> ???? / ???? %s%s\n"
				"<B225:131:27MP> ???? / ???? %s%s\n"
				"<B255:16:16%-14s> %s%s\n",

				cp->name, "<B225:131:27MOV> ??? ", "<B225:131:27WSP> ??? ",
				cp->lv,		0,
				"<B225:131:27ATK> ??? ", "<B225:131:27DEF> ??? ",
				"<B225:131:27HIT> ??? ", "<B225:131:27AVD> ??? ",
				CondStr(cno),	"<B225:131:27MGK> ??? ", "<B225:131:27RGS> ??? "
			);
		}else{
			wsprintf( buf, 
				"%-14s %s%s\n"
				"<B225:131:27LV.>%3d <B225:131:27ATTR>    <B225:131:27NEXT>%11d\n"
				"<B225:131:27HP><B%s %5d>/%-5d %s%s\n"
				"<B225:131:27MP><B%s %5d>/%-5d %s%s\n"
				"<B255:16:16%-14s> %s%s\n",

				cp->name, mov_clr, wsp_clr,
				cp->lv,		cp->GetNextExp()-cp->exp,
				HpClr(cno), cp->php,	cp->GetHp(),	atk_clr,	def_clr,
				MpClr(cno), cp->pmp,	cp->GetMp(),	hit_clr,	avd_clr,
				CondStr(cno),	mgk_clr,	rgs_clr
			);
		}
		
		
		
		

		SetGameWindowText( type, buf, BMP_CHAR_POPUP+type, 0,0, 18 );
	}

	SetGameWindowColor( type, PopupWindow[type].count*255/15, 128, 128, 128 );
	
	
	GD_SetGraph( GRP_CHAR_POPUP+type*10+3, BMP_SYSTEM, LAY_SYSTEM+1, TRUE );
	if(type==0){
		GD_SetGraphRect( GRP_CHAR_POPUP+type*10+3, BTL_POPUP_U_X+168, BTL_POPUP_U_Y+48,  768+max(0,CharParam[ cno ].attr)*24, 320, 22, 22 );
	}else{
		GD_SetGraphRect( GRP_CHAR_POPUP+type*10+3, BTL_POPUP_D_X+168, BTL_POPUP_D_Y+48,  768+max(0,CharParam[ cno ].attr)*24, 320, 22, 22 );
	}
	GD_SetGraphBrightNo( GRP_CHAR_POPUP+type*10+3, 1 );
	GD_SetGraphAlpha( GRP_CHAR_POPUP+type*10+3, PopupWindow[type].count*255/15 );


	PopupWindow[type].bak_no = cno;
	PopupWindow[type].hp  = cp->php;
	PopupWindow[type].mp  = cp->pmp;
	PopupWindow[type].exp = cp->exp;
}
int AllCharControl( int unit, int cno, int scpoe_type=0, int scope=0 )
{
	int	ret=-1;
	int	i,j;
	int	layer_tbl[CHAR_CONTROL_MAX],l;
	int	tbl_max=0;
	static int	cno_bak=-1;

	for(i=0;i<CHAR_CONTROL_MAX;i++){
		if(CharControl[i].flag && !CharParam[CharControl[i].cno].did){
			if( (CharControl[i].px-MapControl.px-24)*MapControl.zoom < GameKey.mx && GameKey.mx <= (CharControl[i].px-MapControl.px+24)*MapControl.zoom &&
				(CharControl[i].py-MapControl.py-96)*MapControl.zoom < GameKey.my && GameKey.my <= (CharControl[i].py-MapControl.py+ 0)*MapControl.zoom ){

				layer_tbl[tbl_max] = i;
				tbl_max++;
			}
		}
	}
	if(tbl_max==0){
		ResetPouupCharWindow( 0 );
		if(cno!=-1)	SetPouupCharWindow( 1, CharControl[cno].cno );
		else		SetPouupCharWindow( 1, -1 );
	}else{
		l=0;
		j=0;
		for(i=0;i<tbl_max;i++){
			if(CharControl[ layer_tbl[i] ].py>=l){
				l = (int)CharControl[ layer_tbl[i] ].py;
				j = layer_tbl[i];
			}
		}
		if(cno==-1){
			if(GameKey.my>300){
				SetPouupCharWindow( 0, CharControl[j].cno );
				SetPouupCharWindow( 1, -1 );
			}else{
				SetPouupCharWindow( 0, -1 );
				SetPouupCharWindow( 1, CharControl[j].cno );
			}
		}else{
			SetPouupCharWindow( 0, CharControl[j].cno );
			SetPouupCharWindow( 1, CharControl[cno].cno );
		}
		if(GameKey.click){
			if(CharParam[CharControl[j].cno].unit==unit || unit==-1){
				ret=j;
			}else{
			}
		}
	}

	for(i=0;i<CHAR_CONTROL_MAX;i++){
		CharMarkerSelect[i]=0;		
		CharMarkerAtack[i]=0;		
	}
	if(cno!=-1){
		CharMarkerSelect[cno] = 1;		
	}
	
	if(unit>=-1){
		if(tbl_max==0){
		}else{
			if(cno_bak!=j){
				SND_Select();
			}
			cno_bak=j;

			if(unit==0){
				if(CharParam[CharControl[j].cno].unit==0){
					CharMarkerSelect[j] = 1;		
					CharMarkerAtack[j]  = 0;
				}else{
					CharMarkerSelect[j] = 0;		
					CharMarkerAtack[j]  = 1;
				}
			}else{
				if(CharParam[CharControl[j].cno].unit==0){
					CharMarkerSelect[j] = 2;		
					CharMarkerAtack[j]  = 0;
				}else{
					CharMarkerSelect[j] = 0;		
					CharMarkerAtack[j]  = 2;
				}
			}
		}
	}else{
		float	tx=0,ty=0;
		switch(scpoe_type){
		case RANGE_SCOPE:
			tx = GameKey.mx/MapControl.zoom + MapControl.px;
			ty = GameKey.my/MapControl.zoom + MapControl.py;
			break;
		case RANGE_SELF_SCOPE:
			tx = CharControl[cno].px;
			ty = CharControl[cno].py;
			break;
		}
		for(i=0;i<CHAR_CONTROL_MAX;i++){
			if(CharControl[i].flag && !CharParam[CharControl[i].cno].did){
				if( pow(tx-CharControl[i].px,2)+pow(ty-CharControl[i].py,2) < pow(scope,2) ){
					if(CharParam[CharControl[i].cno].unit==0){
						CharMarkerSelect[i] = 2;		
						CharMarkerAtack[i]  = 0;
					}else{
						CharMarkerSelect[i] = 0;		
						CharMarkerAtack[i]  = 2;
					}

				}
			}
		}
	}
	return ret;
}


void SetAriaTable( int range )
{
	int	x,y, i;
	int	w = MapData.head.w/MapData.head.sgrid;
	int	h = MapData.head.h/MapData.head.sgrid;

	i=0;
	for(y=0;y<h;y++){
		for(x=0;x<w;x++,i++){



			if( abs(x*MapData.head.sgrid-StartPoint.px) + abs(y*MapData.head.sgrid-StartPoint.py) < range && MapData.sinkin[y*w+x]==0 ){
				MapControl.aria_table[i] = 2;
			}else{
				MapControl.aria_table[i] = 0;
			}

			int	k;
			for(k=0;k<CHAR_CONTROL_MAX;k++){
				if( !CharControl[k].flag ) continue;
				if( CharParam[CharControl[k].cno].unit!=0 ) continue;

				if( x==int(CharControl[k].px/MapData.head.sgrid)
				 && y==int(CharControl[k].py/MapData.head.sgrid) ){
					MapControl.aria_table[i] = 0;
					break;
				}
			}

		}
	}
}


void DrawMapMoveAria( int del, int aria_type, int a, int r, int g, int b )
{
	int	i,j, k;
	int	x,y;
	int	w = MapData.head.w/MapData.head.sgrid;
	int	h = MapData.head.h/MapData.head.sgrid;
	int	px= (int)(MapControl.px/MapData.head.sgrid);
	int	py= (int)(MapControl.py/MapData.head.sgrid);
	float	dx,dy;
	static	back_j=0;
	int		flag;

	for(j=0;j<back_j;j++){
		GD_ResetGraph( GRP_MAP_MOVE+j );
	}
	back_j=0;
	if(del) return ;
	i=0;
	j=0;
	for(y=0;y<h;y++){
		if( !(py<=y && y<py+600/MapControl.zoom/32+2) ) continue;
		for(x=0;x<w;x++){
			if( !(px<=x && x<px+800/MapControl.zoom/32+1) ) continue;

			if(aria_type==0){
				flag = !MapData.sinkin[y*w+x];
			}else{
				flag = MapControl.aria_table[y*w+x];
			}

			switch(flag){
			default:
			case 0:
				if(i==0){
					dx = x*MapData.head.sgrid*MapControl.zoom-MapControl.px*MapControl.zoom;
					dy = y*MapData.head.sgrid*MapControl.zoom-MapControl.py*MapControl.zoom;
				}
				i++;
				break;
			case 1:
				if(i){
					GD_SetGraphPrimitive( GRP_MAP_MOVE+j, 4, LAY_MAP_MOVE2, TRUE );
					GD_SetGraphDstRect( GRP_MAP_MOVE+j, dx, dy, i*MapData.head.sgrid*MapControl.zoom, MapData.head.sgrid*MapControl.zoom );
					GD_SetGraphBlendParam( GRP_MAP_MOVE+j, BLEND_ALPHA );
					GD_SetGraphColor( GRP_MAP_MOVE+j, a, r, g, b );
					j++;
				}
				i=0;
				break;
			case 2:
				i=0;
				dx = x*MapData.head.sgrid*MapControl.zoom-MapControl.px*MapControl.zoom;
				dy = y*MapData.head.sgrid*MapControl.zoom-MapControl.py*MapControl.zoom;

				GD_SetGraphPrimitive( GRP_MAP_MOVE+j, 4, LAY_MAP_MOVE2, TRUE );
				GD_SetGraphDstRect( GRP_MAP_MOVE+j, dx, dy, MapData.head.sgrid*MapControl.zoom-1, MapData.head.sgrid*MapControl.zoom-1 );
				GD_SetGraphBlendParam( GRP_MAP_MOVE+j, BLEND_ALPHA );

				if( dx<=GameKey.mx&& GameKey.mx<dx+MapData.head.sgrid*MapControl.zoom
				 && dy<=GameKey.my&& GameKey.my<dy+MapData.head.sgrid*MapControl.zoom ){
					GD_SetGraphColor( GRP_MAP_MOVE+j, a, 253,192,28 );
				}else{
					GD_SetGraphColor( GRP_MAP_MOVE+j, a, r, g, b );
				}
				j++;
				break;
			}
		}
		if(i && aria_type==0){
			GD_SetGraphPrimitive( GRP_MAP_MOVE+j, 4, LAY_MAP_MOVE2, TRUE );
			GD_SetGraphDstRect( GRP_MAP_MOVE+j, dx, dy, i*MapData.head.sgrid*MapControl.zoom, MapData.head.sgrid*MapControl.zoom );
			GD_SetGraphBlendParam( GRP_MAP_MOVE+j, BLEND_ALPHA );
			GD_SetGraphColor( GRP_MAP_MOVE+j, a, r, g, b );
			j++;
		}
		i=0;
	}
	back_j = j;
	return ;
}


typedef struct{
	int		ketei;
	int		action;
	int		cno;
	int		mgno;
	int		taget;
	float	tx,ty;
}BTL_COMMAND_STRUCT;
enum{
	BCMD_ACT_MOVE,
	BCMD_ACT_ATK,
	BCMD_ACT_MAGIC,
	BCMD_ACT_TECHNIC,
	BCMD_ACT_ITEM,
	BCMD_ACT_AUTO,
	BCMD_ACT_STAND,

	BCMD_ACT_CANSEL,
};


BTL_COMMAND_STRUCT	CommandStruct;


int	BTL_CoomandAtack( int cno )
{
	enum{
		BCMD_ATK_INIT,
		BCMD_ATK_TERM,
		BCMD_ATK_SELECT,
		BCMD_ATK_SELECT_IN,
		BCMD_ATK_SELECT_OUT,
	};
	static int	step=BCMD_ATK_INIT;
	int			i;
	int			ret=0;
	static int	bak_ret=0;
	

	switch(step){
		case BCMD_ATK_INIT:
			step=BCMD_ATK_SELECT;
			break;
		case BCMD_ATK_TERM:
			CharMarkerEnemy=0;

			step=BCMD_ATK_INIT;
			ret=1;
			break;
		case BCMD_ATK_SELECT:
			CharMarkerEnemy=1;
			MapControl.sc_mode=0;
			i = AllCharControl(1,cno);
			if( i!=-1 ){
				if( CheckMapCharAtack( cno, i ) ){
					CommandStruct.action = BCMD_ACT_ATK;
					CommandStruct.ketei = TRUE;
					CommandStruct.taget = i;
					CommandStruct.cno   = cno;

					step=BCMD_ATK_TERM;
					bak_ret=1;

					SND_Ok();
				}else{
					SND_Ng();
				}
			}else if(GameKey.cansel){
				step=BCMD_ATK_TERM;
				bak_ret=2;
			}
			break;
	}
	return ret;
}




BOOL GetSkillUseFlag( int mgno, int no )
{
	BOOL	use_flag=TRUE;

	if(CharControl[no].sw!=0){
		return FALSE;
	}

	if(GetMagicNo( MAGIC_SKILL, 900 )==mgno && ItemParam[CharParam[CharControl[no].cno].buki].type!=ITEM_BUKI_SWORD)	
		use_flag=FALSE;
	if(GetMagicNo( MAGIC_SKILL, 901 )==mgno && ItemParam[CharParam[CharControl[no].cno].buki].type!=ITEM_BUKI_SPEAR)	
		use_flag=FALSE;
	if(GetMagicNo( MAGIC_SKILL, 902 )==mgno && ItemParam[CharParam[CharControl[no].cno].buki].type!=ITEM_BUKI_HAMMER)	
		use_flag=FALSE;
	if(GetMagicNo( MAGIC_SKILL, 903 )==mgno && ItemParam[CharParam[CharControl[no].cno].buki].type!=ITEM_BUKI_BOW)	
		use_flag=FALSE;
	return use_flag;
}
int BTL_DragRightBar10( int *page, int *page_max, int select_no )
{
	int	i,j,draw=0;

	if(*page_max>10){
		static int	drag_flag=0, drag_page;
		int			mx,my;

		*page = LIM( *page, 0, max(0,*page_max-10) );

		if(select_no==12){

			i = 40+*page*(240-240*10/(*page_max)) / (*page_max-10);
			j = 240*10 / (*page_max)+12;
			
			MUS_GetMousePos( &mx, &my );

			if(GameKey.click){
				SND_Ok();
				if( i<=my && my<i+j ){
					drag_flag = 1;
					drag_page = *page;
				}else{
					*page = LIM( *page+1, 0, *page_max-10 );
					draw=1;
				}
			}
		}
		if(drag_flag){
			draw=1;
			if( MUS_GetMouseDragMove( MOUSE_LBUTTON, &mx, &my ) ){
				i = drag_page + my/(240.0f/(*page_max));
				*page = LIM( i, 0, *page_max-10 );
			}else{
				drag_flag = 0;
			}
		}
		if(GameKey.pup   ||(select_no==10&&GameKey.click)){
			if(GameKey.pup){
				SND_Select();
			}else{
				SND_Ok();
			}
			*page = LIM( *page-1, 0, max(0,*page_max-10) ); draw=1;
		}
		if(GameKey.pdown ||(select_no==11&&GameKey.click)){
			if(GameKey.pdown){
				SND_Select();
			}else{
				SND_Ok();
			}
			*page = LIM( *page+1, 0, max(0,*page_max-10) ); draw=1;
		}
	}else{
		*page=0;
	}
	return draw;
}


int	BTL_CoomandMagic( int cno, int skill_flag )
{
	enum{
		BCMD_MGK_INIT,
		BCMD_MGK_TERM,

		BCMD_MGK_MENU,
		BCMD_MGK_MENU_IN,
		BCMD_MGK_MENU_OUT,

		BCMD_MGK_RAGE,
		BCMD_MGK_RAGE_IN,
		BCMD_MGK_RAGE_OUT,

		BCMD_MGK_SCOPE,


		BCMD_MGK_SCOPE_OUT,
		BCMD_MGK_SELF_SCOPE,

	};
	static int	step=BCMD_MGK_INIT;
	static int	rcount=0;
	static int	scount=0;
	static int	count=0;
	static int	loop_count=0;
	static int	loop_count2=0;

	static int	page=0;
	static int	page_max=0;

	static int	magic_no=0;

	char		buf[1024];
	char		buf2[256];

	int			i,j;
	float		rate;
	int			ret=0;
	static int	bak_ret=0;
	static int	close_mgk=0;

	float		px,py;
	static int	mx,my;

	BOOL		scope_select;
	int			select_no = MUS_GetMouseNo( -1 );
	static int	bak_select = -2;
	int			redraw=0;

	int			range=0;
	int			scope=0;
	if(magic_no!=-1){
		range = GetMagicRange( magic_no, CharControl[cno].cno );
		scope = GetMagicScope( magic_no, CharControl[cno].cno );
	}

	px = (CharControl[cno].px-MapControl.px)*MapControl.zoom;
	py = (CharControl[cno].py-MapControl.py)*MapControl.zoom;
	switch(step){
		case BCMD_MGK_INIT:
			rcount=0;
			scount=0;
			count=0;
			loop_count=0;
			loop_count2=0;
			page=0;
			close_mgk=0;
			bak_ret=0;
			bak_select = -2;

			step=BCMD_MGK_MENU_IN;

			GD_SetGraphPrimitive( GRP_MAP_MOVE+0, 4, LAY_EFFECT+1, OFF );
			GD_SetGraphBlendParam( GRP_MAP_MOVE+0, BLEND_ALPHA );
			GD_SetGraphColor( GRP_MAP_MOVE+0, 0, 0, 0, 0 );

			GD_SetGraphCircle( GRP_MAP_MOVE+1, 64, TRUE, LAY_EFFECT+2, OFF );
			GD_SetGraphBlendParam( GRP_MAP_MOVE+1, BLEND_ADD );
			GD_SetGraphColorNo( GRP_MAP_MOVE+1, 0,   0, 0,   0, 0 );
			GD_SetGraphColorNo( GRP_MAP_MOVE+1, 1, 255, 12, 128, 22 );

			GD_SetGraphCircle( GRP_MAP_MOVE+2, 32, TRUE, LAY_EFFECT+3, OFF );
			GD_SetGraphBlendParam( GRP_MAP_MOVE+2, BLEND_ADD );
			GD_SetGraphColorNo( GRP_MAP_MOVE+2, 0,   0, 0, 0, 0 );
			GD_SetGraphColorNo( GRP_MAP_MOVE+2, 1, 255, 128, 12, 12 );

			GD_SetGraphCircle( GRP_MAP_MOVE+3, 32, TRUE, LAY_EFFECT+2, OFF );
			GD_SetGraphBlendParam( GRP_MAP_MOVE+3, BLEND_ADD );
			GD_SetGraphColorNo( GRP_MAP_MOVE+3, 0,   0, 0,   0, 0 );
			GD_SetGraphColorNo( GRP_MAP_MOVE+3, 1, 255, 12, 128, 22 );

			GD_SetGraphCircle( GRP_MAP_MOVE+4, 16, TRUE, LAY_EFFECT+3, OFF );
			GD_SetGraphBlendParam( GRP_MAP_MOVE+4, BLEND_ADD );
			GD_SetGraphColorNo( GRP_MAP_MOVE+4, 0,   0, 0, 0, 0 );
			GD_SetGraphColorNo( GRP_MAP_MOVE+4, 1, 255, 128, 12, 12 );
			
			SetGameWindow( 2, GRP_SYSTEM, BMP_SYSTEM, LAY_SYSTEM+2, OFF, -1, 1 );

			if(skill_flag){
				for(i=0;i<MAGIC_CHAR_MAX;i++){
					if(CharParam[CharControl[cno].cno].skill[i]==-1) break;
				}
				page_max = i;
				buf[0] = '\0';
				for(i=0;i<10;i++){
					if(CharParam[CharControl[cno].cno].skill[i]==-1) break;
					int	mgno = CharParam[CharControl[cno].cno].skill[page+i];
					int	use_flag=GetSkillUseFlag( mgno, cno );

					if( MagicParam[mgno].mp<=CharParam[CharControl[cno].cno].pmp && use_flag ){
						wsprintf( buf2, "%-20s MP%3d SNG%4d\n", MagicParam[mgno].name, MagicParam[mgno].mp, MagicParam[ mgno ].sng );
					}else{
						wsprintf( buf2, "<B128:128:128:%-20s MP%3d SNG%4d>\n", MagicParam[mgno].name, MagicParam[mgno].mp, MagicParam[ mgno ].sng );
					}
					_mbscat( (BYTE*)buf, (BYTE*)buf2 );
				}
			}else{
				for(i=0;i<MAGIC_CHAR_MAX;i++){
					if(CharParam[CharControl[cno].cno].magic[i]==-1) break;
				}
				page_max = i;
				buf[0] = '\0';
				for(i=0;i<10;i++){
					if(CharParam[CharControl[cno].cno].magic[i]==-1) break;
					int	mgno = CharParam[CharControl[cno].cno].magic[page+i];
					int	sng = GetMagicSng( mgno, CharControl[cno].cno );

					if( MagicParam[mgno].mp<=CharParam[CharControl[cno].cno].pmp ){
						wsprintf( buf2, "%-20s MP%3d SNG%4d\n", MagicParam[mgno].name, MagicParam[mgno].mp, sng );
					}else{
						wsprintf( buf2, "<B128:128:128:%-20s MP%3d SNG%4d>\n", MagicParam[mgno].name, MagicParam[mgno].mp, sng );
					}
					_mbscat( (BYTE*)buf, (BYTE*)buf2 );
				}
			}
			if(page_max>10){
				SetGameWindowRect( 2, BTL_COMMAND_X, 16, 24*21, 288  );
			}else{
				SetGameWindowRect( 2, BTL_COMMAND_X, 16, 24*20, 288  );
			}
			SetGameWindowText( 2, buf, BMP_SYSTEM+2,8 );

			MUS_SetMouseLayer( 2 );
			for(i=0;i<10;i++){
				MUS_SetMouseRect( 2, i, (int)BTL_COMMAND_X+24, 16+24+i*24, 24*18, 24, ON );
			}

	MUS_SetMouseRect( 2, 10, (int)BTL_COMMAND_X+24*18, 16+24-32, 44, 32, ON );
	MUS_SetMouseRect( 2, 11, (int)BTL_COMMAND_X+24*18, 16+264,   44, 32, ON );
	MUS_SetMouseRect( 2, 12, (int)BTL_COMMAND_X+24*19+8, 16+24,    16, 24*10, ON );

	GD_SetGraph( GRP_START_CHAR+10, BMP_SYSTEM, LAY_SYSTEM+3, TRUE );	
	GD_SetGraphRect( GRP_START_CHAR+10, (int)BTL_COMMAND_X+24*18, 16+24-32,     624+0*44, 432, 44, 32 );
	GD_SetGraph( GRP_START_CHAR+11, BMP_SYSTEM, LAY_SYSTEM+3, TRUE );	
	GD_SetGraphRect( GRP_START_CHAR+11, (int)BTL_COMMAND_X+24*18, 16+264, 624+0*44, 464, 44, 32 );

	GD_SetGraph( GRP_START_CHAR+12, BMP_SYSTEM, LAY_SYSTEM+3, TRUE );
	GD_SetGraph( GRP_START_CHAR+13, BMP_SYSTEM, LAY_SYSTEM+3, TRUE );
	GD_SetGraph( GRP_START_CHAR+14, BMP_SYSTEM, LAY_SYSTEM+3, TRUE );
	GD_SetGraphSrcRect( GRP_START_CHAR+12, 640, 32+496,    16,  6 );
	GD_SetGraphSrcRect( GRP_START_CHAR+13, 640, 32+496+6,  16, 20 );
	GD_SetGraphSrcRect( GRP_START_CHAR+14, 640, 32+496+26, 16,  6 );

	GD_SetGraphDstRect( GRP_START_CHAR+12, (int)BTL_COMMAND_X+24*19+8,  40,          16,  6 );
	GD_SetGraphDstRect( GRP_START_CHAR+13, (int)BTL_COMMAND_X+24*19+8,  40+6,        16,  240-12 );
	GD_SetGraphDstRect( GRP_START_CHAR+14, (int)BTL_COMMAND_X+24*19+8,  40+6+240-12, 16,  6 );

	GD_SetGraph( GRP_START_CHAR+15, BMP_SYSTEM, LAY_SYSTEM+4, TRUE );
	GD_SetGraph( GRP_START_CHAR+16, BMP_SYSTEM, LAY_SYSTEM+4, TRUE );
	GD_SetGraph( GRP_START_CHAR+17, BMP_SYSTEM, LAY_SYSTEM+4, TRUE );
	GD_SetGraphSrcRect( GRP_START_CHAR+15, 624, 32+496,    16,  6 );
	GD_SetGraphSrcRect( GRP_START_CHAR+16, 624, 32+496+6,  16, 20 );
	GD_SetGraphSrcRect( GRP_START_CHAR+17, 624, 32+496+26, 16,  6 );
	for( i=0 ; i < 8 ; i++ ){
		GD_SetGraphDisp( GRP_START_CHAR+10+i, FALSE );
	}

			GD_SetGraphx9( GRP_START_CHAR+3, BMP_SYSTEM, LAY_SYSTEM+3, FALSE );	
			GD_SetGraphSrcRect( GRP_START_CHAR+3, 0, 64, 72, 24 );

			break;

		case BCMD_MGK_TERM:
			GD_ResetGraph( GRP_MAP_MOVE+0 );
			GD_ResetGraph( GRP_MAP_MOVE+1 );
			GD_ResetGraph( GRP_MAP_MOVE+2 );
			GD_ResetGraph( GRP_MAP_MOVE+3 );
			GD_ResetGraph( GRP_MAP_MOVE+4 );
			
			for( i=0 ; i < 8 ; i++ ){
				GD_ResetGraph( GRP_START_CHAR+10+i );
			}

			GD_ResetGraph( GRP_START_CHAR+3 );

			ResetGameWindow( 2 );
			GD_ReleaseTexture( BMP_SYSTEM+2 );

			CharMarkerPlayer=0;
			CharMarkerEnemy =0;

			MUS_ResetMouseRect_Layer( 2 );
			MUS_SetMouseLayer( 1 );

			step=BCMD_MGK_INIT;
			ret=bak_ret;
			break;

		case BCMD_MGK_MENU_IN:
			if(count==0){
				ResetPouupCharWindow( 0 );
				for(i=0;i<CHAR_CONTROL_MAX;i++){
					CharMarkerSelect[i] = 0;
					CharMarkerAtack[i] = 0;
				}
				CharMarkerSelect[cno] = 1;
			}
			for(i=0;i<5;i++)
				GD_SetGraphDisp( GRP_MAP_MOVE+i, FALSE );

			count++;
			rate = POYO_CONUT(count,15,10);

			SetGameWindowDisp( 2, ON );
			SetGameWindowColor( 2, count*255/15, 128, 128, 128 );
			SetGameWindowZoom( 2, 0.5f+rate/2 );
			if(page_max>10){
				SetGameWindowRect( 2, BTL_COMMAND_X, 16, 24*21, 288  );
			}else{
				SetGameWindowRect( 2, BTL_COMMAND_X, 16, 24*20, 288  );
			}

			if(count>=15){
				count=0;
				step=BCMD_MGK_MENU;
			}
			break;
		case BCMD_MGK_MENU_OUT:
			GD_SetGraphDisp( GRP_START_CHAR+3, FALSE );

			count--;
			rate = POYO_CONUT(count,15,10);

			SetGameWindowMove( 2, BTL_COMMAND_X, 16+288-rate*288  );
			SetGameWindowColor( 2, count*255/15, 128, 128, 128 );

			if(count<=0){
				step=BCMD_MGK_TERM;
			}
			break;

		case BCMD_MGK_MENU:
	{
		int	page_max2 = max(10,page_max);
		if( page_max>10 ){
			for( i=0 ; i < 8 ; i++ ){
				GD_SetGraphDisp( GRP_START_CHAR+10+i, TRUE );
			}

			if(select_no==10){
				if( select_no+page!=bak_select){
					SND_Select();
				}
				if(GameKey.click)	GD_SetGraphSrcRect( GRP_START_CHAR+10, 624+2*44, 432, 44, 32 ),appData.draw_flag=1;
				else				GD_SetGraphSrcRect( GRP_START_CHAR+10, 624+1*44, 432, 44, 32 );
			}else	GD_SetGraphSrcRect( GRP_START_CHAR+10, 624+0*44, 432, 44, 32 );
			if(select_no==11){
				if( select_no+page!=bak_select){
					SND_Select();
				}
				if(GameKey.click)	GD_SetGraphSrcRect( GRP_START_CHAR+11, 624+2*44, 464, 44, 32 ),appData.draw_flag=1;
				else				GD_SetGraphSrcRect( GRP_START_CHAR+11, 624+1*44, 464, 44, 32 );
			}else	GD_SetGraphSrcRect( GRP_START_CHAR+11, 624+0*44, 464, 44, 32 );
	

			i = page*(240-12-228*10/page_max2)/(page_max2-10);
			j = 228*10/page_max2;

			GD_SetGraphDstRect( GRP_START_CHAR+15, (int)BTL_COMMAND_X+24*19+8,  i + 40,   16,  6 );	
			GD_SetGraphDstRect( GRP_START_CHAR+16, (int)BTL_COMMAND_X+24*19+8,  i + 40+6, 16,  j );
			GD_SetGraphDstRect( GRP_START_CHAR+17, (int)BTL_COMMAND_X+24*19+8,  i + 40+6+j,   16,  6 );
		}else{
			for( i=0 ; i < 8 ; i++ ){
				GD_SetGraphDisp( GRP_START_CHAR+10+i, FALSE );
			}		}
	}
redraw = BTL_DragRightBar10( &page, &page_max, select_no );

			if(count==0 || redraw){

				buf[0] = '\0';
				if(skill_flag){
					for(i=0;i<10;i++){
						if(CharParam[CharControl[cno].cno].skill[page+i]==-1) break;
						int	mgno = CharParam[CharControl[cno].cno].skill[page+i];
						int	sng = MagicParam[ mgno ].sng;
						int	use_flag=GetSkillUseFlag( mgno, cno );
						
						if( MagicParam[mgno].mp<=CharParam[CharControl[cno].cno].pmp && use_flag ){
							wsprintf( buf2, "%-20s MP%3d SNG%4d\n", MagicParam[mgno].name, MagicParam[mgno].mp, sng );
						}else{
							wsprintf( buf2, "<B128:128:128:%-20s MP%3d SNG%4d>\n", MagicParam[mgno].name, MagicParam[mgno].mp, sng );
						}
						_mbscat( (BYTE*)buf, (BYTE*)buf2 );
					}
				}else{
					for(i=0;i<10;i++){
						if(CharParam[CharControl[cno].cno].magic[page+i]==-1) break;

						int	mgno = CharParam[CharControl[cno].cno].magic[page+i];
						int	sng = GetMagicSng( mgno, CharControl[cno].cno );

						if( MagicParam[mgno].mp<=CharParam[CharControl[cno].cno].pmp ){
							wsprintf( buf2, "%-20s MP%3d SNG%4d\n", MagicParam[mgno].name, MagicParam[mgno].mp, sng );
						}else{
							wsprintf( buf2, "<B128:128:128:%-20s MP%3d SNG%4d>\n", MagicParam[mgno].name, MagicParam[mgno].mp, sng );
						}
						_mbscat( (BYTE*)buf, (BYTE*)buf2 );
					}
				}
				SetGameWindowText( 2, buf );
			}
			if( select_no!=-1 && select_no < min(10,page_max)){
				GD_SetGraphDisp( GRP_START_CHAR+3, TRUE );
				GD_SetGraphDstRect( GRP_START_CHAR+3, BTL_COMMAND_X+24, 16+24+24*select_no, 24*18,  24 );
				if( select_no+page!=bak_select){
					SND_Select();

					if(skill_flag){
						if( CharControl[cno].sw!=0 ){
							wsprintf( buf2, "技硬直中です。技は使用できません。\n残り %d.%d 秒。", CharControl[cno].sw/60, CharControl[cno].sw%60/6 );
							SetGameWindowText( 15, buf2 );
						}else{
							SetGameWindowText( 15, MagicParam[CharParam[CharControl[cno].cno].skill[page+select_no]].help );
						}
					}else{
						SetGameWindowText( 15, MagicParam[CharParam[CharControl[cno].cno].magic[page+select_no]].help );
					}
				}
			}else{
				if( select_no+page==bak_select){
					if(skill_flag){
						SetGameWindowText( 15, "使用する技を選んで下さい。" );
					}else{
						SetGameWindowText( 15, "使用する魔法を選んで下さい。" );
					}
				}
				GD_SetGraphDisp( GRP_START_CHAR+3, FALSE );
			}
			count++;

			if(GameKey.click){
				if( select_no!=-1 && select_no < min(10,page_max) ){
					int	use_flag=TRUE;
					if(skill_flag){
						magic_no = CharParam[CharControl[cno].cno].skill[page+select_no];
						use_flag=GetSkillUseFlag( magic_no, cno );
					}else{
						magic_no = CharParam[CharControl[cno].cno].magic[page+select_no];
					}

					if( MagicParam[magic_no].mp<=CharParam[CharControl[cno].cno].pmp && use_flag ){
						SND_Ok();
						for( i=0 ; i < 8 ; i++ ){
							GD_SetGraphDisp( GRP_START_CHAR+10+i, FALSE );
						}

						count=15;
						GD_SetGraphDisp( GRP_START_CHAR+3, FALSE );

						switch(MagicParam[magic_no].scope_type){
							case RANGE_TAGET:		
							case RANGE_SCOPE:		
								step=BCMD_MGK_RAGE_IN;
								break;
							case RANGE_SELF:		
							case RANGE_SELF_SCOPE:	
							case RANGE_ALL:			
								step=BCMD_MGK_SELF_SCOPE;
								break;
							case RANGE_LINE:		
							case RANGE_FAN:			
								break;
						}
					}else{
						SND_Ng();
					}
				}
			}
			if(GameKey.cansel){
				for( i=0 ; i < 8 ; i++ ){
					GD_SetGraphDisp( GRP_START_CHAR+10+i, FALSE );
				}
				count=15;
				bak_ret=2;
				step=BCMD_MGK_MENU_OUT;

				SND_Cansel();
			}
			bak_select = select_no+page;
			break;

		case BCMD_MGK_RAGE_IN:
			count--;
			rate = POYO_CONUT(count,15,10);

			SetGameWindowMove( 2, BTL_COMMAND_X, 16+288-rate*288  );
			SetGameWindowColor( 2, count*255/15, 128, 128, 128 );

			if(count<=0){
				count=0;
			}
			
			
			rate = POYO_CONUT(rcount,15,10);

			GD_SetGraphDisp( GRP_MAP_MOVE+0, TRUE );
			GD_SetGraphColor( GRP_MAP_MOVE+0, 64*rcount/15, 0, 0, 0 );

			GD_SetGraphDisp( GRP_MAP_MOVE+1, TRUE );
			GD_SetGraphDstRect( GRP_MAP_MOVE+1, px - rate*range, py - rate*range, rate*range*2, rate*range*2 );
			GD_SetGraphZoom( GRP_MAP_MOVE+1, MapControl.zoom );
			GD_SetGraphCenter( GRP_MAP_MOVE+1, px, py );

			if(rcount>=15){
				switch(MagicParam[magic_no].scope_type){
					case RANGE_TAGET:		
						step=BCMD_MGK_RAGE;
						break;
					case RANGE_SCOPE:		
						step=BCMD_MGK_SCOPE;
						break;



					case RANGE_FAN:			
					case RANGE_ALL:			
						break;
				}
				scount=0;
			}else{
				rcount++;
			}
			break;
		case BCMD_MGK_RAGE_OUT:
			rate = POYO_CONUT(rcount,15,10);
			MapControl.sc_mode=1;

			GD_SetGraphDisp( GRP_MAP_MOVE+0, TRUE );
			GD_SetGraphColor( GRP_MAP_MOVE+0, 64*rcount/15, 0, 0, 0 );

			GD_SetGraphDisp( GRP_MAP_MOVE+1, TRUE );
			GD_SetGraphDstRect( GRP_MAP_MOVE+1, px - rate*range, py - rate*range, rate*range*2, rate*range*2 );
			GD_SetGraphZoom( GRP_MAP_MOVE+1, MapControl.zoom );
			GD_SetGraphCenter( GRP_MAP_MOVE+1, px, py );

			if(scount){
				rate = POYO_CONUT(scount,15,10);

				GD_SetGraphDisp( GRP_MAP_MOVE+2, TRUE );
				GD_SetGraphDstRect( GRP_MAP_MOVE+2, mx - rate*scope, my - rate*scope, rate*scope*2, rate*scope*2 );
				GD_SetGraphZoom( GRP_MAP_MOVE+2, MapControl.zoom );
				GD_SetGraphCenter( GRP_MAP_MOVE+2, (float)mx, (float)my );
			}
			if(scount<=10){
				rcount = LIM(rcount-1,0,15);
			}
			scount = LIM(scount-1,0,15);

			if( scount==0 ){
				GD_SetGraphDisp( GRP_MAP_MOVE+2, FALSE );
				if( rcount==0 ){
					GD_SetGraphDisp( GRP_MAP_MOVE+1, FALSE );
					count=0;
					if(close_mgk){
						bak_ret=1;
						step=BCMD_MGK_TERM;
					}else{
						step=BCMD_MGK_MENU_IN;
					}
				}
			}
			break;

		case BCMD_MGK_RAGE:
			GD_SetGraphDisp( GRP_MAP_MOVE+1, TRUE );
			GD_SetGraphDstRect( GRP_MAP_MOVE+1, (float)(px - range), (float)(py - range), (float)(range*2), (float)(range*2) );
			GD_SetGraphZoom( GRP_MAP_MOVE+1, MapControl.zoom );
			GD_SetGraphCenter( GRP_MAP_MOVE+1, px, py );

			CharMarkerPlayer=1;
			CharMarkerEnemy =1;
			MapControl.sc_mode=0;
			i = AllCharControl( -1, cno );
			if( i!=-1 ){
				scope_select = pow(CharControl[cno].px-CharControl[i].px,2)+pow(CharControl[cno].py-CharControl[i].py,2) <= pow(range,2);
				if( scope_select ){
					if( !(skill_flag && (CharParam[CharControl[cno].cno].unit==CharParam[CharControl[i].cno].unit)) ){
						CommandStruct.ketei = TRUE;
						CommandStruct.action = BCMD_ACT_MAGIC;
						CommandStruct.mgno = magic_no;	
						CommandStruct.taget = i;
						CommandStruct.cno   = cno;

						close_mgk=1;
						step=BCMD_MGK_RAGE_OUT;
						SND_Ok();
					}else{
						SND_Ng();
					}
				}else{
					SND_Ng();
				}
			}else if(GameKey.cansel){
				close_mgk=0;
				step=BCMD_MGK_RAGE_OUT;
				SND_Cansel();
			}
			break;

		case BCMD_MGK_SCOPE:
			GD_SetGraphDisp( GRP_MAP_MOVE+1, TRUE );
			GD_SetGraphDstRect( GRP_MAP_MOVE+1, (float)(px - range), (float)(py - range), (float)range*2, (float)range*2 );
			GD_SetGraphZoom( GRP_MAP_MOVE+1, MapControl.zoom );
			GD_SetGraphCenter( GRP_MAP_MOVE+1, px, py );

			rate = loop_count%128 /128.0f;
			rate = 1.0f-(1.0f-rate)*(1.0f-rate);

			GD_SetGraphDisp( GRP_MAP_MOVE+3, TRUE );
			GD_SetGraphDstRect( GRP_MAP_MOVE+3, (float)(px - range*rate), (float)(py - range*rate), (float)range*2*rate, (float)range*2*rate );
			GD_SetGraphZoom( GRP_MAP_MOVE+3, MapControl.zoom );
			GD_SetGraphCenter( GRP_MAP_MOVE+3, px, py );
			GD_SetGraphColorNo( GRP_MAP_MOVE+3, 1, 128-126*rate, 12, 128, 22 );

			loop_count++;

			scope_select = pow(px-GameKey.mx,2)+pow(py-GameKey.my,2) <= pow(range*MapControl.zoom,2);
			rate = POYO_CONUT(scount,15,10);
			if( scope_select ){
				mx = GameKey.mx;
				my = GameKey.my;

				GD_SetGraphDisp( GRP_MAP_MOVE+2, TRUE );
				GD_SetGraphDstRect( GRP_MAP_MOVE+2, mx - rate*scope, my - rate*scope, rate*scope*2, rate*scope*2 );
				GD_SetGraphZoom( GRP_MAP_MOVE+2, MapControl.zoom );
				GD_SetGraphCenter( GRP_MAP_MOVE+2, (float)mx, (float)my );

				rate = loop_count2%96 /96.0f;
				rate = 1.0f-(1.0f-rate)*(1.0f-rate);

				GD_SetGraphDisp( GRP_MAP_MOVE+4, TRUE );
				GD_SetGraphDstRect( GRP_MAP_MOVE+4, mx - rate*scope, my - rate*scope, rate*scope*2, rate*scope*2 );
				GD_SetGraphZoom( GRP_MAP_MOVE+4, MapControl.zoom );
				GD_SetGraphCenter( GRP_MAP_MOVE+4, (float)mx, (float)my );
				GD_SetGraphColorNo( GRP_MAP_MOVE+4, 1, 128-126*rate, 128, 12, 12 );

				scount++;
				if(scount>=15)	loop_count2++;
			}else{
				GD_SetGraphDisp( GRP_MAP_MOVE+2, FALSE );
				GD_SetGraphDisp( GRP_MAP_MOVE+4, FALSE );
				scount=0;
				loop_count2=0;
			}

			
			CharMarkerPlayer=1;
			CharMarkerEnemy=1;
			MapControl.sc_mode=0;

			if(scope_select){
				AllCharControl(-2, cno, MagicParam[magic_no].scope_type, scope);
			}else{
				AllCharControl(-2, cno, MagicParam[magic_no].scope_type, 0);
			}
			if( scope_select && GameKey.click ){
				CommandStruct.ketei = TRUE;
				CommandStruct.action = BCMD_ACT_MAGIC;
				CommandStruct.mgno = magic_no;	
				CommandStruct.tx = MapControl.px + mx/MapControl.zoom;
				CommandStruct.ty = MapControl.py + my/MapControl.zoom;
				CommandStruct.cno   = cno;

				close_mgk=1;
				step=BCMD_MGK_RAGE_OUT;
				SND_Ok();
			}else if(GameKey.cansel){



				close_mgk=0;
				step=BCMD_MGK_RAGE_OUT;
				SND_Cansel();
			}
			break;

		case BCMD_MGK_SCOPE_OUT:
			rate = POYO_CONUT(scount,15,10);
			MapControl.sc_mode=1;

			GD_SetGraphDisp( GRP_MAP_MOVE+0, TRUE );
			GD_SetGraphColor( GRP_MAP_MOVE+0, 64*scount/15, 0, 0, 0 );

			if(scount){
				rate = POYO_CONUT(scount,15,10);

				GD_SetGraphDisp( GRP_MAP_MOVE+2, TRUE );
				GD_SetGraphDstRect( GRP_MAP_MOVE+2, px - rate*scope, py - rate*scope, rate*scope*2, rate*scope*2 );
				GD_SetGraphZoom( GRP_MAP_MOVE+2, MapControl.zoom );
				GD_SetGraphCenter( GRP_MAP_MOVE+2, (float)px, (float)py );
			}
			scount = LIM(scount-1,0,15);

			if( scount==0 ){
				GD_SetGraphDisp( GRP_MAP_MOVE+2, FALSE );
				count=0;
				if(close_mgk){
					bak_ret=1;
					step=BCMD_MGK_TERM;
				}else{
					step=BCMD_MGK_MENU_IN;
				}
			}

			break;
		case BCMD_MGK_SELF_SCOPE:
			count--;
			rate = POYO_CONUT(count,15,10);




			SetGameWindowMove( 2, BTL_COMMAND_X, 16+288-rate*288  );
			SetGameWindowColor( 2, count*255/15, 128, 128, 128 );

			if(count<=0){
				count=0;
			}
			rate = POYO_CONUT(scount,15,10);
			scount++;
			GD_SetGraphDisp( GRP_MAP_MOVE+2, TRUE );
			GD_SetGraphDstRect( GRP_MAP_MOVE+2, px - rate*scope, py - rate*scope, rate*scope*2, rate*scope*2 );
			GD_SetGraphZoom( GRP_MAP_MOVE+2, MapControl.zoom );
			GD_SetGraphCenter( GRP_MAP_MOVE+2, (float)px, (float)py );

			
			CharMarkerPlayer=1;
			CharMarkerEnemy=1;
			MapControl.sc_mode=0;
			AllCharControl(-2,cno, MagicParam[magic_no].scope_type, scope);
			if( GameKey.click ){
				CommandStruct.ketei = TRUE;
				CommandStruct.action = BCMD_ACT_MAGIC;
				CommandStruct.mgno = magic_no;	
				CommandStruct.taget = cno;
				CommandStruct.cno   = cno;

				close_mgk=1;
				step=BCMD_MGK_SCOPE_OUT;
				SND_Ok();
			}else if(GameKey.cansel){



				close_mgk=0;
				step=BCMD_MGK_SCOPE_OUT;
				SND_Cansel();
			}
			break;
	}
	return ret;
}

int	BTL_CoomandItem( int cno )
{
	enum{
		BCMD_MGK_INIT,
		BCMD_MGK_TERM,

		BCMD_MGK_MENU,
		BCMD_MGK_MENU_IN,
		BCMD_MGK_MENU_OUT,

		BCMD_MGK_RAGE,
		BCMD_MGK_RAGE_IN,
		BCMD_MGK_RAGE_OUT,

		BCMD_MGK_SCOPE,


		BCMD_MGK_SCOPE_OUT,
		BCMD_MGK_SELF_SCOPE,

	};
	static int	step=BCMD_MGK_INIT;
	static int	rcount=0;
	static int	count=0;
	static int	loop_count=0;
	static int	loop_count2=0;

	static int	page=0;
	static int	page_max=0;

	static int	magic_no=0;

	char		buf[1024];
	char		buf2[256];

	int			i,j;
	float		rate;
	int			ret=0;
	static int	bak_ret=0;
	static int	close_mgk=0;

	float		px,py;
	static int	mx,my;

	BOOL		scope_select;
	int			select_no = MUS_GetMouseNo( -1 );
	static int	bak_select = -2;
	static int	item_list[10];
	int			redraw=0;
	
	int			range=0;
	if(magic_no!=-1){
		range = 128;
	}

	px = (CharControl[cno].px-MapControl.px)*MapControl.zoom;
	py = (CharControl[cno].py-MapControl.py)*MapControl.zoom;
	switch(step){
		case BCMD_MGK_INIT:
			rcount=0;
			count=0;
			loop_count=0;
			loop_count2=0;
			page=0;
			close_mgk=0;
			bak_ret=0;
			bak_select = -2;

			step=BCMD_MGK_MENU_IN;

			GD_SetGraphPrimitive( GRP_MAP_MOVE+0, 4, LAY_EFFECT+1, OFF );
			GD_SetGraphBlendParam( GRP_MAP_MOVE+0, BLEND_ALPHA );
			GD_SetGraphColor( GRP_MAP_MOVE+0, 0, 0, 0, 0 );

			GD_SetGraphCircle( GRP_MAP_MOVE+1, 64, TRUE, LAY_EFFECT+2, OFF );
			GD_SetGraphBlendParam( GRP_MAP_MOVE+1, BLEND_ADD );
			GD_SetGraphColorNo( GRP_MAP_MOVE+1, 0,   0, 0,   0, 0 );
			GD_SetGraphColorNo( GRP_MAP_MOVE+1, 1, 255, 12, 128, 22 );

			GD_SetGraphCircle( GRP_MAP_MOVE+2, 32, TRUE, LAY_EFFECT+3, OFF );
			GD_SetGraphBlendParam( GRP_MAP_MOVE+2, BLEND_ADD );
			GD_SetGraphColorNo( GRP_MAP_MOVE+2, 0,   0, 0, 0, 0 );
			GD_SetGraphColorNo( GRP_MAP_MOVE+2, 1, 255, 128, 12, 12 );

			GD_SetGraphCircle( GRP_MAP_MOVE+3, 32, TRUE, LAY_EFFECT+2, OFF );
			GD_SetGraphBlendParam( GRP_MAP_MOVE+3, BLEND_ADD );
			GD_SetGraphColorNo( GRP_MAP_MOVE+3, 0,   0, 0,   0, 0 );
			GD_SetGraphColorNo( GRP_MAP_MOVE+3, 1, 255, 12, 128, 22 );

			GD_SetGraphCircle( GRP_MAP_MOVE+4, 16, TRUE, LAY_EFFECT+3, OFF );
			GD_SetGraphBlendParam( GRP_MAP_MOVE+4, BLEND_ADD );
			GD_SetGraphColorNo( GRP_MAP_MOVE+4, 0,   0, 0, 0, 0 );
			GD_SetGraphColorNo( GRP_MAP_MOVE+4, 1, 255, 128, 12, 12 );
			
			SetGameWindow( 2, GRP_SYSTEM, BMP_SYSTEM, LAY_SYSTEM+2, OFF, -1, 1 );
			SetGameWindowRect( 2, BTL_COMMAND_X, 16, 24*21, 288  );

			j=0;
			buf[0] = '\0';
			for(i=0;i<ITEM_MAX;i++){
				if(ItemBox[i].ino){
					if(ItemBox[i].amount==0) break;
					if(ITEM_TOOL_POTION1<=ItemParam[ItemBox[i].ino].type && ItemParam[ItemBox[i].ino].type<=ITEM_TOOL_POTION4){
						if( 0<=j-page && j-page<10){
							if(CharControl[cno].sw!=0){
								wsprintf( buf2, "<C2:%-16s HP%4d MP%4d %2d個>\n", ItemParam[ItemBox[i].ino].name, ItemParam[ItemBox[i].ino].hp, ItemParam[ItemBox[i].ino].mp, ItemBox[i].amount );
							}else{
								wsprintf( buf2, "%-16s HP%4d MP%4d %2d個\n", ItemParam[ItemBox[i].ino].name, ItemParam[ItemBox[i].ino].hp, ItemParam[ItemBox[i].ino].mp, ItemBox[i].amount );
							}
							_mbscat( (BYTE*)buf, (BYTE*)buf2 );
							item_list[j-page] = ItemBox[i].ino;
						}
						j++;
					}
				}
			}
			SetGameWindowText( 2, buf, BMP_SYSTEM+2,8 );
			page_max = j;

			MUS_SetMouseLayer( 2 );
			for(i=0;i<10;i++){
				MUS_SetMouseRect( 2, i, (int)BTL_COMMAND_X+24, 16+24+i*24, 24*18, 24, ON );
			}
			MUS_SetMouseRect( 2, 10, (int)BTL_COMMAND_X+24*18, 16+24-32, 44, 32, ON );
			MUS_SetMouseRect( 2, 11, (int)BTL_COMMAND_X+24*18, 16+264,   44, 32, ON );
			MUS_SetMouseRect( 2, 12, (int)BTL_COMMAND_X+24*19+8, 16+24,    16, 24*10, ON );

	GD_SetGraph( GRP_START_CHAR+10, BMP_SYSTEM, LAY_SYSTEM+3, TRUE );	
	GD_SetGraphRect( GRP_START_CHAR+10, (int)BTL_COMMAND_X+24*18, 16+24-32,     624+0*44, 432, 44, 32 );
	GD_SetGraph( GRP_START_CHAR+11, BMP_SYSTEM, LAY_SYSTEM+3, TRUE );	
	GD_SetGraphRect( GRP_START_CHAR+11, (int)BTL_COMMAND_X+24*18, 16+264, 624+0*44, 464, 44, 32 );

	GD_SetGraph( GRP_START_CHAR+12, BMP_SYSTEM, LAY_SYSTEM+3, TRUE );
	GD_SetGraph( GRP_START_CHAR+13, BMP_SYSTEM, LAY_SYSTEM+3, TRUE );
	GD_SetGraph( GRP_START_CHAR+14, BMP_SYSTEM, LAY_SYSTEM+3, TRUE );
	GD_SetGraphSrcRect( GRP_START_CHAR+12, 640, 32+496,    16,  6 );
	GD_SetGraphSrcRect( GRP_START_CHAR+13, 640, 32+496+6,  16, 20 );
	GD_SetGraphSrcRect( GRP_START_CHAR+14, 640, 32+496+26, 16,  6 );

	GD_SetGraphDstRect( GRP_START_CHAR+12, (int)BTL_COMMAND_X+24*19+8,  40,          16,  6 );
	GD_SetGraphDstRect( GRP_START_CHAR+13, (int)BTL_COMMAND_X+24*19+8,  40+6,        16,  240-12 );
	GD_SetGraphDstRect( GRP_START_CHAR+14, (int)BTL_COMMAND_X+24*19+8,  40+6+240-12, 16,  6 );

	GD_SetGraph( GRP_START_CHAR+15, BMP_SYSTEM, LAY_SYSTEM+4, TRUE );
	GD_SetGraph( GRP_START_CHAR+16, BMP_SYSTEM, LAY_SYSTEM+4, TRUE );
	GD_SetGraph( GRP_START_CHAR+17, BMP_SYSTEM, LAY_SYSTEM+4, TRUE );
	GD_SetGraphSrcRect( GRP_START_CHAR+15, 624, 32+496,    16,  6 );
	GD_SetGraphSrcRect( GRP_START_CHAR+16, 624, 32+496+6,  16, 20 );
	GD_SetGraphSrcRect( GRP_START_CHAR+17, 624, 32+496+26, 16,  6 );
	for( i=0 ; i < 8 ; i++ ){
		GD_SetGraphDisp( GRP_START_CHAR+10+i, FALSE );
	}

			GD_SetGraphx9( GRP_START_CHAR+3, BMP_SYSTEM, LAY_SYSTEM+3, FALSE );	
			GD_SetGraphSrcRect( GRP_START_CHAR+3, 0, 64, 72, 24 );
			break;

		case BCMD_MGK_TERM:
			GD_ResetGraph( GRP_MAP_MOVE+0 );
			GD_ResetGraph( GRP_MAP_MOVE+1 );
			GD_ResetGraph( GRP_MAP_MOVE+2 );
			GD_ResetGraph( GRP_MAP_MOVE+3 );
			GD_ResetGraph( GRP_MAP_MOVE+4 );

			for( i=0 ; i < 8 ; i++ ){
				GD_ResetGraph( GRP_START_CHAR+10+i );
			}

			GD_ResetGraph( GRP_START_CHAR+3 );

			ResetGameWindow( 2 );
			GD_ReleaseTexture( BMP_SYSTEM+2 );

			CharMarkerPlayer=0;
			CharMarkerEnemy =0;

			MUS_ResetMouseRect_Layer( 2 );
			MUS_SetMouseLayer( 1 );

			step=BCMD_MGK_INIT;
			ret=bak_ret;
			break;

		case BCMD_MGK_MENU_IN:
			if(count==0){
				ResetPouupCharWindow( 0 );
				for(i=0;i<CHAR_CONTROL_MAX;i++){
					CharMarkerSelect[i] = 0;
					CharMarkerAtack[i] = 0;
				}
				CharMarkerSelect[cno] = 1;
			}
			for(i=0;i<5;i++)
				GD_SetGraphDisp( GRP_MAP_MOVE+i, FALSE );

			count++;
			rate = POYO_CONUT(count,15,10);

			SetGameWindowDisp( 2, ON );
			SetGameWindowColor( 2, count*255/15, 128, 128, 128 );
			SetGameWindowZoom( 2, 0.5f+rate/2 );
			SetGameWindowRect( 2, BTL_COMMAND_X, 16, 24*21, 288  );

			if(count>=15){
				count=0;
				step=BCMD_MGK_MENU;
			}
			break;
		case BCMD_MGK_MENU_OUT:
			GD_SetGraphDisp( GRP_START_CHAR+3, FALSE );

			count--;
			rate = POYO_CONUT(count,15,10);

			SetGameWindowMove( 2, BTL_COMMAND_X, 16+288-rate*288  );
			SetGameWindowColor( 2, count*255/15, 128, 128, 128 );

			if(count<=0){
				step=BCMD_MGK_TERM;
			}
			break;

		case BCMD_MGK_MENU:
	{
		int	page_max2 = max(10,page_max);
		if( page_max>10 ){
			for( i=0 ; i < 8 ; i++ ){
				GD_SetGraphDisp( GRP_START_CHAR+10+i, TRUE );
			}

			if(select_no==10){
				if( select_no+page!=bak_select){
					SND_Select();
				}
				if(GameKey.click)	GD_SetGraphSrcRect( GRP_START_CHAR+10, 624+2*44, 432, 44, 32 ),appData.draw_flag=1;
				else				GD_SetGraphSrcRect( GRP_START_CHAR+10, 624+1*44, 432, 44, 32 );
			}else	GD_SetGraphSrcRect( GRP_START_CHAR+10, 624+0*44, 432, 44, 32 );
			if(select_no==11){
				if( select_no+page!=bak_select){
					SND_Select();
				}
				if(GameKey.click)	GD_SetGraphSrcRect( GRP_START_CHAR+11, 624+2*44, 464, 44, 32 ),appData.draw_flag=1;
				else				GD_SetGraphSrcRect( GRP_START_CHAR+11, 624+1*44, 464, 44, 32 );
			}else	GD_SetGraphSrcRect( GRP_START_CHAR+11, 624+0*44, 464, 44, 32 );
	

			i = page*(240-12-228*10/page_max2)/(page_max2-10);
			j = 228*10/page_max2;

			GD_SetGraphDstRect( GRP_START_CHAR+15, (int)BTL_COMMAND_X+24*19+8,  i + 40,   16,  6 );	
			GD_SetGraphDstRect( GRP_START_CHAR+16, (int)BTL_COMMAND_X+24*19+8,  i + 40+6, 16,  j );
			GD_SetGraphDstRect( GRP_START_CHAR+17, (int)BTL_COMMAND_X+24*19+8,  i + 40+6+j,   16,  6 );
		}else{
			for( i=0 ; i < 8 ; i++ ){
				GD_SetGraphDisp( GRP_START_CHAR+10+i, FALSE );
			}
		}
	}
redraw = BTL_DragRightBar10( &page, &page_max, select_no );

			if(count==0 || redraw){
				j=0;
				buf[0] = '\0';
				for(i=0;i<ITEM_MAX;i++){
					if(ItemBox[i].ino){
						if(ItemBox[i].amount==0) break;
						if(ITEM_TOOL_POTION1<=ItemParam[ItemBox[i].ino].type && ItemParam[ItemBox[i].ino].type<=ITEM_TOOL_POTION4){
							if( 0<=j-page && j-page<10){
								if(CharControl[cno].sw!=0 ){
									wsprintf( buf2, "<C2:%-16s HP%4d MP%4d %2d個>\n", ItemParam[ItemBox[i].ino].name, ItemParam[ItemBox[i].ino].hp, ItemParam[ItemBox[i].ino].mp, ItemBox[i].amount );
								}else{
									wsprintf( buf2, "%-16s HP%4d MP%4d %2d個\n", ItemParam[ItemBox[i].ino].name, ItemParam[ItemBox[i].ino].hp, ItemParam[ItemBox[i].ino].mp, ItemBox[i].amount );
								}
								_mbscat( (BYTE*)buf, (BYTE*)buf2 );
								item_list[j-page] = ItemBox[i].ino;
							}
							j++;
						}
					}
				}
				SetGameWindowText( 2, buf, BMP_SYSTEM+2,8 );
				page_max = j;

			}
			if( select_no!=-1 && select_no < min(10,page_max) ){
				GD_SetGraphDisp( GRP_START_CHAR+3, TRUE );
				GD_SetGraphDstRect( GRP_START_CHAR+3, BTL_COMMAND_X+24, 16+24+24*select_no, 24*18,  24 );
				if( select_no+page!=bak_select){
					SND_Select();

					if(CharControl[cno].sw!=0 ){
						wsprintf( buf2, "技硬直中です。道具を使用できません。\n残り %d.%d 秒。", CharControl[cno].sw/60, CharControl[cno].sw%60/6 );
						SetGameWindowText( 15, buf2 );
					}else{
						SetGameWindowText( 15, ItemParam[item_list[select_no]].help );
					}
				}
			}else{
				if( select_no+page==bak_select){
					SetGameWindowText( 15, "使用する道具を選んで下さい。" );
				}
				GD_SetGraphDisp( GRP_START_CHAR+3, FALSE );
			}
			count++;

			if(GameKey.click){
				if( select_no!=-1 && select_no<min(10,page_max) ){
					if(CharControl[cno].sw!=0){
						SND_Ng();
					}else{
						if( select_no+page < page_max ){
							for( i=0 ; i < 8 ; i++ ){
								GD_SetGraphDisp( GRP_START_CHAR+10+i, FALSE );
							}

							magic_no = item_list[select_no];
							count=15;
							GD_SetGraphDisp( GRP_START_CHAR+3, FALSE );

							step=BCMD_MGK_RAGE_IN;
							SND_Ok();
						}
					}
				}
			}
			if(GameKey.cansel){
				for( i=0 ; i < 8 ; i++ ){
					GD_SetGraphDisp( GRP_START_CHAR+10+i, FALSE );
				}
				count=15;
				bak_ret=2;
				step=BCMD_MGK_MENU_OUT;
				SND_Cansel();
			}
			bak_select = select_no+page;
			break;

		case BCMD_MGK_RAGE_IN:
			count--;
			rate = POYO_CONUT(count,15,10);

			SetGameWindowMove( 2, BTL_COMMAND_X, 16+288-rate*288  );
			SetGameWindowColor( 2, count*255/15, 128, 128, 128 );

			if(count<=0){
				count=0;
			}
			
			rate = POYO_CONUT(rcount,15,10);

			GD_SetGraphDisp( GRP_MAP_MOVE+0, TRUE );
			GD_SetGraphColor( GRP_MAP_MOVE+0, 64*rcount/15, 0, 0, 0 );

			GD_SetGraphDisp( GRP_MAP_MOVE+1, TRUE );
			GD_SetGraphDstRect( GRP_MAP_MOVE+1, px - rate*range, py - rate*range, rate*range*2, rate*range*2 );
			GD_SetGraphZoom( GRP_MAP_MOVE+1, MapControl.zoom );
			GD_SetGraphCenter( GRP_MAP_MOVE+1, px, py );

			if(rcount>=15){
				step=BCMD_MGK_RAGE;
			}else{
				rcount++;
			}
			break;
		case BCMD_MGK_RAGE_OUT:
			rate = POYO_CONUT(rcount,15,10);
			MapControl.sc_mode=1;

			GD_SetGraphDisp( GRP_MAP_MOVE+0, TRUE );
			GD_SetGraphColor( GRP_MAP_MOVE+0, 64*rcount/15, 0, 0, 0 );

			GD_SetGraphDisp( GRP_MAP_MOVE+1, TRUE );
			GD_SetGraphDstRect( GRP_MAP_MOVE+1, px - rate*range, py - rate*range, rate*range*2, rate*range*2 );
			GD_SetGraphZoom( GRP_MAP_MOVE+1, MapControl.zoom );
			GD_SetGraphCenter( GRP_MAP_MOVE+1, px, py );

			rcount = LIM(rcount-1,0,15);

			if( rcount==0 ){
				GD_SetGraphDisp( GRP_MAP_MOVE+1, FALSE );
				count=0;
				if(close_mgk){
					bak_ret=1;
					step=BCMD_MGK_TERM;
				}else{
					step=BCMD_MGK_MENU_IN;
				}
			}
			break;

		case BCMD_MGK_RAGE:
			GD_SetGraphDisp( GRP_MAP_MOVE+1, TRUE );
			GD_SetGraphDstRect( GRP_MAP_MOVE+1, (float)(px - range), (float)(py - range), (float)(range*2), (float)(range*2) );
			GD_SetGraphZoom( GRP_MAP_MOVE+1, MapControl.zoom );
			GD_SetGraphCenter( GRP_MAP_MOVE+1, px, py );

			CharMarkerPlayer=1;
			CharMarkerEnemy =1;
			MapControl.sc_mode=0;
			i = AllCharControl( -1, cno );
			if( i!=-1 ){
				scope_select = pow(CharControl[cno].px-CharControl[i].px,2)+pow(CharControl[cno].py-CharControl[i].py,2) <= pow(range,2);
				if( scope_select ){
					CommandStruct.ketei = TRUE;
					CommandStruct.action = BCMD_ACT_ITEM;
					CommandStruct.mgno = magic_no;	
					CommandStruct.taget = i;
					CommandStruct.cno   = cno;

					close_mgk=1;
					step=BCMD_MGK_RAGE_OUT;
					SND_Ok();
				}else{
					SND_Ng();
				}
			}else if(GameKey.cansel){
				close_mgk=0;
				step=BCMD_MGK_RAGE_OUT;
				SND_Cansel();
			}
			break;
	}
	return ret;
}



int	BTL_CoomandAuto( int cno )
{
	enum{
		BCMD_AUTO_INIT,
		BCMD_AUTO_TERM,

		BCMD_AUTO_MENU,
		BCMD_AUTO_MENU_IN,
		BCMD_AUTO_MENU_OUT,
	};
	static int	step=BCMD_AUTO_INIT;
	static int	count=0;

	static int	ret_bak=0;

	char		buf[1024];
	char		buf2[256];

	int			i;
	float		rate;
	int			ret=0;
	float		px,py;
	static int	mx,my;

	int			select_no = MUS_GetMouseNo( -1 );
	static int	bak_select=-1;

	switch(step){
		case BCMD_AUTO_INIT:
			count=0;
			step=BCMD_AUTO_MENU_IN;
			
			SetGameWindow( 2, GRP_SYSTEM, BMP_SYSTEM, LAY_SYSTEM+2, OFF, -1, 1 );
			SetGameWindowRect( 2, BTL_COMMAND_X, 16, 24*20, 24*5+48  );
			SetGameWindowText( 2, "おまかせCPU\n突撃！\nよく狙え\n回復に専念せよ\n魔力を惜しむな", BMP_SYSTEM+2, 8 );

			MUS_SetMouseLayer( 2 );
			for(i=0;i<5;i++){
				MUS_SetMouseRect( 2, i, (int)BTL_COMMAND_X+24, 16+24+i*24, 24*18, 24, ON );
			}
			bak_select=-2;

			GD_SetGraphx9( GRP_START_CHAR+3, BMP_SYSTEM, LAY_SYSTEM+3, FALSE );	
			GD_SetGraphSrcRect( GRP_START_CHAR+3, 0, 64, 72, 24 );

			break;

		case BCMD_AUTO_TERM:
			ResetGameWindow( 2 );
			GD_ResetGraph( GRP_START_CHAR+3 );
			
			CharMarkerPlayer=0;
			CharMarkerEnemy =0;

			MUS_ResetMouseRect_Layer( 2 );
			MUS_SetMouseLayer( 1 );

			step=BCMD_AUTO_INIT;
			ret=ret_bak;
			break;

		case BCMD_AUTO_MENU_IN:
			count++;
			rate = POYO_CONUT(count,15,10);

			SetGameWindowDisp( 2, ON );
			SetGameWindowColor( 2, count*255/15, 128, 128, 128 );
			SetGameWindowZoom( 2, 0.5f+rate/2 );
			if(count>=15){
				step=BCMD_AUTO_MENU;
			}
			break;
		case BCMD_AUTO_MENU_OUT:
			GD_SetGraphDisp( GRP_START_CHAR+3, FALSE );

			count--;
			rate = POYO_CONUT(count,15,10);

			SetGameWindowMove( 2, BTL_COMMAND_X, 16+288-rate*288  );
			SetGameWindowColor( 2, count*255/15, 128, 128, 128 );

			if(count<=0){
				step=BCMD_AUTO_TERM;
			}
			break;

		case BCMD_AUTO_MENU:

			if( select_no!=-1 ){
				GD_SetGraphDisp( GRP_START_CHAR+3, TRUE );
				GD_SetGraphDstRect( GRP_START_CHAR+3, BTL_COMMAND_X+24, 16+24+24*select_no, 24*18,  24 );
			}else{
				GD_SetGraphDisp( GRP_START_CHAR+3, FALSE );
			}

			if( bak_select != select_no ){
				SND_Select();
				switch(select_no){
				case -1:SetGameWindowText( 15, "自動モードを選択してください。" );	break;

				case 0:	SetGameWindowText( 15, "多彩な攻撃や援護をします。\nちょっぴり、おバカ？" );	break;
				case 1:	SetGameWindowText( 15, "最短の敵に向かって突撃！\n背を見せないので、最前線を任せる戦士に向いています。" );	break;
				case 2:	SetGameWindowText( 15, "最もダメージを与え得る敵を攻撃します。\n後方から攻撃できる弓兵、槍兵に向いています。" );	break;
				case 3:	SetGameWindowText( 15, "回復中心に魔法を使い、MP消費の多い攻撃魔法は控えます。" );	break;
				case 4:	SetGameWindowText( 15, "後先考えずに攻撃魔法を打ち込みつづけます。" );	break;
				}
			}
			
			if(GameKey.click){
				if( select_no!=-1 ){
					CommandStruct.ketei = TRUE;
					CommandStruct.cno = cno;
					CommandStruct.action = BCMD_ACT_AUTO;
					CommandStruct.mgno = select_no;
					step=BCMD_AUTO_MENU_OUT;
					ret_bak=1;
					SND_Ok();
				}
			}
			if(GameKey.cansel){
				ret_bak=2;
				step=BCMD_AUTO_MENU_OUT;
				SND_Cansel();
			}
			bak_select = select_no;
			break;

	}
	return ret;
}
void Retaia( void )
{
	int	i;
	for(i=0;i<CHAR_CONTROL_MAX;i++){
		if( !CharControl[i].flag ) continue;
		if( CharParam[CharControl[i].cno].unit!=0 ) continue;
		
		CharParam[CharControl[i].cno].php=0;
		CharParam[CharControl[i].cno].did=TRUE;
	}
}


void AllAutoComand( int cmd )
{
	int	i;
	for(i=0;i<CHAR_CONTROL_MAX;i++){
		if( !CharControl[i].flag ) continue;
		if( CharParam[CharControl[i].cno].unit!=0 ) continue;
		if( CharParam[CharControl[i].cno].did ) continue;
		if(cmd==0){
			ResetMapCharThink( i );
			SetMapCharStand( i );
			continue;
		}
		switch(CharParam[CharControl[i].cno].job){
		case 13:
		case 0:
		case 6:
		case 20:
			switch(cmd){
			case 1:
				SetMapCharThink( i, MOVE_THINK_ATACK,   ACTION_THINK_ATACK );
				break;
			case 2:
				SetMapCharThink( i, MOVE_THINK_ATACK,   ACTION_THINK_SHOT );
				break;
			case 3:
				SetMapCharThink( i, MOVE_THINK_ATACK,   ACTION_THINK_HEAL );
				break;
			}
			break;

		case 4:
		case 17:
		case 10:
		case 5:
		case 11:
		case 15:
			switch(cmd){
			case 1:
				SetMapCharThink( i, MOVE_THINK_ATACK,   ACTION_THINK_SHOT );
				break;
			case 2:
				SetMapCharThink( i, MOVE_THINK_ATACK,   ACTION_THINK_ATACK );
				break;
			case 3:
				SetMapCharThink( i, MOVE_THINK_ATACK,   ACTION_THINK_HEAL );
				break;
			}
			break;

		case 2:
		case 3:
		case 16:
			switch(cmd){
			case 1:
				SetMapCharThink( i, MOVE_THINK_SUPPORT, ACTION_THINK_HEAL );
				break;
			case 2:
				SetMapCharThink( i, MOVE_THINK_SUPPORT, ACTION_THINK_HEAL_MAGIC );
				break;
			case 3:
				SetMapCharThink( i, MOVE_THINK_SUPPORT, ACTION_THINK_HEAL );
				break;
			}
			break;

		case 1:
		case 19:
			switch(cmd){
			case 1:
				SetMapCharThink( i, MOVE_THINK_ATACK,   ACTION_THINK_KNIGHT );
				break;
			case 2:
				SetMapCharThink( i, MOVE_THINK_ATACK,   ACTION_THINK_MAGIC );
				break;
			case 3:
				SetMapCharThink( i, MOVE_THINK_ATACK, ACTION_THINK_HEAL );
				break;
			}
			break;

		case 7:
		case 12:
		case 18:
			switch(cmd){
			case 1:
				SetMapCharThink( i, MOVE_THINK_ATACK,   ACTION_THINK_KNIGHT );
				break;
			case 2:
				SetMapCharThink( i, MOVE_THINK_ATACK,   ACTION_THINK_ATACK );
				break;
			case 3:
				SetMapCharThink( i, MOVE_THINK_ATACK, ACTION_THINK_HEAL );
				break;
			}
			break;

		case 8:
		case 9:
		case 14:
			switch(cmd){
			case 1:
				SetMapCharThink( i, MOVE_THINK_SUPPORT, ACTION_THINK_MAGIC );
				break;
			case 2:
				SetMapCharThink( i, MOVE_THINK_SUPPORT, ACTION_THINK_MAGIC );
				break;
			case 3:
				SetMapCharThink( i, MOVE_THINK_SUPPORT, ACTION_THINK_HEAL_MAGIC );
				break;
			}
			break;
		}
	}
}




int	BTL_MenuCoomand( int *aria )
{
	enum{
		BMCM_INIT,
		BMCM_TERM,

		BMCM_FADE_OUT,
		BMCM_MAIN,
	};
	static int	step=BMCM_INIT;
	static int	count=0;
	char		buf[512];
	char		buf2[256];
	static char	taget_tbl[CHAR_CONTROL_MAX];
	static int	bpage_max=0;
	static int	set_comand=0;

	int			select_no = MUS_GetMouseNo( -1 );
	static int	bak_select = -1;
	
	float		px,py;

	float		rate;
	int			ret=1;
	int			i,j;
	
	switch(step){
		case BMCM_INIT:
			MUS_SetMouseLayer( 1 );

			SetPouupCharWindow( 0, -1 );
			MapControl.sc_mode=1;

			GD_SetGraphPrimitive( GRP_MAP_FADE, 4, LAY_MAP_FADE, ON );
			GD_SetGraphDstRect( GRP_MAP_FADE, 0, 0, 800, 600 );
			GD_SetGraphColor( GRP_MAP_FADE, 0, 0, 0, 0 );
			GD_SetGraphBlendParam( GRP_MAP_FADE, BLEND_ALPHA );

			j=0;
			for(i=0;i<CHAR_CONTROL_MAX;i++){
				if( !CharControl[i].flag ) continue;
				if( !CharParam[CharControl[i].cno].flag ) continue;
				if( CharParam[CharControl[i].cno].unit!=0 ) continue;

				if( CharParam[CharControl[i].cno].job==89) continue;

				taget_tbl[j] = i;
				j++;
			}
			bpage_max=j;
			buf[0] = '\0';
			for(i=0 ; i<bpage_max ; i++){

				if(CharParam[CharControl[taget_tbl[i]].cno].did){
					wsprintf( buf2, "<C2:%-14s Lv.%3d>\n", CharParam[CharControl[taget_tbl[i]].cno].name, CharParam[CharControl[taget_tbl[i]].cno].lv );
				}else{
					wsprintf( buf2, "%-14s Lv.%3d\n", CharParam[CharControl[taget_tbl[i]].cno].name, CharParam[CharControl[taget_tbl[i]].cno].lv );
				}
				_mbscat( (BYTE*)buf, (BYTE*)buf2 );

				MUS_SetMouseRect( 1, i, 16+24, 16+24+i*24, 24*11, 24, ON );
			}
			SetGameWindow( 3, GRP_SYSTEM+10, BMP_SYSTEM, LAY_SYSTEM, FALSE,-1, 1 );
			SetGameWindowRect( 3, 16, 16, 24*11+48, 48+24*bpage_max );	
			SetGameWindowText( 3, buf, BMP_SYSTEM+3 );

			SetGameWindow( 4, GRP_START_CHAR+5, BMP_SYSTEM, LAY_SYSTEM, FALSE,-1, 1 );
			SetGameWindowRect( 4, 800-(24*14+48)-16, 16, 24*14+48, 24*4+48 );	
			wsprintf ( buf, "命令あるまで、その場で待機。\n"
							"各自の判断で行動せよ。\n"
							"総員、突撃せよ！！！！\n"
							"回復を最優先に。" );
			SetGameWindowText( 4, buf, BMP_START_CHAR+4 );

			for(i=0 ; i<4 ; i++){
				MUS_SetMouseRect( 1, 8+i, 800-(24*14+48)-16+24, 16+24+i*24, 24*14, 24, ON );
			}

			SetGameWindow( 14, GRP_START_CHAR+10, BMP_SYSTEM, LAY_SYSTEM+1, FALSE, -1, 1 );
			SetGameWindowRect( 14, 800-(24*14+48)-16, 16+24*4+48, 24*14+48, 24*1+48  );
			SetGameWindowText( 14, "リタイアする。", BMP_START_CHAR+3 );
			MUS_SetMouseRect( 1, 13, 800-(24*14+48)-16+24, 16+24*4+72, 24*14, 24*1, ON );


			SetGameWindow( 15, GRP_START_CHAR+0, BMP_SYSTEM, LAY_SYSTEM+1, FALSE, -1, 1 );
			SetGameWindowRect( 15, 16, 600-72-16, 360, 72  );
			MUS_SetMouseRect( 1, 12, 16+24, 600-72-16+24, 360-48, 72-48, ON );
			switch(StartPoint.btype){
				default:
				case WIN_NORMAL:	
					wsprintf ( buf, "クリア条件：<B255:128:128:敵を殲滅せよ！>" );
					break;
				case WIN_BOSS:		
					CharMarkerBoss = StartPoint.bcno;
					wsprintf ( buf, "クリア条件：<B255:255:128:目標を撃破せよ！>" );
					break;
				case WIN_POINT:		
					GD_SetGraphCircle( GRP_START_CHAR+23, 64, TRUE, LAY_EFFECT+2, FALSE );
					GD_SetGraphBrightNo( GRP_START_CHAR+23, 1 );

					wsprintf ( buf, "クリア条件：<B128:255:255:目標に到達せよ！>" );
					break;
				case WIN_POINT2:	
				case WIN_TIME:		
					wsprintf ( buf, "クリア条件：<B255:128:128:敵を殲滅せよ！>" );
					break;
				case WIN_GIGA:
					wsprintf ( buf, "クリア条件：<B255:128:128:ギガースを守れ！>" );
					break;
			}
			SetGameWindowText( 15, buf, BMP_START_CHAR+2 );

			GD_SetGraphx9( GRP_START_CHAR+3, BMP_SYSTEM, LAY_SYSTEM+1, FALSE );	
			GD_SetGraphSrcRect( GRP_START_CHAR+3, 0, 64, 72, 24 );

			set_comand = 0;
			step=BMCM_MAIN;
			break;
		case BMCM_TERM:
			CharMarkerBoss = -1;
			GD_ResetGraph( GRP_MAP_FADE );
			GD_ResetGraph( GRP_START_CHAR+3 );
			GD_ResetGraph( GRP_START_CHAR+23 );
			ResetGameWindow( 3 );
			ResetGameWindow( 4 );
			ResetGameWindow( 14 );
			ResetGameWindow( 15 );

			MUS_ResetMouseRect_Layer( 1 );
			MUS_SetMouseLayer( 0 );

			if(set_comand){
				ret = 2;
			}else{
				MapControl.sc_mode=0;
				ret = 0;
			}
			step=BMCM_INIT;
			break;

		case BMCM_FADE_OUT:
			GD_SetGraphDisp( GRP_START_CHAR+3, FALSE );

			count = LIM(count-1,0,15);
			rate = 1-POYO_CONUT(count,15,10);

			GD_SetGraphColor( GRP_MAP_FADE, count*4, 0, 0, 0 );

			SetGameWindowZoomXY( 3, 1-rate, 1+rate );
			SetGameWindowColor( 3, 255*count/15, 128, 128, 128 );

			SetGameWindowZoomXY( 4, 1-rate, 1+rate );
			SetGameWindowColor( 4, 255*count/15, 128, 128, 128 );

			SetGameWindowZoomXY( 14, 1-rate, 1+rate );
			SetGameWindowColor( 14, 255*count/15, 128, 128, 128 );

			SetGameWindowZoomXY( 15, 1-rate, 1+rate );
			SetGameWindowColor( 15, 255*count/15, 128, 128, 128 );


			if(count==0){
				step=BMCM_TERM;
			}
			break;
		case BMCM_MAIN:
			count = LIM(count+1,0,15);
			rate = 1-POYO_CONUT(count,15,10);

			GD_SetGraphColor( GRP_MAP_FADE, count*4, 0, 0, 0 );

			SetGameWindowDisp( 3, TRUE );
			SetGameWindowZoomXY( 3, 1-rate, 1+rate );
			SetGameWindowColor( 3, 255*count/15, 128, 128, 128 );

			SetGameWindowDisp( 4, TRUE );
			SetGameWindowZoomXY( 4, 1-rate, 1+rate );
			SetGameWindowColor( 4, 255*count/15, 128, 128, 128 );


			SetGameWindowDisp( 14, TRUE );
			SetGameWindowZoomXY( 14, 1-rate, 1+rate );
			SetGameWindowColor( 14, 255*count/15, 128, 128, 128 );

			SetGameWindowDisp( 15, TRUE );
			SetGameWindowZoomXY( 15, 1-rate, 1+rate );
			SetGameWindowColor( 15, 255*count/15, 128, 128, 128 );

			switch(StartPoint.btype){
				case WIN_POINT:		
					GD_SetGraphDisp( GRP_START_CHAR+23, TRUE );
					GD_SetGraphColorNo( GRP_START_CHAR+23, 0,   0, 128,   0, 0 );
					GD_SetGraphColorNo( GRP_START_CHAR+23, 1, 128+STD_LimitLoop(GlobalCount*2,64)*2, 255, 128, 128 );

					px = (StartPoint.bx-MapControl.px)*MapControl.zoom;
					py = (StartPoint.by-MapControl.py)*MapControl.zoom;
					rate = StartPoint.brange;
					GD_SetGraphDstRect( GRP_START_CHAR+23, (float)(px - rate), (float)(py - rate), (float)(rate*2), (float)(rate*2) );
					GD_SetGraphZoom( GRP_START_CHAR+23, MapControl.zoom );
					GD_SetGraphCenter( GRP_START_CHAR+23, px, py );
					break;
			}

			if(bak_select != select_no){
				SND_Select();
			}
			if( 0<=select_no && select_no<8 ){
				GD_SetGraphDisp( GRP_START_CHAR+3, TRUE );
				GD_SetGraphDstRect( GRP_START_CHAR+3, 16+24, 16+24+select_no*24, 24*11, 24 );

				if(CharParam[CharControl[taget_tbl[select_no]].cno].did==0){
					SetPouupCharWindow( 1, CharControl[taget_tbl[select_no]].cno );
					SetScrolBattleMapChar( taget_tbl[select_no], MapControl.ezoom, 15, 2 );

					if(GameKey.click){
						SetMapCharCommand(taget_tbl[select_no], TRUE);
						step=BMCM_FADE_OUT;
						set_comand = 1;
						SND_Ok();
					}
				}
			}
			if( 8<=select_no && select_no<8+4 ){
				GD_SetGraphDisp( GRP_START_CHAR+3, TRUE );
				GD_SetGraphDstRect( GRP_START_CHAR+3, 800-(24*14+48)-16+24, 16+24+(select_no-8)*24, 24*14, 24 );

				if(GameKey.click){
					AllAutoComand( select_no-8 );
					step=BMCM_FADE_OUT;
					SND_Ok();
				}
			}
			if(select_no==12){
				switch(StartPoint.btype){
					case WIN_BOSS:		
						SetScrolBattleMapChar( StartPoint.bcno, MapControl.ezoom, 15, 2 );
						if(StartPoint.bcno>=0){
							SetPouupCharWindow( 1, CharControl[StartPoint.bcno].cno );
						}
						break;
					case WIN_POINT:		
						SetScrolBattleMap( StartPoint.bx, StartPoint.by, MapControl.ezoom, 15, 2 );
						break;
				}
			}
			if( select_no==13 ){
				GD_SetGraphDisp( GRP_START_CHAR+3, TRUE );
				GD_SetGraphDstRect( GRP_START_CHAR+3, 800-(24*14+48)-16+24, 16+24+6*24, 24*14, 24 );
				if(GameKey.click){
					Retaia();
					step=BMCM_FADE_OUT;
					SND_Ok();
				}
			}
			if(select_no==-1){
				GD_SetGraphDisp( GRP_START_CHAR+3, FALSE );
				SetPouupCharWindow( 1, -1 );
			}

			if(GameKey.cansel){
				CharMarkerBoss = -1;
				step=BMCM_FADE_OUT;
				SND_Cansel();
			}
			bak_select = select_no;
			break;

	}
	return ret;
}

int	BTL_Coomand( int *aria )
{
	enum{
		BCMD_INIT,
		BCMD_TERM,
		BCMD_SCROLL,
		BCMD_SELECT_IN,
		BCMD_SELECT_IN2,
		BCMD_SELECT,
		BCMD_SELECT_OUT,
		BCMD_MOVE,
		BCMD_ATACK,
		BCMD_MAGIC,
		BCMD_TECHNIC,
		BCMD_ITEM,
		BCMD_AUTO,
	};
	static int	step=BCMD_INIT;
	static int	count=0;
	static int	select_count[BTL_COMMAND_MAX]={0,0,0, 0,0,0,0};
	int			ret=-1;
	int			cno,i,j;
	float		rate;

	int			select_no = MUS_GetMouseNo( -1 );
	static int	bak_select=-1;

	cno = GetCharCommandWait();
	if( cno == -1){
		step=BCMD_TERM;
	}
	*aria=0;
	switch(step){
		case BCMD_INIT:
			SetScrolBattleMapChar( cno, MapControl.ezoom, 15, 2 );
			step=BCMD_SCROLL;
			MapControl.sc_mode=1;

			for(i=0;i<CHAR_CONTROL_MAX;i++){
				CharMarkerSelect[i] = 0;
			}
			CharMarkerSelect[cno] = 1;

			MUS_SetMouseLayer( 1 );
			for(i=0;i<BTL_COMMAND_MAX;i++){
				select_count[i] = 0;
				MUS_SetMouseRect( 1, i, (int)(42*i+BTL_COMMAND_X), (int)BTL_COMMAND_Y, 42, 30, ON );
			}

			SetGameWindow( 15, GRP_START_CHAR+0, BMP_SYSTEM, LAY_SYSTEM+1, TRUE, 6, 1 );
			SetGameWindowRect( 15, 16, 600-(24*4+48)-16, 360, 24*4+48  );
			SetGameWindowText( 15, "ユニットの行動を選択してください。", BMP_START_CHAR+2 );
			SetGameWindowColor( 15, 0, 128, 128, 128 );

			break;
		case BCMD_TERM:
			if(CommandStruct.ketei){
				switch(CommandStruct.action){
				case BCMD_ACT_MOVE:
					SetKipuMapCharThink( CommandStruct.cno  );

					SetMapCharTartgetMove( CommandStruct.cno, (int)CommandStruct.tx, (int)CommandStruct.ty );
					break;
				case BCMD_ACT_ATK:
					SetKipuMapCharThink( CommandStruct.cno  );

					SetMapCharTartgetAtack( CommandStruct.cno, CommandStruct.taget );
					break;
				case BCMD_ACT_MAGIC:
				case BCMD_ACT_TECHNIC:
					SetKipuMapCharThink( CommandStruct.cno  );

					SetMapCharTartgetMagic( CommandStruct.cno, CommandStruct.mgno, CommandStruct.taget, CommandStruct.tx, CommandStruct.ty );
					break;
				case BCMD_ACT_ITEM:
					SetKipuMapCharThink( CommandStruct.cno  );

					SetMapCharTartgetItem( CommandStruct.cno, CommandStruct.mgno, CommandStruct.taget );
					break;
				case BCMD_ACT_AUTO:
					switch( CommandStruct.mgno ){
						default:
						case 0:
							SetMapCharThink( CommandStruct.cno, MOVE_THINK_ATACK, ACTION_THINK_KNIGHT );
							break;
						case 1:
							SetMapCharThink( CommandStruct.cno, MOVE_THINK_ATACK, ACTION_THINK_ATACK );
							break;
						case 2:
							SetMapCharThink( CommandStruct.cno, MOVE_THINK_ATACK, ACTION_THINK_SHOT );
							break;
						case 3:
							SetMapCharThink( CommandStruct.cno, MOVE_THINK_SUPPORT, ACTION_THINK_HEAL );
							break;
						case 4:
							SetMapCharThink( CommandStruct.cno, MOVE_THINK_SUPPORT, ACTION_THINK_MAGIC );
							break;
					}
					break;
				case BCMD_ACT_STAND:
					ResetMapCharThink( CommandStruct.cno );
					SetMapCharStand( CommandStruct.cno );
					break;

				case BCMD_ACT_CANSEL:
					CanselMapCharCommand( CommandStruct.cno );
					break;
				}
			}
			ZeroMemory( &CommandStruct, sizeof(BTL_COMMAND_STRUCT) );
			

			ResetPouupCharWindow( 1 );
			ResetGameWindow( 15 );
			for(i=0;i<BTL_COMMAND_MAX;i++){
				GD_ResetGraph( GRP_SYSTEM+10+i );

			}
			MUS_ResetMouseRect_Layer( 2 );
			MUS_ResetMouseRect_Layer( 1 );
			MUS_SetMouseLayer( 0 );

			MapControl.sc_mode=0;
			count=0;
			step=BCMD_INIT;
			if( GetCharCommandWait()==-1 ){




				return 0;
			}
			break;
		case BCMD_SCROLL:
			SetPouupCharWindow( 0, -1 );
			SetPouupCharWindow( 1, CharControl[cno].cno, 1 );
			if(MapControl.sc_max==0){



				for(i=0;i<BTL_COMMAND_MAX;i++){
					GD_SetGraph( GRP_SYSTEM+10+i, BMP_SYSTEM, LAY_SYSTEM, OFF );	
					GD_SetGraphScaleParam( GRP_SYSTEM+10+i, SCALE_HIGHT );

					GD_SetGraphBlendParam( GRP_SYSTEM+10+i, BLEND_ALPHA );
					GD_SetGraphDstRect( GRP_SYSTEM+10+i, 42*i+BTL_COMMAND_X, BTL_COMMAND_Y, 42, 30 );
					GD_SetGraphSrcRect( GRP_SYSTEM+10+i, 43.0f*(i%5),  32*(i/5), 42, 30 );
					GD_SetGraphCenter( GRP_SYSTEM+10+i, 42*i+BTL_COMMAND_X+21, BTL_COMMAND_Y+15 );
				}

				step=BCMD_SELECT_IN;
			}
			break;
		case BCMD_SELECT:
			SetPouupCharWindow( 1, CharControl[cno].cno );
			for(i=0;i<BTL_COMMAND_MAX;i++){
				if(select_no==i){
					select_count[i] = LIM(select_count[i]+1,0,15);
				}else{
					select_count[i] = LIM(select_count[i]-1,0,15);
				}
				rate = POYO_CONUT(select_count[i],15,9);

				GD_SetGraphZoom( GRP_SYSTEM+10+i, 1.0f+rate );

				GD_SetGraphDisp( GRP_SYSTEM+10+i, ON );
				GD_SetGraphRoll( GRP_SYSTEM+10+i, 0 );
				GD_SetGraphDstMove( GRP_SYSTEM+10+i, 42*i+BTL_COMMAND_X, BTL_COMMAND_Y );
				GD_SetGraphColor( GRP_SYSTEM+10+i, 255, 128, 128, 128 );

				if(select_no==i){
					GD_SetGraphLayer( GRP_SYSTEM+10+i, LAY_SYSTEM+15 );
				}else{
					GD_SetGraphLayer( GRP_SYSTEM+10+i, LAY_SYSTEM+select_count[i] );
				}

			}
			if( bak_select != select_no ){
				SND_Select();

				switch(select_no){
				case -1:SetGameWindowText( 15, "ユニットの行動を選択してください。" );	break;

				case 0:	SetGameWindowText( 15, "移動場所を指定します。\n目標までの経路は自動で判別されます。" );	break;
				case 1:	SetGameWindowText( 15, "攻撃目標を選び、自動で移動攻撃します。\n目標を倒すと解除されます。" );	break;
				case 2:	SetGameWindowText( 15, "魔法を選び使用します。\n魔法詠唱中は、物理被ダメージが増えます。\n詠唱は後方で。" );	break;
				case 3:	SetGameWindowText( 15, "技を使用します。\n技発動後に行動不能時間があります。" );	break;
				case 4:	SetGameWindowText( 15, "道具を使用します。\n道具使用後に行動不能時間があります。" );	break;
				case 5:	SetGameWindowText( 15, "キャラを自動で動かします。他のコマンドを選択すると解除されます。" );	break;
				case 6:	SetGameWindowText( 15, "その場で待機し、行動しません。\n待機中、MPが徐々に回復します。" );	break;
				}
			}

			if( GameKey.click ){
				if( select_no != -1 ){
					if(select_no==2 && CharParam[CharControl[cno].cno].magic[0]==-1 ){
						SND_Ng();
					}else if(select_no==3 && CharParam[CharControl[cno].cno].skill[0]==-1 ){
						SND_Ng();
					}else{
						step=BCMD_SELECT_OUT;

						for(i=0;i<BTL_COMMAND_MAX;i++){
							select_count[i] = 15+i*2;
						}
						SND_Ok();
					}
				}
			}else if( GameKey.cansel ){
				CommandStruct.action = BCMD_ACT_CANSEL;
				CommandStruct.ketei = TRUE;
				CommandStruct.cno = cno;

				step=BCMD_SELECT_OUT;

				for(i=0;i<BTL_COMMAND_MAX;i++){
					select_count[i] = 15+i*2;
				}
				SND_Cansel();
			}
			bak_select = select_no;
			break;
		case BCMD_ATACK:
			switch( BTL_CoomandAtack(cno) ){
				case 1:	step=BCMD_TERM;	break;
				case 2:
					SetScrolBattleMapChar( cno, MapControl.ezoom, 15, 2 );
					step=BCMD_SELECT_IN2;
					break;
			}
			break;
		case BCMD_MAGIC:
			switch( BTL_CoomandMagic(cno,0) ){
				case 1:	step=BCMD_TERM;	break;
				case 2:
					SetScrolBattleMapChar( cno, MapControl.ezoom, 15, 2 );
					step=BCMD_SELECT_IN2;
					break;
			}
			break;
		case BCMD_TECHNIC:
			switch( BTL_CoomandMagic(cno,1) ){
				case 1:	step=BCMD_TERM;	break;
				case 2:
					SetScrolBattleMapChar( cno, MapControl.ezoom, 15, 2 );
					step=BCMD_SELECT_IN2;
					break;
			}
			break;
		case BCMD_ITEM:
			switch( BTL_CoomandItem(cno) ){
				case 1:	step=BCMD_TERM;	break;
				case 2:
					SetScrolBattleMapChar( cno, MapControl.ezoom, 15, 2 );
					step=BCMD_SELECT_IN2;
					break;
			}
			break;
		case BCMD_AUTO:
			switch( BTL_CoomandAuto(cno) ){
				case 1:	step=BCMD_TERM;	break;
				case 2:
					SetScrolBattleMapChar( cno, MapControl.ezoom, 15, 2 );
					step=BCMD_SELECT_IN2;
					break;
			}
			break;

		case BCMD_MOVE:
			*aria=1;
			MapControl.sc_mode=0;

			SetGameWindowText( 15, "色のついている部分には侵入できません。\nSPACEキーを押しながらクリックで中継ポイント設定。" );
			{
				float	tx,ty, tx2,ty2;
				float	rag;
				tx=CharControl[cno].px;
				ty=CharControl[cno].py;
				

				
				for(i=0;i<CharControl[cno].tcnt;i++){
					GD_SetGraphPrimitive( GRP_MAP_CONSOL+i, 3, LAY_MAP_MOVE2+1, TRUE );
					
					rag = atan2( (CharControl[cno].tx[i]-MapControl.px)*MapControl.zoom-(tx-MapControl.px)*MapControl.zoom, (CharControl[cno].ty[i]-MapControl.py)*MapControl.zoom-(ty-MapControl.py)*MapControl.zoom );
					tx2 = sin(rag+3.14/2)*4;
					ty2 = cos(rag+3.14/2)*4;
					
					GD_SetGraphPointPos( GRP_MAP_CONSOL+i, 0, (tx-MapControl.px)*MapControl.zoom, (ty-MapControl.py)*MapControl.zoom );
					GD_SetGraphPointPos( GRP_MAP_CONSOL+i, 1, (CharControl[cno].tx[i]-MapControl.px)*MapControl.zoom, (CharControl[cno].ty[i]-MapControl.py)*MapControl.zoom );
					GD_SetGraphPointPos( GRP_MAP_CONSOL+i, 2, (CharControl[cno].tx[i]-MapControl.px)*MapControl.zoom+tx2, (CharControl[cno].ty[i]-MapControl.py)*MapControl.zoom+ty2 );
					GD_SetGraphBlendParam( GRP_MAP_CONSOL+i, BLEND_ALPHA );
					GD_SetGraphColorNo( GRP_MAP_CONSOL+i, 0, 64, 255, 0,0 );
					GD_SetGraphColorNo( GRP_MAP_CONSOL+i, 1,192, 255, 0,0 );
					GD_SetGraphColorNo( GRP_MAP_CONSOL+i, 2,192, 255, 0,0 );
					tx=CharControl[cno].tx[i];
					ty=CharControl[cno].ty[i];
				}
				if( SetMapCharTartgetMoveCheck( cno, (int)(MapControl.px + GameKey.mx/MapControl.zoom), (int)(MapControl.py + GameKey.my/MapControl.zoom) ) ){
					rag = atan2( GameKey.mx-(tx-MapControl.px)*MapControl.zoom, GameKey.my-(ty-MapControl.py)*MapControl.zoom );
					tx2 = sin(rag+3.14/2)*4;
					ty2 = cos(rag+3.14/2)*4;

					GD_SetGraphPrimitive( GRP_MAP_CONSOL+i, 3, LAY_MAP_MOVE2+1, TRUE );
					GD_SetGraphPointPos( GRP_MAP_CONSOL+i, 0, (tx-MapControl.px)*MapControl.zoom, (ty-MapControl.py)*MapControl.zoom );
					GD_SetGraphPointPos( GRP_MAP_CONSOL+i, 1, GameKey.mx, GameKey.my );
					GD_SetGraphPointPos( GRP_MAP_CONSOL+i, 2, GameKey.mx - tx2, GameKey.my - ty2 );

					GD_SetGraphBlendParam( GRP_MAP_CONSOL+i, BLEND_ALPHA );
					GD_SetGraphColorNo( GRP_MAP_CONSOL+i, 0, 64, 255, 0,0 );
					GD_SetGraphColorNo( GRP_MAP_CONSOL+i, 1,192, 255, 0,0 );
					GD_SetGraphColorNo( GRP_MAP_CONSOL+i, 2,192, 255, 0,0 );

					if( GameKey.click ){
						SND_Ok();
						if( KeyCond.btn.space ){
							CharControl[cno].tx[CharControl[cno].tcnt]  = (float)MapControl.px + GameKey.mx/MapControl.zoom;
							CharControl[cno].ty[CharControl[cno].tcnt]  = (float)MapControl.py + GameKey.my/MapControl.zoom;
							CharControl[cno].tcnt = min(CharControl[cno].tcnt+1, 7);

						}else{
							CommandStruct.action = BCMD_ACT_MOVE;
							CommandStruct.ketei = TRUE;
							CommandStruct.tx = MapControl.px + GameKey.mx/MapControl.zoom;
							CommandStruct.ty = MapControl.py + GameKey.my/MapControl.zoom;
							CommandStruct.cno   = cno;

							GD_SetGraphColor( GRP_MAP_CHAR+cno, 255,128,128,128 );
							count=0;
							step=BCMD_TERM;
							for(i=0;i<8;i++) GD_ResetGraph( GRP_MAP_CONSOL+i );
						}
					}
				}else{
					GD_ResetGraph( GRP_MAP_CONSOL+i );
				}
				if( GameKey.cansel ){
					for(i=0;i<8;i++) GD_ResetGraph( GRP_MAP_CONSOL+i );
					step=BCMD_TERM;
					SND_Cansel();
				}
			}
			break;
		case BCMD_SELECT_OUT:
			for(i=0;i<BTL_COMMAND_MAX;i++){
				select_count[i] = max( select_count[i]-1, 0 );
				rate = POYO_CONUT( min(15,select_count[i]), 15, 10 );
				GD_SetGraphDstMove( GRP_SYSTEM+10+i, 42*i+BTL_COMMAND_X, BTL_COMMAND_Y+100-rate*100 );
				GD_SetGraphAlpha( GRP_SYSTEM+10+i, (int)(rate*255) );
			}
			
			if(count>0 || select_count[BTL_COMMAND_MAX-1]>0){
				count--;


				rate = POYO_CONUT( count, 15, 10 );


			}else{
				if( CommandStruct.ketei && CommandStruct.action==BCMD_ACT_CANSEL){
					step = BCMD_TERM;
				}else{
					switch( bak_select ){
					case 0:
						step = BCMD_MOVE;
						CharControl[cno].tcnt=0;
						break;
					case 1:
						count=0;
						step = BCMD_ATACK;
						break;
					case 2:
						count=0;
						step = BCMD_MAGIC;
						break;
					case 3:
						count=0;
						step = BCMD_TECHNIC;
						break;
					case 4:
						count=0;
						step = BCMD_ITEM;
						break;

					case 5:
						count=0;
						step = BCMD_AUTO;
						break;

					default:
					case 6:
						CommandStruct.action = BCMD_ACT_STAND;
						CommandStruct.ketei = TRUE;
						CommandStruct.cno = cno;
						step = BCMD_TERM;
						break;
					}
				}
			}
			break;
		case BCMD_SELECT_IN:
			SetGameWindowColor( 15, 255*count/20, 128, 128, 128 );
		case BCMD_SELECT_IN2:
			if(count<20){
				count++;

				for(i=0;i<BTL_COMMAND_MAX;i++){
					j = 15-LIM( 15-(count-i), 0, 15 );
					rate = POYO_CONUT( j, 15, 10 );

					GD_SetGraphDisp( GRP_SYSTEM+10+i, ON );
					GD_SetGraphZoom( GRP_SYSTEM+10+i, rate );
	
					GD_SetGraphDstMove( GRP_SYSTEM+10+i, 42*i+BTL_COMMAND_X, BTL_COMMAND_Y+100-rate*100 );

					GD_SetGraphAlpha( GRP_SYSTEM+10+i, (int)(rate*255) );
				}
			}else{
				SetGameWindowColor( 15, 255, 128, 128, 128 );

				step=BCMD_SELECT;
				count=0;
			}
			break;
	}
	return 1;
}


void BTL_SetStartPoint( float px, float py )
{
	StartPoint.px=px;
	StartPoint.py=py;
}
void BTL_WinParam( int btype, int param1, int param2, int all_atack, int range )
{
	int	i;
	StartPoint.btype = btype&0xff;	
	StartPoint.did_ok    = btype&WIN_DID_OK;

	if( ESC_GetFlag(3)==5 && all_atack==-1 ){
		StartPoint.all_atack = 480*60;
	}else if( ESC_GetFlag(3)==4 && all_atack==-1 ){
		StartPoint.all_atack = 480*60;
	}else{
		StartPoint.all_atack = all_atack*60;
	}
	StartPoint.all_atack_back=StartPoint.all_atack;
	StartPoint.bcount = 0;

	switch(btype){
		default:
		case WIN_NORMAL:		
			StartPoint.btype = WIN_NORMAL;	
			break;
		case WIN_BOSS:
			StartPoint.bcno  = param1+10;	
			break;
		case WIN_POINT:
		case WIN_POINT2:	
			StartPoint.bx    = param1;	
			StartPoint.by    = param2;

			if(range==-1){
				StartPoint.brange = 64;
			}else{
				StartPoint.brange = range;
			}
			break;
		case WIN_TIME:
			StartPoint.btime  = param1*60;	
			if(param2){
				for(i=0;i<CHAR_CONTROL_MAX;i++){
					if(CharControl[i].flag){
						if(CharParam[CharControl[i].cno].unit==1){
							CharParam[CharControl[i].cno].hujimi=TRUE;
						}
					}
				}
			}
			break;
		case WIN_GIGA:	
			break;
	}
}

void BTL_SetStartBattle( int btype, int param1, int param2, int all_atack, int range  )
{
	BTL_WinParam( btype, param1, param2, all_atack, range );

	int	i;
	for(i=0;i<CHAR_CONTROL_MAX;i++){
		CharControl[i].event_flag = 0;
		CharControl[i].event_sinkin = 0;
	}
	MapControl.battle_flag = 1;
	MapControl.next_step   = BTL_SORTILE;
}


static int	Zenmetu=0;
static int	BattleWin=0;
BOOL BTL_WaitBattleEnd( int *win )
{
	int	i;
	int	teki   = 0;
	int	mikata = 0;
	int	giga   = 0;
	int	point_flag;

	for(i=0;i<CHAR_CONTROL_MAX;i++){
		if(CharControl[i].flag){
			if(CharParam[CharControl[i].cno].did==0){
				if(CharParam[CharControl[i].cno].unit){
					if( !(JobParam[CharParam[CharControl[i].cno].job].ex&EX_HAKO) ){
						teki++;
					}
				}else{
					if(i<10){
						mikata++;
					}else{
						giga++;
					}
				}
			}
		}
	}

	if( MapControl.next_step==BTL_BATTLE ){
		StartPoint.bcount++;

		if(StartPoint.all_atack>0){
			int	all_atack=StartPoint.all_atack;
			if( ESC_GetFlag(3)==4 ){
				all_atack=StartPoint.all_atack/2;
			}else if( ESC_GetFlag(3)==5 ){
				all_atack=StartPoint.all_atack/4;
			}

			if( all_atack <= StartPoint.bcount ){
				StartPoint.all_atack = -1;
				AllAtackMapCharThink();
			}
		}
		switch(StartPoint.btype){
			default:
			case WIN_NORMAL:		
				if( !(teki && mikata) ){
					if( WaitBattleEffect() ){
						MapControl.next_step=BTL_BATTLE_END;
						MapControl.battle_flag = 0;
					}
				}
				break;
			case WIN_BOSS:	
				if(CharParam[CharControl[ StartPoint.bcno ].cno].did || !mikata ){
					if( WaitBattleEffect() ){
						MapControl.next_step=BTL_BATTLE_END;
						MapControl.battle_flag = 0;
					}
				}
				break;
			case WIN_POINT:	
			case WIN_POINT2:	
				point_flag=0;
				for(i=0;i<CHAR_CONTROL_MAX;i++){
					if(CharControl[i].flag){
						if(CharParam[CharControl[i].cno].did==0 && CharParam[CharControl[i].cno].unit==0){
							if( pow(CharControl[i].px-StartPoint.bx,2) + pow(CharControl[i].py-StartPoint.by,2)<StartPoint.brange*StartPoint.brange ){
								point_flag=1;
								break;
							}
						}
					}
				}
				if( point_flag || !(teki && mikata) ){
					if( WaitBattleEffect() ){
						MapControl.next_step=BTL_BATTLE_END;
						MapControl.battle_flag = 0;
					}
				}

				break;
			case WIN_TIME:	
				if( StartPoint.btime <= StartPoint.bcount || !(teki && mikata) ){
					if(teki==0){
						Zenmetu=1;
					}
					if( WaitBattleEffect() ){
						MapControl.next_step=BTL_BATTLE_END;
						MapControl.battle_flag = 0;
					}
				}
				break;
			case WIN_GIGA:
				if( !(teki && mikata && giga) ){
					if( WaitBattleEffect() ){
						MapControl.next_step=BTL_BATTLE_END;
						MapControl.battle_flag = 0;
					}
				}
				break;
		}
	}

	if(MapControl.next_step==BTL_EVENT){
		*win = BattleWin;
		return 1;
	}else{
		return 0;
	}
}



int	BTL_Sortile( int *aria  )
{
	enum{
		BSTL_INIT,
		BSTL_TERM,
		BSTL_MAIN,
		BSTL_SELECT,
		BSTL_TARGET,
		BSTL_MAP,
	};
	int			ret=1;
	static int	step=BSTL_INIT;
	static int	count=0;
	static int	boss=0;
	int			i,j,k, fix;
	float		rate;
	float		px,py;
	int			range;;
	char		fname[MAX_PATH];
	char		buf[256];

	int			select_no = MUS_GetMouseNo( -1 );
	static int	bak_select=-1;

	int		x,y;
	int	w = MapData.head.w/MapData.head.sgrid;
	int	h = MapData.head.h/MapData.head.sgrid;


	*aria=0;
	px = (StartPoint.px-MapControl.px)*MapControl.zoom;
	py = (StartPoint.py-MapControl.py)*MapControl.zoom;
	switch(step){
		case BSTL_INIT:

			AllPartyHeal();

			SetBrightBattleMap( 128, 128, 128, 15,0 );
			SetScrolBattleMap( StartPoint.px, StartPoint.py, 1.0f, 15, 2 );
			step=BSTL_MAIN;
			count=0;

			StartPoint.fix_count=0;	
			k=-1;
			j=0;

			ZeroMemory( StartPoint.sc, sizeof(START_CHAR)*32) ;
			for(i=0;i<32;i++){
				fix = CheckBackUpMapCharPlayer( i );
				if( fix!=-1 ){
					StartPoint.fix_count++;
				}
				if(CharParam[i].flag && !CharParam[i].did && CharParam[i].unit==0 && fix==-1){
					int	mahura=0;
					if( i == 0 && !ESC_GetFlag(2) ){
						mahura = 1;
					}

					wsprintf( fname, "san%03d%d%02d0.ani", JobParam[CharParam[i].job].jno, mahura, JobParam[CharParam[i].job].buki_no[ItemParam[CharParam[i].buki].type] );
					GD_LoadSprite( SPR_START_CHAR+i, fname, ALPHA_4444, FALSE, DrawSetting.mip_map, NULL,NULL, 256, "btl" );

					StartPoint.sc[i].flag = TRUE;
					StartPoint.sc[i].set  = FALSE;
					k=i;
					j++;
				}else{
					StartPoint.sc[i].flag = FALSE;
					StartPoint.sc[i].set  = FALSE;
				}
			}
			StartPoint.ucount = 0;	
			StartPoint.umax   = j;
			StartPoint.scount = 0;	
			{
				int	max_start = ESC_GetFlag(90);
				if(max_start==0) max_start=8;
				StartPoint.smax   = min(j,max_start-StartPoint.fix_count);	
			}

			for(i=0;i<10;i++){

				GD_SetSprite( GRP_START_CHAR+i, SPR_HIT_EFEECT,   0, TRUE, LAY_SYSTEM+2, FALSE, TRUE, 0, FALSE );
				MUS_SetMouseRect( 1, i, 112+64*i-32, 24, 64, 96, ON );
			}
			MUS_SetMouseRect( 1, 10,      32+8, 40, 40, 64, ON );
			MUS_SetMouseRect( 1, 11, 800-48-32, 40, 40, 64, ON );
			MUS_SetMouseRect( 1, 12, 800-72-48-32+24, 132+24, 96, 24, ON );



			for(i=0;i<10;i++){
				GD_SetSprite( GRP_START_CHAR+10+i, SPR_HIT_EFEECT,   0, TRUE, LAY_SYSTEM+1, FALSE, TRUE, 0, FALSE );

			}
			GD_SetSprite( GRP_START_CHAR+20, SPR_HIT_EFEECT,   0, TRUE, LAY_SYSTEM+3, FALSE, TRUE, 0, FALSE );


			GD_SetGraph( GRP_START_CHAR+21, BMP_SYSTEM, LAY_SYSTEM+1, FALSE );
			GD_SetGraph( GRP_START_CHAR+22, BMP_SYSTEM, LAY_SYSTEM+1, FALSE );
			GD_SetGraphRect( GRP_START_CHAR+21,        32+16, 40+8, 528, 432, 32, 48 );
			GD_SetGraphRect( GRP_START_CHAR+22, 800-32-32-16, 40+8, 528, 480, 32, 48 );
			
			GD_SetGraphCircle( GRP_START_CHAR+23, 64, TRUE, LAY_EFFECT+2, FALSE );
			GD_SetGraphBrightNo( GRP_START_CHAR+23, 1 );

			MUS_SetMouseLayer( 1 );
			SetGameWindow( 2, GRP_SYSTEM, BMP_SYSTEM, LAY_SYSTEM, FALSE, -1, 1 );
			SetGameWindowRect( 2, 8+64-24, 8, 800-16-128+48, 128  );

			SetGameWindow( 3, GRP_SYSTEM+1, BMP_SYSTEM, LAY_SYSTEM, FALSE, -1, 1 );
			SetGameWindowRect( 3, 800-72-48-32, 132, 96+48, 72  );
			SetGameWindowText( 3, "戦闘開始", BMP_START_CHAR+0 );

			SetGameWindow( 4, GRP_SYSTEM+4, BMP_SYSTEM, LAY_SYSTEM, FALSE, -1, 1 );
			SetGameWindowRect( 4, 8, 132, 144+48, 72  );
			wsprintf ( buf, "出撃%2d/%-2d人", StartPoint.scount+StartPoint.fix_count, StartPoint.smax+StartPoint.fix_count );
			SetGameWindowText( 4, buf, BMP_START_CHAR+1 );

			SetGameWindow( 5, GRP_SYSTEM+7, BMP_SYSTEM, LAY_SYSTEM+1, FALSE, -1, 2 );
			SetGameWindowRect( 5, 8+64-24, 8, 64, 128  );

			SetGameWindow( 15, GRP_SYSTEM+9, BMP_SYSTEM, LAY_SYSTEM+1, TRUE, -1, 1 );
			SetGameWindowRect( 15, 16, 600-72-16, 360, 72  );
			switch(StartPoint.btype){
				default:
				case WIN_NORMAL:	
					wsprintf ( buf, "クリア条件：<B255:128:128:敵を殲滅せよ！>" );
					break;
				case WIN_BOSS:		
					wsprintf ( buf, "クリア条件：<B255:128:128:目標を撃破せよ！>" );
					break;
				case WIN_POINT:		
					wsprintf ( buf, "クリア条件：<B255:128:128:目標に到達せよ！>" );
					break;
				case WIN_POINT2:	
				case WIN_TIME:		
					wsprintf ( buf, "クリア条件：<B255:128:128:敵を殲滅せよ！>" );
					break;
				case WIN_GIGA:
					wsprintf ( buf, "クリア条件：<B255:128:128:ギガースを守れ！>" );
					break;
			}
			SetGameWindowText( 15, buf, BMP_START_CHAR+2 );
			
			SetAriaTable( 192 );
			break;
		case BSTL_TERM:
			CharMarkerBoss = -1;
			for(i=0;i<24;i++){
				GD_ResetGraph( GRP_START_CHAR+i );
			}
			for(i=0;i<32;i++){
				GD_ReleaseSprite( SPR_START_CHAR+i );
			}
			for(i=0;i<3;i++){
				GD_ReleaseTexture( BMP_START_CHAR+i );
			}
			for(i=0;i<3;i++){
				ResetGameWindow( 2+i );
			}
			ResetGameWindow( 15 );

			ResetPouupCharWindow( 1 );

			MUS_ResetMouseRect_Layer( 1 );
			MUS_SetMouseLayer( 0 );

			MapControl.sc_mode=0;
			step=BSTL_INIT;

			i=StartPoint.fix_count;
			for(j=0;j<32;j++){
				if( StartPoint.sc[j].flag && StartPoint.sc[j].set ){
					SetMapChar( i, j, StartPoint.sc[j].x, StartPoint.sc[j].y, 0 );
					i++;
				}
			}
			ret=0;
			break;
		case BSTL_SELECT:

			MapControl.sc_mode=1;
			*aria=1;


			SetPouupCharWindow( 1, -1 );

			SetGameWindowDisp( 5, FALSE );

			GD_SetGraphDisp( GRP_START_CHAR+20, TRUE );
			GD_SetSpritePlay( GRP_START_CHAR+20, SPR_START_CHAR+StartPoint.select, 0, TRUE  );

			GD_SetGraphDstMove( GRP_START_CHAR+20, GameKey.mx, GameKey.my );
			if(GameKey.click){
				x = (GameKey.mx+MapControl.px)*MapControl.zoom/MapData.head.sgrid;
				y = (GameKey.my+MapControl.py)*MapControl.zoom/MapData.head.sgrid;
				if(MapControl.aria_table[y*w+x]){
					for(j=0;j<32;j++){
						if( StartPoint.sc[j].flag && StartPoint.sc[j].set ){
							if( StartPoint.sc[j].x==x*MapData.head.sgrid+MapData.head.sgrid/2 && StartPoint.sc[j].y==y*MapData.head.sgrid+MapData.head.sgrid/2 ){
								break;
							}
						}
					}

					StartPoint.sc[StartPoint.select].set = TRUE;
					StartPoint.sc[StartPoint.select].x = x*MapData.head.sgrid+MapData.head.sgrid/2;
					StartPoint.sc[StartPoint.select].y = y*MapData.head.sgrid+MapData.head.sgrid/2;
					if(j==32){
						StartPoint.scount++;
						StartPoint.umax--;

						GD_SetGraphDisp( GRP_START_CHAR+20, FALSE );

						wsprintf ( buf, "出撃%2d/%-2d人", StartPoint.scount+StartPoint.fix_count, StartPoint.smax+StartPoint.fix_count );
						SetGameWindowText( 4, buf );

						step=BSTL_MAIN;
						SND_Ok();
					}else{
						StartPoint.sc[j].set = FALSE;
						StartPoint.select = j;
						SND_Cansel();
					}
				}else{
					SND_Ng();
				}
			}else if(GameKey.cansel){
				GD_SetGraphDisp( GRP_START_CHAR+20, FALSE );
				step=BSTL_MAIN;
				SND_Cansel();
			}

			for(i=0;i<10;i++){
				GD_SetGraphDisp( GRP_START_CHAR+10+i, FALSE );
			}
			i=0;
			for(j=0;j<32;j++){
				if( StartPoint.sc[j].flag && StartPoint.sc[j].set ){
					GD_SetGraphDisp( GRP_START_CHAR+10+i, TRUE );
					GD_SetGraphDstMove( GRP_START_CHAR+10+i,
						(StartPoint.sc[j].x-MapControl.px)*MapControl.zoom,
						(StartPoint.sc[j].y-MapControl.py)*MapControl.zoom );
					GD_SetSpritePlay( GRP_START_CHAR+10+i, SPR_START_CHAR+j, 0, TRUE  );

					GD_SetGraphLayer( GRP_START_CHAR+10+i, LAY_MAP_OBJECT+StartPoint.sc[j].y );
					i++;
					if(i>=10){
						break;
					}
				}
			}

			break;
		case BSTL_MAP:
			*aria=1;
			i=0;
			for(j=0;j<32;j++){
				if( StartPoint.sc[j].flag && StartPoint.sc[j].set ){
					GD_SetGraphDisp( GRP_START_CHAR+10+i, TRUE );
					GD_SetGraphDstMove( GRP_START_CHAR+10+i, (StartPoint.sc[j].x-MapControl.px)*MapControl.zoom, (StartPoint.sc[j].y-MapControl.py)*MapControl.zoom );
					GD_SetGraphZoom( GRP_START_CHAR+10+i, MapControl.zoom );
					i++;
					if(i>=10){
						break;
					}
				}
			}
			switch(StartPoint.btype){
				default:
				case WIN_NORMAL:	
				case WIN_TIME:		
				case WIN_POINT2:	
					ret=2;
					MapControl.sc_mode=0;
					break;
				case WIN_BOSS:		
					if(count==0){
						SetScrolBattleMapChar( StartPoint.bcno, 1.0f, 15, 2 );
					}

					if(boss){
						ret=2;
						MapControl.sc_mode=0;
					}else{
						SetPouupCharWindow( 1, CharControl[StartPoint.bcno].cno );
					}
					CharMarkerBoss = StartPoint.bcno;

					if(GameKey.click){
						boss=1;
					}
					break;
				case WIN_POINT:		
					ret=2;
					MapControl.sc_mode=0;

					if(count==0){
						SetScrolBattleMap( StartPoint.bx, StartPoint.by, 1.0f, 15, 2 );
					}

					GD_SetGraphColorNo( GRP_START_CHAR+23, 0,   0, 128,   0, 0 );
					GD_SetGraphColorNo( GRP_START_CHAR+23, 1, 128+STD_LimitLoop(GlobalCount*2,64)*2, 255, 128, 128 );
					GD_SetGraphDisp( GRP_START_CHAR+23, TRUE );

					px = (StartPoint.bx-MapControl.px)*MapControl.zoom;
					py = (StartPoint.by-MapControl.py)*MapControl.zoom;
					range = StartPoint.brange;
					GD_SetGraphDstRect( GRP_START_CHAR+23, (float)(px - range), (float)(py - range), (float)(range*2), (float)(range*2) );
					GD_SetGraphZoom( GRP_START_CHAR+23, MapControl.zoom );
					GD_SetGraphCenter( GRP_START_CHAR+23, px, py );
					break;
			}
			count++;
			if(GameKey.cansel){
				MapControl.sc_mode=1;
				SetScrolBattleMap( StartPoint.px, StartPoint.py, 1.0f, 15, 2 );

				GD_SetGraphDisp( GRP_START_CHAR+23, FALSE );
				CharMarkerBoss = -1;

				ResetPouupCharWindow( 0 );
				ResetPouupCharWindow( 1 );
				SetGameWindowDisp( 2, TRUE );
				step=BSTL_MAIN;
			}
			break;
		case BSTL_MAIN:

			MapControl.sc_mode=1;
			*aria=1;

			StartPoint.ucount = LIM( StartPoint.ucount, 0, max(0,StartPoint.umax-10) );

			for(i=0;i<10;i++){
				StartPoint.unit_list[i] = -1;
			}
			i=0;
			for(j=0;j<32;j++){
				if( StartPoint.sc[j].flag && StartPoint.sc[j].set==0 ){
					if( StartPoint.ucount <= i && i < StartPoint.ucount+10 && i<StartPoint.umax ){
						StartPoint.unit_list[i-StartPoint.ucount] = j;
					}
					i++;
				}
			}
			SetGameWindowDisp( 2, TRUE );
			SetGameWindowDisp( 3, TRUE );
			SetGameWindowDisp( 4, TRUE );
			for(i=0;i<10;i++){
				if( StartPoint.unit_list[i]!=-1 ){
					GD_SetGraphDisp( GRP_START_CHAR+i, TRUE );

					GD_SetSpritePlay( GRP_START_CHAR+i, SPR_START_CHAR+StartPoint.unit_list[i], 0, TRUE  );

					GD_SetGraphDstMove( GRP_START_CHAR+i, 112+64*i, 100 );
				}else{
					GD_SetGraphDisp( GRP_START_CHAR+i, FALSE );
				}
			}
			i=0;
			for(j=0;j<32;j++){
				if( StartPoint.sc[j].flag && StartPoint.sc[j].set ){
					GD_SetGraphDisp( GRP_START_CHAR+10+i, TRUE );
					GD_SetGraphDstMove( GRP_START_CHAR+10+i,
						(StartPoint.sc[j].x-MapControl.px)*MapControl.zoom,
						(StartPoint.sc[j].y-MapControl.py)*MapControl.zoom );
					GD_SetGraphZoom( GRP_START_CHAR+10+i, MapControl.zoom );
					GD_SetSpritePlay( GRP_START_CHAR+10+i, SPR_START_CHAR+j, 0, TRUE  );
					GD_SetGraphLayer( GRP_START_CHAR+10+i, LAY_MAP_OBJECT+StartPoint.sc[j].y );
					i++;
					if(i>=10){
						break;
					}
				}
			}

			if(StartPoint.umax>10){
				GD_SetGraphDisp( GRP_START_CHAR+21, TRUE );
				GD_SetGraphDisp( GRP_START_CHAR+22, TRUE );
				GD_SetGraphSrcRect( GRP_START_CHAR+21, 528, 432, 32, 48 );
				GD_SetGraphSrcRect( GRP_START_CHAR+22, 528, 480, 32, 48 );
			}else{
				GD_SetGraphDisp( GRP_START_CHAR+21, FALSE );
				GD_SetGraphDisp( GRP_START_CHAR+22, FALSE );
			}

			SetGameWindowBackType( 3, 1 );
			SetGameWindowDisp( 5, FALSE );
			if(select_no==10 ){
				if(StartPoint.umax>10){
					if(bak_select!=select_no){
						SND_Select();
					}
				}

				GD_SetGraphSrcRect( GRP_START_CHAR+21, 528+32, 432, 32, 48 );
				SetPouupCharWindow( 1, -1 );
			}else if(select_no==11 ){
				if(StartPoint.umax>10){
					if(bak_select!=select_no){
						SND_Select();
					}
				}

				GD_SetGraphSrcRect( GRP_START_CHAR+22, 528+32, 480, 32, 48 );
				SetPouupCharWindow( 1, -1 );
			}else if(select_no==12 ){
				if(StartPoint.scount || StartPoint.fix_count ){
					if(bak_select!=select_no){
						SND_Select();
					}
					SetGameWindowBackType( 3, 2 );
					SetPouupCharWindow( 1, -1 );
				}
			}else if(select_no!=-1 && select_no<10 ){
				if(StartPoint.unit_list[select_no]!=-1 ){
					if(bak_select!=select_no){
						SND_Select();
					}
					SetGameWindowDisp( 5, TRUE );
					SetGameWindowRect( 5, 4+24+64-24+64*select_no, 16, 64+24, 112  );
					SetPouupCharWindow( 1, StartPoint.unit_list[select_no] );
				}else{
					SetPouupCharWindow( 1, -1 );
				}
			}else{
				SetPouupCharWindow( 1, -1 );
			}
			count = LIM( count+1, 0, 15 );


			if(StartPoint.umax>=10){
				if(GameKey.pdown){
					SND_Select();
					StartPoint.ucount = LIM( StartPoint.ucount+1, 0, StartPoint.umax-10 );
				}else if(GameKey.pup){
					SND_Select();
					StartPoint.ucount = LIM( StartPoint.ucount-1, 0, StartPoint.umax-10 );
				}
			}

			if(GameKey.click){
				switch(select_no){
				case -1:
					for(j=0;j<32;j++){
						if( StartPoint.sc[j].flag && StartPoint.sc[j].set ){
							x = (StartPoint.sc[j].x-MapControl.px)*MapControl.zoom;
							y = (StartPoint.sc[j].y-MapControl.py)*MapControl.zoom;
							if( x-16<=GameKey.mx && GameKey.mx<x+16 && y-16<=GameKey.my && GameKey.my<y+16 ){
								break;
							}
						}
					}
					if(j!=32){
						StartPoint.select = j;
						StartPoint.sc[j].set=FALSE;

						step=BSTL_SELECT;

						StartPoint.scount--;
						StartPoint.umax++;
						wsprintf ( buf, "出撃%2d/%-2d人", StartPoint.scount+StartPoint.fix_count, StartPoint.smax+StartPoint.fix_count );
						SetGameWindowText( 4, buf );
						SND_Ok();
					}
					break;
				default:
					if(StartPoint.unit_list[select_no]!=-1 && StartPoint.scount<StartPoint.smax){
						StartPoint.select = StartPoint.unit_list[select_no];
						GD_SetGraphDisp( GRP_START_CHAR+select_no, FALSE );
						step=BSTL_SELECT;
						SND_Ok();
					}else{
						SND_Ng();
					}
					break;
				case 10:
					if(StartPoint.umax>=10){
						SND_Ok();
					}
					StartPoint.ucount = LIM( StartPoint.ucount-1, 0, StartPoint.umax-10 );
					break;
				case 11:
					if(StartPoint.umax>=10){
						SND_Ok();
					}
					StartPoint.ucount = LIM( StartPoint.ucount+1, 0, StartPoint.umax-10 );
					break;
				case 12:
					if(StartPoint.scount || StartPoint.fix_count ){
						SND_Ok();
						step=BSTL_TERM;
					}else{
						SND_Ng();
					}
					break;
				}
			}else if(GameKey.cansel){
				for(i=0;i<10;i++){
					if( StartPoint.unit_list[i]!=-1 ){
						GD_SetGraphDisp( GRP_START_CHAR+i, FALSE );
					}
				}
				SetGameWindowDisp( 2, FALSE );
				SetGameWindowDisp( 3, FALSE );
				SetGameWindowDisp( 4, FALSE );
				SetGameWindowDisp( 5, FALSE );
				GD_SetGraphDisp( GRP_START_CHAR+21, FALSE );
				GD_SetGraphDisp( GRP_START_CHAR+22, FALSE );

				boss=0;
				count=0;
				step=BSTL_MAP;
				SND_Cansel();
			}
			break;
	}
	bak_select = select_no;

	return ret;
}


int	BEfontXW[2][10][2]={
	{
		{  0, 76},
		{ 78, 52},
		{140, 62},
		{210, 72},
		{282, 66},

		{358, 78},
		{436, 76},
		{  0,  0},
		{  0,  0},
		{  0,  0}
	},{
		{  0, 68},
		{ 70, 76},
		{161, 88},
		{262, 72},
		{372, 68},

		{454, 80},
		{534, 72},
		{620, 80},
		{  0,  0},
		{  0,  0}
	}
};
int	BEfontH[2] = {86,96};
int	BEfontX[2] = {144,50};


void BTL_SetMoneyWindow( int wno, int gno, int x, int y, int alpha, int money )
{
	SetGameWindow( wno, gno, BMP_SYSTEM, LAY_SYSTEM, TRUE,-1, TRUE );
	SetGameWindowRect( wno, x, y, 24*8+48,  16*1+48 );
	SetGameWindowAlpha( wno, alpha );

	GD_SetGraph( gno+1, BMP_SYSTEM, LAY_SYSTEM+1, TRUE );
	GD_SetGraphRect( gno+1, x+24*7, y+24, 656, 496+16, 48, 16 );
	GD_SetGraphColor( gno+1, alpha, 255, 255, 255);
	GD_SetGraphBrightNo( gno+1, 1 );

	GD_SetGraphDigit( gno+2, BMP_HIT_DAMEGE+2, LAY_SYSTEM+1, TRUE, money, 8 );
	GD_SetGraphRect( gno+2, x+85, y+24, 0, 16, 10, 16 );
	GD_SetGraphColor( gno+2, alpha, 255,255,255 );
	GD_SetGraphBrightNo( gno+2, 1 );

	GD_SetGraph( gno+3, BMP_SYSTEM, LAY_SYSTEM+1, TRUE );
	GD_SetGraphRect( gno+3, x+24, y+24, 656, 496, 48, 16 );
	GD_SetGraphColor( gno+3, alpha, 255, 255, 255 );
	GD_SetGraphBrightNo( gno+3, 1 );
}

int	BTL_BattleEnd( void )
{
	enum{
		BSTL_INIT,
		BSTL_TERM,

		BSTL_IN,
		BSTL_OUT,

		BSTL_MAIN,
		BSTL_SELECT,
	};
	int			ret=0;
	static int	step=BSTL_INIT;
	static int	count=0;
	static int	end_font_no;
	static int	BackUpMus=-1;
	static int	money=0;
	static int	title=0;

	int			i,j;
	float		rate;
	float		px,py;
	char		buf[256];
	char		buf2[256];

	int			select_no = MUS_GetMouseNo( -1 );
	static int	bak_select=-1;

	int		x,y;
	int	w = MapData.head.w/MapData.head.sgrid;
	int	h = MapData.head.h/MapData.head.sgrid;


	px = (StartPoint.px-MapControl.px)*MapControl.zoom;
	py = (StartPoint.py-MapControl.py)*MapControl.zoom;
	switch(step){
		case BSTL_INIT:

			BackUpMus = AVG_GetBGMEno();
			AVG_StopBGM( 30 );
			ResetPouupCharWindow( 0 );
			ResetPouupCharWindow( 1 );
			MapControl.sc_mode=1;

			BattleWin=0;
			for(i=0;i<CHAR_CONTROL_MAX;i++){
				if(CharControl[i].flag){
					if(CharParam[CharControl[i].cno].unit==0 && CharParam[CharControl[i].cno].did==0){
						BattleWin=1;
						break;
					}
				}
			}
			if(StartPoint.btype==WIN_GIGA){
				BattleWin=1;
				for(i=0;i<CHAR_CONTROL_MAX;i++){
					if(CharControl[i].flag){
						if(CharParam[CharControl[i].cno].unit==1 && CharParam[CharControl[i].cno].did==0){
							BattleWin=0;
							break;
						}
					}
				}
			}

			if(BattleWin){
				SetBrightBattleMap( 64, 64, 64, 30,0 );
				if(StartPoint.btype==WIN_TIME || StartPoint.btype==WIN_POINT2){
					if(Zenmetu){
						end_font_no=0;
						AVG_PlaySE3( 9200, 255 );
						GD_LoadTexture( BMP_START_CHAR, "BattleEnd00.tga", ALPHA_4444|DITHER_OFF );
					}else{
						end_font_no=2;
					}
					Zenmetu=0;
				}else{
					end_font_no=0;
					AVG_PlaySE3( 9200, 255 );
					GD_LoadTexture( BMP_START_CHAR, "BattleEnd00.tga", ALPHA_4444|DITHER_OFF );
				}
			}else{
				SetBrightBattleMap( 0, 0, 0, 30,0 );
				if(StartPoint.did_ok){
					BattleWin=2;
					end_font_no=0;
				}else{
					SetGameWindow( 5, GRP_SYSTEM+12, BMP_SYSTEM, LAY_SYSTEM+4, FALSE,-1, 1 );
					SetGameWindowRect( 5, 400-(24*7+48)/2, 300-72, 24*7+48, 72  );
					SetGameWindowText( 5, "　　再挑戦　　", BMP_START_CHAR+2, 0  );

					SetGameWindow( 6, GRP_SYSTEM+16, BMP_SYSTEM, LAY_SYSTEM+4, FALSE,-1, 1 );
					SetGameWindowRect( 6, 400-(24*7+48)/2, 300, 24*7+48, 72  );
					SetGameWindowText( 6, "タイトルへ戻る",  BMP_START_CHAR+3, 0  );

					MUS_SetMouseRect( 1, 0, 400-(24*7+48)/2, 300-72, 24*7+48, 72, ON );
					MUS_SetMouseRect( 1, 1, 400-(24*7+48)/2, 300,    24*7+48, 72, ON );

					end_font_no=1;
					AVG_PlaySE3( 9201, 255 );
					GD_LoadTexture( BMP_START_CHAR, "BattleEnd01.tga", ALPHA_4444|DITHER_OFF );
				}
			}


			for(i=0;i<10;i++){
				if(BEfontXW[end_font_no][i][1]){
					GD_SetGraph( GRP_START_CHAR+i, BMP_START_CHAR, LAY_SYSTEM, TRUE );
					GD_SetGraphSrcRect( GRP_START_CHAR+i, BEfontXW[end_font_no][i][0], 0, BEfontXW[end_font_no][i][1], BEfontH[end_font_no] );
					GD_SetGraphDstRect( GRP_START_CHAR+i, BEfontXW[end_font_no][i][0]+BEfontX[end_font_no], 256, BEfontXW[end_font_no][i][1], BEfontH[end_font_no] );

					GD_SetGraphScaleParam( GRP_START_CHAR+i, SCALE_HIGHT );

					GD_SetGraphBrightNo( GRP_START_CHAR+i, 1 );

					GD_SetGraphAlpha( GRP_START_CHAR+i, 0 );
				}
			}

			if(BattleWin==2){
				step=BSTL_OUT;
			}else{
				step=BSTL_IN;
			}
			count=0;

			MUS_SetMouseLayer( 1 );

			if(BattleWin==1){
				SetGameWindow( 2, GRP_SYSTEM, BMP_SYSTEM, LAY_SYSTEM, ON, -1, TRUE );
				SetGameWindowRect( 2, 12, 12, 24*10+24, 24*22+48  );

				ITM_GetBttleItemInit();
				money=0;
				for(i=0;i<CHAR_CONTROL_MAX;i++){
					if(CharControl[i].flag){
						if(CharParam[CharControl[i].cno].unit==1 && CharParam[CharControl[i].cno].did){
							j = GetBackUpMapCharRank(i);
							if( j ){
								ITM_ItemDropRank( j );
							}else{
								j = GetBackUpMapCharItem(i);
								if( j ){
									ITM_ItemDropItem( j );
								}else{
									int	kari=0,kk;
									for(kk=0;kk<CHAR_CONTROL_MAX;kk++){
										if(!CharControl[kk].flag) continue;
										if(CharParam[CharControl[kk].cno].unit) continue;
										if(CharParam[CharControl[kk].cno].did) continue;
										if(JobParam[CharParam[CharControl[kk].cno].job].ex&EX_KARI){
											kari++;
										}
									}
									ITM_ItemDrop( CharParam[CharControl[i].cno].job, CharParam[CharControl[i].cno].lv, kari);
								}
							}
							int	mmm=5;
							switch( ESC_GetFlag(3) ){
								case 1:
								case 2:
									mmm = pow( (float)CharParam[CharControl[i].cno].lv+10,(float)2.6)/50*2;
									break;
								default:
									mmm = pow( (float)CharParam[CharControl[i].cno].lv+10,(float)2.6)/50;
									break;
							}
							money += LIM(mmm,1,10000);
						}
					}
				}
				buf2[0] = 0;
				if(GetBttleItemCount>22) DebugBox(NULL,"アイテム多すぎ");
				for(i=0;i<GetBttleItemCount;i++){
					if( GetBttleItem[i] ){
						ITM_GetItem( GetBttleItem[i], 1 );
						wsprintf ( buf, "%s\n", ItemParam[ GetBttleItem[i] ].name );
						_mbscat( (BYTE*) buf2, (BYTE*) buf );
					}
				}
				SetGameWindowText( 2, buf2, BMP_START_CHAR+1 );
				SetGameWindowColor( 2, 0, 128, 128, 128 );


				BTL_SetMoneyWindow( 3, GRP_SYSTEM+4, 12+264, 12,    0, money );
				BTL_SetMoneyWindow( 4, GRP_SYSTEM+8, 12+264, 12+72, 0, PlayerParam.money );
			}

			break;
		case BSTL_TERM:


			AllPartyHeal();
			ResetMapCharAll();
			ResetMagicActionAll();
			ResetMagicActionAll2();
			ResetBardActionAll();
			
			ResetGameWindow( 2 ); 
			ResetGameWindow( 3 ); 
			ResetGameWindow( 4 ); 
			ResetGameWindow( 5 ); 
			ResetGameWindow( 6 ); 
			for(i=0;i<10;i++){
				GD_ReleaseTexture( BMP_START_CHAR+i );
			}
			for(i=0;i<20;i++){
				GD_ResetGraph( GRP_START_CHAR+i );
			}

			MUS_ResetMouseRect_Layer( 1 );
			MUS_SetMouseLayer( 0 );

			step=BSTL_INIT;
			if(BattleWin){
				ResetBackUpMapChar();
				ret=1;
			}else{
				if(title){
					BTL_SetNextGameStep( 5, NULL );
					ret=3;
				}else{
					for(i=0;i<CHAR_CONTROL_MAX;i++){
						CharControl[i].event_flag = 0;
						CharControl[i].event_sinkin = 0;
					}
					MapControl.battle_flag = 1;

					StartPoint.all_atack=StartPoint.all_atack_back;
					StartPoint.bcount = 0;

					PlayerParam.retry++;
					ReloadBackUpMapChar();
					AVG_PlayBGM( BackUpMus, -2, TRUE, 255, 0 );
					ret=2;
				}
			}

			break;
		case BSTL_IN:

			if(count<64){
				if(end_font_no==2 ){
					count=128;
				}

				for(i=0;i<10;i++){
					if( 0<=(count-i*6) && (count-i*6)<8 ){
						GD_SetGraphAlpha( GRP_START_CHAR+i, 255-(count-i*6)*32 );

						GD_SetGraphDstMove( GRP_START_CHAR+i, (800-BEfontXW[end_font_no][i][1])/2, 256 );

						GD_SetGraphZoom( GRP_START_CHAR+i, (16-(count-i*6))/8.0f );
						GD_SetGraphCenter( GRP_START_CHAR+i, 400, 300 );

					}else{
						GD_SetGraphAlpha( GRP_START_CHAR+i, 0 );
					}
				}
			}else{
				if( AVG_GetHitKey() ){
					count=256;
				}
				for(i=0;i<10;i++){

					GD_SetGraphDstMove( GRP_START_CHAR+i, BEfontXW[end_font_no][i][0]+BEfontX[end_font_no] + (400-(BEfontXW[end_font_no][i][0]+BEfontX[end_font_no]))*(256-count)/64.0f/8, 256 );
					GD_SetGraphZoom( GRP_START_CHAR+i, (count-64+256)/256.0f );
					if(count<128){
						GD_SetGraphAlpha( GRP_START_CHAR+i, 256*(count-64)/32 );
					}else{
						GD_SetGraphAlpha( GRP_START_CHAR+i, 256-2*(count-128) );
					}
				}
			}
			if(count>=128 ){
				SetGameWindowColor( 2, 2*(count-128), 128, 128, 128 );

				if(BattleWin==1){
					BTL_SetMoneyWindow( 3, GRP_SYSTEM+4, 12+264, 12,    2*(count-128), money );
					BTL_SetMoneyWindow( 4, GRP_SYSTEM+8, 12+264, 12+72, 2*(count-128), PlayerParam.money );
				}
			}

			
			count++;
			if(count>256){
				count=0;
				step=BSTL_MAIN;
			}
			break;
		case BSTL_OUT:
			count++;
			SetGameWindowColor( 2, 256-256*count/30, 128, 128, 128 );
			if(BattleWin==1){
				BTL_SetMoneyWindow( 3, GRP_SYSTEM+4, 12+264, 12,    256-256*count/30, money );
				BTL_SetMoneyWindow( 4, GRP_SYSTEM+8, 12+264, 12+72, 256-256*count/30, PlayerParam.money );
			}

			if(count>30){
				step=BSTL_TERM;
			}
			break;

		case BSTL_MAIN:
			if(BattleWin==1){
				if(money>0){
					int	keta;
					if(count<20){
						keta=1;
					}else if(count<40){
						keta=6;
					}else if(count<60){
						keta=11;
					}else if(count<80){
						keta=66;
					}else if(count<100){
						keta=111;
					}else{
						keta=666;
					}
					money-=keta;
					PlayerParam.money += keta;
					if(money<0){
						PlayerParam.money += money;
						money=0;
					}
				}
				BTL_SetMoneyWindow( 3, GRP_SYSTEM+4, 12+264, 12,    255, money );
				BTL_SetMoneyWindow( 4, GRP_SYSTEM+8, 12+264, 12+72, 255, PlayerParam.money );
			}
			count++;

			if(BattleWin==0){
				SetGameWindowDisp( 5, TRUE );
				SetGameWindowDisp( 6, TRUE );
				SetGameWindowBackType( 5, 1 );
				SetGameWindowBackType( 6, 1 );
				if(select_no!=-1 && count>30 ){
					if(bak_select != select_no){
						SND_Select();
					}

					SetGameWindowBackType( 5+select_no, 2 );

					if(GameKey.click){
						title=select_no;
						count=0;
						step=BSTL_TERM;

						SND_Ok();
					}
				}
			}else{
				if(GameKey.click || GameKey.cansel){
					if(BattleWin==1){
						PlayerParam.money += money;
						money=0;
						BTL_SetMoneyWindow( 3, GRP_SYSTEM+4, 12+264, 12,    255, money );
						BTL_SetMoneyWindow( 4, GRP_SYSTEM+8, 12+264, 12+72, 255, PlayerParam.money );
					}

					count=0;
					SetBrightBattleMap( 0, 0, 0, 30, 0 );
					step=BSTL_OUT;
				}
			}
			break;
	}
	bak_select = select_no;

	return ret;
}


int		NextGameStep = SYS_WORLD;

int BTL_SetNextGameStep( int step, char *next_scr )
{
	switch(step){
		default:
		case 0:
			NextGameStep = SYS_WORLD;
			break;
		case 1:
			NextGameStep = SYS_CASTLE;
			if( ESC_GetFlag(71) ){
				ESC_SetFlag(71,0);

				NextGameStep = SYS_BATTLE;
				_mbscpy( (BYTE*)GoScript, (BYTE*)"sys_kyoten.sdt" );
			}
			break;
		case 2:
			NextGameStep = SYS_ITEM1;
			break;
		case 3:
			NextGameStep = SYS_ITEM2;
			break;
		case 4:
			NextGameStep = SYS_SAVE_POINT;
			wsprintf( NextScript, "%s.sdt", next_scr );
			break;
		case 5:
			NextGameStep = SYS_TITLE;
			break;
		case 6:
			NextGameStep = SYS_SAVE_POINT2;
			wsprintf( NextScript, "%s.sdt", next_scr );
			break;
		case 7:
			NextGameStep = SYS_END_SAVE;
			break;
	}
	return 1;
}


int	BTL_System( int script )
{
	int			ret=-1;
	int	i,j;
	int	aria=0;

	int	mx,my;

	if(script==0){
		MapControl.next_step = BTL_TERM;
	}
	switch(MapControl.step){
		default:
		case BTL_INIT:
AVG_SetTimeMode( 0 );
AVG_SetChromaMode( 256, 0 );
AVG_SetEffctMode( "", 0 );

NextGameStep = SYS_WORLD;
			MUS_SetMouseLayer( 0 );

			GD_LoadTexture( BMP_SYSTEM, "btsys_0000.tga", ALPHA_4444|DITHER_OFF );
			GD_LoadTexture( BMP_ARROW, "arrow.tga", ALPHA_4444 );

			GD_LoadTexture( BMP_EFFECT, "魔方陣2.tga", ALPHA_4444 );
			GD_LoadTexture( BMP_EFFECT+1, "BARD.tga", ALPHA_4444 );

			GD_LoadTexture( BMP_HIT_DAMEGE, "digit000.tga",  ALPHA_4444 );
			GD_LoadTexture( BMP_HIT_DAMEGE+1, "digit001.tga",  ALPHA_4444 );
			GD_LoadTexture( BMP_HIT_DAMEGE+2, "digit002.tga", ALPHA_4444 );

			GD_LoadSprite( SPR_HIT_EFEECT, "btef0000.ani", ALPHA_4444 );

			GD_LoadSprite( SPR_TEXT_WAIT+0, "page_00.ani", ALPHA_4444 );
			GD_LoadSprite( SPR_TEXT_WAIT+1, "page_01.ani", ALPHA_4444 );

			MapControl.epx = MapControl.spx = MapControl.px=0;
			MapControl.epy = MapControl.spy = MapControl.py=0;
			MapControl.szoom = MapControl.ezoom = MapControl.zoom = 1.0f;
			MapControl.sc_mode  = 1;	
			MapControl.sc_count = 0;
			MapControl.sc_max   = 0;

			MapControl.fd_type  = 0;
			MapControl.fd_count = 0;
			MapControl.fd_max   = 0;
			MapControl.er = MapControl.sr = MapControl.r = 128;
			MapControl.eg = MapControl.sg = MapControl.g = 128;
			MapControl.eb = MapControl.sb = MapControl.b = 128;
			MapControl.ecno = 0;
			MapControl.battle_flag = 0;
			













			






			
			MapControl.next_step = BTL_EVENT;

			break;
		case BTL_TERM:

AVG_ResetBack( -1, FALSE, 0, 0 );
BTL_ReleaseBattleMap();
ResetMapCharAll();
ResetMagicActionAll();
ResetMagicActionAll2();
ResetBardActionAll();
AVG_ResetWeather(1);

SetBrightBattleMap( 128, 128, 128, 0, 0 );
GD_SetGlobalColor( 128, 128, 128, 0, 0 );

		Avg.msg_cut_mode = 0;

			for(i=0;i<GRP_DEBUG;i++){
				GD_ResetGraph( i );
			}
			for(i=0;i<BMP_DEBUG;i++){
				GD_ReleaseTexture( i );
			}
			for(i=0;i<SPR_DEBUG;i++){
				GD_ReleaseSprite( i );
			}
			MUS_ResetMouseRect_All();
			ResetGameWindow_All();

			ret = NextGameStep;
			MapControl.next_step = BTL_INIT;

			break;
		case BTL_EVENT:
			RenewBattleMap();
			RenewCharAction( 1 );
			break;

		case BTL_SORTILE:
			RenewBattleMap();
			switch( BTL_Sortile( &aria  ) ){
				case 0:
					MapControl.next_step = BTL_BATTLE;
				case 1:
					break;
				case 2:
					AllCharControl(0,-1);
					break;
			}
			DrawMapMoveAria(!aria,1, 96+STD_LimitLoop( GlobalCount*2, 64), 28,97,253 );
			RenewCharAction( 0 );
			break;

		case BTL_MENU:
			RenewBattleMap();
			switch( BTL_MenuCoomand( &aria  ) ){
				default:
					break;
				case 0:
					SetMapCharSpriteModeAll( 1 );
					MapControl.next_step = BTL_BATTLE;
					break;
				case 2:
					MapControl.next_step = BTL_COMMAND;
					break;

			}
			DrawMapMoveAria(!aria,0, 64, 28,97,253 );
			RenewCharAction( 0 );
			break;

		case BTL_COMMAND:
			RenewBattleMap();
			if( !BTL_Coomand( &aria  ) ){
				SetMapCharSpriteModeAll( 1 );
				MapControl.next_step = BTL_BATTLE;
			}
			DrawMapMoveAria(!aria,0, 64, 28,97,253 );
			RenewCharAction( 0 );
			break;

		case BTL_BATTLE_END:
			RenewBattleMap();
			switch( BTL_BattleEnd() ){
				case 1:MapControl.next_step = BTL_EVENT;	break;
				case 2:MapControl.next_step = BTL_SORTILE;	break;
				case 3:MapControl.next_step = BTL_TERM;		break;
			}
			RenewCharAction( 0 );
			break;

		case BTL_BATTLE:
			i = AllCharControl(0,-1);
			if( i!=-1){
				SetMapCharCommand(i, TRUE);
			}

			RenewBattleMap();
			RenewCharSinkin();
			RenewMapCharThink();
			switch( RenewCharAction( 1 ) ){
				case 1:
					SetMapCharSpriteModeAll( 0 );
					MapControl.next_step = BTL_COMMAND;
					break;
				default:
					if(GameKey.cansel){
						SetMapCharSpriteModeAll( 0 );
						MapControl.next_step = BTL_MENU;
					}
					break;
			}

			break;
	}
#if !TAIKEN_FLAG
	if(KeyCond.trg.f1){
	}
	if(KeyCond.trg.f2){
	}
	if(KeyCond.trg.f3){
	}
	if(KeyCond.trg.f4){
	}
	if(KeyCond.trg.f6){
	}
	if(KeyCond.trg.f7){
	}
	if(KeyCond.trg.f8){
	}
	if(KeyCond.trg.f9){
	}
#endif

	AVG_ControlShake();
	AVG_ControlBack();
	AVG_ControlWeather();
	AVG_ControlChar();
	AVG_ControlMessage();
	AVG_ControlSelectWindow();
	
	MapControl.step = MapControl.next_step;

	return ret;
}