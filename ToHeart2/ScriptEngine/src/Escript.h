
#ifndef _ESCRIPT_H__
#define _ESCRIPT_H__

#define	ESC_OBJSADDR			0x80028000							
#define	ESC_OBJDSIZEMAX			0x100000							
#define ESC_FLAG_MAX			1024								

#define	_MONTH		0	
#define	_DAY		1	
#define	_TIME		2	
#define	_EVENT_NEXT	3	
#define	_EVENT_END	4	
#define	_DEFAULT_NAME	5	
#define	_CALENDER_SKIP	6	
#define	_CLOCK_TIME		7	

#define	_DEBUG_CALL		9	

#define ESC_SetFlag( N, D )		ESC_FlagBuf[ ( N ) ] = ( D )		
#define ESC_GetFlag( N )		ESC_FlagBuf[ ( N ) ]				

extern int  ESC_OprControl( int opr );

extern void ESC_InitFlag( void );


extern void ESC_InitEOprFlag( void );


extern int	ESC_FlagBuf[ ESC_FLAG_MAX ];
extern int	ESC_GameFlagBuf[ ESC_FLAG_MAX ];				

extern void SetVoiceDialog( int flag );
extern void AddVoiceDialogMes( char *mes, ...);

extern void ESC_SetDrawFlag( void );
extern void ESC_CodeCheck(void);


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

extern char ScrCodeList[64][32];

extern void EXEC_SetPackDir( char *pac_dir );

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
	FACT_VAL = 0,
	FACT_REG,
	FACT_CLC,
	FACT_END,
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
extern	char		NowLangFileName[64];
extern	int			RunLangBlockNo;
extern	int			PrevRunLangBlockNo;

extern BOOL		EXEC_ReadLang( char *filename, EXEC_DATA *scr );
extern BOOL		EXEC_StartLang( EXEC_DATA *scr, int scr_no );
extern void		EXEC_ReleaseLang( EXEC_DATA *scr );
extern void		EXEC_EndLang( EXEC_DATA *scr, int init );
extern short	EXEC_GetShortValue( char *buf );
extern long		EXEC_GetLongValue( char *buf );
extern int		EXEC_GetValue( char *buf, int mode );
extern int		EXEC_ControlLang( EXEC_DATA *scr );
extern int		EXEC_SLoad( char *str );

extern void EXEC_OprCallFunc( int call_no, int next_pc, long *num );


#define	EXEC_GetParam( N )		( EXEC_LangInfo->reg[ ( N ) ]         )
#define	EXEC_SetParam( N, V )	( EXEC_LangInfo->reg[ ( N ) ] = ( V ) )
#define	EXEC_AddPC( N )			( EXEC_LangInfo->pc += ( N )          )
#define	EXEC_GetPC				( EXEC_LangInfo->pc                   )
#define	EXEC_GetBusyFlag			( EXEC_LangInfo->BusyFlg              )


#endif 
