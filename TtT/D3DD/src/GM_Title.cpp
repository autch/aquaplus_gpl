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

static int VisModeTable[128][2]={
	{ 0, 1000 },
	{ 0, 1010 },
	{ 0, 1020 },
	{ 0, 1030 },
	{ 0, 1040 },
	{ 0, 1050 },
	{ 0, 2000 },
	{ 0, 2010 },
	{ 0, 3000 },
	{ 0, 3012 },
	{ 0, 3023 },
	{ 0, 4000 },
	{ 0, 4010 },
	{ 0, 5000 },
	{ 0, 6000 },
	{ 0, 6010 },
	{ 0, 7001 },
	{ 0, 7010 },
	{ 0, 8000 },
	{ 0, 8010 },
	{ 0,10000 },
	{ 0,10010 },
	{ 0,10020 },
	{ 0,10030 },
	{ 0,10040 },
	{ 0,10050 },
	{ 0,10060 },
	{ 0,10070 },
	{ 0,10080 },
	{ 0,10090 },
	{ 0,10100 },
	{ 0,10110 },
	{ 0,10120 },
	{ 0,10130 },
	{ 0,10140 },
	{ 0,10150 },
	{ 0,10160 },
	{ 0,10170 },
	{ 0,10180 },
	{ 0,10190 },
	{ 0,10200 },
	{ 0,10210 },
	{ 0,10220 },
	{ 0,10230 },
	{ 0,10240 },
	{ 0,10250 },
	{ 0,20000 },
	{ 0,20010 },
	{ 0,20020 },
	{ 0,20030 },
	{ 0,20040 },
	{ 0,20050 },
	{ 0,20060 },
	{ 0,20070 },
	{ 0,20080 },
	{ 0,20090 },
	{ 0,30000 },
	{ 0,30010 },
	{ 0,30020 },
	{ 0,30030 },
	{ 0,30040 },
	{ 0,30050 },
	{ 0,30060 },
	{ 0,30070 },
	{ 0,30080 },

	{ 1, 1000 },
	{ 1, 1010 },
	{ 1, 1020 },
	{ 1, 2000 },
	{ 1, 2010 },
	{ 1, 2022 },
	{ 1, 3000 },
	{ 1, 3010 },
	{ 1, 3020 },
	{ 1, 4000 },
	{ 1, 4013 },
	{ 1, 4022 },
	{ 1, 5000 },
	{ 1, 5010 },
	{ 1, 5020 },
	{ 1, 6000 },
	{ 1, 6010 },
	{ 1, 6020 },
	{ 1, 6030 },
	{ 1, 6040 },
	{ 1, 6050 },
	{ 1, 7000 },
	{ 1, 7010 },
	{ 1, 7020 },
	{ 1, 7030 },
	{ 1, 8000 },
	{ 1, 8010 },
};


int	CheckSaveVisual10( int vh, int vno )
{
	int	i;
	int	ret=0;
	for(i=0;i<10;i++){
		ret |= SaveVisualFlag[vh][vno][i];
	}
	return ret;
}
int	CheckSaveVisualCount( int vh, int vno, int count )
{
	int	i,j;
	int	ret=0;

	if((vh==0 && vno==100) || (vh==0 && vno==2009)){
		if(count==0)return 0;
		else		return -1;
	}
	j=0;
	for(i=0;i<10;i++){
		if( SaveVisualFlag[vh][vno][i] ){
			if(j==count){
				return i;
			}
			j++;
		}
	}
	return -1;
}


