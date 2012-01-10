
#ifndef	_GM_AVGCHIP_H_
#define _GM_AVGCHIP_H_

typedef struct{
	int		flag;
	int		bak_no;
	float	wx,wy;
	float	ww,wh;

	int		scroll;	
	float	px,py;	
	float	sx,sy;	
	float	tx,ty;	
	int		sc_cnt;	
	int		sc_max;	
}CHIP_BACK_STRUCT;
extern CHIP_BACK_STRUCT	ChipBackStruct;


extern void AVG_SetTitle( void );
extern BOOL AVG_WaitTitle( void );

extern void AVG_PoseChipBack( void );
extern void AVG_PlayChipBack( void );

extern void AVG_ResetBack(int bmp_init);

extern void AVG_ShowChipBack( int chg_type, int frame );

extern void AVG_ControlChipBackChange(void);

extern void AVG_SetChipBack( int bak_no, int wx, int wy, int ww, int wh );
extern BOOL AVG_SetChipBackParts( int bak_ch, int dx, int dy, int w, int h, int lw, int ln, int depth, int nuki, int lno, int cfade, DWORD param );
extern void AVG_SetChipBackScrool( int tx, int ty, int frame, int flag );
extern void AVG_SetChipBackScrool2( int tx, int ty, int frame, int flag );
extern void AVG_SetChipBackScroolSpeed( int sx, int sy, int frame );
extern BOOL AVG_WaitChipBackScrool(void);
extern BOOL AVG_WaitChipBackScroolSpeed(void);
extern void AVG_ControlChipBack( void );
extern void AVG_ControlChipChar( void );

extern BOOL AVG_LoadChipCharCash( int char_no, int pose );
extern void AVG_ResetChipCharCash( void );

extern BOOL AVG_SetChipChar( int char_no, int pose, int dx, int dy, int end, int lnum, int layer );
extern BOOL AVG_LoadChipChar( int char_no, int pose, int end, int lnum );
extern void AVG_ResetChipChar( int ch_no );
extern void AVG_SetChipCharPos( int ch_no, int dx, int dy );
extern void AVG_SetChipCharRev( int char_no, int rev );
extern void AVG_SetChipCharParam( int char_no, DWORD param );
extern void AVG_SetChipCharBright( int char_no, int r, int g, int b );
extern void AVG_SetChipCharMove( int ch_no, int mx, int my, int frame );
extern void AVG_SetChipCharMove2( int char_no, int mx, int my, int frame );
extern void AVG_SetChipCharMoveSpeed( int char_no, int mx, int my, int frame );
extern void AVG_GetChipCharMove( int char_no, long *tx, long *ty );
extern void AVG_GetChipCharMoveSpeed( int char_no, long *mx, long *my );
extern void AVG_CopyChipCharPos( int char_no1, int char_no2 );

extern void AVG_ThroughChipCharAni( int char_no );
extern void AVG_ThroughChipCharAniLoop( int char_no );

extern void AVG_SetSpriteRepeatFind( int char_no );
extern BOOL AVG_WaitChipCharRepeat( int char_no );

extern BOOL AVG_WaitChipCharAni( int char_no );
extern BOOL AVG_WaitChipCharMove( int char_no );

extern void AVG_SetSoundEventVolume( int char_no, int event, int sno, int vol );
extern void AVG_SetSoundEvent( int char_no, int event, int sno1, int sno2, int sno3, int sno4, int flag );
extern BOOL AVG_SetPotaPota( int blood, int amount, int frame );

#endif