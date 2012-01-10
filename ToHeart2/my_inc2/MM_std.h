
 
  
   
    
     
      


#ifndef MM_STD
#define MM_STD

#include <windows.h>
#include <mmsystem.h>
#include <winnls32.h>
#include <mbstring.h>
 





#define	FILE_SPOIT	1


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



extern char szVenderID[13];
extern char szCpuName[49];

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
extern DWORD SSE3_Flag;

extern DWORD fCpuClock;


extern const WORD	SinTbl[256];
extern DWORD	RandTbl[0x10000];
extern DWORD	RandCount;





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

#define PAI					3.1415926535




#define RAND_TBL				(RandTbl[(RandCount++)&0xffff])


#define ABS(X)				(((X)<0)?-(X):(X))			
#define LOOP(X,L)			(((L)-((L)-(X))%(L))%(L))	
#define LIM(D,L,H)			(min(max((D),(L)),(H)))		
#define limit(D,L,H)		(min(max((D),(L)),(H)))
#define LIM_CHK(D,L,H)		((L<=D)&&(D<H))				

#define LIM_LOOP(SR,LI)		(((SR%(LI*2))>=LI)?(LI*2-(SR%(LI*2))):(SR%(LI*2)))


#define GAlloc(SIZE)			GlobalAlloc(GPTR,SIZE)
#define GFree(BUF)				if(BUF){GlobalFree(BUF); BUF=NULL;}




#define	CREATE_READ_FILE(FNAME)		CreateFile( FNAME, GENERIC_READ , FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)
#define	CREATE_WRITE_FILE(FNAME)	CreateFile( FNAME, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL)
#define	CREATE_APEND_FILE(FNAME)	CreateFile( FNAME, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,   FILE_ATTRIBUTE_NORMAL, NULL)

#define	CREATE_READ_FILE2(FNAME,FLAG)		CreateFile( FNAME, GENERIC_READ , FILE_SHARE_READ, NULL, OPEN_EXISTING, FLAG, NULL)
#define	CREATE_WRITE_FILE2(FNAME,FLAG)		CreateFile( FNAME, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FLAG, NULL)
#define	CREATE_REWRITE_FILE2(FNAME,FLAG)	CreateFile( FNAME, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FLAG, NULL)


#define	GetSysColorR(FLG)	(GetSysColor(FLG)&0xff)
#define	GetSysColorG(FLG)	((GetSysColor(FLG)>>8)&0xff)
#define	GetSysColorB(FLG)	((GetSysColor(FLG)>>16)&0xff)

extern void STD_SetMainHwnd( HWND hwnd );

extern void STD_InitTable(void);

extern int STD_Amari( int num1, int num2 );
extern int	Cal2Disgit( char *cal );







extern void memcpy2( void *dst, const void *src, int size );

extern void DebugBox( HWND hwnd, char *mes, ...);
extern int  MessageBox2( HWND hwnd, DWORD flag, char *title, char *mes, ... );
extern int  CheckBox( HWND hwnd, char *title, char *mes, ... );
extern void DebugPrintf( char *mes, ...);
extern void GetPrintErrMessage( int err_code );

extern HWND GetVolumeContorolHandle( void );
extern void STD_SetForgroundMyWnd( HWND hWnd );
extern void STD_SetDialogPos( HWND hd, int sx, int sy );
extern void STD_SetWindowPos( HWND hd, int sx, int sy );
extern void STD_GetWindowPos( HWND hd, int *sx, int *sy );
extern BOOL STD_GetWindowDisp( HWND hd );

extern int	STD_GetScreenBpp( int *w, int *h );

extern BYTE STD_GetBit( BYTE *buf, int pich, int num );
extern BOOL STD_SetBit( BYTE *buf, int pich, int num, int src );

extern char *STD_FileAttrChange( char *dst, char *src,char *attr );
extern char *STD_GetAttr( char *src );
extern char *STD_GetFileName( char *src );
extern char *STD_GetDir( char *dst, char *src=NULL );