int	WLD_CgMode( int win_no, int win_bno, int gno, int bno, int spno, int layer, int mouse )
{
	enum{
		CG_INIT,
		CG_TERM,

		CG_FADE_IN,
		CG_FADE_OUT,

		CG_MAIN,
		CG_UP,
	};
	static int	step=CG_INIT;
	static int	count=0;
	static int	page=0;

	static int	cg_no=0;
	static int	cg_cnt=0;

	int			ret=0;
	static int	ret_bak=0;
	int			i;
	char		buf[256];

	int			select_no = MUS_GetMouseNo( -1 );
	static int	select_bak=-1;
	int			bw,bh;
	static int	bx,by;

	switch(step){
		case CG_INIT:
			MUS_SetMouseLayer( mouse );

			GD_LoadTexture( bno+2, "Tm200.tga", ALPHA_4444|DITHER_OFF );

			GD_LoadTexture( bno+3, "Tm300.tga", ALPHA_1555 );
			GD_LoadTexture( bno+4, "Tm301.tga", ALPHA_4444|DITHER_OFF );

			GD_SetGraph( gno+12, bno+3, layer, FALSE );

			wsprintf( buf, "%2d/8 ページ", page+1 );
			GD_CreateTextTexture( bno+5, buf, 24, 12, 1, 0, 0, 0 );
			GD_SetGraph( gno+13, bno+5, layer+2, FALSE );
			GD_SetGraphDstMove( gno+13, 560+20, 64+4 );

			GD_SetGraph( gno+14, bno+4, layer+1, FALSE );
			GD_SetGraphSrcRect( gno+14, 0,0, 288, 32 );
			GD_SetGraphDstRect( gno+14, 52, 64, 288, 32 );

			GD_SetGraph( gno+15, bno+2, layer+1, FALSE );
			GD_SetGraphSrcRect( gno+15, 0,64, 192, 32 );
			GD_SetGraphDstRect( gno+15, 560, 506, 192, 32 );

			GD_SetGraph( gno+16, bno+2, layer+1, FALSE );
			GD_SetGraph( gno+17, bno+2, layer+2, FALSE );
			GD_SetGraph( gno+18, bno+2, layer+2, FALSE );

			GD_SetGraphSrcRect( gno+16, 0,128, 192, 32 );
			GD_SetGraphSrcRect( gno+17, 192, 64, 24, 32 );
			GD_SetGraphSrcRect( gno+18, 192+24, 64, 24, 32 );
			GD_SetGraphDstRect( gno+16, 560, 64, 192, 32 );
			GD_SetGraphDstRect( gno+17, 560, 64, 24, 32 );
			GD_SetGraphDstRect( gno+18, 720, 64, 24, 32 );

			for(i=0;i<12;i++){
				int vh  = VisModeTable[i+page*12][0];
				int vno = VisModeTable[i+page*12][1]/10;
				if(vno){
					if( CheckSaveVisual10( vh, vno ) ){
						wsprintf( buf, "%s%05d.tga", (vh)?"sh":"sv", vno*10 );
						MUS_SetMouseRect( mouse, i, 58+177*(i%4), 106+136*(i/4), 154, 116, ON );
					}else{
						wsprintf( buf, "sv00000.tga" );
						MUS_ResetMouseRect( mouse, i );
					}
					GD_LoadTexture( bno+10+i, buf, ALPHA_4444|DITHER_OFF );
					GD_SetGraph( gno+20+i, bno+10+i, layer+2, FALSE );
					GD_SetGraphRect( gno+20+i, 58+177*(i%4), 106+136*(i/4), 0,0, 154, 116 );
					if( CheckSaveVisual10( vh, vno ) ){
						GD_SetGraphFade( gno+20+i, 64 );
					}else{
					}
				}else{
					GD_ResetGraph( gno+20+i );
					MUS_ResetMouseRect( mouse, i );
				}
			}

			MUS_SetMouseRect( mouse, 12, 560,506,192,32, ON );

			MUS_SetMouseRect( mouse, 13, 560, 64, 24,32, ON );
			MUS_SetMouseRect( mouse, 14, 720, 64, 24,32, ON );


			appData.draw_flag=1;
			count=0;
			select_bak=-1;
			step=CG_FADE_IN;
			break;
		case CG_TERM:
			for(i=10;i<200;i++)	{	GD_ResetGraph(gno+i);				}
			for(i=1;i<30;i++)	{	GD_ReleaseTexture(bno+i);			}
			for(i=0;i<32;i++)	{	GD_ReleaseSprite(spno+i);			}
			for(i=0;i<4;i++)	{	MUS_ResetMouseRect_Layer(mouse+i);	}
			for(i=1;i<5;i++)	{	ResetGameWindow(win_no+i);			}
			step=CG_INIT;
			ret=ret_bak;
			break;

		case CG_FADE_IN:
			count++;
			for(i=12;i<19;i++){
				GD_SetGraphDisp( gno+i, TRUE );
				GD_SetGraphAlpha( gno+i, count*16 );
			}
			for(i=0;i<12;i++){
				GD_SetGraphDisp( gno+20+i, TRUE );
				GD_SetGraphAlpha( gno+20+i, count*16 );
			}
			if( count>16 ){
				count = 0;
				step = CG_MAIN;
			}
			break;
		case CG_FADE_OUT:
			count++;
			for(i=12;i<19;i++){
				GD_SetGraphDisp( gno+i, TRUE );
				GD_SetGraphAlpha( gno+i, 256-count*16 );
			}
			for(i=0;i<12;i++){
				GD_SetGraphDisp( gno+20+i, TRUE );
				GD_SetGraphAlpha( gno+20+i, 256-count*16 );
			}
			if( count>16 ){
				count = 0;
				step = CG_TERM;
			}
			break;

		case CG_MAIN:
			if(GameKey.pup   || (select_no==13&&GameKey.click)){	page = LOOP( page-1, 8 ); count=0;SND_Ok();}
			if(GameKey.pdown || (select_no==14&&GameKey.click)){	page = LOOP( page+1, 8 ); count=0;SND_Ok();}

			if(count==0){
				for(i=0;i<12;i++){
					int vh  = VisModeTable[i+page*12][0];
					int vno = VisModeTable[i+page*12][1]/10;

					if(vno){
						if( CheckSaveVisual10( vh, vno ) ){
							wsprintf( buf, "%s%05d.tga", (vh)?"sh":"sv", vno*10 );
							MUS_SetMouseRect( mouse, i, 58+177*(i%4), 106+136*(i/4), 154, 116, ON );
						}else{
							wsprintf( buf, "sv00000.tga" );
							MUS_ResetMouseRect( mouse, i );
						}
						GD_LoadTexture( bno+10+i, buf, ALPHA_4444|DITHER_OFF );
						GD_SetGraph( gno+20+i, bno+10+i, layer+2, TRUE );
						GD_SetGraphRect( gno+20+i, 58+177*(i%4), 106+136*(i/4), 0,0, 154, 116 );
					}else{
						GD_ResetGraph( gno+20+i );
						MUS_ResetMouseRect( mouse, i );
					}
				}

				wsprintf( buf, "%2d/8 ページ", page+1 );
				GD_UpdateTextTexture( bno+5, buf );
			}
			for(i=0;i<12;i++){
				if( MUS_GetMouseRect( mouse,i) ){
					GD_SetGraphFade( gno+20+i, 64 );
				}
			}

			count++;
			
			GD_SetGraphSrcRect( gno+15, 0,64, 192, 32 );
			GD_SetGraphSrcRect( gno+16, 0,128, 192, 32 );
			GD_SetGraphSrcRect( gno+17, 192, 64, 24, 32 );
			GD_SetGraphSrcRect( gno+18, 192+24, 64, 24, 32 );

			if(select_no==12)GD_SetGraphSrcRect( gno+15, 0,96, 192, 32 );
			if(select_no==13)GD_SetGraphSrcRect( gno+17, 192+24*2, 64, 24, 32 );
			if(select_no==14)GD_SetGraphSrcRect( gno+18, 192+24*3, 64, 24, 32 );

			if( select_no!=-1 && select_no<12){
				GD_SetGraphFade( gno+20+select_no, 128 );
				if(GameKey.click){
					cg_cnt = 0;
					cg_no  = select_no+page*12;
					count = 0;
					step = CG_UP;
					SND_Ok();
				}
			}else{
			}

			if(GameKey.cansel || select_no==12 && GameKey.click){
				count = 0;
				step = CG_FADE_OUT;
				ret_bak=1;
				SND_Cansel();
			}
			break;
		case CG_UP:
			if(count==0){
				int vh  = VisModeTable[cg_no][0];
				int vno = VisModeTable[cg_no][1]/10;

				i = CheckSaveVisualCount( vh, vno, cg_cnt );
				cg_cnt++;
				bx=0;
				by=0;


				if(i==-1){
					GD_ResetGraph( gno+19 );
					count = 0;
					step = CG_MAIN;
				}else{
					wsprintf( buf, "%s%04d%d.tga", (vh)?"h":"v", vno, i );
					GD_LoadTexture( bno+6, buf, ALPHA_1555 );
					GD_SetGraph( gno+19, bno+6, layer+3, TRUE );
				}
			}
			GD_GetBmpWH( bno+6, &bw, &bh );
			if(GameKey.mx<100) bx = max(bx-2,0);
			if(GameKey.mx>700) bx = min(bx+2,bw-800);

			if(GameKey.my<100) by = max(by-2,0);
			if(GameKey.my>500) by = min(by+2,bh-600);

			GD_SetGraphSrcRect( gno+19, bx, by ,800,600 );
			GD_SetGraphDstRect( gno+19, 0,0,800,600);

			count++;
			if(GameKey.click){
				SND_Ok();
				count=0;
			}
			if(GameKey.cansel){
				GD_ResetGraph( gno+19 );
				count = 0;
				step = CG_MAIN;
			}
			break;
	}

	return ret;
}

typedef struct{
	char	*scr;
	short	gflag;
	int		hno;
}REPLAY_LIST;

