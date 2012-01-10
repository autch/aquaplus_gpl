
#ifndef	AVG_H_
#define AVG_H_


#define WAVE_BUFFER_NUM			16		
#define WAVE_STREAM_NUM			4										
#define WAVE_SOUND_NUM			(WAVE_BUFFER_NUM-WAVE_STREAM_NUM)		

#define SELECT_MAX				(8)
#define SELECT_MES_MAX			(256)
#define SELECT_SNO_MAX			(8)



#define END_STOP		0
#define END_LOOP		1
#define END_RELEASE		-1



#define VOICE_MAX		8
#define VOICE_CHAR_MAX	100


#define MAX_CHAR	8

#define MAX_SCRIPT_OBJ	32

#include "GM_AvgBack.h"



#define SRR_CURSOR		0						



#define BMP_BACK		0						
#define BMP_BACK2		(BMP_BACK+8)			
#define BMP_BACKHALF	(BMP_BACK2+1)			
#define BMP_SPBACK		(BMP_BACKHALF+1)
#define BMP_SPBACKHALF	(BMP_SPBACK+1)

#define BMP_CHIP		(BMP_SPBACKHALF+1)		
#define BMP_CHAR		(BMP_CHIP+8)			
#define BMP_CHARHALF	(BMP_CHAR+MAX_CHAR*2)			
#define BMP_SYSTEM		(BMP_CHARHALF+MAX_CHAR*2)		
#define BMP_SELECT		(BMP_SYSTEM+10)			
#define BMP_SAVE		(BMP_SELECT+4)			
#define BMP_WEATHER		(BMP_SAVE+10)			
#define BMP_WORK		(BMP_WEATHER+10)		
#define BMP_SCRIPT		(BMP_WORK+20)			
#define BMP_ENDING		(BMP_SCRIPT+MAX_SCRIPT_OBJ)			
#define BMP_WINDOW		(BMP_ENDING+40)			
#define BMP_HISTORY		(BMP_WINDOW+4)			
#define BMP_KEYWAIT		(BMP_HISTORY+4)			
#define BMP_CAP			(BMP_KEYWAIT+4)			
#define BMP_MAP			(BMP_CAP+3)			
#define BMP_DISP		(BMP_MAP+32)			

#define GRP_BACK_CHIP	50					
#define GRP_CHAR_CHIP	8					

#define GRP_BACK2	0						
#define GRP_BACK	(GRP_BACK2+1)			
#define GRP_SPBACK	(GRP_BACK+GRP_BACK_CHIP)
#define GRP_SPBACKHALF	(GRP_SPBACK+1+MAX_CHAR)

#define GRP_CHIP	(GRP_SPBACKHALF+MAX_CHAR)		
#define GRP_CHAR	(GRP_CHIP+GRP_CHAR_CHIP)
#define GRP_WINDOW	(GRP_CHAR+MAX_CHAR)			
#define GRP_HISTORY	(GRP_WINDOW+10)			
#define GRP_KEYWAIT	(GRP_HISTORY+15)		
#define GRP_SYSTEM	(GRP_KEYWAIT+4)			
#define GRP_SYSTEM2	(GRP_SYSTEM+20)			
#define GRP_SELECT	(GRP_SYSTEM2+10)		
#define GRP_SAVE	(GRP_SELECT+10)			
#define GRP_WEATHER	(GRP_SAVE+70)			
#define GRP_WORK	(GRP_WEATHER+200)		
#define GRP_SCRIPT	(GRP_WORK+100)			
#define GRP_ENDING	(GRP_SCRIPT+MAX_SCRIPT_OBJ)			
#define GRP_MAP		(GRP_ENDING+20)			
#define GRP_DISP	(GRP_MAP+40)			

#define TXT_BACK	0						
#define TXT_WINDOW	(TXT_BACK+4)			
#define TXT_SYSTEM	(TXT_WINDOW+10)			
#define TXT_SELECT	(TXT_SYSTEM+10)			
#define TXT_SAVE	(TXT_SELECT+10)			
#define TXT_SCRIPT	(TXT_SAVE+40)			
#define TXT_WORK	(TXT_SCRIPT+10)			
#define TXT_MAP		(TXT_WORK+1)
#define TXT_DISP	(TXT_MAP+20)			

#define LAY_BACK	1						
#define LAY_SPBACK	(LAY_BACK+4)
#define LAY_CHIP	(LAY_SPBACK+4)			
#define LAY_FORE	(LAY_CHIP+5)			
#define LAY_CHAR	(LAY_FORE+4)			
#define LAY_WEATHER	(LAY_CHAR+4)			
#define LAY_WINDOW	(LAY_WEATHER+4)			
#define LAY_MAP		(LAY_WINDOW+10)			
#define LAY_SYSTEM	(LAY_MAP+10)			
#define LAY_DISP	(LAY_SYSTEM+10)			

