#ifndef	_GM_AVG_H_
#define _GM_AVG_H_

typedef struct{
	long		frame;			
	float		wait;			

	long		wheel;			
	long		msg_cut_optin;	

	float		msg_wait;		
	long		msg_cut;		
	long		msg_cut_mode;	

	long		bgm;			
	long		se;				
	long		voice;			

	long		bgm_vol;		
	long		se_vol;			
	long		voice_vol;		

	long		demo;			
	long		demo_cnt;		
	long		demo_max;		

	long		omake;			

	long		mov_lv;			
	long		mov1;			
	long		mov2;
	long		mov3;

	long		ero_voice;
	long		menu;
	long		auto_modo;
	long		_Radeon8500;
}AVG_STRUCT;
extern AVG_STRUCT	Avg;

#define MES_CUT				0.0f
#define MES_FAST			0.5f
#define MES_NORMAL			1.0f
#define MES_SLOW			2.0f


typedef struct{
	int		flag;		

	int		bno;		
	int		grp;

	int		x, y;		
	int		x2, y2;		
	int		x3, y3;		
	int		x4, y4;		
	int		w, h;		
	int		zoom;		
	int		effect;		
	int		cg_flag;	
	int		char_disp;	

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
	
	int		time_no;	

	int		chroma_back;
	int		chroma_char;

	int		sp_flag;

	RECT	draw_rect;

	int		redraw;		

}BACK_STRUCT;
extern BACK_STRUCT		BackStruct;

extern char	*time_tone_fname[];
extern char	bak_tone_fname[32];
extern char	eff_tone_fname_b[32];
extern char	eff_tone_fname_c[32];

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

#define BAK_CFADE		1	












#define BAK_CFZOOM1		11	
#define BAK_CFZOOM2		12	
#define BAK_CFZOOM3		13	
#define BAK_CFZOOM4		14	



#define BAK_SLIDE_UP	16	
#define BAK_SLIDE_DO	17	
#define BAK_SLIDE_RI	18	
#define BAK_SLIDE_LE	19	







#define BAK_ROLL		24

#define BAK_PATTERN		0x0080
#define BAK_PTF_REV		0x0100
#define BAK_PTF_REV_W	0x0800
#define BAK_PTF_REV_H	0x1000

extern BOOL AVG_SetDemoFlag( int flag, int count );

extern BOOL AVG_GetHitKey(void);
extern BOOL AVG_GetMesCut(void);
extern BOOL AVG_WaitKey(void);

extern int AVG_EffCnt( int cnt );
extern int AVG_EffCnt2( int cnt );
extern int AVG_EffCnt3( int cnt );
extern int AVG_EffCnt4( int cnt );
extern int AVG_EffCntPuls( void );
extern int AVG_MsgCnt( void );


extern void AVG_SetWaitFrame( int type, int wait );
extern BOOL AVG_WaitFrame( void );

extern void SND_Select( void );
extern void SND_Ok( void );
extern void SND_Cansel( void );
extern void SND_Ng( void );


extern void AVG_SetTimeMode( int time_no );
extern void AVG_SetChromaMode( int chroma_no, int cb );
extern void AVG_SetEffctMode( char *tone_fname, int cb );

extern void AVG_ResetBack( int chg_type, int char_disp, int fd_max, int vague );
extern void AVG_SetBack(int bak_no, int chg_type, int char_disp, int cg_flag, int fd_max=-1, int vague=128, int x=0, int y=0, int w=800, int h=600 );
extern void AVG_ControlBack(void);
extern BOOL AVG_WaitBack(void);


extern int		ShakeDx, ShakeDy;
extern float	ShakeZoom;
extern float	ShakeRoll;

#define	SHAKE_SIN			0
#define	SHAKE_2TI			1
#define	SHAKE_ZOOM			2
#define	SHAKE_RAND			3
#define	SHAKE_ROLL			4
#define	SHAKE_ROLL_SIN		5
#define	SHAKE_ROLL_2TI		6


extern BOOL AVG_SetShake( int type, int pich, int speed, int dir, int swing );
extern void AVG_StopShake( void );
extern BOOL AVG_WaitShake( void );
extern void AVG_ControlShake( void );



#define CHAR_COND_NOMAL		0	
#define CHAR_COND_IN		1	
#define CHAR_COND_OUT		2	
#define CHAR_COND_CHANGE	3	
#define CHAR_COND_WAIT		7	

#define CHAR_TYPE_CFADE		0	
#define CHAR_TYPE_MOVE_R	1	
#define CHAR_TYPE_MOVE_L	2	

#define CHAR_TYPE_NOWAIT	0x100	