REPLAY_LIST	ReplayList[24]={
	{	"8110.sdt", 210,1000  },	
	{	"8111.sdt", 211,1020  },	
	{	"8210.sdt", 220,2020  },	
	{	"8211.sdt", 221,2000  },	

	{	"8310.sdt", 230,3000  },	
	{	"8311.sdt", 231,3020  },	
	{	"8410.sdt", 240,4000  },	
	{	"8411.sdt", 241,4020  },	

	{	"8412.sdt", 242,6040  },	
	{	"8505.sdt", 250,5000  },	
	{	"8506.sdt", 251,5010  },	
	{	"8610.sdt", 260,6000  },	


	{	"8611.sdt", 261,6010  },	
	{	"8612.sdt", 262,6030  },	
	{	"8613.sdt", 263,6020  },	
	{	"8710.sdt", 270,7030  },	

	{	"8711.sdt", 271,7010  },	
	{	"8811.sdt", 280,8000  },	
	{	NULL, 0 },	
	{	NULL, 0 },	

	{	NULL, 0 },	
	{	NULL, 0 },	
	{	NULL, 0 },	
	{	NULL, 0 },	
};

int	WLD_ReplayMode( int win_no, int win_bno, int gno, int bno, int spno, int layer, int mouse )
{
	enum{
		CG_INIT,
		CG_TERM,

		CG_FADE_IN,
		CG_FADE_OUT,

		CG_MAIN,
		CG_UP,
	};
	static int	step=CG_INIT;
	static int	count=0;
	static int	page=0;
	int			ret=0;
	static int	ret_bak=0;
	int			i;
	char		buf[256];

	int			select_no = MUS_GetMouseNo( -1 );
	static int	select_bak=-1;

	switch(step){
		case CG_INIT:
			MUS_SetMouseLayer( mouse );

			GD_LoadTexture( bno+2, "Tm200.tga", ALPHA_4444|DITHER_OFF );

			GD_LoadTexture( bno+3, "Tm300.tga", ALPHA_1555 );
			GD_LoadTexture( bno+4, "Tm301.tga", ALPHA_4444|DITHER_OFF );

			GD_SetGraph( gno+12, bno+3, layer, FALSE );

			wsprintf( buf, "%2d/2 ページ", page+1 );
			GD_CreateTextTexture( bno+5, buf, 24, 12, 1, 0, 0, 0 );
			GD_SetGraph( gno+13, bno+5, layer+2, FALSE );
			GD_SetGraphDstMove( gno+13, 560+20, 64+4 );

			GD_SetGraph( gno+14, bno+4, layer+1, FALSE );
			GD_SetGraphSrcRect( gno+14, 0,32, 288, 32 );
			GD_SetGraphDstRect( gno+14, 52, 64, 288, 32 );

			GD_SetGraph( gno+15, bno+2, layer+1, FALSE );
			GD_SetGraphSrcRect( gno+15, 0,64, 192, 32 );
			GD_SetGraphDstRect( gno+15, 560, 506, 192, 32 );

			GD_SetGraph( gno+16, bno+2, layer+1, FALSE );
			GD_SetGraph( gno+17, bno+2, layer+2, FALSE );
			GD_SetGraph( gno+18, bno+2, layer+2, FALSE );

			GD_SetGraphSrcRect( gno+16, 0,128, 192, 32 );
			GD_SetGraphSrcRect( gno+17, 192, 64, 24, 32 );
			GD_SetGraphSrcRect( gno+18, 192+24, 64, 24, 32 );
			GD_SetGraphDstRect( gno+16, 560, 64, 192, 32 );
			GD_SetGraphDstRect( gno+17, 560, 64, 24, 32 );
			GD_SetGraphDstRect( gno+18, 720, 64, 24, 32 );

			for(i=0;i<12;i++){
				if(ReplayList[i+page*12].scr){
					if( ESC_GetGameFlag(ReplayList[i+page*12].gflag) ){
						wsprintf( buf, "sh%05d.tga", ReplayList[i+page*12].hno );
						MUS_SetMouseRect( mouse, i, 58+177*(i%4), 106+136*(i/4), 154, 116, ON );
					}else{
						wsprintf( buf, "sv00000.tga" );
						MUS_ResetMouseRect( mouse, i );
					}
					GD_LoadTexture( bno+10+i, buf, ALPHA_4444|DITHER_OFF );
					GD_SetGraph( gno+20+i, bno+10+i, layer+2, FALSE );
					GD_SetGraphRect( gno+20+i, 58+177*(i%4), 106+136*(i/4), 0,0, 154, 116 );
					if( MUS_GetMouseRect( mouse,i) ){
						GD_SetGraphFade( gno+20+i, 64 );
					}
				}else{
					GD_ResetGraph( gno+20+i );
					MUS_ResetMouseRect( mouse, i );
				}
			}

			MUS_SetMouseRect( mouse, 12, 560,506,192,32, ON );

			MUS_SetMouseRect( mouse, 13, 560, 64, 24,32, ON );
			MUS_SetMouseRect( mouse, 14, 720, 64, 24,32, ON );


			appData.draw_flag=1;
			ret_bak=0;
			select_bak=-1;
			count=0;
			step=CG_FADE_IN;
			break;
		case CG_TERM:
			for(i=10;i<200;i++)	{	GD_ResetGraph(gno+i);				}
			for(i=1;i<30;i++)	{	GD_ReleaseTexture(bno+i);			}
			for(i=0;i<32;i++)	{	GD_ReleaseSprite(spno+i);			}
			for(i=0;i<4;i++)	{	MUS_ResetMouseRect_Layer(mouse+i);	}
			for(i=1;i<5;i++)	{	ResetGameWindow(win_no+i);			}
			step=CG_INIT;
			ret=ret_bak;
			break;

		case CG_FADE_IN:
			count++;
			for(i=12;i<19;i++){
				GD_SetGraphDisp( gno+i, TRUE );
				GD_SetGraphAlpha( gno+i, count*16 );
			}
			for(i=0;i<12;i++){
				GD_SetGraphDisp( gno+20+i, TRUE );
				GD_SetGraphAlpha( gno+20+i, count*16 );
			}
			if( count>16 ){
				count = 0;
				step = CG_MAIN;
			}
			break;
		case CG_FADE_OUT:
			count++;
			for(i=12;i<19;i++){
				GD_SetGraphDisp( gno+i, TRUE );
				GD_SetGraphAlpha( gno+i, 256-count*16 );
			}
			for(i=0;i<12;i++){
				GD_SetGraphDisp( gno+20+i, TRUE );
				GD_SetGraphAlpha( gno+20+i, 256-count*16 );
			}
			if( count>16 ){
				count = 0;
				step = CG_TERM;
			}
			break;

		case CG_MAIN:
			if(GameKey.pup   || (select_no==13&&GameKey.click)){	page = LOOP( page-1, 2 ); count=0;SND_Ok();}
			if(GameKey.pdown || (select_no==14&&GameKey.click)){	page = LOOP( page+1, 2 ); count=0;SND_Ok();}

			if(count==0){
				for(i=0;i<12;i++){
					if(ReplayList[i+page*12].scr){
						if( ESC_GetGameFlag(ReplayList[i+page*12].gflag) ){
							wsprintf( buf, "sh%05d.tga", ReplayList[i+page*12].hno );
							MUS_SetMouseRect( mouse, i, 58+177*(i%4), 106+136*(i/4), 154, 116, ON );
						}else{
							wsprintf( buf, "sv00000.tga" );
							MUS_ResetMouseRect( mouse, i );
						}
						GD_LoadTexture( bno+10+i, buf, ALPHA_4444|DITHER_OFF );
						GD_SetGraph( gno+20+i, bno+10+i, layer+2, TRUE );
						GD_SetGraphRect( gno+20+i, 58+177*(i%4), 106+136*(i/4), 0,0, 154, 116 );
					}else{
						GD_ResetGraph( gno+20+i );
						MUS_ResetMouseRect( mouse, i );
					}
				}

				wsprintf( buf, "%2d/2 ページ", page+1 );
				GD_UpdateTextTexture( bno+5, buf );
			}
			for(i=0;i<12;i++){
				if( MUS_GetMouseRect( mouse,i) ){
					GD_SetGraphFade( gno+20+i, 64 );
				}
			}

			count++;

			GD_SetGraphSrcRect( gno+15, 0,64, 192, 32 );
			GD_SetGraphSrcRect( gno+16, 0,128, 192, 32 );
			GD_SetGraphSrcRect( gno+17, 192, 64, 24, 32 );
			GD_SetGraphSrcRect( gno+18, 192+24, 64, 24, 32 );

			if(select_no==12)GD_SetGraphSrcRect( gno+15, 0,96, 192, 32 );
			if(select_no==13)GD_SetGraphSrcRect( gno+17, 192+24*2, 64, 24, 32 );
			if(select_no==14)GD_SetGraphSrcRect( gno+18, 192+24*3, 64, 24, 32 );

			if( select_no!=-1 && select_no<12){
				GD_SetGraphFade( gno+20+select_no, 128 );
				if(GameKey.click){

					AVG_StopBGM( 30 );
					count = 0;
					step = CG_TERM;
					
					Avg.omake=1;

					ret_bak=2+SYS_BATTLE;
					wsprintf( GoScript, ReplayList[select_no+page*12].scr );
				}
			}else{
			}

			if(GameKey.cansel || select_no==12 && GameKey.click){
				Avg.omake=0;
				count = 0;
				step = CG_FADE_OUT;
				ret_bak=1;
			}
			break;
	}

	return ret;
}



