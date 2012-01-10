#ifndef	_MY_SAVEWND

#define MAX_CHAR	4
#define WAVE_BUFFER_NUM			16
#define WAVE_STREAM_NUM			4
#define WAVE_SOUND_NUM			(WAVE_BUFFER_NUM-WAVE_STREAM_NUM)

#include "escript.h"

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
	char	sw_mes[8][28*2*4];
	char	sw_sno[8][6];		
	char	sw_no[8];			
	char	sw_value[8];		

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
	int		bs_r,bs_g,bs_b;
}AVG_SAVE_DATA;

typedef struct{
	int			ver_no;		
	int			sin_no;		
	SYSTEMTIME	sys_time;	

	char		message[32];
}SAVE_HEADER;

struct SAVE_STRUCT{
	SAVE_HEADER		save_head;

	EXEC_DATA 		EXEC_LangInfo;					
	char			NowLangFileName[16];			
	int				RunLangBlockNo;
	int				PrevRunLangBlockNo;

	int				ESC_FlagBuf[ ESC_FLAG_MAX ];
	AVG_SAVE_DATA	sdata;

	SAVE_STRUCT(){
		ZeroMemory(this,sizeof(SAVE_STRUCT));
	}
};

struct MySaveHead
{
	char	head[4];
	WORD	ver;
};

class SaveWnd
{
	float		alpha;
	BYTE		state;
	BYTE		selectNum;
	BYTE		offsetNum;
	BYTE		new_time;
	BYTE		useTable[50];
	time_t		file_time[50];
	exec_mode	mode;
	DWORD		startTime;
	ColorBuf	baseWnd;
	ColorBuf	panel;
	ColorBuf	cursor;
	ColorBuf	button;
	ColorBuf	numParts;
	ColorBuf	numPartsSm;
	ColorBuf	typeParts;
	ColorBuf	wallWnd;
	ColorBuf	backWnd;
	ColorBuf	infoWnd;
	ColorBuf	overWriteWnd;
	ColorBuf	overWriteBtn;
	MyButton	btn[3];
	MyButton	ov_btn[2];
public:
	ColorBuf	saveBmp;
	exec_mode	bak_mode;

	SaveWnd(exec_mode s_mode);
	~SaveWnd(){
		Release();
	}
	void Release(){
		baseWnd.Release();
		panel.Release();
		cursor.Release();
		button.Release();
		numParts.Release();
		numPartsSm.Release();
		typeParts.Release();
		wallWnd.Release();
		backWnd.Release();
		infoWnd.Release();
		overWriteWnd.Release();
		EnableMenuItem( sysInf.hMenu,ID_LOADEXEC,   MF_ENABLED  );
		EnableMenuItem( sysInf.hMenu,ID_SAVEEXEC,   MF_ENABLED  );
	}
	void CreateInfo(void);
	void Blt(void);
	void timeout(int x,int y,int type,char *tbuf,int num);
	void TextOutSaveMsg(int x, int y, char *msg);
	BOOL selectExec(void);
	void overWriteExec(void);
	void SetAlpha(void){
		baseWnd.alpha = panel.alpha = cursor.alpha = button.alpha = numParts.alpha = numPartsSm.alpha = typeParts.alpha = wallWnd.alpha = infoWnd.alpha = alpha;
	}
};

void SAV_Save( int save_no );
BOOL SAV_Load( int save_no );
void saveMessage(char *msgBuf);

extern SaveWnd		*saveWnd;
extern SAVE_STRUCT	SaveStruct;

#define	_MY_SAVEWND
#endif	_MY_SAVEWND