extern DWORD STD_GetFileSize( char *fname );
extern BOOL STD_WriteFilePos( char *fname, char *src, DWORD pos, DWORD size );
extern BOOL STD_WriteFile( char *fname, char *src, DWORD size );
extern BOOL STD_ApendFile( char *fname, char *src, DWORD size );
extern BOOL STD_ReadFilePos( char *fname, char *buf, DWORD pos, DWORD size );
extern BOOL STD_ReadFile( char *fname, char *buf, DWORD size );
extern DWORD STD_ReadFileMax( char *fname, char **buf );
extern BOOL STD_DeleteFile( char *fname );
extern BOOL STD_CheckFile( char *fname );

extern BOOL STD_CopyDir( char *src, char *dst, BOOL uwagaki );

typedef struct{
	char	src[1024];
	char	dst[1024];
	BOOL	uwagaki;

	int		count;
	int		countmax;
	int		err_count;

	char	fname[1024];
}COPY_DIR_DATA;

extern BOOL STD_CopyDir_First( char *src, char *dst, BOOL uwagaki, COPY_DIR_DATA *cdd );
extern BOOL STD_CopyDir_Next( COPY_DIR_DATA *cdd );


typedef struct{
	WIN32_FIND_DATA	data;
	HANDLE			handle;
	BOOL			fnf_ret;
	int				step;
	void			*next;
}WIN32_FIND_DATA_LIST;

typedef struct{
	char					dir[1024];
	char					file[256];
	BOOL					subdir_chk;
	char					find_dir[1024];
	WIN32_FIND_DATA			find_data;
	WIN32_FIND_DATA_LIST	*list;
}SEARCH_DIR_DATA;

extern BOOL InitSaerchFile( char *src, BOOL subdir_chk, SEARCH_DIR_DATA *sdd );
extern BOOL SaerchNextFile( SEARCH_DIR_DATA *sdd );
extern BOOL TermSaerchFile( SEARCH_DIR_DATA *sdd );


extern void STD_SetDefaultDir( char *str, int flag );
extern void STD_SetDefaultExt( char *str );
extern void STD_LoadDefaultDir( char *regkey );
extern void STD_SaveDefaultDir( char *regkey );
extern char *STD_GetOpenFileName( HWND hwnd, char *ff );
extern char *STD_GetSaveFileName( HWND hwnd, char *ff );

extern BOOL STD_LuckupDir( HWND hwnd, char *dir );



extern void SetIniFileName( char *dir, char *ini_fname, BOOL *exist=NULL );

extern int GetIniFileKeyList( char *key_list, int size, char *sec_name="DEFAULT", char *ini_fname=NULL );
extern int GetIniFileSectionList( char *sec_list, int size, char *ini_fname=NULL );
extern int WriteIniFileAmount( char *vname, int amount, char *sec_name="DEFAULT", char *ini_fname=NULL );
extern int ReadIniFileAmount( char *vname, int def, char *sec_name="DEFAULT", char *ini_fname=NULL );
extern int WriteIniFileData( char *vname, char *data, int size, char *sec_name="DEFAULT", char *ini_fname=NULL );
extern int ReadIniFileData( char *vname, char *data, int size, char *sec_name="DEFAULT", char *ini_fname=NULL );
extern int WriteIniFileStr( char *vname, char *str, char *sec_name="DEFAULT", char *ini_fname=NULL );
extern int ReadIniFileStr( char *vname, char *str, char *def, int size, char *sec_name="DEFAULT", char *ini_fname=NULL );
extern int DeleteIniFileKey( char *vname, char *sec_name="DEFAULT", char *ini_fname=NULL );
extern int DeleteIniFileSection( char *sec_name="DEFAULT", char *ini_fname=NULL );

extern void SetRegistryKey( char *key);
extern void SetRegistryKeyEx( char *key);
extern int WriteRegistryAmount( char *vname, long amount );
extern int ReadRegistryAmount( char *vname, long *amount );
extern int WriteRegistryData( char *vname, char *data, int size );
extern int ReadRegistryData( char *vname, char *data, int size );
extern int WriteRegistryStr( char *vname, char *str );
extern int ReadRegistryStr( char *vname, char *str );
extern int DeleteRegistry( char *vname);

extern char *Digit2Space( int dg );
extern int Str2Digit( char *str );
extern int Str2Hex( char *str );
extern char *Digit2Str( int digit, int zero, int lr );
extern char * Digit2Str2( int digit, int zero, int lr );

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