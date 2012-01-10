#ifndef	_GM_BATTLE_H_
#define _GM_BATTLE_H_

#define GRP_MAP_BACE	(0)
#define GRP_MAP_BACK	(GRP_MAP_BACE+1)
#define GRP_MAP_MAIN	(GRP_MAP_BACK+1)
#define GRP_MAP_OBJECT	(GRP_MAP_MAIN+1)
#define GRP_MAP_EVT		(GRP_MAP_OBJECT+32)
#define GRP_MAP_CHAR	(GRP_MAP_EVT+16)
#define GRP_START_CHAR	(GRP_MAP_CHAR+256)
#define GRP_MG_EFFECT	(GRP_START_CHAR+10+10+10)
#define GRP_MG_WORK		(GRP_MG_EFFECT+32)
#define GRP_ITEM_WORK	(GRP_MG_WORK+16)

#define GRP_BARD_EFFECT	(GRP_ITEM_WORK+16)
#define GRP_HIT_EFFECT	(GRP_BARD_EFFECT+80)
#define GRP_HIT_DAMEGE	(GRP_HIT_EFFECT+32)
#define GRP_ARROW		(GRP_HIT_DAMEGE+32)
#define GRP_MAP_MOVE	(GRP_ARROW+32)
#define GRP_MAP_FADE	(GRP_MAP_MOVE+512)

#define GRP_MAP_CONSOL	(GRP_MAP_FADE+1)

#define GRP_SCRIPT		(GRP_MAP_CONSOL+16)

#define GRP_WEATHER		(GRP_SCRIPT+32)
#define GRP_BACK		(GRP_WEATHER+200)
#define GRP_CHAR		(GRP_BACK+4)
#define GRP_CHAR_RENDER	(GRP_CHAR+16)
#define GRP_SYSTEM		(GRP_CHAR_RENDER+1)
#define GRP_MAIN_TEXT	(GRP_SYSTEM+32)
#define GRP_TEXT_WAIT	(GRP_MAIN_TEXT+3)
#define GRP_SELECT_TEXT	(GRP_TEXT_WAIT+2)
#define GRP_CHAR_POPUP	(GRP_SELECT_TEXT+16)

#define GRP_WORK		(GRP_CHAR_POPUP+32)
#define GRP_DEBUG		(GRP_WORK+32)




#define BMP_MAP_BACK	(0)
#define BMP_MAP_MAIN	(BMP_MAP_BACK+2)
#define BMP_MAP_EVT		(BMP_MAP_MAIN+2)
#define BMP_MAP_CHAR	(BMP_MAP_EVT+2)
#define BMP_START_CHAR	(BMP_MAP_CHAR+32)
#define BMP_EFFECT		(BMP_START_CHAR+20)
#define BMP_MG_EFFECT	(BMP_EFFECT+8)
#define BMP_HIT_DAMEGE	(BMP_MG_EFFECT+8)
#define BMP_ARROW		(BMP_HIT_DAMEGE+3)

#define BMP_SCRIPT		(BMP_ARROW+1)

#define BMP_WEATHER		(BMP_SCRIPT+32)
#define BMP_BACK		(BMP_WEATHER+1)
#define BMP_CHAR		(BMP_BACK+4)
#define BMP_CHAR_RENDER	(BMP_CHAR+16)
#define BMP_SYSTEM		(BMP_CHAR_RENDER+1)
#define BMP_MAIN_TEXT	(BMP_SYSTEM+32)
#define BMP_SELECT_TEXT	(BMP_MAIN_TEXT+3)
#define BMP_CHAR_POPUP	(BMP_SELECT_TEXT+8)

#define BMP_WORK		(BMP_CHAR_POPUP+32)
#define BMP_DEBUG		(BMP_WORK+32)

#define SPR_MAP_OBJECT	(0)
#define SPR_MAP_EVT		(SPR_MAP_OBJECT+1)
#define SPR_MAP_CHAR	(SPR_MAP_EVT+32)
#define SPR_MAP_CHAR2	(SPR_MAP_CHAR+64)
#define SPR_START_CHAR	(SPR_MAP_CHAR2+64)
#define SPR_MG_EFFECT	(SPR_START_CHAR+32)
#define SPR_MG_WORK		(SPR_MG_EFFECT+32)
#define SPR_ITEM_WORK	(SPR_MG_WORK+8)
#define SPR_TEXT_WAIT	(SPR_ITEM_WORK+8)


#define SPR_HIT_EFEECT	(SPR_TEXT_WAIT+2)
#define SPR_SCRIPT		(SPR_HIT_EFEECT+8)
#define SPR_WORK		(SPR_SCRIPT+32)
#define SPR_DEBUG		(SPR_WORK+32)



#define LAY_MAP_BASE	(0)
#define LAY_MAP_BACK	(LAY_MAP_BASE+2)
#define LAY_MAP_MAIN	(LAY_MAP_BACK+2)
#define LAY_MAP_MOVE	(LAY_MAP_MAIN+1)

