#ifndef _ESCRIPT_H__
#define _ESCRIPT_H__

#define	ESC_OBJSADDR			0x80028000
#define	ESC_OBJDSIZEMAX			0x100000
#define ESC_FLAG_MAX			256

#define ESC_SetFlag( N, D )		ESC_FlagBuf[ ( N ) ] = ( D )
#define ESC_GetFlag( N )		ESC_FlagBuf[ ( N ) ]

#define ESC_GetGameFlag( N )	ESC_GameFlagBuf[ ( N ) ]
void ESC_SetGameFlag(int N, int D );

extern int  ESC_OprControl( int opr );

extern void ESC_InitFlag( void );
extern void ESC_InitGameFlag( void );
extern void ESC_InitEOprFlag( void );


extern int	ESC_FlagBuf[ ESC_FLAG_MAX ];
extern int	ESC_GameFlagBuf[ ESC_FLAG_MAX ];

extern void ESC_SetDrawFlag( void );
extern void ESC_CodeCheck(void);

extern char LangOprList[64][32];

extern void EXEC_SetPackDir( char *pac_dir );

#define	SCRIPT_BLOCK_MAX		256

typedef struct {
	short			h1, h2;
	long			Fsize;
	unsigned long	BlockAddres[ SCRIPT_BLOCK_MAX ];
} SCRIPT_HEADER;

enum {
	OPRS_START = 0,
	OPRS_END,

	OPRS_MOVR,
	OPRS_MOVV,
	OPRS_SWAP,
	OPRS_RAND,

	OPRS_IFR,
	OPRS_IFV,
	OPRS_IFELSER,
	OPRS_IFELSEV,

	OPRS_LOOP,
	OPRS_GOTO,

	OPRS_INC,
	OPRS_DEC,
	OPRS_NOT,
	OPRS_NEG,

	OPRS_ADDR,
	OPRS_ADDV,
	OPRS_SUBR,
	OPRS_SUBV,
	OPRS_MULR,
	OPRS_MULV,
	OPRS_DIVR,
	OPRS_DIVV,
	OPRS_MODR,
	OPRS_MODV,
	OPRS_ANDR,
	OPRS_ANDV,
	OPRS_ORR,
	OPRS_ORV,
	OPRS_XORR,
	OPRS_XORV,

	OPRS_CALC,

	OPRS_PUSHA,
	OPRS_POPA,

	OPRS_CALL,
	OPRS_RET,

	OPRS_WAIT,
	OPRS_TWAIT,
	OPRS_RUN,

	OPRS_SLOAD,
	OPRS_TSTART,

	OPRS_OPREND,
};

#define	MAIN_SCRIPT				0

#define SCCODE_NOOPR			0					
#define SCCODE_RUN				1					
#define SCCODE_WAIT_WAIT		2					
#define SCCODE_WAIT_TWAIT		3					
#define SCCODE_WAIT_SLOAD		4					
#define SCCODE_WAIT_DEBUG		0xFF				

#define REGISTER_MAX			50					
#define	STACK_MAX				512					
#define	TASK_MAX				8					
enum{												
	SCRMODE_EVENT,
	SCRMODE_MAX
};

enum {
	CALL_SMALL = 0,
	CALL_ESMALL,
	CALL_LARGE,
	CALL_ELARGE,
	CALL_EQUAL,
	CALL_NEQUAL,

	CALL_ADD,
	CALL_SUB,
	CALL_MUL,
	CALL_DIV,
	CALL_MOD,
};

enum {
	FACT_VAL = 0,
	FACT_REG,
	FACT_CLC,
	FACT_END,
};

typedef struct {									
	DWORD		BlockAddres[SCRIPT_BLOCK_MAX];		
	long		sp_buf[STACK_MAX];					
	long		reg[REGISTER_MAX];					
	DWORD		pc;									
	DWORD		sp;									
	DWORD		WaitCounter;						
	DWORD		WaitEnd;							
	DWORD		TWaitEnd;							
	char		*LangBuf;							
	char		BusyFlg;							
} EXEC_DATA;

extern	char		*EXEC_LangBuf;
extern	EXEC_DATA	*EXEC_LangInfo;
extern	char		NowLangFileName[16];
extern	int			RunLangBlockNo;
extern	int			PrevRunLangBlockNo;
extern int		EXEC_ReadLang( char *filename, EXEC_DATA *scr );
extern BOOL		EXEC_StartLang( EXEC_DATA *scr, int scr_no );
extern void		EXEC_ReleaseLang( EXEC_DATA *scr );
extern void		EXEC_EndLang( EXEC_DATA *scr, int init );
extern short	EXEC_GetShortValue( char *buf );
extern long		EXEC_GetLongValue( char *buf );
extern int		EXEC_GetValue( char *buf, int mode );
extern int		EXEC_ControlLang( EXEC_DATA *scr );
extern int		EXEC_SLoad( char *str );

extern void EXEC_OprCallFunc( int call_no, int next_pc, long *num );
void InitReadFlag(void);
BOOL AVG_CheckScenarioFlag(void);
void AVG_SetScenarioNo( int scenario_no );
void AVG_SetCgFlag( int cg_no, int sub_no );
BOOL AVG_GetCgFlag( int cg_no, int sub_no );
void AVG_SetSceneFlag( int scene_no );
BOOL AVG_GetSceneFlag( int scene_no );
void ReturnTitile(BOOL bForce=FALSE);
#define	EXEC_GetParam( N )		( EXEC_LangInfo->reg[ ( N ) ]         )
#define	EXEC_SetParam( N, V )	( EXEC_LangInfo->reg[ ( N ) ] = ( V ) )
#define	EXEC_AddPC( N )			( EXEC_LangInfo->pc += ( N )          )
#define	EXEC_GetPC				( EXEC_LangInfo->pc                   )
#define	EXEC_GetBusyFlag			( EXEC_LangInfo->BusyFlg              )

extern EXEC_DATA 	LangData;

#endif 