#define DOS_FONT	20	
#define SYS_FONT	34
#define RUBI_FONT	16	
#define CHIP_FONT	24	

#define MAP_EVENT_MAX	16
typedef struct{
	int	chr_no;	
	int	pos;	
	int	type;	
	char	script_fname[32];
}MAP_EVENT;

extern MAP_EVENT	MapEvent[MAP_EVENT_MAX];
extern int	MapMax;
extern int	MapStep;


typedef struct{
	int		sk_flag;
	int		sk_dir;
	int		sk_pich;
	int		sk_type;
	
	int		bs_bno;			
	int		bs_x, bs_y;		
	int		bs_w, bs_h;		
	int		bs_cg;			
	int		bs_effect;		

	int		bs_flag;		
	int		bs_rate;		
	int		bs_fade;		

	int		bs_zoom;		
	int		bs_cscop;		
	int		bs_tone;		
	int		bs_tone2;		
	int		bs_tone_ch;		
	int		bs_tone_bk;		
	int		bs_wether;
	
	int		cs_flag[MAX_CHAR];		
	int		cs_cno[MAX_CHAR];		
	int		cs_pose[MAX_CHAR];		
	int		cs_loc1[MAX_CHAR];		
	int		cs_layer[MAX_CHAR];		
	int		cs_fade[MAX_CHAR];		
	int		cs_alph[MAX_CHAR];		
	
	char	ms_flag;			
	char	ms_wflag;			
	char	ms_cno;				
	short	ms_wait;			
	short	ms_wait_op;			

	char	ms_add;			
	char	ms_disp;		
	char	ms_step1;		

	short	ms_count;		
	short	ms_kstep;		
	short	ms_max;			
	
	char	ms_str[1024];		

	
	int		sw_flag;			
	int		sw_ex;				
	int		sw_mnum;			
	char	sw_mes[SELECT_MAX][SELECT_MES_MAX];		
	char	sw_sno[SELECT_MAX][SELECT_SNO_MAX];		
	char	sw_no[SELECT_MAX];			
	char	sw_value[SELECT_MAX];		

	
	int		ws_flag;		
	int		ws_wno;			
	int		ws_twind;		
	int		ws_tspeed;		
	int		ws_tamount;		
	
	int		we_flag;		
	int		we_type;		
	int		we_ewav;		
	
	int		pm_mus_no[WAVE_STREAM_NUM];	
	int		pm_mode[WAVE_STREAM_NUM];		
	int		pm_loop[WAVE_STREAM_NUM];		
	int		pm_next_vol[WAVE_STREAM_NUM];	
	
	int		ss_flag[WAVE_SOUND_NUM];	
	int		ss_se_no[WAVE_SOUND_NUM];	
	int		ss_loop[WAVE_SOUND_NUM];	
	int		ss_vol[WAVE_SOUND_NUM];		

	short	sv_cv[VOICE_CHAR_MAX];
	short	sv_sv;

	int		bs_r,bs_g,bs_b;	
	long	VI_EventVoiceNo;
	long	VI_EventVoiceNoAll;

	MAP_EVENT	MapEvent[MAP_EVENT_MAX];
	int			MapMax;
	int			MapStep;

	
	SPRITE_BMP_STRUCT	spright_bmp[MAX_SCRIPT_OBJ];
	int		Set_month;
	int		Set_day;

	char	NameL[13];
	char	NameLK[13];
	char	NameF[13];
	char	NameFK[13];
	char	NameNN[13];
	char	NameNNK[13];

}AVG_SAVE_DATA;


typedef struct{
	long		frame;			
	long		wait;			
	long		level;			
	long		ami;			

	long		interless;			

	long		win_eff;		
	long		win_bld;		
	long		win_prm;		

	long		wheel;			
	long		msg_cut_optin;	

	long		msg_page;		
	long		msg_wait;		
	long		msg_cut;		
	long		msg_cut_mode;	
	long		msg_font;		
	long		half_tone;		

	long		sound;			
	long		bgm;			
	long		cd_fade;		
	long		se;				
	long		se_buf;			
	long		pcm_fmt;		
								
	long		voice;			

	long		bgm_vol;		
	long		se_vol;			
	long		voice_vol;		

	long		demo;			
	long		demo_cnt;		
	long		demo_max;		

	long		omake;

	long		side_option;

	long		mov_lv;
	long		mov1;
	long		mov2;
	long		mov3;

	long		tx_pdwon;
	long		tx_lbtrg;

	int			auto_flag;
	int			auto_key;
	int			auto_page;
}AVG_STRUCT;
extern AVG_STRUCT	Avg;

