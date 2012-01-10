
 
  
   
    
     
      

#ifndef _ESCRIPT_H__
#define _ESCRIPT_H__


#define	ESC_OBJSADDR			0x80028000								
#define	ESC_OBJDSIZEMAX			0x100000								
#define ESC_FLAG_MAX			1024									


#define ESC_SetFlag( N, D )		ESC_FlagBuf[ ( N ) ] = ( D )			
#define ESC_GetFlag( N )		ESC_FlagBuf[ ( N ) ]					

#define ESC_SetGameFlag( N, D )		ESC_GameFlagBuf[ ( N ) ] = ( D )			
#define ESC_GetGameFlag( N )		ESC_GameFlagBuf[ ( N ) ]					


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
	EXEC_START = 0,
	EXEC_END,

	EXEC_MOVR,
	EXEC_MOVV,
	EXEC_SWAP,
	EXEC_RAND,

	EXEC_IFR,
	EXEC_IFV,
	EXEC_IFELSER,
	EXEC_IFELSEV,

	EXEC_LOOP,
	EXEC_GOTO,

	EXEC_INC,
	EXEC_DEC,
	EXEC_NOT,
	EXEC_NEG,

	EXEC_ADDR,
	EXEC_ADDV,
	EXEC_SUBR,
	EXEC_SUBV,
	EXEC_MULR,
	EXEC_MULV,
	EXEC_DIVR,
	EXEC_DIVV,
	EXEC_MODR,
	EXEC_MODV,
	EXEC_ANDR,
	EXEC_ANDV,
	EXEC_ORR,
	EXEC_ORV,
	EXEC_XORR,
	EXEC_XORV,

	EXEC_CALC,

	EXEC_PUSHA,
	EXEC_POPA,

	EXEC_CALL,

	EXEC_RET,

	EXEC_WAIT,
	EXEC_TWAIT,
	EXEC_RUN,

	EXEC_SLOAD,
	EXEC_TSTART,

	EXEC_OPREND,
};



extern char ScriptCodeList[64][32];

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
	CALC_SMALL = 0,
	CALC_ESMALL,
	CALC_LARGE,
	CALC_ELARGE,
	CALC_EQUAL,
	CALC_NEQUAL,

	CALC_ADD,
	CALC_SUB,
	CALC_MUL,
	CALC_DIV,
	CALC_MOD,
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
	char		*ScriptBuf;							
	char		BusyFlg;							
} EXEC_DATA;


extern	BYTE		*EXEC_ScriptBuf;
extern	EXEC_DATA	*EXEC_ScriptInfo;
extern	char		NowScriptFileName[16];
extern	int			RunScriptBlockNo;
extern	int			PrevRunScriptBlockNo;

extern BOOL		EXEC_ReadScript( char *filename, EXEC_DATA *scr );
extern BOOL		EXEC_StartScript( EXEC_DATA *scr, int scr_no );
extern void		EXEC_ReleaseScript( EXEC_DATA *scr );
extern void		EXEC_EndScript( EXEC_DATA *scr, int init );
extern short	EXEC_GetShortValue( BYTE *buf );
extern long		EXEC_GetLongValue( BYTE *buf );
extern int		EXEC_GetValue( BYTE *buf, int mode );
extern int		EXEC_ControlScript( EXEC_DATA *scr );
extern void		EXEC_SLoad( char *str );

extern void EXEC_OprCallFunc( int call_no, int next_pc, long *num );


#define	EXEC_GetParam( N )		( EXEC_ScriptInfo->reg[ ( N ) ]         )
#define	EXEC_SetParam( N, V )	( EXEC_ScriptInfo->reg[ ( N ) ] = ( V ) )
#define	EXEC_AddPC( N )			( EXEC_ScriptInfo->pc += ( N )          )
#define	EXEC_GetPC				( EXEC_ScriptInfo->pc                   )
#define	EXEC_GetBusyFlag			( EXEC_ScriptInfo->BusyFlg              )




#endif 