extern void AVG_SetChar( int char_no, int pose, int locate, int layer, int in_type, int r, int g, int b, int alph, int frame );
extern void AVG_ResetChar( int char_no, int out_type, int frame );
extern void AVG_ResetCharBack( int out_type, int frame );

extern void AVG_SetCharPose( int char_no, int pose, int frame );
extern void AVG_SetCharLocate( int char_no, int locate, int frame );
extern void AVG_SetCharLayer( int char_no, int layer );
extern void AVG_SetCharBright( int char_no, int r, int g, int b, int frame );
extern void AVG_SetCharAlph( int char_no, int alph, int frame );


extern void AVG_ControlChar( void );
extern BOOL AVG_WaitChar( int char_no );

extern int AVG_GetNameNo( void );
extern BOOL AVG_SetNameWindowFree( char *str );
extern BOOL AVG_SetNameWindow( char *str, int center=FALSE );

extern void AVG_StartMessage(void);

extern BOOL AVG_SetMessageWindow( int cno, int type, int frame );
extern BOOL AVG_ResetMessageWindow(  int type, int frame );

extern void AVG_SetMessage( char *str, int type );
extern void AVG_AddMessage( char *str, int type );
extern void AVG_ResetMessage( void );

extern BOOL AVG_WaitMessage( void );
extern void AVG_ControlMessage( void );


extern void AVG_SetSelectMessage( char *mes, int game_flag_no, int game_flag_value );
extern void AVG_SetSelectWindow( void );
extern int AVG_WaitSelect( int nanido );
extern void AVG_ControlSelectWindow( void );

extern void AVG_SetScenario( char *scenario_name );
extern void AVG_SetScenarioFlag( int mes_no );
extern void AVG_SavetScenarioFlag( void );

extern BOOL AVG_CheckScenarioFlag( void );
extern void AVG_LoadScenarioFlag( void );

#define VISUAL_MAX	10000
extern BYTE	SaveVisualFlag[2][VISUAL_MAX][10];
extern void AVG_SaveVisual( int vh, int no );
extern void AVG_LoadVisual( void );

extern void AVG_SetGameFlag( int fno, int flag );
extern void AVG_LoadGameFlag( void );

extern void AVG_InitSound( void );
extern void AVG_TermSound( void );

extern void AVG_SetVI_EventVoiceNo( int vi_ee );

extern void AVG_SetVoiveFlag( int flag );
extern void AVG_StopVoice( int vc_no );
extern void AVG_FadeOutVoice( int vc_no, int fade );
extern int AVG_WaitVoice( int vc_no );
extern void AVG_FadeOutVoiceAll( int fade );
extern void AVG_SetDirectVoumeVoiceAll( int voice_vol );
extern void AVG_PlayVoice( int vc_no, int cno, int sno, int vno, int vol, int loop );
extern void AVG_PlayOmakeVoice( int cno );

extern void AVG_PlayBTVoice( int cno, int bsno );

extern void AVG_ReleaseSeAll( void );
extern void AVG_FadeSeAll( int fade );
extern void AVG_PlaySE3( int se_no, int vol=255 );
extern int AVG_PlaySE( int se_no, int loop, int vol, int direct_vol=FALSE );
extern void AVG_PlaySE2( int sno, int se_no, int fade, int loop, int vol );
extern void AVG_StopSE( int dno );
extern void AVG_StopSE2( int sno, int fade );
extern void AVG_SetVolumeSE( int sno, int vol, int fade );
extern void AVG_SetDirectVoumeSeAll( int se_vol );
extern int AVG_WaitSe( int sno );
extern int AVG_WaitFadeSe( int sno );
extern void AVG_SetSEFlag( int flag );


extern void AVG_SetDirectVoumeMusicAll( int bgm_vol );

extern int AVG_GetBGMEno( void );

extern void AVG_SetBGMLoad( int mus_no );
extern void AVG_PlayBGMLoad( void );

extern void AVG_PlayBGM( int mus_no, int fade, int loop, int vol, int change );
extern int AVG_PauseBGM( int flag );
extern int AVG_StopBGM( int fade );
extern BOOL AVG_WaitLoopBGM( void );
extern BOOL AVG_WaitBGM( void );
extern int AVG_GetBarBGM( int ste );
extern void AVG_SetBGMFlag( int flag );
extern void AVG_SetVolumeBGM( int vol, int fade );

extern void AVG_ControlMusic( void );



extern void AVG_SetWeather( int wno, int wind, int speed, int amount,int layer );
extern void AVG_ResetWeather( int direct=0 );
extern void AVG_SetWeatherWind( int wind );
extern void AVG_SetWeatherSpeed( int speed );
extern void AVG_SetWeatherAmount( int amount );
extern void AVG_SetWeatherLayer( int layer );
extern BOOL AVG_WaitWeather( void );
extern void AVG_ControlWeather( void );


#endif