#define LAY_MAP_OBJECT	(LAY_MAP_MOVE+2)
#define LAY_MAP_MOVE2	(LAY_MAP_OBJECT+4096)

#define LAY_HP_BAR		(LAY_MAP_MOVE2+2)
#define LAY_EFFECT		(LAY_HP_BAR+32)
#define LAY_MAP_FADE	(LAY_EFFECT+10)
#define LAY_MAP_FRONT	(LAY_MAP_FADE+1)

#define LAY_BACK		(LAY_MAP_FRONT+1)
#define LAY_BACK_FRONT	(LAY_BACK+4)	
#define LAY_CHAR		(LAY_BACK_FRONT+1)
#define LAY_FADE_BACK	(LAY_CHAR+8)
#define LAY_CHAR_FRONT	(LAY_FADE_BACK+1)	

#define LAY_SYSTEM		(LAY_FADE_BACK+1)
#define LAY_MAIN_TEXT	(LAY_SYSTEM+10)

#define LAY_DEBUG		(LAY_MAIN_TEXT+1)


typedef struct{
	long	size;	
	short	w,h;	
	char	mgrid;	
	char	sgrid;	
	char	agrid;	
	short	obj_max;

	char	bak_fname[MAX_PATH];
	char	map_fname[MAX_PATH];
}SMAP_FORMAT_HEADER;

typedef struct{
	char	flag;
	short	x,y;
	short	px,py;
	short	obj_no;
}OBJECT_FORMAT;

typedef struct{
	char	flag;
	short	x,y;
	short	ani_no;
	short	sp_no;
	short	play;
	float	zoom;
	short	rev_param;

	short	param;
	short	rate;

	short	fade_cnt;
	short	fade;

	short	tx,ty;
	short	tcnt;
	short	tmax;
	short	ttype;

	short	layer;

}EVENT_OBJECT;

typedef struct{
	SMAP_FORMAT_HEADER	head;

	BYTE	*back;
	BYTE	*map;
	OBJECT_FORMAT	*object;
	BYTE	*sinkin;
	BYTE	*attr;
	BYTE	*hight;

	int		bw,bh;
	int		stage_no, ver_no;
	EVENT_OBJECT	evt[32];
}SMAP_FORMAT;

typedef struct{
	float		px,py;
	float		zoom;
	int			sc_mode;
						
						

	int			sc_type;

	int			sc_count;
	int			sc_max;

	float		spx,spy,szoom;
	float		epx,epy,ezoom;
	int			ecno;



	int			r,g,b;
	int			fd_type;
	int			fd_count;
	int			fd_max;

	int			sr,sg,sb;
	int			er,eg,eb;

	int			step;
	int			next_step;

	int			battle_flag;

	BYTE		*aria_table;
}BTL_MAP_CONTROL;

extern SMAP_FORMAT			MapData;
extern BTL_MAP_CONTROL		MapControl;

enum{
	BTL_INIT=0,
	BTL_TERM,

	BTL_EVENT,

	BTL_SORTILE,
	BTL_BATTLE,
	BTL_BATTLE_END,
	BTL_MENU,
	BTL_COMMAND,
};


extern int	BTL_ReleaseBattleMap( void );
extern BOOL BTL_LoadBattleMap( int stage_no, int ver_no  );

extern BOOL SetMapObjEx( int ono, char *fname, char *pack, int x, int y, int play, int loop );
extern BOOL SetMapObj( int ono, int ani_no, int sp_no, int x, int y, int play, int loop );
extern BOOL SetMapObjNoLoad( int ono, int ono2, int sp_no, int x, int y, int play, int loop );

extern BOOL SetMapObjRevParam( int ono, int rev );
extern BOOL SetMapObjLayer( int ono, int layer );
extern BOOL SetMapObjMove( int ono, int tx, int ty, int frame, int type );
extern BOOL SetMapObjZoom( int ono, int zoom );
extern BOOL SetMapObjParam( int ono, int param, int rate );
extern BOOL PlayMapObj( int ono, int sp_no, int play, int loop );
extern BOOL ResetMapObj( int ono, int fade );

extern BOOL SetBrightBattleMap( int r, int g, int b, int frame, int type );
extern BOOL SetScrolBattleMap( float mcx, float mcy, float zoom, int frame, int type );
extern BOOL SetScrolBattleMapChar( int no, float zoom, int frame, int type );

extern BOOL WaitScrolBattleMap( int flag );



extern void BTL_SetStartPoint( float px, float py );

enum{
	WIN_NORMAL=0,	
	WIN_BOSS,		
	WIN_POINT,		
	WIN_TIME,		
	WIN_POINT2,		
	WIN_GIGA,			
};
#define WIN_DID_OK	0x100

extern void BTL_SetStartBattle( int btype, int param1, int param2, int all_atack, int range );
extern BOOL BTL_WaitBattleEnd( int *win );



extern int	BTL_SetNextGameStep( int step, char *next_scr );

extern int	BTL_System( int script );




#endif