#define CHAR_VOICE_MAX	11
extern int	CharVoiceFlag[CHAR_VOICE_MAX];

extern long	DefaultCharName;

extern int	ConfigOpenMode;

extern void AVG_SetDemoFlag( int flag, int demo_count );

extern void AVG_ReadConfigParam( void );
extern void AVG_WriteConfigParam( void );

#define BGM_CDDA	0
#define BGM_PCM		1

#define PCM_44_16	0
#define PCM_22_16	1
#define PCM_11_16	2
#define PCM_44_08	3
#define PCM_22_08	4
#define PCM_11_08	5

#define DRAW_LOWEND			0
#define DRAW_HIGHEND		1

#define WIN_NOBLEND			0
#define WIN_BLEND			1
#define WIN_AMI				2

#define MES_CUT				0
#define MES_FAST			1
#define MES_NORMAL			2
#define MES_SLOW			3

#define FONT_NORMAL			0
#define FONT_GOTHIC			1
#define FONT_MINTYOU		2
#define FONT_KOIN			3

#define DEMO_NOT			0
#define DEMO_MODE			1

#define __RESERV		0



typedef struct{
	int		click;		
	int		cansel;		
	int		space;		

	int		mes_cut;	
	int		mes_cut_mode;	
	int		diswin;		
	
	int		u,d,l,r;	

	int		end;	
	int		home;	
	int		pup;	
	int		pdown;	

	int		num[10];	
}GAME_KEY;
extern GAME_KEY	GameKey;

extern void AVG_GetGameKey(void);

extern BOOL AVG_GetHitKey( void );

extern int AVG_EffCnt( int cnt );	
extern int AVG_EffCnt2( int cnt );	
extern int AVG_EffCnt3( int cnt );	
extern int AVG_EffCnt4( int cnt );	

extern int AVG_EffCntPuls( void );

extern int AVG_MsgCnt( void );		

extern void AVG_SetWaitFrame( int type, int wait );
extern BOOL AVG_WaitFrame( void );

extern int AVG_GetWeatherNoReset( void );
extern void AVG_SetWeather( int wno, int wind, int speed, int amount, int no_reset=0 );
extern void AVG_ResetWeather();
extern void AVG_SetWeatherWind( int wind );
extern void AVG_SetWeatherSpeed( int speed );
extern void AVG_SetWeatherAmount( int amount );
extern BOOL AVG_WaitWeather( void );
extern void AVG_ControlWeather( void );

extern void AVG_SetLensFrea( int light, int rev, int count );

extern BOOL AVG_GetWavEffectFlag( void );


extern void AVG_SetWarp( int type, int level, int frame, int loop, int dir, int cx, int cy, int blend );
extern void AVG_StopWarp( int loop );
extern BOOL AVG_WaitWarp( void );



extern void AVG_SetWavEffect( int wav, int frame, int wave_no );
extern void AVG_ResetWavEffect( void );
extern BOOL AVG_WaitWavEffect( void );

extern int	PlayMusicNo;
extern int	PlayMusicVol;
extern int	PlayMusicLoop;

extern void AVG_InitSound( void );
extern void AVG_TermSound( void );


extern void AVG_SetVoiveFlag( int flag );
extern void AVG_StopVoice( int vc_no );
extern void AVG_FadeOutVoice( int vc_no, int fade );
extern void AVG_SetVolumeVoiceChar( int cno, int vol );
extern void AVG_SetVolumeVoiceScript( int vol );

extern void AVG_FadeOutVoiceAll( int fade );
extern void AVG_SetDirectVoumeVoiceAll( int voice_vol );


extern void AVG_SizukuPlayVoice( int vc_no, int vno, int char_no, int ex_no, int vol, int loop, int goin=FALSE );
extern void AVG_SetVI_EventVoiceNo( int vi_ee, int all );
extern void AVG_PlayVoice( int vc_no, int cno, int sno, int vno, int vol, int loop, int a_cut=0, int test=0 );
extern int AVG_WaitVoice( int vc_no );

extern void AVG_PlayMusic( int pno, int mus_no, int fade, int loop, int vol, int change );
extern void AVG_StopMusic( int pno, int fade );
extern void AVG_PauseMusic( int pno, int flag );
extern void AVG_SetVoumeMusic( int pno, int vol, int fade );

extern void AVG_SetDirectVoumeMusicAll( int bgm_vol );


extern void AVG_PlayBGM( int mus_no, int fade, int loop, int vol, int change  );
extern void AVG_PlayBGMEx( int mus_no, int fade, int loop, int vol, int change, int goin );
extern int AVG_StopBGM( int fade );
extern int AVG_PauseBGM( int flag );

extern BOOL AVG_WaitLoopBGM( void );
extern BOOL AVG_WaitBGM( void );
extern int AVG_GetBarBGM( int ste );