int	WLD_VoiceMode( int win_no, int win_bno, int gno, int bno, int spno, int layer, int mouse )
{
	enum{
		CG_INIT,
		CG_TERM,

		CG_FADE_IN,
		CG_FADE_OUT,

		CG_MAIN,
		CG_UP,
	};
	static int	step=CG_INIT;
	static int	count=0;
	int			ret=0;
	static int	ret_bak=0;
	int			i;
	char		buf[256];

	int			select_no = MUS_GetMouseNo( -1 );
	static int	select_bak=-1;
	int			VoiceCharNo[12]={0,1,2,3, 4,5,6,7, 8,10,11 };

	switch(step){
		case CG_INIT:
			MUS_SetMouseLayer( mouse );

			GD_LoadTexture( bno+2, "Tm200.tga", ALPHA_4444|DITHER_OFF );

			GD_LoadTexture( bno+3, "Tm300.tga", ALPHA_1555 );
			GD_LoadTexture( bno+4, "Tm301.tga", ALPHA_4444|DITHER_OFF );

			GD_SetGraph( gno+12, bno+3, layer, FALSE );

			GD_SetGraph( gno+14, bno+4, layer+1, FALSE );
			GD_SetGraphSrcRect( gno+14, 0,64, 288, 32 );
			GD_SetGraphDstRect( gno+14, 52, 64, 288, 32 );

			GD_SetGraph( gno+15, bno+2, layer+1, FALSE );
			GD_SetGraphSrcRect( gno+15, 0,64, 192, 32 );
			GD_SetGraphDstRect( gno+15, 560, 506, 192, 32 );

			for(i=0;i<11;i++){
				if( 1 ){
					wsprintf( buf, "scha%02d0.tga", VoiceCharNo[i] );
					MUS_SetMouseRect( mouse, i, 58+177*(i%4), 106+136*(i/4), 154, 116, ON );
				}else{
					wsprintf( buf, "sv00000.tga" );
					MUS_ResetMouseRect( mouse, i );
				}
				GD_LoadTexture( bno+10+i, buf, ALPHA_4444|DITHER_OFF );
				GD_SetGraph( gno+20+i, bno+10+i, layer+2, FALSE );
				GD_SetGraphRect( gno+20+i, 58+177*(i%4), 106+136*(i/4), 0,0, 154, 116 );
				GD_SetGraphFade( gno+20+i, 64 );
			}

			MUS_SetMouseRect( mouse, 12, 560,506,192,32, ON );



			appData.draw_flag=1;
			count=0;
			select_bak=-1;
			step=CG_FADE_IN;
			break;
		case CG_TERM:
			for(i=10;i<200;i++)	{	GD_ResetGraph(gno+i);				}
			for(i=1;i<30;i++)	{	GD_ReleaseTexture(bno+i);			}
			for(i=0;i<32;i++)	{	GD_ReleaseSprite(spno+i);			}
			for(i=0;i<4;i++)	{	MUS_ResetMouseRect_Layer(mouse+i);	}
			for(i=1;i<5;i++)	{	ResetGameWindow(win_no+i);			}
			step=CG_INIT;
			ret=ret_bak;
			break;

		case CG_FADE_IN:
			count++;
			for(i=12;i<19;i++){
				GD_SetGraphDisp( gno+i, TRUE );
				GD_SetGraphAlpha( gno+i, count*16 );
			}
			for(i=0;i<12;i++){
				GD_SetGraphDisp( gno+20+i, TRUE );
				GD_SetGraphAlpha( gno+20+i, count*16 );
			}
			if( count>16 ){
				count = 0;
				step = CG_MAIN;
			}
			break;
		case CG_FADE_OUT:
			count++;
			for(i=12;i<19;i++){
				GD_SetGraphDisp( gno+i, TRUE );
				GD_SetGraphAlpha( gno+i, 256-count*16 );
			}
			for(i=0;i<12;i++){
				GD_SetGraphDisp( gno+20+i, TRUE );
				GD_SetGraphAlpha( gno+20+i, 256-count*16 );
			}
			if( count>16 ){
				count = 0;
				step = CG_TERM;
			}
			break;

		case CG_MAIN:
			for(i=0;i<12;i++){
				GD_SetGraphFade( gno+20+i, 64 );
			}

			count++;
			
			GD_SetGraphSrcRect( gno+15, 0,64, 192, 32 );
			if(select_no==12)GD_SetGraphSrcRect( gno+15, 0,96, 192, 32 );

			if( select_no!=-1 && select_no<12){
				GD_SetGraphFade( gno+20+select_no, 128 );
				if(GameKey.click){
					wsprintf( buf, "cha%02d0.tga", VoiceCharNo[select_no] );
					GD_LoadTexture( bno+6, buf, ALPHA_1555 );
					GD_SetGraph( gno+19, bno+6, layer+3, TRUE );

					count = 0;
					step = CG_UP;

			AVG_StopBGM( 120 );
AVG_PlayOmakeVoice( VoiceCharNo[select_no] );
				}
			}else{
			}

			if(GameKey.cansel || select_no==12 && GameKey.click){
				count = 0;
				step = CG_FADE_OUT;
				ret_bak=1;
			}
			break;
		case CG_UP:
			if(GameKey.click){
			}
			if(GameKey.cansel){
				AVG_PlayBGM( 30, 0, TRUE, 255, FALSE );
				AVG_StopVoice(0);
				GD_ResetGraph( gno+19 );
				count = 0;
				step = CG_MAIN;
			}
			break;
	}

	return ret;
}




