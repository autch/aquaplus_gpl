
#ifndef	_GM_AVGBACK_H_
#define _GM_AVGBACK_H_


typedef struct{
	int		flag;		

	int		bno;		

	int		x, y;		
	int		x2, y2;		
	int		x3, y3;		
	int		x4, y4;		
	int		w, h;		
	int		zoom;		
	int		effect;		
	int		cg_flag;	

	int		ns_flag;
	int		ns_rate;
	int		ns_fade;
	int		ns_count;

	int		cscop;		
	int		cs_flag;	
	int		cs_lock;	
	int		cs_type;	
	int		cs_cnt;		
	int		cs_fade;	
	RECT2	cs_clip;	

	int		fd_flag;	
	int		fd_type;	
	int		fd_vague;	
	int		fd_cnt;		
	int		fd_max;		

	int		sc_flag;	
	int		sx,sy;		
	int		sx2,sy2;	
	int		sx3,sy3;	
	int		sx4,sy4;	
	int		sw,sh;		
	int		sc_type;	
	int		sc_cnt;		
	int		sc_max;		

	int		br_flag;	
	int		br_fade;	
	int		br_cnt;		
	int		br_pat;		
	int		br_type;	
	int		r,g,b;		
	int		er,eg,eb;	
	int		rr,gg,bb;	
	int		rx,ry;		

	int		sk_flag;	
	int		sk_dir;		
	int		sk_pich;	
	int		sk_cnt;		
	int		sk_type;	
	int		sk_speed;	
	int		sk_swing;	
	
	int		tone_no2;	
	int		tone_no;	
	int		tone_char;	
	int		tone_back;	

	int		wether;	

	int		sp_flag;

	RECT	draw_rect;

	int		redraw;		

}BACK_STRUCT;
extern BACK_STRUCT		BackStruct;

extern char	*tone_fname[];
extern char	tone_fname2[];

#define TONE_NORMAL		0
#define TONE_EVENING	1
#define TONE_NIGHT		2
#define TONE_NT_ROOM	3

#define TONE_MAX		4

#define TONE_SEPIA		1
#define TONE_NEGA		2
#define TONE_MONO		3
#define TONE_BULE		4
#define TONE_RED		5
#define TONE_GREEN		6
#define TONE_BULE2		7
#define TONE_BROWN		8


#define BAK_DIRECT		-1	
#define BAK_FADE		0	
#define BAK_CFADE		1	
#define BAK_CFADE_UP	2	
#define BAK_CFADE_DO	3	
#define BAK_CFADE_RI	4	
#define BAK_CFADE_LE	5	
#define BAK_CFADE_CE	6	
#define BAK_CFADE_OU	7	

#define BAK_DIA1		8	
#define BAK_DIA2		9	
#define BAK_DIA3		10	

#define BAK_CFZOOM1		11	
#define BAK_CFZOOM2		12	
#define BAK_CFZOOM3		13	
#define BAK_CFZOOM4		14	

#define BAK_KAMI		15	

#define BAK_SLIDE_UP	16	
#define BAK_SLIDE_DO	17	
#define BAK_SLIDE_RI	18	
#define BAK_SLIDE_LE	19	

#define BAK_LASTERIN	20	

#define BAK_NOISE		21	
#define BAK_PIPLE		22

#define BAK_TV			23
#define BAK_ROLL		24

#define BAK_PATTERN		0x80
#define BAK_PTF_REV		0x100
#define BAK_PTF_ACCL1	0x200
#define BAK_PTF_ACCL2	0x400
#define BAK_PTF_REV_W	0x800
#define BAK_PTF_REV_H	0x1000

#define BACK_SY			64
#define CHIPBACK_SY		32

extern void AVG_SetWeatherMode( int weather );

extern void AVG_SetTimeMode( int tone_no, int eff_no );
extern void AVG_SetTimeBack( int tone_no, int eff_no );
extern void AVG_SetTimeChar( int tone_no, int eff_no );

extern void AVG_GetBack( void );
extern void AVG_SetBack_Delete( void );


extern void AVG_SetNoise( int disp, int rate, int fade );
extern int AVG_WaitNoise( void );

extern void AVG_OpenBack( void );
extern void AVG_CloseBack( void );

