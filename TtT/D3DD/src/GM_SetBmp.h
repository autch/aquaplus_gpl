#ifndef	_GM_SET_BMP_H_
#define _GM_SET_BMP_H_

#define MAX_SCRIPT_OBJ	32


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

	BYTE	r,g,b;
	short	alpha;

	char	bmp_bit;
	int		sp_no;
	char	loop;
	char	layer;
	long	nuki;

	short	dx,dy,dw,dh;
	short	sx,sy,sw,sh;
	float	zoom;
	float	roll;
	short	cx,cy;		

	int		mh;	
}SPRITE_BMP_STRUCT;


extern SPRITE_BMP_STRUCT		SpriteBmp[MAX_SCRIPT_OBJ];

extern void AVG_LoadBmpSetting( int s_bno, SPRITE_BMP_STRUCT sbs );

extern void AVG_LoadBmp( int s_bno, char *fname, int layer, int bit16, int tone_type, char *tfname, int viv, char *pack );
extern void AVG_LoadBmpAnime( int s_bno, char *fname, int layer, char *tfname, int chroma, char *pack );
extern void AVG_SetBmpAvi( int s_bno, char *fname, int layer);
extern BOOL AVG_WaitBmpAvi( int s_bno );

extern void AVG_ReleaseBmp( int s_bno );
extern BOOL AVG_WaitBmpAnime( int s_bno );

extern void AVG_SetBmpAnimePlay( int s_bno, int sp_no, int play, int loop );


extern void AVG_SetBmpDisp( int s_bno, int disp );
extern void AVG_SetBmpLayer( int s_bno, int layer );
extern void AVG_SetBmpParam( int s_bno, int param, int alpha );
extern void AVG_SetBmpRevParam( int s_bno, DWORD rev_param );
extern void AVG_SetBmpBright(  int s_bno, int r, int g, int b );
extern void AVG_SetBmpMove( int s_bno, int x, int y );
extern void AVG_SetBmpPos( int s_bno, int dx, int dy, int sx, int sy, int w, int h );
extern void AVG_SetBmpZoom( int s_bno, int dx, int dy, int dw, int dh );
extern void AVG_SetBmpZoom2( int s_bno, int cx, int cy, int zoom );
extern void AVG_SetBmpRoll( int s_bno, int cx, int cy, int roll );


extern void AVG_SetAviFull( int leaf=0 );
extern int AVG_WaitAviFull( void );


#endif