int	SoundModeNo[30]={
	 1,
	30,
	 2,
	 3,
	15,
	 4,
	 7,
	 9,
	 8,
	10,

	14,
	 6,
	31,
	11,
	12,
	13,
	19,
	20,
	22,
	32,

	29,
	17,
	18,
	16,
	25,
	26,
	21,
	35,
	37,
	27,
};


int	WLD_SoundMode( int win_no, int win_bno, int gno, int bno, int spno, int layer, int mouse )
{
	enum{
		CG_INIT,
		CG_TERM,

		CG_FADE_IN,
		CG_FADE_OUT,

		CG_MAIN,
		CG_UP,
	};
	static int	step=CG_INIT;
	static int	count=0;
	int			ret=0;
	static int	ret_bak=0;
	int			i;
	char		buf[256];

	int			select_no = MUS_GetMouseNo( -1 );
	static int	select_bak=-1;
	int			VoiceCharNo[12]={0,1,2,3, 4,5,6,7, 8,10,11 };
	static int	select_mus=-1;
	static int	rand_falg=0;
	static int	rennzoku_falg=0;

	switch(step){
		case CG_INIT:
			MUS_SetMouseLayer( mouse );

			GD_LoadTexture( bno+2, "Tm200.tga", ALPHA_4444|DITHER_OFF );

			GD_LoadTexture( bno+3, "Tm400.tga", ALPHA_1555 );
			GD_LoadTexture( bno+4, "Tm401.tga", ALPHA_1555 );
			GD_LoadTexture( bno+5, "Tm402.tga", ALPHA_1555 );
			GD_LoadTexture( bno+6, "Tm403.tga", ALPHA_1555 );
			GD_LoadTexture( bno+7, "Tm404.tga", ALPHA_1555 );
			GD_LoadTexture( bno+8, "Tm405.tga", ALPHA_1555 );

			GD_SetGraph( gno+12, bno+3, layer, FALSE );

			GD_SetGraph( gno+15, bno+2, layer+1, FALSE );
			GD_SetGraphSrcRect( gno+15, 0,64, 192, 32 );
			GD_SetGraphDstRect( gno+15, 560, 506, 192, 32 );

			for(i=0;i<30;i++){
				if( ESC_GetGameFlag( 128+SoundModeNo[i] ) ){
					GD_SetGraph( gno+20+i, bno+8, layer+2, FALSE );
					GD_SetGraphRect( gno+20+i, 28+258*(i/10), 110+40*(i%10), 228*(i/10), 18*(i%10), 228, 18 );

					MUS_SetMouseRect( mouse, i, 28+258*(i/10), 110+40*(i%10), 228, 18, ON );
				}else{
					MUS_ResetMouseRect( mouse, i );
				}
			}

			MUS_SetMouseRect( mouse, 30, 560,506,192,32, ON );



			appData.draw_flag=1;
			count=0;
			select_bak=-1;
			step=CG_FADE_IN;
			break;
		case CG_TERM:
			for(i=10;i<200;i++)	{	GD_ResetGraph(gno+i);				}
			for(i=1;i<30;i++)	{	GD_ReleaseTexture(bno+i);			}
			for(i=0;i<32;i++)	{	GD_ReleaseSprite(spno+i);			}
			for(i=0;i<4;i++)	{	MUS_ResetMouseRect_Layer(mouse+i);	}
			for(i=1;i<5;i++)	{	ResetGameWindow(win_no+i);			}
			step=CG_INIT;
			ret=ret_bak;
			break;

		case CG_FADE_IN:
			count++;
			for(i=12;i<19;i++){
				GD_SetGraphDisp( gno+i, TRUE );
				GD_SetGraphAlpha( gno+i, count*16 );
			}
			for(i=0;i<30;i++){
				GD_SetGraphDisp( gno+20+i, TRUE );
				GD_SetGraphAlpha( gno+20+i, count*16 );
			}
			if( count>16 ){
				count = 0;
				step = CG_MAIN;
			}
			break;
		case CG_FADE_OUT:
			count++;
			for(i=12;i<19;i++){
				GD_SetGraphDisp( gno+i, TRUE );
				GD_SetGraphAlpha( gno+i, 256-count*16 );
			}
			for(i=0;i<30;i++){
				GD_SetGraphDisp( gno+20+i, TRUE );
				GD_SetGraphAlpha( gno+20+i, 256-count*16 );
			}
			if( count>16 ){
				count = 0;
				step = CG_TERM;
			}
			break;

		case CG_MAIN:
			for(i=0;i<30;i++){
				if( ESC_GetGameFlag( 128+SoundModeNo[i] ) ){
					GD_SetGraph( gno+20+i, bno+8, layer+2, TRUE );
					GD_SetGraphRect( gno+20+i, 28+258*(i/10), 110+40*(i%10), 228*(i/10), 18*(i%10), 228, 18 );
				}

			}

			count++;
			
			GD_SetGraphSrcRect( gno+15, 0,64, 192, 32 );
			if(select_no==30)GD_SetGraphSrcRect( gno+15, 0,96, 192, 32 );

			if(select_mus!=-1){
				GD_SetGraph( gno+20+select_mus, bno+4, layer+2, TRUE );
				GD_SetGraphRect( gno+20+select_mus, 28+258*(select_mus/10), 110+40*(select_mus%10), 228*(select_mus/10), 18*(select_mus%10), 228, 18 );
			}

			if( select_no!=-1 && select_no<30){
				if(select_mus==select_no){
					GD_SetGraph( gno+20+select_no, bno+6, layer+2, TRUE );
				}else{
					GD_SetGraph( gno+20+select_no, bno+5, layer+2, TRUE );
				}
				if(GameKey.click){
					GD_SetGraph( gno+20+select_no, bno+7, layer+2, TRUE );
					appData.draw_flag=1;

					if(select_mus==-1){
						select_mus=select_no;
						AVG_PlayBGM( SoundModeNo[select_no], 0, 1, 255, TRUE );
					}else{
						if(select_mus==select_no){
							select_mus = -1;
							AVG_StopBGM( 60 );
						}else{
							select_mus=select_no;
							AVG_PlayBGM( SoundModeNo[select_no], 60, 1, 255, TRUE );
						}

					}
				}
				GD_SetGraphRect( gno+20+select_no, 28+258*(select_no/10), 110+40*(select_no%10), 228*(select_no/10), 18*(select_no%10), 228, 18 );
			}else{
			}

			if(GameKey.cansel || select_no==30 && GameKey.click){
				count = 0;
				step = CG_FADE_OUT;
				ret_bak=1;
			}
			break;
		case CG_UP:
			if(GameKey.click){
			}
			if(GameKey.cansel){
				count = 0;
				step = CG_MAIN;
			}
			break;
	}

	return ret;
}