extern void AVG_SetBGMFlag( int flag );
extern void AVG_ChangeDeviceBGM( int device );
extern void AVG_SetVolumeBGM( int vol, int fade );
extern void AVG_ReleaseSeAll( void );
extern void AVG_FadeSeAll( int fade );

extern int  AVG_PlaySePan( int se_no, int loop, int vol, int lr );
extern int  AVG_PlaySE( int se_no, int loop, int vol, int direct_vol=FALSE );
extern void AVG_PlaySE2( int sno, int se_no, int fade, int loop, int vol );
extern void AVG_PlaySE3( int se_no, int vol );
extern void AVG_StopSE( int dno );
extern void AVG_StopSE2( int sno, int fade );
extern void AVG_SetVolumeSE( int sno, int volume, int fade );
extern void AVG_SetDirectVoumeSeAll( int se_vol );

extern int  AVG_WaitSe( int sno );
extern int  AVG_WaitFadeSe( int sno );
extern void AVG_SetSEFlag( int flag );


extern void AVG_SetCorsol( int index, int x, int y, int w, int h );
extern void AVG_ResetCorsol( int index );

extern void AVG_GetBright( int *r, int *g, int *b );
extern void AVG_SetBright( int r, int g, int b );
extern void AVG_SetFade( int r, int g, int b, int disp, int fade );
extern void AVG_SetFlash( int r, int g, int b, int fade1, int fade2 );
extern BOOL AVG_WaitFade( void );
extern void AVG_ColtrolFade( void );

extern void AVG_ControlMusic(void);
extern void AVG_ControlLoad(void);



typedef struct{
	int		flag;	
	int		mode;
	int		cnt;	
	int		mouse;
	int		open_mode;

	int		next_mode;
	int		next_cnt;
}CONFIG_STRUCT;
extern CONFIG_STRUCT	Config;

extern BOOL AVG_GetConfigFlag( void );
extern void AVG_ResetConfigWindow( void );


extern void AVG_InitIndicator( int gno, int bno );
extern void AVG_TermIndicator( int gno, int bno );
extern void AVG_RenewIndicator( int disp, int gno, int lay, int fade=256 );


extern void AVG_SetSave( int save_no );
extern void AVG_SetLoad( int save_no );
extern void AVG_SetSaveData( AVG_SAVE_DATA *sdata );
extern void AVG_SetLoadData( AVG_SAVE_DATA sdata );

extern int	GotoTitle;
extern BOOL AVG_CheckGotoTitle( void );
extern void AVG_SetGotoTitle( void );
extern void AVG_WaitGotoTitle( void );

extern void AVG_OpenNormalDisp( void );
extern void AVG_CloseNormalDisp( void );


extern void AVG_SetConfigWindow( int open_mode );
extern void AVG_ResetConfigWindow( void );
extern int AVG_ControlConfigWindow( void );


extern void AVG_Init(void);
extern void AVG_Term(void);

extern BOOL AVG_ConfigCheck(void);

extern int AVG_GetLaodFlag(void);

extern void AVG_EndConfig( void );
extern void AVG_GoConfig( int mode );

extern void AVG_SetOpening( void );
extern void AVG_SetEndingMov( int end_no );

extern void AVG_SetMovie( int mode, int no );
extern BOOL AVG_KeySkip( void );
extern BOOL AVG_WaitMovie( void );


extern void AVG_SetSakura( int a, int b, int c, int scr );
extern void AVG_StopSakura( void );

extern void AVG_SetMapEvent( int chr_no, int pos, int type, char *filename );
extern int AVG_ChexkMapEventConfig( void );
extern void AVG_CloseMapEvent( void );

extern int	AVG_System( int script );



#include "GM_AvgBack.h"
#include "GM_AvgChip.h"
#include "GM_AvgChar.h"
#include "GM_AvgMsg.h"
#include "GM_AvgEnding.h"

#define CG_MAX			100000
#define REPLAY_MAX		16

extern int DateNo;
extern long	VI_EventVoiceNo;

extern void AVG_SetScenarioNo( int scenario_no );
extern void AVG_SetScenarioFlag( int block_no );
extern BOOL AVG_CheckScenarioFlag( void );
extern void AVG_LoadScenarioFlag( void );

extern void AVG_SetCgFlag( int vh, int cg_no );
extern BOOL AVG_GetCgFlag( int vh, int cg_no );
extern void AVG_LoadCgFlag( void );

extern void AVG_SetReplayFlag( int rp_no );
extern BOOL AVG_GetReplayFlag( int rp_no );
extern void AVG_LoadReplayFlag( void );

extern void AVG_SetGameFlag( int no, int flag );
extern BOOL AVG_GetGameFlag( int no );
extern void AVG_LoadGameFlag( void );

#endif
