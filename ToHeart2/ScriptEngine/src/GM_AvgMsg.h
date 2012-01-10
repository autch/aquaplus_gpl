
#ifndef	_GM_AVGMSG_H_
#define _GM_AVGMSG_H_

enum{
	TONE_NODISP=0,
	TONE_DISP,
	TONE_FADEIN,
	TONE_FADEOUT,
};


#define NLOG_MAX	256
#define NLOG_BUF	1024
#define NLOG_V_MAX	32

enum{
	MSG_NODISP,
	MSG_DISP,
	MSG_SYSTEM,
	MSG_WAIT,
	MSG_STOP,
	MSG_NEXT,
	MSG_UP,
	MSG_DOWN,
	MSG_DRAG,
	MSG_LOG,
	MSG_WAIT2,
};

#define KEYWAIT_SIZE	40

typedef struct{
	char		flag;			
	char		add_flag;			
	char		disp;			
	char		step1;			
	char		step2;			
	short		count;			
	short		kstep;			
	short		max;			
	char		str[NLOG_BUF];	
}NOVEL_MESSEGE;
extern NOVEL_MESSEGE	NovelMessage;

typedef struct{
	int			sno[NLOG_V_MAX];
	short		vno[NLOG_V_MAX];
	short		cno[NLOG_V_MAX];

	int			vstcount[NLOG_V_MAX];
	int			px[NLOG_V_MAX];
	int			py[NLOG_V_MAX];

	int			a_cut[NLOG_V_MAX];

	int			vcount;					
}NOVEL_VOICE;


typedef struct{
	char		buf[NLOG_MAX][NLOG_BUF];	
	char		bflag[NLOG_MAX];			

	NOVEL_VOICE	nv[NLOG_MAX];
	char		vv_mes[NLOG_V_MAX][256];
	int			sno,vno,cno;
	int			a_cut;

	int			bmax;					
	int			bpoint;					
	int			bcount;					
}NOVEL_BUF;
extern NOVEL_BUF	NovelBuf;


typedef struct{
	int			tstep;
	int			tcount;
}HALF_TONE;
extern HALF_TONE	HalfTone;



extern void InitNovelMessage(void);

#define DEF_NAME_L		"‰Í–ì"
#define DEF_NAME_LK		"‚±‚¤‚Ì"
#define DEF_NAME_F		"‹M–¾"
#define DEF_NAME_FK		"‚½‚©‚ ‚«"
#define DEF_NAME_NN		"‚½‚©"
#define DEF_NAME_NNK	"ƒ^ƒJ"


extern char	NameL[13];
extern char	NameLK[13];
extern char	NameF[13];
extern char	NameFK[13];
extern char	NameNN[13];
extern char	NameNNK[13];

extern void ReadRegistryName( void );
extern void WriteRegistryName( void );



extern int	MES_POS_X;
extern int	MES_POS_Y;
extern int	MES_POS_W;
extern int	MES_POS_H;
extern int	MES_PICH_W;
extern int	MES_PICH_H;
extern int	MES_KAGE;


extern BOOL AVG_GetMesCut(void);


extern  int AVG_GetHalfToneFade(void);

extern void AVG_SetHalfTone(void);
extern void AVG_SetHalfToneDirect(void);
extern void AVG_ResetHalfTone(void);
extern void AVG_SetHalfToneStep( int setp );
extern int AVG_CheckHalfToneStep( void );

extern void AVG_ControlHalfTone(void);

extern void SetNovelMessageVoice1( int sno, int vno, int cno, int a_cut);
extern void SetNovelMessageVoice2( void );


extern int	AVG_GetNovelMessageFlag(void);
extern int	AVG_GetNovelMessageConfig(void);

extern void AVG_SetNovelMessage( char *str, int add_flag );
extern void AVG_AddNovelMessage( char *str, int cr );

extern void AVG_SetNovelMessageDisp( int disp );
extern BOOL AVG_WaitNovelMessage( void );
extern void AVG_ControlNovelMessage( void );
extern void AVG_SetSaveDataNovelMessage( AVG_SAVE_DATA *sdata );
extern void AVG_SetLoadDataNovelMessage( AVG_SAVE_DATA *sdata );


extern void AVG_SetWindow( int direct );
extern void AVG_ResetWindow( int direct );
extern BOOL AVG_WaitWindow( void );

extern void AVG_OpenWindow( int tdisp, int flag );
extern void AVG_CloseWindow( int flag );
extern BOOL AVG_GetWindowCond( void );

extern BOOL AVG_GetMessageFlag( void );

extern void AVG_SetTextFormat( int txt_no, int font, int x, int y, int w, int h, int pichw, int pichh, int kage, int alph, int wait );
extern void AVG_SetTextSync( int txt_no1, int txt_no2 );
extern void AVG_SetText( int txt_no, char *str );
extern void AVG_ResetText( int txt_no );
extern void AVG_ResetTextAll( void );
extern BOOL AVG_WaitText( int txt_no, int flag );
extern void AVG_ControlText( void );

extern void AVG_SetMessage( int char_no, char *str, int type, int add_flag, int dy, int font );
extern void AVG_AddMessage( int char_no, char *str, int cr, int add_flag );
extern void AVG_ResetMessage( void );
extern void AVG_SetMessageWait( int wait, int wait_op );

extern BOOL AVG_WaitKeyMessage( void );
extern BOOL AVG_WaitMessage( void );
extern BOOL AVG_WaitKey( void );

extern BOOL AVG_SetLogMessage( int bcount );

extern void AVG_ControlMessageWindow(void);

extern BOOL AVG_ConfigCheckMessage(void);
extern void AVG_SetSaveDataMessage( AVG_SAVE_DATA *sdata );



extern BOOL AVG_GetSelectMessageFlag( void );

extern void AVG_SetSelectMessage( char *mes, int game_flag_no, int game_flag_value, int mes_wait );
extern void AVG_SetSelectMessageEx( char *mes, char *sno, int game_flag_no, int game_flag_value );
extern void AVG_SetSelectWindow( void );
extern void AVG_ResetSelectWindow( void );

extern void AVG_OpenSelectWindow( void );
extern void AVG_CloseSelectWindow( void );

extern int  AVG_WaitSelect( void );
extern char *AVG_WaitSelectEx( void );
extern void AVG_ControlSelectWindow( void );

extern BOOL AVG_ConfigCheckSelect(void);
extern void AVG_SetSaveDataSelect( AVG_SAVE_DATA *sdata );


extern void AVG_LoadWindow( void );
extern void AVG_InitWindow( void );
extern void AVG_TermWindow( void );

#endif