int	NextTTLStep = SYS_BATTLE;

#include "GM_SetBmp.h"


int	TTL_System(void)
{
	enum{
		TTL_LEAF_GO,
		TTL_LEAF_WAIT,
		TTL_MOV_GO,
		TTL_MOV_WAIT,

		TTL_INIT,
		TTL_TERM,

		TTL_FADE_IN,
		TTL_FADE_OUT,

		TTL_SAVE_LOAD,
		TTL_CG,
		TTL_RELAY,
		TTL_SOUND,
		TTL_VOICE,
	
		TTL_COMMAND,
		TTL_OMAKE
	};
	static int	step=TTL_LEAF_GO;
	static int	count=0;
	static int	cx,cy;
	char		buf[256];

	float		rate;
	static int	select_bak=-1;
	static int	select_bak2=-1;
	int			select_no = MUS_GetMouseNo( -1 );
	static int	select_bak_lst[10];

	

	int			omake1 = ESC_GetGameFlag(60);
	int			omake2 = ESC_GetGameFlag(62);

	int			ret=-1;
	int			i,j;

	int			mx,my;

	switch(step){
		case TTL_LEAF_GO:
			AVG_SetAviFull(1);
			count=0;
			step = TTL_LEAF_WAIT;
		case TTL_LEAF_WAIT:
			if( AVG_WaitAviFull() ){
				count=0;
				step = TTL_MOV_GO;
			}
			break;
		case TTL_MOV_GO:
			if( ESC_GetGameFlag(61) ){
				AVG_SetAviFull();
				count=0;
				step = TTL_MOV_WAIT;
			}else{
				count=0;
				step = TTL_INIT;
			}
		case TTL_MOV_WAIT:
			if( AVG_WaitAviFull() ){
				count=0;
				step = TTL_INIT;
			}
			break;
		case TTL_INIT:
			AVG_PlayBGM( 30, 0, TRUE, 255, FALSE );
			GD_LoadTexture( WLD_BMP_SYSTEM, "btsys_0000.tga", ALPHA_4444|DITHER_OFF );
			GD_LoadTexture( WLD_BMP_WORLD+0, "Tm100.tga", ALPHA_1555 );
			GD_LoadTexture( WLD_BMP_SYSTEM+1, "Tm101.tga", ALPHA_1555 );
			GD_LoadTexture( WLD_BMP_SYSTEM+2, "Tm102.tga", ALPHA_1555 );
			cx=400;
			cy=300;

			MUS_ResetMouseRect_All();
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


			for(i=0;i<5;i++){
				select_bak_lst[i]=0;
				select_bak_lst[i+5]=0;
				j = i*50;

				if(i!=3)
					MUS_SetMouseRect( 0, i, 46, 234+j, 160, 40, ON );

				switch(i){
					case 0:
						if( omake1 )
							MUS_SetMouseRect( 1, i, 46, 234+j, 160, 40, ON );
						break;
					case 1:
#if !TAIKEN_FLAG
							MUS_SetMouseRect( 1, i, 46, 234+j, 160, 40, ON );
#endif
						break;
					case 3:
						if( omake1 )
							MUS_SetMouseRect( 1, i, 46, 234+j, 160, 40, ON );
						break;
					default:
						MUS_SetMouseRect( 1, i, 46, 234+j, 160, 40, ON );
						break;
				}

				GD_SetGraph( WLD_GRP_SYSTEM+i, WLD_BMP_SYSTEM+1, WLD_LAY_SYSTEM+2, FALSE );
				GD_SetGraphSrcRect( WLD_GRP_SYSTEM+i, 0, 0, 160, 40 );
				GD_SetGraphDstRect( WLD_GRP_SYSTEM+i, 46, 234+j, 160, 40 );

				GD_SetGraph( WLD_GRP_SYSTEM+5+i, WLD_BMP_SYSTEM+1, WLD_LAY_SYSTEM+2, FALSE );
				GD_SetGraphSrcRect( WLD_GRP_SYSTEM+5+i, 0, 0, 160, 40 );
				GD_SetGraphDstRect( WLD_GRP_SYSTEM+5+i, 46, 234+j, 160, 40 );

				GD_SetGraph( WLD_GRP_SYSTEM+10+i, WLD_BMP_SYSTEM+2, WLD_LAY_SYSTEM+1, FALSE );
				GD_SetGraphSrcRect( WLD_GRP_SYSTEM+10+i, 0, 0, 60, 60 );
				GD_SetGraphDstRect( WLD_GRP_SYSTEM+10+i, 10, 224+j, 60, 60 );
				GD_SetGraphBlendParam( WLD_GRP_SYSTEM+10+i, BLEND_ADD );
				GD_SetGraphScaleParam( WLD_GRP_SYSTEM+10+i, SCALE_HIGHT );
				GD_SetGraphCenter( WLD_GRP_SYSTEM+10+i, 10+30, 224+30+j );

				GD_SetGraph( WLD_GRP_SYSTEM+15+i, WLD_BMP_SYSTEM+2, WLD_LAY_SYSTEM+1, FALSE );
				GD_SetGraphSrcRect( WLD_GRP_SYSTEM+15+i, 0, 0, 60, 60 );
				GD_SetGraphDstRect( WLD_GRP_SYSTEM+15+i, 10, 224+j, 60, 60 );
				GD_SetGraphBlendParam( WLD_GRP_SYSTEM+15+i, BLEND_ADD );
				GD_SetGraphScaleParam( WLD_GRP_SYSTEM+15+i, SCALE_HIGHT );
				GD_SetGraphCenter( WLD_GRP_SYSTEM+15+i, 10+30, 224+30+j );
			}


			NextTTLStep = SYS_BATTLE;
			select_bak2=-1;
			select_bak=-1;
			count=0;
			if(Avg.omake){
				count=16;
				step = TTL_RELAY;
			}else{
				step = TTL_FADE_IN;
			}
			break;
		case TTL_TERM:
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

			ret = NextTTLStep;

			count=0;
			step = TTL_INIT;
			break;
		case TTL_FADE_IN:
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
				step = TTL_COMMAND;
			}
			break;
		case TTL_FADE_OUT:
			count++;
			rate = POW_CONUT(count,20);
			
			GD_SetGraphDisp( WLD_GRP_WORLD+0, ON );
			GD_SetGraphFade( WLD_GRP_WORLD+0, 128-128*rate );

			for(i=0;i<5;i++){
				GD_SetGraphFade( WLD_GRP_SYSTEM+i, 128-128*rate );
				GD_SetGraphFade( WLD_GRP_SYSTEM+5+i, 128-128*rate );
				GD_SetGraphFade( WLD_GRP_SYSTEM+10+i, 128-128*rate );
				GD_SetGraphFade( WLD_GRP_SYSTEM+15+i, 128-128*rate );
			}


			if(count>=30){
				step = TTL_TERM;
			}
			break;
		case TTL_COMMAND:
			for(i=0;i<5;i++){
				if( MUS_GetMouseRect(0,i) ){
					GD_SetGraphDisp( WLD_GRP_SYSTEM+i, TRUE );
					GD_SetGraphSrcRect( WLD_GRP_SYSTEM+i, 0, 40*i, 160, 40 );
					GD_SetGraphAlpha( WLD_GRP_SYSTEM+i, count*16 );

					GD_SetGraphDisp( WLD_GRP_SYSTEM+10+i, TRUE );
					GD_SetGraphRoll( WLD_GRP_SYSTEM+10+i, GlobalCount%360 );
					GD_SetGraphAlpha( WLD_GRP_SYSTEM+10+i, select_bak_lst[i]*16 );
					select_bak_lst[i]   = LIM(select_bak_lst[i]-1, 0, 16 );
				}
				if(select_bak2 == TTL_OMAKE){
					if( MUS_GetMouseRect(1,i) ){
						GD_SetGraphDisp( WLD_GRP_SYSTEM+5+i, TRUE );
						GD_SetGraphSrcRect( WLD_GRP_SYSTEM+5+i, 0, 200+40*i, 160, 40 );
						GD_SetGraphAlpha( WLD_GRP_SYSTEM+5+i, 256-count*16 );

						GD_SetGraphDisp( WLD_GRP_SYSTEM+15+i, TRUE );
						GD_SetGraphRoll( WLD_GRP_SYSTEM+15+i, 360-(GlobalCount%360) );
						GD_SetGraphAlpha( WLD_GRP_SYSTEM+15+i, select_bak_lst[5+i]*16 );
						select_bak_lst[5+i] = LIM(select_bak_lst[5+i]-1, 0, 16 );
					}
				}
			}
			count+=1;

			if( select_no!=-1 ){
				GD_SetGraphSrcRect( WLD_GRP_SYSTEM+select_no, 160, 40*select_no, 160, 40 );
				select_bak_lst[select_no] = LIM(select_bak_lst[select_no]+2, 0, 16 );

				if(select_bak != select_no ){
					SND_Select();
				}
				if( GameKey.click ){
					select_bak2=TTL_COMMAND;
					switch(select_no){
					case 0:
						SAV_StartReset();
						wsprintf( GoScript, "0100.sdt" );
						count=0;
						step = TTL_FADE_OUT;
						SND_Ok();
						break;
					case 1:
						count=16;
						step = TTL_SAVE_LOAD;
						SND_Ok();
						break;
					case 2:
						MUS_SetMouseLayer( 1 );
						count=0;
						step = TTL_OMAKE;
						SND_Ok();
						break;
					case 3:
						break;
					case 4:
						NextTTLStep = SYS_TERM;
						count=0;
						step = TTL_FADE_OUT;
						SND_Ok();
						break;
					}
				}
			}else{
			}

			select_bak=select_no;
			break;
		case TTL_OMAKE:
			for(i=0;i<5;i++){
				if(select_bak2 == TTL_COMMAND){
					if( MUS_GetMouseRect(0,i) ){
						GD_SetGraphDisp( WLD_GRP_SYSTEM+i, TRUE );
						GD_SetGraphSrcRect( WLD_GRP_SYSTEM+i, 0, 40*i, 160, 40 );
						GD_SetGraphAlpha( WLD_GRP_SYSTEM+i, 256-count*16 );

						select_bak_lst[i]   = LIM(select_bak_lst[i]-1, 0, 15 );
						GD_SetGraphDisp( WLD_GRP_SYSTEM+10+i, TRUE );
						GD_SetGraphRoll( WLD_GRP_SYSTEM+10+i, GlobalCount%360 );
						GD_SetGraphAlpha( WLD_GRP_SYSTEM+10+i, select_bak_lst[i]*16 );
					}
				}

				if( MUS_GetMouseRect(1,i) ){
					GD_SetGraphDisp( WLD_GRP_SYSTEM+5+i, TRUE );
					GD_SetGraphSrcRect( WLD_GRP_SYSTEM+5+i, 0, 200+40*i, 160, 40 );
					GD_SetGraphAlpha( WLD_GRP_SYSTEM+5+i, count*16 );

					select_bak_lst[5+i] = LIM(select_bak_lst[5+i]-1, 0, 15 );
					GD_SetGraphDisp( WLD_GRP_SYSTEM+15+i, TRUE );
					GD_SetGraphRoll( WLD_GRP_SYSTEM+15+i, 360-(GlobalCount%360) );
					GD_SetGraphAlpha( WLD_GRP_SYSTEM+15+i, select_bak_lst[5+i]*16 );
				}
			}
			count+=1;

			if( select_no!=-1 ){
				GD_SetGraphSrcRect( WLD_GRP_SYSTEM+5+select_no, 160, 40*(5+select_no), 160, 40 );
				select_bak_lst[select_no+5] = LIM(select_bak_lst[select_no+5]+2, 0, 16 );


				if(select_bak != select_no ){
					SND_Select();
				}
				if( GameKey.click ){
					select_bak2 = TTL_OMAKE;
					switch(select_no){
					case 0:
						count=16;
						step = TTL_CG;
						SND_Ok();
						break;
					case 1:
						count=16;
						step = TTL_RELAY;
						SND_Ok();
						break;
					case 2:
						count=16;
						step = TTL_SOUND;
						SND_Ok();
						break;
					case 3:
						count=16;
						step = TTL_VOICE;
						SND_Ok();
						break;
					case 4:
						MUS_SetMouseLayer( 0 );
						count=0;
						step = TTL_COMMAND;

						SND_Cansel();
						break;
					}
				}
			}else{
			}
			if(GameKey.cansel){
				select_bak2 = TTL_OMAKE;
				MUS_SetMouseLayer( 0 );
				count=0;
				step = TTL_COMMAND;
				SND_Cansel();
			}
			select_bak=select_no;
			break;
		case TTL_SAVE_LOAD:
			count = max(0,count-1);

			for(i=0;i<5;i++){
				GD_SetGraphAlpha( WLD_GRP_SYSTEM+i, count*16 );
				GD_SetGraphDisp( WLD_GRP_SYSTEM+5+i, FALSE );

				select_bak_lst[i]   = LIM(select_bak_lst[i]-1, 0, 15 );
				GD_SetGraphAlpha( WLD_GRP_SYSTEM+10+i, select_bak_lst[i]*16 );
				select_bak_lst[i+5]   = LIM(select_bak_lst[i+5]-1, 0, 15 );
				GD_SetGraphAlpha( WLD_GRP_SYSTEM+15+i, select_bak_lst[i+5]*16 );
			}
			
			i = WLD_SaveLoad( 1, WLD_BMP_SYSTEM, WLD_GRP_WINDW, WLD_BMP_SYSTEM+3, WLD_SPR_CHAR_SP, WLD_LAY_SYSTEM+2, 2, SYS_TITLE );
			
			switch( i ){
				case 0:
					break;
				case 1:
					count=0;
					MUS_SetMouseLayer( 0 );
					step = TTL_COMMAND;
					break;
				default:
				case 2:
					NextTTLStep = i-2;
					count=0;
					step = TTL_FADE_OUT;
					break;
			}
			break;
		case TTL_CG:
			count = max(0,count-1);

			for(i=0;i<5;i++){
				GD_SetGraphDisp( WLD_GRP_SYSTEM+i, FALSE );
				GD_SetGraphAlpha( WLD_GRP_SYSTEM+5+i, count*16 );

				select_bak_lst[i]   = LIM(select_bak_lst[i]-1, 0, 15 );
				GD_SetGraphAlpha( WLD_GRP_SYSTEM+10+i, select_bak_lst[i]*16 );
				select_bak_lst[i+5]   = LIM(select_bak_lst[i+5]-1, 0, 15 );
				GD_SetGraphAlpha( WLD_GRP_SYSTEM+15+i, select_bak_lst[i+5]*16 );
			}
			
			i = WLD_CgMode( 1, WLD_BMP_SYSTEM, WLD_GRP_WINDW, WLD_BMP_SYSTEM+3, WLD_SPR_CHAR_SP, WLD_LAY_SYSTEM+2, 2 );
			
			switch( i ){
				default:
				case 0:
					break;
				case 1:
					count=0;
					MUS_SetMouseLayer( 1 );
					step = TTL_OMAKE;
					break;
			}
			break;
		case TTL_RELAY:
			if(Avg.omake==0)
				GD_SetGraphDisp( WLD_GRP_WORLD+0, TRUE );	
			count = max(0,count-1);

			for(i=0;i<5;i++){
				GD_SetGraphDisp( WLD_GRP_SYSTEM+i, FALSE );
				GD_SetGraphAlpha( WLD_GRP_SYSTEM+5+i, count*16 );

				select_bak_lst[i]   = LIM(select_bak_lst[i]-1, 0, 15 );
				GD_SetGraphAlpha( WLD_GRP_SYSTEM+10+i, select_bak_lst[i]*16 );
				select_bak_lst[i+5]   = LIM(select_bak_lst[i+5]-1, 0, 15 );
				GD_SetGraphAlpha( WLD_GRP_SYSTEM+15+i, select_bak_lst[i+5]*16 );
			}
			
			i = WLD_ReplayMode( 1, WLD_BMP_SYSTEM, WLD_GRP_WINDW, WLD_BMP_SYSTEM+3, WLD_SPR_CHAR_SP, WLD_LAY_SYSTEM+2, 2 );
			
			switch( i ){
				case 0:
					break;
				case 1:
					count=0;
					MUS_SetMouseLayer( 1 );
					step = TTL_OMAKE;
					break;
				default:
				case 2:
					NextTTLStep = i-2;
					count=0;
					step = TTL_FADE_OUT;
					break;
			}
			break;
		case TTL_SOUND:
			count = max(0,count-1);

			for(i=0;i<5;i++){
				GD_SetGraphDisp( WLD_GRP_SYSTEM+i, FALSE );
				GD_SetGraphAlpha( WLD_GRP_SYSTEM+5+i, count*16 );

				select_bak_lst[i]   = LIM(select_bak_lst[i]-1, 0, 15 );
				GD_SetGraphAlpha( WLD_GRP_SYSTEM+10+i, select_bak_lst[i]*16 );
				select_bak_lst[i+5]   = LIM(select_bak_lst[i+5]-1, 0, 15 );
				GD_SetGraphAlpha( WLD_GRP_SYSTEM+15+i, select_bak_lst[i+5]*16 );
			}
			
			i = WLD_SoundMode( 1, WLD_BMP_SYSTEM, WLD_GRP_WINDW, WLD_BMP_SYSTEM+3, WLD_SPR_CHAR_SP, WLD_LAY_SYSTEM+2, 2 );
			
			switch( i ){
				default:
				case 0:
					break;
				case 1:
					count=0;
					MUS_SetMouseLayer( 1 );
					step = TTL_OMAKE;
					break;
			}
			break;
		case TTL_VOICE:
			count = max(0,count-1);

			for(i=0;i<5;i++){
				GD_SetGraphDisp( WLD_GRP_SYSTEM+i, FALSE );
				GD_SetGraphAlpha( WLD_GRP_SYSTEM+5+i, count*16 );

				select_bak_lst[i]   = LIM(select_bak_lst[i]-1, 0, 15 );
				GD_SetGraphAlpha( WLD_GRP_SYSTEM+10+i, select_bak_lst[i]*16 );
				select_bak_lst[i+5]   = LIM(select_bak_lst[i+5]-1, 0, 15 );
				GD_SetGraphAlpha( WLD_GRP_SYSTEM+15+i, select_bak_lst[i+5]*16 );
			}
			
			i = WLD_VoiceMode( 1, WLD_BMP_SYSTEM, WLD_GRP_WINDW, WLD_BMP_SYSTEM+3, WLD_SPR_CHAR_SP, WLD_LAY_SYSTEM+2, 2 );
			
			switch( i ){
				default:
				case 0:
					break;
				case 1:
					count=0;
					MUS_SetMouseLayer( 1 );
					step = TTL_OMAKE;
					break;
			}
			break;
	}

	if(KeyCond.trg.f1){
	}
	if(KeyCond.trg.f2){
	}
	if(KeyCond.trg.f3){
	}

	if(KeyCond.btn.z){
	}
	
	return ret;
}