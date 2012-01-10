
 
  
   
    
     
      


#ifndef MM_STD
#define MM_STD

#include <windows.h>
#include <mmsystem.h>
#include <winnls32.h>
#include <mbstring.h>
 

#define	ON			1
#define	OFF			0

#define	OPEN		1
#define	CLOSE		0

#define	DIR_C		-1
#define	DIR_D		0
#define	DIR_DL		1
#define	DIR_L		2
#define	DIR_UL		3
#define	DIR_U		4
#define	DIR_UR		5
#define	DIR_R		6
#define	DIR_DR		7



extern char szVenderID[32];
extern char szCpuName[64];

extern DWORD TypeID;
extern DWORD FamilyID;
extern DWORD ModelID;
extern DWORD SteppingID;

extern DWORD CPUID_Flag;
extern DWORD FPU_Flag;
extern DWORD TSC_Flag;
extern DWORD MSR_Flag;
extern DWORD CMOV_Flag;
extern DWORD MMX_Flag;
extern DWORD _3DNow_Flag;

extern DWORD SSE_INT_Flag;
extern DWORD SSE_FP_Flag;
extern DWORD _3DNowExt_Flag;

extern DWORD SSE2_Flag;

extern DWORD fCpuClock;


extern const WORD	SinTbl[256];
extern DWORD	Pow2Tbl[2048];
extern WORD		SqrtTbl[0x10000];


#define SIN(X)		((short)SinTbl[(X+64)%256])
#define COS(X)		((short)SinTbl[(X)%256])


#define DIRECT_X		0
#define DIRECT_SOUND	1

#if DIRECT_X
	#define DIRECT_DRAW		1
	#define _DX_60			1

#else	
	#define DIRECT_DRAW		0

#endif	


typedef struct {
	int		x;
	int		y;
	int		w;
	int		h;
}RECT2;

typedef struct {
	char	b1;
	char	b2;
	char	b3;
}BYTE3;

#define PAI					3.141592
#define POW2(X)				(Pow2Tbl[X+1024])			
#define SQRT(X)				(SqrtTbl[X])				

#define ABS2(X)				(AbsoluteTbl[X+256])		
#define ABS(X)				(((X)<0)?-(X):(X))			
#define LOOP(X,L)			(((L)-((L)-(X))%(L))%(L))	
#define LIM(D,L,H)			(min(max((D),(L)),(H)))		
#define limit(D,L,H)		(min(max((D),(L)),(H)))
#define LIM_CHK(D,L,H)		((L<=D)&&(D<H))				

#define POYO_CONUT(CNT,MAX,MAX2)	((float)((pow(MAX2,2)-pow((min(CNT,MAX))-(MAX2),2))/(pow(MAX2,2)-pow((MAX)-(MAX2),2))))
#define POW_CONUT(CNT,MAX)			((float)(pow(CNT,2)/pow(MAX,2)))



#define GAlloc(SIZE)			GlobalAlloc(GPTR,SIZE)
#define GFree(BUF)				if(BUF){GlobalFree(BUF); BUF=NULL;}


#define	GetSysColorR(FLG)	(GetSysColor(FLG)&0xff)
#define	GetSysColorG(FLG)	((GetSysColor(FLG)>>8)&0xff)
#define	GetSysColorB(FLG)	((GetSysColor(FLG)>>16)&0xff)





#define MBS_CPY(S1,S2)	(_mbscpy((BYTE*)(S1),(BYTE*)(S2)))
#define MBS_CAT(S1,S2)	(_mbscat((BYTE*)(S1),(BYTE*)(S2)))



extern void STD_InitTable(void);

extern int STD_Amari( int num1, int num2 );
extern int	Cal2Disgit( char *cal );

extern void memcpy2( void *dst, const void *src, int size );

extern int	GetBitCountDWORD( DWORD flag );

extern HWND GetVolumeContorolHandle( void );
extern void STD_SetForgroundMyWnd( HWND hWnd );
extern void STD_SetDialogPos( HWND hd, int sx, int sy );
extern void STD_SetWindowPos( HWND hd, int sx, int sy );
extern void STD_GetWindowPos( HWND hd, int *sx, int *sy );
extern BOOL STD_GetWindowDisp( HWND hd );

extern int	STD_GetScreenBpp( int *w, int *h );

extern BYTE STD_GetBit( BYTE *buf, int pich, int num );
extern BOOL STD_SetBit( BYTE *buf, int pich, int num, int src );


extern char *Digit2Space( int dg );
extern int Str2Digit( char *str );
extern int Str2Hex( char *str );
extern char *Digit2Str( int digit, int zero, int lr );
extern char *Digit2Str2( int digit, int zero, int lr );
extern int	Digit4keta( int digit );

extern char *Wstr2Bstr( char *str );
extern char *Bstr2Wstr( char *str );

extern void STD_strlower( char *dst, char *src );
extern void STD_strupper( char *dst, char *src );

extern int	STD_strtok( char *dst, char *src, unsigned long code );
extern int	STD_strcmp( char *str1, char *str2 );
extern int	STD_strcmp2( char *str1, char *str2 );
extern int	STD_strcmpUL( char *str1, char *str2 );

extern long STD_StepLimit( int src, int start, int end, int fade, int max, int type );
extern long	STD_Limit( long src, long digit1, long digit2 );
extern long STD_LimitLoop( long src, long max_digit );

extern int STD_Taraimawasi( int x, int y, int z );
extern double STD_Lagrange( double x, int m, POINT *pt );




extern void STD_CheckCPU( void );
extern OSVERSIONINFO STD_CheckOS( void );
extern BOOL STD_CheckOSw2k( void );
extern int STD_GetOSVer( void );
extern char *STD_GetOSName( void );

extern double	TimerCount;
extern int STD_timeGetTime(void);

typedef struct {

	TCHAR FilePath[_MAX_PATH];		
	TCHAR FileName[_MAX_PATH];		

	TCHAR Drive[_MAX_DRIVE];		
	TCHAR Dir[_MAX_DIR];			
	TCHAR Fname[_MAX_FNAME];		
	TCHAR Ext[_MAX_EXT];			

}FILE_PATH_INFO;

FILE_PATH_INFO DivFile( FILE_PATH_INFO *fi, LPSTR str, ...);

#endif 