extern void AVG_ResetBackHalfTone( int bak_no, int chg_type );
extern void AVG_SetBack( int bak_no, int x, int y, int chg_type, int char_disp, int cg_flag, int fd_max, int vague=128 );
extern BOOL AVG_WaitBack( void );

extern void AVG_CopyBack( int sc );

extern void AVG_SetBackFade( int r,int g,int b, int fade );
extern void AVG_SetBackFadePatern( int in_out, int add_sub, int pno, int fade, int rev, int kaiten, int rx, int ry );
extern void AVG_SetBackFadeDirect(int r,int g,int b );
extern BOOL AVG_WaitBackFade( void );

extern void AVG_SetBackPos( int x, int y );
extern void AVG_SetBackPosZoom( int x, int y, int w, int h );
extern void AVG_SetBackScroll( int x, int y, int w, int h, int frame, int type );
extern void AVG_SetBackScrollPoly( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int frame, int type );
extern BOOL AVG_WaitBackScroll(void);

extern void AVG_CreateBackCScope( void );
extern void AVG_SetBackCScope( int flag, int fade );
extern BOOL AVG_WaitBackCScope(void);
extern void AVG_LockBackCScope( int flag );

extern BOOL AVG_SetShake( int type, int pich, int speed, int dir,int swing=256 );
extern BOOL AVG_WaitShake( void );
extern void AVG_StopShake( void );

extern void AVG_ControlBack(void);


extern void AVG_SetRipple( int x, int y, int size, int single=255 );
extern void AVG_SetRippleSet( int angle, int amount, int fade=0 );
extern BOOL AVG_WaitRipple( void );
extern void AVG_ControlRipple( void );

extern void AVG_SetRippleLost( void );
extern BOOL AVG_WaitRippleLost( void );


extern int	Set_month;
extern int	Set_day;

extern void AVG_SkipDate( int month, int day );
extern BOOL AVG_SetCalender( int _month, int _days );
extern BOOL AVG_ViewClock( int clock );

typedef struct{
	char	fname[32];
	long	tone_type;
	char	tfname[32];
	char	pack[8];
	char	flag;
	char	disp;
	char	type;

	DWORD	param;
	DWORD	rev_param;
	short	viv;

	char	bright;
	char	r,g,b;

	char	bmp_bit;
	char	loop;
	char	layer;
	long	nuki;

	short	dx,dy,dw,dh;
	short	sx,sy,sw,sh;
	short	zoom;
	short	roll;
	short	cx,cy;

	int		mh;	
}SPRITE_BMP_STRUCT;

extern SPRITE_BMP_STRUCT		SpriteBmp[MAX_SCRIPT_OBJ];

extern void AVG_LoadBmpSetting( int s_bno, SPRITE_BMP_STRUCT sbs );

extern void AVG_LoadBmp( int bno, int bmp_bit, char *fname, int layer, int nuki, int tone_type, char *tfname, int viv, int bright, char *pack );
extern void AVG_LoadBmpAnime( int s_bno, int bmp_bit, char *fname, int loop, int layer, int nuki );
extern void AVG_ReleaseBmp( int s_bno );
extern BOOL AVG_WaitBmpAnime( int s_bno );


extern void AVG_SetBmpDisp( int s_bno, int disp );
extern void AVG_SetBmpLayer( int s_bno, int layer );
extern void AVG_SetBmpParam( int s_bno, DWORD param );
extern void AVG_SetBmpRevParam( int s_bno, DWORD rev_param );
extern void AVG_SetBmpBright(  int s_bno, int r, int g, int b );
extern void AVG_SetBmpMove( int s_bno, int x, int y );
extern void AVG_SetBmpPos( int s_bno, int dx, int dy, int sx, int sy, int w, int h );
extern void AVG_SetBmpZoom( int s_bno, int dx, int dy, int dw, int dh );
extern void AVG_SetBmpZoom2( int s_bno, int cx, int cy, int zoom );

extern void AVG_SetAvi( int avino, int bno, int w, int h, char *fname );
extern void AVG_ResetAvi( int avino );
extern BOOL AVG_WaitAvi( int avino );






#endif//_GM_AVGBACK_H_